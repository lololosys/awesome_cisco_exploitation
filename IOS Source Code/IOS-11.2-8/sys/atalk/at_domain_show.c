/* $Id: at_domain_show.c,v 3.5.18.3 1996/07/10 22:17:58 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_domain_show.c,v $
 *------------------------------------------------------------------
 * Show support for AppleTalk Remapping
 *
 * September 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_domain_show.c,v $
 * Revision 3.5.18.3  1996/07/10  22:17:58  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.5.18.2  1996/06/29  00:06:38  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.5.18.1  1996/03/23  01:26:07  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.5  1996/02/07  16:09:41  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  05:58:58  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/29  07:26:01  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:42:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/28  00:25:31  slin
 * Placeholders for AppleTalk Code Modularization
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <interface.h>
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../atalk/atalk_private.h"
#include "parser_defs_atalk.h"
#include "at_domain.h"


void
atdomain_show_command (parseinfo *csb)
{

    if (!atalk_running) {
#ifdef ERRMSG_FOR_ERRORS
	errmsg(AT_ERR_NOTRUNNING);
#else
	printf(atalk_not_running);
#endif
	return;
    }

    automore_enable(NULL);
    switch (csb->which) {
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
      case SHOW_APPLE_REMAP:
	show_appletalk_remap(csb);
	break;
      case SHOW_APPLE_DOMAIN:
	show_appletalk_domain(csb);
	break;
    }
}
