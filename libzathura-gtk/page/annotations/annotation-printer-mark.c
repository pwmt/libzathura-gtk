 /* See LICENSE file for license and copyright information */

#include "annotation-printer-mark.h"
#include "../../macros.h"

struct _ZathuraAnnotationPrinterMarkPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_printer_mark_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationPrinterMark, zathura_gtk_annotation_printer_mark, ZATHURA_TYPE_ANNOTATION)

#define ZATHURA_ANNOTATION_PRINTER_MARK_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_PRINTER_MARK, \
                               ZathuraAnnotationPrinterMarkPrivate))

static void
zathura_gtk_annotation_printer_mark_class_init(ZathuraAnnotationPrinterMarkClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_printer_mark_init(ZathuraAnnotationPrinterMark* widget)
{
  ZathuraAnnotationPrinterMarkPrivate* priv = ZATHURA_ANNOTATION_PRINTER_MARK_GET_PRIVATE(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_printer_mark_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_PRINTER_MARK, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationPrinterMarkPrivate* priv = ZATHURA_ANNOTATION_PRINTER_MARK_GET_PRIVATE(widget);

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_printer_mark_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));
  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_printer_mark_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationPrinterMarkPrivate* priv = ZATHURA_ANNOTATION_PRINTER_MARK_GET_PRIVATE(data);

  // TODO: Implement this widget

  return TRUE;
}
