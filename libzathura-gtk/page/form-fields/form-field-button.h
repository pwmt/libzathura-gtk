 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_FORM_FIELD_BUTTON
#define ZATHURA_GTK_FORM_FIELD_BUTTON

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

G_BEGIN_DECLS

typedef struct _ZathuraFormFieldButton ZathuraFormFieldButton;
typedef struct _ZathuraFormFieldButtonClass ZathuraFormFieldButtonClass;

#define ZATHURA_TYPE_FORM_FIELD_BUTTON            (zathura_gtk_form_field_button_get_type())
#define ZATHURA_FORM_FIELD_BUTTON(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_FORM_FIELD_BUTTON, ZathuraFormFieldButton))
#define ZATHURA_FORM_FIELD_BUTTON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_FORM_FIELD_BUTTON, ZathuraFormFieldButtonClass))
#define ZATHURA_IS_FORM_FIELD_BUTTON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_FORM_FIELD_BUTTON))
#define ZATHURA_IS_FORM_FIELD_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_FORM_FIELD_BUTTON))
#define ZATHURA_FORM_FIELD_BUTTON_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_FORM_FIELD_BUTTON, ZathuraFormFieldButtonClass))

typedef struct _ZathuraFormFieldButtonPrivate ZathuraFormFieldButtonPrivate;

struct _ZathuraFormFieldButton {
  GtkDrawingArea parent;
  ZathuraFormFieldButtonPrivate* priv;
};

struct _ZathuraFormFieldButtonClass {
  GtkDrawingAreaClass parent_class;
};


GType zathura_gtk_form_field_button_get_type(void);

GtkWidget* zathura_gtk_form_field_button_new(zathura_form_field_t* button);

G_END_DECLS

#endif /* ZATHURA_GTK_FORM_FIELD_BUTTON */
