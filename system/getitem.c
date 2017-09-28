/* getitem.c - getfirst, getlast, getitem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getfirst  -  Remove a process from the front of a queue
 *------------------------------------------------------------------------
 */
pid32	getfirst(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	pid32	head;

	if (isempty(q)) {
		return EMPTY;
	}

	head = queuehead(q);
	return getitem(queuetab[head].qnext);
}

/*------------------------------------------------------------------------
 *  getfirstofgroup  -  Remove a process in group from the front of a queue
 *------------------------------------------------------------------------
 */
pid32	getfirstofgroup(
	  qid16		q,		/* ID of queue from which to	*/
	  int32		group	/* Group type to get 			*/
	)
{
	if (isempty(q)) {
		return EMPTY;
	}
	
	pid32 tail = queuetail(q);
	pid32 curr = queuetab[queuehead(q)].qnext;
	while (curr != tail) {
		if (!isbadpid(curr)) {
			struct procent *pr = &proctab[curr];
			if (pr->pr_group == group) {
				break;
			}
		}
		curr = queuetab[curr].qnext;
	}
	
	return getitem(curr);
}

/*------------------------------------------------------------------------
 *  getlast  -  Remove a process from end of queue
 *------------------------------------------------------------------------
 */
pid32	getlast(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	pid32 tail;

	if (isempty(q)) {
		return EMPTY;
	}

	tail = queuetail(q);
	return getitem(queuetab[tail].qprev);
}

/*------------------------------------------------------------------------
 *  getitem  -  Remove a process from an arbitrary point in a queue
 *------------------------------------------------------------------------
 */
pid32	getitem(
	  pid32		pid		/* ID of process to remove	*/
	)
{
	pid32	prev, next;

	next = queuetab[pid].qnext;	/* Following node in list	*/
	prev = queuetab[pid].qprev;	/* Previous node in list	*/
	queuetab[prev].qnext = next;
	queuetab[next].qprev = prev;
	return pid;
}
