 /* See LICENSE file for license and copyright information */

#include "../../macros.h"
#include "form-field-text.h"

static gboolean cb_form_field_text_focus_out(GtkWidget* widget, GdkEventFocus* event, ZathuraFormFieldText* form_field_widget);
static gboolean cb_form_field_text_multiline_focus_out(GtkWidget* widget, GdkEventFocus* event, ZathuraFormFieldText* form_field_widget);

struct _ZathuraFormFieldTextPrivate {
  zathura_form_field_t* form_field;

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

  priv->form_field  = NULL;
  priv->text_widget = NULL;

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

  zathura_form_field_text_type_t text_type;
  if (zathura_form_field_text_get_type(priv->form_field, &text_type) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  char* text;
  if (zathura_form_field_text_get_text(priv->form_field, &text) != ZATHURA_ERROR_OK) {
    return NULL;
  }

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

        if (text != NULL) {
          gtk_entry_set_text(GTK_ENTRY(priv->text_widget), text);
        }

        gtk_container_add(GTK_CONTAINER(widget), priv->text_widget);
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
          gtk_container_add(GTK_CONTAINER(widget), priv->scrolled_window);
        } else {
          gtk_container_add(GTK_CONTAINER(widget), priv->text_widget);
        }
      }
      break;
    case ZATHURA_FORM_FIELD_TEXT_TYPE_FILE_SELECT:
      return NULL; // TODO: Implement file selection dialog
  }

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static gboolean
cb_form_field_text_focus_out(GtkWidget* widget, GdkEventFocus* UNUSED(event),
    ZathuraFormFieldText* form_field_widget)
{
  g_return_val_if_fail(widget != NULL, FALSE);
  g_return_val_if_fail(form_field_widget != NULL, FALSE);

  ZathuraFormFieldTextPrivate* priv = ZATHURA_FORM_FIELD_TEXT_GET_PRIVATE(form_field_widget);

  if (zathura_form_field_text_set_text(priv->form_field, gtk_entry_get_text(GTK_ENTRY(priv->text_widget))) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  return TRUE;
}

static gboolean
cb_form_field_text_multiline_focus_out(GtkWidget* widget, GdkEventFocus* UNUSED(event),
    ZathuraFormFieldText* form_field_widget)
{
  g_return_val_if_fail(widget != NULL, FALSE);
  g_return_val_if_fail(form_field_widget != NULL, FALSE);

  ZathuraFormFieldTextPrivate* priv = ZATHURA_FORM_FIELD_TEXT_GET_PRIVATE(form_field_widget);

  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->text_widget));
  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(buffer, &start, &end);

  char* text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

  if (zathura_form_field_text_set_text(priv->form_field, text) !=
      ZATHURA_ERROR_OK) {
    return FALSE;
  }

  return FALSE;
}
