 /* See LICENSE file for license and copyright information */

#include "annotation-3d.h"
#include "../../macros.h"

struct _ZathuraAnnotation3DPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_3d_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotation3D, zathura_gtk_annotation_3d, ZATHURA_TYPE_ANNOTATION)

static void
zathura_gtk_annotation_3d_class_init(ZathuraAnnotation3DClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_3d_init(ZathuraAnnotation3D* widget)
{
  ZathuraAnnotation3DPrivate* priv = zathura_gtk_annotation_3d_get_instance_private(widget);

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_3d_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));
  gtk_widget_show_all(GTK_WIDGET(widget));
}

GtkWidget*
zathura_gtk_annotation_3d_new(zathura_annotation_t* annotation)
{
  return g_object_new(ZATHURA_TYPE_ANNOTATION_3D, "annotation", annotation, NULL);
}

static gboolean
cb_zathura_gtk_annotation_3d_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  /* ZathuraAnnotation3DPrivate* priv = zathura_gtk_annotation_3d_get_instance_private(data); */

  // TODO: Implement this widget

  return GDK_EVENT_PROPAGATE;
}
