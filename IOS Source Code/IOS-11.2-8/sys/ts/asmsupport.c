/* $Id: asmsupport.c,v 3.11.6.4 1996/07/04 00:29:03 irfan Exp $
 * $Source: /release/112/cvs/Xsys/ts/asmsupport.c,v $
 *------------------------------------------------------------------
 * asmsupport.c -- Terminal Server Specific Routines
 *
 * 16-July-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asmsupport.c,v $
 * Revision 3.11.6.4  1996/07/04  00:29:03  irfan
 * CSCdi61001:  Autoselect throws away characters on startup
 * Branch: California_branch
 * Add "flush-at-activation" line subcommand to the ts system.
 *
 * Revision 3.11.6.3  1996/06/17  23:38:32  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.11.6.2  1996/06/12  14:42:55  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.11.6.1  1996/03/18  22:19:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/16  07:44:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.2  1996/03/07  10:57:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  21:23:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/07  00:55:02  irfan
 * CSCdi50845:  tty lines change speed unexpectedly
 * "no autobaud" should not change line speed to AUTOSPEEHIGH (19200).
 *
 * Revision 3.10  1996/02/28  09:23:13  irfan
 * CSCdi50137:  telnet speed is missing from config
 * use correct pointer to line configuration.
 *
 * Revision 3.9  1996/02/07  16:16:15  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/02/03  04:27:22  tkolar
 * CSCdi47986:  IOS modem integration isnt quite there yet
 * Whups!  Never go logic chopping without the chainsaw.
 *
 * Revision 3.7  1996/02/03  04:13:16  tkolar
 * CSCdi47986:  IOS modem integration isnt quite there yet
 * Add a way for the IOS to know if the modem switch has reserved the
 * modem.
 *
 * Revision 3.6  1996/02/01  06:10:52  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/22  23:42:12  irfan
 * CSCdi47309:  autoselect during-login ignored with aaa new-model
 * review comments for CSCdi44865.
 *
 * Revision 3.4  1995/12/22  01:12:24  perryl
 * CSCdi44865:  autoselect during-login should be ignored if login is not
 * set.
 * Avoid the case for "no login" and "autoselect during-login" ending
 * up not firing autoselect on the line. Add warning messages for
 * conflicting configurations.
 *
 * Revision 3.3  1995/11/20  22:28:05  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.2  1995/11/17  18:52:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:32:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/11  21:43:12  tkolar
 * CSCdi40022:  Telnet speed is retained even after the session is closed
 * Move passspeed and maxpassspeed into the userdata struct.
 *
 * Revision 2.2  1995/06/09  23:40:29  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.1  1995/06/07  23:08:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "connect.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_defs_autoselect.h"
#include "parser_defs_ts.h"
#include "../parser/parser_defs_config.h"
#include "sys_registry.h"
#include "name.h"
#include "aaa.h"
#include "../h/cs_registry.regh"

/*
 * Note.  Configuration commands that can be accessed by the EXEC "terminal"
 * command should examine csb->permanent before setting any default values.
 * If csb->permanent == TRUE, then we are setting the permanent value that
 * may be reset when an EXEC is started.  If csb->permanent is FALSE, then
 * we have a user setting a temporary value for that terminal session.
 */

/*
 * passspeed_command
 * Set default line speed to send via Telnet, and the maximum speed
 * we allow to be negotiated. 
 */

static void passspeed_command (parseinfo *csb)
{
    if (! csb->sense) {
	GETOBJ(int,2) = 0;
	GETOBJ(int,3) = 0;
    }
    setuser_integer(csb, GETOBJ(int,3), &console->tty_maxpassspeed);
    setuser_integer(csb, GETOBJ(int,2), &console->tty_passspeed);
}

/*
 * telnet_command
 * Handle per-line telnet protocol configuration
 * 	telnet pass-speed <default> [<maximum>]
 *	telnet refuse-negotiations
 *	telnet sync-on-break
 *      telnet transparent
 *	telnet break-on-ip
 *	telnet ip-on-break
 */

void telnet_command (parseinfo *csb)
{
    ulong caps, caps2;
    tt_soc *tty;
    tty_userdata *ttyconfig;

    tty = MODEMS[csb->line1];

    if (csb->nvgen) {

	caps = tty->def_capabilities;
	caps2 = tty->def_capabilities2;

	switch (GETOBJ(int,1)) {
	  case TN_BREAKIP:
	    nv_write((caps & BREAKONIP), csb->nv_command);
	    break;
	  case TN_REFUSE:
	    nv_write((caps & REFUSEALL), csb->nv_command);
	    break;
	  case TN_SPEED:
	    ttyconfig = tty->defaults ? tty->defaults : &tty->current;
	    nv_write(ttyconfig->u_passspeed,
		     "%s %d %d", csb->nv_command,
		     ttyconfig->u_passspeed,
		     ttyconfig->u_maxpassspeed);
	    break;
	  case TN_SYNC:
	    nv_write((caps & BREAKSYNC), csb->nv_command);
	    break;
	  case TN_TRANS:
	    nv_write((caps & TRANSPARENT), csb->nv_command);
	    break;
          case TN_IPBREAK:
            nv_write((caps2 & IPONBREAK_2), csb->nv_command);
            break;
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
	return;
    }

    switch (GETOBJ(int,1)) {
      case TN_BREAKIP:
	docaps(csb, BREAKONIP, FIRST_CAP);
	break;
      case TN_REFUSE:
	docaps(csb, REFUSEALL, FIRST_CAP);
	break;
      case TN_SPEED:
	passspeed_command(csb);
	break;
      case TN_SYNC:
	docaps(csb, BREAKSYNC, FIRST_CAP);
	break;
      case TN_TRANS:
	docaps(csb, TRANSPARENT, FIRST_CAP);
	break;
      case TN_IPBREAK:
        docaps(csb, IPONBREAK_2, SECOND_CAP);
        break;
      default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
	break;
    }
}

/*
 * asm_line_commands
 * comm server specific line commands
 */

void asm_line_commands (parseinfo *csb)
{
    int nvgen = csb->nvgen;
    tt_soc *tty = MODEMS[(int)csb->line1];
    register ulong caps = 0;
    int i, value;
    tty_userdata *userconfig = 0;

    /* If we're doing NV generation, determine the correct config structs,
     * then fall through to process the selected function.
     */
    if (nvgen) {
	if ((csb->line1 < 0) || (csb->line1 >= ALLlines) || !tty) {
	    printf("Invalid line in NV generation: %t\n", csb->line1);
	    return;
	}
	userconfig = tty->defaults ? tty->defaults : &tty->current;
	caps = tty->def_capabilities;
    }
    /* If this is a temporary setting, get the line number to use */
    if ((csb->permanent == FALSE) && (nvgen == FALSE)) {
	csb->line1 = stdio->ttynum;
	csb->line2 = csb->line1 + 1;
    }

    switch (csb->which) {
      case PARSER_LINE_ACTIVATECHAR:
	if (nvgen) {
	    if (tty->activatechar == -1) {
		nv_write(TRUE, "no %s", csb->nv_command);
	    } else {
		nv_write((tty->activatechar != RETURN),
			 "%s %d", csb->nv_command, tty->activatechar);
	    }
	    return;
	}
	if (csb->set_to_default) {
	    setinteger(csb, RETURN, &console->activatechar);
	} else if (csb->sense) {
	    setinteger(csb, GETOBJ(int,1), &console->activatechar);
	} else {
	    setinteger(csb, -1, &console->activatechar);
	}
	break;

      case PARSER_LINE_AUTOSELECTDEF:
	if (!nvgen) {
	    for (i = csb->line1; i < csb->line2; i++) {
		tty = MODEMS[i];
		if (csb->sense)
		    tty->autoselectmask |= PARSER_AUTOSELECT_ARAP;
		  else
		    tty->autoselectmask &= ~(PARSER_AUTOSELECT_ALL);
	    }
	}
	break;

      case PARSER_LINE_AUTOSELECT:
	value = GETOBJ(int,1);
        if (nvgen && value) {
            nv_write((tty->autoselectmask & value), "%s", csb->nv_command);
            return;
        }
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    if (csb->sense) {
		tty->autoselectmask |= value;
		/*
		 * We should let the administrator know if an
		 * obvious misconfiguration is being attempted.
		 */
		if (systeminit_flag && (value & (PARSER_AUTOSELECT_SLIP |
						 PARSER_AUTOSELECT_PPP))
		    && (0 == (tty->capabilities & MAYSLIP))) {
		    printf("\n%%Autoselect w/o the interface command"
			   " 'Async mode interactive' is useless");
		}
		if (old_access_control && (AC_NONE == tty->actype) &&
		    (value & (PARSER_AUTOSELECT_DURING_LOGIN))) {
		    printf("\n%%Autoselect during-login disabled on"
			   " line %d, until 'login' is set", i);
		}
	    } else {
		tty->autoselectmask &= ~value;
	    }
	}
        break;

      case PARSER_LINE_AUTOBAUD:
	if (nvgen) {
	    if (caps & AUTOBAUD) {
		if (GETOBJ(int,1)) {
		    nv_write(caps & AUTOHIGH, "%s", csb->nv_command);
		} else {
		    nv_write(!(caps & AUTOHIGH), "%s", csb->nv_command);
		}
	    }
	    return;
	}

	/*
	 * Clear current setting
	 */
	i = csb->sense;
	csb->sense = FALSE;
	docaps(csb, AUTOBAUD|AUTOHIGH, FIRST_CAP);
	csb->sense = i;

	/*
	 * Pick bits involved with operation.  Apply to named TTYs.
	 */
	i = AUTOBAUD;
	if (GETOBJ(int, 1)) {
	    i |= AUTOHIGH;
	}
	docaps(csb, i, FIRST_CAP);

	/*
	 * Now update speeds on appropriate modems
	 */
	if (csb->sense) {
	    for (i = csb->line1; i < csb->line2; i++) {
		tty = MODEMS[i];
		if (!(tty->statbits & HASPROC) &&
		    !reg_invoke_modem_hardware_in_use(tty)) {
		    auto_setspeed(tty);
		}
	    }
	}
	break;


      case PARSER_LINE_AUTOHANGUP:
	if (nvgen) {
	    nv_write((caps & AUTOHANGUP), "%s", csb->nv_command);
	    return;
	}
	docaps(csb, AUTOHANGUP, FIRST_CAP);
	break;

      case PARSER_LINE_AUTOHOST:
	if (nvgen) {
	    /* No NV generation */
	    return;
	}
	if (csb->sense) {
	    sprintf(GETOBJ(string,2), "connect %s", GETOBJ(string,1));
	    settoken(csb, GETOBJ(string,2), &console->tty_def_autocommand);
	} else {
	    settoken(csb, NULL, &console->tty_def_autocommand);
	}
	break;

      case PARSER_LINE_DISCONNECTCHAR:
	if (nvgen) {
	    nv_write((MODEMS[csb->line1]->logoutchar != -1),
		     "%s %d", csb->nv_command,
		     MODEMS[csb->line1]->logoutchar);
	    return;
	}
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    if (csb->sense)
		value = GETOBJ(int,1) & tty->tty_specialmask;
	    else
		value = -1;
	    tty->logoutchar = value;
	}
	break;

      case PARSER_LINE_DISPATCHCHAR:
	if (nvgen) {
	    for (i = 0; i < MAXASCII; i++) {
		nv_write(userconfig->u_editinfo.dispatch[i] & TTY_DISP_DISP,
			 "%s %d", csb->nv_command, i);
	    }
	    return;
	}
	if (!csb->sense && (GETOBJ(int,10) == 0)) {
	    for (i = csb->line1; i < csb->line2; i++) {
		tty = MODEMS[i];
		if (csb->permanent == 0 && tty->defaults == 0) {
		    tty_savedefaults(tty);
		}
		memset(tty->tty_editinfo.dispatch, 0, MAXASCII);
		if (csb->permanent && tty->defaults) {
		    memset(tty->defaults->u_editinfo.dispatch, 0, MAXASCII);
		}
	    }
	    return;
	}
	for (i=0; i < GETOBJ(int,10); i++) {
	    int j;
	    uchar ch = (uchar) csb->uiarray[i];

	    for (j = csb->line1; j < csb->line2; j++) {
		tty = MODEMS[j];
		if (csb->permanent == 0 && tty->defaults == 0)
		    tty_savedefaults(tty);
		if (csb->sense) {
		    tty->tty_editinfo.dispatch[ch] |= TTY_DISP_DISP;
		    if (csb->permanent && tty->defaults) {
			tty->defaults->u_editinfo.dispatch[ch] |=
			    TTY_DISP_DISP;
		    }
		} else {
		    tty->tty_editinfo.dispatch[ch] &= ~TTY_DISP_DISP;
		    if (csb->permanent && tty->defaults) {
			tty->defaults->u_editinfo.dispatch[ch] &=
			    ~TTY_DISP_DISP;
		    }
		}
	    }
	}
	break;

      case PARSER_LINE_DISPATCH_TIMEOUT:
	if (nvgen) {
	    nv_write(userconfig->u_dispatchtimeout,
		     "%s %d", csb->nv_command,
		     userconfig->u_dispatchtimeout);
	    return;
	}
	setuser_integer(csb, GETOBJ(int,1), &console->tty_dispatchtimeout);
	break;

      case PARSER_LINE_HOLDCHAR:
	if (nvgen) {
	    nv_write((userconfig->u_holdchar != -1),
		     "%s %d", csb->nv_command, userconfig->u_holdchar);
	    return;
	}
	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    if (csb->sense)
		value = GETOBJ(int,1) & tty->tty_specialmask;
	    else
		value = -1;
	    if (csb->permanent == 0 && tty->defaults == 0)
		tty_savedefaults(tty);
	    if (csb->permanent && tty->defaults)
		tty->defaults->u_holdchar = value;
	    tty->current.u_holdchar = value;
	}
	if (csb->permanent == 0) {
	    if (stdio->tty_holdchar == -1) {
		printf("Local hold is disabled");
	    } else {
		printf("\"%c\" is the local hold character",
		       stdio->tty_holdchar);
	    }
	}
	break;

      case LINE_INSECURE:
	if (nvgen) {
	    nv_write((boolean)caps & INSECURE,
		    csb->nv_command);
	    return;
	}
	docaps (csb, INSECURE, FIRST_CAP);
	break;

      case PARSER_LINE_LOCKABLE:
	if (nvgen) {
	    nv_write((caps & LOCKABLE), "%s", csb->nv_command);
	} else {
	    docaps(csb,LOCKABLE, FIRST_CAP);
	}
	break;

      case PARSER_LINE_PRIVATE:
	if (nvgen) {
	    nv_write((caps & PRIVATE_TTY), "%s", csb->nv_command);
	} else {
	    docaps(csb,PRIVATE_TTY, FIRST_CAP);
	}
	break;

      case LINE_SESSION_LIMIT:
	if (nvgen) {
	    nv_write ((boolean) tty->sessionlimit, "%s %d",
		    csb->nv_command, tty->sessionlimit);
	    return;
	}
	setuser_integer(csb, csb->sense ? (int)GETOBJ(int,1) : 0,
			  &console->sessionlimit);
	break;

      case LINE_SESSIONTIMEOUT_WARNING:
	if (nvgen) {
	    nv_write(tty->sessiontimeout_warning,
                     "%s %d", csb->nv_command,
                     tty->sessiontimeout_warning/ONESEC);
	    if (tty->sessiontimeout_warning_msg) {
		nv_add(TRUE, " message %s", tty->sessiontimeout_warning_msg);
	    }
            return;
	}
	if (csb->sense) {
	    if (GETOBJ(int,1)) {
		if (GETOBJ(int,1)*ONESEC >= tty->tty_sessiontimeout) {
		    printf("\n%%  Warning Timer must be less than Session"
			   " Timeout for tty %t", tty->ttynum);
		    break;
		}	    
		if (GETOBJ(int,1) < SESSIONTIMEOUT_WARNING_MIN) {
		    printf("\n%% Countdown values of less than %d %s",
				SESSIONTIMEOUT_WARNING_MIN,
				"may not generate a Disconnect Warning."); 
		}
	    }
	    setinteger(csb, (GETOBJ(int,1)*ONESEC),
			&console->sessiontimeout_warning);
	} else {
	    setinteger(csb, SESSIONTIMEOUT_WARNING_DEFAULT * ONESEC,
			    &console->sessiontimeout_warning);
	}
	settoken(csb, csb->sense ? GETOBJ(string,1) : NULL,
			&console->sessiontimeout_warning_msg);
	break;

      case PARSER_LINE_ABSOLUTE_TIMEOUT:

	if (nvgen) {
	    nv_write(userconfig->u_absolutetimeout,
                     "%s %d", csb->nv_command,
                     userconfig->u_absolutetimeout/ONEMIN);
            return;
        }

	if (csb->sense)
	    setuser_integer(csb, (GETOBJ(int,1)*ONEMIN),
			    &console->tty_absolutetimeout);
	else
	    setuser_integer(csb, ABSOLUTE_TIMEOUT_DEFAULT * ONEMIN,
			    &console->tty_absolutetimeout);
	break;

      case PARSER_LINE_LOGOUT_WARNING:

	if (nvgen) {
	    nv_write(tty->logoutwarning != LOGOUT_WARNING_DEFAULT * ONESEC,
                     "%s %d", csb->nv_command,
                     tty->logoutwarning/ONESEC);
            return;
        }

	if (csb->sense)
	    setinteger(csb, (GETOBJ(int,1)*ONESEC), &console->logoutwarning);
	else
	    setinteger(csb, LOGOUT_WARNING_DEFAULT * ONESEC,
		       &console->logoutwarning);
	break;

      case PARSER_LINE_FLUSH_ACTIVATE:
	if (nvgen) {
	    nv_write((caps & NO_FLUSH_ACTIVATE), "no %s", csb->nv_command);
	} else {
	    csb->sense = (csb->sense) ? FALSE:TRUE;   /* default on */
	    docaps(csb, NO_FLUSH_ACTIVATE, FIRST_CAP);
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * set_loginstring
 * Set an automatic login sequence for a particular host.
 * Called by configuration routines.
 */
void set_loginstring (parseinfo *csb)
{
    nametype *ptr;
    int errcode;
    char *string = NULL;

    if (csb->nvgen) {
	ptr = (nametype *) nameQ.qhead;
	while (ptr) {
	    nv_write((int)ptr->loginstring, "%s %s \003%s\003",
		     csb->nv_command, ptr->name, ptr->loginstring);
	    ptr = ptr->next;
	}
	return;
    }

    if (csb->sense) {
	banner(csb,&string);
    }
    ptr = (nametype *) name_lookup(GETOBJ(string,2),&errcode,NAM_ANY);
    if (ptr) {
	setstring(&ptr->loginstring, string);
    } else {
	printf("\nUnable to set login string for %s", GETOBJ(string,2));
    }
    free(string);
}

/*
 * set_resumestring
 * Set an automatic sequence to send when a conn to the host is resumed
 * Called by configuration routines.
 */
void set_resumestring (parseinfo *csb)
{
    nametype *ptr;
    int errcode;
    char *string = NULL;

    if (csb->nvgen) {
	ptr = (nametype *) nameQ.qhead;
	while (ptr) {
	    nv_write((int)ptr->resumestring, "%s %s \003%s\003",
		     csb->nv_command, ptr->name, ptr->resumestring);
	    ptr = ptr->next;
	}
	return;
    }

    if (csb->sense) {
	banner(csb,&string);
    }
    ptr = (nametype *) name_lookup(GETOBJ(string,2),&errcode,NAM_ANY);
    if (ptr) {
	setstring(&ptr->resumestring, string);
    } else {
	printf("\nUnable to set resume string for %s", GETOBJ(string,2));
    }
    free(string);
}

/*
 * set_busystring
 * Set an busy message string for a particular host.
 * Called by configuration routines.
 */

void set_busystring (parseinfo *csb)
{
    nametype *ptr;
    int errcode;
    char *string = NULL;

    if (csb->nvgen) {
	nametype *name;

	name = (nametype *) nameQ.qhead;
	while (name) {
	    nv_write((int)name->busystring,
		     "%s %s \003%s\003", csb->nv_command,
		     name->name, name->busystring);
	    name = name->next;
	}
	return;
    }

    if (csb->sense == TRUE) {
	banner(csb,&string);
    }
    ptr = (nametype *) name_lookup(GETOBJ(string,2),&errcode,NAM_ANY);
    if (ptr) {
	setstring(&ptr->busystring,string);
    } else {
	printf("\nUnable to set busy message for %s", GETOBJ(string,2));
    }
    free(string);
}

