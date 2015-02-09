 /* See LICENSE file for license and copyright information */

#include <stdio.h>

#include <gtk/gtk.h>
#include <libzathura-gtk/libzathura-gtk.h>

#ifndef UNUSED
# if defined(__GNUC__) || defined(__clang__)
#  define UNUSED(x) UNUSED_ ## x __attribute__((unused))
# elif defined(__LCLINT__)
#  define UNUSED(x) /*@unused@*/ x
# else
#  define UNUSED(x) x
# endif
#endif

gboolean cb_key_press_event(GtkWidget* UNUSED(widget), GdkEventKey* event, gpointer data)
{
  GtkWidget* document = (GtkWidget*) data;

  if (event->state == 0) {
    switch(event->keyval) {
      case GDK_KEY_q:
        gtk_main_quit();
        break;
      case GDK_KEY_c:
        {
          gboolean continuous_pages;
          g_object_get(G_OBJECT(document), "continuous-pages", &continuous_pages, NULL);
          g_object_set(G_OBJECT(document), "continuous-pages", !continuous_pages, NULL);
        }
        break;
      case GDK_KEY_d:
        {
          guint pages_per_row;;
          g_object_get(G_OBJECT(document), "pages-per-row", &pages_per_row, NULL);

          if (pages_per_row == 1) {
            pages_per_row = 2;
          } else {
            pages_per_row = 1;
          }

          g_object_set(G_OBJECT(document), "pages-per-row", pages_per_row, NULL);
        }
        break;
      case GDK_KEY_r:
        {
          guint rotation;
          g_object_get(G_OBJECT(document), "rotation", &rotation, NULL);
          rotation = (rotation + 90) % 360;
          g_object_set(G_OBJECT(document), "rotation", rotation, NULL);
        }
        break;
      case GDK_KEY_plus:
        {
          double scale;
          g_object_get(G_OBJECT(document), "scale", &scale, NULL);
          scale *= 1.2;
          g_object_set(G_OBJECT(document), "scale", scale, NULL);
        }
        break;
      case GDK_KEY_minus:
        {
          double scale;
          g_object_get(G_OBJECT(document), "scale", &scale, NULL);
          scale *= 0.9;
          g_object_set(G_OBJECT(document), "scale", scale, NULL);
        }
        break;
      case GDK_KEY_J:
        {
          guint current_page_number;
          g_object_get(G_OBJECT(document), "current-page-number", &current_page_number, NULL);
          current_page_number += 1;
          g_object_set(G_OBJECT(document), "current-page-number", current_page_number, NULL);
        }
        break;
      case GDK_KEY_K:
        {
          guint current_page_number;
          g_object_get(G_OBJECT(document), "current-page-number", &current_page_number, NULL);
          if (current_page_number != 0) {
            current_page_number -= 1;
            g_object_set(G_OBJECT(document), "current-page-number", current_page_number, NULL);
          }
        }
        break;
      case GDK_KEY_h:
        zathura_gtk_document_scroll(document, LEFT);
        break;
      case GDK_KEY_j:
        zathura_gtk_document_scroll(document, DOWN);
        break;
      case GDK_KEY_k:
        zathura_gtk_document_scroll(document, UP);
        break;
      case GDK_KEY_l:
        zathura_gtk_document_scroll(document, RIGHT);
        break;
      case GDK_KEY_t:
        zathura_gtk_document_scroll(document, FULL_LEFT);
        break;
      case GDK_KEY_y:
        zathura_gtk_document_scroll(document, FULL_RIGHT);
        break;
      case GDK_KEY_g:
        zathura_gtk_document_scroll(document, TOP);
        break;
    }
  } else if (event->state == GDK_CONTROL_MASK) {
    switch(event->keyval) {
      case GDK_KEY_f:
        zathura_gtk_document_scroll(document, FULL_DOWN);
        break;
      case GDK_KEY_b:
        zathura_gtk_document_scroll(document, FULL_UP);
        break;
      case GDK_KEY_t:
        zathura_gtk_document_scroll(document, HALF_LEFT);
        break;
      case GDK_KEY_d:
        zathura_gtk_document_scroll(document, HALF_DOWN);
        break;
      case GDK_KEY_u:
        zathura_gtk_document_scroll(document, HALF_UP);
        break;
      case GDK_KEY_y:
        zathura_gtk_document_scroll(document, HALF_RIGHT);
        break;
    }
  } else if (event->state == GDK_SHIFT_MASK) {
    switch(event->keyval) {
      case GDK_KEY_G:
        zathura_gtk_document_scroll(document, BOTTOM);
        break;
    }
  }

  return TRUE;
}

int main(int argc, char* argv[])
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <file>\n", argv[0]);
    return 0;
  }

  /* Initialize GTK */
  gtk_init(&argc, &argv);

  /* Setup plugin manager */
  zathura_plugin_manager_t* plugin_manager;
  if (zathura_plugin_manager_new(&plugin_manager) != ZATHURA_ERROR_OK) {
    return -1;
  }

  /* Load plugin from file */
  if (zathura_plugin_manager_load_dir(plugin_manager, "/usr/lib/zathura/") != ZATHURA_ERROR_OK) {
    zathura_plugin_manager_free(plugin_manager);
    return -1;
  }

  /* Get plugin with corresponding mime type */
  zathura_plugin_t* plugin = NULL;
  if (zathura_plugin_manager_get_plugin(plugin_manager, &plugin,
        "application/pdf") != ZATHURA_ERROR_OK) {
    zathura_plugin_manager_free(plugin_manager);
    return -1;
  }

  /* Open document  */
  zathura_document_t* document;
  if (zathura_plugin_open_document(plugin, &document, argv[1], NULL) != ZATHURA_ERROR_OK) {
    zathura_plugin_manager_free(plugin_manager);
    return -1;
  }

  /* Create widget from document */
  GtkWidget* document_widget = zathura_gtk_document_new(document);

  /* Setup window and widget */
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "libzathura-gtk example");
  gtk_container_add(GTK_CONTAINER(window), document_widget);

  g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(cb_key_press_event), document_widget);

  gtk_widget_show_all(window);

  /* Main loop */
  gtk_main();

  /* clean-up */
  gtk_widget_destroy(document_widget);

  zathura_document_free(document);
  zathura_plugin_manager_free(plugin_manager);

  return 0;
}
