 /* See LICENSE file for license and copyright information */

#include "callbacks.h"
#include "internal.h"
#include "grid.h"
#include "../macros.h"

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
    UNUSED(horizontal_adjustment), ZathuraDocumentPrivate* priv)
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
    UNUSED(vertical_adjustment), ZathuraDocumentPrivate* priv)
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

  unsigned int new_current_page_number = priv->document.current_page_number;
  float intersecting_area_perc_max = 0;

  for (unsigned int i = 0; i < priv->document.number_of_pages; i++) {
    GtkWidget* page_widget = g_list_nth_data(priv->document.pages, i);

    /* Get current size of widget */
    int page_widget_width  = gtk_widget_get_allocated_width(page_widget);
    int page_widget_height = gtk_widget_get_allocated_height(page_widget);

    /* Get status */
    zathura_gtk_page_widget_status_t* widget_status = g_list_nth_data(priv->document.pages_status, i);

    /* Get widget coordinates relative to the scrolled window */
    int page_widget_x, page_widget_y;
    if (gtk_widget_translate_coordinates(page_widget, priv->gtk.scrolled_window, x,
        y, &page_widget_x, &page_widget_y) == FALSE) {
      widget_status->visible = false;
      continue;
    };

    /* Save page coordinates and visibility status */
    widget_status->position.x = page_widget_x;
    widget_status->position.y = page_widget_y;

    /* Check if widget is visible */
    GdkRectangle viewport_rectangle = { 0, 0, viewport_width, viewport_height };
    GdkRectangle page_widget_rectangle = { page_widget_x, page_widget_y, page_widget_width, page_widget_height };
    GdkRectangle intersecting_area;

    widget_status->visible = gdk_rectangle_intersect(&viewport_rectangle, &page_widget_rectangle, &intersecting_area);

    if (widget_status->visible == true) {
      int intersecting_area_size = intersecting_area.width * intersecting_area.height;
      float intersecting_area_perc = (float) intersecting_area_size / viewport_area;

      if (intersecting_area_perc >= intersecting_area_perc_max) {
        new_current_page_number = i;
        intersecting_area_perc_max = intersecting_area_perc;
      }
    }
  }

  /* Update current page */
  priv->document.current_page_number = new_current_page_number;

  return;
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

gboolean cb_grid_draw(GtkWidget* UNUSED(scrolled_window), cairo_t* UNUSED(cr),
    ZathuraDocumentPrivate* priv)
{
  if (priv->status.restore_position != NULL) {
    zathura_page_info_t* page_info = priv->status.restore_position;


    /* Get current position */
    zathura_gtk_page_widget_status_t* widget_status =
      g_list_nth_data(priv->document.pages_status, page_info->page_number);

    /* Calculate new position */
    unsigned int x = widget_status->position.x - page_info->position.x;
    unsigned int y = widget_status->position.y - page_info->position.y;

    zathura_gtk_grid_set_position(priv, x, y);

    priv->document.current_page_number = page_info->page_number;

    g_free(priv->status.restore_position);
    priv->status.restore_position = NULL;
  }

  return FALSE;
}
