 /* See LICENSE file for license and copyright information */

#include "annotation-underline.h"
#include "../../macros.h"

struct _ZathuraAnnotationUnderlinePrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_underline_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationUnderline, zathura_gtk_annotation_underline, ZATHURA_TYPE_ANNOTATION)

#define ZATHURA_ANNOTATION_UNDERLINE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_UNDERLINE, \
                               ZathuraAnnotationUnderlinePrivate))

static void
zathura_gtk_annotation_underline_class_init(ZathuraAnnotationUnderlineClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_underline_init(ZathuraAnnotationUnderline* widget)
{
  ZathuraAnnotationUnderlinePrivate* priv = ZATHURA_ANNOTATION_UNDERLINE_GET_PRIVATE(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_underline_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_UNDERLINE, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationUnderlinePrivate* priv = ZATHURA_ANNOTATION_UNDERLINE_GET_PRIVATE(widget);

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_underline_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_underline_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationUnderlinePrivate* priv = ZATHURA_ANNOTATION_UNDERLINE_GET_PRIVATE(data);

  const unsigned int page_height = gtk_widget_get_allocated_height(widget);
  const unsigned int page_width  = gtk_widget_get_allocated_width(widget);

  cairo_save(cairo);
  cairo_rectangle(cairo, 0, 0, page_width, page_height);

  zathura_annotation_color_t color;
  if (zathura_annotation_get_color(priv->annotation, &color) == ZATHURA_ERROR_OK) {
    cairo_set_source_rgb(cairo, 
        color.values[0] / 65535, 
        color.values[1] / 65535, 
        color.values[2] / 65535);
  } else {
    cairo_set_source_rgb(cairo, 0, 0, 0);
  }

  cairo_fill(cairo);
  cairo_restore(cairo);

  return TRUE;
}
