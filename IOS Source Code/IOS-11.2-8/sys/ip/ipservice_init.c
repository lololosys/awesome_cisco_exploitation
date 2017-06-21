/* $Id: ipservice_init.c,v 3.2.60.2 1996/07/06 05:53:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipservice_init.c,v $
 *------------------------------------------------------------------
 * ipservice_init.c -- initialize IP Services support.
 *
 * 20-August-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipservice_init.c,v $
 * Revision 3.2.60.2  1996/07/06  05:53:52  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.2.60.1  1996/04/15  21:16:38  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:35:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:00:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "subsys.h"
#include "ip_registry.h"
#include "ip.h"
#include "../tcp/tcp.h"
#include "../tcp/tcpdriver.h"
#include "../iprouting/route.h"

/*
 * Includes for ipservice_init() calls.
 */
#include "../iprouting/gdp.h"
#include "../servers/dnsix_nat.h"


/*
 * IP Services subsystem header
 *  forward for subsys_init routine.
 */
static void ipservice_init(subsystype *);

#define IPSERVICE_MAJVERSION 1
#define IPSERVICE_MINVERSION 0
#define IPSERVICE_EDITVERSION  1

SUBSYS_HEADER(ipservice, IPSERVICE_MAJVERSION, IPSERVICE_MINVERSION, IPSERVICE_EDITVERSION,
	      ipservice_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);


/*
 * ip_register_thyself
 */
static void ipservice_register_thyself (void)
{
    /*
     * Register some functions with the IP registry.
     */
         /*
	  * Nothing just now.  Question: will this comment be removed
	  * when there *IS* something to do?
	  */
}

/*
 * ip_init
 * Initialize the IP protocol handling
 */

static void ipservice_init (subsystype *subsys)
{
    ipservice_register_thyself();

    /*
     * Initialize the queues
     */
    tcpdriver_init();			/* enable TCP driver stuff */

    /*
     * Initialize alternatives to routing.
     */
    path_init();

    /*
     * Initialize some applications that run only on IP.
     */
    dnsix_init();		/* init DNSIX audit stuff */
}
