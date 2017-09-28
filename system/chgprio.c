/* chgprio.c - chgprio */

#include <xinu.h>

pri16 	pr_group_priority[PR_GROUP_COUNT];

/*------------------------------------------------------------------------
 *  chgprio  -  Change the scheduling priority of a process group
 *------------------------------------------------------------------------
 */
pri16	chgprio(
	  int32		group,		/* Process group to change	*/
	  pri16		newprio		/* New priority				*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	pri16	oldprio;		/* Priority to return		*/

	mask = disable();
	if (group != PROPORTIONALSHARE && group != TSSCHED) {
		restore(mask);
		return (pri16) SYSERR;
	}
	
	oldprio = pr_group_priority[group];
	pr_group_priority[group] = newprio;
	
	restore(mask);
	return oldprio;
}

int32 highest_prgroup()
{
	int32 group = pr_group_priority[PROPORTIONALSHARE] >= pr_group_priority[TSSCHED] ? PROPORTIONALSHARE : TSSCHED;
	if (queue_count(readylist, group) == 0) {
		group = (group == PROPORTIONALSHARE) ? TSSCHED : PROPORTIONALSHARE;
	}
	return group;
}
