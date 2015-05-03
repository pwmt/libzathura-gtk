 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_STRIKE_OUT
#define ZATHURA_GTK_ANNOTATION_STRIKE_OUT

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationStrikeOut ZathuraAnnotationStrikeOut;
typedef struct _ZathuraAnnotationStrikeOutClass ZathuraAnnotationStrikeOutClass;

#define ZATHURA_TYPE_ANNOTATION_STRIKE_OUT            (zathura_gtk_annotation_strike_out_get_type())
#define ZATHURA_ANNOTATION_STRIKE_OUT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_STRIKE_OUT, ZathuraAnnotationStrikeOut))
#define ZATHURA_ANNOTATION_STRIKE_OUT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_STRIKE_OUT, ZathuraAnnotationStrikeOutClass))
#define ZATHURA_IS_ANNOTATION_STRIKE_OUT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_STRIKE_OUT))
#define ZATHURA_IS_ANNOTATION_STRIKE_OUT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_STRIKE_OUT))
#define ZATHURA_ANNOTATION_STRIKE_OUT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_STRIKE_OUT, ZathuraAnnotationStrikeOutClass))

typedef struct _ZathuraAnnotationStrikeOutPrivate ZathuraAnnotationStrikeOutPrivate;

struct _ZathuraAnnotationStrikeOut {
  ZathuraAnnotation parent;
  ZathuraAnnotationStrikeOutPrivate* priv;
};

struct _ZathuraAnnotationStrikeOutClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_strike_out_get_type(void);

GtkWidget* zathura_gtk_annotation_strike_out_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_STRIKE_OUT */
