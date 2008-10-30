#include "jobs.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define JOBS_SIZE_INITIAL 10
#define JOBS_SIZE_MULTIPLIER 1.5f

static job_t **__jobs = NULL;
static size_t __jobsSize = JOBS_SIZE_INITIAL;
static size_t __jobsCount = 0;

static job_t *_jobCreate(command_t *command, pid_t pid)
{
	job_t *newJob;
	newJob = malloc(sizeof(*newJob));
	if(newJob != NULL) {
		newJob->pid = pid;
		newJob->command = command;
	}
	return newJob;
}

int jobAddWithCommandAndPid(command_t *command, pid_t pid)
{
	job_t *newJob = NULL;
	assert(command != NULL);
	assert(pid != 0);

	newJob = _jobCreate(command, pid);
	if(newJob == NULL) {
		return 0;
	}
	/* Allocate memory if we haven't already */
	if(__jobs == NULL) {
		__jobs = malloc(__jobsSize * sizeof(*__jobs));
		if(__jobs == NULL) {
			return 0;
		}
	/* Increase array size if amount of jobs exceeds it */
	} else if(__jobsCount >= __jobsSize) {
		__jobsSize *= JOBS_SIZE_MULTIPLIER;
		__jobs = realloc(__jobs, __jobsSize);
	}
	__jobs[__jobsCount] = newJob;
	__jobsCount++;
	return 1;
}

int jobWithPid(pid_t pid, job_t **job)
{
	int index;
	int jobNumber = -1;
	for(index = 0; index < __jobsCount; index++) {
		if(pid == __jobs[index]->pid) {
			jobNumber = index;
			*job = __jobs[index];
		}
	}
	return jobNumber + 1;
}

int jobRemove(job_t *job)
{
	int index;
	/* Find index of job to remove */
	for(index = 0; index < __jobsCount; index++) {
		if(job == __jobs[index]) {
			break;
		}
	}
	/* Check if we found the job */
	if(index == __jobsCount) {
		return 1;
	}
	/* Move down the rest of the jobs */
	__jobsCount--;
	for(; index < __jobsCount; index++) {
		__jobs[index] = __jobs[index + 1];
	}
	__jobs[__jobsCount] = NULL;
	return 0;
}

size_t jobsCount()
{
	return __jobsCount;
}
