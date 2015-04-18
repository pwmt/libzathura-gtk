/* See LICENSE file for license and copyright information */

#include <math.h>
#include <libzathura/libzathura.h>

#include "overlay.h"
#include "../internal.h"
#include "../utils.h"
#include "../../macros.h"

#include "annotation-highlight.h"
#include "annotation-line.h"
#include "annotation-squiggly.h"
#include "annotation-strike-out.h"
#include "annotation-underline.h"

struct _ZathuraAnnotationOverlayPrivate {
  ZathuraPage* page;
  zathura_list_t* annotations;

  struct {
    GtkWidget* annotations;
  } gtk;

  struct {
    bool show;
  } settings;
};

enum {
  PROP_0,
  PROP_ANNOTATIONS_SHOW,
};

typedef struct annotation_widget_mapping_s {
  GtkWidget* widget;
  zathura_annotation_t* annotation;
  zathura_rectangle_t position;
} annotation_widget_mapping_t;

static void zathura_gtk_annotation_overlay_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_annotation_overlay_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void zathura_gtk_annotation_overlay_size_allocate(GtkWidget* widget, GdkRectangle* allocation);
static void create_widgets(GtkWidget* overlay);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationOverlay, zathura_gtk_annotation_overlay, GTK_TYPE_BIN)

#define ZATHURA_ANNOTATION_OVERLAY_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_OVERLAY, \
                               ZathuraAnnotationOverlayPrivate))

static void
zathura_gtk_annotation_overlay_class_init(ZathuraAnnotationOverlayClass* class)
{
  /* overwrite methods */
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
  widget_class->size_allocate = zathura_gtk_annotation_overlay_size_allocate;

  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_annotation_overlay_set_property;
  object_class->get_property = zathura_gtk_annotation_overlay_get_property;

  /* properties */
  g_object_class_install_property(
    object_class,
    PROP_ANNOTATIONS_SHOW,
    g_param_spec_boolean(
      "show-form-fields",
      "show-form-fields",
      "show form-fields by drawing rectangles around them",
      FALSE,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );
}

static void
zathura_gtk_annotation_overlay_init(ZathuraAnnotationOverlay* widget)
{
  ZathuraAnnotationOverlayPrivate* priv = ZATHURA_ANNOTATION_OVERLAY_GET_PRIVATE(widget);

  priv->gtk.annotations = NULL;
  priv->page            = NULL;
  priv->settings.show   = false;
}

GtkWidget*
zathura_gtk_annotation_overlay_new(ZathuraPage* page)
{
  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_OVERLAY, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationOverlayPrivate* priv = ZATHURA_ANNOTATION_OVERLAY_GET_PRIVATE(widget);

  priv->page = page;

  /* Fixed container for form field widgets */
  priv->gtk.annotations = gtk_fixed_new();

  /* Setup container */
  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->gtk.annotations));
  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static void
zathura_gtk_annotation_overlay_size_allocate(GtkWidget* widget, GdkRectangle* allocation)
{
  ZathuraAnnotationOverlayPrivate* priv = ZATHURA_ANNOTATION_OVERLAY_GET_PRIVATE(widget);

  if (priv->annotations == NULL) {
    create_widgets(widget);
  }

  annotation_widget_mapping_t* annotation_mapping;
  ZATHURA_LIST_FOREACH(annotation_mapping, priv->annotations) {
      zathura_rectangle_t position = calculate_correct_position(priv->page, annotation_mapping->position);
      const unsigned int width  = ceil(position.p2.x) - floor(position.p1.x);
      const unsigned int height = ceil(position.p2.y) - floor(position.p1.y);

      gtk_fixed_move(GTK_FIXED(priv->gtk.annotations), annotation_mapping->widget, position.p1.x, position.p1.y);
      gtk_widget_set_size_request(annotation_mapping->widget, width, height);
  }

  GTK_WIDGET_CLASS(zathura_gtk_annotation_overlay_parent_class)->size_allocate(widget, allocation);
}

static void
zathura_gtk_annotation_overlay_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraAnnotationOverlay* editor      = ZATHURA_ANNOTATION_OVERLAY(object);
  ZathuraAnnotationOverlayPrivate* priv = ZATHURA_ANNOTATION_OVERLAY_GET_PRIVATE(editor);

  switch (prop_id) {
    case PROP_ANNOTATIONS_SHOW:
      {
        priv->settings.show = g_value_get_boolean(value);
        gtk_widget_queue_resize(GTK_WIDGET(editor));
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
zathura_gtk_annotation_overlay_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec)
{
  ZathuraAnnotationOverlay* editor      = ZATHURA_ANNOTATION_OVERLAY(object);
  ZathuraAnnotationOverlayPrivate* priv = ZATHURA_ANNOTATION_OVERLAY_GET_PRIVATE(editor);

  switch (prop_id) {
    case PROP_ANNOTATIONS_SHOW:
      g_value_set_boolean(value, priv->settings.show);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
create_widgets(GtkWidget* overlay)
{
  ZathuraAnnotationOverlayPrivate* priv = ZATHURA_ANNOTATION_OVERLAY_GET_PRIVATE(overlay);
  ZathuraPagePrivate* page_priv         = ZATHURA_PAGE_GET_PRIVATE(priv->page);

  zathura_list_t* annotations;
  if (zathura_page_get_annotations(page_priv->page, &annotations) != ZATHURA_ERROR_OK) {
    return;
  }

  zathura_annotation_t* annotation;
  ZATHURA_LIST_FOREACH(annotation, annotations) {
    zathura_annotation_type_t annotation_type = ZATHURA_ANNOTATION_UNKNOWN;
    if (zathura_annotation_get_type(annotation, &annotation_type) != ZATHURA_ERROR_OK) {
      continue;
    }

    zathura_rectangle_t position;
    if (zathura_annotation_get_position(annotation, &position) != ZATHURA_ERROR_OK) {
      continue;
    }

    GtkWidget* annotation_widget = NULL;
    switch (annotation_type) {
      case ZATHURA_ANNOTATION_LINE:
        annotation_widget = zathura_gtk_annotation_line_new(annotation);
        break;
      case ZATHURA_ANNOTATION_HIGHLIGHT:
        annotation_widget = zathura_gtk_annotation_highlight_new(annotation);
        break;
      case ZATHURA_ANNOTATION_STRIKE_OUT:
        annotation_widget = zathura_gtk_annotation_strike_out_new(annotation);
        break;
      case ZATHURA_ANNOTATION_UNDERLINE:
        annotation_widget = zathura_gtk_annotation_underline_new(annotation);
        break;
      case ZATHURA_ANNOTATION_SQUIGGLY:
        annotation_widget = zathura_gtk_annotation_squiggly_new(annotation);
        break;
      default:
        continue;
    }

    if (annotation_widget != NULL) {
      /* Create new mapping */
      annotation_widget_mapping_t* mapping = g_malloc0(sizeof(annotation_widget_mapping_t));

      mapping->position   = position;
      mapping->widget     = annotation_widget;
      mapping->annotation = annotation;

      priv->annotations = zathura_list_append(priv->annotations, mapping);

      /* Setup initial position of widgets */
      position = calculate_correct_position(priv->page, position);
      const unsigned int width  = ceil(position.p2.x) - floor(position.p1.x);
      const unsigned int height = ceil(position.p2.y) - floor(position.p1.y);

      gtk_fixed_put(GTK_FIXED(priv->gtk.annotations), annotation_widget, position.p1.x, position.p1.y);
      gtk_widget_set_size_request(annotation_widget, width, height);
      gtk_widget_show(annotation_widget);
    }
  }
}
