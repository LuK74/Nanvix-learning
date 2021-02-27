#include <nanvix/const.h>
#include <nanvix/pm.h>
#include <signal.h>
#include <sys/barrier.h>

/*
 * This function is used to apply severals options
 * on a already existing barrier
 * (in this case we only have one operation possible)
 */
PUBLIC int sys_barop(int barid, int op) {
  if (op >= 0) {
    reach(barid);
  }
  return 0;
}
