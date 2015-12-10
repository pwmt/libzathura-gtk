 /* See LICENSE file for license and copyright information */

#include "callbacks.h"
#include "internal.h"
#include "utils.h"
#include "../macros.h"

#define RGB_TO_CAIRO(r, g, b) \
  (r)/255.0, (g)/255.0, (b)/255.0


gboolean
cb_page_draw(GtkWidget *widget, cairo_t *cairo, gpointer data)
{
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(data);

  const unsigned int page_width  = gtk_widget_get_allocated_width(widget);
  const unsigned int page_height = gtk_widget_get_allocated_height(widget);

  cairo_save(cairo);

  /* Fill white */
  cairo_set_source_rgb(cairo, 255, 255, 255);
  cairo_rectangle(cairo, 0, 0, page_width, page_height);
  cairo_fill(cairo);

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

  return TRUE;
}

gboolean
cb_page_draw_links(GtkWidget* UNUSED(widget), cairo_t *cairo, gpointer data)
{
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(data);

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
      zathura_rectangle_t position = calculate_correct_position(ZATHURA_PAGE(data), link_mapping->position);
      unsigned int width  = position.p2.x - position.p1.x;
      unsigned int height = position.p2.y - position.p1.y;

      cairo_set_line_width(cairo, 1);
      cairo_set_source_rgb(cairo, RGB_TO_CAIRO(84, 208, 237));

      cairo_rectangle(cairo, position.p1.x, position.p1.y, width, height);
      cairo_stroke(cairo);
    }

    cairo_restore(cairo);
  }

  return FALSE;
}

void
cb_page_overlay_realized(GtkWidget* UNUSED(widget), gpointer data)
{
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(data);

  if (priv->form_fields.edit == true) {
    gtk_widget_show(priv->layer.form_fields);
  } else {
    gtk_widget_hide(priv->layer.form_fields);
  }
}
