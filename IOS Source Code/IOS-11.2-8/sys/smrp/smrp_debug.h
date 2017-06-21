/* $Id: smrp_debug.h,v 3.2 1995/11/17 18:53:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_debug.h,v $
 *------------------------------------------------------------------
 * Support routines for SMRP debug commands.
 *
 * April 1995, Kent Leung
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_debug.h,v $
 * Revision 3.2  1995/11/17  18:53:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:25:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/01  00:22:43  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.1  1995/06/07  22:45:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_DEBUG_H__
#define __SMRP_DEBUG_H__

#define SMRP_INTERFACE_MASK 0x80000000	/* Used for debugging interfaces. */
#define SMRP_NULL_MASK      0x0        /* Used for general debugging. */
#define SMRP_DEBUG_INTERFACE 1

/* needed?
#include "logger.h"
#include "../if/network.h"
 */


/*
 * Debugging
 */
typedef struct smrp_idblist_ {
    idbtype *idb;
    struct smrp_idblist_ *next;
} smrp_idblist;

typedef struct smrpdebugtype_ {
    int flag;
    char *text;
    smrp_idblist list;
} smrpdebugtype;

extern void smrp_debug_init(void);
extern void smrp_debug_commands(parseinfo *);
extern void smrp_debug_all(boolean);
extern void smrp_debug_show(void);
extern void smrp_debug_setflag(parseinfo const *, smrpdebugtype *);
extern boolean smrp_debug(smrpdebugtype const *, int, ...);
extern void smrp_debug_show_flag(smrpdebugtype const *, boolean, boolean);

extern smrpdebugtype *smrptrue_debug;
extern smrpdebugtype *smrpfalse_debug;
extern smrpdebugtype *smrp_group_debug;
extern smrpdebugtype *smrp_mcache_debug;
extern smrpdebugtype *smrp_neighbor_debug;
extern smrpdebugtype *smrp_port_debug;
extern smrpdebugtype *smrp_route_debug;
extern smrpdebugtype *smrp_trans_debug;

/* Keyword codes for the parser */
enum {
    DEBUG_SMRP_ALL,
    DEBUG_SMRP_GROUP,
    DEBUG_SMRP_MCACHE,
    DEBUG_SMRP_NEIGHBOR,
    DEBUG_SMRP_PORT,
    DEBUG_SMRP_ROUTE,
    DEBUG_SMRP_TRANSACTION,
};

#endif
