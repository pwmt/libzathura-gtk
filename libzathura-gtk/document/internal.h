 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_DOCUMENT_INTERNAL
#define ZATHURA_GTK_DOCUMENT_INTERNAL

#include <gtk/gtk.h>
#include "../document.h"

struct _ZathuraDocumentPrivate {
  struct {
    zathura_document_t* document;
    GList* pages;
    GtkWidget* current_page;
  } document;

  struct {
    GtkWidget* scrolled_window;
    GtkWidget* viewport;
    GtkWidget* grid;
  } gtk;

  struct {
    gboolean continuous_pages;
    guint pages_per_row;
  } settings;
};

#endif /* ZATHURA_GTK_DOCUMENT_INTERNAL */
