 /* See LICENSE file for license and copyright information */

#include "annotation-free-text.h"
#include "../../macros.h"

struct _ZathuraAnnotationFreeTextPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static void cb_zathura_gtk_annotation_free_text_draw(GtkDrawingArea* area, cairo_t* cairo, int allocated_width, int allocated_height, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationFreeText, zathura_gtk_annotation_free_text, ZATHURA_TYPE_ANNOTATION)

static void
zathura_gtk_annotation_free_text_class_init(ZathuraAnnotationFreeTextClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_free_text_init(ZathuraAnnotationFreeText* widget)
{
  ZathuraAnnotationFreeTextPrivate* priv = zathura_gtk_annotation_free_text_get_instance_private(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_free_text_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_FREE_TEXT, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationFreeTextPrivate* priv = zathura_gtk_annotation_free_text_get_instance_private(ZATHURA_ANNOTATION_FREE_TEXT(widget));

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(priv->drawing_area), cb_zathura_gtk_annotation_free_text_draw, widget, NULL);
  gtk_widget_set_hexpand(priv->drawing_area, TRUE);
  gtk_widget_set_vexpand(priv->drawing_area, TRUE);
  gtk_widget_set_visible(priv->drawing_area, TRUE);

  gtk_box_append(GTK_BOX(widget), GTK_WIDGET(priv->drawing_area));
  gtk_widget_set_visible(GTK_WIDGET(widget), TRUE);

  return GTK_WIDGET(widget);
}

static void
cb_zathura_gtk_annotation_free_text_draw(GtkDrawingArea* area, cairo_t* cairo, int allocated_width, int allocated_height, gpointer data)
{
  GtkWidget* widget = GTK_WIDGET(area);
  (void) allocated_width;
  (void) allocated_height;
  ZathuraAnnotationFreeTextPrivate* priv = zathura_gtk_annotation_free_text_get_instance_private(data);

  double scale;
  g_object_get(G_OBJECT(data), "scale", &scale, NULL);

  char* text;
  if (zathura_annotation_free_text_get_text(priv->annotation, &text) != ZATHURA_ERROR_OK) {
    return;
  }

  zathura_annotation_justification_t justification;
  if (zathura_annotation_free_text_get_justification(priv->annotation, &justification) != ZATHURA_ERROR_OK) {
    return;
  }

  char* rich_text;
  if (zathura_annotation_free_text_get_rich_text(priv->annotation, &rich_text) != ZATHURA_ERROR_OK) {
    return;
  }

  char* style_string;
  if (zathura_annotation_free_text_get_style_string(priv->annotation, &style_string) != ZATHURA_ERROR_OK) {
    return;
  }

  zathura_annotation_callout_line_t callout_line;
  if (zathura_annotation_free_text_get_callout_line(priv->annotation, &callout_line) != ZATHURA_ERROR_OK) {
    return;
  }

  zathura_annotation_border_t border;
  if (zathura_annotation_free_text_get_border(priv->annotation, &border) != ZATHURA_ERROR_OK) {
    return;
  }

  zathura_annotation_markup_intent_t intent;
  if (zathura_annotation_free_text_get_intent(priv->annotation, &intent) != ZATHURA_ERROR_OK) {
    return;
  }

  zathura_annotation_padding_t padding;
  if (zathura_annotation_free_text_get_padding(priv->annotation, &padding) != ZATHURA_ERROR_OK) {
    return;
  }

  zathura_annotation_line_ending_t line_ending[2];
  if (zathura_annotation_free_text_get_line_ending(priv->annotation, line_ending) != ZATHURA_ERROR_OK) {
    return;
  }

  const unsigned int height = gtk_widget_get_height(widget);
  const unsigned int width  = gtk_widget_get_width(widget);

  cairo_save(cairo);
  cairo_set_font_size(cairo, 10 * scale);
  cairo_set_source_rgb(cairo, 0, 0, 0);
  cairo_move_to(cairo, 0, height);
  cairo_show_text(cairo, text);
  cairo_restore(cairo);

  return;
}
