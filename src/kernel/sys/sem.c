/*
* CHALOYARD Lucas
* Semaphore implementation
*
*/

#include <sys/sem.h>
#include <nanvix/pm.h>

PUBLIC struct semaphore semtab[SEM_MAX];

PUBLIC int create(int ressources) {

  for (unsigned int i = 0; i < SEM_MAX; i++) {
    if (semtab[i].id == SEM_MAX+1) {
      semtab[i].id = i;
      semtab[i].key = 0;
      semtab[i].ressources = ressources;

      for (unsigned int j = 0; j < MAX_PROCESS_WAITING; j++) {
        semtab[i].waitingProcess[j] = NULL;
      }
      return semtab[i].id;
    }
  }

  return -1;
}

PUBLIC void destroy(int semId) {
  if (semId >= SEM_MAX || semtab[semId].id==SEM_MAX+1) {
    return;
  }

  semtab[semId].key = 0;
  semtab[semId].id = SEM_MAX+1;

  for (unsigned int j = 0; j < MAX_PROCESS_WAITING; j++) {
    semtab[semId].waitingProcess[semId] = NULL;
  }
}

PUBLIC void up(int semId) {
  if (semId >= SEM_MAX || semtab[semId].id==SEM_MAX+1) {
    return;
  }

  disable_interrupts();
  if (semtab[semId].ressources == 0) {
    struct process * wakeupProc = semtab[semId].waitingProcess[0];

    if (wakeupProc != NULL) {
      sndsig(wakeupProc, SIGCONT);
      //resume(wakeupProc);

      unsigned int i ;
      for (i = 0; i < (SEM_MAX-1) && semtab[i].id != SEM_MAX+1; i++) {
        semtab[i] = semtab[i+1];
      }

      if (i < SEM_MAX) {
        semtab[i].id = SEM_MAX+1;
      }

    }

  } else {

    semtab[semId].ressources++;

  }
  enable_interrupts();
}

PUBLIC void down(int semId) {
  if (semId >= SEM_MAX || semtab[semId].id==SEM_MAX+1) {
    return;
  }

  disable_interrupts();
  if (semtab[semId].ressources > 0) {

    semtab[semId].ressources--;

  } else {

    for (unsigned int i = 0; i < MAX_PROCESS_WAITING ; i++) {
      if (semtab[semId].waitingProcess[i] != NULL) {
        semtab[semId].waitingProcess[i] = curr_proc;
        i = MAX_PROCESS_WAITING;
      }
    }

    stop();

  }
  enable_interrupts();
}

PUBLIC void init_sem() {
  for (unsigned int i = 0; i < SEM_MAX; i++) {
    semtab[i].id = SEM_MAX+1;
    semtab[i].key = 65536;
  }
}
