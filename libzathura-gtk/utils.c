/* See LICENSE file for license and copyright information */

#include "utils.h"

cairo_operator_t
zathura_blend_mode_to_cairo_operator(zathura_blend_mode_t blend_mode)
{
  switch (blend_mode) {
    case ZATHURA_BLEND_MODE_NORMAL:
      return CAIRO_OPERATOR_OVER;
    case ZATHURA_BLEND_MODE_MULTIPLY:
      return CAIRO_OPERATOR_MULTIPLY;
    case ZATHURA_BLEND_MODE_SCREEN:
      return CAIRO_OPERATOR_SCREEN;
    case ZATHURA_BLEND_MODE_OVERLAY:
      return CAIRO_OPERATOR_OVERLAY;
    case ZATHURA_BLEND_MODE_DARKEN:
      return CAIRO_OPERATOR_DARKEN;
    case ZATHURA_BLEND_MODE_LIGHTEN:
      return CAIRO_OPERATOR_LIGHTEN;
    case ZATHURA_BLEND_MODE_COLOR_DODGE:
      return CAIRO_OPERATOR_COLOR_DODGE;
    case ZATHURA_BLEND_MODE_COLOR_BURN:
      return CAIRO_OPERATOR_COLOR_BURN;
    case ZATHURA_BLEND_MODE_HARD_LIGHT:
      return CAIRO_OPERATOR_HARD_LIGHT;
    case ZATHURA_BLEND_MODE_SOFT_LIGHT:
      return CAIRO_OPERATOR_SOFT_LIGHT;
    case ZATHURA_BLEND_MODE_DIFFERENCE:
      return CAIRO_OPERATOR_DIFFERENCE;
    case ZATHURA_BLEND_MODE_EXCLUSION:
      return CAIRO_OPERATOR_DIFFERENCE;
  }

  return CAIRO_OPERATOR_OVER;
}

zathura_rectangle_t
zathura_rectangle_scale(zathura_rectangle_t rectangle, double scale)
{
  zathura_rectangle_t scaled_rectangle;

  scaled_rectangle.p1.x = rectangle.p1.x * scale;
  scaled_rectangle.p1.y = rectangle.p1.y * scale;
  scaled_rectangle.p2.x = rectangle.p2.x * scale;
  scaled_rectangle.p2.y = rectangle.p2.y * scale;

  return scaled_rectangle;
}
