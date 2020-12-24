/* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_FORM_FIELD_EDITOR
#define ZATHURA_GTK_FORM_FIELD_EDITOR

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "../page.h"

G_BEGIN_DECLS

typedef struct _ZathuraFormFieldEditor ZathuraFormFieldEditor;
typedef struct _ZathuraFormFieldEditorClass ZathuraFormFieldEditorClass;

#define ZATHURA_TYPE_FORM_FIELD_EDITOR            (zathura_gtk_form_field_editor_get_type())
#define ZATHURA_FORM_FIELD_EDITOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_FORM_FIELD_EDITOR, ZathuraFormFieldEditor))
#define ZATHURA_FORM_FIELD_EDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_FORM_FIELD_EDITOR, ZathuraFormFieldEditorClass))
#define ZATHURA_IS_FORM_FIELD_EDITOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_FORM_FIELD_EDITOR))
#define ZATHURA_IS_FORM_FIELD_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_FORM_FIELD_EDITOR))
#define ZATHURA_FORM_FIELD_EDITOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_FORM_FIELD_EDITOR, ZathuraFormFieldEditorClass))

typedef struct _ZathuraFormFieldEditorPrivate ZathuraFormFieldEditorPrivate;

struct _ZathuraFormFieldEditor {
  GtkWidget parent;
  ZathuraFormFieldEditorPrivate* priv;
};

struct _ZathuraFormFieldEditorClass {
  GtkWidgetClass parent_class;
};


GType zathura_gtk_form_field_editor_get_type(void);

GtkWidget* zathura_gtk_form_field_editor_new(ZathuraPage* page);

G_END_DECLS

#endif /* ZATHURA_GTK_FORM_FIELD_EDITOR */
