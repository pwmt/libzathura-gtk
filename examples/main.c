 /* See LICENSE file for license and copyright information */

#include <gtk/gtk.h>
#include <libzathura-gtk/libzathura-gtk.h>

int main(int argc, char* argv[])
{
  /* Initialize GTK */
  gtk_init(&argc, &argv);

  /* Setup plugin manager */
  zathura_plugin_manager_t* plugin_manager;
  if (zathura_plugin_manager_new(&plugin_manager) != ZATHURA_ERROR_OK) {
    return -1;
  }

  /* Load plugin from file */
  if (zathura_plugin_manager_load(plugin_manager, "../tests/plugin/plugin.so") != ZATHURA_ERROR_OK) {
    zathura_plugin_manager_free(plugin_manager);
    return -1;
  }

  /* Get plugin with corresponding mime type */
  zathura_plugin_t* plugin = NULL;
  if (zathura_plugin_manager_get_plugin(plugin_manager, &plugin,
        "libzathura-gtk/test-plugin") != ZATHURA_ERROR_OK) {
    zathura_plugin_manager_free(plugin_manager);
    return -1;
  }

  /* Open document  */
  zathura_document_t* document;
  if (zathura_plugin_open_document(plugin, &document, "Makefile", NULL) != ZATHURA_ERROR_OK) {
    zathura_plugin_manager_free(plugin_manager);
    return -1;
  }

  /* Create widget from document */
  GtkWidget* document_widget = zathura_gtk_document_new(document);

  /* Setup window and widget */
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "libzathura-gtk example");
  gtk_container_add(GTK_CONTAINER(window), document_widget);

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
