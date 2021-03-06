 /* See LICENSE file for license and copyright information */

#include "annotation-highlight.h"
#include "../../macros.h"

struct _ZathuraAnnotationHighlightPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_highlight_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationHighlight, zathura_gtk_annotation_highlight, ZATHURA_TYPE_ANNOTATION)

static void
zathura_gtk_annotation_highlight_class_init(ZathuraAnnotationHighlightClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_highlight_init(ZathuraAnnotationHighlight* widget)
{
  ZathuraAnnotationHighlightPrivate* priv = zathura_gtk_annotation_highlight_get_instance_private(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_highlight_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_HIGHLIGHT, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationHighlightPrivate* priv = zathura_gtk_annotation_highlight_get_instance_private(ZATHURA_ANNOTATION_HIGHLIGHT(widget));

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_highlight_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_highlight_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationHighlightPrivate* priv = zathura_gtk_annotation_highlight_get_instance_private(data);

  zathura_list_t* quad_points;
  if (zathura_annotation_highlight_get_quad_points(priv->annotation, &quad_points) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  double scale;
  g_object_get(G_OBJECT(data), "scale", &scale, NULL);

  cairo_save(cairo);
  cairo_set_operator(cairo, CAIRO_OPERATOR_MULTIPLY);

  zathura_quad_point_t* quad_point;
  ZATHURA_LIST_FOREACH(quad_point, quad_points) {
    cairo_new_path(cairo);
    cairo_move_to(cairo, quad_point->p1.x * scale, quad_point->p1.y * scale);
    cairo_line_to(cairo, quad_point->p2.x * scale, quad_point->p2.y * scale);
    cairo_line_to(cairo, quad_point->p4.x * scale, quad_point->p4.y * scale);
    cairo_line_to(cairo, quad_point->p3.x * scale, quad_point->p3.y * scale);
    cairo_close_path(cairo);
  }

  double opacity = 1.0;
  if (zathura_annotation_markup_get_opacity(priv->annotation, &opacity) != ZATHURA_ERROR_OK) {
  }

  zathura_annotation_color_t color;
  if (zathura_annotation_get_color(priv->annotation, &color) == ZATHURA_ERROR_OK) {
    zathura_gtk_annotation_set_cairo_color(cairo, color, opacity);
  } else {
    cairo_set_source_rgba(cairo, 0, 0, 0, opacity);
  }

  cairo_fill(cairo);
  cairo_restore(cairo);

  return GDK_EVENT_STOP;
}
