 /* See LICENSE file for license and copyright information */

#include "annotation-stamp.h"
#include "../../macros.h"

struct _ZathuraAnnotationStampPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static void cb_zathura_gtk_annotation_stamp_draw(GtkDrawingArea* area, cairo_t* cairo, int allocated_width, int allocated_height, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationStamp, zathura_gtk_annotation_stamp, ZATHURA_TYPE_ANNOTATION)

static void
zathura_gtk_annotation_stamp_class_init(ZathuraAnnotationStampClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_stamp_init(ZathuraAnnotationStamp* widget)
{
  ZathuraAnnotationStampPrivate* priv = zathura_gtk_annotation_stamp_get_instance_private(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_stamp_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_STAMP, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationStampPrivate* priv = zathura_gtk_annotation_stamp_get_instance_private(ZATHURA_ANNOTATION_STAMP(widget));

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(priv->drawing_area), cb_zathura_gtk_annotation_stamp_draw, widget, NULL);
  gtk_widget_set_hexpand(priv->drawing_area, TRUE);
  gtk_widget_set_vexpand(priv->drawing_area, TRUE);
  gtk_widget_set_visible(priv->drawing_area, TRUE);

  gtk_box_append(GTK_BOX(widget), GTK_WIDGET(priv->drawing_area));
  gtk_widget_set_visible(GTK_WIDGET(widget), TRUE);

  return GTK_WIDGET(widget);
}

static void
cb_zathura_gtk_annotation_stamp_draw(GtkDrawingArea* area, cairo_t* cairo, int allocated_width, int allocated_height, gpointer data)
{
  GtkWidget* widget = GTK_WIDGET(area);
  (void) allocated_width;
  (void) allocated_height;
  /* ZathuraAnnotationStampPrivate* priv = zathura_gtk_annotation_stamp_get_instance_private(data); */

  // TODO: Implement this widget

  return;
}
