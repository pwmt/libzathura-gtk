 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_HIGHLIGHT
#define ZATHURA_GTK_ANNOTATION_HIGHLIGHT

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationHighlight ZathuraAnnotationHighlight;
typedef struct _ZathuraAnnotationHighlightClass ZathuraAnnotationHighlightClass;

#define ZATHURA_TYPE_ANNOTATION_HIGHLIGHT            (zathura_gtk_annotation_highlight_get_type())
#define ZATHURA_ANNOTATION_HIGHLIGHT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_HIGHLIGHT, ZathuraAnnotationHighlight))
#define ZATHURA_ANNOTATION_HIGHLIGHT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_HIGHLIGHT, ZathuraAnnotationHighlightClass))
#define ZATHURA_IS_ANNOTATION_HIGHLIGHT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_HIGHLIGHT))
#define ZATHURA_IS_ANNOTATION_HIGHLIGHT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_HIGHLIGHT))
#define ZATHURA_ANNOTATION_HIGHLIGHT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_HIGHLIGHT, ZathuraAnnotationHighlightClass))

typedef struct _ZathuraAnnotationHighlightPrivate ZathuraAnnotationHighlightPrivate;

struct _ZathuraAnnotationHighlight {
  GtkDrawingArea parent;
  ZathuraAnnotationHighlightPrivate* priv;
};

struct _ZathuraAnnotationHighlightClass {
  GtkDrawingAreaClass parent_class;
};


GType zathura_gtk_annotation_highlight_get_type(void);

GtkWidget* zathura_gtk_annotation_highlight_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_HIGHLIGHT */
