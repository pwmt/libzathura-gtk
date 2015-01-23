 /* See LICENSE file for license and copyright information */

#include <math.h>

#include "document.h"
#include "page.h"
#include "document/internal.h"
#include "document/callbacks.h"
#include "document/grid.h"

static void zathura_gtk_document_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_document_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void set_continuous_pages(ZathuraDocumentPrivate* priv, gboolean enable);
static void set_pages_per_row(ZathuraDocumentPrivate* priv, guint pages_per_row);

enum {
  PROP_0,
  PROP_DOCUMENT,
  PROP_CONTINUOUS_PAGES,
  PROP_PAGES_PER_ROW,
  PROP_ROTATION,
  PROP_SCALE
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

  g_object_class_install_property(
    object_class,
    PROP_ROTATION,
    g_param_spec_uint(
      "rotation",
      "Rotation",
      "Defines the degree of rotation (0, 90, 180, 270)",
      0,
      270,
      0,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_SCALE,
    g_param_spec_double(
      "scale",
      "Scale",
      "The scale level",
      0.01,
      100.0,
      1.0,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );
}

static void
zathura_gtk_document_init(ZathuraDocument* widget)
{
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(widget);

  priv->document.document        = NULL;
  priv->document.pages           = NULL;
  priv->document.number_of_pages = 0;

  priv->gtk.scrolled_window = NULL;
  priv->gtk.viewport        = NULL;
  priv->gtk.grid            = NULL;

  priv->settings.continuous_pages = TRUE;
  priv->settings.pages_per_row    = 1;
  priv->settings.rotation         = 0;
  priv->settings.scale            = 1.0;

  priv->position.x = 0.0;
  priv->position.y = 0.0;
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

  GtkAdjustment* horizontal_adjustment = gtk_scrolled_window_get_hadjustment(
      GTK_SCROLLED_WINDOW(priv->gtk.scrolled_window)
      );

  g_signal_connect(
      G_OBJECT(horizontal_adjustment),
      "value-changed",
      G_CALLBACK(cb_scrolled_window_horizontal_adjustment_value_changed),
      priv);

  g_signal_connect(
      G_OBJECT(horizontal_adjustment),
      "changed",
      G_CALLBACK(cb_scrolled_window_horizontal_adjustment_changed),
      priv);

  GtkAdjustment* vertical_adjustment = gtk_scrolled_window_get_vadjustment(
      GTK_SCROLLED_WINDOW(priv->gtk.scrolled_window)
      );

  g_signal_connect(
      G_OBJECT(vertical_adjustment),
      "value-changed",
      G_CALLBACK(cb_scrolled_window_vertical_adjustment_value_changed),
      priv);

  g_signal_connect(
      G_OBJECT(vertical_adjustment),
      "changed",
      G_CALLBACK(cb_scrolled_window_vertical_adjustment_changed),
      priv);

  /* Setup viewport */
  priv->gtk.viewport = gtk_viewport_new(NULL, NULL);

  /* Create page widgets */
  if (zathura_document_get_number_of_pages(document, &(priv->document.number_of_pages)) !=
      ZATHURA_ERROR_OK) {
    return NULL; // FIXME: Leaks
  }

  for (unsigned int i = 0; i < priv->document.number_of_pages; i++) {
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
    case PROP_ROTATION:
      {
        unsigned int rotation = g_value_get_uint(value);
        switch (rotation) {
          case 0:
          case 90:
          case 180:
          case 270:
            if (priv->settings.rotation != rotation) {
              priv->settings.rotation = rotation;
              g_list_foreach(priv->document.pages, (GFunc) cb_document_pages_set_rotation, priv);
            }
            break;
          default:
            // TODO: Pring warning message
            break;
        }
      }
      break;
    case PROP_SCALE:
      {
        double scale = g_value_get_double(value);
        if (priv->settings.scale != scale) {
          priv->settings.scale = scale;
          g_list_foreach(priv->document.pages, (GFunc) cb_document_pages_set_scale, priv);
        }
      }
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
    case PROP_ROTATION:
      g_value_set_uint(value, priv->settings.rotation);
      break;
    case PROP_SCALE:
      g_value_set_double(value, priv->settings.scale);
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

    GtkWidget* current_page = priv->document.current_page;
    gtk_widget_set_halign(current_page, GTK_ALIGN_CENTER);
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
  if (priv->settings.continuous_pages == true) {
    zathura_gtk_clear_grid(priv);
    zathura_gtk_fill_grid(priv);
  }
}
