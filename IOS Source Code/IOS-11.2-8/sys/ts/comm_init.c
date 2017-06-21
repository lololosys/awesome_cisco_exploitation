/* $Id: comm_init.c,v 3.2 1995/11/17 18:54:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/comm_init.c,v $
 *------------------------------------------------------------------
 * comm_init.c  -- Initialize Communications Server Specific Functions.
 *
 * 4-June-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: comm_init.c,v $
 * Revision 3.2  1995/11/17  18:54:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:09:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"

/*
 * External Symbols
 */
extern void rlogin_init(void);
extern void ts_parser_init(void);
extern void termqueue_init(void);
extern void prompt_init(void);


/*
 * comm_init
 * Initialize the software functions specific to the Comm Server.
 */

static void comm_init (subsystype *subsys)
{
    rlogin_init();
    ts_parser_init();		/* init the command parser */
    termqueue_init();		/* init terminal waiting queue */
    prompt_init();		/* init custom prompt system */
}

/*
 * Communications Server subsystem header
 */

#define COM_MAJVERSION 1
#define COM_MINVERSION 0
#define COM_EDITVERSION  1

SUBSYS_HEADER(commserver, COM_MAJVERSION, COM_MINVERSION, COM_EDITVERSION,
	      comm_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);
