/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BARRIER_H_
#define BARRIER_H_

#include <nanvix/const.h>
#include <nanvix/config.h>

/**
 * @brief Comand values for semaphores.
 */
/**@{*/
#define GETVAL 0   /**< Returns the value of a barrier. */
#define SETVAL 1   /**< Sets the value of a barrrier.    */
#define IPC_RMID 3 /**< Destroys a barrier.            */
/**@}*/

#define BARRIER_MAX 128
#define MAX_PROCESS_WAITING_B PROC_MAX

struct barrier {
  // Key associated to the semaphore
  unsigned key;

  // Id needed to apply operation on it
  unsigned int id;

  // Requested process
  int request;

  // Flag allowing to know if a barrier is currently used in an atomic function
  // (reach for exemple) -1 : Barrier not initialized 0 : No flag 1 : Is used by
  // a process in an atomic function
  int flag;

  // Array containing every process waiting on the barrier
  struct process *waitingProcess[MAX_PROCESS_WAITING_B];
};

EXTERN struct barrier bartab[BARRIER_MAX];

/* Forward definitions. */
extern int barget(unsigned);
extern int barctl(int, int, int);
extern int barop(int, int);

extern int createB(int);
extern void reach(int);
extern void destroyB(int);
extern void init_barrier();

#endif /* BARRIER_H_ */
