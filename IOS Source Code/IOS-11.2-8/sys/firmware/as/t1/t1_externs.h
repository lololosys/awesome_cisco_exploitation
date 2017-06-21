/* $Id: externs.h,v 1.6 1996/05/09 21:25:48 towu Exp $
 * $Source: /cscka/t1_neat/src/t1/externs.h,v $
 *------------------------------------------------------------------
 * externs.h  NEAT global variable declarations
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: externs.h,v $
 * Revision 1.6  1996/05/09 21:25:48  towu
 * Added new feature Robbed bit signals.
 *
 * Revision 1.5  1996/05/06  06:01:05  jliu
 *     o Cleaned some code for LED on and off
 *     o Set DSX1_XMT_AIS in dsx1LineStatus after sending AIS
 *
 * Revision 1.4  1996/04/14  20:49:48  jliu
 *     o clean up some comments
 *     o made some changes foe phone digits collection
 *
 * Revision 1.3  1996/04/08  18:14:58  jliu
 *     o Made some configuration changes to get tone data generation
 *       works
 *     o Implemented transmit underrun error interrupt
 *
 * Revision 1.2  1996/03/02  03:51:17  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	HAVE_EXTERNS_H
#define	HAVE_EXTERNS_H

#ifdef	DECLARE_GLOBS
#	define	EXTERN
#	undef	DECLARE_GLOBS
#else
#	define	EXTERN			extern
#endif

/*
 *	External variable declarations
 */
EXTERN	T1InfoType      Interface[2];
EXTERN  DTMFInfoType    DTMF[2];
EXTERN	dsx1MIBDataType	MIBData[2];

EXTERN volatile long    GotBusError;
EXTERN volatile long    PossibleBusError;
EXTERN ulong            OneSecTimer[2];
EXTERN uchar            BusyToneTimer; 
EXTERN uchar            MPU_reload;
EXTERN uchar            BUSYTONE_ON;
EXTERN uchar            Have1Sec;
EXTERN ipc_section	*ipc;
EXTERN uchar		ct1_enable;
EXTERN bool		rbs_changed;
EXTERN int		rbs_report_timer;
EXTERN bool		rbs_report;
/*
 * 	External functions 
 */

/* Functions defined in utils.c */
extern int sprintf (char *cp, char *fmt, ...);
extern size_t strlen (const char *s);
extern void buginf(char *fmt, ...);
extern void neat_status_report(char *fmt, ...);

/* Functions defined in memory.c */
extern void mem_init(void);
extern void free(char *);
extern char *malloc(int size);
extern char *malloc_within_page(int size);

/* Functions defined in neat_ipc.c */
extern void neat_ipc_msg_process(void);
extern void neat_ipc_msgrcv( void );
extern void neat_ipc_msg_mgmt_init( void );
extern neat_ipc_msg_ret neat_ipc_message_send(ushort msg_type, uchar *data, ushort size);
extern void neat_ipc_message_register(ushort msg_type, neat_ipc_msg_function_t function,  ulong priority);
extern void ct1_enable_rbs_handler ( void *c);
extern void t1_rbs_report ( void );
extern void t1_rbs_int (T1InfoType *t1);
extern void t1_rbs_transmit (ushort unit, uchar channel, uchar signals);
extern void t1_rbs_init(void);
extern void t1_rbs_reset(void);


#undef	EXTERN
#endif
