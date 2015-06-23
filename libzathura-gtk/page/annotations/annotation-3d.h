 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_3D
#define ZATHURA_GTK_ANNOTATION_3D

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotation3D ZathuraAnnotation3D;
typedef struct _ZathuraAnnotation3DClass ZathuraAnnotation3DClass;

#define ZATHURA_TYPE_ANNOTATION_3D            (zathura_gtk_annotation_3d_get_type())
#define ZATHURA_ANNOTATION_3D(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_3D, ZathuraAnnotation3D))
#define ZATHURA_ANNOTATION_3D_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_3D, ZathuraAnnotation3DClass))
#define ZATHURA_IS_ANNOTATION_3D(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_3D))
#define ZATHURA_IS_ANNOTATION_3D_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_3D))
#define ZATHURA_ANNOTATION_3D_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_3D, ZathuraAnnotation3DClass))

typedef struct _ZathuraAnnotation3DPrivate ZathuraAnnotation3DPrivate;

struct _ZathuraAnnotation3D {
  ZathuraAnnotation parent;
  ZathuraAnnotation3DPrivate* priv;
};

struct _ZathuraAnnotation3DClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_3d_get_type(void);

GtkWidget* zathura_gtk_annotation_3d_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_3D */
