 /* See LICENSE file for license and copyright information */

#include "callbacks.h"
#include "internal.h"
#include "grid.h"
#include "../macros.h"

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

void
cb_document_pages_set_highlight_links(GtkWidget* page, ZathuraDocumentPrivate* priv)
{
  g_object_set(G_OBJECT(page), "highlight-links", priv->settings.links.highlight, NULL);
}

void
cb_document_pages_set_edit_form(GtkWidget* page, ZathuraDocumentPrivate* priv)
{
  g_object_set(G_OBJECT(page), "edit-form-fields", priv->settings.forms.edit, NULL);
}

void
cb_document_pages_set_highlight_form(GtkWidget* page, ZathuraDocumentPrivate* priv)
{
  g_object_set(G_OBJECT(page), "highlight-form-fields", priv->settings.forms.highlight, NULL);
}
