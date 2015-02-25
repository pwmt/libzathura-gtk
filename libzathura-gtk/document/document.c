 /* See LICENSE file for license and copyright information */

#include <math.h>

#include "document.h"
#include "internal.h"
#include "callbacks.h"
#include "grid.h"

#include "../page/page.h"

static void zathura_gtk_document_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec);
static void zathura_gtk_document_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec);
static void set_continuous_pages(ZathuraDocumentPrivate* priv, gboolean enable);
static void set_pages_per_row(ZathuraDocumentPrivate* priv, guint pages_per_row);
static void set_first_page_column(ZathuraDocumentPrivate* priv, guint first_page_column);
static void set_current_page_number(ZathuraDocumentPrivate* priv, guint page_number);
static void restore_current_page(ZathuraDocumentPrivate* priv);

enum {
  PROP_0,
  PROP_DOCUMENT,
  PROP_CURRENT_PAGE_NUMBER,
  PROP_CONTINUOUS_PAGES,
  PROP_PAGES_PER_ROW,
  PROP_FIRST_PAGE_COLUMN,
  PROP_ROTATION,
  PROP_SCALE,
  PROP_SCROLL_STEP,
  PROP_SCROLL_OVERLAP,
  PROP_SCROLL_PAGE_AWARE,
  PROP_SCROLL_WRAP,
  PROP_LINKS_HIGHLIGHT
};

G_DEFINE_TYPE_WITH_PRIVATE(ZathuraDocument, zathura_gtk_document, GTK_TYPE_BIN)

#define ZATHURA_DOCUMENT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), ZATHURA_TYPE_DOCUMENT, \
                               ZathuraDocumentPrivate))

static void
zathura_gtk_document_class_init(ZathuraDocumentClass* class)
{
  /* overwrite methods */
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  object_class->set_property = zathura_gtk_document_set_property;
  object_class->get_property = zathura_gtk_document_get_property;

  /* properties */
  g_object_class_install_property(
    object_class,
    PROP_DOCUMENT,
    g_param_spec_pointer(
      "document",
      "Document",
      "The zathura_document_t instance",
      G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_CURRENT_PAGE_NUMBER,
    g_param_spec_uint(
      "current-page-number",
      "current-page-number",
      "Defines the current page number",
      0,
      G_MAXUINT,
      0,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_CONTINUOUS_PAGES,
    g_param_spec_boolean(
      "continuous-pages",
      "Continuous-pages",
      "Enables continuous pages",
      TRUE,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_PAGES_PER_ROW,
    g_param_spec_uint(
      "pages-per-row",
      "pages-per-row",
      "Defines the number of pages per row",
      1,
      G_MAXUINT,
      1,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_FIRST_PAGE_COLUMN,
    g_param_spec_uint(
      "first-page-column",
      "first-page-column",
      "Defines the column of the first page",
      1,
      G_MAXUINT,
      1,
      G_PARAM_WRITABLE | G_PARAM_READABLE
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
    PROP_SCROLL_STEP,
    g_param_spec_uint(
      "scroll-step",
      "Scroll step",
      "Number of pixels that should be scrolled per step",
      1,
      G_MAXUINT,
      40,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_SCROLL_OVERLAP,
    g_param_spec_double(
      "scroll-full-overlap",
      "Scroll-full-overlap",
      "Optional overlap if full scroll actions are performed",
      0,
      0.99,
      0.0,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_SCROLL_PAGE_AWARE,
    g_param_spec_boolean(
      "scroll-page-aware",
      "Scroll-page-aware",
      "Enables page aware scrolling",
      FALSE,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_SCROLL_WRAP,
    g_param_spec_boolean(
      "scroll-wrap",
      "Scroll-wrap",
      "Enables scroll wrapping",
      FALSE,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );

  g_object_class_install_property(
    object_class,
    PROP_LINKS_HIGHLIGHT,
    g_param_spec_boolean(
      "highlight-links",
      "Highlight-links",
      "Highlight links by drawing rectangles around them",
      FALSE,
      G_PARAM_WRITABLE | G_PARAM_READABLE
    )
  );
}

static void
zathura_gtk_document_init(ZathuraDocument* widget)
{
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(widget);

  priv->document.document        = NULL;
  priv->document.pages           = NULL;
  priv->document.pages_status    = NULL;
  priv->document.number_of_pages = 0;

  priv->gtk.scrolled_window = NULL;
  priv->gtk.viewport        = NULL;
  priv->gtk.grid            = NULL;

  priv->settings.continuous_pages    = TRUE;
  priv->settings.pages_per_row       = 1;
  priv->settings.first_page_column   = 1;
  priv->settings.rotation            = 0;
  priv->settings.scale               = 1.0;
  priv->settings.scroll.step         = 40;
  priv->settings.scroll.full_overlap = 0.0;
  priv->settings.scroll.page_aware   = false;
  priv->settings.scroll.wrap         = false;
  priv->settings.links.highlight     = false;

  priv->position.x = 0.0;
  priv->position.y = 0.0;

  priv->status.restore_position = FALSE;
}

GtkWidget*
zathura_gtk_document_new(zathura_document_t* document)
{
  if (document == NULL) {
    return NULL;
  }

  GObject* widget = g_object_new(ZATHURA_TYPE_DOCUMENT, "document", document, NULL);

  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(widget);

  /* Setup scrolled window */
  priv->gtk.scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (priv->gtk.scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  g_signal_connect(
      G_OBJECT(priv->gtk.scrolled_window),
      "draw",
      G_CALLBACK(cb_grid_draw),
      priv);

  GtkAdjustment* horizontal_adjustment = gtk_scrolled_window_get_hadjustment(
      GTK_SCROLLED_WINDOW(priv->gtk.scrolled_window)
      );

  g_signal_connect(
      G_OBJECT(horizontal_adjustment),
      "value-changed",
      G_CALLBACK(cb_scrolled_window_horizontal_adjustment_value_changed),
      priv);

  g_signal_connect(
      G_OBJECT(horizontal_adjustment),
      "changed",
      G_CALLBACK(cb_scrolled_window_horizontal_adjustment_changed),
      priv);

  GtkAdjustment* vertical_adjustment = gtk_scrolled_window_get_vadjustment(
      GTK_SCROLLED_WINDOW(priv->gtk.scrolled_window)
      );

  g_signal_connect(
      G_OBJECT(vertical_adjustment),
      "value-changed",
      G_CALLBACK(cb_scrolled_window_vertical_adjustment_value_changed),
      priv);

  g_signal_connect(
      G_OBJECT(vertical_adjustment),
      "changed",
      G_CALLBACK(cb_scrolled_window_vertical_adjustment_changed),
      priv);

  /* Setup viewport */
  priv->gtk.viewport = gtk_viewport_new(NULL, NULL);

  /* Create page widgets */
  if (zathura_document_get_number_of_pages(document, &(priv->document.number_of_pages)) !=
      ZATHURA_ERROR_OK) {
    g_object_unref(priv->gtk.scrolled_window);
    g_object_unref(priv->gtk.viewport);
    g_object_unref(widget);
    return NULL;
  }

  for (unsigned int i = 0; i < priv->document.number_of_pages; i++) {
    zathura_page_t* page;
    if (zathura_document_get_page(document, i, &page) != ZATHURA_ERROR_OK) {
      g_object_unref(priv->gtk.scrolled_window);
      g_object_unref(priv->gtk.viewport);
      g_object_unref(widget);
      g_list_foreach(priv->document.pages, (GFunc) g_object_unref, NULL);
      g_list_foreach(priv->document.pages, (GFunc) g_free, NULL);
      return NULL;
    }

    /* Create page widget */
    GtkWidget* page_widget = zathura_gtk_page_new(page);
    gtk_widget_set_halign(page_widget, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(page_widget, GTK_ALIGN_CENTER);

    /* Create new page status */
    zathura_gtk_page_widget_status_t* page_widget_status =
      g_malloc0(sizeof(zathura_gtk_page_widget_status_t));
    page_widget_status->visible = false;

    /* Append to list */
    priv->document.pages        = g_list_append(priv->document.pages, page_widget);
    priv->document.pages_status = g_list_append(priv->document.pages_status, page_widget_status);
  }

  /* Setup grid */
  zathura_gtk_setup_grid(priv);

  /* Setup container */
  gtk_container_add(GTK_CONTAINER(priv->gtk.scrolled_window), GTK_WIDGET(priv->gtk.viewport));
  gtk_container_add(GTK_CONTAINER(priv->gtk.viewport), GTK_WIDGET(priv->gtk.grid));
  gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(priv->gtk.scrolled_window));

  return GTK_WIDGET(widget);
}

void
zathura_gtk_document_scroll(GtkWidget* document, zathura_gtk_document_scroll_direction_t direction)
{
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(document);

  /* Current position */
  gdouble position_x = priv->position.x;
  gdouble position_y = priv->position.y;

  /* Go to top or go to bottom */
  if (direction == TOP) {
    zathura_gtk_grid_set_position(priv, position_x, 0);
    return;
  } else if (direction == BOTTOM) {
    zathura_gtk_grid_set_position(priv, position_x, G_MAXDOUBLE);
    return;
  }

  /* Current properties */
  const double vertical_step   = (double) gtk_widget_get_allocated_width(priv->gtk.viewport);
  const double horizontal_step = (double) gtk_widget_get_allocated_height(priv->gtk.viewport);

  const double grid_width  = (double) gtk_widget_get_allocated_width(priv->gtk.grid);
  const double grid_height = (double) gtk_widget_get_allocated_height(priv->gtk.grid);

  /* Go to top or bottom of current page */
  if (direction == PAGE_TOP || direction == PAGE_BOTTOM) {
    zathura_gtk_page_widget_status_t* widget_status =
      g_list_nth_data(priv->document.pages_status, priv->document.current_page_number);

    if (direction == PAGE_TOP) {
      position_y = widget_status->position.y;
    } else if (direction == PAGE_BOTTOM){
      position_y = widget_status->position.y + widget_status->size.height - horizontal_step;
    }

    zathura_gtk_grid_set_position(priv, position_x, position_y);
    return;
  }

  /* Settings */
  gdouble scroll_step         = priv->settings.scroll.step;
  gdouble scroll_full_overlap = priv->settings.scroll.full_overlap;

  /* Adjust direction */
  gdouble scroll_direction = 1.0;
  switch (direction) {
    case LEFT:
    case UP:
    case FULL_LEFT:
    case FULL_UP:
    case HALF_LEFT:
    case HALF_UP:
      scroll_direction = -1.0;
      break;
    default:
      break;
  }

  /* Calculate new position */
  switch (direction) {
    case UP:
    case DOWN:
      position_y += scroll_direction * scroll_step;
      break;
    case RIGHT:
    case LEFT:
      position_x += scroll_direction * scroll_step;
      break;

    case FULL_UP:
    case FULL_DOWN:
      position_y += scroll_direction * (1.0 - scroll_full_overlap) * vertical_step;
      break;
    case FULL_RIGHT:
    case FULL_LEFT:
      position_x += scroll_direction * (1.0 - scroll_full_overlap) * horizontal_step;
      break;

    case HALF_UP:
    case HALF_DOWN:
      position_y += scroll_direction * 0.5 * vertical_step;
      break;
    case HALF_RIGHT:
    case HALF_LEFT:
      position_x += scroll_direction * 0.5 * horizontal_step;
      break;

    default:
      break;
  }

  /* Scroll-wrap */
  if (priv->settings.scroll.wrap == true) {
    if ((position_y + horizontal_step) > grid_height) {
      position_y = 0;
    } else if (position_y < 0) {
      position_y = G_MAXDOUBLE;
    }

    if ((position_x + vertical_step) > grid_width) {
      position_x = 0;
    } else if (position_x < 0) {
      position_x = G_MAXDOUBLE;
    }
  }

  /* Page-aware scrolling */
  if (priv->settings.scroll.page_aware == true) {
    unsigned int new_current_page = zathura_gtk_grid_position_to_page_number(priv, position_x, position_y);

    switch (direction) {
      case FULL_UP:
      case HALF_UP:
        zathura_gtk_grid_page_number_to_position(priv, new_current_page, 0.0, 0.0, NULL, &position_y);
        break;
      case FULL_DOWN:
      case HALF_DOWN:
        zathura_gtk_grid_page_number_to_position(priv, new_current_page, 0.0, 1.0, NULL, &position_y);
        position_y -= horizontal_step;
        break;
      case FULL_LEFT:
      case HALF_LEFT:
        zathura_gtk_grid_page_number_to_position(priv, new_current_page, 1.0, 0.0, &position_x, NULL);
        position_x -= vertical_step;
        break;
      case FULL_RIGHT:
      case HALF_RIGHT:
        zathura_gtk_grid_page_number_to_position(priv, new_current_page, 0.0, 0.0, &position_x, NULL);
        break;
      default:
        break;
    }
  }

  /* Update position */
  zathura_gtk_grid_set_position(priv, position_x, position_y);
}

static void
zathura_gtk_document_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* param_spec)
{
  ZathuraDocument* document    = ZATHURA_DOCUMENT(object);
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(document);

  switch (prop_id) {
    case PROP_DOCUMENT:
      priv->document.document = g_value_get_pointer(value);
      break;
    case PROP_CURRENT_PAGE_NUMBER:
      set_current_page_number(priv, g_value_get_uint(value));
      break;
    case PROP_CONTINUOUS_PAGES:
      set_continuous_pages(priv, g_value_get_boolean(value));
      break;
    case PROP_PAGES_PER_ROW:
      set_pages_per_row(priv, g_value_get_uint(value));
      break;
    case PROP_FIRST_PAGE_COLUMN:
      set_first_page_column(priv, g_value_get_uint(value));
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
              g_list_foreach(priv->document.pages, (GFunc) cb_document_pages_set_rotation, priv);
            }
            break;
          default:
            // TODO: Pring warning message
            break;
        }
        restore_current_page(priv);
      }
      break;
    case PROP_SCALE:
      {
        double scale = g_value_get_double(value);
        if (priv->settings.scale != scale) {
          priv->settings.scale = scale;
          g_list_foreach(priv->document.pages, (GFunc) cb_document_pages_set_scale, priv);
        }
      }
      break;
    case PROP_SCROLL_STEP:
      priv->settings.scroll.step = g_value_get_uint(value);
      break;
    case PROP_SCROLL_OVERLAP:
      priv->settings.scroll.full_overlap = g_value_get_double(value);
      break;
    case PROP_SCROLL_PAGE_AWARE:
      priv->settings.scroll.page_aware = g_value_get_boolean(value);
      break;
    case PROP_SCROLL_WRAP:
      priv->settings.scroll.wrap = g_value_get_boolean(value);
      break;
    case PROP_LINKS_HIGHLIGHT:
      {
        gboolean highlight = g_value_get_boolean(value);
        if (priv->settings.links.highlight != highlight) {
          priv->settings.links.highlight = highlight;
          g_list_foreach(priv->document.pages,
              (GFunc) cb_document_pages_set_highlight_links,
              priv
            );
        }
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
zathura_gtk_document_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* param_spec)
{
  ZathuraDocument* document    = ZATHURA_DOCUMENT(object);
  ZathuraDocumentPrivate* priv = ZATHURA_DOCUMENT_GET_PRIVATE(document);

  switch (prop_id) {
    case PROP_DOCUMENT:
      g_value_set_pointer(value, priv->document.document);
      break;
    case PROP_CURRENT_PAGE_NUMBER:
      g_value_set_uint(value, priv->document.current_page_number);
      break;
    case PROP_CONTINUOUS_PAGES:
      g_value_set_boolean(value, priv->settings.continuous_pages);
      break;
    case PROP_PAGES_PER_ROW:
      g_value_set_uint(value, priv->settings.pages_per_row);
      break;
    case PROP_FIRST_PAGE_COLUMN:
      g_value_set_uint(value, priv->settings.first_page_column);
      break;
    case PROP_ROTATION:
      g_value_set_uint(value, priv->settings.rotation);
      break;
    case PROP_SCALE:
      g_value_set_double(value, priv->settings.scale);
      break;
    case PROP_SCROLL_STEP:
      g_value_set_uint(value, priv->settings.scroll.step);
      break;
    case PROP_SCROLL_OVERLAP:
      g_value_set_double(value, priv->settings.scroll.full_overlap);
      break;
    case PROP_SCROLL_PAGE_AWARE:
      g_value_set_boolean(value, priv->settings.scroll.page_aware);
      break;
    case PROP_SCROLL_WRAP:
      g_value_set_boolean(value, priv->settings.scroll.wrap);
      break;
    case PROP_LINKS_HIGHLIGHT:
      g_value_set_boolean(value, priv->settings.links.highlight);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
  }
}

static void
set_continuous_pages(ZathuraDocumentPrivate* priv, gboolean enable)
{
  priv->settings.continuous_pages = enable;

  zathura_gtk_clear_grid(priv);
  zathura_gtk_fill_grid(priv);

  if (priv->settings.continuous_pages == true) {
    restore_current_page(priv);
  }
}

static void
set_pages_per_row(ZathuraDocumentPrivate* priv, guint pages_per_row)
{
  /* Update pages per row */
  priv->settings.pages_per_row = pages_per_row;

  /* Empty and refill grid */
  zathura_gtk_clear_grid(priv);
  zathura_gtk_fill_grid(priv);
}

static void
set_first_page_column(ZathuraDocumentPrivate* priv, guint first_page_column)
{
  /* Update pages per row */
  if (first_page_column < 1) {
    first_page_column = 1;
  } else if (first_page_column > priv->settings.pages_per_row) {
    first_page_column = ((first_page_column - 1) % priv->settings.pages_per_row) + 1;
  }

  priv->settings.first_page_column = first_page_column;

  /* Empty and refill grid */
  zathura_gtk_clear_grid(priv);
  zathura_gtk_fill_grid(priv);
}

static void
set_current_page_number(ZathuraDocumentPrivate* priv, guint page_number)
{
  if (page_number >= priv->document.number_of_pages) {
    return;
  }

  priv->document.current_page_number = page_number;
  zathura_gtk_grid_set_page(priv, page_number);
}

static void
restore_current_page(ZathuraDocumentPrivate* priv)
{
  GtkWidget* page_widget = g_list_nth_data(priv->document.pages, priv->document.current_page_number);

  zathura_page_info_t* page_info = g_malloc0(sizeof(zathura_page_info_t));

  /* Calculate offset */
  int page_widget_offset_x, page_widget_offset_y;
  if (gtk_widget_translate_coordinates(page_widget, priv->gtk.viewport, 0,
      0, &page_widget_offset_x, &page_widget_offset_y) == FALSE) {
    return; // Should not happen!
  };

  page_info->priv = priv;
  page_info->page_number = priv->document.current_page_number;
  page_info->offset.x = page_widget_offset_x,
  page_info->offset.y = page_widget_offset_y;

  priv->status.restore_position = page_info;
}
