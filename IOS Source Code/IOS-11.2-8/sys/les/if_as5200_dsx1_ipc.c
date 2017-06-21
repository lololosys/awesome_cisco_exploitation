/* $Id: if_as5200_dsx1_ipc.c,v 3.1.14.5 1996/08/28 12:57:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_dsx1_ipc.c,v $
 *------------------------------------------------------------------
 * Including the routine to handle the message queue for ipc between
 * dsx1 firmware and IOS.
 *
 * April 1996, Tony Wu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_dsx1_ipc.c,v $
 * Revision 3.1.14.5  1996/08/28  12:57:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.14.4  1996/07/15  20:49:08  towu
 * CSCdi61121:  the IPC between MPU and NEAT crashes after 256 messages
 * are sent
 * Branch: California_branch
 *         o Fixed neat memory mapping during the sequence of receiving
 *         msg then
 *           transmit a message.
 *         o Removed wait_for_system_init call in ipc_reset process
 *         context.
 *           This will cause nvram configruation hung will cas channel
 *           involved.
 *           (CSCdi61551).
 *
 * Revision 3.1.14.3  1996/06/28  23:23:40  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.14.2  1996/06/19  19:29:53  snyder
 * CSCdi60807:  spelllink errors
 * Branch: California_branch
 *
 *         wiht-> with
 *         occured -> occurred
 *         unexpeced -> unexpected
 *
 * Revision 3.1.14.1  1996/06/16  21:14:38  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/05/19  20:07:11  vnguyen
 * Placeholder file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../if/if_msg_controller.c"	/* Not a typo, see logger.h */
#include "if_les.h"
#include "../if/if_controller.h"
#include "les_controller.h"
#include "../if/dsx1.h"
#include "if_as5200_dsx1_ipc.h"
#include "if_as5200_mcn.h"
#include "if_c4000_m32.h"
#include "if_c4000_dsx1.h"
#include "if_as5200_dsx1_framer.h"
#include "if_as5200_t1.h"
#include "if_as5200_dsx1.h"
#include "../os/timer_generic.h"
#include "packet.h"
#include "if_as5200_dummy.h"


static void t1_null_function(uchar slot, void *data);
static void t1_errmsg_report(uchar slot, void *data);	
static void t1_buginfo (uchar slot, void *s);
static void msg_process ( void );

typedef struct t1_msg_handle_ {
	neat_ipc_msg_function_t handler;
	neat_ipc_msg_priority	priority;
} t1_msg_handle;


static	t1_msg_handle t1_rcv_msg_table[MSG_NEAT_MAX];

/***************** local message buffer management definition ***********/

typedef struct  proc_level_msg {
		struct proc_level_msg	*next;	/* next proc_msg */
		ushort slot;			/* slot number */
		ushort msg_type;		/* message type */
		ushort msg_size;		/* message size */
		ushort data[MSG_DATA_SIZE];	/* data buffer */
} proc_level_msg_t;

typedef struct proc_msg_queue {
		proc_level_msg_t *first;
		proc_level_msg_t *last;
} proc_msg_queue_t;

#define MAX_PROC_LEVEL_MSGS	200  /* number of local message buffers */

static proc_level_msg_t proc_msg_bufs[MAX_PROC_LEVEL_MSGS];
static proc_msg_queue_t local_msgbuf_freelist;


static watched_queue *t1_proc_msg_queue;
static neat_ipc_message *t1_null_ptr = NULL;

ipc_section *ipc;
neat_ipc_info neat_ipc_slots[AS5200_MAX_SLOTS];

/*
 * dsx1_set_neat_page_reg
 * This function will set the neat page register to the specified
 * value and will return the previous page register setting.
 */
ushort dsx1_set_neat_page_reg (uchar slot, ushort new_page_reg)
{
    mcn_instance_t *mcn;
    ushort old_page_reg;
    leveltype      SR;       /* interrupt level */
    static ushort page_reg = 0;

    mcn = MCN_INSTANCE(slot);    /* get the mcn instance */

    SR = raise_interrupt_level(ALL_DISABLE);
    old_page_reg = page_reg;

    /* Change the page register for NEAT processor memory */
    mcn->mcn_regs->page_ram = (volatile short) new_page_reg;
    page_reg = new_page_reg;
    reset_interrupt_level(SR);	

    return old_page_reg;
}




/*
 * local_msg_enq -
 *   enqueue the local message to the end of queue
 */
static void local_msg_enq (proc_msg_queue_t *list, proc_level_msg_t *local_msg)
{
	leveltype SR;

	SR = raise_interrupt_level(NETS_DISABLE);

	if ( list->first == (proc_level_msg_t *)NULL )
		list->first = local_msg;
	else  
		list->last->next = local_msg;
	list->last = local_msg;
	local_msg->next = NULL;

	reset_interrupt_level(SR);
}

/*
 * local_msg_deq
 *   dequeue a local message from the beginning of queue
 */
static proc_level_msg_t *local_msg_deq (proc_msg_queue_t *list)
{
	leveltype SR;
	proc_level_msg_t *msg;

	SR = raise_interrupt_level(NETS_DISABLE);

	if ( list->first == NULL )
		msg = NULL;
	else  {
		msg = list->first;
		list->first = msg->next;
		if (list->first == NULL)
			list->last = NULL;
		msg->next = NULL;
	}

	reset_interrupt_level(SR);

	return msg;
}

/*
 * localmsg_getbuf -
 *    get an empty local message buffer.
 */
static proc_level_msg_t *localmsg_getbuf (void)
{

	return local_msg_deq(&local_msgbuf_freelist);
}	

/*
 * localmsg_freebuf -
 *    put a local message buffer to the free list.
 */
static void localmsg_freebuf (proc_level_msg_t *msg)
{

	local_msg_enq(&local_msgbuf_freelist, msg);
}	

/*
 * localmsg_init
 *  Initialize local message free list.
 */
static void localmsg_init ( void )
{
	int i;

	local_msgbuf_freelist.first = local_msgbuf_freelist.last = NULL;
	
	for ( i = 0; i < MAX_PROC_LEVEL_MSGS; i++)
		localmsg_freebuf(&proc_msg_bufs[i]);
}
/********************** end of local message buffer management ***************/

/*
 * =====================================================================
 * neat_msgbuf_lock
 *
 * Description:
 *	Locked rcv/xmt neat buffer pool pages. If some already locked with
 *	different pages return false. 
 *
 * Parameters: 
 *	slot - slot number
 *	flags - MSG_POOL_RCV/MSG_POOL_XMT
 * Returns:
 *	FLASE - buffer pool locked with different pool already
 *	TRUE  - otherwise.	
 *
 * =====================================================================
 */
static boolean neat_msgbuf_lock ( uchar slot, int flags)
{
        uint            temp;

	/* setup page register to point to message buffer space */
	if ( flags == MSG_POOL_RCV )
        	temp = (uint)&ipc->msgbuf_tompu;
	else
        	temp = (uint)&ipc->msgbuf_frommpu;
        temp &= PAGE_ADDR_MASK;
        temp >>= NEAT_PAGE_BITS_SHIFT;
	/* check someone did locked already */
	if (neat_ipc_slots[slot].locked == TRUE) {
		if (neat_ipc_slots[slot].lock_page == temp)
		/* don't bother, return from here */
			return TRUE;
		else
			/* some locked with other page */
			return FALSE;
	}
	neat_ipc_slots[slot].lock_page = temp; /* save the current page register */
	dsx1_set_neat_page_reg(slot, temp);
	neat_ipc_slots[slot].locked = TRUE;
	return TRUE;
}
/*
 * =====================================================================
 * neat_msgbuf_release
 *
 * Description:
 * reset the interrupt level back to the level before locking the msgbuf
 * space.
 *
 * Parameters: 
 *	slot - slot number
 * Returns:
 *	none	
 *
 * =====================================================================
 */
static void neat_msgbuf_release ( uchar slot)
{
	neat_ipc_slots[slot].locked = FALSE;
}
/*
 * =====================================================================
 * neat_msgbuf_locked -
 *
 * Description:
 * check if the neat page is locked with caller specified.
 * space.
 *
 * Parameters: 
 *	slot - slot number
 *	flags - indicate which neat page
 * Returns:
 *	TRUE: the page is locked
 *	FALSE: no page is locked or the page is different with caller 
 *		specified.	
 *
 * =====================================================================
 */
static boolean neat_msgbuf_locked ( uchar slot, int flags)
{
        uint            temp;

	if (neat_ipc_slots[slot].locked == FALSE)
		return FALSE;

	/* setup page register to point to message buffer space */
	if ( flags == MSG_POOL_RCV )
        	temp = (uint)&ipc->msgbuf_tompu;
	else
        	temp = (uint)&ipc->msgbuf_frommpu;

        temp &= PAGE_ADDR_MASK;
        temp >>= NEAT_PAGE_BITS_SHIFT;

	/* check current lock page */
	if (neat_ipc_slots[slot].lock_page == temp)
		return TRUE;
	else
		return FALSE;
		
}
/*
 * =====================================================================
 * t1_addr_n2c
 *
 * Description:
 * Convert neat address to mpu address.
 *
 * Parameters: 
 *	slot - slot number
 *	addr - neat's space address
 *
 * Returns:
 *	address under mpu address space.
 *
 * =====================================================================
 */
ushort *t1_addr_n2c (uchar slot, ushort  *addr)
{

        (uint)addr &= NEAT_PAGE_SPACE_MASK;
        (uint)addr += NEAT_SPACE;
        return ((ushort *)((uint)GET_SLOT_BASE(slot) + (uint)addr));
}
	

/*
 * =====================================================================
 * t1_addr_c2n
 *
 * Description:
 * Convert mpu address to neat address base upon the current page register
 *
 * Parameters: 
 *	slot - slot number
 *	addr - mpu space address
 *
 * Returns:
 *	address under neat address space.
 *
 * =====================================================================
 */
ulong t1_addr_c2n (uchar slot, ushort *addr)
{
	ushort	       page_num;

        mcn_instance_t *ds_mcn;

        ds_mcn = MCN_INSTANCE(slot);    /* get the mcn instance */

	page_num = neat_ipc_slots[slot].lock_page;
	return ((ulong)((page_num << NEAT_PAGE_BITS_SHIFT ) | 
			((ulong)addr & NEAT_PAGE_SPACE_MASK)));
}
/*
 * msg_enq -
 *	queue a message buffer to the end of queue.
 */
static void msg_enq (ipc_msg_queue *header, neat_ipc_message *msg, uchar slot)
{
	neat_ipc_message *item;
	neat_ipc_message *neat_msg_addr;

	neat_msg_addr = (neat_ipc_message *) t1_addr_c2n(slot, (ushort *)msg);
	if ( header->message_cnt == 0) {
		header->first_item = neat_msg_addr;
		header->last_item = neat_msg_addr;
	}
	else {
		item = (neat_ipc_message *)t1_addr_n2c(slot, (ushort *)header->last_item);
		NEAT_LONGWORD_WR(&(item->next), neat_msg_addr);
		NEAT_LONGWORD_WR(&(msg->next), t1_null_ptr);
		header->last_item = neat_msg_addr;
	}
	header->message_cnt++;
}

/*
 * msg_enq_common -
 *	queue a message buffer to the end of common queue.
 */
static void msg_enq_common (ipc_msg_queue *header, neat_ipc_message *msg, uchar slot)
{
	neat_ipc_message *prev_msg;
	neat_ipc_message *neat_msg_addr;

	neat_msg_addr = (neat_ipc_message *)t1_addr_c2n(slot, (ushort *)msg);

	if ( header->message_cnt == 0) {
		NEAT_LONGWORD_WR(&header->first_item, neat_msg_addr);
		NEAT_LONGWORD_WR(&header->last_item, neat_msg_addr);
	}
	else {
		prev_msg = (neat_ipc_message *)t1_addr_n2c(slot, (ushort *) NEAT_LONGWORD_RD(&header->last_item));
		NEAT_LONGWORD_WR(&(prev_msg->next), neat_msg_addr);
		NEAT_LONGWORD_WR(&(msg->next), t1_null_ptr);
		NEAT_LONGWORD_WR(&(header->last_item), neat_msg_addr);
	}
	header->message_cnt++;
}
/*
 * msg_deq -
 *     return and remove a message buffer from beginning of the queue. 
 */

static neat_ipc_message *msg_deq (ipc_msg_queue *header, uchar slot)
{
	neat_ipc_message *msg;

	if ( header->message_cnt == 0)
		msg = NULL;
	else {
		msg = (neat_ipc_message *)t1_addr_n2c(slot, (ushort *)header->first_item);
		header->first_item = (neat_ipc_message *)NEAT_LONGWORD_RD(&msg->next);
		header->message_cnt--;
		if ( header->message_cnt == 0)
			header->last_item = NULL;
		NEAT_LONGWORD_WR(&msg->next, t1_null_ptr);
	}
	return msg;
}

#ifdef NODEF
/*
 * msg_deq_common -
 *     return and remove a message buffer from beginning of the common queue. 
 */

static neat_ipc_message *msg_deq_common (ipc_msg_queue *header, uchar slot)
{
	neat_ipc_message *msg;
	neat_ipc_message *tmp;

	if ( header->message_cnt == 0)
		msg = NULL;
	else {
		msg = (neat_ipc_message *)t1_addr_n2c(slot, (ushort *)NEAT_LONGWORD_RD(&header->first_item));
		tmp = (neat_ipc_message *)NEAT_LONGWORD_RD(&msg->next);
		NEAT_LONGWORD_WR(&header->first_item, tmp);
		header->message_cnt--;
		if ( header->message_cnt == 0)
			NEAT_LONGWORD_WR(&header->last_item, t1_null_ptr);
	}
	return msg;
}

#endif /* NODEF */

/*
 * msg_que_merge_cp -
 *	merge 2nd queue within the 1st queue.
 *	Note: The 1st queue is common queue and 2nd queue is private queue.
 */
static void msg_que_merge_cp (ipc_msg_queue *h1, ipc_msg_queue *h2, uchar slot)
{
	neat_ipc_message *last_msg;

	if ( h2->message_cnt != 0 ) {
		if ( h1->message_cnt == 0 ) {
			NEAT_LONGWORD_WR(&h1->first_item, h2->first_item);
		}
		else {
			last_msg = (neat_ipc_message *)t1_addr_n2c(slot,  
			  (ushort *) NEAT_LONGWORD_RD(&h1->last_item));
			NEAT_LONGWORD_WR(&last_msg->next, h2->first_item);
		}

		NEAT_LONGWORD_WR(&h1->last_item, h2->last_item);
		h1->message_cnt += h2->message_cnt;

		h2->first_item = h2->last_item = NULL;
		h2->message_cnt = 0;
	}
}

/*
 * msg_que_merge_pc -
 *	merge 2nd queue within the 1st queue.
 *	Note: The 1st queue is private queue and 2nd queue is common queue.
 */
static void msg_que_merge_pc (ipc_msg_queue *h1, ipc_msg_queue *h2, uchar slot)
{
	neat_ipc_message *last_msg;
	ulong tmp;

	if ( h2->message_cnt != 0 ) {
		if ( h1->message_cnt == 0 )
			h1->first_item = (neat_ipc_message *)NEAT_LONGWORD_RD(&h2->first_item);
		else {
			last_msg = (neat_ipc_message *)t1_addr_n2c(slot, (ushort *)h1->last_item);
			tmp = NEAT_LONGWORD_RD(&h2->first_item);
			NEAT_LONGWORD_WR(&last_msg->next, tmp);
		}

		h1->last_item = (neat_ipc_message *)NEAT_LONGWORD_RD(&h2->last_item);
		h1->message_cnt += h2->message_cnt;

		NEAT_LONGWORD_WR(&h2->first_item, t1_null_ptr);
		NEAT_LONGWORD_WR(&h2->last_item, t1_null_ptr);
		h2->message_cnt = 0;
	}
}
/*
 * msg_scan -
 *	return a message pointer which is the same as current pointer in the queue.
 *	and update current pointer to point to next item in the queue.
 */
static  neat_ipc_message *msg_scan (ipc_msg_queue *h, uchar slot)
{
	neat_ipc_message *msg;
	neat_ipc_message *mpu_msg;
	ulong tmp;

	if ((msg = (neat_ipc_message *)NEAT_LONGWORD_RD(&h->current)) != NULL) { /* Get current message */
		mpu_msg = (neat_ipc_message *)t1_addr_n2c(slot, (ushort *)msg);
		tmp = NEAT_LONGWORD_RD(&mpu_msg->next);
		NEAT_LONGWORD_WR(&h->current, tmp);
	}
	else {
		mpu_msg = NULL;
	}
	return mpu_msg;
}
/*
 * neat_ipc_init -
 *    initialize ipc message buffer management. It should be called when 
 *    anaylize inited. The message buffers pool is located at NEAT memory space.
 *    It is devided to two sections, one for messages sending to NEAT, the other
 *    for messages sending from NEAT.  Each message buffers section has its own
 *    free header and work queue header located . There are two common
 *    queue headers located at NEAT memory space, msg_from_neat and msg_to_neat.
 *    the "free header" is used for the message buffers are valid to use.
 *    The "work header" is used to queue the message buffers which is ready to
 *    send to NEAT while msg_to_neat header is not owned by MPU. The 
 *    "msg_to_neat" header can be processed by both processors. It come with
 *    an ownership flag to indicate the current processor can access the queue.
 *    The "msg_from_neat" is the same as "msg_to_neat". The difference between
 *    those two queues is that msg_to_neat header is for the messages to send
 *    to NEAT, while msg_from_neat header are for the messages to send from NEAT
 */ 

void neat_ipc_init (uchar slot)
{
	int i;
	neat_ipc_info *neat_q_inf;
	static boolean  first_time = TRUE;
	pid_t	t1_msg_proc_id;

	/* initialize local message buffer management */
	if (first_time == TRUE) {
    		/* Setup ipc address */
    		ipc = (ipc_section *)IPC_AREA;
		if (dsx1_cas_supp == FALSE)
			return;
		localmsg_init();
		/* create watch_queue for process level message handler */
		t1_proc_msg_queue = create_watched_queue("t1 message queue",0 ,0);
		t1_msg_proc_id = process_create(msg_process, "Neat msg", NORMAL_STACK, PRIO_NORMAL);
		if (t1_msg_proc_id == NO_PROCESS) {
			printf("Can not create neat ipc message process\n");
			crashdump(0);
		}
		/* clean the receiving function table */
		for ( i = 0; i < MSG_NEAT_MAX; i++) {
			t1_rcv_msg_table[i].handler = t1_null_function;
			t1_rcv_msg_table[i].priority = NEAT_IPC_MSG_PRI_LO;
		}

		/* resgister receive message handler */
		neat_ipc_message_register( MSG_NEAT_ERROR_REPORT,  
				t1_errmsg_report, NEAT_IPC_MSG_PRI_HI);
		neat_ipc_message_register( MSG_NEAT_BUGINFO, t1_buginfo, 
					NEAT_IPC_MSG_PRI_HI);
		t1_cas_init(slot);
		first_time = FALSE;
	}

	neat_q_inf = &neat_ipc_slots[slot];

	/* allocate private queue headers for work and free queues */
	if ((neat_q_inf->work = malloc_fast(sizeof(ipc_msg_queue))) == NULL)
			crashdump(0);
	if ((neat_q_inf->free = malloc_fast(sizeof(ipc_msg_queue))) == NULL)
			crashdump(0);
	neat_q_inf->msg_from_neat = (ipc_msg_queue *)t1_addr_n2c(slot, 
				(ushort *)&ipc->msg_to_mpu);
	neat_q_inf->msg_to_neat = (ipc_msg_queue *)t1_addr_n2c(slot, 
				(ushort *)&ipc->msg_from_mpu);
}

/*      neat_ipc_reset -
 * 		reset neat ipc queueing system.
 *		This routine should be called whenever the download happened.
 */

process neat_ipc_reset (void )
{
	neat_ipc_info *neat_q_inf;
	neat_ipc_message *msg;
	ipc_msg_queue	*msg_to_neat;
	int i;
        leveltype sr;
	ulong	slot;

	if (dsx1_cas_supp == FALSE)
		process_kill(THIS_PROCESS);

	if (!process_get_arg_num(&slot)) {
	   printf("neat_ipc_reset process: not get argument\n");
	   process_kill(THIS_PROCESS);
	}
		    
	neat_q_inf = &neat_ipc_slots[slot];

	bzero((void *)neat_q_inf->work, sizeof(ipc_msg_queue));
	bzero((void *)neat_q_inf->free, sizeof(ipc_msg_queue));

        sr = raise_interrupt_level(NETS_DISABLE);


	/* convert the queue header neat_q_inf->msg_from_mpu to mpu address space */
	msg_to_neat = neat_q_inf->msg_to_neat;
	
	/* setup page register to point to message buffer space */
        /* release page buffer pool first */
        neat_msgbuf_release(slot);
	neat_msgbuf_lock(slot, MSG_POOL_XMT);
	/* wait for the ownership changed to mpu */
	while (msg_to_neat->owner != IPCQ_OWNER_CORE) {
		neat_msgbuf_release(slot);
                reset_interrupt_level(sr);
		process_sleep_for(ONESEC/8);
		/* setup page register to point to message buffer space */
                sr = raise_interrupt_level(NETS_DISABLE);
		neat_msgbuf_lock(slot, MSG_POOL_XMT);
	}

	for ( msg = (neat_ipc_message *)t1_addr_n2c(slot, 
	(ushort *)ipc->msgbuf_frommpu), i = 0; i < TOTAL_MSG_BUFFERS ; i++)
		/* link each buffer in the free list */
		msg_enq(neat_q_inf->free, &msg[i], slot);

	neat_msgbuf_release(slot);
	neat_q_inf->init_status = IPC_INITED;
        reset_interrupt_level(sr);
	process_kill(THIS_PROCESS);         /* terminate process */

}
	
/*
 * msg_getbuf -
 * 	return and demove a message buffer from the free list.
 */
static neat_ipc_message *msg_getbuf (uchar slot )
{
	return (msg_deq(neat_ipc_slots[slot].free, slot));
}

#ifdef NODEF
/*
 * msg_putbuf -
 *	put and enqueue a message buffer to the free list.
 */

static void msg_putbuf (uchar slot, neat_ipc_message *msg)
{

	return (msg_enq(neat_ipc_slots[slot].free, msg, slot));
}

#endif /* NODEF */
/*
 * msg_send -
 *	If the owner of msg_to_mpu is IPCQ_OWNER_NEAT, then enqueue the item
 *	to the msg_to_mpu and notify the core by initiate level 4 interrupt.
 *	If the owner of msg_to_mpu is IPCQ_OWNER_CORE, then enqueue the item
 *	to the msg_working_queue.
 */
static void msg_send (uchar slot, neat_ipc_message *msg)
{
	neat_ipc_info *neat_q_inf;
	ipc_msg_queue *msg_to_neat;
	ulong item;

	neat_q_inf = &neat_ipc_slots[slot];
	msg_to_neat = neat_q_inf->msg_to_neat;

	if  (msg_to_neat->owner == IPCQ_OWNER_NONE) {
		/* It shouldn't be occur */
		/* put the messages in the msg_to_mpu back to free list */
		msg_que_merge_pc(neat_q_inf->free, msg_to_neat, slot);
		/* Change the ownership to NEAT processor */
		msg_to_neat->owner = IPCQ_OWNER_CORE;
		msg_to_neat->who_wrote = (WHO_MSG_SEND|WHO_CORE);
	}
	
	if (msg_to_neat->owner == IPCQ_OWNER_CORE) {
		/* move working queue to ready queue. The number of messsages in 
		   msg_working_queue should be 0 in most of time.*/
		if (neat_q_inf->work->message_cnt > 0)
			msg_que_merge_cp(msg_to_neat, neat_q_inf->work, slot);
		/* enqueue the current message in the msg_to_mpu queue */
		msg_enq_common(msg_to_neat, msg, slot);
		/* update the current to point to first item in the queue */
	        item = (ulong)NEAT_LONGWORD_RD(&msg_to_neat->first_item);
		NEAT_LONGWORD_WR(&msg_to_neat->current, item); 
		/* change ownership to CORE and interrupt it */
		msg_to_neat->owner = IPCQ_OWNER_NEAT;
		msg_to_neat->who_wrote = (WHO_MSG_SEND|WHO_CORE);
		t1_neat_int(slot);      /* initiate interrupt to neat processor */
	}
	else 
		/* queue the message to the working queue if msg_to_neat queue is not
		   owned by CORE */
		msg_enq (neat_q_inf->work, msg, slot);

}
/*
 * msg_queue_check --
 *    This routine will be called in the level 4 interrupt context to see if neat has
 *	finished the processes of all the messages in the queue. Return those messages
 *	to the free list. If there are outstanding messages, move them from working 
 *	queue to msg_to_neat queue and send them to neat. Otherwise, change the 
 *	ownership of msg_to_neat.
 */
void msg_queue_check ( uchar slot )
{
	neat_ipc_info *neat_q_inf;
	ipc_msg_queue	*msg_to_neat;
	ulong	tmp;

	neat_q_inf = &neat_ipc_slots[slot];
	msg_to_neat = neat_q_inf->msg_to_neat;

	if  (msg_to_neat->owner == IPCQ_OWNER_NONE) {
		/* queue must be empty now! */
		msg_que_merge_pc(neat_q_inf->free, msg_to_neat, slot);
		if (neat_q_inf->work->message_cnt > 0 ) {
			msg_que_merge_cp(msg_to_neat, neat_q_inf->work, slot);
			tmp = NEAT_LONGWORD_RD(&msg_to_neat->first_item);
			NEAT_LONGWORD_WR(&msg_to_neat->current, tmp);
			/* change ownership to CORE and interrupt it */
			msg_to_neat->owner = IPCQ_OWNER_NEAT;
			msg_to_neat->who_wrote = (WHO_CHECK|WHO_CORE);
			t1_neat_int(slot);
	        }	
		else {
			msg_to_neat->owner = IPCQ_OWNER_CORE;
			msg_to_neat->who_wrote = (WHO_CHECK|WHO_CORE);
		}
	}
}

/*
 * msg_get --
 *	return a message from msg_from_mpu queue.
 */
static neat_ipc_message *msg_get (uchar slot)
{

	neat_ipc_message *msg = NULL;
	neat_ipc_info *neat_q_inf;
	ipc_msg_queue	*msg_from_neat;
	boolean		release_queue = FALSE;

	neat_q_inf = &neat_ipc_slots[slot];
	msg_from_neat = neat_q_inf->msg_from_neat;

	if ( msg_from_neat->owner == IPCQ_OWNER_CORE ) {
		if ( msg_from_neat->message_cnt == 0 ) {
		    release_queue = TRUE;
		}
		else {
		    msg = msg_scan(msg_from_neat, slot);
		    if ( msg == NULL )
			release_queue = TRUE;
		}
		if (release_queue == TRUE) {
			msg_from_neat->owner = IPCQ_OWNER_NONE;
			msg_from_neat->who_wrote = (WHO_GET|WHO_CORE);
			t1_neat_int(slot);
		}
	}
	return msg;
}


static uchar t1_info_stream[256];
/*
 * msg_parser_high --
 * 	handle the message in interrupt context.
 */
static void msg_parser_high ( uchar slot, neat_ipc_message *msg)
{
	ushort para[9];
	ushort *d, *s, *dest;
	ushort size;
	ushort msg_size;
	ulong temp;
	ushort msg_type;

	msg_type = msg->msg_type;
	msg_size = msg->length;

	/* copy data buffer */
	if ( msg->length > MSG_DATA_SIZE) {
		s = (ushort *)t1_addr_n2c(slot, (ushort *)msg->data);
		/* setup page register to point to message buffer space */
        	temp = (ulong)msg->data;
        	temp &= PAGE_ADDR_MASK;
        	temp >>= NEAT_PAGE_BITS_SHIFT;
		dsx1_set_neat_page_reg(slot, temp);
		dest = d  =  (ushort *)t1_info_stream;

       }
       else {
		s = (ushort *)t1_addr_n2c(slot, (ushort *)msg->para);
		dest = d =  para;
       }

       size = (msg_size >> 1);
       if ( msg_size & 1 ) size++;
		
	while ( size--)
		*d++ = *s++;

	if ( msg_size > MSG_DATA_SIZE )
		/* restore locked page number */
	    dsx1_set_neat_page_reg(slot, neat_ipc_slots[slot].lock_page);

	(t1_rcv_msg_table[msg_type].handler)(slot, (uchar *)dest);
}

/*
 *	message handler function has not been registered
 */
static void t1_null_function ( uchar slot, void *data)
{
	buginf("\n t1 null function");
}

/*
 * t1_errmsg_report -
 *     message handler for errmsg
 */
static void t1_errmsg_report (uchar slot, void *s)
{
	char name[10];
	sprintf(name, "<T1-%d>", slot);
	errmsg(&msgsym(FIRMWARE, CONTROLLER), name, s);
}

/*
 * t1_buginfo -
 *	message handler for buginf
 */
static void t1_buginfo (uchar slot, void *s)
{
	buginf("\nt1_neat at slot %d: %s", slot, s);
}

/*
 * msg_parser_low --
 * 	parseing the t1 message in process level
 */
static void msg_parser_low ( void )
{

        proc_level_msg_t *lmsg;

	while ((lmsg = process_dequeue(t1_proc_msg_queue))) {
	  (t1_rcv_msg_table[lmsg->msg_type].handler)(lmsg->slot, (uchar *)lmsg->data);
	  localmsg_freebuf(lmsg);
        }	


}

/*
 * msg_process --
 * 	The process to handle low priority ipc message.
 */
static void msg_process ( void )
{
	ulong major, minor;

	if (dsx1_cas_supp == FALSE)
		process_kill(THIS_PROCESS);
	/* Make sure everybody else is ready */
	process_wait_on_system_init();

	process_watch_queue(t1_proc_msg_queue, ENABLE, RECURRING);
	for (;;) {
		process_wait_for_event();
		while (process_get_wakeup(&major, &minor)) {
			switch (major) {
			
			case QUEUE_EVENT: 
			     
			     msg_parser_low();
			     break;
			
			default:
                		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
				break;
			}
		}
	}
}

/*
 * msg_send_to_process -
 * 	hand message to process queue.
 */
static void msg_send_to_process ( uchar slot, neat_ipc_message *msg)
{
	proc_level_msg_t *lmsg;
	ushort *d, *s;
	ushort size;
	ushort msg_size;

	msg_size = msg->length;

	/* Get a local message buffer */
	if ((lmsg = localmsg_getbuf()) == NULL) {
		/* drop the message and printout worning message */
		buginf("\nneat_ipc:Running out of local message buffers");
		return;
	}

	if (msg->flags & IPCMSGFLAG_DATA_EXT) {
		/* Unable to handle a message with external data buffer */
		buginf("\nneat_ipc:Unable to handle a low priority message with external buffer. message type = %d", msg->msg_type);
		localmsg_freebuf(lmsg);
		return;
	}
	/* copy data buffer */
	if ( msg->length > MSG_DATA_SIZE) {
		buginf("\nneat_ipc: The message should not have length greater than 18 bytes with internal message data buffer");
		localmsg_freebuf(lmsg);
		return;

        }
	s = (ushort *)msg->para;
	d =  lmsg->data;

        size = (msg_size >> 1);
        if ( msg_size & 1 ) size++;
		
	while ( size--)
		*d++ = *s++;
	lmsg->msg_type = msg->msg_type;
	lmsg->msg_size = msg->length;
	lmsg->slot = slot;

	if (process_enqueue(t1_proc_msg_queue, lmsg) == FALSE) {
	   buginf("\nneat_ipc: unable to enqueue message to process queue");
	   localmsg_freebuf(lmsg);
	}
}
/*
 * neat_ipc_message_register
 *  register t1 message callback routine.
 */
void neat_ipc_message_register (ushort msg_type, neat_ipc_msg_function_t function,  ulong priority)
{
	if (msg_type >	MSG_NEAT_MAX)
		crashdump(0);
	if (priority != NEAT_IPC_MSG_PRI_LO && priority != NEAT_IPC_MSG_PRI_HI)
		crashdump(0);
	t1_rcv_msg_table[msg_type].priority = priority;
	t1_rcv_msg_table[msg_type].handler = function;
}

/*
 * neat_ipc_message_send -
 * 	compose a message buffer and send to NEAT processor
 */
	
neat_ipc_msg_ret neat_ipc_message_send (uchar slot, ushort msg_type, uchar *data, ushort size)
{
	neat_ipc_message *msg;
	ushort *dp, *sp;
	ushort cp_size;
	leveltype sr;
	ushort restore_lock = 0;
	neat_ipc_msg_ret error = IPCMSG_ERROR_NOERROR;
	
	if ( size > MSG_DATA_SIZE)
		return IPCMSG_ERROR_ILLEGAL_SIZE;

	sr = raise_interrupt_level(NETS_DISABLE);
	if (neat_msgbuf_locked(slot, MSG_POOL_RCV) == TRUE) {
		neat_msgbuf_release(slot);
		restore_lock = MSG_POOL_RCV;
	}
	if (neat_msgbuf_lock(slot, MSG_POOL_XMT) == FALSE)
		crashdump(0);

	if ( (msg = msg_getbuf(slot)) != NULL) {
		msg->flags = 0;
		msg->msg_type = msg_type;
		msg->flags = 0;
		msg->length = size;
		dp = (ushort *)data;
		cp_size = (size >> 1);
		if ( size & 1)
			cp_size++;
		for ( sp = (ushort *)msg->para, dp = (ushort *)data; cp_size != 0; 
			 cp_size--)
			*sp++ = *dp++;
		msg_send(slot, msg);
	}
	else
		error = IPCMSG_ERROR_NOBUF;		

	neat_msgbuf_release(slot);

	if (restore_lock == MSG_POOL_RCV)
		if (neat_msgbuf_lock(slot, MSG_POOL_RCV) == FALSE)
			crashdump(0);

	reset_interrupt_level(sr);
	return error;
}

void neat_msgrcv_process ( uchar slot)
{
	neat_ipc_message  *msg;
	
	int	restore_lock = 0;

	if (dsx1_cas_supp == FALSE)
		return;

	
	 /* First, check if messages in msg_to_neat queue have been processed by NEAT */
	if (neat_msgbuf_locked(slot, MSG_POOL_RCV) == TRUE)
		restore_lock = MSG_POOL_RCV;
	else if (neat_msgbuf_locked(slot, MSG_POOL_XMT) == TRUE)
		restore_lock = MSG_POOL_XMT;

	if (neat_ipc_slots[slot].init_status == IPC_INITED) {
	/* check the transmit buffer header only when MPU is ready for IPC */
		/* do the release anyway */
		neat_msgbuf_release(slot);
		neat_msgbuf_lock(slot, MSG_POOL_XMT);
		msg_queue_check( slot);
	}

	neat_msgbuf_release(slot);
	neat_msgbuf_lock(slot, MSG_POOL_RCV);

	/* check if any receiving message outstanding */
	for( msg = msg_get(slot); msg != NULL; msg = msg_get(slot)) {
		if (t1_rcv_msg_table[msg->msg_type].priority == NEAT_IPC_MSG_PRI_HI)
			/* message have to be handle in the interrupt level */
			msg_parser_high(slot, msg);
		else
			/* message have to be handle in the process level */
			msg_send_to_process(slot, msg);
	}
	neat_msgbuf_release(slot);
	if (restore_lock != 0)
		neat_msgbuf_lock(slot, restore_lock);
}

