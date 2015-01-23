 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_DOCUMENT_CALLBACKS
#define ZATHURA_GTK_DOCUMENT_CALLBACKS

#include <gtk/gtk.h>

#include "../document.h"

void cb_scrolled_window_horizontal_adjustment_value_changed(GtkAdjustment* horizontal_adjustment, ZathuraDocumentPrivate* priv);

void cb_scrolled_window_horizontal_adjustment_changed(GtkAdjustment* horizontal_adjustment, ZathuraDocumentPrivate* priv);

void cb_scrolled_window_vertical_adjustment_value_changed(GtkAdjustment* vertical_adjustment, ZathuraDocumentPrivate* priv);

void cb_scrolled_window_vertical_adjustment_changed(GtkAdjustment* vertical_adjustment, ZathuraDocumentPrivate* priv);

void cb_document_pages_set_rotation(GtkWidget* page, ZathuraDocumentPrivate* priv);

#endif /* ZATHURA_GTK_DOCUMENT_CALLBACKS */
