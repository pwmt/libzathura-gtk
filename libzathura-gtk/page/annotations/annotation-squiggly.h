 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_SQUIGGLY
#define ZATHURA_GTK_ANNOTATION_SQUIGGLY

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"
#include "utils.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationSquiggly ZathuraAnnotationSquiggly;
typedef struct _ZathuraAnnotationSquigglyClass ZathuraAnnotationSquigglyClass;

#define ZATHURA_TYPE_ANNOTATION_SQUIGGLY            (zathura_gtk_annotation_squiggly_get_type())
#define ZATHURA_ANNOTATION_SQUIGGLY(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_SQUIGGLY, ZathuraAnnotationSquiggly))
#define ZATHURA_ANNOTATION_SQUIGGLY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_SQUIGGLY, ZathuraAnnotationSquigglyClass))
#define ZATHURA_IS_ANNOTATION_SQUIGGLY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_SQUIGGLY))
#define ZATHURA_IS_ANNOTATION_SQUIGGLY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_SQUIGGLY))
#define ZATHURA_ANNOTATION_SQUIGGLY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_SQUIGGLY, ZathuraAnnotationSquigglyClass))

typedef struct _ZathuraAnnotationSquigglyPrivate ZathuraAnnotationSquigglyPrivate;

struct _ZathuraAnnotationSquiggly {
  ZathuraAnnotation parent;
  ZathuraAnnotationSquigglyPrivate* priv;
};

struct _ZathuraAnnotationSquigglyClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_squiggly_get_type(void);

GtkWidget* zathura_gtk_annotation_squiggly_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_SQUIGGLY */
