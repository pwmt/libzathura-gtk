 /* See LICENSE file for license and copyright information */

#include "utils.h"
#include "internal.h"

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

  /* Scale rectangle */
  gint scale_factor = gtk_widget_get_scale_factor(GTK_WIDGET(page));
  zathura_rectangle_t correct_position = scale_rectangle(position, priv->settings.scale * scale_factor);

  return correct_position;
}

