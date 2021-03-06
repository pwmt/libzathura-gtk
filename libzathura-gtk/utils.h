/* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_UTILS
#define ZATHURA_GTK_UTILS

#include <libzathura/libzathura.h>
#include <cairo.h>

cairo_operator_t zathura_blend_mode_to_cairo_operator(zathura_blend_mode_t blend_mode);
zathura_rectangle_t zathura_rectangle_scale(zathura_rectangle_t rectangle, double scale);

#endif /* ZATHURA_PAGE_UTILS */
