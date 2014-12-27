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
  zathura_document_free(document);
  zathura_plugin_manager_free(plugin_manager);

  return 0;
}
