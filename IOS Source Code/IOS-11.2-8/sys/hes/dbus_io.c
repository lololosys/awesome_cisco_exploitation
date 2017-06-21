/* $Id: dbus_io.c,v 3.8.10.4 1996/09/04 18:12:37 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/hes/dbus_io.c,v $
 *------------------------------------------------------------------
 * dbus_io.c
 *
 * March 1994, Stig Thormodsrud
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dbus_io.c,v $
 * Revision 3.8.10.4  1996/09/04  18:12:37  kmitchel
 * CSCdi66418:  IOS overreacts to DBUS io port-whine
 * Branch: California_branch
 *
 * Revision 3.8.10.3  1996/07/09  21:14:51  getchell
 * Modify read bus exception handler and DCL interrrupt handler to deal
 * with dbus accesses to non-existent card.
 * CSCdi58555:  bus exception caused by dbus access to non-existant card
 * Branch: California_branch
 *
 * Revision 3.8.10.2  1996/06/03  17:37:32  getchell
 * modify if-con to use new process scheduling primitives
 * CSCdi57383:  dbus polling excessive when if-con open
 * Branch: California_branch
 *
 * Revision 3.8.10.1  1996/05/21  09:47:38  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8  1996/02/22  14:33:11  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/01  06:02:18  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  16:28:33  banderse
 * CSCdi47646:  Add alternative quit string to CIP/VIP console
 *
 * Revision 3.5  1996/01/29  07:27:37  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/18  22:46:48  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/17  18:27:19  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:15:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:59:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/08  16:46:32  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.2  1995/06/09 13:04:30  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:38:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "subsys.h"
#include "ttysrv.h"
#include "dgbus.h"
#include "dbus.h"
#include "dbus_io.h"
#include "ucode.h" 
#include "../dev/flash_dvr_spec.h"
#include "interface_private.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/slot.h"
#include "login.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#define DBUS_CONSOLE_POLL_INTERVAL  100   /* 100 ms. */

/*
 * Alternative quit string for IP consoles
 */
#define QUIT_STRING "if-quit"
#define QUIT_STRLEN 8                       /* one extra byte for \r */
static char quit_string[QUIT_STRLEN+1];

/* 
 * forward declarations
 */
static process dbus_io_console_daemon(void);

/*
 * dbus_io_console
 */
void dbus_io_console (parseinfo *csb)
{
    pid_t pid;
    char string[DBUS_IO_MAXLEN];
    int slot;
    int rc;
    dcbtype dcb;
    int tty_device;
 
    if (GETOBJ(int,2) == 1) {
	slot = GETOBJ(int,1);
    } else {
	printf("Enter interface slot to connect to: ");
	rdtty(string, DBUS_IO_MAXLEN);
	if (string[0] == '\0')
	    return;
	slot = parse_unsigned(string, &rc);
	if (rc != TRUE) {
	    printf("\n%%Error: invalid input [%s]\n", string);
	    return;
	}
    }
    if (slot < 0 || slot > MAX_SLOTS) {
	printf("\n%%Error: invalid slot [%d]\n", slot);
	return;
    }
    if (!slots[slot].io_regs) {
	printf("\n%%Slot %d does not support DBUS I/O registers.\n", slot);
	return;
    }

    dcb.dbus = pslot_to_dbus(slot);
    if (!touch_device(dcb.dbus)) {
	printf("\n%%Dbus access for slot %d failed", slot);
	return;
    }

    /*
     * Connect to console or debug register
     */
    switch (GETOBJ(int,3)) {
    case DBUS_IO_CONSOLE:
	string[0] = 'c';
	break;
    case DBUS_IO_DEBUG:
	string[0] = 'd';
	break;
    default:
	printf("\nConsole or Debug [C]: ");
	rdtty(string, DBUS_IO_MAXLEN);
	if (string[0] == '\0')  
	    string[0] = 'c';    /* default */
    }
    switch (string[0]) {
    case 'c':
    case 'C':
	tty_device = DBUS_IO_TTY_CONSOLE;
	dcb.reg = DBUS_IO_CONSOLE;
	break;
    case 'd':
    case 'D':
	tty_device = DBUS_IO_TTY_DEBUG;
	dcb.reg = DBUS_IO_DEBUG;
	break;
    default:
	printf("\n%%Invalid response");
	return;
    }

    if (slots[slot].dbus_io_tty[tty_device] != DBUS_IO_NO_TTY) {
	printf("\nSlot %d %s in use by line %t", 
	       slot, 
	       tty_device == DBUS_IO_TTY_CONSOLE ? "CONSOLE" : "DEBUG", 
	       slots[slot].dbus_io_tty[tty_device]);
	return;
    }

    slots[slot].dbus_io_tty[tty_device] = tt2number(stdio);
    stdio->statbits |= CARDCONSOLE;
    dcb.tty = stdio;
    dcb.card_type = dbus_controller_type(dcb.dbus);
    dcb.alt_quit_count = 0;

    dcb.daemon_done = create_watched_boolean("IO_Daemon_Done", 0);

    pid = process_create(dbus_io_console_daemon, "Dbus I/O", NORMAL_STACK,
			 PRIO_NORMAL);

    if (pid != NO_PROCESS ) {
	process_set_arg_ptr(pid, &dcb);
	process_set_ttynum(pid,  tt2number(dcb.tty));
	process_watch_boolean(dcb.daemon_done, ENABLE, ONE_SHOT);
	process_wait_for_event();
    }

    /*
     * dbus_io_console_daemon has died and we're back in the
     * exec process.  Marked the port as unused and return;
     */
    delete_watched_boolean(&dcb.daemon_done);
    slots[slot].dbus_io_tty[tty_device] = DBUS_IO_NO_TTY;
    stdio->statbits &= ~CARDCONSOLE;
}

/*
 * dbus_if_need_local_echo : 
 * Return FALSE if the IP card (Checker, Viper) does echoing.
 * The CIP card relies on the RP to echo the characters, so return
 * TRUE.
 */
static inline boolean dbus_if_need_local_echo (dcbtype *dcb)
{
    if (dcb->card_type == FCI_RVIP_CONTROLLER ||
	dcb->card_type == FCI_SVIP_CONTROLLER) {
	return(FALSE);
    }
    return(TRUE);
}

static void dbus_io_disconnect (
   dcbtype *dcb,
   sys_timestamp connected,
   boolean announce)
{
    int slot;

    slot = dbus_to_pslot(dcb->dbus);
    if (announce) {
	printf("\n\nDisconnecting from slot %d %s after ", slot,
	       dcb->reg == DBUS_IO_CONSOLE ? "CONSOLE" : "DEBUG");
	print_dhms(connected);
	printf("\n");
    }
    flush();
    /* Only change the bits if the board is there! */
    if ((slots[slot].flags & BOARD_NONEXIST) == 0)
	dbus_io_status_bit(slot, 
			   (dcb->reg == DBUS_IO_CONSOLE) ?
			   DBUS_IO_CONSOLE_ACTIVE : DBUS_IO_DEBUG_ACTIVE,
			   DBUS_IO_BIT_OFF);
    process_set_boolean(dcb->daemon_done, TRUE);
    process_kill(THIS_PROCESS); /* suicide */
    
}

static boolean dbus_io_check_for_exit (dcbtype *dcb) 
{
    int slot;

    /* 
     * Leave if connection has been dropped or the
     * board has been removed
     */
    if (modemcheck(dcb->tty)) 
	return TRUE;

    slot = dbus_to_pslot(dcb->dbus);
    if (slots[slot].flags & BOARD_NONEXIST)
	return TRUE;

    return FALSE;
}


/*
 * dbus_io_getstring
 * Get a string of keystrokes from the tty to give to the 
 * DBUS console of the interface card
 */
static inline int dbus_io_getstring (dcbtype *dcb, char *string, int max,
			      boolean *quit_session, boolean debug)
{
    char ch;
    int i = 0;

    while (i<max) {
	/*
	 * Get a character from tty
	 */
	ch = getc(dcb->tty);
	if (ch <= 0) {
	    return(i);
	}

	/*
	 * Search for quit character (three ^c)
	 */
	if (ch == DBUS_IO_QUIT_CHAR) {
	    dcb->quit_count += 1;
	    if (dcb->quit_count >= DBUS_IO_QUIT_THRESHOLD) {
		if (confirm("\nTerminate DBUS IO session? ")) {
		    *quit_session = TRUE;
		    return(0);
		} else {
		    printf("\n");
		    flush();
		    dcb->quit_count = 0;
		}
	    }
	} else {
	    dcb->quit_count = 0;
	}

	/*
	 * If the card needs local echo from the RP
	 * process the character IE CIP card
	 */
	if (!debug && dbus_if_need_local_echo(dcb)) {
	    /*
	     * Handle backspaces
	     */
	    if (ch == '\b') {
		if (i == 0)
		    return(0);
		i -= 1;
		string[i] = 0;
		printc(ch);
		printc(' ');
	    }
	    printc(ch);
	}
	string[i++] = ch;

	/*
	 * Search for alternative quit string
	 */
	if (ch == quit_string[dcb->alt_quit_count]) {
	    dcb->alt_quit_count += 1;
	    if (dcb->alt_quit_count >= QUIT_STRLEN) {
		*quit_session = TRUE;
		return(i);
	    } 
	} else {
	    dcb->alt_quit_count = 0;
	}
    }
    return(i);
}

/*
 * dbus_io_console_deamon
 */
static process dbus_io_console_daemon (void)
{
    char ch, string[DBUS_IO_MAXLEN]; 
    int  i, count, slot;
    sys_timestamp timer;
    sys_timestamp connected;
    boolean qflag, debug;
    dcbtype *dcb;
    dgtype  *dbus;
    uchar   dbus_data;

    GET_TIMESTAMP(connected);

    process_get_arg_ptr((void **)&dcb);

    slot = dbus_to_pslot(dcb->dbus);
    printf("\nEntering %s for %s %d", 
	   dcb->reg == DBUS_IO_CONSOLE ? "CONSOLE" : "DEBUG", 
	   slots[slot].card_name, slot); 
    printf("\nType \"^C^C^C\" or \"%s\" to end this session\n\n", QUIT_STRING);
    flush();

    /* notify controller of session  */
    dbus_io_status_bit(slot, 
		       (dcb->reg == DBUS_IO_CONSOLE) ?
		       DBUS_IO_CONSOLE_ACTIVE : DBUS_IO_DEBUG_ACTIVE,
		       DBUS_IO_BIT_ON);
    
    while (1) {

	/*
	 * check for dropped connection
	 */
	if (dbus_io_check_for_exit(dcb)) {
	    /*
	     * Disconnect the virtual console. Note, this will
	     * not return, it will commit process suicide.
	     */
	    dbus_io_disconnect(dcb, connected, FALSE);
	}

	dbus = dcb->dbus;

	/* 
	 * Make sure card is operational - could have been reset while
	 * process was sleeping
	 */
	dbus_data = dbus_prot_rd(&dbus->diag_status);
	if (dbus_data & DBUS_SW_READY) {

		    GET_TIMESTAMP(timer);

	    if (dbus_data & DBUS_IO_STATUS_SET) {
		dbus_data = dbus_io_status_reg_read(slot);
		/* Make sure interface hasn't disappeared */
		if ((dbus_data != DBUS_REG_TIMEOUT) && 
		    (dbus_data & dcb->reg)) {
		    /*
		     * 1st look for data on the dbus i/o port
		     */
		    dbus_data = dbus_io_reg_read(slot, dcb->reg);
		    while ((dbus_data != DBUS_REG_TIMEOUT) &&
			   ((ch = (char)dbus_data) != DBUS_IO_EMPTY)) {
			/* Dont't print garbage characters */
			if (ch > 0)
			    printc(ch);
			if (ELAPSED_TIME(timer) > ONESEC)
			    break;
			dbus_data = dbus_io_reg_read(slot, dcb->reg);
		    }
		    flush();
		}
	    }

	    /*
	     * next look for data from the user
	     */
	    while (inputpending(dcb->tty)) {
		qflag = FALSE;
		debug = (dcb->reg == DBUS_IO_DEBUG);
		count = dbus_io_getstring(dcb, string, DBUS_IO_MAXLEN, 
					  &qflag, debug);
		for (i = 0; i < count; i++) {
		    dbus_io_reg_write(slot, dcb->reg, string[i]);
		}
		if (qflag) { 
		    /*
		     * Disconnect the virtual console. Note, this will
		     * not return, it will commit process suicide.
		     */
		    dbus_io_disconnect(dcb, connected, TRUE);
		}
		if (ELAPSED_TIME(timer) > ONESEC)
		    break;
	    }

	    flush();
	}

	process_sleep_for(DBUS_CONSOLE_POLL_INTERVAL);

    }
}




/*
 * Parse chains for if-console
 */
#define ALTERNATE       NONE
#include "exec_if_console.h"
LINK_POINT(if_console, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for channel
 */
static parser_extension_request dbus_io_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(if_console) },
    { PARSE_LIST_END, NULL }
};

/*
 * dbus_io_init
 */
static void dbus_io_init (subsystype *subsys) 
{
    static boolean dbus_io_parser_initialized = FALSE;
    
    if (!dbus_io_parser_initialized) {
	parser_add_command_list(dbus_io_chain_init_table, "dbus_io");
	dbus_io_parser_initialized = TRUE;
    }
    strcpy(quit_string, QUIT_STRING);
    strcat(quit_string, "\r");
}


/*
 * dbus_io subsystem header
 */

#define DBUS_IO_MAJVERSION  1
#define DBUS_IO_MINVERSION  0
#define DBUS_IO_EDITVERSION 1

SUBSYS_HEADER(dbus_io, DBUS_IO_MAJVERSION, DBUS_IO_MINVERSION, DBUS_IO_EDITVERSION,
	      dbus_io_init, SUBSYS_CLASS_LIBRARY,
	      NULL,
	      NULL);
