/* xsh_test.c - xsh_test */

#include <xinu.h>
#include <stdio.h>

void process_cpu() {
    int i, j;
    int LOOP1 = 10; 
    int LOOP2 = 10000000;
    
    struct procent *pr = &proctab[currpid];

    int v = 0;
    for (i=0; i<LOOP1; i++) {
        for (j=0; j<LOOP2; j++) { // CPU Intensive Loop
                v += i * j;
        }   

        kprintf("[CPU] PID: %d, Loop: %d, Class: %s, Priority: %d \n", currpid, i, pr->pr_class == PR_IO ? "IO":"CPU", pr->prprio);
    }   

    kprintf("===== CPU BOUNDED PID %d ends ===== \n", currpid);
}

void process_io_multiple() {
    int i, j;
    int LOOP1 = 10;
    int LOOP2 = 12;

    struct procent *pr = &proctab[currpid];

    for (i=0; i<LOOP1; i++) {
        for (j=0; j<LOOP2; j++) { // IO Bound Loop
            sleepms(50);
        } 
        kprintf("[IO] PID: %d, Loop: %d, Class: %s, Priority: %d \n", currpid, i, pr->pr_class == PR_IO ? "IO":"CPU", pr->prprio);
    }   

    kprintf("===== IO BOUNDED PID %d ends ===== \n", currpid);
}

void process_io_single(uint32 time) {
    int i;
    int LOOP1 = 30; 

    struct procent *pr = &proctab[currpid];

    for (i=0; i<LOOP1; i++) {
        sleepms(time);
        kprintf("[IO] PID: %d, Loop: %d, Class: %s, Priority: %d \n", currpid, i, pr->pr_class == PR_IO ? "IO":"CPU", pr->prprio);
    }   

    kprintf("===== IO BOUNDED PID %d ends\n", currpid);
}

void sample_ts_test() {
	kprintf("===TS TEST3===\n");
	int i;
	resched_cntl(DEFER_START);
    for (i = 0; i < 6; i++) {
        if (i % 2 == 0) {
            resume(create(process_cpu, 1024, TSSCHED, 20, "cpu-intense", 0));
        }
        else {
            resume(create(process_io_single, 1024, TSSCHED, 20, "io-intense", 1, 32));
        }
    }
    resched_cntl(DEFER_STOP);
}

/*------------------------------------------------------------------------
 * xsh_test - Run Tests
 *------------------------------------------------------------------------
 */
shellcmd xsh_test(int nargs, char *args[])
{
	if (nargs == 2) {
		if (args[1][0] == 't') {
			sample_ts_test();
		}
		return 0;
	}
	
	if (nargs < 4) {
		fprintf(stderr,"use is: %s [Scheduler Group] [# CPU Processes] [# IO Processes]\n", args[0]);
		return 1;
	}
	
	int sched_group = atoi(args[1]);
	int num_cpu = atoi(args[2]);
	int num_io = atoi(args[3]);
	
	if (sched_group != PROPORTIONALSHARE && sched_group != TSSCHED) {
		fprintf(stderr,"use is: %s [Scheduler Group] [# CPU Processes] [# IO Processes]\n", args[0]);
		return 1;
	}
	
	int i;
	resched_cntl(DEFER_START);
	for (i = 0; i < num_cpu; i++) {
		resume(create(process_cpu, 1024, sched_group, INITPRIO, "CPU-intense", 0, NULL));
	}
	for (i = 0; i < num_io; i++) {
		resume(create(process_io_multiple, 1024, sched_group, INITPRIO, "IO-intense", 0, NULL));
	}
	resched_cntl(DEFER_STOP);
	
	
    printf(" ===== Process Test Complete ===== \n");
    return 0;
}
