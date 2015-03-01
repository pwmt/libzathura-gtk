 /* See LICENSE file for license and copyright information */

#define M_PI 3.14159265358979323846

#define RGB_TO_CAIRO(r, g, b) \
  (r)/255.0, (g)/255.0, (b)/255.0

#include "form-field-button.h"

static gboolean zathura_gtk_form_field_button_draw(GtkWidget* widget, cairo_t* cairo);
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

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraFormFieldButton, zathura_gtk_form_field_button, GTK_TYPE_DRAWING_AREA)

#define ZATHURA_FORM_FIELD_BUTTON_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_FORM_FIELD_BUTTON, \
                               ZathuraFormFieldButtonPrivate))

static void
zathura_gtk_form_field_button_class_init(ZathuraFormFieldButtonClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_form_field_button_set_property;
  object_class->get_property = zathura_gtk_form_field_button_get_property;

  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
  widget_class->draw               = zathura_gtk_form_field_button_draw;
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
}

static void
zathura_gtk_form_field_button_init(ZathuraFormFieldButton* widget)
{
  ZathuraFormFieldButtonPrivate* priv = ZATHURA_FORM_FIELD_BUTTON_GET_PRIVATE(widget);

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
  ZathuraFormFieldButton* button    = ZATHURA_FORM_FIELD_BUTTON(object);
  ZathuraFormFieldButtonPrivate* priv = ZATHURA_FORM_FIELD_BUTTON_GET_PRIVATE(button);

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
  ZathuraFormFieldButtonPrivate* priv = ZATHURA_FORM_FIELD_BUTTON_GET_PRIVATE(button);

  switch (prop_id) {
    case PROP_FORM_FIELD:
      g_value_set_pointer(value, priv->button);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static gboolean
zathura_gtk_form_field_button_draw(GtkWidget* widget, cairo_t* cairo)
{
  ZathuraFormFieldButtonPrivate* priv = ZATHURA_FORM_FIELD_BUTTON_GET_PRIVATE(widget);

  zathura_form_field_button_type_t button_type;
  if (zathura_form_field_button_get_type(priv->button, &button_type) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  bool button_state;
  if (zathura_form_field_button_get_state(priv->button, &button_state) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  const unsigned int width    = gtk_widget_get_allocated_width(widget);
  const unsigned int height   = gtk_widget_get_allocated_height(widget);
  const unsigned int diameter = (width > height) ? height : width;
  const unsigned int border   = (diameter/8 == 0) ? 1 : diameter/8;

  switch (button_type) {
    case ZATHURA_FORM_FIELD_BUTTON_TYPE_PUSH:
      {
        cairo_save(cairo);

        /* Draw rectangle */
        cairo_rectangle(cairo, 0, 0, width, height);

        if (button_state == true) {
          cairo_set_source_rgb(cairo, 0, 0, 0);
        } else {
          cairo_set_source_rgb(cairo, RGB_TO_CAIRO(75, 181, 193));
        }

        cairo_fill_preserve(cairo);

        /* Draw button name */
        char* name;
        if (zathura_form_field_get_name(priv->button, &name) == ZATHURA_ERROR_OK) {
          cairo_select_font_face(cairo, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
          cairo_set_font_size(cairo, height / 2);
          cairo_set_source_rgb(cairo, 0, 0, 0);

          if (button_state == true) {
            cairo_set_source_rgb(cairo, 255, 255, 255);
          } else {
            cairo_set_source_rgb(cairo, 0, 0, 0);
          }

          cairo_text_extents_t extents;
          cairo_text_extents(cairo, name, &extents);
          double x = width  * 0.5 - (extents.width  * 0.5 + extents.x_bearing);
          double y = height * 0.5 - (extents.height * 0.5 + extents.y_bearing);
          cairo_move_to(cairo, x, y);
          cairo_show_text(cairo, name);
        };

        /* Draw border */
        cairo_set_source_rgb(cairo, 0, 0, 0);
        cairo_set_line_width(cairo, border);
        cairo_stroke(cairo);

        cairo_restore(cairo);
      }
      break;
    case ZATHURA_FORM_FIELD_BUTTON_TYPE_CHECK:
      {

        cairo_save(cairo);

        /* Draw rectangle */
        cairo_rectangle(cairo, 0, 0, width, height);
        cairo_set_source_rgb(cairo, RGB_TO_CAIRO(75, 181, 193));
        cairo_fill_preserve(cairo);

        /* Draw border */
        cairo_set_source_rgb(cairo, 0, 0, 0);
        cairo_set_line_width (cairo, border);
        cairo_stroke(cairo);

        /* Draw cross */
        if (button_state == true) {
          cairo_move_to(cairo, 0, 0);
          cairo_line_to(cairo, width, height);
          cairo_move_to(cairo, width, 0);
          cairo_line_to(cairo, 0, height);
          cairo_stroke(cairo);
        }

        cairo_restore(cairo);
      }
      break;
    case ZATHURA_FORM_FIELD_BUTTON_TYPE_RADIO:
      {
        /* Draw border as outer circle */
        cairo_save(cairo);
        cairo_translate(cairo, width / 2., height / 2.);
        cairo_scale(cairo, width/2.0, height/2.0);
        cairo_arc(cairo, 0., 0., 1., 0., 2 * M_PI);
        cairo_set_source_rgb(cairo, RGB_TO_CAIRO(0,0,0));
        cairo_fill(cairo);
        cairo_restore(cairo);

        /* Draw inner circle */
        cairo_save(cairo);

        cairo_translate(cairo, width / 2., height / 2.);
        cairo_scale(cairo, width/2.0 - border, height/2.0 -border);
        cairo_arc(cairo, 0., 0., 1., 0., 2 * M_PI);
        cairo_set_source_rgb(cairo, RGB_TO_CAIRO(75, 181, 193));
        cairo_fill_preserve(cairo);

        /* Draw cross */
        if (button_state == true) {
          cairo_clip(cairo);
          cairo_set_source_rgb(cairo, 0, 0, 0);
          cairo_translate (cairo, -width/2.0, -height/2.0);
          cairo_move_to(cairo, 0, 0);
          cairo_line_to(cairo, width, height);
          cairo_move_to(cairo, width, 0);
          cairo_line_to(cairo, 0, height);
          cairo_set_line_width(cairo, border);
          cairo_scale (cairo, 1/(width/2.0 - border), 1/(height/2.0 -border));
          cairo_stroke(cairo);
        }

        cairo_restore(cairo);
      }
      break;
  }

  return FALSE;
}

static gboolean
zathura_gtk_form_field_button_button_press_event(GtkWidget* widget, GdkEventButton* event_button)
{
  /* Only allow left clicks */
  if (event_button->button != 1) {
    return FALSE;
  }

  ZathuraFormFieldButtonPrivate* priv = ZATHURA_FORM_FIELD_BUTTON_GET_PRIVATE(widget);

  /* Toggle button state */
  bool button_state;
  if (zathura_form_field_button_get_state(priv->button, &button_state) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  if (zathura_form_field_button_set_state(priv->button, !button_state) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  /* Queue redrawing of the button */
  gtk_widget_queue_draw(widget);

  return TRUE;
}
