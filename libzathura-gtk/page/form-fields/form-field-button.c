 /* See LICENSE file for license and copyright information */

#include "form-field-button.h"

static gboolean zathura_gtk_form_field_button_button_press_event(GtkWidget* widget, GdkEventButton* event_button);
static void zathura_gtk_form_field_button_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_form_field_button_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);

struct _ZathuraFormFieldButtonPrivate {
  zathura_form_field_t* button;
};

enum {
  PROP_0,
  PROP_FORM_FIELD,
};

enum {
  SIGNAL_CHANGED,
  N_SIGNALS
};

static guint signals[N_SIGNALS];

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraFormFieldButton, zathura_gtk_form_field_button, GTK_TYPE_DRAWING_AREA)

static void
zathura_gtk_form_field_button_class_init(ZathuraFormFieldButtonClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_form_field_button_set_property;
  object_class->get_property = zathura_gtk_form_field_button_get_property;

  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
  widget_class->button_press_event = zathura_gtk_form_field_button_button_press_event;

  /* properties */
  g_object_class_install_property(
    object_class,
    PROP_FORM_FIELD,
    g_param_spec_pointer(
      "form-field",
      "form-field",
      "The zathura_form_field_t instance",
      G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS
    )
  );

  /* signals */
  signals[SIGNAL_CHANGED] = g_signal_new("changed",
      ZATHURA_TYPE_FORM_FIELD_BUTTON,
      G_SIGNAL_RUN_LAST,
      0,
      NULL,
      NULL,
      g_cclosure_marshal_generic,
      G_TYPE_NONE,
      0);
}

static void
zathura_gtk_form_field_button_init(ZathuraFormFieldButton* widget)
{
  ZathuraFormFieldButtonPrivate* priv = zathura_gtk_form_field_button_get_instance_private(widget);

  priv->button = NULL;

  gtk_widget_add_events(GTK_WIDGET(widget), GDK_BUTTON_PRESS_MASK);
}

GtkWidget*
zathura_gtk_form_field_button_new(zathura_form_field_t* button)
{
  g_return_val_if_fail(button != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_FORM_FIELD_BUTTON, "form-field", button, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  return GTK_WIDGET(widget);
}

static void
zathura_gtk_form_field_button_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraFormFieldButton* button      = ZATHURA_FORM_FIELD_BUTTON(object);
  ZathuraFormFieldButtonPrivate* priv = zathura_gtk_form_field_button_get_instance_private(button);

  switch (prop_id) {
    case PROP_FORM_FIELD:
      priv->button = g_value_get_pointer(value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
zathura_gtk_form_field_button_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec)
{
  ZathuraFormFieldButton* button        = ZATHURA_FORM_FIELD_BUTTON(object);
  ZathuraFormFieldButtonPrivate* priv = zathura_gtk_form_field_button_get_instance_private(button);

  switch (prop_id) {
    case PROP_FORM_FIELD:
      g_value_set_pointer(value, priv->button);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static gboolean
zathura_gtk_form_field_button_button_press_event(GtkWidget* widget, GdkEventButton* event_button)
{
  /* Only allow left clicks */
  if (event_button->button != 1) {
    return GDK_EVENT_PROPAGATE;
  }

  ZathuraFormFieldButtonPrivate* priv = zathura_gtk_form_field_button_get_instance_private(ZATHURA_FORM_FIELD_BUTTON(widget));

  /* Toggle button state */
  bool button_state;
  if (zathura_form_field_button_get_state(priv->button, &button_state) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  if (zathura_form_field_button_set_state(priv->button, !button_state) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  if (zathura_form_field_save(priv->button) != ZATHURA_ERROR_OK) {
    return GDK_EVENT_PROPAGATE;
  }

  g_signal_emit(widget, signals[SIGNAL_CHANGED], 0);

  /* Queue redrawing of the button */
  gtk_widget_queue_draw(widget);

  return GDK_EVENT_STOP;
}
