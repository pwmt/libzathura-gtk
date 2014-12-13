 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_DOCUMENT
#define ZATHURA_GTK_DOCUMENT

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#define ZATHURA_TYPE_DOCUMENT \
  zathura_gtk_document_get_type()
#define ZATHURA_DOCUMENT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZATHURA_TYPE_DOCUMENT, ZathuraDocument))
#define ZATHURA_DOCUMENT_CLASS(obj) \
  (G_TYPE_CHECK_CLASS_CAST ((obj), ZATHURA_TYPE_DOCUMENT, ZathuraDocumentClass))
#define ZATHURA_IS_PAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZATHURA_TYPE_DOCUMENT))
#define ZATHURA_IS_PAGE_CLASS(obj) \
  (G_TYPE_CHECK_CLASS_TYPE ((obj), ZATHURA_TYPE_DOCUMENT))
#define ZATHURA_DOCUMENT_GET_CLASS \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ZATHURA_TYPE_DOCUMENT, ZathuraDocumentClass))

GType zathura_gtk_document_get_type(void);

GtkWidget* zathura_gtk_document_new(zathura_document_t* document);

#endif /* ZATHURA_GTK_DOCUMENT */

