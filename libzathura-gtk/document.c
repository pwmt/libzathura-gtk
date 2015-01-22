 /* See LICENSE file for license and copyright information */

#include "document.h"
#include "page.h"

static void zathura_gtk_document_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_document_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void zathura_gtk_setup_grid(ZathuraDocumentPrivate* priv);
static void zathura_gtk_fill_grid(ZathuraDocumentPrivate* priv);
static void zathura_gtk_clear_grid(ZathuraDocumentPrivate* priv);
static void zathura_gtk_free_grid(ZathuraDocumentPrivate* priv);
static void set_continuous_pages(ZathuraDocumentPrivate* priv, gboolean enable);
static void set_pages_per_row(ZathuraDocumentPrivate* priv, guint pages_per_row);

struct _ZathuraDocumentPrivate {
  struct {
    zathura_document_t* document;
    GList* pages;
    GtkWidget* current_page;
  } document;

  struct {
    GtkWidget* scrolled_window;
    GtkWidget* viewport;
    GtkWidget* grid;
  } gtk;

  struct {
    gboolean continuous_pages;
    guint pages_per_row;
  } settings;
};

enum {
  PROP_0,
  PROP_DOCUMENT,
  PROP_CONTINUOUS_PAGES,
  PROP_PAGES_PER_ROW
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

  g_object_class_install_property(
    object_class,
    PROP_CONTINUOUS_PAGES,
    g_param_spec_boolean(
      "continuous-pages",
      "Continuous-pages",
      "Enables continuous pages",
      TRUE,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_PAGES_PER_ROW,
    g_param_spec_uint(
      "pages-per-row",
      "pages-per-row",
      "Defines the number of pages per row",
      1,
      G_MAXUINT,
      1,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );
}

static void
zathura_gtk_document_init(ZathuraDocument* widget)
{
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(widget);
  priv->document.document         = NULL;
  priv->document.pages            = NULL;
  priv->gtk.scrolled_window       = NULL;
  priv->gtk.viewport              = NULL;
  priv->gtk.grid                  = NULL;
  priv->settings.continuous_pages = TRUE;
  priv->settings.pages_per_row    = 1;
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
  priv->gtk.scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (priv->gtk.scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  /* Setup viewport */
  priv->gtk.viewport = gtk_viewport_new(NULL, NULL);

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
    gtk_widget_set_halign(page_widget, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(page_widget, GTK_ALIGN_CENTER);

    /* Append to list */
    priv->document.pages = g_list_append(priv->document.pages, page_widget);
  }

  /* Setup grid */
  zathura_gtk_setup_grid(priv);

  /* Setup container */
  gtk_container_add(GTK_CONTAINER(priv->gtk.scrolled_window), GTK_WIDGET(priv->gtk.viewport));
  gtk_container_add(GTK_CONTAINER(priv->gtk.viewport), GTK_WIDGET(priv->gtk.grid));
  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->gtk.scrolled_window));

  return GTK_WIDGET(widget);
}

static void
zathura_gtk_setup_grid(ZathuraDocumentPrivate* priv)
{
  /* Setup grid */
  priv->gtk.grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(priv->gtk.grid), 10);
  gtk_grid_set_row_homogeneous(GTK_GRID(priv->gtk.grid), FALSE);
  gtk_grid_set_column_homogeneous(GTK_GRID(priv->gtk.grid), FALSE);
  gtk_widget_set_halign(priv->gtk.grid, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(priv->gtk.grid, GTK_ALIGN_CENTER);

  /* Fill grid */
  zathura_gtk_fill_grid(priv);
}

static void
zathura_gtk_fill_grid(ZathuraDocumentPrivate* priv)
{
  /* Fill grid */
  unsigned int number_of_pages = g_list_length(priv->document.pages);

  for (unsigned int i = 0; i < number_of_pages; i++) {
    /* Get page widget */
    GtkWidget* page_widget = g_list_nth_data(priv->document.pages, i);

    /* Get coordinates */
    unsigned int x = i % priv->settings.pages_per_row;
    unsigned int y = i / priv->settings.pages_per_row;

    if (x == 0) {
      gtk_widget_set_halign(page_widget, GTK_ALIGN_END);
    } else {
      gtk_widget_set_halign(page_widget, GTK_ALIGN_START);
    }

    /* Attach to grid */
    gtk_grid_attach(GTK_GRID (priv->gtk.grid), page_widget, x, y, 1, 1);
  }

  gtk_widget_show_all(priv->gtk.grid);
}

static void
cb_zathura_gtk_grid_clear(GtkWidget* widget, gpointer data)
{
  g_object_ref(widget);
  gtk_container_remove(GTK_CONTAINER(data), widget);
}

static void
zathura_gtk_clear_grid(ZathuraDocumentPrivate* priv)
{
  gtk_container_foreach(GTK_CONTAINER(priv->gtk.grid), cb_zathura_gtk_grid_clear, priv->gtk.grid);
}

static void
zathura_gtk_free_grid(ZathuraDocumentPrivate* priv)
{
  zathura_gtk_clear_grid(priv);
  g_object_unref(priv->gtk.grid);
  priv->gtk.grid = NULL;
}

static void
zathura_gtk_document_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraDocument* document    = ZATHURA_DOCUMENT(object);
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(document);

  switch (prop_id) {
    case PROP_DOCUMENT:
      priv->document.document = g_value_get_pointer(value);
      break;
    case PROP_CONTINUOUS_PAGES:
      set_continuous_pages(priv, g_value_get_boolean(value));
      break;
    case PROP_PAGES_PER_ROW:
      set_pages_per_row(priv, g_value_get_uint(value));
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
      g_value_set_pointer(value, priv->document.document);
      break;
    case PROP_CONTINUOUS_PAGES:
      g_value_set_boolean(value, priv->settings.continuous_pages);
      break;
    case PROP_PAGES_PER_ROW:
      g_value_set_uint(value, priv->settings.pages_per_row);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
set_continuous_pages(ZathuraDocumentPrivate* priv, gboolean enable)
{
  /* Remove child from viewport */
  GtkWidget* child = gtk_bin_get_child(GTK_BIN(priv->gtk.viewport));
  if (child != NULL) {
    g_object_ref(child);
    gtk_container_remove(GTK_CONTAINER(priv->gtk.viewport), child);
  }

  /* Setup grid */
  if (enable == TRUE && priv->settings.continuous_pages == FALSE) {
    zathura_gtk_setup_grid(priv);
    gtk_container_add(GTK_CONTAINER(priv->gtk.viewport), GTK_WIDGET(priv->gtk.grid));
  /* Setup single page */
  } else if (enable == FALSE && priv->settings.continuous_pages == TRUE) {
    zathura_gtk_free_grid(priv);

    GtkWidget* current_page = (GtkWidget*) g_list_nth_data(priv->document.pages, 0); // FIXME: Current page
    gtk_container_add(GTK_CONTAINER(priv->gtk.viewport), GTK_WIDGET(current_page));
  }

  priv->settings.continuous_pages = enable;
}

static void
set_pages_per_row(ZathuraDocumentPrivate* priv, guint pages_per_row)
{
  /* Update pages per row */
  priv->settings.pages_per_row = pages_per_row;

  /* Empty and refill grid */
  zathura_gtk_clear_grid(priv);
  zathura_gtk_fill_grid(priv);
}
