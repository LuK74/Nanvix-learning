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

/*
*	Lottery Scheduler
*
*/
// Tickets hold by process
PUBLIC int tickets[PROC_MAX];
//PUBLIC int ticket_median = 100;

PUBLIC void yield(void) {
	struct process *p;    /* Working process.     */
	struct process *next; /* Next process to run. */

	int nb_tickets = 0;

	// Here we randomly give our current process a number of tickets
	// But his maximal number of tickets will depends on how much
	// of his quantum it has used

	/* Re-schedule process for execution. */

	if (curr_proc != IDLE) {
		if (curr_proc->counter < PROC_QUANTUM/2) {
			tickets[curr_proc - IDLE] -= (10 * ((PROC_QUANTUM - curr_proc->counter)/PROC_QUANTUM));
			if (tickets[curr_proc - IDLE] <= 0+500*(((NZERO*2-1) - (curr_proc->nice)) / (NZERO*2-1))) {
				tickets[curr_proc - IDLE] = 1 + 500*(((NZERO*2-1) - (curr_proc->nice)) / (NZERO*2-1));
			}
		} else if (curr_proc->counter > PROC_QUANTUM/2) {
			tickets[curr_proc - IDLE] += (10 - (10 * ((PROC_QUANTUM - curr_proc->counter)/PROC_QUANTUM)));
			if (tickets[curr_proc - IDLE] > 1000-500*(curr_proc->nice/NZERO*2-1)) {
				tickets[curr_proc - IDLE] = 1000-500*(curr_proc->nice/(NZERO*2-1));
			}
		}
	} else {
		tickets[0] = 0;
	}

	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;


	/* Check alarm. */
	/* Count number of ready processes */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{

		/* Skip invalid processes. */
		if (!IS_VALID(p)) {
			tickets[p - IDLE] = 0;
			continue;
		}

		if (p->state == PROC_READY) {
			if (tickets[p - IDLE] == 0) {
				tickets[p - IDLE] = 500;
			}
			nb_tickets += tickets[p - IDLE];
		}

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	// The lottery starts
	unsigned int winner_ticket = 0;
	if (nb_tickets != 0) {
		winner_ticket = (krand()%nb_tickets) + 1;
	}
	unsigned int process_ticket = 0;
	/* Choose a process to run next. */
	next = IDLE;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;
		/*
		 * Process holding the winning ticket will be chosen
		 */
		process_ticket += tickets[p - IDLE];
		if (winner_ticket <= process_ticket) {
			next = p;
			break;
		}

	}

	/* Switch to next process. */
	next->state = PROC_RUNNING;
	next->priority = PRIO_USER;
	next->counter = PROC_QUANTUM;

	if (curr_proc != next)
		switch_to(next);
}
