 /* See LICENSE file for license and copyright information */

#include "grid.h"
#include "internal.h"

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
  /* Fill grid */
  unsigned int number_of_pages = g_list_length(priv->document.pages);

  for (unsigned int i = 0; i < number_of_pages; i++) {
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

