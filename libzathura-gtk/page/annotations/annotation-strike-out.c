 /* See LICENSE file for license and copyright information */

#include "annotation-strike-out.h"
#include "../../macros.h"

struct _ZathuraAnnotationStrikeOutPrivate {
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_strike_out_draw(GtkWidget* widget, cairo_t *cairo);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationStrikeOut, zathura_gtk_annotation_strike_out, GTK_TYPE_DRAWING_AREA)

#define ZATHURA_ANNOTATION_STRIKE_OUT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_STRIKE_OUT, \
                               ZathuraAnnotationStrikeOutPrivate))

static void
zathura_gtk_annotation_strike_out_class_init(ZathuraAnnotationStrikeOutClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

  widget_class->draw = cb_zathura_gtk_annotation_strike_out_draw;
}

static void
zathura_gtk_annotation_strike_out_init(ZathuraAnnotationStrikeOut* widget)
{
  ZathuraAnnotationStrikeOutPrivate* priv = ZATHURA_ANNOTATION_STRIKE_OUT_GET_PRIVATE(widget);

  priv->annotation = NULL;
}

GtkWidget*
zathura_gtk_annotation_strike_out_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_STRIKE_OUT, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationStrikeOutPrivate* priv = ZATHURA_ANNOTATION_STRIKE_OUT_GET_PRIVATE(widget);

  priv->annotation = annotation;

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_strike_out_draw(GtkWidget* widget, cairo_t *cairo)
{
  ZathuraAnnotationStrikeOutPrivate* priv = ZATHURA_ANNOTATION_STRIKE_OUT_GET_PRIVATE(widget);

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
