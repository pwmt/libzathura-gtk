 /* See LICENSE file for license and copyright information */

#include "annotation.h"
#include "../../macros.h"
#include "../../utils.h"

static void zathura_gtk_annotation_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_annotation_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static gboolean zathura_gtk_annotation_draw(GtkWidget* widget, cairo_t* cairo);

enum {
  PROP_0,
  PROP_SCALE,
  PROP_ANNOTATION
};

struct _ZathuraAnnotationPrivate {
  zathura_annotation_t* annotation;

  struct {
    double scale;
  } settings;
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotation, zathura_gtk_annotation, GTK_TYPE_BIN)

#define ZATHURA_ANNOTATION_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION, \
                               ZathuraAnnotationPrivate))

static void
zathura_gtk_annotation_class_init(ZathuraAnnotationClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_annotation_set_property;
  object_class->get_property = zathura_gtk_annotation_get_property;

  /* widget class */
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
  widget_class->draw = zathura_gtk_annotation_draw;

  /* properties */
  g_object_class_install_property(
    object_class,
    PROP_ANNOTATION,
    g_param_spec_pointer(
      "annotation",
      "Annotation",
      "The libzathura annotation",
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
zathura_gtk_annotation_init(ZathuraAnnotation* widget)
{
  ZathuraAnnotationPrivate* priv = ZATHURA_ANNOTATION_GET_PRIVATE(widget);

  priv->settings.scale = 1.0 * gtk_widget_get_scale_factor(GTK_WIDGET(widget));
}

static void zathura_gtk_annotation_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraAnnotation* page        = ZATHURA_ANNOTATION(object);
  ZathuraAnnotationPrivate* priv = ZATHURA_ANNOTATION_GET_PRIVATE(page);

  switch (prop_id) {
    case PROP_ANNOTATION:
      priv->annotation = g_value_get_pointer(value);
      break;
    case PROP_SCALE:
      {
        double scale = g_value_get_double(value);
        gint scale_factor = gtk_widget_get_scale_factor(GTK_WIDGET(object));
        double new_scale = scale * scale_factor;
        if (priv->settings.scale != new_scale) {
          priv->settings.scale = new_scale;
        }
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void zathura_gtk_annotation_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec)
{
  ZathuraAnnotation* page        = ZATHURA_ANNOTATION(object);
  ZathuraAnnotationPrivate* priv = ZATHURA_ANNOTATION_GET_PRIVATE(page);

  switch (prop_id) {
    case PROP_ANNOTATION:
      g_value_set_pointer(value, priv->annotation);
      break;
    case PROP_SCALE:
      g_value_set_double(value, priv->settings.scale);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static gboolean
zathura_gtk_annotation_draw(GtkWidget* widget, cairo_t* cairo)
{
  ZathuraAnnotationPrivate* priv = ZATHURA_ANNOTATION_GET_PRIVATE(widget);

  if (priv->annotation == NULL) {
    goto propagate_event;
  }

  bool has_appearance_stream = false;
  if (zathura_annotation_has_appearance_stream(priv->annotation,
        &has_appearance_stream) != ZATHURA_ERROR_OK || has_appearance_stream ==
      false) {
    goto propagate_event;
  }

  /* Try to render appearance stream */
  const unsigned int width  = gtk_widget_get_allocated_width(widget);
  const unsigned int height = gtk_widget_get_allocated_height(widget);

  /* Create image surface */
  cairo_surface_t* image_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  if (image_surface == NULL) {
    goto propagate_event;
  }

  cairo_t* image_cairo = cairo_create(image_surface);
  if (image_cairo == NULL) {
    cairo_surface_destroy(image_surface);
    goto propagate_event;
  }

  /* Scale */
  cairo_scale(image_cairo, priv->settings.scale, priv->settings.scale);

  /* Render page */
  if (zathura_annotation_render_cairo(priv->annotation, image_cairo, priv->settings.scale) != ZATHURA_ERROR_OK) {
    cairo_destroy(image_cairo);
    cairo_surface_destroy(image_surface);
    goto propagate_event;
  }

  cairo_destroy(image_cairo);

  /* Save cairo */
  cairo_save(cairo);

  /* Set blend mode */
  zathura_blend_mode_t blend_mode = ZATHURA_BLEND_MODE_NORMAL;
  if (zathura_annotation_get_blend_mode(priv->annotation, &blend_mode) != ZATHURA_ERROR_OK) {
  }

  /* Get opacity */
  float opacity = 1.0;
  if (zathura_annotation_get_opacity(priv->annotation, &opacity) != ZATHURA_ERROR_OK) {
  }

  cairo_operator_t cairo_operator = zathura_blend_mode_to_cairo_operator(blend_mode);
  cairo_set_operator(cairo, cairo_operator);

  /* Paint */
  cairo_set_source_surface(cairo, image_surface, 0, 0);
  cairo_paint_with_alpha(cairo, opacity);
  cairo_restore(cairo);

  /* Clean-up */
  cairo_surface_destroy(image_surface);

  return GDK_EVENT_STOP;

propagate_event:

  gtk_container_propagate_draw(GTK_CONTAINER(widget), gtk_bin_get_child(GTK_BIN(widget)), cairo);

  return GDK_EVENT_PROPAGATE;
}
