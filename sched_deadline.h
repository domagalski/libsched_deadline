/*******************************************************************************
 * Author: Rachel Simone Domagalski
 * Email: rsdomagalski@gmail.com
 *
 * sched_deadline: deadline scheduler
 *
 * This file is part of cameractl.
 *
 * sched_deadline is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cameractl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef _SCHED_DEADLINE_H
#define _SCHED_DEADLINE_H

// deadline scheduler, requires kernel 3.14
#ifndef SCHED_DEADLINE
#define SCHED_DEADLINE 6
#endif

struct sched_attr {
    uint32_t size;              /* Size of this structure */
    uint32_t sched_policy;      /* Policy (SCHED_*) */
    uint64_t sched_flags;       /* Flags */
    int32_t  sched_nice;        /* Nice value (SCHED_OTHER, SCHED_BATCH) */
    uint32_t sched_priority;    /* Static priority (SCHED_FIFO, SCHED_RR) */
    /* Remaining fields are for SCHED_DEADLINE */
    uint64_t sched_runtime;
    uint64_t sched_deadline;
    uint64_t sched_period;
};

// deadline args are for passing multiple args to pthread functions easily.
typedef struct {
    uint64_t sched_runtime;
    uint64_t sched_deadline;
    uint64_t sched_period;
    int retval;
    int (*function)(void **args);
    void **func_args;
} deadline_function;

// wrap syscall to sched_setattr
int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags);
int sched_getattr(pid_t pid, struct sched_attr *attr, unsigned int size, unsigned int flags);

// main function
void *run_sched_deadline(void *data);

#endif
