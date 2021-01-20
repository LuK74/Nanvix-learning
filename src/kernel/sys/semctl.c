#include <sys/sem.h>
#include <nanvix/pm.h>
#include <nanvix/const.h>
#include <signal.h>

PUBLIC int sys_semctl(int semid, int cmd, int val) {
  switch (cmd) {
    case GETVAL :
      return semtab[semid].ressources;
      break;
    case SETVAL :
      semtab[semid].ressources = val;
      break;
    case IPC_RMID :
      destroy(semid);
      break;
    default :
      return -1;
  }

  return 0;
}
