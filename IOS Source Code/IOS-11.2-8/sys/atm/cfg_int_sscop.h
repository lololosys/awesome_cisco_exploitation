/* $Id: cfg_int_sscop.h,v 3.2.56.1 1996/03/18 18:59:50 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/atm/cfg_int_sscop.h,v $
 *------------------------------------------------------------------
 * parser/cfg_int_sscop.h
 *
 * November 1993,  Percy P. Khabardar 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_sscop.h,v $
 * Revision 3.2.56.1  1996/03/18  18:59:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:09:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  13:35:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:46:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  22:07:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * sscop 
 *
 * sscop interface config commands
 */


/******************************************************************
 * [no] sscop quick-poll
 * Handle Poll Pdu at interrupt
 */
EOLS    (sscop_quick_poll_eol, sscop_interface_commands, SSCOP_QUICK_POLL);
KEYWORD (sscop_quick_poll, sscop_quick_poll_eol, no_alt,
	 "quick-poll", "Quick Poll", PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 * sscop max-stat <n>
 */
PARAMS_KEYONLY(sscop_max_stat, sscop_quick_poll, "max-stat",
	       OBJ(int,1), 1, 255,
	       sscop_interface_commands, SSCOP_MAX_STAT,
	       "Maximum number of entries in a Stat frame",
	       "Maximum number of entries in a Stat frame",
	       PRIV_CONF );

/***************************************************************
 * sscop rcv-window <n> 
 */
PARAMS_KEYONLY(sscop_rcv_window, sscop_max_stat, "receive-window-",
	       OBJ(int,1), 1, 127,
	       sscop_interface_commands, SSCOP_RCV_WINDOW,
	       "Max number of Sd(p) frames our partner can send",
	       "Max number of Sd(p) frames our partner can send",
	       PRIV_CONF );

/***************************************************************
 * sscop send-window <n> 
 */
PARAMS_KEYONLY(sscop_send_window, sscop_rcv_window, "send-window",
	       OBJ(int,1), 1, 127,
	       sscop_interface_commands, SSCOP_SEND_WINDOW,
	       "Max number of Sd frames to send before waiting for acknowledgment",
	       "Max number of Sd frames to send before waiting for acknowledgment", 
               PRIV_CONF );

/***************************************************************
 * sscop max-cc <n> 
 */
PARAMS_KEYONLY(sscop_max_cc, sscop_send_window, "max-cc",
	       OBJ(int,1), 1, 127,
	       sscop_interface_commands, SSCOP_MAX_CC,
	       "Max number of retries for connection control operations",
	       "Max number of retries for connection control operations", 
               PRIV_CONF );

/***************************************************************
 * sscop cc-timer <n> 
 */
PARAMS_KEYONLY(sscop_cc_timer, sscop_max_cc, "cc-timer",
	       OBJ(int,1), 1, 60000,
	       sscop_interface_commands, SSCOP_CC_TIMER_T,
	       "Max time (in msecs) to wait for a response before tx another cc pdu", 
               "Max time (in msecs) to wait for a response before tx another cc pdu", 
               PRIV_CONF);

/***************************************************************
 * sscop keepalive-timer <n> 
 */
PARAMS_KEYONLY(sscop_keepalive_timer, sscop_cc_timer, "keepalive-timer",
	       OBJ(int,1), 1, 60000, 
	       sscop_interface_commands, SSCOP_KEEPALIVE_TIMER_T,
	       "timer (in msecs) to transmit poll pdu during periods of inactivity",
	       "timer (in msecs) to transmit poll pdu during periods of inactivity", 
               PRIV_CONF );

/***************************************************************
 *
 * sscop poll-timer 
 */
PARAMS_KEYONLY(sscop_poll_timer, sscop_keepalive_timer, "poll-timer",
	       OBJ(int,1), 1, 60000, 
	       sscop_interface_commands, SSCOP_POLL_TIMER_T,
	       "Max time (in msecs) to wait for a response before sending a poll pdu", 
               "Max time (in msecs) to wait for a response before sending a poll pdu", 
               PRIV_CONF);

KEYWORD	(sscop, sscop_poll_timer, NONE, "sscop",
	 "SSCOP Interface Subcommands", PRIV_CONF);

ASSERT (sscop_test, sscop, ALTERNATE, is_atm(csb->interface->hwptr));

#undef	ALTERNATE
#define	ALTERNATE    sscop_test	
