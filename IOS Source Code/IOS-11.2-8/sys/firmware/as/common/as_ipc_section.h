/* $Id: ipc_section.h,v 1.11.6.1 1997/02/21 07:40:54 jliu Exp $
 * $Source: /cscka/t1_neat/src/common/ipc_section.h,v $
 *------------------------------------------------------------------
 * Definitions the data area for IPC process between neat and core. 
 *
 * Feb 1996, Tony Wu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_section.h,v $
 * Revision 1.11.6.1  1997/02/21 07:40:54  jliu
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *
 * Revision 1.11  1996/09/10 02:53:07  jchlin
 * Add thw following new features:
 * o       busy dsx0 back to CO trunk when there's no free modem available
 *         in the system
 * o       Support E&M feature group D, FXS loop start and ground start,
 *         and SAS loop start and ground start.
 *
 * Revision 1.10  1996/07/13  05:24:04  towu
 * o Added new message type for transmit rbs status.
 * o Added definition for trace log function.
 *
 * Revision 1.9  1996/05/12  23:56:22  towu
 * Fixed bugs for ct1 rbs features.
 *
 * Revision 1.8  1996/05/12  22:57:04  jliu
 *     o Added bus error and illegal intruction handling routine
 *
 * Revision 1.7  1996/05/09  21:25:51  towu
 * Added new feature Robbed bit signals.
 *
 * Revision 1.6  1996/04/20  02:37:38  jliu
 *     o Changed timer inetrrupt resolution from 50ms to 1 ms
 *
 * Revision 1.5  1996/04/14  20:49:50  jliu
 *     o clean up some comments
 *     o made some changes foe phone digits collection
 *
 * Revision 1.4  1996/03/19  03:54:40  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.3  1996/03/06  00:33:52  jliu
 *     o comment clean up
 *
 * Revision 1.2  1996/03/02  03:45:19  towu
 * The base code for both mailbox and message queue ipc.
 *
 * Revision 1.1  1996/03/01  02:42:11  towu
 * Create
 * Note: You must update ipc_section.h under sys/les directory whenever
 * this file changed.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef IPC_SECTIONH
#define IPC_SECTIONH

#define SRAM_PAGESIZE	    (1024*4)	/* NEAT page size is 4K */
#define SRAM_PAGEMASK	    ~(SRAM_PAGESIZE-1)

#define MAX_DIGITS      65
#define MAX_T1		2
#define MAX_DSX1	2
#define FW_VERSION_EXT  0xa5

#define NUM_STACK_FRAMES 5
#define H_BUSERR        0x80

typedef volatile uchar vuchar;
typedef volatile ushort vushort;
typedef ushort LineStatus;

typedef struct stack_trace_info {
    unsigned long *fp;  /* frame pointer */
    unsigned long ra;   /* return address */
} stack_trace_info_t;

typedef struct excpt_stack_frame {
        unsigned short sr;             /* status register */
        unsigned long pc;              /* program counter */
        unsigned int type : 4;         /* stack contents type */
        unsigned int offset : 12;      /* vector offset */
        union {
            struct {
                unsigned long instaddr;         /* instruction address */
            } type2;
            struct {
                unsigned long instaddr;         /* instruction address */
                unsigned long ireg[4];          /* internal registers */
            } type9;
            struct {
                unsigned long eaddr;            /* effective address */
                unsigned short ssw;             /* special status word */
                unsigned short wb3s;            /* writeback 3 status */
                unsigned short wb2s;            /* writeback 2 status */
                unsigned short wb1s;            /* writeback 1 status */
                unsigned long faddr;            /* data cycle fault address */
                unsigned long wb3a;             /* writeback 3 address */
                unsigned long wb3d;             /* writeback 3 data */
                unsigned long wb2a;             /* writeback 2 address */
                unsigned long wb2d;             /* writeback 2 data */
                unsigned long wb1a;             /* writeback 1 address */
                unsigned long wb1d;             /* writeback 1 data */
                unsigned long pd1;              /* push data lw1 */
                unsigned long pd2;              /* push data lw2 */
                unsigned long pd3;              /* push data lw3 */
            } type7;
            struct {
                unsigned short ireg1;           /* internal register */
                unsigned short ssw;             /* special status word */
                unsigned short ipsc;            /* instruction pipe stage c */
                unsigned short ipsb;            /* instruction pipe stage b */
                unsigned long faddr;            /* data cycle fault address */
                unsigned short ireg2;           /* internal register */
                unsigned short ireg3;           /* internal register */
                unsigned long outbuf;           /* data output buffer */
                unsigned short ireg4;           /* internal register */
                unsigned short ireg5;           /* internal register */
            } typea;
            struct {
                unsigned short ireg1;           /* internal register */
                unsigned short ssw;             /* special status word */
                unsigned short ipsc;            /* instruction pipe stage c */
                unsigned short ipsb;            /* instruction pipe stage b */
                unsigned long faddr;            /* data cycle fault address */
                unsigned short ireg2[2];        /* internal register */
                unsigned long outbuf;           /* data output buffer */
                unsigned short ireg3[4];        /* internal register */
                unsigned long sbaddr;           /* stage b address */
                unsigned short ireg4[2];        /* internal register */
                unsigned long inbuf;            /* data input buffer */
                unsigned short ireg5[22];       /* internal register */
            } typeb;
            struct {
                unsigned long faddr;            /* faulted address */
                unsigned long dbuf;
                unsigned long current_pc;       /* current instruction pc */
                unsigned short xfer_count;      /* internal transfer count */
                unsigned short ssw;             /* special status word */
            } typec;
        } stack;
} excpt_stack_frame;

typedef struct cpu_context_t {
    unsigned long d0,d1,d2,d3,d4,d5,d6,d7;
    unsigned long a0,a1,a2,a3,a4,a5,a6,a7;
    excpt_stack_frame esf;
} cpu_context_t;

/*
 * neat2mpu_mail_type
 * Communication from NEAT to MPU
 */
typedef struct neat2mpu_mail_type {
    vushort     message_type;
    LineStatus  board_status;
    ushort      other_status;
    ushort      BPVcounter;
    ushort      PCVcounter;
    ushort      CRCcounter;
    ushort      FBEcounter;
    ushort      loopback_status;
    ushort      error;
} neat2mpu_mail_type;

typedef struct carrier_mail_type {
    ushort        carrier_state;
    LineStatus    board_status;
    cpu_context_t *stack;
} carrier_mail_type;

/*
 * dtmfmail_from_mpu
 * The request digit collection with the one byte modem ID.
 */
typedef struct dtmfmail_from_mpu {
    ushort      current_mid;   /* bit 7-0 store current modem ID which is */
                               /* sending phone digits */
    ushort      new_req_mid;   /* bit 15=1 indicates new request, bit 7-0 */
                               /* store new modem ID */
} dtmfmail_from_mpu;

/*
 * dtmfmail_to_mpu
 * Neat processor send DTMF messages back to MPU. The bits definition of the
 * message type is defined in message.h file. digit array contains collected
 * phone number for call set up in system side.
 */
typedef struct dtmfmail_to_mpu {
    ushort      message;      /* four type messages defined in message.h */
    ushort      total_digits; /* total number of phone digits */
    ushort      num;          /* index to store the next coming phone digit */
    uchar       digit[MAX_DIGITS];
} dtmfmail_to_mpu;

#define MSG_DATA_SIZE   18
 /*
  * 
  * This type of message is used in queueing process. The message space is in
  * neat's SRAM space. Each message unit cannot be spaced over 4K page boundary.
  */
typedef struct _neat_ipc_message {
    struct _neat_ipc_message     *next;          /* point to next item */
    uchar                   *data;          /* parameter pointer */
    ushort                  msg_type;       /* message type */
    ushort                  flags;          /* flags to used determine
					       the message status */
    ushort                  length;         /* parameter length */
    uchar                   para[MSG_DATA_SIZE];       /* internal parameters */
} neat_ipc_message;
														

/* definition of flags */
#define IPCMSGFLAG_DATA_EXT          0x0001    /* the parameters is pointed
						by data field */

/* definition of message type */
/* message sending from neat */

enum {
 MSG_NEAT_NULL 	= 0,				/* NULL message */
 MSG_NEAT_MIB,                  		/* MIBs message */
 MSG_NEAT_BUGINFO,              		/* debug info */
 MSG_NEAT_ERROR_REPORT,         	        /* errmsgs */
 MSG_NEAT_RX_RBS_DATA,         	        	/* RBS data for rx line */
 MSG_NEAT_TX_RBS_DATA,         	        	/* RBS data for tx line */
 MSG_NEAT_RBS2CSM_HI,                           /* rbs to csm , hi priority */
 MSG_NEAT_RBS2CSM_LO,                           /* rbs to csm , hi priority */
 MSG_NEAT_MAX 		      			/* Max value for the message type */
};

 /* messages sending from core to neat */
enum {
 MSG_MPU_NULL	= 0,	    			/* NULL message */
 MSG_MPU_SET_CT1_CHANNELS,    			/* set channelized channels */
 MSG_MPU_SHOW_RBS,		        	/* show robbed bit signals */
 MSG_MPU_CSM2RBS,                               /* csm to rbs */
 MSG_MPU_MAX	            			
};


#define MAX_DSX1_CHANNELS		24
 
/* parameter structures for MSG_MPU_SET_CT1_CHANNELS */
typedef struct {
  	ushort unit;
 	uchar  sig_type[MAX_DSX1_CHANNELS/2];
} msg_ct1_channels;

typedef struct {
	ushort unit;
	ulong chan_map;
	uchar signals[MAX_DSX1_CHANNELS/2];
} msg_show_rbs_resp;

typedef struct _ipc_msg_queue {
        ushort			owner;		/* Who owns the queue */
	ushort			message_cnt;    /* number of messages in the queue */
	neat_ipc_message     	*first_item;	/* first message in the queue */
	neat_ipc_message	*last_item;	/* last message in the queue */
	neat_ipc_message	*current;	/* current processing item */
	ushort			who_wrote;	/* debug use */
	ushort			res_short;	
	ulong			reserve[3];	/* To make sure it has the 
						same size of neat_ipc_message */
} ipc_msg_queue;

#define IPCQ_OWNER_NONE		0	/* No one own the queue */
#define IPCQ_OWNER_NEAT		0x01	/* NEAT processor own the queue */
#define IPCQ_OWNER_CORE		0x02	/* Core own the queue */

#define WHO_NEAT		0x0001
#define WHO_CORE		0x0002
#define WHO_INIT		0x0100
#define WHO_CHECK		0x0200
#define WHO_MSG_SEND		0x0400
#define WHO_GET			0x0800

#define IPC_MSG_SIZE            sizeof(neat_ipc_message)
/*
 * To make the messages easy to be accessed by core, all the messages and messages
 * headers have to be in the same NEAT's page.
 */
    
#define TOTAL_MSG_BUFFERS \
	    ((SRAM_PAGESIZE - sizeof(ipc_msg_queue))/IPC_MSG_SIZE)

typedef struct _ipc_section {
	uchar		   dial_tone[SRAM_PAGESIZE]; /* dail tone data */ 
        uchar		   busy_tone[SRAM_PAGESIZE]; /* busy tone data */
	ipc_msg_queue	   msg_to_mpu;   /* The next two fields has to be in the */
                                         /* same page */
	neat_ipc_message   msgbuf_tompu[TOTAL_MSG_BUFFERS];
	ipc_msg_queue	   msg_from_mpu; /* The next two fields has to be in the */
                                         /* same page */
	neat_ipc_message   msgbuf_frommpu[TOTAL_MSG_BUFFERS];
	carrier_mail_type  carrier_to_mpu[MAX_DSX1];  /* mailbox to handle level 6 */
                                                      /* message */
	vushort  	   mail_from_mpu[MAX_DSX1];   /* mailbox for setting framer */
				                      /* configurations */
	ushort		   firmware_ver_num;
	ushort		   mpu_status;
	neat2mpu_mail_type mail_to_mpu[MAX_DSX1];   /* mailbox for level 4 commands */
	dtmfmail_to_mpu	   dtmf_to_mpu[MAX_DSX1];   /* dtmf to send message to mpu */
	dtmfmail_from_mpu  dtmf_from_mpu[MAX_DSX1]; /* dtmf to recv message from mpu */
} ipc_section;



/* definitions for backward compatible */
#define DIALTONE_ADDR			(ipc->dial_tone)
#define BUSYTONE_ADDR			(ipc->busy_tone)
#define IPC_MSG_BUFF_ADDR		(ipc->msg_buffer)
#define CARRIER_TO_MPU(subunit)        	(&ipc->carrier_to_mpu[subunit])
#define MAIL_FROM_MPU(subunit)      	(&ipc->mail_from_mpu[subunit])
#define MSG_FROM_MPU			(&ipc->msg_from_mpu) 
#define MSG_TO_MPU			(&ipc->msg_to_mpu) 
#define FIRMWARE_VER_ADDR		(&ipc->firmware_ver_num) 
#define MPU_RELOAD_DONE_ADDR		(&ipc->mpu_status) 

#define FW_VERSION  			(ipc->firmware_ver_num)
#define MPU_RELOAD_DONE 		(ipc->mpu_status)

#define MAIL_TO_MPU(subunit)		(&ipc->mail_to_mpu[subunit])
#define DTMF_TO_MPU(subunit)   		(&ipc->dtmf_to_mpu[subunit])
#define DTMF_FROM_MPU(subunit) 		(&ipc->dtmf_from_mpu[subunit])


/*
 * The IPC area is the location that core and neat both will access.
 */
#define IPC_AREA             LOCAL_STACK_POINTER  /* The ipc section has to start
						     on NEAT's page boundary */
#define IPC_MSG_AREA         ((uint)(&ipc->msg_from_mpu)) /* the starting address of
              							messsage data area */

#define DYNAMIC_MEM_AREA     (IPC_AREA + sizeof (ipc_section))

/* message priority */
typedef enum  {
	NEAT_IPC_MSG_PRI_LO = 0,
	NEAT_IPC_MSG_PRI_HI
} neat_ipc_msg_priority; 

/* error code */
typedef enum {
	IPCMSG_ERROR_NOERROR = 0,
	IPCMSG_ERROR_ILLEGAL_SIZE,
	IPCMSG_ERROR_NOBUF
} neat_ipc_msg_ret;

/* message receiving handler */ 
typedef void (*neat_ipc_msg_function_t)(void *);

typedef struct csm_message_t_ {
    uchar           src_slot;
    uchar           src_controller;
    uchar           src_entity;   /* port when sent from host or
                                     dsx0 channel number when sent from neat */
    uchar           dest_slot;
    uchar           dest_controller;
    uchar           dest_entity;  /* port when sent from host or
                                     dsx0 channel number when sent from neat */
    ushort          event;        /* event enumeration */
    ushort          cause;        /* cause for the event */
    union {
        ushort          call_id;      /* outgoing call starts from 0xa000,
                                         incoming call starts from 0 */
        ushort          busyout_channels;
        ushort          value;
    } data;
} csm_message_t;


/* The following definitions are platform dependent */


/* trace log definitions */
#undef IPC_TRACE
#ifdef IPC_TRACE
extern void trace_log(ulong a, ulong b, ulong c, ulong d);
#define TRACE(a,b,c,d)	trace_log((ulong)(a),(ulong)(b),(ulong)(c),(ulong)(d))
#else
#define TRACE(a,b,c,d)
#endif /* IPC_DEBUG */

#define MAX_IPC_TRACE_SIZE	512	/* 8k trace buffer size */

#define T_BEGIN 		0
#define T_END			0xff

#define IPC_MSGRCV              1         
#define MSG_QUEUE_CHECK         2
#define IPC_SEND_TO_PROC        3
#define IPC_MSG_PROC            4
#define IPC_MSG_GET             5
#define IPC_QUE_MRG             6
#define IPC_MSG_DEQ             7
#define IPC_MSG_ENQ             8
#define LCL_MSG_DEQ             9
#define LCL_MSG_ENQ             0x0a
#define IPC_MSG_SEND            0x0b
#define IPC_MESSAGE_SEND        0x0c

#endif  /* IPC_SECTIONH */
