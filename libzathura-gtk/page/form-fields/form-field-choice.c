
#include <string.h>

#include "../../macros.h"
#include "form-field-choice.h"

static GtkWidget *form_field_choice_combo_new(GtkWidget *form_field_widget);
static void cb_draw_button(GtkDrawingArea *area, cairo_t *cairo, int width,
                           int height, gpointer data);
static void cb_form_field_choice_combo_changed(GtkComboBox *widget,
                                               GtkWidget *form_field_widget);

static GtkWidget *form_field_choice_list_new(GtkWidget *form_field_widget);
static void cb_form_field_choice_list_changed(GtkTreeSelection *selection,
                                              zathura_form_field_t *form_field);
static void cb_form_field_choice_list_destroy(GtkTreeSelection *selection,
                                              GtkWidget *widget);

static void cb_form_field_choice_pressed_event(GtkGestureClick *UNUSED(gesture),
                                               guint n_press, double x,
                                               double y,
                                               GtkWidget *form_field_widget);
static void reset_to_drawing_area(GtkWidget *widget);

#define RGB_TO_CAIRO(r, g, b) (r) / 255.0, (g) / 255.0, (b) / 255.0

struct _ZathuraFormFieldChoicePrivate {
  zathura_form_field_t *form_field;

  GtkWidget *overlay;

  struct {
    GtkWidget *drawing_area;
    GtkWidget *choice_widget;
  } layer;
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraFormFieldChoice,
                           zathura_gtk_form_field_choice, GTK_TYPE_WIDGET)

static void
zathura_gtk_form_field_choice_class_init(ZathuraFormFieldChoiceClass *class) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

static void zathura_gtk_form_field_choice_init(ZathuraFormFieldChoice *widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(widget);

  priv->form_field = NULL;

  priv->overlay = NULL;
  priv->layer.choice_widget = NULL;
  priv->layer.drawing_area = NULL;
}

GtkWidget *zathura_gtk_form_field_choice_new(zathura_form_field_t *form_field) {
  g_return_val_if_fail(form_field != NULL, NULL);

  GObject *widget = g_object_new(ZATHURA_TYPE_FORM_FIELD_CHOICE, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(widget));
  priv->form_field = form_field;

  zathura_form_field_choice_type_t choice_type;
  if (zathura_form_field_choice_get_type(priv->form_field, &choice_type) !=
      ZATHURA_ERROR_OK) {
    return NULL;
  }

  switch (choice_type) {
  case ZATHURA_FORM_FIELD_CHOICE_TYPE_COMBO: {
    priv->layer.choice_widget = form_field_choice_combo_new(GTK_WIDGET(widget));
  } break;
  case ZATHURA_FORM_FIELD_CHOICE_TYPE_LIST: {
    priv->layer.choice_widget = form_field_choice_list_new(GTK_WIDGET(widget));
  } break;
  }

  if (priv->layer.choice_widget == NULL) {
    return NULL;
  }

  /* Setup drawing_area */
  priv->layer.drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(priv->layer.drawing_area),
                                 cb_draw_button, widget, NULL);

  GtkGesture *gesture = gtk_gesture_click_new();
  gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 1);
  g_signal_connect(gesture, "pressed",
                   G_CALLBACK(cb_form_field_choice_pressed_event), NULL);
  gtk_widget_add_controller(priv->layer.drawing_area,
                            GTK_EVENT_CONTROLLER(gesture));

  /* Setup overlay */
  priv->overlay = gtk_overlay_new();
  gtk_overlay_set_child(GTK_OVERLAY(priv->overlay), priv->layer.drawing_area);
  gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay),
                          priv->layer.choice_widget);

  gtk_widget_hide(priv->layer.choice_widget);

  /* Add widget to container */
  gtk_widget_set_parent(priv->overlay, GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static GtkWidget *form_field_choice_combo_new(GtkWidget *form_field_widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(form_field_widget));
  GtkWidget *widget = NULL;

  /* Setup combo box with text renderer */
  GtkListStore *list_store = gtk_list_store_new(1, G_TYPE_STRING);

  /* If true the box also includes an editable text box */
  bool is_editable;
  if (zathura_form_field_choice_is_editable(priv->form_field, &is_editable) !=
      ZATHURA_ERROR_OK) {
    return NULL;
  }

  if (is_editable == true) {
    widget = gtk_combo_box_new_with_model_and_entry(GTK_TREE_MODEL(list_store));
  } else {
    widget = gtk_combo_box_new_with_model(GTK_TREE_MODEL(list_store));
  }

  g_signal_connect(widget, "changed",
                   G_CALLBACK(cb_form_field_choice_combo_changed),
                   form_field_widget);

  GtkCellRenderer *cell = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(widget), cell, TRUE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(widget), cell, "text", 0,
                                 NULL);

  /* Add all items */
  GtkTreeIter iter;
  zathura_list_t *choice_items;
  if (zathura_form_field_choice_get_items(priv->form_field, &choice_items) !=
      ZATHURA_ERROR_OK) {
    return NULL;
  }

  zathura_form_field_choice_item_t *choice_item;
  ZATHURA_LIST_FOREACH(choice_item, choice_items) {
    char *name;
    if (zathura_form_field_choice_item_get_name(choice_item, &name) !=
        ZATHURA_ERROR_OK) {
      continue;
    }

    bool is_selected;
    if (zathura_form_field_choice_item_is_selected(choice_item, &is_selected) !=
        ZATHURA_ERROR_OK) {
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

static void cb_form_field_choice_combo_changed(GtkComboBox *widget,
                                               GtkWidget *form_field_widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(form_field_widget));
  GtkTreeIter iter;
  const char *name = NULL;

  if (gtk_combo_box_get_active_iter(widget, &iter) == true) {
    GtkTreeModel *model = gtk_combo_box_get_model(widget);
    gtk_tree_model_get(model, &iter, 0, &name, -1);
  } else {
    // /TODO
    // GtkWidget* entry = gtk_bin_get_child(GTK_BIN(widget));
    // if (entry != NULL) {
    // /TODO
    // name = gtk_entry_get_text(GTK_ENTRY(entry));
    // }
  }

  if (name == NULL) {
    return;
  }

  zathura_list_t *choice_items;
  if (zathura_form_field_choice_get_items(priv->form_field, &choice_items) !=
      ZATHURA_ERROR_OK) {
    return;
  }

  zathura_form_field_choice_item_t *choice_item;
  ZATHURA_LIST_FOREACH(choice_item, choice_items) {
    char *item_name;
    if (zathura_form_field_choice_item_get_name(choice_item, &item_name) !=
        ZATHURA_ERROR_OK) {
      continue;
    }

    if (strcmp(name, item_name) == 0) {
      if (zathura_form_field_choice_item_select(choice_item) !=
          ZATHURA_ERROR_OK) {
        continue;
      }
    } else {
      if (zathura_form_field_choice_item_deselect(choice_item) !=
          ZATHURA_ERROR_OK) {
        continue;
      }
    }
  }

  if (zathura_form_field_save(priv->form_field) != ZATHURA_ERROR_OK) {
    return;
  }

  reset_to_drawing_area(form_field_widget);
}

static GtkWidget *form_field_choice_list_new(GtkWidget *form_field_widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(form_field_widget));

  /* Setup model */
  GtkListStore *list_store =
      gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  /* Setup widget */
  GtkWidget *tree_view =
      gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view), FALSE);

  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

  gtk_tree_view_insert_column_with_attributes(
      GTK_TREE_VIEW(tree_view), -1, "Name", renderer, "text", 0, NULL);

  GtkTreeSelection *selection =
      gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

  g_signal_connect(selection, "changed",
                   G_CALLBACK(cb_form_field_choice_list_changed),
                   priv->form_field);

  g_signal_connect_after(selection, "changed",
                         G_CALLBACK(cb_form_field_choice_list_destroy),
                         form_field_widget);

  bool is_multiselect;
  if (zathura_form_field_choice_is_multiselect(
          priv->form_field, &is_multiselect) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  if (is_multiselect == true) {
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
  } else {
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
  }

  /* Add all items to model */
  zathura_list_t *choice_items;
  if (zathura_form_field_choice_get_items(priv->form_field, &choice_items) !=
      ZATHURA_ERROR_OK) {
    return NULL;
  }

  GtkTreeIter iter;
  zathura_form_field_choice_item_t *choice_item;
  ZATHURA_LIST_FOREACH(choice_item, choice_items) {
    char *name;
    if (zathura_form_field_choice_item_get_name(choice_item, &name) !=
        ZATHURA_ERROR_OK) {
      continue;
    }

    bool is_selected;
    if (zathura_form_field_choice_item_is_selected(choice_item, &is_selected) !=
        ZATHURA_ERROR_OK) {
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
cb_form_field_choice_list_changed(GtkTreeSelection *selection,
                                  zathura_form_field_t *form_field) {
  GtkTreeView *tree_view = gtk_tree_selection_get_tree_view(selection);
  GtkTreeModel *tree_model = gtk_tree_view_get_model(tree_view);

  GtkTreeIter iter;
  gtk_tree_model_get_iter_first(tree_model, &iter);

  do {
    zathura_form_field_choice_item_t *choice_item;
    gtk_tree_model_get(tree_model, &iter, 1, &choice_item, -1);

    if (gtk_tree_selection_iter_is_selected(selection, &iter) == TRUE) {
      if (zathura_form_field_choice_item_select(choice_item) !=
          ZATHURA_ERROR_OK) {
        continue;
      }
    } else {
      if (zathura_form_field_choice_item_deselect(choice_item) !=
          ZATHURA_ERROR_OK) {
        continue;
      }
    }
  } while (gtk_tree_model_iter_next(tree_model, &iter) == TRUE);

  if (zathura_form_field_save(form_field) != ZATHURA_ERROR_OK) {
    return;
  }
}

static void
cb_form_field_choice_list_destroy(GtkTreeSelection *UNUSED(selection),
                                  GtkWidget *widget) {
  reset_to_drawing_area(widget);
}

static gboolean set_back_to_drawing_area(GtkWidget *widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(widget));

  /* Don't reset if current visible widget is the drawing_area already */
  GtkWidget *child = gtk_widget_get_first_child(widget);
  if (child == priv->layer.drawing_area) {
    return FALSE;
  }

  /* Remove child */
  // g_object_ref(child);
  // gtk_container_remove(GTK_CONTAINER(widget), child);

  /* Add drawing_area */
  // gtk_container_add(GTK_CONTAINER(widget), priv->drawing_area);

  return FALSE;
}

static void reset_to_drawing_area(GtkWidget *widget) {
  g_idle_add((GSourceFunc)set_back_to_drawing_area, widget);
}

static void cb_form_field_choice_pressed_event(GtkGestureClick *UNUSED(gesture),
                                               guint n_press, double x,
                                               double y,
                                               GtkWidget *form_field_widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(form_field_widget));

  gtk_widget_show(priv->layer.choice_widget);
}

static void cb_draw_button(GtkDrawingArea *area, cairo_t *cairo, int width,
                           int height, gpointer data) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(data);

  cairo_save(cairo);

  /* Draw rectangle */
  cairo_set_source_rgba(cairo, RGB_TO_CAIRO(75, 181, 193), 0.5);
  cairo_rectangle(cairo, 0, 0, width, height);
  cairo_fill(cairo);

  cairo_restore(cairo);

  gint device_scale = gtk_widget_get_scale_factor(GTK_WIDGET(area));
  cairo_save(cairo);

  /* Create image surface */
  cairo_surface_t *image_surface = cairo_image_surface_create(
      CAIRO_FORMAT_RGB24, width * device_scale, height * device_scale);
  if (image_surface == NULL) {
    return;
  }

  cairo_surface_set_device_scale(image_surface, device_scale, device_scale);

  cairo_t *image_cairo = cairo_create(image_surface);
  if (image_cairo == NULL) {
    cairo_surface_destroy(image_surface);
    return;
  }

  /* Scale */
  cairo_save(image_cairo);

  /* Render page */
  if (zathura_form_field_render_cairo(priv->form_field, image_cairo,
                                      device_scale) != ZATHURA_ERROR_OK) {
    cairo_restore(image_cairo);
    return;
  }

  cairo_restore(image_cairo);
  cairo_destroy(image_cairo);

  cairo_set_source_surface(cairo, image_surface, 0, 0);
  cairo_paint(cairo);
  cairo_restore(cairo);

  /* Clean-up */
  cairo_surface_destroy(image_surface);
}
