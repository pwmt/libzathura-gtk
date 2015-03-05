 /* See LICENSE file for license and copyright information */

#include "../../macros.h"
#include "form-field-text.h"

#define RGB_TO_CAIRO(r, g, b) \
  (r)/255.0, (g)/255.0, (b)/255.0

static gboolean cb_form_field_text_focus_out(GtkWidget* widget, GdkEventFocus* event, GtkWidget* form_field_widget);
static gboolean cb_form_field_text_changed(GtkWidget* widget, GtkWidget* form_field_widget);
static gboolean cb_form_field_text_activate(GtkWidget* widget, GtkWidget* form_field_widget);
static gboolean cb_form_field_text_multiline_focus_out(GtkWidget* widget, GdkEventFocus* event, GtkWidget* form_field_widget);
static gboolean cb_form_field_text_rectangle_draw(GtkWidget* widget, cairo_t* cairo, GtkWidget* form_field_widget);
static gboolean cb_form_field_text_rectangle_button_press_event(GtkWidget* widget, GdkEventButton* event_button, GtkWidget* form_field_widget);

struct _ZathuraFormFieldTextPrivate {
  zathura_form_field_t* form_field;

  GtkWidget* previous_focus;
  GtkWidget* rectangle;
  GtkWidget* text_widget;
  GtkWidget* scrolled_window;
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraFormFieldText, zathura_gtk_form_field_text, GTK_TYPE_BIN)

#define ZATHURA_FORM_FIELD_TEXT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_FORM_FIELD_TEXT, \
                               ZathuraFormFieldTextPrivate))

static void
zathura_gtk_form_field_text_class_init(ZathuraFormFieldTextClass* UNUSED(class))
{
}

static void
zathura_gtk_form_field_text_init(ZathuraFormFieldText* widget)
{
  ZathuraFormFieldTextPrivate* priv = ZATHURA_FORM_FIELD_TEXT_GET_PRIVATE(widget);

  priv->form_field     = NULL;
  priv->text_widget    = NULL;
  priv->rectangle      = NULL;
  priv->previous_focus = NULL;

  gtk_widget_add_events(GTK_WIDGET(widget), GDK_BUTTON_PRESS_MASK);
}

GtkWidget*
zathura_gtk_form_field_text_new(zathura_form_field_t* form_field)
{
  g_return_val_if_fail(form_field != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_FORM_FIELD_TEXT, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraFormFieldTextPrivate* priv = ZATHURA_FORM_FIELD_TEXT_GET_PRIVATE(widget);
  priv->form_field = form_field;

  /* Get type */
  zathura_form_field_text_type_t text_type;
  if (zathura_form_field_text_get_type(priv->form_field, &text_type) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  /* Get text */
  char* text;
  if (zathura_form_field_text_get_text(priv->form_field, &text) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  /* Setup rectangle */
  priv->rectangle = gtk_drawing_area_new();
  gtk_widget_add_events(GTK_WIDGET(priv->rectangle), GDK_BUTTON_PRESS_MASK);
  g_signal_connect(priv->rectangle, "draw",
      G_CALLBACK(cb_form_field_text_rectangle_draw),
      widget);

  g_signal_connect(priv->rectangle, "button-press-event",
      G_CALLBACK(cb_form_field_text_rectangle_button_press_event),
      widget);

  /* Build widget depending on type */
  switch (text_type) {
    case ZATHURA_FORM_FIELD_TEXT_TYPE_NORMAL:
      {
        unsigned int max_length;
        if (zathura_form_field_text_get_max_length(priv->form_field, &max_length) != ZATHURA_ERROR_OK) {
          return NULL;
        }

        bool is_password;
        if (zathura_form_field_text_is_password(priv->form_field, &is_password) != ZATHURA_ERROR_OK) {
          return NULL;
        }

        priv->text_widget = gtk_entry_new();
        gtk_entry_set_has_frame(GTK_ENTRY(priv->text_widget), FALSE);
        gtk_entry_set_max_length(GTK_ENTRY(priv->text_widget), max_length);
        gtk_entry_set_visibility(GTK_ENTRY(priv->text_widget), !is_password);

        g_signal_connect(priv->text_widget, "focus-out-event",
            G_CALLBACK(cb_form_field_text_focus_out),
            widget);

        g_signal_connect(priv->text_widget, "changed",
            G_CALLBACK(cb_form_field_text_changed),
            widget);

        g_signal_connect(priv->text_widget, "activate",
            G_CALLBACK(cb_form_field_text_activate),
            widget);

        if (text != NULL) {
          gtk_entry_set_text(GTK_ENTRY(priv->text_widget), text);
        }
      }
      break;
    case ZATHURA_FORM_FIELD_TEXT_TYPE_MULTILINE:
      {
        priv->text_widget = gtk_text_view_new();
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->text_widget));

        if (text != NULL) {
          gtk_text_buffer_set_text(buffer, text, -1);
        }

        g_signal_connect(priv->text_widget, "focus-out-event",
            G_CALLBACK(cb_form_field_text_multiline_focus_out),
            widget);

        bool do_scroll;
        if (zathura_form_field_text_do_scroll(priv->form_field, &do_scroll) != ZATHURA_ERROR_OK) {
          return NULL;
        }

        if (do_scroll == true) {
          priv->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
          gtk_container_add(GTK_CONTAINER(priv->scrolled_window), priv->text_widget);
        }
      }
      break;
    case ZATHURA_FORM_FIELD_TEXT_TYPE_FILE_SELECT:
      return NULL; // TODO: Implement file selection dialog
  }

  gtk_container_add(GTK_CONTAINER(widget), priv->rectangle);
  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
set_back_to_drawing_area(GtkWidget* widget)
{
  ZathuraFormFieldTextPrivate* priv = ZATHURA_FORM_FIELD_TEXT_GET_PRIVATE(widget);

  /* Don't reset if current visible widget is the rectangle already */
  GtkWidget* child = gtk_bin_get_child(GTK_BIN(widget));
  if (child == priv->rectangle) {
    return FALSE;
  }

  /* Remove child */
  g_object_ref(child);
  gtk_container_remove(GTK_CONTAINER(widget), child);

  /* Add rectangle */
  gtk_container_add(GTK_CONTAINER(widget), priv->rectangle);

  gtk_widget_show_all(GTK_WIDGET(widget));

  /* Restore old focus */
  if (priv->previous_focus != NULL) {
    gtk_widget_grab_focus(priv->previous_focus);
  }

  return FALSE;
}

static void
reset_to_drawing_area(GtkWidget* widget)
{
  g_idle_add((GSourceFunc) set_back_to_drawing_area, widget);
}

static bool
save_text_single_line(GtkWidget* widget)
{
  ZathuraFormFieldTextPrivate* priv = ZATHURA_FORM_FIELD_TEXT_GET_PRIVATE(widget);

  if (zathura_form_field_text_set_text(priv->form_field, gtk_entry_get_text(GTK_ENTRY(priv->text_widget))) != ZATHURA_ERROR_OK) {
    return false;
  }

  return true;
}

static gboolean
cb_form_field_text_focus_out(GtkWidget* widget, GdkEventFocus* UNUSED(event),
    GtkWidget* form_field_widget)
{
  g_return_val_if_fail(widget != NULL, FALSE);
  g_return_val_if_fail(form_field_widget != NULL, FALSE);

  reset_to_drawing_area(form_field_widget);

  return TRUE;
}

static gboolean
cb_form_field_text_changed(GtkWidget* UNUSED(widget), GtkWidget* form_field_widget)
{
  save_text_single_line(form_field_widget);

  return TRUE;
}

static bool
save_text_multi_line(GtkWidget* widget)
{
  ZathuraFormFieldTextPrivate* priv = ZATHURA_FORM_FIELD_TEXT_GET_PRIVATE(widget);

  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->text_widget));
  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(buffer, &start, &end);

  char* text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

  if (zathura_form_field_text_set_text(priv->form_field, text) !=
      ZATHURA_ERROR_OK) {
    return false;
  }

  return true;
}

static gboolean
cb_form_field_text_multiline_focus_out(GtkWidget* widget, GdkEventFocus* UNUSED(event),
    GtkWidget* form_field_widget)
{
  g_return_val_if_fail(widget != NULL, FALSE);
  g_return_val_if_fail(form_field_widget != NULL, FALSE);

  save_text_multi_line(form_field_widget);
  reset_to_drawing_area(form_field_widget);

  return FALSE;
}

static gboolean
cb_form_field_text_rectangle_draw(GtkWidget* widget, cairo_t* cairo, GtkWidget* UNUSED(form_field_widget))
{
  const unsigned int width  = gtk_widget_get_allocated_width(widget);
  const unsigned int height = gtk_widget_get_allocated_height(widget);

  cairo_save(cairo);

  /* Draw background */
  cairo_set_source_rgba(cairo, RGB_TO_CAIRO(75, 181, 193), 0.5);
  cairo_rectangle(cairo, 0, 0, width, height);
  cairo_fill_preserve(cairo);

  cairo_restore(cairo);

  return FALSE;
}

static gboolean
cb_form_field_text_rectangle_button_press_event(GtkWidget* UNUSED(widget),
    GdkEventButton* event_button, GtkWidget* form_field_widget)
{
  ZathuraFormFieldTextPrivate* priv = ZATHURA_FORM_FIELD_TEXT_GET_PRIVATE(form_field_widget);

  /* Only allow left clicks */
  if (event_button->button != 1) {
    return TRUE;
  }

  g_object_ref(priv->rectangle);
  gtk_container_remove(GTK_CONTAINER(form_field_widget), priv->rectangle);

  if (priv->scrolled_window != NULL) {
    gtk_container_add(GTK_CONTAINER(form_field_widget), priv->scrolled_window);
  } else {
    gtk_container_add(GTK_CONTAINER(form_field_widget), priv->text_widget);
  }

  gtk_widget_show_all(form_field_widget);

  /* Save old focus */
  GtkWidget* top_level = gtk_widget_get_toplevel(form_field_widget);

  if (top_level != NULL) {
    priv->previous_focus = gtk_window_get_focus(GTK_WINDOW(top_level));
  }

  gtk_widget_grab_focus(priv->text_widget);

  return TRUE;
}

static gboolean
cb_form_field_text_activate(GtkWidget* UNUSED(widget), GtkWidget* form_field_widget)
{
  reset_to_drawing_area(form_field_widget);

  return TRUE;
}
