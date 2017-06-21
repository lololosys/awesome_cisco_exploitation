/* $Id: if_c4000_dsx1.h,v 3.3.62.2 1996/09/07 22:37:10 jliu Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_dsx1.h,v $
 *------------------------------------------------------------------
 * if_c4000_dsx1.h - dsx1 related defines for C4XXX E1/T1 driver.
 *
 * Nov 27 1994, Michael Hubenthal and Manoj Leelanivas
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_dsx1.h,v $
 * Revision 3.3.62.2  1996/09/07  22:37:10  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.3.62.1  1996/06/16  21:15:31  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3  1995/11/17  17:39:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:34:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define DSX1_M32_XMIT_BUF 2
#define W_BOOLEAN_M32_TYPE      1
#define W_BOOLEAN_CALL_TYPE     1

#define DSX1_BIG_BUFFER_LOW_WATERMARK      8
#define DSX1_LARGE_BUFFER_LOW_WATERMARK    4
#define DSX1_HUGE_BUFFER_LOW_WATERMARK    2

/*
 * Events for as5200 clock source selection statemachine
 * (for selecting the proper clk source during automatic loopback)
 */
#define DSX1_INTO_LOOP_EVENT		1
#define DSX1_AUTORECOVERED_EVENT	2
#define DSX1_USER_CONFIG_EVENT	3

/*
 * Prototypes
 */
dsx1info *dsx1_init_context(int, int, int, int, int, int);
void dsx1_create_idle_channel(dsx1info *dsx1);

