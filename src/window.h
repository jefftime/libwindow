#ifndef WINDOW_H
#define WINDOW_H

#include <sized_types.h>
#include <xcb/xcb.h>

struct window {
  struct {
    xcb_connection_t *cn;
    xcb_setup_t *setup;
    xcb_screen_t *screen;
    xcb_window_t wn;
  } xcb;
  char *title;
  uint16_t width;
  uint16_t height;
};

int window_init(struct window *w,
                char *title,
                uint16_t width,
                uint16_t height);


#endif
