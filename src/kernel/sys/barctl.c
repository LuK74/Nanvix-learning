#include <sys/barrier.h>
#include <nanvix/pm.h>
#include <nanvix/const.h>
#include <signal.h>

/*
* barid : Id of the barrier on which we want to apply
* this command
* cmd : Command that we want to do
* val : If we request a SETVAL command, we we'll
* set request with the value "val"
*/
PUBLIC int sys_barctl(int barid, int cmd, int val) {
  switch (cmd) {
    // Returns the number of requests
    case GETVAL :
      return bartab[barid].request;
      break;
    // Set the number of requests
    case SETVAL :
      bartab[barid].request = val;
      break;
    // Destroy the barrier
    case IPC_RMID :
      destroyB(barid);
      break;
    default :
      return -1;
  }

  return 0;
}
