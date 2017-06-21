/* $Id: modem_async_if.c,v 3.1.66.9 1996/09/12 22:19:11 pgoginen Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_async_if.c,v $
 *------------------------------------------------------------------
 * modem_async_if.c Modem Async Interfaces
 *
 * Nov 95, Eddy Lem
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_async_if.c,v $
 * Revision 3.1.66.9  1996/09/12  22:19:11  pgoginen
 * CSCdi68632:  copy rcp modem does not work
 * Branch: California_branch
 * Set remote username to NULL before calling reg_invoke_file_read
 *
 * Revision 3.1.66.8  1996/09/12  22:13:24  pgoginen
 * CSCdi68635:  copy tftp|rcp modem cmd displays garbage if modem is out
 * of range
 * Branch: California_branch
 *
 * Revision 3.1.66.7  1996/09/10  03:52:12  elem
 * CSCdi65653:  Modems come up bad on reboot with dialer in-band configured
 * Branch: California_branch
 * When booting with async mode dedicated, there is a window when the
 * async lines are brought up in hardware PPP mode, and then later
 * brought down.  In this situation simply disable the the hardware
 * PPP mode before attempting to send the ATP to the modems.
 *
 * Revision 3.1.66.6  1996/09/10  03:29:02  elem
 * CSCdi54417:  modem busyout generates report as cleanup pending.
 * Delaying DTR
 * Branch: California_branch
 * The setting of the MODEM_AUTOCONFIG_2 flag to hold off the modem
 * state machine is moved off to modem_lock_tty().  This will prevent
 * the cleanup pending message in all situations when the CSM needs
 * to lock the tty line -- not just during modem maintainence activity.
 *
 * Revision 3.1.66.5  1996/08/28  12:59:10  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.66.4  1996/08/07  09:01:19  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.66.3  1996/07/14  18:27:38  elem
 * CSCdi61142:  Upon reload modems do not respond to polling and CTS is low
 * Branch: California_branch
 * Main problem was some modems failed to initialize, and was marked bad.
 * Cause is PPP dedicated mode putting cirrus chip in hardware ppp mode,
 * and resets tty line during startup.  For now extended the number of
 * retries during modem startup.
 * Fixed flash_copy_file_name call which dropped one argument for
 * CSCdi52582.
 * Also some minor code clean up.
 *
 * Revision 3.1.66.2  1996/06/28  23:23:51  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/06/16  21:16:11  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/11  21:36:35  elem
 * Place holder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*******************************************************************
 * 
 * This module is intented to support any maintenance type activity
 * for the modems, by way of the dte/async port.  Since the requirements
 * of the modems will vary substantially from modem vendor to modem
 * vendor, the structure of this software "tries" to make the additional
 * of new vendors as painless as possible.
 * 
 * Each maintenance action/task is queued using just a simple array,
 * with the maintenance task number as the index, and the contents of
 * the array is the index to the next task:
 *                                     maintn_pending[]
 *                                          +---+
 *                                   task 0 |   |
 *                                          +---+
 *              maintn_head -------> task 1 | 3 |---+
 *                                          +---+   |
 *                                   task 2 |   |   |  next task
 *                                          +---+   |
 *                                   task 3 |   |<--+
 *                                          +---+
 * Enqueuing something here does not wake up the process -- this needs
 * to be done separately.
 *
 * The software runs using a state machine.  This state machine can
 * be tailored to any vendor as they see fit.  The vendor specific
 * state machines are assigned to the particular modems based on a
 * vendor name match with the vendor banner found in the modem module
 * cookie.  The state variable is a single unsigned byte, thus allowing
 * up to 255 states.  The upper half (0x80+) is dedicated for handling
 * common activities, such as executing the basic "AT" type commands,
 * and back-to-back testing.  These states should apply to all vendors.
 * The lower half (0x01-0x7f) is for vendor specific processing.  In
 * addition, each of these states can be further divided, as required,
 * into a multitude of sub-states, by using the "mai_step" variable.  This
 * will allow each vendor to implement how much or how little processing
 * they actually need.  The sending of "AT" commands is considered the 
 * most basic operation of a modem, and thus is provided as a commom, but
 * separate sequences of actions (i.e. states).  Some generic functions,
 * "mai_send_atcmd" and "mai_send_atcmd_while_not_connected", have been
 * provided as entry points into this send state machine.  When these
 * are invoked, the current running state is saved so that after
 * executing the send state machine, the original entry point can be
 * returned to.
 *
 * The state machine will see only two type of events, QUEUE_EVENT's
 * and TIMER_EVENT's.  The QUEUE_EVENT's comes from process_enqueue'ing
 * the rsp_info structure.  When there is no current modem maintenance
 * activity, the QUEUE_EVENT is used to jump start it to perform whatever
 * new tasks has been enqueued in the maintenance action/task queue.  This
 * event is also used by the send state machine to wake up the
 * process when it has received some data.  As for the TIMER_EVENT,
 * there is only one global timer event, which is an unmanaged timer.
 * Each modem maintains its own unmanaged dallytimer, which is checked
 * during each time when the global event time expires.
 *
 **********************************************************************/

#include "master.h"
#include <stdlib.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "config.h"
#include "logger.h"
#include "../les/msg_modem.c"		/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "ctype.h"
#include "globs.h"
#include "ttysrv.h"
#include "../os/async.h"
#include "../ts/if_async.h"

#include "file.h"
#include "name.h"
#include "../os/boot.h"
#include "../ui/parse_util.h"
#include "../dev/flash_defines.h"
#include "../dev/flash_services.h"
#include "../dev/flash_registry.h"
#include "parser_defs_modem.h"

#include "if_as5200_tdm.h"
#include "if_as5200_modem.h"
#include "../ts/as5200_cd2430.h"
#include "cs_registry.regh"
#include "modem_mgmt.h"

extern boolean system_configured;

#define MODEM_TEST_BEGIN_STR    "AT"

static addrtype mai_hostip;	/* host ip address used in downloads */

/* Parser command to turn this on is not yet implemented */
static boolean mai_debug = 0;

/*
 * Prototypes for building the state machines.
 */
static boolean mai_handle_maintn_start(modem_info_t *, int *);
static boolean mai_handle_dl_init(modem_info_t *, int *);
static boolean microcom_handle_dl_modem(modem_info_t *, int *);
static boolean mai_handle_dl_complete(modem_info_t *, int *);
static boolean mai_handle_send(modem_info_t *, int *);
static boolean mai_handle_b2b_init(modem_info_t *, int *);
static boolean mai_handle_b2b_connect(modem_info_t *, int *);
static boolean mai_handle_b2b_data_test(modem_info_t *, int *);
static boolean microcom_handle_poweron_init(modem_info_t *, int *);
static void mai_set_modem_poweron(modem_info_t *);
static boolean mai_reset_modem(modem_info_t *, int *);

/*
 * Microcom Specific Maintenance State Machine Functions
 */
static boolean (*microcom_proc_ftns[])(modem_info_t *, int*) = {
    mai_handle_maintn_start,		/* IDLE */
    mai_handle_dl_init,			/* DL_INIT */
    microcom_handle_dl_modem,		/* DL_INIT_MODEM */
    mai_handle_dl_complete,		/* DL_COMPLETE */
    microcom_handle_poweron_init,	/* PWRON_INIT */
    mai_reset_modem,                    /* RESET_MODEM */
};

/*
 * Default Maintenance State Machine Functions
 */
static boolean (*mai_default_proc_ftns[])(modem_info_t *, int*) = {
    mai_handle_maintn_start,		/* IDLE */
    NULL,				/* DL_INIT */
    NULL,				/* DL_INIT_MODEM */
    NULL,				/* DL_COMPLETE */
    NULL,				/* PWRON_INIT */
    NULL,                               /* RESET_MODEM */ 
};

/*
 * Common Maintenace State Machine Functions
 */
static boolean (*const mai_common_proc_ftns[])(modem_info_t *, int*) = {
    /* sending AT commands */
    mai_handle_send,		/* CM_WAIT_CR */
    mai_handle_send,		/* CM_WAIT_CLR */
    mai_handle_send,		/* CM_WAIT_ECHO */
    mai_handle_send,		/* CM_WAIT_RESP */

    /* back2back tests */
    mai_handle_b2b_init,	/* MAI_B2B_INIT */
    mai_handle_b2b_connect,	/* MAI_B2B_CONNECTING */
    mai_handle_b2b_data_test,	/* MAI_B2B_DATA */
};

/*
 * Vendor Specific Parameters
 */
modem_vendor_defs_t mai_modem_vendor_specific_table[] = {
    { "Microcom", "01", FALSE, mai_set_modem_poweron, &microcom_proc_ftns},
    { "Rockwell", "01", TRUE, NULL, &mai_default_proc_ftns },
    { "US Robotics", "01", TRUE, NULL, &mai_default_proc_ftns },
    { "", "", FALSE, NULL, &mai_default_proc_ftns }
};


/*
 * Modem Maintenance Strings
 */
static const char *const mai_maintn_strings[] = {
	"None",					/* MAI_NONE_PENDING */
	"Modem Firmware Download",		/* MAI_PENDING_DL */
	"Back-to-back Test",			/* MAI_PENDING_B2B */
	"Power-On Initialization",		/* MAI_PENDING_PWRON */
	"Power-On Back-to-back Test",		/* MAI_PENDING_PWRON_B2B */
	"Reset Modem",                          /* MAI_PENDING_RESET */
	"",
};

/*
 *  mai_free_dl_info 
 *    -	Dump all the data buffers allocated to hold the file that
 *	was downloaded, and then the download file descriptor
 *	itself.
 */
static void mai_free_dl_info (mai_dl_info_t *dlp)
{
    mai_dl_buffer_t *bptr;

    while ((bptr = dequeue(&dlp->bufqueue))) {
	free(bptr);
    }
    free(dlp);
}

/*
 *  mai_maintn_cleanup
 *    -	Clean up everything after all maintenance actions for this modem
 *	are completed.
 */
static void mai_maintn_cleanup (modem_info_t *modem)
{
    if (LIST_ELEMENT_QUEUED(&modem->tmonext)) {
	list_remove(&modem_pool->mai_tmolist, &modem->tmonext, modem);
    }
    if (!(modem->mai_statbits & MAI_USER_BUSYOUT)) {
	csm_clear_busy_out_status(modem);
    }
    if (modem->mai_statbits & MAI_MAINTN_PENDING) {
	modem->mai_statbits &= ~MAI_MAINTN_PENDING;
	--modem_pool->mai_maintn_cnt;
    }
    modem->modem_tty->statbits |= HANGUP;
    modem->modem_tty->statbits &= ~(READY+CARDROP+IDLE+RINGIN);
    tty_dtrstate(modem->modem_tty, FALSE);
    tty_portdisable(modem->modem_tty);
    
    if (modem->replystrg) {
	free(modem->replystrg);
	modem->replystrg = NULL;
    };
}

/*
 *  mai_next_maintns 
 *    -	Terminate currently maintenance action.
 */
static boolean mai_next_maintns (modem_info_t *modem)
{
    mai_cmd_t maintn;
    mai_dl_info_t *dlp;

    modem->mai_state = MAI_IDLE;
    modem->mai_step = MAI_STP_BEGIN;
    TIMER_STOP(modem->dallytimer);

    /* clean up old one first */
    switch (maintn = modem->maintn_head) {

	case MAI_PENDING_PWRON:
	    /* hold on to modem if need to do b2b test */
	    if (modem->maintn_pending[maintn] != MAI_PENDING_PWRON_B2B) {
		csm_clear_busy_status(modem);
	    }
	    break;

	case MAI_PENDING_PWRON_B2B:

	    csm_clear_busy_status(modem);

	    /* fall through */

	case MAI_PENDING_B2B:

	    modem->alt_modem = NULL;
	    modem->mai_statbits &= ~(MAI_B2B_ATD | MAI_B2B_ATA);
	    break;

	case MAI_PENDING_DL:

	    if ((dlp = modem->download_info) == NULL) {
		break;
	    }
	    modem->download_info = NULL;
	    if (--dlp->modemcnt) {
		break;
	    }
	    mai_free_dl_info(dlp);
	    break;

        case MAI_PENDING_RESET:
	    break;
	    
	case MAI_NONE_PENDING:

	    break;

	default:

	    errmsg(&msgsym(BADMODEM, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			"NEXT_MAINTN: software error");
 	    for (maintn = 0; maintn < MAI_MAX_PENDING_MAINTN; maintn++) {
	    	modem->maintn_pending[maintn] = MAI_NONE_PENDING;
	    }
	    maintn =  modem->maintn_head = MAI_NONE_PENDING;
	    break;

    }
    modem->maintn_head = modem->maintn_pending[maintn];
    modem->maintn_pending[maintn] = MAI_NONE_PENDING;
    if (modem->maintn_head == MAI_NONE_PENDING) {
	/* if there is a stray response message queued up, */
	/* then do not clean up now, do it when it comes in */
	if ((modem->mai_statbits & MAI_STRAY_QUEUE_EVENT) == 0) {
	    mai_maintn_cleanup(modem);
	}
	return (FALSE);
    }
    return (TRUE);
}

/*
 *  mai_abort_all_maintns 
 *    -	Abort all the pending maintenance actions.
 */
static void mai_abort_all_maintns (modem_info_t *modem)
{
    int ix;
    modem_slot_t *slotinfo;
    modem_info_t *next_modem;

    for (ix = 0; ix < MAI_MAX_PENDING_MAINTN; ix++) {
	if (!mai_next_maintns(modem)) {
	    return;
	} else if (modem->maintn_head == MAI_PENDING_PWRON_B2B) {
	    if (mai_debug) {
		buginf("\nMAI(%d/%d): pwronb2b aborted. ",
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem));
	    }
	    slotinfo = modem_pool->modem_slot[CSM_MODEM_SLOT(modem)];
	    --slotinfo->mai_pwron_b2b_undetermined;
	    --slotinfo->mai_pwron_b2b_total;
	    --slotinfo->mai_pwron_b2b_max_modems;
	    /* See if some one is waiting for this modem to do  */
	    /* the poweron b2b test.  If so, restart that modem */
	    /* so that it can get paired off properly.          */
	    if ((slotinfo->mai_pwron_b2b_total == 0) &&
				slotinfo->mai_pwron_b2b_next_modem) {
		next_modem = slotinfo->mai_pwron_b2b_next_modem;
		slotinfo->mai_pwron_b2b_next_modem = NULL;
	    	slotinfo->mai_pwron_b2b_total++;
		next_modem->mai_state = MAI_IDLE;
		next_modem->mai_step = MAI_STP_BEGIN;
		process_enqueue(modem_pool->mai_command_queue,
						&next_modem->rsp_info);
		if (mai_debug) {
		    buginf("\nMAI(%d/%d): pwronb2b restarted next modem. ",
			CSM_MODEM_SLOT(next_modem), CSM_MODEM_PORT(next_modem));
		}
	    }
	}
    }
    errmsg(&msgsym(BADMODEM, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			"ABORT_ALL_MAINTN: software error");
}

/*
 *  mai_execute_proc 
 *    -	Function dispatcher which call the appropriate function for
 *	the given state.
 */
static void mai_execute_proc (modem_info_t *modem, int event)
{
    boolean doit;

    doit = TRUE;
    while (doit) {
	if (modem->mai_state >= MAI_COMMON_TASK &&
			modem->mai_state < MAI_COMMON_MAX_STATE_FTN) {
	    doit = (*mai_common_proc_ftns[modem->mai_state - MAI_COMMON_TASK])
							(modem, &event);
	} else {
	    if (modem->mai_state < MAI_MAX_STATE && modem->do_ftn != NULL &&
				(*modem->do_ftn)[modem->mai_state] != NULL) {
		doit = (*((*modem->do_ftn)[modem->mai_state]))(modem, &event);
	    } else {
		mai_abort_all_maintns(modem);
		break;
	    }
	}
    }
}

/*
 *  mai_timer_handler 
 *    -	This is the root handler for timer events.  There is only one
 *	global timer event, which is an unmanaged timer.  This handles it
 *	and then checks the individual unmanaged timers on each modem to
 *	see if they expired.
 */
static void mai_timer_handler ( void )
{
    modem_info_t *modem;
    list_element *tmolink;
    int	event;
    int	maxtimes;

    TIMER_STOP(modem_pool->mai_global_timer);
    maxtimes = 0;
    FOR_ALL_ELEMENTS_IN_LIST(&modem_pool->mai_tmolist, tmolink) {

	modem = LIST_GET_DATA(tmolink);
	if (modem->mai_state == MAI_IDLE) {
	    /* Should not get this when already processing */
	    list_remove(&modem_pool->mai_tmolist, &modem->tmonext, modem);
	    continue;
	}

	if (TIMER_RUNNING_AND_AWAKE(modem->dallytimer)) {
	    event = TIMER_EVENT;
	} else {
	    event = 0;
	}

	if (event) {
	    maxtimes++;
	    mai_execute_proc(modem, event);
	    if (maxtimes > MAI_MAX_RUNS) {
		process_may_suspend();
		maxtimes = 0;
	    }
	}
    }
    /* reset global timer */
    TIMER_START(modem_pool->mai_global_timer, MAI_TIMEOUT_CHECK_INTERVAL);
}

/*
 *  mai_queue_handler 
 *    -	This the root of the queue event handler.  All queued events
 *	are picked up here and executed for the appropriate modem and
 *	state.
 */
static void mai_queue_handler (void)
{
    modem_q_info_t *rspinfo;
    modem_info_t   *modem;
    int maxtimes;

    maxtimes = 0;
    while (TRUE) {

	for (maxtimes = MAI_MAX_RUNS; maxtimes > 0; --maxtimes) {

	    rspinfo = process_dequeue(modem_pool->mai_command_queue);
	    if (rspinfo == NULL) {
		return;
	    }
	    modem = (modem_info_t *)rspinfo->modem_info;
            /* 
             * Since this is a stray event, we will not
             * do any processing for this event.
             */
	    if (modem->mai_statbits & MAI_STRAY_QUEUE_EVENT) {
               modem->mai_statbits  &= ~MAI_STRAY_QUEUE_EVENT;
	       continue;
            }
	    rspinfo->next = NULL;
	    mai_execute_proc(modem, QUEUE_EVENT);
	}
	process_may_suspend();

    }

}

/*
 *  mai_maintn_process 
 *    -	This is the main process which handles all the maintenance
 *	activity for all the modems.
 */
static process mai_maintn_process (void)
{
    ulong	major, minor;

    /* Make sure everybody else is ready */
    process_wait_on_system_init();

    process_watch_queue(modem_pool->mai_command_queue, ENABLE, RECURRING);
    process_watch_timer(&modem_pool->mai_global_timer, ENABLE);
    TIMER_STOP(modem_pool->mai_global_timer);

    while (modem_pool->mai_maintn_cnt) {

	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch(major) {

	    case TIMER_EVENT:
		mai_timer_handler();
		break;

	    case QUEUE_EVENT:
		mai_queue_handler();
		break;

	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;

	    }
	}
    }

    process_watch_queue(modem_pool->mai_command_queue, DISABLE, RECURRING);
    process_watch_timer(&modem_pool->mai_global_timer, DISABLE);
    modem_pool->mai_maintn_process_pid = 0;
    process_kill(THIS_PROCESS);
}

/*
 *  mai_handle_maintn_start 
 *    -	Start the maintenance operation.
 */
static boolean mai_handle_maintn_start (modem_info_t *modem, int *event)
{
    modem_q_info_t *qlink;
    modem_info_t *alt_modem;
    modem_slot_t *slotinfo;

    switch (modem->maintn_head) {

        case MAI_PENDING_PWRON:
	    /* set the state */
	    modem->mai_step = MAI_STP_BEGIN;
	    modem->mai_state = MAI_PWRON_INIT; 
	    break;

	case MAI_PENDING_PWRON_B2B:
	    slotinfo = modem_pool->modem_slot[CSM_MODEM_SLOT(modem)];
	    --slotinfo->mai_pwron_b2b_total;
	    alt_modem = NULL;
	    if (slotinfo->mai_pwron_b2b_next_modem) {
		alt_modem = slotinfo->mai_pwron_b2b_next_modem;
		slotinfo->mai_pwron_b2b_next_modem = NULL;
	    } else if (slotinfo->mai_pwron_b2b_total) {
		slotinfo->mai_pwron_b2b_next_modem = modem;
		if (mai_debug) {
		    buginf("\nMAI(%d/%d): pwronb2b next modem. ",
				CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem));
		}
	    } else if (QUEUESIZE(&slotinfo->mai_pwron_b2b_good_queue)) {
		/* no more untested modems, if a good */
		/* modem is available, just grab it */
		if (QUEUESIZE(&slotinfo->mai_pwron_b2b_good_queue)) {
		    qlink = dequeue(&slotinfo->mai_pwron_b2b_good_queue);
		    alt_modem = qlink->modem_info;
		    modem->mai_statbits |= MAI_FAILED_LAST_B2B;
		    if (mai_debug) {
			buginf("\nMAI(%d/%d): pwronb2b direct dequeue from "
				"good (%d/%d). ",
				CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
				CSM_MODEM_SLOT(alt_modem),
				CSM_MODEM_PORT(alt_modem));
		    }
		}
	    }
	    /* else, let it simply run thru b2b_init and */
	    /* try to pair it up with a good one later */

	    /* prepare the modems for b2b test */
	    if (alt_modem) {
		modem->alt_modem = alt_modem;
		alt_modem->alt_modem = modem;
		modem->mai_statbits |= MAI_B2B_ATD;
		alt_modem->mai_statbits |= MAI_B2B_ATA;
		modem->mai_b2b_max = alt_modem->mai_b2b_max = 1;
		modem->mai_b2b_cnt = alt_modem->mai_b2b_cnt = 0;
		modem->mai_b2b_good = alt_modem->mai_b2b_good = 0;
		if (mai_debug) {
		    buginf("\nMAI(%d/%d): pwronb2b (%d,%d) ",
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			CSM_MODEM_SLOT(alt_modem), CSM_MODEM_PORT(alt_modem));
		}
	    }
	    /* if no alt modem, still let it run thru and it */
	    /* will stop after the first AT command step */
	    modem->mai_step = MAI_STP_BEGIN;
	    modem->mai_state = MAI_B2B_INIT; 
	    break;

	case MAI_PENDING_B2B:
	    if (modem->alt_modem == NULL) {
		mai_abort_all_maintns(modem);
		return (FALSE);
	    }
	    /* set the state */
	    modem->mai_step = MAI_STP_BEGIN;
	    modem->mai_state = MAI_B2B_INIT; 
	    break;

	case MAI_PENDING_DL:

	    modem->mai_step = MAI_STP_BEGIN;
	    if (!csm_set_download_file_status(modem)) {
	       mai_abort_all_maintns(modem);
	       return (FALSE);
	    } 
	    csm_clear_download_fail_status(modem);
	    MCOM_MODEM_CTR(modem).mai_dl_retry_stp_ru_there     = 0;
	    MCOM_MODEM_CTR(modem).mai_dl_retry_stp_xfer_done    = 0;
	    MCOM_MODEM_CTR(modem).mai_dl_retry_xfer_timedout    = 0;
	    MCOM_MODEM_CTR(modem).mai_dl_retry_stp_version_done = 0;
	    MCOM_MODEM_CTR(modem).mai_dl_retry_stp_atp_done     = 0;
            MCOM_MODEM_CTR(modem).mai_dl_ppp_mode = FALSE;
	    modem->mai_state = modem->mai_laststate = MAI_DL_INIT;
	    break;
        case MAI_PENDING_RESET:
	    modem->mai_state = MAI_RESET_MODEM;
	    break;

	default:
	    modem->maintn_head = MAI_NONE_PENDING;
	    mai_maintn_cleanup(modem);
	    return (FALSE);
    }

    /* enueue it for timer handling */
    if (!LIST_ELEMENT_QUEUED(&modem->tmonext)) {
	list_enqueue(&modem_pool->mai_tmolist, &modem->tmonext, modem);
    }
    if (!TIMER_RUNNING(modem_pool->mai_global_timer)) {
	TIMER_START(modem_pool->mai_global_timer, MAI_TIMEOUT_CHECK_INTERVAL);
    }

    tty_portenable(modem->modem_tty);
    tty_dtrstate(modem->modem_tty, TRUE);

    /*
     * Reply buffer should have been allocated before.
     * If fail here we assume catastrophic, and abort
     * everything.
     */
    if (modem->replystrg == NULL) {
	modem->replystrg = malloc_named(MAI_REPLYSTRGLEN,
			"MAI Modem Reply String");
	if (modem->replystrg == NULL) {
	    mai_abort_all_maintns(modem);
	    return (FALSE);
	}
    }

    *event = QUEUE_EVENT;
    return (TRUE);
}

/*
 *  mai_rxservice 
 *    -	RX service for the separate send state machine.
 */
static void mai_rxservice (tt_soc *tty, uchar *ptr, int len)
{
    modem_info_t	*modem;

    modem = modem_ttynum_to_modem_info(tty->ttynum);
    if (len > modem->rsp_bcount) {
	len = modem->rsp_bcount;
    }
    if (len > 0) {
	bcopy(ptr, modem->cur_rsptr, len);
	modem->cur_rsptr += len;
	modem->rsp_bcount -= len;
	if (modem->mai_statbits & MAI_RX_SIGNAL_REQ) {
	    modem->mai_statbits &= ~MAI_RX_SIGNAL_REQ;
	    process_enqueue(modem_pool->mai_command_queue, &modem->rsp_info);
	}
    }
}

/*
 *  mai_do_send_strg
 *    - Sends a given string to modem.  It saves the pointer to string
 *	so the string should not be touched until the send process is
 *	complete.  This is a jumping point into the separate send
 *	state machine states.
 */
static boolean mai_do_send_strg (modem_info_t *modem, char *str, int timeout)
{
    TIMER_STOP(modem->dallytimer);
    modem->save_ptr = str;
    modem->mai_laststate = modem->mai_state;
    modem->rsp_waittime = timeout;
    if (str) {
	/* inhibits the rxservice */
	modem->rsp_bcount = 0;
	TIMER_START(modem->dallytimer, MAI_TIMEOUT_QUICK_WAIT);
	modem->mai_state = MAI_CM_WAIT_CR;
    } else {
	modem->cur_rsptr = modem->save_ptr = modem->replystrg;
	*modem->cur_rsptr = 0;
	modem->rsp_bcount = MAI_MODEM_RSPLEN;
	modem->mai_statbits |= MAI_RX_SIGNAL_REQ;
	tty_servicehook(modem->modem_tty, NULL, mai_rxservice);
	TIMER_START(modem->dallytimer, timeout);
	modem->mai_state = MAI_CM_WAIT_RESP;
    }
    quick_putc(modem->modem_tty, RETURN);
    return (FALSE);
}

/*
 *  mai_do_send_nocr_strg
 */
static boolean mai_do_send_nocr_strg (modem_info_t *modem, char *str, int timeout)
{
    TIMER_STOP(modem->dallytimer);
    modem->save_ptr = str;
    modem->mai_laststate = modem->mai_state;
    modem->rsp_waittime = timeout;
    if (str) {
	/* inhibits the rxservice */
	modem->rsp_bcount = 0;
	TIMER_START(modem->dallytimer, MAI_TIMEOUT_QUICK_WAIT);
	modem->mai_state = MAI_CM_WAIT_CR;
    } else {
	modem->cur_rsptr = modem->save_ptr = modem->replystrg;
	*modem->cur_rsptr = 0;
	modem->rsp_bcount = MAI_MODEM_RSPLEN;
	modem->mai_statbits |= MAI_RX_SIGNAL_REQ;
	tty_servicehook(modem->modem_tty, NULL, mai_rxservice);
	TIMER_START(modem->dallytimer, timeout);
	modem->mai_state = MAI_CM_WAIT_RESP;
    }
    return (FALSE);
}


/*
 *  mai_send_nocr_atcmd_while_not_connected
 */
static boolean mai_send_nocr_atcmd_while_not_connected (modem_info_t *modem,
				char *str)
{
    return (mai_do_send_nocr_strg(modem, str, MAI_TIMEOUT_NOT_CONNECTED_AT_CMD));
}


/*
 *  mai_send_nocr_atcmd 
 */
static boolean mai_send_nocr_atcmd (modem_info_t *modem, char *str)
{
    return (mai_do_send_nocr_strg(modem, str, MAI_TIMEOUT_NORMAL_AT_CMD));
}

/*
 *  mai_send_atcmd_while_not_connected
 *    -	Generic send AT commands to modems which may not yet be ready
 *	(i.e. coming out of reset or something).
 *	Entry point into the separate send state machine states with
 *	a time-out for responses of 10 seconds.
 */
static boolean mai_send_atcmd_while_not_connected (modem_info_t *modem,
				char *str)
{
    return (mai_do_send_strg(modem, str, MAI_TIMEOUT_NOT_CONNECTED_AT_CMD));
}

/*
 *  mai_send_atcmd 
 *    -	Generic send AT commands to modems which are ready.
 *	Entry point into the separate send state machine states with
 *	a time-out for responses of 3 seconds.
 */
static boolean mai_send_atcmd (modem_info_t *modem, char *str)
{
    return (mai_do_send_strg(modem, str, MAI_TIMEOUT_NORMAL_AT_CMD));
}

/*
 *  mai_abort_modem_send
 *    - asynchronously abort the sending of a string to "another" modem.
 *	"Another" being a modem that is not the currently be processed
 *	by the state machine.
 *    - Returns TRUE if the rsp_info signal is already enqueued, and
 *	too late to abort.
 *	  
 */
static boolean mai_abort_modem_send (modem_info_t *modem)
{
    leveltype	SR;
    boolean	rval;

    rval = FALSE;
    /* quickly disable the rsp_info, if possible */
    SR = raise_interrupt_level(CD2430R_DISABLE);
    if (modem->mai_state == MAI_CM_WAIT_ECHO ||
			modem->mai_state == MAI_CM_WAIT_RESP) {
	tty_noservicehook(modem->modem_tty);
        reset_interrupt_level(SR);
	if ((modem->mai_statbits & MAI_RX_SIGNAL_REQ) == 0) {
	    rval = TRUE;
	} else {
	    modem->mai_statbits &= ~MAI_RX_SIGNAL_REQ;
	}
    } else {
        reset_interrupt_level(SR);
    }

    if (modem->mai_state >= MAI_CM_WAIT_CR &&
				modem->mai_state <= MAI_CM_WAIT_RESP) {
	modem->mai_state = modem->mai_laststate;
	TIMER_STOP(modem->dallytimer);
	tty_clearbuffers(modem->modem_tty);
	modem->rsp_bcount = 0;
	*modem->replystrg = 0;
    }

    return (rval);

}

/*
 *  mai_do_send_data
 *    -	sends data to modem.  It requires a txservice function which
 *	handle how the data will be sent to the modem.
 *	This is a jumping point into the separate send state machine states.
 */
static boolean mai_do_send_data (modem_info_t *modem,
			uint (*txfn)(tt_soc *, uchar *, int),
			int timeout)
{
    TIMER_STOP(modem->dallytimer);
    modem->cur_rsptr = modem->save_ptr = modem->replystrg;
    *modem->cur_rsptr = 0;
    if (txfn == NULL) {
	return (TRUE);
    }
    modem->mai_laststate = modem->mai_state;
    modem->rsp_waittime = timeout;
    modem->rsp_bcount = MAI_MODEM_RSPLEN;
    modem->mai_statbits |= MAI_RX_SIGNAL_REQ;
    tty_servicehook(modem->modem_tty, txfn, mai_rxservice);
    startoutput(modem->modem_tty);
    modem->mai_state = MAI_CM_WAIT_RESP;
    TIMER_START(modem->dallytimer, timeout);
    return (FALSE);
}

/*
 *  mai_handle_send 
 *    -	Handles all the generic interaction with the modem for the
 *	separate "send" state machine.  It will take care of all
 *	the generic interactions with the modem for sending character
 *	string commands, and retrieving character string responses.
 */
static boolean mai_handle_send (modem_info_t *modem, int *event)
{
    tt_soc	*tty;
    char	*ptr, *endp;
    leveltype	SR;
    boolean	done;

    TIMER_STOP(modem->dallytimer);

    tty = modem->modem_tty;
    done = FALSE;

    switch (modem->mai_state) {

    case MAI_CM_WAIT_CR:

	tty_clearbuffers(tty);
	TIMER_START(modem->dallytimer, MAI_TIMEOUT_QUICK_WAIT);
	modem->mai_state = MAI_CM_WAIT_CLR;
	return (FALSE);

    case MAI_CM_WAIT_CLR:

	/* ready to send get rx buffer ready */
	modem->cur_rsptr = modem->replystrg;
	modem->rsp_bcount = MAI_MODEM_RSPLEN;
	modem->mai_statbits |= MAI_RX_SIGNAL_REQ;
	tty_servicehook(tty, NULL, mai_rxservice);
	ptr = modem->save_ptr;
	modem->save_ptr = modem->replystrg;

	quick_puts(tty, ptr);
	TIMER_START(modem->dallytimer, MAI_TIMEOUT_ECHO_RESPONSE);
	modem->mai_state = MAI_CM_WAIT_ECHO;

	quick_putc(tty, RETURN);
	break;

    case MAI_CM_WAIT_ECHO:

	ptr = modem->replystrg;
	if (modem->cur_rsptr > ptr) {
	    SR = raise_interrupt_level(CD2430R_DISABLE);
	    modem->mai_statbits |= MAI_RX_SIGNAL_REQ;
	    endp = modem->cur_rsptr;
	    while (ptr < endp) {
		if (*ptr++ == RETURN) {
		    /* clear out all other white space */
		    while (ptr < endp && !isgraph(*ptr)) {
			ptr++;
		    }
		    if (ptr < endp) {
			int len;
			/* move rsp already in the buffer to the front */
			len = endp - ptr;
			memmove(modem->replystrg, ptr, len);
			modem->rsp_bcount = MAI_MODEM_RSPLEN - len;
			modem->cur_rsptr = modem->replystrg + len;
			TIMER_START(modem->dallytimer, MAI_TIMEOUT_INTER_CHAR);
		    } else {
			modem->rsp_bcount = MAI_MODEM_RSPLEN;
			modem->cur_rsptr = modem->replystrg;
			TIMER_START(modem->dallytimer, modem->rsp_waittime);
		    }
		    modem->mai_state = MAI_CM_WAIT_RESP;
		    *modem->cur_rsptr = 0;
		    reset_interrupt_level(SR);
		    return (FALSE);
		}
	    }
	    /* reset buffer */
	    modem->rsp_bcount = MAI_MODEM_RSPLEN;
	    modem->cur_rsptr = modem->replystrg;
	    reset_interrupt_level(SR);
	}

	if (*event == TIMER_EVENT) {
	   modem->rsp_bcount = MAI_MODEM_RSPLEN;
	   modem->cur_rsptr = modem->replystrg;
	   TIMER_START(modem->dallytimer, modem->rsp_waittime);
	   modem->mai_state = MAI_CM_WAIT_RESP;
	   *modem->cur_rsptr = 0;
	   return (FALSE);
	}

	TIMER_START(modem->dallytimer, MAI_TIMEOUT_ECHO_RESPONSE);
	break;

    case MAI_CM_WAIT_RESP:

	/*
	 * For the AT response string, we grab all the data,
	 * taking out the white spaces.   The termination is
	 * signaled using the QUEUE_EVENT, so if we have data
	 * and time out before a CR/LF, TIMER_EVENT gets changed
	 * to a QUEUE_EVENT.
	 */
	ptr = modem->save_ptr;
	if (modem->cur_rsptr > ptr) {
	    SR = raise_interrupt_level(CD2430R_DISABLE);
	    endp = modem->cur_rsptr;
	    for (; ptr < endp; ptr++) {
		if ((*ptr == RETURN || *ptr == LINEFEED) &&
		    			modem->save_ptr > modem->replystrg) {
		    modem->mai_statbits &= ~MAI_RX_SIGNAL_REQ;
		    reset_interrupt_level(SR);
		    *modem->save_ptr = 0;
		    *event = QUEUE_EVENT;
		    done = TRUE;
		    break;
		}
		if (isgraph(*ptr)) {
		    *modem->save_ptr++ = *ptr;
		}
	    }
	    if (done) {
		break;
	    }
	    modem->cur_rsptr = modem->save_ptr;
	    modem->rsp_bcount = MAI_MODEM_RSPLEN -
				(modem->save_ptr - modem->replystrg);
	    modem->mai_statbits |= MAI_RX_SIGNAL_REQ;
	    reset_interrupt_level(SR);
	}
	if (*event == TIMER_EVENT) {
	    if (modem->save_ptr > modem->replystrg) {
		*modem->save_ptr = 0;
		*event = QUEUE_EVENT;
	    }
	    done = TRUE;
	    break;
	}

	/* Last character timeout 1/2 sec */
	TIMER_START(modem->dallytimer, MAI_TIMEOUT_ECHO_RESPONSE);
	break;

    default:

	done = TRUE;
    }

    if (done) {
	modem->mai_state = modem->mai_laststate;
	modem->mai_statbits &= ~MAI_RX_SIGNAL_REQ;
	tty_noservicehook(tty);
	TIMER_STOP(modem->dallytimer);
	return (TRUE);
    }

    *modem->cur_rsptr = 0;
    return (FALSE);
}

/*
 *  mai_sleep_for 
 *    -	Generic sleep function for maintenance process processing.
 *	Make use of the timer events for providing a sleep without
 *	suspending the maintenance process on the spot.
 */
static boolean mai_sleep_for (modem_info_t *modem, int sleeptime)
{
    modem->rsp_bcount = 0;
    TIMER_START(modem->dallytimer, sleeptime);
    return (FALSE);
}

/*
 *  mai_modem_pool_init
 *    -	Called during board analyze phase for each modem board.
 *	Initialize some modem_info stuff for new modem board, and
 *	any vendor specific handles.
 */
void mai_modem_pool_init (modem_slot_t *slotinfo)
{
    modem_info_t *modem;
    int port;
 
    for (port = 0; port < slotinfo->total_modem_num; port++) {
	modem = slotinfo->modem_info[port];
	if (modem == NULL) {
	    continue;
	}
	modem->rsp_info.modem_info = modem;
	modem->pwron_b2b_qlink.modem_info = modem;

	/* pre-set vendor specific processing */
	modem->do_init_ftn = modem->vendor_info->init_ftn;
	modem->do_ftn = modem->vendor_info->maintn_ftn;
    }
}

/*
 *  mai_enqueue_pending_maintn 
 *    -	Enqueue a maintenance item at the end of the maintenance
 *	pending queue.
 */
static boolean mai_enqueue_pending_maintn (modem_info_t *modem,
						mai_cmd_t newmaintn)
{
    mai_cmd_t maintn;

    if (newmaintn == MAI_NONE_PENDING || newmaintn >= MAI_MAX_PENDING_MAINTN) {
	return (FALSE);
    }

    maintn = modem->maintn_head;
    if (maintn == MAI_NONE_PENDING) {
    	modem->maintn_head = newmaintn;
    } else {
	/* look for end of maintenance action queue */
	while (modem->maintn_pending[maintn] != MAI_NONE_PENDING) {
	   if (newmaintn == maintn) {
		/* already there?? big mess if this happens */
		return (FALSE);
	   } else {
		maintn = modem->maintn_pending[maintn];
	   }
	}
	modem->maintn_pending[maintn] = newmaintn;
    }

    modem->maintn_pending[newmaintn] = MAI_NONE_PENDING;
    return (TRUE);
}

/*
 *  mai_remove_pending_maintn 
 *    -	Remove a maintenance item from the maintenance pending queue.
 */
static boolean mai_remove_pending_maintn (modem_info_t *modem,
						mai_cmd_t deq_maintn)
{
    mai_cmd_t maintn;
    boolean found;

    found = FALSE;
    if (modem->maintn_head == deq_maintn) {
	modem->maintn_head = modem->maintn_pending[deq_maintn];
	found = TRUE;
    } else {
	for (maintn = modem->maintn_head;
			maintn != MAI_NONE_PENDING;
			maintn = modem->maintn_pending[maintn]) {
	    if (modem->maintn_pending[maintn] == deq_maintn) {
		modem->maintn_pending[maintn] =
			modem->maintn_pending[deq_maintn];
		found = TRUE;
		break;
	    }
	}
    }
    if (found) {
	modem->maintn_pending[deq_maintn] = MAI_NONE_PENDING;
    }
    return (found);
}

/*
 *  mai_check_maintn_process_ready
 *    -	Check if the maintenance process exists, and if not, start it.
 */
static void mai_check_maintn_process_ready (void)
{
    if (modem_pool->mai_maintn_cnt && modem_pool->mai_maintn_process_pid == 0) {
	modem_pool->mai_maintn_process_pid = process_create(mai_maintn_process,
			"MAI Action Process", NORMAL_STACK, PRIO_NORMAL);
    }
}

/*
 *  mai_set_modem_poweron 
 *    -	Enqueue poweron request to the maintenance process.
 */
static void mai_set_modem_poweron (modem_info_t *modem)
{

    mai_enqueue_pending_maintn(modem, MAI_PENDING_PWRON);
    if (!(modem->mai_statbits & MAI_MAINTN_PENDING)) {
	process_enqueue(modem_pool->mai_command_queue, &modem->rsp_info);
	modem->mai_statbits |= MAI_MAINTN_PENDING;
	modem_pool->mai_maintn_cnt++;
	mai_check_maintn_process_ready();
    }
}

/*
 *  mai_line_init 
 *    -	Called during line initialization phase.  At this point, all
 *	feature boards have been found and the tty lines should be
 *	ready.  Initialize global queues and start to prepare the
 *	modems.
 */
void mai_line_init (void)
{
    modem_info_t *modem;
    modem_slot_t *slotinfo;
    int	slot, port;

    modem_pool->mai_command_queue = create_watched_queue("MAI Cmd Queue", 0, 0);
    list_create(&modem_pool->mai_tmolist, 0, "MAI:Time Out List", 0);
    /*
     * Initially keep the modems in reset state until the maintenance
     * process can deal with them.   But if its an unknown vendor type
     * just let it come out of reset.
     */
    for (slot = 0; slot < modem_pool->max_slots; slot++) {
	if (modem_pool->modem_slot[slot] != NULL) {
	    slotinfo = modem_pool->modem_slot[slot];
	    for (port = 0; port < slotinfo->total_modem_num; port++) {
		modem = slotinfo->modem_info[port];
                reg_invoke_modem_mgmt_oob_operation((void *)modem,OOB_PORT_OFF);
		if (modem == NULL) {
		    continue;
		}
		if (modem->do_init_ftn) {
		    csm_set_busy_status(modem);
		    csm_set_modem_reset_status(modem);
		    (*modem->do_init_ftn)(modem);
		} else {
		    csm_clear_modem_reset_status(modem, FALSE);
		}
	    }
	}
    }
}

/*
 *  mai_set_poweron_b2b_test 
 *    -	If the start up test enabled, then enqueue poweron back-to-back
 *	request to the main maintenance process.
 */
static void mai_set_poweron_b2b_test (modem_info_t *modem)
{

    mai_enqueue_pending_maintn(modem, MAI_PENDING_PWRON_B2B);
    if (!(modem->mai_statbits & MAI_MAINTN_PENDING)) {
	process_enqueue(modem_pool->mai_command_queue, &modem->rsp_info);
	modem->mai_statbits |= MAI_MAINTN_PENDING;
	modem_pool->mai_maintn_cnt++;
	mai_check_maintn_process_ready();
    }
}

/*
 *  mai_enable_poweron_b2b_test
 *    -	Called from the parser command when enabling modem startup
 *	back-to-back test.  This will check to see if it is the in
 *	the system startup, and then enqueue the request.
 *	If the system is already configured, then the user is only
 *	enabling for the next reload.  In this case we do nothing.
 */
void mai_enable_poweron_b2b_test (void)
{
    modem_info_t *modem;
    modem_slot_t *slotinfo;
    int	slot, port;

    if (!system_configured) {
	for (slot = 0; slot < modem_pool->max_slots; slot++) {
	    slotinfo = modem_pool->modem_slot[slot];
	    if (slotinfo == NULL) {
		continue;
	    }
	    slotinfo->mai_pwron_b2b_total = 0;
	    slotinfo->mai_pwron_b2b_good_held = 0;
	    queue_init(&slotinfo->mai_pwron_b2b_good_queue,
				modem_pool->max_slots * MAX_MODEM_PER_SLOT);
	    queue_init(&slotinfo->mai_pwron_b2b_recheck_queue,
				modem_pool->max_slots * MAX_MODEM_PER_SLOT);

	    for (port = 0; port < slotinfo->total_modem_num; port++) {
		modem = slotinfo->modem_info[port];
		if (modem) {
		    slotinfo->mai_pwron_b2b_total++;
		    modem->mai_statbits &=
				~(MAI_FAILED_LAST_B2B | MAI_PWRON_B2B_SUCCESS);
		    mai_set_poweron_b2b_test(modem);
		}
	    }
	    slotinfo->mai_pwron_b2b_undetermined =
				slotinfo->mai_pwron_b2b_max_modems =
				slotinfo->mai_pwron_b2b_total;
	}
    }
}

/*
 *  mai_restart_modem
 *    -	Called whenever anybody needs to re-do modem initialization.
 *	(for example, after modem is reset).
 */
void mai_restart_modem (modem_info_t *modem)
{
    if (modem->do_init_ftn) {
	(*modem->do_init_ftn)(modem);
    }
}

/*
 * mai_start_maintn
 *    - Initiate an maintenance action requested by the user.
 *      Because the  maintenance action is queued and will be activated
 *      after the other maintenance actions are complete.
 */
static void mai_start_maintn (modem_info_t *modem, mai_cmd_t maintn)
{
   /*
    * If there is another pending maintenance action, then return.
    */
    if (modem->maintn_head == maintn) {

	/*
	 * At the front of the maintenance action queue.
	 * Need to trigger the maintenance action.
	 */
	if (CSM_IS_BUSY_OUT(modem)) {
	    if (!(modem->mai_statbits & MAI_MAINTN_PENDING)) {
		process_enqueue(modem_pool->mai_command_queue,&modem->rsp_info);
		modem->mai_statbits |= MAI_MAINTN_PENDING;
		modem_pool->mai_maintn_cnt++;
		modem->mai_state = MAI_IDLE;
		return;
	    }
	} else {
	    modem->mai_statbits |= MAI_MAINTN_PENDING;
	    modem_pool->mai_maintn_cnt++;
	    modem->mai_state = MAI_IDLE;
	    if (csm_set_busy_out_status(modem, modem_pool->mai_command_queue,
						&modem->rsp_info)) {
		process_enqueue(modem_pool->mai_command_queue,&modem->rsp_info);
		return;
	    }
	}
    }
    
    /* 
     * Print a message that the operation will be delayed except for
     * the modem reset request which is not user initiated, but
     * initiated internally.
     */

    if(maintn != MAI_PENDING_RESET) {
      printf("\nModem %d/%d is currently busy, %s will be delayed. ",
	     CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem), 
	     mai_maintn_strings[maintn]);
    }
}

/*
 *  mai_recover_modem_from_failure 
 *    - Internal request to recover modem from failure by
 *      resetting the modem. Enqueue modem reset
 *      request to the main maintenance process.
 */
boolean mai_recover_modem_from_failure (modem_info_t *modem_info)
{
 
  if(modem_info) {
 
    /* If modem is already held in reset, no need to queue our request */
    if (CSM_IS_MODEM_RESET(modem_info)) {
      return(FALSE);
    }
    mai_enqueue_pending_maintn(modem_info, MAI_PENDING_RESET);
    mai_start_maintn(modem_info, MAI_PENDING_RESET);
    mai_check_maintn_process_ready();
    return(TRUE);
  }
  return(FALSE);

}

/*
 *  mai_busyout_a_modem
 *    -	Called through the CLI to implement user request to busyout a
 *	modem.  The busyout needed to be managed through here because
 *	the maintenance actions like download and back-to-back test
 *	required busying out through the CSM; however the CSM does not
 *	keep track of the source of the busyouts.  Thus at the end of
 *	the maintenance operation, the user specified busyout state can
 *	be preserved.
 */
void mai_busyout_a_modem (modem_info_t *modem, boolean busyout_requested)
{
    if (busyout_requested) {
	/* If modem is disable, nothing can be done */
	if (!CSM_IS_BUSY_OUT(modem)) {
	    if (!csm_set_busy_out_status(modem,
			modem_pool->mai_command_queue, &modem->rsp_info)) {
		modem->mai_statbits |= MAI_MAINTN_PENDING;
		modem_pool->mai_maintn_cnt++;
	    }
	}
	modem->mai_statbits |= MAI_USER_BUSYOUT;
	mai_check_maintn_process_ready();
    } else {
	modem->mai_statbits &= ~MAI_USER_BUSYOUT;
	if (modem->maintn_head == MAI_NONE_PENDING &&
				modem->mai_state == MAI_IDLE) {
	    csm_clear_busy_out_status(modem);
	}
    }
}

/*
 * mai_back2back_test
 *    -	Called through the CLI to implement the test modem back-to-back.
 *	Will basically check for the valid of the arguments, and fire
 *	off the requests to the maintenance process.
 */
boolean mai_back2back_test (modem_info_t *modem1, modem_info_t *modem2)
{
 
    char userstr[MAI_USER_INPUT_STRLEN];
    int num;
    boolean m1_flag, m2_flag;

    m1_flag = FALSE;
    m2_flag = FALSE;

    /* First, make sure both modems are valid */
    if (modem1 == NULL || modem2 == NULL) {
       printf("\n%%ERROR: Modem(s) does not exist. "); 
       return (FALSE); 
    }

    if (CSM_MODEM_SLOT(modem1) != CSM_MODEM_SLOT(modem2)) {
       printf("\n%%ERROR: Cannot test modems from different slots. "); 
       return (FALSE); 
    }

    if (modem1 == modem2) {
       printf("\n%%ERROR: Cannot test on the same modem. "); 
       return (FALSE); 
    }

    if (modem1->alt_modem) {
	m1_flag = TRUE;
	printf("\n%%ERROR: Modem %d/%d currently being back-to-back tested. ",
			CSM_MODEM_SLOT(modem1), CSM_MODEM_PORT(modem1));
    }
    if (modem2->alt_modem) {
	m2_flag = TRUE;
	printf("\n%%ERROR: Modem %d/%d currently being back-to-back tested. ",
			CSM_MODEM_SLOT(modem2), CSM_MODEM_PORT(modem2));
    }

    /* If modem is disable, nothing can be done */
    if (CSM_IS_MODEM_RESET(modem1)) {
	m1_flag = TRUE;
	printf("\n%%ERROR: Modem %d/%d is held in reset. ",
			CSM_MODEM_SLOT(modem1), CSM_MODEM_PORT(modem1));
    }
    if (CSM_IS_MODEM_RESET(modem2)) {
	m2_flag = TRUE;
	printf("\n%%ERROR: Modem %d/%d is held in reset. ",
			CSM_MODEM_SLOT(modem2), CSM_MODEM_PORT(modem2));
    }

    if (m1_flag || m2_flag) {
	return (FALSE);
    }

    flush();
    printf("\nRepetitions (of 10-byte packets) [1]: ");
    if (!rdtty(userstr, MAI_USER_INPUT_STRLEN-1)) {
	return (0);
    }
    num = atoi(userstr);
    if (num <= 0) {
	num = 1;
    }

    /*
     * Put the modem's in the maintenance action queue.
     * Enqueue should not fail if not already in b2b, alt_modem
     * should have told us this. 
     */
    mai_enqueue_pending_maintn(modem1, MAI_PENDING_B2B);
    mai_enqueue_pending_maintn(modem2, MAI_PENDING_B2B);

    /* setup parameters */
    /* pair off the modems */
    modem1->alt_modem = modem2;
    modem2->alt_modem = modem1;
    modem1->mai_b2b_max = modem2->mai_b2b_max = num;
    modem1->mai_b2b_good = modem2->mai_b2b_good = 0;
    modem1->mai_b2b_cnt = modem2->mai_b2b_cnt = 0;
    /* setup caller */
    modem1->mai_statbits |= MAI_B2B_ATD;
    /* setup answer modem */
    modem2->mai_statbits |= MAI_B2B_ATA;

    mai_start_maintn(modem1, MAI_PENDING_B2B);
    mai_start_maintn(modem2, MAI_PENDING_B2B);
    mai_check_maintn_process_ready();
    return (TRUE);
}

/*
 *  mai_get_modem_group_list
 *    -	Scan the group async list and identify all the members/modems
 *	in the group.
 */
static int mai_get_modem_group_list (int group, mai_range_t *modemlist,
					int maxlist)
{
    int slot;
    int port;
    hwidbtype *hwidb = NULL;
    async_member_info *member;
    queuetype *memberQ = NULL;
    modem_group_t *tmp;
    int queue_size = 0, count, listcnt = 0;
    mai_range_t *modemlistptr;

    /* 
     * find the group hwidb
     */
    tmp = modem_group_list;
    while (tmp && (tmp->hwidb->unit < group)) {
	tmp = tmp->next;
    }
    if (tmp && (tmp->hwidb->unit == group)) {
	hwidb = tmp->hwidb;
	/*
	 * make sure the group isn't deleted before proceeding
	 */
	if (hwidb && !(hwidb->status & IDB_DELETED)) {
	    /* 
	     * traverses memberQ and retrieve modem info to get slot/port
	     */
	    memberQ = &hwidb->firstsw->async_groupQ;
	    queue_size = QUEUESIZE(memberQ);
	    modemlistptr = modemlist;
	    if (queue_size == 0) {
		printf("\nModem Group %d is empty. ", group);
	    }
            member = memberQ->qhead;
            for (count = 0; (member && (count < queue_size)); count++,
						member = member->next) {
                modem_info_t *modem;

                modem = idb_get_hwsb(member->hwidb, HWIDB_SB_MODEM_INFO);
                if (modem) {
                    slot = CSM_MODEM_SLOT(modem);
                    port = CSM_MODEM_PORT(modem);
		    /*
		     * check to see if this modem follows is the next one 
		     * after the previous one; if so, just bump the previous
		     * end slot/port include this modem.
		     */
		    if (modemlistptr > modemlist) {
			if ((modemlistptr - 1)->endslot == slot &&
				((modemlistptr - 1)->endport + 1) == port) {
			    (modemlistptr - 1)->endport++;
			    continue;
			}
		    }
		    modemlistptr->startslot = modemlistptr->endslot = slot;
		    modemlistptr->startport = modemlistptr->endport = port;
		    modemlistptr++;
		    listcnt++;
		    if (listcnt == maxlist) {
			return (maxlist);
		    }
                }
            }
	}
    }
    return (listcnt);
}

/*
 *  mai_get_modem_list
 *    -	Get a list of modems from the user.
 *	The user can specify a specific modem, a range of modems,
 *	modems from an async group, or all modems.
 *    -	Returns the count of modem ranges (start and end), each range is
 *	put in the modemlist array.
 */
static int mai_get_modem_list (mai_range_t *modemlist, int maxlist)
{
    char	userstr[MAI_USER_INPUT_STRLEN];
    char	*tailptr;
    mai_range_t *modemlistptr, *maxlistptr;
    modem_slot_t *slotinfo;
    int	        slot, port;
    int		group, num; 
    boolean	range;
    
    printf("\nModem Numbers (<slot>/<port>[-<slot>/<port>] |"
				" group <number> | all)? ");
    flush();
    if (!rdtty(userstr, MAI_USER_INPUT_STRLEN-1)) {
	printf("\n%%ERROR: Bad Input. ");
	return (0);
    }
    
    modemlistptr = modemlist;
    maxlistptr = modemlist + maxlist;
    tailptr = userstr;
    range = FALSE;
    
    while (*tailptr != 0) {
	
	/* Get rid of filler white space */
	if (!range) {
	    tailptr = deblank(tailptr);
	}
	slot = port = 0;
	if (!isdigit(*tailptr)) {
	    if (null(tailptr)) {
		break;
	    }
	    if (modemlistptr == modemlist && !range) {

		/*
		 * All modems?
		 */
		if (strncasecmp("ALL", tailptr, 3) == 0  && null(tailptr + 3)) {
		    /* find first and last valid modems */
		    for (slot = 0; slot < modem_pool->max_slots; slot++) {
			slotinfo = modem_pool->modem_slot[slot];
			if (slotinfo) {
			    for (port = 0; port < slotinfo->total_modem_num;
								port++) {
				if (slotinfo->modem_info[port]) {
				    break;
				}
			    }
			    if (port < slotinfo->total_modem_num) {
				modemlist->startslot = slot;
				modemlist->startport = port;
				break;
			    }
			}
		    }
		    if (slot == modem_pool->max_slots) {
			printf("\n%%ERROR: No modems. ");
			return (0);
		    }
		    for (slot = modem_pool->max_slots - 1; slot >= 0; slot--) {
			if (modem_pool->modem_slot[slot]) {
			    modemlist->endslot = slot;
			    modemlist->endport =
				modem_pool->modem_slot[slot]->total_modem_num-1;
			    return (1);
			}
		    }
		    return (0);
		}

		/*
		 * Group async?
		 */
		else if (strncasecmp("GROUP", tailptr, 5) == 0) {
		    /* get group number */
		    group = num = 0;
		    tailptr += 5;
		    tailptr = deblank(tailptr);
		    while (isdigit(*tailptr)) {
			group = group * 10 + (*tailptr++ - '0');
		    }
		    if (null(tailptr)) {
			/* find group members */
			num = mai_get_modem_group_list(group,modemlist,maxlist);
			if (num == 0) {
			    printf("\n%%ERROR: No modems in this group %d. ",
					num);
			}
		    }
		    return (num);
		}
	    }
	    printf("\n%%ERROR: Use (<slot>/<port>[-<slot>/<port>] |"
					" group <number> | all). ");
	    return (0);
	}

	/*
	 * get slot/port number
	 */
	while (isdigit(*tailptr)) {
	    slot = slot * 10 + (*tailptr++ - '0');
	    if (slot >= modem_pool->max_slots) {
		printf("\n%%ERROR: Maximum slot number is %d. ",
					modem_pool->max_slots-1);
		return (0);
	   }
	}
	if (*tailptr++ != '/') {
	    printf("\n%%ERROR: Modem number should be <slot>/<port>. ");
	    return (0);
	}
	if (isdigit(*tailptr)) {
	    while (isdigit(*tailptr)) {
		port = port * 10 + (*tailptr++ - '0');
		if (port >= MAX_MODEM_PER_SLOT) {
		    printf("\n%%ERROR: Maximum port number is %d. ",
						MAX_MODEM_PER_SLOT - 1);
		    return (0);
		}
	    }
	} else {
	    printf("\n%%ERROR: Modem number should be <slot>/<port>. ");
	    return (0);
	}

	/* is this a valid slot/port */
	if (modem_pool->modem_slot[slot] == NULL ||
		modem_pool->modem_slot[slot]->modem_info[port] == NULL) {
	    printf("\n%%ERROR: Modem %d/%d does not exist. ", slot, port);
	    return (0);
	}

	if (range) {
	    /* check the range of modems */
	    /* should be a higher modem number */
	    if (slot > modemlistptr->startslot ||
				(slot == modemlistptr->startslot
				&& port > modemlistptr->startport)) {
		modemlistptr->endslot = slot;
		modemlistptr->endport = port;
	    } else {
		printf("\n%%ERROR: List modems in ascending order. ");
		return (0);
	    }
	    if (++modemlistptr == maxlistptr) {
		printf("\nWarning: List too big, truncated at %d/%d. ",
					slot, port);
		return (maxlist);
	    }
	    range = FALSE;
	} else {
	    if (modemlistptr != modemlist && (slot < (modemlistptr-1)->endslot
				|| (slot == (modemlistptr-1)->endslot &&
				port < (modemlistptr-1)->endport))) {
		printf("\n%%ERROR: List modems in ascending order. ");
		return (0);
	    }
	    modemlistptr->startslot = slot;
	    modemlistptr->startport = port;
	    if (*tailptr == '-') {
		range = TRUE;
		tailptr++;
	    } else if (++modemlistptr == maxlistptr) {
		printf("\nWarning: List too big, truncated at %d/%d. ",
					slot, port);
		return (maxlist);
	    }
	}
    } /* while (*tailptr != 0) */

    if (range) {
	printf("\ERROR: Modem number range should be"
			"<slot>/<port>-<slot>/<port>. ");
	return (0);
    }

    return (modemlistptr - modemlist);
}

/*
 * mai_read_to_buffer
 *    -	This function is a call back function to the tftp/rcp/flash read
 *	transfer functions, to supply empty data buffers.
 */
static boolean mai_read_to_buffer (uchar **buf, long *bytesleft, long len,
				void *opaque_args)
{
    mai_dl_info_t *dlp;
    flash_copy_parms_t *fparms;
    mai_dl_buffer_t *dlbufp;
    long bytesleft_in_buf;
    
    /* nothing transferred */
    if (len == 0) {
	return(TRUE);	
    }

    dlp = (mai_dl_info_t *)opaque_args;
    fparms = &dlp->fparms;
    dlbufp = (mai_dl_buffer_t *)dlp->bufqueue.qtail;
    
    /* Total length read so far */
    fparms->datalen += len;
    dlbufp->bytecnt += len;
    bytesleft_in_buf = MAI_MAX_BUFFSIZE - dlbufp->bytecnt;
    
    if (bytesleft_in_buf < dlp->bsize) {
	dlbufp = malloc_named(sizeof(mai_dl_buffer_t), "MAI DL Data Buffer");
	if (dlbufp == NULL) {
	     /* Insufficient Memory */
	    return (FALSE);
	}
	dlbufp->nextbuf = NULL;
	dlbufp->bytecnt = 0;
	enqueue(&dlp->bufqueue, dlbufp);
	*buf = dlbufp->data;
	/* 
	 * For TFTP, bytesleft is bytes left in the buffer, For RCP, it is
	 * bytes left to be read in the file.
	 */
	*bytesleft = MAI_MAX_BUFFSIZE;   
    } else {
	*buf = dlbufp->data + dlbufp->bytecnt;
    }

    return(TRUE);
}

/*
 * mai_decode_file_read_status
 *    -	Convert return codes from the tftp/rcp read functions, into
 *	a relevant display string.
 */
static char *mai_decode_file_read_status (int sts)
{
    switch (sts) {
	case ERROR_OK:
	    /* success */
	    return (NULL);

	case ERROR_NOMEM:
	    return ("No Memory");

	case ERROR_TIMEOUT:
	    return ("Time out");

	case ERROR_NOFILE:
	    return ("No Such File");

	case ERROR_ABORT:
	    return ("Aborted");
	case ERROR_TOOBIG:
	    return ("Too big");
	default:
	    return ("Unknown Failure");
    }
}

/*
 *  mai_copy_flash_buffer 
 *    -	Initiate a transfer of the file from flash to main memory.
 */
static boolean mai_copy_flash_buffer (mai_dl_info_t *dlp)
{
    flash_copy_parms_t *fparms;

    fparms = &dlp->fparms;

    if (reg_invoke_get_num_flash_part() <= 1) {
	if (!get_source_file(fparms->sfname, (FLASH_FILE_NAME_LEN-1),
					"copy", TRUE)) {
	    return (FALSE);
	}
    } else {
	/* 
	 * multiple partions we need to display and let user choose
	 * partition number as well as filename
	 */
	if (!reg_invoke_multi_part_get_file_name(fparms->sfname)) {
	    return (FALSE);
	}
    }
    if (!reg_invoke_file_exists(FILE_ACCESS_FLASH, fparms->sfname)) {
	printf("\nFile not found ");
	return (FALSE);
    }
    flush();

    fparms->datalen = 0;
    fparms->buflen = MAI_MAX_BUFFSIZE;
    dlp->bsize = TFTP_MAXDATA;
    
    fparms->sts = reg_invoke_flash_block_server(fparms->sfname,
			((mai_dl_buffer_t *)(dlp->bufqueue.qhead))->data,
			&fparms->buflen,
			fparms->verbose,
			mai_read_to_buffer,
			(void *)dlp);

    if (fparms->verbose) {
	if (fparms->sts) {
	    printf("\n[OK - %d/%d bytes]\n", fparms->datalen, fparms->buflen);
	} else {
	    printf("\n%%Flash Failed: %s. ", fparms->sfname);
	}
    }
    return (fparms->sts);
}

/*
 *  mai_copy_net_buffer 
 *    -	Initiate a transfer of the file from tftp/rcp to main memory.
 */
static boolean mai_copy_net_buffer (mai_dl_info_t *dlp, int ftype)
{
    flash_copy_parms_t *fparms;
    filetype	net_file;		
    char	*prtstr;

    fparms = &dlp->fparms;
    fparms->copy_option = ftype;

    /*
     * Get IP address of net server.
     */
    if (fparms->addr.ip_addr == 0) {
       address_copy(&fparms->addr, &mai_hostip);
    }
    if (!flash_copy_get_server(&fparms->addr, (char *)fparms->buff,
			       mai_hostip, fparms->prompt)) {
	/* Unknown host; give up */
	return (FALSE);
    }
    /* save host address for next time default */
    address_copy(&mai_hostip, &fparms->addr); 

    /*
     * Get file names. If no file name entered, bail out.
     */
    if (!flash_copy_file_name(fparms->sfname, (FLASH_FILE_NAME_LEN - 1),
		       fparms->prompt ? "Source file name? " : NULL, TRUE)) {
	fparms->snmp_sts = SNMP_INVALID_SRC_FILE;
	return (FALSE);
    }

    /*
     * Before we start transfer, check if the file is accessible
     * from the server.
     */
    if (fparms->verbose) {
	printf("\nAccessing file '%s' on %s... ", fparms->sfname, 
		name_addr2string(&fparms->addr));
	flush();
    }

    net_file.filename = (char *)&fparms->sfname;
    net_file.proto = fparms->copy_option;
    address_copy(&net_file.ps.tuba.boothost, &fparms->addr);
    net_file.ps.tuba.display = (fparms->verbose ? TRUE : FALSE);
    net_file.ps.tuba.username = fparms->rcp_username;
    /*
     * Now do the actual TFTP/RCP transfer into main memory.
     * No need to do a dummy transfer because the whole file is
     * always read into main memory first before starting download
     * to modems.
     */
    fparms->datalen = 0;
    fparms->buflen = MAI_MAX_BUFFSIZE;
    dlp->bsize = TFTP_MAXDATA;
    
    fparms->sts = reg_invoke_file_read(
		net_file.proto, &net_file,
		((mai_dl_buffer_t *)(dlp->bufqueue.qhead))->data,
		&fparms->buflen, fparms->verbose,
		mai_read_to_buffer,
		(void *)dlp);
    if (fparms->sts != ERROR_OK) {
	/* decode the return value */
	prtstr = mai_decode_file_read_status(fparms->sts);
	if (prtstr != NULL) {
	    if (fparms->verbose) {
		if (ftype == FILE_ACCESS_TFTP) {
		    printf("\n%%TFTP Failed: %s. ", prtstr);
		} else {
		    printf("\n%%RCP Failed: %s. ", prtstr);
		}
	    }
	    return (FALSE);
	}
    }

    return (TRUE);
}

/*
 *  mai_download_to_modem 
 *    -	Called through the CLI to implement the copy-to-modem function.
 *	This function will basically get a list of modems to be 
 *	downloaded, get a description of the file to download, either 
 *	from a tftp server, rcp host, or the local flash, read that
 *	file into system memory, and then fire off the requests to
 *	the maintenance process to download the file to the modem.
 */
static void mai_download_to_modem (uint dlsrc)
{
    mai_range_t *modemlist, *listptr, *endlist;
    ushort	slot, port, endslot, endport;
    boolean	result;
    modem_info_t *modem;
    modem_slot_t *slotinfo;
    mai_dl_info_t *dlp;
    flash_copy_parms_t *fparms;
    mai_dl_buffer_t *bptr;
    int listcnt;

    /* Allocate one data buffer first to see if enough memory */
    bptr = malloc_named(sizeof(mai_dl_buffer_t), "MAI DL Data Buffer");
    if (bptr == NULL) {
	/* Insufficient Memory */
	return;
    }

    dlp = malloc_named(sizeof(mai_dl_info_t), "MAI DL Info");
    if (dlp == NULL) {
	/* Insufficient Memory */
	free(bptr);
	return;
    }

    /* Allocate modem list array */
    modemlist = malloc_named(sizeof(mai_range_t)*MAX_MODEM_PER_SLOT * 
			modem_pool->max_slots, "MAI DL Modemlist");
    if (modemlist == NULL) {
	/* Insufficient Memory */
	free(bptr);
	free(dlp);
	return;
    }
    bzero(dlp, sizeof(mai_dl_info_t));

    queue_init(&dlp->bufqueue, MAI_MAX_DLBUFFERS);
    bptr->bytecnt = 0;
    bptr->nextbuf = NULL;
    enqueue(&dlp->bufqueue, bptr);

    /* Get a list of modems */
    listcnt = mai_get_modem_list(modemlist, MAX_MODEM_PER_SLOT *
			modem_pool->max_slots);
    if (listcnt == 0) {
	free(modemlist);
	mai_free_dl_info(dlp);
	return;
    }

    fparms = &dlp->fparms;
    strcpy(fparms->devnm, "flash");
    fparms->prompt  = FILE_FLAGS_PROMPT;
    fparms->verbose = FILE_FLAGS_VERBOSE;

    /* Retrieve the file into a memory buffer(s) */
    result = FALSE;
    switch (dlsrc) {

    case COPY_TFTP_MODEM:

	result = mai_copy_net_buffer(dlp, FILE_ACCESS_TFTP);
	break;

    case COPY_RCP_MODEM:
	fparms->rcp_username = NULL;
	result = mai_copy_net_buffer(dlp, FILE_ACCESS_RCP);
	break;

    case COPY_FLASH_MODEM:

	result = mai_copy_flash_buffer(dlp);
	break;

    }

    if (!result) {
	free(modemlist);
	mai_free_dl_info(dlp);
	return;
    }

    dlp->modemcnt = 0;

    for (listptr = modemlist, endlist = listptr + listcnt;
				listptr < endlist; listptr++) {

	slot = listptr->startslot;
	port = listptr->startport;
	endslot = listptr->endslot;
	endport = listptr->endport;
	slotinfo = modem_pool->modem_slot[slot];
	modem = slotinfo->modem_info[port];

	while (modem) {

	    /* If modem is disable, nothing can be done */
	    if (CSM_IS_MODEM_RESET(modem)) {
		printf("\n%%ERROR: Modem %d/%d is held in reset. ", slot, port);
	    }
	    /* If downloading is already specified, then skip */
	    else if (modem->download_info != NULL ||
			!mai_enqueue_pending_maintn(modem, MAI_PENDING_DL)) {
		printf("\n%%ERROR: Modem %d/%d currently being downloaded. ",
							slot, port);
	    } else {
		dlp->modemcnt++;
		modem->download_info = dlp;
		mai_start_maintn(modem, MAI_PENDING_DL);
	    }

	    /* Find next modem */
	    modem = NULL;
	    while (slot < endslot || (slot == endslot && port < endport)) {
		if (++port >= slotinfo->total_modem_num) {
		    port = 0;
		    while (++slot <= endslot && (slotinfo =
				modem_pool->modem_slot[slot]) == NULL) {
			;
		   }
		   if (slot > endslot) {
			break;
		   }
		}
		modem = slotinfo->modem_info[port];
		if (modem) {
		    break;
		}
	    }
	}
    } 

    free(modemlist);
    if (dlp->modemcnt == 0) {
	mai_free_dl_info(dlp);
    } else {
	mai_check_maintn_process_ready();
    }
}

/*
 *  mai_command_parser 
 *    -	Parser function for the copy-to-modem commands.
 */
void mai_command_parser (parseinfo *csb)
{

    switch (csb->which) {

    case COPY_FLASH_MODEM:
    case COPY_TFTP_MODEM:
    case COPY_RCP_MODEM:

	mai_download_to_modem(csb->which);
	break;

    default:
	errmsg(&msgsym(BADMODEM, MODEM), 0, 0, "CMD_PARSER: software error");
	return;
    }
}

/*
 * mai_pwron_b2b_fail_modem
 *    -	Set the modem as having failed poweron back2back test.
 */
static void mai_pwron_b2b_fail_modem (modem_info_t *modem)
{
    modem_slot_t *slotinfo;

    slotinfo = modem_pool->modem_slot[CSM_MODEM_SLOT(modem)];
    csm_set_bad_modem_status(modem);
    errmsg(&msgsym(BADMODEM, MODEM),
		CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
		"startup back-to-back test");
    /* turn off led if all modems on this module failed */
    modem->mai_statbits |= MAI_FAILED_LAST_B2B;
    if ((--slotinfo->good_modem_count[modem->module_num]) <= 0) {
	slotinfo->modem_reg->mdb_control1 &=
			~(MDB_CNTL1_MDB1_LED << modem->module_num);
    }
    if (mai_next_maintns(modem)) {
	process_enqueue(modem_pool->mai_command_queue,&modem->rsp_info);
    }
}

/*
 * mai_b2b_complete
 *    -	This function is a general function which will perform the
 *	clean up processing for the back-to-back test.
 */
static boolean mai_b2b_complete (modem_info_t *modem, boolean modem_ok)
{
    modem_info_t *alt_modem, *next_modem;
    modem_q_info_t *qlink;
    modem_slot_t *slotinfo;
    int ix;
    boolean alt_signal_queued;

    alt_signal_queued = FALSE;
    alt_modem = modem->alt_modem;
    slotinfo = modem_pool->modem_slot[CSM_MODEM_SLOT(modem)];

    /* drop dtr to hang up modem */
    tty_dtrstate(modem->modem_tty, FALSE);
    tty_noservicehook(modem->modem_tty);
    TIMER_STOP(modem->dallytimer);

    modem->alt_modem = NULL;
    modem->mai_statbits &= ~(MAI_B2B_ATD | MAI_B2B_ATA | MAI_RX_SIGNAL_REQ);

    if (alt_modem) {
	alt_modem->alt_modem = NULL;
	alt_modem->mai_statbits &= ~(MAI_B2B_ATD | MAI_B2B_ATA);

	/* check if the other modem has started yet */
	if ((alt_modem->maintn_head != MAI_PENDING_B2B &&
    			alt_modem->maintn_head != MAI_PENDING_PWRON_B2B) ||
			alt_modem->mai_state == MAI_IDLE) {
	    /* not started yet, so just dequeue it */
	    mai_remove_pending_maintn(alt_modem, MAI_PENDING_B2B);
	    errmsg(&msgsym(BADMODEMS, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			CSM_MODEM_SLOT(alt_modem), CSM_MODEM_PORT(alt_modem),
			modem->cur_txptr);
	    return (mai_next_maintns(modem));
	}

	/* drop dtr to hang up alternate modem */
	tty_dtrstate(alt_modem->modem_tty, FALSE);
	tty_noservicehook(alt_modem->modem_tty);
	TIMER_STOP(alt_modem->dallytimer);

	/* need to tidy up the other modem, which may be in any */
	/* state of the test, thus check all combinations */
	if (alt_modem->mai_state != MAI_B2B_DATA) {
	    alt_signal_queued = mai_abort_modem_send(alt_modem);
	} else if (alt_modem->mai_state == MAI_B2B_DATA &&
			(alt_modem->mai_statbits & MAI_RX_SIGNAL_REQ)) {
	    alt_signal_queued = TRUE;
	}

	/* if there is already an response message queued */
	/* need to mark this so mai_next_maintn will not */
	/* prematurely cleanup until we process it */
	if (alt_signal_queued) {
	    alt_modem->mai_statbits |= MAI_STRAY_QUEUE_EVENT;
	}
	alt_modem->mai_statbits &= ~MAI_RX_SIGNAL_REQ;
	csm_clear_modem_back2back_test(modem, alt_modem);

	if (modem->maintn_head == MAI_PENDING_B2B) {
	    if (modem_ok) {
		csm_clear_bad_modem_status(modem);
		csm_clear_bad_modem_status(alt_modem);
		modem->mai_statbits &= ~MAI_FAILED_LAST_B2B;
		alt_modem->mai_statbits &= ~MAI_FAILED_LAST_B2B;
		errmsg(&msgsym(B2BMODEMS, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem), 
			CSM_MODEM_SLOT(alt_modem), CSM_MODEM_PORT(alt_modem),
			modem->mai_b2b_good + alt_modem->mai_b2b_good,
			modem->mai_b2b_cnt + alt_modem->mai_b2b_cnt);
	    } else {
		/* don't mark bad because don't know which of two is culprit */
		alt_modem->mai_statbits |= MAI_FAILED_LAST_B2B;
		modem->mai_statbits |= MAI_FAILED_LAST_B2B;
		errmsg(&msgsym(BADMODEMS, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			CSM_MODEM_SLOT(alt_modem), CSM_MODEM_PORT(alt_modem),
			modem->cur_txptr);
	    }

	    if (mai_next_maintns(alt_modem) && !alt_signal_queued) {
		process_enqueue(modem_pool->mai_command_queue,
						&alt_modem->rsp_info);
	        alt_modem->mai_statbits &= ~MAI_STRAY_QUEUE_EVENT;
	    }
	    return (mai_next_maintns(modem));
	}

	/* first complete clean up */
	modem->mai_state = alt_modem->mai_state = MAI_B2B_CONNECTING;
	modem->mai_step = alt_modem->mai_step = MAI_STP_BEGIN;

    } else {
	/* must be poweron tests, just mark it */
	/* to break out of the b2b testing */
	slotinfo->mai_pwron_b2b_next_modem = NULL;
    }

    for (ix = 0; ix < 2 ; ix++, modem = alt_modem) {
	if (modem == NULL) {
	    continue;
	}
	qlink = NULL;
	if (modem_ok || (modem->mai_statbits & MAI_PWRON_B2B_SUCCESS)) {
	    if (!(modem->mai_statbits & MAI_PWRON_B2B_SUCCESS)) {
		modem->mai_statbits = (modem->mai_statbits &
				~MAI_FAILED_LAST_B2B) | MAI_PWRON_B2B_SUCCESS;
		--slotinfo->mai_pwron_b2b_undetermined;
		/* just assume I need to hold on to this one */
		slotinfo->mai_pwron_b2b_good_held++;
		if (mai_debug) {
		    buginf("\nMAI(%d/%d): pwronb2b good. ",
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem));
		}
	    }
	    if (QUEUESIZE(&slotinfo->mai_pwron_b2b_recheck_queue)) {
		/* get modem for second test (at bottom of this ftn) */
		qlink = dequeue(&slotinfo->mai_pwron_b2b_recheck_queue);
		if (mai_debug) {
		    buginf("\nMAI(%d/%d): pwronb2b dequeue recheck. ",
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem));
		}
	    } else if (slotinfo->mai_pwron_b2b_undetermined >=
				slotinfo->mai_pwron_b2b_good_held) {
		/* need to save good modem for testing against other modems */
		enqueue(&slotinfo->mai_pwron_b2b_good_queue, 
					&modem->pwron_b2b_qlink);
		if (mai_debug) {
		    buginf("\nMAI(%d/%d): pwronb2b enqueue good. ",
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem));
		}
	    } else {
		--slotinfo->mai_pwron_b2b_good_held;
		if (mai_next_maintns(modem)) {
		    process_enqueue(modem_pool->mai_command_queue,
						&modem->rsp_info);
		}
	    }
	} else if (modem->mai_statbits & MAI_FAILED_LAST_B2B) {
	    /* second failure is against known good modem, therefore its bad */
	    --slotinfo->mai_pwron_b2b_undetermined;
	    mai_pwron_b2b_fail_modem(modem);
	} else {
	    /* first time failure */
	    modem->mai_statbits |= MAI_FAILED_LAST_B2B;
	    if (QUEUESIZE(&slotinfo->mai_pwron_b2b_good_queue)) {
		qlink = dequeue(&slotinfo->mai_pwron_b2b_good_queue);
		if (mai_debug) {
		    buginf("\nMAI(%d/%d): pwronb2b 1st fail, dequeue good. ",
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem));
		}
	    } else {
		enqueue(&slotinfo->mai_pwron_b2b_recheck_queue, 
					&modem->pwron_b2b_qlink);
		if (mai_debug) {
		    buginf("\nMAI(%d/%d): pwronb2b 1st fail, enqueue recheck. ",
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem));
		}
		if (QUEUESIZE(&slotinfo->mai_pwron_b2b_recheck_queue) >=
				slotinfo->mai_pwron_b2b_max_modems) {
		    while (QUEUESIZE(&slotinfo->mai_pwron_b2b_recheck_queue)) {
			qlink = dequeue(&slotinfo->mai_pwron_b2b_recheck_queue);
			mai_pwron_b2b_fail_modem(qlink->modem_info);
	    	    }
		    slotinfo->mai_pwron_b2b_undetermined = 0;
		    return (mai_next_maintns(modem));
		}
	    }
	}
	if (qlink) {
	    next_modem = qlink->modem_info;
	    if (!csm_set_modem_back2back_test(modem, next_modem)) {
		if (mai_debug) {
		    buginf("\nMAI(%d/%d): pwronb2b fail switch (%d,%d) ",
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			CSM_MODEM_SLOT(next_modem), CSM_MODEM_PORT(next_modem));
		}
		mai_pwron_b2b_fail_modem(modem);
		mai_pwron_b2b_fail_modem(next_modem);
		/* one of these should have been a good modem */
		--slotinfo->mai_pwron_b2b_good_held;
		continue;
	    }
	    modem->alt_modem = next_modem;
	    next_modem->alt_modem = modem;

	    modem->mai_statbits |= MAI_B2B_ATD;
	    next_modem->mai_statbits |= MAI_B2B_ATA;
	    modem->mai_retry = next_modem->mai_retry = 0;
	    modem->mai_b2b_max = next_modem->mai_b2b_max = 1;
	    modem->mai_b2b_cnt = next_modem->mai_b2b_cnt = 0;
	    modem->mai_b2b_good = next_modem->mai_b2b_good = 0;

	    if (mai_debug) {
		buginf("\nMAI(%d/%d): pwronb2b retesting (%d,%d) ",
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			CSM_MODEM_SLOT(next_modem), CSM_MODEM_PORT(next_modem));
	    }
	    TIMER_START(modem->dallytimer, CSM_MODEM_RESET_MINIMUM_TIME);
	}
    }

    while ((QUEUESIZE(&slotinfo->mai_pwron_b2b_good_queue)) &&
					(slotinfo->mai_pwron_b2b_good_held >
					slotinfo->mai_pwron_b2b_undetermined)) {
	qlink = dequeue(&slotinfo->mai_pwron_b2b_good_queue);
	modem = qlink->modem_info;
	if (mai_debug) {
	    buginf("\nMAI(%d/%d): pwronb2b too many good, dequeue good. ",
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem));
	}
	--slotinfo->mai_pwron_b2b_good_held;
	if (mai_next_maintns(modem)) {
	    process_enqueue(modem_pool->mai_command_queue, &modem->rsp_info);
	}
    }
    return (FALSE);

}

/*
 * mai_abort_active_b2b_test
 *    -	This function will abort the back-to-back test on a modem
 *	which is currently executing the back-to-back test.
 */
static void mai_abort_active_b2b_test (modem_info_t *modem)
{
    modem_info_t *alt_modem;
    boolean signal_queued;

    if (modem->maintn_head != MAI_PENDING_B2B) {
	return;
    }

    alt_modem = modem->alt_modem;
    signal_queued = mai_abort_modem_send(modem);
    /* if there is already an response message queued */
    /* need to mark this so mai_next_maintn will not */
    /* prematurely cleanup until we process it */
    if (signal_queued) {
	modem->mai_statbits |= MAI_STRAY_QUEUE_EVENT;
    }
    if (modem->mai_state >= MAI_B2B_INIT &&
				modem->mai_state <= MAI_B2B_CONNECTING) {
	modem->cur_txptr = "connect aborted";
	if (mai_b2b_complete(modem, FALSE) && !signal_queued) {
	    process_enqueue(modem_pool->mai_command_queue, &modem->rsp_info);
	}
    } else if (modem->mai_state == MAI_B2B_DATA) {
	if (modem->mai_statbits & MAI_RX_SIGNAL_REQ) {
	    signal_queued = TRUE;
	    modem->mai_statbits |= MAI_STRAY_QUEUE_EVENT;
	}
	if (modem->mai_b2b_cnt > 0 || alt_modem->mai_b2b_cnt > 0) {
	    if (mai_b2b_complete(modem, TRUE) && !signal_queued) {
		process_enqueue(modem_pool->mai_command_queue,&modem->rsp_info);
	    }
	} else {
	    modem->cur_txptr = "aborted data transfer";
	    if (mai_b2b_complete(modem, FALSE) && !signal_queued) {
		process_enqueue(modem_pool->mai_command_queue,&modem->rsp_info);
	    }
	}
    }
    modem->mai_statbits &= ~MAI_STRAY_QUEUE_EVENT;
}

/*
 *  mai_clear_maintns
 *    -	This will clear the pending or currently active back-to-back
 *	test, and the pending copy-to-modem request.  Other maintenance
 *	items can not be cleared.
 *    - This is called through one of the CLI.
 *    -	Returns TRUE, if there are no maintenance stuff left.
 */
boolean mai_clear_maintns (modem_info_t *modem)
{
    modem_info_t *altmodem;
    mai_cmd_t	prevmaintn, nextmaintn, tmp;
    mai_dl_info_t *dlp;

    prevmaintn = MAI_NONE_PENDING;
    nextmaintn = modem->maintn_head;
    if (modem->mai_state != MAI_IDLE) {
	if (nextmaintn == MAI_PENDING_B2B) {
	    printf("\n%s aborted. ", mai_maintn_strings[nextmaintn]);
	    nextmaintn = modem->maintn_pending[nextmaintn];
	    /* make sure it does not start another maintn */
	    modem->maintn_pending[modem->maintn_head] = MAI_NONE_PENDING;
	    mai_abort_active_b2b_test(modem);
	    modem->maintn_head = nextmaintn;
	} else if (nextmaintn != MAI_NONE_PENDING) {
	    printf("\n%s in progress, cannot be aborted. ",
				mai_maintn_strings[nextmaintn]);
	    prevmaintn = nextmaintn;
	    nextmaintn = modem->maintn_pending[nextmaintn];
	}
    }
    /* remove pending downloads and back-to-back test */
    while (nextmaintn != MAI_NONE_PENDING) {

	switch (nextmaintn) {

	case MAI_PENDING_B2B:

	    altmodem = modem->alt_modem;
	    if (altmodem->maintn_head == MAI_PENDING_B2B  &&
					altmodem->mai_state != MAI_IDLE) {
		printf("\n%s aborted. ", mai_maintn_strings[nextmaintn]);
		nextmaintn = modem->maintn_pending[MAI_PENDING_B2B];
		mai_abort_active_b2b_test(altmodem);
		/* the above abort should have automatically */
		/* dequeued it for this modem */
		continue;
	    }
	    modem->alt_modem = altmodem->alt_modem = NULL;
	    modem->mai_statbits &= ~(MAI_B2B_ATD | MAI_B2B_ATA);
	    altmodem->mai_statbits &= ~(MAI_B2B_ATD | MAI_B2B_ATA);
	    mai_remove_pending_maintn(altmodem, MAI_PENDING_B2B);
	    break;

	case MAI_PENDING_DL:

	    if ((dlp = modem->download_info) == NULL) {
		break;
	    }
	    modem->download_info = NULL;
	    if (--dlp->modemcnt) {
		break;
	    }
	    mai_free_dl_info(dlp);
	    break;
	
	default:

	    prevmaintn = nextmaintn;
	    nextmaintn = modem->maintn_pending[nextmaintn];
	    continue;
	}
	printf("\n%s aborted. ", mai_maintn_strings[nextmaintn]);
	if (prevmaintn != MAI_NONE_PENDING) {
	    modem->maintn_pending[prevmaintn] =
					modem->maintn_pending[nextmaintn];
	}
	tmp = nextmaintn;
	nextmaintn = modem->maintn_pending[nextmaintn];
	modem->maintn_pending[tmp] = MAI_NONE_PENDING;
	if (modem->maintn_head == tmp) {
	    modem->maintn_head = nextmaintn;
	}
    }
    return (modem->maintn_head == MAI_NONE_PENDING);
}

/*
 * mai_handle_b2b_init
 *    -	Handle back-to-back initial activity.  This will reset the
 *	factory defaults, and then wait for the other modem to
 *	become ready.
 */
static boolean mai_handle_b2b_init (modem_info_t *modem, int *event)
{
    modem_info_t *alt_modem;

    enum init_steps_ {
	MAI_STP_CHK_ATF = 1,
	MAI_STP_ATF_DONE,
    };

    alt_modem = modem->alt_modem;

    switch (modem->mai_step) {

    case MAI_STP_BEGIN:

	modem->mai_step = MAI_STP_CHK_ATF;
	modem->mai_retry = 0;
	return (mai_send_atcmd(modem, MODEM_TEST_BEGIN_STR));

    case MAI_STP_CHK_ATF:

	if (strncmp("OK", modem->replystrg, 2) != 0) {
	    if (++modem->mai_retry < MAI_AT_MAX_RETRY) {
		return (mai_send_atcmd(modem, MODEM_TEST_BEGIN_STR));
	    }
	    modem->cur_txptr = "modem not responding";
	    modem->mai_statbits |= MAI_FAILED_LAST_B2B;
	    return (mai_b2b_complete(modem, FALSE));
	}

	modem->mai_state = MAI_B2B_CONNECTING;
	modem->mai_step = MAI_STP_BEGIN;

	/* for poweron test, check if other modem is also ready */
	if ((modem->maintn_head == MAI_PENDING_PWRON_B2B) &&
				(modem->alt_modem == NULL)) {
	    /* if this is the last modem, see if it can be paired */
	    /* off with any good modems, or be queued for recheck */
	    if (modem_pool->modem_slot[CSM_MODEM_SLOT(modem)]->
					mai_pwron_b2b_total == 0) {
		return (mai_b2b_complete(modem, FALSE));
	    }
	    return (FALSE);
	}

	if (alt_modem->mai_state != MAI_B2B_CONNECTING ||
				alt_modem->mai_step != MAI_STP_BEGIN) {
	    return (FALSE);
	}

	if (!csm_set_modem_back2back_test(modem, alt_modem)) {
	    modem->cur_txptr = "failed to switch modems";
	    return (mai_b2b_complete(modem, FALSE));
	}
	modem->mai_retry = alt_modem->mai_retry = 0;
	return (TRUE);

    default:

	modem->cur_txptr = "B2BINIT: software error";
	return (mai_b2b_complete(modem, FALSE));
    }


} 

/*
 *  mai_handle_b2b_connect 
 *    -	Handle the connection of the two modems.
 */
static boolean mai_handle_b2b_connect (modem_info_t *modem, int *event)
{
    modem_info_t *alt_modem;

    enum init_steps_ {
	MAI_STP_RETRY_WAIT = 1,
	MAI_STP_ATD_BEGIN,
	MAI_STP_ATD_CONNECTING,
	MAI_STP_ATA_CONNECTING,
	MAI_STP_ATD_CONNECTED,
	MAI_STP_ATA_CONNECTED,
    };

    alt_modem = modem->alt_modem;

    switch (modem->mai_step) { /* switch breaks mean successful connection */

    case MAI_STP_RETRY_WAIT:

        TIMER_STOP(modem->dallytimer);
	if (alt_modem->mai_step == MAI_STP_RETRY_WAIT) {
	    modem->mai_step = MAI_STP_BEGIN;
	    return (FALSE);
	}
	/* otherwise we are ready, fall through */

    case MAI_STP_BEGIN:

	/* set modem to atd modem, alt_modem to ata modem */
	if (modem->mai_statbits & MAI_B2B_ATA) {
	    alt_modem = modem;
	    modem = modem->alt_modem;
	}

	modem->mai_step = MAI_STP_ATD_BEGIN;
	alt_modem->mai_step = MAI_STP_ATA_CONNECTING;
	tty_dtrstate(modem->modem_tty, TRUE);
	tty_dtrstate(alt_modem->modem_tty, TRUE);

	/* give some time for modems to sync up */
	mai_do_send_strg(alt_modem, "ATA", MAI_TIMEOUT_ATD_ATA_CONNECT);
	/* then let ata command take effect */
	return (mai_sleep_for(modem, MAI_TIMEOUT_ATA_HEADSTART));

    case MAI_STP_ATD_BEGIN:
	    
	if (modem->mai_statbits & MAI_B2B_ATD) {
	    /* ata modem should still be waiting */
	    modem->mai_step = MAI_STP_ATD_CONNECTING;
	    /* give some time for modems to sync up */
	    return (mai_do_send_strg(modem, "ATD",MAI_TIMEOUT_ATD_ATA_CONNECT));
	}

    case MAI_STP_ATA_CONNECTING:

	/* ata modem will get connected here */
	if (*event == QUEUE_EVENT) {
	    if (strncmp("CONNECT", modem->replystrg, 7) == 0) {
		if (alt_modem->mai_step == MAI_STP_ATD_CONNECTED) {
		    /* Connection Complete */
		    break;
		}
		/* let atd modem know its ready, and just wait */
		modem->mai_step = MAI_STP_ATA_CONNECTED;
		return (FALSE);
	    }
	    modem->replystrg[20] = 0;
	    modem->cur_txptr = modem->replystrg;
	} else {
	    modem->cur_txptr = "answer time-out";
	}
	if (mai_debug) {
	    buginf("\nMAI(%d/%d): pwronb2b_conn afail: %s. ",
				CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
				modem->cur_txptr);
	}
	if (++modem->mai_retry < MAI_AT_MAX_RETRY) {
	    mai_abort_modem_send(alt_modem);
	    modem->mai_step = alt_modem->mai_step = MAI_STP_RETRY_WAIT;
	    TIMER_START(alt_modem->dallytimer, MAI_TIMEOUT_WAIT_NEXTSTEP);
	    return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP));
	}
	return (mai_b2b_complete(modem, FALSE));

    case MAI_STP_ATD_CONNECTING:

	if (*event == TIMER_EVENT) {
	    if (modem->mai_step == MAI_STP_ATA_CONNECTED) {
		modem->cur_txptr = "answer connected, dial time-out";
	    } else {
		modem->cur_txptr = "dial time-out";
	    }
	} else if (strncmp("CONNECT", modem->replystrg, 7) == 0) {
	    /* just chop it at 20 characters */
	    modem->replystrg[20] = 0;
	    if (modem->maintn_head == MAI_PENDING_B2B) {
		errmsg(&msgsym(B2BCONNECT, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem), 
			CSM_MODEM_SLOT(alt_modem), CSM_MODEM_PORT(alt_modem),
			modem->replystrg);
	    }
	    if (alt_modem->mai_step == MAI_STP_ATA_CONNECTED) {
		break;
	    }
	    /* wait for the ata modem */
	    modem->mai_step = MAI_STP_ATD_CONNECTED;
	    return (FALSE);
	} else {
	    modem->replystrg[20] = 0;
	    modem->cur_txptr = modem->replystrg;
	}
	if (mai_debug) {
	    buginf("\nMAI(%d/%d): pwronb2b_conn dfail: %s. ",
				CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
				modem->cur_txptr);
	}
	if (++modem->mai_retry < MAI_AT_MAX_RETRY) {
	    mai_abort_modem_send(alt_modem);
	    modem->mai_step = alt_modem->mai_step = MAI_STP_RETRY_WAIT;
	    TIMER_START(alt_modem->dallytimer, MAI_TIMEOUT_WAIT_NEXTSTEP);
	    return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP));
	}
	return (mai_b2b_complete(modem, FALSE));

    default:

	modem->cur_txptr = "B2BCONNECT: software error";
	return (mai_b2b_complete(modem, FALSE));
    }

    /* if break to here means connection success */
    TIMER_STOP(modem->dallytimer);
    TIMER_STOP(alt_modem->dallytimer);
    tty_clearbuffers(modem->modem_tty);
    tty_clearbuffers(alt_modem->modem_tty);
    alt_modem->mai_state = modem->mai_state = MAI_B2B_DATA;
    alt_modem->mai_step = modem->mai_step = MAI_STP_BEGIN;
    /* use this to indicate the signal has been enqueued */
    modem->mai_statbits &= ~MAI_RX_SIGNAL_REQ;
    alt_modem->mai_statbits &= ~MAI_RX_SIGNAL_REQ;
    return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP));

}

/*
 *  mai_b2b_rxservice
 *    -	Rx service which will wait for all characters and then
 *	generate a event to the maintenance process.
 */
static void mai_b2b_rxservice (tt_soc *tty, uchar *ptr, int len)
{
    modem_info_t	*modem;

    modem = modem_ttynum_to_modem_info(tty->ttynum);
    if (modem == NULL) {
	return;
    }
    if (len > modem->rsp_bcount) {
	len = modem->rsp_bcount;
    }
    if (len > 0) {
	bcopy(ptr, modem->cur_rsptr, len);
	modem->cur_rsptr += len;

	modem->rsp_bcount -= len;
	if (modem->rsp_bcount == 0) {
	   modem->mai_statbits |= MAI_RX_SIGNAL_REQ;
	   process_enqueue(modem_pool->mai_command_queue, &modem->rsp_info);
	}
    }
}

/*
 * mai_handle_b2b_data_test
 *    -	Handle the data transfer test of the two modems.
 */
static boolean mai_handle_b2b_data_test (modem_info_t *modem, int *event)
{
    modem_info_t *alt_modem;

    enum init_steps_ {
	MAI_STP_ATD_RCV_DATA = 1,
	MAI_STP_ATA_RCV_DATA,
	MAI_STP_ATD_OK,
	MAI_STP_ATA_OK,
    };

    alt_modem = modem->alt_modem;

    switch(modem->mai_step) { /* switch breaks mean failure */

    case MAI_STP_BEGIN:
			
	/* set modem to atd modem */
	if (*event != TIMER_EVENT) {
	    modem->cur_txptr = "B2BDATA: software error";
	    return (mai_b2b_complete(modem, FALSE));
	}
	if (modem->mai_statbits & MAI_B2B_ATA) {
		alt_modem = modem;
		modem = modem->alt_modem;
	}
	modem->cur_rsptr = modem->replystrg;
	alt_modem->cur_rsptr = alt_modem->replystrg;
	alt_modem->rsp_bcount = modem->rsp_bcount = 0;
	tty_servicehook(modem->modem_tty, NULL, mai_b2b_rxservice);
	tty_servicehook(alt_modem->modem_tty, NULL, mai_b2b_rxservice);

	/* sending 10 byte data + CR, with ascending order pattern */
	modem->save_ptr = "0123456789\r";
	alt_modem->save_ptr = "abcdefghij\r";
	modem->rsp_bcount = alt_modem->rsp_bcount = 11;
	quick_puts(modem->modem_tty, modem->save_ptr);
	quick_puts(alt_modem->modem_tty, alt_modem->save_ptr);

	modem->mai_step = MAI_STP_ATD_RCV_DATA;
	alt_modem->mai_step = MAI_STP_ATA_RCV_DATA;
	TIMER_START(modem->dallytimer, MAI_TIMEOUT_B2B_DATA_TRANSFER);
	TIMER_START(alt_modem->dallytimer, MAI_TIMEOUT_B2B_DATA_TRANSFER);
	return (FALSE);
	
    case MAI_STP_ATD_RCV_DATA:
    case MAI_STP_ATA_RCV_DATA:

	if (*event == QUEUE_EVENT && (modem->mai_statbits & MAI_RX_SIGNAL_REQ)){
	    modem->mai_statbits &= ~MAI_RX_SIGNAL_REQ;
	}
	if ((modem->rsp_bcount == 0) &&
		(strncmp(modem->replystrg, alt_modem->save_ptr, 11) == 0)) {
	    modem->mai_b2b_good++;
	} else {
	    clearinput(modem->modem_tty);
	    clearoutput(alt_modem->modem_tty);
	}
	if (++modem->mai_b2b_cnt < modem->mai_b2b_max) {
	    modem->cur_rsptr = modem->replystrg;
	    quick_puts(alt_modem->modem_tty, alt_modem->save_ptr);
	    modem->rsp_bcount = 11;
	    TIMER_START(modem->dallytimer, MAI_TIMEOUT_B2B_DATA_TRANSFER);
	    return (FALSE);
	}
	if (modem->mai_step == MAI_STP_ATD_RCV_DATA) {
	    modem->mai_step = MAI_STP_ATD_OK;
	    if (alt_modem->mai_step == MAI_STP_ATA_OK) {
		break;
	    }
	} else {
	    modem->mai_step = MAI_STP_ATA_OK;
	    if (alt_modem->mai_step == MAI_STP_ATD_OK) {
		break;
	    }
	}
	TIMER_STOP(modem->dallytimer);
	return (FALSE);

    default:

	modem->cur_txptr = "B2BDATA: software error";
	return (mai_b2b_complete(modem, FALSE));
    }

    /* test failed if here, abort whole test */
    if ((modem->mai_b2b_good > 0) || (alt_modem->mai_b2b_good > 0)) {
	return (mai_b2b_complete(modem, TRUE));
    }
    modem->cur_txptr = "data transfer failed";
    return (mai_b2b_complete(modem, FALSE));

}
	    
/*
 *  mai_set_tty_parms 
 *    -	Set the desired tty characteristics to be used during downloading.
 */
static void mai_set_tty_parms (modem_info_t *modem)
{

    /* first save original values */
    modem->save_txspeed = modem->modem_tty->tty_txspeed;
    modem->save_rxspeed = modem->modem_tty->tty_rxspeed;
    modem->save_parity = modem->modem_tty->tty_parity;
    modem->save_databits = modem->modem_tty->tty_databits;
    modem->save_stopbits = modem->modem_tty->tty_stopbits;
    modem->save_capabilities = modem->modem_tty->capabilities;

    setspeed(modem->modem_tty, ASYNC_MAXSPEED, ASYNC_MAXSPEED);
    setstopbits(modem->modem_tty, STOPBITS1);
    setdatabits(modem->modem_tty, 8);
    setparity(modem->modem_tty, PARITY_NO);
    modem->modem_tty->capabilities |= HFLOWIN | HFLOWOUT;
    modem->modem_tty->capabilities &= ~(SFLOWIN | SFLOWOUT);
    setflow(modem->modem_tty);
}

/*
 *  mai_restore_tty_parms 
 *    -	Restore the original tty characteristics.
 */
static void mai_restore_tty_parms (modem_info_t *modem)
{
    setspeed(modem->modem_tty, modem->save_txspeed, modem->save_rxspeed);
    setstopbits(modem->modem_tty, modem->save_stopbits);
    setdatabits(modem->modem_tty, modem->save_databits);
    setparity(modem->modem_tty, modem->save_parity);
    modem->modem_tty->capabilities = modem->save_capabilities;
    setflow(modem->modem_tty);
}

/*
 *  mai_handle_dl_init 
 *    -	Handle the initial maintenance activity before downloading.
 *	This activity is should be generic to all modem vendors.
 *	Basically, reset the modems and see if there is any response
 *	to a plain AT command.
 */
static boolean mai_handle_dl_init (modem_info_t *modem, int *event)
{
    char *ptr, *end;
    int ix;

    enum init_steps_ {
	MAI_STP_RESET_DONE = 1,
	MAI_STP_RU_THERE,
    };

    switch(modem->mai_step) {

    case MAI_STP_BEGIN:
        if (!MCOM_MODEM_CTR(modem).mai_dl_ppp_mode){
                if (modem->modem_tty->statbits & PPPMODE) {
                        MCOM_MODEM_CTR(modem).mai_dl_ppp_mode = TRUE;
                        tty_dtrstate(modem->modem_tty, TRUE);
                        tty_pppmode(modem->modem_tty, FALSE);
                }
        }
	errmsg(&msgsym(DL_START, MODEM),
				CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem));
	/* turn off the oob port */
	reg_invoke_modem_mgmt_oob_operation((void *)modem, OOB_PORT_OFF);
        /* reset the current firmware version and boot version to unknown */
	for (ix = 0; ix < MAX_MODEM_REV_LEN; ix++) {
	  modem->modem_mgmt_info->modem_status_info.modem_firmware_rev[ix] = 0;
	  modem->modem_mgmt_info->modem_status_info.modem_boot_firmware_rev[ix] = 0;
	}
	/* Let's reset it to clean up any funny modem conditions */
	csm_set_modem_reset_status(modem);
	csm_clear_modem_reset_status(modem, TRUE);
	modem->mai_step = MAI_STP_RESET_DONE;
	return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP + 6*ONESEC));

    case MAI_STP_RESET_DONE:

	/* try AT command */
	mai_set_tty_parms(modem);
	modem->mai_retry = 0;
	modem->mai_step = MAI_STP_RU_THERE;
	return (mai_send_nocr_atcmd_while_not_connected(modem, "AT"));

    case MAI_STP_RU_THERE:

	/* try at command */
	ptr = modem->replystrg;
	end = modem->cur_rsptr;
	for (; ptr < end; ptr++) {
	    if (strncasecmp("OK", ptr , 2) == 0) {
		modem->mai_state = MAI_DL_INIT_MODEM;
		modem->mai_step = MAI_STP_BEGIN;
		return (TRUE);
	    }
	}
	if (++modem->mai_retry < MAI_AT_DL_MAX_RETRY) {
	    return (mai_send_atcmd_while_not_connected(modem, "AT"));
	}
	if (++MCOM_MODEM_CTR(modem).mai_dl_retry_stp_ru_there < MAI_DL_MAX_RETRY) {
            modem->mai_state = MAI_DL_INIT;
	    modem->mai_step = MAI_STP_BEGIN;
	    return(mai_sleep_for(modem, ONEMIN));
	}
	MCOM_MODEM_CTR(modem).mai_dl_retry_stp_ru_there = 0;
	errmsg(&msgsym(BADMODEM, MODEM),
		CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
		"download initialization (AT command)");
	break;

    default:

	errmsg(&msgsym(DL_FAIL, MODEM),
		CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
		0, "DL_INIT: software error");
    }

    mai_restore_tty_parms(modem);
    csm_set_download_fail_status(modem);
    csm_set_bad_modem_status(modem);
    csm_clear_download_file_status(modem);
    mai_abort_all_maintns(modem);
    return (TRUE);
}

/*
 *  mai_dl_txservice 
 *    -	Download TX service routine.  Basically transfers the download
 *	file store in main memory to the modems.
 */
static uint mai_dl_txservice (tt_soc *tty, uchar *ptr, int max)
{
    modem_info_t *modem;
    int len;
    uchar *start;
    uchar *xp;

    modem = modem_ttynum_to_modem_info(tty->ttynum);
    if (modem == NULL) {
	return (0);
    }
    if (modem->dlbufptr == NULL) {
	TIMER_START(modem->dallytimer, MAI_TIMEOUT_DL_TX_COMPLETE);
	return (0);
    }

    start = ptr;
    while (max) {
	if (modem->tx_bcount == 0) {
	    modem->dlbufptr = modem->dlbufptr->nextbuf;
	    if (modem->dlbufptr == NULL) {
		break;
	    }
	    modem->cur_txptr = modem->dlbufptr->data;
	    modem->tx_bcount =  modem->dlbufptr->bytecnt;
	}

	len = 0;
	if (max < modem->tx_bcount) {
	    len = max;
	} else {
	    len = modem->tx_bcount;
	}
	xp = modem->cur_txptr + (len-1);
	while (*xp != '\n' && *xp != '\r' && xp > modem->cur_txptr) {
	    --xp;
	}
	if (xp > modem->cur_txptr) {
	    len = xp - modem->cur_txptr + 1;
	}
	bcopy(modem->cur_txptr, ptr, len);
	modem->cur_txptr += len;
	modem->tx_bcount -= len;
	ptr += len;
	max -= len;
	if (*xp == '\n' || *xp == '\r') {
	    break;
	}
    }
    if (ptr == start) {
	TIMER_START(modem->dallytimer, MAI_TIMEOUT_DL_TX_COMPLETE);
	return (0);
    }
    modem->save_numchar++;

    return (ptr - start);
}

/*
 * microcom_dl_decode_uploaderrmsg
 *
 *    -	Decode Microcom predefined upload error codes.
 */
char *microcom_dl_decode_uploaderrmsg (char *str) {

    if (strncasecmp(str, "UPLOADERROR", 11) == 0) {
	/* skip over the UPLOADERROR characters */
	str += 11;
	while (!isdigit(*str)) {
	    if (*str == 0) {
		return ("");
	    }
	    str++;
	}
	switch (atoi(str)) {

	case 1:
	    return (" : Flash ID unsupported or damaged flash device");

	case 2:
	    return (" : Illegal S0(header) record in the upgrade file");

	case 3:
	    return (" : Incorrect upgrade file checksum");

	case 4:
	    return (" : Unused");

	case 5:
	    return (" : Unexpected record type encountered in upgrade file");

	case 6:
	    return (" : Upgrade file does not contain an end record");

	case 7:
	    return (" : Timed-out waiting for input character");

	case 8:
	    return (" : Data record with illegal data address");

	case 9:
	    return (" : Data record wraps 16K page");

	case 10:
	    return (" : Programming error burning flash");

	case 11:
	    return (" : Flash erasing error");

	case 12:
	    return (" : Flash program verification error");

	case 13:
	    return (" : Serial number exists - can not be overwritten");

	case 14:
	    return (" : Network address exists - can not be overwritten");

	case 15:
	    return (" : Illegal user entry for serial number or net address");

	case 16:
	    return (" : Over length SDLC packet");

	case 50:
	    return (" : Internal S-record parser and data handler code");

	case 51:
	    return (" : Internal device address map table error code");

	case 128:
	    return (" : S-record parsing error:  Illegal character at start");

	case 129:
	    return (" : S-record parsing error:  Incorrect record type");

	case 130:
	    return (" : S-record parsing error:  Incorrect record length");

	case 131:
	    return (" : S-record parsing error:  Incorrect record checksum");

	case 132:
	    return (" : S-record parsing error:  Illegal record character");

	case 200:
	    return (" : Internal code used by S-record parsing to indicate "
		    "complete data S-record has been parsed");

	}
    }
    return ("");
}

/*
 *  microcom_handle_dl_modem 
 *    -	Handle Microcom specific download activity.
 *	Basically, do a AT^H command, and then transfer the file.
 *	Then get the version, and re-send the AT^P command for the
 *	oob port address.
 */
static boolean microcom_handle_dl_modem (modem_info_t *modem, int *event)
{
    char *ptr, *end;
    char *donestrg = "COMPLETE";
    char *upload_string;
    int ix;

    enum init_steps_ {
        MAI_STP_WAIT_FOR_RESET = 1,
	MAI_STP_INIT_DONE,
	MAI_STP_PREP_TRANSFER,
	MAI_STP_TRANSFER_DONE,
	MAI_STP_GET_VERSION,
	MAI_STP_VERSION_DONE,
	MAI_STP_RESTORE_ATP,
	MAI_STP_ATP_DONE,
    };

    switch(modem->mai_step) {

    case MAI_STP_BEGIN:

	tty_dtrstate(modem->modem_tty, FALSE);
	modem->mai_step = MAI_STP_WAIT_FOR_RESET;
	return(mai_sleep_for(modem, CSM_MODEM_RESET_MINIMUM_TIME));

    case MAI_STP_WAIT_FOR_RESET:
  
        modem->mai_step = MAI_STP_INIT_DONE;
        /* AT^H has no response, so just do a short timeout */
        return (mai_do_send_strg(modem, "AT^H", MAI_TIMEOUT_QUICK_WAIT));

    case MAI_STP_INIT_DONE:

	tty_dtrstate(modem->modem_tty, TRUE);
	modem->mai_step = MAI_STP_PREP_TRANSFER;
	return (mai_sleep_for(modem, MAI_TIMEOUT_QUICK_WAIT));

    case MAI_STP_PREP_TRANSFER:

	modem->dlbufptr = modem->download_info->bufqueue.qhead;
	modem->tx_bcount = modem->dlbufptr->bytecnt;
	modem->cur_txptr = modem->dlbufptr->data;
	modem->mai_step = MAI_STP_TRANSFER_DONE;
	modem->save_numchar = 0;
	/* set a reasonable time out value for the given amount of data */
	ix = modem->download_info->fparms.datalen / 10000; 
	if (ix <= 30) {
	    /* not less than 1 min */
	    ix = 60 * 2;		/* make it 2 minutes now */
	} else if (ix > 300) {
	    /* not more than 5 min beyond expected download time */
	    ix += 300;
	} else {
	    /* otherwise double the time */
	    ix += ix;
	}
	return (mai_do_send_data(modem, mai_dl_txservice, ix*ONESEC));

    case MAI_STP_TRANSFER_DONE:

	if (modem->cur_rsptr > modem->replystrg) {
	    /* Find out if the completion string is in response */
	    ptr = modem->replystrg;
	    end = modem->cur_rsptr - (strlen(donestrg) - 1);
	    for (; ptr < end; ptr++) {
		if (strncasecmp(donestrg, ptr, strlen(donestrg)) == 0){
		    modem->mai_step = MAI_STP_GET_VERSION;
		    /* modem should now be booting new code */
		    return (mai_sleep_for(modem, CSM_MODEM_RESET_MINIMUM_TIME + 6*ONESEC));
		}
	    }

	    /* just chop it at 20 characters */
	    modem->replystrg[20] = 0;
	    upload_string = microcom_dl_decode_uploaderrmsg(modem->replystrg);

	    if (upload_string[0]) {
	       strncat(modem->replystrg, upload_string, MAI_REPLYSTRGLEN - 21);
	       errmsg(&msgsym(DL_FAIL, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			modem->save_numchar, modem->replystrg);
            } else {
	       errmsg(&msgsym(DL_FAIL, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			modem->save_numchar, "Invalid reply String");
            }

	    if (++MCOM_MODEM_CTR(modem).mai_dl_retry_stp_xfer_done < MAI_DL_MAX_RETRY) {
	    	modem->mai_state = MAI_DL_INIT;
            	modem->mai_step = MAI_STP_BEGIN;
            	return (mai_sleep_for(modem, ONEMIN));
            }
	} else {
	    errmsg(&msgsym(DL_FAIL, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			modem->save_numchar, "timed-out");
	    if (++MCOM_MODEM_CTR(modem).mai_dl_retry_xfer_timedout < (MAI_DL_MAX_RETRY + 200)) {
	    	modem->mai_state = MAI_DL_INIT;
            	modem->mai_step = MAI_STP_BEGIN;
            	return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP));
            }
	}
	MCOM_MODEM_CTR(modem).mai_dl_retry_stp_xfer_done = 0;
	MCOM_MODEM_CTR(modem).mai_dl_retry_xfer_timedout = 0;
	modem->mai_step = MAI_STP_RESTORE_ATP;
	csm_set_modem_reset_status(modem);
	csm_clear_modem_reset_status(modem, TRUE);
	csm_set_download_fail_status(modem);
	return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP));

    case MAI_STP_GET_VERSION:

	modem->mai_step = MAI_STP_VERSION_DONE;
	return (mai_send_nocr_atcmd(modem, "AT%V1"));

    case MAI_STP_VERSION_DONE:

	if (*modem->replystrg == 0) {
	    /* modem may be resetting, try around about 10 times */
	    if (++modem->mai_retry < MAI_AT_AFTER_RESET) {
		return (mai_send_atcmd(modem, "AT%V1"));
	    }
	    if (++MCOM_MODEM_CTR(modem).mai_dl_retry_stp_version_done < MAI_DL_MAX_RETRY) {
            	modem->mai_state = MAI_DL_INIT;
	    	modem->mai_step = MAI_STP_BEGIN;
	    	return(mai_sleep_for(modem, ONEMIN));
	    }
	    MCOM_MODEM_CTR(modem).mai_dl_retry_stp_version_done = 0;
	    errmsg(&msgsym(DL_GOOD, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			"Failed to get version");
	} else {
	    /* display maximum 80 characters */
	    modem->replystrg[80] = 0;
	    csm_clear_bad_modem_status(modem);
	    errmsg(&msgsym(DL_GOOD, MODEM), CSM_MODEM_SLOT(modem),
			CSM_MODEM_PORT(modem), modem->replystrg);
	}
	modem->mai_step = MAI_STP_RESTORE_ATP;
	return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP));

    case MAI_STP_RESTORE_ATP:

	ix = CSM_MODEM_ADDRESS(modem);
	if (ix >= MAX_MODEM_PER_SLOT) {
	    ix = MAX_MODEM_PER_SLOT;
	}
	/* stick it at end of replystrg so it can be preserved */
	sprintf(modem->replystrg + MAI_MODEM_RSPLEN, "AT^P%d\r", ix);
	modem->mai_retry = 0;
	modem->mai_step = MAI_STP_ATP_DONE;
	/* modem must be ok, do regular send */
	return (mai_send_atcmd(modem, modem->replystrg + MAI_MODEM_RSPLEN));

    case MAI_STP_ATP_DONE:

	/*
	 * Try 5 times, then say its initialized.  Not mark bad
	 * because it is just the oob port address, modem may
	 * still be usable.
	 */
	if (strncmp("OK", modem->replystrg, 2) != 0) {
	    if (++modem->mai_retry < MAI_ATP_MAX_RETRY) {
		return (mai_send_atcmd(modem,
				modem->replystrg + MAI_MODEM_RSPLEN));
	    }
	    if (++MCOM_MODEM_CTR(modem).mai_dl_retry_stp_atp_done < MAI_DL_MAX_RETRY) {
            	modem->mai_state = MAI_DL_INIT;
	    	modem->mai_step = MAI_STP_BEGIN;
	    	return(mai_sleep_for(modem, ONEMIN));
	    }
	}
	MCOM_MODEM_CTR(modem).mai_dl_retry_stp_atp_done = 0;
	/* else stop */
	if (modem->mgmt_tty) {
            reg_invoke_modem_mgmt_oob_operation((void *)modem, OOB_PORT_ON);
        }
	modem->mai_state = MAI_DL_COMPLETE;
	modem->mai_step = MAI_STP_BEGIN;
	return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP));

    }

    errmsg(&msgsym(DL_FAIL, MODEM),
		CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
		modem->save_numchar, "DL_MODEM: software error");
    csm_set_download_fail_status(modem);
    /* go check if modem is still there */
    modem->mai_state = MAI_DL_COMPLETE;
    modem->mai_step = MAI_STP_BEGIN;
    return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP));
}

/*
 *  mai_handle_dl_complete 
 *    -	Handles the download completion activity.  Basically do
 *	a final AT command to see if the modem is still ok.
 */
static boolean mai_handle_dl_complete (modem_info_t *modem, int *event)
{
    enum init_steps_ {
	MAI_STP_CHECKOK = 1,
    };

    switch(modem->mai_step) {

    case MAI_STP_BEGIN:
	modem->mai_retry = 0;
	modem->mai_step = MAI_STP_CHECKOK;
	return (mai_send_atcmd(modem, "AT"));

    case MAI_STP_CHECKOK:

	if (strncmp("OK", modem->replystrg, 2) == 0) {
	    mai_restore_tty_parms(modem);
	    csm_clear_download_file_status(modem);
	    return (mai_next_maintns(modem));
	}
	if (++modem->mai_retry < MAI_AT_MAX_RETRY) {
	    return (mai_send_atcmd(modem, "AT"));
	}
	errmsg(&msgsym(BADMODEM, MODEM),
		CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
		"download completion (AT command)");
	break;

    default:

	errmsg(&msgsym(DL_FAIL, MODEM),
		CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
		0, "DL_COMPLETE: software error");
    }
    if (MCOM_MODEM_CTR(modem).mai_dl_ppp_mode) {
           tty_dtrstate(modem->modem_tty, TRUE);
           tty_pppmode(modem->modem_tty, TRUE);
           MCOM_MODEM_CTR(modem).mai_dl_ppp_mode = FALSE;
    }
    mai_restore_tty_parms(modem);
    csm_set_bad_modem_status(modem);
    csm_clear_download_file_status(modem);
    mai_abort_all_maintns(modem);
    return (FALSE);

}

/*
 * microcom_handle_poweron_init
 *    -	Handles the poweron initialization requirements for Microcom
 *	modems.  Basically this will initialize the oob port address
 *	using the AT^P.
 */
static boolean microcom_handle_poweron_init (modem_info_t *modem, int *event)
{
  ulong		port;
  uchar		*ptr, *end;
  uchar		*errstrg = "ERROR";

  enum init_steps_ {
	MAI_STP_REENABLE_PORT = 1,
	MAI_STP_SEND_ATH,
	MAI_STP_SEND_ATP,
	MAI_STP_INIT_DONE
    };

    switch (modem->mai_step) {

    case MAI_STP_BEGIN:

	csm_clear_modem_reset_status(modem, FALSE);
	modem->mai_step = MAI_STP_REENABLE_PORT;
	return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP));

    case MAI_STP_REENABLE_PORT:

	modem->mai_step = MAI_STP_SEND_ATH;
	return (mai_sleep_for(modem, MAI_TIMEOUT_WAIT_NEXTSTEP));

    case MAI_STP_SEND_ATH:

	modem->mai_retry = 0;
	modem->mai_step = MAI_STP_SEND_ATP;
	return (mai_send_nocr_atcmd_while_not_connected(modem, "ATH"));

    case MAI_STP_SEND_ATP:

	if (strncmp("OK", modem->replystrg, 2) == 0) {
	    port = CSM_MODEM_ADDRESS(modem);
	    if (port >= MAX_MODEM_PER_SLOT) {
		port = MAX_MODEM_PER_SLOT;
	    }
	    /* stick it at end of replystrg so it can be preserved */
	    sprintf(modem->replystrg + MAI_MODEM_RSPLEN, "AT^P%d\r", port);
	    modem->mai_retry = 0;
	    modem->mai_step = MAI_STP_INIT_DONE;
	    /* modem must be ok, do regular send */
	    return (mai_send_atcmd(modem, modem->replystrg + MAI_MODEM_RSPLEN));
	}
	if (++modem->mai_retry > 2 && modem->cur_rsptr > modem->replystrg) {
	    /* Find out if the error string is in response */
	    ptr = modem->replystrg;
	    end = modem->cur_rsptr - (strlen(errstrg) - 1);
	    for (; ptr < end; ptr++) {
		if (strncasecmp(errstrg, ptr, strlen(errstrg)) == 0 &&
					modem->mai_retry < MAI_AT_AFTER_RESET) {
		    modem->mai_retry = MAI_AT_AFTER_RESET;
		}
	    }
	}
	if (modem->mai_retry < MAI_AT_AFTER_RESET) {
	    /*
	     * In case line is still set in hardware ppp mode,
	     * turn it off. Make sure te port is enabled
	     */
            tty_portenable(modem->modem_tty);
	    tty_dtrstate(modem->modem_tty, TRUE);
	    tty_pppmode(modem->modem_tty, FALSE);
	    return (mai_send_nocr_atcmd_while_not_connected(modem, "ATH"));
	}
	/* else stop */
	csm_set_bad_modem_status(modem);
	errmsg(&msgsym(BADMODEM, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			"initialization (ATH command)");
	break;

    case MAI_STP_INIT_DONE:

	/*
	 * Try 5 times, then say its initialized.  Not mark bad
	 * because it is just the oob port address, modem may
	 * still be usable.
	 */
	if (strncmp("OK", modem->replystrg, 2) != 0) {
	    if (++modem->mai_retry < MAI_ATP_MAX_RETRY) {
		/*
		 * In case line is still set in hardware ppp mode,
		 * turn it off
		 */
		tty_dtrstate(modem->modem_tty, TRUE);
		tty_pppmode(modem->modem_tty, FALSE);
		return (mai_send_atcmd(modem,
				modem->replystrg + MAI_MODEM_RSPLEN));
	    }
	}
	/* else stop */
	if (modem->mgmt_tty) {
            reg_invoke_modem_mgmt_oob_operation((void *)modem, OOB_PORT_ON);
        }
	return (mai_next_maintns(modem));

    default:

	errmsg(&msgsym(BADMODEM, MODEM),
			CSM_MODEM_SLOT(modem), CSM_MODEM_PORT(modem),
			"PWRON: software error");
    }

    mai_abort_all_maintns(modem);
    return (FALSE);
}

/*
 * mai_reset_modem
 *
 * Turn off the OOB port. Reset the modem. Wait for the modems to come
 * out of reset. Restart the modem.
 *
 */

static boolean mai_reset_modem (modem_info_t *modem_info, int *event)
{

  if(!modem_info) {
    return (FALSE);
  }

  /* turn off the oob port */
  reg_invoke_modem_mgmt_oob_operation((void *)modem_info, OOB_PORT_OFF);


  /* Let's reset the modem */

  csm_set_modem_reset_status(modem_info);
  csm_clear_modem_reset_status(modem_info, TRUE);
  mai_restart_modem(modem_info);
  return (mai_next_maintns(modem_info));
}


