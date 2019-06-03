#include "window.h"
#include <sized_types.h>
#include <stdio.h>
#include <string.h>

/* **************************************** */
/* window_<platform>.c */
int window_native_init(struct window *w,
                       char *title,
                       uint16_t width,
                       uint16_t height);
int window_native_map(struct window *w);
/* **************************************** */

/* **************************************** */
/* Public */
/* **************************************** */

int window_init(struct window *w,
                char *title,
                uint16_t width,
                uint16_t height) {
  int error;

  if (!w) return -1;
  memset(w, 0, sizeof(struct window));
  /* Initialize native window information  */
  error = window_native_init(w, title, width, height);
  if (error) return -1;
  error = window_native_map(w);
  if (error) return -1;
  return 0;
}
