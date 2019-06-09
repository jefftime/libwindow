#include "window.h"
#include <sized_types.h>
#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

struct xcb {
  xcb_connection_t *cn;
  xcb_setup_t *setup;
  xcb_screen_t *screen;
  xcb_window_t wn;
  xcb_gcontext_t gc;
  xcb_shm_seg_t shmseg;
  uint32_t *shm_data;
  xcb_atom_t win_delete;
  int should_close;
};

static int init_struct(struct window *w,
                       char *title,
                       uint16_t width,
                       uint16_t height) {
  memset(w, 0, sizeof(struct window));
  w->internal = calloc(1, sizeof(struct xcb));
  if (!w->internal) return -1;
  w->width = width;
  w->height = height;
  w->title = title;
  return 0;
}

static int setup_xcb(struct xcb *xcb) {
  int i, screen_index;
  xcb_screen_iterator_t screen_iter;

  xcb->cn = xcb_connect(NULL, &screen_index);
  if (!xcb->cn) return -1;
  xcb->setup = (xcb_setup_t *) xcb_get_setup(xcb->cn);
  screen_iter = xcb_setup_roots_iterator(xcb->setup);
  /* Get selected screen index information */
  for (i = 0; i < screen_index; ++i) {
    xcb_screen_next(&screen_iter);
  }
  xcb->screen = screen_iter.data;
  return 0;
}

static int setup_window(struct window *w) {
  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t values[] = { XCB_EVENT_MASK_STRUCTURE_NOTIFY };
  struct xcb *xcb;
  xcb_intern_atom_cookie_t protocol_cookie, delete_cookie;
  xcb_intern_atom_reply_t *protocol_reply, *delete_reply;

  xcb = (struct xcb *) w->internal;
  xcb->wn = xcb_generate_id(xcb->cn);
  xcb_create_window(xcb->cn,
                    XCB_COPY_FROM_PARENT,
                    xcb->wn,
                    xcb->screen->root,
                    0, 0,
                    w->width, w->height,
                    0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    xcb->screen->root_visual,
                    mask,
                    values);
  xcb_change_property(xcb->cn,
                      XCB_PROP_MODE_REPLACE,
                      xcb->wn,
                      XCB_ATOM_WM_NAME,
                      XCB_ATOM_STRING,
                      8,
                      (uint32_t) strlen(w->title),
                      w->title);
  xcb_flush(xcb->cn);
  /* Watch for delete event */
  protocol_cookie = xcb_intern_atom(xcb->cn,
                                    0,
                                    strlen("WM_PROTOCOLS"),
                                    "WM_PROTOCOLS");
  protocol_reply = xcb_intern_atom_reply(xcb->cn, protocol_cookie, NULL);
  if (!protocol_reply) return -1;
  delete_cookie = xcb_intern_atom(xcb->cn,
                                  0,
                                  strlen("WM_DELETE_WINDOW"),
                                  "WM_DELETE_WINDOW");
  delete_reply = xcb_intern_atom_reply(xcb->cn, delete_cookie, NULL);
  if (!delete_reply) {
    free(protocol_reply);
    return -1;
  }
  xcb_change_property(xcb->cn,
                      XCB_PROP_MODE_REPLACE,
                      xcb->wn,
                      (protocol_reply->atom),
                      XCB_ATOM_ATOM,
                      32,
                      1,
                      &delete_reply->atom);
  xcb->win_delete = delete_reply->atom;
  free(protocol_reply);
  free(delete_reply);
  return 0;
}

static void setup_graphics_context(struct xcb *xcb) {
  xcb->gc = xcb_generate_id(xcb->cn);
  xcb_create_gc(xcb->cn, xcb->gc, xcb->wn, 0, NULL);
}

static int setup_shm_pixmap(struct window *w) {
  int shmid;
  struct xcb *xcb;

  xcb = (struct xcb *) w->internal;
  shmid = shmget(IPC_PRIVATE,
                 sizeof(uint32_t) * w->width * w->height,
                 IPC_CREAT | 0777);
  if (shmid < 0) return -1;
  xcb->shm_data = shmat(shmid, 0, 0);
  xcb->shmseg = xcb_generate_id(xcb->cn);
  xcb_shm_attach(xcb->cn, xcb->shmseg, (uint32_t) shmid, 0);
  xcb_flush(xcb->cn);
  shmctl(shmid, IPC_RMID, 0);
  return 0;
}

/* **************************************** */
/* Public */
/* **************************************** */

int window_init(struct window *w,
                char *title,
                uint16_t width,
                uint16_t height) {
  struct xcb *xcb;

  if (!w) return -1;
  if (init_struct(w, title, width, height)) return -1;
  xcb = (struct xcb *) w->internal;
  if (setup_xcb((struct xcb *) w->internal)) return -1;
  if (setup_window(w)) return -1;
  setup_graphics_context((struct xcb *) w->internal);
  if (setup_shm_pixmap(w)) return -1;
  xcb_flush(xcb->cn);
  return 0;
}

void window_show(struct window *w) {
  struct xcb *xcb;

  if (!w) return;
  xcb = (struct xcb *) w->internal;
  xcb_map_window(xcb->cn, xcb->wn);
  xcb_flush(xcb->cn);
}

void window_update(struct window *w) {
  struct xcb *xcb;
  xcb_generic_event_t *event;

  xcb = (struct xcb *) w->internal;
  /* Check if window has been closed */
  while ((event = xcb_poll_for_event(xcb->cn))) {
    switch (event->response_type & ~0x80) {
      case XCB_CLIENT_MESSAGE: {
        xcb_client_message_event_t *e;

        e = (xcb_client_message_event_t *) event;
        if (e->data.data32[0] == xcb->win_delete) xcb->should_close = 1;
        break;
      }
    }
  }
  xcb_shm_put_image(xcb->cn,
                    xcb->wn,
                    xcb->gc,
                    w->width, w->height,
                    0, 0,
                    w->width, w->height,
                    0, 0,
                    xcb->screen->root_depth,
                    XCB_IMAGE_FORMAT_Z_PIXMAP,
                    0,
                    xcb->shmseg,
                    0);
  xcb_flush(xcb->cn);
}

uint32_t *window_buffer(struct window *w) {
  return w ? ((struct xcb *) w->internal)->shm_data : NULL;
}

int window_close(struct window *w) {
  return ((struct xcb *) w->internal)->should_close;
}
