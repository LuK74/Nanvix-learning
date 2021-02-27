#include <nanvix/const.h>
#include <nanvix/pm.h>
#include <signal.h>
#include <sys/sem.h>

/*
 * This function is used to apply severals options
 * on a already existing semaphore
 * (in this case we only have one operation possible)
 */
PUBLIC int sys_semop(int semid, int op) {
  if (op >= 0) {
    up(semid);
  } else {
    down(semid);
  }
  return 0;
}
