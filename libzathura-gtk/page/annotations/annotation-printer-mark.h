 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_PRINTER_MARK
#define ZATHURA_GTK_ANNOTATION_PRINTER_MARK

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"
#include "utils.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationPrinterMark ZathuraAnnotationPrinterMark;
typedef struct _ZathuraAnnotationPrinterMarkClass ZathuraAnnotationPrinterMarkClass;

#define ZATHURA_TYPE_ANNOTATION_PRINTER_MARK            (zathura_gtk_annotation_printer_mark_get_type())
#define ZATHURA_ANNOTATION_PRINTER_MARK(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_PRINTER_MARK, ZathuraAnnotationPrinterMark))
#define ZATHURA_ANNOTATION_PRINTER_MARK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_PRINTER_MARK, ZathuraAnnotationPrinterMarkClass))
#define ZATHURA_IS_ANNOTATION_PRINTER_MARK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_PRINTER_MARK))
#define ZATHURA_IS_ANNOTATION_PRINTER_MARK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_PRINTER_MARK))
#define ZATHURA_ANNOTATION_PRINTER_MARK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_PRINTER_MARK, ZathuraAnnotationPrinterMarkClass))

typedef struct _ZathuraAnnotationPrinterMarkPrivate ZathuraAnnotationPrinterMarkPrivate;

struct _ZathuraAnnotationPrinterMark {
  ZathuraAnnotation parent;
  ZathuraAnnotationPrinterMarkPrivate* priv;
};

struct _ZathuraAnnotationPrinterMarkClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_printer_mark_get_type(void);

GtkWidget* zathura_gtk_annotation_printer_mark_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_PRINTER_MARK */
