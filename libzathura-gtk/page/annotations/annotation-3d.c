 /* See LICENSE file for license and copyright information */

#include "annotation-3d.h"
#include "../../macros.h"

struct _ZathuraAnnotation3DPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static void cb_zathura_gtk_annotation_3d_draw(GtkDrawingArea* area, cairo_t* cairo, int allocated_width, int allocated_height, gpointer data);

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
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(priv->drawing_area), cb_zathura_gtk_annotation_3d_draw, widget, NULL);
  gtk_widget_set_hexpand(priv->drawing_area, TRUE);
  gtk_widget_set_vexpand(priv->drawing_area, TRUE);
  gtk_widget_set_visible(priv->drawing_area, TRUE);

  gtk_box_append(GTK_BOX(widget), GTK_WIDGET(priv->drawing_area));
  gtk_widget_set_visible(GTK_WIDGET(widget), TRUE);
}

GtkWidget*
zathura_gtk_annotation_3d_new(zathura_annotation_t* annotation)
{
  return g_object_new(ZATHURA_TYPE_ANNOTATION_3D, "annotation", annotation, NULL);
}

static void
cb_zathura_gtk_annotation_3d_draw(GtkDrawingArea* area, cairo_t* cairo, int allocated_width, int allocated_height, gpointer data)
{
  GtkWidget* widget = GTK_WIDGET(area);
  (void) allocated_width;
  (void) allocated_height;
  /* ZathuraAnnotation3DPrivate* priv = zathura_gtk_annotation_3d_get_instance_private(data); */

  // TODO: Implement this widget

  return;
}
