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
