 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_CIRCLE
#define ZATHURA_GTK_ANNOTATION_CIRCLE

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationCircle ZathuraAnnotationCircle;
typedef struct _ZathuraAnnotationCircleClass ZathuraAnnotationCircleClass;

#define ZATHURA_TYPE_ANNOTATION_CIRCLE            (zathura_gtk_annotation_circle_get_type())
#define ZATHURA_ANNOTATION_CIRCLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_CIRCLE, ZathuraAnnotationCircle))
#define ZATHURA_ANNOTATION_CIRCLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_CIRCLE, ZathuraAnnotationCircleClass))
#define ZATHURA_IS_ANNOTATION_CIRCLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_CIRCLE))
#define ZATHURA_IS_ANNOTATION_CIRCLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_CIRCLE))
#define ZATHURA_ANNOTATION_CIRCLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_CIRCLE, ZathuraAnnotationCircleClass))

typedef struct _ZathuraAnnotationCirclePrivate ZathuraAnnotationCirclePrivate;

struct _ZathuraAnnotationCircle {
  ZathuraAnnotation parent;
  ZathuraAnnotationCirclePrivate* priv;
};

struct _ZathuraAnnotationCircleClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_circle_get_type(void);

GtkWidget* zathura_gtk_annotation_circle_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_CIRCLE */
