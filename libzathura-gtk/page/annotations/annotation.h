 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION
#define ZATHURA_GTK_ANNOTATION

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

G_BEGIN_DECLS

typedef struct _ZathuraAnnotation ZathuraAnnotation;
typedef struct _ZathuraAnnotationClass ZathuraAnnotationClass;

#define ZATHURA_TYPE_ANNOTATION            (zathura_gtk_annotation_get_type())
#define ZATHURA_ANNOTATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION, ZathuraAnnotation))
#define ZATHURA_ANNOTATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION, ZathuraAnnotationClass))
#define ZATHURA_IS_ANNOTATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION))
#define ZATHURA_IS_ANNOTATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION))
#define ZATHURA_ANNOTATION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION, ZathuraAnnotationClass))

typedef struct _ZathuraAnnotationPrivate ZathuraAnnotationPrivate;

struct _ZathuraAnnotation {
  GtkWidget parent;
  ZathuraAnnotationPrivate* priv;
};

struct _ZathuraAnnotationClass {
  GtkWidgetClass parent_class;
};


GType zathura_gtk_annotation_get_type(void);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION */
