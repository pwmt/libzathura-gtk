 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_POLY_LINE
#define ZATHURA_GTK_ANNOTATION_POLY_LINE

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"
#include "utils.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationPolyLine ZathuraAnnotationPolyLine;
typedef struct _ZathuraAnnotationPolyLineClass ZathuraAnnotationPolyLineClass;

#define ZATHURA_TYPE_ANNOTATION_POLY_LINE            (zathura_gtk_annotation_poly_line_get_type())
#define ZATHURA_ANNOTATION_POLY_LINE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_POLY_LINE, ZathuraAnnotationPolyLine))
#define ZATHURA_ANNOTATION_POLY_LINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_POLY_LINE, ZathuraAnnotationPolyLineClass))
#define ZATHURA_IS_ANNOTATION_POLY_LINE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_POLY_LINE))
#define ZATHURA_IS_ANNOTATION_POLY_LINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_POLY_LINE))
#define ZATHURA_ANNOTATION_POLY_LINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_POLY_LINE, ZathuraAnnotationPolyLineClass))

typedef struct _ZathuraAnnotationPolyLinePrivate ZathuraAnnotationPolyLinePrivate;

struct _ZathuraAnnotationPolyLine {
  ZathuraAnnotation parent;
  ZathuraAnnotationPolyLinePrivate* priv;
};

struct _ZathuraAnnotationPolyLineClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_poly_line_get_type(void);

GtkWidget* zathura_gtk_annotation_poly_line_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_POLY_LINE */
