 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_STAMP
#define ZATHURA_GTK_ANNOTATION_STAMP

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationStamp ZathuraAnnotationStamp;
typedef struct _ZathuraAnnotationStampClass ZathuraAnnotationStampClass;

#define ZATHURA_TYPE_ANNOTATION_STAMP            (zathura_gtk_annotation_stamp_get_type())
#define ZATHURA_ANNOTATION_STAMP(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_STAMP, ZathuraAnnotationStamp))
#define ZATHURA_ANNOTATION_STAMP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_STAMP, ZathuraAnnotationStampClass))
#define ZATHURA_IS_ANNOTATION_STAMP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_STAMP))
#define ZATHURA_IS_ANNOTATION_STAMP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_STAMP))
#define ZATHURA_ANNOTATION_STAMP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_STAMP, ZathuraAnnotationStampClass))

typedef struct _ZathuraAnnotationStampPrivate ZathuraAnnotationStampPrivate;

struct _ZathuraAnnotationStamp {
  ZathuraAnnotation parent;
  ZathuraAnnotationStampPrivate* priv;
};

struct _ZathuraAnnotationStampClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_stamp_get_type(void);

GtkWidget* zathura_gtk_annotation_stamp_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_STAMP */
