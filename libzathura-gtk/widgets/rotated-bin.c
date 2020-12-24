 /* See LICENSE file for license and copyright information */

#include <stdlib.h>
#include <math.h>
#include <cairo.h>

#include "../macros.h"
#include "rotated-bin.h"

/* forward declaration */
static void zathura_gtk_rotated_bin_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_rotated_bin_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void zathura_gtk_rotated_bin_realize(GtkWidget* widget);
static void zathura_gtk_rotated_bin_unrealize(GtkWidget* widget);
static void zathura_gtk_rotated_bin_get_preferred_width(GtkWidget* widget, gint*
    minimum, gint* natural);
static void zathura_gtk_rotated_bin_get_preferred_height(GtkWidget* widget,
    gint* minimum, gint* natural);
static void zathura_gtk_rotated_bin_size_allocate(GtkWidget* widget,
    int width, int height, int baseline);
/* static gboolean zathura_gtk_rotated_bin_damage(GtkWidget* widget, */
/*     GdkEventExpose* event); */
static gboolean zathura_gtk_rotated_bin_draw(GtkWidget* widget, cairo_t* cairo);
/* static void zathura_gtk_rotated_bin_add(GtkWidget* container, GtkWidget* */
/*     child); */
/* static void zathura_gtk_rotated_bin_remove(GtkWidget* container, GtkWidget* */
/*     widget); */
/* static void zathura_gtk_rotated_bin_forall(GtkWidget* container, gboolean */
/*     include_internals, GtkCallback callback, gpointer callback_data); */
/* static GType zathura_gtk_rotated_bin_child_type(GtkWidget* container); */

struct _ZathuraRotatedBinPrivate {
  GtkWidget* child;
  GdkSurface* offscreen_window;

  struct {
    gdouble angle;
  } settings;
};

enum {
  PROP_0,
  PROP_ANGLE
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraRotatedBin, zathura_gtk_rotated_bin, GTK_TYPE_WIDGET)

static void
zathura_gtk_rotated_bin_class_init(ZathuraRotatedBinClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_rotated_bin_set_property;
  object_class->get_property = zathura_gtk_rotated_bin_get_property;

  /* properties */
  g_object_class_install_property(
    object_class,
    PROP_ANGLE,
    g_param_spec_double(
      "angle",
      "Angle",
      "The angle",
      0.0,
      360.0,
      0.0,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  /* widget class */
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
  /* widget_class->realize              = zathura_gtk_rotated_bin_realize; */
  /* widget_class->unrealize            = zathura_gtk_rotated_bin_unrealize; */
  /* widget_class->get_preferred_width  = zathura_gtk_rotated_bin_get_preferred_width; */
  /* widget_class->get_preferred_height = zathura_gtk_rotated_bin_get_preferred_height; */
  /* widget_class->size_allocate        = zathura_gtk_rotated_bin_size_allocate; */
  /* widget_class->draw                 = zathura_gtk_rotated_bin_draw; */

  /* g_signal_override_class_closure( */
  /*     g_signal_lookup("damage-event", GTK_TYPE_WIDGET), */
  /*     ZATHURA_TYPE_ROTATED_BIN, */
  /*     g_cclosure_new(G_CALLBACK(zathura_gtk_rotated_bin_damage), NULL, NULL) */
  /*   ); */

  /* container class */
  /* GtkWidgetClass* container_class = GTK_CONTAINER_CLASS(class); */
  /* container_class->add        = zathura_gtk_rotated_bin_add; */
  /* container_class->remove     = zathura_gtk_rotated_bin_remove; */
  /* container_class->forall     = zathura_gtk_rotated_bin_forall; */
  /* container_class->child_type = zathura_gtk_rotated_bin_child_type; */

  gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

static void
zathura_gtk_rotated_bin_init(ZathuraRotatedBin* widget)
{
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(widget);

  priv->child            = NULL;
  priv->offscreen_window = NULL;

  priv->settings.angle = 0.0;
}

GtkWidget*
zathura_gtk_rotated_bin_new(void)
{
  GObject* widget = g_object_new(ZATHURA_TYPE_ROTATED_BIN, NULL);

  return GTK_WIDGET(widget);
}

void
zathura_gtk_rotated_bin_set_angle(ZathuraRotatedBin* bin, gdouble angle)
{
  g_return_if_fail (ZATHURA_IS_ROTATED_BIN(bin));
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(bin);

  priv->settings.angle = angle * ((double) G_PI / 180.0);

  gtk_widget_queue_resize(GTK_WIDGET(bin));
  /* gdk_window_geometry_changed(priv->offscreen_window); */
}

static void
zathura_gtk_rotated_bin_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraRotatedBin* bin        = ZATHURA_ROTATED_BIN(object);
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(bin);

  switch (prop_id) {
    case PROP_ANGLE:
      {
        double angle = g_value_get_double(value);
        double new_angle = angle * ((double) G_PI / 180.0);
        if (priv->settings.angle != new_angle) {
          priv->settings.angle = new_angle;
          gtk_widget_queue_resize(GTK_WIDGET(bin));
          /* gdk_window_geometry_changed(priv->offscreen_window); */
        }
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
zathura_gtk_rotated_bin_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec)
{
  ZathuraRotatedBin* page        = ZATHURA_ROTATED_BIN(object);
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(page);

  switch (prop_id) {
    case PROP_ANGLE:
      g_value_set_double(value, priv->settings.angle);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
to_child (ZathuraRotatedBin *bin, double widget_x, double widget_y, double*
    x_out, double* y_out)
{
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(bin);

  double s = sin(priv->settings.angle);
  double c = cos(priv->settings.angle);

  GtkAllocation child_area;
  gtk_widget_get_allocation(priv->child, &child_area);

  double w = c * child_area.width + s * child_area.height;
  double h = s * child_area.width + c * child_area.height;

  w = fabs(w);
  h = fabs(h);

  double x = widget_x;
  double y = widget_y;

  x -= (w - child_area.width) / 2;
  y -= (h - child_area.height) / 2;

  x -= child_area.width / 2;
  y -= child_area.height / 2;

  double xr = x * c + y * s;
  double yr = y * c - x * s;

  x = xr;
  y = yr;

  x += child_area.width / 2;
  y += child_area.height / 2;

  if (x <= 0.0) {
    x += child_area.width;
  }

  if (y <= 0.0) {
    y += child_area.height;
  }

  *x_out = x;
  *y_out = y;
}

static void
to_parent (ZathuraRotatedBin *bin, double offscreen_x, double offscreen_y,
    double* x_out, double* y_out)
{
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(bin);

  double s = sin(priv->settings.angle);
  double c = cos(priv->settings.angle);

  GtkAllocation child_area;
  gtk_widget_get_allocation(priv->child, &child_area);

  double w = c * child_area.width + s * child_area.height;
  double h = s * child_area.width + c * child_area.height;

  double x = offscreen_x;
  double y = offscreen_y;

  x -= child_area.width / 2;
  y -= child_area.height / 2;

  double xr = x * c - y * s;
  double yr = x * s + y * c;

  x = xr;
  y = yr;

  x += child_area.width / 2;
  y += child_area.height / 2;

  x -= (w - child_area.width) / 2;
  y -= (h - child_area.height) / 2;

  *x_out = x;
  *y_out = y;
}

static GdkSurface*
pick_offscreen_child(GdkSurface* UNUSED(offscreen_window), double widget_x,
    double widget_y, ZathuraRotatedBin* bin)
{
  GtkAllocation child_area;
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(bin);

  if (priv->child != NULL && gtk_widget_get_visible(priv->child) == TRUE) {
    double x, y;
    to_child(bin, widget_x, widget_y, &x, &y);

    gtk_widget_get_allocation(priv->child, &child_area);

    if (x >= 0 && x < child_area.width && y >= 0 && y < child_area.height) {
      return priv->offscreen_window;
    }
  }

  return NULL;
}

static void
offscreen_window_to_parent(GdkSurface* UNUSED(offscreen_window), double
    offscreen_x, double offscreen_y, double* parent_x, double* parent_y,
    ZathuraRotatedBin* bin)
{
  to_parent(bin, offscreen_x, offscreen_y, parent_x, parent_y);
}

static void
offscreen_window_from_parent(GdkSurface* UNUSED(window), double parent_x, double
    parent_y, double* offscreen_x, double* offscreen_y, ZathuraRotatedBin* bin)
{
  to_child(bin, parent_x, parent_y, offscreen_x, offscreen_y);
}

static void
zathura_gtk_rotated_bin_realize(GtkWidget* widget)
{
  ZathuraRotatedBin *bin = ZATHURA_ROTATED_BIN(widget);
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(bin);

  /* gtk_widget_set_realized (widget, TRUE); */

  GtkAllocation allocation;
  gtk_widget_get_allocation(widget, &allocation);
  guint border_width = 0; //gtk_container_get_border_width(GTK_CONTAINER(widget));

  /* GdkSurfaceAttr attributes; */
  /* attributes.x = allocation.x + border_width; */
  /* attributes.y = allocation.y + border_width; */
  /* attributes.width = allocation.width - 2 * border_width; */
  /* attributes.height = allocation.height - 2 * border_width; */
  /* attributes.window_type = GDK_WINDOW_CHILD; */
  /* attributes.event_mask = gtk_widget_get_events(widget) */
  /*                       | GDK_EXPOSURE_MASK */
  /*                       | GDK_POINTER_MOTION_MASK */
  /*                       | GDK_BUTTON_PRESS_MASK */
  /*                       | GDK_BUTTON_RELEASE_MASK */
  /*                       | GDK_SCROLL_MASK */
  /*                       | GDK_ENTER_NOTIFY_MASK */
  /*                       | GDK_LEAVE_NOTIFY_MASK; */
  /*  */
  /* attributes.visual = gtk_widget_get_visual(widget); */
  /* attributes.wclass = GDK_INPUT_OUTPUT; */
  /*  */
  /* gint attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL; */
  /*  */
  /* GdkSurface *window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask); */
  /* gtk_widget_set_window(widget, window); */
  /* gdk_window_set_user_data(window, widget); */
  /* g_signal_connect (window, "pick-embedded-child", */
  /*                   G_CALLBACK (pick_offscreen_child), bin); */
  /*  */
  /* attributes.window_type = GDK_WINDOW_OFFSCREEN; */
  /*  */
  /* GtkRequisition child_requisition; */
  /* child_requisition.width = child_requisition.height = 0; */
  /* if (priv->child != NULL && gtk_widget_get_visible(priv->child) == TRUE) { */
  /*   GtkAllocation child_allocation; */
  /*  */
  /*   gtk_widget_get_allocation(priv->child, &child_allocation); */
  /*   attributes.width = child_allocation.width; */
  /*   attributes.height = child_allocation.height; */
  /* } */
  /*  */
  /* priv->offscreen_window = */
  /*   gdk_window_new(gdk_screen_get_root_window(gtk_widget_get_screen(widget)), */
  /*       &attributes, attributes_mask); */
  /*  */
  /* gdk_window_set_user_data(priv->offscreen_window, widget); */
  /*  */
  /* if (priv->child != NULL) { */
  /*   gtk_widget_set_parent_window(priv->child, priv->offscreen_window); */
  /* } */
  /*  */
  /* gdk_offscreen_window_set_embedder(priv->offscreen_window, window); */
  /*  */
  /* g_signal_connect(priv->offscreen_window, "to-embedder", */
  /*     G_CALLBACK(offscreen_window_to_parent), bin); */
  /*  */
  /* g_signal_connect(priv->offscreen_window, "from-embedder", */
  /*     G_CALLBACK(offscreen_window_from_parent), bin); */
  /*  */
  /* gdk_window_show(priv->offscreen_window); */
}

static void
zathura_gtk_rotated_bin_unrealize(GtkWidget* widget)
{
  ZathuraRotatedBin *bin = ZATHURA_ROTATED_BIN(widget);
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(bin);

  /* gdk_window_set_user_data(priv->offscreen_window, NULL); */
  /* gdk_window_destroy(priv->offscreen_window); */
  priv->offscreen_window = NULL;

  GTK_WIDGET_CLASS(zathura_gtk_rotated_bin_parent_class)->unrealize(widget);
}

static void
zathura_gtk_rotated_bin_size_request(GtkWidget* widget, GtkRequisition*
    requisition)
{
  ZathuraRotatedBin *bin = ZATHURA_ROTATED_BIN(widget);
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(bin);

  GtkRequisition child_requisition;
  child_requisition.width = 0;
  child_requisition.height = 0;

  if (priv->child != NULL && gtk_widget_get_visible(priv->child) == TRUE) {
    gtk_widget_get_preferred_size(priv->child, &child_requisition, NULL);
  }

  double s = fabs(sin(priv->settings.angle));
  double c = fabs(cos(priv->settings.angle));
  double w = c * child_requisition.width + s * child_requisition.height;
  double h = s * child_requisition.width + c * child_requisition.height;

  /* guint border_width = gtk_container_get_border_width(GTK_CONTAINER(widget)); */
  /* requisition->width  = border_width * 2 + w; */
  /* requisition->height = border_width * 2 + h; */
}

static void
zathura_gtk_rotated_bin_get_preferred_width(GtkWidget* widget, gint* minimum,
    gint* natural)
{
  GtkRequisition requisition;
  zathura_gtk_rotated_bin_size_request(widget, &requisition);

  *minimum = *natural = requisition.width;
}

static void
zathura_gtk_rotated_bin_get_preferred_height(GtkWidget* widget, gint* minimum,
    gint* natural)
{
  GtkRequisition requisition;
  zathura_gtk_rotated_bin_size_request(widget, &requisition);

  *minimum = *natural = requisition.height;
}

static void
zathura_gtk_rotated_bin_size_allocate(GtkWidget* widget, int width, int height, int baseline)
{
  ZathuraRotatedBin *bin = ZATHURA_ROTATED_BIN(widget);
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(bin);

  /* gtk_widget_set_allocation(widget, allocation); */

  /* guint border_width = gtk_container_get_border_width(GTK_CONTAINER(widget)); */

  gint w = width;// - border_width * 2;
  gint h = height;// - border_width * 2;

  if (gtk_widget_get_realized(widget) == TRUE) {
    /* gdk_window_move_resize(gtk_widget_get_window(widget), */
    /*                         allocation->x + border_width, */
    /*                         allocation->y + border_width, */
    /*                         w, h); */
  }

  if (priv->child != NULL && gtk_widget_get_visible(priv->child) == TRUE)
  {
    GtkRequisition child_requisition;
    GtkAllocation child_allocation;

    double s = sin(priv->settings.angle);
    double c = cos(priv->settings.angle);

    gtk_widget_get_preferred_size(priv->child, &child_requisition, NULL);

    child_allocation.x = 0;
    child_allocation.y = 0;
    child_allocation.height = child_requisition.height;

    if (fabs(c - 0.0) <= FLT_EPSILON) {
      child_allocation.width = h / s;
    } else if (fabs(s - 0.0) <= FLT_EPSILON) {
      child_allocation.width = w / c;
    } else {
      child_allocation.width = MIN((w - s * child_allocation.height) / c,
                                   (h - c * child_allocation.height) / s);
    }

    child_allocation.width  = abs(child_allocation.width);
    child_allocation.height = abs(child_allocation.height);

    if (gtk_widget_get_realized(widget) == TRUE) {
      /* gdk_window_move_resize(priv->offscreen_window, */
      /*                         child_allocation.x, */
      /*                         child_allocation.y, */
      /*                         child_allocation.width, */
      /*                         child_allocation.height); */
    }

    child_allocation.x = child_allocation.y = 0;
    /* gtk_widget_size_allocate(priv->child, &child_allocation); */
  }
}

/* static gboolean */
/* zathura_gtk_rotated_bin_damage(GtkWidget* widget, GdkEventExpose* UNUSED(event)) */
/* { */
/*   gdk_window_invalidate_rect(gtk_widget_get_window(widget), NULL, FALSE); */
/*  */
/*   return TRUE; */
/* } */

static gboolean
zathura_gtk_rotated_bin_draw(GtkWidget* widget, cairo_t* cairo)
{
  ZathuraRotatedBin *bin = ZATHURA_ROTATED_BIN(widget);
  ZathuraRotatedBinPrivate* priv = zathura_gtk_rotated_bin_get_instance_private(bin);

#if 0
  GdkSurface* window = gtk_widget_get_window(widget);
  if (gtk_cairo_should_draw_window (cairo, window)) {
    if (priv->child != NULL && gtk_widget_get_visible(priv->child) == TRUE) {
      GtkAllocation child_area;
      gtk_widget_get_allocation (priv->child, &child_area);

      /* transform */
      double s = fabs(sin(priv->settings.angle));
      double c = fabs(cos(priv->settings.angle));
      double w = c * child_area.width + s * child_area.height;
      double h = s * child_area.width + c * child_area.height;

      cairo_translate(cairo, (w - child_area.width) / 2, (h - child_area.height) / 2);
      cairo_translate(cairo, child_area.width / 2, child_area.height / 2);
      cairo_rotate(cairo, priv->settings.angle);
      cairo_translate(cairo, -child_area.width / 2, -child_area.height / 2);

      /* clip */
      cairo_rectangle(cairo, 0, 0,
          gdk_window_get_width(priv->offscreen_window),
          gdk_window_get_height(priv->offscreen_window));
      cairo_clip(cairo);

      /* paint */
      cairo_surface_t *surface = gdk_offscreen_window_get_surface(priv->offscreen_window);
      cairo_set_source_surface(cairo, surface, 0, 0);
      cairo_paint(cairo);
    }
  }

  if (gtk_cairo_should_draw_window(cairo, priv->offscreen_window)) {
    gtk_render_background(gtk_widget_get_style_context(widget), cairo, 0, 0,
        gdk_window_get_width(priv->offscreen_window),
        gdk_window_get_height(priv->offscreen_window));

    if (priv->child != NULL) {
      gtk_container_propagate_draw(GTK_CONTAINER(widget), priv->child, cairo);
    }
  }
#endif

  return FALSE;
}
