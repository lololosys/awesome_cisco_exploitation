/* $Id: atip_init.c,v 3.2.60.3 1996/07/02 17:00:45 slin Exp $
 * $Source: /release/112/cvs/Xsys/atalk/atip_init.c,v $
 *------------------------------------------------------------------
 * Appletalk IP-services Initialization routines
 *
 * March 1994, Steven Lin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atip_init.c,v $
 * Revision 3.2.60.3  1996/07/02  17:00:45  slin
 * CSCdi61526:  MacIP ipgpAssign Error - Overlapping IP address with
 * client range
 * Branch: California_branch
 * - During IP address assignment, skip addresses that fail IP alias chec
 *
 * Revision 3.2.60.2  1996/05/02  08:41:10  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2.60.1  1996/03/23  01:28:14  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:44:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
$Endlog$
 */

#include "master.h"
#include <interface.h>
#include "packet.h"
#include "subsys.h"
#include "atalk_private.h"
#include "cayman_talk.h"
#include "macip_parse.h"

static void atip_init(subsystype *);
void atip_debug_init(void);
void atip_parser_init(void);

/*
 * AppleTalk IP-services subsystem header
 */
#define ATIP_MAJVERSION  1
#define ATIP_MINVERSION  0
#define ATIP_EDITVERSION 1

SUBSYS_HEADER(atip, ATIP_MAJVERSION, ATIP_MINVERSION, ATIP_EDITVERSION,
	      atip_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, tunnel, atalk", "req: iphost, ipservice, tunnel, "
	      "atalk");


/*
 * atip_register_thyself
 */
static void
atip_register_thyself (void)
{
    /*
     * Nothing just now.  Question: will this comment be removed
     * when there *IS* something to do?
     */

}

/*
 * atip_init
 *
 * Initialize the AppleTalk IP-services.
 */
static void
atip_init (subsystype *subsys)
{
    atip_register_thyself();

    atip_parser_init();
    atip_debug_init();

    macip_init();
    iptalk_init();
    cayman_init();
}
