/*! Hello world program */

#include <stdio.h>
#include <time.h>
#include <api/prog_info.h>
#include <arch/processor.h>

static timespec_t t0;


static void alarm_nt ( sigval_t param )
{
	timespec_t t;

	clock_gettime ( CLOCK_REALTIME, &t );
	time_sub ( &t, &t0 );
	t.tv_sec = 1;
	/*t.tv_nsec = 0;*/
	printf("In alarm, interrupt 36\n");
	raise_interrupt(36);
	/*printf ( "In alarm, now sleeping for %d sec\n", t.tv_sec);*/
	/*while ( TIME_IS_SET (&t) )*/
		/*if ( clock_nanosleep ( CLOCK_REALTIME, 0, &t, &t ) )*/
			/*printf ( "Interrupted sleep?\n" );*/
}

int hello_world ()
{
	timespec_t t;
	itimerspec_t t1, t2;
	timer_t timer1, timer2;
	sigevent_t evp;

	printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
		 timer_PROG_HELP );

	clock_gettime ( CLOCK_REALTIME, &t );
	t0 = t;
	printf ( "System time: %d:%d\n", t.tv_sec, t.tv_nsec/100000000 );

	evp.sigev_notify = SIGEV_THREAD;
	evp.sigev_notify_function = alarm_nt;
	evp.sigev_notify_attributes = NULL;

	/* timer1 */
	t1.it_interval.tv_sec = 0;
	t1.it_interval.tv_nsec = 0;
	t1.it_value.tv_sec = 4;
	t1.it_value.tv_nsec = 0;
	evp.sigev_value.sival_int = t1.it_interval.tv_sec;
	timer_create ( CLOCK_REALTIME, &evp, &timer1 );
	timer_settime ( &timer1, 0, &t1, NULL );
	/* timer2 */

	t2.it_interval.tv_sec = 0;
	t2.it_interval.tv_nsec = 0;
	t2.it_value.tv_sec = 7;
	t2.it_value.tv_nsec = 0;
	evp.sigev_value.sival_int = t2.it_interval.tv_sec;
	timer_create ( CLOCK_REALTIME, &evp, &timer2 );
	timer_settime ( &timer2, 0, &t2, NULL );




	t.tv_sec = 14;
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
