/* See LICENSE file for license and copyright information */

#include <math.h>

#include "editor.h"
#include "../internal.h"
#include "../../utils.h"
#include "../../macros.h"

#include "form-field-button.h"
#include "form-field-choice.h"
#include "form-field-text.h"

#define RGB_TO_CAIRO(r, g, b) \
  (r)/255.0, (g)/255.0, (b)/255.0

#define M_PI 3.14159265358979323846

struct _ZathuraFormFieldEditorPrivate {
  GtkWidget* overlay;

  struct {
    GtkWidget* drawing_area;
    GtkWidget* form_fields;
  } layer;

  ZathuraPage* page;
  zathura_list_t* form_fields;
  bool highlight;
};

enum {
  PROP_0,
  PROP_FORM_FIELDS_HIGHLIGHT,
};

typedef struct form_field_widget_mapping_s {
  GtkWidget* widget;
  zathura_form_field_t* form_field;
  zathura_rectangle_t position;
} form_field_widget_mapping_t;

static void zathura_gtk_form_field_editor_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_form_field_editor_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void zathura_gtk_form_field_editor_size_allocate(GtkWidget* widget, GdkRectangle* allocation);
static gboolean cb_draw_highlights(GtkWidget *widget, cairo_t *cairo, gpointer data);
static void cb_form_field_button_changed(GtkWidget* widget, gpointer data);
static void create_widgets(GtkWidget* editor);

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraFormFieldEditor, zathura_gtk_form_field_editor, GTK_TYPE_BIN)

static void
zathura_gtk_form_field_editor_class_init(ZathuraFormFieldEditorClass* class)
{
  /* overwrite methods */
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
  widget_class->size_allocate = zathura_gtk_form_field_editor_size_allocate;

  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_form_field_editor_set_property;
  object_class->get_property = zathura_gtk_form_field_editor_get_property;

  /* properties */
  g_object_class_install_property(
    object_class,
    PROP_FORM_FIELDS_HIGHLIGHT,
    g_param_spec_boolean(
      "highlight-form-fields",
      "highlight-form-fields",
      "Highlight form-fields by drawing rectangles around them",
      FALSE,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );
}

static void
zathura_gtk_form_field_editor_init(ZathuraFormFieldEditor* widget)
{
  ZathuraFormFieldEditorPrivate* priv = zathura_gtk_form_field_editor_get_instance_private(widget);

  priv->overlay            = NULL;
  priv->layer.drawing_area = NULL;
  priv->layer.form_fields  = NULL;
  priv->form_fields        = NULL;
  priv->page               = NULL;
  priv->highlight          = false;
}

GtkWidget*
zathura_gtk_form_field_editor_new(ZathuraPage* page)
{
  GObject* widget = g_object_new(ZATHURA_TYPE_FORM_FIELD_EDITOR, NULL);
  g_return_val_if_fail(widget != NULL, NULL);

  ZathuraFormFieldEditorPrivate* priv = zathura_gtk_form_field_editor_get_instance_private(ZATHURA_FORM_FIELD_EDITOR(widget));

  priv->page = page;

  /* Drawing area for highlighting */
  priv->layer.drawing_area = gtk_drawing_area_new();
  gtk_widget_set_halign(priv->layer.drawing_area, GTK_ALIGN_START);
  gtk_widget_set_valign(priv->layer.drawing_area, GTK_ALIGN_START);

  g_signal_connect(G_OBJECT(priv->layer.drawing_area), "draw", G_CALLBACK(cb_draw_highlights), priv);

  /* Fixed container for form field widgets */
  priv->layer.form_fields = gtk_fixed_new();

  /* Setup over lay*/
  priv->overlay = gtk_overlay_new();
  gtk_container_add(GTK_CONTAINER(priv->overlay), GTK_WIDGET(priv->layer.drawing_area));
  gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay), priv->layer.form_fields);

  /* Setup container */
  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->overlay));
  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static void
create_widgets(GtkWidget* editor)
{
  ZathuraFormFieldEditorPrivate* priv = zathura_gtk_form_field_editor_get_instance_private(ZATHURA_FORM_FIELD_EDITOR(editor));

  zathura_page_t* page = NULL;
  double scale = 1.0;
  g_object_get(G_OBJECT(priv->page), "page", &page, "scale", &scale, NULL);

  zathura_list_t* form_fields;
  if (zathura_page_get_form_fields(page, &form_fields) != ZATHURA_ERROR_OK) {
    return;
  }

  zathura_form_field_mapping_t* form_field_mapping;
  ZATHURA_LIST_FOREACH(form_field_mapping, form_fields) {
    zathura_form_field_type_t form_field_type = ZATHURA_FORM_FIELD_UNKNOWN;
    if (zathura_form_field_get_type(form_field_mapping->form_field, &form_field_type) != ZATHURA_ERROR_OK) {
      continue;
    }

    GtkWidget* form_field_widget = NULL;
    switch (form_field_type) {
      case ZATHURA_FORM_FIELD_BUTTON:
        form_field_widget = zathura_gtk_form_field_button_new(form_field_mapping->form_field);
        g_signal_connect(G_OBJECT(form_field_widget), "changed", G_CALLBACK(cb_form_field_button_changed), priv);
        break;
      case ZATHURA_FORM_FIELD_TEXT:
        form_field_widget = zathura_gtk_form_field_text_new(form_field_mapping->form_field);
        break;
      case ZATHURA_FORM_FIELD_CHOICE:
        form_field_widget = zathura_gtk_form_field_choice_new(form_field_mapping->form_field);
        break;
      default:
        break;
    }

    if (form_field_widget != NULL) {
      /* Create new mapping */
      form_field_widget_mapping_t* mapping = g_malloc0(sizeof(form_field_widget_mapping_t));

      mapping->position   = form_field_mapping->position;
      mapping->widget     = form_field_widget;
      mapping->form_field = form_field_mapping->form_field;

      priv->form_fields = zathura_list_append(priv->form_fields, mapping);

      /* Setup initial position of widgets */
      zathura_rectangle_t position = zathura_rectangle_scale(form_field_mapping->position, scale);
      const unsigned int width  = ceil(position.p2.x) - floor(position.p1.x);
      const unsigned int height = ceil(position.p2.y) - floor(position.p1.y);

      gtk_fixed_put(GTK_FIXED(priv->layer.form_fields), form_field_widget, position.p1.x, position.p1.y);
      gtk_widget_set_size_request(form_field_widget, width, height);
      gtk_widget_show(form_field_widget);
    }
  }
}

static void
zathura_gtk_form_field_editor_size_allocate(GtkWidget* widget, GdkRectangle* allocation)
{
  ZathuraFormFieldEditorPrivate* priv = zathura_gtk_form_field_editor_get_instance_private(ZATHURA_FORM_FIELD_EDITOR(widget));

  if (priv->form_fields == NULL) {
    create_widgets(widget);
  }

  double scale = 1.0;
  g_object_get(G_OBJECT(priv->page), "scale", &scale, NULL);

  if (priv->highlight == true) {
    gtk_widget_set_size_request(priv->layer.drawing_area, allocation->width, allocation->height);
    gtk_widget_queue_resize(priv->layer.drawing_area);
  }

  form_field_widget_mapping_t* form_field_mapping;
  ZATHURA_LIST_FOREACH(form_field_mapping, priv->form_fields) {
      zathura_rectangle_t position = zathura_rectangle_scale(form_field_mapping->position, scale);
      const unsigned int width  = ceil(position.p2.x) - floor(position.p1.x);
      const unsigned int height = ceil(position.p2.y) - floor(position.p1.y);

      gtk_fixed_move(GTK_FIXED(priv->layer.form_fields), form_field_mapping->widget, position.p1.x, position.p1.y);
      gtk_widget_set_size_request(form_field_mapping->widget, width, height);
  }

  GTK_WIDGET_CLASS(zathura_gtk_form_field_editor_parent_class)->size_allocate(widget, allocation);
}

static void
zathura_gtk_form_field_editor_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraFormFieldEditor* editor      = ZATHURA_FORM_FIELD_EDITOR(object);
  ZathuraFormFieldEditorPrivate* priv = zathura_gtk_form_field_editor_get_instance_private(editor);

  switch (prop_id) {
    case PROP_FORM_FIELDS_HIGHLIGHT:
      {
        priv->highlight = g_value_get_boolean(value);
        gtk_widget_queue_resize(priv->layer.drawing_area);
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
zathura_gtk_form_field_editor_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec)
{
  ZathuraFormFieldEditor* editor      = ZATHURA_FORM_FIELD_EDITOR(object);
  ZathuraFormFieldEditorPrivate* priv = zathura_gtk_form_field_editor_get_instance_private(editor);

  switch (prop_id) {
    case PROP_FORM_FIELDS_HIGHLIGHT:
      g_value_set_boolean(value, priv->highlight);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static gboolean
cb_draw_highlights(GtkWidget* UNUSED(widget), cairo_t* cairo, gpointer data)
{
  ZathuraFormFieldEditorPrivate* priv = (ZathuraFormFieldEditorPrivate*) data;

  /* Draw links if requested */
  if (priv->highlight == false) {
    return GDK_EVENT_PROPAGATE;
  }

  cairo_save(cairo);

  /* Get scale */
  double scale = 1.0;
  g_object_get(G_OBJECT(priv->page), "scale", &scale, NULL);

  form_field_widget_mapping_t* form_field_mapping;
  ZATHURA_LIST_FOREACH(form_field_mapping, priv->form_fields) {
    zathura_rectangle_t position = zathura_rectangle_scale(form_field_mapping->position, scale);
    const unsigned int width  = ceil(position.p2.x) - floor(position.p1.x);
    const unsigned int height = ceil(position.p2.y) - floor(position.p1.y);

    zathura_form_field_type_t form_field_type = ZATHURA_FORM_FIELD_UNKNOWN;
    if (zathura_form_field_get_type(form_field_mapping->form_field, &form_field_type) != ZATHURA_ERROR_OK) {
      continue;
    }

    if (form_field_type == ZATHURA_FORM_FIELD_BUTTON) {
      zathura_form_field_button_type_t button_type;
      if (zathura_form_field_button_get_type(form_field_mapping->form_field, &button_type) != ZATHURA_ERROR_OK) {
        continue;
      }

      if (button_type == ZATHURA_FORM_FIELD_BUTTON_TYPE_RADIO) {
        cairo_save(cairo);
        cairo_translate(cairo, position.p1.x + width / 2, position.p1.y + height / 2);
        cairo_scale(cairo, width/2.0, height/2.0);
        cairo_arc(cairo, 0., 0., 1., 0., 2 * M_PI);
        cairo_set_source_rgba(cairo, RGB_TO_CAIRO(75, 181, 193), 0.5);
        cairo_fill(cairo);
        cairo_restore(cairo);
        continue;
      }
    }

    /* Draw rectangle */
    cairo_set_source_rgba(cairo, RGB_TO_CAIRO(75, 181, 193), 0.5);
    cairo_rectangle(cairo, floor(position.p1.x), floor(position.p1.y), width, height);
    cairo_fill(cairo);
  }

  cairo_restore(cairo);

  return GDK_EVENT_PROPAGATE;
}

static void
cb_form_field_button_changed(GtkWidget* UNUSED(widget), gpointer data)
{
  ZathuraFormFieldEditorPrivate* priv = (ZathuraFormFieldEditorPrivate*) data;

  form_field_widget_mapping_t* form_field_mapping;
  ZATHURA_LIST_FOREACH(form_field_mapping, priv->form_fields) {
    zathura_form_field_type_t form_field_type = ZATHURA_FORM_FIELD_UNKNOWN;
    if (zathura_form_field_get_type(form_field_mapping->form_field, &form_field_type) != ZATHURA_ERROR_OK) {
      continue;
    }

    if (form_field_type != ZATHURA_FORM_FIELD_BUTTON) {
      continue;
    }

    gtk_widget_queue_draw(form_field_mapping->widget);
  }
}
