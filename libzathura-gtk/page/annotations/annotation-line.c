 /* See LICENSE file for license and copyright information */

#include "annotation-line.h"
#include "../../macros.h"

struct _ZathuraAnnotationLinePrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_line_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationLine, zathura_gtk_annotation_line, ZATHURA_TYPE_ANNOTATION)

#define ZATHURA_ANNOTATION_LINE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_LINE, \
                               ZathuraAnnotationLinePrivate))

static void
zathura_gtk_annotation_line_class_init(ZathuraAnnotationLineClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_line_init(ZathuraAnnotationLine* widget)
{
  ZathuraAnnotationLinePrivate* priv = ZATHURA_ANNOTATION_LINE_GET_PRIVATE(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_line_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_LINE, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationLinePrivate* priv = ZATHURA_ANNOTATION_LINE_GET_PRIVATE(widget);

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_line_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_line_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationLinePrivate* priv = ZATHURA_ANNOTATION_LINE_GET_PRIVATE(data);

  double scale = 1.0;
  g_object_get(G_OBJECT(data), "scale", &scale, NULL);

  const unsigned int page_height = gtk_widget_get_allocated_height(widget);
  const unsigned int page_width  = gtk_widget_get_allocated_width(widget);

  /* Set opacity */
  double opacity = 1.0;
  if (zathura_annotation_markup_get_opacity(priv->annotation, &opacity) != ZATHURA_ERROR_OK) {
  }

  /* Set color */
  zathura_annotation_color_t color;
  if (zathura_annotation_get_color(priv->annotation, &color) == ZATHURA_ERROR_OK) {
    zathura_gtk_annotation_set_cairo_color(cairo, color, opacity);
  } else {
    cairo_set_source_rgba(cairo, 0, 0, 0, opacity);
  }

  cairo_save(cairo);
  cairo_set_line_width(cairo, 1*scale);
  cairo_move_to(cairo, 0, page_height);
  cairo_line_to(cairo, page_width, 0);
  cairo_stroke(cairo);

  cairo_restore(cairo);

  return TRUE;
}
