/* $Id: pt_autocommand.c,v 3.4 1996/02/01 06:08:07 hampton Exp $
 * $Source: /release/111/cvs/Xsys/pt/pt_autocommand.c,v $
 *------------------------------------------------------------------
 * Autocommand specific Protocol Translator code
 *
 * Nov. 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pt_autocommand.c,v $
 * Revision 3.4  1996/02/01  06:08:07  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/29  07:29:29  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:59:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/24  05:07:22  eschaffe
 * CSCdi41300:  translate commands keyword options should nv-genn in lower
 * case
 *
 * Revision 2.2  1995/06/28  09:30:16  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  22:34:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "packet.h"
#include "interface_private.h"
#include "registry.h"
#include "sys_registry.h"
#include "../pt/pt_registry.h"
#include "subsys.h"
#include "address.h"
#include "name.h"
#include "ttysrv.h"
#include "connect.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "parser_defs_pt.h"
#include "translate.h"
#include "../pt/pt_debug.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "access.h"
#include "../pt/pt_autocommand.h"


/*
 * Autocommand Protocol Translator subsystem header
 */

#define PT_AUTOCOMMAND_MAJVERSION 1
#define PT_AUTOCOMMAND_MINVERSION 0
#define PT_AUTOCOMMAND_EDITVERSION  1

SUBSYS_HEADER(pt_autocommand, 
	      PT_AUTOCOMMAND_MAJVERSION, PT_AUTOCOMMAND_MINVERSION, 
	      PT_AUTOCOMMAND_EDITVERSION, 
	      pt_autocommand_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: pad, iphost, tn3270, lat, xremote, ppp, arap",
              NULL);

/*
 * pt_autocommand_init
 *
 * Subsystem initialization point
 */

void pt_autocommand_init (subsystype *subsys)
{
    /*
     * Add parser commands for this subsystem
     */
    pt_autocommand_parser_init();

    /*
     * Add service points for this subsystem
     */
    reg_add_pt_translate_command(TRANS_PROT_AUTOCOMMAND,
				 pt_autocommand_command,
				 "pt_autocommand_command");

    reg_add_pt_show_translate(TRANS_AUTOCOMMAND,
			      pt_show_autocommand_transinfo,
			      "pt_show_autocommand_transinfo");

    reg_add_pt_nv_translate_entry(TRANS_AUTOCOMMAND,
				  pt_nv_autocommand_transinfo,
				  "pt_nv_autocommand_transinfo");

    reg_add_pt_translate_string(TRANS_AUTOCOMMAND, pt_auto_trans_string,
				"pt_auto_trans_string");

    pt_transauto_init();
}

/*
 * pt_autocommand_commmand
 *
 * Registry service point for protocol translator parser chain
 */
int pt_autocommand_command (parseinfo *csb,
			    translatetype *trans,
			    int dir)
{
    trans_autoinfo *autoinfo = &trans->out.autocommand;
    
    setstring(&autoinfo->autocommand_text, GETOBJ(string,5));
    return(TRANS_AUTOCOMMAND);		
}

/*
 * pt_show_autocommand_transinfo
 * 
 * Show AUTOCOMMAND related translate information
 */

void pt_show_autocommand_transinfo (translatetype *trans, int dir)
{
    trans_autoinfo *autocommand = &trans->out.autocommand;
    
    printf("AUTOCOMMAND \"%s\"", autocommand->autocommand_text);
}
/*
 * pt_nv_autocommand_transinfo
 * nv_add AUTOCOMAND related translate information
 */
void pt_nv_autocommand_transinfo (translatetype *trans, int dir)
{
    trans_autoinfo *autocommand = &trans->out.autocommand;
    if (autocommand->autocommand_text != NULL)
	nv_add(TRUE, " autocommand \"%s\"", autocommand->autocommand_text);
}

/*
 * pt_auto_trans_string
 * Take an int and return a string of the protocol name
 */
char *pt_auto_trans_string (int prot)
{
    return("autocommand");
}
