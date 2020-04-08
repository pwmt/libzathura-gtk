 /* See LICENSE file for license and copyright information */

#include "annotation-square.h"
#include "../../macros.h"

struct _ZathuraAnnotationSquarePrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_square_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationSquare, zathura_gtk_annotation_square, ZATHURA_TYPE_ANNOTATION)

static void
zathura_gtk_annotation_square_class_init(ZathuraAnnotationSquareClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_square_init(ZathuraAnnotationSquare* widget)
{
  ZathuraAnnotationSquarePrivate* priv = zathura_gtk_annotation_square_get_instance_private(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_square_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_SQUARE, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationSquarePrivate* priv = zathura_gtk_annotation_square_get_instance_private(widget);

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_square_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));
  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_square_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationSquarePrivate* priv = zathura_gtk_annotation_square_get_instance_private(data);

  // TODO: Implement this widget

  return GDK_EVENT_PROPAGATE;
}
