/* $Id: qllc_chain.c,v 3.7.10.3 1996/07/09 06:05:26 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/qllc_chain.c,v $
 *------------------------------------------------------------------
 * QLLC: parser chains
 *
 * April, 1994          Phil Morton
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qllc_chain.c,v $
 * Revision 3.7.10.3  1996/07/09  06:05:26  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.7.10.2  1996/05/21  10:05:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.1  1996/05/17  12:13:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.2  1996/04/26  15:23:11  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.2.1  1996/04/26  07:58:22  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7  1996/02/22  14:38:43  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:10:18  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:30:08  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:38:50  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:55:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:31:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/04  01:58:14  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/28  09:31:25  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:01:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "address.h"
#include "interface_generic.h"
#include "subsys.h"
#include "config.h"
#include "ieee.h"
#include "../if/rif.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../cls/dlc_public.h"
#include "qllc_debug.h"
#include "logger.h"
#include "../cls/clsi_msg.h"
#include "../util/avl.h"
#include "qllc.h"
#include "sr_qllc01mib.h"
#include "sr_snadlcconv01mib.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_qllc.h"
#include "../parser/parser_defs_parser.h"

/* parse chains for qllc debug commands */
#define ALTERNATE        NONE
#include "exec_debug_qllc.h"
LINK_POINT(qllc_debug_commands, ALTERNATE);
#undef  ALTERNATE

/* parse chains for qllc interface commands */

#define  ALTERNATE     NONE
#include "cfg_int_qllc.h"
LINK_POINT(qllc_int_commands, ALTERNATE);
#undef   ALTERNATE

/* parse chains for qllc show commands */

#define  ALTERNATE     NONE
#include "exec_show_qllc.h"
LINK_POINT(qllc_show_commands, ALTERNATE);
#undef   ALTERNATE



/*
 * Parse chain registration array for QLLC
 */
static parser_extension_request qllc_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(qllc_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(qllc_show_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(qllc_int_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * qllc_parser_init - Initialize parse chains for QLLC
 * 
 * This routine is called at system startup to register the parse chains
 * for QLLC.
 */

void qllc_ui_init (void)
{
    parser_add_protoaddr    (LINK_QLLC, PMATCH_QLLC PMATCH_DGRAM, "qllc",
                             "qllc protocol", 0, ADDR_QLLC);
    parser_add_address      (ADDR_QLLC, qllc_addr_short_help,
                             match_qllc_addr);
                           
    parser_add_command_list(qllc_chain_init_table, "qllc");
    init_qllcmib();
    init_snadlcconvmib();
}
