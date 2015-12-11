 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_UTILS
#define ZATHURA_GTK_ANNOTATION_UTILS

#include <libzathura/annotations.h>
#include <cairo.h>

void zathura_gtk_annotation_set_cairo_color(cairo_t* cairo, zathura_annotation_color_t color, double opacity);

#endif /* ZATHURA_GTK_ANNOTATION_UTILS */
