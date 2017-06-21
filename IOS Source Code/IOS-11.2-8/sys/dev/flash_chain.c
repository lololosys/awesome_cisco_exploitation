/* $Id: flash_chain.c,v 3.5.10.3 1996/05/21 09:45:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_chain.c,v $
 *------------------------------------------------------------------
 * Flash parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: flash_chain.c,v $
 * Revision 3.5.10.3  1996/05/21  09:45:18  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.2  1996/05/07  00:37:41  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.5.10.1  1996/04/15  21:13:23  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/22  14:32:43  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:27:15  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:25:54  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:00:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/27  21:53:07  izhu
 * CSCdi41172:  7000 dir/show flash command not consistant w/ 7500
 *
 * Revision 2.3  1995/07/14  19:39:48  izhu
 * CSCdi37095:  copy command doesnt support file-id
 * For 7000, add parser link points to support file-id as RSP did in
 * src-rsp/rsp_flash.c
 * Since dev/flash_chain.c is shared by other platforms, we first
 * need to check if it is 7000 before adding those parser link points.
 * Corresponding changes in src-68-c7000/rp_flash.c
 *
 * Revision 2.2  1995/07/04  01:52:43  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  20:24:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_defs_config.h"
#include "../dev/flash_spec.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"



#define SF    "flash"
#define BF    "bootflash"

static boolean flash_has_multiple_partitions(char *dev_name)
{
    return (FALSE);
}
static boolean flash_alias_match (char *alias)
{
    if (strcmp(alias, BF) == 0)
        return (FALSE);
    return (TRUE);
}

/*
 * Parse chains for Flash exec commands
 */
#define	ALTERNATE	NONE
#include "exec_copy_flash.h"
LINK_POINT(flash_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Flash show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_flash.h"
LINK_POINT(flash_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for verify and erase exec commands. 
 */
#define	ALTERNATE	NONE
#include "exec_erase_cmnds.h"
LINK_POINT(exec_erase_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "exec_verify_cmnds.h"
LINK_POINT(exec_verify_commands, ALTERNATE);
#undef	ALTERNATE

#define  ALTERNATE       NONE
#include "exec_show_flash_filesys.h"
LINK_POINT(show_flash_filesys_command, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for Flash
 */
static parser_extension_request flash_chain_init_table[] = {
    { PARSE_ADD_COPY_CMD, &pname(flash_exec_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(flash_show_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_erase_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_verify_commands) },
    { PARSE_LIST_END, NULL }
};

static parser_extension_request flash_filesys_chain_init_table[] = {
    { PARSE_ADD_SHOW_FLASH_FILESYS_CMD, &pname(show_flash_filesys_command)},
    { PARSE_LIST_END, NULL }
};


/*
 * flash_parser_init - Initialize Flash parser support
 */
void flash_parser_init (void)
{
    /* For 7000, we need to extend these commands w/ file-id */

    if (!system_loading) {
	if (reg_invoke_rsp_flash_file_system()) {
	    /* show flash filesys */
	    parser_add_link_point(PARSE_ADD_SHOW_FLASH_FILESYS_CMD,
				  "show flash filesys",
				  &pname(show_flash_filesys_extend_here));

	    /* verify file-id */
	    parser_add_link_point(PARSE_ADD_VERIFY_FILE_CMD,
				  "verify file",
				  &pname(exec_verify_file_extend_here));
	}

	/* copy flash ... */
	parser_add_link_point(PARSE_ADD_COPY_FLASH_CMD,
			      "copy flash ...",
			      &pname(exec_copy_flash_extend_here));
	parser_add_link_point(PARSE_ADD_COPY_FLASH_FILE_CMD,
			      "copy flash ...",
			      &pname(exec_copy_flash_file_extend_here));

	parser_add_command_list(flash_filesys_chain_init_table, "flash");
    }
    parser_add_command_list(flash_chain_init_table, "flash");
}
