/* $Id: if_as5200_csm.c,v 3.1.68.10 1996/09/10 17:36:24 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_csm.c,v $
 *------------------------------------------------------------------
 * if_as5200_csm.c -- Brasil Call Switching Module
 *
 * November, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_csm.c,v $
 * Revision 3.1.68.10  1996/09/10  17:36:24  snyder
 * CSCdi68626:  declare things const save memory
 *              360 data, 368 image
 *              This file has so many opportunities
 *              to save memory, but I'll contain myself.
 * Branch: California_branch
 *
 * Revision 3.1.68.9  1996/09/09  20:56:40  jchlin
 * CSCdi66525:  ct1/rbs cannot busy back the CO trunk when theres no free
 * modem
 * Branch: California_branch
 * o       The bugfix will busy dsx0s back to CO thru RBS when there's
 *         no free modem to accept incoming calls.
 * o       The modem pool is changed to have multiple pools for different
 *         types of modems.
 * o       Add hidden debug commands.
 *
 * Revision 3.1.68.8  1996/09/02  23:35:54  jchlin
 * CSCdi64950:  some ISDN modem incoming calls fails in CSM (modem_info is
 * NULL)
 * Branch: California_branch
 * The race condition of using the modem_info pointer saved in the
 * idb for the same B channel between the last disconnected ISDN
 * call and the newly incoming ISDN call. Make sure idb's modem_info
 * pointer is not cleared by different modems.
 *
 * Revision 3.1.68.7  1996/09/02  23:26:14  jchlin
 * CSCdi66909:  HDLC controller receives excessive errors on voice calls
 * Branch: California_branch
 * Make the B channel to cut through as soon as the incoming call is
 * detected.
 *
 * Revision 3.1.68.6  1996/09/02  23:22:40  jchlin
 * CSCdi66886:  ISDN cause sent back to CO doesnt make CO to rehunt next
 * PRI group
 * Branch: California_branch
 * Per PacBell request, use REQ_CHANNEL_NOT_AVAIL to reject an incoming
 * ISDN call so that the CO's switch can rehunt the next PRI trunk.
 *
 * Revision 3.1.68.5  1996/08/28  12:56:58  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.68.4  1996/08/26  23:08:43  wmay
 * CSCdi66907:  ISDN incoming calls getting rejected - enforce call ranges
 * for modem and dialer type calls
 * Branch: California_branch
 *
 * Revision 3.1.68.3  1996/07/23  18:54:29  elem
 * CSCdi62565:  Lines hung and cannot be cleared with clear line
 * Branch: California_branch
 * Remove the extraneous lock on the tty line for outgoing calls since the
 * process doing the call will have already done so.  This extraneous lock
 * causes the line to be left lock if there were no process there (see
 * ddts for details how this can occur).
 *
 * Revision 3.1.68.2  1996/07/15  20:36:08  towu
 * CSCdi62430:  CT1 RBS is not enabled
 * Branch: California_branch
 *         o Code cleanup.
 *         o Create msg_dsx0.c for supporting CT1 RBS.
 *
 * Revision 3.1.68.1  1996/06/16  21:14:29  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:12:28  jchlin
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../os/old_timer_callbacks.h"
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "linktype.h"
#include "config.h"
#include "logger.h"
#include "msg_modem.c"		/* Not a typo, see logger.h */
#include "msg_dsx0.c"		/* Not a typo, see logger.h */
#include "msg_csm.c"            /* Not a typo, see logger.h */
#include "ttysrv.h"
#include "isdn_bri.h"
#include "if_les.h"
#include "../wan/isdn.h"
#include "../isdn/gendef.h"
#include "../isdn/ccie.h"
#include "media_registry.h"
#include "../ui/command.h"
#include "../if/if_controller.h"
#include "../if/dsx1.h"
#include "../if/dsx1_registry.h"
#include "../if/dsx1_registry.regc"
#include "if_as5200_dsx1_ipc.h"
#include "parser_defs_modem_mgmt.h"
#include "../dev/flash_defines.h"
#include "if_as5200_mcn.h"
#include "if_as5200_t1.h"
#include "if_as5200_tdm.h"
#include "if_as5200_modem.h"
#include "modem_mgmt.h"
#include "../h/cs_registry.regh"
#include "csm_registry.h"
#include "../les/csm_registry.regc"
#include "../isdn/isdn_registry.h"
#include "../os/clock.h"
#include "../if/network.h"
#include "../wan/dialer.h"

/********************************************************************
 * CSM_SET_STATUS & CSM_CLEAR_STATUS are used only by CSM internally.
 * Don't use them elsewhere.
 */
#define CSM_SET_STATUS(modem_info, mask)   ((modem_info)->csm_status |= (mask))
#define CSM_CLEAR_STATUS(modem_info, mask) ((modem_info)->csm_status &= ~(mask))

/*
 * The microcom modem firmware version 1.33 starts supporting the fastring.
 * Therefore, 1.32 and below doesn't support this feature.
 */
#define MICROCOM_SLOWRING_FIRMWARE_VERSION "\x1\x0\x20"

/********************************************************************
 * 
 */
long csm_regcode;             /* for dynamically allocated registry */

#define CSM_EVENT_QUEUE_ENABLE    0
#define CSM_USE_AT_COMMAND_CTRL   0        
#define TDM_WORKAROUND            1

#if CSM_EVENT_QUEUE_ENABLE
static csm_event_queue_t  *csm_event_queue;
static csm_event_queue_t *csm_event_tail, *csm_event_head, *csm_event_end;
static ushort             csm_event_queue_count;

static process csm_event_handler_process(void);
static pid_t   csm_event_process_id;

/*
 * =====================================================================
 * write_next_csm_event_inline()
 *
 * Description:
 * The inline routine returns the next free queue interrupt event pointer 
 * in the ring buffer.
 *
 * Parameters:
 * None.
 *
 * Returns:
 * NULL if the queue is full or the next free queue interrupt event pointer.
 * =====================================================================
 */
static inline csm_event_queue_t* write_next_csm_event_inline (void)
{
    if (csm_event_queue_count == CSM_EVENT_QUEUE_SIZE)
        return (NULL);

    csm_event_queue_count++;
    if (csm_event_head == csm_event_end)
        csm_event_head = csm_event_queue;
    return (csm_event_head++);
}

/*
 * =====================================================================
 * read_next_csm_event_inline()
 *
 * Description:
 * The inline routine to read the queued interrupt event in the ring
 * buffer.
 *
 * Parameters:
 * None.
 *
 * Returns:
 * NULL if there's no event pending or the queued interrupt event pointer.
 * =====================================================================
 */ 
static inline csm_event_queue_t* read_next_csm_event_inline (void)
{
    if (!csm_event_queue_count)
        return (NULL);

    csm_event_queue_count--;
    if (csm_event_tail == csm_event_end)
        csm_event_tail = csm_event_queue;
    return (csm_event_tail++);

}
#endif

/********************************************************************
 *
 */
static void csm_add_digit_collector(int slot, int max_dc_number, int dummy);
static void csm_init_interface(int slot, int dummy1, int dummy2);
static void csm_activate_periodical_background_check(void *dummy1, 
                                                     void *dummy2, 
                                                     void *dummy3);
static process csm_periodical_background_check_process(void);
static void csm_ring_indication_proc(void *dummy1, void *dummy2, void *dummy3);
static void csm_add_pri_signaling_interface(hwidbtype *hwidb);
static void csm_delete_pri_signaling_interface(hwidbtype *hwidb);
static ushort modem_report(hwidbtype *dchan_idb, ushort call_id, ushort ces,
                           short bchan, ushort event, char *remote_num, 
                           char *local_num, ushort cause);
static void csm_tty_platform_dependent_info_report(tt_soc *tty); 
static void csm_idb_platform_dependent_info_report(hwidbtype *hwidb); 
static void csm_rx_ct1_event_from_neat(uchar slot, void *msg);
static void csm_disable_oh_interrupt(modem_info_t *modem_info);
static void csm_enable_oh_interrupt(modem_info_t *modem_info);
static void csm_stop_modem_reset(modem_info_t *modem_info);
static void csm_start_modem_reset(modem_info_t *modem_info);
static void csm_process_next_digit_collect(digit_collect_t *dc);
static ushort csm_proc_idle(modem_info_t* modem_info);
static ushort csm_proc_ic4_oc8_disconnecting(modem_info_t* modem_info);
static ushort csm_proc_ic1_ring(modem_info_t* modem_info);
static ushort csm_proc_ic2_wait_for_carrier(modem_info_t* modem_info);
static ushort csm_proc_ic3_oc6_connected(modem_info_t* modem_info);
static ushort csm_proc_oc1_request_digit(modem_info_t* modem_info);
static ushort csm_proc_oc2_collect_1st_digit(modem_info_t* modem_info);
static ushort csm_proc_oc3_collect_all_digit(modem_info_t* modem_info);
static ushort csm_proc_oc4_dialing(modem_info_t* modem_info);
static ushort csm_proc_oc5_wait_for_carrier(modem_info_t* modem_info);
static ushort csm_proc_oc7_busy_error(modem_info_t* modem_info);

static const uchar *const csm_event_name[CSM_EVENT_MAX_LENGTH] =
{
"CSM_EVENT_NONE", "CSM_EVENT_WDT_TIMEOUT", "CSM_EVENT_ISDN_CALL", 
"CSM_EVENT_ISDN_CONNECTED", "CSM_EVENT_ISDN_DISCONNECTED", 
"CSM_EVENT_ISDN_BCHAN_ASSIGNED", "CSM_EVENT_DSX0_ENABLE_CHANNEL", 
"CSM_EVENT_DSX0_DISABLE_CHANNEL", "CSM_EVENT_DSX0_CALL", 
"CSM_EVENT_DSX0_CONNECTED", "CSM_EVENT_DSX0_DISCONNECTED",
"CSM_EVENT_DSX0_BCHAN_ASSIGNED", "CSM_EVENT_DSX0_START_TX_TONE", 
"CSM_EVENT_DSX0_START_RX_TONE", "CSM_EVENT_MODEM_OFFHOOK", 
"CSM_EVENT_MODEM_ONHOOK", "CSM_EVENT_DIGIT_COLLECT_READY",
"CSM_EVENT_GET_1ST_DIGIT", "CSM_EVENT_GET_ALL_DIGITS", 
"CSM_EVENT_ABORT_DIGIT_COLLECT", "CSM_EVENT_SHUTDOWN_INTERFACE"
};

static const uchar *const csm_status_name[CSM_STATUS_MAX_LENGTH] =
{
"CSM_STATUS_ACTIVE_WDT", "CSM_STATUS_ACTIVE_CALL", "CSM_STATUS_BUSYOUT_REQ",
"CSM_STATUS_BAD_MODEM", "CSM_STATUS_BACK2BACK_TEST", "CSM_STATUS_MODEM_RESET",
"CSM_STATUS_DOWNLOAD_FILE", "CSM_STATUS_DOWNLOAD_FAIL", "CSM_STATUS_SHUTDOWN",
"CSM_STATUS_INTERNAL_BUSY"
};

static const uchar *const csm_fail_code_name[CSM_FAIL_MAX_LENGTH] =
{
"CSM_FAIL_NONE", "CSM_FAIL_WDT_TIMEOUT", "CSM_FAIL_UNEXPECTED_ON_HOOK",
"CSM_FAIL_MISS_EXPECTED_ON_HOOK", "CSM_FAIL_NO_DC_READY_RSP",
"CSM_FAIL_DC_ABORT_RSP", "CSM_FAIL_SHUTDOWN_INTERFACE", "CSM_FAIL_ISDN_CAUSE",
"CSM_FAIL_NO_SIGANL_CHANNEL", "CSM_FAIL_BUSY_TONE", "CSM_FAIL_RINGING"
};

static const uchar *const stream_name[TDM_STREAM_MAX_LENGTH] = 
{
"TDM_ISDN_STREAM", "TDM_PRI_STREAM",
"TDM_MODEM_STREAM", "TDM_DIGIT_COLLECT_STREAM",
"TDM_TONE_STREAM"
};

static const uchar *const dc_state_name[DC_EVENT_MAX_LENGTH] =
{
NULL, "DC_RESET_REQ", "DC_READY_RSP", "DC_ABORT_RSP",
"DC_FIRST_DIGIT_RSP", "DC_ALL_DIGIT_RSP"
};

static uchar *const ic_state[CSM_IC_STATE_MAX_LENGTH] =
{
"CSM_IC_STATE", "CSM_IC1_RINGING", "CSM_IC2_WAIT_FOR_CARRIER", 
"CSM_IC3_CONNECTED", "CSM_IC4_DISCONNECTING"
};

static uchar *const oc_state[CSM_OC_STATE_MAX_LENGTH] =
{
"CSM_OC_STATE", "CSM_OC1_REQUEST_DIGIT", "CSM_OC2_COLLECT_1ST_DIGIT", 
"CSM_OC3_COLLECT_ALL_DIGIT", "CSM_OC4_DIALING", "CSM_OC5_WAIT_FOR_CARRIER", 
"CSM_OC6_CONNECTED", "CSM_OC7_BUSY_ERROR", "CSM_OC8_DISCONNECTING" 
};

/**********************************************************************
 *
 */
/*
 * =====================================================================
 * csm_get_modem_firmware_version()
 *
 * Description:
 * Convert the firmware version from string to integer.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * The integer firmware version.
 * =====================================================================
 */
uint csm_get_modem_firmware_version (modem_info_t *modem_info)
{
    if (modem_info->modem_mgmt_info) {
        uchar *ch;
        ch =  modem_info->modem_mgmt_info->modem_status_info.modem_firmware_rev;
        return ((((uint)*ch++)<<16) + (((uint)*ch++)<<8) + *ch);
    } else
        return (0);
}
 
/*
 * =====================================================================
 * csm_modem_is_ready_to_take_ic()
 *
 * Description:
 * Check the tty port to see if it's ready to take the incoming calls
 * from dsx1.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * TRUE  - when tty is ready
 * FALSE - if tty is either in use or not ready
 * =====================================================================
 */
static boolean csm_modem_is_ready_to_take_ic (modem_info_t 
                                              *modem_info)
{
    tt_soc       *tty;
 
    tty = modem_info->modem_tty;
 
    return (tty && !TTY_IN_USE(tty)
            && ((tty->capabilities & MODEMMASK) != MODEMOUT)
            && (tty->statbits & IDLE)
            && (tty->answercountdown == 0)); /* make sure the DTR is raised */
}

/*
 * =====================================================================
 * csm_is_newer_modem_firmware()
 *
 * Description:
 * Compare the string of firmware version.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * version    - the firmware version to compare with.
 *
 * Returns:
 * FALSE      - if the modem firmware version is lower or equal to version.
 * TRUE       - if the modem firmware version is higher than version.
 * =====================================================================
 */
static boolean csm_is_newer_modem_firmware (modem_info_t *modem_info,
                                            char        *version)
{
    if (modem_info->modem_mgmt_info) {
        uchar *ch1, *ch2;
        ch1 = modem_info->modem_mgmt_info->modem_status_info.modem_firmware_rev;
        ch2 = version;
        if ((*ch1++ <= *ch2++) && (*ch1++ <= *ch2++) && (*ch1 <= *ch2))
            return (FALSE);
        else
            return (TRUE);
    } else
        return (FALSE);
}
 
/*
 * =====================================================================
 * csm_setup_capability_flags()
 *
 * Description:
 * This function will use the firmware version and modem vendor type to
 * setup the proper setting flags inside the CSM.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
void csm_setup_capability_flags (modem_info_t *modem_info)
{
    if (!modem_info)
        return;
 
    /*
     * Clear old modem related flags first.
     * Shouldn't clear the whole csm_flags 'cause some of
     * them are unconfigurable control flags used during
     * the IC/OC.
     */
    modem_info->csm_flags &= ~(CSM_FLAG_V110_MODEM|CSM_FLAG_FAST_RING);

    /*
     * Set new modem related flags now.
     */
    switch (get_modem_vendor_type_inline(modem_info)) {
    case MODEM_MICROCOM_ID:
         if (csm_is_newer_modem_firmware(modem_info,
                                         MICROCOM_SLOWRING_FIRMWARE_VERSION))
             modem_info->csm_flags |=  CSM_FLAG_FAST_RING;
         break;
 
    case MODEM_NEC_ID:
         modem_info->csm_flags |= CSM_FLAG_V110_MODEM;
         break;
 
    case MODEM_ROCKWELL_ID:
    case MODEM_USR_ID:
    case MODEM_TELEBIT_ID:
    default:
         break;
    }
}
 
/*
 * =====================================================================
 * csm_get_csm_state()
 *
 * Description:
 * The support routine to provide the asc string of the current
 * CSM state.
 *
 * Parameters:
 * csm_state - the current csm state enumation number.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static const uchar* csm_get_csm_state (csm_state_t csm_state)
{
    static const uchar *idle="CSM_IDLE_STATE";

    if (csm_state & CSM_IDLE_STATE)
        return (idle);
    else if (csm_state & CSM_IC_STATE)
        return (ic_state[csm_state & 0xff]);

    return (oc_state[csm_state & 0xff]);
}

/*
 * =====================================================================
 * csm_init_modem_pool()
 *
 * Description:
 * Initialize the modem_pool's contents associated with CSM.
 * The modem_pool memory block is already allocated by modem mgmt.
 * subsystem.
 *
 * Parameters:
 * None.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_init_modem_pool (void)
{   
   modem_pool->modem_mgmt_subsys_running = TRUE;
   modem_pool->current_dchan_idb_index = 0;
   modem_pool->max_dchan_idb_index = 0;
   modem_pool->max_dchan_idb_num       = MAX_TOTAL_SIGNAL_CHANNEL;
   modem_pool->wdt_timeout_val         = CSM_DEFAULT_WATCH_DOG_TIME_OUT; 
                                         /* in ticks */

   modem_pool->modem_control_flags |= MODEM_CTRL_USE_ROUND_ROOBIN;
   modem_pool->next_slot_search     = 0;
   modem_pool->next_port_search     = 0;
   modem_pool->background_check_countdown 
                                    = CSM_PERIODICAL_BACKGROUND_CHECK_TIME;
   modem_pool->min_free_modem_threshold   = DEFAULT_FREE_MODEM_THRESHOLD;
   modem_pool->total_free_rbs_timeslot = 0;
   modem_pool->total_busy_rbs_timeslot = 0;
   modem_pool->background_check_task_pid = 
       process_create(csm_periodical_background_check_process, 
                      "CSM periodical background checker", NORMAL_STACK, 
                      PRIO_LOW);

   if (modem_pool->background_check_task_pid == NO_PROCESS) {
       /*
        * Error: cannot not create the process
        */
       errmsg(&msgsym(INITSYS, CSM), 
              "Cannot create the csm_periodical_background_check_process");
   }
   else {
       modem_pool->wdt = create_timer(csm_activate_periodical_background_check,
                                      NULL, NULL, NULL, 
                                      "csm_periodical_background_check",
                                      TMT_CONTINUOUS);

       set_timer(modem_pool->wdt, CSM_MODEM_POOL_CHECK_TIME);
   }

   modem_pool->ring_timer = create_timer(csm_ring_indication_proc, NULL,
                                  NULL, NULL, "csm_ring_indication_proc",
                                  TMT_CONTINUOUS);

   modem_pool->ring_state = RING_OFF_END;

   set_timer(modem_pool->ring_timer, 2 * ONESEC);

   list_create(&modem_pool->atcmd_answer_header, 0, 
               "ModemMgmt CSM: atcmd_answer_list", 
               LIST_FLAGS_INTERRUPT_SAFE);

   list_create(&modem_pool->wait_for_dialing_header, 0, 
               "ModemMgmt CSM: wait_for_dialing_list", 
               LIST_FLAGS_INTERRUPT_SAFE);

   list_create(&modem_pool->wait_for_bchan_header, 0, 
               "ModemMgmt CSM: wait_for_bchan_list", 
               LIST_FLAGS_INTERRUPT_SAFE);

   list_create(&modem_pool->free_modem_header[CSM_FREE_MODEM], 0, 
               "ModemMgmt CSM: free_modem_list",
               LIST_FLAGS_INTERRUPT_SAFE);

   list_create(&modem_pool->free_modem_header[CSM_FREE_V110_MODEM], 0, 
               "ModemMgmt CSM: free_v110_modem_list",
               LIST_FLAGS_INTERRUPT_SAFE);

}

/*
 * =====================================================================
 * csm_exists()
 *
 * Description:
 * This function returns true when CSM subsystem exists.
 *
 * Parameters:
 * None.
 *
 * Returns:
 * TRUE  - if CSM subsystem exists.
 * FALSE - if CSM subsystem doesn't exist.
 * =====================================================================
 */
static boolean csm_exists (void)
{
   return (TRUE);
}

/*
 * =====================================================================
 * csm_modem_get_calling_number()
 * csm_modem_get_called_number()
 *
 * Description:
 * These functions are called through registry 
 * reg_invoke_modem_get_calling_number() and 
 * reg_invoke_modem_get_called_number() respectively.
 *
 * Call these two registry services to retrieve the Calling Party Number or 
 * the Called Party Number for users on async modem lines of the AS5200.
 *
 * Parameters:
 * tty       - tty line.
 * idb       - interface (ie for isdn)  Not used by this function
 * ret_ptr   - The calling function uses ret_ptr to pass a pointer to
 *             the location where the Calling/Called Number should be
 *             put (or rather where a char pointer to the Number should
 *             be put).
 *
 * Returns:
 * True if the Number is available; False if Number is unavailable.
 * =====================================================================
 */

static boolean
csm_modem_get_calling_number (tt_soc *tty, hwidbtype *idb, char **ret_ptr)
{
    modem_info_t *modem_info;

    /* 
     * registry call; so have paranoia!
     */
    if (tty == NULL || ret_ptr == NULL)
        return FALSE;
    
    modem_info = modem_ttynum_to_modem_info(tty->ttynum);

    if (modem_info == NULL)
	return FALSE;	/* No modem associated with this tty line */
    
    if (modem_info->calling_party_num[0] == '\0')
	return FALSE;	/* Calling party number unavailable */

    *ret_ptr = modem_info->calling_party_num;
    return TRUE;
}

static boolean
csm_modem_get_called_number (tt_soc *tty, hwidbtype *idb, char **ret_ptr)
{
    modem_info_t *modem_info;

    /* 
     * registry call; so have paranoia!
     */
    if (tty == NULL || ret_ptr == NULL)
        return FALSE;
    
    modem_info = modem_ttynum_to_modem_info(tty->ttynum);
    
    if (modem_info == NULL)
	return FALSE;	/* No modem associated with this tty line */

    if (modem_info->called_party_num[0] == '\0')
	return FALSE;	/* Called party number unavailable */

    *ret_ptr = modem_info->called_party_num;
    return TRUE;
}

/*
 * =====================================================================
 * csm_init()
 *
 * Description:
 * The initialization for CSM subsystem
 *
 * Parameters:
 * subsys - subsystem's structure pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_init (subsystype *subsys)
{
   modem_mgmt_parser_init();

   if (modem_pool == NULL) {
       errmsg(&msgsym(INITSYS, CSM), "modem_pool == NULL");
       return;
   }

#if CSM_EVENT_QUEUE_ENABLE
   csm_event_queue = malloc(CSM_EVENT_QUEUE_SIZE*sizeof(csm_event_queue_t));
   csm_event_queue_count = 0;
   csm_event_head = csm_event_tail = csm_event_queue;
   if (csm_event_queue == NULL) {
       csm_event_end = 0;
       errmsg(&msgsym(INITSYS, CSM), "csm_event_queue malloc fails");
   } else {
       csm_event_end  = csm_event_queue + CSM_EVENT_QUEUE_SIZE;
       csm_event_process_id = process_create(csm_event_handler_process,
                                             "CSM Event Handler Process", 
                                             NORMAL_STACK, PRIO_NORMAL);
   }
#endif

   csm_init_modem_pool();

   csm_regcode = registry_create(REG_UNKNOWN, SERVICE_CSM_MAX, "CSM Registry");
   if (csm_regcode != REG_ILLEGAL) {
       create_registry_csm();
       reg_add_csm_exists(csm_exists, "csm_exists");
       reg_add_csm_interface_init(CSM_INIT_INTERFACE, csm_init_interface, 
                                  "csm_init_interface");
       reg_add_csm_interface_init(CSM_ADD_DIGIT_COLLECTOR, 
                                  csm_add_digit_collector,
                                  "csm_add_digit_collector");
   }
   else
       errmsg(&msgsym(INITSYS, CSM), "cannot create the CSM Registry");

   reg_add_modem_report(modem_report, "modem_report");

   reg_add_tty_show(csm_tty_platform_dependent_info_report, 
                    "csm_tty_platform_dependent_info_report");

   reg_add_show_int(csm_idb_platform_dependent_info_report,
                    "csm_idb_platform_dependent_info_report"); 

   reg_add_modem_get_calling_number(csm_modem_get_calling_number, 
				    "csm_modem_get_calling_number");

   reg_add_modem_get_called_number(csm_modem_get_called_number, 
				   "csm_modem_get_called_number");

}

/*
 * =====================================================================
 * csm_clear_modem_info_counter()
 *
 * Description:
 * Clear the statistics information maintained by CSM.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
void csm_clear_modem_info_counter (modem_info_t *modem_info)
{
    modem_info->invalid_event_count      = 0;
    modem_info->wdt_timeout_count        = 0;
    modem_info->stat_ring_no_answer      = 0;
    modem_info->stat_ic_failure          = 0;
    modem_info->stat_ic_complete         = 0;
    modem_info->stat_dial_failure        = 0;
    modem_info->stat_oc_failure          = 0; 
    modem_info->stat_oc_complete         = 0;   
    modem_info->stat_oc_busy             = 0;
    modem_info->stat_oc_no_dial_tone     = 0;
    modem_info->stat_oc_dial_timeout     = 0;
    modem_info->stat_remote_link_disc    = 0;
    modem_info->stat_busyout             = 0;
    modem_info->stat_modem_reset         = 0;
    TS_VALUE(modem_info->stat_call_duration_started) = 0;
    TS_VALUE(modem_info->stat_call_duration_ended)   = 0;
    TS_VALUE(modem_info->stat_total_call_duration)   = 0;
}

/*
 * =====================================================================
 * csm_init_modem_info()
 *
 * Description:
 * Initialize the modem_info's contents associated with CSM.
 * The modem_info memory block is already allocated by modem mgmt.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * slot       - the modem slot number
 * port       - the modem port number
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_init_modem_info (modem_info_t *modem_info,
                                 int slot, int port)
{
   ushort port_offset, port_bitmap;

   if (!modem_pool || !modem_pool->modem_slot[slot] || !modem_info) {
       errmsg(&msgsym(INITPORT, CSM), "CSM_INIT_MODEM_INFO fails", slot, port);
       return;
   }

   /*
    * Assume that the content of modem_info is zero out when it's allocated.
    */
   port_offset = MODEM_PORT_OFFSET(port);
   port_bitmap = MODEM_PORT_BITMAP(port);
   modem_pool->modem_slot[slot]->modem_exist_mask[port_offset] |= port_bitmap;

   /* 
    * need to enable the interrupt during the interface configuration
    */
   modem_pool->modem_slot[slot]->modem_reg->oh_int_enable[port_offset] 
                                                               |= port_bitmap;

   modem_info->modem_pool          = modem_pool;
   modem_info->remote_b2b_modem_info = NULL;
   modem_info->dc                  = NULL;
   modem_info->unit                = CSM_MODEM_UNIT(slot, port);
   modem_info->modem_mask          = port_bitmap;
   modem_info->modem_port_offset   = port_offset;
   modem_info->csm_state           = CSM_IDLE_STATE;
   modem_info->csm_event_proc      = csm_proc_idle;
   modem_info->csm_status          = CSM_STATUS_UNLOCKED;
   modem_info->csm_flags           = 0;

   csm_clear_modem_info_counter(modem_info);
   csm_setup_capability_flags(modem_info);

   /*
    * modem_chnl and pri_chnl.channel_group_used will not be changed
    * after this initialization.
    */
   modem_info->modem_chnl.stream_type = TDM_MODEM_STREAM;
   modem_info->modem_chnl.channel     = port;
   modem_info->modem_chnl.slot        = slot;
   modem_info->modem_chnl.unit        = 0;
   modem_info->modem_chnl.channel_group_used = FALSE;
   modem_info->pri_chnl.channel_group_used   = FALSE;

   if (CSM_IS_V110_MODEM(modem_info))
       modem_info->free_modem_header_ptr 
           = &modem_pool->free_modem_header[CSM_FREE_V110_MODEM];
   else
       modem_info->free_modem_header_ptr 
                              = &modem_pool->free_modem_header[CSM_FREE_MODEM];

   list_enqueue(modem_info->free_modem_header_ptr, &modem_info->free_modem_link,
                modem_info);
       
   modem_info->busyout_rsp_queue   = NULL;
   modem_info->busyout_rsp_msg     = NULL;
   modem_info->bchan_num           = INVALID_B_CHANNEL;
}

/*
 * =====================================================================
 * csm_add_digit_collector()
 *
 * Description:
 * The call back routine is associated with reg_add_csm_interface_init().
 * This routine is called when a PRI/T1/E1 card is detected. This routine
 * will initialize the content of digit collection structures associated
 * with each PRI/T1 lines. 
 * 
 * Parameters:
 * slot - the PRI/T1/E1 card slot number.
 * max_dc_number - the number of digit collectors in the card.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_add_digit_collector (int slot, int max_dc_number, int dummy)
{
   int                ix;
   digit_collect_t   *dc;
   static boolean     first_time = TRUE;

   if (slot >= MAX_SLOTS)
       return;
 
   if (first_time) {
       first_time = FALSE;

       neat_ipc_message_register(MSG_NEAT_RBS2CSM_HI,
                                 csm_rx_ct1_event_from_neat,
                                 NEAT_IPC_MSG_PRI_HI);
       neat_ipc_message_register(MSG_NEAT_RBS2CSM_LO,
                                 csm_rx_ct1_event_from_neat,
                                 NEAT_IPC_MSG_PRI_LO);

       reg_add_dsx1_add_signaling_interface(csm_add_pri_signaling_interface,
                                            "dsx1_add_pri_signaling_interface");
       reg_add_dsx1_delete_signaling_interface
           (csm_delete_pri_signaling_interface,
            "dsx1_delete_pri_signaling_interface");
   }

   if (max_dc_number > MAX_DIGIT_COLLECT_PER_SLOT) {
       errmsg(&msgsym(INITSYS, CSM), "The max_dc_number is bigger than "
              "the maximum number allowed");
       max_dc_number = MAX_DIGIT_COLLECT_PER_SLOT;
   }
       
   modem_pool->max_digit_collect_num[slot] = max_dc_number;

   for (ix=0; ix < max_dc_number; ix++) {
       if (modem_mgmt_csm_debug)
           buginf("%s at slot %d and unit %d\n", 
                  "CSM_ADD_DIGIT_COLLECTOR", slot, ix);
       dc                      =  &modem_pool->digit_collect_pool[slot][ix];
       dc->dc_chnl.stream_type = TDM_DIGIT_COLLECT_STREAM;
       dc->dc_chnl.slot        = slot;
       dc->dc_chnl.unit        = ix;
       dc->collecting_digit    = DC_STATE_IDLE;
       dc->message_id          = 0;
       dc->modem_info          = NULL;
       dc->wdt_timeout_count   = 0;
       modem_pool->digit_collect_list[modem_pool->max_digit_collect_index++] 
                               = dc;
       modem_pool->free_digit_collector_num++;
   }
}

/*
 * =====================================================================
 * csm_init_interface()
 *
 * Description:
 * The call back routine is associated with reg_add_csm_interface_init().
 * This routine is called to initialize the modem_info contents associated
 * with CSM for a perticular modem slot found.
 *
 * Parameters:
 * slot - the modem card slot number.
 * dummy1, dummy2 - no parameters.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_init_interface (int slot, int dummy1, int dummy2)
{
    int port;
    modem_info_t *modem_info;
    modem_slot_t *modem_slot;

    if (slot >= MAX_SLOTS)
        return;

    modem_slot =  modem_pool->modem_slot[slot];
    if (modem_slot) {
        modem_slot->total_bitmap_modem_num 
                                = MAX_MODEM_BITMAP(modem_slot->total_modem_num);
        TS_VALUE(modem_slot->modem_reset_minimum_ticks) 
                                = CSM_MODEM_RESET_MINIMUM_TIME;
        for (port=0; port < modem_slot->total_modem_num; port++) {
            modem_info = modem_slot->modem_info[port];
            csm_init_modem_info(modem_info, slot, port);
        }
        
        nim_register_hi_irq_handler((ushort) slot, csm_modem_interrupt,
                                    AS5200_NIM_HI_IRQ_PRIOR_LOW, 
                                    ((ushort) slot));
    }
}

/****************************************************************************
 * CSM task/process:
 *
 */
/****************************************************************************
 * CSM timer call back functions:
 *
 */

/*
 * =====================================================================
 * csm_activate_periodical_background_check()
 *
 * Description:
 * A timer callback function to enable the next round of csm periodical
 * background checking for all of modems.
 *
 * Parameters:
 * None. All of three are dummy parameters.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_activate_periodical_background_check (void *dummy1, 
                                                      void *dummy2, 
                                                      void *dummy3)
{
    process_wakeup(modem_pool->background_check_task_pid);
}

/*
 * =====================================================================
 * csm_ct1_rbs_adjust_idle_dsx0()
 *
 * Description:
 * This routine will busyout or unbusyout dsx0 on the T1 trunk.
 *
 * Parameters:
 * busyout        - TRUE to busyout dsx0s, FALSE to unbusyout dsx0s.
 * num_of_channel - number of channels to be adjusted.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_ct1_rbs_adjust_idle_dsx0 (boolean busyout, 
                                          ushort num_of_channel)
{
    ushort           index, num_of_dsx0_to_use, num_of_dsx0_to_adjust;
    dchan_info_t    *dchan_info;
    csm_message_t    csm_message;
    neat_ipc_msg_ret rc;

    if (!modem_pool->max_dchan_idb_index || !num_of_channel)
        return;

    csm_message.event = (busyout) ? EVENT_DISABLE_CHANNEL 
                                  : EVENT_ENABLE_CHANNEL;
    csm_message.cause = USER_BUSY;

    for(index=0; (index < modem_pool->max_dchan_idb_index) && num_of_channel; 
        index++) {
        dchan_info = modem_pool->dchan_info[index];
        if (!dchan_info->dchan_idb) {
            num_of_dsx0_to_use = (busyout) ? dchan_info->num_of_channel 
                                           : dchan_info->num_of_busyout;
            if (!num_of_dsx0_to_use)
                continue;

            if (num_of_dsx0_to_use >= num_of_channel)
                num_of_dsx0_to_adjust = num_of_channel;
            else
                num_of_dsx0_to_adjust = num_of_channel - num_of_dsx0_to_use;
            num_of_channel                   -= num_of_dsx0_to_adjust;
            csm_message.dest_slot             = dchan_info->slot;
            csm_message.data.busyout_channels = num_of_dsx0_to_adjust;
            rc = neat_ipc_message_send(csm_message.dest_slot,
                                       MSG_MPU_CSM2RBS,
                                       (uchar*) &csm_message,
                                       sizeof(csm_message_t));
            if ((rc != IPCMSG_ERROR_NOERROR) && modem_mgmt_csm_debug)
                buginf("%s: failed rc = 0x%x", "CSM_CT1_RBS_ADJUST_IDLE_DSX0"
                       , rc);
        }
    }    
}


/*
 * =====================================================================
 * process csm_periodical_background_check_process()
 *
 * Description:
 * A process invoked periodically to do the background checking for all of
 * modems. The main purpose is to check if the modem's wdt timeout. If 
 * timeout happens, a recovery procedure needs to be done to bring the
 * modem back to the default state again.
 *
 * Parameters:
 * None. 
 *
 * Returns:
 * None.
 * =====================================================================
 */
static process csm_periodical_background_check_process (void)
{
   ushort         slot, port, total_modem_num, modem_checked_num;
   modem_slot_t **modem_slot_p;
   modem_info_t **modem_info_p, *modem_info;
   ulong          time_elapsed;
   ushort         total_rbs_timeslot;
   
   process_wait_on_system_init();

   while (modem_pool && modem_pool->modem_mgmt_subsys_running) {

       process_wait_for_event();
       
       /*
        * Check if the modem pool is empty
        * if yes, we need to busy out some CT1 channels ...
        */
       total_rbs_timeslot = modem_pool->total_free_rbs_timeslot + 
                            modem_pool->total_busy_rbs_timeslot;
       if (total_rbs_timeslot && modem_pool->min_free_modem_threshold) {
           list_element           *element;
           list_header            *header;
           ushort                  total_free_modem;
           free_modem_list_index_t modem_list_index;
           boolean                 stop_searching;
           
           total_free_modem = 0;
           
           header = modem_pool->free_modem_header;
           stop_searching=FALSE; 
           for (modem_list_index=CSM_FREE_MODEM;
                (modem_list_index < MAX_CSM_FREE_MODEM_LIST) && !stop_searching;
                modem_list_index++, header++) {
               FOR_ALL_ELEMENTS_IN_LIST(header, element) {
                   modem_info = LIST_GET_DATA(element);
                   if (CSM_IS_IDLE_READY(modem_info)
                       && csm_modem_is_ready_to_take_ic(modem_info)) {
                       if ((++total_free_modem >= total_rbs_timeslot) ||
                           (!modem_pool->total_busy_rbs_timeslot && 
                            (total_free_modem > 
                             modem_pool->min_free_modem_threshold))) {
                           stop_searching = TRUE;
                           break;
                       }
                   }
               }
           }
           
           if (modem_pool->total_free_rbs_timeslot >= total_free_modem) {
               /*
                * If the number of free timeslot is more than free modem, we 
                * need to disable some timeslots.
                */
               if (total_free_modem <= modem_pool->min_free_modem_threshold)
                   csm_ct1_rbs_adjust_idle_dsx0
                       (CSM_BUSYOUT_DSX0, (modem_pool->total_free_rbs_timeslot 
                                           - total_free_modem));
           } else if (modem_pool->total_busy_rbs_timeslot) {
               /*
                * If the free modems are more than free timeslots,
                * we can unbusyout timeslot if any.
                */
               if (total_free_modem >= total_rbs_timeslot)
                   csm_ct1_rbs_adjust_idle_dsx0
                       (CSM_UNBUSYOUT_DSX0, 
                        modem_pool->total_busy_rbs_timeslot);
               else
                   csm_ct1_rbs_adjust_idle_dsx0
                       (CSM_UNBUSYOUT_DSX0, 
                        (total_free_modem - 
                         modem_pool->total_free_rbs_timeslot));
           }
       }

       /*
        * Check if the watchdog timer times out for each individual 
        * modem calling state
        */ 
       if (--modem_pool->background_check_countdown == 0) {
           modem_pool->background_check_countdown 
                                     = CSM_PERIODICAL_BACKGROUND_CHECK_TIME;
           modem_checked_num = 0;
           modem_slot_p      = modem_pool->modem_slot;
           for (slot=0; slot < MAX_SLOTS; slot++, modem_slot_p++) {
               if (!*modem_slot_p)
                   continue;
               modem_info_p    = (*modem_slot_p)->modem_info;
               total_modem_num = (*modem_slot_p)->total_modem_num;
               for (port=0; port < total_modem_num; port++, modem_info_p++) {
                   modem_info = *modem_info_p;
                   if (modem_info && CSM_IS_ACTIVE_WDT(modem_info)) {
                       time_elapsed 
                           = ELAPSED_TIME(modem_info->wdt_timestamp_started);
                       if (time_elapsed >= modem_pool->wdt_timeout_val) {
                           /*
                            * WDT timeout, need to recover the bad modem ...
                            */
                           leveltype SR;
                           SR = raise_interrupt_level(NETS_DISABLE);
                           modem_info->csm_event = CSM_EVENT_WDT_TIMEOUT;
                           (*modem_info->csm_event_proc)(modem_info);
                           reset_interrupt_level(SR);
                           /*
                            * if there's a timeout occurred, increase 
                            * modem_checked_num by 5 to push 
                            * process_may_suspend() to happen earlier.
                            */
                           modem_checked_num += 5;
                       }
                   }
                   
                   if (++modem_checked_num > 10) {
                       process_may_suspend();
                       modem_checked_num = 0;
                   }
               }
           }
       }
   }

   if (modem_pool) 
       modem_pool->modem_mgmt_subsys_running = NO_PROCESS;
}

#if CSM_EVENT_QUEUE_ENABLE
static process csm_event_handler_process (void)
{
    csm_event_queue_t *csm_event_queue;
    modem_info_t      *modem_info;
    leveltype          SR;
    uchar              ix;

    process_wait_on_system_init();

    while (TRUE) {
        process_suspend();
        for(ix=0; csm_event_queue_count;) {
            SR = raise_interrupt_level(NETS_DISABLE);
            csm_event_queue = read_next_csm_event_inline();
            reset_interrupt_level(SR);
            
            modem_info            = csm_event_queue->modem_info;
            modem_info->csm_event = csm_event_queue->csm_event;
            (*modem_info->csm_event_proc)(modem_info);
            
            if (++ix > 20) {
                process_may_suspend();
                ix = 0;
            }
        }
    }
}
#endif


#if CSM_USE_AT_COMMAND_CTRL
boolean async_tty_transmit_hold(modem_info_t *modem_info)
{
    /*
     * this function will return TRUE when nobody is using the tty port
     * or waiting for tty response from modem.
     */
    return (FALSE);
}
#endif

/*
 * =====================================================================
 * csm_ring_indication_proc()
 *
 * Description:
 * A timer callback function used to toggle the modem's ring indication pin
 * for every 2 seconds and 4 seconds. In the North America, the standard ringing
 * will be 2 seconds' ringing and 4 seconds' silence for every ringing cycle.
 *
 * Parameters:
 * None. All of three are dummy parameters.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_ring_indication_proc (void *dummy1, void *dummy2, void *dummy3)
{
   int            slot, ix;
   modem_slot_t **modem_slot_p, *modem_slot;
   modem_reg_t   *modem_reg;
   ushort        *ring_map, *ring_map_ignored, total_bitmap_modem_num;
   io_reg16_t    *ring_indicator;
   leveltype      SR;
   static const char func_name[] = "CSM_RING_INDICATION_PROC:";
#if CSM_USE_AT_COMMAND_CTRL
   list_element  *next_element;
   list_header   *list_hdr;
   modem_info_t  *modem_info;
#endif
   /*
    * if ring state is still in the RING_ON, we need to turn ring off when
    * there's no phone is ringing
    */
   if (modem_pool->ringer_num || (modem_pool->ring_state == RING_ON)) {
      switch (modem_pool->ring_state) {
      case RING_ON:       /* it's ringing, turn it off */
          if (modem_mgmt_csm_debug)
              buginf("\n%s%soff\n", func_name, " RI is ");

           modem_slot_p = modem_pool->modem_slot;
           for (slot=0; slot < MAX_SLOTS; slot++, modem_slot_p++) {
               if (*modem_slot_p) {  /* if modem exists */
                   /* 
                    * we need to make sure that there's no bus error when
                    * writing to modem card's reserved reg area.
                    */
                   modem_slot       = *modem_slot_p;
                   modem_reg        = modem_slot->modem_reg;
                   ring_map_ignored = modem_slot->ring_map_ignored;
                   ring_indicator   = modem_reg->ring_indicator;
                   total_bitmap_modem_num = modem_slot->total_bitmap_modem_num;
                   SR = raise_interrupt_level(NETS_DISABLE);
                   for (ix=0; ix < total_bitmap_modem_num; ix++) {
                       /* turn off ring indicators except for those T1 rings */
                       *(ring_indicator++) = -1 & ~*(ring_map_ignored++); 
                                            /* 0xffff */
                   }
                   reset_interrupt_level(SR);
               }
           }
           modem_pool->ring_state = RING_OFF_BEGIN;
           break;

      case RING_OFF_BEGIN: 
           /* 
            * it's in RING_OFF_BEGIN state, wait for another 2 seconds
            */
           modem_pool->ring_state = RING_OFF_END;
           break;

      case RING_OFF_END:   /* it's off, turn it on */
          if (modem_mgmt_csm_debug)
              buginf("\n%s%son\n", func_name, " RI is ");

           modem_slot_p = modem_pool->modem_slot;
           for (slot=0; slot < MAX_SLOTS; slot++, modem_slot_p++) {
               if (*modem_slot_p) {  /* if modem exists */
                   modem_slot       = *modem_slot_p;
                   modem_reg        = modem_slot->modem_reg;
                   ring_map         = modem_slot->ring_map;
                   ring_map_ignored = modem_slot->ring_map_ignored;
                   ring_indicator   = modem_reg->ring_indicator;
                   total_bitmap_modem_num = modem_slot->total_bitmap_modem_num;
                   SR = raise_interrupt_level(NETS_DISABLE);
                   for (ix=0; ix < total_bitmap_modem_num; ix++) { 
                       /* active low */
                       *(ring_indicator++) =  ~(*(ring_map++) 
                                                | *(ring_map_ignored++)); 
                   }
                   reset_interrupt_level(SR);
               }
           }
           modem_pool->ring_state = RING_ON;
           break;

      default:
           break;
      }
   }

#if CSM_USE_AT_COMMAND_CTRL
  /*
   * Use the AT command to answer the incoming call because
   * some modems don't support regular ring indication pulses
   * to make modem go off-hook.
   * Since tty is already locked when modem is ringing, there's
   * no need to lock the tty again when writing the AT string
   * to the modem.
   */
   list_hdr     = &modem_pool->atcmd_answer_header;
   next_element = LIST_HEAD_ELEMENT(list_hdr);
   while (next_element) {
       /*
        * Don't block the interrupt. The list element will only be deleted
        * at process level. If it's deleted in the interrupt in the future,
        * this loop should block the interrupt when remove the element out 
        * of queue.
        */
       modem_info   = LIST_GET_DATA(next_element);
       next_element = LIST_NEXT_ELEMENT(next_element);
       if (!async_tty_transmit_hold(modem_info)) {
           /*
            * we will send out the AT command only when no other process
            * is using the tty port or waiting for response from the tty
            * port. If there's a process doing that, we wait until next ring
            * cycle to try to send the command.
            */
           list_remove(&modem_pool->atcmd_answer_header,
                       &modem_info->atcmd_answer_link,
                       modem_info);
           quick_puts(modem_info->modem_tty, AT_CMD_ANSWER_CALL);
           if (CSM_DEBUG_ENABLED(modem_info))
               buginf("\n%s send AT command to modem slot %d, port %d\n", 
                      func_name, CSM_MODEM_SLOT(modem_info),
                      CSM_MODEM_PORT(modem_info));
       }
   }
#endif
}

/****************************************************************************
 * CSM functions running only under processes:
 *
 */
static void csm_print_modem_list (list_header *list_hdr, uchar *description)
{
    list_element *list_link;
    modem_info_t *modem_info;

    printf("%s:", description);
    FOR_ALL_ELEMENTS_IN_LIST(list_hdr, list_link) {
        modem_info = LIST_GET_DATA(list_link);
        if (modem_info) 
            printf("(s%d, c%d) ", CSM_MODEM_SLOT(modem_info), 
                   CSM_MODEM_PORT(modem_info));
    }
    printf("\n");
}

/*
 * =====================================================================
 * csm_show_modem_info()
 *
 * Description:
 * A command line end function to show the internal information of the modem.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
void csm_show_modem_info (modem_info_t *modem_info)
{
    ushort        ix;
    ulong        mask, status;
    modem_reg_t  *modem_reg;
    digit_collect_t *dc;

    if (!modem_info) {
        return;
    }

    printf("\nMODEM_INFO: slot %d, port %d, unit %d, modem_mask=0x%04x, "
           "modem_port_offset=%d\n"
           , CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info), 
           modem_info->unit, modem_info->modem_mask,
           modem_info->modem_port_offset);

    printf("tty_hwidb=0x%08x, modem_tty=0x%08x, mgmt_tty=0x%08x, "
           "modem_pool=0x%08x\n", 
           (modem_info->modem_tty ? modem_info->tty_hwidb : NULL), 
           modem_info->modem_tty, modem_info->mgmt_tty, modem_info->modem_pool);

    if (modem_info->busyout_rsp_queue || modem_info->busyout_rsp_msg)
        printf("busyout_rsp_queue=0x%08x, busyout_rsp_msg=0x%08x\n",
               modem_info->busyout_rsp_queue, modem_info->busyout_rsp_msg);

    if (modem_info->remote_b2b_modem_info)
        printf("remote_b2b_modem_info: slot %d, port %d, unit %d, "
               "modem_mask=0x%04x, modem_port_offset=%d\n", 
               CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info), 
               modem_info->unit, modem_info->modem_mask, 
               modem_info->modem_port_offset);
    
    if (CSM_IS_UNLOCKED(modem_info))
        printf("csm_status(0): %s\n", "CSM_STATUS_UNLOCKED");
    else {
        printf("csm_status(0x%08x): ", modem_info->csm_status);
        status = modem_info->csm_status;
        for (ix=0, mask=1; status; mask<<=1, ix++) {
            if (mask & status) {
                printf("%s.", (ix < CSM_STATUS_MAX_LENGTH) 
                              ? (char*) csm_status_name[ix] : "UNKNOWN");
                status &= ~mask;
            }
        }
        printf("\n");
    }

    printf("csm_state(0x%08x)=%s, csm_event_proc=0x%08x", modem_info->csm_state,
           csm_get_csm_state(modem_info->csm_state),  
           modem_info->csm_event_proc);

    if (CSM_IS_IC_OC_STATE(modem_info))
        printf(", current call thru %s line\n", 
               CSM_USE_PRI_LINE(modem_info) ? "PRI" : "CT1");
    else
        printf("\n");
    
    printf("invalid_event_count=%d, wdt_timeout_count=%d\n",
           modem_info->invalid_event_count, modem_info->wdt_timeout_count);

    if (CSM_IS_ACTIVE_WDT(modem_info)) {
        printf("elapsed time for wdt_timestamp_started=");
        print_ticks_in_dhms(ELAPSED_TIME(modem_info->wdt_timestamp_started));
        printf(", timeout threshold=");
        print_ticks_in_dhms(modem_pool->wdt_timeout_val);
        printf("\n");
    }
    else 
        printf("wdt_timestamp_started is not activated\n");

    printf("wait_for_dialing:%s, wait_for_bchan:%s\n",
           (LIST_ELEMENT_QUEUED(&modem_info->waiting_dial_link))
           ? "True" : "False",
           (LIST_ELEMENT_QUEUED(&modem_info->waiting_bchan_link))
           ?"True" : "False");
        
    dc = modem_info->dc;
    if (dc)
        printf("collecting_digit=%s, message_id=%d, dc_chnl=%s(s%d, u%d)\n",
               dc_state_name[dc->collecting_digit], dc->message_id, 
               stream_name[dc->dc_chnl.stream_type], dc->dc_chnl.slot, 
               dc->dc_chnl.unit);

    printf("pri_chnl=%s(s%d, u%d, c%d), modem_chnl=%s(s%d, c%d)\n",
           stream_name[modem_info->pri_chnl.stream_type], 
           modem_info->pri_chnl.slot, 
           modem_info->pri_chnl.unit, modem_info->pri_chnl.channel, 
           stream_name[modem_info->modem_chnl.stream_type], 
           modem_info->modem_chnl.slot, modem_info->modem_chnl.channel);

    printf("dchan_idb_start_index=%d, dchan_idb_index=%d, call_id=0x%04x, "
           "bchan_num=%d\n",
           modem_info->dchan_idb_start_index, modem_info->dchan_idb_index,
           modem_info->call_id, modem_info->bchan_num);
    
    printf("csm_event=%s, cause=0x%04x\n", 
           csm_event_name[modem_info->csm_event], modem_info->cause);
    
    modem_reg = modem_pool->modem_slot[CSM_MODEM_SLOT(modem_info)]->modem_reg;
    ix        = modem_info->modem_port_offset;
    printf("ring_indicator=%d, oh_state=%d, oh_int_enable=%d, modem_reset=%d\n",
           (modem_reg->ring_indicator[ix] & modem_info->modem_mask) ? 1 : 0,
           (modem_reg->oh_state[ix] & modem_info->modem_mask) ? 1 : 0,
           (modem_reg->oh_int_enable[ix] &  modem_info->modem_mask) ? 1 : 0,
           (modem_reg->modem_reset[ix] &  modem_info->modem_mask) ? 1 : 0);

    if (modem_info->connection_fail_reason) {
        printf("fail_csm_state(0x%08x)=%s, ", modem_info->fail_csm_state,
               csm_get_csm_state(modem_info->fail_csm_state));
    
        printf("connection_fail_reason=%s\n", 
               csm_fail_code_name[modem_info->connection_fail_reason]);
    }

    printf("ring_no_answer=%d, ic_failure=%d, ic_complete=%d\n",
           modem_info->stat_ring_no_answer, modem_info->stat_ic_failure,
           modem_info->stat_ic_complete);

    printf("dial_failure=%d, oc_failure=%d, oc_complete=%d\n",
           modem_info->stat_dial_failure, modem_info->stat_oc_failure,
           modem_info->stat_oc_complete);

    printf("oc_busy=%d, oc_no_dial_tone=%d, oc_dial_timeout=%d\n",
           modem_info->stat_oc_busy, modem_info->stat_oc_no_dial_tone,
           modem_info->stat_oc_dial_timeout);

    printf("remote_link_disc=%d, busyout=%d, modem_reset=%d\n",
           modem_info->stat_remote_link_disc, modem_info->stat_busyout,
           modem_info->stat_modem_reset);

    printf("call_duration_started=");
    print_ticks_in_dhms(TS_VALUE32(modem_info->stat_call_duration_started));
    printf(", call_duration_ended=");
    print_ticks_in_dhms(TS_VALUE32(modem_info->stat_call_duration_ended));
    printf(", total_call_duration=");
    print_ticks_in_dhms(TS_VALUE32(modem_info->stat_total_call_duration));
    printf("\n");
    printf("The calling party phone number = %s\n", 
           modem_info->calling_party_num);
    printf("The called party phone number  = %s\n", 
           modem_info->called_party_num);

    if (modem_mgmt_csm_debug) {
        printf("total_free_rbs_timeslot = %d, total_busy_rbs_timeslot = %d, "
               "min_free_modem_threshold = %d\n",
               modem_pool->total_free_rbs_timeslot, 
               modem_pool->total_busy_rbs_timeslot, 
               modem_pool->min_free_modem_threshold);
    }
}

/*
 * =====================================================================
 * csm_lock_modem_port()
 *
 * Description:
 * Set the status bits in the variable csm_status. Only one status bit
 * is supposed to be set for each function call. This function will disable
 * the modem oh interrupt and remove the modem out of modem pool.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * status_mask - status bit mask. Bit value set to 1 indicates the request.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_lock_modem_port (modem_info_t *modem_info, ulong status_mask)
{
    csm_disable_oh_interrupt(modem_info);
    modem_lock_tty(modem_info);
    list_remove(modem_info->free_modem_header_ptr,
                &modem_info->free_modem_link,
                modem_info);
    CSM_SET_STATUS(modem_info, status_mask);
}

/*
 * =====================================================================
 * csm_unlock_modem_port()
 *
 * Description:
 * Clear the status bits in the variable csm_status. Only one status bit
 * is supposed to be set for each function call. This function will enable
 * the modem oh interrupt and insert the modem into modem pool.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * status_mask - status bit mask. Bit value set to 1 indicates the request.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_unlock_modem_port (modem_info_t *modem_info, ulong status_mask)
{
    CSM_CLEAR_STATUS(modem_info, status_mask);
    if (CSM_IS_UNLOCKED(modem_info)) {    
        modem_unlock_tty(modem_info);
        list_enqueue(modem_info->free_modem_header_ptr,
                     &modem_info->free_modem_link,
                     modem_info);
        csm_enable_oh_interrupt(modem_info);
    }
}

/*
 * =====================================================================
 * csm_set_csm_status()
 *
 * Description:
 * Set the status bits in the variable csm_status. Only one status bit 
 * is supposed to be set for each function call. If the CSM_STATUS_MODEM_RESET
 * is set, csm_start_modem_reset() will be called to set the modem reset
 * ping to 1. The reason for placing this reset action here is because the
 * level 4 interrupt is disable in the function context. Setting any bit of the 
 * status bits will prevent the modem from making outgoing calls and answering
 * incoming calls.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * status_mask - status bit mask. Bit value set to 1 indicates the request. 
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
static boolean csm_set_csm_status (modem_info_t *modem_info, ulong status_mask)
{
    leveltype SR;
    boolean   rc;
 
    if (!modem_info) {   /* error */
        errmsg(&msgsym(MODEM_INFO, CSM), "CSM_SET_CSM_STATUS");
        return (RC_FAILURE);
    }
 
    rc = RC_SUCCESS;
    SR = raise_interrupt_level(NETS_DISABLE);
 
    if (CSM_IS_IDLE_STATE(modem_info)) {
        if (CSM_IS_UNLOCKED(modem_info)) {
            if (MODEM_TTY_IN_USE(modem_info))
                /*
                 * somebody is using the tty port
                 * reject the request.
                 */
                rc = RC_FAILURE;
            else 
                csm_lock_modem_port(modem_info, status_mask);
        } else
            CSM_SET_STATUS(modem_info, status_mask);

        if ((rc == RC_SUCCESS) && (status_mask & CSM_STATUS_MODEM_RESET))
                csm_start_modem_reset(modem_info);
    } else
        rc = RC_FAILURE;

    reset_interrupt_level(SR);
    return (rc);
}

/*
 * =====================================================================
 * csm_clear_csm_status()
 *
 * Description:
 * Clear the status bits in the variable csm_status. Only one status bit
 * is supposed to be set for each function call. If the CSM_STATUS_MODEM_RESET
 * is clear, csm_stop_modem_reset() will be called to set the modem reset
 * ping to 0. The reason for placing this reset action here is because the
 * level 4 interrupt is disable in the function context.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * status_mask - status bit mask. Bit value set to 1 indicates the request.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
static boolean csm_clear_csm_status (modem_info_t *modem_info, 
                                     ulong status_mask)
{
    leveltype SR;
    boolean   rc;
 
    if (!modem_info) {   /* error */
        errmsg(&msgsym(MODEM_INFO, CSM), "CSM_CLEAR_CSM_STATUS");
        return (RC_FAILURE);
    }
 
    rc = RC_SUCCESS;
    SR = raise_interrupt_level(NETS_DISABLE);
    
    if (CSM_IS_IDLE_STATE(modem_info)) {
        csm_unlock_modem_port(modem_info, status_mask);

        if (status_mask & CSM_STATUS_MODEM_RESET)
            csm_stop_modem_reset(modem_info);
    }
    else if (CSM_IS_SHUTDOWN(modem_info))
        CSM_CLEAR_STATUS(modem_info, status_mask);
    else
        rc = RC_FAILURE;

    reset_interrupt_level(SR);
    return (rc);
}

/*
 * =====================================================================
 * csm_set_busy_out_status()
 *
 * Description:
 * Request the modem to be nicly busyout. If the modem is not in the idle
 * state, the busyout action needs to be pending until the modem enters
 * idle state and then a message will be sent back to the requester if
 * the message queue is provided when this function is called. Setting the
 * busyout bit will prevent the modem from making outgoing calls and answering
 * incoming calls.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * event_queue - the message queue pointer which will be used to notify
 *               the requester when modem enters idle state.
 * busyout_rsp - the message pointer which will be used to notify
 *               the requester when modem enters idle state.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the modem is busyout immediately.
 * RC_FAILURE (FALSE) if the busyout request is pending.
 * =====================================================================
 */
boolean csm_set_busy_out_status (modem_info_t *modem_info, 
                                 watched_queue *event_queue,
                                 void *busyout_rsp)
{
    leveltype SR;
    boolean   rc;
    static const char func_name[] = "CSM_SET_BUSY_OUT_STATUS:";

    if (!modem_info) {   /* error */
        errmsg(&msgsym(MODEM_INFO, CSM), func_name);
        return (RC_FAILURE);
    }

    rc = RC_SUCCESS;

    SR = raise_interrupt_level(NETS_DISABLE);

    if (CSM_IS_IDLE_STATE(modem_info) && 
        (CSM_IS_LOCKED(modem_info) ||
         !MODEM_TTY_IN_USE(modem_info))) { 
        csm_lock_modem_port(modem_info, CSM_STATUS_BUSYOUT_REQ);
        modem_info->stat_busyout++;
    }
    else {  
        /*
         * the request is pending
         * when either CSM is doing something or modem tty is doing something
         */
        if (event_queue && busyout_rsp) {
            if (!modem_info->busyout_rsp_queue && !modem_info->busyout_rsp_msg){
                modem_info->busyout_rsp_queue = event_queue;
                modem_info->busyout_rsp_msg   = busyout_rsp;
            }
            else if (CSM_DEBUG_ENABLED(modem_info))
                buginf("ERROR: %s cannot overwrite the existing "
                       "pending busyout request at slot %d, port %d.\n", 
                       func_name, CSM_MODEM_SLOT(modem_info), 
                       CSM_MODEM_PORT(modem_info));
        }
        else {
            modem_info->busyout_rsp_queue = NULL;
            modem_info->busyout_rsp_msg   = NULL;
        }
        CSM_SET_STATUS(modem_info, CSM_STATUS_BUSYOUT_REQ);
        rc = RC_FAILURE;
    }

    reset_interrupt_level(SR);
    return (rc);
}

/*
 * =====================================================================
 * csm_clear_busy_out_status()
 *
 * Description:
 * Request the modem to clear busyout. 
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_clear_busy_out_status (modem_info_t *modem_info)
{
    leveltype SR;
    boolean   rc;

    if (!modem_info) {   /* error */
        errmsg(&msgsym(MODEM_INFO, CSM), "CSM_CLEAR_BUSY_OUT_STATUS");
        return (RC_FAILURE);
    }

    rc = RC_SUCCESS;
    SR = raise_interrupt_level(NETS_DISABLE);

    CSM_CLEAR_STATUS(modem_info, CSM_STATUS_BUSYOUT_REQ);
    modem_info->busyout_rsp_queue = NULL;
    modem_info->busyout_rsp_msg   = NULL;

    if (CSM_IS_IDLE_READY(modem_info)) { 
        /*
         * When the busy out is cancelled, it doesn't mean
         * that the csm can start running because some other
         * busyout dependent actions may be in the middle of
         * execution. Will enable the interrupt only when 
         * CSM_IS_IDLE_READY() is true.
         */
        csm_unlock_modem_port(modem_info, CSM_STATUS_NONE);
    }

    reset_interrupt_level(SR);
    return (rc);
}

 
/*
 * =====================================================================
 * csm_set_modem_back2back_test()
 *
 * Description:
 * Request two modems to enter the back to back test configuration. Setting the 
 * back2back testing bit will prevent the modem from making outgoing calls and 
 * answering incoming calls.
 *
 * Parameters:
 * modem_info1 - modem1 information descriptor pointer.
 * modem_info2 - modem2 information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_set_modem_back2back_test (modem_info_t *modem_info1, 
                                      modem_info_t *modem_info2)
{
    boolean rc;
    leveltype SR;

    if (!modem_info1 || !modem_info2) {
        if (modem_mgmt_csm_debug)
            buginf("\nERROR: %s: modem_info1 or modem_info2 is NULL\n",
                   "CSM_SET_MODEM_BACK2BACK_TEST");
        return (RC_FAILURE);       /* error */
    }

    SR = raise_interrupt_level(NETS_DISABLE);
    rc = RC_SUCCESS;

    /*
     * Either CSM_STATUS_BACK2BACK_TEST or CSM_STATUS_BUSYOUT_REQ 
     * can lock out CSM.
     */
    if (CSM_IS_IDLE_STATE(modem_info1) && CSM_IS_IDLE_STATE(modem_info2)) {
        rc = tdm_connect_twoway(&modem_info1->modem_chnl, 
                                &modem_info2->modem_chnl);
        if (rc == RC_SUCCESS) {
            modem_info1->remote_b2b_modem_info = modem_info2;
            csm_lock_modem_port(modem_info1, CSM_STATUS_BACK2BACK_TEST);

            modem_info2->remote_b2b_modem_info = modem_info1;
            csm_lock_modem_port(modem_info2, CSM_STATUS_BACK2BACK_TEST);
        }
    } else
        rc = RC_FAILURE;
    
    reset_interrupt_level(SR);
    return (rc);
}

/*
 * =====================================================================
 * csm_clear_modem_back2back_test()
 *
 * Description:
 * Request two modems to exit the back to back test configuration. 
 *
 * Parameters:
 * modem_info1 - modem1 information descriptor pointer.
 * modem_info2 - modem2 information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_clear_modem_back2back_test (modem_info_t *modem_info1, 
                                        modem_info_t *modem_info2)
{
    boolean rc;
    leveltype SR;

    if (modem_info1 && CSM_IS_BACK2BACK_TEST(modem_info1)) {
        if (!modem_info2)
            modem_info2 = modem_info1->remote_b2b_modem_info;
        else if (modem_info1->remote_b2b_modem_info != modem_info2)
            return (RC_FAILURE);  /* error */
    }
    else if (modem_info2 && CSM_IS_BACK2BACK_TEST(modem_info2)) {
        if (modem_info1)
            return (RC_FAILURE);  /* error */
        modem_info1 = modem_info2->remote_b2b_modem_info;
    }
    else
        return (RC_FAILURE);       /* error */

    SR = raise_interrupt_level(NETS_DISABLE);
    rc = RC_SUCCESS;

    rc = tdm_connect_special(DEFAULT_RX_TX, &modem_info1->modem_chnl);
    
    if (rc == RC_SUCCESS)
        rc = tdm_connect_special(DEFAULT_RX_TX, &modem_info2->modem_chnl);
    
    if (rc == RC_SUCCESS) {
        modem_info1->remote_b2b_modem_info = NULL;
        CSM_CLEAR_STATUS(modem_info1, CSM_STATUS_BACK2BACK_TEST);
        if (CSM_IS_IDLE_READY(modem_info1))
            csm_unlock_modem_port(modem_info1, CSM_STATUS_NONE);

        modem_info2->remote_b2b_modem_info = NULL;
        CSM_CLEAR_STATUS(modem_info2, CSM_STATUS_BACK2BACK_TEST);
        if (CSM_IS_IDLE_READY(modem_info2))
            csm_unlock_modem_port(modem_info2, CSM_STATUS_NONE);
    }

    reset_interrupt_level(SR);
    return (rc);
}

/*
 * =====================================================================
 * csm_set_download_file_status()
 *
 * Description:
 * Before a modem gets its firmware downloaded, this function needs to 
 * be called to lock out the incoming and outgoing calls. Therefore,
 * a downloading can occur without busying out the modem.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_set_download_file_status (modem_info_t *modem_info)
{
    return (csm_set_csm_status(modem_info, CSM_STATUS_DOWNLOAD_FILE));
}

/*
 * =====================================================================
 * csm_clear_download_file_status()
 *
 * Description:
 * After a modem gets its firmware downloaded, this function needs to
 * be called to clear the downloading status.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_clear_download_file_status (modem_info_t *modem_info)
{
    return (csm_clear_csm_status(modem_info, CSM_STATUS_DOWNLOAD_FILE));
}

/*
 * =====================================================================
 * csm_set_download_fail_status()
 *
 * Description:
 * During a modem fails its firmware downloading process, this function
 * needs to be called to stop the future incoming calls and outgoing calls
 * activities at this modem. 
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_set_download_fail_status (modem_info_t *modem_info)
{
    return (csm_set_csm_status(modem_info, CSM_STATUS_DOWNLOAD_FAIL));
}

/*
 * =====================================================================
 * csm_clear_download_file_status()
 *
 * Description:
 * After a modem gets its firmware downloaded, this function needs to
 * be called to clear this failure status.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_clear_download_fail_status (modem_info_t *modem_info)
{
    return (csm_clear_csm_status(modem_info, CSM_STATUS_DOWNLOAD_FAIL));
}

/*
 * =====================================================================
 * csm_set_bad_modem_status()
 *
 * Description:
 * Whenever a modem is identified as a bad one by either self-test 
 * program or users, this function needs to be called to stop any
 * future incoming calls and outgoing calls activities at this modem.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_set_bad_modem_status (modem_info_t *modem_info)
{
    return (csm_set_csm_status(modem_info, CSM_STATUS_BAD_MODEM));
}

/*
 * =====================================================================
 * csm_clear_bad_modem_status()
 *
 * Description:
 * When the modem is verified to be good or user wants to
 * test the bad modem, this flag will be cleared.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_clear_bad_modem_status (modem_info_t *modem_info)
{
    return (csm_clear_csm_status(modem_info, CSM_STATUS_BAD_MODEM));
}

/*
 * =====================================================================
 * csm_set_busy_status()
 *
 * Description:
 * Whenever the system wants to lock a particular modem for
 * any reason which is not supported by those existing status, 
 * this function can be called to stop any future incoming calls 
 * and outgoing calls activities at this modem.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_set_busy_status (modem_info_t *modem_info)
{
    return (csm_set_csm_status(modem_info, CSM_STATUS_BUSY));
}

/*
 * =====================================================================
 * csm_clear_busy_status()
 *
 * Description:
 * To reverse the internal busy lock out of a modem,
 * this flag will be cleared.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_clear_busy_status (modem_info_t *modem_info)
{
    return (csm_clear_csm_status(modem_info, CSM_STATUS_BUSY));
}

/*
 * =====================================================================
 * csm_set_modem_reset_status()
 *
 * Description:
 * A modem can be reset and hold the reset ping high as long as user
 * wants. This function is called to raise the reset pin to stay high.
 * For different modem vendors, the minimum time may be different. A 
 * passive timer is used to time and ensure the minimum reset time is
 * met.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_set_modem_reset_status (modem_info_t *modem_info)
{
    if (!modem_info) {   /* error */
        errmsg(&msgsym(MODEM_INFO, CSM), "CSM_SET_MODEM_RESET_STATUS");
        return (RC_FAILURE);
    }

    if (CSM_IS_MODEM_RESET(modem_info))
        return (RC_SUCCESS);

    if (csm_set_csm_status(modem_info, CSM_STATUS_MODEM_RESET)) {
        GET_TIMESTAMP(modem_info->modem_reset_started);
        return (RC_SUCCESS);
    }

    return (RC_FAILURE);
}

/*
 * =====================================================================
 * csm_clear_modem_reset_status()
 *
 * Description:
 * A modem can be reset and hold the reset ping high as long as user
 * wants. This function is called to lower the reset pin down to 0.
 * For different modem vendors, the minimum time may be different. A 
 * passive timer is used to time and ensure the minimum reset time is
 * met. If the minimum is not met when this function is called, the calling
 * process will be suspended until the minimum time is met when the flag
 * wait_for_reset is TRUE. If it's FALSE, there's no process suspension.
 * The flag wait_for_reset is useful when there're a list of modems needed
 * to be reset at the same time. All of function calls can pass FALSE flag
 * except the last one which will save lots of waiting time.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * wait_for_reset - TRUE if the calling process can be suspended 
 *                  FALSE if the calling process cannot be suspended
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_clear_modem_reset_status (modem_info_t *modem_info, 
                                      boolean wait_for_reset)
{
    /*
     * called only by processes
     */
    
    if (!modem_info) {   /* error */
        errmsg(&msgsym(MODEM_INFO, CSM), "CSM_CLEAR_MODEM_RESET_STATUS");
        return (RC_FAILURE);
    }

    if (CSM_IS_MODEM_RESET(modem_info) && wait_for_reset) {
        sys_timestamp current_ticks, min_reset_ticks;
        modem_slot_t *modem_slot;

        modem_slot      = modem_pool->modem_slot[CSM_MODEM_SLOT(modem_info)];
        TS_VALUE(min_reset_ticks)
                        = TS_VALUE(modem_slot->modem_reset_minimum_ticks);
        GET_TIMESTAMP(current_ticks);
        /*
         * the modem need to be reset at least for certain minimum amount 
         * of time
         */
        while ((ELAPSED_TIME(modem_info->modem_reset_started) < 
                TS_VALUE32(min_reset_ticks))
               && (ELAPSED_TIME(current_ticks) < TS_VALUE32(min_reset_ticks)))
            process_suspend();
    }
    
    return (csm_clear_csm_status(modem_info, CSM_STATUS_MODEM_RESET));
}

/*
 * =====================================================================
 * csm_set_shutdown_status()
 *
 * Description:
 * This is a function to force hard busyout on the specified modem. If 
 * the modem is connected on a call, this function will terminate the
 * call immediately and lock out the modem resource.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_set_shutdown_status (modem_info_t *modem_info)
{
    /*
     * called only by processes
     */
    leveltype SR;
    boolean   rc;

    if (!modem_info) {   /* error */
        errmsg(&msgsym(MODEM_INFO, CSM), "CSM_SET_SHUTDOWN_STATUS");
        return (RC_FAILURE);
    }
 
    rc = RC_SUCCESS;
    SR = raise_interrupt_level(NETS_DISABLE);
 
    if (CSM_IS_IDLE_STATE(modem_info)) {
        if (CSM_IS_UNLOCKED(modem_info)) {
            if (MODEM_TTY_IN_USE(modem_info))
                rc = RC_FAILURE;
            else
                csm_lock_modem_port(modem_info, CSM_STATUS_SHUTDOWN);
        } else 
            CSM_SET_STATUS(modem_info, CSM_STATUS_SHUTDOWN);
    }
    else {
        CSM_SET_STATUS(modem_info, CSM_STATUS_SHUTDOWN);
        modem_info->csm_event = CSM_EVENT_SHUTDOWN_INTERFACE;
        modem_info->cause     = CSM_CAUSE_SHUTDOWN_INTERFACE;
        (*modem_info->csm_event_proc)(modem_info);
    }

    reset_interrupt_level(SR);
    return (rc);
}

/*
 * =====================================================================
 * csm_clear_shutdown_status()
 *
 * Description:
 * This is a function to exit hard busyout on the specified modem. 
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
boolean csm_clear_shutdown_status (modem_info_t *modem_info)
{
    return (csm_clear_csm_status(modem_info, CSM_STATUS_SHUTDOWN));
}

/*
 * =====================================================================
 * csm_modem_allocate()
 *
 * Description:
 * This function is called when an incoming call needs to find and 
 * allocate a free modem resource to answer the call. The search order
 * of the free modem is from the lower slot to higher slot. 
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
static modem_info_t* csm_modem_allocate (modem_pool_t *modem_pool, 
                                 free_modem_list_index_t free_modem_list_req)
{
    list_header  *header;
    list_element *element;
    free_modem_list_index_t index;
    modem_info_t *modem_info;
    boolean       round_robin;
    leveltype     SR;
    static const char func_name[] = "CSM_MODEM_ALLOCATE:";
 
    SR = raise_interrupt_level(NETS_DISABLE);

    round_robin = (modem_pool->modem_control_flags 
                   & MODEM_CTRL_USE_ROUND_ROOBIN);

    if (round_robin) {
        for (index=free_modem_list_req; index < MAX_CSM_FREE_MODEM_LIST; 
             index++) {
            header = &modem_pool->free_modem_header[index];
            FOR_ALL_ELEMENTS_IN_LIST(header, element) {
                modem_info = LIST_GET_DATA(element);
                if (CSM_IS_IDLE_READY(modem_info)
                    && csm_modem_is_ready_to_take_ic(modem_info)) {
                    /*
                     * Don't consider the situation that the auto modem
                     * configuration may use the modem port during this
                     * allocation time. If modem's under auto-config,
                     * we treat it as a lock.
                     *
                     * Now we lock the modem internally without calling
                     * CLAIM_TTY().
                     */
                    list_remove(header, element, modem_info);
                    CSM_SET_STATUS(modem_info, CSM_STATUS_ACTIVE_CALL);
                    if (CSM_DEBUG_ENABLED(modem_info))
                        buginf("\n%s slot %d and port %d is allocated.\n", 
                               func_name, CSM_MODEM_SLOT(modem_info), 
                               CSM_MODEM_PORT(modem_info));
                    
                    reset_interrupt_level(SR);
                    return (modem_info);
                }
            }
            /*
             * If it's a special modem request, the search fails
             */
            if (free_modem_list_req != CSM_FREE_MODEM)
                break;
        }
    } else if (free_modem_list_req < MAX_CSM_FREE_MODEM_LIST) {
        ushort         slot,  port, old_slot, old_port;
        modem_info_t **modem_info_p;
        modem_slot_t  *modem_slot;
        ushort         total_modem_num, max_loop;
        boolean        first_free_slot, find_first_free_port, search;
        
        old_slot = slot = modem_pool->next_slot_search;
        old_port = port = modem_pool->next_port_search;
        first_free_slot = search = TRUE;
        find_first_free_port = CSM_MODEM_INFO_OR_NULL(slot,port) == NULL;
        max_loop = 0;
        header = &modem_pool->free_modem_header[free_modem_list_req];
        
        while (search) {
            modem_slot = modem_pool->modem_slot[slot];
            if (!modem_slot) {
                if (max_loop++ == MAX_SLOTS) {
                    search = FALSE;
                    break;
                }
                slot = (slot+1) % MAX_SLOTS;
                port = 0;
                continue;
            }
            total_modem_num = modem_slot->total_modem_num;
            modem_info_p = modem_slot->modem_info + port;
            for (; port < total_modem_num; modem_info_p++, port++) {
                if (*modem_info_p) {
                    if (find_first_free_port && first_free_slot 
                        && !round_robin) {
                        /*
                         * Skip those empty modem slots/ports when it's not
                         * a round robin
                         */
                        modem_pool->next_slot_search = slot;
                        modem_pool->next_port_search = port;
                        find_first_free_port = FALSE;
                    }
                    
                    modem_info = *modem_info_p;
                    if (CSM_IS_IDLE_READY(modem_info)
                        && csm_modem_is_ready_to_take_ic(modem_info)
                        && ((modem_info->free_modem_header_ptr == header) 
                            || (free_modem_list_req == CSM_FREE_MODEM))) {
                        /*
                         * Don't consider the situation that the auto modem 
                         * configuration may use the modem port during this 
                         * allocation time. If modem's under auto-config,
                         * we treat it as a lock.
                         *
                         * Now we lock the modem internally without calling
                         * CLAIM_TTY().
                         */
                        list_remove(modem_info->free_modem_header_ptr, 
                                    &modem_info->free_modem_link, 
                                    modem_info);
                        CSM_SET_STATUS(modem_info, CSM_STATUS_ACTIVE_CALL);
                        if (CSM_DEBUG_ENABLED(modem_info))
                            buginf("\n%s slot %d and port %d is allocated.\n", 
                                   func_name, slot, port);
                        if (round_robin)
                            if (++port < total_modem_num) {
                                modem_pool->next_slot_search = slot;
                                modem_pool->next_port_search = port;
                            } else {
                                modem_pool->next_slot_search 
                                                      = (slot+1) % MAX_SLOTS;
                                modem_pool->next_port_search = 0;
                            }
                        reset_interrupt_level(SR);
                        return (modem_info);  /* found it */
                    }
               }
                if (!first_free_slot && (slot == old_slot) && 
                    (port == old_port)) {
                    /*
                     * Cannot find an available modem to use
                     */
                    search = FALSE;
                    break;
                }
            }
            slot = (slot+1) % MAX_SLOTS;
            port = 0;
            first_free_slot = FALSE;
        }
    }
    if (modem_mgmt_csm_debug)
        buginf("\n%s failed to allocate a modem", func_name);
    reset_interrupt_level(SR);
    return (NULL);
}

/*
 * =====================================================================
 * csm_do_pending_request()
 *
 * Description:
 * After modem is deallocated or unlock, the modem may enter into IDLE state, 
 * nice busyout state, or shutdown (hard busyout) state. We need to make
 * sure that this function is coordinated with modem code to lock the port.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
boolean csm_do_pending_request (modem_info_t *modem_info)
{
    if (modem_info->csm_status & (CSM_STATUS_BUSYOUT_REQ|CSM_STATUS_SHUTDOWN)) {
        /* 
         * for calling from modem_release_override(), 
         * if call is still active, we need to lock the modem 
         * before we serve the pending request after the call 
         * is disconnected and the async process on the tty hangs up.
         */
      if (CSM_IS_IC_OC_STATE(modem_info) ||
	  TTY_IN_USE(modem_info->modem_tty)) 
	 return (TRUE);

        csm_lock_modem_port(modem_info, CSM_STATUS_NONE);

        /*
         * If there's a pending busyout request or shutdown status bits set, 
         * the modem needs to be disabled and appropriate message will be 
         * sent out if requested.
         */
        if (CSM_IS_BUSY_OUT(modem_info) && 
            modem_info->busyout_rsp_queue && modem_info->busyout_rsp_msg) {
            process_enqueue(modem_info->busyout_rsp_queue, 
                            modem_info->busyout_rsp_msg);
            modem_info->busyout_rsp_queue = NULL;
            modem_info->busyout_rsp_msg   = NULL;
        }

        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s: modem is busyout at slot %d, port %d\n",
                   "CSM_DO_PENDING_REQUEST", CSM_MODEM_SLOT(modem_info), 
                   CSM_MODEM_PORT(modem_info));
        
        return (TRUE);
     }
    return (FALSE);
}

/*
 * =====================================================================
 * csm_modem_deallocate()
 *
 * Description:
 * When an incoming call or outgoing call is disconnected, this function
 * will be called to free the modem back to the modem pool. After modem
 * is deallocated or unlock, the modem may enter into IDLE state, nice
 * busyout state, or shutdown (hard busyout) state.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * RC_SUCCESS (TRUE)  if the operation is successful.
 * RC_FAILURE (FALSE) if the operation fails.
 * =====================================================================
 */
static void csm_modem_deallocate (modem_info_t *modem_info)
{
   /*
    * Attention:
    * make sure that new modem status can have extra bit to handle
    * the tty locked out in addition to HASPROC. There're more status
    * extension bits defined in the 11.1 which can be utilized for this purpose.
    */
   leveltype     SR;
   static const char func_name[] = "CSM_MODEM_DEALLOCATE:";

   if (!modem_info) {   /* error */
       errmsg(&msgsym(MODEM_INFO, CSM), func_name);
       return;
   }
 
   SR = raise_interrupt_level(NETS_DISABLE);
   if (CSM_IS_IDLE_STATE(modem_info)) { 
       CSM_CLEAR_STATUS(modem_info, 
                        (CSM_STATUS_ACTIVE_WDT | CSM_STATUS_ACTIVE_CALL));
       csm_do_pending_request(modem_info);

       if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s slot %d and port %d is deallocated\n", func_name,
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
   }

   if (CSM_IS_LOCKED(modem_info)) {
       if (LIST_ELEMENT_QUEUED(&modem_info->free_modem_link))
           list_remove(modem_info->free_modem_header_ptr, 
                       &modem_info->free_modem_link, 
                       modem_info);
   } else {
       if (!LIST_ELEMENT_QUEUED(&modem_info->free_modem_link))
           list_enqueue(modem_info->free_modem_header_ptr, 
                        &modem_info->free_modem_link, 
                        modem_info);
   }

   reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * csm_add_pri_signaling_interface()
 *
 * Description:
 * This is a registry function which will be called from other subsystem.
 * Whenever there is a D channel is configured, this function will be called 
 * to register the signaling interface which will be used when an outgoing call 
 * is initiated.
 *
 * Parameters:
 * hwidb - hardware idb pointer for the D channel.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_add_pri_signaling_interface (hwidbtype *hwidb)
{
   ushort ix,     max_index;
   dchan_info_t **dchan;
   leveltype      SR;

   if (!hwidb) {
       errmsg(&msgsym(HWIDB, CSM), "CSM_ADD_PRI_SIGNALING_INTERFACE");
       return;    /* error */
   }

   max_index = modem_pool->max_dchan_idb_index;
   dchan     = modem_pool->dchan_info;
   for (ix=0; ix < max_index; ix++, dchan++) {
       if (!*dchan) {
           break; /*found the new slot */
       }

      if ((*dchan)->dchan_idb == hwidb)
         return;      /* duplicate */
   }

   if (ix >= MAX_TOTAL_SIGNAL_CHANNEL) 
      return;         /* no more free entry */

   *dchan = malloc(sizeof(dchan_info_t));
   
   if (*dchan == NULL) {
       errmsg(&msgsym(PRI, CSM), "malloc fails", 
              hwidb->slot, hwidb->unit, hwidb->vc, ix);
       return;
   }

   SR = raise_interrupt_level(NETS_DISABLE);
   modem_pool->max_dchan_idb_index++;
   (*dchan)->dchan_idb              = hwidb;
   (*dchan)->current_oc_call_id     = ISDN_MODEM_CALL_ID_MIN;
   (*dchan)->slot = CSM_PRI_SLOT;
   reset_interrupt_level(SR);
   errmsg(&msgsym(PRI, CSM), "add PRI", hwidb->slot, hwidb->unit, 
          hwidb->vc, ix);
}

/*
 * =====================================================================
 * csm_add_ct1_signaling_interface()
 *
 * Description:
 * This is a registry function which will be called whenever there is a
 * A/B Bit signaling channel is configured, this function will be called 
 * to register the signaling interface which will be used when an outgoing 
 * call is initiated.
 *
 * Parameters:
 * slot - the slot number where the signaling channel is created.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_add_ct1_signaling_interface (uchar slot, uchar controller,
                                             uchar timeslot, ushort cause)
{
   ushort ix,     max_index;
   dchan_info_t **dchan;
   leveltype      SR;
   static const char func_name[] = "CSM_ADD_CT1_SIGNALING_INTERFACE:";

   if (!modem_pool || modem_pool->modem_slot[slot]) {
       errmsg(&msgsym(RBS, CSM), "fail to add rbs", slot, controller,
              timeslot);
       return;    /* error */
   }

   max_index = modem_pool->max_dchan_idb_index;
   dchan     = modem_pool->dchan_info;
   for (ix=0; ix < max_index; ix++, dchan++) {
       if (!*dchan) {
           break; /*found the new slot */
       }

       if ((*dchan)->slot == slot) {
           if (cause == USER_BUSY) {
               (*dchan)->num_of_busyout--;
               (*dchan)->num_of_channel++;
               modem_pool->total_busy_rbs_timeslot--;
               modem_pool->total_free_rbs_timeslot++;
               if (modem_mgmt_csm_debug)
                   errmsg(&msgsym(RBSLINEUNBUSYOUT, DSX0), controller, 
                          timeslot+1);
               return;
           }

           errmsg(&msgsym(RBSLINEUP, DSX0), controller, timeslot+1);
           (*dchan)->num_of_channel++;
           modem_pool->total_free_rbs_timeslot++;
           return; 
       }
   }

   if ((ix >= MAX_TOTAL_SIGNAL_CHANNEL) || (cause == USER_BUSY))
      return;         /* no more free entry */

   *dchan = malloc(sizeof(dchan_info_t));

   if (*dchan == NULL) {
       errmsg(&msgsym(RBS, CSM), "malloc fails", slot, controller,
              timeslot);
       return;
   }

   SR = raise_interrupt_level(NETS_DISABLE);
   modem_pool->max_dchan_idb_index++;
   (*dchan)->dchan_idb              = NULL;  /* CT1 interface */
   (*dchan)->current_oc_call_id     = ISDN_MODEM_CALL_ID_MIN;
   (*dchan)->num_of_channel = 1;
   (*dchan)->num_of_busyout = 0;
   modem_pool->total_free_rbs_timeslot++;
   (*dchan)->slot = slot;
   reset_interrupt_level(SR);
   errmsg(&msgsym(RBSLINEUP, DSX0), controller, timeslot+1);
   if (modem_mgmt_csm_debug)
       buginf("\n%s add the first CT1 channel at slot %d with index %d\n",
              func_name, slot, ix);
}

/*
 * =====================================================================
 * csm_delete_pri_signaling_interface()
 *
 * Description:
 * This is a registry function which will be called from other subsystem.
 * Whenever there is a D channel is removed, this function will be called 
 * to de-register the signaling interface which will not be used when an 
 * outgoing call is initiated.
 *
 * Parameters:
 * hwidb - the hardware idb pointer for the D channel.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_delete_pri_signaling_interface (hwidbtype *hwidb)
{
   ushort ix,     max_index;
   dchan_info_t **dchan, *dchan_freed;
   leveltype      SR;
   static const char func_name[] = "CSM_DELETE_PRI_SIGNALING_INTERFACE:";

   if (!hwidb) {
       errmsg(&msgsym(HWIDB, CSM), func_name);
       return;    /* error */
   }

   max_index = modem_pool->max_dchan_idb_index;
   dchan     = modem_pool->dchan_info;
   for (ix=0; ix < max_index; ix++, dchan++) {
       if (!*dchan) {
           if (modem_mgmt_csm_debug)
               buginf("ERROR: %s cannot find the matched hwidb\n", func_name);
           return;        /* error */
       }

      if ((*dchan)->dchan_idb == hwidb) {
          /* 
           * need to make sure the existing calls associated with this interface
           * are disconnected correctly
           */
          ushort         slot, port, total_modem_num;
          modem_info_t **modem_info_p, *modem_info;
          modem_slot_t  *modem_slot;

          SR = raise_interrupt_level(NETS_DISABLE);
          for (slot=0; slot < MAX_SLOTS; slot++) {
              modem_slot = modem_pool->modem_slot[slot];
              if (!modem_slot)
                  continue;
              total_modem_num = modem_slot->total_modem_num;
              modem_info_p = modem_slot->modem_info;
              for (port=0; port < total_modem_num; port++, modem_info_p++) {
                  if (*modem_info_p) {
                      modem_info = *modem_info_p;
                      /*
                       * If there's an incoming call or outgoing call associated
                       * with the signaling interface deleted, the call needs to
                       * be disconnected immediately.
                       */
                      if ((modem_info->csm_state 
                           & (CSM_IC_STATE | CSM_OC_STATE))
                           && (modem_info->dchan_idb == hwidb)) {
                          modem_info->csm_event = CSM_EVENT_SHUTDOWN_INTERFACE;
                          modem_info->cause     = CSM_CAUSE_SHUTDOWN_INTERFACE;
                          (*modem_info->csm_event_proc)(modem_info);
                      }
                  }
              }
          }
          
          /*
           * delete the interface
           */
          dchan_freed = *dchan;
          max_index = --(modem_pool->max_dchan_idb_index);

          if (modem_pool->current_dchan_idb_index >= max_index) {
              modem_pool->current_dchan_idb_index = 0;
          }
          if (ix < max_index) {    /* if it's not the last one */
              *dchan = modem_pool->dchan_info[max_index];
              modem_pool->dchan_info[max_index] = NULL;
          }
          else
              *dchan = NULL;
          reset_interrupt_level(SR);
          free(dchan_freed);
          errmsg(&msgsym(PRI, CSM), "delete PRI", hwidb->slot, hwidb->unit,
                 hwidb->vc, ix);
          return;
      }
   } /* for */
}

/*
 * =====================================================================
 * csm_delete_ct1_signaling_interface()
 *
 * Description:
 * This is a registry function which will be called whenever there is a 
 * A/B Bit signaling channel is removed, this function will be called to 
 * de-register the signaling interface which will not be used when an outgoing 
 * call is initiated.
 *
 * Parameters:
 * slot - the slot number where the signaling channel is removed.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_delete_ct1_signaling_interface (uchar slot, uchar controller,
                                                uchar timeslot, ushort cause)
{
   ushort ix,     max_index;
   dchan_info_t **dchan, *dchan_freed;
   leveltype      SR;
   static const char func_name[] = "CSM_DELETE_CT1_SIGNALING_INTERFACE:";

   if (!modem_pool || modem_pool->modem_slot[slot]) {
       if (modem_mgmt_csm_debug)
           buginf("\nERROR: %s cannot add on the modem slot %d\n", func_name, 
                  slot);
       return;    /* error */
   }

   max_index = modem_pool->max_dchan_idb_index;
   dchan     = modem_pool->dchan_info;
   for (ix=0; ix < max_index; ix++, dchan++) {
       if (!*dchan) {
           if (modem_mgmt_csm_debug)
               buginf("ERROR: %s cannot find the matched slot %d\n", func_name,
                      slot);
           return;        /* error */
       }
       
       if ((*dchan)->slot == slot) {
           /*
            * delete the interface
            */
           if (cause == USER_BUSY) {
               (*dchan)->num_of_busyout++;
               (*dchan)->num_of_channel--;
               modem_pool->total_busy_rbs_timeslot++;
               modem_pool->total_free_rbs_timeslot--;
               if (modem_mgmt_csm_debug)
                   errmsg(&msgsym(RBSLINEBUSYOUT, DSX0), controller, 
                          timeslot+1);
               return;
           }

           modem_pool->total_free_rbs_timeslot--;
           if ((--((*dchan)->num_of_channel) == 0) && 
               !(*dchan)->num_of_busyout) {
               SR = raise_interrupt_level(NETS_DISABLE);
               dchan_freed = *dchan;
               max_index = --(modem_pool->max_dchan_idb_index);
               if (modem_pool->current_dchan_idb_index >= max_index) {
                   modem_pool->current_dchan_idb_index = 0;
               }
               if (ix < max_index) {    /* if it's not the last one */
                   *dchan = modem_pool->dchan_info[max_index];
                   modem_pool->dchan_info[max_index] = NULL;
               }
               else
                   *dchan = NULL;
               reset_interrupt_level(SR);
               free(dchan_freed);
               if (modem_mgmt_csm_debug)
                   buginf("\n%s delete the last CT1 channel at slot %d with "
                          "index %d\n", func_name, slot, ix);
           }
           errmsg(&msgsym(RBSLINEDOWN, DSX0), controller, timeslot+1);
           return;
       }
   } /* for */
}

/*
 * =====================================================================
 * find_modem_info_by_call_id()
 *
 * Description:
 * Find the modem_info pointer which has not been assigned a b channel number
 * yet in the waiting list with the call_id as a key.
 *
 * Parameters:
 * call_id - the call_id assigned when the outgoing call is generated.
 *
 * Returns:
 * modem_info - modem information descriptor pointer.
 * =====================================================================
 */
static modem_info_t* find_modem_info_by_call_id (ushort call_id)
{
    list_element *element;
    modem_info_t *modem_info;

    FOR_ALL_ELEMENTS_IN_LIST(&modem_pool->wait_for_bchan_header, element) {
        modem_info = LIST_GET_DATA(element);
        if (modem_info->call_id == call_id) 
            return (modem_info);
    }

    return (NULL);
}

/*
 * =====================================================================
 * csm_tty_platform_dependent_info_report()
 *
 * Description:
 * This function is called thru registry reg_invoke_csm_tty_platform_info_
 * report(). By passing the tty number (line), this routine will print out 
 * the platform dependent data which including --
 * 1. CSM state of the line
 * 2. CSM status of the line 
 * 3. modem slot and port number associated with the line 
 * 4. B channel number used by the line.
 *
 * Parameters:
 * ttynum - tty number
 *
 * Returns:
 * None
 * =====================================================================
 */
static void  csm_tty_platform_dependent_info_report (tt_soc *tty) 
{
    modem_info_t *modem_info;
    ushort        ix;
    ulong         mask, status;


    modem_info = modem_ttynum_to_modem_info(tty->ttynum);
    if (!modem_info) {
        return;
    }

    printf("\nmodem(slot/port)=%d/\%d, csm_state(0x%08x)=%s, bchan_num=%d\n", 
           CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info), 
           modem_info->csm_state, csm_get_csm_state(modem_info->csm_state), 
           modem_info->bchan_num);

    if (CSM_IS_UNLOCKED(modem_info))
        printf("csm_status(0): %s\n", "CSM_STATUS_UNLOCKED");
    else {
        printf("csm_status(0x%08x): ", modem_info->csm_status);
        status = modem_info->csm_status;
        for (ix=0, mask=1; status; mask<<=1, ix++) {
            if (mask & status) {
                printf("%s.", (ix < CSM_STATUS_MAX_LENGTH) 
                              ? (char*) csm_status_name[ix] : "UNKNOWN");
                status &= ~mask;
            }
        }
        printf("\n");
    }
}

/*
 * =====================================================================
 * csm_idb_platform_dependent_info_report()
 *
 * Description:
 * This function is called thru registry reg_invoke_csm_idb_platform_info_
 * report(). By passing the hardware idb number, the routine 
 * csm_tty_platform_dependent_info_report will be called if hardware idb is 
 * not a null pointer.
 *
 * Parameters:
 * hwidb - hardware idb pointer 
 *
 * Returns:
 * None
 * =====================================================================
 */
static void  csm_idb_platform_dependent_info_report (hwidbtype *hwidb) 
{

   if (!hwidb) { 
       return;    /* error */
   }

   csm_tty_platform_dependent_info_report(hwidb->idb_tty); 
}

/*
 * =====================================================================
 * modem_report()
 *
 * Description:
 * This function is called thru registry reg_invoke_modem_report().
 * Whenever there's an ISDN call event generated, the event will be
 * passed on to CSM thru this registry.
 *
 * Parameters:
 * dchan_idb - the hardware idb pointer for the D channel.
 * call_id   - the call_id assigned when the incoming/outgoing call is generated
 * ces       - is always set to 0.
 * bchan     - the B channel is assigned for this call.
 * event     - the event.
 * remote_num - the calling party's phone number if not NULL.
 * local_num - the called party's phone number if not NULL.
 * cause     - the ISDN event's cause.
 *
 * Returns:
 * Return the ISDN causes.
 * =====================================================================
 */
static ushort modem_report (hwidbtype *dchan_idb, ushort call_id, ushort ces,
                            short bchan, ushort event, char *remote_num, 
                            char *local_num, ushort cause)
{
   hwidbtype    *hwidb;
   hwidb_sb_t   sb_type;
   modem_info_t *modem_info;
   ushort        rc;
   leveltype     SR;
   static const char func_name[] = "MODEM_REPORT";
   isdn_bri_t *isdn_info;
   dialerdbtype *ddb = NULL;

   if (!dchan_idb)
       return (UNSPECIFIED_CAUSE);    /* error */

   SR = raise_interrupt_level(NETS_DISABLE);

   rc      = NO_CAUSE;      /* successfully processed */
   sb_type = HWIDB_SB_MODEM_INFO;

   if (modem_mgmt_csm_debug) {
       buginf("\n%s:dchan_idb=0x%x, call_id=0x%x, ces=0x%x" 
              "\n             bchan=0x%x, event=0x%x, cause=0x%x\n",
              func_name, dchan_idb, call_id, ces, bchan, event, cause);
   }

   switch (event) {
   case DEV_IDLE:          /* incoming and outgoing calls */
        if (bchan == INVALID_B_CHANNEL)
            modem_info = find_modem_info_by_call_id(call_id);
        else {
            hwidb = dchan_idb->isdn_info->b_chan_idb[bchan];
            modem_info = idb_get_hwsb(hwidb, sb_type);
        }

        if (modem_info) {
            if ((modem_info->call_id != call_id) 
                || (modem_info->dchan_idb != dchan_idb) 
                || (modem_info->bchan_num != bchan)) {
                /*
                 * Error:
                 */
                if (CSM_DEBUG_ENABLED(modem_info))
                    buginf("\nERROR: %s(%04x): %s: bad parameters from "
                           "ISDN at slot %d and port %d\n", func_name, call_id, 
                           "DEV_IDLE", CSM_MODEM_SLOT(modem_info), 
                           CSM_MODEM_PORT(modem_info));
                rc = INVALID_MODEM_INFO;
            }
            else if (CSM_DEBUG_ENABLED(modem_info))
                buginf("\n%s(%04x): %s at slot %d and port %d\n", 
                       func_name, call_id, "DEV_IDLE",
                       CSM_MODEM_SLOT(modem_info), 
                       CSM_MODEM_PORT(modem_info));
            modem_info->csm_event = CSM_EVENT_ISDN_DISCONNECTED;
            modem_info->cause = cause;
            (*modem_info->csm_event_proc)(modem_info); 
        }
        else {
            if (modem_mgmt_csm_debug)
                buginf("\n%s(%04x): %s:  %s due to %s\n", 
                       func_name, call_id, "DEV_IDLE", "modem_info is NULL", 
                       (bchan == INVALID_B_CHANNEL)? "call_id is invalid" : 
                       "wrong b channel number or b channel is deallocated");
            rc = NULL_MODEM_INFO;
        }
        break;

   case DEV_INCALL:                 /* incoming call */
        modem_info = csm_modem_allocate(modem_pool, CSM_FREE_MODEM);
        hwidb      = dchan_idb->isdn_info->b_chan_idb[bchan];

        if (modem_info && modem_pool->max_dchan_idb_index
            && (idb_get_hwsb(hwidb, sb_type) == NULL)
            && (dchan_idb->state == IDBS_UP)) {
            /*
             * D channel may not be up so soon
             */
            /*
             * Need to make sure that there's signaling channel available
             * in case where the signaling channel is shutdown after the isdn call
             * is received.
             * dchan_idb_index is not used for incoming call 
             */
            if (CSM_DEBUG_ENABLED(modem_info))
                buginf("\n%s(%04x): %s at slot %d and port %d\n", 
                       func_name, call_id, "DEV_INCALL", 
                       CSM_MODEM_SLOT(modem_info), 
                       CSM_MODEM_PORT(modem_info));   
            if (remote_num)
                sstrncpy(modem_info->calling_party_num, remote_num,
                             MAX_PHONE_NUM_LEN);
            else
                *modem_info->calling_party_num = 0;
            
            if (local_num)
                sstrncpy(modem_info->called_party_num, local_num,
                             MAX_PHONE_NUM_LEN);
            else
                *modem_info->called_party_num = 0;
            reg_invoke_csm_event_log(CSM_ELOG_PHONE_NUMBER_COLLECTED, 
                                     modem_info);
            
            hwidb->isdn_info->outgoing = FALSE;

            idb_add_hwsb(hwidb, &sb_type, modem_info);
            modem_info->csm_flags       |= CSM_FLAG_USE_PRI_LINE;
            modem_info->bchan_idb        = hwidb;
            modem_info->dchan_idb        = dchan_idb; 
            modem_info->call_id          = call_id;
            modem_info->bchan_num        = bchan;
            modem_info->csm_event        = CSM_EVENT_ISDN_CALL;
            modem_info->cause            = cause;
            (*modem_info->csm_event_proc)(modem_info);  
        }
        else {
	    /* If the user has specified which cause code they would
	     * like to be sent for a no modem available situation, use
	     * that code.  If nothing has been specified, use 
	     * REQ_CHANNEL_NOT_AVAIL.
	     */
	    if((rc = dchan_idb->isdn_info->modem_busy_cause) == NO_BUSY_CAUSE_SPECIFIED)
                rc = REQ_CHANNEL_NOT_AVAIL;
            
            if (modem_info) {
                csm_modem_deallocate(modem_info);
            }

            if (!hwidb) { 
                errmsg(&msgsym(HWIDB, CSM), func_name);
                if (modem_mgmt_csm_debug)
                    buginf("%s(%04x): %s: %s\n", func_name, call_id, 
                           "DEV_INCALL", "hwidb is NULL");
            }
            
            if (!modem_pool->max_dchan_idb_index && modem_mgmt_csm_debug)
                buginf("%s(%04x): %s: no signaling device is available\n",
                       func_name, call_id, "DEV_INCALL");
        }
        break;

   case DEV_CALL_PROC:           /* outgoing call */
        /*
         * Falling Through...
         */
   case DEV_SETUP_ACK:           /* outgoing call */
        if (bchan == INVALID_B_CHANNEL) {
            /*
             * Error:
             */
            if (modem_mgmt_csm_debug)
                buginf("\nERROR: %s(%04x): %s: receives %s\n", func_name,
                       call_id, (event==DEV_CALL_PROC) ? "DEV_CALL_PROC" 
                                                       : "DEV_SETUP_ACK",
                       "INVALID_B_CHANNEL");
            rc = UNFOUND_B_CHANNEL;
            break;
        }

        modem_info = find_modem_info_by_call_id(call_id);
        hwidb = dchan_idb->isdn_info->b_chan_idb[bchan];
        if (modem_info && (idb_get_hwsb(hwidb,sb_type)==NULL)) {
            if (dchan_idb->state == IDBS_UP) {
                /*
                 * D channel may not be up so soon
                 */
                if (modem_info->dchan_idb != dchan_idb) {
                    /*
                     * Error:
                     */
                    if (CSM_DEBUG_ENABLED(modem_info))
                        buginf("\nERROR: %s(%04x): %s: receives wrong "
                               "dchan_idb\n", func_name, call_id, 
                               (event==DEV_CALL_PROC) ? "DEV_CALL_PROC" 
                                                      : "DEV_SETUP_ACK");
                    rc = UNKNOWN_MODEM_ERROR;
                }
                else if (CSM_DEBUG_ENABLED(modem_info))
                    buginf("\n%s(%04x): %s at slot %d and port %d\n", func_name,
                           call_id, (event==DEV_CALL_PROC) ? "DEV_CALL_PROC" 
                                                           : "DEV_SETUP_ACK",
                           CSM_MODEM_SLOT(modem_info), 
                           CSM_MODEM_PORT(modem_info));
                
                hwidb->isdn_info->outgoing = TRUE;

                idb_add_hwsb(hwidb, &sb_type, modem_info);
                modem_info->bchan_idb   = hwidb;
                modem_info->bchan_num   = bchan;
                modem_info->csm_event   = CSM_EVENT_ISDN_BCHAN_ASSIGNED;
                modem_info->cause       = cause;
                (*modem_info->csm_event_proc)(modem_info);
            }
            else {
                if (CSM_DEBUG_ENABLED(modem_info))
                    buginf("\nERROR: %s(%04x): %s: %s state is not up\n", 
                           func_name, call_id, 
                           (event==DEV_CALL_PROC) ? "DEV_CALL_PROC" 
                                                  : "DEV_SETUP_ACK",
                           (dchan_idb->state != IDBS_UP) ? "dchan_idb" 
                                                         : "hwidb");
                modem_info->csm_event = CSM_EVENT_SHUTDOWN_INTERFACE;
                modem_info->cause     = CSM_CAUSE_SIGNAL_CHANNEL_NOT_UP;
                (*modem_info->csm_event_proc)(modem_info);
                rc = INVALID_MODEM_STATE;
            }
        }
        else {
            /*
             * Error:
             */
            if (modem_mgmt_csm_debug)
                buginf("\nERROR: %s(%04x): %s: modem_info or %s\n", 
                       func_name, call_id, 
                       (event==DEV_CALL_PROC) ? "DEV_CALL_PROC"
                                              : "DEV_SETUP_ACK",
                       "hwidb is NULL");
            if (modem_info) {
                modem_info->csm_event = CSM_EVENT_SHUTDOWN_INTERFACE;
                modem_info->cause     = CSM_CAUSE_NO_DATA_CHANNEL;
                (*modem_info->csm_event_proc)(modem_info);
            }
            rc = UNDEFINED_B_IDB;
        }
        break;

   case DEV_CONNECTED:             /* incoming and outgoing calls */
        if (bchan != INVALID_B_CHANNEL) {
            hwidb = dchan_idb->isdn_info->b_chan_idb[bchan];
            if (hwidb) {
                modem_info = idb_get_hwsb_inline(hwidb, sb_type);
                if (modem_info &&
                    (dchan_idb->state == IDBS_UP)) {     
                    /*
                     * D channel may not be up so soon
                     */
                    if ((modem_info->call_id != call_id) 
                        || (modem_info->dchan_idb != dchan_idb)
                        || (modem_info->bchan_num != bchan)) {
                        /*
                         * Error:
                         */
                        if (CSM_DEBUG_ENABLED(modem_info))
                            buginf("\nERROR: %s(%04x): %s: bad parameters from "
                                   "ISDN at slot %d and port %d\n", 
                                   func_name, call_id, "DEV_CONNECTED",
                                   CSM_MODEM_SLOT(modem_info), 
                                   CSM_MODEM_PORT(modem_info));
                        rc = INVALID_MODEM_INFO;
                    }
                    else if (CSM_DEBUG_ENABLED(modem_info))
                        buginf("\n%s(%04x): %s at slot %d and port %d\n", 
                               func_name, call_id, "DEV_CONNECTED", 
                               CSM_MODEM_SLOT(modem_info), 
                               CSM_MODEM_PORT(modem_info));
                    
                    modem_info->csm_event = CSM_EVENT_ISDN_CONNECTED;
                    modem_info->cause = cause;

                    isdn_info = hwidb->isdn_info;
		    if(modem_info->tty_hwidb != NULL) {
                       isdn_info->tx_packets = idb_outputs(modem_info->tty_hwidb);
                       isdn_info->tx_bytes = idb_tx_cumbytes(modem_info->tty_hwidb);
                       isdn_info->rx_packets = idb_inputs(modem_info->tty_hwidb);
                       isdn_info->rx_bytes = idb_rx_cumbytes(modem_info->tty_hwidb);
		    }
                    isdn_info->call_connected = TRUE;
                    GET_TIMESTAMP(isdn_info->connect_time);
                    current_time_string(isdn_info->connect_tod);
                    isdn_info->async_idb = modem_info->tty_hwidb;
                    if(isdn_info->outgoing){                
                        if(modem_info->tty_hwidb != NULL)
                           ddb = get_dialerdb(modem_info->tty_hwidb);
                        if (isdn_info->remote_number)
                            free(isdn_info->remote_number);
                        if(ddb != NULL)
                           isdn_info->remote_number = strdup(ddb->number_called)
;
                        else
                           isdn_info->remote_number = strdup(modem_info->called_party_num);
                        isdn_info->remote_src = ISDN_SRC_DIALER;
                    }
                    reg_invoke_isdn_history_update(hwidb);

                    (*modem_info->csm_event_proc)(modem_info); 
                }
                else if (modem_info) {
                    if (CSM_DEBUG_ENABLED(modem_info))
                        buginf("\nERROR: %s(%04x): %s: %s state is not up\n",
                               func_name, call_id, "DEV_CONNECTED",
                               (dchan_idb->state != IDBS_UP) ? "dchan_idb" 
                                                             : "hwidb");
                    modem_info->csm_event = CSM_EVENT_SHUTDOWN_INTERFACE;
                    modem_info->cause     = CSM_CAUSE_SIGNAL_CHANNEL_NOT_UP;
                    (*modem_info->csm_event_proc)(modem_info);
                    rc = INVALID_MODEM_STATE;
                }
                else {
                    if (modem_mgmt_csm_debug)
                        buginf("%s(%04x): %s: %s\n",
                               func_name, call_id, DEV_CONNECTED,
                               "modem_info is NULL");
                    rc = NULL_MODEM_INFO;
                }
            }
            else {
                /*
                 * Error:
                 */
                if (modem_mgmt_csm_debug)
                    buginf("\nERROR: %s(%04x): %s: %s at channel"
                           " %d\n", func_name, call_id, "DEV_CONNECTED", 
                           "hwidb is NULL", bchan);
                rc = UNDEFINED_B_IDB;
            }
        }
        else {
            /*
             * invalid event
             */
            if (modem_mgmt_csm_debug)
                buginf("\nERROR: %s(%04x): %s: receives %s\n", 
                        func_name, call_id, "DEV_CONNECTED", 
                       "INVALID_B_CHANNEL");
            rc = UNFOUND_B_CHANNEL;
        }

        break;
        
   default:
        if (modem_mgmt_csm_debug)
            buginf("\nERROR: %s(%04x): unknown event 0x%x from ISDN\n", 
                   func_name, call_id, event);
        rc = UNKNOWN_MODEM_ERROR;
        break;
   }

   reset_interrupt_level(SR);

   return (rc);       
}

/****************************************************************************
 * CSM ISR:
 *    This is a level 4 interrupt generated by modem on-hook/off-hook line
 *    or neat processor.
 *
 */
/*
 * =====================================================================
 * csm_modem_interrupt()
 *
 * Description:
 * The modem interrupt service routine for the modem's on-hook/off-hook 
 * interrupts.
 *
 * Parameters:
 * slot - the modem card slot number which generates the interrupts.
 *
 * Returns:
 * None.
 * =====================================================================
 */
void csm_modem_interrupt (int slot)
{
   modem_info_t  **modem_info_p, *modem_info;   
   ushort          mask, oh_int_status, oh_state;
   ushort          ix, total_bitmap_modem_num, *modem_mask;
   modem_reg_t    *modem_reg;
   modem_slot_t   *modem_slot;
#if CSM_EVENT_QUEUE_ENABLE
   csm_event_queue_t      *csm_event_queue;
#endif

   /*
    * We need to make sure that when reading the modem's reserved 
    * register area, the value should be read as 0.
    */
   modem_slot             = modem_pool->modem_slot[slot];
   modem_info_p           = modem_slot->modem_info;
   modem_reg              = modem_slot->modem_reg; 
   modem_mask             = modem_slot->modem_exist_mask;
   total_bitmap_modem_num = modem_slot->total_bitmap_modem_num;

   for (ix=0; ix < total_bitmap_modem_num; 
        ix++, modem_info_p=&modem_slot->modem_info[ix*MODEM_BITMAP_SIZE], 
        modem_mask++) {

      if (!(*modem_mask & modem_reg->oh_int_enable[ix]))
         continue;       /* no modem exists or interrupts are disabled */

      oh_int_status = modem_reg->oh_int_status[ix];
      oh_state      = modem_reg->oh_state[ix];
      modem_reg->oh_int_status[ix] = oh_int_status;     /* clear interrupt */
      oh_int_status &= *modem_mask;

      if (!oh_int_status)
         continue;      /* no interrupts */ 

      /*
       * Process the interrupt
       */
      mask = 1;
      do {
         /*
          * Search for the interrupt bit
          */
         while (!(oh_int_status & mask)) {
            mask<<=1;
            modem_info_p++;
         }

         oh_int_status    &= ~mask;
         modem_info        = *modem_info_p;
#if CSM_EVENT_QUEUE_ENABLE
         csm_event_queue = write_next_csm_event_inline();
         csm_event_queue->modem_info = modem_info;
         csm_event_queue->csm_event  = (oh_state & mask)
                                       ? CSM_EVENT_MODEM_ONHOOK
                                       : CSM_EVENT_MODEM_OFFHOOK;
#else
         modem_info->csm_event = (oh_state & mask)
                                ? CSM_EVENT_MODEM_ONHOOK
                                : CSM_EVENT_MODEM_OFFHOOK;
         (*modem_info->csm_event_proc)(modem_info);
#endif

         mask<<=1;
         modem_info_p++;
      } while (oh_int_status && mask); /* do while */
   } /* for ix */
}

/*
 * =====================================================================
 * dsx1_mail_from_neat()
 *
 * Description:
 * The digit collection events coming from NEAT processor will be passed
 * on to CSM thru this function. This function is called in the interrupt
 * level 4 context.
 *
 * Parameters:
 * event - the digit collection events.
 * mid   - the message id associated with this event. the new mid is generated 
 *         for every digit collection request.
 * slot  - the PRI card slot number where the digit collector resides.
 * unit  - the unit id number for the digit collector in the pri card.
 *
 * Returns:
 * None.
 * =====================================================================
 */
void dsx1_mail_from_neat (ushort event, uchar mid, uchar slot, uchar unit)
{
    /*
     * This function is called within level 4 interrupt context from neat
     */
   modem_info_t    *modem_info;
   digit_collect_t *dc;
   csm_event_t     csm_event;
   static const char func_name[] = "T1_MAIL_FROM_NEAT:";
#if CSM_EVENT_QUEUE_ENABLE
    leveltype          SR;
   csm_event_queue_t  *csm_event_queue;
#endif
    
   dc = &modem_pool->digit_collect_pool[slot][unit];
   if ((dc->message_id != mid) || (dc->collecting_digit == DC_STATE_IDLE))
      return;     /* wrong/old message id received, discard the message */

   modem_info = dc->modem_info;

   switch (event) {
   case DC_READY_RSP:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s: mid = %d, slot = %d, unit = %d\n",
                   func_name, "DC_READY_RSP", mid, slot, unit);
        dc->collecting_digit = DC_STATE_READY_RSP_RECEIVED;
        if (!modem_info) {
            /*
             * a delayed digit collection is previously posted
             * search for the next pending digit collection
             */
            csm_process_next_digit_collect(dc);
            return;
        }
        csm_event = CSM_EVENT_DIGIT_COLLECT_READY;
        break;

   case DC_FIRST_DIGIT_RSP:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s: mid = %d, slot = %d, unit = %d\n",
                   func_name, "DC_FIRST_DIGIT_RSP", mid, slot, unit);
        csm_event = CSM_EVENT_GET_1ST_DIGIT;
        break;

   case DC_ALL_DIGIT_RSP:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s: mid = %d, slot = %d, unit = %d\n",
                   func_name, "DC_ALL_DIGIT_RSP", mid, slot, unit);
        csm_event = CSM_EVENT_GET_ALL_DIGITS;
        break;

   case DC_ABORT_RSP:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s: mid = %d, slot = %d, unit = %d\n",
                   func_name, "DC_ABORT_RSP", mid, slot, unit);
        csm_event = CSM_EVENT_ABORT_DIGIT_COLLECT;
        break;

   default:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s unknown event: mid = %d, slot = %d, unit = %d\n",
                   func_name, mid, slot, unit);
        modem_info->invalid_event_count++;
        return;   /* wrong event message */
   }

#if CSM_EVENT_QUEUE_ENABLE
   SR = raise_interrupt_level(NETS_DISABLE);
   csm_event_queue             = write_next_csm_event_inline();
   reset_interrupt_level(SR);
   csm_event_queue->modem_info = modem_info;
   csm_event_queue->csm_event  = csm_event;
#else
   modem_info->csm_event   = csm_event;
   (*modem_info->csm_event_proc)(modem_info);        
#endif
}

/*
 * =====================================================================
 * csm_rx_ct1_event_from_neat()
 *
 * Description:
 * This routine is called whenever there's an event associated with the 
 * CT1 Robbed bit signaling running in the NEAT processor. This routine 
 * take advantage of using new IPC created between NEAT and main processor.
 * It's called in the interrupt level 4 context.
 *
 * Parameters:
 * msg - the message which contains the source and destination entities,
 *       call_id, event, and cause.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_rx_ct1_event_from_neat (uchar slot, void *msg)
{
   modem_info_t *modem_info;
   ushort        call_id, cause;
   leveltype     SR;
   uchar         port;
   short         bchan;
   csm_message_t send_msg, *csm_message;
   static const char func_name[] = "CSM_RX_CT1_EVENT_FROM_NEAT:";
   csm_message = (csm_message_t*) msg;
   csm_message->src_slot = slot;
   slot    = csm_message->dest_slot;
   port    = csm_message->dest_entity;
   call_id = csm_message->data.call_id;
   bchan   = csm_message->src_entity;
   cause   = csm_message->cause;

   SR = raise_interrupt_level(NETS_DISABLE);

   switch (csm_message->event) {
   case EVENT_ENABLE_CHANNEL:
        csm_add_ct1_signaling_interface(csm_message->src_slot,
                                        csm_message->src_controller,
                                        csm_message->src_entity,
                                        cause);
        break;

   case EVENT_DISABLE_CHANNEL:
        csm_delete_ct1_signaling_interface(csm_message->src_slot,
                                           csm_message->src_controller,
                                           csm_message->src_entity,
                                           cause);
        /*
         * Falling through to disconnect the call if any ...
         */

   case EVENT_CALL_IDLE:     /* incoming and outgoing calls */
        if (port == UNKNOWN_B_CHANNEL)
            modem_info = find_modem_info_by_call_id(call_id);
        else 
            modem_info = CSM_MODEM_INFO_OR_NULL(slot, port);     

        if (modem_info &&
            (modem_info->call_id == call_id) 
            && (modem_info->bchan_num == bchan)) {
            if (CSM_DEBUG_ENABLED(modem_info))
                buginf("\n%s(%04x):  %s at slot %d and port %d\n",
                       func_name, call_id, "EVENT_CALL_IDLE", slot, port);
            modem_info->csm_event = CSM_EVENT_DSX0_DISCONNECTED;
            modem_info->cause = cause;
                (*modem_info->csm_event_proc)(modem_info); 
        }
        break;

   case EVENT_CALL_DIAL_IN:             /* incoming call */
        modem_info = csm_modem_allocate(modem_pool, CSM_FREE_MODEM);
        send_msg.dest_slot      = csm_message->src_slot;
        send_msg.dest_controller= csm_message->src_controller;
        send_msg.dest_entity    = csm_message->src_entity;
        send_msg.data.call_id   = call_id;

        if (modem_info) {
            send_msg.src_slot       = slot = CSM_MODEM_SLOT(modem_info);
            send_msg.src_controller = 0;
            send_msg.src_entity     = port = CSM_MODEM_PORT(modem_info);
            send_msg.event          = EVENT_CHANNEL_LOCK;
            send_msg.cause          = NO_CAUSE;

            if (CSM_DEBUG_ENABLED(modem_info))
                buginf("\n%s(%04x): %s at slot %d and port %d\n", 
                       func_name, call_id, "EVENT_CALL_DIAL_IN", slot, port);
            *modem_info->calling_party_num = 0;
            *modem_info->called_party_num = 0;
            reg_invoke_csm_event_log(CSM_ELOG_PHONE_NUMBER_COLLECTED, 
                                     modem_info);
            modem_info->csm_flags       &= ~CSM_FLAG_USE_PRI_LINE;
            modem_info->bchan_idb        = NULL;
            modem_info->dchan_idb        = NULL; 
            modem_info->call_id          = call_id;
            modem_info->dsx0_slot        = csm_message->src_slot;
            modem_info->dsx0_controller  = csm_message->src_controller;
            modem_info->bchan_num        = bchan;
            modem_info->csm_event        = CSM_EVENT_DSX0_CALL;
            modem_info->cause            = cause;
            (*modem_info->csm_event_proc)(modem_info);  
        }
        else {
            send_msg.src_slot       = UNKNOWN_B_CHANNEL;
            send_msg.src_controller = UNKNOWN_B_CHANNEL;
            send_msg.src_entity     = UNKNOWN_B_CHANNEL;

            send_msg.event          = EVENT_CALL_IDLE;
            send_msg.cause          = USER_BUSY;

            if (modem_mgmt_csm_debug)
                buginf("\nERROR: %s(%04x): %s: no modem is available.\n",
                       func_name, call_id, "EVENT_CALL_DIAL_IN");
        }

        {
            neat_ipc_msg_ret rc;
 
            rc = neat_ipc_message_send(send_msg.dest_slot,
                                       MSG_MPU_CSM2RBS,
                                       (uchar*) &send_msg,
                                       sizeof(send_msg));
            if ((rc != IPCMSG_ERROR_NOERROR) && modem_mgmt_csm_debug)
                buginf("ERROR: %s: %s: failed rc = 0x%x", func_name, 
                       "SEND_NEAT_MESSAGE", rc);
        }
        break;

   case EVENT_CHANNEL_LOCK:           /* outgoing call */
        if (bchan == UNKNOWN_B_CHANNEL) {
            /*
             * Error:
             */
            if (modem_mgmt_csm_debug)
                buginf("\nERROR: %s(%04x): %s: receives %s\n",
                       func_name, call_id, "EVENT_CHANNEL_LOCK", 
                       "INVALID_B_CHANNEL");
            break;
        }

        if (port == UNKNOWN_B_CHANNEL)
            /*
             * not supposed to rx this UNKNOWN_B_CHANNEL, 
             * however, just wanna be on the safe side.
             */
            modem_info = find_modem_info_by_call_id(call_id);
        else
            modem_info = CSM_MODEM_INFO_OR_NULL(slot, port);
        if (modem_info) {
            if (modem_info->dsx0_slot != csm_message->src_slot) {
                /*
                 * Error:
                 */
                if (CSM_DEBUG_ENABLED(modem_info))
                    buginf("\nERROR: %s(%04x): %s: receives from a wrong "
                           "slot\n", func_name, call_id, "EVENT_CHANNEL_LOCK");
                break;
            }
            else if (CSM_DEBUG_ENABLED(modem_info))
                buginf("\n%s(%04x): %s at slot %d and port %d\n",
                       func_name, call_id, "EVENT_CHANNEL_LOCK", slot, port);
            modem_info->dsx0_controller  = csm_message->src_controller;
            modem_info->bchan_num        = bchan; 
            modem_info->csm_event        = CSM_EVENT_DSX0_BCHAN_ASSIGNED;
            modem_info->cause            = cause;
            (*modem_info->csm_event_proc)(modem_info);
        }
        else {
            /*
             * Error:
             */
            errmsg(&msgsym(MODEM_INFO, CSM), func_name);
            if (modem_mgmt_csm_debug)
                buginf("\nERROR: %s(%04x): %s: %s\n",
                       func_name, call_id, "EVENT_CHANNEL_LOCK",
                       "modem_info is NULL");
        }
        break;

   case EVENT_CHANNEL_CONNECTED:             /* incoming and outgoing calls */
        modem_info = CSM_MODEM_INFO_OR_NULL(slot, port);
        if (bchan != UNKNOWN_B_CHANNEL) {
            if (modem_info) {
                if ((modem_info->call_id != call_id) 
                    || (modem_info->dsx0_slot != csm_message->src_slot)
                    || (modem_info->bchan_num != bchan)) {
                    /*
                     * Error:
                     */
                    if (CSM_DEBUG_ENABLED(modem_info))
                        buginf("\nERROR: %s(%04x): %s: bad parameters from "
                               "NEAT at slot %d and port %d\n", func_name, 
                               call_id, "EVENT_CHANNEL_CONNECTED", slot, port);
                    break;
                }
                else if (CSM_DEBUG_ENABLED(modem_info))
                    buginf("\n%s(%04x): %s at slot %d and port %d\n", 
                           func_name, call_id, "EVENT_CHANNEL_CONNECTED", 
                           slot, port);
                modem_info->csm_event = CSM_EVENT_DSX0_CONNECTED;
                modem_info->cause = cause;
                (*modem_info->csm_event_proc)(modem_info); 
            }
            else {
                /*
                 * Error:
                 */
                errmsg(&msgsym(MODEM_INFO, CSM), func_name);
                if (modem_mgmt_csm_debug)
                    buginf("\nERROR: %s(%04x): %s: %s at "
                           "channel %d\n", func_name, call_id, 
                           "EVENT_CHANNEL_CONNECTED", "modem_info is NULL",
                           bchan);
            }
        }
        else {
            /*
             * invalid event
             */
            if (modem_mgmt_csm_debug)
                buginf("\nERROR: %s(%04x): %s: receives %s\n", 
                       func_name, call_id, "EVENT_CHANNEL_CONNECTED", 
                       "UNKNOWN_B_CHANNEL");
        }
        
        break;

   case EVENT_START_RX_TONE:
        /*
         * Currently, the cut thru is done after the modem goes off-hook.
         * Therefore, this message is ignored for the current design.
         * Otherwise, this case will fall thru to the next case 
         * EVENT_START_TX_TONE.
         */
        break;

   case EVENT_START_TX_TONE:
        modem_info = CSM_MODEM_INFO_OR_NULL(slot, port);
        if (modem_info && (modem_info->call_id == call_id)) {
            modem_info->csm_event = (csm_message->event == EVENT_START_TX_TONE)
                ? CSM_EVENT_DSX0_START_TX_TONE : CSM_EVENT_DSX0_START_RX_TONE;
            modem_info->cause = cause;
            (*modem_info->csm_event_proc)(modem_info);
        }
        break;
        
   default:
        if (modem_mgmt_csm_debug)
            buginf("\nERROR: %s(%04x): unknown event 0x%x from NEAT\n", 
                   func_name, call_id, csm_message->event);
        break;
   }

   reset_interrupt_level(SR);
}


/****************************************************************************
 * CSM State Machine Supporting Functions:
 *    The network interrupt should be disable prior to calling the
 *    following functions.
 *
 */
/*
 * =====================================================================
 * csm_disable_oh_interrupt()
 *
 * Description:
 * This support routine will disable the modem on-hook/off-hook interrupts.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_disable_oh_interrupt (modem_info_t *modem_info)
{
    modem_reg_t *modem_reg;
    ushort port;
 
    modem_reg = modem_pool->modem_slot[CSM_MODEM_SLOT(modem_info)]->modem_reg;
    port = modem_info->modem_port_offset;
    modem_reg->oh_int_enable[port] &= ~modem_info->modem_mask; 
                                       /* disable oh interrupt */
}

/*
 * =====================================================================
 * csm_enable_oh_interrupt()
 *
 * Description:
 * This support routine will enable the modem on-hook/off-hook interrupts.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_enable_oh_interrupt (modem_info_t *modem_info)
{
    modem_reg_t *modem_reg;
    ushort port;
 
    modem_reg = modem_pool->modem_slot[CSM_MODEM_SLOT(modem_info)]->modem_reg;
    port = modem_info->modem_port_offset;
    modem_reg->oh_int_status[port] |= modem_info->modem_mask; 
                                     /* clear pending interrupt */
    modem_reg->oh_int_enable[port] |= modem_info->modem_mask; 
                                     /* enable oh interrupt */
}

/*
 * =====================================================================
 * csm_stop_modem_reset()
 *
 * Description:
 * This support routine will bring modem out of reset mode.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_stop_modem_reset (modem_info_t *modem_info)
{
    modem_reg_t *modem_reg;
    ushort port;
 
    modem_reg = modem_pool->modem_slot[CSM_MODEM_SLOT(modem_info)]->modem_reg;
    port = modem_info->modem_port_offset;
    modem_reg->modem_reset[port] |= modem_info->modem_mask; 
                                   /* disable oh interrupt */
}

/*
 * =====================================================================
 * csm_start_modem_reset()
 *
 * Description:
 * This support routine will put modem in the reset mode.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_start_modem_reset (modem_info_t *modem_info)
{
    modem_reg_t *modem_reg;
    ushort port;
 
    modem_reg = modem_pool->modem_slot[CSM_MODEM_SLOT(modem_info)]->modem_reg;
    port = modem_info->modem_port_offset;
    modem_reg->modem_reset[port] &= ~modem_info->modem_mask; 
                                    /* enable oh interrupt */
    modem_info->stat_modem_reset++;
}

/*
 * =====================================================================
 * csm_turn_ring_off()
 *
 * Description:
 * This support routine will stop the ring indication to the 
 * specified modem. This can be done in two ways. One is thru the RI
 * pin provided by modem card. The other one is thru the AT command
 * 'atd' to make modem go off-hook.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_turn_ring_off (modem_info_t *modem_info)
{
    modem_slot_t *modem_slot;
    
    modem_slot = modem_pool->modem_slot[CSM_MODEM_SLOT(modem_info)];
    if (CSM_USE_AT_CMD_ANSWER(modem_info)) {
        if (LIST_ELEMENT_QUEUED(&modem_info->atcmd_answer_link))
            /*
             * in case where we need to turn off the ring when modem does
             * not go off-hook.
             */
            list_remove(&modem_pool->atcmd_answer_header,
                        &modem_info->atcmd_answer_link,
                        modem_info);
    }
    else if (modem_slot->ring_map_ignored[modem_info->modem_port_offset] &
	     modem_info->modem_mask) {
        modem_slot->ring_map_ignored[modem_info->modem_port_offset] 
            &= ~modem_info->modem_mask;
        modem_slot->modem_reg->ring_indicator[modem_info->modem_port_offset]
            |= modem_info->modem_mask;        
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\nCSM: Fast Ringing Off at modem slot %d, port %d\n", 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
    }
    else {
        modem_pool->ringer_num--;
        modem_slot->ring_map[modem_info->modem_port_offset] 
            &= ~modem_info->modem_mask;
        modem_slot->modem_reg->ring_indicator[modem_info->modem_port_offset]
            |= modem_info->modem_mask;
    }
}

/*
 * =====================================================================
 * csm_turn_ring_on()
 *
 * Description:
 * This support routine will start the ring indication to the
 * specified modem. This can be done in two ways. One is thru the RI
 * pin provided by modem card. The other one is thru the AT command
 * 'atd' to make modem go off-hook.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_turn_ring_on (modem_info_t *modem_info)
{
    modem_slot_t *modem_slot;

    modem_slot = modem_pool->modem_slot[CSM_MODEM_SLOT(modem_info)];
    if (CSM_USE_AT_CMD_ANSWER(modem_info)) {
        if (LIST_ELEMENT_QUEUED(&modem_info->atcmd_answer_link) && 
            CSM_DEBUG_ENABLED(modem_info))
            buginf("ERROR: %s: previous ringing is not terminated "
                   "at slot %d, port %d\n", "CSM_TURN_RING_ON",
                   CSM_MODEM_SLOT(modem_info),
                   CSM_MODEM_PORT(modem_info));
        else
            list_enqueue(&modem_pool->atcmd_answer_header, 
                         &modem_info->atcmd_answer_link,
                         modem_info);
    }
    else if (CSM_IS_FAST_RING(modem_info)) {
        modem_slot->ring_map_ignored[modem_info->modem_port_offset] 
            |= modem_info->modem_mask;
        modem_slot->modem_reg->ring_indicator[modem_info->modem_port_offset]
            &= ~modem_info->modem_mask;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\nCSM: Fast Ringing On at modem slot %d, port %d\n", 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
    }
    else {
        modem_slot->ring_map[modem_info->modem_port_offset] 
            |= modem_info->modem_mask;
        modem_pool->ringer_num++;
    }
}

/*
 * =====================================================================
 * csm_process_next_digit_collect()
 *
 * Description:
 * This function will free the currently finished digit collection resource
 * and then service the next pending digit collection request in the queue.
 *
 * Parameters:
 * dc - digit collection descriptor pointer which already finishes the
 *      collection.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_process_next_digit_collect (digit_collect_t *dc)
{
    modem_info_t    *next_modem_info;

    if (LIST_EMPTY(&modem_pool->wait_for_dialing_header)) {
        if (dc->collecting_digit != DC_STATE_IDLE) {
            dc->modem_info = NULL;
            if (dc->collecting_digit != DC_STATE_READY_RSP_PENDING) {
                modem_pool->free_digit_collector_num++;
                dc->collecting_digit = DC_STATE_IDLE;
            }
        }
        return;
    }
    else if (dc->collecting_digit == DC_STATE_READY_RSP_PENDING) {
        /* 
         * delay the digit collection until the ready response is received
         * NEAT should guarantee that it'll return the response, otherwise
         * this dc will pend forever. 
         * The reason for pending this dc is because NEAT cannot handle 
         * contiguous messages sent from MPU because of lacking of share
         * memory lock machanism. Therefore, CSM cannot use DC_RESET_REQ 
         * message to restart the next digit collection without waiting 
         * for the DC_READY_RSP message which is a much cleaner and easier 
         * implementation than this pending one.
         */
        dc->modem_info = NULL;
        return;   
    }
    
    next_modem_info = list_dequeue(&modem_pool->wait_for_dialing_header);
    
    tdm_connect_dc(&next_modem_info->modem_chnl, &dc->dc_chnl);
    dc->message_id++;
    dc->modem_info      = next_modem_info;
    next_modem_info->dc = dc;
    t1_mail_to_neat(DC_RESET_REQ, dc->message_id, dc->dc_chnl.slot, 
                    dc->dc_chnl.unit, next_modem_info->called_party_num);
}

/*
 * =====================================================================
 * csm_process_next_pending_digit_collect()
 *
 * Description:
 * This function will disconnect the digit collection TDM stream and 
 * free the currently finished digit collection resource and then service 
 * the next pending digit collection request in the queue.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * park_tx_rx - disconnect both modem's tx and rx lines if TRUE.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_process_next_pending_digit_collect (modem_info_t *modem_info, 
                                                    boolean park_tx_rx)
{
   digit_collect_t *dc;

   dc = modem_info->dc;

   if (!dc) {
       list_remove(&modem_pool->wait_for_dialing_header,
                   &modem_info->waiting_dial_link,
                   modem_info);
       return;
   }

   /*
    * Disconnect the Digit Collect Time Slot from the modem's tx/rx  line
    * Disconnect modem
    */
   tdm_disconnect_dc(&modem_info->modem_chnl, &dc->dc_chnl);
   if (park_tx_rx) {
       modem_info->modem_chnl.stream_type = TDM_TONE_STREAM;
       tdm_connect_special(DEFAULT_RX, &modem_info->modem_chnl);
       modem_info->modem_chnl.stream_type = TDM_MODEM_STREAM;
   }
       
   modem_info->dc = NULL;
   csm_process_next_digit_collect(dc);
}

/*
 * =====================================================================
 * csm_get_signaling_channel()
 *
 * Description:
 * This routine check to see if the current free signaling device
 * can be used for make the next outgoing call. The signaling device 
 * can be either a PRI D channel or a CT1 robbed signaling channel.
 *
 * Parameters:
 * modem_info      - modem information descriptor pointer.
 * search_pri_line - search for PRI D channel if TRUE.
 * dchan_index     - the current signaling device's index pointer
 * next_index      - the next signaling device's index pointer
 *
 * Returns:
 * Return the signaling device info pointer if found.
 * Return NULL if no more signaling channel is available.
 * =====================================================================
 */
static dchan_info_t* csm_get_signaling_channel (modem_info_t *modem_info,
                                                boolean search_pri_line,
                                                ushort *dchan_index,
                                                ushort *next_index)

{
    dchan_info_t *dchan_info;

    *dchan_index = *next_index;
    *next_index  = (*next_index+1) % modem_pool->max_dchan_idb_index;
    dchan_info   = modem_pool->dchan_info[*dchan_index];
    if ((dchan_info->dchan_idb && search_pri_line) ||
        !(dchan_info->dchan_idb || search_pri_line)) {
        modem_info->dchan_idb_index = *dchan_index;
        modem_info->dchan_idb       = dchan_info->dchan_idb;
        modem_info->call_id         
            = call_id_for_modem(&dchan_info->current_oc_call_id);
        modem_pool->current_dchan_idb_index = *next_index;
        return (dchan_info);
    }
    return (NULL);
}

/*
 * =====================================================================
 * csm_get_1st_signaling_channel()
 *
 * Description:
 * This routine search for the next first free signaling device which
 * can be used for make the next outgoing call. The signaling device can be
 * either a PRI D channel or a CT1 robbed signaling channel.
 *
 * Parameters:
 * modem_info      - modem information descriptor pointer.
 * search_pri_line - search for PRI D channel if TRUE.
 *
 * Returns:
 * Return the signaling device info pointer if found.
 * Return NULL if no more signaling channel is available.
 * =====================================================================
 */
static dchan_info_t* csm_get_1st_signaling_channel (modem_info_t *modem_info, 
                                                    boolean search_pri_line)
{
    dchan_info_t *dchan_info;
    ushort        dchan_index, next_index;

    if (!modem_pool->max_dchan_idb_index)
        return (NULL);

    next_index = modem_pool->current_dchan_idb_index;
    do {
        dchan_info = csm_get_signaling_channel(modem_info, search_pri_line, 
                                               &dchan_index, &next_index);
        if (dchan_info) {
            modem_info->dchan_idb_start_index = modem_info->dchan_idb_index;
            return (dchan_info);
        }
    } while (next_index != modem_pool->current_dchan_idb_index);
    return (NULL);
}

/*
 * =====================================================================
 * csm_get_next_signaling_channel()
 *
 * Description:
 * After the first signaling channel allocated for an outgoing call
 * fails for dialing out, this function will search for the next
 * free signaling channel to continue the dialing.
 *
 * Parameters:
 * modem_info      - modem information descriptor pointer.
 * search_pri_line - search for PRI D channel if TRUE.
 *
 * Returns:
 * Return the signaling device info pointer if found.
 * return NULL if no more signaling channel is available.
 * =====================================================================
 */
static dchan_info_t* csm_get_next_signaling_channel (modem_info_t *modem_info,
                                                     boolean search_pri_line)
{
    dchan_info_t *dchan_info;
    ushort        dchan_index, next_index;

    if (!modem_pool->max_dchan_idb_index)
        return (NULL);

    next_index = (modem_info->dchan_idb_index+1) 
                 % modem_pool->max_dchan_idb_index;
    while (next_index != modem_info->dchan_idb_start_index) {
        dchan_info = csm_get_signaling_channel(modem_info, search_pri_line,
                                               &dchan_index, &next_index);
        if (dchan_info)
            return (dchan_info);
    } 
    return (NULL);
}

/*
 * =====================================================================
 * csm_start_digit_collection()
 *
 * Description:
 * This routine will search a free digit collector for an outgoing call
 * and then send the digit collection request to NEAT processor. If there's 
 * none available at the moment, the request will be queued until one of the 
 * digit collector becomes free and the request will be served.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_start_digit_collection (modem_info_t *modem_info)
{
   digit_collect_t *dc;
   ushort           ix;

   if (modem_pool->free_digit_collector_num) {
       for (ix=0; ix < modem_pool->max_digit_collect_index; ix++) {
           dc = modem_pool->digit_collect_list[modem_pool->current_digit_collect_index++];
           modem_pool->current_digit_collect_index %= modem_pool->max_digit_collect_index;
           if (dc->collecting_digit == DC_STATE_IDLE) {
               /*
                * A free digit collector is found.
                * Connect the Digit Collect Time Slot to the modem's tx line
                */
               tdm_connect_dc(&modem_info->modem_chnl, &dc->dc_chnl);
               dc->message_id++;
               dc->collecting_digit = DC_STATE_READY_RSP_PENDING;
               dc->modem_info       = modem_info;
               modem_info->dc       = dc;
               modem_pool->free_digit_collector_num--;
               
               t1_mail_to_neat(DC_RESET_REQ, dc->message_id,
                               dc->dc_chnl.slot, dc->dc_chnl.unit,
                               modem_info->called_party_num);
               break;
           }
       }
   }
   else {
       list_enqueue(&modem_pool->wait_for_dialing_header,
                    &modem_info->waiting_dial_link,
                    modem_info);
   }
   
   modem_info->csm_flags     |= CSM_FLAG_USE_PRI_LINE;
   modem_info->csm_state      = CSM_OC1_REQUEST_DIGIT;
   modem_info->csm_event_proc = csm_proc_oc1_request_digit;
}

/*
 * =====================================================================
 * csm_connect_pri_modem()
 *
 * Description:
 * This function will call connect the modem's tx and rx lines to the
 * T1/PRI B channel's rx and tx lines thru the TDM backplane.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static int csm_connect_pri_modem (modem_info_t *modem_info)
{
   boolean rc;
   modem_info_t *modem_info_saved;

   rc = RC_SUCCESS;

   if (modem_info->bchan_idb)
       modem_info_saved = idb_get_hwsb_inline(modem_info->bchan_idb, 
                                              HWIDB_SB_MODEM_INFO);
   else
       modem_info_saved = NULL;

   if (modem_info && (!CSM_USE_PRI_LINE(modem_info) || modem_info_saved)) {
       if (modem_info->bchan_idb && (modem_info != modem_info_saved)) {
           /*
            * Error:
            * the b channel is connected to another modem on the TDM backplane
            */
           return (RC_FAILURE);
       }
       
       /*
        * hwidb->unit contains the logical unit for T1/E1 controller.
        * hwidb->subunit contains the unit number based on slot/subcont
        * for T1/E1 controller.
        */
       if (modem_info_saved) {
           modem_info->pri_chnl.unit         = modem_info->bchan_idb->subcont;
           modem_info->pri_chnl.slot         = modem_info->bchan_idb->slot;
       }
       else {
           modem_info->pri_chnl.unit         = modem_info->dsx0_controller;
           modem_info->pri_chnl.slot         = modem_info->dsx0_slot;
       }
       modem_info->pri_chnl.channel      = modem_info->bchan_num;
       
#if !TDM_WORKAROUND
       /* 
        * we don't park ISDN stream any more 'cause it's automatically
        * parked after tdm_connect_twoway() is called.
        */
       modem_info->pri_chnl.stream_type  = TDM_ISDN_STREAM;
       tdm_connect_special(DEFAULT_RX_TX, &modem_info->pri_chnl);
#endif  
       modem_info->pri_chnl.stream_type  = TDM_PRI_STREAM;
       
       /*
        * tdm_connect_twoway() will tri-state the ISDN stream
        * and connect pri and modem tx/rx streams.
       */
       rc = tdm_connect_twoway(&modem_info->pri_chnl, &modem_info->modem_chnl);
   }
   else
       rc = RC_FAILURE;
   
   return (rc);
}

/*
 * =====================================================================
 * csm_disconnect_pri_modem()
 *
 * Description:
 * This function will call disconnect the modem's tx and rx lines to the
 * T1/PRI B channel's rx and tx lines thru the TDM backplane.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static int csm_disconnect_pri_modem (modem_info_t *modem_info)
{
   boolean   rc;
 
   rc = RC_SUCCESS;

   if (modem_info && (!CSM_USE_PRI_LINE(modem_info) ||
       (modem_info->bchan_idb && CSM_USE_PRI_LINE(modem_info)))) {
       /*
        * Use HWIDB_SB_MODEM_INFO subblock to indicate that the link to PRI 
        * is broken. The tdm_connect_special() will connect PRI and ISDN 
        * streams and tri-state the modem stream. modem_info->bchan_idb 
        * will be null when channelized T1 is running.
        */
#if !TDM_WORKAROUND
       ch_info_t isdn_chnl;
#endif
       if (modem_info ==
           idb_get_hwsb(modem_info->bchan_idb, HWIDB_SB_MODEM_INFO))
           idb_delete_hwsb(modem_info->bchan_idb, HWIDB_SB_MODEM_INFO);

#if TDM_WORKAROUND
       rc = tdm_connect_special(DEFAULT_RX_TX, &modem_info->modem_chnl);
       rc = rc && tdm_connect_special(DEFAULT_RX_TX, &modem_info->pri_chnl);
#else
       modem_info->pri_chnl.stream_type  = TDM_ISDN_STREAM;
       isdn_chnl.stream_type  = TDM_PRI_STREAM;
       isdn_chnl.unit         = modem_info->pri_chnl.unit;
       isdn_chnl.channel      = modem_info->pri_chnl.channel;
       isdn_chnl.slot         = modem_info->pri_chnl.slot;
       isdn_chnl.channel_group_used = FALSE;
       rc = rc && tdm_connect_twoway(&modem_info->pri_chnl, &isdn_chnl);
#endif
   }
   else
       rc = RC_FAILURE;

   return (rc);
}


/*
 * =====================================================================
 * csm_enter_idle_state()
 *
 * Description:
 * This routine reset those CSM related information before the CSM state
 * gets set back to idle.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_enter_idle_state (modem_info_t *modem_info)                    
{
   /*
    * modem_info->bchan_idb will be null when channelized T1 is running.
    */
    if (modem_info ==
        idb_get_hwsb(modem_info->bchan_idb, HWIDB_SB_MODEM_INFO))
        idb_delete_hwsb(modem_info->bchan_idb, HWIDB_SB_MODEM_INFO);
    
   modem_info->csm_state      = CSM_IDLE_STATE;
   modem_info->csm_event_proc = csm_proc_idle;
   modem_info->bchan_num      = INVALID_B_CHANNEL;
   modem_info->bchan_idb      = modem_info->dchan_idb = NULL;

   if (LIST_ELEMENT_QUEUED(&modem_info->waiting_bchan_link))
       list_remove(&modem_pool->wait_for_bchan_header,
                   &modem_info->waiting_bchan_link,
                   modem_info);

   csm_modem_deallocate(modem_info);

   GET_TIMESTAMP(modem_info->stat_call_duration_ended);
   TS_VALUE(modem_info->stat_total_call_duration) += 
       CLOCK_DIFF_UNSIGNED(modem_info->stat_call_duration_started, 
                           modem_info->stat_call_duration_ended);
}

/*
 * =====================================================================
 * csm_enter_disconnecting_state()
 *
 * Description:
 * This routine will set some appropriate information before CSM
 * enter into the IC4_OC8_DISCONNECTING state.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * state      - it's either CSM_IC4_DISCONNECTING or CSM_OC8_DISCONNECTING.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_enter_disconnecting_state (modem_info_t *modem_info, 
                                                  csm_state_t state)
{
    /*
     * modem_info->bchan_idb will be null when channelized T1 is running.
     */
    if (modem_info ==
        idb_get_hwsb(modem_info->bchan_idb, HWIDB_SB_MODEM_INFO))
        idb_delete_hwsb(modem_info->bchan_idb, HWIDB_SB_MODEM_INFO);

    modem_info->csm_state      = state;
    modem_info->csm_event_proc = csm_proc_ic4_oc8_disconnecting;
    GET_TIMESTAMP(modem_info->wdt_timestamp_started);
    CSM_SET_STATUS(modem_info, CSM_STATUS_ACTIVE_WDT);
}

/*
 * =====================================================================
 * csm_enter_busy_tone_state()
 *
 * Description:
 * This routine will set some appropriate information and connect
 * the busy tone to the rx line of modem before CSM enter into the 
 * OC7_BUSY_ERROR state.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_enter_busy_tone_state (modem_info_t *modem_info)
{
    /*
     * modem_info->bchan_idb will be null when channelized T1 is running.
     */
    if (modem_info ==
        idb_get_hwsb(modem_info->bchan_idb, HWIDB_SB_MODEM_INFO))
        idb_delete_hwsb(modem_info->bchan_idb, HWIDB_SB_MODEM_INFO);

    modem_info->modem_chnl.stream_type = TDM_TONE_STREAM;
    tdm_connect_special(BUSYTONE_RX, &modem_info->modem_chnl);
    modem_info->modem_chnl.stream_type = TDM_MODEM_STREAM;
    modem_info->csm_state      = CSM_OC7_BUSY_ERROR;
    modem_info->csm_event_proc = csm_proc_oc7_busy_error;
    GET_TIMESTAMP(modem_info->wdt_timestamp_started);
    CSM_SET_STATUS(modem_info, CSM_STATUS_ACTIVE_WDT);
}

/*
 * =====================================================================
 * csm_send_neat_ct1_event()
 *
 * Description:
 * This support routine will format the message which will be sent to
 * NEAT processor with the new event and cause.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 * event      - the new event for CT1.
 * cause      - the cause for the event.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void csm_send_neat_ct1_event (modem_info_t *modem_info, ushort event, 
                                     ushort cause)
{
    csm_message_t    csm_message;
    neat_ipc_msg_ret rc;
    
    csm_message.src_slot       = CSM_MODEM_SLOT(modem_info);
    csm_message.src_controller = 0;
    csm_message.src_entity     = CSM_MODEM_PORT(modem_info);
    csm_message.dest_slot      = modem_info->dsx0_slot;
    csm_message.dest_controller= modem_info->dsx0_controller;
    csm_message.dest_entity    = modem_info->bchan_num;
    csm_message.data.call_id   = modem_info->call_id;
    csm_message.event          = event;
    csm_message.cause          = cause;
    
    rc = neat_ipc_message_send(csm_message.dest_slot, 
                               MSG_MPU_CSM2RBS,
                               (uchar*) &csm_message,
                               sizeof(csm_message_t));
    if ((rc != IPCMSG_ERROR_NOERROR) && modem_mgmt_csm_debug)
        buginf("CSM_SEND_NEAT_CT1_EVENT: failed rc = 0x%x", rc);
}

/****************************************************************************
 * CSM State Machine Functions:
 *    The network interrupt should be disable prior to calling the
 *    following functions. It means that the network interrupt should
 *    be disable before (*modem_info->csm_event_proc)(modem_info) is
 *    executed.
 */
/*
 * =====================================================================
 * csm_proc_idle()
 *
 * Description:
 * This routine handles all of events happening under the state CSM_IDLE_STATE.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_idle (modem_info_t* modem_info)
{
   dchan_info_t    *dchan_info;
   boolean rc;
   static const char func_name[] = "CSM_PROC_IDLE:";
 
   if (!modem_pool->max_dchan_idb_index) {
       if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s %s at slot %d, port %d: no signaling device is "
                  "available\n", func_name, 
                  csm_event_name[modem_info->csm_event], 
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
       return (RC_FAILURE);
   }

   rc = RC_SUCCESS;

   switch (modem_info->csm_event) {
   case CSM_EVENT_DSX0_CALL:           /* incoming call, Neat interrupt */
        modem_pool->total_free_rbs_timeslot--;
        /*
         * Falling through ...
         */
   case CSM_EVENT_ISDN_CALL:           /* incoming call, ISDN process  */
        /*
         * B channel cut thru here. It can be in csm_proc_ic2_wait_for_carrier()
         * for pri. However, for robbed bit signaling, it should be here.
         */
        rc = csm_connect_pri_modem(modem_info);

        /*
         * a modem_info is choosen and locked out of the modem pool 
         * before this function is called 
         */
        csm_turn_ring_on(modem_info);
        modem_info->csm_state      = CSM_IC1_RINGING;
        modem_info->csm_event_proc = csm_proc_ic1_ring;
        GET_TIMESTAMP(modem_info->wdt_timestamp_started);   /* start wdt */
        CSM_SET_STATUS(modem_info, CSM_STATUS_ACTIVE_WDT);
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        break;

   case CSM_EVENT_MODEM_OFFHOOK:       /* outgoing call, interrupt */
        if (CSM_IS_LOCKED(modem_info)) {
            /*
             * Modem is not ready yet to dial out.
             */
            modem_info->invalid_event_count++;
            rc = RC_FAILURE;
            break;
        }

        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        dchan_info = csm_get_1st_signaling_channel(modem_info, FALSE);
        if (dchan_info) {
            /*
             * Check if there's a free DSX0 signaling channel.
             * The call_id is assigned in csm_get_1st_signaling_channel() 
             * already.
             */
            modem_info->csm_flags      &= ~CSM_FLAG_USE_PRI_LINE;
            modem_info->dsx0_slot       = dchan_info->slot;
            modem_info->dsx0_controller = UNKNOWN_B_CHANNEL;
            modem_info->bchan_num       = UNKNOWN_B_CHANNEL;
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_DIAL_OUT, NO_CAUSE);
            modem_info->csm_state       = CSM_OC4_DIALING;
            modem_info->csm_event_proc  = csm_proc_oc4_dialing;            
        }
        else 
            /*
             * If not, collect the phone number and then use the PRI D channel.
             */
            csm_start_digit_collection(modem_info);

        list_enqueue(&modem_pool->wait_for_bchan_header,
                     &modem_info->waiting_bchan_link,
                     modem_info);            
        GET_TIMESTAMP(modem_info->wdt_timestamp_started);     /* start wdt */
        CSM_SET_STATUS(modem_info, (CSM_STATUS_ACTIVE_WDT 
                                   | CSM_STATUS_ACTIVE_CALL));
        break;

   default:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s unknown at slot %d, port %d\n", func_name, 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;
        break;
   }

   if (rc == RC_SUCCESS) {
       GET_TIMESTAMP(modem_info->stat_call_duration_started);
       TS_VALUE(modem_info->stat_call_duration_ended) = 0;
   }

   return (rc);
}

/*
 * =====================================================================
 * csm_proc_ic4_oc8_disconnecting()
 *
 * Description:
 * This routine handles all of events happening under the state 
 * CSM_IC4_DISCONNECTING or CSM_OC8_DISCONNECTING.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_ic4_oc8_disconnecting (modem_info_t* modem_info)
{
   boolean rc;
   static const char func_name[] = "CSM_PROC_IC4_OC8_DISCONNECTING:";
 
   rc = RC_SUCCESS;

   switch (modem_info->csm_event) {
   case CSM_EVENT_WDT_TIMEOUT:
        modem_info->wdt_timeout_count++;
        if (CSM_IS_IC_STATE(modem_info)) {
            modem_info->connection_fail_reason = CSM_FAIL_MISS_EXPECTED_ON_HOOK;
            modem_info->fail_csm_state = CSM_IC4_DISCONNECTING;
        }
        else {
            modem_info->connection_fail_reason = CSM_FAIL_MISS_EXPECTED_ON_HOOK;
            modem_info->fail_csm_state = CSM_OC8_DISCONNECTING;
        }
        /* 
         * Falling through ...
         */
   case CSM_EVENT_MODEM_ONHOOK:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        csm_enter_idle_state(modem_info);
        break;

   case CSM_EVENT_ISDN_DISCONNECTED:
   case CSM_EVENT_DSX0_DISCONNECTED:
   case CSM_EVENT_SHUTDOWN_INTERFACE:
        /*
         * Ignore these events
         */
        break;

   default:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s unknown at slot %d, port %d\n", func_name, 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;
        break;
   }   

   return (rc);
}

/*
 * =====================================================================
 * csm_proc_ic1_ring()
 *
 * Description:
 * This routine handles all of events happening under the state CSM_IC1_RINGING.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_ic1_ring (modem_info_t* modem_info)
{
   boolean rc;
   csm_fail_reason_code_t fcode;
   static const char func_name[] = "CSM_PROC_IC1_RING:";
   
   rc    = RC_SUCCESS;
   fcode = CSM_FAIL_NONE;
   /*
    * Stop ringing whatever the event happens
    */
   switch (modem_info->csm_event) {
   case CSM_EVENT_MODEM_OFFHOOK:      /* interrupt */
        if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s %s at slot %d, port %d\n", func_name,
                  csm_event_name[modem_info->csm_event], 
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        csm_turn_ring_off(modem_info);

        if (CSM_USE_PRI_LINE(modem_info))  { /* if it's an isdn call */
              reg_invoke_isdn_frommodem(modem_info->dchan_idb, modem_info->call_id,
                                       0, modem_info->bchan_num, VOICE_ANS, NO_CAUSE,
                                       NULL, modem_pool->encoding);
        }
        else
            csm_send_neat_ct1_event(modem_info, EVENT_VOICE_ANS, NO_CAUSE);

        modem_info->csm_state      = CSM_IC2_WAIT_FOR_CARRIER;
        modem_info->csm_event_proc = csm_proc_ic2_wait_for_carrier;
        break;

   case CSM_EVENT_WDT_TIMEOUT:       /* modem never picks up the phone */
        modem_info->wdt_timeout_count++;
        fcode = CSM_FAIL_RINGING;
        /*
         * Falling through ...
         */
   case CSM_EVENT_SHUTDOWN_INTERFACE:
        if (!fcode)
            fcode = CSM_FAIL_SHUTDOWN_INTERFACE;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name,
                   csm_event_name[modem_info->csm_event],
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        csm_turn_ring_off(modem_info);
        if (CSM_USE_PRI_LINE(modem_info)) /* if it's an isdn call */
              reg_invoke_isdn_frommodem(modem_info->dchan_idb, modem_info->call_id,
                                       0, modem_info->bchan_num, VOICE_HANGUP, TIMER_EXPIRY,
                                       NULL, modem_pool->encoding);
        else {
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_IDLE, TIMER_EXPIRY);
            modem_pool->total_free_rbs_timeslot++;
        }

        rc = csm_disconnect_pri_modem(modem_info);
        csm_enter_idle_state(modem_info);
        modem_info->stat_ic_failure++;
        break;

   case CSM_EVENT_MODEM_ONHOOK:      /* invalid interrupt */
        fcode = CSM_FAIL_UNEXPECTED_ON_HOOK;
        if (CSM_USE_PRI_LINE(modem_info)) /* if it's an isdn call */
              reg_invoke_isdn_frommodem(modem_info->dchan_idb, modem_info->call_id,
                                       0, modem_info->bchan_num, VOICE_HANGUP,
                                       UNSPECIFIED_CAUSE, NULL, modem_pool->encoding);
        else
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_IDLE, 
                                    UNSPECIFIED_CAUSE);
        /* 
         * Falling through ...
         */
   case CSM_EVENT_DSX0_DISCONNECTED:
        if (!CSM_USE_PRI_LINE(modem_info))
            modem_pool->total_free_rbs_timeslot++;
        /* 
         * Falling through ...
         */
   case CSM_EVENT_ISDN_DISCONNECTED:
        if (!fcode) {
            fcode = CSM_FAIL_ISDN_CAUSE;
            modem_info->stat_remote_link_disc++;
        }
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s: %s at slot %d, port %d\n", func_name,
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));

        rc = csm_disconnect_pri_modem(modem_info);
        csm_turn_ring_off(modem_info);
        csm_enter_idle_state(modem_info);
        modem_info->stat_ic_failure++;
        modem_info->stat_ring_no_answer++;
        break;

   default:
       if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s unknown at slot %d, port %d\n", func_name,
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;
        break;
   }

   modem_info->connection_fail_reason = fcode;
   modem_info->fail_csm_state = CSM_IC1_RINGING;
   return (rc);
}

/*
 * =====================================================================
 * csm_proc_ic2_wait_for_carrier()
 *
 * Description:
 * This routine handles all of events happening under the state 
 * CSM_IC2_WAIT_FOR_CARRIER.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_ic2_wait_for_carrier (modem_info_t* modem_info)
{
   boolean rc;
   csm_fail_reason_code_t fcode;
   static const char func_name[] = "CSM_PROC_IC2_WAIT_FOR_CARRIER:";
 
   rc    = RC_SUCCESS;
   fcode = CSM_FAIL_NONE;

   switch (modem_info->csm_event) {
   case CSM_EVENT_ISDN_CONNECTED:
   case CSM_EVENT_DSX0_CONNECTED:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        CSM_CLEAR_STATUS(modem_info, CSM_STATUS_ACTIVE_WDT);
        modem_info->csm_state      = CSM_IC3_CONNECTED;
        modem_info->csm_event_proc = csm_proc_ic3_oc6_connected;
        modem_info->stat_ic_complete++;
        break;

   case CSM_EVENT_MODEM_ONHOOK:       /* valid interrupt */
        fcode = CSM_FAIL_UNEXPECTED_ON_HOOK;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        if (CSM_USE_PRI_LINE(modem_info))
              reg_invoke_isdn_frommodem(modem_info->dchan_idb,
                                        modem_info->call_id,
                                        0, modem_info->bchan_num,
                                        VOICE_HANGUP, NORMAL_CLEARING, NULL,
                                        modem_pool->encoding);
        else {
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_IDLE, 
                                    NORMAL_CLEARING);
            modem_pool->total_free_rbs_timeslot++;
        }
        rc = csm_disconnect_pri_modem(modem_info);
        csm_enter_idle_state(modem_info);
        modem_info->stat_ic_failure++;
        break;

   case CSM_EVENT_WDT_TIMEOUT:
        fcode = CSM_FAIL_WDT_TIMEOUT;
        modem_info->wdt_timeout_count++;
        /*
         * Falling through:
         */
   case CSM_EVENT_SHUTDOWN_INTERFACE:
        if (!fcode)
            fcode = CSM_FAIL_SHUTDOWN_INTERFACE;
        if (CSM_USE_PRI_LINE(modem_info))
              reg_invoke_isdn_frommodem(modem_info->dchan_idb,
                                        modem_info->call_id,
                                        0, modem_info->bchan_num,
                                        VOICE_HANGUP, TIMER_EXPIRY, NULL,
                                        modem_pool->encoding);
        else
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_IDLE, TIMER_EXPIRY);
        /*
         * Falling through:
         */
   case CSM_EVENT_DSX0_DISCONNECTED:
        if (!CSM_USE_PRI_LINE(modem_info))
            modem_pool->total_free_rbs_timeslot++;
        /*
         * Falling through:
         */
   case CSM_EVENT_ISDN_DISCONNECTED:
        if (!fcode)
            fcode = CSM_FAIL_ISDN_CAUSE;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        rc = csm_disconnect_pri_modem(modem_info);
        csm_enter_disconnecting_state(modem_info, CSM_IC4_DISCONNECTING);
        modem_info->stat_ic_failure++;
        modem_info->stat_remote_link_disc++;
        break;

   default:
       if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s unknown at slot %d, port %d\n", func_name, 
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;
        break;
   }

   modem_info->connection_fail_reason = fcode;
   modem_info->fail_csm_state = CSM_IC2_WAIT_FOR_CARRIER;
   return (rc);
}

/*
 * =====================================================================
 * csm_proc_ic3_oc6_connected()
 *
 * Description:
 * This routine handles all of events happening under the state 
 * CSM_IC3_CONNECTED or CSM_OC6_CONNECTED.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_ic3_oc6_connected (modem_info_t* modem_info)
{
   boolean rc;
   static const char func_name[] = "CSM_PROC_IC3_OC6_CONNECTED:";
 
   rc = RC_SUCCESS;

   switch (modem_info->csm_event) {
   case CSM_EVENT_MODEM_ONHOOK:          /* valid interrupt */
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name,
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        rc = csm_disconnect_pri_modem(modem_info);
        if (CSM_USE_PRI_LINE(modem_info))
              reg_invoke_isdn_frommodem(modem_info->dchan_idb,
                                        modem_info->call_id,
                                        0, modem_info->bchan_num,
                                        VOICE_HANGUP, NORMAL_CLEARING, NULL,
                                        modem_pool->encoding);
        else {
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_IDLE, 
                                    NORMAL_CLEARING);
            modem_pool->total_free_rbs_timeslot++;
        }
        csm_enter_idle_state(modem_info);
        break;

   case CSM_EVENT_SHUTDOWN_INTERFACE:
        if (CSM_USE_PRI_LINE(modem_info))
              reg_invoke_isdn_frommodem(modem_info->dchan_idb,
                                        modem_info->call_id,
                                        0, modem_info->bchan_num,
                                        VOICE_HANGUP, NORMAL_CLEARING, NULL,
                                        modem_pool->encoding);
        else
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_IDLE, 
                                    NORMAL_CLEARING);
        /*
         * Falling through:
         */
   case CSM_EVENT_DSX0_DISCONNECTED:
        if (!CSM_USE_PRI_LINE(modem_info))
            modem_pool->total_free_rbs_timeslot++;
        /*
         * Falling through:
         */
   case CSM_EVENT_ISDN_DISCONNECTED:
       if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s %s at slot %d, port %d\n", func_name,
                  csm_event_name[modem_info->csm_event], 
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        rc = csm_disconnect_pri_modem(modem_info);
        csm_enter_disconnecting_state(modem_info, 
                                      ((modem_info->csm_state & CSM_IC_STATE) 
                                       ? CSM_IC4_DISCONNECTING 
                                       : CSM_OC8_DISCONNECTING));
        modem_info->stat_remote_link_disc++;
        break;
 
   default:
       if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s unknown at slot %d, port %d\n", func_name,
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;
        break;
   }

   return (rc);
}

/*
 * =====================================================================
 * csm_proc_oc1_request_digit()
 *
 * Description:
 * This routine handles all of events happening under the state
 * CSM_OC1_REQUEST_DIGIT.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_oc1_request_digit (modem_info_t* modem_info)
{
   boolean rc;
   csm_fail_reason_code_t fcode;
   static const char func_name[] = "CSM_PROC_OC1_REQUEST_DIGIT:";
 
   rc    = RC_SUCCESS;
   fcode = CSM_FAIL_NONE;

   switch (modem_info->csm_event) {
   case CSM_EVENT_DIGIT_COLLECT_READY:
        /*
         * Connect the modem's rx line to the dial-tone time slot
         * to let the modem start dialing digits
         */
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->modem_chnl.stream_type = TDM_TONE_STREAM;
        rc = tdm_connect_special(DIALTONE_RX, &modem_info->modem_chnl);
        modem_info->modem_chnl.stream_type = TDM_MODEM_STREAM;
        modem_info->csm_state      = CSM_OC2_COLLECT_1ST_DIGIT;
        modem_info->csm_event_proc = csm_proc_oc2_collect_1st_digit;        
        break;

   case CSM_EVENT_WDT_TIMEOUT:
        fcode = CSM_FAIL_WDT_TIMEOUT;
        modem_info->wdt_timeout_count++;
        modem_info->stat_oc_dial_timeout++;
        /*
         * Falling through ...
         */
   case CSM_EVENT_SHUTDOWN_INTERFACE:
        if (!fcode)
            fcode = CSM_FAIL_SHUTDOWN_INTERFACE;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /*
         * Disconnect the Digit Collect Time Slot from the modem's tx line
         * Disconnect modem
         */
        csm_process_next_pending_digit_collect(modem_info, PARK_ONLY_MODEM_TX);
        csm_enter_disconnecting_state(modem_info, CSM_OC8_DISCONNECTING);
        modem_info->stat_dial_failure++;
        modem_info->stat_oc_failure++;
        modem_info->stat_oc_no_dial_tone++;
        break;

   case CSM_EVENT_MODEM_ONHOOK:
        fcode = CSM_FAIL_UNEXPECTED_ON_HOOK;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /*
         * Disconnect the Digit Collect Time Slot from the modem's tx line
         */
        csm_process_next_pending_digit_collect(modem_info, PARK_ONLY_MODEM_TX);
        csm_enter_idle_state(modem_info);
        modem_info->stat_dial_failure++;
        modem_info->stat_oc_failure++;
        break;

   default:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s unknown at slot %d, port %d\n", func_name, 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;        
        break;
   }

   modem_info->connection_fail_reason = fcode;
   modem_info->fail_csm_state = CSM_OC1_REQUEST_DIGIT;
   return (rc);
}

/*
 * =====================================================================
 * csm_proc_oc2_collect_1st_digit()
 *
 * Description:
 * This routine handles all of events happening under the state 
 * CSM_OC2_COLLECT_1ST_DIGIT.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_oc2_collect_1st_digit (modem_info_t* modem_info)
{
   boolean rc;
   csm_fail_reason_code_t fcode;
   static const char func_name[] = "CSM_PROC_OC2_COLLECT_1ST_DIGIT:";
 
   rc    = RC_SUCCESS;
   fcode = CSM_FAIL_NONE;

   switch (modem_info->csm_event) {
   case CSM_EVENT_GET_1ST_DIGIT:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /*
         * Disconnect the modem's rx line from the dial-tone time slot
         * - turn off the dial tone 
         */
        modem_info->modem_chnl.stream_type = TDM_TONE_STREAM;
        rc = tdm_connect_special(DEFAULT_RX, &modem_info->modem_chnl);
        modem_info->modem_chnl.stream_type = TDM_MODEM_STREAM;
        modem_info->csm_state      = CSM_OC3_COLLECT_ALL_DIGIT;
        modem_info->csm_event_proc = csm_proc_oc3_collect_all_digit;
        break;

   case CSM_EVENT_WDT_TIMEOUT:
        fcode = CSM_FAIL_WDT_TIMEOUT;
        modem_info->wdt_timeout_count++;
        modem_info->stat_oc_dial_timeout++;
        /*
         * Falling through:
         */
   case CSM_EVENT_SHUTDOWN_INTERFACE:
        if (!fcode)
            fcode = CSM_FAIL_SHUTDOWN_INTERFACE;
        /*
         * Falling through:
         */
   case CSM_EVENT_ABORT_DIGIT_COLLECT:
        if (!fcode) {
            modem_info->stat_oc_dial_timeout++;   /* timeout in NEAT */
            fcode = CSM_FAIL_DC_ABORT_RSP;
        }

        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /*
         * Disconnect the Digit Collect Time Slot from the modem's tx/rx line
         * Disconnect modem
         */
        csm_process_next_pending_digit_collect(modem_info, 
                                               PARK_BOTH_MODEM_TX_RX);
        csm_enter_disconnecting_state(modem_info, CSM_OC8_DISCONNECTING);
        modem_info->stat_dial_failure++;
        modem_info->stat_oc_failure++;
        break;

   case CSM_EVENT_MODEM_ONHOOK:
        fcode = CSM_FAIL_UNEXPECTED_ON_HOOK;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /*
         * Disconnect the Digit Collect Time Slot from the modem's tx/rx line
         */
        csm_process_next_pending_digit_collect(modem_info, 
                                               PARK_BOTH_MODEM_TX_RX);
        csm_enter_idle_state(modem_info);
        modem_info->stat_dial_failure++;
        modem_info->stat_oc_failure++;
        break;

   default:
       if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s unknown at slot %d, port %d\n", func_name, 
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;
        break;
   }

   modem_info->connection_fail_reason = fcode;
   modem_info->fail_csm_state = CSM_OC2_COLLECT_1ST_DIGIT;
   return (rc);
}

/*
 * =====================================================================
 * csm_proc_oc3_collect_all_digit()
 *
 * Description:
 * This routine handles all of events happening under the state 
 * CSM_OC3_COLLECT_ALL_DIGIT.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_oc3_collect_all_digit (modem_info_t* modem_info)
{
   dchan_info_t *dchan_info;
   boolean       rc;
   csm_fail_reason_code_t fcode;
   static const char func_name[] = "CSM_PROC_OC3_COLLECT_ALL_DIGIT:";
 
   rc    = RC_SUCCESS;
   fcode = CSM_FAIL_NONE;

   switch (modem_info->csm_event) {
   case CSM_EVENT_GET_ALL_DIGITS:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s (%s) at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   modem_info->called_party_num,
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        dchan_info = csm_get_1st_signaling_channel(modem_info, TRUE);
        if (dchan_info) {
            /*
             * Select a PRI line for dialing the phone number
             */
              reg_invoke_isdn_frommodem(modem_info->dchan_idb, modem_info->call_id,
                                        0, modem_info->bchan_num,
                                        VOICE_ISDNCALL, NO_CAUSE,
                                        modem_info->called_party_num,
                                        modem_pool->encoding);
            /*
             * The number dialed will be logged.
             */
            reg_invoke_csm_event_log(CSM_ELOG_PHONE_NUMBER_COLLECTED, 
                                     modem_info);
            modem_info->csm_state      = CSM_OC4_DIALING;
            modem_info->csm_event_proc = csm_proc_oc4_dialing;
            /*
             * Disconnect the Digit Collect Time Slot from the modem's tx line
             */
            csm_process_next_pending_digit_collect(modem_info, 
                                                   PARK_ONLY_MODEM_TX);
            break;
        }
        else
            if (CSM_DEBUG_ENABLED(modem_info))
                buginf("\n%s no signaling channel available for slot %d, "
                       "port %d\n", func_name, CSM_MODEM_SLOT(modem_info), 
                       CSM_MODEM_PORT(modem_info));
        fcode = CSM_FAIL_NO_SIGANL_CHANNEL;
        /*
         * Falling through...
         */
   case CSM_EVENT_WDT_TIMEOUT:
        if (!fcode) {
            fcode = CSM_FAIL_WDT_TIMEOUT;
            modem_info->wdt_timeout_count++;
            modem_info->stat_oc_dial_timeout++;
        }
        /*
         * Falling through...
         */
   case CSM_EVENT_SHUTDOWN_INTERFACE:
        if (!fcode)
            fcode = CSM_FAIL_SHUTDOWN_INTERFACE;

        if (CSM_DEBUG_ENABLED(modem_info) 
            && (modem_info->csm_event != CSM_EVENT_GET_ALL_DIGITS))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /*
         * Disconnect the Digit Collect Time Slot from the modem's tx line
         */
        csm_process_next_pending_digit_collect(modem_info, PARK_ONLY_MODEM_TX);
        
        /*
         * Supply a busy tone to modem to disconnect the modem call
         */
        csm_enter_busy_tone_state(modem_info);
        modem_info->stat_dial_failure++;
        modem_info->stat_oc_failure++;
        break;

   case CSM_EVENT_MODEM_ONHOOK:
        fcode = CSM_FAIL_UNEXPECTED_ON_HOOK;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /*
         * Disconnect the Digit Collect Time Slot from the modem's tx line
         */
        csm_process_next_pending_digit_collect(modem_info, PARK_ONLY_MODEM_TX);
        csm_enter_idle_state(modem_info);
        modem_info->stat_dial_failure++;
        modem_info->stat_oc_failure++;
        break;

   default:
       if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s unknown at slot %d, port %d\n", func_name, 
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;
        break;
   }

   modem_info->connection_fail_reason = fcode;
   modem_info->fail_csm_state = CSM_OC3_COLLECT_ALL_DIGIT;
   return (rc);
}

/*
 * =====================================================================
 * csm_proc_oc4_dialing()
 *
 * Description:
 * This routine handles all of events happening under the state CSM_OC4_DIALING.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_oc4_dialing (modem_info_t* modem_info)
{
   dchan_info_t *dchan_info;
   boolean       rc, use_pri_line;
   csm_fail_reason_code_t fcode;
   static const char func_name[] = "CSM_PROC_OC4_DIALING:";
 
   rc    = RC_SUCCESS;
   fcode = CSM_FAIL_NONE;

   switch (modem_info->csm_event) {
   case CSM_EVENT_DSX0_BCHAN_ASSIGNED:
        modem_pool->total_free_rbs_timeslot--;
       
   case CSM_EVENT_ISDN_BCHAN_ASSIGNED:
        rc = csm_connect_pri_modem(modem_info);      /* b channel cut thru */
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        list_remove(&modem_pool->wait_for_bchan_header,
                    &modem_info->waiting_bchan_link,
                    modem_info);
        modem_info->csm_state      = CSM_OC5_WAIT_FOR_CARRIER;
        modem_info->csm_event_proc = csm_proc_oc5_wait_for_carrier;
        break;

   case CSM_EVENT_ISDN_DISCONNECTED:
   case CSM_EVENT_DSX0_DISCONNECTED:
       if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s %s at slot %d, port %d\n", func_name, 
                  csm_event_name[modem_info->csm_event], 
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /* 
         * Try another PRI line until the modem hangs up
         * or runs out of pri lines
         */
        if (modem_info->cause == NO_CIRCUIT_AVAILABLE) {
            /*
             * if there's no b channel available
             */
            use_pri_line = CSM_USE_PRI_LINE(modem_info);
            dchan_info = csm_get_next_signaling_channel(modem_info, 
                                                        use_pri_line);
            if (dchan_info) {
                if (use_pri_line)
                    /*
                     * send the VOICE_ISDNCALL message to ISDN mesage queue
                     */
                    reg_invoke_isdn_frommodem(modem_info->dchan_idb, 
                                              modem_info->call_id, 0, 
                                                modem_info->bchan_num, VOICE_ISDNCALL,
                                                NO_CAUSE,
                                                modem_info->called_party_num,
                                                modem_pool->encoding);
                else {
                    modem_info->dsx0_slot       = dchan_info->slot;
                    csm_send_neat_ct1_event(modem_info, EVENT_CALL_DIAL_OUT, 
                                            NO_CAUSE);
                }
                break;
            }
            else if (!use_pri_line) {
                /*
                 * Since we run out of CT1 signaling interfaces, 
                 * start collecting digits if a pri line exists now
                 * before modem gets timeout.
                 */
                modem_info->bchan_num = INVALID_B_CHANNEL;
                csm_start_digit_collection(modem_info);
                break;
            }
        }
        else if ((modem_info->cause == USER_BUSY) ||
                 (modem_info->cause == REQ_CHANNEL_NOT_AVAIL) ||
                 (modem_info->cause ==  NO_CIRCUIT_AVAILABLE))
            modem_info->stat_oc_busy++;

        /*
         * For other failure causes:
         * Supply a busy tone to modem to disconnect the modem call
         */
        fcode = CSM_FAIL_ISDN_CAUSE;
        csm_enter_busy_tone_state(modem_info);
        modem_info->stat_dial_failure++;
        modem_info->stat_oc_failure++;
        modem_info->stat_remote_link_disc++;
        break;

   case CSM_EVENT_WDT_TIMEOUT:
        fcode = CSM_FAIL_WDT_TIMEOUT;
        modem_info->wdt_timeout_count++;
        modem_info->stat_oc_dial_timeout++;
        /*
         * Falling through:
         */
   case CSM_EVENT_SHUTDOWN_INTERFACE:
        if (!fcode)
            fcode = CSM_FAIL_SHUTDOWN_INTERFACE;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /*
         * Supply a busy tone to modem to disconnect the modem call
         */
        if (CSM_USE_PRI_LINE(modem_info))
              reg_invoke_isdn_frommodem(modem_info->dchan_idb, modem_info->call_id,
                                        0, modem_info->bchan_num,
                                        VOICE_HANGUP, TIMER_EXPIRY, NULL,
                                        modem_pool->encoding);
        else
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_IDLE, TIMER_EXPIRY);
        csm_enter_busy_tone_state(modem_info);
        modem_info->stat_dial_failure++;
        modem_info->stat_oc_failure++;
        break;

   case CSM_EVENT_MODEM_ONHOOK:          /* valid interrupt */
        fcode = CSM_FAIL_UNEXPECTED_ON_HOOK;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /*
         * Both modem's tx/rx lines are already parked
         */
        if (CSM_USE_PRI_LINE(modem_info))        
              reg_invoke_isdn_frommodem(modem_info->dchan_idb, modem_info->call_id,
                                        0, modem_info->bchan_num,
                                        VOICE_HANGUP, NORMAL_CLEARING, NULL,
                                        modem_pool->encoding);
        else
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_IDLE, 
                                    NORMAL_CLEARING);
        csm_enter_idle_state(modem_info);
        modem_info->stat_dial_failure++;
        modem_info->stat_oc_failure++;
        break;

   default:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s unknown at slot %d, port %d\n", func_name, 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;
        break;
   }
   
   modem_info->connection_fail_reason = fcode;
   modem_info->fail_csm_state         = CSM_OC4_DIALING;
   return (rc);
}

/*
 * =====================================================================
 * csm_proc_oc5_wait_for_carrier()
 *
 * Description:
 * This routine handles all of events happening under the state 
 * CSM_OC5_WAIT_FOR_CARRIER.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_oc5_wait_for_carrier (modem_info_t* modem_info)
{
   boolean rc;
   csm_fail_reason_code_t fcode;
   static const char func_name[] = "CSM_PROC_OC5_WAIT_FOR_CARRIER:";
 
   rc    = RC_SUCCESS;
   fcode = CSM_FAIL_NONE;

   switch (modem_info->csm_event) {
   case CSM_EVENT_DSX0_CONNECTED:
        modem_info->modem_chnl.stream_type = TDM_TONE_STREAM;
        rc = tdm_connect_special(DEFAULT_RX, &modem_info->modem_chnl);
        modem_info->modem_chnl.stream_type = TDM_MODEM_STREAM;
        rc = csm_connect_pri_modem(modem_info);      /* b channel cut thru */
        /*
         * Falling through:
         */
   case CSM_EVENT_ISDN_CONNECTED:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        CSM_CLEAR_STATUS(modem_info, CSM_STATUS_ACTIVE_WDT);
        modem_info->csm_state      = CSM_OC6_CONNECTED;
        modem_info->csm_event_proc = csm_proc_ic3_oc6_connected;
        modem_info->stat_oc_complete++;
        break;

   case CSM_EVENT_DSX0_START_TX_TONE:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->modem_chnl.stream_type = TDM_TONE_STREAM;
        rc = tdm_connect_special(DIALTONE_RX, &modem_info->modem_chnl);
        modem_info->modem_chnl.stream_type = TDM_MODEM_STREAM;
        break;

   case CSM_EVENT_WDT_TIMEOUT:
        fcode = CSM_FAIL_WDT_TIMEOUT;
        modem_info->wdt_timeout_count++;
        modem_info->stat_oc_dial_timeout++;
        /*
         * Falling through:
         */
   case CSM_EVENT_SHUTDOWN_INTERFACE:
        if (!fcode)
            fcode = CSM_FAIL_SHUTDOWN_INTERFACE;
        if (CSM_USE_PRI_LINE(modem_info))
              reg_invoke_isdn_frommodem(modem_info->dchan_idb, modem_info->call_id,
                                        0, modem_info->bchan_num,
                                        VOICE_HANGUP, TIMER_EXPIRY, NULL,
                                        modem_pool->encoding);
        else
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_IDLE, TIMER_EXPIRY);
        /*
         * Falling through...
         */ 
   case CSM_EVENT_DSX0_DISCONNECTED:
        if (!CSM_USE_PRI_LINE(modem_info))
            modem_pool->total_free_rbs_timeslot++;
        /*
         * Falling through...
         */ 
   case CSM_EVENT_ISDN_DISCONNECTED:
        if (!fcode)
            fcode = CSM_FAIL_ISDN_CAUSE;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        /*
         * Supply a busy tone to modem to disconnect the modem call
         */
        if (modem_info->cause == USER_BUSY)
            modem_info->stat_oc_busy++;
        csm_enter_busy_tone_state(modem_info);
        modem_info->stat_oc_failure++;
        modem_info->stat_remote_link_disc++;
        break;
 
   case CSM_EVENT_MODEM_ONHOOK:          /* valid interrupt */
        fcode = CSM_FAIL_UNEXPECTED_ON_HOOK;
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        if (CSM_USE_PRI_LINE(modem_info))
            /*
             * Both modem's tx/rx lines are already parked for PRI line
             */
              reg_invoke_isdn_frommodem(modem_info->dchan_idb, modem_info->call_id,
                                        0, modem_info->bchan_num,
                                        VOICE_HANGUP, NORMAL_CLEARING, NULL,
                                        modem_pool->encoding);
        else {
            csm_send_neat_ct1_event(modem_info, EVENT_CALL_IDLE, 
                                    NORMAL_CLEARING);
            /*
             * Question? If it's not under RX_DIAL_TONE, tdm_connect_special()
             * still work????
             */
            modem_info->modem_chnl.stream_type = TDM_TONE_STREAM;
            rc = tdm_connect_special(DEFAULT_RX, &modem_info->modem_chnl);
            modem_info->modem_chnl.stream_type = TDM_MODEM_STREAM;
            modem_pool->total_free_rbs_timeslot++;
        }
	/* Fix for CSCdj10145. csm_disconnect_pri_modem() has to be called
 	 * in both cases (when CSM_USE_PRI_LINE is TRUE and CSM_USE_PRI_LINE is 
	 * FALSE. 
	 */
        rc = csm_disconnect_pri_modem(modem_info);
        csm_enter_idle_state(modem_info);
        modem_info->stat_oc_failure++;
        break;

   default:
       if (CSM_DEBUG_ENABLED(modem_info))
           buginf("\n%s unknown at slot %d, port %d\n", func_name, 
                  CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;
        break;
   }

   modem_info->connection_fail_reason = fcode;
   modem_info->fail_csm_state         = CSM_OC5_WAIT_FOR_CARRIER;
   return (rc);
}

/*
 * =====================================================================
 * csm_proc_oc7_busy_error()
 *
 * Description:
 * This routine handles all of events happening under the state 
 * CSM_OC7_BUSY_ERROR.
 *
 * Parameters:
 * modem_info - modem information descriptor pointer.
 *
 * Returns:
 * Return RC_SUCCESS or RC_FAILURE.
 * =====================================================================
 */
static ushort csm_proc_oc7_busy_error (modem_info_t* modem_info)
{
   boolean rc;   
   static const char func_name[] = "CSM_PROC_OC7_BUSY_ERROR:";
 
   rc = RC_SUCCESS;

   switch (modem_info->csm_event) {
   case CSM_EVENT_WDT_TIMEOUT:
        modem_info->wdt_timeout_count++;
        modem_info->fail_csm_state = CSM_OC7_BUSY_ERROR;
        modem_info->connection_fail_reason = CSM_FAIL_BUSY_TONE;
        /*
         * Falling through ...
         */
   case CSM_EVENT_MODEM_ONHOOK:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s %s at slot %d, port %d\n", func_name, 
                   csm_event_name[modem_info->csm_event], 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->modem_chnl.stream_type = TDM_TONE_STREAM;
        rc = tdm_connect_special(DEFAULT_RX, &modem_info->modem_chnl);
        modem_info->modem_chnl.stream_type = TDM_MODEM_STREAM;
        csm_enter_idle_state(modem_info);
        break;
 
   case CSM_EVENT_SHUTDOWN_INTERFACE:
        /*
         * Ignore this event
         */
        break;

   default:
        if (CSM_DEBUG_ENABLED(modem_info))
            buginf("\n%s unknown at slot %d, port %d\n", func_name, 
                   CSM_MODEM_SLOT(modem_info), CSM_MODEM_PORT(modem_info));
        modem_info->invalid_event_count++;
        rc = RC_FAILURE;
        break;
   }
   
   return (rc);
}



/*************************************************************
 * modem_mgmt_exec_command
 * Implement modem management EXEC commands which can be used
 * for internal debugging purpose. The 'service internal' exec
 * command will enable this exec command.
 */

int csm_make_bit_field (int start, int end)
{
    int retval;
 
    for (retval = 0 ; start <= end ; start++)
        retval |= 1 << start;
    return(retval);
}

static void csm_ct1_rbs_debug (boolean enabled)
{
    ushort           index;
    dchan_info_t    *dchan_info;
    csm_message_t    csm_message;
    neat_ipc_msg_ret rc;
 
    if (!modem_pool->max_dchan_idb_index)
        return;
 
    for(index=0; index < modem_pool->max_dchan_idb_index; index++) {
        dchan_info  = modem_pool->dchan_info[index];
        if (!dchan_info->dchan_idb) {
            csm_message.dest_slot             = dchan_info->slot;
            csm_message.event                 = (enabled) ? EVENT_RBS_DEBUG_ON
                                                          : EVENT_RBS_DEBUG_OFF;
            csm_message.cause                 = NO_CAUSE;
            rc = neat_ipc_message_send(csm_message.dest_slot,
                                       MSG_MPU_CSM2RBS,
                                       (uchar*) &csm_message,
                                       sizeof(csm_message_t));
            if ((rc != IPCMSG_ERROR_NOERROR) && modem_mgmt_csm_debug)
                buginf("%s: failed rc = 0x%x", "CSM_CT1_RBS_DEBUG", rc);
        }
    }
}
 
void modem_mgmt_csm_exec_command (parseinfo *csb)
{
    modem_slot_t *modem_slot;
    ushort       slot;

    ulong        port_map;
    ulong  mask;
    ushort ix;
    modem_info_t *modem_info;

    switch (GETOBJ(int,1)) {
    case CSM_EXEC_DEBUG_RBS:
         csm_ct1_rbs_debug(TRUE);
         return;
 
    case CSM_EXEC_NO_DEBUG_RBS:
         csm_ct1_rbs_debug(FALSE);
         return;
 
    case CSM_EXEC_ENABLE_ROUND_ROBIN:
         modem_pool->modem_control_flags |= MODEM_CTRL_USE_ROUND_ROOBIN;
         return;

    case CSM_EXEC_DISABLE_ROUND_ROBIN:
         modem_pool->next_slot_search = 0;
         modem_pool->next_port_search = 0;
         modem_pool->modem_control_flags &= ~MODEM_CTRL_USE_ROUND_ROOBIN;
         return;

    case CSM_EXEC_RING_DELAY:
         printf("csm simulation is not enable.\n");
         return;
    default:
    }

    slot     = (ushort) GETOBJ(int, 2);
    port_map = (ulong) GETOBJ(int, 3);
    modem_slot = modem_pool->modem_slot[slot];
    if (!modem_slot) {
        printf("slot %d doesn't exist\n", slot);
        return;
    }

    if (modem_mgmt_csm_debug)
        printf("%s(slot=%d, port_map=0x%08x):\n",
               "MODEM_MGMT_CSM_EXEC_COMMAND", slot, port_map);

    switch (GETOBJ(int,1)) {
    case CSM_EXEC_RINGING:
         for (mask=1, ix=0; port_map ; mask<<=1, ix++) {
             if (mask & port_map) {
                 modem_info = modem_slot->modem_info[ix];
                 if (modem_info) {
                     modem_info->csm_flags |= CSM_FLAG_USE_PRI_LINE;
                     csm_turn_ring_on(modem_info);
                 }
                 port_map &= ~mask;
             }
         }         
         break;

    case CSM_EXEC_BUSYOUT:
         for (mask=1, ix=0; port_map ; mask<<=1, ix++) {
             if (mask & port_map) {
                 if (csm_set_busy_out_status(modem_slot->modem_info[ix], 
                                             NULL, NULL))
                     printf("Busy out modem slot %d, port %d\n", slot, ix);
                 else
                     printf("Pending busy out modem slot %d, port %d\n", 
                            slot, ix);
                 port_map &= ~mask;
             }
         }
         break;

    case CSM_EXEC_NO_BUSYOUT:
         for (mask=1, ix=0; port_map ; mask<<=1, ix++) {
             if (mask & port_map) {
                 if (csm_clear_busy_out_status(modem_slot->modem_info[ix]))
                     printf("Clear busy out modem slot %d, port %d\n", 
                            slot, ix);
                 else
                     printf("Fail to clear busy out modem slot %d, port %d\n", 
                            slot, ix);
                 port_map &= ~mask;
             }
         }
         break;

    case CSM_EXEC_OUTBOUND_CALL:  /* off hook */
    case CSM_EXEC_ON_HOOK:
    case CSM_EXEC_ISDN_IDLE:
    case CSM_EXEC_INBOUND_CALL:
    default:
         break;
    }
}


void modem_mgmt_exec_command (parseinfo *csb)
{
  switch (csb->which) {
  case MODEM_MGMT_EXEC_CSM_MODE:
       modem_mgmt_csm_exec_command(csb);
       break;

  default:
       break;
  }
}

void modem_mgmt_csm_show_command (parseinfo *csb)
{
    ushort        slot, ix;
    ulong         port_map, *modem_mask, mask;
    modem_slot_t *modem_slot;

    slot     = (ushort) GETOBJ(int, 1);
    port_map = (ulong) GETOBJ(int, 2);
    modem_slot = modem_pool->modem_slot[slot];
    
    if (!modem_slot) {
        printf("slot %d doesn't exist\n", slot);
        return;
    } 

    printf("\n%s(slot=%d, port_map=0x%08x):\n",
           "MODEM_MGMT_SHOW_CSM_MODEM_INFO", slot, port_map);        
    modem_mask = (ulong*) modem_slot->modem_exist_mask;

    for (ix=0, mask=1; port_map; ix++, mask<<=1) {
        if (port_map & mask) {
            if (modem_slot->modem_info[ix])
                csm_show_modem_info(modem_slot->modem_info[ix]);
            port_map &= ~mask;
        }
    }
}

void modem_mgmt_show_command (parseinfo *csb)
{
   switch (csb->which) {
   case MODEM_MGMT_SHOW_CSM_MODEM_INFO:
       modem_mgmt_csm_show_command(csb);
       break;

   case MODEM_MGMT_SHOW_CSM_LIST:
       printf("\nMODEM_MGMT_SHOW_CSM_LIST:\n");
       csm_print_modem_list(&modem_pool->wait_for_dialing_header, 
                            "wait_for_dialing");
       csm_print_modem_list(&modem_pool->wait_for_bchan_header, 
                            "wait_for_bchan");
       break;

   default:
       break;
   }
}


#define CSM_MAJVERSION  1
#define CSM_MINVERSION  0
#define CSM_EDITVERSION 1

SUBSYS_HEADER(csm, CSM_MAJVERSION, CSM_MINVERSION,
              CSM_EDITVERSION, csm_init, SUBSYS_CLASS_DRIVER,
              "seq: dsx1, modem_pool", "req: dsx1, modem_pool");
