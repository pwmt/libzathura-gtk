 /* See LICENSE file for license and copyright information */

#include "form-field-button.h"

/* static gboolean zathura_gtk_form_field_button_button_press_event(GtkWidget* widget, GdkEventButton* event_button); */
static void cb_draw_button(GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer data);
static void zathura_gtk_form_field_button_dispose(GObject* object);
static void zathura_gtk_form_field_button_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_form_field_button_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);

struct _ZathuraFormFieldButtonPrivate {
  zathura_form_field_t* button;
  GtkWidget* drawing_area;
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

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraFormFieldButton, zathura_gtk_form_field_button, GTK_TYPE_WIDGET)

static void
zathura_gtk_form_field_button_class_init(ZathuraFormFieldButtonClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_form_field_button_set_property;
  object_class->get_property = zathura_gtk_form_field_button_get_property;
  object_class->dispose = zathura_gtk_form_field_button_dispose;

  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
  /* widget_class->button_press_event = zathura_gtk_form_field_button_button_press_event; */

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
  priv->drawing_area = NULL;

  /* gtk_widget_add_events(GTK_WIDGET(widget), GDK_BUTTON_PRESS_MASK); */
}

static void
zathura_gtk_form_field_button_dispose(GObject* object)
{
  ZathuraFormFieldButton* form_field_button = ZATHURA_FORM_FIELD_BUTTON(object);
  ZathuraFormFieldButtonPrivate* priv = zathura_gtk_form_field_button_get_instance_private(form_field_button);

  gtk_widget_unparent(priv->drawing_area);
  priv->drawing_area = NULL;
}

GtkWidget*
zathura_gtk_form_field_button_new(zathura_form_field_t* button)
{
  g_return_val_if_fail(button != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_FORM_FIELD_BUTTON, "form-field", button, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraFormFieldButtonPrivate* priv = zathura_gtk_form_field_button_get_instance_private(ZATHURA_FORM_FIELD_BUTTON(widget));

  priv->drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(priv->drawing_area), cb_draw_button, widget, NULL);
  gtk_widget_set_parent(priv->drawing_area, GTK_WIDGET(widget));

  zathura_rectangle_t position;
  if (zathura_form_field_get_position(priv->button, &position) != ZATHURA_ERROR_OK) {
    // TODO: Fix
  }

  const unsigned int width  = ceil(position.p2.x) - floor(position.p1.x);
  const unsigned int height = ceil(position.p2.y) - floor(position.p1.y);

  gtk_widget_set_size_request(priv->drawing_area, width, height);

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

/* static gboolean */
/* zathura_gtk_form_field_button_button_press_event(GtkWidget* widget, GdkEventButton* event_button) */
/* { */
/*   #<{(| Only allow left clicks |)}># */
/*   if (event_button->button != 1) { */
/*     return GDK_EVENT_PROPAGATE; */
/*   } */
/*  */
/*   ZathuraFormFieldButtonPrivate* priv = zathura_gtk_form_field_button_get_instance_private(ZATHURA_FORM_FIELD_BUTTON(widget)); */
/*  */
/*   #<{(| Toggle button state |)}># */
/*   bool button_state; */
/*   if (zathura_form_field_button_get_state(priv->button, &button_state) != ZATHURA_ERROR_OK) { */
/*     return GDK_EVENT_PROPAGATE; */
/*   } */
/*  */
/*   if (zathura_form_field_button_set_state(priv->button, !button_state) != ZATHURA_ERROR_OK) { */
/*     return GDK_EVENT_PROPAGATE; */
/*   } */
/*  */
/*   if (zathura_form_field_save(priv->button) != ZATHURA_ERROR_OK) { */
/*     return GDK_EVENT_PROPAGATE; */
/*   } */
/*  */
/*   g_signal_emit(widget, signals[SIGNAL_CHANGED], 0); */
/*  */
/*   #<{(| Queue redrawing of the button |)}># */
/*   gtk_widget_queue_draw(widget); */
/*  */
/*   return GDK_EVENT_STOP; */
/* } */

#define RGB_TO_CAIRO(r, g, b) \
  (r)/255.0, (g)/255.0, (b)/255.0

static void
cb_draw_button(GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer data)
{
  ZathuraFormFieldButtonPrivate* priv = zathura_gtk_form_field_button_get_instance_private(data);

  cairo_save(cairo);

  /* Draw rectangle */
  cairo_set_source_rgba(cairo, RGB_TO_CAIRO(75, 181, 193), 0.5);
  cairo_rectangle(cairo, 0, 0, width, height);
  cairo_fill(cairo);

  fprintf(stderr, "%zu %zu\n", width, height);

  cairo_restore(cairo);

  gint device_scale = gtk_widget_get_scale_factor(GTK_WIDGET(area));
  cairo_save(cairo);

  /* Create image surface */
  cairo_surface_t* image_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width * device_scale, height * device_scale);
  if (image_surface == NULL) {
    return;
  }

  cairo_surface_set_device_scale(image_surface, device_scale, device_scale);

  cairo_t* image_cairo = cairo_create(image_surface);
  if (image_cairo == NULL) {
    cairo_surface_destroy(image_surface);
    return;
  }

  /* Scale */
  cairo_save(image_cairo);

  /* Render page */
  if (zathura_form_field_render_cairo(priv->button, image_cairo, device_scale) != ZATHURA_ERROR_OK) {
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
