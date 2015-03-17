/* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_OVERLAY
#define ZATHURA_GTK_ANNOTATION_OVERLAY

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "../page.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationOverlay ZathuraAnnotationOverlay;
typedef struct _ZathuraAnnotationOverlayClass ZathuraAnnotationOverlayClass;

#define ZATHURA_TYPE_ANNOTATION_OVERLAY            (zathura_gtk_annotation_overlay_get_type())
#define ZATHURA_ANNOTATION_OVERLAY(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_OVERLAY, ZathuraAnnotationOverlay))
#define ZATHURA_ANNOTATION_OVERLAY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_OVERLAY, ZathuraAnnotationOverlayClass))
#define ZATHURA_IS_ANNOTATION_OVERLAY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_OVERLAY))
#define ZATHURA_IS_ANNOTATION_OVERLAY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_OVERLAY))
#define ZATHURA_ANNOTATION_OVERLAY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_OVERLAY, ZathuraAnnotationOverlayClass))

typedef struct _ZathuraAnnotationOverlayPrivate ZathuraAnnotationOverlayPrivate;

struct _ZathuraAnnotationOverlay {
  GtkBin parent;
  ZathuraAnnotationOverlayPrivate* priv;
};

struct _ZathuraAnnotationOverlayClass {
  GtkBinClass parent_class;
};


GType zathura_gtk_annotation_overlay_get_type(void);

GtkWidget* zathura_gtk_annotation_overlay_new(ZathuraPage* page);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_OVERLAY */
