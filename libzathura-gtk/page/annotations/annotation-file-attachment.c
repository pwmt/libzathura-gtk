 /* See LICENSE file for license and copyright information */

#include <librsvg/rsvg.h>
#include <string.h>

#include "annotation-file-attachment.h"
#include "icons.h"
#include "../../macros.h"

struct _ZathuraAnnotationFileAttachmentPrivate {
  GtkWidget* drawing_area;
  zathura_annotation_t* annotation;
};

static gboolean cb_zathura_gtk_annotation_file_attachment_draw(GtkWidget* widget, cairo_t *cairo, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraAnnotationFileAttachment, zathura_gtk_annotation_file_attachment, ZATHURA_TYPE_ANNOTATION)

#define ZATHURA_ANNOTATION_FILE_ATTACHMENT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_ANNOTATION_FILE_ATTACHMENT, \
                               ZathuraAnnotationFileAttachmentPrivate))

static void
zathura_gtk_annotation_file_attachment_class_init(ZathuraAnnotationFileAttachmentClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
}

static void
zathura_gtk_annotation_file_attachment_init(ZathuraAnnotationFileAttachment* widget)
{
  ZathuraAnnotationFileAttachmentPrivate* priv = ZATHURA_ANNOTATION_FILE_ATTACHMENT_GET_PRIVATE(widget);

  priv->drawing_area = NULL;
  priv->annotation   = NULL;
}

GtkWidget*
zathura_gtk_annotation_file_attachment_new(zathura_annotation_t* annotation)
{
  g_return_val_if_fail(annotation != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_ANNOTATION_FILE_ATTACHMENT, "annotation", annotation, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraAnnotationFileAttachmentPrivate* priv = ZATHURA_ANNOTATION_FILE_ATTACHMENT_GET_PRIVATE(widget);

  priv->annotation = annotation;

  priv->drawing_area = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_zathura_gtk_annotation_file_attachment_draw), widget);

  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->drawing_area));
  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_zathura_gtk_annotation_file_attachment_draw(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraAnnotationFileAttachmentPrivate* priv = ZATHURA_ANNOTATION_FILE_ATTACHMENT_GET_PRIVATE(data);

  const unsigned int page_height = gtk_widget_get_allocated_height(widget);
  const unsigned int page_width  = gtk_widget_get_allocated_width(widget);

  char* icon_name;
  if (zathura_annotation_sound_get_icon_name(priv->annotation, &icon_name) !=
      ZATHURA_ERROR_OK) {
    return FALSE;
  }
  cairo_save(cairo);

  /* Open SVG file */
  RsvgHandle* svg_handle = NULL;

  if (icon_name != NULL && strcmp(icon_name, "Graph") == 0) {
    svg_handle = zathura_gtk_annotation_icon_get_handle(ZATHURA_GTK_ANNOTATION_ICON_GRAPH);
  } else if (icon_name != NULL && strcmp(icon_name, "Paperclip") == 0) {
    svg_handle = zathura_gtk_annotation_icon_get_handle(ZATHURA_GTK_ANNOTATION_ICON_PAPER_CLIP);
  } else if (icon_name != NULL && strcmp(icon_name, "Tag") == 0) {
    svg_handle = zathura_gtk_annotation_icon_get_handle(ZATHURA_GTK_ANNOTATION_ICON_TAG);
  } else { // PushPin (default)
    svg_handle = zathura_gtk_annotation_icon_get_handle(ZATHURA_GTK_ANNOTATION_ICON_PUSH_PIN);
  }
  if (svg_handle == NULL) {
    cairo_restore(cairo);
    return FALSE;
  }

  /* Calculate scale to fit SVG into area */
  RsvgDimensionData svg_dimension_data;
  rsvg_handle_get_dimensions(svg_handle, &svg_dimension_data);

  double scale_x = (double) page_width  / svg_dimension_data.width;
  double scale_y = (double) page_height / svg_dimension_data.height;

  if (scale_x > scale_y) {
    cairo_scale(cairo, scale_y, scale_y);
  } else {
    cairo_scale(cairo, scale_x, scale_x);
  }

  if (rsvg_handle_render_cairo(svg_handle, cairo) == FALSE) {
    cairo_restore(cairo);
    return FALSE;
  }

  if (scale_x > scale_y) {
    cairo_scale(cairo, 1/scale_y, 1/scale_y);
  } else {
    cairo_scale(cairo, 1/scale_x, 1/scale_x);
  }

  cairo_restore(cairo);
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

  /* cairo_set_operator (cairo, CAIRO_OPERATOR_IN); */
  /* cairo_rectangle(cairo, 0, 0, page_width, page_height); */
  /* cairo_fill(cairo); */
  /*  */

  cairo_restore(cairo);

  return TRUE;
}
