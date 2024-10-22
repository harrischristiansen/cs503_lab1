/* process.h - isbadpid */

/* Maximum number of processes in the system */

#ifndef NPROC
#define	NPROC		8
#endif

/* Process Scheduler Group Constants */

#define PR_GROUP_COUNT		10	/* Number of Process Groups	to create	*/
#define PR_GROUP_INITIAL	10	/* Initial Priority for Process Groups	*/
#define PROPORTIONALSHARE  	0   /* Proportional Share Scheduler 		*/
#define TSSCHED   			1   /* Timeshare Scheduler 					*/

/* Process state constants */

#define	PR_FREE		0		/* Process table entry is unused		*/
#define	PR_CURR		1		/* Process is currently running			*/
#define	PR_READY	2		/* Process is on ready queue			*/
#define	PR_RECV		3		/* Process waiting for message			*/
#define	PR_SLEEP	4		/* Process is sleeping					*/
#define	PR_SUSP		5		/* Process is suspended					*/
#define	PR_WAIT		6		/* Process is on semaphore queue		*/
#define	PR_RECTIM	7		/* Process is receiving with timeout	*/

/* Process Classification Constants */

#define PR_CPU  0   		/* CPU Bound 							*/
#define PR_IO   1   		/* I/O Bound 							*/

/* Miscellaneous process definitions */

#define	PNMLEN		16		/* Length of process "name"				*/
#define	NULLPROC	0		/* ID of the null process				*/

/* Process initialization constants */

#define	INITSTK		65536	/* Initial process stack size			*/
#define	INITPRIO	20		/* Initial process priority				*/
#define	INITRET		userret	/* Address to which process returns		*/

/* Inline code to check process ID (assumes interrupts are disabled)	*/

#define	isbadpid(x)	( ((pid32)(x) < 0) || \
			  ((pid32)(x) >= NPROC) || \
			  (proctab[(x)].prstate == PR_FREE))

/* Number of device descriptors a process can have open */

#define NDESC		5		/* must be odd to make procent 4N bytes	*/

/* Definition of the process table (multiple of 32 bits) */

struct procent {			/* Entry in the process table			*/
	int32	pr_group;		/* Scheduler Group for process			*/
	uint16	prstate;		/* Process state: PR_CURR, etc.			*/
	pri16	prprio;			/* Process priority						*/
	char	*prstkptr;		/* Saved stack pointer					*/
	char	*prstkbase;		/* Base of run time stack				*/
	uint32	prstklen;		/* Stack length in bytes				*/
	char	prname[PNMLEN];	/* Process name							*/
	sid32	prsem;			/* Semaphore on which process waits		*/
	pid32	prparent;		/* ID of the creating process			*/
	umsg32	prmsg;			/* Message sent to this process			*/
	bool8	prhasmsg;		/* Nonzero iff msg is valid				*/
	int16	prdesc[NDESC];	/* Device descriptors for process		*/
	
	uint32  pr_cputime; 	/* Process total CPU time in clk count	*/
	uint32  pr_cputimecurrent; 	/* Process CPU time since last set current	*/
	uint32  pr_tsready; 	/* Timestamp of last PR_READY   		*/
	uint32  pr_tsscheduled; /* Timestamp of last PR_CURR   			*/
	
	bool8   pr_class;   	/* CPU/IO Process Classification (0=CPU, 1=IO)	*/
	int32   pr_pi;   		/* Process Proportional Share Priority Value    */
	int32   pr_ri;   		/* Process Proportional Share Rate Value    */
};

/* Marker for the top of a process stack (used to help detect overflow)	*/
#define	STACKMAGIC	0x0A0AAAA9

extern	struct	procent proctab[];
extern	int32	prcount;	/* Currently active processes			*/
extern	pid32	currpid;	/* Currently executing process			*/

extern  pri16 	pr_group_priority[PR_GROUP_COUNT];	/* Process Group Priorities */
