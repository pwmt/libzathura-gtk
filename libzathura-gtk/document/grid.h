 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_DOCUMENT_GRID
#define ZATHURA_GTK_DOCUMENT_GRID

#include "document.h"

void zathura_gtk_setup_grid(ZathuraDocumentPrivate* priv);
void zathura_gtk_fill_grid(ZathuraDocumentPrivate* priv);
void zathura_gtk_clear_grid(ZathuraDocumentPrivate* priv);
void zathura_gtk_free_grid(ZathuraDocumentPrivate* priv);

void zathura_gtk_grid_set_position(ZathuraDocumentPrivate* priv, gdouble x, gdouble y);
void zathura_gtk_grid_set_page(ZathuraDocumentPrivate* priv, guint page_number);

void cb_scrolled_window_horizontal_adjustment_value_changed(GtkAdjustment* horizontal_adjustment, ZathuraDocumentPrivate* priv);

void cb_scrolled_window_horizontal_adjustment_changed(GtkAdjustment* horizontal_adjustment, ZathuraDocumentPrivate* priv);

void cb_scrolled_window_vertical_adjustment_value_changed(GtkAdjustment* vertical_adjustment, ZathuraDocumentPrivate* priv);

void cb_scrolled_window_vertical_adjustment_changed(GtkAdjustment* vertical_adjustment, ZathuraDocumentPrivate* priv);

gboolean cb_grid_draw(GtkWidget* scrolled_window, cairo_t* cr, ZathuraDocumentPrivate* priv);


#endif /* ZATHURA_GTK_DOCUMENT_GRID */
