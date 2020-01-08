 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_UNDERLINE
#define ZATHURA_GTK_ANNOTATION_UNDERLINE

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"
#include "utils.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationUnderline ZathuraAnnotationUnderline;
typedef struct _ZathuraAnnotationUnderlineClass ZathuraAnnotationUnderlineClass;

#define ZATHURA_TYPE_ANNOTATION_UNDERLINE            (zathura_gtk_annotation_underline_get_type())
#define ZATHURA_ANNOTATION_UNDERLINE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_UNDERLINE, ZathuraAnnotationUnderline))
#define ZATHURA_ANNOTATION_UNDERLINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_UNDERLINE, ZathuraAnnotationUnderlineClass))
#define ZATHURA_IS_ANNOTATION_UNDERLINE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_UNDERLINE))
#define ZATHURA_IS_ANNOTATION_UNDERLINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_UNDERLINE))
#define ZATHURA_ANNOTATION_UNDERLINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_UNDERLINE, ZathuraAnnotationUnderlineClass))

typedef struct _ZathuraAnnotationUnderlinePrivate ZathuraAnnotationUnderlinePrivate;

struct _ZathuraAnnotationUnderline {
  ZathuraAnnotation parent;
  ZathuraAnnotationUnderlinePrivate* priv;
};

struct _ZathuraAnnotationUnderlineClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_underline_get_type(void);

GtkWidget* zathura_gtk_annotation_underline_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_UNDERLINE */
