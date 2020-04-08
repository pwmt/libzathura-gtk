/* See LICENSE file for license and copyright information */

#include <check.h>
#include <fiu.h>
#include <fiu-control.h>

#include <libzathura-gtk/libzathura-gtk.h>
#include "utils.h"

zathura_page_t* page;
zathura_document_t* document;
zathura_plugin_manager_t* plugin_manager;

Suite* create_suite(void);

static void setup_page(void) {
  fail_unless(zathura_plugin_manager_new(&plugin_manager) == ZATHURA_ERROR_OK);
  fail_unless(plugin_manager != NULL);
  fail_unless(zathura_plugin_manager_load(plugin_manager, get_plugin_path()) == ZATHURA_ERROR_OK);

  zathura_plugin_t* plugin = NULL;
  fail_unless(zathura_plugin_manager_get_plugin(plugin_manager, &plugin, "libzathura-gtk/test-plugin") == ZATHURA_ERROR_OK);
  fail_unless(plugin != NULL);

  fail_unless(zathura_plugin_open_document(plugin, &document, get_plugin_test_file_path(), NULL) == ZATHURA_ERROR_OK);
  fail_unless(document != NULL);

  fail_unless(zathura_document_get_page(document, 0, &page) == ZATHURA_ERROR_OK);
  fail_unless(page != NULL);
}

static void teardown_page(void) {
  fail_unless(zathura_document_free(document) == ZATHURA_ERROR_OK);
  document = NULL;

  fail_unless(zathura_plugin_manager_free(plugin_manager) == ZATHURA_ERROR_OK);
  plugin_manager = NULL;
}

START_TEST(test_page_new) {
  /* basic invalid arguments */
  fail_unless(zathura_gtk_page_new(NULL) == NULL);

  /* basic invalid arguments */
  GtkWidget* widget = zathura_gtk_page_new(page);
  fail_unless(widget != NULL);

  zathura_page_t* page_tmp = NULL;
  g_object_get(widget, "page", &page_tmp, NULL);
  fail_unless(page == page_tmp);
} END_TEST

Suite*
create_suite(void)
{
  TCase* tcase = NULL;
  Suite* suite = suite_create("page");

  tcase = tcase_create("basic");
  tcase_add_checked_fixture(tcase, setup_page, teardown_page);
  tcase_add_test(tcase, test_page_new);
  suite_add_tcase(suite, tcase);

  return suite;
}
