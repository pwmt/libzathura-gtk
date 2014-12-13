/* See LICENSE file for license and copyright information */

#include <gtk/gtk.h>
#include <check.h>
#include <fiu.h>

Suite* suite_document(void);

int
main(int argc, char* argv[])
{
  /* initialize libfiu */
#ifdef FIU_ENABLE
  fiu_init(0);
#endif

  /* Initialize GTK */
  gtk_init(&argc, &argv);

  /* setup test suite */
  SRunner* suite_runner = srunner_create(NULL);
  srunner_set_fork_status(suite_runner, CK_NOFORK);

  srunner_add_suite(suite_runner, suite_document());

  int number_failed = 0;
  srunner_run_all(suite_runner, CK_ENV);
  number_failed += srunner_ntests_failed(suite_runner);
  srunner_free(suite_runner);

  return (number_failed == 0) ? 0 : 1;
}
