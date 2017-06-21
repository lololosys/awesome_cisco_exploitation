/* $Id: srswitch_init.c,v 3.2.62.1 1996/03/18 22:00:14 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-68-c7000/srswitch_init.c,v $
 *------------------------------------------------------------------
 * source route switch init 
 *
 * Nov 1994 Jacob Teplitsky
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srswitch_init.c,v $
 * Revision 3.2.62.1  1996/03/18  22:00:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:43:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:57:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:44:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"

/*
 * srswitch_print_message - our contribution to the print_copyright
 * registration point list.
 */
static void srswitch_print_message (void)
{
    printf("Source Route Switch software.\n");
}

/*
 * srswitch_init
 * Add our message to the copyright list.
 */

static void srswitch_init(subsystype * subsys)
{
    reg_add_print_copyright(srswitch_print_message, "srswitch_print_message");
}

#define SRSWITCH_MAJVERSION 1
#define SRSWITCH_MINVERSION 0
#define SRSWITCH_EDITVERSION 1

SUBSYS_HEADER(srswitch, SRSWITCH_MAJVERSION, SRSWITCH_MINVERSION,
	      SRSWITCH_EDITVERSION, 
	      srswitch_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);
