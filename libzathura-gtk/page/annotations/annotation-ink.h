 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_INK
#define ZATHURA_GTK_ANNOTATION_INK

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"
#include "utils.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationInk ZathuraAnnotationInk;
typedef struct _ZathuraAnnotationInkClass ZathuraAnnotationInkClass;

#define ZATHURA_TYPE_ANNOTATION_INK            (zathura_gtk_annotation_ink_get_type())
#define ZATHURA_ANNOTATION_INK(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_INK, ZathuraAnnotationInk))
#define ZATHURA_ANNOTATION_INK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_INK, ZathuraAnnotationInkClass))
#define ZATHURA_IS_ANNOTATION_INK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_INK))
#define ZATHURA_IS_ANNOTATION_INK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_INK))
#define ZATHURA_ANNOTATION_INK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_INK, ZathuraAnnotationInkClass))

typedef struct _ZathuraAnnotationInkPrivate ZathuraAnnotationInkPrivate;

struct _ZathuraAnnotationInk {
  ZathuraAnnotation parent;
  ZathuraAnnotationInkPrivate* priv;
};

struct _ZathuraAnnotationInkClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_ink_get_type(void);

GtkWidget* zathura_gtk_annotation_ink_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_INK */
