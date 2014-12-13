 /* See LICENSE file for license and copyright information */

#include "document.h"

static void zathura_gtk_document_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_document_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);

struct _ZathuraDocumentPrivate {
  zathura_document_t* document;
};

enum {
  PROP_0,
  PROP_DOCUMENT
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraDocument, zathura_gtk_document, GTK_TYPE_CONTAINER)

#define ZATHURA_DOCUMENT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_DOCUMENT, \
                               ZathuraDocumentPrivate))

static void
zathura_gtk_document_class_init(ZathuraDocumentClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_document_set_property;
  object_class->get_property = zathura_gtk_document_get_property;

  /* properties */
  g_object_class_install_property(
    object_class,
    PROP_DOCUMENT,
    g_param_spec_pointer(
      "document",
      "Document",
      "The zathura_document_t instance",
      G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS
    )
  );
}

static void
zathura_gtk_document_init(ZathuraDocument* widget)
{
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(widget);
  priv->document = NULL;
}

GtkWidget*
zathura_gtk_document_new(zathura_document_t* document)
{
  if (document == NULL) {
    return NULL;
  }

  GObject* ret = g_object_new(ZATHURA_TYPE_DOCUMENT, "document", document, NULL);
  if (ret == NULL) {
    return NULL;
  }

  return GTK_WIDGET(ret);
}

static void
zathura_gtk_document_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraDocument* document    = ZATHURA_DOCUMENT(object);
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(document);

  switch (prop_id) {
    case PROP_DOCUMENT:
      priv->document = g_value_get_pointer(value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
zathura_gtk_document_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec)
{
  ZathuraDocument* document    = ZATHURA_DOCUMENT(object);
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(document);

  switch (prop_id) {
    case PROP_DOCUMENT:
      g_value_set_pointer(value, priv->document);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}
