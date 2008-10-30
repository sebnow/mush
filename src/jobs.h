#include "command.h"
#include "queue.h"
#include "unistd.h"

#ifndef JOBS_H
#define JOBS_H

typedef struct __job_t {
	command_t *command;
	pid_t pid;
} job_t;

int jobAddWithCommandAndPid(command_t *command, pid_t pid);
int jobWithPid(pid_t pid, job_t **job);
int jobRemove(job_t *job);

#endif /* JOBS_H */
