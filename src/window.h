#ifndef WINDOW_H
#define WINDOW_H

#include <sized_types.h>
#include <xcb/xcb.h>
#include <xcb/shm.h>

struct window {
  void *internal;
  uint16_t width;
  uint16_t height;
  char *title;
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
int window_close(struct window *w);
/* **************************************** */


#endif
