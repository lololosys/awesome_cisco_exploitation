/* $Id: iprouting_debug.h,v 3.2.60.1 1996/04/05 03:46:35 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/iprouting_debug.h,v $
 *------------------------------------------------------------------
 * iprouting_debug.h -- Debugging declarations for IP routing
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iprouting_debug.h,v $
 * Revision 3.2.60.1  1996/04/05  03:46:35  rchandra
 * CSCdi53755:  BGP: flap dampening settings not flexible enough
 * Branch: California_branch
 * - add route-map support to set flap dampening parameters
 *
 * Revision 3.2  1995/11/17  17:34:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/05  22:08:53  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.1  1995/06/07  21:07:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up IP routing debugging */
extern void iprouting_debug_init(void);

/*
 * The effects of some IP routing flags can be restricted. The following
 * variables do the restricting.
 */
extern ipaddrtype egp_debug_addr;
extern ipaddrtype igrp_debug_addr;
extern int bgpupdate_acl;
extern int bgpdamp_acl;
extern int iprouting_acl;
extern int ipcache_debug_acl;
extern int ospf_lsa_gen_acl;
extern int ospf_spf_intra_acl;
extern int ospf_spf_inter_acl;
extern int ospf_spf_ext_acl;
extern int ospf_flood_acl;

/*
 * The actual debugging flags are defined in iprouting_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../iprouting/iprouting_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../iprouting/iprouting_debug_flags.h"
