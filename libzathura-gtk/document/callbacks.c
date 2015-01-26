 /* See LICENSE file for license and copyright information */

#include "callbacks.h"
#include "internal.h"

static double adjustment_get_ratio(GtkAdjustment* adjustment);
static void update_visible_pages_and_current_page(ZathuraDocumentPrivate* priv, double x, double y);

void
cb_scrolled_window_horizontal_adjustment_value_changed(GtkAdjustment*
    horizontal_adjustment, ZathuraDocumentPrivate* priv)
{
  priv->position.x = adjustment_get_ratio(horizontal_adjustment);
  update_visible_pages_and_current_page(priv, priv->position.x, priv->position.y);
}

void 
cb_scrolled_window_horizontal_adjustment_changed(GtkAdjustment*
    horizontal_adjustment, ZathuraDocumentPrivate* priv)
{
  update_visible_pages_and_current_page(priv, priv->position.x, priv->position.y);
}

void
cb_scrolled_window_vertical_adjustment_value_changed(GtkAdjustment*
    vertical_adjustment, ZathuraDocumentPrivate* priv)
{
  priv->position.y = adjustment_get_ratio(vertical_adjustment);
  update_visible_pages_and_current_page(priv, priv->position.x, priv->position.y);
}

void
cb_scrolled_window_vertical_adjustment_changed(GtkAdjustment*
    vertical_adjustment, ZathuraDocumentPrivate* priv)
{
  update_visible_pages_and_current_page(priv, priv->position.x, priv->position.y);
}

static double
adjustment_get_ratio(GtkAdjustment* adjustment)
{
  gdouble lower     = gtk_adjustment_get_lower(adjustment);
  gdouble upper     = gtk_adjustment_get_upper(adjustment);
  gdouble page_size = gtk_adjustment_get_page_size(adjustment);
  gdouble value     = gtk_adjustment_get_value(adjustment);

  return (value - lower + page_size / 2.0) / (upper - lower);
}

static void
update_visible_pages_and_current_page(ZathuraDocumentPrivate* priv, double x, double y)
{
  int viewport_width  = gtk_widget_get_allocated_width(priv->gtk.viewport);
  int viewport_height = gtk_widget_get_allocated_height(priv->gtk.viewport);
  int viewport_area   = viewport_width * viewport_height;

  GtkWidget* new_current_page_widget = priv->document.current_page;
  float intersecting_area_perc_max = 0;

  for (unsigned int i = 0; i < priv->document.number_of_pages; i++) {
    GtkWidget* page_widget = g_list_nth_data(priv->document.pages, i);

    /* Get current size of widget */
    int page_widget_width  = gtk_widget_get_allocated_width(page_widget);
    int page_widget_height = gtk_widget_get_allocated_height(page_widget);

    /* Get widget coordinates relative to the scrolled window */
    int page_widget_x, page_widget_y;
    gtk_widget_translate_coordinates(page_widget, priv->gtk.scrolled_window, x,
        y, &page_widget_x, &page_widget_y);

    /* Check if widget is visible */
    GdkRectangle viewport_rectangle = { 0, 0, viewport_width, viewport_height };
    GdkRectangle page_widget_rectangle = { page_widget_x, page_widget_y, page_widget_width, page_widget_height };
    GdkRectangle intersecting_area;

    bool is_visible = gdk_rectangle_intersect(&viewport_rectangle, &page_widget_rectangle, &intersecting_area);
    if (is_visible == true) {
      int intersecting_area_size = intersecting_area.width * intersecting_area.height;
      float intersecting_area_perc = (float) intersecting_area_size / viewport_area;

      if (intersecting_area_perc >= intersecting_area_perc_max) {
        new_current_page_widget = page_widget;
        intersecting_area_perc_max = intersecting_area_perc;
      }
    }
  }

  /* Update current page */
  priv->document.current_page = new_current_page_widget;

  return 0;
}

void
cb_document_pages_set_rotation(GtkWidget* page, ZathuraDocumentPrivate* priv)
{
  g_object_set(G_OBJECT(page), "rotation", priv->settings.rotation, NULL);
}

void
cb_document_pages_set_scale(GtkWidget* page, ZathuraDocumentPrivate* priv)
{
  g_object_set(G_OBJECT(page), "scale", priv->settings.scale, NULL);
}
