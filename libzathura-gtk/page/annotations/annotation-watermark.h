 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_WATERMARK
#define ZATHURA_GTK_ANNOTATION_WATERMARK

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationWatermark ZathuraAnnotationWatermark;
typedef struct _ZathuraAnnotationWatermarkClass ZathuraAnnotationWatermarkClass;

#define ZATHURA_TYPE_ANNOTATION_WATERMARK            (zathura_gtk_annotation_watermark_get_type())
#define ZATHURA_ANNOTATION_WATERMARK(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_WATERMARK, ZathuraAnnotationWatermark))
#define ZATHURA_ANNOTATION_WATERMARK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_WATERMARK, ZathuraAnnotationWatermarkClass))
#define ZATHURA_IS_ANNOTATION_WATERMARK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_WATERMARK))
#define ZATHURA_IS_ANNOTATION_WATERMARK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_WATERMARK))
#define ZATHURA_ANNOTATION_WATERMARK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_WATERMARK, ZathuraAnnotationWatermarkClass))

typedef struct _ZathuraAnnotationWatermarkPrivate ZathuraAnnotationWatermarkPrivate;

struct _ZathuraAnnotationWatermark {
  ZathuraAnnotation parent;
  ZathuraAnnotationWatermarkPrivate* priv;
};

struct _ZathuraAnnotationWatermarkClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_watermark_get_type(void);

GtkWidget* zathura_gtk_annotation_watermark_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_WATERMARK */
