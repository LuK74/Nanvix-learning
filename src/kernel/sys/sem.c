/*
 * CHALOYARD Lucas
 * Semaphore implementation
 *
 */

#include <nanvix/klib.h>
#include <nanvix/pm.h>
#include <sys/sem.h>

/*
 * semtab : Array the contains every barrier structure
 */
PUBLIC struct semaphore semtab[SEM_MAX];

/*
 * sem_chain : Array of "struct process *" used for the sleep function that
 * allow use to do some pasive waiting rather than active waiting
 */
PUBLIC struct process *sem_chain[SEM_MAX];

/*
 * Create a semaphore if one is available
 * Returns -1 if none is available
 * Else return the id of the new barrier
 */
PUBLIC int create(int ressources) {

  for (unsigned int i = 0; i < SEM_MAX; i++) {
    // Flag -1 means that the barrier hasn't been initialized
    if (semtab[i].flag == -1) {
      semtab[i].id = i;
      semtab[i].key = 0;
      semtab[i].ressources = ressources;
      semtab[i].flag = 0;

      // Filling the waitingProcess array with NULL value
      // in order to keep simple the waitingProcess handling
      for (unsigned int j = 0; j < MAX_PROCESS_WAITING; j++) {
        semtab[i].waitingProcess[j] = NULL;
      }

      return semtab[i].id;
    }
  }

  return -1;
}

/*
 * Destroy a semaphore
 * Will reset the flag to -1
 * And also resume the execution of all waiting processes
 */
PUBLIC void destroy(int semId) {
  if (semId >= SEM_MAX || semtab[semId].id == SEM_MAX + 1) {
    return;
  }

  semtab[semId].key = 0;
  semtab[semId].id = SEM_MAX + 1;
  semtab[semId].flag = -1;

  // Resume all waiting processes
  for (unsigned int j = 0; j < MAX_PROCESS_WAITING; j++) {
    resume(semtab[semId].waitingProcess[j]);
    semtab[semId].waitingProcess[j] = NULL;
  }
}

/*
 * This function is used in order to up the ressources of a semaphore
 * If after increasing the ressources of the semaphore by one,
 * ressources is less than 0, it means that we could have a waiting process
 * So if we increase the number of ressources, we should wake him up
 */
PUBLIC void up(int semId) {
  if (semId >= SEM_MAX || semtab[semId].id == SEM_MAX + 1) {
    return;
  }

  /*
   * This code is used in order to make our function atomic
   * The "__sync_lock_test_and_set(&adress, value)" will test the
   * value pointed by adresse and will set with the value atomically
   * It allows us to make our function atomical too
   *
   * So we'll test the value flag, if it is different than 0, the test
   * will pass, and our process will sleep
   * If the flag is 0, it means that our semaphore isn't used by any other
   * process
   */
  while (__sync_lock_test_and_set(&(semtab[semId].flag), 1)) {
    sleep(sem_chain + semId, 1);
  }

  // Case where a process might be waiting on this semaphore
  if (semtab[semId].ressources < 0) {

    struct process *wakeupProc = semtab[semId].waitingProcess[0];

    // If there is really a process waiting, we resume his execution
    if (wakeupProc != NULL) {

      resume(wakeupProc);

      // Now we reorder our array in order to always have
      // a FIFO order
      unsigned int i;
      for (i = 0; i < (MAX_PROCESS_WAITING - 1) &&
                  semtab[semId].waitingProcess[i] != NULL;
           i++) {
        semtab[semId].waitingProcess[i] = semtab[semId].waitingProcess[i + 1];
      }

      if (i >= MAX_PROCESS_WAITING - 1) {
        semtab[semId].waitingProcess[i] = NULL;
      }
    }
  }
  semtab[semId].ressources++;

  // After we finish our work in the "up" function
  // we wake up every process waiting on the atomic loop
  semtab[semId].flag = 0;
  wakeup(sem_chain + semId);
}

PUBLIC void down(int semId) {
  if (semId >= SEM_MAX || semtab[semId].id == SEM_MAX + 1) {
    return;
  }

  /*
   * This code is used in order to make our function atomic
   * The "__sync_lock_test_and_set(&adress, value)" will test the
   * value pointed by adresse and will set with the value atomically
   * It allows us to make our function atomical too
   *
   * So we'll test the value flag, if it is different than 0, the test
   * will pass, and our process will sleep
   * If the flag is 0, it means that our semaphore isn't used by any other
   * process
   */
  while (__sync_lock_test_and_set(&(semtab[semId].flag), 1)) {
    sleep(sem_chain + semId, 1);
  }

  // We decrease the ressources of our semaphore by one
  semtab[semId].ressources--;

  // If ressources are below 0 after we decrease it
  // We need to block the current process
  if (semtab[semId].ressources < 0) {

    for (unsigned int i = 0; i < MAX_PROCESS_WAITING; i++) {
      if (semtab[semId].waitingProcess[i] == NULL) {
        semtab[semId].waitingProcess[i] = curr_proc;
        i = MAX_PROCESS_WAITING;
      }
    }
    curr_proc->state = PROC_STOPPED;

    // If the current process is put to sleep
    // We need to free our semaphore in order to allow other processes
    // to do the "up" and "down" functions
    semtab[semId].flag = 0;
    yield();
    semtab[semId].flag = 1;
  }

  // After we finish our work in the "down" function
  // we wake up every process waiting on the atomic loop
  semtab[semId].flag = 0;
  wakeup(sem_chain + semId);
}

PUBLIC void init_sem() {
  for (unsigned int i = 0; i < SEM_MAX; i++) {
    semtab[i].id = SEM_MAX + 1;
    semtab[i].key = 65536;
    semtab[i].flag = -1;
    sem_chain[i] = NULL;
  }
}
