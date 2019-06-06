#ifndef WINDOW_H
#define WINDOW_H

#include <sized_types.h>
#include <xcb/xcb.h>
#include <xcb/shm.h>

struct window {
  struct {
    xcb_connection_t *cn;
    xcb_setup_t *setup;
    xcb_screen_t *screen;
    xcb_window_t wn;
    xcb_gcontext_t gc;
    xcb_shm_seg_t shmseg;
    uint32_t *shm_data;
  } xcb;
  char *title;
  uint16_t width;
  uint16_t height;
};

/* **************************************** */
/* window_<platform>.c */
int window_init(struct window *w,
                char *title,
                uint16_t width,
                uint16_t height);
void window_show(struct window *w);
void window_update(struct window *w);
uint32_t *window_buffer(struct window *w);
/* **************************************** */


#endif
