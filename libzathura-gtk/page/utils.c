 /* See LICENSE file for license and copyright information */

#include "utils.h"
#include "internal.h"

static zathura_rectangle_t
rotate_rectangle(zathura_rectangle_t rectangle, unsigned int degree, unsigned int width, unsigned int height)
{
  zathura_rectangle_t tmp;

  switch (degree) {
    case 90:
      tmp.p1.x = height - rectangle.p2.y;
      tmp.p1.y = rectangle.p1.x;
      tmp.p2.x = height - rectangle.p1.y;
      tmp.p2.y = rectangle.p2.x;
      break;
    case 180:
      tmp.p1.x = width  - rectangle.p2.x;
      tmp.p1.y = height - rectangle.p2.y;
      tmp.p2.x = width  - rectangle.p1.x;
      tmp.p2.y = height - rectangle.p1.y;
      break;
    case 270:
      tmp.p1.x = rectangle.p1.y;
      tmp.p1.y = width - rectangle.p2.x;
      tmp.p2.x = rectangle.p2.y;
      tmp.p2.y = width - rectangle.p1.x;
      break;
    default:
      tmp.p1.x = rectangle.p1.x;
      tmp.p1.y = rectangle.p1.y;
      tmp.p2.x = rectangle.p2.x;
      tmp.p2.y = rectangle.p2.y;
      break;
  }

  return tmp;
}

static zathura_rectangle_t
scale_rectangle(zathura_rectangle_t rectangle, double scale)
{
  zathura_rectangle_t scaled_rectangle;

  scaled_rectangle.p1.x = rectangle.p1.x * scale;
  scaled_rectangle.p1.y = rectangle.p1.y * scale;
  scaled_rectangle.p2.x = rectangle.p2.x * scale;
  scaled_rectangle.p2.y = rectangle.p2.y * scale;

  return scaled_rectangle;
}

zathura_rectangle_t
calculate_correct_position(ZathuraPage* page, zathura_rectangle_t position)
{
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(page);

  /* Rotate rectangle */
  zathura_rectangle_t correct_position = rotate_rectangle(
      position,
      priv->settings.rotation,
      priv->dimensions.width,
      priv->dimensions.height
      );

  /* Scale rectangle */
  correct_position = scale_rectangle(correct_position, priv->settings.scale);

  return correct_position;
}

