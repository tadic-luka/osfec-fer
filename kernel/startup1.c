/*! Startup function - initialize kernel subsystem */
#define _K_STARTUP_C_

#include <arch/processor25.h>
#include <api/stdio321.h>
#include <api/prog_info123.h>
#include <types/io4.h>
#include <kernel/errno4200.h>

/*! kernel stack */
uint8 system_stack [ STACK_SIZE ];

char system_info[] =         OS_NAME ": " NAME_MAJOR ":" NAME_MINOR ", "
                        "Version: " VERSION " (" ARCH ")";

/*!
 * First kernel function (after boot loader loads it to memory)
 */
void k_startup ()
{
        extern console_t K_INITIAL_STDOUT, K_STDOUT;
        extern console_t *k_stdout; /* console for kernel messages */

        /* set initial stdout */
        k_stdout = &K_INITIAL_STDOUT;
        k_stdout->init (0);

        /*! start with regular initialization */

        /* switch to default 'stdout' for kernel */
        k_stdout = &K_STDOUT;
        k_stdout->init (0);

        kprintf ( "%s\n", system_info );

        stdio_init (); /* initialize standard output devices */

        /* start desired program(s) */
        hello_world ();
	lab1_test ();
        debug ();

        kprintf ( "\nSystem halted!\n" );
        halt ();
}
