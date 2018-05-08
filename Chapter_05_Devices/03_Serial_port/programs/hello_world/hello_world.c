/*! Hello world program */

#include <stdio.h>
#include <time.h>
#include <api/prog_info.h>
#include <arch/processor.h>
#include <arch/interrupt.h>

static timespec_t t0;
extern int prio[INTERRUPTS];


static void test_handler_38(unsigned int sig, void *device)
{
	printf("Handling signal %d\n", sig);
}
static void test_handler_37(unsigned int sig, void *device)
{
	printf("Handling signal %d\n", sig);
}
static void alarm_36 ( sigval_t param )
{
	timespec_t t;

	clock_gettime ( CLOCK_REALTIME, &t );
	time_sub ( &t, &t0 );
	t.tv_sec = 1;
	/*t.tv_nsec = 0;*/
	printf("In alarm, interrupt 38\n");
	raise_interrupt(38);
	/*printf ( "In alarm, now sleeping for %d sec\n", t.tv_sec);*/
	/*while ( TIME_IS_SET (&t) )*/
		/*if ( clock_nanosleep ( CLOCK_REALTIME, 0, &t, &t ) )*/
			/*printf ( "Interrupted sleep?\n" );*/
}

static void alarm_33 ( sigval_t param )
{
	timespec_t t;

	clock_gettime ( CLOCK_REALTIME, &t );
	time_sub ( &t, &t0 );
	t.tv_sec = 1;
	/*t.tv_nsec = 0;*/
	printf("In alarm, interrupt 38\n");
	raise_interrupt(38);
	/*printf ( "In alarm, now sleeping for %d sec\n", t.tv_sec);*/
	/*while ( TIME_IS_SET (&t) )*/
		/*if ( clock_nanosleep ( CLOCK_REALTIME, 0, &t, &t ) )*/
			/*printf ( "Interrupted sleep?\n" );*/
}
int hello_world ()
{
	printf("interrupts: %d\n", INTERRUPTS);
	timespec_t t;
	itimerspec_t t1, t2;
	timer_t timer1, timer2;
	sigevent_t evp;
	sigevent_t evp2;
	arch_register_interrupt_handler ( 38, &test_handler_38, NULL, prio[38] );
	arch_register_interrupt_handler ( 37, &test_handler_37, NULL, prio[37] );


	printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
		 timer_PROG_HELP );

	clock_gettime ( CLOCK_REALTIME, &t );
	t0 = t;
	printf ( "System time: %d:%d\n", t.tv_sec, t.tv_nsec/100000000 );

	evp.sigev_notify = SIGEV_THREAD;
	evp.sigev_notify_function = alarm_36;
	evp.sigev_notify_attributes = NULL;

	evp2.sigev_notify = SIGEV_THREAD;
	evp2.sigev_notify_function = alarm_33;
	evp2.sigev_notify_attributes = NULL;
	/* timer1 */
	t1.it_interval.tv_sec = 28;
	t1.it_interval.tv_nsec = 0;
	t1.it_value.tv_sec = 4;
	t1.it_value.tv_nsec = 0;
	evp.sigev_value.sival_int = t1.it_interval.tv_sec;
	timer_create ( CLOCK_REALTIME, &evp, &timer1 );
	timer_settime ( &timer1, 0, &t1, NULL );
	/* timer2 */

	t2.it_interval.tv_sec = 28;
	t2.it_interval.tv_nsec = 0;
	t2.it_value.tv_sec = 7;
	t2.it_value.tv_nsec = 0;
	evp2.sigev_value.sival_int = t2.it_interval.tv_sec;
	timer_create ( CLOCK_REALTIME, &evp2, &timer2 );
	timer_settime ( &timer2, 0, &t2, NULL );




	t.tv_sec = 9;
	/*int i = 0;*/
	t.tv_nsec = 0;
	printf("main program sleeping for %d sec\n", t.tv_sec);
	/*for(i=0;i<999999;++i);*/
	while ( TIME_IS_SET (&t) )
		if ( clock_nanosleep ( CLOCK_REALTIME, 0, &t, &t ) )
			printf ( "Interrupted sleep?\n" );

	/*printf("Raising interrupt 36\n");*/
	/*raise_interrupt(36);*/
	printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
		 hello_world_PROG_HELP );

	printf ( "Hello World!\n" );
	timer_delete ( &timer1 );
	timer_delete ( &timer2 );


#if 0	/* test escape sequence */
	printf ( "\x1b[20;40H" "Hello World at 40, 20!\n" );
#endif

	return 0;
}
