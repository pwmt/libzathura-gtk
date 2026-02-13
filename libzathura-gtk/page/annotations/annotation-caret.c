 /* See LICENSE file for license and copyright information */

#include "annotation-caret.h"
#include "../../macros.h"
#include "utils.h"

struct _ZathuraAnnotationCaretPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static void cb_zathura_gtk_annotation_caret_draw(GtkDrawingArea* area, cairo_t* cairo, int allocated_width, int allocated_height, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationCaret, zathura_gtk_annotation_caret, ZATHURA_TYPE_ANNOTATION)

static void
zathura_gtk_annotation_caret_class_init(ZathuraAnnotationCaretClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_caret_init(ZathuraAnnotationCaret* widget)
{
  ZathuraAnnotationCaretPrivate* priv = zathura_gtk_annotation_caret_get_instance_private(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_caret_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_CARET, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationCaretPrivate* priv = zathura_gtk_annotation_caret_get_instance_private(ZATHURA_ANNOTATION_CARET(widget));

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(priv->drawing_area), cb_zathura_gtk_annotation_caret_draw, widget, NULL);
  gtk_widget_set_hexpand(priv->drawing_area, TRUE);
  gtk_widget_set_vexpand(priv->drawing_area, TRUE);
  gtk_widget_set_visible(priv->drawing_area, TRUE);

  gtk_box_append(GTK_BOX(widget), GTK_WIDGET(priv->drawing_area));

  gtk_widget_set_visible(GTK_WIDGET(widget), TRUE);

  return GTK_WIDGET(widget);
}

static void
cb_zathura_gtk_annotation_caret_draw(GtkDrawingArea* area, cairo_t* cairo, int allocated_width, int allocated_height, gpointer data)
{
  GtkWidget* widget = GTK_WIDGET(area);
  (void) allocated_width;
  (void) allocated_height;
  ZathuraAnnotationCaretPrivate* priv = zathura_gtk_annotation_caret_get_instance_private(data);

  const unsigned int widget_height = gtk_widget_get_height(widget);

  cairo_save(cairo);

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

  cairo_select_font_face(cairo, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cairo, widget_height);

  cairo_move_to(cairo, 0, widget_height);
  cairo_show_text(cairo, "¶");

  cairo_restore(cairo);

  return;
}
