
#include <string.h>

#include "../../macros.h"
#include "form-field-choice.h"

static GtkWidget *form_field_choice_combo_new(GtkWidget *form_field_widget);
static void cb_draw_button(GtkDrawingArea *area, cairo_t *cairo, int width,
                           int height, gpointer data);
static void cb_form_field_choice_combo_changed(GObject *dropdown,
                                               GParamSpec *pspec,
                                               GtkWidget *form_field_widget);

static GtkWidget *form_field_choice_list_new(GtkWidget *form_field_widget);
static void cb_form_field_choice_list_changed(GtkSelectionModel *model,
                                              guint position, guint n_items,
                                              GtkWidget *form_field_widget);
static void cb_form_field_choice_list_item_setup(GtkSignalListItemFactory *factory,
                                                 GtkListItem *list_item,
                                                 gpointer data);
static void cb_form_field_choice_list_item_bind(GtkSignalListItemFactory *factory,
                                                GtkListItem *list_item,
                                                gpointer data);

static void cb_form_field_choice_pressed_event(GtkGestureClick *UNUSED(gesture),
                                               guint n_press, double x,
                                               double y,
                                               GtkWidget *form_field_widget);
static void reset_to_drawing_area(GtkWidget *widget);
static void zathura_gtk_form_field_choice_set_property(GObject *object,
                                                        guint prop_id,
                                                        const GValue *value,
                                                        GParamSpec *param_spec);
static void zathura_gtk_form_field_choice_get_property(GObject *object,
                                                        guint prop_id,
                                                        GValue *value,
                                                        GParamSpec *param_spec);

#define RGB_TO_CAIRO(r, g, b) (r) / 255.0, (g) / 255.0, (b) / 255.0

struct _ZathuraFormFieldChoicePrivate {
  zathura_form_field_t *form_field;
  double scale;

  GtkWidget *overlay;

  struct {
    GtkWidget *drawing_area;
    GtkWidget *choice_widget;
  } layer;
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraFormFieldChoice,
                           zathura_gtk_form_field_choice, GTK_TYPE_WIDGET)

enum {
  PROP_0,
  PROP_SCALE,
};

static zathura_form_field_choice_item_t *
choice_item_at(zathura_list_t *list, guint index) {
  guint i = 0;
  zathura_form_field_choice_item_t *item;
  ZATHURA_LIST_FOREACH(item, list) {
    if (i++ == index) {
      return item;
    }
  }
  return NULL;
}

static void
zathura_gtk_form_field_choice_class_init(ZathuraFormFieldChoiceClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_form_field_choice_set_property;
  object_class->get_property = zathura_gtk_form_field_choice_get_property;

  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);

  g_object_class_install_property(
      object_class, PROP_SCALE,
      g_param_spec_double("scale", "scale", "The page scale factor", 0.01,
                          100.0, 1.0,
                          G_PARAM_WRITABLE | G_PARAM_READABLE |
                              G_PARAM_STATIC_STRINGS));
}

static void zathura_gtk_form_field_choice_init(ZathuraFormFieldChoice *widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(widget);

  priv->form_field = NULL;
  priv->scale = 1.0;

  priv->overlay = NULL;
  priv->layer.choice_widget = NULL;
  priv->layer.drawing_area = NULL;
}

static void
zathura_gtk_form_field_choice_set_property(GObject *object, guint prop_id,
                                           const GValue *value,
                                           GParamSpec *param_spec) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(object));

  switch (prop_id) {
  case PROP_SCALE:
    priv->scale = g_value_get_double(value);
    if (priv->layer.drawing_area != NULL) {
      gtk_widget_queue_draw(priv->layer.drawing_area);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
zathura_gtk_form_field_choice_get_property(GObject *object, guint prop_id,
                                           GValue *value,
                                           GParamSpec *param_spec) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(object));

  switch (prop_id) {
  case PROP_SCALE:
    g_value_set_double(value, priv->scale);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
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
                   G_CALLBACK(cb_form_field_choice_pressed_event), widget);
  gtk_widget_add_controller(priv->layer.drawing_area,
                            GTK_EVENT_CONTROLLER(gesture));

  /* Setup overlay */
  priv->overlay = gtk_overlay_new();
  gtk_overlay_set_child(GTK_OVERLAY(priv->overlay), priv->layer.drawing_area);
  gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay),
                          priv->layer.choice_widget);

  gtk_widget_set_visible(priv->layer.choice_widget, FALSE);

  /* Add widget to container */
  gtk_widget_set_parent(priv->overlay, GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static GtkWidget *form_field_choice_combo_new(GtkWidget *form_field_widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(form_field_widget));
  GtkWidget *widget = NULL;
  GtkStringList *list_store = gtk_string_list_new(NULL);

  /* If true the box also includes an editable text box */
  bool is_editable;
  if (zathura_form_field_choice_is_editable(priv->form_field, &is_editable) !=
      ZATHURA_ERROR_OK) {
    return NULL;
  }
  (void)is_editable;

  /* Add all items */
  guint selected_index = GTK_INVALID_LIST_POSITION;
  guint index = 0;
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

    gtk_string_list_append(list_store, name);

    if (is_selected == true && selected_index == GTK_INVALID_LIST_POSITION) {
      selected_index = index;
    }

    index++;
  }

  widget = gtk_drop_down_new(G_LIST_MODEL(list_store), NULL);
  g_signal_connect(widget, "notify::selected",
                   G_CALLBACK(cb_form_field_choice_combo_changed),
                   form_field_widget);

  if (selected_index != GTK_INVALID_LIST_POSITION) {
    gtk_drop_down_set_selected(GTK_DROP_DOWN(widget), selected_index);
  }

  return widget;
}

static void cb_form_field_choice_combo_changed(GObject *dropdown,
                                               GParamSpec *UNUSED(pspec),
                                               GtkWidget *form_field_widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(form_field_widget));
  guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown));
  if (selected == GTK_INVALID_LIST_POSITION) {
    return;
  }

  zathura_list_t *choice_items;
  if (zathura_form_field_choice_get_items(priv->form_field, &choice_items) !=
      ZATHURA_ERROR_OK) {
    return;
  }

  guint index = 0;
  zathura_form_field_choice_item_t *choice_item;
  ZATHURA_LIST_FOREACH(choice_item, choice_items) {
    if (index == selected) {
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
    index++;
  }

  if (zathura_form_field_save(priv->form_field) != ZATHURA_ERROR_OK) {
    return;
  }

  reset_to_drawing_area(form_field_widget);
  gtk_widget_queue_draw(priv->layer.drawing_area);
  gtk_widget_queue_draw(form_field_widget);
}

static GtkWidget *form_field_choice_list_new(GtkWidget *form_field_widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(form_field_widget));

  GtkStringList *list_store = gtk_string_list_new(NULL);

  bool is_multiselect;
  if (zathura_form_field_choice_is_multiselect(
          priv->form_field, &is_multiselect) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  /* Add all items to model */
  guint index = 0;
  GArray *selected_indices = g_array_new(FALSE, FALSE, sizeof(guint));
  zathura_list_t *choice_items;
  if (zathura_form_field_choice_get_items(priv->form_field, &choice_items) !=
      ZATHURA_ERROR_OK) {
    g_array_free(selected_indices, TRUE);
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

    gtk_string_list_append(list_store, name);
    if (is_selected == true) {
      g_array_append_val(selected_indices, index);
    }
    index++;
  }

  GtkSelectionModel *selection = NULL;
  if (is_multiselect == true) {
    selection = GTK_SELECTION_MODEL(gtk_multi_selection_new(G_LIST_MODEL(list_store)));
  } else {
    selection = GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(list_store)));
  }

  if (selected_indices->len > 0) {
    if (is_multiselect == true) {
      for (guint i = 0; i < selected_indices->len; ++i) {
        guint selected_index = g_array_index(selected_indices, guint, i);
        gtk_selection_model_select_item(selection, selected_index, FALSE);
      }
    } else {
      guint selected_index = g_array_index(selected_indices, guint, 0);
      gtk_selection_model_select_item(selection, selected_index, TRUE);
    }
  }
  g_array_free(selected_indices, TRUE);

  GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
  g_signal_connect(factory, "setup",
                   G_CALLBACK(cb_form_field_choice_list_item_setup), NULL);
  g_signal_connect(factory, "bind",
                   G_CALLBACK(cb_form_field_choice_list_item_bind), NULL);

  g_signal_connect(selection, "selection-changed",
                   G_CALLBACK(cb_form_field_choice_list_changed),
                   form_field_widget);

  return gtk_list_view_new(selection, factory);
}

static void
cb_form_field_choice_list_changed(GtkSelectionModel *model,
                                  guint UNUSED(position),
                                  guint UNUSED(n_items),
                                  GtkWidget *form_field_widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(form_field_widget));

  zathura_list_t *choice_items;
  if (zathura_form_field_choice_get_items(priv->form_field, &choice_items) !=
      ZATHURA_ERROR_OK) {
    return;
  }

  guint n = g_list_model_get_n_items(G_LIST_MODEL(model));
  for (guint i = 0; i < n; ++i) {
    zathura_form_field_choice_item_t *choice_item =
        choice_item_at(choice_items, i);
    if (choice_item == NULL) {
      continue;
    }

    if (gtk_selection_model_is_selected(model, i) == TRUE) {
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
  gtk_widget_queue_draw(priv->layer.drawing_area);
  gtk_widget_queue_draw(form_field_widget);
}

static gboolean set_back_to_drawing_area(GtkWidget *widget) {
  ZathuraFormFieldChoicePrivate *priv =
      zathura_gtk_form_field_choice_get_instance_private(
          ZATHURA_FORM_FIELD_CHOICE(widget));

  if (priv->layer.choice_widget != NULL) {
    gtk_widget_set_visible(priv->layer.choice_widget, FALSE);
  }

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

  gtk_widget_set_visible(priv->layer.choice_widget, TRUE);
}

static void
cb_form_field_choice_list_item_setup(GtkSignalListItemFactory *UNUSED(factory),
                                     GtkListItem *list_item,
                                     gpointer UNUSED(data)) {
  GtkWidget *label = gtk_label_new(NULL);
  gtk_list_item_set_child(list_item, label);
}

static void
cb_form_field_choice_list_item_bind(GtkSignalListItemFactory *UNUSED(factory),
                                    GtkListItem *list_item,
                                    gpointer UNUSED(data)) {
  GtkWidget *label = gtk_list_item_get_child(list_item);
  GtkStringObject *item = GTK_STRING_OBJECT(gtk_list_item_get_item(list_item));
  gtk_label_set_text(GTK_LABEL(label), gtk_string_object_get_string(item));
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

  /* Render field */
  cairo_save(cairo);
  if (zathura_form_field_render_cairo(priv->form_field, image_cairo) !=
      ZATHURA_ERROR_OK) {
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
