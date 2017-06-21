/* $Id: headers.h,v 1.5 1996/07/13 05:20:36 towu Exp $
 * $Source: /cscka/t1_neat/src/t1/headers.h,v $
 *------------------------------------------------------------------
 * headers.h  NEAT include files
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: headers.h,v $
 * Revision 1.5  1996/07/13 05:20:36  towu
 * o Added include file interrupt.h
 *
 * Revision 1.4  1996/05/12  22:14:45  jchlin
 * o       Add more header include files under headers.h.
 * o       Add rbs_process() in the main process loop.
 * o
 *
 * Revision 1.3  1996/05/09  21:25:49  towu
 * Added new feature Robbed bit signals.
 *
 * Revision 1.2  1996/03/02  03:51:18  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Types first, found in a few different places
 */

#define BIT(pos)  (((uchar) 1) << (pos))
#define BITW(pos) (((ushort) 1) << (pos))
#define BITL(pos) (((ulong) 1) << (pos))

typedef unsigned long   ulong;
typedef unsigned int    uint;
typedef unsigned short  ushort;
typedef unsigned char   uchar;
typedef unsigned char boolean;

#include	"t1_chips.h"
#include	"as_ipc_section.h"
#include	"t1_types.h"
#include	"t1_mib.h"
#include	"t1_framer.h"
#include        "t1_rcv.h"
#include	"t1_externs.h"
#include        "as_utils.h"
#include        "t1_message.h"
#include        "t1_fdl.h"
#include	"as_list.h"
#include	"as_timer.h"
#include        "as_robbed_bit.h"
