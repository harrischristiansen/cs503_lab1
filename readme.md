# CS 503 - Lab 1: Process Scheduling  

@Harris Christiansen (christih@purdue.edu) (HarrisChristiansen.com)  
September 2017  

## Description
- Implement a two-level process scheduler and few scheduling policies in XINU

## Usage
- Build: `make` in compile/ directory
	- Clean: `make clean`
	- Rebuild: `make rebuild`
- Test: Tests are found in `shell/xsh_test.c`
	- Spawn x CPU processes, y IO processes: `test x y`
	- Perform provided sample timeshare scheduler test: `test t`

## Tasks
- [ ] Aging Scheduler (20%)
- [X] Proportional Share Scheduler (40%)
- [X] Timeshare Scheduler (40%)
- [X] System Call - create
- [ ] System Call - chgprio
- [ ] System Call - resched
