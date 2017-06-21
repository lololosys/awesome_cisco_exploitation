/* $Id: hes_chain.c,v 3.5.10.3 1996/06/22 00:42:04 ganesh Exp $
 * $Source: /release/112/cvs/Xsys/hes/hes_chain.c,v $
 *------------------------------------------------------------------
 * HES parse chains
 *
 * Feb 1995, Chris Shaker
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hes_chain.c,v $
 * Revision 3.5.10.3  1996/06/22  00:42:04  ganesh
 * CSCdi57740:  backing store should not default to on
 * Branch: California_branch
 *
 * Revision 3.5.10.2  1996/06/17  23:30:32  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.5.10.1  1996/05/21  09:47:42  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:33:15  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/18  15:45:36  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/17  18:27:20  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:16:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:39:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/02  01:41:10  shaker
 * CSCdi32715:  no transmit-buffers command is urecognized by parser.
 * Change parser code so it is clear that 'backing-store' must follow
 * 'transmit-buffers', instead of implying that '<Return>' would be
 * acceptable.
 *
 * Revision 2.2  1995/07/02  10:51:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:39:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

enum {
    PARSER_TRANSMIT_BUFFERS_BACKINGSTORE = 1,
};




void
transmit_buffers_command (parseinfo *csb)
{
    hwidbtype *hwidb;

    if (!csb->interface)
	return;

    hwidb = csb->interface->hwptr;

    if (csb->nvgen) {
        if (hwidb->status & IDB_CBUS)
             nv_write( (hwidb->pak_backing_store), "%s",csb->nv_command);
	return;
    }

    hwidb->pak_backing_store = csb->set_to_default || csb->sense;
 
    reg_invoke_interface_cfg_modified(hwidb);
 
}

#define	ALTERNATE	NONE
#include "cfg_int_transmit-buffers.h"
LINK_POINT(hes_interface_commands, ALTERNATE);
#undef	ALTERNATE




/*
 * Parse chain registration array for XX
 */
static parser_extension_request hes_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(hes_interface_commands) },
    { PARSE_LIST_END, NULL }
};




/*
 * hes_parser_init - Initialize product specific parser support
 */
static void
hes_parser_init (subsystype* subsys)
{
    parser_add_command_list(hes_chain_init_table, "hes");
}

/*
 * hes subsystem header
 */

#define HES_MAJVERSION   1
#define HES_MINVERSION   0
#define HES_EDITVERSION  1

SUBSYS_HEADER(hes,
	      HES_MAJVERSION, HES_MINVERSION, HES_EDITVERSION,
	      hes_parser_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

