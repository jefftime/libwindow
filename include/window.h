/* This file is a part of libwindow
 *
 * Copyright 2019-2020, Jeffery Stager
 *
 * libwindow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libwindow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libwindow.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <sized_types.h>

#ifdef WINDOW_TARGET_OS_LINUX
#include <xcb/xcb.h>
#endif  /* WINDOW_TARGET_OS_LINUX */

struct window {
  void *internal;
  uint16_t width;
  uint16_t height;
  char *title;
};

/* **************************************** */
/* window_<platform>.c */
int window_init(
  struct window *w,
  char *title,
  uint16_t width,
  uint16_t height
);
void window_deinit(struct window *w);
void window_update(struct window *w);
void window_draw(struct window *w);
uint32_t *window_buffer(struct window *w);
int window_close(struct window *w);
void window_swap(struct window *w);

#ifdef WINDOW_TARGET_OS_LINUX
xcb_connection_t *window_xcb_connection(struct window *w);
xcb_window_t window_xcb_window(struct window *w);
#endif  /* WINDOW_TARGET_OS_LINUX */
/* **************************************** */

#endif
