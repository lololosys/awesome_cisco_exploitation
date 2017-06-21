/* $Id: mop.h,v 3.2 1995/11/17 08:59:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/decnet/mop.h,v $
 *------------------------------------------------------------------
 * mop.h - Maintenance Operation Protocol definitions
 *
 * 30-May-1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mop.h,v $
 * Revision 3.2  1995/11/17  08:59:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/07  22:46:46  hampton
 * Convert DECNET to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi36830]
 *
 * Revision 2.1  1995/06/07 20:23:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MOP_H__
#define __MOP_H__

/*
 * Define version number
 */

#define MOP_VERSION 3
#define MOP_ECO 0
#define MOP_USER_ECO 0

/*
 * Define MOP message types
 */

#define MOP_LOAD_XFER 0			/* Memory load with transfer addr */
#define MOP_LOAD 2			/* Memory load */
#define MOP_REQUEST_DUMP 4		/* Request memory dump */
#define MOP_REQUEST_PROGRAM 8		/* Request program */
#define MOP_REQUEST_MEMORY_LOAD 10	/* Request memory load */
#define MOP_REQUEST_DUMP_SERVICE 12	/* Request dump service */
#define MOP_MEMORY_DUMP 14		/* Memory dump data */
#define MOP_PARAMETER_LOAD_XFER 20	/* Parameter load with transfer addr */
#define MOP_DUMP_COMPLETE 1		/* Dump complete */
#define MOP_ASSISTANCE_VOLUNTEER 3	/* Assistance volunteer */
#define MOP_LOOP_DATA 24		/* Non-broadcast loop test */
#define MOP_LOOPED_DATA 26		/* Reply to MOP_LOOP_DATA */
#define MOP_BOOT 6			/* Boot remote system */
#define MOP_REQUEST_ID 5		/* Request sysid message */
#define MOP_SYSTEM_ID 7			/* Sysid message */
#define MOP_REQUEST_COUNTERS 9		/* Request counters */
#define MOP_COUNTERS 11			/* Counters report */
#define MOP_RESERVE_CONSOLE 13		/* Reserve virtual console */
#define MOP_RELEASE_CONSOLE 15		/* Release console */
#define MOP_CONSOLE_COMMAND 17		/* Console command and poll */
#define MOP_CONSOLE_RESPONSE 19		/* Console response and acknowledge */

/*
 * Define parameter codes for SYSID messages
 */

#define MOP_SYSID_VERSION 1		/* MOP version */
#define MOP_SYSID_FUNCTIONS 2		/* Functions supported */
#define MOP_SYSID_CONSOLE_USER 3	/* Current user of remote console */
#define MOP_SYSID_RESERVATION_TIMER 4	/* Console Reservation timer */
#define MOP_SYSID_COMMAND_SIZE 5	/* Remote console buffer size */
#define MOP_SYSID_RESPONSE_SIZE 6	/* Remote console output buffer size */
#define MOP_SYSID_MAC_ADDRESS 7		/* MAC address for this interface */
#define MOP_SYSID_DEVICE 100		/* Communications device */
#define MOP_SYSID_DATALINK 400		/* Datalink type */
#define MOP_SYSID_MAXMTU 401		/* Datalink buffersize */
/*
 * Define sysid function mask
 */

#define MOP_FUNCTION_LOOP 1		/* Loop function supported */
#define MOP_FUNCTION_DUMP 2		/* Dump function supported */
#define MOP_FUNCTION_PRIMARY 4		/* This is a primary loader */
#define MOP_FUNCTION_MULTIBLOCK 8	/* This is a multiblock loader */
#define MOP_FUNCTION_BOOT 16		/* Boot message supported */
#define MOP_FUNCTION_CONSOLE 32		/* Remote console supported */
#define MOP_FUNCTION_COUNTERS 64	/* Counters read message supported */
#define MOP_FUNCTION_CONSOLE_RESERVED 128 /* Console currently reserved */ 

/*
 * Device codes
 */

#define MOP_DEVICE_DS200 33		/* DECserver 200 device code */
#define MOP_DEVICE_DS500 35		/* DECserver 500 device code */
#define MOP_DEVICE_MUXSERVER 41		/* MUXserver */
#define MOP_DEVICE_CISCO 121		/* cisco device code */

/*
 * Definitions for program request function
 */

#define MOP_PROGRAM_VERSION 1		/* Version number */

#define MOP_PROGRAM_TYPE_SECONDARY 0	/* Secondary loader */
#define MOP_PROGRAM_TYPE_TERTIARY 1	/* Tertiary loader */
#define MOP_PROGRAM_TYPE_SYSTEM 2	/* System image */

#define MOP_PROGRAM_MAX_FILENAME 16	/* Maximum filename length */

#define MOP_PROGRAM_PROC_SYSTEM 0	/* System processor */
#define MOP_PROGRAM_PROC_COMM 1		/* Communications processor */

#define MOP_MIN_RETRIES 3		/* Minimum number of retries */
#define MOP_DEFAULT_RETRIES 8		/* Number of default retries */
#define MOP_MAX_RETRIES 24		/* Maximum number of retries */

#define MOP_MIN_RETRANSMIT 1		/* Minimum retransmit interval */
#define MOP_DEFAULT_RETRANSMIT 4	/* Default retransmit interval */
#define MOP_MAX_RETRANSMIT 20		/* Maximum retransmit interval */

/*
 * Console carrier parameters
 */

#define MOP_CONSOLE_TIMEOUT 60		/* Reservation timer */

#define MOP_CONSOLE_SEQUENCE 1		/* Sequence number bit */
#define MOP_CONSOLE_BREAK 2		/* Simulate break condition */

#define MOP_RESPONSE_SEQUENCE 1		/* Response sequence number bit */
#define MOP_RESPONSE_CMD_LOST 2		/* Command data lost bit */
#define MOP_RESPONSE_RESP_LOST 4	/* Response data lost bit */

#define MOP_CONSOLE_INBUF (MAXETHERSIZE-2-2) /* Most data we'll take at once */
#define MOP_CONSOLE_OUTBUF 256		/* Most data we'll send at once */
#define MOP_INBUF MOP_CONSOLE_INBUF    	/* Size of internal data buffer */

/*
 * Define datalink types (used in MOP_SYSID_DATALINK messages)
 */

#define MOP_DATALINK_ETHERNET 1		/* Ethernet datalink */
#define MOP_DATALINK_DDCMP 2		/* DDCMP datalink */
#define MOP_DATALINK_LAPB 3		/* LAPB/X.25 */

/*
 * Define MOP packet data structure
 */

typedef struct mophdrtype_ {
    ushort datalen;		/* Length of real data (little-endian) */
    uchar code;			/* MOP function code */
    uchar parameters[1497];	/* Message parameters */
} mophdrtype;

#define mopheadstart(ptr) ((ptr)->network_start)

#define mop_flags unspecified[0] /* MOP message flags */
#define mop_offset unspecified[1] /* Current offset from parameter start */

#define MOP_WAIT_ACK 0x1	/* Message is waiting for an ACK */

/*
 * Console carrier data structure
 */

struct mopcon_ {
    tt_soc *tty;		/* Pointer to VTY being used for console */
    sys_timestamp reservation;	/* Time at which reservation expires */
    paktype *xmit[2];		/* Transmit buffers */
    uchar *inhead;		/* Input buffer head pointer */
    uchar *intail;		/* Input buffer tail pointer */
    int incount;		/* Count of pending input */
    uchar sequence;		/* Sequence bit for next message */
    uchar user[6];		/* MAC address of console user */
    uchar inbuf[MOP_INBUF];	/* Input data buffer */
};


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * mop.c
 */
extern boolean mop_loadBLOCK(int);
extern boolean mop_loadwaitBLOCK(int);
extern process mop_protocols(void);
extern paktype *mop_assistance_volunteer(hwidbtype *, paktype *, int);
extern paktype *mop_load(hwidbtype *, paktype *, int);
extern paktype *mop_request_id(hwidbtype *, paktype *, int);
extern uchar *mop_system_info(hwidbtype *, uchar *);
extern void mop_enqueue(paktype *);
extern void mop_init_idb(hwidbtype *);
extern void mop_init(subsystype *);
extern void mop_send_request_program(hwidbtype *, char *, int , uchar *);
extern void send_periodic_sysid(void);
extern void send_sysid(hwidbtype *, uchar *, ushort);
extern int mop_validations(int, hwidbtype *, boolean);
extern boolean mop_program_req(char *, uchar *, hwidbtype *, boolean);
extern int mop_process_pkt(paktype *, uchar *, ulong, boolean);
extern void mop_load_reply(paktype *, hwidbtype *, uchar *);
extern int mop_load_reply_retry(hwidbtype *, uchar *, boolean);

/*
 * mopvty.c
 */
#include "connect.h"

extern boolean mop_alive(conntype *);
extern boolean mop_outputblock(tt_soc *);
extern boolean mop_startoutput(tt_soc *);
extern int mop_getc(tt_soc *);
extern int mop_inputpending(tt_soc *);
extern int mop_outputpending(tt_soc *);
extern mopcon *tty2con(tt_soc *);
extern paktype *mop_console_command(hwidbtype *, paktype *, int);
extern paktype *mop_release_console(hwidbtype *, paktype *, int);
extern paktype *mop_reserve_console(hwidbtype *, paktype *, int);
extern void mop_cleanup(hwidbtype *);
extern void mop_clearinput(tt_soc *);
extern void mop_close(conntype *);
extern void mop_putc(tt_soc *, int);
extern void mop_vty_init(void);
extern int mop_unread(conntype *);

/*
 * mop_chain.c
 */
extern void mop_parser_init(void);

/*
 * mop_setup.c
 */
extern void mop_setup_int(setup_struct *);

/*
 * mop_media.c
 */
extern void mop_media_registry(void);


#endif __MOP_H__
