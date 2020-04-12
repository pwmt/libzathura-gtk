 /* See LICENSE file for license and copyright information */

#include "annotation-free-text.h"
#include "../../macros.h"

struct _ZathuraAnnotationFreeTextPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_free_text_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

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
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_free_text_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));
  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_free_text_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationFreeTextPrivate* priv = zathura_gtk_annotation_free_text_get_instance_private(data);

  double scale;
  g_object_get(G_OBJECT(data), "scale", &scale, NULL);

  char* text;
  if (zathura_annotation_free_text_get_text(priv->annotation, &text) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  zathura_annotation_justification_t justification;
  if (zathura_annotation_free_text_get_justification(priv->annotation, &justification) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  char* rich_text;
  if (zathura_annotation_free_text_get_rich_text(priv->annotation, &rich_text) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  char* style_string;
  if (zathura_annotation_free_text_get_style_string(priv->annotation, &style_string) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  zathura_annotation_callout_line_t callout_line;
  if (zathura_annotation_free_text_get_callout_line(priv->annotation, &callout_line) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  zathura_annotation_border_t border;
  if (zathura_annotation_free_text_get_border(priv->annotation, &border) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  zathura_annotation_markup_intent_t intent;
  if (zathura_annotation_free_text_get_intent(priv->annotation, &intent) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  zathura_annotation_padding_t padding;
  if (zathura_annotation_free_text_get_padding(priv->annotation, &padding) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  zathura_annotation_line_ending_t line_ending[2];
  if (zathura_annotation_free_text_get_line_ending(priv->annotation, line_ending) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  const unsigned int height = gtk_widget_get_allocated_height(widget);
  const unsigned int width  = gtk_widget_get_allocated_width(widget);

  cairo_save(cairo);
  cairo_set_font_size(cairo, 10 * scale);
  cairo_set_source_rgb(cairo, 0, 0, 0);
  cairo_move_to(cairo, 0, height);
  cairo_show_text(cairo, text);
  cairo_restore(cairo);

  return GDK_EVENT_PROPAGATE;
}
