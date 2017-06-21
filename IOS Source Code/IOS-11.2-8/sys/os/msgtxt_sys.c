/* $Id: msgtxt_sys.c,v 3.1.2.1 1996/08/28 13:03:44 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/msgtxt_sys.c,v $
 *------------------------------------------------------------------
 * M S G T X T _ S Y S . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgtxt_sys.c,v $
 * Revision 3.1.2.1  1996/08/28  13:03:44  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:51  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Managed timer error messages
 */
const char msgtxt_notfenced[] = "Expired timer is not fenced, timer = %x, type %d";

/*
 * Strings used in various driver error messages
 */
const char msgtxt_nomemory[] = "Unit %d, no memory for %s";
const char msgtxt_initfail[] = "Unit %d, initialization timeout failure, csr%d=0x%04x";
const char msgtxt_memoryerror[] = "Unit %d, memory error, csr%d=0x%04x";
const char msgtxt_badunit[] = "Bad unit number %d";
const char msgtxt_badencap[] = "Interface %s, bad encapsulation in idb->enctype = 0x%x";
const char msgtxt_ownerror[] = "Unit %d, buffer ownership error, pak = 0x%x";
const char msgtxt_badringsize[] = "Bad %s ring size";
const char msgtxt_badcable[] = "Unit %d, Transmits stalled. Check ethernet cable connection";


/*
 * CiscoPro startup error messages
 */
const char msgtxt_ciscopro[] =
	"\n\n%%SYS-1-CISCOPRO: CiscoPro hardware requires CiscoPro software.\n\n";

/*
 * Documentation Support
 *
 msgdef(CISCOPRO, SYS, LOG_ALERT, 0, "CiscoPro hardware requires CiscoPro software.");
 msgdef_explanation(
	"The CiscoPro hardware platforms require the use of CiscoPro
	specific software.  Other software images from Cisco Systems
	will not function on this hardware");
 msgdef_recommended_action(
	"Obtain the appropriate CiscoPro software for your hardware.");
 msgdef_ddts_component("none");
 */
