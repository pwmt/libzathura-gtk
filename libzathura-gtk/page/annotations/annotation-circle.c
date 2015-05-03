 /* See LICENSE file for license and copyright information */

#include "annotation-circle.h"
#include "../../macros.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

struct _ZathuraAnnotationCirclePrivate {
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_circle_draw(GtkWidget* widget, cairo_t *cairo);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationCircle, zathura_gtk_annotation_circle, GTK_TYPE_DRAWING_AREA)

#define ZATHURA_ANNOTATION_CIRCLE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_CIRCLE, \
                               ZathuraAnnotationCirclePrivate))

static void
zathura_gtk_annotation_circle_class_init(ZathuraAnnotationCircleClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

  widget_class->draw = cb_zathura_gtk_annotation_circle_draw;
}

static void
zathura_gtk_annotation_circle_init(ZathuraAnnotationCircle* widget)
{
  ZathuraAnnotationCirclePrivate* priv = ZATHURA_ANNOTATION_CIRCLE_GET_PRIVATE(widget);

  priv->annotation = NULL;
}

GtkWidget*
zathura_gtk_annotation_circle_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_CIRCLE, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationCirclePrivate* priv = ZATHURA_ANNOTATION_CIRCLE_GET_PRIVATE(widget);

  priv->annotation = annotation;

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_circle_draw(GtkWidget* widget, cairo_t *cairo)
{
  ZathuraAnnotationCirclePrivate* priv = ZATHURA_ANNOTATION_CIRCLE_GET_PRIVATE(widget);

  const unsigned int height = gtk_widget_get_allocated_height(widget);
  const unsigned int width  = gtk_widget_get_allocated_width(widget);

  /* Set opacity */
  double opacity = 0.5;
  if (zathura_annotation_markup_get_opacity(priv->annotation, &opacity) != ZATHURA_ERROR_OK) {
    opacity = 1.0;
  }

  /* Set color */
  zathura_annotation_color_t color;
  if (zathura_annotation_get_color(priv->annotation, &color) == ZATHURA_ERROR_OK) {
    cairo_set_source_rgba(cairo,
        color.values[0] / 65535,
        color.values[1] / 65535,
        color.values[2] / 65535,
        opacity);
  } else {
    cairo_set_source_rgb(cairo, 0, 0, 0);
  }

  /* Draw circle */
  cairo_save(cairo);

  /* Define blending-mode */
  cairo_set_operator(cairo, CAIRO_OPERATOR_MULTIPLY);

  double line_width = 1.333334;

  double circle_width_half  = (width - 2*line_width) / 2;
  double circle_height_half = (height - 2*line_width) / 2;

  cairo_translate(cairo, line_width + circle_width_half, line_width + circle_height_half);
  cairo_save(cairo);
  cairo_scale(cairo, circle_width_half, circle_height_half);
  cairo_arc(cairo, 0., 0., 1, 0., 2 * M_PI);
  cairo_restore(cairo);

  cairo_set_line_width (cairo, line_width);
  cairo_stroke(cairo);
  cairo_restore(cairo);

  return TRUE;
}
