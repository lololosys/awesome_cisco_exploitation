/* $Id: snmp_setup.c,v 3.3 1995/11/28 03:58:58 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/snmp_setup.c,v $
 *------------------------------------------------------------------
 * SNMP setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: snmp_setup.c,v $
 * Revision 3.3  1995/11/28  03:58:58  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  18:58:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:48:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "../ui/setup.h"
#include "../snmp/sr_snmpd.h"

void snmp_setup_common (setup_struct *sp)
{
    char buffer[MAXBUF], *cp;

    sp->run_ip_snmp = yes_or_no("  Configure SNMP Network Management",
				systeminit_flag ? snmp_serving : TRUE,
				TRUE);
    if (sp->run_ip_snmp) {
        cp = setup_string(buffer, "    Community string", "public");
	setup_printf(sp, "\nsnmp-server community %s", cp);
    } else {
	setup_printf(sp, "\nno snmp-server");
    }
}
