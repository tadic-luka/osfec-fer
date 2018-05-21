/*! Semaphore example (threads) */

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

char PROG_HELP[] = "Thread example (barriers): producer consumer example.";

#define THREADS		3
#define ITERS		4

static timespec_t sleep;

static pthread_barrier_t barrier;

static int res = 0;

/* consumer thread */
static void *thread_func ( void *param )
{
	int i, thr_no;

	thr_no = (int) param;
	printf ( "Thread %d starting\n", thr_no );

	for(i = 0; i < ITERS; ++i) {
		printf("thread %d, waiting for barrier\n", thr_no);
		if(pthread_barrier_wait(&barrier) == 0) {
			printf("thread %d, got zero\n", thr_no);
			--i;
			continue;
		}
		printf("thread %d in C.O.\n", thr_no);
		if(thr_no == 0) {
			++res;
			printf("Thread %d, incrementing, now it is: %d\n", thr_no, res);
		} else {
			--res;
			printf("Thread %d, decrementing, now it is: %d\n", thr_no, res);
		}
		nanosleep ( &sleep, NULL );

	}

	return NULL;
}


int barriers ( char *args[] )
{
	pthread_t thread[THREADS];
	int i;

	printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
			PROG_HELP );

	sleep.tv_sec = 1;
	sleep.tv_nsec = 0;

	pthread_barrier_init ( &barrier, THREADS);

	for ( i = 0; i < THREADS; i++ )
		pthread_create ( &thread[i], NULL, thread_func, (void *) (i) );

	for ( i = 0; i < THREADS; i++ )
		pthread_join ( thread[i], NULL );

	pthread_barrier_destroy ( &barrier );

	return 0;
}
