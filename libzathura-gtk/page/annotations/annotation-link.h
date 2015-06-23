 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_LINK
#define ZATHURA_GTK_ANNOTATION_LINK

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationLink ZathuraAnnotationLink;
typedef struct _ZathuraAnnotationLinkClass ZathuraAnnotationLinkClass;

#define ZATHURA_TYPE_ANNOTATION_LINK            (zathura_gtk_annotation_link_get_type())
#define ZATHURA_ANNOTATION_LINK(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_LINK, ZathuraAnnotationLink))
#define ZATHURA_ANNOTATION_LINK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_LINK, ZathuraAnnotationLinkClass))
#define ZATHURA_IS_ANNOTATION_LINK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_LINK))
#define ZATHURA_IS_ANNOTATION_LINK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_LINK))
#define ZATHURA_ANNOTATION_LINK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_LINK, ZathuraAnnotationLinkClass))

typedef struct _ZathuraAnnotationLinkPrivate ZathuraAnnotationLinkPrivate;

struct _ZathuraAnnotationLink {
  ZathuraAnnotation parent;
  ZathuraAnnotationLinkPrivate* priv;
};

struct _ZathuraAnnotationLinkClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_link_get_type(void);

GtkWidget* zathura_gtk_annotation_link_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_LINK */
