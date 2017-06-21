/* $Id: neat_ipc.c,v 1.4 1996/07/13 05:28:51 towu Exp $
 * $Source: /cscka/t1_neat/src/common/neat_ipc.c,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * March 1996, Tony Wu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: neat_ipc.c,v $
 * Revision 1.4  1996/07/13 05:28:51  towu
 * o Added trace log routines.
 * o Inserted trace in each routine.
 * o Changed the number of local message buffers from 200 to 400
 *
 * Revision 1.3  1996/05/12  23:50:16  towu
 * Fixed bugs for ct1 rbs features.
 *
 * Revision 1.2  1996/05/09  21:42:59  towu
 * Added features to provide message ipc queue.
 *
 * Revision 1.1  1996/04/21  00:17:36  towu
 * Created for supporting message queue and robbed bit signals
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef T1_FIRMWARE
#include "t1_headers.h"
#else
#include "e1_headers.h"
#endif


static ipc_msg_queue msg_working_queue;		/* queue for the messages outstanding */
static ipc_msg_queue msg_free_list;		/* free list of message pool */
static ipc_msg_queue  *msg_to_mpu;		/* point to the queue that including messages
						   ready to be read by mpu */

static ipc_msg_queue  *msg_from_mpu;	/* point to the queue that including messages
						   ready to be read by neat */


static void neat_ipc_null_function(void *data);


typedef struct neat_ipc_msg_handle_ {
	neat_ipc_msg_function_t handler; 
	neat_ipc_msg_priority	priority;
} neat_ipc_msg_handle;


static	neat_ipc_msg_handle neat_ipc_msg_table[MSG_MPU_MAX];

/******************* trace log *************************/
#ifdef IPC_TRACE
typedef struct {
	ulong a;
	ulong b;
	ulong c;
	ulong d;
} trace_type;

trace_type trace_buff[MAX_IPC_TRACE_SIZE];
trace_type *tep = trace_buff;
trace_type *trace_end = &trace_buff[MAX_IPC_TRACE_SIZE];

void trace_log(ulong a, ulong b, ulong c, ulong d)
{
	ushort sr =  set_ipl(PRIO_FDL);
	tep->a = a;
	tep->b = b;
	tep->c = c;
	tep->d = d;
	tep++;
	if (tep	== trace_end)
		tep = trace_buff;
	reset_ipl(sr);
}
#endif /* IPC_TRACE */
/******************* trace log end *************************/

/***************** local message buffer management *******************/

typedef struct  proc_level_msg {
		struct proc_level_msg	*next;	/* next proc_msg */
		ushort msg_type;		/* message type */
		ushort msg_size;		/* message size */
		ushort data[MSG_DATA_SIZE];	/* data buffer */
} proc_level_msg_t;

typedef struct proc_msg_queue {
		proc_level_msg_t *first;
		proc_level_msg_t *last;
} proc_msg_queue_t;

#define MAX_PROC_LEVEL_MSGS	400  /* number of local message buffers */

static proc_level_msg_t proc_msg_bufs[MAX_PROC_LEVEL_MSGS];
static proc_msg_queue_t local_msgbuf_freelist;
static proc_msg_queue_t local_msgqueue;


/*
 * local_msg_enq -
 *   enqueue the local message to the end of queue
 */
static void local_msg_enq (proc_msg_queue_t *list, proc_level_msg_t *local_msg)
{
	ushort sr =  set_ipl(PRIO_FDL);

	TRACE(LCL_MSG_ENQ, list, local_msg, T_BEGIN);
	if ( list->first == (proc_level_msg_t *)NULL )
		list->first = local_msg;
	else  
		list->last->next = local_msg;
	list->last = local_msg;
	local_msg->next = NULL;
	TRACE(LCL_MSG_ENQ, list, local_msg, T_END);
	reset_ipl(sr);
}

/*
 * local_msg_deq
 *   dequeue a local message from the beginning of queue
 */
static proc_level_msg_t *local_msg_deq (proc_msg_queue_t *list)
{
	proc_level_msg_t *msg;
	ushort sr =  set_ipl(PRIO_FDL);
	
	TRACE(LCL_MSG_DEQ, list, 0, T_BEGIN);
	if ( list->first == NULL )
		msg = NULL;
	else  {
		msg = list->first;
		list->first = msg->next;
		if (list->first == NULL)
			list->last = NULL;
		msg->next = NULL;
	}

	reset_ipl(sr);
	TRACE(LCL_MSG_DEQ, list, msg, T_END);
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
	bzero(&local_msgqueue, sizeof(proc_msg_queue_t)); /* clean local message queue */
}

/****************  End of local message queue process section ********************/

/*
 * msg_enq -
 *	queue a message buffer to the end of queue.
 */
static void msg_enq(ipc_msg_queue *header, neat_ipc_message *msg)
{
	ushort sr =  set_ipl(PRIO_FDL);	

	TRACE(IPC_MSG_ENQ, header, msg, T_BEGIN);
	if ( header->message_cnt == 0) {
		header->first_item = msg;
		header->last_item = msg;
	}
	else {
		header->last_item->next = msg;
		header->last_item = msg;
	}
	msg->next = NULL;
	header->message_cnt++;
	TRACE(IPC_MSG_ENQ, header, header->message_cnt, T_END);
	reset_ipl(sr);	
}
/*
 * msg_deq -
 *     return and remove a message buffer from beginning of the queue. 
 */


static neat_ipc_message *msg_deq(ipc_msg_queue *header)
{
	neat_ipc_message *msg;
	ushort sr =  set_ipl(PRIO_FDL);	

	TRACE(IPC_MSG_DEQ, header, 0, T_BEGIN);

	if (header->message_cnt == 0) {
		msg = NULL;
	}
	else {
		msg = header->first_item;
		header->first_item = msg->next;
		header->message_cnt--;
		if ( header->message_cnt == 0)
			header->last_item = NULL;
		msg->next = NULL;
		TRACE(IPC_MSG_DEQ, header, header->message_cnt, T_BEGIN);
	}
	TRACE(IPC_MSG_DEQ, header, msg, T_END);
	reset_ipl(sr);	
	return msg;
}

/*
 * msg_que_merge -
 *	merge 2nd queue within the 1st queue.
 */
static void msg_que_merge (ipc_msg_queue *h1, ipc_msg_queue *h2)
{
	ushort sr =  set_ipl(PRIO_FDL);	

	TRACE(IPC_QUE_MRG, h1, h2, T_BEGIN);
	if ( h2->message_cnt != 0 ) {
		if ( h1->message_cnt == 0 )
			h1->first_item = h2->first_item;
		else
			h1->last_item->next = h2->first_item;

		h1->last_item = h2->last_item;
		h1->message_cnt += h2->message_cnt;

		h2->first_item = h2->last_item = NULL;
		h2->message_cnt = 0;
	}
	TRACE(IPC_QUE_MRG, h1, h2, T_END);
	reset_ipl(sr);	
}

/*
 * msg_scan -
 *	return a message pointer which is the same as current pointer in the queue.
 *	and update current pointer to point to next item in the queue.
 */
static  neat_ipc_message *msg_scan (ipc_msg_queue *h)
{
	neat_ipc_message *msg;

	if ((msg = h->current) != NULL)    /* Get current message */
		h->current = msg->next;    /* prepare for next scan */
	else
		h->current = NULL;
	return msg;
}

/*
 * neat_ipc_msg_mgmt_init -
 *    initialize ipc message buffer management. It should be called when NEAT processor inited.
 *    Basically, The message buffers pool is divided into two pools. one for the messages 
 *    to send to MPU, the other is for the messages to send from MPU. The reason for that is
 *    to prevent two processors (NEAT and MPU) from accessing the same queue.
 *    The messages in the pool will be queued up in the free lists ( one for each type of 
 *    messages pools) in this routine. The working and free list queue are private queue ( only
 *    accessed by NEAT processor, and msg_to_mpu is the queue that will be processed by both
 *    MPU and NEAT. The way to prevent the queue from accessing by both processors at the 
 *    same time is to use ownership flag in the queue header. The NEAT processor has ability
 *    to assign the owner to MPU, while the MPU can only release the ownership.  The MPU can
 *    only process the queue when the ownership is assigned to him and release the queue
 *    whenever it done with the queue.
 *    
 */
void neat_ipc_msg_mgmt_init (void)
{
	int i;
	neat_ipc_message *msg;

	if ( ct1_enable == 0)
		return;
	/* init the 3 queue headers, working_queue, msg_free_list and ready queue. The ready_queue
	   has to be in the ipc_section due to the MPU access */
	mem_init();
	bzero((void *)&msg_working_queue, sizeof(ipc_msg_queue));
	bzero((void *)&msg_free_list, sizeof(ipc_msg_queue));
	bzero((void *)&ipc->msg_to_mpu, sizeof(ipc_msg_queue));
	bzero((void *)&ipc->msg_from_mpu, sizeof(ipc_msg_queue));

	msg_to_mpu = &ipc->msg_to_mpu;
	msg_from_mpu = &ipc->msg_from_mpu;

	/* initialize the free list first. Assign first half of the buffer pool
	   to free list */
	for ( msg = ipc->msgbuf_tompu, i = 0; i < TOTAL_MSG_BUFFERS ; i++)
		msg_enq(&msg_free_list, &msg[i]);
	localmsg_init();
	for (i = 0; i < MSG_MPU_MAX; i++) {
		neat_ipc_msg_table[i].priority = NEAT_IPC_MSG_PRI_LO;
		neat_ipc_msg_table[i].handler = neat_ipc_null_function;
	}
	msg_to_mpu->owner = IPCQ_OWNER_NEAT;	/* Set the owner to NEAT */
	msg_from_mpu->owner = IPCQ_OWNER_CORE;
	msg_to_mpu->who_wrote = (WHO_INIT|WHO_NEAT);
	msg_from_mpu->who_wrote = (WHO_INIT|WHO_NEAT);
}
	
/*
 * msg_getbuf -
 * 	return and demove a message buffer from the free list.
 */
neat_ipc_message *msg_getbuf (void )
{
	return (msg_deq(&msg_free_list));
}

/*
 * msg_putbuf -
 *	put and enqueue a message buffer to the free list.
 */

void msg_putbuf (neat_ipc_message *msg)
{
	return (msg_enq(&msg_free_list, msg));
}

/*
 * msg_send -
 *	If the owner of msg_to_mpu is IPCQ_OWNER_NEAT, then enqueue the item
 *	to the msg_to_mpu and notify the core by initiate level 4 interrupt.
 *	If the owner of msg_to_mpu is IPCQ_OWNER_CORE, then enqueue the item
 *	to the msg_working_queue.
 */
void msg_send (neat_ipc_message *msg)
{
	neat_ipc_message *m;
        ushort sr =  set_ipl(PRIO_FDL);
	
	TRACE(IPC_MSG_SEND, msg, msg->msg_type, T_BEGIN);
	if  (msg_to_mpu->owner == IPCQ_OWNER_NONE) {
		TRACE(IPC_MSG_SEND, msg, msg->msg_type, 1);
		/* check if any buffer need to return */
		msg_to_mpu->current = msg_to_mpu->first_item;
		while ( (m = msg_scan(msg_to_mpu)) != NULL) {
			if ( m->flags & IPCMSGFLAG_DATA_EXT)
				free(m->data);
				m->data = NULL;
				m->flags &= ~IPCMSGFLAG_DATA_EXT;
		}
		/* It shouldn't be occur */
		/* put the messages in the msg_to_mpu back to free list */
		msg_que_merge(&msg_free_list, msg_to_mpu);
		/* Change the ownership to NEAT processor */
		msg_to_mpu->owner = IPCQ_OWNER_NEAT;
		msg_to_mpu->who_wrote = (WHO_MSG_SEND|WHO_NEAT);

	}
	
	if (msg_to_mpu->owner == IPCQ_OWNER_NEAT) {
		TRACE(IPC_MSG_SEND, msg, msg->msg_type, 2);
		/* move working queue to ready queue. The number of messsages in 
		   msg_working_queue should be 0 in most of time.*/
		if (msg_working_queue.message_cnt > 0) {
			TRACE(IPC_MSG_SEND, &msg_working_queue, 
			msg_working_queue.message_cnt, 3);
			msg_que_merge(msg_to_mpu, &msg_working_queue);
		}
		/* enqueue the current message in the msg_to_mpu queue */
		msg_enq(msg_to_mpu, msg);
		/* update the current to point to first item in the queue */
		msg_to_mpu->current = msg_to_mpu->first_item;
		/* change ownership to CORE and interrupt it */
		msg_to_mpu->owner = IPCQ_OWNER_CORE;
		msg_to_mpu->who_wrote = (WHO_MSG_SEND|WHO_NEAT);
		int4_mpu();
	}
	else  {
		/* queue the message to the working queue if msg_to_mpu queue is not
		   owned by NEAT */
		TRACE(IPC_MSG_SEND, &msg_working_queue, msg, 4);
		msg_enq(&msg_working_queue, msg);
	}
	TRACE(IPC_MSG_SEND, &msg_working_queue, msg, T_END);
	reset_ipl(sr);
}
/*
 * msg_queue_check --
 * This routine will be called in the level 2 interrupt context to see if core has
 * finished the processes of all the messages in the queue. Return those messages
 * to the free list. If there are outstanding messages, move them from working queue
 * to msg_to_mpu queue and send them to core. Otherwise, change ownership of msg_to_mpu
 *  queue to NEAT.
 */
void msg_queue_check ( void )
{
	neat_ipc_message *msg;
        ushort sr =  set_ipl(PRIO_FDL);
	
	TRACE(MSG_QUEUE_CHECK, 0, 0, T_BEGIN);
	if  (msg_to_mpu->owner == IPCQ_OWNER_NONE) {
		/* check if any buffer need to return */
		TRACE(MSG_QUEUE_CHECK, 0, 0, 1);
		msg_to_mpu->current = msg_to_mpu->first_item;
		while ( (msg = msg_scan(msg_to_mpu)) != NULL) {
			TRACE(MSG_QUEUE_CHECK, msg, 0, 2);
			if ( msg->flags & IPCMSGFLAG_DATA_EXT) {
				free(msg->data);
				msg->data = NULL;
				msg->flags &= ~IPCMSGFLAG_DATA_EXT;
			}
		}
		msg_que_merge(&msg_free_list, msg_to_mpu);
		if (msg_working_queue.message_cnt > 0 ) {
			/* merge items to msg_to_mpu queue from working queue if there are
			   outstanding message ready to send in working queue */
			TRACE(MSG_QUEUE_CHECK, msg_working_queue.message_cnt, 0, 3);
			msg_que_merge(msg_to_mpu, &msg_working_queue);
			msg_to_mpu->current = msg_to_mpu->first_item;
			msg_to_mpu->owner = IPCQ_OWNER_CORE;
			msg_to_mpu->who_wrote = (WHO_CHECK|WHO_NEAT);
			int4_mpu();
	        }	
		else {
			TRACE(MSG_QUEUE_CHECK, 0, 0, 4);
			msg_to_mpu->owner = IPCQ_OWNER_NEAT;
			msg_to_mpu->who_wrote = (WHO_CHECK|WHO_NEAT);
		}
	}
	TRACE(MSG_QUEUE_CHECK, 0, 0, T_END);
        reset_ipl(sr);
}

/*
 * msg_get --
 *	return a message from msg_from_mpu queue.
 */
neat_ipc_message *msg_get (void)
{
	neat_ipc_message *msg = NULL;
	ushort sr = set_ipl(PRIO_FDL);

	TRACE(IPC_MSG_GET, 0, 0, T_BEGIN);
	if ( msg_from_mpu->owner == IPCQ_OWNER_NEAT) {
		TRACE(IPC_MSG_GET, 0, 0, 1);
		if (msg_from_mpu->message_cnt != 0) {
			TRACE(IPC_MSG_GET, msg_from_mpu->message_cnt, 0, 2);
			if ((msg = msg_scan(msg_from_mpu)) == NULL) {
				TRACE(IPC_MSG_GET, 0, 0, 3);
				/* release queue */
				msg_from_mpu->owner = IPCQ_OWNER_NONE;
				msg_from_mpu->who_wrote = (WHO_GET|WHO_NEAT);
				int4_mpu();
			}
		} 
		else {
			/* release queue */
			TRACE(IPC_MSG_GET, 0, 0, 4);
			msg_from_mpu->owner = IPCQ_OWNER_NONE;
			msg_from_mpu->who_wrote = (WHO_GET|WHO_NEAT);
			int4_mpu();
		}
	}
	TRACE(IPC_MSG_GET, msg, 0, T_END);
	reset_ipl(sr);
	return msg;
}


static void neat_ipc_null_function (void *data)
{
	/* stub routine */
}


/*
 * neat_ipc_msg_process --
 * 	The process to handle low priority ipc message.
 */
void neat_ipc_msg_process ( void )
{
        proc_level_msg_t *lmsg;

	if (ct1_enable == 0)
		return;
	TRACE(IPC_MSG_PROC, 0, 0, T_BEGIN);
	while ((lmsg = local_msg_deq(&local_msgqueue))) {
	  (neat_ipc_msg_table[lmsg->msg_type].handler)((uchar *)lmsg->data);
	  localmsg_freebuf(lmsg);
        }	
	TRACE(IPC_MSG_PROC, lmsg, lmsg->msg_type, T_END);


}

/*
 * neat_ipc_send_to_proc -
 * 	hand message to process queue.
 */
static void neat_ipc_send_to_proc (neat_ipc_message *msg)
{
	proc_level_msg_t *lmsg;
	ushort *d, *s;
	ushort size;
	ushort msg_size;

	msg_size = msg->length;

	TRACE(IPC_SEND_TO_PROC, msg, msg->msg_type, T_BEGIN);
	if (msg->flags & IPCMSGFLAG_DATA_EXT) {
		/* Unable to handle a message with external data buffer */
		buginf("\nUnable to handle a low priority message with external buffer. message type = %d", msg->msg_type);
		return;
	}
	/* copy data buffer */
	if ( msg->length > MSG_DATA_SIZE) {
		buginf("\nno mpu message with parameter length greater than 18bytes");
		return;

        }
	/* Get a local message buffer */
	if ((lmsg = localmsg_getbuf()) == NULL) {
	       TRACE(IPC_SEND_TO_PROC, msg, msg->msg_type, 1);
               /* drop the message and printout warning message */
               buginf("\nneat_ipc:Running out of local message buffers");
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

	local_msg_enq(&local_msgqueue, lmsg);
	TRACE(IPC_SEND_TO_PROC, msg, lmsg, T_END);
}
/*
 * neat_ipc_message_register
 *  register neat_ipc message callback routine.
 */
void neat_ipc_message_register (ushort msg_type, neat_ipc_msg_function_t function,  ulong priority)
{
	if (msg_type >	MSG_MPU_MAX) {
	    neat_status_report("Unable to register function with msg_type %d", 
			msg_type);
		return;
	}
	if (priority != NEAT_IPC_MSG_PRI_LO && priority != NEAT_IPC_MSG_PRI_HI) {
		neat_status_report("Illegal priority for msg_type %d", msg_type);
		return;
	}
	neat_ipc_msg_table[msg_type].priority = priority;
	neat_ipc_msg_table[msg_type].handler = function;
}


/*
 * neat_ipc_message_send -
 * 	compose a message buffer and send to NEAT processor
 */
	
neat_ipc_msg_ret neat_ipc_message_send (ushort msg_type, uchar *data, ushort size)
{
	neat_ipc_message *msg;
	ushort *dp, *sp;
	ushort cp_size;
	neat_ipc_msg_ret error = IPCMSG_ERROR_NOERROR;
	
	TRACE(IPC_MESSAGE_SEND, msg_type, size, T_BEGIN);
	if ( (msg = msg_getbuf()) != NULL) {
		TRACE(IPC_MESSAGE_SEND, msg_type, msg, 1);
		msg->flags = 0;
		msg->msg_type = msg_type;
		msg->flags = 0;
		msg->length = size;
		dp = (ushort *)data;
		cp_size = (size >> 1);
		if ( size & 1)
			cp_size++;
		if (size > MSG_DATA_SIZE) {
		    if ((sp = (ushort *)malloc_within_page(size)) != NULL) {
			TRACE(IPC_MESSAGE_SEND, msg_type, sp, 2);
		    	msg->flags |= IPCMSGFLAG_DATA_EXT;
		    	msg->data = (uchar *)sp;
		    }
		    else {
			TRACE(IPC_MESSAGE_SEND, msg_type, 0, 3);
			msg_putbuf(msg);
			return IPCMSG_ERROR_NOBUF;
		    }
		}
		else 
			sp = (ushort *)msg->para;

		for (dp = (ushort *)data; cp_size != 0; cp_size--)
			*sp++ = *dp++;
		msg_send( msg);
	}
	else {
		TRACE(IPC_MESSAGE_SEND, msg_type, 0, 4);
		error = IPCMSG_ERROR_NOBUF;		
	}
	TRACE(IPC_MESSAGE_SEND, msg_type, error, T_END);
	return error;
}

void neat_ipc_msgrcv ( void )
{
	neat_ipc_message  *msg;

	/* First, check if messages in msg_to_neat queue have been processed by NEAT */
	if (ct1_enable == 0)
		return;
	TRACE(IPC_MSGRCV,0,0,T_BEGIN);
	msg_queue_check();
	/* check if any receiving message outstanding */
	while ( (msg = msg_get()) != NULL) {
		TRACE(IPC_MSGRCV,msg, msg->msg_type,1);
		if (msg->msg_type >= MSG_MPU_MAX)
			continue;
		if (neat_ipc_msg_table[msg->msg_type].priority == NEAT_IPC_MSG_PRI_HI)
			/* message have to be handle in the interrupt level */
			(neat_ipc_msg_table[msg->msg_type].handler)((uchar *)msg->para);
		else
			/* message have to be handle in the process level */
			neat_ipc_send_to_proc(msg);
	}
	/* in case we miss the interrupt */
	msg_queue_check();
	TRACE(IPC_MSGRCV,0,0,T_END);
}
