/* queue.c - enqueue, dequeue */

#include <xinu.h>

struct qentry	queuetab[NQENT];	/* Table of process queues	*/

/*------------------------------------------------------------------------
 *  enqueue  -  Insert a process at the tail of a queue
 *------------------------------------------------------------------------
 */
pid32	enqueue(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q		/* ID of queue to use		*/
	)
{
	qid16	tail, prev;		/* Tail & previous node indexes	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	tail = queuetail(q);
	prev = queuetab[tail].qprev;

	queuetab[pid].qnext  = tail;	/* Insert just before tail node	*/
	queuetab[pid].qprev  = prev;
	queuetab[prev].qnext = pid;
	queuetab[tail].qprev = pid;
	return pid;
}

/*------------------------------------------------------------------------
 *  dequeue  -  Remove and return the first process on a list
 *------------------------------------------------------------------------
 */
pid32	dequeue(
	  qid16		q		/* ID queue to use		*/
	)
{
	pid32	pid;			/* ID of process removed	*/

	if (isbadqid(q)) {
		return SYSERR;
	} else if (isempty(q)) {
		return EMPTY;
	}

	pid = getfirst(q);
	queuetab[pid].qprev = EMPTY;
	queuetab[pid].qnext = EMPTY;
	return pid;
}

/*------------------------------------------------------------------------
 *  dequeueofgroup  -  Remove and return the first process in group on a list
 *------------------------------------------------------------------------
 */
pid32	dequeueofgroup(
	  qid16		q,		/* ID queue to use		*/
	  int32		group	/* Group type to get 			*/
	)
{
	pid32	pid;			/* ID of process removed	*/

	if (isbadqid(q) || (group != PROPORTIONALSHARE && group != TSSCHED)) {
		return SYSERR;
	} else if (isempty(q)) {
		return EMPTY;
	}

	pid = getfirstofgroup(q, group);
	queuetab[pid].qprev = EMPTY;
	queuetab[pid].qnext = EMPTY;
	return pid;
}

/*------------------------------------------------------------------------
 *  queue_count  -  Count number of *group* elements in queue
 *------------------------------------------------------------------------
 */
int32	queue_count(
	  qid16		q,		/* ID of queue to use		*/
	  int32		group	/* Group which to count in queue	*/
	)
{
	int32 count = 0;

	if (group != PROPORTIONALSHARE && group != TSSCHED) {
		return SYSERR;
	}

	pid32 curr;
	curr = firstid(q);
	while (queuetab[curr].qkey > MINKEY) {
		if (!isbadpid(curr)) {
			struct procent *pr = &proctab[curr];
			if (pr->pr_group == group) {
				count = count + 1;
			}
		}
		curr = queuetab[curr].qnext;
	}

	return count;
}