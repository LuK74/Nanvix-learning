#include <sys/sem.h>
#include <nanvix/pm.h>
#include <nanvix/const.h>
#include <signal.h>

PUBLIC int sys_semop(int semid, int op) {
  if (op >= 0) {
    up(semid);
  } else {
    down(semid);
  }
  return 0;
}
