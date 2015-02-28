 /* See LICENSE file for license and copyright information */

#include "callbacks.h"
#include "internal.h"
#include "../macros.h"

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
  gdouble device_scale_x = 1.0;
  gdouble device_scale_y = 1.0;

#ifdef HAVE_HIDPI_SUPPORT
  cairo_surface_t* surface = cairo_get_target(cairo);
  cairo_surface_get_device_scale(surface, &device_scale_x, &device_scale_y);
#endif

  double scale_x = priv->settings.scale * device_scale_x;
  double scale_y = priv->settings.scale * device_scale_y;

  cairo_scale(cairo, scale_x, scale_y);

  /* Render page */
  if (zathura_page_render_cairo(priv->page, cairo, scale_x, 0, 0) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  cairo_set_operator(cairo, CAIRO_OPERATOR_DEST_OVER);
  cairo_paint(cairo);
  cairo_restore(cairo);

  return FALSE;
}

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

static zathura_rectangle_t
calculate_correct_position(ZathuraPagePrivate* priv, GtkWidget* UNUSED(widget), zathura_rectangle_t position)
{
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

gboolean
cb_page_draw_links(GtkWidget *widget, cairo_t *cairo, gpointer data)
{
  ZathuraPagePrivate* priv = (ZathuraPagePrivate*) data;

  /* Draw links if requested */
  if (priv->links.draw == true) {
    /* Retrieve links of page */
    if (priv->links.retrieved == false) {
      priv->links.retrieved = true;
      if (zathura_page_get_links(priv->page, &(priv->links.list)) != ZATHURA_ERROR_OK) {
        return FALSE;
      }
    }

    cairo_save(cairo);

    /* Draw each link */
    zathura_link_mapping_t* link_mapping;
    ZATHURA_LIST_FOREACH(link_mapping, priv->links.list) {
      zathura_rectangle_t position = calculate_correct_position(priv, widget, link_mapping->position);
      unsigned int width  = position.p2.x - position.p1.x;
      unsigned int height = position.p2.y - position.p1.y;

      /* Fill pink */
      cairo_set_source_rgba(cairo, 72, 0, 98, 0.5);
      cairo_rectangle(cairo, position.p1.x, position.p1.y, width, height);
      cairo_fill(cairo);
    }

    cairo_restore(cairo);
  }

  return FALSE;
}
