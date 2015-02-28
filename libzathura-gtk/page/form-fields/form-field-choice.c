 /* See LICENSE file for license and copyright information */

#include <string.h>

#include "../../macros.h"
#include "form-field-choice.h"

static GtkWidget* form_field_choice_combo_new(zathura_form_field_t* form_field);
static void cb_form_field_choice_combo_changed(GtkComboBox* widget, zathura_form_field_t* form_field);
static GtkWidget* form_field_choice_list_new(zathura_form_field_t* form_field);
static void cb_form_field_choice_list_changed(GtkTreeSelection* selection, zathura_form_field_t* form_field);

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
    case ZATHURA_FORM_FIELD_CHOICE_TYPE_COMBO:
      {
        priv->choice_widget = form_field_choice_combo_new(priv->form_field);
      }
      break;
    case ZATHURA_FORM_FIELD_CHOICE_TYPE_LIST:
      {
        priv->choice_widget = form_field_choice_list_new(priv->form_field);
      }
      break;
  }

  /* Add widget to container */
  gtk_container_add(GTK_CONTAINER(widget), priv->choice_widget);

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static GtkWidget*
form_field_choice_combo_new(zathura_form_field_t* form_field)
{
  GtkWidget* widget = NULL;

  /* Setup combo box with text renderer */
  GtkListStore* list_store = gtk_list_store_new(1, G_TYPE_STRING);

  /* If true the box also includes an editable text box */
  bool is_editable;
  if (zathura_form_field_choice_is_editable(form_field, &is_editable) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  if (is_editable == true) {
    widget = gtk_combo_box_new_with_model_and_entry(GTK_TREE_MODEL(list_store));
  } else {
    widget = gtk_combo_box_new_with_model(GTK_TREE_MODEL(list_store));
  }

  g_signal_connect(widget, "changed",
      G_CALLBACK(cb_form_field_choice_combo_changed), form_field);

  GtkCellRenderer* cell = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(widget), cell, TRUE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(widget), cell, "text", 0, NULL);

  /* Add all items */
  GtkTreeIter iter;
  zathura_list_t* choice_items;
  if (zathura_form_field_choice_get_items(form_field, &choice_items) != ZATHURA_ERROR_OK) {
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
      gtk_combo_box_set_active_iter(GTK_COMBO_BOX(widget), &iter);
    }
  }

  return widget;
}

static void
cb_form_field_choice_combo_changed(GtkComboBox* widget, zathura_form_field_t* form_field)
{
  GtkTreeIter iter;
  const char* name = NULL;

  if (gtk_combo_box_get_active_iter(widget, &iter) == true) {
    GtkTreeModel* model = gtk_combo_box_get_model(widget);
    gtk_tree_model_get(model, &iter, 0, &name, -1);
  } else {
    GtkWidget* entry = gtk_bin_get_child(GTK_BIN(widget));
    if (entry != NULL) {
      name = gtk_entry_get_text(GTK_ENTRY(entry));
    }
  }

  if (name == NULL) {
    return;
  }

  zathura_list_t* choice_items;
  if (zathura_form_field_choice_get_items(form_field, &choice_items) != ZATHURA_ERROR_OK) {
    return;
  }

  zathura_form_field_choice_item_t* choice_item;
  ZATHURA_LIST_FOREACH(choice_item, choice_items) {
    char* item_name;
    if (zathura_form_field_choice_item_get_name(choice_item, &item_name) != ZATHURA_ERROR_OK) {
      continue;
    }

    if (strcmp(name, item_name) == 0) {
      if (zathura_form_field_choice_item_select(choice_item) != ZATHURA_ERROR_OK) {
        continue;
      }
    } else {
      if (zathura_form_field_choice_item_deselect(choice_item) != ZATHURA_ERROR_OK) {
        continue;
      }
    }
  }
}

static GtkWidget*
form_field_choice_list_new(zathura_form_field_t* form_field)
{
  /* Setup model */
  GtkListStore* list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  /* Setup widget */
  GtkWidget* tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view), FALSE);

  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();

  gtk_tree_view_insert_column_with_attributes(
      GTK_TREE_VIEW (tree_view),
      -1,
      "Name",
      renderer,
      "text",
      0,
      NULL);

  GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

  g_signal_connect(selection, "changed",
      G_CALLBACK(cb_form_field_choice_list_changed), form_field);

  bool is_multiselect;
  if (zathura_form_field_choice_is_multiselect(form_field, &is_multiselect) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  if (is_multiselect == true) {
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
  } else {
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
  }

  /* Add all items to model */
  zathura_list_t* choice_items;
  if (zathura_form_field_choice_get_items(form_field, &choice_items) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  GtkTreeIter iter;
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
    gtk_list_store_set(list_store, &iter, 0, name, 1, choice_item, -1);

    if (is_selected == true) {
      gtk_tree_selection_select_iter(selection, &iter);
    }
  }

  return tree_view;
}

static void
cb_form_field_choice_list_changed(GtkTreeSelection* selection, zathura_form_field_t* UNUSED(form_field))
{
  GtkTreeView* tree_view   = gtk_tree_selection_get_tree_view(selection);
  GtkTreeModel* tree_model = gtk_tree_view_get_model(tree_view);

  GtkTreeIter iter;
  gtk_tree_model_get_iter_first(tree_model, &iter);

  do {
    zathura_form_field_choice_item_t* choice_item;
    gtk_tree_model_get(tree_model, &iter, 1, &choice_item, -1);

    if (gtk_tree_selection_iter_is_selected(selection, &iter) == TRUE) {
      if (zathura_form_field_choice_item_select(choice_item) != ZATHURA_ERROR_OK) {
        continue;
      }
    } else {
      if (zathura_form_field_choice_item_deselect(choice_item) != ZATHURA_ERROR_OK) {
        continue;
      }
    }
  } while (gtk_tree_model_iter_next(tree_model, &iter) == TRUE);
}
