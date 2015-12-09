 /* See LICENSE file for license and copyright information */

#include <math.h>

#include "page.h"
#include "internal.h"
#include "callbacks.h"
#include "form-fields/editor.h"
#include "rotated-bin.h"

static void zathura_gtk_page_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_page_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void render_page(ZathuraPage* widget);

enum {
  PROP_0,
  PROP_PAGE,
  PROP_ROTATION,
  PROP_SCALE,
  PROP_LINKS_HIGHLIGHT,
  PROP_FORM_FIELDS_EDIT,
  PROP_FORM_FIELDS_HIGHLIGHT
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraPage, zathura_gtk_page, GTK_TYPE_BIN)

static void
zathura_gtk_page_class_init(ZathuraPageClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_page_set_property;
  object_class->get_property = zathura_gtk_page_get_property;

  /* properties */
  g_object_class_install_property(
    object_class,
    PROP_PAGE,
    g_param_spec_pointer(
      "page",
      "Page",
      "The zathura_page_t instance",
      G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_ROTATION,
    g_param_spec_uint(
      "rotation",
      "Rotation",
      "Defines the degree of rotation (0, 90, 180, 270)",
      0,
      270,
      0,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_SCALE,
    g_param_spec_double(
      "scale",
      "Scale",
      "The scale level",
      0.01,
      100.0,
      1.0,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_LINKS_HIGHLIGHT,
    g_param_spec_boolean(
      "highlight-links",
      "highlight-links",
      "Highlight links by drawing rectangles around them",
      FALSE,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_FORM_FIELDS_EDIT,
    g_param_spec_boolean(
      "edit-form-fields",
      "edit-form-fields",
      "Allow editing of form fields",
      TRUE,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

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
zathura_gtk_page_init(ZathuraPage* widget)
{
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(widget);

  priv->page               = NULL;
  priv->rotated_bin        = NULL;
  priv->overlay            = NULL;
  priv->layer.drawing_area = NULL;
  priv->layer.links        = NULL;
  priv->layer.form_fields  = NULL;

  priv->dimensions.width  = 0;
  priv->dimensions.height = 0;

  priv->settings.rotation = 0;
  priv->settings.scale    = 1.0;

  priv->links.list      = NULL;
  priv->links.retrieved = false;
  priv->links.draw      = false;

  priv->form_fields.list      = NULL;
  priv->form_fields.retrieved = false;
  priv->form_fields.edit      = true;
}

GtkWidget*
zathura_gtk_page_new(zathura_page_t* page)
{
  if (page == NULL) {
    return NULL;
  }

  GObject* widget = g_object_new(ZATHURA_TYPE_PAGE, "page", page, NULL);
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(widget);

  if (zathura_page_get_width(page, &(priv->dimensions.width)) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  if (zathura_page_get_height(page, &(priv->dimensions.height)) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  gint scale_factor = gtk_widget_get_scale_factor(GTK_WIDGET(widget));

  /* Setup drawing area */
  priv->layer.drawing_area = gtk_drawing_area_new();
  gtk_widget_set_halign(priv->layer.drawing_area, GTK_ALIGN_START);
  gtk_widget_set_valign(priv->layer.drawing_area, GTK_ALIGN_START);
  gtk_widget_set_size_request(priv->layer.drawing_area, priv->dimensions.width *
      scale_factor, priv->dimensions.height * scale_factor);
  g_signal_connect(G_OBJECT(priv->layer.drawing_area), "draw", G_CALLBACK(cb_page_draw), widget);

  /* Setup links layer */
  priv->layer.links = gtk_drawing_area_new();
  g_signal_connect(G_OBJECT(priv->layer.links), "draw", G_CALLBACK(cb_page_draw_links), widget);

  /* Setup form fields layer */
  priv->layer.form_fields = zathura_gtk_form_field_editor_new(ZATHURA_PAGE(widget));

  /* Setup over lay */
  priv->overlay = gtk_overlay_new();
  gtk_container_add(GTK_CONTAINER(priv->overlay), GTK_WIDGET(priv->layer.drawing_area));
  gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay), priv->layer.links);
  gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay), priv->layer.form_fields);

  g_signal_connect(priv->overlay, "realize", G_CALLBACK(cb_page_overlay_realized), widget);

  /* Setup rotated bin container */
  priv->rotated_bin = zathura_gtk_rotated_bin_new();
  gtk_container_add(GTK_CONTAINER(priv->rotated_bin), GTK_WIDGET(priv->overlay));

  /* Setup container */
  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->rotated_bin));

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static void
zathura_gtk_page_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraPage* page        = ZATHURA_PAGE(object);
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(page);

  switch (prop_id) {
    case PROP_PAGE:
      priv->page = g_value_get_pointer(value);
      break;
    case PROP_ROTATION:
      {
        unsigned int rotation = g_value_get_uint(value);
        switch (rotation) {
          case 0:
          case 90:
          case 180:
          case 270:
            if (priv->settings.rotation != rotation) {
              priv->settings.rotation = rotation;
              zathura_gtk_rotated_bin_set_angle(ZATHURA_ROTATED_BIN(priv->rotated_bin), rotation);
            }
            break;
          default:
            // TODO: Pring warning message
            break;
        }
      }
      break;
    case PROP_SCALE:
      {
        double scale = g_value_get_double(value);
        if (priv->settings.scale != scale) {
          priv->settings.scale = scale;
          render_page(page);
        }
      }
      break;
    case PROP_LINKS_HIGHLIGHT:
      {
        priv->links.draw = g_value_get_boolean(value);
        render_page(page);
      }
      break;
    case PROP_FORM_FIELDS_EDIT:
      {
        priv->form_fields.edit = g_value_get_boolean(value);
        render_page(page);
      }
      break;
    case PROP_FORM_FIELDS_HIGHLIGHT:
      g_object_set(priv->layer.form_fields, "highlight-form-fields", g_value_get_boolean(value), NULL);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
zathura_gtk_page_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec)
{
  ZathuraPage* page        = ZATHURA_PAGE(object);
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(page);

  switch (prop_id) {
    case PROP_PAGE:
      g_value_set_pointer(value, priv->page);
      break;
    case PROP_ROTATION:
      g_value_set_uint(value, priv->settings.rotation);
      break;
    case PROP_SCALE:
      g_value_set_double(value, priv->settings.scale);
      break;
    case PROP_LINKS_HIGHLIGHT:
      g_value_set_boolean(value, priv->links.draw);
      break;
    case PROP_FORM_FIELDS_EDIT:
      g_value_set_boolean(value, priv->form_fields.edit);
      break;
    case PROP_FORM_FIELDS_HIGHLIGHT:
      {
        bool highlight_form_fields;
        g_object_get(priv->layer.form_fields, "highlight-form-fields", &highlight_form_fields, NULL);
        g_value_set_boolean(value, highlight_form_fields);
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
calculate_widget_size(ZathuraPagePrivate* priv, unsigned int* widget_width,
    unsigned int* widget_height)
{
  /* if (priv->settings.rotation % 180) { */
    /* *widget_width  = round(priv->dimensions.height * priv->settings.scale); */
    /* *widget_height = round(priv->dimensions.width  * priv->settings.scale); */
  /* } else { */
    *widget_width  = round(priv->dimensions.width  * priv->settings.scale);
    *widget_height = round(priv->dimensions.height * priv->settings.scale);
  /* } */
}

static void
render_page(ZathuraPage* widget)
{
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(widget);

  unsigned int page_widget_width;
  unsigned int page_widget_height;

  calculate_widget_size(priv, &page_widget_width, &page_widget_height);

  if (priv->form_fields.edit == true) {
    gtk_widget_show(priv->layer.form_fields);
  } else {
    gtk_widget_hide(priv->layer.form_fields);
  }

  gint scale_factor = gtk_widget_get_scale_factor(GTK_WIDGET(widget));
  gtk_widget_set_size_request(priv->layer.drawing_area, page_widget_width * scale_factor, page_widget_height * scale_factor);
  gtk_widget_queue_resize(priv->layer.drawing_area);
}
