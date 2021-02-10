/*
* CHALOYARD Lucas
* Semaphore implementation
*
*/

#include <sys/sem.h>
#include <nanvix/pm.h>
#include <nanvix/klib.h>

PUBLIC struct semaphore semtab[SEM_MAX];
PUBLIC struct process * sem_chain[SEM_MAX];
PUBLIC int sem_held[SEM_MAX];

PUBLIC int create(int ressources) {

  for (unsigned int i = 0; i < SEM_MAX; i++) {
    if (semtab[i].id == SEM_MAX+1) {
      semtab[i].id = i;
      semtab[i].key = 0;
      semtab[i].ressources = ressources;

      for (unsigned int j = 0; j < MAX_PROCESS_WAITING; j++) {
        semtab[i].waitingProcess[j] = NULL;
      }
      sem_held[i] = 0;
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
    resume(semtab[semId].waitingProcess[j]);
    semtab[semId].waitingProcess[j] = NULL;
  }
  sem_held[semId] = 0;
}

PUBLIC void up(int semId) {
  if (semId >= SEM_MAX || semtab[semId].id==SEM_MAX+1) {
    return;
  }


  while (__sync_lock_test_and_set(sem_held+semId, 1)) {
    sleep(sem_chain+semId, 1);
  }
  if (semtab[semId].ressources < 0) {
    struct process * wakeupProc = semtab[semId].waitingProcess[0];

    if (wakeupProc != NULL) {
      
      resume(wakeupProc);

      unsigned int i ;
      for (i = 0; i < (MAX_PROCESS_WAITING-1) && semtab[semId].waitingProcess[i] != NULL; i++) {
        semtab[semId].waitingProcess[i] = semtab[semId].waitingProcess[i+1];
      }

      if (i < SEM_MAX) {
        semtab[semId].waitingProcess[i] = NULL;
      }

    }

  }
  semtab[semId].ressources++;
  sem_held[semId] = 0;
  wakeup(sem_chain+semId);
}

PUBLIC void down(int semId) {
  if (semId >= SEM_MAX || semtab[semId].id==SEM_MAX+1) {
    return;
  }

  while (__sync_lock_test_and_set(sem_held+semId, 1)) {
    sleep(sem_chain+semId, 1);
  }
 semtab[semId].ressources--;
  if (semtab[semId].ressources < 0) {

    for (unsigned int i = 0; i < MAX_PROCESS_WAITING ; i++) {
      if (semtab[semId].waitingProcess[i] == NULL) {
        semtab[semId].waitingProcess[i] = curr_proc;
        i = MAX_PROCESS_WAITING;
      }
    }
    curr_proc->state = PROC_STOPPED;
    sem_held[semId] = 0;
	  yield();
    sem_held[semId] = 1;

  }
  sem_held[semId] = 0;
  wakeup(sem_chain+semId);
}

PUBLIC void init_sem() {
  for (unsigned int i = 0; i < SEM_MAX; i++) {
    semtab[i].id = SEM_MAX+1;
    semtab[i].key = 65536;
    sem_held[i] = 0;
    sem_chain[i] = NULL;
  }
}
