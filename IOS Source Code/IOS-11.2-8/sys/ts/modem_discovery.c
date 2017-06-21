/* $Id: modem_discovery.c,v 3.7.10.4 1996/08/12 21:32:02 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/ts/modem_discovery.c,v $
 *------------------------------------------------------------------
 * modem_discovery.c -- automatic recognition of modems
 *
 * September 1995, Tim Kolar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_discovery.c,v $
 * Revision 3.7.10.4  1996/08/12  21:32:02  sjackson
 * CSCdi65763:  Desired modem type should only be applicable in discovery
 *              mode
 * Branch: California_branch
 *
 * Revision 3.7.10.3  1996/05/21  10:07:13  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.2  1996/03/28  18:38:29  sjackson
 * CSCdi50364:  Modem autoconfig database should contain Microcom commands
 * Branch: California_branch
 *
 * Revision 3.7.10.1  1996/03/18  22:19:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  07:44:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  10:58:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  21:24:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/22  14:39:21  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/01/25  02:16:17  sjackson
 * CSCdi46992:  modem auto discover doesnot recognize codex, usr_courier,
 * etc.
 * Added required ID strings
 *
 * Revision 3.5  1996/01/22  07:29:57  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/18  15:54:13  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/17  18:39:53  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:55:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:33:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/27  04:18:50  tkolar
 * Files for the modemcap and modem_discovery subsystems.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#include <string.h>
#include "ttysrv.h"
#include "registry.h"
#include "modem_discovery.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "../h/cs_registry.regh"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ts/modemcap_public.h"
#include "../parser/parser_defs_config.h"

char *modem_discovery (tt_soc *tty)
{
    int i;
    int count;
    char buf[MAX_MODEM_STRING];
    char *type;

    if (tty->capabilities2 & MA_DISCOVERY_2) {
	/*
	 * A certain modem type may always be present for discovering
	 */
	type = reg_invoke_modem_type_desired(tty);
	if (type) {
	    return(type);
	}

	for (i = 0; autodetected_modems[i].sendstring[0] != '\0'; i++) {
	    count = modem_talk(tty, autodetected_modems[i].sendstring, 
				    buf, MAX_MODEM_STRING);
	    if (count >= MAX_MODEM_STRING) {
		count = MAX_MODEM_STRING - 1;
	    }
	    buf[count] = '\0';
	    if (strstr(buf, autodetected_modems[i].id))
		break;
	}

	if (autodetected_modems[i].modem_name[0]) {
	    return(autodetected_modems[i].modem_name);
	} else {
	    return(DEFAULT_HAYES_MODEM);
	}
    }
    return(NULL);
}


#define ALTERNATE	NONE
#include "../ts/cfg_modem_autoconfigure_discovery.h"
LINK_POINT(modem_discovery_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for Modem discovery
 */

static parser_extension_request modem_discovery_chain_init_table[] = {
    { PARSE_ADD_CFG_LINE_MODEM_AUTOCONFIGURE_CMD, 
					&pname(modem_discovery_commands) },
    { PARSE_LIST_END, NULL }
};

static void modem_discovery_init (subsystype *subsys) 
{
    parser_add_command_list(modem_discovery_chain_init_table, 
						"Modem Discovery");
    reg_add_modem_discovery(modem_discovery, "modem_discovery");

}

/*
 * Modem discovery subsystem header
 */
#define MODEM_DISCOVERY_MAJVERSION 1
#define MODEM_DISCOVERY_MINVERSION 0
#define MODEM_DISCOVERY_EDITVERSION 1

SUBSYS_HEADER(modem_discovery,
              MODEM_DISCOVERY_MAJVERSION, MODEM_DISCOVERY_MINVERSION,
              MODEM_DISCOVERY_EDITVERSION,
              modem_discovery_init, SUBSYS_CLASS_LIBRARY,
              "req: modemcap",
              NULL);
