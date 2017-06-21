/* $Id: async_chain.c,v 3.7.10.11 1996/09/05 01:17:56 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/os/async_chain.c,v $
 *------------------------------------------------------------------
 * Async parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: async_chain.c,v $
 * Revision 3.7.10.11  1996/09/05  01:17:56  sjackson
 * CSCdi67473:  big buffers created not trimmed (queued on TCP input with
 * rtelnet)
 * Enable coalescing of TCP input packets by default for reverse telnet
 * Branch: California_branch
 *
 * Revision 3.7.10.10  1996/08/28  13:02:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.10.9  1996/08/08  17:57:11  sjackson
 * CSCdi65288:  Modem autoconfig should always redetect and attempt to
 * lock speed
 * Branch: California_branch
 * A line speed change to a line which has modem autoconfigure will
 * force a redetect of the modem after the next hangup.
 *
 * Revision 3.7.10.8  1996/06/28  23:24:34  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.10.7  1996/06/17  23:35:06  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.7.10.6  1996/06/17  08:34:38  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.7.10.5  1996/06/12  14:29:47  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.7.10.4  1996/05/21  09:58:02  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.3  1996/04/15  14:58:50  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.7.10.2  1996/03/27  10:46:32  irfan
 * CSCdi33144:  telnet should be configurable to refuse some options
 * Branch: California_branch
 * Add a bit in tty->capabilities to allow telnet/rlogin to silently
 * ignore remote host's flow control change requests.
 *
 * Revision 3.7.10.1  1996/03/18  21:28:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.10.3  1996/03/16  07:20:17  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.10.2  1996/03/09  05:08:45  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.10.1  1996/02/20  16:36:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/22  14:36:31  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:07:33  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:29:08  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:34:08  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:44:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:16:52  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:50:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:53:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "config.h"
#include "parser.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "logger.h"
#include "aaa.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_misc.h"
#include "../ts/parser_defs_ts.h"
#include "../parser/parser_defs_autoselect.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../tcp/http.h"
#include "../os/nv.h"

/*
 * Parse chains for Async show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_terminal.h"
#include "exec_show_line.h"
LINK_POINT(async_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for async terminal commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_download.h"
LINK_POINT(async_terminal_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for Async line commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_autocommand.h"
#include "cfg_line_rotary.h"
#include "cfg_line_modem.h"
#include "cfg_line_refuse-message.h"
#include "cfg_line_login.h"
#include "cfg_line_vacant-message.h"
#include "cfg_line_logsync.h"
LINK_POINT(async_line_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Async term commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_flowcontrol.h"
#include "cfg_line_speed.h"
#include "cfg_line_stopchar.h"
#include "cfg_line_startchar.h"
#include "cfg_line_stopbits.h"
#include "cfg_line_parity.h"
#include "cfg_line_databits.h"
#include "cfg_line_padding.h"
LINK_POINT(async_term_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Async
 */
static parser_extension_request async_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(async_show_commands) },
    { PARSE_ADD_TERM_CMD, &pname(async_terminal_commands) },
    { PARSE_ADD_CFG_LINE_CMD,  &pname(async_line_commands) },
    { PARSE_ADD_LINE_TERM_CMD, &pname(async_term_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * async_parser_init - Initialize Async parser support
 */
void async_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_CFG_LINE_MODEM_CMD, "line modem",
			  &pname(line_modem_extend_here));
    parser_add_command_list(async_chain_init_table, "async");
}



/*
 * vacant_command
 * Set up a vacant banner for a terminal line
 *    "no vacant-message"      -- never print a vacant banner, clear existing.
 *    "vacant-message"	      -- print default vacant banner
 *    "vacant-message #string# -- print custom vacant banner
 */

void vacant_command (parseinfo *csb)
{
    int i;
    char *tmpstring = NULL;

    if (csb->nvgen) {
	/* NVGEN is only for a single line because we're called 
	 * once for each line.
	 */
	if (MODEMS[(int)csb->line1]->vacantsuppressed) {
	    nv_write(TRUE, "no %s", csb->nv_command);
	} else {
	    nv_write((boolean)MODEMS[(int)csb->line1]->vacantstring,
		    "%s \003%s\003", csb->nv_command, 
		    MODEMS[(int)csb->line1]->vacantstring);
	}
	return;
    }

    if (csb->sense == TRUE && GETOBJ(int,1) == TRUE)
	banner(csb, &tmpstring);
    
    for (i = csb->line1; i < csb->line2; i++) {
	MODEMS[i]->vacantsuppressed = !(csb->set_to_default || csb->sense);
	free(MODEMS[i]->vacantstring);
	MODEMS[i]->vacantstring = tmpstring;
	if (tmpstring)
	  mem_lock(tmpstring);
    }
    free(tmpstring);
}

/*
 * login_command
 * Set logins required flag
 */

void login_command (parseinfo *csb)
{
    int i, type;
    char *list_name;

    if (csb->nvgen) {
	tt_soc *tty = MODEMS[(int)csb->line1];
	switch (csb->which) {
	case LINE_LOGIN:
	    /* Normal TTYs are NO login by default. VTYs are login by default.
	     * Always write something for VTYs to avoid confusion.
	     */
	    type = tty->actype;
	    if (type == AC_NONE && GETOBJ(int,1) == AC_PASSWORD)
		nv_write(tty->type == VTY_LINE, "no %s", csb->nv_command);
	    else if (type == GETOBJ(int,1))
		nv_write(TRUE, "%s", csb->nv_command);
	    break;
	case LINE_AUTHEN:
	    nv_write(tty->login_acc_list_name[0], "%s %s", csb->nv_command,
		     tty->login_acc_list_name);
	    break;
	}
	return;
    }
    switch (csb->which) {
    case LINE_LOGIN:
	if (csb->set_to_default)
	    type = AC_PASSWORD;
	else if (csb->sense)
	    type = GETOBJ(int,1);
	else
	    type = AC_NONE;

	for (i = csb->line1; i < csb->line2; i++) {
	    MODEMS[i]->actype = type;
	    if (old_access_control && (AC_NONE == type) &&
		(MODEMS[i]->autoselectmask & PARSER_AUTOSELECT_DURING_LOGIN))
		printf("\n%% 'No login' overrides 'autoselect"
		       " during-login' on line %d", i);
	}
	break;
    case LINE_AUTHEN:
	if (csb->sense) {
	    switch (GETOBJ(int,2)) {
	    case 1:
		list_name = "default";
		for (i = csb->line1; i < csb->line2; i++) {
		    MODEMS[i]->login_acc_list_name[0] = '\0';
		}
		break;
	    case 2:
		list_name = GETOBJ(string,1);
		for (i = csb->line1; i < csb->line2; i++) {
		    sstrncpy(MODEMS[i]->login_acc_list_name, list_name,
			     sizeof(MODEMS[i]->login_acc_list_name));
		}
		break;
	    default:
		buginf("\n AAA: LOGIN: bad value in authentication list.");
		return;
		break;
	    }
	    if ((csb->resolvemethod == RES_MANUAL) && list_name &&
		!aaa_is_list_defined(list_name, AAA_SVC_LOGIN)) {
		printf("AAA: Warning authentication list \"%s\" is not "
		       "defined for LOGIN.\n", list_name);
	    }
	} else {
	    for (i = csb->line1; i < csb->line2; i++) {
		MODEMS[i]->login_acc_list_name[0] = '\0';
	    }
	}
    }
}

/*
 * pad_command
 * Alter the padding characteristics of a terminal line.
 *     "[no] pading <char> <count> "
 * If there isn't any char specified in a "no" command, unset all the
 * pad characters. 
 */

void pad_command (parseinfo *csb)
{
    uchar ch, pads;
    int i;
    tt_soc *tty = NULL;

    /* NVGEN */
    if (csb->nvgen) {
	uchar *padding;

	tty = MODEMS[(int)csb->line1];
	padding = tty->defaults ?
	    tty->defaults->padding_ptr : tty->current.padding_ptr;
	if (!padding)
	    return;
	for (i = 0; i < MAXASCII; i++) {
	    nv_write(padding[i], "%s %d %d",
		     csb->nv_command, i, (uint)padding[i]);
	}
	return;
    }

    /* "no padding" */
    if (csb->sense == FALSE && GETOBJ(int,3) == CHAR_NONE) {
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    if (csb->permanent == 0 && tty->defaults == 0)
		tty_savedefaults(tty);
	    free(tty->current.padding_ptr);
	    tty->current.padding_ptr = NULL;
	    if (csb->permanent && tty->defaults) {
		free(tty->defaults->padding_ptr);
		tty->defaults->padding_ptr = NULL;
	    }
	}
	return;
    }

    if (GETOBJ(int,3) == CHAR_CHAR)
	ch = (int)*GETOBJ(string,1) & 0x7f;	/* 7-bit chars */
    else
	ch = GETOBJ(int,1);

    /* no padding { <decimal-number> | <char> } [<count>] */
    if (csb->sense == FALSE)
	GETOBJ(int,2) = 0;
	/* Fall-through */

    /* padding { <decimal-number> | <char> } <count> */
    pads = GETOBJ(int,2);
    for (i = csb->line1; i < csb->line2; i++) {
	tty = MODEMS[i];
	if (tty->type != VTY_LINE) {
	    if (csb->permanent == 0 && tty->defaults == 0)
		tty_savedefaults(tty);
	    if (!tty->current.padding_ptr) {
		tty->current.padding_ptr = malloc(MAXASCII);
		if (!tty->current.padding_ptr)
		    return;	/* give up - malloc prints error */
	    }
	    tty->current.padding_ptr[ch] = pads;

	    if (csb->permanent && tty->defaults) {
		    if (!tty->defaults->padding_ptr) {
			tty->defaults->padding_ptr = malloc(MAXASCII);
			if (!tty->defaults->padding_ptr)
			    return;	/* give up - malloc prints error */
		    }
		  tty->defaults->padding_ptr[ch] = pads;
	    }
	} else
	    printf("\nLine %t does not support padding", i);
    }
}

/*
 * logging synchronous command.
 *     "[no] logging synchronous [level x] [limit y]
 * OBJ(int,1) = TRUE if level x
 * OBJ(int,2) = x (-1 if "level all")
 * OBJ(int,3) = TRUE if limit y
 * OBJ(int,4) = y
 */

void log_sync_command (parseinfo *csb)
{
    int i;
    char nvgen_string[80];
    tt_soc *tty = NULL;

    /* NVGEN */
    if (csb->nvgen) {
	tty = MODEMS[(int)csb->line1];
	if (tty->log_sync) {
	    nvgen_string[0] = '\0';
	    if (tty->log_sync_level != LOG_SYNC_DEF_LEVEL) {
		if (tty->log_sync_level >=0)
		    sprintf(nvgen_string, " level %d", tty->log_sync_level);
		else
		    strcpy(nvgen_string, " level all");
	    }
	    if (tty->log_sync_msgsize != LOG_SYNC_DEF_SIZE) {
		nv_write(1, "%s%s limit %d", 
			 csb->nv_command, nvgen_string, tty->log_sync_msgsize);
	    } else {
		nv_write(1, "%s%s", csb->nv_command, nvgen_string);
	    }
	}
	return;
    }

    /* "no logging synchronous" */
    if (csb->sense == FALSE) {
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    tty->log_sync = FALSE;
	    tty->messageQ.maximum = TTY_MESSAGEQ;
	}
	return;
    }

    /* logging synchronous level x limit y */
    for (i = csb->line1; i < csb->line2; i++) {
	tty = MODEMS[i];
	tty->log_sync = TRUE;

	if (GETOBJ(int,1))
	    tty->log_sync_level = GETOBJ(int,2);
	
	if (GETOBJ(int,3)) {
	    tty->log_sync_msgsize = GETOBJ(int,4);
	    tty->messageQ.maximum = tty->log_sync_msgsize;
	}
	
    }
}

/*
 * speed_common
 * Common routine to set terminal speed
 */

static void speed_common (parseinfo *csb, boolean txflag, boolean rxflag)
{
    uint i, txspeed, rxspeed;
    tt_soc *tty;
 
    for (i = csb->line1; i < csb->line2; i++) {
	tty = MODEMS[i];
	if (txflag) {
	    if (csb->sense) {
		txspeed = GETOBJ(int,1);
	    } else {
		txspeed = platformdef_asyncspeed[tty->type];
	    }
	} else {
	    if (tty->tty_txspeed) {
		txspeed = tty->tty_txspeed;
	    } else {
		txspeed = platformdef_asyncspeed[tty->type];
	    }
	}
	if (rxflag) {
	    if (csb->sense) {
		rxspeed = GETOBJ(int,1);
	    } else {
		rxspeed = platformdef_asyncspeed[tty->type];
	    }
	} else {
	    if (tty->tty_rxspeed) {
		rxspeed = tty->tty_rxspeed;
	    } else {
		rxspeed = platformdef_asyncspeed[tty->type];
	    }
	}
	if (csb->permanent == 0 && tty->defaults == NULL) {
	    tty_savedefaults(tty);
	}
	if (setspeed(tty, txspeed, rxspeed) == FALSE) {
	    printf("\nFailed to change line %t's speed", i);
	} else {
	    if (csb->permanent && tty->defaults) {
		tty->defaults->u_rxspeed = rxspeed;
		tty->defaults->u_txspeed = txspeed;
	    }
	    /*
	     * A change of line speed could cause problems for modem
	     * autoconfig so force a redetect when line is next idle.
	     */
	    if (tty->statbits2 & MODEM_CONFIGURED_2) {
		tty->statbits2 &= ~MODEM_CONFIGURED_2;
		tty->statbits  &= ~MODEM_DETECTED;
		printf("\nWarning: modem autoconfig will redetect, this may "
		    "change the line speed");
	    }
	}
    }
}

/* 
 * rxspeed_command
 * txspeed_command
 * speed_command
 * Set RX, TX, or both terminal baud rates
 */

void rxspeed_command (parseinfo *csb)
{
    if (csb->nvgen) {
	tt_soc *tty;
	ulong caps;
	tty_userdata *userconfig;

	tty = MODEMS[csb->line1];
	caps = tty->def_capabilities;
	userconfig = tty->defaults ? tty->defaults : &tty->current;
	if (!(caps & AUTOBAUD) && (tty->tty_passspeed == 0) &&
	    (tty->type != VTY_LINE)) {
	    nv_write((userconfig->u_rxspeed &&
		      (userconfig->u_rxspeed != 
		       platformdef_asyncspeed[tty->type])),
		     "%s %d", csb->nv_command, userconfig->u_rxspeed);
	}
	return;
    }
    speed_common(csb, FALSE, TRUE);
}

void txspeed_command (parseinfo *csb)
{
    if (csb->nvgen) {
	tt_soc *tty;
	ulong caps;
	tty_userdata *userconfig;

	tty = MODEMS[csb->line1];
	caps = tty->def_capabilities;
	userconfig = tty->defaults ? tty->defaults : &tty->current;
	if (!(caps & AUTOBAUD) && (tty->tty_passspeed == 0) &&
	    (tty->type != VTY_LINE)) {
	    nv_write((userconfig->u_txspeed &&
		      (userconfig->u_txspeed != 
		       platformdef_asyncspeed[tty->type])),
		     "%s %d", csb->nv_command, userconfig->u_txspeed);
	}
	return;
    }
    speed_common(csb, TRUE, FALSE);
}

void speed_command (parseinfo *csb)
{
    if (csb->nvgen) {
	/* Do Nothing */
	return;
    }
    speed_common(csb, TRUE, TRUE);
}

/*
 * tty_busymsg
 * assign a message to be printed out on an incoming tcp connection if
 * the line (or all the lines ina rotary group) is busy.
 */

void tty_busymsg (parseinfo *csb)
{
    char *tmpstring = NULL;
    int i;

    if (csb->nvgen) {
        nv_write((boolean)MODEMS[(int)csb->line1]->tty_busymsg,
		 "%s \003%s\003", csb->nv_command, 
		 MODEMS[(int)csb->line1]->tty_busymsg);
	return;
    }

    if (csb->sense == TRUE) {
	banner(csb, &tmpstring);
    }

    for (i = csb->line1; i < csb->line2; i++) {
	free(MODEMS[i]->tty_busymsg);
	MODEMS[i]->tty_busymsg = tmpstring;
	if (tmpstring) {
	    mem_lock(tmpstring);
	}
    }
    free(tmpstring);
}

/*
 * setdef_mask
 * Set a default mask to the specified value.  This means setting the
 * pointer specifed, and also setting ALL the terminal lines.
 */

void setdef_mask (parseinfo *csb, uchar *mask)
{
    int i;

    *mask = (GETOBJ(int,1) == 7) ? 0x7F : 0xFF;
    for (i = 0; i < ALLlines; i++) {
	tt_soc *tty = MODEMS[i];
	if (mask == &datamask_default) {
	    if (csb->permanent && tty->defaults)
		tty->defaults->u_datamask = *mask;
	    else
		tty->current.u_datamask = *mask;
	} else if (mask == &specialmask_default) {
	    if (csb->permanent && tty->defaults)
		tty->defaults->u_specialmask = *mask;
	    else
		tty->current.u_specialmask = *mask;
	} else if (mask == &execmask_default) {
	    if (csb->permanent && tty->defaults)
		tty->defaults->u_execmask = *mask;
	    else
		tty->current.u_execmask = *mask;
	}
    }
}

/*
 * setmask_cmd
 * set an individual line's masks...
 */
void setmask_cmd (parseinfo *csb, uchar *which)
{
    int i, offset;
    tt_soc *tty;
    uchar mask;
    uint number;

    offset = (int) (which - (uchar *)&console->current);

    number = GETOBJ(int,1);
    mask = (number == 7) ? 0x7F : 0xFF;
    for (i = csb->line1; i < csb->line2; i++) {
	tty = MODEMS[i];
	if (csb->permanent == 0 && tty->defaults == 0)
	    tty_savedefaults(tty);
	if (csb->permanent && tty->defaults)
	    *((uchar *) tty->defaults + offset) = mask;
	*((uchar *) (&tty->current) + offset) = mask;
    }
}

/*
 * flowcontrol_command
 * "flowcontrol none|hardware|software [in|out]"
 */

void flowcontrol_command (parseinfo *csb)
{
    int i;
    register ulong oldflags, newflags;
    tt_soc *tty = MODEMS[csb->line1];

    if (csb->nvgen) {
	ulong caps, caps2;

	caps = MODEMS[csb->line1]->def_capabilities;
	caps2 = MODEMS[csb->line1]->def_capabilities2;

	switch (GETOBJ(int,1)) {
	  case FLOWCONTROL_SOFTWARE:
	    if ((caps & SFLOWOUT) && (caps & SFLOWIN)) {
		nv_write(TRUE, csb->nv_command);
	    } else {
		nv_write((caps & SFLOWIN), "%s in", csb->nv_command);
		nv_write((caps & SFLOWOUT), "%s out", csb->nv_command);
	    }
	    nv_add((caps & (SFLOWIN | SFLOWOUT)) &&
		   (caps2 & FLOWPERMANENT_2), " lock");
	    break;

	  case FLOWCONTROL_HARDWARE:
	    if ((caps & HFLOWOUT) && (caps & HFLOWIN)) {
		nv_write(!(TTY_PLATFORM_DEF_CAPABILITIES(tty) & (HFLOWIN
			   | HFLOWOUT)), csb->nv_command);
	    } else {
		nv_write((caps & HFLOWIN), "%s in", csb->nv_command);
		nv_write((caps & HFLOWOUT), "%s out", csb->nv_command);
	    }
	    break;

	  case FLOWCONTROL_NONE:
	    if (!(caps & (SFLOWIN | SFLOWOUT | HFLOWIN | HFLOWOUT))) {
	      if (caps2 & FLOWPERMANENT_2) {
		nv_write(TRUE, "%s lock", csb->nv_command);       /* Flow none
								     lock */
	      } else {
		nv_write((TTY_PLATFORM_DEF_CAPABILITIES(tty) &
                             (SFLOWIN | SFLOWOUT | HFLOWIN | HFLOWOUT)), 
                             csb->nv_command);
	      }
	    }
	    break;	/* Do nothing */

	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
	return;
    }

    /* 
     * You cannot set hardware flow control in while you are doing
     * callin, callout or cts-required. 
     */

    if ((csb->sense == TRUE) && 
	(GETOBJ(int,1) == FLOWCONTROL_HARDWARE) &&
	((GETOBJ(int,2) == FLOWCONTROL_IN) ||
	 (GETOBJ(int,2) == FLOWCONTROL_BOTH))) {
	
	for (i = csb->line1; i < csb->line2; i++) {
	    ulong caps = MODEMS[i]->def_capabilities;

	    /* 
	     * If the MODEMOUT bit is set, we could be doing callout
	     * or inout (inout == MODEMRI|MODEMOUT), so check that
	     * we're not doing inout before relying on the MODEMOUT
	     * bit to indicate callout 
	     *
	     * Also, all "new style" modem control (using modem_type)
	     * allows concurrent HW flow control.
	     */

	    if ((caps & MODEMINOUT) == MODEMINOUT ||
		MODEMS[i]->modem_type > MODEMTYPE_LOCAL) {
		/* modem inout allows h/w flocontrol */
		continue;
	    }

	    /* Test for cts-required, callin or callout and complain
	     * if we are manually configuring
	     */

	    if (caps & (MODEMCTS|MODEMIN|MODEMOUT)) {
		if (csb->resolvemethod == RES_MANUAL) {
		    printf("\n%%Line %t: cts-required, callin or callout"
			   " are incompatible with h/w flow control", i);
		}
		return;
	    }
	}
    }

    oldflags = 0L;
    newflags = 0L;
    switch (GETOBJ(int,2)) {
	case FLOWCONTROL_BOTH:
	    oldflags = SFLOWIN+SFLOWOUT+HFLOWIN+HFLOWOUT;
	    switch (GETOBJ(int,1)) {
		case FLOWCONTROL_SOFTWARE: newflags = SFLOWIN+SFLOWOUT; break;
		case FLOWCONTROL_HARDWARE: newflags = HFLOWIN+HFLOWOUT; break;
	    }
	    break;
	case FLOWCONTROL_IN:
	    oldflags = SFLOWIN+HFLOWIN;
	    switch (GETOBJ(int,1)) {
		case FLOWCONTROL_SOFTWARE: newflags = SFLOWIN; break;
		case FLOWCONTROL_HARDWARE: newflags = HFLOWIN; break;
	    }
	    break;
	case FLOWCONTROL_OUT:
	    oldflags = SFLOWOUT+HFLOWOUT;
	    switch (GETOBJ(int,1)) {
		case FLOWCONTROL_SOFTWARE: newflags = SFLOWOUT; break;
		case FLOWCONTROL_HARDWARE: newflags = HFLOWOUT; break;
	    }
	    break;
    }
    if (csb->sense == FALSE) {			/* did we said "no"? */
	newflags = TTY_PLATFORM_DEF_CAPABILITIES(tty);
    }
    csb->sense = FALSE;
    docaps(csb, oldflags, FIRST_CAP);			/* erase old flags */
    docaps(csb, FLOWPERMANENT_2, SECOND_CAP);           /* erase lock flag */
    csb->sense = TRUE;
    docaps(csb, newflags, FIRST_CAP);			/* set any new flags */
    for (i = csb->line1; i < csb->line2; i++) {
	setflow(MODEMS[i]);			/* toggle hardware */
	setsflow(MODEMS[i]);			/* toggle software */
	/*
	 * sending a gratuitous XON during boot up 
	 */
	(void) reg_invoke_tty_xon(MODEMS[i]);
    }
    if (!(newflags & (HFLOWIN | HFLOWOUT))) {
	if (GETOBJ(int, 3)) {
	    docaps(csb, FLOWPERMANENT_2, SECOND_CAP); /* set lock bit */
	}
    }
}

/*
 * nv_ttyname
 * Write a line to the config file defining a line.
 */
static void nv_ttyname (parseinfo *csb)
{
    char *typename = "";
    tt_soc *tty = MODEMS[csb->line1];
    int base = 0;

    switch (tty->type) {
      case CTY_LINE:
	typename = "con ";
	break;
      case VTY_LINE:
	typename = "vty ";
	base = VTYBase;
	break;
      case AUX_LINE:
	typename = "aux ";
	base = AuxBase;
	break;
      case TTY_LINE:
      default:
	break;
    }
    nv_line(typename, csb->line1 - base);
}

/*
 * line_command
 *
 * Line configuration and terminal option setting.
 *
 */
pdecl(top_config_line);

void line_command (parseinfo *csb)
{
    tt_soc *tty = 0;
    register ulong caps = 0, caps2 = 0;
    tty_userdata *userconfig = 0;
    int nvgen;
    int i, value;

    tty = MODEMS[(int)csb->line1];
    nvgen = csb->nvgen;

    /* If we're doing NV generation, determine the correct config structs, 
     * then fall through to process the selected function.
     */
    if (nvgen) {
	if (csb->line1 < 0 || csb->line1 >= ALLlines || !tty) {
	    errmsg(&msgsym(INVLDLINE, PARSER), csb->line1);
	    return;
	}
	userconfig = tty->defaults ? tty->defaults : &tty->current;
	caps = tty->def_capabilities;
        caps2 = tty->def_capabilities2;
    }

    /* If this is a temporary setting, get the line number to use */
    if (csb->permanent == FALSE && nvgen == FALSE) {
	csb->line1 = stdio->ttynum;
	csb->line2 = csb->line1 + 1;
    }

    switch (csb->which) {
    case TERM_IP_NETMASK_FORMAT:
        if (nvgen) {
            switch(GETOBJ(int, 1)) {
            case MASK_BITCOUNT:
                break;
            case MASK_DOTTED_DECIMAL:
                if (userconfig->u_ip_netmask_format == MASK_DOTTED_DECIMAL)
                    nv_write(TRUE, csb->nv_command);
                break;                  /* default */
            case MASK_HEXADECIMAL:
                if (userconfig->u_ip_netmask_format == MASK_HEXADECIMAL)
                    nv_write(TRUE, csb->nv_command);
                break;
            }
	    return;
        }
	setuser_integer(csb, csb->sense ? (int) GETOBJ(int, 1) :
			MASK_BITCOUNT, &console->tty_ip_netmask_format);
        break;

    case TERM_IP_TCP_QUEUE_SIZE:
	if (nvgen) {
	    if (userconfig->u_tcp_coalesce_size != DEF_TCP_COALESCE) {
		nv_write(TRUE, "%s%s", userconfig->u_tcp_coalesce_size ?
		    common_str_empty_str : "no ", csb->nv_command);
		nv_add(userconfig->u_tcp_coalesce_size, " %d",
		    userconfig->u_tcp_coalesce_size);
	    }
	} else {
	    setuser_integer(csb, csb->sense ? GETOBJ(int, 1) : 0,
		&console->tty_tcp_coalesce_size);
	}
	break;

      case LINE_NUMBER:

	if (nvgen) {
	    /* The TTYLINE action routine will have pushed parsenodes
	     * for each of the lines in the system.  As each line is
	     * processed, we get called.  Clear the nv_command for
	     * the new set of sub-commands, save the current line number
	     * in the csb, and push a node for the top of the line
	     * command parse tree.
	     */
	    *csb->nv_command = '\0';
	    csb->line1 = GETOBJ(int,1);
	    csb->line2 = csb->line1 + 1;

            if (reg_invoke_invalid_line_specified(csb->line1, FALSE))
                return;

	    /* Output a separator comment before the first line only */
	    if (csb->line1 == 0)
		nv_write(TRUE, "!");
	    nv_ttyname(csb);	    /* Generate the tty name */

	    /*
	     * If we're the same as the previous line, just copy his
	     * config directly.
	     */
	    i = csb->line1;
	    if (nv_line_buffer && i && MODEMS[i] && MODEMS[i-1] &&
		    (MODEMS[i]->cfg_counter == MODEMS[i-1]->cfg_counter)) {
		char *p;

		p = nv_line_buffer;
		while (*p == '\n') {
		    ++p;
		}
		if (*p) {
		    nv_write(TRUE, "%s", p);
		}
	    } else {
		/*
		 * Otherwise walk all the nodes and generate it
		 */
		csb->mode = line_mode;
		recurse_node(csb, get_mode_nv(csb));
		csb->mode = config_mode;
	    }
	    return;
	}

	if (csb->sense == FALSE) {
	    destroy_vtylines(GETOBJ(int,1));
	    return;
	}
	if (MODEMS[GETOBJ(int,2)] == 0) {
	    create_vtylines(GETOBJ(int,2));
	}
	/*
	 * Sigh.  Check to see if we succeeded in creating more lines.  This
	 * should only fail during netbooting, when we won't have any vtys.
	 */
	if (MODEMS[GETOBJ(int,2)] == 0) {
	    if (!system_loading)
		printf("\n%% Bad line number - Can't create VTYs");
	    return;
	}
	csb->mode = line_mode;
	if (csb->flags & CONFIG_HTTP) {
	    char *new_uri = string_getnext();
	    char *old_uri = csb->http_state->uri;

	    /* Setup new URI */
	    sprintf(new_uri, "/%s/%d", get_mode_name(csb->mode),
		    GETOBJ(int,1));

	    /* Call parser and generate help for new URI */
	    csb->http_state->uri = new_uri;
	    reg_invoke_http_parser(csb->http_state, line_mode, FALSE);

	    /* Put back old URI */
	    csb->http_state->uri = old_uri;
	} else {
	    csb->line1 = GETOBJ(int, 1);
	    csb->line2 = GETOBJ(int, 2) + 1;
	    mark_range(csb->line1, csb->line2);
            for (i = csb->line1; i < csb->line2; i++) {
                if (reg_invoke_invalid_line_specified(i, TRUE))
                    return;
            }
	}
	break;

      case LINE_ACCESS_CLASS:
	if (nvgen) {
	    nv_write((boolean)tty->accessin,
		    "%s %d in", csb->nv_command, tty->accessin);
	    nv_write((boolean)tty->accessout_default,
		    "%s %d out", csb->nv_command, tty->accessout_default);
	    return;
	}
	if (csb->sense == FALSE)
	    GETOBJ(int,1) = 0;
	switch (GETOBJ(int,2)) {
	case IN:
	    setinteger(csb, (int)GETOBJ(int,1), &console->accessin);
	    break;
	case OUT:
	    setinteger(csb, (int)GETOBJ(int,1), &console->accessout);
	    setinteger(csb, (int)GETOBJ(int,1), &console->accessout_default);
	    break;
	}
	break;

      case LINE_BANNER:
	if (nvgen) {
	    nv_write((boolean)caps & NOEXECBANNER, 
		    "no %s", csb->nv_command);
	    return;
	}
	csb->sense = !(csb->set_to_default || csb->sense);
	docaps(csb,NOEXECBANNER, FIRST_CAP);
	break;

      case LINE_MOTD_BANNER:
        if (nvgen) {
            nv_write((boolean)(caps2 & NOMOTDBANNER_2),
                    "no %s", csb->nv_command);
            return;
        }
	csb->sense = !(csb->set_to_default || csb->sense);
        docaps(csb,NOMOTDBANNER_2, SECOND_CAP);
        break;

      case PARSER_LINE_EXEC:
	if (nvgen) {
	    nv_write((caps & NOEXEC), "no %s", csb->nv_command);
	    return;
	}
	csb->sense = !(csb->set_to_default || csb->sense);
	docaps(csb,NOEXEC, FIRST_CAP);
	break;

      case PARSER_LINE_EXEC_MASK:
	if (nvgen) {
	    nv_write(userconfig->u_execmask != execmask_default,
		     "%s %d", csb->nv_command,
		     (userconfig->u_execmask == 0x7f) ? 7 : 8);
	    return;
	}
	setmask_cmd(csb, &console->tty_execmask);
	break;

      case LINE_EXECTIMEOUT:
	if (nvgen) {
	    nv_write(userconfig->u_exectimeout != IDLE_EXEC_DEFAULT,
		     "%s %d %d", csb->nv_command,
		     userconfig->u_exectimeout/ONEMIN, 
		     (userconfig->u_exectimeout%ONEMIN)/ONESEC);
	    return;
	}
	if (csb->set_to_default)
	    GETOBJ(int,1) = IDLE_EXEC_DEFAULT;
	else if (csb->sense == FALSE)
	    GETOBJ(int,1) = 0;
	else
	    GETOBJ(int,1) = GETOBJ(int,1) * ONEMIN + GETOBJ(int,2) * ONESEC;
	setuser_integer(csb, (int)GETOBJ(int,1) ,&console->tty_exectimeout);
	break;

      case LINE_EXIT_CONFIG:
	if (nvgen) {
	    return;
	}
	csb->mode = config_mode;
	break;

      case LINE_LOC:
	if (nvgen) {
	    nv_write((boolean) tty->location, "%s %s",
		    csb->nv_command, tty->location);
	    return;
	}
	settoken(csb, csb->sense ? GETOBJ(string,1) : NULL,
		 &console->location);
	break;

      case PARSER_LINE_SESSION_TIMEOUT:
	if (nvgen) {
	     nv_write(userconfig->u_sessiontimeout, "%s %d %s",
		      csb->nv_command, userconfig->u_sessiontimeout/ONEMIN,
		      (caps & OUTIDLE)? " output" : "");
	     return;
	 }
	if (csb->sense) {
	    if (tty->sessiontimeout_warning && GETOBJ(int,1) && 
	       (tty->sessiontimeout_warning >= GETOBJ(int,1)*ONEMIN)) {
		printf("\n%% Session Timeout must be greater than"
		       " Session Warning for tty %t", tty->ttynum);
		break;
	    }
	    setuser_integer(csb, (GETOBJ(int,1)*ONEMIN),
			    &console->tty_sessiontimeout);
	    if (GETOBJ(int,2)) {
		docaps(csb, OUTIDLE, FIRST_CAP);
	    } else {
		csb->sense = FALSE;
		docaps(csb, OUTIDLE, FIRST_CAP);
	    }
	} else {
	    setuser_integer(csb, IDLE_SESSION_DEFAULT * ONEMIN,
			    &console->tty_sessiontimeout);
	    csb->sense = FALSE;
	    docaps(csb, OUTIDLE, FIRST_CAP);
	}
	break;

      case LINE_VACANT:
	vacant_command(csb);
	break;

      case PARSER_LINE_DATAMASK:
	if (nvgen) {
	    nv_write(userconfig->u_datamask != datamask_default,
		     "%s %d", csb->nv_command,
		     (userconfig->u_datamask == 0x7f) ? 7 : 8);
	    return;
	}
	setmask_cmd(csb, &console->tty_datamask);
	break;

      case TERM_LENGTH:
	if (nvgen) {
	    nv_write((boolean) userconfig->u_length != LENGTH_DEFAULT,
		     "%s %d", csb->nv_command,
		     userconfig->u_length);
	    return;
	}
	setuser_integer(csb, csb->sense ? (int)GETOBJ(int,1) : LENGTH_DEFAULT,
			&console->tty_length);
	break;

      case TERM_DNS:
	if (nvgen)
	    return;
	
	setuser_integer(csb, csb->sense, &console->tty_dns);

	break;

      case TERM_NOTIFY:
	if (nvgen) {
	    nv_write((boolean)caps & NOTIFY, 
		    csb->nv_command);
	    return;
	}
	docaps (csb, NOTIFY, FIRST_CAP);
	break;

      case PARSER_LINE_SPECIALMASK:
	if (nvgen) {
	    nv_write(userconfig->u_specialmask != specialmask_default,
		     "%s %d", csb->nv_command,
		     (userconfig->u_specialmask == 0x7f) ? 7 : 8);
	    return;
	}
	setmask_cmd(csb, &console->tty_specialmask);
	break;

      case PARSER_LINE_TERM_TYPE:
	if (nvgen) {
	    nv_write((boolean)tty->def_termtype,
		     "%s %s", csb->nv_command,
		     tty->def_termtype);
	    return;
	}
	if (csb->permanent) {
	    settoken(csb, GETOBJ(string,1), &console->def_termtype);
	}
	settoken(csb, GETOBJ(string,1), &console->termtype);
	break;

      case TERM_WIDTH:
	if (nvgen) {
	    nv_write((boolean) userconfig->u_width != WIDTH_DEFAULT, "%s %d",
		     csb->nv_command, userconfig->u_width);
	    return;
	}
	setuser_integer(csb, csb->sense ? (int)GETOBJ(int,1) : WIDTH_DEFAULT,
			&console->tty_width);
	break;

      case PARSER_LINE_HISTORY:
	if (nvgen) {
	    nv_write(!userconfig->u_historyon, "no %s", csb->nv_command);
	    return;
	}
	setuser_integer(csb, csb->set_to_default || csb->sense,
			&console->tty_historyon);
	break;

      case PARSER_LINE_HISTORY_SIZE:
	if (nvgen) {
	    nv_write((userconfig->u_historyon &&
		      (userconfig->u_historysize != HISTORY_SIZE_DEFAULT)),
		     "%s %d", csb->nv_command, userconfig->u_historysize);
	    return;
	}
	setuser_integer(csb, csb->sense ? GETOBJ(int,1) :
			HISTORY_SIZE_DEFAULT, &console->tty_historysize);
	break;

      case PARSER_LINE_EDITING:
	if (nvgen) {
	    nv_write(!userconfig->u_editingon, "no %s", csb->nv_command);
	    return;
	}
	setuser_integer(csb, csb->set_to_default || csb->sense,
			&console->tty_editingon);
	break;

      case PARSER_LINE_HELP:
	if (nvgen) {
	    nv_write(userconfig->u_help, "%s", csb->nv_command);
	    return;
	}
	setuser_integer(csb, csb->sense, &console->tty_help);
	break;

	/*
	 * Commands moved here from ts_line_commands
	 */
      case PARSER_LINE_AUTOCOMMAND:
	if (nvgen) {
	    nv_write((boolean)tty->tty_def_autocommand,
		      "%s %s", csb->nv_command,
		      tty->tty_def_autocommand);
	    return;
	}
	settoken(csb, GETOBJ(string,1), &console->tty_def_autocommand);
	break;

      case PARSER_LINE_DATABITS:
	if (nvgen) {
	    nv_write((userconfig->u_databits && userconfig->u_databits != 8),
		     "%s %d", csb->nv_command, userconfig->u_databits);
	    return;
	}
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    if (csb->permanent == 0 && tty->defaults == 0) {
		tty_savedefaults(tty);
	    }
	    setdatabits(tty, GETOBJ(int,1));

	    /* This might be redundant to set tty_databits, but some */
	    /* of the vectors like lat, 2681 are not setting it up.  */
	    /* To avoid any ambiguity of responsibility, we will     */
	    /* do the parameter word assignment here */
	    tty->tty_databits = GETOBJ(int,1);
	    if (csb->permanent && tty->defaults) {
		tty->defaults->u_databits = GETOBJ(int,1);
	    }
	}
	break;

      case PARSER_LINE_PARITY:
	if (nvgen) {
	    if (GETOBJ(int,1) != PARITY_NO) {
		nv_write((userconfig->u_parity == GETOBJ(int,1)),
			  "%s", csb->nv_command);
	    }
	    return;
	}
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    if (csb->permanent == 0 && tty->defaults == 0) {
		tty_savedefaults(tty);
	    }
	    setparity(tty, GETOBJ(int,1));

	    /* This might be redundant to set tty_parity, but some */
	    /* of the vectors like lat, 2681 are not setting it up.  */
	    /* To avoid any ambiguity of responsibility, we will     */
	    /* do the parameter word assignment here */
	    tty->tty_parity = GETOBJ(int,1);
	    if (csb->permanent && tty->defaults) {
		tty->defaults->u_parity = GETOBJ(int,1);
	    }
	}
	break;

      case PARSER_LINE_ROTARY:
	if (nvgen) {
	    nv_write(tty->rotary, "%s %d", csb->nv_command, tty->rotary);
	    return;
	}
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    if (tty->type != CTY_LINE) {
		tty->rotary = GETOBJ(int,1);
	    }
	}
	reg_invoke_line_handoff();
	break;

      case PARSER_LINE_STARTCHAR:
	if (nvgen) {
	    nv_write((userconfig->u_startchar &&
		      (userconfig->u_startchar != CTRLQ)),
		     "%s %d", csb->nv_command, userconfig->u_startchar);
	    return;
	}
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    value = GETOBJ(int,1) & tty->tty_specialmask;
	    if (csb->permanent == 0 && tty->defaults == 0)
		tty_savedefaults(tty);
	    tty->tty_startchar = value;
	    if (csb->permanent && tty->defaults)
		tty->defaults->u_startchar =  value;
	}
	break;

      case PARSER_LINE_STOPCHAR:
	if (nvgen) {
	    nv_write((userconfig->u_stopchar &&
		      (userconfig->u_stopchar != CTRLS)),
		     "%s %d", csb->nv_command, userconfig->u_stopchar);
	    return;
	}
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    value = GETOBJ(int,1) & tty->tty_specialmask;
	    if (csb->permanent == 0 && tty->defaults == 0)
		tty_savedefaults(tty);
	    tty->tty_stopchar = (uchar) value;
	    if (csb->permanent && tty->defaults) {
		tty->defaults->u_stopchar = (uchar) value;
	    }
	}
	break;

      case LINE_STOPBITS:
	if (nvgen) {
	    nv_write((userconfig->u_stopbits &&
		      (userconfig->u_stopbits != 
		       platformdef_stopbits[tty->type])),
		     "%s %s", csb->nv_command,
		     stopstring(userconfig->u_stopbits));
	    return;
	}
	if (csb->sense == FALSE) {
	  GETOBJ(int,1) = platformdef_stopbits[tty->type];
	}
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    if (csb->permanent == 0 && tty->defaults == 0) {
		tty_savedefaults(tty);
	    }
	    setstopbits(tty, GETOBJ(int,1));

	    /* This might be redundant to set tty_stopbits, but some */
	    /* of the vectors like lat, 2681 are not setting it up.  */
	    /* To avoid any ambiguity of responsibility, we will     */
	    /* do the parameter word assignment here */
	    tty->tty_stopbits = GETOBJ(int,1);
	    if (csb->permanent && tty->defaults) {
		tty->defaults->u_stopbits = GETOBJ(int,1);
	    }
	}
	break;

      case PARSER_LINE_PRIVILEGE:
	if (nvgen) {
	    if (CUR_DEFPRIV(tty->privilege) != DEFAULT_PRIV) {
		nv_write(TRUE, "%s %d", csb->nv_command,
			 CUR_DEFPRIV(tty->privilege));
	    }
	} else {
	    for (i = csb->line1; i < csb->line2; i++) {
		tty = MODEMS[i];
		if (csb->sense) {
		    SET_DEFPRIV(tty->privilege, GETOBJ(int,1));
		} else {
		    SET_DEFPRIV(tty->privilege, DEFAULT_PRIV);
		}
	    }
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * download_command
 * Force a line into "download" mode.  This configuration command is
 * equivalent to giving the following configuration commands.
 *	  - "terminal telnet-transparent"
 *	  - "terminal no escape-character"
 *	  - "terminal no hold-character"
 *	  - "terminal no pad [0-128]"
 *	  - "terminal parity none"
 *	  - "terminal databits 8"
 * Download is for use only with the "terminal" EXEC command.  It is not
 * ever written out to non-volatile memory.
 */

void download_command (parseinfo *csb)
{
    register tt_soc *tty;
    register int i;

    if (csb->nvgen)
	return;	    /* Don't ever NV generate; see comment above. */

    if (csb->permanent) {
	printf("\n\"download\" not for use in configuration files");
	return;
    }
    for (i = csb->line1; i < csb->line2; i++) {
	tty = MODEMS[i];
	if (csb->permanent == 0 && tty->defaults == 0)
	    tty_savedefaults(tty);
	if (csb->sense) {
	    tty->capabilities |= TRANSPARENT;
	    tty->tty_holdchar = -1;
	    tty->tty_escapechar = BREAK_CHAR;
	    free(tty->current.padding_ptr);
	    tty->current.padding_ptr = NULL;
	    setparity(tty, PARITY_NO);
	    setdatabits(tty, 8);
	} else if (tty->defaults) {
	    tty->capabilities &= ~TRANSPARENT;
	    tty->capabilities |= (tty->def_capabilities & TRANSPARENT);
	    tty->tty_holdchar = tty->defaults->u_holdchar;
	    tty->tty_escapechar = tty->defaults->u_escapechar;
	    setparity(tty, tty->defaults->u_parity);
	    setdatabits(tty, tty->defaults->u_databits);
	    if (tty->defaults->padding_ptr) {
		free(tty->current.padding_ptr); /* Just in case */
		tty->current.padding_ptr = malloc(MAXASCII);
		if (!tty->current.padding_ptr)
		    return;	/* give up - malloc prints error */
		bcopy(tty->defaults->padding_ptr, tty->current.padding_ptr,
		      MAXASCII);
	    }
	}
    }
}


