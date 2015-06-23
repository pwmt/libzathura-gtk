 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_WIDGET
#define ZATHURA_GTK_ANNOTATION_WIDGET

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationWidget ZathuraAnnotationWidget;
typedef struct _ZathuraAnnotationWidgetClass ZathuraAnnotationWidgetClass;

#define ZATHURA_TYPE_ANNOTATION_WIDGET            (zathura_gtk_annotation_widget_get_type())
#define ZATHURA_ANNOTATION_WIDGET(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_WIDGET, ZathuraAnnotationWidget))
#define ZATHURA_ANNOTATION_WIDGET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_WIDGET, ZathuraAnnotationWidgetClass))
#define ZATHURA_IS_ANNOTATION_WIDGET(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_WIDGET))
#define ZATHURA_IS_ANNOTATION_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_WIDGET))
#define ZATHURA_ANNOTATION_WIDGET_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_WIDGET, ZathuraAnnotationWidgetClass))

typedef struct _ZathuraAnnotationWidgetPrivate ZathuraAnnotationWidgetPrivate;

struct _ZathuraAnnotationWidget {
  ZathuraAnnotation parent;
  ZathuraAnnotationWidgetPrivate* priv;
};

struct _ZathuraAnnotationWidgetClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_widget_get_type(void);

GtkWidget* zathura_gtk_annotation_widget_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_WIDGET */
