 /* See LICENSE file for license and copyright information */

#include <math.h>

#include "page.h"

static void zathura_gtk_page_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_page_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void render_page(ZathuraPagePrivate* priv);

struct _ZathuraPagePrivate {
  zathura_page_t* page;
  GtkWidget* overlay;
  GtkWidget* drawing_area;

  struct {
    unsigned int width;
    unsigned int height;
  } dimensions;

  struct {
    guint rotation;
    double scale;
  } settings;
};

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

  priv->page         = NULL;
  priv->drawing_area = NULL;
  priv->overlay      = NULL;

  priv->dimensions.width  = 0;
  priv->dimensions.height = 0;

  priv->settings.rotation = 0;
  priv->settings.scale    = 1.0;
}

gboolean
cb_draw(GtkWidget *widget, cairo_t *cairo, gpointer data)
{
  ZathuraPagePrivate* priv = (ZathuraPagePrivate*) data;

  const unsigned int page_width  = gtk_widget_get_allocated_width(widget);
  const unsigned int page_height = gtk_widget_get_allocated_height(widget);

  cairo_save(cairo);

  /* Rotate */
  switch (priv->settings.rotation) {
    case 90:
      cairo_translate(cairo, page_width, 0);
      break;
    case 180:
      cairo_translate(cairo, page_width, page_height);
      break;
    case 270:
      cairo_translate(cairo, 0, page_height);
      break;
  }

  if (priv->settings.rotation != 0) {
    cairo_rotate(cairo, priv->settings.rotation * G_PI / 180.0);
  }

  /* Scale */
  cairo_scale(cairo, priv->settings.scale, priv->settings.scale);

  cairo_set_source_rgb(cairo, 255, 255, 255);
  cairo_rectangle(cairo, 0, 0, page_width, page_height);
  cairo_fill(cairo);

  if (zathura_page_render_cairo(priv->page, cairo, priv->settings.scale, 0, 0) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  cairo_set_operator(cairo, CAIRO_OPERATOR_DEST_OVER);
  cairo_paint(cairo);
  cairo_restore(cairo);

  return FALSE;
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

  /* Setup over lay */
  priv->overlay = gtk_overlay_new();

  /* Setup drawing area */
  priv->drawing_area = gtk_drawing_area_new();
  gtk_widget_set_halign(priv->drawing_area, GTK_ALIGN_START);
  gtk_widget_set_valign(priv->drawing_area, GTK_ALIGN_START);
  gtk_widget_set_size_request(priv->drawing_area, priv->dimensions.width, priv->dimensions.height);
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_draw), priv);

  /* Setup container */
  gtk_container_add(GTK_CONTAINER(priv->overlay), GTK_WIDGET(priv->drawing_area));
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

  gtk_widget_set_size_request(priv->drawing_area, page_widget_width, page_widget_height);
  gtk_widget_queue_resize(priv->drawing_area);
}
