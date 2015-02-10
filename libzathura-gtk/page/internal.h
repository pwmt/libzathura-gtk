 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_PAGE_INTERNAL
#define ZATHURA_GTK_PAGE_INTERNAL

#include <gtk/gtk.h>
#include "page.h"

struct _ZathuraPagePrivate {
  zathura_page_t* page;
  GtkWidget* overlay;

  struct {
    GtkWidget* drawing_area;
    GtkWidget* links;
  } layer;

  struct {
    unsigned int width;
    unsigned int height;
  } dimensions;

  struct {
    guint rotation;
    double scale;
  } settings;
};

#endif /* ZATHURA_GTK_PAGE_INTERNAL */
