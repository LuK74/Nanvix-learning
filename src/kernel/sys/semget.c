#include <sys/sem.h>
#include <nanvix/pm.h>
#include <nanvix/const.h>
#include <signal.h>

PUBLIC int sys_semget(unsigned key) {
  for (int i = 0; i < SEM_MAX; i++) {
    if (semtab[i].id != SEM_MAX+1) {
      if (semtab[i].key == key) {
        return semtab[i].id;
      }
    }
  }

  int semId = create(0);
  semtab[semId].key = key;

  if(semtab[semId].id == SEM_MAX+1) return -1;

  return key;
}
