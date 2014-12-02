/* See LICENSE file for license and copyright information */

#include <check.h>
#include <fiu.h>

int main(void)
{
  /* initialize libfiu */
#ifdef FIU_ENABLE
  fiu_init(0);
#endif

  /* setup test suite */
  SRunner* suite_runner = srunner_create(NULL);
  srunner_set_fork_status(suite_runner, CK_NOFORK);

  int number_failed = 0;
  srunner_run_all(suite_runner, CK_ENV);
  number_failed += srunner_ntests_failed(suite_runner);
  srunner_free(suite_runner);

  return (number_failed == 0) ? 0 : 1;
}
