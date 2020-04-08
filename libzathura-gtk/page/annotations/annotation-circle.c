 /* See LICENSE file for license and copyright information */

#include <math.h>

#include "annotation-circle.h"
#include "../../macros.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

struct _ZathuraAnnotationCirclePrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_circle_draw(GtkWidget* widget, cairo_t
    *cairo, gpointer data);
static void calculate_cloud_line(cairo_t* cairo, double x, double y, double
    x_previous, double y_previous, double width, bool switch_axes);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationCircle, zathura_gtk_annotation_circle, ZATHURA_TYPE_ANNOTATION)

static void
zathura_gtk_annotation_circle_class_init(ZathuraAnnotationCircleClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_circle_init(ZathuraAnnotationCircle* widget)
{
  ZathuraAnnotationCirclePrivate* priv = zathura_gtk_annotation_circle_get_instance_private(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_circle_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_CIRCLE, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationCirclePrivate* priv = zathura_gtk_annotation_circle_get_instance_private(ZATHURA_ANNOTATION_CIRCLE(widget));

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_circle_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_circle_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationCirclePrivate* priv = zathura_gtk_annotation_circle_get_instance_private(data);

  double scale = 1.0;
  g_object_get(G_OBJECT(data), "scale", &scale, NULL);

  const unsigned int height = gtk_widget_get_allocated_height(widget);
  const unsigned int width  = gtk_widget_get_allocated_width(widget);

  /* Get opacity */
  double opacity = 0.5;
  if (zathura_annotation_markup_get_opacity(priv->annotation, &opacity) != ZATHURA_ERROR_OK) {
    opacity = 1.0;
  }

  /* Get border */
  zathura_annotation_border_t border;
  if (zathura_annotation_border_init(&border) != ZATHURA_ERROR_OK) {
  }

  if (zathura_annotation_circle_get_border(priv->annotation, &border) != ZATHURA_ERROR_OK) {
  }

  /* Draw circle */
  cairo_save(cairo);

  /* Set color */
  zathura_annotation_color_t color;
  if (zathura_annotation_circle_get_color(priv->annotation, &color) == ZATHURA_ERROR_OK) {
    zathura_gtk_annotation_set_cairo_color(cairo, color, opacity);
  } else {
    cairo_set_source_rgb(cairo, 0, 0, 0);
  }

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

  double line_width = border.width * scale;

  if (border.effect == ZATHURA_ANNOTATION_BORDER_EFFECT_CLOUDY) {
    double cloud_effect_width = 10 * scale;
    double circle_width_half  = (width  - 2 * line_width - 2 * cloud_effect_width) / 2.0;
    double circle_height_half = (height - 2 * line_width - 2 * cloud_effect_width) / 2.0;

    // Determine minor and major radius
    double a = circle_width_half;
    double b = circle_height_half;
    bool switch_axes = false;

    if (a < b) {
      a = circle_height_half;
      b = circle_width_half;
      switch_axes = true;
    }

    // Hudson approximation of circumference
    double h = (a - b) * (a - b) / ((a + b) * (a + b));
    double circumference = 0.25 * M_PI * (a + b) * (3 * (1 + h/4) + 1/(1 - h/4));

    // Pre-calculated values
    double a2 = pow(a, 2);
    double b2 = pow(b, 2);
    double ab = a * b;

    double x_previous = 0;
    double y_previous = 0;
    double x_start = 0;
    double y_start = 0;

    unsigned int points_on_ellipsis = circumference / (10 * border.intensity);
    for (unsigned int i = 0; i < points_on_ellipsis; i++) {
      double angle = i * 360.0 / points_on_ellipsis;
      double phi = i * (2 * M_PI) / points_on_ellipsis;

      double x = (ab) / (sqrt(b2 + a2 * pow(tan(phi), 2)));
      double y = (ab) / (sqrt(a2 + b2 / pow(tan(phi), 2)));

      if (90 <= angle && angle < 180) {
        x = -x;
      } else if (180 <= angle && angle <= 270) {
        x = -x;
        y = -y;
      } else if (270 < angle && angle < 360) {
        y = -y;
      }

      if (switch_axes == true) {
        double tmp = x;
        x = y;
        y = tmp;
      }

      x += width / 2;
      y += height / 2;

      if (i == 0) {
        cairo_move_to(cairo, x, y);
        x_start = x;
        y_start = y;
      } else {
        calculate_cloud_line(cairo, x, y, x_previous, y_previous, cloud_effect_width, switch_axes);
      }

      x_previous = x;
      y_previous = y;
    }

    calculate_cloud_line(cairo, x_start, y_start, x_previous, y_previous, cloud_effect_width, switch_axes);
    cairo_set_line_width(cairo, line_width);
    cairo_stroke(cairo);
  } else {
    double circle_width_half  = (width  - 2 * line_width) / 2.0;
    double circle_height_half = (height - 2 * line_width) / 2.0;

    cairo_translate(cairo, width / 2.0, height / 2.0);
    cairo_save(cairo);
    cairo_scale(cairo, circle_width_half, circle_height_half);
    cairo_arc(cairo, 0, 0, 1, 0, 2 * M_PI);
    cairo_restore(cairo);

    cairo_set_line_width(cairo, line_width);
    cairo_stroke(cairo);
  }

  cairo_restore(cairo);

  return GDK_EVENT_STOP;
}

static void calculate_cloud_line(cairo_t* cairo, double x, double y, double x_previous,
    double y_previous, double width, bool switch_axes)
{
  /* Normalize orthogonal vector */
  double normal_vector_x =  (y - y_previous);
  double normal_vector_y = -(x - x_previous);
  double length = sqrt(pow(normal_vector_x, 2) + pow(normal_vector_y,2));
  normal_vector_x /= length;
  normal_vector_y /= length;

  int direction = 1;
  if (switch_axes == true) {
    direction = -1;
  }

  double cp_1_x = x_previous + (x - x_previous) * 0.25;
  double cp_1_y = y_previous + (y - y_previous) * 0.25;
  double cp_2_x = x_previous + (x - x_previous) * 0.75;
  double cp_2_y = y_previous + (y - y_previous) * 0.75;

  cp_1_x = cp_1_x + direction * normal_vector_x * width;
  cp_1_y = cp_1_y + direction * normal_vector_y * width;
  cp_2_x = cp_2_x + direction * normal_vector_x * width;
  cp_2_y = cp_2_y + direction * normal_vector_y * width;

  cairo_curve_to(cairo, cp_1_x, cp_1_y, cp_2_x, cp_2_y, x, y);
}
