/* $Id: command1.c,v 3.3.62.5 1996/08/07 09:03:21 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ui/command1.c,v $
 *------------------------------------------------------------------
 * command1.c  -- EXEC Command Support
 *  
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: command1.c,v $
 * Revision 3.3.62.5  1996/08/07  09:03:21  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.62.4  1996/06/21  17:31:36  swenson
 * CSCdi58991:  Username no longer displayed for vty-async sessions
 * Branch: California_branch
 * try to get username from idb if tty doesn't have one
 *
 * Revision 3.3.62.3  1996/05/28  16:20:51  vandys
 * CSCdi50153:  systat does not display ISDN access users
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/05/09  14:47:48  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.62.1.18.1  1996/04/27  07:37:49  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.62.1.8.1  1996/04/08  02:10:43  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.62.1  1996/03/18  22:28:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.66.1  1996/04/05  03:19:52  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3  1995/11/17  17:47:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:45:52  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:40:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:33:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/07/14  07:03:29  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.3  1995/06/16 04:30:03  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/14  19:23:02  vandys
 * CSCdi35265:  Should use intfc description if no line description
 * Don't reference tty->location directly; pass tt_soc to a
 * function, let it take from either tt_soc or IDB.
 *
 * Revision 2.1  1995/06/07  23:10:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../dev/monitor1.h"
#include "ttysrv.h"
#include "connect.h"
#include "config.h"
#include "file.h"
#include "command.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "../parser/parser_defs_config.h"
#include "../os/clock.h"
#include "../../boot/buff.h"
#include "aaa.h"
#include "../util/random.h"
#include "../h/auth_registry.regh"
#include "interface_private.h"
#include "../if/network.h"
#include "../../boot/cpu.h"

/*
 * exit_command
 * Close connections, possibly with confirmation.
 */

boolean exit_command (boolean askflag)
{
    conntype *currentcon;

    reg_invoke_parse_cli_quit();

    /*
     * prevent possible lockout of other configuring-users
     */
    exit_config_modes(stdio->csb);

    if (askflag && stdio->connhead.qhead != NULL) {
	clearinput(stdio);
	if (!confirm(" (You have open connections) "))
	    return(FALSE);
    }
    while ((currentcon = (conntype *) stdio->connhead.qhead)) {
	printf("\nClosing: ");
	(void) print_host(currentcon);
	printf(" ! ");
	startoutput(stdio);
	conn_close(currentcon);
    }
    logout_banner(stdio);
    return(TRUE);
}

/*
 * validlogicalname
 */

boolean validlogicalname (char *s, tt_soc *t, conntype *c)
{
    conntype *cc;
    ulong i;
    ulong exact;

    if (null(s))
	return(TRUE);			/* nothing to check */
    if (t_number(s)) {
	printf("%% Numeric logical name illegal");
	return(FALSE);
    }
    cc = findlogicalname(t,s,&i);	/* check for duplicity */
    exact = i & 0xFFFF0000;		/* n exact matches */
    if (exact == 0)			/* NO exact match. Thus, unique. */
	return(TRUE);
    if (c == cc)			/* duplicate OK for rename */
	return(TRUE);
    printf("%% Duplicate logical names illegal");
    return(FALSE);
}

/*
 * lognamerr
 */

void lognamerr (int i)
{
    switch (i) {
	case 0: 
	    printf("?Invalid connection name");
	    break;
        default:
	    printf("?Ambiguous connection name");
	    break;
    } 
}


static void show_systat_serial(hwidbtype *hwidb,void *arg)
{
    char *name;
    int wide;

    if ((hwidb->enctype != ET_PPP) ||
	(hwidb->type == IDBTYPE_ASYNC) ||
	(hwidb->type == IDBTYPE_VTY_ASYNC) ||
	(hwidb->type == IDBTYPE_ASYNC_GROUP) ||
	(hwidb->type == IDBTYPE_VACCESS) ||
	!hw_interface_up(hwidb) ||
	is_isdn(hwidb) ||
	is_ddr(hwidb))
	return;


    if (arg)
	wide = 1;
    else
	wide = 0;

    name = "";
    (void)reg_invoke_get_authname(hwidb, &name);
    if (wide) {
	printf("  %10s %s", hwidb->hw_short_namestring, name);
    } else {
	printf("  %10s %9s %20s ", hwidb->hw_short_namestring, name, "Sync PPP");
	print_dhms(hwidb->lastinput);
    }
    printf("\n");
}


/*
 * show_systat
 * Give a quick overview of tty usage and connection status
 */

static const char syshead[] = "    Line     User      Host(s)               Idle Location\n";

void show_systat (parseinfo *csb)
{
    int i, base;
    boolean no_location;
    tt_soc *tx;
    char *str, *user;
    const char *proc_name;
    conntype *dumcon;
    boolean verbose;
    boolean wide = 0;
    sys_timestamp time;

    verbose = (GETOBJ(int,1) != 0);
    automore_enable(syshead);
    for (i = 0; i < ALLlines; i++) {
	tx = MODEMS[i];
	if (!verbose && (!(tx->statbits & HASPROC))) 
	    continue;
	switch (tx->type) {
	  case CTY_LINE: str = "con"; base = 0; break;
	  case AUX_LINE: str = "aux"; base = AuxBase; break;
	  case VTY_LINE: str = "vty"; base = VTYBase; break;	   
	  default: str = "tty"; base = 0; break;
	}
	    
	user = NULL;
	if (tx->statbits & HASPROC) {
	    if (old_access_control) {
	    user = tx->user;
	    } else if (aaa_get_user_p(tx->ustruct, &user) == FALSE) {
		reg_invoke_get_authname(tx->tty_idb, &user);
	    }
	} 
	if (user == NULL)
	    user = " ";

	printf("%c%-3t %s %4t%10s", ((i == stdio->ttynum) ? '*' : ' '),
		i, str, (i - base), user);
	if (tx->statbits & HASPROC) {
	    int j;

	    if (!reg_invoke_xremote_show_systat(tx)) {
		if (tx->connhead.qhead != NULL) { /* Open connections */
		    dumcon = (conntype *) tx->connhead.qhead;
		    no_location = TRUE;
		    while (dumcon) {
			if (!validmem(dumcon))
			    break;
			j = print_host(dumcon);
			j = CONNLEN - j;
			if (j > 0)
			    printf("%*c",j,' ');
			printf(" %-#5TE ", dumcon->idletime);
			if (no_location == TRUE) {
			    if (tx->conntop)
			      (void) print_host(tx->conntop);
			    else
			      printf("%s", get_tty_location(tx));
			    no_location = FALSE;
			}
			printc('\n');
			dumcon = dumcon->link;
			if (!dumcon) break;
			printf("%23c",' ');
		    } /* while dumcon */
		} else { /* No connections */
		    if (tx->arapptr) {          /* ARAP connection */
			printf("ARAP                       ");
			printf("%s\n", get_tty_location(tx));
		    } else
		    if (tx->conntop) {		/* Incoming connection */
			if (tx->statbits & SLIPMODE) {
                            printf("Vty-async interface ");
			}
			else if (tx->capabilities & NOEXEC)
			    printf("incoming            ");
			else
			    printf("idle                ");
			printf(" %-#5TE ", tx->conntop->idletime);
			(void) print_host(tx->conntop);
			printc('\n');
		    } else {			/* No connection */
			if (tx->statbits & SLIPMODE) {
			    printf("Async interface      %-#5TE ",
				   TIMER_SUB_DELTA(tx->dallytimer,
						   tx->tty_sessiontimeout));
			} else if (tx->csb) {
			    /*
			     * Exec sessions have CSB's allocated.
			     */
			    printf("idle                 %-#5TE ",
				   TIMER_SUB_DELTA(tx->dallytimer,
				  ((tx->statbits & CONNECTED) ?
				   tx->tty_sessiontimeout :
				   tx->tty_exectimeout)));
			} else {
			    /*
			     * Dig into scheduler to see what we might
			     * be running, and how long since it last ran.
			     */
			    if (process_exists(tx->tiptop)) {
				if (!process_get_name(tx->tiptop, &proc_name))
				    proc_name = "Unknown";
				printf("%20s", proc_name);
				if (process_get_starttime(tx->tiptop, &time))
				    printf(" %-#5TE ", time);
				else
				    printf("unknown");
			    }
			}			    
			printf("%s\n", get_tty_location(tx));
		    }
		}
	    }
	} else {	/* No process */
	    printf("                    ");
	    print_ticks_in_dhms(0);
	    printf("%s\n", get_tty_location(tx));
	}
    }

    /*
     * Now let any other protocols tack on *their* access users
     */
    reg_invoke_systat();

    idb_for_all_on_hwlist(HWIDBLIST_SERIAL, show_systat_serial, (void *)wide);

    automore_disable();
}

/*
 * logout_banner
 * Clear screen and print a pretty logout banner
 */

void logout_banner (tt_soc *tty)
{
    int i;
    int count, random;

    if ((tty->statbits & NOBANNER) || tty->vacantsuppressed ||
	 outputxoffed(tty))
	return;
    tty->statbits &= ~CARDROP;          /* need to do screen output. */
    count = tty->tty_length ? tty->tty_length + 10 : 62;
    for (i = 0; i < count; i++)		/* clear very long screens */
	ttyprintf(tty, "\n");
    if (tty->vacantstring) {
	ttyprintf(tty,"%r",tty->vacantstring);
	if (tty->conncount & 1)		/* don't move very much -- UC Lib */
	    ttyprintf(tty,"\n\n");
    } else {
	ttyprintf(tty,"%s %s%t is now available",hostname,
		  ((tty->ttynum == 0) ? "con" : "tty"),tty->ttynum);
	ttyprintf(tty,"\n\n\n\n\n\nPress RETURN to get started.\n\n");
	random = (random_gen() % 15);
	for (i = 1; i < random; i++)		  /* save phosphorus */
	    ttyprintf(tty,"\n");
    }
    ttyflush(tty);
}

/*
 * print_config_default
 * Print out configuration file information
 */

static boolean print_config_default (char *type, filetype *confg)
{
    uchar *buffer;

    if (confg->responder && confg->proto) {
 	printf("\n%s file is ", type);
	if (confg->filename)
	    printf("\"%s\"", confg->filename);
	else
	    printf("unknown");
	buffer = string_getnext();
	if (buffer) {
	    reg_invoke_file_source(confg->proto, confg, buffer);
	    printf(", booted via %s", buffer);
	}
	return(TRUE);
    }
    return(FALSE);
}

/*
 * print_first_line
 * Print out the first line of a bootstrap monitor version banner
 */

static void print_first_line (char* cp)
{
    char c;

    if (cp) {
	for (;;) {
	    c = *cp++;
	    if (c == '\0')
		break;
	    if (c == '\r')
		continue;
	    printc(c);
	    if (c == '\n')
		break;
	}
    }
}

/*
 * print_version_string
 * Try to extract version information from the string passed in, and print
 * it.  This is temporary until we can get the banner format formalized.
 *
 * What we want exists right past the "(tm) " text.  If the "(tm) " text
 * doesn't exist, just print the first line.
 */

#define VERSION_MARKER_END "(tm) "

static void print_version_string (char *version_string)
{
    char *version_info;
    char *template = VERSION_MARKER_END;

    version_info = strstr(version_string, template);

    if (version_info)
	print_first_line(version_info + strlen(template));
    else
	print_first_line(version_string);
}

/*
 * host_banner
 * Print a banner describing the local host
 */

void host_banner (void)
{
    char* cp;
    char *bootp;

    printf("%s", version);
    print_text_base_string();

    cp = mon_vstring();
    if (cp != (char*) -1) {
	printf("ROM: ");
	print_first_line(&cp[2]);	/* skip leading "\r\n" */
	cp = mon_vstring_sys_rom();
	if (cp != (char*) -1) {
	    if (strcmp(cp, version)) {
		bootp = platform_get_string(PLATFORM_STRING_BOOTROM_OR_BOOTFLASH);
		if (bootp){
		    printf("%s ", bootp);
		} else {	
		    printf("ROM: ");
		}
		print_version_string(cp);
	    }
	}
    }

    print_uptime();
    if (boot_reason)
	printf("\nSystem restarted by %s", boot_reason);
    if (clock_is_probably_valid()) {
	clock_epoch current_time;
	clock_get_time(&current_time);
	current_time.epoch_secs -= system_uptime_seconds();
	printf(" at %CC", "%C", &current_time);
    }

    if (!print_config_default("System image", &sysconfg))
	printf("\nRunning default software");

    print_config_default("Host configuration", &hostconfg);
    print_config_default("Network configuration", &netconfg);
}

/*
 * escape_command
 * Set the escape character from config file or user-typein.
 * csb->permanent is FALSE if this is user type-in.
 */

void escape_command (parseinfo *csb)
{
    if (csb->nvgen) {
	int escapechar;
	escapechar = MODEMS[(int)csb->line1]->defaults ?
	    (MODEMS[(int)csb->line1]->defaults->u_escapechar) :
	    (MODEMS[(int)csb->line1]->current.u_escapechar);
	if (escapechar != TTY_ESCAPECHAR) {
	    if (escapechar == BREAK_CHAR) {
		nv_write (TRUE, "%s BREAK", csb->nv_command);
	    } else if (escapechar == -1) {
		nv_write (TRUE, "%s NONE", csb->nv_command);
	    } else {
		nv_write(TRUE, "%s %d", csb->nv_command, escapechar);
	    }
	}
	return;
    }

    if (csb->which == TERM_ESCAPE_NONE) {
	GETOBJ(int,1) = -1;
    } else if (csb->which == TERM_ESCAPE_BREAK) {
	GETOBJ(int,1) = BREAK_CHAR;
    } else if (csb->which == TERM_ESCAPE_DEFAULT) {
	GETOBJ(int,1) = TTY_ESCAPECHAR;
    } else {
	/* Strip upper bits */
	GETOBJ(int,1) = GETOBJ(int,1) & 0x7f;

	/* Compatibility--"no escape-char" maps to BREAK */
	GETOBJ(int,1) = csb->sense ? GETOBJ(int,1) : BREAK_CHAR;
    }
    if ((csb->permanent == FALSE) && (GETOBJ(int,1) == BREAK_CHAR)) {
	if (stdio->type == CTY_LINE) {
	    printf("%% <BREAK> not allowed on console terminal");
	    return;
	}
    }
    setuser_integer(csb, GETOBJ(int,1), &console->tty_escapechar);
    if (csb->permanent == FALSE) {
	if (stdio->tty_escapechar == BREAK_CHAR) {
	    printf("<BREAK>");
	} else if (stdio->tty_escapechar == -1) {
	    printf("<NONE>");
	} else {
	    printf("\"%c\"",stdio->tty_escapechar);
	}
	printf(" is the escape character");
    }
}
