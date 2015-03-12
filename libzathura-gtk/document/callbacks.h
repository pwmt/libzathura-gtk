 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_DOCUMENT_CALLBACKS
#define ZATHURA_GTK_DOCUMENT_CALLBACKS

#include <gtk/gtk.h>

#include "document.h"

void cb_document_pages_set_rotation(GtkWidget* page, ZathuraDocumentPrivate* priv);

void cb_document_pages_set_scale(GtkWidget* page, ZathuraDocumentPrivate* priv);

void cb_document_pages_set_highlight_links(GtkWidget* page, ZathuraDocumentPrivate* priv);

void cb_document_pages_set_edit_form(GtkWidget* page, ZathuraDocumentPrivate* priv);

void cb_document_pages_set_highlight_form(GtkWidget* page, ZathuraDocumentPrivate* priv);

#endif /* ZATHURA_GTK_DOCUMENT_CALLBACKS */
