 /* See LICENSE file for license and copyright information */

#include "callbacks.h"
#include "internal.h"

gboolean
cb_page_draw(GtkWidget *widget, cairo_t *cairo, gpointer data)
{
  ZathuraPagePrivate* priv = (ZathuraPagePrivate*) data;

  const unsigned int page_width  = gtk_widget_get_allocated_width(widget);
  const unsigned int page_height = gtk_widget_get_allocated_height(widget);

  cairo_save(cairo);

  /* Fill white */
  cairo_set_source_rgb(cairo, 255, 255, 255);
  cairo_rectangle(cairo, 0, 0, page_width, page_height);
  cairo_fill(cairo);

  /* Rotate */
  switch (priv->settings.rotation) {
    case 90:
      cairo_translate(cairo, page_width, 0);
      break;
    case 180:
      cairo_translate(cairo, page_width, page_height);
      break;
    case 270:
      cairo_translate(cairo, 0, page_height);
      break;
  }

  if (priv->settings.rotation != 0) {
    cairo_rotate(cairo, priv->settings.rotation * G_PI / 180.0);
  }

  /* Scale */
  cairo_scale(cairo, priv->settings.scale, priv->settings.scale);

  if (zathura_page_render_cairo(priv->page, cairo, priv->settings.scale, 0, 0) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  cairo_set_operator(cairo, CAIRO_OPERATOR_DEST_OVER);
  cairo_paint(cairo);
  cairo_restore(cairo);

  return FALSE;
}

