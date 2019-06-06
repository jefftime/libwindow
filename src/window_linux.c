#include "window.h"
#include <sized_types.h>
#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

static int setup_xcb(struct window *w) {
  int i, screen_index;
  xcb_screen_iterator_t screen_iter;

  w->xcb.cn = xcb_connect(NULL, &screen_index);
  if (!w->xcb.cn) return -1;
  w->xcb.setup = (xcb_setup_t *) xcb_get_setup(w->xcb.cn);
  screen_iter = xcb_setup_roots_iterator(w->xcb.setup);
  /* Get selected screen index information */
  for (i = 0; i < screen_index; ++i) {
    xcb_screen_next(&screen_iter);
  }
  w->xcb.screen = screen_iter.data;
  return 0;
}

static void setup_window(struct window *w,
                         char *title,
                         uint16_t width,
                         uint16_t height) {
  w->xcb.wn = xcb_generate_id(w->xcb.cn);
  xcb_create_window(w->xcb.cn,
                    w->xcb.screen->root_depth,
                    w->xcb.wn,
                    w->xcb.screen->root,
                    0, 0,
                    width, height,
                    0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    w->xcb.screen->root_visual,
                    0,
                    NULL);
  xcb_change_property(w->xcb.cn,
                      XCB_PROP_MODE_REPLACE,
                      w->xcb.wn,
                      XCB_ATOM_WM_NAME,
                      XCB_ATOM_STRING,
                      8,
                      (uint32_t) strlen(title),
                      title);
  xcb_flush(w->xcb.cn);
}

static void setup_graphics_context(struct window *w) {
  w->xcb.gc = xcb_generate_id(w->xcb.cn);
  xcb_create_gc(w->xcb.cn, w->xcb.gc, w->xcb.wn, 0, NULL);
}

static int setup_shm_pixmap(struct window *w) {
  int shmid;

  shmid = shmget(IPC_PRIVATE,
                 sizeof(uint32_t) * w->width * w->height,
                 IPC_CREAT | 0777);
  if (shmid < 0) return -1;
  w->xcb.shm_data = shmat(shmid, 0, 0);
  w->xcb.shmseg = xcb_generate_id(w->xcb.cn);
  xcb_shm_attach(w->xcb.cn, w->xcb.shmseg, (uint32_t) shmid, 0);
  xcb_flush(w->xcb.cn);
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
  if (!w) return -1;
  memset(w, 0, sizeof(struct window));
  w->width = width;
  w->height = height;
  if (setup_xcb(w)) return -1;
  setup_window(w, title, width, height);
  setup_graphics_context(w);
  if (setup_shm_pixmap(w)) return -1;
  xcb_flush(w->xcb.cn);
  return 0;
}

void window_show(struct window *w) {
  if (!w) return;
  xcb_map_window(w->xcb.cn, w->xcb.wn);
  xcb_flush(w->xcb.cn);
}

void window_update(struct window *w) {
  xcb_shm_put_image(w->xcb.cn,
                    w->xcb.wn,
                    w->xcb.gc,
                    w->width, w->height,
                    0, 0,
                    w->width, w->height,
                    0, 0,
                    w->xcb.screen->root_depth,
                    XCB_IMAGE_FORMAT_Z_PIXMAP,
                    0,
                    w->xcb.shmseg,
                    0);
  xcb_flush(w->xcb.cn);
}

uint32_t *window_buffer(struct window *w) {
  return w ? w->xcb.shm_data : NULL;
}
