 /* See LICENSE file for license and copyright information */

#include "annotation.h"
#include "../../macros.h"
#include "../../utils.h"

static void zathura_gtk_annotation_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_annotation_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void zathura_gtk_annotation_snapshot(GtkWidget* widget, GtkSnapshot* snapshot);

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

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotation, zathura_gtk_annotation, GTK_TYPE_BOX)

static void
zathura_gtk_annotation_class_init(ZathuraAnnotationClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_annotation_set_property;
  object_class->get_property = zathura_gtk_annotation_get_property;

  /* widget class */
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
  widget_class->snapshot = zathura_gtk_annotation_snapshot;

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
  ZathuraAnnotationPrivate* priv = zathura_gtk_annotation_get_instance_private(widget);

  priv->settings.scale = 1.0 * gtk_widget_get_scale_factor(GTK_WIDGET(widget));
}

static void zathura_gtk_annotation_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraAnnotation* page        = ZATHURA_ANNOTATION(object);
  ZathuraAnnotationPrivate* priv = zathura_gtk_annotation_get_instance_private(page);

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
  ZathuraAnnotationPrivate* priv = zathura_gtk_annotation_get_instance_private(page);

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

static void
zathura_gtk_annotation_snapshot(GtkWidget* widget, GtkSnapshot* snapshot)
{
  ZathuraAnnotationPrivate* priv = zathura_gtk_annotation_get_instance_private(ZATHURA_ANNOTATION(widget));
  GtkWidget* child = gtk_widget_get_first_child(widget);
  const int width  = gtk_widget_get_width(widget);
  const int height = gtk_widget_get_height(widget);

  if (priv->annotation == NULL) {
    if (child != NULL) {
      gtk_widget_snapshot_child(widget, child, snapshot);
    }
    return;
  }

  bool has_appearance_stream = false;
  if ((zathura_annotation_has_appearance_stream(priv->annotation, &has_appearance_stream) != ZATHURA_ERROR_OK)) {
  /* } || has_appearance_stream == */
  /*     false) { */
    if (child != NULL) {
      gtk_widget_snapshot_child(widget, child, snapshot);
    }
    return;
  }

  if (has_appearance_stream == false) {
    if (child != NULL) {
      gtk_widget_snapshot_child(widget, child, snapshot);
    }
    return;
  }

  /* Try to render appearance stream */
  if (width <= 0 || height <= 0) {
    return;
  }

  graphene_rect_t bounds = GRAPHENE_RECT_INIT(0, 0, width, height);
  cairo_t* cairo = gtk_snapshot_append_cairo(snapshot, &bounds);
  if (cairo == NULL) {
    return;
  }

  /* Create image surface */
  cairo_surface_t* image_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  if (image_surface == NULL) {
    return;
  }

  cairo_t* image_cairo = cairo_create(image_surface);
  if (image_cairo == NULL) {
    cairo_surface_destroy(image_surface);
    return;
  }

  /* Scale */
  /// XXX: Define how to correctly render in respect to scale and widget size
  /* cairo_scale(image_cairo, priv->settings.scale, priv->settings.scale); */

  /* Render annotation */
  has_appearance_stream = false; // FIXME
  if (has_appearance_stream == true) {
    if (zathura_annotation_render_cairo(priv->annotation, image_cairo) !=
        ZATHURA_ERROR_OK) {
      cairo_destroy(image_cairo);
      cairo_surface_destroy(image_surface);
      return;
    }
  } else {
    if (child != NULL) {
      gtk_widget_snapshot_child(widget, child, snapshot);
    }
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
}
