#include <sys/sem.h>
#include <nanvix/pm.h>
#include <nanvix/const.h>
#include <signal.h>

/*
* This function is used to create semaphore or get
* semaphore that already exists
* So it will return the id of the semaphore (newly created
* or not)
* If the barrier couldn't be created, it will return -1
*/
PUBLIC int sys_semget(unsigned key) {
  // Looking for semaphore with the given key
  for (int i = 0; i < SEM_MAX; i++) {
    if (semtab[i].flag != -1) {
      if (semtab[i].key == key) {
        return semtab[i].id;
      }
    }
  }

  // If the key isn't linked to any barrier
  // we will create a new one
  // and set his key field with the given key
  int semId = create(0);
  semtab[semId].key = key;

  if(semtab[semId].flag == -1) return -1;

  return semId;
}
