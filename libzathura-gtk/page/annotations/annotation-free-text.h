 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_FREE_TEXT
#define ZATHURA_GTK_ANNOTATION_FREE_TEXT

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationFreeText ZathuraAnnotationFreeText;
typedef struct _ZathuraAnnotationFreeTextClass ZathuraAnnotationFreeTextClass;

#define ZATHURA_TYPE_ANNOTATION_FREE_TEXT            (zathura_gtk_annotation_free_text_get_type())
#define ZATHURA_ANNOTATION_FREE_TEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_FREE_TEXT, ZathuraAnnotationFreeText))
#define ZATHURA_ANNOTATION_FREE_TEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_FREE_TEXT, ZathuraAnnotationFreeTextClass))
#define ZATHURA_IS_ANNOTATION_FREE_TEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_FREE_TEXT))
#define ZATHURA_IS_ANNOTATION_FREE_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_FREE_TEXT))
#define ZATHURA_ANNOTATION_FREE_TEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_FREE_TEXT, ZathuraAnnotationFreeTextClass))

typedef struct _ZathuraAnnotationFreeTextPrivate ZathuraAnnotationFreeTextPrivate;

struct _ZathuraAnnotationFreeText {
  ZathuraAnnotation parent;
  ZathuraAnnotationFreeTextPrivate* priv;
};

struct _ZathuraAnnotationFreeTextClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_free_text_get_type(void);

GtkWidget* zathura_gtk_annotation_free_text_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_FREE_TEXT */
