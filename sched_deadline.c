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

// sched_deadline
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
//#include <pthread.h>

#include "sched_deadline.h"

// wrap syscall to sched_setattr
int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags)
{
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

// wrap syscall to sched_getattr
int sched_getattr(pid_t pid, struct sched_attr *attr, unsigned int size, unsigned int flags)
{
    return syscall(__NR_sched_getattr, pid, attr, size, flags);
}

// Run a function using the deadline scheduler.
void *run_sched_deadline(void *data)
{
    struct sched_attr attr;
    int ret;
    unsigned int flags = 0;
    deadline_function *args = (deadline_function *)data;

    // setup SCHED_DEADLINE
    attr.size = sizeof(attr);
    attr.sched_flags = 0;
    attr.sched_nice = 0;
    attr.sched_priority = 0;
    attr.sched_policy = SCHED_DEADLINE;
    attr.sched_runtime = args -> sched_runtime;
    attr.sched_deadline= args -> sched_deadline;
    attr.sched_period  = args -> sched_period;

    ret = sched_setattr(0, &attr, flags);
    if (ret < 0) {
        perror("sched_setattr");
        exit(-1);
    }

    // run a function and capture it's return value
    args -> retval = args -> function(args -> func_args);

    return NULL;
}
