 /* See LICENSE file for license and copyright information */

#include <math.h>

#include "document.h"
#include "internal.h"
#include "callbacks.h"
#include "grid.h"

#include "../page/page.h"

static void zathura_gtk_document_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_document_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void set_continuous_pages(ZathuraDocumentPrivate* priv, gboolean enable);
static void set_pages_per_row(ZathuraDocumentPrivate* priv, guint pages_per_row);
static void set_current_page_number(ZathuraDocumentPrivate* priv, guint page_number);
static void restore_current_page(ZathuraDocumentPrivate* priv);

enum {
  PROP_0,
  PROP_DOCUMENT,
  PROP_CURRENT_PAGE_NUMBER,
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
    PROP_CURRENT_PAGE_NUMBER,
    g_param_spec_uint(
      "current-page-number",
      "current-page-number",
      "Defines the current page number",
      0,
      G_MAXUINT,
      0,
      G_PARAM_WRITABLE | G_PARAM_READABLE
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
  priv->document.pages_status    = NULL;
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

  priv->status.restore_position = FALSE;
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

  g_signal_connect(
      G_OBJECT(priv->gtk.scrolled_window),
      "draw",
      G_CALLBACK(cb_grid_draw),
      priv);

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
    g_object_unref(priv->gtk.scrolled_window);
    g_object_unref(priv->gtk.viewport);
    g_object_unref(widget);
    return NULL;
  }

  for (unsigned int i = 0; i < priv->document.number_of_pages; i++) {
    zathura_page_t* page;
    if (zathura_document_get_page(document, i, &page) != ZATHURA_ERROR_OK) {
      g_object_unref(priv->gtk.scrolled_window);
      g_object_unref(priv->gtk.viewport);
      g_object_unref(widget);
      g_list_foreach(priv->document.pages, (GFunc) g_object_unref, NULL);
      g_list_foreach(priv->document.pages, (GFunc) g_free, NULL);
      return NULL;
    }

    /* Create page widget */
    GtkWidget* page_widget = zathura_gtk_page_new(page);
    gtk_widget_set_halign(page_widget, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(page_widget, GTK_ALIGN_CENTER);

    /* Create new page status */
    zathura_gtk_page_widget_status_t* page_widget_status =
      g_malloc0(sizeof(zathura_gtk_page_widget_status_t));
    page_widget_status->visible = false;

    /* Append to list */
    priv->document.pages        = g_list_append(priv->document.pages, page_widget);
    priv->document.pages_status = g_list_append(priv->document.pages_status, page_widget_status);
  }

  /* Setup grid */
  zathura_gtk_setup_grid(priv);

  /* Setup container */
  gtk_container_add(GTK_CONTAINER(priv->gtk.scrolled_window), GTK_WIDGET(priv->gtk.viewport));
  gtk_container_add(GTK_CONTAINER(priv->gtk.viewport), GTK_WIDGET(priv->gtk.grid));
  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->gtk.scrolled_window));

  return GTK_WIDGET(widget);
}

void
zathura_gtk_document_scroll(GtkWidget* document, zathura_gtk_document_scroll_direction_t direction)
{
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(document);

  gdouble position_x = priv->position.x;
  gdouble position_y = priv->position.y;

  gdouble scroll_step = 40;
  gdouble scroll_direction = 1.0;

  switch (direction) {
    case LEFT:
    case UP:
      scroll_direction = -1.0;
      break;
    default:
      break;
  }

  switch (direction) {
    case UP:
    case DOWN:
      position_y += scroll_direction * scroll_step;
      break;
    case RIGHT:
    case LEFT:
      position_x += scroll_direction * scroll_step;
      break;
    default:
      break;
  }

  zathura_gtk_grid_set_position(priv, position_x, position_y);
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
    case PROP_CURRENT_PAGE_NUMBER:
      set_current_page_number(priv, g_value_get_uint(value));
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
    case PROP_CURRENT_PAGE_NUMBER:
      g_value_set_uint(value, priv->document.current_page_number);
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
  priv->settings.continuous_pages = enable;

  zathura_gtk_clear_grid(priv);
  zathura_gtk_fill_grid(priv);

  if (priv->settings.continuous_pages == true) {
    restore_current_page(priv);
  }
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

static void
set_current_page_number(ZathuraDocumentPrivate* priv, guint page_number)
{
  if (page_number >= priv->document.number_of_pages) {
    return;
  }

  priv->document.current_page_number = page_number;
  zathura_gtk_grid_set_page(priv, page_number);
}

static void
restore_current_page(ZathuraDocumentPrivate* priv)
{
  GtkWidget* page_widget = g_list_nth_data(priv->document.pages, priv->document.current_page_number);

  zathura_page_info_t* page_info = g_malloc0(sizeof(zathura_page_info_t));

  /* Calculate offset */
  int page_widget_offset_x, page_widget_offset_y;
  if (gtk_widget_translate_coordinates(page_widget, priv->gtk.viewport, 0,
      0, &page_widget_offset_x, &page_widget_offset_y) == FALSE) {
    return; // Should not happen!
  };

  page_info->priv = priv;
  page_info->page_number = priv->document.current_page_number;
  page_info->offset.x = page_widget_offset_x,
  page_info->offset.y = page_widget_offset_y;

  priv->status.restore_position = page_info;
}
