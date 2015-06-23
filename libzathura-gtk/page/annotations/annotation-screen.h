 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_SCREEN
#define ZATHURA_GTK_ANNOTATION_SCREEN

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationScreen ZathuraAnnotationScreen;
typedef struct _ZathuraAnnotationScreenClass ZathuraAnnotationScreenClass;

#define ZATHURA_TYPE_ANNOTATION_SCREEN            (zathura_gtk_annotation_screen_get_type())
#define ZATHURA_ANNOTATION_SCREEN(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_SCREEN, ZathuraAnnotationScreen))
#define ZATHURA_ANNOTATION_SCREEN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_SCREEN, ZathuraAnnotationScreenClass))
#define ZATHURA_IS_ANNOTATION_SCREEN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_SCREEN))
#define ZATHURA_IS_ANNOTATION_SCREEN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_SCREEN))
#define ZATHURA_ANNOTATION_SCREEN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_SCREEN, ZathuraAnnotationScreenClass))

typedef struct _ZathuraAnnotationScreenPrivate ZathuraAnnotationScreenPrivate;

struct _ZathuraAnnotationScreen {
  ZathuraAnnotation parent;
  ZathuraAnnotationScreenPrivate* priv;
};

struct _ZathuraAnnotationScreenClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_screen_get_type(void);

GtkWidget* zathura_gtk_annotation_screen_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_SCREEN */
