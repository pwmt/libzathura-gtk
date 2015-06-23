 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_SQUARE
#define ZATHURA_GTK_ANNOTATION_SQUARE

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationSquare ZathuraAnnotationSquare;
typedef struct _ZathuraAnnotationSquareClass ZathuraAnnotationSquareClass;

#define ZATHURA_TYPE_ANNOTATION_SQUARE            (zathura_gtk_annotation_square_get_type())
#define ZATHURA_ANNOTATION_SQUARE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_SQUARE, ZathuraAnnotationSquare))
#define ZATHURA_ANNOTATION_SQUARE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_SQUARE, ZathuraAnnotationSquareClass))
#define ZATHURA_IS_ANNOTATION_SQUARE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_SQUARE))
#define ZATHURA_IS_ANNOTATION_SQUARE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_SQUARE))
#define ZATHURA_ANNOTATION_SQUARE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_SQUARE, ZathuraAnnotationSquareClass))

typedef struct _ZathuraAnnotationSquarePrivate ZathuraAnnotationSquarePrivate;

struct _ZathuraAnnotationSquare {
  ZathuraAnnotation parent;
  ZathuraAnnotationSquarePrivate* priv;
};

struct _ZathuraAnnotationSquareClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_square_get_type(void);

GtkWidget* zathura_gtk_annotation_square_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_SQUARE */
