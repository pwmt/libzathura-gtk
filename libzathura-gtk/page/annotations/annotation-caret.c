 /* See LICENSE file for license and copyright information */

#include "annotation-caret.h"
#include "../../macros.h"

struct _ZathuraAnnotationCaretPrivate {
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_caret_draw(GtkWidget* widget, cairo_t *cairo);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationCaret, zathura_gtk_annotation_caret, GTK_TYPE_DRAWING_AREA)

#define ZATHURA_ANNOTATION_CARET_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_CARET, \
                               ZathuraAnnotationCaretPrivate))

static void
zathura_gtk_annotation_caret_class_init(ZathuraAnnotationCaretClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

  widget_class->draw = cb_zathura_gtk_annotation_caret_draw;
}

static void
zathura_gtk_annotation_caret_init(ZathuraAnnotationCaret* widget)
{
  ZathuraAnnotationCaretPrivate* priv = ZATHURA_ANNOTATION_CARET_GET_PRIVATE(widget);

  priv->annotation = NULL;
}

GtkWidget*
zathura_gtk_annotation_caret_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_CARET, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationCaretPrivate* priv = ZATHURA_ANNOTATION_CARET_GET_PRIVATE(widget);

  priv->annotation = annotation;

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_caret_draw(GtkWidget* widget, cairo_t *cairo)
{
  ZathuraAnnotationCaretPrivate* priv = ZATHURA_ANNOTATION_CARET_GET_PRIVATE(widget);

  const unsigned int widget_height = gtk_widget_get_allocated_height(widget);

  cairo_save(cairo);

  cairo_select_font_face(cairo, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cairo, widget_height);

  cairo_move_to (cairo, 0, widget_height);
  cairo_show_text (cairo, "¶");

  cairo_restore(cairo);

  return TRUE;
}
