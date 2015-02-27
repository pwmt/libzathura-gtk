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
    GtkWidget* form_fields;
  } layer;

  struct {
    unsigned int width;
    unsigned int height;
  } dimensions;

  struct {
    guint rotation;
    double scale;
  } settings;

  struct {
    bool retrieved;
    bool draw;
    zathura_list_t* list;
  } links;

  struct {
    bool edit;
    bool retrieved;
    zathura_list_t* list;
  } form_fields;
};

#endif /* ZATHURA_GTK_PAGE_INTERNAL */
