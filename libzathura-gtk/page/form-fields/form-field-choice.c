 /* See LICENSE file for license and copyright information */

#include "../../macros.h"
#include "form-field-choice.h"

struct _ZathuraFormFieldChoicePrivate {
  zathura_form_field_t* form_field;

  GtkWidget* choice_widget;
  GtkWidget* scrolled_window;
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraFormFieldChoice, zathura_gtk_form_field_choice, GTK_TYPE_BIN)

#define ZATHURA_FORM_FIELD_CHOICE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_FORM_FIELD_CHOICE, \
                               ZathuraFormFieldChoicePrivate))

static void
zathura_gtk_form_field_choice_class_init(ZathuraFormFieldChoiceClass* UNUSED(class))
{
}

static void
zathura_gtk_form_field_choice_init(ZathuraFormFieldChoice* widget)
{
  ZathuraFormFieldChoicePrivate* priv = ZATHURA_FORM_FIELD_CHOICE_GET_PRIVATE(widget);

  priv->form_field  = NULL;
  priv->choice_widget = NULL;

  gtk_widget_add_events(GTK_WIDGET(widget), GDK_BUTTON_PRESS_MASK);
}

GtkWidget*
zathura_gtk_form_field_choice_new(zathura_form_field_t* form_field)
{
  g_return_val_if_fail(form_field != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_FORM_FIELD_CHOICE, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraFormFieldChoicePrivate* priv = ZATHURA_FORM_FIELD_CHOICE_GET_PRIVATE(widget);
  priv->form_field = form_field;

  zathura_form_field_choice_type_t choice_type;
  if (zathura_form_field_choice_get_type(priv->form_field, &choice_type) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  switch (choice_type) {
    case ZATHURA_FORM_FIELD_CHOICE_TYPE_LIST:
      {
        /* Setup combo box with text renderer */
        GtkListStore* list_store = gtk_list_store_new(1, G_TYPE_STRING);
        priv->choice_widget      = gtk_combo_box_new_with_model(GTK_TREE_MODEL(list_store));
        GtkCellRenderer* cell    = gtk_cell_renderer_text_new();
        gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(priv->choice_widget), cell, TRUE);
        gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(priv->choice_widget), cell, "text", 0, NULL);

        /* Add all items */
        GtkTreeIter iter;
        zathura_list_t* choice_items;
        if (zathura_form_field_choice_get_items(priv->form_field, &choice_items) != ZATHURA_ERROR_OK) {
          return NULL;
        }

        zathura_form_field_choice_item_t* choice_item;
        ZATHURA_LIST_FOREACH(choice_item, choice_items) {
          char* name;
          if (zathura_form_field_choice_item_get_name(choice_item, &name) != ZATHURA_ERROR_OK) {
            continue;
          }

          bool is_selected;
          if (zathura_form_field_choice_item_is_selected(choice_item, &is_selected) != ZATHURA_ERROR_OK) {
            continue;
          }

          gtk_list_store_append(list_store, &iter);
          gtk_list_store_set(list_store, &iter, 0, name, -1);

          if (is_selected == true) {
            gtk_combo_box_set_active_iter(GTK_COMBO_BOX(priv->choice_widget), &iter);
          }
        }

        /* Add widget to container */
        gtk_container_add(GTK_CONTAINER(widget), priv->choice_widget);
      }
      break;
    case ZATHURA_FORM_FIELD_CHOICE_TYPE_COMBO:
      {
      }
      break;
  }

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}
