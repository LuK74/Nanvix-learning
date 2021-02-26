/*
* CHALOYARD Lucas
* Semaphore implementation
*
*/

#include <sys/barrier.h>
#include <nanvix/pm.h>
#include <nanvix/klib.h>

/*
* bartab : Array the contains every barrier structure 
*/
PUBLIC struct barrier bartab[BARRIER_MAX];

/*
* bar_chain : Array of "struct process *" used for the sleep function that allow use 
* to do some pasive waiting rather than active waiting 
*/
PUBLIC struct process * bar_chain[BARRIER_MAX];

/* 
* Create a barrier if one is available
* Returns -1 if none is available
* Else return the id of the new barrier
*/
PUBLIC int createB(int request) {

  for (unsigned int i = 0; i < BARRIER_MAX; i++) {
    // Flag -1 means that the barrier hasn't been initialized
    if (bartab[i].flag == -1) {
      bartab[i].id = i;
      bartab[i].key = 0;
      bartab[i].request = request;
      bartab[i].flag = 0;

      // Filling the waitingProcess array with NULL value
      // in order to keep simple the waitingProcess handling
      for (unsigned int j = 0; j < MAX_PROCESS_WAITING_B; j++) {
        bartab[i].waitingProcess[j] = NULL;
      }

      return bartab[i].id;
    }
  }

  return -1;
}

/*
* Destroy a barrier
* Will reset the flag to -1
* And also resume the execution of all waiting processes
*/
PUBLIC void destroyB(int barId) {
  if (barId >= BARRIER_MAX || bartab[barId].id==BARRIER_MAX+1) {
    return;
  }

  bartab[barId].key = 0;
  bartab[barId].flag = -1;
  bartab[barId].id = BARRIER_MAX + 1;

  // Resume all waiting processes 
  for (unsigned int j = 0; j < MAX_PROCESS_WAITING_B; j++) {
    resume(bartab[barId].waitingProcess[j]);
    bartab[barId].waitingProcess[j] = NULL;
  }

}

/*
* Most important function of our barrier
* reach is called when a process "reach" the barrier
* The process will be put in a waiting queue if the 
* request value is greather than zero (after the come of the current process)
* When a process arrived and the request is less or equal than 0 (only equal if the
* function is working properly), all waiting processes will be resumed
*/
PUBLIC void reach(int barId) {
  if (barId >= BARRIER_MAX || bartab[barId].id==BARRIER_MAX+1) {
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
  * If the flag is 0, it means that our barrier isn't used by any other
  * process
  */
  while (__sync_lock_test_and_set(&(bartab[barId].flag), 1)) {
    sleep(bar_chain+barId, 1);
  }

  // When one process reach the barrier, will decrease request by one
  bartab[barId].request--;

  // When request is less or equal than 0, it means that every process
  // reach our barrier
  // So we'll resume the execution of each waitingProcess
  if (bartab[barId].request <= 0) {
    
    for (int i = 0; i < MAX_PROCESS_WAITING_B && bartab[barId].waitingProcess[i] != NULL; i++) {
      resume(bartab[barId].waitingProcess[i]);
      bartab[barId].waitingProcess[i] = NULL;
    }

    bartab[barId].flag = 0;
    wakeup(bar_chain+barId);

  } else {

    // If request is greater than 0
    // We'll put our current process into the waitingProcess array
    // And we'll put it to sleep
    for (int i = 0; i < MAX_PROCESS_WAITING_B;i++) {
      if (bartab[barId].waitingProcess[i] == NULL) {
        bartab[barId].waitingProcess[i] = curr_proc;
        break;
      }
    }

    curr_proc->state = PROC_STOPPED;

    // Before putting to sleep our current process
    // We'll make sure that other processes should
    // be able to enter the reach function
    bartab[barId].flag = 0;
    wakeup(bar_chain+barId);

	  yield();

  }

}

// We'll init each barrier struct
PUBLIC void init_barrier() {
  for (unsigned int i = 0; i < BARRIER_MAX; i++) {
    bartab[i].id = BARRIER_MAX+1;
    bartab[i].key = 65536;
    bartab[i].flag = -1;
    bar_chain[i] = NULL;
  }
}
