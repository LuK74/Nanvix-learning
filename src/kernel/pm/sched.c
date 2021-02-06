/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h>




/**
 * @brief Schedules a process to execution.
 *
 * @param proc Process to be scheduled.
 */
PUBLIC void sched(struct process *proc)
{
	proc->state = PROC_READY;
	proc->counter = 0;

}

/**
 * @brief Stops the current running process.
 */
PUBLIC void stop(void)
{
	curr_proc->state = PROC_STOPPED;
	sndsig(curr_proc->father, SIGCHLD);
	yield();
}

/**
 * @brief Resumes a process.
 *
 * @param proc Process to be resumed.
 *
 * @note The process must stopped to be resumed.
 */
PUBLIC void resume(struct process *proc)
{
	/* Resume only if process has stopped. */
	if (proc->state == PROC_STOPPED)
		sched(proc);
}

/**
 * @brief Yields the processor.
 * Round-Robin version
 */
PUBLIC void yieldOld(void)
{
	struct process *p;    /* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run next. */
	next = IDLE;
	IDLE->counter = 0 ;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;

		/*
		 * Process with higher
		 * waiting time found.
		 */
		if (p->counter > next->counter)
		{
			next->counter++;
			next = p;
		}

		/*
		 * Increment waiting
		 * time of process.
		 */
		else
			p->counter++;
	}

	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next);
}

// We keep in this tab, the indice of the queue of each processes
// - 1 : Not used
// 0 : Min priority
// 7 : Max priority
PUBLIC int queue[PROC_MAX];

/*
* Multiple Queue Scheduler
* Use of 8 queues
*/
PUBLIC void yield(void)
{
  struct process *p;    /* Working process.     */
  struct process *next; /* Next process to run. */


  /* Re-schedule process for execution. */
  if (curr_proc->state == PROC_RUNNING)
    sched(curr_proc);

  /* Remember this process. */
  last_proc = curr_proc;

  /*
     If the current process use all his quantum, we'll decrease his
     priority level by one
  */
	/*
	// Here we dynamically modify the priority queue which a process is in
	if (curr_proc != IDLE) {
		// We're checking if the quantum is less than a proportion of the
		// entire quantum allocated
		// This proportion depends of the current rank of the process
		// Lower the rank is, lower the proportion is
		// So it make it more difficult to go the lowest queue
		if (curr_proc->counter < ((queue[(curr_proc-IDLE)])/8  * PROC_QUANTUM)) {
			if (queue[(curr_proc-IDLE)] > 0)
			{
				queue[(curr_proc-IDLE)]--;
			}
		}
		// We're checking if the quantum is more than a proportion of the
		// entire quantum allocated
		// This proportion depends of the current rank of the process
		// Higher the rank is, higher the proportion is
		// So it make it more difficult to go the highest queue
		else if (curr_proc->counter >= ((queue[(curr_proc-IDLE)]+1)/8  * PROC_QUANTUM)) {
			if (queue[(curr_proc-IDLE)] < 7)
			{
				queue[(curr_proc-IDLE)]++;
			}
		}
	} else {
		queue[0] = -1;
	}*/

	// Old version
	// In this version we were moving a process to an inferior queue
	// when a process used all of his Quantum
	// And it could never go up again
  if (curr_proc->counter == 0 && curr_proc != IDLE)
  {
      if (queue[(curr_proc-IDLE)] > 0)
	{
	  queue[(curr_proc-IDLE)]--;
	}
      else
	{
	  if (curr_proc == IDLE)
	    queue [0] = -1 ;
	}
}


  /* Check alarm. */
  for (p = FIRST_PROC; p <= LAST_PROC; p++)
    {
      /* Skip invalid processes. */
      if (!IS_VALID(p))
	continue;

      /* Alarm has expired. */
      if ((p->alarm) && (p->alarm < ticks))
	p->alarm = 0, sndsig(p, SIGALRM);
    }

  /* Choose a process to run next. */
  next = IDLE;
  IDLE->counter = 0 ;

  for (p = FIRST_PROC; p <= LAST_PROC; p++)
    {
      /* Skip non-ready process. */

      if (p->state != PROC_READY)
	{
	  continue;
	}
      else
	{
	  /* If the process is ready, we give it the highest user priority */
	  if (queue[(p-IDLE)] == -1)
	    {
	      queue[(p-IDLE)] = 7;
	    }
	}

    /*
    * Process with higher
    * waiting time and highess priority found
    */
    if (next != IDLE)
		{

			// Access to the queue rank of the two processes we're going to compare
	  	int p_queueRank = queue[(p-IDLE)];
	  	int n_queueRank = queue[(next-IDLE)];


			// Selection of a process according to their priority and counter
			// Firstly we're checking if the new process we're studying has a priority
			// higher than our current next process
			// If it does, we know that we need to select the new process
			// If not, we're checking if their priority are equal
			//	 If it does, we compare their waiting time
			//		 If the new process has a waiting time (counter) high, we choose it
			//		 If not, we skip
			// 	 If not, we skip
	  	if ((p_queueRank > n_queueRank) || ((p_queueRank == n_queueRank) && (p->counter > next->counter)))
	    	{
	      	next->counter++;
	      	next = p;
	    	}
	  	else
	    	// Incrementing waiting process
	    	p->counter++;
			}
      else
			{
	  		next = p;
			}

    }

  /* Switch to next process. */
  next->state = PROC_RUNNING;
  next->counter = PROC_QUANTUM;
  next->priority = PRIO_USER;

  if (curr_proc != next)
    switch_to(next);
}
