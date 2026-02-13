 /* See LICENSE file for license and copyright information */

#include "../../macros.h"
#include "form-field-text.h"

static gboolean cb_form_field_text_changed(GtkWidget* widget, GtkWidget* form_field_widget);
static gboolean cb_form_field_text_activate(GtkWidget* widget, GtkWidget* form_field_widget);
static void cb_form_field_text_focus_notify(GObject* object, GParamSpec* pspec, gpointer data);
static void cb_form_field_text_rectangle_pressed(GtkGestureClick* gesture, int n_press, double x, double y, gpointer data);
static void zathura_gtk_form_field_text_dispose(GObject* object);
static gboolean grab_text_focus_idle(GtkWidget* form_field_widget);

struct _ZathuraFormFieldTextPrivate {
  zathura_form_field_t* form_field;

  GtkWidget* overlay;

  struct {
    GtkWidget* drawing_area;
    GtkWidget* text_widget;
  } layer;

  // GtkWidget* previous_focus;
  GtkWidget* rectangle;
  GtkWidget* text_widget;
  GtkWidget* scrolled_window;
  bool editor_had_focus;
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraFormFieldText, zathura_gtk_form_field_text, GTK_TYPE_BOX)

static void
zathura_gtk_form_field_text_class_init(ZathuraFormFieldTextClass* class)
{
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->dispose = zathura_gtk_form_field_text_dispose;
}

static void
zathura_gtk_form_field_text_init(ZathuraFormFieldText* widget)
{
  ZathuraFormFieldTextPrivate* priv = zathura_gtk_form_field_text_get_instance_private(widget);

  priv->form_field     = NULL;
  priv->text_widget    = NULL;
  priv->rectangle      = NULL;
  priv->scrolled_window = NULL;
  priv->editor_had_focus = false;
  // priv->previous_focus = NULL;
}

GtkWidget*
zathura_gtk_form_field_text_new(zathura_form_field_t* form_field)
{
  g_return_val_if_fail(form_field != NULL, NULL);

  GObject* widget = g_object_new(ZATHURA_TYPE_FORM_FIELD_TEXT, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraFormFieldTextPrivate* priv = zathura_gtk_form_field_text_get_instance_private(ZATHURA_FORM_FIELD_TEXT(widget));
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
  g_object_ref(priv->rectangle);
  gtk_widget_set_hexpand(priv->rectangle, TRUE);
  gtk_widget_set_vexpand(priv->rectangle, TRUE);

  GtkGesture* click_gesture = gtk_gesture_click_new();
  gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click_gesture),
      GDK_BUTTON_PRIMARY);
  gtk_widget_add_controller(priv->rectangle, GTK_EVENT_CONTROLLER(click_gesture));
  g_signal_connect(click_gesture, "pressed",
      G_CALLBACK(cb_form_field_text_rectangle_pressed),
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
        g_object_ref(priv->text_widget);
        gtk_widget_add_css_class(priv->text_widget, "zathura-form-field-entry");
        gtk_widget_set_halign(priv->text_widget, GTK_ALIGN_FILL);
        gtk_widget_set_valign(priv->text_widget, GTK_ALIGN_FILL);
        gtk_widget_set_hexpand(priv->text_widget, TRUE);
        gtk_widget_set_vexpand(priv->text_widget, TRUE);
        gtk_entry_set_has_frame(GTK_ENTRY(priv->text_widget), FALSE);
        gtk_entry_set_max_length(GTK_ENTRY(priv->text_widget), max_length);
        gtk_entry_set_visibility(GTK_ENTRY(priv->text_widget), !is_password);

        g_signal_connect(priv->text_widget, "notify::has-focus",
            G_CALLBACK(cb_form_field_text_focus_notify),
            widget);

        g_signal_connect(priv->text_widget, "changed",
            G_CALLBACK(cb_form_field_text_changed),
            widget);

        g_signal_connect(priv->text_widget, "activate",
            G_CALLBACK(cb_form_field_text_activate),
            widget);

        if (text != NULL) {
          GtkEntryBuffer* buffer = gtk_entry_get_buffer(GTK_ENTRY(priv->text_widget));
          gtk_entry_buffer_set_text(buffer, text, strlen(text));
        }
      }
      break;
    case ZATHURA_FORM_FIELD_TEXT_TYPE_MULTILINE:
      {
        priv->text_widget = gtk_text_view_new();
        g_object_ref(priv->text_widget);
        gtk_widget_add_css_class(priv->text_widget, "zathura-form-field-textview");
        gtk_widget_set_halign(priv->text_widget, GTK_ALIGN_FILL);
        gtk_widget_set_valign(priv->text_widget, GTK_ALIGN_FILL);
        gtk_widget_set_hexpand(priv->text_widget, TRUE);
        gtk_widget_set_vexpand(priv->text_widget, TRUE);
        gtk_text_view_set_left_margin(GTK_TEXT_VIEW(priv->text_widget), 0);
        gtk_text_view_set_right_margin(GTK_TEXT_VIEW(priv->text_widget), 0);
        gtk_text_view_set_top_margin(GTK_TEXT_VIEW(priv->text_widget), 0);
        gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(priv->text_widget), 0);
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->text_widget));

        if (text != NULL) {
          gtk_text_buffer_set_text(buffer, text, -1);
        }

        g_signal_connect(priv->text_widget, "notify::has-focus",
            G_CALLBACK(cb_form_field_text_focus_notify),
            widget);

        bool do_scroll;
        if (zathura_form_field_text_do_scroll(priv->form_field, &do_scroll) != ZATHURA_ERROR_OK) {
          return NULL;
        }

        if (do_scroll == true) {
          priv->scrolled_window = gtk_scrolled_window_new();
          g_object_ref(priv->scrolled_window);
          gtk_widget_add_css_class(priv->scrolled_window, "zathura-form-field-scroller");
          gtk_widget_set_halign(priv->scrolled_window, GTK_ALIGN_FILL);
          gtk_widget_set_valign(priv->scrolled_window, GTK_ALIGN_FILL);
          gtk_widget_set_hexpand(priv->scrolled_window, TRUE);
          gtk_widget_set_vexpand(priv->scrolled_window, TRUE);
          gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(priv->scrolled_window), FALSE);
          gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(priv->scrolled_window), priv->text_widget);
        }
      }
      break;
    case ZATHURA_FORM_FIELD_TEXT_TYPE_FILE_SELECT:
      return NULL; // TODO: Implement file selection dialog
  }

  gtk_box_append(GTK_BOX(widget), priv->rectangle);

  return GTK_WIDGET(widget);
}

static gboolean
set_back_to_drawing_area(GtkWidget* widget)
{
  ZathuraFormFieldTextPrivate* priv = zathura_gtk_form_field_text_get_instance_private(ZATHURA_FORM_FIELD_TEXT(widget));

  /* Don't reset if current visible widget is the rectangle already */
  GtkWidget* child = gtk_widget_get_first_child(GTK_WIDGET(widget));
  if (child == priv->rectangle) {
    return FALSE;
  }

  /* Remove child */
  gtk_box_remove(GTK_BOX(widget), child);

  /* Add rectangle */
  gtk_box_append(GTK_BOX(widget), priv->rectangle);

  gtk_widget_set_visible(GTK_WIDGET(widget), TRUE);

  /* Restore old focus */
  // if (priv->previous_focus != NULL) {
    // gtk_widget_grab_focus(priv->previous_focus);
  // }

  return FALSE;
}

static void
zathura_gtk_form_field_text_dispose(GObject* object)
{
  ZathuraFormFieldTextPrivate* priv = zathura_gtk_form_field_text_get_instance_private(
      ZATHURA_FORM_FIELD_TEXT(object));

  g_clear_object(&priv->scrolled_window);
  g_clear_object(&priv->text_widget);
  g_clear_object(&priv->rectangle);

  G_OBJECT_CLASS(zathura_gtk_form_field_text_parent_class)->dispose(object);
}

static void
reset_to_drawing_area(GtkWidget* widget)
{
  ZathuraFormFieldTextPrivate* priv = zathura_gtk_form_field_text_get_instance_private(
      ZATHURA_FORM_FIELD_TEXT(widget));
  priv->editor_had_focus = false;
  g_idle_add((GSourceFunc) set_back_to_drawing_area, widget);
}

static bool
save_text_single_line(GtkWidget* widget)
{
  ZathuraFormFieldTextPrivate* priv = zathura_gtk_form_field_text_get_instance_private(ZATHURA_FORM_FIELD_TEXT(widget));

  if (zathura_form_field_text_set_text(priv->form_field, gtk_editable_get_text(GTK_EDITABLE(priv->text_widget))) != ZATHURA_ERROR_OK) {
    return false;
  }

  if (zathura_form_field_save(priv->form_field) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  return true;
}

static gboolean
cb_form_field_text_changed(GtkWidget* UNUSED(widget), GtkWidget* form_field_widget)
{
  save_text_single_line(form_field_widget);

  return GDK_EVENT_STOP;
}

static bool
save_text_multi_line(GtkWidget* widget)
{
  ZathuraFormFieldTextPrivate* priv = zathura_gtk_form_field_text_get_instance_private(ZATHURA_FORM_FIELD_TEXT(widget));

  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->text_widget));
  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(buffer, &start, &end);

  char* text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

  if (zathura_form_field_text_set_text(priv->form_field, text) !=
      ZATHURA_ERROR_OK) {
    return false;
  }

  if (zathura_form_field_save(priv->form_field) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  return true;
}

static void
cb_form_field_text_focus_notify(GObject* object, GParamSpec* UNUSED(pspec), gpointer data)
{
  g_return_if_fail(object != NULL);
  g_return_if_fail(data != NULL);

  ZathuraFormFieldTextPrivate* priv =
    zathura_gtk_form_field_text_get_instance_private(ZATHURA_FORM_FIELD_TEXT(data));

  if (gtk_widget_has_focus(GTK_WIDGET(object)) == TRUE) {
    priv->editor_had_focus = true;
    return;
  }

  GtkWidget* child = gtk_widget_get_first_child(GTK_WIDGET(data));
  if (child == priv->rectangle) {
    return;
  }

  if (priv->editor_had_focus == false) {
    return;
  }

  priv->editor_had_focus = false;

  if (GTK_IS_TEXT_VIEW(priv->text_widget)) {
    save_text_multi_line(GTK_WIDGET(data));
  }

  reset_to_drawing_area(GTK_WIDGET(data));
}

static void
cb_form_field_text_rectangle_pressed(GtkGestureClick* gesture, int UNUSED(n_press),
    double UNUSED(x), double UNUSED(y), gpointer data)
{
  GtkWidget* form_field_widget = GTK_WIDGET(data);
  ZathuraFormFieldTextPrivate* priv =
    zathura_gtk_form_field_text_get_instance_private(ZATHURA_FORM_FIELD_TEXT(form_field_widget));

  guint button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
  /* Only allow left clicks */
  if (button != GDK_BUTTON_PRIMARY) {
    return;
  }

  gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);
  priv->editor_had_focus = false;

  gtk_box_remove(GTK_BOX(form_field_widget), priv->rectangle);

  if (priv->scrolled_window != NULL) {
    gtk_box_append(GTK_BOX(form_field_widget), priv->scrolled_window);
  } else {
    gtk_box_append(GTK_BOX(form_field_widget), priv->text_widget);
  }

  int width = gtk_widget_get_width(form_field_widget);
  int height = gtk_widget_get_height(form_field_widget);
  if (width <= 0 || height <= 0) {
    width = gtk_widget_get_width(priv->rectangle);
    height = gtk_widget_get_height(priv->rectangle);
  }
  if (width <= 0) {
    width = 1;
  }
  if (height <= 0) {
    height = 1;
  }
  if (priv->scrolled_window != NULL) {
    gtk_widget_set_size_request(priv->scrolled_window, width, height);
  } else {
    gtk_widget_set_size_request(priv->text_widget, width, height);
  }

  gtk_widget_set_visible(form_field_widget, TRUE);

  /* Save old focus */
  // GtkWidget* top_level = gtk_widget_get_toplevel(form_field_widget);

  // if (top_level != NULL) {
  //   priv->previous_focus = gtk_window_get_focus(GTK_WINDOW(top_level));
  // }

  g_idle_add((GSourceFunc) grab_text_focus_idle, form_field_widget);

}

static gboolean
grab_text_focus_idle(GtkWidget* form_field_widget)
{
  ZathuraFormFieldTextPrivate* priv =
    zathura_gtk_form_field_text_get_instance_private(ZATHURA_FORM_FIELD_TEXT(form_field_widget));

  gtk_widget_grab_focus(priv->text_widget);
  return FALSE;
}


static gboolean
cb_form_field_text_activate(GtkWidget* UNUSED(widget), GtkWidget* form_field_widget)
{
  reset_to_drawing_area(form_field_widget);

  return GDK_EVENT_STOP;
}
