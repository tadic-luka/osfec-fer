/*! Interrupt handling - 'arch' layer (only basic operations) */

#define _ARCH_INTERRUPTS_C_
#include "interrupt.h"

#include <arch/processor.h>
#include <kernel/errno.h>
#include <lib/list.h>
#include <kernel/memory.h>

/*! Interrupt controller device */
extern arch_ic_t IC_DEV;
static arch_ic_t *icdev = &IC_DEV;

/*! For priority handling */
static int prio[INTERRUPTS];
static list_t reqs;
static int in_process = 0;
struct request {
	int priority;
	int in_process;
	void *device;
	int (*ihandler) (unsigned int, void *device);
	int irq;
	list_h list;
};

/*! interrupt handlers */
static list_t ihandlers[INTERRUPTS];

struct ihndlr
{
	void *device;
	int (*ihandler) ( unsigned int, void *device );

	list_h list;
};

int cmp_reqs_by_priority(void *a, void *b)
{
	struct request *req_a = (struct request *)a;
	struct request *req_b = (struct request *)b;

	if(prio[req_a->irq] < prio[req_b->irq])
		return -1;
	if(prio[req_a->irq] == prio[req_b->irq])
		return 1;
	return 0;
}
/*! Initialize interrupt subsystem (in 'arch' layer) */
void arch_init_interrupts ()
{
	int i;

	icdev->init ();

	for ( i = 0; i < INTERRUPTS; i++ ) {
		list_init ( &ihandlers[i] );
		// random_shuffle
		prio[i] = (i * 22) % INTERRUPTS;
	}
	// initalize list of reqs
	list_init(&reqs);
}

/*!
 * enable and disable interrupts generated outside processor, controller by
 * interrupt controller (PIC or APIC or ...)
 */
void arch_irq_enable ( unsigned int irq )
{
	icdev->enable_irq ( irq );
}
void arch_irq_disable ( unsigned int irq )
{
	icdev->disable_irq ( irq );
}

/*! Register handler function for particular interrupt number */
void arch_register_interrupt_handler ( unsigned int inum, void *handler,
				       void *device )
{
	struct ihndlr *ih;
	if ( inum < INTERRUPTS )
	{
		LOG ( DEBUG, "REGISTERING %d!\n", inum );
		ih = kmalloc ( sizeof (struct ihndlr) );
		ASSERT ( ih );

		ih->device = device;
		ih->ihandler = handler;

		list_append ( &ihandlers[inum], ih, &ih->list );
	}
	else {
		LOG ( ERROR, "Interrupt %d can't be used!\n", inum );
		halt ();
	}
}

/*! Unregister handler function for particular interrupt number */
void arch_unregister_interrupt_handler ( unsigned int irq_num, void *handler,
					 void *device )
{
	struct ihndlr *ih, *next;

	ASSERT ( irq_num >= 0 && irq_num < INTERRUPTS );

	ih = list_get ( &ihandlers[irq_num], FIRST );

	while ( ih )
	{
		next = list_get_next ( &ih->list );

		if ( ih->ihandler == handler && ih->device == device )
			list_remove ( &ihandlers[irq_num], FIRST, &ih->list );

		ih = next;
	}
}

/*!
 * "Forward" interrupt handling to registered handler
 * (called from interrupts.S)
 */
void arch_interrupt_handler ( int irq_num )
{
	struct ihndlr *ih;

	struct request *req;
	if ( irq_num < INTERRUPTS && (ih = list_get (&ihandlers[irq_num], FIRST)) )
	{
		LOG ( INFO, "Interrupt: %d", irq_num);
		/* enable interrupts on PIC immediately since program may not
		 * return here immediately */
		if ( icdev->at_exit )
			icdev->at_exit ( irq_num );

		/* Call registered handlers */
		while ( ih )
		{
			/*ih->ihandler ( irq_num, ih->device );*/
			req = kmalloc( sizeof(struct request) + 100000);
			if(req == NULL)
				break;
			/*ASSERT( req );*/

			req->priority = prio[irq_num];
			req->in_process = 0;
			req->device = ih->device;
			req->ihandler = ih->ihandler;
			req->irq = irq_num;
			LOG(INFO, "REQ %d inserted, in_process: %d", irq_num, in_process);

			list_sort_add (&reqs , req, &req->list, cmp_reqs_by_priority );

			ih = list_get_next ( &ih->list );
		}

		while((req = list_get(&reqs, FIRST)) && req->in_process == 0) {
			/*LOG(DEBUG, "GOT req with irq: %d, prio: %d", req->irq, req->priority);*/
			req->in_process = 1;
			++in_process;
			LOG(INFO, "Handling irq %d, in process: %d", req->irq, in_process);
			enable_interrupts();
			/*arch_irq_enable(req->irq);*/
			req->ihandler(req->irq, req->device);
			/*arch_irq_disable(req->irq);*/
			disable_interrupts();
			req->in_process = 0;
			req = list_remove(&reqs, FIRST, NULL);
			if( req ) {
				--in_process;
			LOG(INFO, "Handler irq %d, in process: %d", req->irq, in_process);
				kfree(req);
				req = NULL;
			}
		}
		if(req) {
			--in_process;
			LOG(INFO, "Handler irq %d, in process: %d", req->irq, in_process);
			list_remove(&reqs, FIRST, NULL);
			kfree(req);
		}
		/*arch_irq_disable(irq_num);*/
		/*arch_irq_enable(irq_num);*/
			/*disable_interrupts();*/
	}

	else if ( irq_num < INTERRUPTS )
	{
		LOG ( ERROR, "Unregistered interrupt: %d - %s!\n",
		      irq_num, icdev->int_descr ( irq_num ) );
		halt ();
	}
	else {
		LOG ( ERROR, "Unregistered interrupt: %d !\n", irq_num );
		halt ();
	}
}
