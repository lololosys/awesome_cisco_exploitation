/* $Id: sr_core.c,v 3.11.4.4 1996/07/17 06:51:43 bchan Exp $
 * $Source: /release/112/cvs/Xsys/os/sr_core.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 * sr_core.c:  core snmp routines
 *
 * February 1994, Jeffrey T. Johnson
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_core.c,v $
 * Revision 3.11.4.4  1996/07/17  06:51:43  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.11.4.3  1996/05/21  09:58:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.4.2  1996/05/21  06:36:38  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.11.4.1  1996/05/17  11:34:21  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.10.30.3  1996/04/26  07:56:55  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.10.30.2  1996/03/26  00:23:16  bchan
 * CSCdi52555:  Allow cip index to be passed in the community string
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.30.1  1996/03/17  18:18:43  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.11  1996/03/09  01:56:02  sdurham
 * CSCdi51080:  Remove unneeded object from tty data structure
 *             tty->snmp_if_index
 *
 * Revision 3.10  1995/12/28  22:35:06  mordock
 * CSCdi46168:  improve snmp modularity
 * move some code and globals out of the kernel subsystem and back into
 * the snmp subsystem
 *
 * Revision 3.9  1995/12/26  23:08:58  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add rpratt's code for MIB/queue manipulation.
 *
 * Revision 3.8  1995/12/15  03:14:06  rpratt
 * CSCdi45651:  Chassis MIB enhancements made Port Ready
 *
 * Revision 3.7  1995/12/10  23:27:53  rpratt
 * CSCdi45343:  Chassis MIB enhancements for VIP & port adapters
 *
 * Revision 3.6  1995/12/07  22:31:16  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.5  1995/11/24  16:40:42  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.4  1995/11/20  22:27:04  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  18:51:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:54  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:19:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/01  05:53:36  thille
 * CSCdi39707:  snmp_get_snmpidb() needs better usage comment
 * Insert one.
 *
 * Revision 2.6  1995/07/17  07:33:34  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.5  1995/07/03  22:24:37  jjohnson
 * CSCdi35401:  Parser help for snmp-server packetsize is incorrect
 * Differentiate between the minimum, maximum, and default packetsize
 *
 * Revision 2.4  1995/06/23  18:48:26  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.3  1995/06/19  06:48:58  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/13  04:45:09  thille
 * CSCdi35746:  More control needed over tftp from SNMP
 * Add access-list checking to SNMP sets which cause tftp reads or writes
 * of config files.
 *
 * Revision 2.1  1995/06/07  21:58:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 *
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "../snmp/snmp_api.h"
#include "interface_private.h"


/* 
 * globals
 * still accessed by various code in the kernel subsystem
 */
boolean snmp_serving;
int snmp_size;
ulong snmp_def_timeout;
int snmp_tftp_server_acl;		/* tftp server acl, or 0 if none */
int snmp_def_qlen;
idbtype *snmp_trap_idb;


void
snmp_core_init (void)
{
    /*
     * initialize variables required by protocol-specific snmp code
     */
    snmp_serving = FALSE;
    snmp_size = SNMP_MAX_MSG_SIZE_DEF;
    snmp_def_timeout = SNMP_DEF_TIMEOUT;
    snmp_def_qlen = SNMP_DEF_QLEN;
  
    snmp_trap_idb = NULL;
}
      
