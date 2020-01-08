 /* See LICENSE file for license and copyright information */

#include "annotation-caret.h"
#include "../../macros.h"
#include "utils.h"

struct _ZathuraAnnotationCaretPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_caret_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationCaret, zathura_gtk_annotation_caret, ZATHURA_TYPE_ANNOTATION)

#define ZATHURA_ANNOTATION_CARET_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_CARET, \
                               ZathuraAnnotationCaretPrivate))

static void
zathura_gtk_annotation_caret_class_init(ZathuraAnnotationCaretClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_caret_init(ZathuraAnnotationCaret* widget)
{
  ZathuraAnnotationCaretPrivate* priv = ZATHURA_ANNOTATION_CARET_GET_PRIVATE(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_caret_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_CARET, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationCaretPrivate* priv = ZATHURA_ANNOTATION_CARET_GET_PRIVATE(widget);

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_caret_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_caret_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationCaretPrivate* priv = ZATHURA_ANNOTATION_CARET_GET_PRIVATE(data);

  const unsigned int widget_height = gtk_widget_get_allocated_height(widget);

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

  return GDK_EVENT_STOP;
}
