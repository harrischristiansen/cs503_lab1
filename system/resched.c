/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

// ======================================== Proportional Share Scheduler ======================================== //

int32 calculatePi(int32 oldPi, int32 ticks, int32 valueRi)
{
	return oldPi + (ticks * 100 / valueRi);
}

int32 validate_pi(int32 valuePi)
{
	return (valuePi < clktime*1000) ? valuePi : clktime*1000;
}

int32 calculatePRIOi(int32 valuePi) {
	return 2147483646 - valuePi;
}

void updatePSProcessPriority(struct procent *prptr)
{
	if (prptr == &proctab[NULLPROC]) {
		return;
	}
	//kprintf("Updating Priority(%d, %d, %d) ", prptr->pr_pi, prptr->pr_cputimecurrent, prptr->pr_ri);
	prptr->pr_pi = calculatePi(prptr->pr_pi, prptr->pr_cputimecurrent, prptr->pr_ri);
	//kprintf("to %d\n", prptr->pr_pi);
	prptr->prprio = calculatePRIOi(prptr->pr_pi);
}



// ======================================== Timeshare Scheduler ======================================== //

const int32 QUANTUM_FOR_LEVEL[] = {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 20};
const int32 NEXT_CPU_PRIORITY_FOR_PRIOITY[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49};
const int32 NEXT_IO_PRIORITY_FOR_PRIOITY[] = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 56, 57, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 59};

bool8 classifyProcess(struct procent *prptr)
{
	// Determine Process Classification
	bool8 oldClass = prptr->pr_class;
	if (preempt <= 0 || prptr->prstate == PR_CURR || prptr->prstate == PR_READY) { // Process is not IO blocked
		prptr->pr_class = PR_CPU;
	} else {
		prptr->pr_class = PR_IO;
	}
	
	return oldClass;
}

void updateTSProcessPriority(struct procent *prptr)
{
	if (prptr == &proctab[NULLPROC]) {
		return;
	}
	
	if (prptr->pr_class == PR_IO) { // Determine next priority
		prptr->prprio = NEXT_IO_PRIORITY_FOR_PRIOITY[prptr->prprio];
	} else {
		prptr->prprio = NEXT_CPU_PRIORITY_FOR_PRIOITY[prptr->prprio];
	}
}

int32 quantumForPriority(int32 priority)
{
	if (priority < 0 || priority > 59) {
		return QUANTUM;
	}
	return QUANTUM_FOR_LEVEL[priority];
}

// ======================================== Quantum ======================================== //

int32 quantumForGroup(struct procent *prptr)
{
	if (prptr->pr_group == TSSCHED) {
		return quantumForPriority(prptr->prprio);
	} else if (prptr->pr_group == PROPORTIONALSHARE) {
		return QUANTUM;
	} else {
		return QUANTUM;
	}
}

void setScheduled(struct procent *prptr)
{
	prptr->pr_cputimecurrent = 0;
	prptr->pr_tsscheduled = clktime;
}

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	
	if (ptold->pr_group == TSSCHED) {
		classifyProcess(ptold);
		updateTSProcessPriority(ptold);
	} else if (ptold->pr_group == PROPORTIONALSHARE) {
		updatePSProcessPriority(ptold);
	}

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		if (ptold->prprio > firstkey(readylist)) {
			setScheduled(ptold); // TODO: Should this be reset here?
			preempt = quantumForGroup(ptold); /* Reset time slice for process	*/
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		ptold->pr_tsready = clktime;
		insert(currpid, readylist, ptold->prprio);
	}

	/* Force context switch to highest priority ready process */
	currpid = dequeue(readylist);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	setScheduled(ptnew);
	preempt = quantumForGroup(ptnew); /* Reset time slice for process	*/
	ptnew->pr_pi = validate_pi(ptnew->pr_pi); // Validate Pi Value when first scheduled
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
