 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_FORM_FIELD_CHOICE
#define ZATHURA_GTK_FORM_FIELD_CHOICE

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

G_BEGIN_DECLS

typedef struct _ZathuraFormFieldChoice ZathuraFormFieldChoice;
typedef struct _ZathuraFormFieldChoiceClass ZathuraFormFieldChoiceClass;

#define ZATHURA_TYPE_FORM_FIELD_CHOICE            (zathura_gtk_form_field_choice_get_type())
#define ZATHURA_FORM_FIELD_CHOICE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_FORM_FIELD_CHOICE, ZathuraFormFieldChoice))
#define ZATHURA_FORM_FIELD_CHOICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_FORM_FIELD_CHOICE, ZathuraFormFieldChoiceClass))
#define ZATHURA_IS_FORM_FIELD_CHOICE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_FORM_FIELD_CHOICE))
#define ZATHURA_IS_FORM_FIELD_CHOICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_FORM_FIELD_CHOICE))
#define ZATHURA_FORM_FIELD_CHOICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_FORM_FIELD_CHOICE, ZathuraFormFieldChoiceClass))

typedef struct _ZathuraFormFieldChoicePrivate ZathuraFormFieldChoicePrivate;

struct _ZathuraFormFieldChoice {
  GtkWidget parent;
  ZathuraFormFieldChoicePrivate* priv;
};

struct _ZathuraFormFieldChoiceClass {
  GtkWidgetClass parent_class;
};


GType zathura_gtk_form_field_choice_get_type(void);

GtkWidget* zathura_gtk_form_field_choice_new(zathura_form_field_t* form_field);

G_END_DECLS

#endif /* ZATHURA_GTK_FORM_FIELD_CHOICE */
