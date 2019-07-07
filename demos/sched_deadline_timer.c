
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#include <sched_deadline.h>

// generate timing data for SCHED_DEADLINE
int time_deadline(void **args){
    // function arguments
    long long int *latency = (long long int *) args[0];
    long long int *delays = (long long int *) args[1];
    int n_lat = *((int *) args[2]);
    uint64_t period = *((uint64_t *) args[3]);

    int idx;
    // struct timeval time1, time2;
    struct timespec time1, time2;
    long long int t1, t2, prev_delay = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &time1);
    t1 = time1.tv_sec * 1000000000L + time1.tv_nsec;

    // measure time delay between each iteration of the loop.
    for (idx = 0; idx<n_lat; idx++) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &time2);
        t2 = time2.tv_sec * 1000000000L + time2.tv_nsec;

        latency[idx] = t2 - t1;
        delays[idx] = latency[idx] - period;
        t1 = t2;

        // pause computation until the next SCHED_DEADLINE period.
        sched_yield();
    }
    return 0;
}

int main(int argc, char **argv){
    int idx, n_lat;
    long long int *latency, *delays;
    uint64_t runtime, deadline, period;
    deadline_function timer_setup;

    // parse command line arguments.
    if (argc != 5){
        printf("Usage: %s NUM_DELAYS RUNTIME DEADLINE PERIOD\n", argv[0]);
        exit(-1);
    }
    n_lat = atoi(argv[1]) + 1;
    runtime = strtoul(argv[2], NULL, 10);
    deadline = strtoul(argv[3], NULL, 10);
    period = strtoul(argv[4], NULL, 10);

    // create arrays for timer measurements
    latency = (long long int *) malloc(n_lat*sizeof(long long int));
    delays = (long long int *) malloc(n_lat*sizeof(long long int));

    // Deadline scheduler parameters
    timer_setup.sched_runtime = runtime;
    timer_setup.sched_deadline = deadline;
    timer_setup.sched_period = period;

    // populate the arguments of the timer function
    timer_setup.function = time_deadline;
    timer_setup.func_args = (void **) malloc(4*sizeof(void *));
    timer_setup.func_args[0] = latency;
    timer_setup.func_args[1] = delays;
    timer_setup.func_args[2] = &n_lat;
    timer_setup.func_args[3] = &period;

    // times are in nanoseconds
    printf("Runtime: %lu\n", runtime);
    printf("Deadline: %lu\n", deadline);
    printf("Period: %lu\n", period);

    // run the timer on the deadline scheduler
    run_sched_deadline((void *)&timer_setup);

    // print the results. skip the first number since it's useless.
    for (idx = 1; idx < n_lat; idx++){
        printf("%lld %lld\n", latency[idx], delays[idx]);
    }

    // cleanup
    free(delays);
    free(latency);
    free(timer_setup.func_args);
    return 0;
}
