 /* See LICENSE file for license and copyright information */

#include "annotation-highlight.h"
#include "../../macros.h"

struct _ZathuraAnnotationHighlightPrivate {
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_highlight_draw(GtkWidget* widget, cairo_t *cairo);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationHighlight, zathura_gtk_annotation_highlight, GTK_TYPE_DRAWING_AREA)

#define ZATHURA_ANNOTATION_HIGHLIGHT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_HIGHLIGHT, \
                               ZathuraAnnotationHighlightPrivate))

static void
zathura_gtk_annotation_highlight_class_init(ZathuraAnnotationHighlightClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

  widget_class->draw = cb_zathura_gtk_annotation_highlight_draw;
}

static void
zathura_gtk_annotation_highlight_init(ZathuraAnnotationHighlight* widget)
{
  ZathuraAnnotationHighlightPrivate* priv = ZATHURA_ANNOTATION_HIGHLIGHT_GET_PRIVATE(widget);

  priv->annotation = NULL;
}

GtkWidget*
zathura_gtk_annotation_highlight_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_HIGHLIGHT, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationHighlightPrivate* priv = ZATHURA_ANNOTATION_HIGHLIGHT_GET_PRIVATE(widget);

  priv->annotation = annotation;

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_highlight_draw(GtkWidget* widget, cairo_t *cairo)
{
  ZathuraAnnotationHighlightPrivate* priv = ZATHURA_ANNOTATION_HIGHLIGHT_GET_PRIVATE(widget);

  cairo_save(cairo);

  cairo_set_operator(cairo, CAIRO_OPERATOR_MULTIPLY);

  zathura_list_t* quad_points;
  zathura_annotation_highlight_get_quad_points(priv->annotation, &quad_points);

  zathura_quad_point_t* quad_point;
  ZATHURA_LIST_FOREACH(quad_point, quad_points) {
    cairo_new_path(cairo);
    cairo_move_to(cairo, quad_point->p1.x, quad_point->p1.y);
    cairo_line_to(cairo, quad_point->p3.x, quad_point->p3.y);
    cairo_line_to(cairo, quad_point->p4.x, quad_point->p4.y);
    cairo_line_to(cairo, quad_point->p2.x, quad_point->p2.y);
    cairo_close_path(cairo);
  }

  double opacity = 0.5;
  if (zathura_annotation_markup_get_opacity(priv->annotation, &opacity) != ZATHURA_ERROR_OK) {
  }
  opacity = 1.5;

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

  cairo_fill(cairo);
  cairo_restore(cairo);

  return TRUE;
}
