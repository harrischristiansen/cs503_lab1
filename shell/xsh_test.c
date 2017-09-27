/* xsh_test.c - xsh_test */

#include <xinu.h>
#include <stdio.h>

void process_cpu() {
        int i, j;
        int LOOP1 = 10; 
        int LOOP2 = 10000000;

        int v = 0;
        for (i=0; i<LOOP1; i++) {
            for (j=0; j<LOOP2; j++) { // CPU Intensive Loop
                    v += i * j;
            }   

            kprintf("[CPU] PID: %d, Loop: %d \n", currpid, i);
        }   

        kprintf("===== CPU BOUNDED PID %d ends ===== \n", currpid);
}

void process_io() {
        int i, j;
        int LOOP1 = 10;
        int LOOP2 = 12;

        //struct procent *pr = &proctab[currpid];

        for (i=0; i<LOOP1; i++) {
            for (j=0; j<LOOP2; j++) { // IO Bound Loop
                    sleepms(50);
            } 
            kprintf("[IO] PID: %d, Loop: %d\n", currpid, i);
        }   

        kprintf("===== IO BOUNDED PID %d ends ===== \n", currpid);
}

/*------------------------------------------------------------------------
 * xsh_test - Run Tests
 *------------------------------------------------------------------------
 */
shellcmd xsh_test(int nargs, char *args[])
{
	if (nargs < 3) {
		fprintf(stderr,"use is: %s [# CPU Processes] [# IO Processes]\n", args[0]);
		return 1;
	}
	
	int num_cpu = atoi(args[1]);
	int num_io = atoi(args[2]);
	
	int i;
	for (i = 0; i < num_cpu; i++) {
		resume(create(process_cpu, 1024, INITPRIO, "CPU-intense", 0, NULL));
	}
	for (i = 0; i < num_io; i++) {
		resume(create(process_io, 1024, INITPRIO, "IO-intense", 0, NULL));
	}

    printf(" ===== Process Test Complete ===== \n");
    return 0;
}
