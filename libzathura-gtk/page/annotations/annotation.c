 /* See LICENSE file for license and copyright information */

#include "annotation.h"
#include "../../macros.h"

static void zathura_gtk_annotation_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_annotation_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);

enum {
  PROP_0,
  PROP_SCALE
};

struct _ZathuraAnnotationPrivate {
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

  /* properties */
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

  priv->settings.scale = 1.0;
}

static void zathura_gtk_annotation_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraAnnotation* page        = ZATHURA_ANNOTATION(object);
  ZathuraAnnotationPrivate* priv = ZATHURA_ANNOTATION_GET_PRIVATE(page);

  switch (prop_id) {
    case PROP_SCALE:
      {
        double scale = g_value_get_double(value);
        if (priv->settings.scale != scale) {
          priv->settings.scale = scale;
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
    case PROP_SCALE:
      g_value_set_double(value, priv->settings.scale);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}
