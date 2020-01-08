 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_FILE_ATTACHMENT
#define ZATHURA_GTK_ANNOTATION_FILE_ATTACHMENT

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"
#include "utils.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationFileAttachment ZathuraAnnotationFileAttachment;
typedef struct _ZathuraAnnotationFileAttachmentClass ZathuraAnnotationFileAttachmentClass;

#define ZATHURA_TYPE_ANNOTATION_FILE_ATTACHMENT            (zathura_gtk_annotation_file_attachment_get_type())
#define ZATHURA_ANNOTATION_FILE_ATTACHMENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_FILE_ATTACHMENT, ZathuraAnnotationFileAttachment))
#define ZATHURA_ANNOTATION_FILE_ATTACHMENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_FILE_ATTACHMENT, ZathuraAnnotationFileAttachmentClass))
#define ZATHURA_IS_ANNOTATION_FILE_ATTACHMENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_FILE_ATTACHMENT))
#define ZATHURA_IS_ANNOTATION_FILE_ATTACHMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_FILE_ATTACHMENT))
#define ZATHURA_ANNOTATION_FILE_ATTACHMENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_FILE_ATTACHMENT, ZathuraAnnotationFileAttachmentClass))

typedef struct _ZathuraAnnotationFileAttachmentPrivate ZathuraAnnotationFileAttachmentPrivate;

struct _ZathuraAnnotationFileAttachment {
  ZathuraAnnotation parent;
  ZathuraAnnotationFileAttachmentPrivate* priv;
};

struct _ZathuraAnnotationFileAttachmentClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_file_attachment_get_type(void);

GtkWidget* zathura_gtk_annotation_file_attachment_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_FILE_ATTACHMENT */
