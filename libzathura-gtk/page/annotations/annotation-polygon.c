 /* See LICENSE file for license and copyright information */

#include "annotation-polygon.h"
#include "../../macros.h"

struct _ZathuraAnnotationPolygonPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_polygon_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationPolygon, zathura_gtk_annotation_polygon, ZATHURA_TYPE_ANNOTATION)

static void
zathura_gtk_annotation_polygon_class_init(ZathuraAnnotationPolygonClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_polygon_init(ZathuraAnnotationPolygon* widget)
{
  ZathuraAnnotationPolygonPrivate* priv = zathura_gtk_annotation_polygon_get_instance_private(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_polygon_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_POLYGON, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationPolygonPrivate* priv = zathura_gtk_annotation_polygon_get_instance_private(ZATHURA_ANNOTATION_POLYGON(widget));

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_polygon_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));
  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_polygon_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationPolygonPrivate* priv = zathura_gtk_annotation_polygon_get_instance_private(data);

  double scale = 1.0;
  g_object_get(G_OBJECT(data), "scale", &scale, NULL);

  /* Draw area */
  const unsigned int height = gtk_widget_get_allocated_height(widget);
  const unsigned int width  = gtk_widget_get_allocated_width(widget);

  /* Get opacity */
  double opacity = 1.0;
  if (zathura_annotation_markup_get_opacity(priv->annotation, &opacity) != ZATHURA_ERROR_OK) {
  }

  /* Get position */
  zathura_rectangle_t position;
  if (zathura_annotation_get_position(priv->annotation, &position) != ZATHURA_ERROR_OK) {
    goto error_out;
  }

  // TODO: Implement this widget
  zathura_list_t* vertices;
  if (zathura_annotation_polygon_get_vertices(priv->annotation, &vertices) != ZATHURA_ERROR_OK) {
    goto error_out;
  }

  /* Get border */
  zathura_annotation_border_t border;
  if (zathura_annotation_border_init(&border) != ZATHURA_ERROR_OK) {
    goto error_out;
  }

  if (zathura_annotation_polygon_get_border(priv->annotation, &border) != ZATHURA_ERROR_OK) {
    goto error_out;
  }

  cairo_save(cairo);

  /* Get dash */
  if (border.dash_pattern.dash_array != NULL) {
    /* Copy dash array to cairo format */
    unsigned int length = zathura_list_length(border.dash_pattern.dash_array);
    double* dashes = calloc(length, sizeof(size_t));
    if (dashes != NULL) {
      for (unsigned int i = 0; i < length; i++) {
        dashes[i] = (size_t) zathura_list_nth_data(border.dash_pattern.dash_array, i);
      }

      cairo_set_dash(cairo, dashes, length, border.dash_pattern.dash_phase);

      free(dashes);
    }
  }

  /* Set color */
  zathura_annotation_color_t color;
  if (zathura_annotation_get_color(priv->annotation, &color) == ZATHURA_ERROR_OK) {
    zathura_gtk_annotation_set_cairo_color(cairo, color, opacity);
  } else {
    cairo_set_source_rgb(cairo, 0, 0, 0);
  }

  float annotation_height = position.p2.y - position.p1.y;

  int length = zathura_list_length(vertices);
  for (int i = 0; i < length; i++) {
    zathura_point_t* point = zathura_list_nth_data(vertices, i);
    float x = point->x - position.p1.x;
    float y = annotation_height - (position.p2.y - point->y);

    if (i == 0) {
      cairo_move_to(cairo, x, y);
    } else {
      cairo_line_to(cairo, x, y);
    }
  }

  cairo_set_source_rgb(cairo, 0, 0, 0);
  cairo_close_path(cairo);
  cairo_set_line_width(cairo, border.width * scale);
  cairo_stroke(cairo);
  cairo_restore(cairo);

  return GDK_EVENT_STOP;

error_out:

  return GDK_EVENT_PROPAGATE;
}
