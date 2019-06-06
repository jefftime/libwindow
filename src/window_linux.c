#include "window.h"
#include <sized_types.h>
#include <xcb/xcb.h>
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

/* **************************************** */
/* Public */
/* **************************************** */

int window_init(struct window *w,
                char *title,
                uint16_t width,
                uint16_t height) {
  if (!w) return -1;
  memset(w, 0, sizeof(struct window));
  if (setup_xcb(w)) return -1;
  setup_window(w, title, width, height);
  return 0;
}

void window_show(struct window *w) {
  xcb_map_window(w->xcb.cn, w->xcb.wn);
  xcb_flush(w->xcb.cn);
}
