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

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_UNDERLINE, "annotation", annotation, NULL);
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

  zathura_list_t* quad_points = NULL;
  if (zathura_annotation_underline_get_quad_points(priv->annotation, &quad_points) != ZATHURA_ERROR_OK || quad_points == NULL) {
    return FALSE;
  }

  double scale;
  g_object_get(G_OBJECT(data), "scale", &scale, NULL);

  zathura_quad_point_t* quad_point;
  ZATHURA_LIST_FOREACH(quad_point, quad_points) {
    cairo_save(cairo);

    /* Draw underline rectangle */
    cairo_new_path(cairo);
    cairo_move_to(cairo, quad_point->p1.x * scale, quad_point->p1.y * scale);
    cairo_line_to(cairo, quad_point->p2.x * scale, quad_point->p2.y * scale);
    cairo_line_to(cairo, quad_point->p4.x * scale, quad_point->p4.y * scale);
    cairo_line_to(cairo, quad_point->p3.x * scale, quad_point->p3.y * scale);
    cairo_close_path(cairo);

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

    cairo_fill(cairo);
    cairo_restore(cairo);
  }

  return TRUE;
}
