 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_FORM_FIELD_TEXT
#define ZATHURA_GTK_FORM_FIELD_TEXT

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

G_BEGIN_DECLS

typedef struct _ZathuraFormFieldText ZathuraFormFieldText;
typedef struct _ZathuraFormFieldTextClass ZathuraFormFieldTextClass;

#define ZATHURA_TYPE_FORM_FIELD_TEXT            (zathura_gtk_form_field_text_get_type())
#define ZATHURA_FORM_FIELD_TEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_FORM_FIELD_TEXT, ZathuraFormFieldText))
#define ZATHURA_FORM_FIELD_TEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_FORM_FIELD_TEXT, ZathuraFormFieldTextClass))
#define ZATHURA_IS_FORM_FIELD_TEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_FORM_FIELD_TEXT))
#define ZATHURA_IS_FORM_FIELD_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_FORM_FIELD_TEXT))
#define ZATHURA_FORM_FIELD_TEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_FORM_FIELD_TEXT, ZathuraFormFieldTextClass))

typedef struct _ZathuraFormFieldTextPrivate ZathuraFormFieldTextPrivate;

struct _ZathuraFormFieldText {
  GtkBin parent;
  ZathuraFormFieldTextPrivate* priv;
};

struct _ZathuraFormFieldTextClass {
  GtkBinClass parent_class;
};


GType zathura_gtk_form_field_text_get_type(void);

GtkWidget* zathura_gtk_form_field_text_new(zathura_form_field_t* form_field);

G_END_DECLS

#endif /* ZATHURA_GTK_FORM_FIELD_TEXT */
