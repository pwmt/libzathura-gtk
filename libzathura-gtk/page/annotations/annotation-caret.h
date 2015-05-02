 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_CARET
#define ZATHURA_GTK_ANNOTATION_CARET

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationCaret ZathuraAnnotationCaret;
typedef struct _ZathuraAnnotationCaretClass ZathuraAnnotationCaretClass;

#define ZATHURA_TYPE_ANNOTATION_CARET            (zathura_gtk_annotation_caret_get_type())
#define ZATHURA_ANNOTATION_CARET(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_CARET, ZathuraAnnotationCaret))
#define ZATHURA_ANNOTATION_CARET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_CARET, ZathuraAnnotationCaretClass))
#define ZATHURA_IS_ANNOTATION_CARET(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_CARET))
#define ZATHURA_IS_ANNOTATION_CARET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_CARET))
#define ZATHURA_ANNOTATION_CARET_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_CARET, ZathuraAnnotationCaretClass))

typedef struct _ZathuraAnnotationCaretPrivate ZathuraAnnotationCaretPrivate;

struct _ZathuraAnnotationCaret {
  GtkDrawingArea parent;
  ZathuraAnnotationCaretPrivate* priv;
};

struct _ZathuraAnnotationCaretClass {
  GtkDrawingAreaClass parent_class;
};


GType zathura_gtk_annotation_caret_get_type(void);

GtkWidget* zathura_gtk_annotation_caret_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_CARET */
