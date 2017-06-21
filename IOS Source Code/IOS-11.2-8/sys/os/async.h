/* $Id: async.h,v 3.3.62.2 1996/06/12 14:29:44 bjthomas Exp $
 * $Source: /release/112/cvs/Xsys/os/async.h,v $
 *------------------------------------------------------------------
 * async.h -- common asynchronous serial line support.
 *  
 * June 1991, David Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: async.h,v $
 * Revision 3.3.62.2  1996/06/12  14:29:44  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/03/18  21:28:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.2  1996/03/09  05:08:43  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.24.1  1996/02/20  16:36:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:44:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:16:50  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/24  00:54:42  dkatz
 * CSCdi40898:  Synchronized reload needed
 *
 * Revision 2.1  1995/06/07  21:53:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

#define TTYDAEMON_OUTLIMIT(tty) (tty->outsize)
#define TTYDAEMON_INLIMIT(tty) 400
#define CHECK_TTY_CYCLE 5      /* Period to scan ttys for absolute timeout */
#define AUTOSELECT_CHARACTERS  70

/*
 * async.c
 */

extern void async_daemon_kill(tt_soc *);
extern void async_daemon_attach(tt_soc *);
extern boolean finishoutBLOCK(tt_soc *);
extern boolean readyBLOCK(tt_soc *);
extern boolean serial_outputblock(tt_soc *);
extern boolean sputBLOCK(tt_soc *);
extern boolean ttyBLOCK(tt_soc *);
extern forktype tty_daemon(tt_soc *);
extern int generic_putpkt(tt_soc *, paktype *, uchar *, int);
extern int quick_getc(tt_soc *);
extern int serial_outputpending(tt_soc *);
extern tt_soc *rotary_hunt(tt_soc *);
extern void death(tt_soc *);
extern void kfork(tt_soc *);
extern void linewatch(void);
extern boolean quick_putc(tt_soc *, int);
extern void quick_puts(tt_soc *, char *);
extern void serial_clearinput(tt_soc *);
extern void serial_clearoutput(tt_soc *);
extern void serial_putc(tt_soc *, int);
extern void serial_setbuffers(tt_soc *, int, int );
extern void start_process(tt_soc *);
extern void tty_dallytimer(tt_soc *);
extern void tty_reset(tt_soc *);
extern void tty_sendmessage(tt_soc *, paktype *);
extern void ttyline_sendmessage(int, paktype *);
extern void tty_send_logmessage(tt_soc *, paktype *, int, int);
extern void tty_log_lostcount(tt_soc *, int);
extern void setmask_cmd(parseinfo *, uchar *);
extern void setdef_mask(parseinfo *, uchar *);
extern void tty_wait_output (tt_soc *, int);
extern void statem_rxservice(tt_soc *, uchar *, int);
extern void async_setup_global(setup_struct *);
extern boolean start_exec_proc(tt_soc * tty);
extern process tty_background(void);
extern void tty_core_init(void);
extern void start_sessiontimeout (conntype *conn, tt_soc *tty);
