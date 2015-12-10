 /* See LICENSE file for license and copyright information */

#include "utils.h"

void
zathura_gtk_annotation_set_cairo_color(cairo_t* cairo, zathura_annotation_color_t color)
{
  cairo_set_source_rgb(cairo,
      (double) color.values[0] / 65535.0,
      (double) color.values[1] / 65535.0,
      (double) color.values[2] / 65535.0);
}
