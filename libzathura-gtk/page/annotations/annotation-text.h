 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_TEXT
#define ZATHURA_GTK_ANNOTATION_TEXT

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"
#include "utils.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationText ZathuraAnnotationText;
typedef struct _ZathuraAnnotationTextClass ZathuraAnnotationTextClass;

#define ZATHURA_TYPE_ANNOTATION_TEXT            (zathura_gtk_annotation_text_get_type())
#define ZATHURA_ANNOTATION_TEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_TEXT, ZathuraAnnotationText))
#define ZATHURA_ANNOTATION_TEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_TEXT, ZathuraAnnotationTextClass))
#define ZATHURA_IS_ANNOTATION_TEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_TEXT))
#define ZATHURA_IS_ANNOTATION_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_TEXT))
#define ZATHURA_ANNOTATION_TEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_TEXT, ZathuraAnnotationTextClass))

typedef struct _ZathuraAnnotationTextPrivate ZathuraAnnotationTextPrivate;

struct _ZathuraAnnotationText {
  ZathuraAnnotation parent;
  ZathuraAnnotationTextPrivate* priv;
};

struct _ZathuraAnnotationTextClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_text_get_type(void);

GtkWidget* zathura_gtk_annotation_text_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_TEXT */
