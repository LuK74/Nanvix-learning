#include <nanvix/const.h>
#include <nanvix/pm.h>
#include <signal.h>
#include <sys/sem.h>

/*
 * semid : Id of the semaphore on which we want to apply
 * this command
 * cmd : Command that we want to do
 * val : If we request a SETVAL command, we we'll
 * set ressources with the value "val"
 */
PUBLIC int sys_semctl(int semid, int cmd, int val) {
  switch (cmd) {
  // Returns the number of ressources
  case GETVAL:
    return semtab[semid].ressources;
    break;
  // Set the number of ressources
  case SETVAL:
    semtab[semid].ressources = val;
    break;
  // Destroy the semaphore
  case IPC_RMID:
    destroy(semid);
    break;
  default:
    return -1;
  }

  return 0;
}
