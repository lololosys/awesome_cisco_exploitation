/* $Id: modem_mgmt.c,v 3.1.64.9 1996/09/15 12:38:29 styang Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_mgmt.c,v $
 *------------------------------------------------------------------
 * modem_mgmt.c - Modem Management functions 
 *
 * November, 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: modem_mgmt.c,v $
 * Revision 3.1.64.9  1996/09/15  12:38:29  styang
 * CSCdi55742:  modem oob port output queue is not flushed out properly
 * Branch: California_branch
 * A new 'DC termination' command is sent to modem at end of DC session.
 *
 * Revision 3.1.64.8  1996/08/28  12:59:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.64.7  1996/08/07  09:01:21  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.64.6  1996/07/23  13:25:59  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.64.5  1996/07/16  11:31:06  styang
 * CSCdi58062:  %MODEM-3-UNEXPECTED_STSTUS_EV: Error -- status state
 * machine receive
 * Branch: California_branch
 * 1. The error message is generated wrong, code modified to correct such
 * error.
 * 2. Code clean up.
 *
 * Revision 3.1.64.4  1996/07/11  01:11:38  styang
 * CSCdi62212:  code review clean up
 * Branch: California_branch
 * Code review clean up.
 *
 * Revision 3.1.64.3  1996/06/28  23:23:54  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/06/19  19:29:55  snyder
 * CSCdi60807:  spelllink errors
 * Branch: California_branch
 *
 *         wiht-> with
 *         occured -> occurred
 *         unexpeced -> unexpected
 *
 * Revision 3.1.64.1  1996/06/16  21:16:20  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:34  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "packet.h"
#include "parser.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "subsys.h"
#include "ttysrv.h"
#include "logger.h"
#include "../les/msg_modem.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "../os/free.h"
#include "../os/chunk.h"
#include "../os/async.h"
#include "../ts/if_async.h"
#include "../ui/debug.h"
#include "../ui/command.h"
#include "modem_debug.h"
#include "../dev/flash_defines.h"
#include "if_as5200_tdm.h"
#include "if_as5200_modem.h"
#include "modem_mgmt.h"
#include "parser_defs_modem.h"
#include "csm_registry.h"

modem_group_t *modem_group_list = NULL;
int modem_mgmt_reply_len;
watched_boolean *at_response_ready;
watched_boolean *hdms_reply_ready;
int hdms_reply_status;
static pid_t modem_mgmt_at_mode_reader_fork;
static watched_queue *modem_csm_eventQ;
static queuetype *modem_csm_event_freeQ;
chunk_type *modem_event_hdr_chunk = NULL;
chunk_type *modem_small_event_chunk = NULL;
chunk_type *modem_large_event_chunk = NULL;

modem_mgmt_at_mode_block *modem_mgmt_active_at_mode_blocks,
                         *modem_mgmt_current_at_mode_block;
paktype *send_pak_ptr;
paktype *receive_pak_ptr;
modem_mgmt_pkt_hdr_t *hdms_msg_ptr;
modem_mgmt_pkt_hdr_t *hdms_reply_ptr;


/* Warning : The values for the defines 
 *            - MAX_MODEM SPEED_INDEX
 *            - MAX_MODULATION_TXT_INDEX
 *            - MAX_PROTOCOL_TXT_INDEX
 *            - MAX_COMPRESS_TXT_INDEX
 *            - MAX_MODEM_STATES
 *            - MAX_DIRECTION_TXT_INDEX
 *            - MAX_CALL_PROGRESS_VALUES
 *            - MAX_DISC_REASON_TXT_INDEX
 *
 *	must also be modified in les/modem_mgmt.h for a respective change in the
 *      array declarations
 */

const int modem_speed[MAX_MODEM_SPEED_INDEX] = { 
           75, 300, 600, 1200, 2400, 4800, 
           7200, 9600, 12000, 14400, 16800, 
           19200, 21600, 24000, 26400, 28800, 
           31200, 33600 };
 
const char *const modulation_txt[MAX_MODULATION_TXT_INDEX] = {
           "", "Bel103", "Bel212", 
           "V21","V22", "V22bis", "V23",
           "V32", "V32bis", "VFC", "V34", "V17",
           "V29", "V33", "K56Pls" };
 
const char *const protocol_txt[MAX_PROTOCOL_TXT_INDEX] = {
           "Normal", "Direct", "reliable/MNP",
           "reliable/LAPM", "Sync" };
 
const char *const compress_txt[MAX_COMPRESS_TXT_INDEX] = {
           "None", "V42bis", "MNP Class 5" };
 
const char *const modem_state_txt[MAX_MODEM_STATES] = {
           "Idle", "Incoming ring", 
           "Dialing/Answering",
           "Waiting for Carrier", "Connected",
           "Busy Out", "Disconnecting", "Retrain Initiated",
           "Retrain Accepted", "Rate Renegotiation Initiated",
           "Rate Renegotiation Accepted" };
 
const char *const direction_txt[MAX_DIRECTION_TXT_INDEX] = { 
           "Answer", "Originate" };
 
const char *const call_progress_txt[MAX_CALL_PROGRESS_VALUES] = {
           "No Dial Tone", "No Answer",
           "Ringback", "Busy" };
 
const char *const disc_reason_txt[MAX_DISC_REASON_TXT_INDEX] = { 
           "Unknown", "Lost Carrier", "No carrier",
           "No Dialtone", "Busy", "Reserved", "Reserved", "Watchdog Timeout",
           "Unused", "DTR Drop", "User Hangup", "Unused", "Compression Problem",
           "Retrain Failure", "Remote Link Disc", "Abort",
           "Terminate-Lost Carrier", "Reserved", "Inactivity Timeout",
           "Dial String Error", "Auto Logon Error", "CCP not seen",
           "Loop Sense Failure", "Unused", "DAA Failure", "Link Failure",
           "Callback Failed", "Modulation Error", "Dial Timeout",
           "Unused", "Remote Hangup", "Reserved", "Blacklist",
           "MNP10 Protocol Error", "LAPM Protocol Error",
           "Fax Class 2 Error" };

/*
 * The microcom modem firmware version 1.31 starts supporting the
 * 'dc termination' command (command type= MCOM_DC_SESSION, length=0).
 * Therefore, 1.30 and below doesn't support this feature.
 */
#define MCOM_DC_TERMINATION_FIRMWARE_VERSION "\x1\x0\x1E"

/*
 * =====================================================================
 * modem_mgmt_create_history_eventQ()
 * Description:
 * Create a history event queue if it has not already exists; otherwise,
 * adjust queue size appropriately. 
 *
 * Parameters:
 * modem_info - pointer to modem table of a modem to add the event queue 
 *
 * Returns:
 * None
 * =====================================================================
 */
void modem_mgmt_create_history_eventQ (modem_info_t *modem_info)
{
    modem_mgmt_info_t *modem_mgmt_info;
    modem_history_event_t *ptr = NULL;
    int delta;
    int queuesize;
    int i;

    modem_mgmt_info = modem_info->modem_mgmt_info;
    if (!(modem_mgmt_info)) {
        return;
    }

    /* 
     * check to see if history queue already exists, if
     * so, we need to adjust queue accordingly.
     */
    if (modem_mgmt_info->modem_history_eventQ) {
        /*
         * need to do some resizing of eventQ, so lock it
         */
        while (!lock_semaphore(&(modem_mgmt_info->lock_eventQ))) {
            process_sleep_for(1);
        }
        delta = 
            modem_buffer_size - modem_mgmt_info->modem_history_eventQ->maximum;
        /* 
         * if the number of events in current queue is greater than the new 
         * size, need to dequeue and free some events to fit the new queue size
         *
         * if the number of events in current queue is smaller than the new size
         * OR the new buffer size is bigger than the current buffer size,
         * all is needed is to adjust the maximum queue size
         */
        if (delta < 0) {
            queuesize = QUEUESIZE(modem_mgmt_info->modem_history_eventQ);
            if (queuesize > modem_buffer_size) {
                 for (i = queuesize - modem_buffer_size; i > 0;  i--) {
                     ptr = (modem_history_event_t *)
                         dequeue(modem_mgmt_info->modem_history_eventQ);
                     if (ptr->event.event_length) {
                         if (ptr->event.event_length < 
                             MAX_MODEM_SMALL_EVENT_SIZE) {
                             chunk_free(modem_small_event_chunk,
                                        ptr->event.data_ptr);
                         } else {
                             chunk_free(modem_large_event_chunk,
                                        ptr->event.data_ptr);
                         }
                     }
                     chunk_free(modem_event_hdr_chunk, ptr);
                 }
            }
        }

        /* now adjust the maximum queuesize for eventQ */
        modem_mgmt_info->modem_history_eventQ->maximum = modem_buffer_size;

        /*
         * done with resizing, so unlock eventQ 
         */
        unlock_semaphore(&(modem_mgmt_info->lock_eventQ));
    } else {
        /* 
         * history queue doesn't exist so create one
         */
        modem_mgmt_info->modem_history_eventQ = malloc(sizeof(queuetype));
        unlock_semaphore(&(modem_mgmt_info->lock_eventQ));
   
        /* make sure we can allocate the eventQ */
        if (!(modem_mgmt_info->modem_history_eventQ))
            return;
    
        queue_init(modem_mgmt_info->modem_history_eventQ, modem_buffer_size);
    }
}

/*
 * =====================================================================
 * modem_mgmt_enqueue_event()
 *
 * Description:
 * Put a modem event into the history event queue
 *
 * Parameters:
 * frame_type - type of frame event this is
 * data - pointer to the beginning of the event
 * modem_info - pointer to modem table of a modem to add the event queue
 *
 * Returns:
 * None
 * =====================================================================
 */
static void modem_mgmt_enqueue_event (uchar frame_type, uchar *data, 
                                      modem_info_t *modem_info)
{
    modem_mgmt_info_t *modem_mgmt_info;
    modem_history_event_t *current;

    if (!(modem_info))
        return;

    modem_mgmt_info = modem_info->modem_mgmt_info;
    if (!(modem_mgmt_info)) 
        return;

    /* need to lock eventQ */
    while (!lock_semaphore(&(modem_mgmt_info->lock_eventQ))) {
        process_sleep_for(1);
    }
    /* 
     * check to see if the queue size has been exceeded, if so, need
     * to dequeue the oldest event and free it
     */
    if (QUEUEFULL(modem_mgmt_info->modem_history_eventQ)) {
        current = (modem_history_event_t *)
            dequeue(modem_mgmt_info->modem_history_eventQ);
        if (current->event.event_length) {
            if (current->event.event_length < MAX_MODEM_SMALL_EVENT_SIZE) {
                chunk_free(modem_small_event_chunk, current->event.data_ptr);
            } else {
                chunk_free(modem_large_event_chunk, current->event.data_ptr);
            }
        }
        chunk_free(modem_event_hdr_chunk, current);
    } 

    current = chunk_malloc(modem_event_hdr_chunk);
    if (!current) {
        unlock_semaphore(&(modem_mgmt_info->lock_eventQ));
        return;
    }
    
    current->event.frame_type = frame_type;
    current->event.event_type = *data;
    current->event.event_length = *(data+1);
     
    if (current->event.event_length) {
        if (current->event.event_length < MAX_MODEM_SMALL_EVENT_SIZE) {
            current->event.data_ptr = chunk_malloc(modem_small_event_chunk);
        } else {
            current->event.data_ptr = chunk_malloc(modem_large_event_chunk);
        }
        if (current->event.data_ptr) {      
            bcopy(data+2, current->event.data_ptr, current->event.event_length);
        }
    }
    GET_TIMESTAMP(current->event_time);

    enqueue(modem_mgmt_info->modem_history_eventQ, current);
    unlock_semaphore(&(modem_mgmt_info->lock_eventQ));
}


/*
 * =====================================================================
 * modem_mgmt_check_index()
 *
 * Description:
 * Check the bounds of the array being passed
 *
 * Parameters:
 * index, arr_ptr = Array in which "index"'s range is to be checked
 *
 * Returns:
 * TRUE: If index is within the range of arr_ptr
 * FALSE: Otherwise
 * =====================================================================
 */

boolean modem_mgmt_check_index (ushort index, char *arr_ptr)
{
        ushort        ubound;

        if (arr_ptr == (char *)&modem_speed[0]) {
                ubound = MAX_MODEM_SPEED_INDEX;
        } else if (arr_ptr == (char *)&modulation_txt[0]) {
                ubound = MAX_MODULATION_TXT_INDEX;
        } else if (arr_ptr == (char *)&protocol_txt[0]) {
                ubound = MAX_PROTOCOL_TXT_INDEX;
        } else if (arr_ptr == (char *)&compress_txt[0]) {
                ubound = MAX_COMPRESS_TXT_INDEX;
        } else if (arr_ptr == (char *)&modem_state_txt[0]) {
                ubound = MAX_MODEM_STATES;
        } else if (arr_ptr == (char *)&direction_txt[0]) {
                ubound = MAX_DIRECTION_TXT_INDEX;
        } else if (arr_ptr == (char *)&call_progress_txt[0]) {
                ubound = MAX_DIRECTION_TXT_INDEX;
        } else if (arr_ptr == (char *)&disc_reason_txt[0]) {
                ubound = MAX_DISC_REASON_TXT_INDEX;
        } else {
                printf("modem_mgmt_check_index : Invalid 2nd Parameter\n");
                return(FALSE);
        }
        if (index < ubound) {
                return(TRUE);
        }
        return(FALSE);
}

/*
 * =====================================================================
 * modem_mgmt_upload_trace_to_syslog()
 *
 * Description:
 * Upload Modem event trace from event history queue to a pre-configured
 * syslog server.
 *
 * Parameters:
 * slot, port = modem slot/port #
 *
 * Returns:
 * None
 * =====================================================================
 */

static void modem_mgmt_upload_trace_to_syslog (int slot, int port)
{
    modem_info_t *modem_info;
    modem_mgmt_info_t *modem_mgmt_info;
    modem_history_event_t *ptr = NULL;
    int count;
    uchar *data_ptr = NULL;
    uchar rs232_event, rs232_changed_event;
    static char buff[40], *tmp_buff;
    char        local_disc[40],
                remote_disc[40],
                mod_txt[40],
                dir_txt[40],
                comp_txt[40],
                prot_txt[40];
    ulong        rx_speed = 0,
                tx_speed = 0;

 
    modem_info = CSM_MODEM_INFO_OR_NULL(slot,port);
    if (!modem_info) {
        return;
    }
 
    modem_mgmt_info = modem_info->modem_mgmt_info;
 
    if (!(TRACE_IS_NORMAL_TRIG) && !(TRACE_IS_ABNORMAL_TRIG) && 
        !(TRACE_IS_ALL_TRIG)) {
        return;
    }
 
    /* print trace header */
    errmsg(&msgsym(TRACE_HDR, MODEM_HIST), slot, port); 
 
    if (modem_mgmt_info && modem_mgmt_info->modem_history_eventQ) {
        ptr = modem_mgmt_info->modem_history_eventQ->qhead;
        count = queryqueuedepth(modem_mgmt_info->modem_history_eventQ);
            while ((ptr) && (count > MAX_UPLOAD_TRACE)) {
                count--;
                ptr = ptr->next;
            }
        for (; ((ptr) && (count != 0)); count--) {
            sprint_dhms(buff, ptr->event_time);
            switch (ptr->event.frame_type) {
            case MCOM_STARTUP:
                data_ptr = ptr->event.data_ptr;
                if (*data_ptr == BRASIL_MODEM) {
                    errmsg(&msgsym(STARTUP, MODEM_HIST), buff, 
                        MODEM_VENDOR_NAME(modem_info),
                        *(data_ptr+MCOM_MODEM_REVISION_OFFSET), 
                        *(data_ptr+MCOM_MODEM_REVISION_OFFSET+1),
                        *(data_ptr+MCOM_MODEM_REVISION_OFFSET+2), 
                        *(data_ptr+MCOM_BOOT_REVISION_OFFSET),
                        *(data_ptr+MCOM_BOOT_REVISION_OFFSET+1), 
                        *(data_ptr+MCOM_BOOT_REVISION_OFFSET+2));
                } else {
                    errmsg(&msgsym(STARTUP, MODEM_HIST), buff, "Unknown Modem",
                        *(data_ptr+MCOM_MODEM_REVISION_OFFSET), 
                        *(data_ptr+MCOM_MODEM_REVISION_OFFSET+1),
                        *(data_ptr+MCOM_MODEM_REVISION_OFFSET+2), 
                        *(data_ptr+MCOM_BOOT_REVISION_OFFSET),
                        *(data_ptr+MCOM_BOOT_REVISION_OFFSET+1), 
                        *(data_ptr+MCOM_BOOT_REVISION_OFFSET+2));
                }
                break;
            case MCOM_CONTROL:
                data_ptr = ptr->event.data_ptr;
                errmsg(&msgsym(CONTROL, MODEM_HIST), buff,
                    *(data_ptr+MCOM_EVENT_STATUS0_OFFSET), 
                    *(data_ptr+MCOM_EVENT_STATUS1_OFFSET));
                break;
            case MCOM_DC_SESSION:
                errmsg(&msgsym(DC_SESSION, MODEM_HIST), buff);
                break;
            case MCOM_REFRESH_EVENT_POLL:
            case MCOM_EVENT_POLL:
                data_ptr = ptr->event.data_ptr;
                switch (ptr->event.event_type) {
                case MCOM_TX_RX_EVENT:
                   errmsg(&msgsym(POLL_TX_RX, MODEM_HIST), buff, 
                      (*data_ptr & RS232_TX)?"On":"Off",
                      (*data_ptr & RS232_RX)?"On":"Off");
                   break;
                case MCOM_RS232_EVENT:
                   rs232_event = *(data_ptr + MCOM_RS232_EV_OFFSET);
                   rs232_changed_event =
                       *(data_ptr + MCOM_RS232_EV_CHANGED_OFFSET);
                   errmsg(&msgsym(POLL_RS232, MODEM_HIST), buff,
                       rs232_event & RS232_RTS?"RTS":"noRTS",
                       rs232_changed_event & RS232_RTS?'*':' ',
                       rs232_event & RS232_DTR?"DTR":"noDTR",
                       rs232_changed_event & RS232_DTR?'*':' ',
                       rs232_event & RS232_CTS?"CTS":"noCTS",
                       rs232_changed_event & RS232_CTS?'*':' ',
                       rs232_event & RS232_DSR?"DSR":"noDSR",
                       rs232_changed_event & RS232_DSR?'*':' ',
                       rs232_event & RS232_DCD?"DCD":"noDCD",
                       rs232_changed_event & RS232_DCD?'*':' ',
                       rs232_event & RS232_RI?"RI":"noRI",
                       rs232_changed_event & RS232_RI?'*':' ',
                       rs232_event & RS232_TST?"TST":"noTST",
                       rs232_changed_event & RS232_TST?'*':' ');
                   break;
                case MCOM_MODEM_STATE_EVENT:
                   if (modem_mgmt_check_index((ushort)*data_ptr,(char *)&modem_state_txt[0])) {
                           errmsg(&msgsym(POLL_MODEM_STATE, MODEM_HIST), buff,
                                       modem_state_txt[*data_ptr]);
                   } else {
                           errmsg(&msgsym(POLL_MODEM_STATE, MODEM_HIST), buff,
                                                  "Invalid State");
                   }
                   break;
                case MCOM_CONNECTION_EVENT:
                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_TX_SPEED_OFFSET),
                                              (char *)&modem_speed[0])) {
                        tx_speed = modem_speed[*(data_ptr+MCOM_TX_SPEED_OFFSET)];
                   } else {
                        tx_speed = 0;
                   }
                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_RX_SPEED_OFFSET),
                                              (char *)&modem_speed[0])) {
                        rx_speed = modem_speed[*(data_ptr+MCOM_RX_SPEED_OFFSET)];
                   } else {
                        rx_speed = 0;
                   }
                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_MODULATION_OFFSET),
                                              (char *)&modulation_txt[0])) {
                        strcpy(mod_txt,modulation_txt[*(data_ptr+MCOM_MODULATION_OFFSET)]);
                   } else {
                        strcpy(mod_txt,"Invalid Modulation");
                   }
                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_ORIG_ANSWER_OFFSET),
                                              (char *)&direction_txt[0])) {
                         strcpy(dir_txt,direction_txt[*(data_ptr+MCOM_ORIG_ANSWER_OFFSET)]);
                   } else {
                        strcpy(dir_txt,"Invalid Direction");
                   }
                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_PROTOCOL_OFFSET),
                                              (char *)&protocol_txt[0])) {
                   strcpy(prot_txt,protocol_txt[*(data_ptr+MCOM_PROTOCOL_OFFSET)]);
                   } else {
                        strcpy(prot_txt,"Invalid Protocol");
                   }
                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_COMPRESSION_OFFSET),
                                              (char *)&compress_txt[0])) {
                        strcpy(comp_txt,compress_txt[*(data_ptr+MCOM_COMPRESSION_OFFSET)]);
                   } else {
                        strcpy(comp_txt,"Invalid Compression");
                   }
                   errmsg(&msgsym(POLL_CONN_1, MODEM_HIST), buff,
                                  tx_speed, rx_speed, mod_txt);
                   errmsg(&msgsym(POLL_CONN_2, MODEM_HIST),
                                  dir_txt, prot_txt, comp_txt);
                   break;
                case MCOM_CONNECTION_UPDATE_EVENT:
                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_TX_SPEED_OFFSET),
                                              (char *)&modem_speed[0])) {
                        tx_speed = modem_speed[*(data_ptr+MCOM_TX_SPEED_OFFSET)];
                   } else {
                        tx_speed = 0;
                   }
                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_RX_SPEED_OFFSET),
                                              (char *)&modem_speed[0])) {
                        rx_speed = modem_speed[*(data_ptr+MCOM_RX_SPEED_OFFSET)];
                   } else {
                        rx_speed = 0;
                   }
                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_MODULATION_OFFSET),
                                              (char *)&modulation_txt[0])) {
                        strcpy(mod_txt,modulation_txt[*(data_ptr+MCOM_MODULATION_OFFSET)]);
                   } else {
                        strcpy(mod_txt,"Invalid Modulation");
                   }
                   errmsg(&msgsym(POLL_CONX_UPDATE, MODEM_HIST), buff,
                                  tx_speed, rx_speed, mod_txt);
                   break;
                case MCOM_CALL_PROGRESS_EVENT:
                   if (modem_mgmt_check_index((ushort)*data_ptr,(char *)&call_progress_txt[0])) {
                           errmsg(&msgsym(POLL_PROGRESS, MODEM_HIST), buff,
                               call_progress_txt[*data_ptr]);
                   } else {
                          errmsg(&msgsym(POLL_PROGRESS, MODEM_HIST), buff,
                                   "Invalid Direction");
                   }
                   break;
                case MCOM_DTR_EVENT:
                   errmsg(&msgsym(POLL_DTR, MODEM_HIST), buff,
                       *data_ptr?"DTR On":"DTR Off");
                   break;
                case MCOM_MODEM_INACTIVE_EVENT:
                   errmsg(&msgsym(POLL_INACT, MODEM_HIST), buff,
                       *data_ptr?"Data Inactive":"Data Active");
                   break;
                case MCOM_ANALOG_REPORT_EVENT:
                   errmsg(&msgsym(POLL_ANALOG, MODEM_HIST), buff,
                       *(data_ptr+MCOM_TX_LEVEL_OFFSET) * -1,
                       *(data_ptr+MCOM_RX_LEVEL_OFFSET) * -1,
                       *(data_ptr+MCOM_SIGNAL_TO_NOISE_OFFSET));
                   break;
                case MCOM_END_CONNECTION_EVENT:
                   errmsg(&msgsym(POLL_END_1, MODEM_HIST),
                      buff, *(data_ptr+MCOM_RETRANSMITS_TX_OFFSET),
                      *(data_ptr+MCOM_RETRANSMITS_RX_OFFSET));
                   errmsg(&msgsym(POLL_END_2, MODEM_HIST),
                      *(data_ptr+MCOM_DURATION_HOURS_OFFSET),
                      *(data_ptr+MCOM_DURATION_MINUTES_OFFSET),
                      *(data_ptr+MCOM_DURATION_SECONDS_OFFSET),
                      GETLONG(data_ptr+MCOM_NUM_TX_CHARACTERS_OFFSET),
                      GETLONG(data_ptr+MCOM_NUM_RX_CHARACTERS_OFFSET));

                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_DISC_REASON_LOCAL_OFFSET),
                                              (char *)&disc_reason_txt[0])) {
                         strcpy(local_disc,disc_reason_txt[*(data_ptr+MCOM_DISC_REASON_LOCAL_OFFSET)]);
                   } else {
                        strcpy(local_disc,"Invalid Local Disconnect");
                   }
                   if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_DISC_REASON_REMOTE_OFFSET),
                                              (char *)&disc_reason_txt[0])) {
                        strcpy(remote_disc,disc_reason_txt[*(data_ptr+MCOM_DISC_REASON_REMOTE_OFFSET)]);
                   } else {
                        strcpy(remote_disc,"Invalid Remote Disconnect");
                   }

                   if (*(data_ptr+MCOM_DISC_REASON_LOCAL_OFFSET) == DISC_CONNECTED) {
                        strcpy(local_disc,"Connected");
                   }
                   if (*(data_ptr+MCOM_DISC_REASON_REMOTE_OFFSET) == DISC_CONNECTED) {
                        strcpy(remote_disc,"Connected");
                   }
                   errmsg(&msgsym(POLL_END_3, MODEM_HIST),local_disc,remote_disc);
                   break;
                case MCOM_PHONE_NUMBER_EVENT:
                   errmsg(&msgsym(POLL_PHONE, MODEM_HIST), buff, data_ptr);
                   break;
                case MCOM_CALLER_ID_EVENT:
                   errmsg(&msgsym(POLL_CALLER_ID, MODEM_HIST), buff, data_ptr);
                   break;
                case MCOM_NO_RESPONSE_EVENT:
                   errmsg(&msgsym(POLL_NO_RESP, MODEM_HIST), buff, *data_ptr);
                   break;
                case MCOM_EVENT_LOST_EVENT:
                   errmsg(&msgsym(POLL_EVT_LOST, MODEM_HIST), buff, *data_ptr);
                   break;
                case MCOM_PROTOCOL_ERR_EVENT:
                   errmsg(&msgsym(POLL_ERROR, MODEM_HIST), buff, *data_ptr);
                   break;
                default:
                   errmsg(&msgsym(POLL_UNKNOWN, MODEM_HIST),
                       buff, ptr->event.event_type);
                   break;
                } /* case poll event */
                break;
            case MCOM_DC_SESSION_EVENT_POLL:
                tmp_buff = chunk_malloc(modem_large_event_chunk);
                if (tmp_buff) {
                    strncpy(tmp_buff, ptr->event.data_ptr, 
                            ptr->event.event_length);
                    /* add a null terminate to the string */
                    tmp_buff[ptr->event.event_length] = '\0';
                    errmsg(&msgsym(DC_EVT_POLL, MODEM_HIST), buff, tmp_buff);
                    chunk_free(modem_large_event_chunk, tmp_buff);
                }
                break;
            case CSM_CALLED_NUMBER:
                if (ptr->event.event_length) {
                    tmp_buff = chunk_malloc(modem_large_event_chunk);
                    if (tmp_buff) {
                        strncpy(tmp_buff, ptr->event.data_ptr,
                                ptr->event.event_length);
                        /* add a null terminate to the string */
                        tmp_buff[ptr->event.event_length] = '\0';
                        errmsg(&msgsym(CSM_CALLED_NUM, MODEM_HIST), 
                            buff, tmp_buff);
                        chunk_free(modem_large_event_chunk, tmp_buff);
                    }
                } else {
                    errmsg(&msgsym(CSM_CALLED_NUM,MODEM_HIST), buff, "Unknown");
                }
                break;
            case CSM_CALLER_NUMBER:
                if (ptr->event.event_length) {
                    tmp_buff = chunk_malloc(modem_large_event_chunk);
                    if (tmp_buff) {
                        strncpy(tmp_buff, ptr->event.data_ptr,
                                ptr->event.event_length);
                        /* add a null terminate to the string */
                        tmp_buff[ptr->event.event_length] = '\0';
                        errmsg(&msgsym(CSM_CALLER_NUM, MODEM_HIST), 
                            buff, tmp_buff);
                        chunk_free(modem_large_event_chunk, tmp_buff);
                    }
                } else {
                    errmsg(&msgsym(CSM_CALLER_NUM,MODEM_HIST), buff, "Unknown");
                }
                break;
            default:
                errmsg(&msgsym(UNKNOWN_FRAME, MODEM_HIST), buff);
                break;
            } /* case frame_type */
            ptr = ptr->next;
        }     /* for count loop */
    }
}

/*
 * =====================================================================
 * modem_mgmt_event_handler()
 *
 * Description:
 * Parses a packet received from the modem state machine.
 *
 * Parameters:
 * pak - pointer to packet received from the out of band management port
 * modem_info - pointer to modem table of a modem to add the event queue
 *
 * Returns:
 * None
 * =====================================================================
 */
void modem_mgmt_event_handler (paktype *pak, modem_info_t *modem_info)
{
    modem_mgmt_pkt_hdr_t *mpkt;
    modem_mgmt_info_t *modem_mgmt_info;
    modem_status_info_t *modem_status_info;
    modem_statistics_t *modem_stats;
    uchar frame_type;
    short length;
    ushort event_type = 0;
    ushort event_length = 0;
    uchar *data;
    uchar tx_speed_data, rx_speed_data;
    uchar *tmp_buff;
    int slot = CSM_MODEM_SLOT(modem_info); 
    int port = CSM_MODEM_PORT(modem_info);
    uchar bad_modem_count;

    bad_modem_count = 0;
    modem_mgmt_info = modem_info->modem_mgmt_info;
    modem_status_info = &(modem_mgmt_info->modem_status_info);
    modem_stats = &(modem_mgmt_info->modem_stats);
    mpkt = (modem_mgmt_pkt_hdr_t *) MODEM_MGMT_HEADSTART(pak);

    frame_type = mpkt->type;
    length = (short) mpkt->length;
    data = mpkt->data;

    switch (frame_type) {
    case MCOM_STARTUP:
        /*
         * startup reply
         */
        modem_mgmt_enqueue_event(frame_type, &(mpkt->type), modem_info);
        modem_mgmt_info->modem_status_info.modem_type = 
            *(data+MCOM_MODEM_TYPE_OFFSET);
        bcopy(data+MCOM_MODEM_REVISION_OFFSET,
              modem_status_info->modem_firmware_rev, MAX_MODEM_REV_LEN);
        bcopy(data+MCOM_BOOT_REVISION_OFFSET,
              modem_status_info->modem_boot_firmware_rev, MAX_MODEM_REV_LEN);
        if (MODEM_OOB_DEBUG(modem_info)) {
            buginf("\nMODEM(%d/%d): Modem startup reply "
                "(0x%02x 0x%02x%02x%02x 0x%02x%02x%02x)",
                slot, port,
                modem_status_info->modem_type,
                modem_status_info->modem_firmware_rev[0],
                modem_status_info->modem_firmware_rev[1],
                modem_status_info->modem_firmware_rev[2],
                modem_status_info->modem_boot_firmware_rev[0],
                modem_status_info->modem_boot_firmware_rev[1],
                modem_status_info->modem_boot_firmware_rev[2]);
        }
        csm_setup_capability_flags(modem_info);
        datagram_done(pak);
        break;    
    case MCOM_CONTROL:
        /* 
         * control reply
         */
        modem_mgmt_enqueue_event(frame_type, &(mpkt->type), modem_info);
        if (MODEM_OOB_DEBUG(modem_info)) {
            buginf("\nMODEM(%d/%d): Modem control reply (0x%02x 0x%02x)",
                slot, port, *(data+MCOM_EVENT_STATUS0_OFFSET), 
                *(data+MCOM_EVENT_STATUS1_OFFSET));
        }
        datagram_done(pak);
        break;
    case MCOM_DC_SESSION:
        /*
         * dc session data reply
         */
	bcopy(hdms_msg_ptr->data, &(mpkt->data[0]), hdms_msg_ptr->length);
	mpkt->length = hdms_msg_ptr->length;
        modem_mgmt_enqueue_event(frame_type, &(mpkt->type), modem_info);
        if (MODEM_OOB_DEBUG(modem_info)) {
            buginf("\nMODEM(%d/%d): Modem AT mode data reply.", slot, port);
        }
        datagram_done(pak);
        break;
    case MCOM_REFRESH_EVENT_POLL:
    case MCOM_EVENT_POLL:
        /* 
         * It's a refresh/event poll reply
         */
        if (length == 0) {
            if (MODEM_OOB_DEBUG(modem_info)) {
                buginf("\nMODEM(%d/%d): No status changes since last polled", 
                    slot, port);
            }
        }
        while (length > 0) {
            switch (*data) {
            case MCOM_TX_RX_EVENT:
                /*
                 * rs232 tx/rx event
                 */
                event_type = *data++;
                event_length = *data++;
                if (event_length == 1) {
                  modem_status_info->tx_rx_data = *data;
                  if (MODEM_OOB_DEBUG(modem_info)) {
                      buginf("\nMODEM(%d/%d): RS232 TX/RX event (0x%02x)", slot,
                          port, modem_status_info->tx_rx_data);
                  }
                } else {
                  if (MODEM_OOB_DEBUG(modem_info)) {
                     buginf("\nMODEM(%d/%d): RS232 TX/RX event: %s",
                            slot, port, "Invalid length");
                  }
                }
                break;
            case MCOM_RS232_EVENT:
                /*
                 * rs232 signals event other than tx/rx
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (event_length == 2) {
                    modem_status_info->modem_rs232 = 
                        *(data+MCOM_RS232_EV_OFFSET);
                    modem_status_info->modem_rs232_change = 
                        *(data+MCOM_RS232_EV_CHANGED_OFFSET);
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): RS232 event (0x%02x 0x%02x)",
                            slot, port, modem_status_info->modem_rs232,
                            modem_status_info->modem_rs232_change);
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): RS232 event: %s",
                          slot, port, "Invalid length");
                    }
                }
                break; 
            case MCOM_MODEM_STATE_EVENT:
                /*
                 * modem state event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (event_length == 1) {
                    modem_status_info->modem_state = *data;
                    if (*data == MODEM_STATE_CONNECTED) {
                        if (modem_mgmt_info->modem_in_progress) {
                            if (CSM_IS_OC_STATE(modem_info)) {
                                modem_stats->modem_oc_complete++;
                            } else {
                                modem_stats->modem_ic_complete++;
                            }
                            modem_mgmt_info->modem_in_progress = FALSE;
                        }
                    }
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Modem state event (0x%02x)", 
                        slot, port, modem_status_info->modem_state);
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Modem state event: %s",
                        slot, port, "Invalid length");
                    }
                }
                break; 
            case MCOM_CONNECTION_EVENT:
                /*
                 * connection event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (event_length == 6) {
                    tx_speed_data = *(data+MCOM_TX_SPEED_OFFSET);
                    rx_speed_data = *(data+MCOM_RX_SPEED_OFFSET);
                    if (tx_speed_data  < MAX_MODEM_LINE_SPEEDS) {
                        modem_status_info->tx_speed =
                             modem_speed[tx_speed_data];
                    }
                    if (rx_speed_data < MAX_MODEM_LINE_SPEEDS) {
                        modem_status_info->rx_speed =
                             modem_speed[rx_speed_data];
                    }

                    switch(modem_status_info->rx_speed) {
                        case 75:
                              modem_stats->modem_75_connections++;
                              break;
                        case 300:
                              modem_stats->modem_300_connections++;
                              break;
                        case 600:
                              modem_stats->modem_600_connections++;
                              break;
                        case 1200:
                              modem_stats->modem_1200_connections++;
                              break;
                        case 2400:
                              modem_stats->modem_2400_connections++;
                              break;
                        case 4800:
                              modem_stats->modem_4800_connections++;
                              break;
                        case 7200:
                              modem_stats->modem_7200_connections++;
                              break;
                        case 9600:
                              modem_stats->modem_9600_connections++;
                              break;
                        case 12000:
                              modem_stats->modem_12000_connections++;
                              break;
                        case 14400:
                              modem_stats->modem_14400_connections++;
                              break;
                        case 16800:
                              modem_stats->modem_16800_connections++;
                              break;
                        case 19200:
                              modem_stats->modem_19200_connections++;
                              break;
                        case 21600:
                              modem_stats->modem_21600_connections++;
                              break;
                        case 24000:
                              modem_stats->modem_24000_connections++;
                              break;
                        case 26400:
                              modem_stats->modem_26400_connections++;
                              break;
                        case 28800:
                              modem_stats->modem_28800_connections++;
                              break;
                        case 31200:
                              modem_stats->modem_31200_connections++;
                              break;
                        case 33600:
                              modem_stats->modem_33600_connections++;
                              break;
                        default:
                              errmsg(&msgsym(BADEVENT, MODEM), "Invalid speed",                                                          event_type, slot, port);
                              break;
                    }
 
                    modem_status_info->modulation = 
                        *(data+MCOM_MODULATION_OFFSET);
                    modem_status_info->orig_answer_status = 
                        *(data+MCOM_ORIG_ANSWER_OFFSET);
                    modem_status_info->protocol = 
                        *(data+MCOM_PROTOCOL_OFFSET);
                    modem_status_info->compression =
                        *(data+MCOM_COMPRESSION_OFFSET);
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Connection event "
                            "(0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x)",
                            slot, port,
                            tx_speed_data, rx_speed_data,
                            modem_status_info->modulation,
                            modem_status_info->orig_answer_status,
                            modem_status_info->protocol,
                            modem_status_info->compression); 
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Connection event: %s",
                            slot, port, "Invalid length");
                    }
                }
                break; 
            case MCOM_CONNECTION_UPDATE_EVENT:
                /*
                 * connection update event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (event_length == 3) {
                    tx_speed_data = *(data+MCOM_TX_SPEED_OFFSET);
                    rx_speed_data = *(data+MCOM_RX_SPEED_OFFSET);
                    modem_status_info->tx_speed = modem_speed[tx_speed_data];
                    modem_status_info->rx_speed = modem_speed[rx_speed_data];
                    modem_status_info->modulation = 
                        *(data+MCOM_MODULATION_OFFSET);
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Connection update event "
                            "(0x%02x 0x%02x 0x%02x 0x%02x)",
                            slot, port,
                            tx_speed_data, rx_speed_data,
                            modem_status_info->modulation);
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Connection update event: %s",
                            slot, port, "Invalid length");
                    }
                }
                break;
            case MCOM_CALL_PROGRESS_EVENT:
                /*
                 * call progress event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (event_length == 1) {
                    modem_status_info->call_progress = *data;
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Call progress event (0x%02x)", 
                            slot, port,
                            modem_status_info->call_progress);
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Call progress event: %s", 
                            slot, port, "Invalid length");
                    }
                }
                break;
            case MCOM_DTR_EVENT:
                /*
                 * dtr event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (event_length == 1) {
                    modem_status_info->dtr_status = *data;
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): DTR event (0x%02x)", 
                            slot, port, modem_status_info->dtr_status);
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): DTR event: %s",
                            slot, port, "Invalid length");
                    }
                }
                break;
            case MCOM_MODEM_INACTIVE_EVENT:
                /*
                 * modem inactive event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (event_length == 1) {
                    modem_status_info->inactivity_status = *data;
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Inactive event (0x%02x)",
                            slot,port, modem_status_info->inactivity_status);
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Inactive event: %s",
                            slot, port, "Invalid length");
                    }
                }
                break;
            case MCOM_ANALOG_REPORT_EVENT:
                /*
                 * analog signal report event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (event_length == 3) {
                    modem_status_info->tx_level = *(data+MCOM_TX_LEVEL_OFFSET);
                    modem_status_info->rx_level = *(data+MCOM_RX_LEVEL_OFFSET);
                    modem_status_info->signal_noise_ratio = 
                        *(data+MCOM_SIGNAL_TO_NOISE_OFFSET);
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Analog signal report event "
                          "(0x%02x 0x%02x 0x%02x)", 
                          slot, port,
                          modem_status_info->tx_level,
                          modem_status_info->rx_level,
                          modem_status_info->signal_noise_ratio);
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Analog signal report event: %s",
                            slot, port, "Invalid length");
                    }
                }
                break;
            case MCOM_END_CONNECTION_EVENT:
                /*
                 * end connection event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (event_length == 15) {
                    modem_status_info->tx_retransmits = 
                        *(data+MCOM_RETRANSMITS_TX_OFFSET);
                    modem_status_info->rx_retransmits =
                        *(data+MCOM_RETRANSMITS_RX_OFFSET);
                    modem_status_info->duration_hour = 
                        *(data+MCOM_DURATION_HOURS_OFFSET);
                    modem_status_info->duration_min = 
                        *(data+MCOM_DURATION_MINUTES_OFFSET);
                    modem_status_info->duration_sec =
                        *(data+MCOM_DURATION_SECONDS_OFFSET);
                    modem_status_info->xmit_count += 
                        GETLONG(data+MCOM_NUM_TX_CHARACTERS_OFFSET);
                    modem_status_info->rcv_count += 
                        GETLONG(data+MCOM_NUM_RX_CHARACTERS_OFFSET);
                    modem_status_info->disconnect_reason_local =
                        *(data+MCOM_DISC_REASON_LOCAL_OFFSET);
                    modem_status_info->disconnect_reason_remote =
                        *(data+MCOM_DISC_REASON_REMOTE_OFFSET);

                    switch (modem_status_info->disconnect_reason_local) {
                        case DISC_NO_CARRIER:
                            modem_stats->modem_no_carriers++;
                            break;
                        case DISC_NO_DIALTONE:
                            modem_stats->modem_no_dial_tones++;
                            break;
                        case DISC_LINK_FAILURE:
                            modem_stats->modem_link_failures++;
                            break;
                        case DISC_DIAL_TIMEOUT:
                            modem_stats->modem_dial_timeouts++;
                            break;
                        default:
                            /* 
                             * do nothing for now for other fail reasons
                             */
                            break;
                    }

                    if (MODEM_OOB_DEBUG(modem_info)) {
                      buginf("\nMODEM(%d/%d): End connection event (0x%02x "
                          "0x%02x 0x%02x 0x%02x 0x%02x 0x%08x 0x%08x "
                          "0x%02x 0x%02x)", 
                          slot, port, 
                          modem_status_info->tx_retransmits,
                          modem_status_info->rx_retransmits,
                          modem_status_info->duration_hour,
                          modem_status_info->duration_min,
                          modem_status_info->duration_sec,
                          modem_status_info->xmit_count,
                          modem_status_info->rcv_count,
                          modem_status_info->disconnect_reason_local,
                          modem_status_info->disconnect_reason_remote);
                    }
                    if (TRACE_ENABLE) {
                        modem_mgmt_upload_trace_to_syslog(slot, port);
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): End connection event: %s",
                            slot, port, "Invalid length");
                    }
                }
                break;
            case MCOM_PHONE_NUMBER_EVENT:
                /*
                 * phone number event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (event_length <= MAX_MODEM_STRING_LEN) {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Phone number event (%s)", 
                            slot, port, data);
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Phone number event: %s",
                            slot, port, "Invalid length");
                    }
                }
                /* 
                 * do nothing with the string for now 
                 */
                break;
            case MCOM_CALLER_ID_EVENT:
                /*
                 * caller id event
                 */
                /*
                 * Do not enqueue this event now, until we figure what
                 * we want to do with this.
                 *
                 * modem_mgmt_enqueue_event(frame_type, data, modem_info);
                 */
 
                event_type = *data++;
                event_length = *data++;
                if (event_length <= MAX_MODEM_STRING_LEN) {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Caller ID event (%s)",
                            slot, port, data);
                    }
                } else {
                    if (MODEM_OOB_DEBUG(modem_info)) {
                        buginf("\nMODEM(%d/%d): Caller ID event: %s",
                            slot, port, "Invalid length");
                    }
                }
                /* 
                 * do nothing with the string for now 
                 */
                break;
            case MCOM_NO_RESPONSE_EVENT:
                /*
                 * no response event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (MODEM_OOB_DEBUG(modem_info)) {
                    buginf("\nMODEM(%d/%d): No response from poll event 0x%02x",
                        slot, port, *data++);
                }
                break;
            case MCOM_EVENT_LOST_EVENT:
                /*
                 * lost event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (MODEM_OOB_DEBUG(modem_info)) {
                    buginf("\nMODEM(%d/%d): oob port modem events lost",
                        slot, port);
                }
                break;
            case MCOM_PROTOCOL_ERR_EVENT:
                /*
                 * protocol error event
                 */
                modem_mgmt_enqueue_event(frame_type, data, modem_info);
                event_type = *data++;
                event_length = *data++;
                if (MODEM_OOB_DEBUG(modem_info)) {
                    buginf("\nMODEM(%d/%d): Protocol error event", 
                        slot, port);
                }
                break;
            default:
                if (!bad_modem_count) {
                   errmsg(&msgsym(BADEVENT, MODEM), "status poll", 
                          *data, slot, port);
                }
                bad_modem_count++; 
                /* 
                 * In case we get MAX_MODEM_BAD_EVENT_COUNT bad events 
                 * then we will  go ahead and do a reset of oobp
                 */
                if (bad_modem_count > MAX_MODEM_BAD_EVENT_COUNT) {
                   modem_mgmt_info->state = MCOM_STARTUP_STATE;
                   modem_mgmt_info->mgmt_port_status
                     = modem_mgmt_info->mgmt_port_status &
                                 (~MODEM_STATUS_POLL_RESP_OK);
                   errmsg(&msgsym(MODEMPOLLOFF, MODEM), 
                          *data, slot, port);
                   length = 0;
                   continue;
                } else {
                    data++;
                    event_length = *data++;
                }
                break;
            }
            /* 
             * move data point to next event
             */
            data += event_length;
            length -= (event_length + 2);
        }
        datagram_done(pak);
        break;
    case MCOM_DC_SESSION_EVENT_POLL:
        /*
         * DC Data Session Poll Reply
         */
        modem_mgmt_enqueue_event(frame_type, &(mpkt->type), modem_info);

        pak->datagramsize = (ushort) mpkt->length;
        pak->datagramstart = mpkt->data;
 
        if (pak->datagramsize > 0) {
            enqueue(&(modem_info->modem_mgmt_info->dc.mgmt_at_frommodemQ), pak);
            process_set_boolean(at_response_ready, TRUE);
        }
 
        if (MODEM_OOB_DEBUG(modem_info)) {
            tmp_buff = chunk_malloc(modem_large_event_chunk); 
            if (tmp_buff) {
                strncpy(tmp_buff, pak->datagramstart, pak->datagramsize);
                /* add a null terminate to the string */
                tmp_buff[pak->datagramsize] = '\0';
                buginf("\nMODEM(%d/%d): AT mode event = %s", 
                       slot, port, tmp_buff);
                chunk_free(modem_large_event_chunk, tmp_buff);
            }
        }
        break;
    default:
        /*
         * We shouldn't get anything to fall into here 
         */
        datagram_done(pak);
        errmsg(&msgsym(BADEVENT, MODEM), "out of band", frame_type, slot, port);
        break;
    }
}

/*
 * =====================================================================
 * modem_mgmt_csm_event_handler()
 *
 * Description:
 * Handle a csm event by getting an event buffer from the free queue
 * and enqueuing the event to a watched queue 
 *
 * Parameters:
 * type - type of event 
 * modem_info - pointer to modem table of a modem to add the event 
 *
 * Returns:
 * None
 * =====================================================================
 */
void modem_mgmt_csm_event_handler (uint type, modem_info_t *modem_info) 
{
    modem_q_info_t *event = NULL;

    event = dequeue(modem_csm_event_freeQ);
    event->modem_info = modem_info;
    event->type = type;
    process_enqueue(modem_csm_eventQ, event);
}

/*
 * =====================================================================
 * modem_mgmt_csm_event_process()
 *
 * Description:
 * Process sleep until there is something in the modem event queue 
 * from other subsystems (i.e. csm) and build a valid event frame to 
 * log into modem event queue.
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */
void modem_mgmt_csm_event_process (void)
{
    modem_mgmt_pkt_hdr_t *mpkt;
    modem_q_info_t *event;
    modem_mgmt_info_t *modem_mgmt_info;

    process_watch_queue(modem_csm_eventQ, ENABLE, RECURRING);

    while (1) {
      process_wait_for_event();
      while ((event = (modem_q_info_t *)process_dequeue(modem_csm_eventQ))) {
        if (!event->modem_info) {
            event->type = 0;
            enqueue(modem_csm_event_freeQ, event);
            continue;
        }

        switch (event->type) {
        case CSM_ELOG_PHONE_NUMBER_COLLECTED:
            /*
             * generate an event packet and enqueue it to the modem event queue
             */
            mpkt = chunk_malloc(modem_large_event_chunk);
            if (!mpkt) {
                event->type = 0;
                enqueue(modem_csm_event_freeQ, event);
                continue;
            }
            if (CSM_IS_OC_STATE(event->modem_info)) {
                mpkt->type = CSM_CALLED_NUMBER;
                mpkt->length = strlen(event->modem_info->called_party_num);
                strcpy(mpkt->data, event->modem_info->called_party_num);
                /*
                 * save a copy of the outgoing call number
                 */
                bcopy(event->modem_info->called_party_num, 
                  event->modem_info->last_called_number,MAX_PHONE_NUM_LEN);
                event->modem_info->last_call_direction = CALL_ORIGINATE;
                if (MODEM_OOB_DEBUG(event->modem_info)) {
                  buginf("\nMODEM(%d/%d): ISDN outgoing called number: %s", 
                      CSM_MODEM_SLOT(event->modem_info), 
                      CSM_MODEM_PORT(event->modem_info), 
                      mpkt->length?
                      (char *)event->modem_info->last_called_number:
                      "Unknown");
                }
                /*
                 * if managed modems and polling is enabled and operational,
                 * increment total outgoing call attempts
                 */
                modem_mgmt_info = event->modem_info->modem_mgmt_info;
                if ((event->modem_info->modem_config & STATUS_POLL_ALLOWED) &&
                    (modem_mgmt_info) && 
                    (modem_mgmt_info->mgmt_port_status & STATUS_POLL_ALLOWED)) {
                    modem_mgmt_info->modem_stats.modem_oc_total++;
                    modem_mgmt_info->modem_in_progress = TRUE;
                }
            } else {
                mpkt->type = CSM_CALLER_NUMBER;
                mpkt->length = strlen(event->modem_info->calling_party_num);
                strcpy(mpkt->data, event->modem_info->calling_party_num);
                /*
                 * save a copy of the incoming caller id 
                 */
                bcopy(event->modem_info->calling_party_num, 
                  event->modem_info->last_calling_number,MAX_PHONE_NUM_LEN);
                event->modem_info->last_call_direction = CALL_ANSWER;
                if (MODEM_OOB_DEBUG(event->modem_info)) {
                  buginf("\nMODEM(%d/%d): ISDN incoming caller number: %s", 
                      CSM_MODEM_SLOT(event->modem_info), 
                      CSM_MODEM_PORT(event->modem_info), 
                      mpkt->length? 
                      (char *)event->modem_info->last_calling_number:
                      "Unknown");
                }
                /*
                 * if managed modems and polling is enabled and operational,
                 * increment total outgoing call attempts
                 */
                modem_mgmt_info = event->modem_info->modem_mgmt_info;
                if ((event->modem_info->modem_config & STATUS_POLL_ALLOWED) &&
                    (modem_mgmt_info) && 
                    (modem_mgmt_info->mgmt_port_status & STATUS_POLL_ALLOWED)) {
                    modem_mgmt_info->modem_stats.modem_ic_total++;
                    modem_mgmt_info->modem_in_progress = TRUE;
                }   
            }
        
            modem_mgmt_enqueue_event(mpkt->type, &(mpkt->type), 
                                     event->modem_info);
            chunk_free(modem_large_event_chunk, mpkt);
            break;
        default:
            break;   
        }
       
        /*
         * now clear this event and put it back in the free queue
         */
        event->modem_info = NULL;
        event->type = 0;
        enqueue(modem_csm_event_freeQ, event);
      }
    }
}

/*
 * =====================================================================
 * modem_mgmt_group_created()
 *
 * Description:
 * Whenever an interface group-async is configured, an idb is created.
 * Store the IDB so that we query for the available modem groups faster
 * than traversing all the HWIDB's.
 *
 * Parameters:
 * hwidbtype - ptr to the group-async idb
 *
 * Returns:
 * None
 * =====================================================================
 */
void modem_mgmt_group_created (hwidbtype *hwidb)
{
    modem_group_t *new_group, *current;

    if (hwidb == NULL) 
        return;
    
    new_group = malloc(sizeof(modem_group_t));
    if (!new_group) {
        return;
    }

    if (!modem_group_list) {
        /*
         * this is the first modem group
         */
        modem_group_list = new_group;
        modem_group_list->hwidb = hwidb;
        modem_group_list->next = NULL;
    } else {
        /*
         * add group-async hwidb to existing hwidb list in sequential order
         */
        new_group->hwidb = hwidb;

        if (hwidb->unit < modem_group_list->hwidb->unit) {
           new_group->next = modem_group_list;
           modem_group_list = new_group;
        } else {
            current = modem_group_list;
            while ((current->next) && 
                   (new_group->hwidb->unit > current->next->hwidb->unit)) {
                current = current->next;
            }
            new_group->next = current->next;
            current->next = new_group;
        }
    }
}

/*
 * =====================================================================
 * modem_mgmt_init_mcom() 
 *
 * Description
 * Init. HDMS related fields (variables) in the modem tables to 
 * default values.
 *
 * Parameters:
 * modem_info -- pointer points to a modem table
 *
 * Returns:
 * None
 * =====================================================================
 */

static void modem_mgmt_init_mcom (modem_info_t *modem_info)
{
   modem_mgmt_info_t *ptr;

   ptr = modem_info->modem_mgmt_info;

   ptr->state = MCOM_STARTUP_STATE;
   ptr->mgmt_port_status = MODEM_STATUS_POLL_RESP_OK | 
                           MODEM_DC_SESSION_RESP_OK;
   ptr->clear_dc = CLEARED_ALLOW_NEW_DC;
   ptr->modem_in_progress = FALSE;
   ptr->sequence_num = 0;
   ptr->dc_session_flg = FALSE;
   ptr->dc.dc_state = MCOM_DC_SEND_STATE;
   queue_init(&(ptr->dc.mgmt_at_tomodemQ), 0);
   queue_init(&(ptr->dc.mgmt_at_frommodemQ), 0);
   modem_info->last_call_direction = CALL_NONE_YET;
}

/*
 * =====================================================================
 * modem_mgmt_init_mcom_statistic() 
 *
 * Description
 * Init. statistic fields (counters) in the modem table.
 *
 * Parameters:
 * modem_info -- pointer points to a modem table
 *
 * Returns:
 * None
 * =====================================================================
 */

static void modem_mgmt_init_mcom_statistic (modem_info_t *modem_info)
{
   modem_mgmt_info_t *ptr;

   ptr = modem_info->modem_mgmt_info;
      
   if (ptr != NULL) {
      bzero(&(ptr->modem_stats), sizeof(modem_statistics_t));
   }
}

/*
 * =====================================================================
 * void modem_mgmt_fake_a_packet()
 *
 * Description
 * This procedure will 
 * 1. generate debug message if needed.
 * 2. fask a receiving reply packet and sends it to the event_handker().
 *
 * Parameters:
 * err_event -- error event needs be generated
 * modem_info -- pointer points to modem table
 * 
 * Returns:
 * None
 *
 * =====================================================================
 */

static void modem_mgmt_fake_a_packet(uchar err_event, 
                                     modem_info_t *modem_info)
{
   paktype *err_pak_ptr;
   modem_mgmt_pkt_hdr_t *hdms_reply_err_ptr;
   uchar *data_ptr;
   ushort slot_num, modem_num;

   slot_num = CSM_MODEM_SLOT(modem_info);
   modem_num = CSM_MODEM_PORT(modem_info);

   /* debuginfo */
   if (MODEM_OOB_DEBUG(modem_info)) {
      switch (err_event) {
         case MCOM_PROTOCOL_ERR_EVENT:
            buginf("\nMODEM(%d/%d): oob port protocol error",
                    slot_num, modem_num);
            break;
         case MCOM_WRONG_SEQ_NUM_EVENT:
            buginf("\nMODEM(%d/%d): oob port sequence number error",
                    slot_num, modem_num);
            break;
         case MCOM_UNEXPECTED_EVENT:
            buginf("\nMODEM(%d/%d): oob port unexpected event received",
                    slot_num, modem_num);
            break;
         case MCOM_NO_RESPONSE_EVENT:
            buginf("\nMODEM(%d/%d): oob port modem reply timeout error",
                    slot_num, modem_num);
            break;
         case MCOM_EVENT_LOST_EVENT:
            buginf("\nMODEM(%d/%d): oob port modem events lost",
                    slot_num, modem_num);
            break;
         default:
            ;
      } /* switch */
    }

   err_pak_ptr = getbuffer(MAX_MCOM_MESSAGE_SIZE);
   if (err_pak_ptr==NULL) {
      return;
   }

   hdms_reply_err_ptr= (modem_mgmt_pkt_hdr_t *)
                                      MODEM_MGMT_HEADSTART(err_pak_ptr);

   bcopy(hdms_msg_ptr, hdms_reply_err_ptr, sizeof(modem_mgmt_pkt_hdr_t));
   hdms_reply_err_ptr->type = MCOM_EVENT_POLL; 
   hdms_reply_err_ptr->length = 3;
   data_ptr = hdms_reply_err_ptr->data;
   /* following is event type */
   *data_ptr++ = err_event;
   /* following is event length */
   *data_ptr++ = 1;
   if (err_event == MCOM_NO_RESPONSE_EVENT) { 
      /* which hdms message type cause the timeout */
      *data_ptr = hdms_msg_ptr->type;
      hdms_reply_err_ptr->modem_address = hdms_msg_ptr->modem_address;
      hdms_reply_err_ptr->c_field = hdms_msg_ptr->c_field;
   }
   else {
      *data_ptr = hdms_reply_ptr->type;
      hdms_reply_err_ptr->modem_address = hdms_reply_ptr->modem_address;
      hdms_reply_err_ptr->c_field = hdms_reply_ptr->c_field;
   }
   modem_mgmt_event_handler (err_pak_ptr, modem_info);

} 
/*
 * =====================================================================
 * modem_mgmt_send_and_wait_msg()
 *
 * Description
 * This procedure is responsible to send out one msg to the targeted modem, 
 * waiting for the reply from the modem and analyze the reply.
 *
 * Parameters:
 * modem_info -- pointer points to modem table
 * 
 * Returns:
 * event -- function call return value
 *
 * =====================================================================
 */

static int modem_mgmt_send_and_wait_msg (modem_info_t *modem_info)
{
   uchar sequence_num;
   uchar retry_count;
   boolean  loop_flag;
   ulong  reason, wake_up_reason,  not_used;
   boolean more_data_flag;
   modem_mgmt_info_t *modem_mgmt_info;
   int return_code,  output;
   ushort slot_num, modem_num;
 
   slot_num = CSM_MODEM_SLOT(modem_info);
   modem_num = CSM_MODEM_PORT(modem_info);

   modem_mgmt_info = modem_info->modem_mgmt_info;
   sequence_num = (uchar) (hdms_msg_ptr->c_field & MCOM_SEQ_NUM_MASK);

   /* retry count = # of rerety + 1st time message sent */
   retry_count = modem_poll_retry + 1;

   loop_flag = TRUE;
   output = MCOM_EV_UNKNOWN;
   process_watch_boolean(hdms_reply_ready, ENABLE, RECURRING);

   while ((retry_count-- > 0) && (loop_flag)) {

      if (MODEM_OOB_DEBUG(modem_info)) {
          buginf("\nMODEM(%d/%d): 1 msg sent (Msg type=%x, Seq #=%d)",
                 slot_num, modem_num, hdms_msg_ptr->type,
                 hdms_msg_ptr->c_field& MCOM_SEQ_NUM_MASK);
      }
      send_pak_ptr->datagramsize = hdms_msg_ptr->length 
                                         +MODEM_MGMT_OVERHEADBYTES;

      process_set_boolean(hdms_reply_ready, FALSE);
      return_code= modem_oob_request_send(modem_info, 
                              send_pak_ptr,
                              &receive_pak_ptr, 
                              &hdms_reply_status, 
                              &hdms_reply_ready);
      if (return_code == MGMT_FAIL) { 
         if (MODEM_OOB_DEBUG(modem_info)) {
             buginf("\nMODEM(%d/%d): Send request failed (Err code"
                    "=%d, Retry counter=%d)",
                     slot_num, modem_num, return_code, retry_count);
         }

         modem_oob_request_reset(modem_info);
         output = MCOM_EV_TIME_OUT;
         process_sleep_for(MODEM_MGMT_REST_TIME_100_MS);
         continue;
      }

      process_wait_for_event_timed(MCOM_HDMS_TIME_OUT_VALUE); 

      /* init */
      reason = ALL_EVENTS;
 
      /* find out the reason of process wakeup*/
      while (process_get_wakeup(&wake_up_reason, &not_used)) {
         switch (wake_up_reason) {
            case BOOLEAN_EVENT:
               process_set_boolean(hdms_reply_ready, FALSE);
               reason = BOOLEAN_EVENT;
               break;
            default:
               /* time out */
               process_set_boolean(hdms_reply_ready, FALSE);
               if (reason != BOOLEAN_EVENT) {
                  reason = TIMER_EVENT;
               } 
               break;
         }
      }   

      if (reason == BOOLEAN_EVENT) {
         if (hdms_reply_status == MGMT_STATUS_GOOD_DATA) {
            hdms_reply_ptr= (modem_mgmt_pkt_hdr_t *)
                                         MODEM_MGMT_HEADSTART(receive_pak_ptr);

            if ((hdms_msg_ptr->type == hdms_reply_ptr->type) &&
                (hdms_msg_ptr->modem_address == hdms_reply_ptr->modem_address)
               ) {
               if (sequence_num != 
                   (hdms_reply_ptr->c_field&MCOM_SEQ_NUM_MASK)) { 
                  /* sequence # wrong and protocl error can both occur at same 
                   * time, but we decide to choose sequence # wrong first 
                   * because there is a recover mechnism implemented for it                        */
                  modem_mgmt_info->modem_stats.modem_protocol_errors++;
                  output = MCOM_EV_WRONG_SEQ_NUM;
                  modem_mgmt_fake_a_packet(MCOM_NO_RESPONSE_EVENT, modem_info); 
 
                  /* if this is a DC session command, after recording error,  */
                  /* reset the sequence number to let the DC session continue */
                  if ((hdms_msg_ptr->type == MCOM_DC_SESSION) ||
                      (hdms_msg_ptr->type == MCOM_DC_SESSION_EVENT_POLL)) {
                     modem_mgmt_info->sequence_num = 
                          (hdms_reply_ptr->c_field&MCOM_SEQ_NUM_MASK) + 1;
                     hdms_msg_ptr->c_field = modem_mgmt_info->sequence_num
                                       & MCOM_SEQ_NUM_MASK;
                     sequence_num = (uchar) 
                                  (hdms_msg_ptr->c_field & MCOM_SEQ_NUM_MASK);
                  }
               }
               else {

                  /* it's the expected reply! */
                  loop_flag = FALSE;

                  if (hdms_reply_ptr->c_field & MCOM_MORE_DATA) {
                     more_data_flag = TRUE;
                  }
                  else {
                     more_data_flag = FALSE;
                  }

                  if (hdms_reply_ptr->c_field & MCOM_EVENT_LOST) {
                     modem_mgmt_info->modem_stats.modem_lost_events++;
                     modem_mgmt_fake_a_packet(MCOM_EVENT_LOST_EVENT, 
                                              modem_info); 
                  }

                  if (hdms_reply_ptr->c_field & MCOM_PROTOCOL_ERR) {
                     modem_mgmt_info->modem_stats.modem_protocol_errors++;
                     output = MCOM_EV_PROTOCOL_ERR;
                     modem_mgmt_fake_a_packet(MCOM_PROTOCOL_ERR_EVENT, 
                                              modem_info); 
                  }
                  else if ((hdms_msg_ptr->type == MCOM_EVENT_POLL) ||
                            (hdms_msg_ptr->type == MCOM_REFRESH_EVENT_POLL ) ) {
                     if (more_data_flag) {
                        output = MCOM_EV_STA_POLL_MORE_DATA;
                     }
                     else if (hdms_reply_ptr->length == 0) {
                        output = MCOM_EV_STA_POLL_NO_DATA;
                     }
                     else {
                        output = MCOM_EV_STA_POLL;
                     }
                  }
                  else if (hdms_msg_ptr->type == MCOM_DC_SESSION_EVENT_POLL) {
                     if ((!more_data_flag) && (hdms_reply_ptr->length == 0)) {
                        output = MCOM_EV_DC_POLL_BAD_AT;
                     }
                     else if ((!more_data_flag) && 
                              (hdms_reply_ptr->length > 0)) {
                        output = MCOM_EV_DC_POLL;
                     }
                     else if ((more_data_flag) && 
                              (hdms_reply_ptr->length == 0)) {
                        output = MCOM_EV_DC_POLL_NO_DATA;
                     }
                     else { 
                        /* it is: 
                         * ((more_data_flag) && (hdms_reply_ptr->length > 0)) 
                         */
                        output = MCOM_EV_DC_POLL_MORE_DATA;
                     }
                  }
                  else {
                     output = hdms_reply_ptr->type;
                  } 
                  if (sequence_num == 
                             (hdms_reply_ptr->c_field&MCOM_SEQ_NUM_MASK)) {  
                     modem_mgmt_info->sequence_num = (sequence_num + 1) 
                                                     % 16;
                  }
               } /* end of expected reply */

               /* log events if it's expected */
               modem_mgmt_event_handler(receive_pak_ptr, modem_info); 
            }
            else {
               /* It's a valid but unwanted REPLY -- either is a 
                * unexpected type or unexpected modem address. 
                * Trash it and try again.
                */
               if (MODEM_OOB_DEBUG(modem_info)) {
                  buginf("\nMODEM(%d/%d): Unexpected reply received",
                         slot_num, modem_num);
               }
               output = MCOM_EV_UNKNOWN;
            }
       

         } /* if (hdms_reply_status == MGMT_STATUS_GOOD_DATA)*/
         else {
            /* hdms_reply_status indicates error */
            /* at phase one, we treat this error as time out error*/
            /* sleep 100 ms to give cpu a break */
            if (MODEM_OOB_DEBUG(modem_info)) {
               buginf("\nMODEM(%d/%d): Async driver reported oob reply error",
                      slot_num, modem_num);
            }
            modem_mgmt_info->modem_stats.modem_timeouts++;
            output = MCOM_EV_TIME_OUT;
            modem_mgmt_fake_a_packet(MCOM_NO_RESPONSE_EVENT, modem_info); 
            process_sleep_for(MODEM_MGMT_REST_TIME_100_MS);
         }
      } 
      else {
         /*reason == TIMER_EVENT */
         modem_oob_request_reset(modem_info);

         modem_mgmt_info->modem_stats.modem_timeouts++;
         /* buginf will be reported by modem_mgmt_fake_a_packet() */
         modem_mgmt_fake_a_packet(MCOM_NO_RESPONSE_EVENT, modem_info);
         output = MCOM_EV_TIME_OUT;
      }  

  } /* end of while */

  return (output);
}

/*
 * =====================================================================
 * modem_mgmt_get_modem_firmware_version()
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
static inline uint modem_mgmt_get_modem_firmware_version 
                                               (modem_info_t *modem_info)
{
    if (modem_info->modem_mgmt_info) {
        uchar *ch;

        ch =  
          modem_info->modem_mgmt_info->modem_status_info.modem_firmware_rev;
        return ((((uint)*ch++)<<16) + (((uint)*ch++)<<8) + *ch);
    } else
        return (0);
}
 
/*
 * =====================================================================
 * modem_mgmt_is_newer_modem_firmware()
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
static inline boolean modem_mgmt_is_newer_modem_firmware 
                        (modem_info_t *modem_info, char *version)
{
    if (modem_info->modem_mgmt_info) {
        uchar *ch1, *ch2;
        ch1 = 
          modem_info->modem_mgmt_info->modem_status_info.modem_firmware_rev;
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
 * modem_mgmt_clear_at_in_out_queues()
 *
 * Description
 * To clear modem AT in/output queues.
 *
 * Parameters:
 * modem_mgmt_info_t *modem_mgmt_info
 *
 * Returns:
 * None
 *
 * =====================================================================
 */

static void modem_mgmt_clear_at_in_out_queues(modem_mgmt_info_t 
                                                   *modem_mgmt_info)
{
   nexthelper *to_modem_pak;
   paktype *from_modem_pak;

   /* empty both AT input/output queues */
   to_modem_pak = dequeue(&(modem_mgmt_info->dc.mgmt_at_tomodemQ));
   while (to_modem_pak != NULL) {
      free(to_modem_pak);
      to_modem_pak = dequeue(&(modem_mgmt_info->dc.mgmt_at_tomodemQ));
   }

   from_modem_pak = dequeue(&(modem_mgmt_info->dc.mgmt_at_frommodemQ));
   while (from_modem_pak != NULL) {
      datagram_done(from_modem_pak);
      from_modem_pak = dequeue(&(modem_mgmt_info->dc.mgmt_at_frommodemQ));
   }
}

/*
 * =====================================================================
 * modem_mgmt_terminate_dc_session()
 *
 * Description   
 * To terminate one DC session --
 * 1. empty at in/out queues.
 * 2. delete AT block.
 *
 * Parameters:   
 * slot_number  -- modem card slot number
 * modem_number -- modem number
 *
 * Returns:
 * None   
 *
 * =====================================================================
 */
static void modem_mgmt_terminate_dc_session(ushort slot_num,
                                            ushort modem_num)
{
  modem_mgmt_at_mode_block *at_mode_block, *prev;
  ushort slot_modem;
  modem_info_t *modem_info;

  slot_modem = slot_num << 8 | (modem_num & 0xff);
  
  prev = NULL;   
  for (at_mode_block = modem_mgmt_active_at_mode_blocks;
       (at_mode_block != NULL);
       at_mode_block = at_mode_block->next) {
     if (at_mode_block->slot_modem == slot_modem) {
 
        if (prev == NULL) {
           modem_mgmt_active_at_mode_blocks = at_mode_block->next;
        }     
        else {
           prev->next = at_mode_block->next;
        }    
 
        if (modem_mgmt_current_at_mode_block == at_mode_block) {
           if (at_mode_block->next == NULL) {
               modem_mgmt_current_at_mode_block =
                                    modem_mgmt_active_at_mode_blocks;
           }
           else {
              modem_mgmt_current_at_mode_block = at_mode_block->next;
           }
              
           if (modem_mgmt_active_at_mode_blocks == NULL) {
              modem_mgmt_current_at_mode_block = NULL;
              /* No more DC session in the system now, */
              /* send boolen event to at mode reader to terminate process*/
              process_set_boolean(at_response_ready, TRUE);
           }
        }     
 
        free(at_mode_block);
 
        modem_info = CSM_MODEM_INFO_OR_NULL(slot_num, modem_num);
 
        if (MODEM_OOB_DEBUG(modem_info)) {
           buginf("\nMODEM(%d/%d): DC session closed", slot_num, modem_num);
        }     
 
        modem_mgmt_clear_at_in_out_queues (modem_info->modem_mgmt_info);
        return;
    } 
    if (at_mode_block->slot_modem > slot_modem) {
       break;
    }
    prev = at_mode_block;
     
  }
} 

/*
 * =====================================================================
 * modem_mgmt_status_state_machine()
 *
 * Description
 * This procedure is the state machine responsible for modem status polling,
 * it is called by modem_mgmt_main() and handles one modem at a time.
 *
 * This status polling state machine is designed based on
 * "Microcom Asynchronous Backplane Protocol Specification".
 * For more detail information, please refer to that Specification
 * and "AS5200 modem management software design specification".
 *
 * Parameters:
 * modem_info_t -- pointer points to one modem table
 *
 * Returns:
 * None 
 *
 * =====================================================================
 */
static void modem_mgmt_status_state_machine (modem_info_t *modem_info)
{
   boolean continue_flag;
   int event;
   modem_mgmt_info_t *modem_mgmt_info;
   char *char_ptr;
   ushort slot_num, modem_num;
 
   slot_num = CSM_MODEM_SLOT(modem_info);
   modem_num = CSM_MODEM_PORT(modem_info);

   if (modem_info->initialized == FALSE) {
      if (MODEM_OOB_DEBUG(modem_info)) {
          buginf("\nMODEM(%d/%d): No status polling"
                 " (no oob modem address)", slot_num, modem_num);
      }
      return;
   }
   continue_flag = TRUE;
   event = MCOM_EV_ROUND_ROBIN;
   modem_mgmt_info = modem_info->modem_mgmt_info;

   while (continue_flag) {
      hdms_msg_ptr->type = MCOM_NOT_USED;
      hdms_msg_ptr->length = MCOM_NO_DATA; 

      switch (modem_mgmt_info->state) {
         case MCOM_STARTUP_STATE:
	    switch (event) {
	       case MCOM_EV_STARTUP:
                  /* After the Start Up message, seq # needs to 
                   * start from ZERO 
                   */
                  modem_mgmt_info->sequence_num = 0;

	          modem_mgmt_info->state = MCOM_STARTUP_REPLY_STATE;
		  hdms_msg_ptr->type = MCOM_CONTROL;
		  hdms_msg_ptr->length = MCOM_LEN_CONTROL; 
                  char_ptr = (char *)&(hdms_msg_ptr->data[0]);
		  *char_ptr++ = MCOM_EVENT_SELECT_BITS_0; 
		  *char_ptr = MCOM_EVENT_SELECT_BITS_1; 
	 	  break; 
	       case MCOM_EV_TIME_OUT:
		  modem_mgmt_info->state = MCOM_NO_RESPONSE_STATE;
	          break;
	       case MCOM_EV_ROUND_ROBIN:
		  modem_mgmt_info->sequence_num = 0;
		  hdms_msg_ptr->type = MCOM_STARTUP;
		  break;
               case MCOM_EV_PROTOCOL_ERR:
                  modem_mgmt_info->state = MCOM_CLEAN_UP_STATE;
		  break;
               case MCOM_EV_WRONG_SEQ_NUM:
                  modem_mgmt_info->state = MCOM_STARTUP_STATE;
                  continue_flag = FALSE;
                  break;
               default:
                  modem_mgmt_info->state = MCOM_CLEAN_UP_STATE;
                  errmsg(&msgsym(UNEXPECTED_STATUS_EV, MODEM), 
                         event, slot_num, modem_num);
                  break;
            }
            break;

	 case MCOM_STARTUP_REPLY_STATE:
	    switch (event) {
               case MCOM_EV_CONTROL:
	          modem_mgmt_info->state = MCOM_STATUS_POLL_STATE;
		  hdms_msg_ptr->type = MCOM_REFRESH_EVENT_POLL;

                  hdms_msg_ptr->length = MCOM_LEN_CONTROL;
                  char_ptr = (char *)&(hdms_msg_ptr->data[0]);
                  *char_ptr++ = MCOM_EVENT_SELECT_BITS_0;
                  *char_ptr = MCOM_EVENT_SELECT_BITS_1;
	          break;
               case MCOM_EV_TIME_OUT:
		  modem_mgmt_info->state = MCOM_NO_RESPONSE_STATE;
		  break;
               case MCOM_EV_PROTOCOL_ERR:
	          modem_mgmt_info->state = MCOM_CLEAN_UP_STATE;
		  break;
               case MCOM_EV_WRONG_SEQ_NUM: 
                  modem_mgmt_info->state = MCOM_STARTUP_STATE; 
                  continue_flag = FALSE; 
                  break;
               default:
	          modem_mgmt_info->state = MCOM_CLEAN_UP_STATE;
                  errmsg(&msgsym(UNEXPECTED_STATUS_EV, MODEM), 
                         event, slot_num, modem_num);
		  break;
             }
             break;

	 case MCOM_STATUS_POLL_STATE:
	    switch (event) {
	       case MCOM_EV_STA_POLL:
		  continue_flag = FALSE;
	          break;
	       case MCOM_EV_STA_POLL_NO_DATA:
	          continue_flag = FALSE;
	          break;
	       case MCOM_EV_STA_POLL_MORE_DATA:
                  hdms_msg_ptr->type = MCOM_EVENT_POLL;
	          break;
	       case MCOM_EV_ROUND_ROBIN:
                  hdms_msg_ptr->type = MCOM_EVENT_POLL;
	          break;
	       case MCOM_EV_TIME_OUT:
		  modem_mgmt_info->state = MCOM_NO_RESPONSE_STATE;
	          break;
               case MCOM_EV_PROTOCOL_ERR:
	          modem_mgmt_info->state = MCOM_CLEAN_UP_STATE;
	          break;
               case MCOM_EV_WRONG_SEQ_NUM: 
                  modem_mgmt_info->state = MCOM_STARTUP_STATE; 
                  continue_flag = FALSE; 
                  break;
               default:
	          modem_mgmt_info->state = MCOM_CLEAN_UP_STATE;
                  errmsg(&msgsym(UNEXPECTED_STATUS_EV, MODEM), 
                         event, slot_num, modem_num);
		  break;
            }
            break;

	case MCOM_CLEAN_UP_STATE:
           if (MODEM_OOB_DEBUG(modem_info)) {
               buginf("\nMODEM(%d/%d): Status polling enters clean up state",
                       slot_num, modem_num);
           }
           modem_mgmt_info->state = MCOM_STARTUP_STATE;
           continue_flag = FALSE;
           modem_mgmt_info->mgmt_port_status = modem_mgmt_info->mgmt_port_status
                                               & (~MODEM_STATUS_POLL_RESP_OK);

	   break;

	case MCOM_NO_RESPONSE_STATE:
           if (MODEM_OOB_DEBUG(modem_info)) {
               buginf("\nMODEM(%d/%d): Status Polling enters no response state",
                       slot_num, modem_num);
           }
           modem_mgmt_info->state = MCOM_STARTUP_STATE;
           continue_flag = FALSE;
	   modem_mgmt_info->mgmt_port_status 
	     = modem_mgmt_info->mgmt_port_status & 
	                 (~MODEM_STATUS_POLL_RESP_OK);
	   /*
	    * There is a bug in microcom firmware rev 1.0.37 and above
	    * which causes random OOB port failures. It is reported
	    * that when the OOB port is in this state, the calls may
	    * be dropped; there may be Ring No Answers. So, until the
	    * firmware is fixed, temporary fix is to request that the
	    * modem be reset after the current call completes. This
	    * should be removed when the firmware is fixed to correct
	    * the OOB failure problem. 
	    */
	   if (mai_recover_modem_from_failure(modem_info)) {
	     if (MODEM_OOB_DEBUG(modem_info)) {
	       buginf("\nMODEM(%d/%d): Modem reset request queued to "
		      "recover from OOBP failure", slot_num, modem_num);
	     }
	     modem_mgmt_info->modem_stats.modem_reset_to_recover_oob++;
	   }
	   break;

        default:
           errmsg(&msgsym(UNEXPECTED_STATUS_STATE, MODEM),
                  modem_mgmt_info->state, slot_num, modem_num);
           continue_flag = FALSE;
           break;
     }

     if ((modem_info->modem_config & STATUS_POLL_ALLOWED) &&
         (modem_mgmt_info->mgmt_port_status & STATUS_POLL_ALLOWED)) {

        if (hdms_msg_ptr->type != MCOM_NOT_USED) {
           hdms_msg_ptr->modem_address = modem_num + 1;
           hdms_msg_ptr->c_field = modem_mgmt_info->sequence_num 
                                                      & MCOM_SEQ_NUM_MASK;
           event = modem_mgmt_send_and_wait_msg(modem_info);
        }
     } else {
       continue_flag = FALSE;
       modem_mgmt_info->state = MCOM_STARTUP_STATE;
     }
   }
}

/*
 * =====================================================================
 * modem_mgmt_dc_state_machine()
 *
 * Description
 * This is state machine for DC status polling,
 * it is called by modem_mgmt_main() and handles one modem at a time.
 *
 * This DC session (AT mode) state machine is designed based on
 * "Microcom Asynchronous Backplane Protocol Specification".
 * For more detail information, please refer to that Specification 
 * and "AS5200 modem management software design specification".

 * Parameters:
 * modem_info -- pointer points to one modem table
 *
 * Returns:
 * None  
 *
 * =====================================================================
 */
static void modem_mgmt_dc_state_machine (modem_info_t *modem_info)
{
   boolean continue_flag;
   int event;
   modem_mgmt_info_t *modem_mgmt_info;
   nexthelper *at_data;
   ushort slot_num, modem_num;
 
   slot_num = CSM_MODEM_SLOT(modem_info);
   modem_num = CSM_MODEM_PORT(modem_info);

   if (modem_info->initialized == FALSE) {
      if (MODEM_OOB_DEBUG(modem_info)) {
          buginf("\nMODEM(%d/%d): AT mode is not functional"
                 " (no oob modem address)",
                  slot_num, modem_num);
      }
      return;
   }

   continue_flag = TRUE;
   event = MCOM_EV_ROUND_ROBIN;
   modem_mgmt_info = modem_info->modem_mgmt_info;
 
   while (continue_flag) {
      hdms_msg_ptr->type = MCOM_NOT_USED;
      hdms_msg_ptr->length = MCOM_NO_DATA;

      switch (modem_mgmt_info->dc.dc_state) {
	 case MCOM_DC_SEND_STATE: 
            switch (event) {
               case MCOM_EV_ROUND_ROBIN:
                  if (!QUEUEEMPTY(&(modem_mgmt_info->dc.mgmt_at_tomodemQ))) {
                     at_data = dequeue(&(modem_mgmt_info->dc.mgmt_at_tomodemQ));
                     hdms_msg_ptr->length =  strlen(&at_data->data[0]);
                     bcopy(&(at_data->data[0]), hdms_msg_ptr->data, 
                           hdms_msg_ptr->length);
                     /* HDMS protocol expect a <cr> at end of AT command, 
                      * add it in
                      */
                     *( hdms_msg_ptr->data + hdms_msg_ptr->length ) = 0xd;
                     hdms_msg_ptr->length++;
                     hdms_msg_ptr->type = MCOM_DC_SESSION;
                     free(at_data);
                  }
                  else
                     /* no AT command to send now, do nothing */
                     continue_flag = FALSE;
		  break;
	       case MCOM_EV_DC_CMD:
	          modem_mgmt_info->dc.dc_state = MCOM_DC_POLL_RESPONSE_STATE;
                  modem_mgmt_info->mgmt_port_status |= MODEM_DC_SESSION_RESP_OK;
                  hdms_msg_ptr->type = MCOM_DC_SESSION_EVENT_POLL;
		  break;
	       case MCOM_EV_TIME_OUT:
	       case MCOM_EV_PROTOCOL_ERR:
	          modem_mgmt_info->dc.dc_state = MCOM_DC_CLOSE_STATE;
		  break;
               case MCOM_EV_WRONG_SEQ_NUM: 
	          modem_mgmt_info->dc.dc_state = MCOM_DC_CLOSE_STATE;
		  modem_mgmt_info->sequence_num = 0;
                  break;
               default:
	          modem_mgmt_info->dc.dc_state = MCOM_DC_CLOSE_STATE;
                  errmsg(&msgsym(UNEXPECTED_DC_EV, MODEM),
                         event, slot_num, modem_num);
		  break;
	    }
            /* case MCOM_EV_TERM_SESSION */
            if (!modem_mgmt_info->dc_session_flg) {
               event = MCOM_EV_TERM_SESSION;
	       modem_mgmt_info->dc.dc_state = MCOM_DC_CLOSE_STATE;
               continue_flag = TRUE;
            }
            break;

	 case MCOM_DC_POLL_RESPONSE_STATE: 
	    switch (event) {
               case MCOM_EV_ROUND_ROBIN:
                  hdms_msg_ptr->type = MCOM_DC_SESSION_EVENT_POLL;
		  break;
	       case MCOM_EV_DC_POLL:
	          modem_mgmt_info->dc.dc_state = MCOM_DC_SEND_STATE;
		  continue_flag = FALSE;
		  break;
	       case MCOM_EV_DC_POLL_NO_DATA:
	          continue_flag = FALSE;
		  break;
	       case MCOM_EV_DC_POLL_BAD_AT:
	          modem_mgmt_info->dc.dc_state = MCOM_DC_SEND_STATE;
	          continue_flag = FALSE;
		  break;
	       case MCOM_EV_DC_POLL_MORE_DATA:
                  hdms_msg_ptr->type = MCOM_DC_SESSION_EVENT_POLL;
		  break;
	       case MCOM_EV_TIME_OUT:
	       case MCOM_EV_PROTOCOL_ERR:
	          modem_mgmt_info->dc.dc_state = MCOM_DC_CLOSE_STATE;
		  break;
               case MCOM_EV_WRONG_SEQ_NUM: 
	          modem_mgmt_info->dc.dc_state = MCOM_DC_CLOSE_STATE;
		  modem_mgmt_info->sequence_num = 0;
                  break;
               default:
	          modem_mgmt_info->dc.dc_state = MCOM_DC_CLOSE_STATE;
                  errmsg(&msgsym(UNEXPECTED_DC_EV, MODEM),
                         event, slot_num, modem_num);
		  break;
	     }
	     /* case MCOM_EV_TERM_SESSION */
             if (!modem_mgmt_info->dc_session_flg) {
                event = MCOM_EV_TERM_SESSION;
                modem_mgmt_info->dc.dc_state = MCOM_DC_CLOSE_STATE;
                continue_flag = TRUE;
             }
             break;

	 case MCOM_DC_CLOSE_STATE:
             modem_mgmt_info->mgmt_port_status &= (~MODEM_DC_SESSION_RESP_OK);
             if (modem_mgmt_info->dc_session_flg) {
                /* error occurs, clean up AT input/output queues and continue */
                modem_mgmt_clear_at_in_out_queues (modem_mgmt_info);
             } else {
                /* user requests to terminate DC session */
                modem_mgmt_terminate_dc_session(slot_num, modem_num);

                if (modem_mgmt_is_newer_modem_firmware(modem_info,
                                   MCOM_DC_TERMINATION_FIRMWARE_VERSION)) {
                   /* yes, 'DC termination' command supported */

                   /* send 'dc termination' command to modem.
                    * 'DC termination' command:  type is MCOM_DC_SESSION and 
                    * length equals to 0.
                    */
 
                   hdms_msg_ptr->type = MCOM_DC_SESSION;
                   hdms_msg_ptr->length = 0;
                   hdms_msg_ptr->modem_address = modem_num + 1;
                   hdms_msg_ptr->c_field = modem_mgmt_info->sequence_num
                                                   & MCOM_SEQ_NUM_MASK;
                   event = modem_mgmt_send_and_wait_msg(modem_info);
                   if (event != MCOM_DC_SESSION) {
                      if (MODEM_OOB_DEBUG(modem_info)) {
                         buginf("\nMODEM(%d/%d): DC session clear modem buffer "
                                "failed", slot_num, modem_num);
                      }
                   }
                }
		modem_mgmt_info->clear_dc = CLEARED_ALLOW_NEW_DC;
             }

	     if(event == MCOM_EV_TIME_OUT) {
                if (MODEM_OOB_DEBUG(modem_info)) {
                    buginf("\nMODEM(%d/%d): AT mode no response error",
                            slot_num, modem_num);
	        }
	     }
             else if (event == MCOM_EV_TERM_SESSION) {
                /*user wants to close AT mode, reset mgmt_port_status */
                modem_mgmt_info->mgmt_port_status = 
                                     modem_mgmt_info->mgmt_port_status | 
                                     MODEM_DC_SESSION_RESP_OK;
	     }
	     else if (event == MCOM_EV_PROTOCOL_ERR) {
                if (MODEM_OOB_DEBUG(modem_info)) {
                    buginf("\nMODEM(%d/%d): AT mode protocol error",
                            slot_num, modem_num);
	        }
	     }
	     else if (event == MCOM_EV_WRONG_SEQ_NUM) {
                if (MODEM_OOB_DEBUG(modem_info)) {
                    buginf("\nMODEM(%d/%d): AT mode sequence number "
                           "error", slot_num, modem_num);
	        }
	     }
             else if (event != MCOM_DC_SESSION) {
                if (MODEM_OOB_DEBUG(modem_info)) {
                      buginf("\nMODEM(%d/%d): AT mode unexpected "
                             "event error", slot_num, modem_num);
                }
	     }
    
             /* put into the default state */
	     modem_mgmt_info->dc.dc_state = MCOM_DC_SEND_STATE;
             hdms_msg_ptr->type = MCOM_NOT_USED; 
             continue_flag = FALSE;
	     break;

         default:
             modem_mgmt_info->dc.dc_state = MCOM_DC_CLOSE_STATE;
             continue_flag = FALSE;
             errmsg(&msgsym(UNEXPECTED_DC_STATE, MODEM),
                    modem_mgmt_info->dc.dc_state, slot_num, modem_num);
	     break;
      } /* end of switch */
 
      if (hdms_msg_ptr->type != MCOM_NOT_USED) {
         hdms_msg_ptr->modem_address = modem_num + 1;
         hdms_msg_ptr->c_field = modem_mgmt_info->sequence_num 
                                                & MCOM_SEQ_NUM_MASK;
         event = modem_mgmt_send_and_wait_msg(modem_info);
      }
   } 
}

/*
 * =====================================================================
 * modem_mgmt_current_active_at_mode_block()
 *
 * Description
 * 1. Finds the next modem on line to do AT mode.
 * 2. Sets the global variable modem_mgmt_current_at_mode_block points 
 *    to the next targeted at_mode_block.
 *
 * Parameters:
 * None
 *
 * Returns:
 * pointer points to modem table of the targeted modem
 * NULL if there is no DC session
 *
 * =====================================================================
 */
static modem_info_t *modem_mgmt_current_active_at_mode_block 
                     (void)
{
   modem_mgmt_at_mode_block *output;

   if ((modem_mgmt_active_at_mode_blocks == NULL) ||
       (modem_mgmt_current_at_mode_block == NULL)) {
      return(NULL);
   }
   else {
      output = modem_mgmt_current_at_mode_block;
 
      if (modem_mgmt_current_at_mode_block->next !=NULL) {
         modem_mgmt_current_at_mode_block = 
                                modem_mgmt_current_at_mode_block->next;
      }
      else {
         modem_mgmt_current_at_mode_block = 
                                modem_mgmt_active_at_mode_blocks;
      }
      return(output->modem_info); 
   }
}

/*
 * =====================================================================
 * modem_mgmt_create_active_at_mode_block()
 *
 * Description
 * Create an active AT mode block. 
 * Returns pointer to AT mode block on success.
 * Non-NULL reason string describing reason of failure. This function
 * keeps the list sorted.
 *
 * Parameters:
 * slot_number -- modem card slot number 
 * modem_number -- modem number
 *
 * Returns:
 * pointer points to modem_mgmt_at_mode_block if successed
 *
 * =====================================================================
 */

static modem_mgmt_at_mode_block *modem_mgmt_create_active_at_mode_block
                           (ushort slot_num, ushort modem_num, char **reason)
{
  modem_mgmt_at_mode_block *at_mode_block, *prev, *new_block;
  ushort slot_modem;
  
  slot_modem = slot_num << 8 | (modem_num & 0xff);

  prev = NULL;   
  for (at_mode_block = modem_mgmt_active_at_mode_blocks;
       (at_mode_block != NULL);
       at_mode_block = at_mode_block->next) {
    if (at_mode_block->slot_modem == slot_modem) {
      *reason = "Modem already being managed in at-mode";
      return(NULL);
    } else if (at_mode_block->slot_modem > slot_modem) {
      break;
    } else 
      prev = at_mode_block;
  } 

  /* Not found. Create a new entry. */
  new_block = malloc(sizeof(modem_mgmt_at_mode_block));
  if (new_block == NULL) {
    *reason = "no memory";  
    return(NULL);
  }

  /* Fill in data fields */

  new_block->slot_modem = slot_modem;
  new_block->tty = stdio;   
  new_block->modem_info = CSM_MODEM_INFO_OR_NULL(slot_num, modem_num);
  /* Put sorted into list */
  if (prev == NULL) {
    modem_mgmt_active_at_mode_blocks = new_block;

    /*added for Brasil*/
    modem_mgmt_current_at_mode_block = modem_mgmt_active_at_mode_blocks;
  } else {
    prev->next = new_block;
  }
  new_block->next = at_mode_block;
  if (MODEM_OOB_DEBUG(new_block->modem_info)) {
      buginf("\nMODEM(%d/%d): DC session opened", slot_num, modem_num);
  }
  return(new_block);
}

/*
 * =====================================================================
 * modem_mgmt_find_active_at_mode_block()
 *
 * Description
 * Find an active AT mode block.
 *
 * Parameters:
 * slot_number -- modem card slot number
 * modem_number -- modem number
 *
 * Returns:
 * pointer points to active AT mode block if find one    
 * NULL if can't find 
 *
 * =====================================================================
 */

/*
 * modem_mgmt_find_active_at_mode_block
 * Find an active AT mode block.
 */
modem_mgmt_at_mode_block *modem_mgmt_find_active_at_mode_block
                          (ushort slot_num, ushort modem_num)
{
  ushort slot_modem;
  modem_mgmt_at_mode_block *at_mode_block;

  slot_modem = slot_num << 8 | (modem_num & 0xff);

  for (at_mode_block = modem_mgmt_active_at_mode_blocks;
       (at_mode_block != NULL);
       at_mode_block = at_mode_block->next) {
    if (at_mode_block->slot_modem == slot_modem) {
      return(at_mode_block);
    }
    if (at_mode_block->slot_modem > slot_modem) {
      break;
    }
  }
  return(NULL);
}

/*
 * =====================================================================
 * modem_mgmt_write_at_data()
 *
 * Description
 * Write out data as an AT command to the given modem.
 *
 * Parameters:
 * at_mode_block -- pointer points to an AT mode block
 * buffer -- char pointer points to user's AT commands
 *
 * Returns:
 * None
 *
 * =====================================================================
 */

static void modem_mgmt_write_at_data (modem_mgmt_at_mode_block *at_mode_block,
			       char *buffer)
{
    nexthelper *q_buf;

    q_buf = malloc(MODEM_MGMT_MAX_AT_MODE_READ_LINE_SIZE);  
    if (q_buf == NULL) {
        return;
    }

    strcpy(&(q_buf->data[0]), buffer);
    enqueue(&(at_mode_block->modem_info->modem_mgmt_info->dc.mgmt_at_tomodemQ), 
              q_buf);
}

/*
 * =====================================================================
 * modem_mgmt_at_mode_reader()
 *
 * Description
 * This process will wake up when there is data on any AT-MODE session
 * and send it to the appropriate terminal. This is a separate process since
 * sending the output to the correct terminal has the potential of taking a
 * bit of time.
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 *
 * =====================================================================
 */

process modem_mgmt_at_mode_reader (void)
{
    modem_mgmt_info_t *modem_mgmt_info;
    modem_mgmt_at_mode_block *at_mode_block;
    paktype *pak;
    queuetype *q;

    process_wait_on_system_init();

    process_set_priority(PRIO_LOW);

    process_set_boolean(at_response_ready, FALSE); 
    process_watch_boolean(at_response_ready, ENABLE, RECURRING);
    while (TRUE) {
        process_wait_for_event();
        process_set_boolean(at_response_ready, FALSE); 

	if (modem_mgmt_active_at_mode_blocks == NULL) {
	    modem_mgmt_at_mode_reader_fork = 0;
            process_kill(THIS_PROCESS);
	    /* s_kill_self_with_no_analysis();*/
	}

	for (at_mode_block = modem_mgmt_active_at_mode_blocks;
	     at_mode_block != NULL;
	     at_mode_block = at_mode_block->next) {
            modem_mgmt_info = at_mode_block->modem_info->modem_mgmt_info;
            q = &(modem_mgmt_info->dc.mgmt_at_frommodemQ);
	    while (TRUE) {
		pak = dequeue(q);
		if (!pak) {
                    modem_mgmt_info->clear_dc = TERMINATION_ALLOWED;
		    break;
                }
		if (pak->datagramsize > 0) {
		    tty_sendmessage(at_mode_block->tty, pak);
                }
		datagram_done(pak);
	    }
	}
    }
}

/*
 * =====================================================================
 * modem_mgmt_start_closing_dc_session()
 *
 * Description
 * Start closing the DC session if possible.
 *
 * Parameters:
 * oob_off_flag -- TRUE if is called from oob turned off operation
 *                 FALSE if is not calling from oob turned off operation
 * slot -- modem card slot number
 * port -- modem number
 * modem_info -- modem table pointer
 *
 * Returns:
 * TRUE -- start closing the DC session
 * FALSE -- can't close the DC session 
 *
 * =====================================================================
 */

boolean modem_mgmt_start_closing_dc_session (boolean oob_off_flag, 
                                             ushort slot, ushort port, 
                                             modem_info_t *modem_info)
{
    paktype *pak;
    modem_mgmt_at_mode_block *at_mode_block;
    modem_mgmt_info_t *modem_mgmt_info;

    if (!modem_info) {
       printf("\nModem DC session can't be closed -- "
              "Modem %d/%d is not available", slot, port);
       return(FALSE);
    }  
    modem_mgmt_info = modem_info->modem_mgmt_info;

    if (!modem_mgmt_info) {
       if (oob_off_flag) {
          /* When we try to turn off the oob port, just do nothing 
           *if no modem_mgmt_info allocated   
           */
          return(TRUE);
       }
       else {
          printf("\nModem DC session can't be closed -- "
                 "Modem %d/%d is not manageable modem", 
                 slot, port); 
          return(FALSE); 
       }
       return(FALSE); 
    }    
    else if (!(modem_mgmt_info->dc_session_flg)) {
       if (oob_off_flag) {
          /* When we try to turn off the oob port, it's ok if there 
           *is no DC session 
           */
          return(TRUE);
       }
       else {
          printf("\nModem DC session can't be closed -- "
                 "Modem %d/%d is not running DC session", 
                 slot, port);
          return(FALSE); 
       }
    }     

    pak = getbuffer(MAX_MCOM_MESSAGE_SIZE);

    if (!pak) {
       return(TRUE);
    } else {
       /* yes, we can start to close the DC session */
       printf("DC session is closed at modem %d/%d ", slot, port);

       /* Prepare message to be printed out at the targeted session */
       sprintf(pak->datagramstart,"Direct Connect session cleared by %s",
               user_on_stdio_str());
       pak->datagramsize = strlen(pak->datagramstart);
       enqueue(&(modem_mgmt_info->dc.mgmt_at_frommodemQ), pak);

       process_set_boolean(at_response_ready, TRUE);

       /* Set a flag that modem_mgmt_exec_at_mode monitors so    */
       /* the process handling the DC session will be terminated */
       modem_mgmt_info->clear_dc = CLEAR_BUT_WAIT_FOR_OUTPUT_MSG;
 
       at_mode_block = modem_mgmt_find_active_at_mode_block(slot, port);
 
       /* To set control c at targeted tty while the targeted tty is      */ 
       /* always waiting at rdtty routine. After it gets this control c,  */
       /* it will start the termination process of the current DC session */

       if (at_mode_block) {
          at_mode_block->tty->statbits |=  CONTROLC;
       }
       return(TRUE);
   }
}


/*
 * =====================================================================
 * modem_mgmt_oob_operation()
 *
 * Description
 * To turn on/off oob port, modem management needs to be notified by 
 * calling this procedure.
 *
 * Parameters:
 * oob_action-- OOB_PORT_ON to turn on oob port
 *              OOB_PORT_OFF to turn off oob port
 * modem_info_ptr -- moden table pointer
 *
 * Returns:
 * None
 *
 * =====================================================================
 */
 
void modem_mgmt_oob_operation (void *modem_info_ptr, boolean oob_action)
{
    ushort slot_num, modem_num;
    modem_info_t *modem_info;
    modem_mgmt_info_t *modem_mgmt_info;
    
    modem_info = (modem_info_t *) modem_info_ptr;
    if ((!modem_info) || (!(modem_info->modem_mgmt_info))) {
       return;
    }
    modem_mgmt_info = modem_info->modem_mgmt_info;

    if (oob_action==OOB_PORT_ON) {
          /* Reset -- oob status bits; initialized flg; sequence number */
          modem_info->initialized = TRUE;
          modem_mgmt_info->state = MCOM_STARTUP_STATE;
          modem_mgmt_info->sequence_num = 0;
          modem_mgmt_info->mgmt_port_status |=
                         MODEM_STATUS_POLL_RESP_OK | MODEM_DC_SESSION_RESP_OK;
    }
    else {
       /* oob_action==OOB_PORT_OFF */
          modem_info->initialized = FALSE;
          slot_num = CSM_MODEM_SLOT(modem_info);
          modem_num = CSM_MODEM_PORT(modem_info);
          if (modem_mgmt_start_closing_dc_session( TRUE, slot_num, 
                                                   modem_num, modem_info)) {
             /* To set up oob status bits */
             modem_mgmt_info->mgmt_port_status &= (~MODEM_STATUS_POLL_RESP_OK)& 
                                                  (~MODEM_DC_SESSION_RESP_OK); 
          }
    }
} 
/*
 * =====================================================================
 * modem_mgmt_exec_at_mode()
 *
 * Description
 * Enter AT mode on the given modem, if available, and accept commands
 * until user quits with CTRL-C.
 *
 * Parameters:
 * modem_info -- pointer points to one modem table 
 *
 * Returns:
 * None
 *
 * =====================================================================
 */

void modem_mgmt_exec_at_mode (modem_info_t *modem_info)
{
    modem_mgmt_info_t *modem_mgmt_info;
    modem_mgmt_at_mode_block *at_mode_block;
    char buffer[MODEM_MGMT_MAX_AT_MODE_READ_LINE_SIZE];
    ushort slot_num, modem_num;
    char *reason;

    if (!modem_info) {
       return;
    }

    modem_mgmt_info = modem_info->modem_mgmt_info;
    slot_num = CSM_MODEM_SLOT(modem_info);
    modem_num = CSM_MODEM_PORT(modem_info); 

    at_mode_block = modem_mgmt_create_active_at_mode_block
                    (slot_num, modem_num, &reason);
 
    if (at_mode_block == NULL) {
      if (MODEM_OOB_DEBUG(modem_info)) {
          buginf("\nMODEM(%d/%d): Unable to activate AT-MODE "
                 "session. Reason: %s",
                  slot_num, modem_num, reason);
        return;
      }
    }
    
    modem_mgmt_info->dc_session_flg =TRUE;
    modem_mgmt_info->clear_dc = TERMINATION_ALLOWED;
    
    if (modem_mgmt_at_mode_reader_fork == 0) {
        modem_mgmt_at_mode_reader_fork =
            process_create (modem_mgmt_at_mode_reader, "MM/AT Read", 
                            NORMAL_STACK, PRIO_LOW);
        if (modem_mgmt_at_mode_reader_fork == NO_PROCESS) {
          printf("\nERROR: Cannot create the modem management at reader");
        }

    }
 
    /*
     * Turn CTRL-C catching on, and clear the CTRL-C variable
     */
    printf("\nYou are now entering AT command mode on modem "
           "(slot %d / port %d).", slot_num, modem_num);
    printf("\nPlease type CTRL-C to exit AT command mode.\n");
    stdio->statbits |= CTRLC_ENBL;
    stdio->statbits &= ~CONTROLC;
    while (!(stdio->statbits & (CONTROLC+CARDROP+IDLETIMEOUT+HANGUP))) {
        /* - 1 to save one byte for <cr> which will be added later */
        if (!rdtty(buffer, MODEM_MGMT_MAX_AT_MODE_READ_LINE_SIZE -1)) {
            continue;
        }
        modem_mgmt_write_at_data(at_mode_block, buffer);
    }    

    while (modem_mgmt_info->clear_dc == CLEAR_BUT_WAIT_FOR_OUTPUT_MSG) {
       /* Normally we will not be here. 
        * Come here only if the clear modem AT commanmd is received and 
        * waiting for the DC session termination message been sent.
        */
       process_sleep_for(MODEM_MGMT_REST_TIME_100_MS*3); 
    }

    stdio->statbits &= ~CTRLC_ENBL;
    stdio->statbits &= ~CONTROLC;
    modem_mgmt_info->dc_session_flg = FALSE;
}
 
/*
 * =====================================================================
 * modem_mgmt_main()
 *
 * Description
 * This is the main routine of the modem management process.
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */


process modem_mgmt_main (void)
{
   boolean flag;
   ushort slot, port;
   modem_info_t *modem_info;
   modem_mgmt_info_t *modem_mgmt_info;
   sys_timestamp status_poll_start_time;
   uchar dc_warning_cnt;
  
   process_wait_on_system_init();

   dc_warning_cnt = 0;
   while (TRUE) {
      GET_TIMESTAMP(status_poll_start_time);
 
      /*do status polling */
      for (slot=0; slot < modem_pool->max_slots; slot++) {
         if (modem_pool->modem_slot[slot] != NULL) {
            for (port= 0; 
                 port < modem_pool->modem_slot[slot]->total_modem_num; 
                 port++) {
               modem_info= CSM_MODEM_INFO_OR_NULL(slot, port);
               if (modem_info!= NULL) {
                  modem_mgmt_info = modem_info->modem_mgmt_info;
                  if ((modem_info->modem_config & STATUS_POLL_ALLOWED) && 
                      (modem_mgmt_info->mgmt_port_status & 
                      STATUS_POLL_ALLOWED)) {
                     modem_mgmt_status_state_machine(modem_info);
                     process_sleep_for(MODEM_MGMT_REST_TIME_30_MS);
                  }
                  else {
                     if (MODEM_OOB_DEBUG(modem_info)) {
                        if (!(modem_info->modem_config & STATUS_POLL_ALLOWED)) {
                           buginf("\nMODEM(%d/%d): No status polling"
                                  " (not configured, config=%x)",
                                   slot, port, modem_info->modem_config);
                        }
                        else if (!(modem_mgmt_info->mgmt_port_status
                                                     & STATUS_POLL_ALLOWED)) {
                           buginf("\nMODEM(%d/%d): No status polling"
                                  " (oob no response %d"
                                  " times or in cmd execution, status=%x)",
                                  slot, port,
                                  modem_poll_retry+1,
                                  modem_mgmt_info->mgmt_port_status);
                        }
                        else
                           buginf("\nMODEM(%d/%d): No status polling"
                                  " (unknown reason)", slot, port);
                     }
                  }
               }
            } /*for*/
         }
      }
 
      process_sleep_for(MODEM_MGMT_REST_TIME_10_MS);

      /* process DC session if there is any */
      modem_info = modem_mgmt_current_active_at_mode_block();
      if (modem_info != NULL) {
         /* To avoid the DC session not been processed at all 
          * always do ONE DC processing first          
          */
         modem_mgmt_dc_state_machine(modem_info);
      }

      flag = CLOCK_IN_INTERVAL(status_poll_start_time, modem_poll_time - 
                               MODEM_MGMT_SAFE_GUARD_TIME);
      if ((flag == FALSE) && (modem_mgmt_active_at_mode_blocks != NULL)) {
          /* possible does not have enough time to process DC session */
          if ( ++dc_warning_cnt == MODEM_MGMT_NO_DC_PROCESSING_LIMIT) {
             dc_warning_cnt = 0; 
             errmsg(&msgsym(NO_TIME_FOR_DC, MODEM));
          }
      }

      while (flag) {
         modem_info = modem_mgmt_current_active_at_mode_block();
         if (modem_info != NULL) {
             modem_mgmt_dc_state_machine(modem_info);
             process_sleep_for(MODEM_MGMT_REST_TIME_30_MS);
         }
         else {
           process_sleep_for(MODEM_MGMT_CHECK_DC_TIME);
         }
 
         flag = CLOCK_IN_INTERVAL(status_poll_start_time, 
                                  modem_poll_time - MODEM_MGMT_SAFE_GUARD_TIME);
      }
   }
}
 
/*
 * =====================================================================
 * modem_mgmt_exists()
 *
 * Description
 * modem management software module exists
 *
 * Parameters:
 * None
 *
 * Returns:
 * TRUE always 
 * =====================================================================
 */

boolean modem_mgmt_exists (void)
{
   return TRUE;
}


/*
 * =====================================================================
 * modem_mgmt_inits()
 *
 * Description
 * This procedure will be called at system init time.
 * do following:
 * 1. call proc modem_mgmt_init_mcom() to init HDMS protocol related fields.
 * 2. call proc modem_mgmt_init_mcom_statistic() to init all statistic
 *    related variables.
 * 3. add parser link.
 * 4. fork process modem_mgmt_main.
 *
 * Parameters:
 * None
 *
 * Returns:
 * None  
 * =====================================================================
 */
 
void modem_mgmt_inits (void)
{
   ushort slot, port;
   modem_info_t *modem_info;
   modem_q_info_t *event;
   uint modems_installed = 0;
   pid_t process_id;
  
   /* 
    * create some queues for CSM to pass events from interrupt level
    * to modem mgmt so that it can log the info into the history queue
    */
   modem_csm_event_freeQ = malloc(sizeof(queuetype));
   queue_init(modem_csm_event_freeQ, 0);
   modem_csm_eventQ = create_watched_queue("CSM modem event Queue", 0, 0);

   for (slot=0; slot<  modem_pool->max_slots; slot++) {
      if (modem_pool->modem_slot[slot] != NULL) {
         for (port= 0; port < modem_pool->modem_slot[slot]->total_modem_num; 
                                                                   port++) {
            modem_info = CSM_MODEM_INFO_OR_NULL(slot,port);
            if (modem_info!= NULL) {
               modem_info->modem_config = CFG_MODEM_CONFIG_DEFAULT;   
 
               modem_info->modem_mgmt_info = malloc(sizeof(modem_mgmt_info_t));
               if (!modem_info->modem_mgmt_info) {
                  return;
               }
               modem_mgmt_create_history_eventQ(modem_info);
               modem_mgmt_init_mcom(modem_info);
               modem_mgmt_init_mcom_statistic(modem_info);
               csm_clear_modem_info_counter(modem_info);
            }
            /*
             * create and save a free CSM event for each modem
             */
            event = malloc(sizeof(modem_q_info_t));
            enqueue(modem_csm_event_freeQ, event);
         } /* for */
         modems_installed += modem_pool->modem_slot[slot]->total_modem_num;
      }   
   } /* for */   
   modem_debug_init();
   /*
    * create chunks of memory so that we won't be fragmenting memory since 
    * most of the events are small in the modem history log
    */
   modem_event_hdr_chunk = 
       chunk_create(sizeof(modem_history_event_t), 
                    modem_buffer_size + (modem_buffer_size * modems_installed),
                    CHUNK_FLAGS_DYNAMIC, NULL, 0, "Modem Event Header Chunks");
   modem_small_event_chunk = 
       chunk_create(MAX_MODEM_SMALL_EVENT_SIZE, 
                    modem_buffer_size + (modem_buffer_size * modems_installed),
                    CHUNK_FLAGS_DYNAMIC, NULL, 0, "Modem Small Event Chunks");
   modem_large_event_chunk = 
       chunk_create(MAX_MODEM_LARGE_EVENT_SIZE, modems_installed,
                    CHUNK_FLAGS_DYNAMIC, NULL, 0, "Modem Large Event Chunks");

   reg_add_modem_group(modem_mgmt_group_created, "modem_mgmt_group_created");
   reg_add_csm_event_log(modem_mgmt_csm_event_handler, 
                         "modem_mgmt_csm_event_handler");
   process_create(modem_mgmt_csm_event_process, "MM/CSM Event Bkgnd", 
                  NORMAL_STACK, PRIO_NORMAL);
 
   at_response_ready = create_watched_boolean("at response ready", 0);
   hdms_reply_ready = create_watched_boolean("hdms reply ready", 0);

   modem_mgmt_active_at_mode_blocks = NULL;
   modem_mgmt_current_at_mode_block = NULL;

   send_pak_ptr = getbuffer(MAX_MCOM_MESSAGE_SIZE);
   if (send_pak_ptr) {
      hdms_msg_ptr= (modem_mgmt_pkt_hdr_t *)MODEM_MGMT_HEADSTART(send_pak_ptr);
 
      process_id = process_create (modem_mgmt_main, "MM/main", 
                                   NORMAL_STACK, PRIO_LOW);
      if (process_id == NO_PROCESS) {
          printf("\nERROR: Cannot create the modem management main process");
      }
   }

}

/*
 * =====================================================================
 * modem_mgmt_subsystem_init()
 *
 * Description:
 * Modem management subsytem init.
 *
 * Parameters:
 * subsystype -- subsytem type SUBSYS_CLASS_DRIVER
 *
 * Returns:
 * None
 * =====================================================================
 */

static void modem_mgmt_subsystem_init ( subsystype *subsys )
{
   reg_add_modem_mgmt_exists(modem_mgmt_exists, "modem_mgmt_exists");

   reg_add_modem_mgmt_oob_operation(modem_mgmt_oob_operation, 
                                    "modem_mgmt_oob_operation"); 

   reg_add_modem_mgmt_inits(modem_mgmt_inits, "modem_mgmt_inits");
}

#define MODEM_MGMT_MAJVERSION  1
#define MODEM_MGMT_MINVERSION  0
#define MODEM_MGMT_EDITVERSION 1

/*
 * as5200_modem_mgmt subsystem header
 */
SUBSYS_HEADER(modem_mgmt,
              MODEM_MGMT_MAJVERSION,
              MODEM_MGMT_MINVERSION,
              MODEM_MGMT_EDITVERSION,
              modem_mgmt_subsystem_init,
              SUBSYS_CLASS_DRIVER,
              NULL,
              NULL);
