/* $Id: if_as5200_dsx1_ipc.h,v 3.1.14.4 1996/09/09 20:56:43 jchlin Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_dsx1_ipc.h,v $
 *------------------------------------------------------------------
 * if_as5200_dsx1_ipc.h - definition of message ipc for neat processor
 *
 * May 1996, Tony Wu
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_dsx1_ipc.h,v $
 * Revision 3.1.14.4  1996/09/09  20:56:43  jchlin
 * CSCdi66525:  ct1/rbs cannot busy back the CO trunk when theres no free
 * modem
 * Branch: California_branch
 * o       The bugfix will busy dsx0s back to CO thru RBS when there's
 *         no free modem to accept incoming calls.
 * o       The modem pool is changed to have multiple pools for different
 *         types of modems.
 * o       Add hidden debug commands.
 *
 * Revision 3.1.14.3  1996/07/15  20:49:10  towu
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
 * Revision 3.1.14.2  1996/07/11  22:36:58  jliu
 * CSCdi62357:  need to support AT&T and ANSI facility data link standard
 * Branch: California_branch
 *           o Added new fdl command for t1 controller in order to support
 *             CSU function
 *
 * Revision 3.1.14.1  1996/06/16  21:14:40  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/05/19  20:07:13  vnguyen
 * Placeholder file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Note: You must update ipc_section.h under t1_neat/ipc_section.h
 * for neat's firmware code whenever this file changed.
 */

#ifndef HAVE_IF_AS5200_DSX1_IPCH
#define HAVE_IF_AS5200_DSX1_IPCH

#define SRAM_PAGESIZE	    (1024*4)	/* NEAT page size is 4K */
#define SRAM_PAGEMASK	    ~(SRAM_PAGESIZE-1)

#define MAX_DIGITS      65
#define MAX_T1		2
#define MAX_DSX1	2
#define FW_VERSION_EXT  0xa5

typedef volatile uchar vuchar;
typedef volatile ushort vushort;
typedef ushort LineStatus;

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
    ushort      carrier_state;
    LineStatus  board_status;
    uint        stack;
} carrier_mail_type;

/*
 * dtmfmail_from_mpu
 * The request digit collection with the one byte modem ID. The addr to
 * store those modem ID and new command bits for DTMF0 is at 0x1ba00 and
 * for DTMF1 is at 0x1ba80.
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
 * phone number for call set up in system side. For DTMF0 is at 0x1b900 and
 * for DTMF1 is at 0x1b980.
 */
typedef struct dtmfmail_to_mpu {
    ushort      message;      /* four type messages defined in message.h */
    ushort      total_digits; /* total number of phone digits */
    ushort      num;          /* index to store the next coming phoe digit */
    uchar       digit[MAX_DIGITS];
} dtmfmail_to_mpu;

#define MSG_DATA_SIZE	18
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
#define IPCMSGFLAG_DATA_EXT          0x0001    /* the parameters pointed by
						data field.  */

/* definition of message type */
/* message sending from neat */
enum {
	MSG_NEAT_NULL = 0,
	MSG_NEAT_MIB,    			/* MIBs message */
	MSG_NEAT_BUGINFO,     			/* debug info */
	MSG_NEAT_ERROR_REPORT,    		/* errmsgs */
 	MSG_NEAT_RX_RBS_DATA,         	       	/* RBS data for rx line */
 	MSG_NEAT_TX_RBS_DATA,         	       	/* RBS data for tx line */
        MSG_NEAT_RBS2CSM_HI,                    /* rbs to csm , hi priority */
        MSG_NEAT_RBS2CSM_LO,                    /* rbs to csm , hi priority */
	MSG_NEAT_MAX				/* Max value for the message type */
};

 /* messages sending from core to neat */
enum {
	MSG_MPU_NULL = 0,		    	/* NULL message */
	MSG_MPU_SET_CT1_CHANNELS,    		/* set channelized channels */
        MSG_MPU_SHOW_RBS,                       /* show robbed bit signals */
        MSG_MPU_CSM2RBS,                        /* csm to rbs */
	MSG_MPU_MAX	            
};

#define MAX_DSX1_CHANNELS 24

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
	ushort			resv;		/* debug use */
	ulong			reserve[3];	/* To make sure it has the 
						same size of neat_ipc_message */
} ipc_msg_queue;

#define IPCQ_OWNER_NONE		0	/* No one own the queue */
#define IPCQ_OWNER_NEAT		0x01	/* NEAT processor own the queue */
#define IPCQ_OWNER_CORE		0x02	/* Core own the queue */

#define WHO_NEAT                0x0001
#define WHO_CORE                0x0002
#define WHO_INIT                0x0100
#define WHO_CHECK               0x0200
#define WHO_MSG_SEND            0x0400
#define WHO_GET                 0x0800

#define IPC_MSG_SIZE		sizeof(neat_ipc_message)
/*
 * To make the messages easy to be accessed by core, all the messages and messages
 * headers have to be in the same NEAT's page.
 */

#define TOTAL_MSG_BUFFERS \
	    ((SRAM_PAGESIZE - sizeof(ipc_msg_queue))/IPC_MSG_SIZE)

typedef struct _ipc_section {
	uchar		   dial_tone[SRAM_PAGESIZE]; /* dail tone data */ 
        uchar		   busy_tone[SRAM_PAGESIZE]; /* busy tone data */
/* The next two fields has to be in the same page */
	ipc_msg_queue	   msg_to_mpu;
	neat_ipc_message   msgbuf_tompu[TOTAL_MSG_BUFFERS];
/* The next two fields has to be in the same page */
	ipc_msg_queue	   msg_from_mpu;
	neat_ipc_message   msgbuf_frommpu[TOTAL_MSG_BUFFERS];
	carrier_mail_type  carrier_to_mpu[MAX_DSX1];  /* mailbox to handle level 6 message */
	vushort  	   mail_from_mpu[MAX_DSX1];   /* mailbox for setting framer
						configurations */
	ushort		   firmware_ver_num;
	ushort		   mpu_status;
#ifndef IPC_MESSAGE_ONLY
	neat2mpu_mail_type mail_to_mpu[MAX_DSX1];   /* mailbox for level 4 commands */
	dtmfmail_to_mpu	   dtmf_to_mpu[MAX_DSX1];	   /* dtmf to send message to mpu */
	dtmfmail_from_mpu  dtmf_from_mpu[MAX_DSX1];  /* dtmf to recv message from mpu */
#endif /* IPC_MESSAGE_ONLY */
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

#ifndef IPC_MESSAGE_ONLY
#define MAIL_TO_MPU(subunit)		(&ipc->mail_to_mpu[subunit])
#define DTMF_TO_MPU(subunit)   		(&ipc->dtmf_to_mpu[subunit])
#define DTMF_FROM_MPU(subunit) 		(&ipc->dtmf_from_mpu[subunit])
#endif /* IPC_MESSAGE_ONLY */


/*
 * The IPC area is the location that core and neat both will access.
 */
#define LOCAL_STACK_POINTER	0x17000
#define IPC_AREA	     LOCAL_STACK_POINTER  /* The ipc section has to start
						     on NEAT's page boundary */
#define IPC_MSG_AREA	     ((uint)(&ipc->msg_from_mpu)) /* the starting address of
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

/****************  platform specific definition ***************************/
/*
 * The following definitions is for core
 */


typedef struct neat_ipc_info_ {
	ipc_msg_queue	   *msg_from_neat;
	ipc_msg_queue	   *msg_to_neat;
	ipc_msg_queue	   *work;
	ipc_msg_queue	   *free;
	boolean	   	   locked;
	uchar		   lock_page;	/* locked neat ram page */
	uchar		   init_status; /* ipc init status */
} neat_ipc_info;

/* init_status field */

#define IPC_RESET	0		/* initial value */
#define	IPC_INITED	1		/* ipc is ready */


/* Neat processor page number definition */
#define NEAT_PAGE_BITS_SHIFT  12      /* the bits 13, 12 are the page
                                           number for neat's sram memory map */
 
/* Neat processor's ram address contains mail from T1 controller */
#define DSX1_MAIL_FROM_C(unit) 	((uint)MAIL_TO_MPU(unit)) 
/*
 * bits definition of message type from NEAT processor
 * Only DSX1_MAIL_FROM_C(0) location has meaning in MSG_DTMF0 & MSG_DTMF1 bits
 */
#define MSG_TYPE_NONE    0x00
#define MSG_TYPE_FULL    0x01
#define MSG_LOOPBACK     0x02
#define MSG_ERROR        0x04
#define MSG_CARRIER      0x10
#define MSG_DTMF0        0x100
#define MSG_DTMF1        0x200

#define BOARD_STATUS_OFFSET   0x2  /* NEAT processor's ram address offset */
                                   /* which contains DSX1 line status */
#define OTHER_STATUS_OFFSET   0x4  /* NEAT processor's ram address offset */
                                   /* which contains some MIB errors info */
#define BPV_COUNT_OFFSET      0x6  /* NEAT processor's ram address offset */
                                   /* which contains BPV error count info */
#define PCV_COUNT_OFFSET      0x8  /* NEAT processor's ram address offset */
                                   /* which contains PCV error count info */
#define CRC_COUNT_OFFSET      0xa  /* NEAT processor's ram address offset */
                                   /* which contains CRC error count info */
#define FBE_COUNT_OFFSET      0xc  /* NEAT processor's ram address offset */
                                   /* which contains FBE error count info */
#define LOOPBACK_OFFSET       0xe  /* NEAT processor's ram address offset */
                                   /* which contains rem_lpbk test result */
#define ERRORS_OFFSET         0x10 /* NEAT processor's ram address offset */
                                   /* which contains errors events occurred */
/*
 * These definitions are used for remote loopback test result
 */
#define REM_LPBK_PASS        0x01
#define REM_LPBK_FAIL        0x02
#define LINE_LP_NET          0x04
#define NO_LINE_LP_NET       0x08
#define PAYLD_LP_NET         0x10
#define NO_PAYLD_LP_NET      0x20

/* Neat processor's ram address contains carrier available byte from DSX1 */
#define DSX1_CARRIER_AVAIL(unit)  ((uint)CARRIER_TO_MPU(unit))
/*
 * These definitions reflect the latest DSX1 carrier status
 */
#define CARRIER_UP     0x01
#define CARRIER_DOWN   0x02
#define CARRIER_MASK   0x03
#define BUS_ERROR      0x80000000
#define ILLEGAL        0x40000000
#define NEAT_ERR_MASK  0xc0000000

/* Neat processor's ram address contains mail for DSX1 controller */
#define DSX1_MAIL_TO_C(unit) 	((uint)MAIL_FROM_MPU(unit))

/* bit definition for mail sent to Neat processor from MPU */
#define MPU_MSG_DISABLED        0x01  /*enable = 0, disable = 1*/
#define MPU_MSG_FRAME           0x02  /*SF = 0, ESF =1*/
#define MPU_MSG_LINECODE        0x04  /*AMI = 0, B8ZS = 1*/
#define MPU_MSG_LPBK_LOC        0x08  /*disable = 0, enable = 1*/
#define MPU_MSG_LPBK_REM        0x10  /*disable = 0, enable = 1*/
#define MPU_MSG_CLK_SOURCE      0x20  /*external = 0, internal = 1*/
#define MPU_MSG_FDL_ATT         0x40  /*ATT = 01*/
#define MPU_MSG_FDL_ANSI        0x80  /*ANSI = 10*/
#define MPU_MSG_FDL_NONE        0x00  /*NONE = 00*/
#define MPU_MSG_FDL             0xc0

#define DSX1_FW_VERSION  (uint)FIRMWARE_VER_ADDR   /* Neat processor firmware version */
#define DSX1_RELOAD_DONE (uint)MPU_RELOAD_DONE_ADDR /* Core CPU finishes the reload */

/* Neat processor's ram address contains DTMF messages to MPU */
#define DSX1_FROM_DTMF(unit)  ((uint)DTMF_TO_MPU(unit))
/*
 * Bit definition for event type which is stored in DSX1_FROM_DTMF0 or
 * DSX1_FROM_DTMF1 location 
 */
#define MSG_RDY_DIGCOL   0x01
#define MSG_1STDIG_TO    0x02
#define MSG_1STDIG_RCV   0x04
#define MSG_DIG_DONE     0x08

/* Neat processor's ram address contains DTMF messages from MPU */
#define DSX1_TO_DTMF(unit)   ((uint)DTMF_FROM_MPU(unit))

#define RESET_REQ         1
#define READY_COL         2
#define FIRST_DIGIT_TO    3
#define FIRST_DIGIT_RCV   4
#define ALL_DIGIT_RCV     5

/* message receiving handler */ 
typedef void (*neat_ipc_msg_function_t)(uchar, void *);

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

/*
 * Definitions for message data read/write
 */


#define NEAT_LONGWORD_WR(addr, data) \
	{ \
        *((ushort *)addr) = *(ushort *)(&data); \
	*((ushort *)addr + 1) = *((ushort *)(&data) + 1); \
	}

#define NEAT_LONGWORD_RD(addr) \
	(uint)(((*((ushort *)addr)) << 16) | (*((ushort *)((uint)(addr) + 2))))

enum {
	MSG_POOL_RCV = 1,	/* message buffer pool for neat to mpu use */
	MSG_POOL_XMT		/* message buffer pool for mpu_to neat use */
	};

extern void neat_msgrcv_process ( uchar slot);
extern neat_ipc_msg_ret  neat_ipc_message_send (uchar slot, ushort msg_type, uchar *data, ushort size);
extern void neat_ipc_message_register (ushort msg_type, neat_ipc_msg_function_t function,  ulong priority);
extern void neat_ipc_init (uchar slot);
extern process neat_ipc_reset (void);
extern ipc_section *ipc;
extern neat_ipc_info neat_ipc_slots[];
extern ushort dsx1_set_neat_page_reg (uchar slot, ushort new_page_reg);

#endif /* HAVE_IF_AS5200_DSX_IPCH */
