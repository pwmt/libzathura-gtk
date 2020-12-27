 /* See LICENSE file for license and copyright information */

#include <math.h>

#include "page.h"
#include "internal.h"
#include "annotations/overlay.h"
#include "form-fields/editor.h"
#include "../utils.h"

#define RGB_TO_CAIRO(r, g, b) \
  (r)/255.0, (g)/255.0, (b)/255.0

static void zathura_gtk_page_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_page_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void render_page(ZathuraPage* widget);

static void cb_page_draw(GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer data);
static gboolean cb_page_draw_links(GtkWidget *widget, cairo_t *cairo, gpointer data);
static void cb_page_overlay_realized(GtkWidget* overlay, gpointer data);

enum {
  PROP_0,
  PROP_PAGE,
  PROP_ROTATION,
  PROP_SCALE,
  PROP_LINKS_HIGHLIGHT,
  PROP_FORM_FIELDS_EDIT,
  PROP_FORM_FIELDS_HIGHLIGHT
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraPage, zathura_gtk_page, GTK_TYPE_WIDGET)

void
zathura_gtk_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(ZATHURA_PAGE(widget));

  /* Apply rotation */
  double width = gtk_widget_get_width(widget);
  double height = gtk_widget_get_height(widget);
  double x = (width / 2.0);
  double y = (height / 2.0);

  gtk_snapshot_translate(snapshot, &GRAPHENE_POINT_INIT(x, y));
  gtk_snapshot_rotate(snapshot, priv->settings.rotation);
  gtk_snapshot_translate(snapshot, &GRAPHENE_POINT_INIT(-x, -y));

  /* Draw background */
  GdkRGBA white;
  gdk_rgba_parse(&white, "white");
  gtk_snapshot_append_color(snapshot, &white, &GRAPHENE_RECT_INIT(0, 0, width, height));

  gtk_snapshot_save(snapshot);
  gtk_widget_snapshot_child(widget, priv->overlay, snapshot);
  gtk_snapshot_restore(snapshot);
}

static void
zathura_gtk_dispose(GObject* object)
{
  ZathuraPage* page = ZATHURA_PAGE(object);
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(page);

  gtk_widget_unparent(priv->overlay);
  priv->overlay = NULL;

  G_OBJECT_CLASS(zathura_gtk_page_parent_class)->dispose(object);
}

static void
zathura_gtk_page_class_init(ZathuraPageClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_page_set_property;
  object_class->get_property = zathura_gtk_page_get_property;
  object_class->dispose = zathura_gtk_dispose;

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

  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
  widget_class->snapshot = zathura_gtk_snapshot;
  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

static void
zathura_gtk_page_init(ZathuraPage* widget)
{
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(widget);

  priv->page               = NULL;
  priv->overlay            = NULL;
  priv->layer.drawing_area = NULL;
  priv->layer.links        = NULL;
  priv->layer.form_fields  = NULL;
  priv->layer.annotations  = NULL;

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

  /* gtk_widget_init_template(GTK_WIDGET(widget)); */

  /* gtk_widget_add_events(GTK_WIDGET(widget), GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); */
}

GtkWidget*
zathura_gtk_page_new(zathura_page_t* page)
{
  if (page == NULL) {
    return NULL;
  }

  ZathuraPage* widget = g_object_new(ZATHURA_TYPE_PAGE, "page", page, NULL);
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(widget);

  if (zathura_page_get_width(page, &(priv->dimensions.width)) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  if (zathura_page_get_height(page, &(priv->dimensions.height)) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  double scale_factor = priv->settings.scale;

  /* Setup drawing area */
  priv->layer.drawing_area = gtk_drawing_area_new();
  gtk_widget_set_halign(priv->layer.drawing_area, GTK_ALIGN_START);
  gtk_widget_set_valign(priv->layer.drawing_area, GTK_ALIGN_START);
  gtk_widget_set_size_request(priv->layer.drawing_area, priv->dimensions.width, priv->dimensions.height);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(priv->layer.drawing_area), cb_page_draw, widget, NULL);

  /* Setup links layer */
  priv->layer.links = gtk_drawing_area_new();
  /* g_signal_connect(G_OBJECT(priv->layer.links), "draw", G_CALLBACK(cb_page_draw_links), widget); */

  /* Setup form fields layer */
  /* priv->layer.form_fields = zathura_gtk_form_field_editor_new(ZATHURA_PAGE(widget)); */

  /* Setup annotation layer */
  /* priv->layer.annotations = zathura_gtk_annotation_overlay_new(ZATHURA_PAGE(widget)); */

  /* Setup over lay */
  priv->overlay = gtk_overlay_new();

  gtk_overlay_set_child(GTK_OVERLAY(priv->overlay), GTK_WIDGET(priv->layer.drawing_area));
  gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay), priv->layer.links);
  /* gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay), priv->layer.annotations); */
  /* gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay), priv->layer.form_fields); */

  /* g_signal_connect(priv->overlay, "realize", G_CALLBACK(cb_page_overlay_realized), widget); */

  gtk_widget_set_parent(priv->overlay, GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static void
zathura_gtk_page_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraPage* page        = ZATHURA_PAGE(object);
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(page);

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
              render_page(page);
            }
            break;
          default:
            // TODO: Print warning message
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
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(page);

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
calculate_widget_size(ZathuraPage* page, unsigned int* widget_width,
    unsigned int* widget_height)
{
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(page);

  double scale_factor = priv->settings.scale;

  *widget_width  = round(priv->dimensions.width  * scale_factor);
  *widget_height = round(priv->dimensions.height * scale_factor);
}

static void
render_page(ZathuraPage* widget)
{
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(widget);

  unsigned int page_widget_width;
  unsigned int page_widget_height;

  calculate_widget_size(widget, &page_widget_width, &page_widget_height);

  if (priv->form_fields.edit == true) {
    /* gtk_widget_show(priv->layer.form_fields); */
  } else {
    /* gtk_widget_hide(priv->layer.form_fields); */
  }

  gtk_widget_set_size_request(priv->layer.drawing_area, page_widget_width, page_widget_height);

  gtk_widget_queue_allocate(GTK_WIDGET(widget));
}

static void
cb_page_draw(GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer data)
{
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(data);

  gint device_scale = gtk_widget_get_scale_factor(GTK_WIDGET(area));

  const unsigned int page_width  = gtk_widget_get_allocated_width(GTK_WIDGET(area));
  const unsigned int page_height = gtk_widget_get_allocated_height(GTK_WIDGET(area));

  cairo_save(cairo);

  /* Create image surface */
  cairo_surface_t* image_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, page_width * device_scale, page_height * device_scale);
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
  double scale_factor = priv->settings.scale;

  cairo_save(image_cairo);

  /* Render page */
  if (zathura_page_render_cairo(priv->page, image_cairo, scale_factor * device_scale, 0, 0) != ZATHURA_ERROR_OK) {
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

static gboolean
cb_page_draw_links(GtkWidget* widget, cairo_t *cairo, gpointer data)
{
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(data);

  /* Draw links if requested */
  if (priv->links.draw == true) {
    /* Retrieve links of page */
    if (priv->links.retrieved == false) {
      priv->links.retrieved = true;
      if (zathura_page_get_links(priv->page, &(priv->links.list)) != ZATHURA_ERROR_OK) {
        return FALSE;
      }
    }

    cairo_save(cairo);
    double scale_factor = priv->settings.scale * gtk_widget_get_scale_factor(GTK_WIDGET(widget));

    /* Draw each link */
    zathura_link_mapping_t* link_mapping;
    ZATHURA_LIST_FOREACH(link_mapping, priv->links.list) {
      zathura_rectangle_t position = zathura_rectangle_scale(link_mapping->position, scale_factor);
      unsigned int width  = position.p2.x - position.p1.x;
      unsigned int height = position.p2.y - position.p1.y;

      cairo_set_line_width(cairo, 1);
      cairo_set_source_rgb(cairo, RGB_TO_CAIRO(84, 208, 237));

      cairo_rectangle(cairo, position.p1.x, position.p1.y, width, height);
      cairo_stroke(cairo);
    }

    cairo_restore(cairo);
  }

  return FALSE;
}

static void
cb_page_overlay_realized(GtkWidget* overlay, gpointer data)
{
  ZathuraPagePrivate* priv = zathura_gtk_page_get_instance_private(data);

  if (priv->form_fields.edit == true) {
    gtk_widget_show(priv->layer.form_fields);
  } else {
    gtk_widget_hide(priv->layer.form_fields);
  }
}
