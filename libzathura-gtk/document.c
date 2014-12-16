 /* See LICENSE file for license and copyright information */

#include "document.h"
#include "page.h"

static void zathura_gtk_document_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_document_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);

struct _ZathuraDocumentPrivate {
  zathura_document_t* document;
  GList* pages;
  GtkWidget* scrolled_window;
  GtkWidget* grid;
};

enum {
  PROP_0,
  PROP_DOCUMENT
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraDocument, zathura_gtk_document, GTK_TYPE_BIN)

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
  priv->document        = NULL;
  priv->pages           = NULL;
  priv->scrolled_window = NULL;
  priv->grid            = NULL;
}

GtkWidget*
zathura_gtk_document_new(zathura_document_t* document)
{
  if (document == NULL) {
    return NULL;
  }

  GObject* widget = g_object_new(ZATHURA_TYPE_DOCUMENT, "document", document, NULL);

  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(widget);

  /* Setup scrolled window */
  priv->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (priv->scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  /* Setup grid */
  priv->grid = gtk_grid_new();

  /* Create page widgets */
  unsigned int number_of_pages = 0;
  if (zathura_document_get_number_of_pages(document, &number_of_pages) !=
      ZATHURA_ERROR_OK) {
    return NULL; // FIXME: Leaks
  }

  for (unsigned int i = 0; i < number_of_pages; i++) {
    zathura_page_t* page;
    if (zathura_document_get_page(document, i, &page) != ZATHURA_ERROR_OK) {
      return NULL; // FIXME: Leaks
    }

    /* Create page widget */
    GtkWidget* page_widget = zathura_gtk_page_new(page);

    /* Attach to grid */
    if (priv->pages == NULL) {
      gtk_grid_attach (GTK_GRID (priv->grid), page_widget, 0, 0, 1, 1);
    } else {
      GList* last_page = g_list_last(priv->pages);
      gtk_grid_attach_next_to (GTK_GRID (priv->grid), page_widget, last_page->data, GTK_POS_BOTTOM, 1, 1);
    }

    /* Append to list */
    priv->pages = g_list_append(priv->pages, page_widget);
  }

  /* Setup container */
  gtk_container_add(GTK_CONTAINER(priv->scrolled_window), GTK_WIDGET(priv->grid));
  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->scrolled_window));

  return GTK_WIDGET(widget);
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
