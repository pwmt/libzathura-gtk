 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_DOCUMENT
#define ZATHURA_GTK_DOCUMENT

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

G_BEGIN_DECLS

typedef struct _ZathuraDocument ZathuraDocument;
typedef struct _ZathuraDocumentClass ZathuraDocumentClass;

#define ZATHURA_TYPE_DOCUMENT            (zathura_gtk_document_get_type())
#define ZATHURA_DOCUMENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_DOCUMENT, ZathuraDocument))
#define ZATHURA_DOCUMENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_DOCUMENT, ZathuraDocumentClass))
#define ZATHURA_IS_DOCUMENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_DOCUMENT))
#define ZATHURA_IS_DOCUMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_DOCUMENT))
#define ZATHURA_DOCUMENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_DOCUMENT, ZathuraDocumentClass))

typedef struct _ZathuraDocumentPrivate ZathuraDocumentPrivate;

struct _ZathuraDocument {
  GtkBin parent;
  ZathuraDocumentPrivate* priv;
};

struct _ZathuraDocumentClass {
  GtkBinClass parent_class;
};


GType zathura_gtk_document_get_type(void);

GtkWidget* zathura_gtk_document_new(zathura_document_t* document);

G_END_DECLS

#endif /* ZATHURA_GTK_DOCUMENT */
