 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_DOCUMENT_INTERNAL
#define ZATHURA_GTK_DOCUMENT_INTERNAL

#include <gtk/gtk.h>
#include "document.h"

typedef struct zathura_gtk_page_widget_status_s {
  struct {
    int x;
    int y;
  } position;

  bool visible;
} zathura_gtk_page_widget_status_t;

typedef struct zathura_page_info_s zathura_page_info_t;

struct _ZathuraDocumentPrivate {
  struct {
    zathura_document_t* document;
    GList* pages;
    GList* pages_status;
    unsigned int number_of_pages;
    unsigned int current_page_number;
  } document;

  struct {
    GtkWidget* scrolled_window;
    GtkWidget* viewport;
    GtkWidget* grid;

    struct {
      guint refresh_view;
    } signals;
  } gtk;

  struct {
    double x;
    double y;
  } position;

  struct {
    gboolean continuous_pages;
    guint pages_per_row;
    guint first_page_column;
    guint rotation;
    double scale;
  } settings;

  struct {
    zathura_page_info_t* restore_position;
  } status;
};

struct zathura_page_info_s {
  ZathuraDocumentPrivate* priv;
  int page_number;
  struct {
    unsigned int x;
    unsigned int y;
  } offset;
};

#endif /* ZATHURA_GTK_DOCUMENT_INTERNAL */
