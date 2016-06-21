 /* See LICENSE file for license and copyright information */

#include "annotation-polyline.h"
#include "../../macros.h"

struct _ZathuraAnnotationPolyLinePrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_poly_line_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationPolyLine, zathura_gtk_annotation_poly_line, ZATHURA_TYPE_ANNOTATION)

#define ZATHURA_ANNOTATION_POLY_LINE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_POLY_LINE, \
                               ZathuraAnnotationPolyLinePrivate))

static void
zathura_gtk_annotation_poly_line_class_init(ZathuraAnnotationPolyLineClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_poly_line_init(ZathuraAnnotationPolyLine* widget)
{
  ZathuraAnnotationPolyLinePrivate* priv = ZATHURA_ANNOTATION_POLY_LINE_GET_PRIVATE(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_poly_line_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_POLY_LINE, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationPolyLinePrivate* priv = ZATHURA_ANNOTATION_POLY_LINE_GET_PRIVATE(widget);

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_poly_line_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));
  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_poly_line_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationPolyLinePrivate* priv = ZATHURA_ANNOTATION_POLY_LINE_GET_PRIVATE(data);

  // TODO: Implement this widget

  return GDK_EVENT_PROPAGATE;
}
