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

#ifndef SEM_H_
#define SEM_H_

#include <nanvix/const.h>
#include <nanvix/config.h>

/**
 * @brief Comand values for semaphores.
 */
/**@{*/
#define GETVAL 0   /**< Returns the value of a semaphore. */
#define SETVAL 1   /**< Sets the value of a semaphore.    */
#define IPC_RMID 3 /**< Destroys a semaphore.            */
/**@}*/

#define SEM_MAX 128
#define MAX_PROCESS_WAITING PROC_MAX

struct semaphore {
  // Key associated to the semaphore
  unsigned key;

  // Id needed to apply operation on it
  unsigned int id;

  // Ressources
  int ressources;

  // Flag
  // -1 : Semaphore not initialized
  // 0 : Semaphore initialized
  // 1 : Semaphore currently used by a process for an atomic function
  int flag;

  // Array containing every process waiting on the semaphore
  struct process *waitingProcess[MAX_PROCESS_WAITING];
};

EXTERN struct semaphore semtab[SEM_MAX];

/* Forward definitions. */
extern int semget(unsigned);
extern int semctl(int, int, int);
extern int semop(int, int);

extern int create(int);
extern void down(int);
extern void up(int);
extern void destroy(int);
extern void init_sem();

#endif /* SEM_H_ */
