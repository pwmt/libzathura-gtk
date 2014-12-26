 /* See LICENSE file for license and copyright information */

#include "page.h"

static void zathura_gtk_page_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_page_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);

struct _ZathuraPagePrivate {
  zathura_page_t* page;
  GtkWidget* overlay;
  GtkWidget* drawing_area;
};

enum {
  PROP_0,
  PROP_PAGE
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraPage, zathura_gtk_page, GTK_TYPE_BIN)

#define ZATHURA_PAGE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_PAGE, \
                               ZathuraPagePrivate))

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
}

static void
zathura_gtk_page_init(ZathuraPage* widget)
{
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(widget);
  priv->page         = NULL;
  priv->drawing_area = NULL;
  priv->overlay      = NULL;
}

gboolean
cb_draw(GtkWidget *widget, cairo_t *cairo, gpointer data)
{
  ZathuraPagePrivate* priv = (ZathuraPagePrivate*) data;

  const unsigned int page_height = gtk_widget_get_allocated_height(widget);
  const unsigned int page_width  = gtk_widget_get_allocated_width(widget);

  cairo_save(cairo);

  cairo_set_source_rgb(cairo, 255, 255, 255);
  cairo_rectangle(cairo, 0, 0, page_width, page_height);
  cairo_fill(cairo);

  if (zathura_page_render_cairo(priv->page, cairo, 1.0, 0, 0) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  cairo_set_operator(cairo, CAIRO_OPERATOR_DEST_OVER);
  cairo_paint(cairo);
  cairo_restore(cairo);

  return FALSE;
}

GtkWidget*
zathura_gtk_page_new(zathura_page_t* page)
{
  if (page == NULL) {
    return NULL;
  }

  GObject* widget = g_object_new(ZATHURA_TYPE_PAGE, "page", page, NULL);
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(widget);

  unsigned int page_width = 0;
  if (zathura_page_get_width(page, &page_width) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  unsigned int page_height = 0;
  if (zathura_page_get_height(page, &page_height) != ZATHURA_ERROR_OK) {
    return NULL;
  }

  /* Setup over lay */
  priv->overlay = gtk_overlay_new();

  /* Setup drawing area */
  priv->drawing_area = gtk_drawing_area_new();
  gtk_widget_set_halign(priv->drawing_area, GTK_ALIGN_START);
  gtk_widget_set_valign(priv->drawing_area, GTK_ALIGN_START);
  gtk_widget_set_size_request(priv->drawing_area, page_width, page_height);
  g_signal_connect(G_OBJECT(priv->drawing_area), "draw", G_CALLBACK(cb_draw), priv);

  /* Setup container */
  gtk_container_add(GTK_CONTAINER(priv->overlay), GTK_WIDGET(priv->drawing_area));
  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->overlay));

  gtk_widget_show_all(GTK_WIDGET(widget));

  return GTK_WIDGET(widget);
}

static void
zathura_gtk_page_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraPage* page    = ZATHURA_PAGE(object);
  ZathuraPagePrivate* priv = ZATHURA_PAGE_GET_PRIVATE(page);

  switch (prop_id) {
    case PROP_PAGE:
      priv->page = g_value_get_pointer(value);
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
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}
