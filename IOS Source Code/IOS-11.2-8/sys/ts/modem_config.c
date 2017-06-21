/* $Id: modem_config.c,v 3.15.10.8 1996/09/12 17:02:22 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/ts/modem_config.c,v $
 *------------------------------------------------------------------
 * modem_config.c --  Automatic configuration of modems
 *
 * September 1995, Tim Kolar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_config.c,v $
 * Revision 3.15.10.8  1996/09/12  17:02:22  tkolar
 * CSCdi66675:  modem autoconfigure does not send out miscellaneous
 * configuration
 * Branch: California_branch
 * Add an overlooked attribute to the default config string
 *
 * Revision 3.15.10.7  1996/08/08  17:56:34  sjackson
 * CSCdi65288:  Modem autoconfig should always redetect and attempt to
 * lock speed
 * Branch: California_branch
 * Modem locking speed is now configureable which prevents changing of the
 * desired line speed. Configuration is also attempted after every hangup.
 *
 * Revision 3.15.10.6  1996/08/07  09:03:16  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.15.10.5  1996/06/16  21:20:13  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.15.10.4  1996/05/21  10:07:12  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.15.10.3  1996/03/29  18:11:00  sjackson
 * CSCdi52586:  Modem busyout cannot be used without autoconfig
 * Busyout can be achieved via the allowed registry call
 * Branch: California_branch
 *
 * Revision 3.15.10.2  1996/03/28  18:38:26  sjackson
 * CSCdi50364:  Modem autoconfig database should contain Microcom commands
 * Branch: California_branch
 *
 * Revision 3.15.10.1  1996/03/18  22:19:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.3  1996/03/16  07:44:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.2  1996/03/07  10:58:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  21:24:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/02/23  04:12:02  tkolar
 * CSCdi49648:  Modem autoconfig break modularity rules
 * Move confmodem_debug out of the commserver subsystem.
 *
 * Revision 3.14  1996/02/22  14:39:20  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.13  1996/02/03  04:13:12  tkolar
 * CSCdi47986:  IOS modem integration isnt quite there yet
 * Add a way for the IOS to know if the modem switch has reserved the
 * modem.
 *
 * Revision 3.12  1996/02/01  06:11:10  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.11  1996/01/30  00:05:49  sjackson
 * CSCdi47778:  Requirement for modem busyout without modem autoconfig
 * Registry added to allow busyout without autoconfig
 *
 * Revision 3.10  1996/01/29  07:30:40  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.9  1996/01/25  02:16:18  sjackson
 * CSCdi46992:  modem auto discover doesnot recognize codex, usr_courier,
 * etc.
 * Added required ID strings
 *
 * Revision 3.8  1996/01/18  15:54:12  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.7  1996/01/12  20:03:20  sjackson
 * CSCdi44704:  modem busyout command is needed
 * Command added as an extension of the line modem autoconfigure
 *
 * Revision 3.6  1995/12/17  18:39:52  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/09  00:44:02  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.4  1995/11/21  20:59:07  snyder
 * CSCdi44272:  more spellink errors
 *              suceeeded -> succeeded
 *
 * Revision 3.3  1995/11/18  22:17:59  tkolar
 * CSCdi44151:  Modem autoconfig gets line speeds confused
 * Change the way that the modemcap SPD attribute is used by modem
 * configuration.
 *
 * Revision 3.2  1995/11/17  18:55:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:33:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/27  04:18:53  tkolar
 * Files for the modemcap and modem_discovery subsystems.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#include <string.h>
#include <ciscolib.h>
#include <stdlib.h>
#include "ttysrv.h"
#include "sys_registry.h"
#include "registry.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "config.h"
#include "../ui/command.h"
#include "../parser/parser_defs_config.h"
#include "../ts/modemcap_public.h"
#include "../ts/modem_config.h"
#include "../os/async.h"
#include "../h/cs_registry.regh"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "modem_config_debug.h"

/* 0 terminated array of modem speeds */
static const int modem_speeds[] = {115200, 57600, 38400, 9600, 2400, 1200, 300, 0};

void modem_autoconfigure_command (parseinfo *csb)
{
    int i;
    tt_soc *tty;

    if (csb->which == PARSER_LINE_MODEM_AUTOCONFIGURE) {
	if (csb->nvgen) {
	    tty = MODEMS[csb->line1];

	    /* if we're doing modem autoconfigure, write it out */
	    if (tty->capabilities2 & (MA_DISCOVERY_2 + MA_FIXED_2)) {
		nv_write(TRUE, csb->nv_command);
		if (tty->capabilities2 & MA_DISCOVERY_2) {
		    nv_add(TRUE, " discovery");
		} else {
		    nv_add(TRUE, " type %s", tty->modem_name);
		}
	    }
	    return;
	}

	/* not NVGENing */

        for (i = csb->line1; i < csb->line2; i++) {

	    /* 
	     * Return the line to a known state (no modem autoconfigure 
	     * or busyout values at all) and then set what is necessary.
	     */
	    free(MODEMS[i]->modem_name);
	    MODEMS[i]->modem_name = NULL;
	    MODEMS[i]->capabilities2 &= ~(MA_FIXED_2 + MA_DISCOVERY_2 +
						MODEM_BUSYOUT_2 +
						BUSYOUT_OVERRIDE_2);
	    MODEMS[i]->def_capabilities2 &= ~(MA_FIXED_2 + MA_DISCOVERY_2 +
						MODEM_BUSYOUT_2 +
						BUSYOUT_OVERRIDE_2);
	    MODEMS[i]->statbits &= ~MODEM_DETECTED;
	    MODEMS[i]->statbits2 &= ~MODEM_CONFIGURED_2;

	    /*
	     *  If csb->sense, turn on modem autoconfigure 
	     */
	    if (csb->sense) {
		if (GETOBJ(int,1) == MODEM_AUTOCONFIGURE_TYPE) {
		     MODEMS[i]->capabilities2 |= MA_FIXED_2;
		     MODEMS[i]->def_capabilities2 |= MA_FIXED_2;
		     setstring((char **) &(MODEMS[i]->modem_name), 
					    GETOBJ(string,1));
		     if (!modemcap_record_exists(MODEMS[i]->modem_name)) {
			 printf("Warning:  no modemcap record exists for"
				  " type %s\n", MODEMS[i]->modem_name);
		     }
		} else {
		     MODEMS[i]->capabilities2 |= MA_DISCOVERY_2;
		     MODEMS[i]->def_capabilities2 |= MA_DISCOVERY_2;
		}
	    } else {
		/*
		 * Remove our view of busyout as no modem commands will
		 * now be sent and let anyone who is interested know about
		 * this....
		 */
		MODEMS[i]->statbits2 &= ~BUSYOUT_2;
		reg_invoke_modem_busyout(MODEMS[i], csb->sense);
	    }
	}
	return;
    }
}

void modem_busyout_command (parseinfo *csb)
{
    int i;
    tt_soc *tty;

    if (csb->which == PARSER_LINE_MODEM_BUSYOUT) {
	if (csb->nvgen) {
	    tty = MODEMS[csb->line1];
	    if (tty->capabilities2 & MODEM_BUSYOUT_2) {
		nv_write(TRUE, csb->nv_command);
	    }
	    return;
	}

	/*
	 * not NVGENing
	 */
	for (i = csb->line1; i < csb->line2; i++) {

	    /* 
	     * Return the line to a no busyout state
	     */
	    MODEMS[i]->capabilities2 &= ~(MODEM_BUSYOUT_2 +
	    					BUSYOUT_OVERRIDE_2);
	    MODEMS[i]->def_capabilities2 &= ~(MODEM_BUSYOUT_2 +
	    					BUSYOUT_OVERRIDE_2);

	    if ((MODEMS[i]->capabilities2 & (MA_DISCOVERY_2 + MA_FIXED_2)) ||
		reg_invoke_modem_busyout_allowed(MODEMS[i], csb->sense)) {
		if (csb->sense) {
		    MODEMS[i]->capabilities2 |= MODEM_BUSYOUT_2;
		    MODEMS[i]->def_capabilities2 |= MODEM_BUSYOUT_2;
		    /*
		     * Does another part of the system provide this
		     * functionality....
		     */
		    if (reg_invoke_modem_busyout(MODEMS[i], csb->sense)) {
		        MODEMS[i]->capabilities2 |= BUSYOUT_OVERRIDE_2;
		        MODEMS[i]->def_capabilities2 |= BUSYOUT_OVERRIDE_2;
		    }
		} else if (MODEMS[i]->statbits2 & BUSYOUT_2) {
		    /*
		     * If the modem is in busyout state then we have to
		     * remove it (as long as the line is inactive).
		     */
		    if (!(MODEMS[i]->statbits & HASPROC)) {
			terminate_line(MODEMS[i]);
		    }
		} else {
		    /*
		     * Let other interested parties know about this....
		     */
		    reg_invoke_modem_busyout(MODEMS[i], csb->sense);
		}
	    } else {
		printf("\n%% Line %t requires modem autoconfig",
		    MODEMS[i]->ttynum);
		return;
	    }
	}
	return;
    }
}

/*  
 *  Begin modem autoconfiguration routines 
 */

static boolean modem_expectBLOCK (tt_soc *tty)
{
    if (AWAKE(tty->dallytimer)) {
      return(FALSE);
    }

    if (modemcheck(tty)) {
      return(FALSE);
    }

    if (inputpending(tty)) {
      return(FALSE);
    }


    return(TRUE);
}

/*
 * modem_retrieve
 *
 * Populate a buffer with the data from the modem
 */
static int modem_retrieve (tt_soc *tty, char *receive, int reclen)
{
    int count;
    int ch = 0;
    int sleeptime;

    sleeptime = 3*ONESEC;
    count = 0;
    while (count < reclen) {
	TIMER_START(tty->dallytimer, sleeptime);
        edisms((blockproc *)modem_expectBLOCK, (int)tty);
	sleeptime = ONESEC/4;
	if (AWAKE(tty->dallytimer)) {
	    break;
	}
	if (modemcheck(tty)) {
	    break;
	}
	ch = getc(tty);
	if (ch == -1 || ch == LINEFEED || ch == RETURN) {
	    /* do nothing */
	} else {
	    receive[count] = ch;
	    count++;
	}
    }
    tty_clearbuffers(tty);      /* clear type-ahead */
    process_sleep_for(ONESEC/8);
    return(count);
}

/*
 *  modem_talk  -- send and receive one string from a ttyline.
 *
 *  returns number of characters read.
 */

int modem_talk (tt_soc *tty, char *send, char *receive, int reclen)
{
    int ch =0;

    tty_clearbuffers(tty);      /* clear type-ahead */

    quick_putc(tty, RETURN);
    TIMER_START(tty->dallytimer, 2*ONESEC);
    do {
	edisms((blockproc *)modem_expectBLOCK, (int)tty);
	ch = getc(tty);
    } while ((ch != RETURN) && !AWAKE(tty->dallytimer) && !modemcheck(tty));

    /*  If modem didn't return our return, it may be unable to */
    if (modemcheck(tty)) {
	return(-1);
    }

    tty_clearbuffers(tty);      /* Clear it again for luck */
    process_sleep_for(ONESEC/8); /* Hang out for a bit to give the modem
					a chance to catch up */

    quick_puts(tty, send);
    quick_putc(tty, RETURN);

    do {
    	TIMER_START(tty->dallytimer, ONESEC);
        edisms((blockproc *)modem_expectBLOCK, (int)tty);
	ch = getc(tty);
    } while ((ch != RETURN) && !AWAKE(tty->dallytimer) && !modemcheck(tty));

    /* 
     *  the above block should have read the entire echo of what we just
     *	sent, up to and including the return.
     */
    if (ch != RETURN)
	return(-1);

    return (modem_retrieve(tty, receive, reclen));
}


/*
 *  modem_initial_detect()
 *      Find out if we can talk to a modem on this line at all, and 
 *   set the baud rate if so.
 *
 *      We call this before start, and any time we change the speed
 */

static int modem_initial_detect (tt_soc *tty)
{
    uchar buf[RETURNBUFFERLEN];
    int speed, which, len;

    for (which = 0; (speed = modem_speeds[which]); which++) {
	if (setspeed(tty, speed, speed) == FALSE) {
	    continue;
	}
	len = modem_talk(tty, "AT", buf, RETURNBUFFERLEN);
	if (confmodem_debug)
	    buginf("\nTTY%t: detection speed (%d) response ---%s---", 
		tty->ttynum, speed, buf);
	if (!strncmp("OK", buf, strlen("OK")))  {
	    tty->statbits |= MODEM_DETECTED;
	    break;
	}
    }

    if (!speed) {
	if (setspeed(tty, ASYNC_DEFSPEED, ASYNC_DEFSPEED) == FALSE) {
	    if (confmodem_debug) {
		ttybug(tty, "unable to reset default speed");
	    }
	}
	tty->statbits &= ~MODEM_DETECTED;
	tty->statbits2 |= MODEM_SPEED_LOCKED_2;
    }

    return(speed);
}

/*
 * modem_lock_speed
 *
 * We have a speed at which the line must be locked at, attempt to talk to
 * the modem to verify that it has indeed locked the DTE speed. In the event
 * of a failure to communicate we notify an error.
 */
static boolean modem_lock_speed (tt_soc *tty, char *str)
{
    uchar buf[RETURNBUFFERLEN];
    boolean locked = TRUE;
    int speed;
    int ch = 0;

    speed = atoi(str);
    if (setspeed(tty, speed, speed) == FALSE) {
	if (confmodem_debug) {
	    buginf("\nTTY%t: unable to set speed to lock DTE at %d",
		tty->ttynum, speed);
	}
	return (FALSE);
    }

    modem_talk(tty, "AT", buf, RETURNBUFFERLEN);

    while (TRUE) {
	if (confmodem_debug) {
	    buginf("\nTTY%t: locking speed (%d) response ---%s---", 
		tty->ttynum, speed, buf);
	}
	if (strncmp("OK", buf, strlen("OK")) == 0)  {
	    if (confmodem_debug) {
		buginf("\nTTY%t: locked DTE speed at %d", tty->ttynum, speed);
	    }
	    return (TRUE);
	}
	if (locked == FALSE) {
	    if (confmodem_debug) {
		buginf("\nTTY%t: failed to lock DTE speed %d", tty->ttynum,
		    speed);
	    }
	    return (FALSE);
	}

	/*
	 * Send another AT command but one character at a time delayed
	 */
	tty_clearbuffers(tty);

	quick_putc(tty, 'A');
	process_sleep_for(ONESEC/4);
	quick_putc(tty, 'T');
	process_sleep_for(ONESEC/4);
	quick_putc(tty, RETURN);
	TIMER_START(tty->dallytimer, (2*ONESEC));
	do {
	    edisms((blockproc *)modem_expectBLOCK, (int)tty);
	    ch = getc(tty);
	} while (ch != RETURN && !AWAKE(tty->dallytimer) && !modemcheck(tty));

	/*
	 * Check for the echo of the above including the return
	 */
	if (ch != RETURN) {
	    if (confmodem_debug) {
		buginf("\nTTY%t: failed to detect RETURN at %d", tty->ttynum,
		    speed);
	    }
	    return (FALSE);
	}
	modem_retrieve(tty, buf, RETURNBUFFERLEN);
	locked = FALSE;
    };
}

/*
 *  modem_probe_line()
 *      Find out if we can talk to a modem on this line at all, and 
 *   return the baud rate if so.
 */

static forktype modem_probe_line (tt_soc *tty)
{
    int speed;
    char *nameptr;

    if (modem_debug)
	ttybug(tty, "autoconfigure probe started");
    tty->statbits &= ~(IDLE + HANGUP);
    tty->statbits |= READY;
    CLAIM_TTY(tty);
    tty->statbits2 |= MODEM_AUTOCONFING_2;

    tty_portenable(tty);

    if (!(tty->capabilities2 & (MA_DISCOVERY_2 + MA_FIXED_2))) {
	if (confmodem_debug)
	    ttybug(tty, "Attempt to probe a line without autoconfigure set");
	tty_clearbuffers(tty);      /* Clear out anything left over */
	tty->statbits &= ~(IDLE + HANGUP + READY);
	tty->statbits2 &= ~MODEM_AUTOCONFING_2;
	tty->statbits |= IDLE;
	RELEASE_TTY(tty);
	tty_portdisable(tty);
	tty->tiptop = -1;
	process_kill(THIS_PROCESS);
	return;
    }

    /* if we haven't previously detected a modem, go look for one */
    if (!(tty->statbits & MODEM_DETECTED)) {
	speed = modem_initial_detect(tty);

	if (!(tty->statbits & MODEM_DETECTED)) {
	    if (confmodem_debug)
		ttybug(tty,"No modem found");
	    tty_clearbuffers(tty);      /* Clear out anything left over */
	    tty->statbits &= ~(IDLE + HANGUP + READY);
	    tty->statbits2 &= ~MODEM_AUTOCONFING_2;
	    tty->statbits |= IDLE;
	    RELEASE_TTY(tty);
	    tty_portdisable(tty);
	    tty->tiptop = -1;
	    process_kill(THIS_PROCESS);
	    return;
	}

	nameptr = reg_invoke_modem_discovery(tty);
        tty->statbits2 &= ~MODEM_SPEED_LOCKED_2;

	if (nameptr) {
	    setstring((char **) &tty->modem_name, nameptr);

	    if (confmodem_debug) {
		buginf("\nTTY%t: Modem type is %s", tty->ttynum, nameptr);
	    }
	}
    }

    modem_configure(tty);
    tty_clearbuffers(tty);      /* Clear out anything left over */
    tty_portdisable(tty);

    tty->statbits &= ~(IDLE + HANGUP + READY);
    tty->statbits2 &= ~MODEM_AUTOCONFING_2;
    tty->statbits |= IDLE;
    RELEASE_TTY(tty);
    reg_invoke_modem_hardware_override(tty);
    tty->tiptop = -1;
    process_kill(THIS_PROCESS);
}


/*
 *  modem_autoconfigure
 *
 *   This is the fuction called from elsewhere (through 
 *   reg_invoke_modem_autoconfigure) to start the whole thing in motion.
 */
int modem_autoconfigure (tt_soc *tty)
{
    /*
     * Prevent modem autoconfiguration if line is already active
     */
    if ((tty->statbits & HASPROC)
		|| reg_invoke_modem_hardware_in_use(tty)) {
        return(FALSE);
    }

    if (!(tty->capabilities2 & (MA_DISCOVERY_2 + MA_FIXED_2))) {
	return(FALSE);
    }

    tty->tiptop = cfork((forkproc *)modem_probe_line,(long)tty,0,"Modem Autoconfigure",tty->ttynum);

    if (tty->tiptop != NO_PROCESS) {
	tty->isavedchar = -1;
	return(TRUE);
    } else {
	return(FALSE);
    }
}

void modem_configure (tt_soc *tty) 
{
    char modem_string[MAX_MODEM_STRING], buf[MAX_MODEM_STRING], *ptr;

    if (!tty->modem_name) {
	return;
    }

    memset(modem_string, 0, MAX_MODEM_STRING);

    strcpy(modem_string, "AT");

    ptr = modemcap_get_string(tty->modem_name, "FD", 0);
    if (ptr) {
	strcat(modem_string, ptr); 
    }

    ptr = modemcap_get_string(tty->modem_name, "CD", 0);
    if (ptr) {
	strcat(modem_string, ptr); 
    }

    ptr = modemcap_get_string(tty->modem_name, "DTR", 0);
    if (ptr) {
	strcat(modem_string, ptr); 
    }

    ptr = modemcap_get_string(tty->modem_name, "MSC", 0);
    if (ptr) {
	strcat(modem_string, ptr); 
    }

    if (tty->capabilities & (HFLOWIN + HFLOWOUT)) {
	ptr= modemcap_get_string(tty->modem_name, "HFL", 0);
	if (ptr) {
	    strcat(modem_string, ptr);
	}
    }

    if (tty->capabilities & (SFLOWIN + SFLOWOUT)) {
	ptr= modemcap_get_string(tty->modem_name, "SFL", 0);
	if (ptr) {
	    strcat(modem_string, ptr);
	}
    }

    if (tty->capabilities2 & NO_MODEM_ERRCNTRL_2) {
	ptr= modemcap_get_string(tty->modem_name, "NER", 0);
	if (ptr) {
	    strcat(modem_string, ptr);
	}
    } else {
	ptr= modemcap_get_string(tty->modem_name, "BER", 0);
	if (ptr) {
	    strcat(modem_string, ptr);
	}
    }

    if (tty->capabilities2 & NO_MODEM_COMPRESS_2) {
	ptr= modemcap_get_string(tty->modem_name, "NCP", 0);
	if (ptr) {
	    strcat(modem_string, ptr);
	}
    } else {
	ptr= modemcap_get_string(tty->modem_name, "BCP", 0);
	if (ptr) {
	    strcat(modem_string, ptr);
	}
    }

    ptr= modemcap_get_string(tty->modem_name, "SPD", 0);
    if (ptr) {
	strcat(modem_string, ptr);
    }

    ptr= modemcap_get_string(tty->modem_name, "AA", 0);
    if (ptr) {
	strcat(modem_string, ptr);
    }

    if (!(tty->capabilities2 & BUSYOUT_OVERRIDE_2)) {
	if (tty->capabilities2 & MODEM_BUSYOUT_2) {
	    ptr= modemcap_get_string(tty->modem_name, "OFH", 0);
	    if (ptr) {
		strcat(modem_string, ptr);
	    }
	} else {
	    ptr= modemcap_get_string(tty->modem_name, "ONH", 0);
	    if (ptr) {
		strcat(modem_string, ptr);
	    }
	}
    }

    if (confmodem_debug) {
	buginf("\nTTY%t: Modem command:  --%s--", tty->ttynum, modem_string);
    }

    if (modem_talk(tty, modem_string, buf, MAX_MODEM_STRING) < 0) {
	if (confmodem_debug) {
	    ttybug(tty, "Modem configuration FAILED");
	}
    } else {
	if (confmodem_debug) {
	    ttybug(tty, "Modem configuration succeeded");
	}
    }

  
    if (!(tty->statbits2 & MODEM_SPEED_LOCKED_2)) {
	ptr= modemcap_get_string(tty->modem_name, "SPD", 0);
	if (ptr) {
	    boolean failed = FALSE;

	    modem_talk(tty, modem_string, buf, MAX_MODEM_STRING);

	    ptr= modemcap_get_string(tty->modem_name, "DTE", 0);
	    if (ptr) {
		if (!modem_lock_speed(tty, ptr)) {
		    failed = TRUE;
		}
	    } else {
		if (!modem_initial_detect(tty)) {
		    failed = TRUE;
		}
	    }
	    if (failed) {
		if (confmodem_debug) {
		    ttybug(tty, "Failed to redetect modem after locking"
				" speed");
		}
		return;
	    }
	}
    }

    tty->statbits2 |= MODEM_CONFIGURED_2;

    if (!(tty->capabilities2 & BUSYOUT_OVERRIDE_2)) {
	if (tty->capabilities2 & MODEM_BUSYOUT_2) {
	    tty->statbits2 |= BUSYOUT_2;
	    if (confmodem_debug) {
		ttybug(tty, "Modem in busyout state");
	    }
	} else if (tty->statbits2 & BUSYOUT_2) {
	    tty->statbits2 &= ~BUSYOUT_2;
	    if (confmodem_debug) {
		ttybug(tty, "Modem removed from busyout state");
	    }
	}
    }

    if (confmodem_debug) {
	ttybug(tty, "Done with modem configuration");
    }

    return;
}

/*
 * Parse chains for modem autoconfig debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_modemcap.h"
LINK_POINT(confmodem_debug_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for modemcap config commands
 */
#define ALTERNATE       NONE
#include "../ts/cfg_modemcap.h"
LINK_POINT(modemcap_config_commands, ALTERNATE);
#undef  ALTERNATE


#define ALTERNATE	NONE
#include "../ts/cfg_modem_autoconfigure.h"
LINK_POINT(modem_autoconfigure_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE	NONE
#include "../ts/cfg_modem_busyout.h"
LINK_POINT(modem_busyout_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for modemcap show commands
 */
#define ALTERNATE       NONE
#include "../ts/exec_show_modemcap.h"
LINK_POINT(modemcap_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for Modem discovery
 */

static parser_extension_request modemcap_chain_init_table[] = {
    { PARSE_ADD_CFG_LINE_MODEM_CMD, &pname(modem_autoconfigure_commands) },
    { PARSE_ADD_CFG_LINE_MODEM_CMD, &pname(modem_busyout_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(modemcap_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(modemcap_config_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(confmodem_debug_commands) },
    { PARSE_LIST_END, NULL }
};

static void modemcap_subsys_init (subsystype *subsys) 
{
    modemcap_init();
    parser_add_link_point(PARSE_ADD_CFG_LINE_MODEM_AUTOCONFIGURE_CMD, 
		"line modem autoconfigure", 
		&pname(line_modem_autoconfigure_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_LINE_MODEM_BUSYOUT_CMD, 
		"line modem busyout", 
		&pname(line_modem_busyout_extend_here));

    parser_add_command_list(modemcap_chain_init_table, "Modem Autoconfigure");

    reg_add_modem_autoconfigure(modem_autoconfigure, "Modem Autoconfigure");
    reg_add_modemcap_get_string(modemcap_get_string, "Modemcap Get String");

    modem_config_debug_init();
}

/*
 * Modem configuration subsystem header
 */
#define MODEMCAP_MAJVERSION 1
#define MODEMCAP_MINVERSION 0
#define MODEMCAP_EDITVERSION 1

SUBSYS_HEADER(modemcap,
              MODEMCAP_MAJVERSION, MODEMCAP_MINVERSION,
              MODEMCAP_EDITVERSION,
              modemcap_subsys_init, SUBSYS_CLASS_LIBRARY,
              NULL,
              NULL);
