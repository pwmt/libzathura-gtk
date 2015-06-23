 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_POLYGON
#define ZATHURA_GTK_ANNOTATION_POLYGON

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationPolygon ZathuraAnnotationPolygon;
typedef struct _ZathuraAnnotationPolygonClass ZathuraAnnotationPolygonClass;

#define ZATHURA_TYPE_ANNOTATION_POLYGON            (zathura_gtk_annotation_polygon_get_type())
#define ZATHURA_ANNOTATION_POLYGON(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_POLYGON, ZathuraAnnotationPolygon))
#define ZATHURA_ANNOTATION_POLYGON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_POLYGON, ZathuraAnnotationPolygonClass))
#define ZATHURA_IS_ANNOTATION_POLYGON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_POLYGON))
#define ZATHURA_IS_ANNOTATION_POLYGON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_POLYGON))
#define ZATHURA_ANNOTATION_POLYGON_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_POLYGON, ZathuraAnnotationPolygonClass))

typedef struct _ZathuraAnnotationPolygonPrivate ZathuraAnnotationPolygonPrivate;

struct _ZathuraAnnotationPolygon {
  ZathuraAnnotation parent;
  ZathuraAnnotationPolygonPrivate* priv;
};

struct _ZathuraAnnotationPolygonClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_polygon_get_type(void);

GtkWidget* zathura_gtk_annotation_polygon_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_POLYGON */
