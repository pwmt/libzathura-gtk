/* See LICENSE file for license and copyright information */

#include <math.h>

#include "editor.h"
#include "../internal.h"
#include "../utils.h"
#include "../../macros.h"

#include "form-field-button.h"
#include "form-field-choice.h"
#include "form-field-text.h"

struct _ZathuraFormFieldEditorPrivate {
  ZathuraPage* page;
  zathura_list_t* form_fields;
};

typedef struct form_field_widget_mapping_s {
  GtkWidget* widget;
  zathura_rectangle_t position;
} form_field_widget_mapping_t;

static void create_widgets(GtkWidget* editor);
static void zathura_gtk_form_field_editor_size_allocate(GtkWidget* widget, GdkRectangle* allocation);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraFormFieldEditor, zathura_gtk_form_field_editor, GTK_TYPE_FIXED)

#define ZATHURA_FORM_FIELD_EDITOR_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_FORM_FIELD_EDITOR, \
                               ZathuraFormFieldEditorPrivate))

static void
zathura_gtk_form_field_editor_class_init(ZathuraFormFieldEditorClass* class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
  widget_class->size_allocate = zathura_gtk_form_field_editor_size_allocate;
}

static void
zathura_gtk_form_field_editor_init(ZathuraFormFieldEditor* widget)
{
  ZathuraFormFieldEditorPrivate* priv = ZATHURA_FORM_FIELD_EDITOR_GET_PRIVATE(widget);

  priv->form_fields = NULL;
  priv->page        = NULL;
}

GtkWidget*
zathura_gtk_form_field_editor_new(ZathuraPage* page)
{
  GObject* widget = g_object_new(ZATHURA_TYPE_FORM_FIELD_EDITOR, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraFormFieldEditorPrivate* priv = ZATHURA_FORM_FIELD_EDITOR_GET_PRIVATE(widget);

  priv->page = page;

  return GTK_WIDGET(widget);
}

static void
create_widgets(GtkWidget* editor)
{
  ZathuraFormFieldEditorPrivate* priv = ZATHURA_FORM_FIELD_EDITOR_GET_PRIVATE(editor);
  ZathuraPagePrivate* page_priv = ZATHURA_PAGE_GET_PRIVATE(priv->page);

  zathura_list_t* form_fields;
  if (zathura_page_get_form_fields(page_priv->page, &form_fields) != ZATHURA_ERROR_OK) {
    return;
  }

  zathura_form_field_mapping_t* form_field_mapping;
  ZATHURA_LIST_FOREACH(form_field_mapping, form_fields) {
    zathura_form_field_type_t form_field_type = ZATHURA_FORM_FIELD_UNKNOWN;
    if (zathura_form_field_get_type(form_field_mapping->form_field, &form_field_type) != ZATHURA_ERROR_OK) {
      continue;
    }

    GtkWidget* form_field_widget = NULL;
    switch (form_field_type) {
      case ZATHURA_FORM_FIELD_BUTTON:
        form_field_widget = zathura_gtk_form_field_button_new(form_field_mapping->form_field);
        break;
      case ZATHURA_FORM_FIELD_TEXT:
        form_field_widget = zathura_gtk_form_field_text_new(form_field_mapping->form_field);
        break;
      case ZATHURA_FORM_FIELD_CHOICE:
        form_field_widget = zathura_gtk_form_field_choice_new(form_field_mapping->form_field);
        break;
      default:
        break;
    }

    if (form_field_widget != NULL) {
      /* Create new mapping */
      form_field_widget_mapping_t* mapping = g_malloc0(sizeof(form_field_widget_mapping_t));

      mapping->position = form_field_mapping->position;
      mapping->widget   = form_field_widget;

      priv->form_fields = zathura_list_append(priv->form_fields, mapping);

      /* Setup initial position of widgets */
      zathura_rectangle_t position = calculate_correct_position(priv->page, form_field_mapping->position);
      const unsigned int width  = ceil(position.p2.x) - floor(position.p1.x);
      const unsigned int height = ceil(position.p2.y) - floor(position.p1.y);

      gtk_fixed_put(GTK_FIXED(editor), form_field_widget, position.p1.x, position.p1.y);
      gtk_widget_set_size_request(form_field_widget, width, height);
      gtk_widget_show(form_field_widget);
    }
  }
}

static void
zathura_gtk_form_field_editor_size_allocate(GtkWidget* widget, GdkRectangle* allocation)
{
  ZathuraFormFieldEditorPrivate* priv = ZATHURA_FORM_FIELD_EDITOR_GET_PRIVATE(widget);

  if (priv->form_fields == NULL) {
    create_widgets(widget);
  }

  form_field_widget_mapping_t* form_field_mapping;
  ZATHURA_LIST_FOREACH(form_field_mapping, priv->form_fields) {
      zathura_rectangle_t position = calculate_correct_position(priv->page, form_field_mapping->position);
      const unsigned int width  = ceil(position.p2.x) - floor(position.p1.x);
      const unsigned int height = ceil(position.p2.y) - floor(position.p1.y);

      gtk_fixed_move(GTK_FIXED(widget), form_field_mapping->widget, position.p1.x, position.p1.y);
      gtk_widget_set_size_request(form_field_mapping->widget, width, height);
  }

  GTK_WIDGET_CLASS(zathura_gtk_form_field_editor_parent_class)->size_allocate(widget, allocation);
}
