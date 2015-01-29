 /* See LICENSE file for license and copyright information */

#include "internal.h"
#include "grid.h"
#include "callbacks.h"
#include "../macros.h"

static double adjustment_get_position(GtkAdjustment* adjustment);
static void update_visible_pages_and_current_page(ZathuraDocumentPrivate* priv, double x, double y);

void
zathura_gtk_setup_grid(ZathuraDocumentPrivate* priv)
{
  /* Setup grid */
  priv->gtk.grid = gtk_grid_new();

  gtk_grid_set_row_spacing(GTK_GRID(priv->gtk.grid), 10);
  gtk_grid_set_column_spacing(GTK_GRID(priv->gtk.grid), 2);
  gtk_grid_set_row_homogeneous(GTK_GRID(priv->gtk.grid), FALSE);
  gtk_grid_set_column_homogeneous(GTK_GRID(priv->gtk.grid), FALSE);
  gtk_widget_set_halign(priv->gtk.grid, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(priv->gtk.grid, GTK_ALIGN_CENTER);

  /* Fill grid */
  zathura_gtk_fill_grid(priv);
}

void
zathura_gtk_fill_grid(ZathuraDocumentPrivate* priv)
{
  unsigned int i = 0;
  unsigned int n = priv->document.number_of_pages;

  /* If we have continuous pages disable we calculate the pages
   * which are in the same row as the current page */
  if (priv->settings.continuous_pages == false) {
    unsigned int nrow = priv->document.current_page_number / priv->settings.pages_per_row;
    i = nrow * priv->settings.pages_per_row;
    n = i + priv->settings.pages_per_row;
  }

  /* Fill grid */
  for (; i < n && i < priv->document.number_of_pages; i++) {
    /* Get page widget */
    GtkWidget* page_widget = g_list_nth_data(priv->document.pages, i);

    /* Get coordinates */
    unsigned int x = i % priv->settings.pages_per_row;
    unsigned int y = i / priv->settings.pages_per_row;

    if (x == 0) {
      gtk_widget_set_halign(page_widget, GTK_ALIGN_END);
    } else {
      gtk_widget_set_halign(page_widget, GTK_ALIGN_START);
    }

    /* Attach to grid */
    gtk_grid_attach(GTK_GRID (priv->gtk.grid), page_widget, x, y, 1, 1);
  }

  gtk_widget_show_all(priv->gtk.grid);
}

void
cb_zathura_gtk_grid_clear(GtkWidget* widget, gpointer data)
{
  g_object_ref(widget);
  gtk_container_remove(GTK_CONTAINER(data), widget);
}

void
zathura_gtk_clear_grid(ZathuraDocumentPrivate* priv)
{
  gtk_container_foreach(GTK_CONTAINER(priv->gtk.grid), cb_zathura_gtk_grid_clear, priv->gtk.grid);
}

void
zathura_gtk_free_grid(ZathuraDocumentPrivate* priv)
{
  zathura_gtk_clear_grid(priv);
  g_object_unref(priv->gtk.grid);
  priv->gtk.grid = NULL;
}

#define SET_ADJUSTMENT_TO_VALUE(adjustment, value) \
  gtk_adjustment_set_value((adjustment), MAX(gtk_adjustment_get_lower((adjustment)), \
        MIN( gtk_adjustment_get_upper((adjustment)) - \
          gtk_adjustment_get_page_size((adjustment)), (value))));

static void
zathura_gtk_grid_set_position_x(ZathuraDocumentPrivate* priv, int x)
{
  GtkAdjustment* horizontal_adjustment = gtk_scrolled_window_get_hadjustment(
      GTK_SCROLLED_WINDOW(priv->gtk.scrolled_window));

  SET_ADJUSTMENT_TO_VALUE(horizontal_adjustment, x);
}

static void
zathura_gtk_grid_set_position_y(ZathuraDocumentPrivate* priv, int y)
{
  GtkAdjustment* vertical_adjustment = gtk_scrolled_window_get_vadjustment(
      GTK_SCROLLED_WINDOW(priv->gtk.scrolled_window));

  SET_ADJUSTMENT_TO_VALUE(vertical_adjustment, y);
}

void
zathura_gtk_grid_set_position(ZathuraDocumentPrivate* priv, int x, int y)
{
  zathura_gtk_grid_set_position_x(priv, x);
  zathura_gtk_grid_set_position_y(priv, y);
}

void
zathura_gtk_grid_set_page(ZathuraDocumentPrivate* priv, guint page_number)
{
  zathura_gtk_page_widget_status_t* widget_status = g_list_nth_data(priv->document.pages_status, page_number);

  zathura_gtk_grid_set_position(priv, widget_status->position.x, widget_status->position.y);

  priv->document.current_page_number = page_number;
}

void
cb_scrolled_window_horizontal_adjustment_value_changed(GtkAdjustment*
    horizontal_adjustment, ZathuraDocumentPrivate* priv)
{
  priv->position.x = adjustment_get_position(horizontal_adjustment);
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
  priv->position.y = adjustment_get_position(vertical_adjustment);
  update_visible_pages_and_current_page(priv, priv->position.x, priv->position.y);
}

void
cb_scrolled_window_vertical_adjustment_changed(GtkAdjustment*
    UNUSED(vertical_adjustment), ZathuraDocumentPrivate* priv)
{
  update_visible_pages_and_current_page(priv, priv->position.x, priv->position.y);
}

static double
adjustment_get_position(GtkAdjustment* adjustment)
{
  gdouble lower = gtk_adjustment_get_lower(adjustment);
  gdouble upper = gtk_adjustment_get_upper(adjustment);
  gdouble value = gtk_adjustment_get_value(adjustment);

  return (value - lower) / (upper - lower);
}

static void
update_visible_pages_and_current_page(ZathuraDocumentPrivate* priv, double x, double y)
{
  /* Calculate current position */
  GtkAdjustment* horizontal_adjustment = gtk_scrolled_window_get_hadjustment(
      GTK_SCROLLED_WINDOW(priv->gtk.scrolled_window)
      );

  double position_x = gtk_adjustment_get_upper(horizontal_adjustment) * x;

  GtkAdjustment* vertical_adjustment = gtk_scrolled_window_get_vadjustment(
      GTK_SCROLLED_WINDOW(priv->gtk.scrolled_window)
      );

  double position_y = gtk_adjustment_get_upper(vertical_adjustment) * y;

  /* Calculate visible area */
  int viewport_width  = gtk_widget_get_allocated_width(priv->gtk.viewport);
  int viewport_height = gtk_widget_get_allocated_height(priv->gtk.viewport);
  int viewport_area   = viewport_width * viewport_height;

  /* Update pages */
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
    if (gtk_widget_translate_coordinates(page_widget, priv->gtk.grid, 0,
        0, &page_widget_x, &page_widget_y) == FALSE) {
      widget_status->visible = false;
      continue;
    };

    /* Save page coordinates and visibility status */
    widget_status->position.x = page_widget_x;
    widget_status->position.y = page_widget_y;

    /* Check if widget is visible */
    GdkRectangle viewport_rectangle = { position_x, position_y, viewport_width, viewport_height };
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

gboolean cb_grid_draw(GtkWidget* UNUSED(scrolled_window), cairo_t* UNUSED(cr),
    ZathuraDocumentPrivate* priv)
{
  if (priv->status.restore_position != NULL) {
    zathura_page_info_t* page_info = priv->status.restore_position;

    /* Get current position */
    zathura_gtk_page_widget_status_t* widget_status =
      g_list_nth_data(priv->document.pages_status, page_info->page_number);

    /* Calculate new position */
    unsigned int x = widget_status->position.x - page_info->offset.x;
    unsigned int y = widget_status->position.y - page_info->offset.y;

    zathura_gtk_grid_set_position(priv, x, y);

    priv->document.current_page_number = page_info->page_number;

    g_free(priv->status.restore_position);
    priv->status.restore_position = NULL;
  }

  return FALSE;
}
