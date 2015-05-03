 /* See LICENSE file for license and copyright information */

#include "annotation-squiggly.h"
#include "../../macros.h"

struct _ZathuraAnnotationSquigglyPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_squiggly_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationSquiggly, zathura_gtk_annotation_squiggly, ZATHURA_TYPE_ANNOTATION)

#define ZATHURA_ANNOTATION_SQUIGGLY_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_SQUIGGLY, \
                               ZathuraAnnotationSquigglyPrivate))

static void
zathura_gtk_annotation_squiggly_class_init(ZathuraAnnotationSquigglyClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_squiggly_init(ZathuraAnnotationSquiggly* widget)
{
  ZathuraAnnotationSquigglyPrivate* priv = ZATHURA_ANNOTATION_SQUIGGLY_GET_PRIVATE(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_squiggly_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_SQUIGGLY, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationSquigglyPrivate* priv = ZATHURA_ANNOTATION_SQUIGGLY_GET_PRIVATE(widget);

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_squiggly_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_squiggly_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationSquigglyPrivate* priv = ZATHURA_ANNOTATION_SQUIGGLY_GET_PRIVATE(data);

  const unsigned int height = gtk_widget_get_allocated_height(widget);
  const unsigned int width  = gtk_widget_get_allocated_width(widget);

  double square_height = 2.5;
  double square        = height / square_height;
  double unit_width    = (square_height - 1) * square;

  int width_units = (width + unit_width / 2) / unit_width;

  double x        = (width - width_units * unit_width) / 2;
  double y_top    = 0;
  double y_bottom = height;

  double x_middle = x + unit_width;
  double x_right  = x + 2*unit_width;

  cairo_save(cairo);
  cairo_move_to(cairo, x - square/2, y_top + square/2);

  int i = 0;
  for (i = 0; i < width_units - 2; i+= 2) {
    cairo_line_to(cairo, x_middle, y_bottom);
    cairo_line_to(cairo, x_right, y_top + square);

    x_middle += 2*unit_width;
    x_right  += 2*unit_width;
  }

  cairo_line_to(cairo, x_middle, y_bottom);

  if (i + 1 == width_units) {
    cairo_line_to(cairo, x_middle + square/2, y_bottom - square / 2);
  } else {
    cairo_line_to(cairo, x_right + square/2, y_top + square / 2);
    cairo_line_to(cairo, x_right, y_top);
  }

  double x_left = x_middle - unit_width;

  for (i = i; i >= 0; i -= 2) {
    cairo_line_to(cairo, x_middle, y_bottom - square);
    cairo_line_to(cairo, x_left, y_top);

    x_left   -= 2*unit_width;
    x_middle -= 2*unit_width;
  }

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
