 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_LINE
#define ZATHURA_GTK_ANNOTATION_LINE

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"
#include "utils.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationLine ZathuraAnnotationLine;
typedef struct _ZathuraAnnotationLineClass ZathuraAnnotationLineClass;

#define ZATHURA_TYPE_ANNOTATION_LINE            (zathura_gtk_annotation_line_get_type())
#define ZATHURA_ANNOTATION_LINE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_LINE, ZathuraAnnotationLine))
#define ZATHURA_ANNOTATION_LINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_LINE, ZathuraAnnotationLineClass))
#define ZATHURA_IS_ANNOTATION_LINE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_LINE))
#define ZATHURA_IS_ANNOTATION_LINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_LINE))
#define ZATHURA_ANNOTATION_LINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_LINE, ZathuraAnnotationLineClass))

typedef struct _ZathuraAnnotationLinePrivate ZathuraAnnotationLinePrivate;

struct _ZathuraAnnotationLine {
  ZathuraAnnotation parent;
  ZathuraAnnotationLinePrivate* priv;
};

struct _ZathuraAnnotationLineClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_line_get_type(void);

GtkWidget* zathura_gtk_annotation_line_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_LINE */
