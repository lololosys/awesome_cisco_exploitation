/* $Id: msg_ipnat.c,v 3.1.6.4 1996/09/13 23:49:39 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/msg_ipnat.c,v $
 *------------------------------------------------------------------
 * msg_ipnat.c - Message file for IP Network Address Translation
 *
 * January 1996, Andy Heffernan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ipnat.c,v $
 * Revision 3.1.6.4  1996/09/13  23:49:39  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/08/28  12:50:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.6.2  1996/05/13  23:21:21  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:29  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:59:55  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(IPNAT);
msgdef_section("");
