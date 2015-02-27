 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_PAGE_CALLBACKS
#define ZATHURA_GTK_PAGE_CALLBACKS

#include <gtk/gtk.h>

#include "page.h"

gboolean cb_page_draw(GtkWidget *widget, cairo_t *cairo, gpointer data);
gboolean cb_page_draw_links(GtkWidget *widget, cairo_t *cairo, gpointer data);
gboolean cb_page_draw_form_fields(GtkWidget *widget, cairo_t *cairo, gpointer data);

#endif /* ZATHURA_GTK_PAGE_CALLBACKS */
