/* $Id: clns_globals.c,v 3.2 1995/11/17 08:52:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/clns_globals.c,v $
 *------------------------------------------------------------------
 * Allocation for CLNS global variables
 *
 * January 1995, Dave Katz
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_globals.c,v $
 * Revision 3.2  1995/11/17  08:52:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:17:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "clns.h"

/*
 * Some globals for CLNS are declared here.  This is to allow ISIS code
 * (which includes CLNS adjacency management) to exist without the
 * inclusion of CLNS itself.  This is a relatively small price to pay
 * to get rid of the code...
 */

int clns_cache_version;			/* CLNS cache version number */

NSAP_T         clns_myNET;	          /* Network Entity Title 
					     Must have an NET before an bring 
					     up the clns processes or an 
					     interface */

/* 
 * Routing protocol process queue.
 */
queuetype clns_protoQ;

hwaddrtype     all_iss;			  /* all intermediates systems */
