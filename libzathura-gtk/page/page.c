 /* See LICENSE file for license and copyright information */

#include <math.h>

#include "page.h"
#include "internal.h"
#include "callbacks.h"

static void zathura_gtk_page_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_page_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void render_page(ZathuraPagePrivate* priv);

enum {
  PROP_0,
  PROP_PAGE,
  PROP_ROTATION,
  PROP_SCALE,
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraPage, zathura_gtk_page, GTK_TYPE_BIN)

#define ZATHURA_PAGE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_PAGE, \
                               ZathuraPagePrivate))

static void
zathura_gtk_page_class_init(ZathuraPageClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_page_set_property;
  object_class->get_property = zathura_gtk_page_get_property;

  /* properties */
  g_object_class_install_property(
    object_class,
    PROP_PAGE,
    g_param_spec_pointer(
      "page",
      "Page",
      "The zathura_page_t instance",
      G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS
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
zathura_gtk_page_init(ZathuraPage* widget)
{
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(widget);

  priv->page               = NULL;
  priv->overlay            = NULL;
  priv->layer.drawing_area = NULL;
  priv->layer.links        = NULL;

  priv->dimensions.width  = 0;
  priv->dimensions.height = 0;

  priv->settings.rotation = 0;
  priv->settings.scale    = 1.0;

  priv->properties.links = NULL;
}

GtkWidget*
zathura_gtk_page_new(zathura_page_t* page)
{
  if (page == NULL) {
    return NULL;
  }

  GObject* widget = g_object_new(ZATHURA_TYPE_PAGE, "page", page, NULL);
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(widget);

  if (zathura_page_get_width(page, &(priv->dimensions.width)) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  if (zathura_page_get_height(page, &(priv->dimensions.height)) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  /* Setup drawing area */
  priv->layer.drawing_area = gtk_drawing_area_new();
  gtk_widget_set_halign(priv->layer.drawing_area, GTK_ALIGN_START);
  gtk_widget_set_valign(priv->layer.drawing_area, GTK_ALIGN_START);
  gtk_widget_set_size_request(priv->layer.drawing_area, priv->dimensions.width, priv->dimensions.height);
  g_signal_connect(G_OBJECT(priv->layer.drawing_area), "draw", G_CALLBACK(cb_page_draw), priv);

  /* Setup links layer */
  priv->layer.links = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->layer.links), "draw", G_CALLBACK(cb_page_draw_links), priv);

  /* Setup over lay */
  priv->overlay = gtk_overlay_new();
  gtk_container_add(GTK_CONTAINER(priv->overlay), GTK_WIDGET(priv->layer.drawing_area));
  gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay), priv->layer.links);

  /* Setup container */
  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->overlay));

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static void
zathura_gtk_page_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraPage* page    = ZATHURA_PAGE(object);
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(page);

  switch (prop_id) {
    case PROP_PAGE:
      priv->page = g_value_get_pointer(value);
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
              render_page(priv);
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
          render_page(priv);
        }
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
zathura_gtk_page_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec)
{
  ZathuraPage* page        = ZATHURA_PAGE(object);
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(page);

  switch (prop_id) {
    case PROP_PAGE:
      g_value_set_pointer(value, priv->page);
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
calculate_widget_size(ZathuraPagePrivate* priv, unsigned int* widget_width,
    unsigned int* widget_height)
{
  if (priv->settings.rotation % 180) {
    *widget_width  = round(priv->dimensions.height * priv->settings.scale);
    *widget_height = round(priv->dimensions.width  * priv->settings.scale);
  } else {
    *widget_width  = round(priv->dimensions.width  * priv->settings.scale);
    *widget_height = round(priv->dimensions.height * priv->settings.scale);
  }
}

static void
render_page(ZathuraPagePrivate* priv)
{
  unsigned int page_widget_width;
  unsigned int page_widget_height;

  calculate_widget_size(priv, &page_widget_width, &page_widget_height);

  gtk_widget_set_size_request(priv->layer.drawing_area, page_widget_width, page_widget_height);
  gtk_widget_queue_resize(priv->layer.drawing_area);
}
