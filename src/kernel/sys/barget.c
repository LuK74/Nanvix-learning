#include <nanvix/const.h>
#include <nanvix/pm.h>
#include <signal.h>
#include <sys/barrier.h>

/*
 * This function is used to create barrier or get
 * barrier that already exists
 * So it will return the id of the barrier (newly created
 * or not)
 * If the barrier couldn't be created, it will return -1
 */
PUBLIC int sys_barget(unsigned key) {

  // Looking for barrier with the given key
  for (int i = 0; i < BARRIER_MAX; i++) {
    if (bartab[i].flag != -1) {
      if (bartab[i].key == key) {
        return bartab[i].id;
      }
    }
  }

  // If the key isn't linked to any barrier
  // we will create a new one
  // and set his key field with the given key
  int barId = createB(0);
  bartab[barId].key = key;

  if (bartab[barId].flag == -1)
    return -1;

  return barId;
}
