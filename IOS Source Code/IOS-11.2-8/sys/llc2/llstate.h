/* $Id: llstate.h,v 3.2 1995/11/17 17:44:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/llc2/llstate.h,v $
 *------------------------------------------------------------------
 * llstate.h -- Static prototypes for llstate.c
 * 
 * January 1991, Rick Watson
 * 
 * Copyright (c) 1991-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: llstate.h,v $
 * Revision 3.2  1995/11/17  17:44:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:42:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

static void admstate(llctype *);
static void setupstate(llctype *);
static void resetstate(llctype *);
static void d_connstate(llctype *);
static void errorstate(llctype *);
static int runstates(llctype *);
static void inval_recv(llctype *, int);
static void normalstate(llctype *);
static void busystate(llctype *);
static void rejectstate(llctype *);
static void awaitstate(llctype *);
static void aw_busystate(llctype *);
static void aw_rejstate(llctype *);
static void clear_rem_busy(llctype *);
static void indinfo(llctype *);
static void send_frame(llctype *, int);
static void send_FRMR(llctype *);
static int send_i_xxx(llctype *);
static int resend_i_xxx(llctype *);
static void re_q_akchain(llctype *);
static void send_ack_xxx(llctype *);
static void set_rem_busy(llctype *);
static void update_n_r(llctype *);
static void update_p_flag(llctype *);
static void start_p_timer(llctype *);
static void stop_p_timer(llctype *);
static void stop_all_timers(llctype *);
static void stop_other_timers(llctype *);
static void zero_v_vals(llctype *);
