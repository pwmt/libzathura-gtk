 /* See LICENSE file for license and copyright information */

#include "internal.h"
#include "grid.h"
#include "callbacks.h"

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
zathura_gtk_grid_set_page(ZathuraDocumentPrivate* priv, int page_number)
{
  zathura_gtk_page_widget_status_t* widget_status = g_list_nth_data(priv->document.pages_status, page_number);

  zathura_gtk_grid_set_position_x(priv, widget_status->position.x);
  zathura_gtk_grid_set_position_y(priv, widget_status->position.y);
}
