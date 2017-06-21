/* $Id: setup.c,v 3.8.4.10 1996/08/28 13:18:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/ui/setup.c,v $
 *------------------------------------------------------------------
 * setup.c  -- simple configuration, generic routines
 *  
 * May 1990, David Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: setup.c,v $
 * Revision 3.8.4.10  1996/08/28  13:18:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.4.9  1996/08/13  13:29:43  widmer
 * CSCdi64261:  setup dialog should purge all input before initial prompt
 * Branch: California_branch
 * Require user input to enter setup instead of defaulting to yes
 *
 * Revision 3.8.4.8  1996/06/28  23:31:18  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.8.4.7  1996/05/21  10:07:51  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.4.6  1996/04/29  20:42:41  jquan
 * CSCdi50033:  Crash after setup dialog intermitnently
 * Branch: California_branch. If autoinstall_over_serial process is
 * running, kill it & continue setup dialog.
 *
 * Revision 3.8.4.5  1996/04/27  06:29:24  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.8.4.4  1996/04/24  05:55:54  jquan
 * CSCdi50033:  Crash after setup dialog intermitnently
 * Branch: California_branch. Remove the fix for now to solve the VIP
 * modularity problem.
 *
 * Revision 3.8.4.3  1996/04/23  01:10:48  jquan
 * CSCdi50033:  Crash after setup dialog intermitnently
 * Branch: California_branch. If autoinstall_over_serial process is
 * running, kill it and continue the setup dialog.
 *
 * Revision 3.7.8.3  1996/04/25  23:52:48  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.8.2  1996/04/16  23:51:01  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.8.1  1996/02/23  20:59:37  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.8.4.2  1996/04/15  15:00:46  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.8.4.1  1996/03/18  22:29:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.3  1996/03/16  07:46:01  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.12.2  1996/03/14  01:36:26  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.7.12.1  1996/03/07  10:58:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8  1996/03/09  22:58:12  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.7  1996/01/04  23:41:03  thille
 * CSCdi46461:  Getting Started Guide is toast, dont refer user to it.
 * Rip out string referring user to obsolete manual.
 *
 * Revision 3.6  1995/12/17  18:40:22  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/14  23:24:25  widmer
 * CSCdi45614:  Need vsprintf()
 *
 * Revision 3.4  1995/11/28  03:59:10  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.3  1995/11/17  17:49:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:46:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:41:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:33:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/26  13:36:32  thille
 * CSCdi36960:  setup messages overwrite previous lines
 * Fix errors made by folks who apparently weren't aware that for some
 * reason we prepend \n on the front of lines of output instead of
 * appending it to the end.
 *
 * Revision 2.4  1995/07/20  22:19:51  pilee
 * CSCdi36925:  Spurious memory access made at 0x605D8228  (strcpy+0x0)
 *
 * Revision 2.3  1995/06/16  22:34:14  shankar
 * CSCdi35485:  autoinstall over Frame Relay breaks in 103-3.3
 *
 * Revision 2.2  1995/06/09  13:22:20  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:11:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <stdarg.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "logger.h"
#include "setup.h"
#include "ttysrv.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "config.h"
#include "login.h"
#include "../os/nv.h"
#include "../if/network.h"
#include "../util/md5_crypt.h"
#include "file.h"
#include "../parser/parser_actions.h"
#include "../snmp/config_history.h"
#include "../wan/dialer_registry.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../tcp/http.h"

static int setup_pid = 0;	/* Process ID of setup process */
static boolean resolve_all_protocols_running = FALSE;


/*
 * Forward declarations
 */
static void setup_cleanup(void);
static void setup_global(setup_struct *);
static void setup_interfaces(setup_struct *);
static void ip_setup_free_all_int_history(setup_struct *sp);

const char setup_iphelp[] = 
"\n%% Enter an Internet address of the form 'X.X.X.X', where each
%% letter corresponds to a decimal number between 0 and 255.\n";

/*
 *  Design Philosophy:
 *
 * The setup command is not a replacement for the configure command.  It is
 * intended for the initial configuration of a new system and for the
 * configuration of simple demo systems by inexperienced customers.
 * Detailed configuration must still be done with the configure command.
 *
 * The setup command is dialog, not menu, driven.  You cannot skip
 * questions.  To prevent tedium on the customer's part, the number of
 * interactions must be kept short.  Each question must be important.
 * Defaults should be used wherever possible.  We are not going to try to
 * do everything.
 * 
 * Always allow the customer to override defaults.  This prevents us from
 * screwing up active networks and allows the customer to use the setup
 * command as a limited alternative to the configure command.
 * 
 * This command interface is documented in a hardcopy document.  Please do
 * not alter the setup command without also altering the supporting
 * documentation.
 */

/*
 * setup_string
 * Read a string from the terminal,
 * Pressing RETURN would cause the default to be accepted
 */

char *setup_string (char *buffer, const char *prompt, char *defstring)
{
    char *cp;

    while (!(stdio->statbits & CONTROLC)) {
	if (null(defstring)) {
	    printf("\n%s: ", prompt);
	} else {
	    printf("\n%s [%s]: ", prompt, defstring);
	}
	if (!rdtty(buffer, MAXBUF)) {
	    continue;
	}
	cp = deblank(buffer);
	if (null(cp)) {
	    if (!null(defstring)) {
		return(defstring);
	    }
	    printf("\n%% No defaulting allowed");
	} else {
	    if (*cp != '?'){
		return(cp);
	    }
	    printf("Type a string, then press RETURN");
	}
    }
    return(NULL);
}


/*
 * setup_common
 * Ask the questions common to all configurations.
 * Last configuration command we write must have a trailing "!".
 * Return a boolean indicating that SNMP was asked for.  This indicates
 * that we must have some IP addresses somewhere in the configuration.
 */

static void setup_common (setup_struct *sp)
{
    char buffer[MAXBUF], tmp_secret[CRYPT_SECRET_LEN], *cp;
    int encrypt, i;

    if (system_loading) {
	return;
    }
    printf("\nConfiguring global parameters:\n");

    /*
     * The host name
     */
    cp = setup_string(buffer, "  Enter host name", hostname);
    setup_printf(sp, "\nhostname %s", cp);

    /*
     * The enable secret
     */
    printf("\n\nThe enable secret is a one-way cryptographic secret used");
    printf("\ninstead of the enable password when it exists.\n");
    cp = setup_string(buffer, "  Enter enable secret",
		      enable_secret[PRIV_MAX].pw ?
		      "<Use current secret>" : NULL);
    if (cp == buffer) {
	md5_crypt(buffer, NULL, tmp_secret);
	memset(buffer, 0, sizeof(buffer));
    } else {
        if (enable_secret[PRIV_MAX].pw)
          strcpy(tmp_secret, enable_secret[PRIV_MAX].pw);
        else /* enable_secret[PRIV_MAX].pw has never been initted before, 
              * do not strcpy */
          memset(tmp_secret, 0, sizeof(tmp_secret));
    }
    setup_printf(sp, "\nenable secret %d %s", ENCRYPT_MD5, tmp_secret);

    /*
     * The privileged password
     */
    printf("\n\nThe enable password is used when there is no enable secret");
    printf("\nand when using older software and some boot images.\n");
    for (i = 2; i; i--) {
	cp = setup_string(buffer, "  Enter enable password",
			  enable_password[PRIV_MAX].pw);
	if ((i > 1) && tmp_secret &&
	    md5_crypt_compare(tmp_secret, cp, cp == buffer ? ENCRYPT_NONE :
			      enable_password[PRIV_MAX].encrypt_type)) {
	    printf("\n%% Please choose a password that is different from the "
		   "enable secret");
	} else {
	    if (password_encryption_enable) {
		encrypt = enable_password[PRIV_MAX].encrypt_type;
		if (cp != enable_password[PRIV_MAX].pw) {
		    encrypt = ENCRYPT_NONE;
		}
		setup_printf(sp, "\nenable password %d %s", encrypt, cp);
		setup_printf(sp, "\nservice password-encryption");
	    } else {
		setup_printf(sp, "\nenable password %s", cp);
	    }
	    break;
	}
    }

    /*
     * The virtual terminal password
     */
    if (MODEMS[VTYBase] != NULL) {
	cp = setup_string(buffer, "  Enter virtual terminal password",
		      MODEMS[VTYBase]->password ?
		      MODEMS[VTYBase]->password->pw : NULL);
	if (password_encryption_enable) {
	    encrypt = enable_password[PRIV_MAX].encrypt_type;
	    if (MODEMS[VTYBase]->password &&
	    	(cp != MODEMS[VTYBase]->password->pw)) {
	        encrypt = ENCRYPT_NONE;
	    }
	    setup_printf(sp, "\nline vty 0 %d\npassword %d %s",
		     nvtylines-1, encrypt, cp);
	} else {
	    setup_printf(sp, "\nline vty 0 %d\npassword %s", nvtylines-1, cp);
	}
    }

    /*
     * Call the registered functions
     */
    reg_invoke_setup_common(sp);
    setup_printf(sp, "\n!");
}

/*
 * resolve_all_protocols
 * Process to ask a human for our protocol addresses.  If you modify this
 * routine, please note that all called subroutines must gracefully handle
 * the case of a carrier drop on a line.  These are ignored (for ease of
 * programming) until the end of the routine.  There is a final test for
 * carrier before anything is comitted to the system.
 */

static const char setup_headstr[] =
"\r         --- System Configuration Dialog ---

At any point you may enter a question mark '?' for help.
Use ctrl-c to abort configuration dialog at any prompt.
Default settings are in square brackets '[]'.\r\r";

static const char setup_tailstr[] =
"\rUse the enabled mode 'configure' command to modify this configuration.\r\r";

void resolve_all_protocols (void)
{
    char *cmdstring, *cp;
    setup_struct ss;
    int index;
    hwidbtype *idb;

    /* 
     * Turn on flag to indicate that the interactive setup routine is
     * running.  Flag is turned off in setup_cleanup().
     */
    resolve_all_protocols_running = TRUE;

    memset(&ss, 0, sizeof(setup_struct));

    /*
     * to save redudnant code, the below relys on the caller of this
     * routine to turn off the ctrl-c checking.
     */
    stdio->statbits |= CTRLC_ENBL;		/* turn  ctrl-c catching on */
    stdio->statbits &= ~CONTROLC;		/* clear ctrl-c variable */

    /*
     * Always print introductory header.
     * If system is running, warn user that this is a long dialog
     */
    printf(setup_headstr);
    if (systeminit_flag) {
	if (!yes_or_no("Continue with configuration dialog", TRUE, FALSE)) {
	    setup_cleanup();
	    return;
	}
    } else {
	if (!yes_or_no("Would you like to enter the "
		       "initial configuration dialog", TRUE, TRUE)) {
	    FOR_ALL_HWIDBS(idb) {
	    	if (idb && idb->serial_autoinstall) {
		    if (yes_or_no("\nWould you like to terminate autoinstall",
				   TRUE, TRUE)) {
			reg_invoke_autoinstall_reset_serial_encap();
	    		reg_invoke_autoinstall_terminate();
			reg_invoke_autoinstall_complete(idb->enctype, idb);
			break;
		    } else {
			if (stdio->statbits & CONTROLC) {
    			    if (reg_invoke_autoinstall_process_running()) {
        			reg_invoke_autoinstall_reset_serial_encap();
        			reg_invoke_autoinstall_terminate();
    			    }
			    setup_cleanup();
			    return;
			}
			printf("\n\nPlease Wait. Autoinstall being attempted over %s ",
						idb->hw_namestring);
			flush();
			while (!(stdio->statbits & CONTROLC) &&
					idb->serial_autoinstall) {
			    process_sleep_for(ONESEC);
			    printf("!");
			    flush();
			}
		    }
		}
	    }
	    /* dont write nv_ram here due to possible diagnostic mode setup */
	    setup_cleanup();
	    return;
	}
    }

    if(reg_invoke_autoinstall_process_running()) {
        reg_invoke_autoinstall_reset_serial_encap();
        reg_invoke_autoinstall_terminate();

        FOR_ALL_HWIDBS(idb) {
            if (idb->serial_autoinstall)
                reg_invoke_autoinstall_complete(idb->enctype, idb);
        }
    }
 
    if (yes_or_no("\nFirst, would you like to see the "
		  "current interface summary",
		  TRUE, TRUE)) {
	printf("\n");
	reg_invoke_showproto_brief(NULL);
    }
    printf("\n");

    /*
     * Initialize
     */
    if ((cmdstring = malloc(config_bytes)) == NULL) {
	printf(nomemory);
	setup_cleanup();
	return;
    }
    ss.cmds = cmdstring;
    ss.idb = NULL;
    ss.defnetwork = 1;
    ss.run_ip = TRUE;
    ss.run_ip_snmp = FALSE;
    ss.ip_history_head = NULL;
    ss.http_state = NULL;
    ss.http_request = HTTP_REQUEST_UNKNOWN;
    for (index = 0; index < MAXINTERFACES; index++) {
	ss.addrlist[index] = (ipaddrtype) 0L;
	ss.masklist[index] = (ipaddrtype) 0L;
    }
    ss.args[LINK_MOP_CONSOLE] = FALSE;

    /*
     * Initialize functions
     */
    reg_invoke_setup_init(&ss);

    /*
     * Global setup
     */
    setup_global(&ss);

    /*
     * Interface setup
     */
    setup_interfaces(&ss);

    /*
     * Compute more global commands and cleanup.  NO QUESTIONS HERE!!
     */
    reg_invoke_setup_end(&ss);
    ip_setup_free_all_int_history(&ss);
    
    setup_printf(&ss, "\n!\nend\n");

    /*
     * Dead line or aborted setup?  We're outta here.
     */
    if (stdio->statbits & (CONTROLC+CARDROP+IDLETIMEOUT+HANGUP)) {
	if (stdio->statbits & CONTROLC) {
	    printf("\n\nConfiguration aborted, no changes made.\n");
	}
	free(cmdstring);
	setup_cleanup();
	return;
    }

    /*
     * Show user the configuration command script we constructed.
     * Get her used to the look of the real commands.
     */
    automore_enable(NULL);
    printf("\n\nThe following configuration command script was created:\n");
    cp = cmdstring;
    while (*cp != '\0') {
	printc(*cp++);
    }
    automore_disable();
    if (yes_or_no("Use this configuration", FALSE, FALSE)) {
	/* this gets two history events, one for changing running and
	 * one for saving to startup
	 */
	reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
					CONFIG_MEDIUM_COMMANDSOURCE, 
					CONFIG_MEDIUM_RUNNING, 
					0, NULL, NULL);
	parse_configure(cmdstring, TRUE, RES_MANUAL, PRIV_ROOT);
	reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
					CONFIG_MEDIUM_RUNNING, 
					CONFIG_MEDIUM_STARTUP, 
					0, NULL, NULL);
	(void) nv_current_to_nv(TRUE);
	process_sleep_for(ONESEC*2);	/* allow routing tables to build */
	printf(setup_tailstr);
    }
    free(cmdstring);
    setup_cleanup();
}

/*
 * Macro to check if interface has proper addresses to use for IP
 */
#define IDB_IP_BAD_ADDR(idb) \
    (idb->ip_enabled && !idb->ip_address && !idb->ip_unnumbered)

/*
 * setup_running
 * This function is used to determine if the interactive resolver is
 * running.
 */

boolean setup_running (void)
{
    return resolve_all_protocols_running; 
}

static void setup_cleanup (void)
{
    idbtype *idb;

    /* Shut down all questionable interfaces */
    FOR_ALL_SWIDBS(idb) {
	if (IDB_IP_BAD_ADDR(idb) &&
	    !reg_invoke_dialer_huntgroup_member(idb->hwptr)) {
	    idb->ip_enabled = FALSE;
	    shutdown_going_down(idb->hwptr);
	    continue;
	}
	if ((idb->hwptr->status & IDB_TR) && !idb->hwptr->tr_ring_speed)
	    shutdown_going_down(idb->hwptr);
    }
    stdio->statbits &= ~CTRLC_ENBL;		 /* turn off ctrl-c checking */
    stdio->statbits &= ~CONTROLC;		    /* reset tty setup abort */
    if (!nv_valid) {		/* then service config was default, stop now */
	netconfig_enable = FALSE;
    }

    resolve_all_protocols_running = FALSE;
}

/*
 * setup_abort
 * We received a configuration from the network, abort the current setup
 * process.
 */

void setup_abort (void)
{
    setup_cleanup();
    if (setup_pid > 0) {
	if (hostconfg.filename &&
	    hostconfg.ps.tuba.respondent.ip_addr) {
	    printf("\n\nNetbooted %s from %i, aborting setup.\n",
		   hostconfg.filename,
		   hostconfg.ps.tuba.respondent.ip_addr);
	    errmsg(&msgsym(OUTSETUP, SYS), hostconfg.filename);
	}
	process_kill(setup_pid);
    }
    setup_pid = 0;
}

/*
 * boottime_resolver
 * Ask questions the first time this system is run.
 */

static forktype boottime_resolver (void)
{
    ulong savedtimeout;
    idbtype *idb;

    resolve_all_protocols_running = FALSE;
    /*
     * If we have interfaces without IP addressing and we have valid
     * NVRAM, then this implies that we have some new interfaces
     * in the system.  We disable IP on them and place them in a
     * shutdown state.  Warnings are given.  We do NOT go into setup mode
     * and annoy the hell out of an experienced cisco user.
     */
    if (nv_valid || usable_interfaces()) {
	FOR_ALL_SWIDBS(idb) {
	if (IDB_IP_BAD_ADDR(idb) &&
		!reg_invoke_dialer_huntgroup_member(idb->hwptr)) {
		idb->ip_enabled = FALSE;
		shutdown_going_down(idb->hwptr);
		if (!system_loading)
		    printf("\nSETUP: new interface %s placed "
			   "in \"shutdown\" state",
			   idb->namestring);
		continue;
	    }
	    if ((idb->hwptr->status & IDB_TR) && !idb->hwptr->tr_ring_speed)
		shutdown_going_down(idb->hwptr);
	}
	reg_invoke_autoinstall_terminate();
	reg_invoke_autoaddr_okay();
	setup_pid = 0;
	process_kill(THIS_PROCESS);
    }

    /*
     * No valid NVRAM.  Go into setup dialog.
     * Disable input timeouts.
     */
    savedtimeout = stdio->tty_exectimeout;
    stdio->tty_exectimeout = 0;
    do {
	(void) resolve_all_protocols();
    } while (!reg_invoke_autoaddr_okay() &&
	     !(stdio->statbits & CONTROLC));
    stdio->tty_exectimeout = savedtimeout;
    setup_pid = 0;
    process_kill(THIS_PROCESS);
}

/*
 * interface_protocol
 * Ask if we are running something on this interface
 */

boolean interface_protocol (const char *name, boolean dflt)
{
    char buffer[MAXBUF];

    (void) sprintf(buffer, "  Configure %s on this interface", name);
    return(yes_or_no(buffer, dflt, TRUE));
}

/*
 * start_setup - Start a setup process running
 */

void start_setup (void)
{
    if (!setup_pid) {
	setup_pid = cfork(boottime_resolver, 0L, 0, "Setup", startup_ttynum);
    }
}



/*
 * system_protocol
 * Set up routing for a level 3 protocol
 */

boolean system_protocol (setup_struct *sp, const char *name, boolean defflag)
{
    char buffer[MAXBUF];
    int answer;

    (void) sprintf(buffer, "  Configure %s", name);
    answer = yes_or_no(buffer, defflag, TRUE);
    (void) sprintf(buffer, "%s", name);
    lowercase(buffer);
    if (answer) {
	setup_printf(sp, "\n%s routing", buffer);
    } else {
	setup_printf(sp, "\nno %s routing", buffer);
    }
    return(answer);
}


/*
 * setup_global - Global setup functions
 */
static void setup_global (setup_struct *sp)
{
    setup_common(sp);
    reg_invoke_setup_global(sp);
    reg_invoke_setup_global2(sp);
}


/*
 * setup_interfaces - Interface setup functions
 */
static void setup_interfaces (setup_struct *sp)
{
    hwidbtype *hwidb;

    if (!(stdio->statbits & CONTROLC)) {
	printf("\n\nConfiguring interface parameters%s",
	       system_loading ? " for netbooting:" : ":");
    }

    FOR_ALL_HWIDBS(hwidb) {
	if (stdio->statbits & CONTROLC) {
	    break;
	}

	if (hwidb->status & IDB_DELETED) {
	    continue;
	}

	sp->idb = hwidb->firstsw;
	sp->args[LINK_MOP_CONSOLE] = FALSE;
	/*
	 * Don't disturb any "created" interface configruations, if they exist.
	 */
	if (hwidb->status & IDB_DONTSETUP) {
	    continue;
	}
	setup_printf(sp, "\n!\ninterface %s", hwidb->hw_namestring);
	printf("\n\nConfiguring interface %s:", hwidb->hw_namestring);

	/*
	 * Is interface in use?
	 */
	if (!yes_or_no("  Is this interface in use",
		       (hwidb->state != IDBS_ADMINDOWN), TRUE)) {
	    setup_printf(sp, "\nshutdown\nno ip address");
	} else {
	    if (hwidb->state == IDBS_ADMINDOWN) {
		setup_printf(sp, "\nno shutdown");
	    }

	    reg_invoke_setup_int(sp);
	    reg_invoke_setup_int2(sp);
	    sp->defnetwork++;
	}
	reg_invoke_setup_hub(sp);
    }
    sp->args[LINK_MOP_CONSOLE] = FALSE;
    if (!(stdio->statbits & CONTROLC)) {
	reg_invoke_setup_ephemeral_int(sp);
    }
}


static void ip_setup_free_all_int_history (setup_struct *sp)
{
    struct setup_ip_history_ *sih, *next;

    sih = sp->ip_history_head;
    while (sih) {
        next = sih->next;
        free(sih);
        sih = next;
    }
}

/*
 * setup_printf
 *
 * Perform error checking and write parser commands to setup buffer
 */
int setup_printf (setup_struct *sp, const char *format, ...)
{
    va_list ap;
    int count = 0;

    if (sp &&
	sp->cmds &&
	(sp->http_request != HTTP_REQUEST_GET) &&
	!(stdio->statbits & (CONTROLC+CARDROP+IDLETIMEOUT+HANGUP))) {

	va_start(ap, format);
	count = vsprintf(sp->cmds, format, ap);
	va_end(ap);
	sp->cmds += count;
    }
    return(count);
}
