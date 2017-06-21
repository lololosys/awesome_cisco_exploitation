/* $Id: if_slip.h,v 3.5.44.1 1996/04/29 09:42:22 fox Exp $
 * $Source: /release/112/cvs/Xsys/ts/if_slip.h,v $
 *------------------------------------------------------------------
 * if_slip.h - Berkeley compatible Serial Line Interface "Prococol"
 *
 * September, 1987   Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_slip.h,v $
 * Revision 3.5.44.1  1996/04/29  09:42:22  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.5  1996/01/04  22:15:14  lol
 * CSCdi43469:  per-user route wont use default interface
 *
 * Revision 3.4  1995/11/18  17:43:21  fox
 * CSCdi43045:  ppp and slip exec-level commands are broken
 * When checking for a duplicate peer address, don't check our own
 * interface.
 *
 * Revision 3.3  1995/11/17  18:55:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:45:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:39:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:33:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/18  19:38:58  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/06/07  23:09:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * versions of SLIP (and generic ASYNC line protocols)
 */
#define SLIP_SLIP 0
#define SLIP_PPP  1


#define MAXSLIPSIZE  MAXETHERSIZE /* largest SLIP datagram we'll send */
#define SLIP_QUEUESIZE 3	/* max packets to be held for sending */

#define SLIP_FRAME_END	0300	/* End of Frame */
#define SLIP_FRAME_ESC	0333	/* Frame escape for byte stuffing */
#define SLIP_TRANS_END	0334	/* Transmitted form of Frame End */
#define SLIP_TRANS_ESC	0335	/* Transmitted form of Frame escape */

/*
 * Slip Transmitter states
 */
#define SLIP_TXIDLE 0
#define SLIP_XMIT 1
#define SLIP_TXESCAPE 2
#define SLIP_NEEDSEND 3

/*
 * SLIP receiver states
 */
#define SLIP_RXIDLE 10
#define SLIP_RECEIVE 11
#define SLIP_RXESCAPE 12
#define SLIP_RXERROR 13

/*
 * SLIP flag bits
 */
#define SLIP_DYNAMIC	  1	/* address assignment by user */
#define SLIP_HEX 	  2	/* send to TACACSD in hex format */
#define SLIP_NAME	  4	/* send to TACACSD as typed */
#define SLIP_DOTTED	  8	/* send to TACACSD as %i */
#define SLIP_FORMAT (SLIP_HEX|SLIP_NAME|SLIP_DOTTED)
#define SLIP_TOSS      0x10	/* toss pkt with implicit index */
/*    #define SLIP_ROUTINGF  0x20	line is allowed to route */
#define SLIP_ROUTEMODE 0x40	/* line is routing, bring up interface */
#define SLIP_SFLOWIN_SUSPENDED   0x80     /* SW flow control on phys-line */
#define SLIP_SFLOWOUT_SUSPENDED  0x100    /* is suspended for SLIP session */
#define SLIP_DEF_ROUTING 0x200   /* will do routing...everytime */
#define SLIP_DYN_ROUTING 0x400   /* will do routing...if /route switch */


#define ASYNC_HOLDQ 10

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * if_slip.c
 */

extern void slip_exec_command(parseinfo *);
extern boolean async_protocols_mode (tt_soc *, addrtype *);
extern void show_slipdata(tt_soc *);
extern void slip_gotchar(tt_soc *, uchar *, int);
extern uint slip_sndchar(tt_soc *, uchar *, int);
extern boolean slip_setup(tt_soc *);
extern void slip_disable(tt_soc *);
extern boolean async_proto_mode(tt_soc *tty);
extern boolean slip_encap_setup(parseinfo *, hwidbtype *);
extern boolean slip_vencap(paktype *, long);
extern void slip_suspend_swflow(tt_soc *);
extern void slip_restore_swflow(tt_soc *);
extern idbtype *get_slip_idb (tt_soc *tty);
extern boolean check_other_asyncs(idbtype *idb,ipaddrtype addr);
extern void slip_parser_init(void);
extern boolean check_slip_on_access(idbtype *idb, char *user_name, 
				    char *password, char **reason, 
				    tt_soc *tty, ipaddrtype address, 
				    boolean routing, boolean exec_cmd);
