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
  cairo_set_source_rgb(cairo, RGB_TO_CAIRO(255, 255, 255));
  cairo_rectangle(cairo, 0, 0, page_width, page_height);
  cairo_fill(cairo);

  /* Create image surface */
  cairo_surface_t* image_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, page_width, page_height);
  if (image_surface == NULL) {
    return FALSE;
  }

  cairo_t* image_cairo = cairo_create(image_surface);
  if (image_cairo == NULL) {
    cairo_surface_destroy(image_surface);
    return FALSE;
  }

  /* Scale */
  cairo_scale(image_cairo, priv->settings.scale, priv->settings.scale);

  /* Render page */
  if (zathura_page_render_cairo(priv->page, image_cairo, priv->settings.scale, 0, 0) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  cairo_destroy(image_cairo);

  cairo_set_source_surface(cairo, image_surface, 0, 0);
  cairo_paint(cairo);
  cairo_restore(cairo);

  /* Clean-up */
  cairo_surface_destroy(image_surface);

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
