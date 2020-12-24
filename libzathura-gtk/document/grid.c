 /* See LICENSE file for license and copyright information */

#include "internal.h"
#include "grid.h"
#include "callbacks.h"
#include "../macros.h"

static double adjustment_get_position(GtkAdjustment* adjustment);
static unsigned int update_visible_pages_and_current_page(ZathuraDocumentPrivate* priv, double x, double y, bool update);

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
    i = nrow * priv->settings.pages_per_row + (priv->settings.first_page_column - 1);
    n = i + priv->settings.pages_per_row;
  }

  /* Fill grid */
  for (; i < n && i < priv->document.number_of_pages; i++) {
    /* Get page widget */
    GtkWidget* page_widget = g_list_nth_data(priv->document.pages, i);

    /* Get coordinates */
    unsigned int x = (i + priv->settings.first_page_column - 1) % priv->settings.pages_per_row;
    unsigned int y = (i + priv->settings.first_page_column - 1) / priv->settings.pages_per_row;

    if (x == 0) {
      gtk_widget_set_halign(page_widget, GTK_ALIGN_END);
    } else {
      gtk_widget_set_halign(page_widget, GTK_ALIGN_START);
    }

    /* Attach to grid */
    gtk_grid_attach(GTK_GRID (priv->gtk.grid), page_widget, x, y, 1, 1);
  }
}

void
zathura_gtk_clear_grid(ZathuraDocumentPrivate* priv)
{
  for (GtkWidget* child = gtk_widget_get_first_child(priv->gtk.grid); child != NULL; child = gtk_widget_get_next_sibling(child)) {
    gtk_grid_remove(GTK_GRID(priv->gtk.grid), child);
  }
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
zathura_gtk_grid_set_position_x(ZathuraDocumentPrivate* priv, gdouble x)
{
  GtkAdjustment* horizontal_adjustment = gtk_scrolled_window_get_hadjustment(
      GTK_SCROLLED_WINDOW(priv->gtk.scrolled_window));

  SET_ADJUSTMENT_TO_VALUE(horizontal_adjustment, x);
}

static void
zathura_gtk_grid_set_position_y(ZathuraDocumentPrivate* priv, gdouble y)
{
  GtkAdjustment* vertical_adjustment = gtk_scrolled_window_get_vadjustment(
      GTK_SCROLLED_WINDOW(priv->gtk.scrolled_window));

  SET_ADJUSTMENT_TO_VALUE(vertical_adjustment, y);
}

void
zathura_gtk_grid_set_position(ZathuraDocumentPrivate* priv, gdouble x, gdouble y)
{
  zathura_gtk_grid_set_position_x(priv, x);
  zathura_gtk_grid_set_position_y(priv, y);
}

static bool
zathura_gtk_grid_is_page_in_grid(ZathuraDocumentPrivate* priv, guint page_number)
{
  GtkWidget* page = g_list_nth_data(priv->document.pages, page_number);
  GtkWidget* parent = gtk_widget_get_parent(page);

  if (parent == NULL || parent != priv->gtk.grid) {
    return false;
  } else {
    return true;
  }
}

void
zathura_gtk_grid_set_page(ZathuraDocumentPrivate* priv, guint page_number)
{
  priv->document.current_page_number = page_number;

  zathura_gtk_page_widget_status_t* widget_status = g_list_nth_data(priv->document.pages_status, page_number);

  if (zathura_gtk_grid_is_page_in_grid(priv, page_number) == false) {
    zathura_gtk_clear_grid(priv);
    zathura_gtk_fill_grid(priv);
  } else {
    zathura_gtk_grid_set_position(priv, widget_status->position.x, widget_status->position.y);
    return;
  }
}

void
cb_scrolled_window_horizontal_adjustment_value_changed(GtkAdjustment*
    horizontal_adjustment, ZathuraDocumentPrivate* priv)
{
  priv->position.x = adjustment_get_position(horizontal_adjustment) * gtk_adjustment_get_upper(horizontal_adjustment);
  update_visible_pages_and_current_page(priv, priv->position.x, priv->position.y, true);
}

void
cb_scrolled_window_horizontal_adjustment_changed(GtkAdjustment*
    UNUSED(horizontal_adjustment), ZathuraDocumentPrivate* priv)
{
  update_visible_pages_and_current_page(priv, priv->position.x, priv->position.y, true);
}

void
cb_scrolled_window_vertical_adjustment_value_changed(GtkAdjustment*
    vertical_adjustment, ZathuraDocumentPrivate* priv)
{
  priv->position.y = adjustment_get_position(vertical_adjustment) * gtk_adjustment_get_upper(vertical_adjustment);
  update_visible_pages_and_current_page(priv, priv->position.x, priv->position.y, true);
}

void
cb_scrolled_window_vertical_adjustment_changed(GtkAdjustment*
    UNUSED(vertical_adjustment), ZathuraDocumentPrivate* priv)
{
  update_visible_pages_and_current_page(priv, priv->position.x, priv->position.y, true);
}

static double
adjustment_get_position(GtkAdjustment* adjustment)
{
  gdouble lower = gtk_adjustment_get_lower(adjustment);
  gdouble upper = gtk_adjustment_get_upper(adjustment);
  gdouble value = gtk_adjustment_get_value(adjustment);

  return (value - lower) / (upper - lower);
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
    gdouble x = widget_status->position.x - page_info->offset.x;
    gdouble y = widget_status->position.y - page_info->offset.y;

    zathura_gtk_grid_set_position(priv, x, y);

    priv->document.current_page_number = page_info->page_number;

    g_free(priv->status.restore_position);
    priv->status.restore_position = NULL;
  }

  return FALSE;
}

guint
zathura_gtk_grid_position_to_page_number(ZathuraDocumentPrivate* priv, gdouble x, gdouble y)
{
  return update_visible_pages_and_current_page(priv, x, y, false);
}

void
zathura_gtk_grid_page_number_to_position(ZathuraDocumentPrivate* priv, unsigned int page_number,
    double x_align, double y_align, double* position_x, double* position_y)
{
  zathura_gtk_page_widget_status_t* widget_status =
    g_list_nth_data(priv->document.pages_status, page_number);

  if (position_x != NULL) {
    *position_x = widget_status->position.x + x_align * widget_status->size.width;
  }

  if (position_y != NULL) {
    *position_y = widget_status->position.y + y_align * widget_status->size.height;
  }
}

static unsigned int
update_visible_pages_and_current_page(ZathuraDocumentPrivate* priv, double x, double y, bool update)
{
  double position_x = x;
  double position_y = y;

  /* Calculate visible area */
  int viewport_width  = gtk_widget_get_allocated_width(priv->gtk.viewport);
  int viewport_height = gtk_widget_get_allocated_height(priv->gtk.viewport);
  int viewport_area   = viewport_width * viewport_height;

  /* Update pages */
  unsigned int new_current_page_number = priv->document.current_page_number;
  float intersecting_area_perc_max = 0;

  for (unsigned int i = 0; i < priv->document.number_of_pages; i++) {
    GtkWidget* page_widget = g_list_nth_data(priv->document.pages, i);
    bool is_visible = false;

    /* Get current size of widget */
    int page_widget_width  = gtk_widget_get_allocated_width(page_widget);
    int page_widget_height = gtk_widget_get_allocated_height(page_widget);

    /* Get status */
    zathura_gtk_page_widget_status_t* widget_status = g_list_nth_data(priv->document.pages_status, i);

    /* Get widget coordinates relative to the scrolled window */
    double page_widget_x, page_widget_y;
    if (gtk_widget_translate_coordinates(page_widget, priv->gtk.grid, 0,
        0, &page_widget_x, &page_widget_y) == FALSE) {
      is_visible = false;

      if (update == true) {
        widget_status->visible = is_visible;
      }

      continue;
    };

    /* Save page coordinates and visibility status */
    if (update == true) {
      widget_status->position.x  = page_widget_x;
      widget_status->position.y  = page_widget_y;
      widget_status->size.width  = page_widget_width;
      widget_status->size.height = page_widget_height;
    }

    /* Check if widget is visible */
    GdkRectangle viewport_rectangle = { position_x, position_y, viewport_width, viewport_height };
    GdkRectangle page_widget_rectangle = { page_widget_x, page_widget_y, page_widget_width, page_widget_height };
    GdkRectangle intersecting_area;

    is_visible = gdk_rectangle_intersect(&viewport_rectangle, &page_widget_rectangle, &intersecting_area);
    if (update == true) {
      widget_status->visible = is_visible;
    }

    if (is_visible == true) {
      int intersecting_area_size = intersecting_area.width * intersecting_area.height;
      float intersecting_area_perc = (float) intersecting_area_size / viewport_area;

      if (intersecting_area_perc >= intersecting_area_perc_max) {
        new_current_page_number = i;
        intersecting_area_perc_max = intersecting_area_perc;
      }
    }
  }

  /* Update current page */
  if (update == true) {
    priv->document.current_page_number = new_current_page_number;
  }

  return new_current_page_number;
}
