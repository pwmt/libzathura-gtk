 /* See LICENSE file for license and copyright information */

#include "annotation-line.h"
#include "../../macros.h"

struct _ZathuraAnnotationLinePrivate {
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_line_draw(GtkWidget* widget, cairo_t *cairo);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationLine, zathura_gtk_annotation_line, GTK_TYPE_DRAWING_AREA)

#define ZATHURA_ANNOTATION_LINE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_LINE, \
                               ZathuraAnnotationLinePrivate))

static void
zathura_gtk_annotation_line_class_init(ZathuraAnnotationLineClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

  widget_class->draw = cb_zathura_gtk_annotation_line_draw;
}

static void
zathura_gtk_annotation_line_init(ZathuraAnnotationLine* widget)
{
  ZathuraAnnotationLinePrivate* priv = ZATHURA_ANNOTATION_LINE_GET_PRIVATE(widget);

  priv->annotation = NULL;
}

GtkWidget*
zathura_gtk_annotation_line_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_LINE, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationLinePrivate* priv = ZATHURA_ANNOTATION_LINE_GET_PRIVATE(widget);

  priv->annotation = annotation;

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_line_draw(GtkWidget* widget, cairo_t *cairo)
{
  const unsigned int page_height = gtk_widget_get_allocated_height(widget);
  const unsigned int page_width  = gtk_widget_get_allocated_width(widget);

  cairo_save(cairo);
  cairo_set_line_width(cairo, 1);
  cairo_move_to(cairo, 0, page_height);
  cairo_line_to(cairo, page_width, 0);
  cairo_set_source_rgb(cairo, 0.5, 0.3, 0);
  cairo_stroke(cairo);

  cairo_restore(cairo);

  return TRUE;
}
