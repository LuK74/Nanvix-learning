#include <sys/barrier.h>
#include <nanvix/pm.h>
#include <nanvix/const.h>
#include <signal.h>

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
