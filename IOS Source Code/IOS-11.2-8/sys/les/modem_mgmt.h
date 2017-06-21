/* $Id: modem_mgmt.h,v 3.1.64.4 1996/08/28 12:59:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_mgmt.h,v $
 *------------------------------------------------------------------
 * modem_mgmt.h -- Modem Management header files
 *
 * November, 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: modem_mgmt.h,v $
 * Revision 3.1.64.4  1996/08/28  12:59:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.64.3  1996/08/07  09:01:23  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/07/16  11:31:08  styang
 * CSCdi58062:  %MODEM-3-UNEXPECTED_STSTUS_EV: Error -- status state
 * machine receive
 * Branch: California_branch
 * 1. The error message is generated wrong, code modified to correct such
 * error.
 * 2. Code clean up.
 *
 * Revision 3.1.64.1  1996/06/16  21:16:23  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:35  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MODEM_MGMT_H__
#define __MODEM_MGMT_H__

/***********************************************
 * MICROCOM HDMS frame types
 ***********************************************/
#define MCOM_STARTUP               0x01
#define MCOM_CONTROL               0x02
#define MCOM_DC_SESSION            0x03
#define MCOM_REFRESH_EVENT_POLL    0x81
#define MCOM_EVENT_POLL            0x82
#define MCOM_DC_SESSION_EVENT_POLL 0x83
#define MCOM_NOT_USED              0x99
/* 
 * Additional CSM generated events to be included 
 * in modem event queue.
 */
#define CSM_CALLED_NUMBER          0xaa         
#define CSM_CALLER_NUMBER          0xab         
 
/***********************************************
 * MICROCOM HDMS reply offsets
 ***********************************************/
/* Startup Reply */
#define MCOM_MODEM_TYPE_OFFSET         0
#define MCOM_MODEM_REVISION_OFFSET     1
#define MCOM_BOOT_REVISION_OFFSET      4
/* Control Reply */
#define MCOM_EVENT_STATUS0_OFFSET      0
#define MCOM_EVENT_STATUS1_OFFSET      1
/* Poll Event Replies */
/* RS-232 Event */
#define MCOM_RS232_EV_OFFSET           0
#define MCOM_RS232_EV_CHANGED_OFFSET   1
/* Connection and Connection Update Events */
#define MCOM_TX_SPEED_OFFSET           0
#define MCOM_RX_SPEED_OFFSET           1
#define MCOM_MODULATION_OFFSET         2
#define MCOM_ORIG_ANSWER_OFFSET        3
#define MCOM_PROTOCOL_OFFSET           4
#define MCOM_COMPRESSION_OFFSET        5
/* Analog Report Event */
#define MCOM_TX_LEVEL_OFFSET           0
#define MCOM_RX_LEVEL_OFFSET           1
#define MCOM_SIGNAL_TO_NOISE_OFFSET    2
/* End Connection Event */
#define MCOM_RETRANSMITS_TX_OFFSET     0
#define MCOM_RETRANSMITS_RX_OFFSET     1
#define MCOM_DURATION_HOURS_OFFSET     2
#define MCOM_DURATION_MINUTES_OFFSET   3
#define MCOM_DURATION_SECONDS_OFFSET   4
#define MCOM_NUM_TX_CHARACTERS_OFFSET  5
#define MCOM_NUM_RX_CHARACTERS_OFFSET  9
#define MCOM_DISC_REASON_LOCAL_OFFSET  13
#define MCOM_DISC_REASON_REMOTE_OFFSET 14

/***********************************************
 * MICROCOM HDMS frame length
 ***********************************************/
#define MCOM_NO_DATA                   0 
#define MCOM_LEN_CONTROL               4 

/***********************************************
 * MICROCOM HDMS frame type MCOM_CONTROL
 * default setting
 ***********************************************/
#define MCOM_EVENT_SELECT_BITS_0       0xff
#define MCOM_EVENT_SELECT_BITS_1       0xff

/***********************************************
 * Modem internal events used in Modem State Machine
 ***********************************************/
#define MCOM_EV_STARTUP                 0x01
#define MCOM_EV_CONTROL                 0x02
#define MCOM_EV_STA_POLL                0x10
#define MCOM_EV_STA_POLL_MORE_DATA      0x11
#define MCOM_EV_STA_POLL_NO_DATA        0x12
#define MCOM_EV_ROUND_ROBIN             0x90
#define MCOM_EV_TIME_OUT                0x92
#define MCOM_EV_PROTOCOL_ERR            0x94
#define MCOM_EV_WRONG_SEQ_NUM           0x96
#define MCOM_EV_UNKNOWN                 0x98
 
/***********************************************
 * DC internal events used in DC state machine
 ***********************************************/
#define MCOM_EV_DC_CMD                  0x03
#define MCOM_EV_DC_POLL                 0x83
#define MCOM_EV_DC_POLL_MORE_DATA       0x20
#define MCOM_EV_DC_POLL_NO_DATA         0x21
#define MCOM_EV_DC_POLL_BAD_AT          0x22
#define MCOM_EV_TERM_SESSION            0x25
/* #define MCOM_EV_ROUND_ROBIN          0x90 */
/* #define MCOM_EV_TIME_OUT             0x92 */
/* #define MCOM_EV_PROTOCOL_ERR         0x94 */
/* #define MCOM_EV_WRONG_SEQ_NUM        0x96 */
/* #define MCOM_EV_UNKNOWN              0x98 */
 
/***********************************************
 * MICROCOM HDMS event types
 ***********************************************/
#define MCOM_TX_RX_EVENT                0x00
#define MCOM_RS232_EVENT                0x01
#define MCOM_MODEM_STATE_EVENT          0x02
#define MCOM_CONNECTION_EVENT           0x03
#define MCOM_CALL_PROGRESS_EVENT        0x04
#define MCOM_DTR_EVENT                  0x05
#define MCOM_MODEM_INACTIVE_EVENT       0x06
#define MCOM_ANALOG_REPORT_EVENT        0x07
#define MCOM_END_CONNECTION_EVENT       0x08
#define MCOM_PHONE_NUMBER_EVENT         0x09
#define MCOM_CALLER_ID_EVENT            0x0a
#define MCOM_CONNECTION_UPDATE_EVENT    0x0b
/* state machine generated events */
#define MCOM_PROTOCOL_ERR_EVENT         0xfa
#define MCOM_NO_RESPONSE_EVENT          0xfb
#define MCOM_EVENT_LOST_EVENT           0xfc
#define MCOM_WRONG_SEQ_NUM_EVENT        0xfd
#define MCOM_UNEXPECTED_EVENT           0xfe 

/***********************************************
 * RS232 Transmit and Receive bitmasks
 ***********************************************/
#define RS232_TX                        0x01
#define RS232_RX                        0x02
 
/***********************************************
 * RS232 EIA signals bitmasks
 ***********************************************/
#define RS232_RTS                       0x01
#define RS232_DTR                       0x02
#define RS232_CTS                       0x04
#define RS232_DSR                       0x08
#define RS232_DCD                       0x10
#define RS232_RI                        0x20
#define RS232_TST                       0x40

/***********************************************
 * Modem State values 
 ***********************************************/
#define MODEM_STATE_IDLE                    0x00
#define MODEM_STATE_INCOMING_RING           0x01
#define MODEM_STATE_DIALING_ANSWERING       0x02
#define MODEM_STATE_WAITING_FOR_CARRIER     0x03
#define MODEM_STATE_CONNECTED               0x04
#define MODEM_STATE_BUSY_OUT                0x05
#define MODEM_STATE_DISCONNECTING           0x06
#define MODEM_STATE_RETRAIN_INITIATED       0x07
#define MODEM_STATE_RETRAIN_ACCEPTED        0x08
#define MODEM_STATE_RATE_RENEGOTIATE_INIT   0x09
#define MODEM_STATE_RATE_RENEGOTIATE_ACCEPT 0x0A

/***********************************************
 * Modulation values 
 ***********************************************/
#define MODULATION_BELL_103A       0x01
#define MODULATION_BELL_212A       0x02
#define MODULATION_V21             0x03
#define MODULATION_V22             0x04
#define MODULATION_V22BIS          0x05
#define MODULATION_V23             0x06
#define MODULATION_V32             0x07
#define MODULATION_V32BIS          0x08
#define MODULATION_VFC             0x09
#define MODULATION_V34             0x0a
#define MODULATION_V17             0x0b
#define MODULATION_V29             0x0c
#define MODULATION_V33             0x0d
#define MODULATION_K56PLUS         0x0e

/***********************************************
 * Orig/Answer Status values
 ***********************************************/
#define CALL_ANSWER                0x00
#define CALL_ORIGINATE             0x01
#define CALL_NONE_YET              0x02

/***********************************************
 * Protcol values
 ***********************************************/
#define PROTOCOL_NORMAL            0x00
#define PROTOCOL_DIRECT            0x01
#define PROTOCOL_RELIABLE_MNP      0x02
#define PROTOCOL_RELIABLE_LAPM     0x03
#define PROTOCOL_SYNC              0x04

/***********************************************
 * Compression values
 ***********************************************/
#define COMPRESSION_NONE           0x00
#define COMPRESSION_V42BIS         0x01
#define COMPRESSION_MNP_CLASS_5    0x02

/***********************************************
 * Disconnect Reason values
 ***********************************************/
#define DISC_UNKNOWN               0x00
#define DISC_LOST_CARRIER          0x01
#define DISC_NO_CARRIER            0x02
#define DISC_NO_DIALTONE           0x03
#define DISC_BUSY                  0x04
#define DISC_RESERVED1             0x05
#define DISC_RESERVED2             0x06
#define DISC_WATCHDOG_TIMEOUT      0x07
#define DISC_UNUSED1               0x08
#define DISC_DTR_DROP              0x09
#define DISC_USER_HANGUP           0x0a
#define DISC_UNUSED2               0x0b
#define DISC_COMPRESSION_PROBLEM   0x0c
#define DISC_RETRAIN_FAILURE       0x0d
#define DISC_REMOTE_LINK           0x0e
#define DISC_ABORT                 0x0f
#define DISC_TERMINATE             0x10
#define DISC_RESERVED3             0x11
#define DISC_INACTIVITY_TIMEOUT    0x12
#define DISC_DIAL_STRING_ERROR     0x13
#define DISC_AUTO_LOGON_ERROR      0x14
#define DISC_CCP_NOT_SEEN          0x15
#define DISC_RESERVED4             0x16
#define DISC_UNUSED3               0x17
#define DISC_RESERVED5             0x18
#define DISC_LINK_FAILURE          0x19
#define DISC_CALLBACK_FAILED       0x1a
#define DISC_MODULATION_ERROR      0x1b
#define DISC_DIAL_TIMEOUT          0x1c
#define DISC_UNUSED4               0x1d
#define DISC_REMOTE_HANGUP         0x1e
#define DISC_RESERVED6             0x1f
#define DISC_BLACKLIST             0x20
#define DISC_MNP10_PROTOCOL_ERROR  0x21
#define DISC_LAPM_PROTOCOL_ERROR   0x22
#define DISC_FAX_CLASS2_ERROR      0x23
#define DISC_CONNECTED             0xff

/***********************************************
 * States used in modem state machine
 ***********************************************/
typedef enum {
        MCOM_STARTUP_STATE,
        MCOM_STARTUP_REPLY_STATE,
        MCOM_STATUS_POLL_STATE,
        MCOM_NO_RESPONSE_STATE,
        MCOM_CLEAN_UP_STATE
}mcom_modem_state_machine_t;
 
/***********************************************
 * States used in modem state machine
 ***********************************************/
typedef enum {
        MCOM_DC_CLOSE_STATE,
        MCOM_DC_SEND_STATE,
        MCOM_DC_POLL_RESPONSE_STATE
}mcom_modem_dc_state_machine_t;

/***********************************************
 * bit mask used for HDMS message
***********************************************/
#define MCOM_SEQ_NUM_MASK           0x0f
#define MCOM_BITMAP_MASK            0xf0
#define MCOM_PROTOCOL_ERR           0x20
#define MCOM_EVENT_LOST             0x40
#define MCOM_MORE_DATA              0x80
 
/***********************************************
 * Microcom modem HDMS time out values(in msec)
 * and management processing values
 ***********************************************/
#define MCOM_HDMS_TIME_OUT_VALUE    700
#define MODEM_MGMT_SAFE_GUARD_TIME  300
#define MODEM_MGMT_REST_TIME_10_MS  10
#define MODEM_MGMT_REST_TIME_30_MS  30
#define MODEM_MGMT_REST_TIME_100_MS 100
#define MODEM_MGMT_CHECK_DC_TIME    1500

/***********************************************
 * oob port no dc processing warning limit
 ***********************************************/
#define MODEM_MGMT_NO_DC_PROCESSING_LIMIT 200

/***********************************************
 * Action of the oob port 
 ***********************************************/
#define OOB_PORT_ON                 TRUE
#define OOB_PORT_OFF                FALSE

/***********************************************
 * bit masks used for dc session and  status poll
 ***********************************************/
#define STATUS_POLL_ALLOWED         0x01
#define DC_SESSION_ALLOWED          0x02
 
#define MODEM_STATUS_POLL_RESP_OK   0x01
 
#define MODEM_DC_SESSION_RESP_OK    0x02
 
/***********************************************
 * packet bufer size of Microcom modem message and
 * modem reply
 ***********************************************/
#define MAX_MCOM_MESSAGE_SIZE       270

/***********************************************
 * Modem Default configurations
 ***********************************************/
#define MODEM_BUFFER_SIZE_DEFAULT    100           /* 100 bytes */
#define MODEM_POLL_RETRY_DEFAULT     3             /* 3 times */
#define MODEM_POLL_TIME_DEFAULT      (12 * ONESEC) /* 12 seconds */
#define MODEM_NOT_FORCING_A_LAW      0xffff

/*************************************************
 * This is the maximum size of a line to be read 
 * from the user in AT command mode. 
 *************************************************/
#define MODEM_MGMT_MAX_AT_MODE_READ_LINE_SIZE 80

#define MAX_MODEM_REV_LEN               3
#define MAX_MODEM_STRING_LEN           80         /* 80 characters */ 
#define MAX_MODEM_SMALL_EVENT_SIZE     16
#define MAX_MODEM_LARGE_EVENT_SIZE     256
#define MODEM_OOB_DEBUG(modem_info)  (modem_oob_debug && \
                        ((modem_info)->modem_config & CFG_MODEM_DEBUG_OOB))

/*
 * Max values of modem states and call progress
 */


#define MAX_MODEM_SPEED_INDEX       18
#define MAX_MODULATION_TXT_INDEX    15
#define MAX_PROTOCOL_TXT_INDEX      05
#define MAX_COMPRESS_TXT_INDEX      03
#define MAX_MODEM_STATES            11
#define MAX_DIRECTION_TXT_INDEX     02
#define MAX_CALL_PROGRESS_VALUES    04
#define MAX_DISC_REASON_TXT_INDEX   36

/*
 * MAX number of times we will tolerate a bad status poll
 * event from a modem, before reseting oobp of the modem
 */
 
#define MAX_MODEM_BAD_EVENT_COUNT   03
 

/***********************************************
 * Modem call trace upload
 ***********************************************/
#define TRACE_ENABLE (modem_info->modem_config & CFG_MODEM_DEBUG_TRACE)
#define TRACE_ON_NORMAL (modem_info->trace_on & DEBUG_TRACE_NORM)
#define TRACE_ON_ABNORMAL (modem_info->trace_on & DEBUG_TRACE_ABNORM)
#define TRACE_ON_ALL (modem_info->trace_on & DEBUG_TRACE_ALL)
#define TRACE_NORMAL_REASON \
    ((modem_info->modem_mgmt_info->modem_status_info.disconnect_reason_local == DISC_USER_HANGUP) || \
    (modem_info->modem_mgmt_info->modem_status_info.disconnect_reason_local == DISC_REMOTE_HANGUP))
#define TRACE_IS_NORMAL_TRIG    ((TRACE_ON_NORMAL) && (TRACE_NORMAL_REASON))
#define TRACE_IS_ABNORMAL_TRIG  ((TRACE_ON_ABNORMAL) && !(TRACE_NORMAL_REASON))
#define TRACE_IS_ALL_TRIG       (TRACE_ON_ALL)
#define TRACE_TRIGGERED			(TRACE_IS_NORMAL_TRIG || TRACE_IS_ABNORMAL_TRIG || TRACE_IS_ALL_TRIG)
#define MAX_UPLOAD_TRACE			35	

/***********************************************
 * Number of Modem Speeds allowed or defined
 ***********************************************/
#define MAX_MODEM_LINE_SPEEDS             18

/***********************************************
 * Modem retrievable info
 ***********************************************/
typedef struct modem_status_info modem_status_info_t;
struct modem_status_info {
    int xmit_count;
    int rcv_count;
    uint tx_speed;
    uint rx_speed;
    uchar modem_type;
    char  modem_firmware_rev[MAX_MODEM_REV_LEN];
    char  modem_boot_firmware_rev[MAX_MODEM_REV_LEN];
    /*
     * rs232 Event
     */
    uchar tx_rx_data;
    uchar modem_rs232;
    uchar modem_rs232_change;
    /* 
     * Modem State Event
     */
    uchar modem_state;
    /*
     * Connection Event
     */
    uchar modulation;
    uchar orig_answer_status;
    uchar protocol;
    uchar compression;
    /*
     * Call Progress Event
     */
    uchar call_progress;
    /*
     * DTR Event
     */
    uchar dtr_status;
    /*
     * Modem Inactivity Event
     */
    uchar inactivity_status;
    /*
     * Analog Report Event
     */
    uchar tx_level;
    uchar rx_level;
    uchar signal_noise_ratio;
    /*
     * End Connection Event
     */
    uchar tx_retransmits;
    uchar rx_retransmits;
    uchar duration_hour;
    uchar duration_min;
    uchar duration_sec;
    uchar disconnect_reason_local;
    uchar disconnect_reason_remote;
};

/***********************************************
 * modem statistic counters
 ***********************************************/
typedef struct modem_statistics modem_statistics_t;
struct modem_statistics {
    ulong modem_ic_complete;
    ulong modem_ic_complete_shadow;
    ulong modem_oc_complete;
    ulong modem_oc_complete_shadow;
    ulong modem_ic_total;
    ulong modem_ic_total_shadow;
    ulong modem_oc_total;
    ulong modem_oc_total_shadow;
    ulong modem_no_dial_tones;
    ulong modem_no_dial_tones_shadow;
    ulong modem_dial_timeouts;
    ulong modem_dial_timeouts_shadow;
    ulong modem_no_carriers;
    ulong modem_no_carriers_shadow;
    ulong modem_link_failures;
    ulong modem_link_failures_shadow;
    ulong modem_protocol_errors;
    ulong modem_protocol_errors_shadow;
    ulong modem_lost_events;
    ulong modem_lost_events_shadow;
    ulong modem_timeouts;
    ulong modem_timeouts_shadow;
    ulong modem_reset_to_recover_oob;
    ulong modem_reset_to_recover_oob_shadow;
    ulong modem_75_connections;
    ulong modem_75_connections_shadow;
    ulong modem_300_connections;
    ulong modem_300_connections_shadow;
    ulong modem_600_connections;
    ulong modem_600_connections_shadow;
    ulong modem_1200_connections;
    ulong modem_1200_connections_shadow;
    ulong modem_2400_connections;
    ulong modem_2400_connections_shadow;
    ulong modem_4800_connections;
    ulong modem_4800_connections_shadow;
    ulong modem_7200_connections;
    ulong modem_7200_connections_shadow;
    ulong modem_9600_connections;
    ulong modem_9600_connections_shadow;
    ulong modem_12000_connections;
    ulong modem_12000_connections_shadow;
    ulong modem_14400_connections;
    ulong modem_14400_connections_shadow;
    ulong modem_16800_connections;
    ulong modem_16800_connections_shadow;
    ulong modem_19200_connections;
    ulong modem_19200_connections_shadow;
    ulong modem_21600_connections;
    ulong modem_21600_connections_shadow;
    ulong modem_24000_connections;
    ulong modem_24000_connections_shadow;
    ulong modem_26400_connections;
    ulong modem_26400_connections_shadow;
    ulong modem_28800_connections;
    ulong modem_28800_connections_shadow;
    ulong modem_31200_connections;
    ulong modem_31200_connections_shadow;
    ulong modem_33600_connections;
    ulong modem_33600_connections_shadow;

};

/***********************************************
 * modem event structure
 ***********************************************/
typedef struct modem_event modem_event_t;
struct modem_event {
    uchar frame_type;
    uchar event_type;
    uchar event_length;
    uchar *data_ptr;
};

/***********************************************
 * modem event history structure
 ***********************************************/
typedef struct modem_history_event modem_history_event_t;
struct modem_history_event {
    modem_history_event_t *next;
    modem_event_t event;
    sys_timestamp event_time;
};

/***********************************************
 * modem dc session structure
 ***********************************************/
typedef struct modem_dc_session modem_dc_session_t;
struct modem_dc_session {
    mcom_modem_dc_state_machine_t dc_state;
    queuetype mgmt_at_tomodemQ;
    queuetype mgmt_at_frommodemQ;
};

/***********************************************
 * Flags for clearing the DC session 
 ***********************************************/

/* Queues are flushed and at_mode_blocks are freed from earlier DC session,
 * allow new DC sessions 
 */
#define CLEARED_ALLOW_NEW_DC           0x0  

/* DC session can be terminated  */
#define TERMINATION_ALLOWED            0x1

/* Clear DC session after sending out all remaining messages to the terminal */
#define CLEAR_BUT_WAIT_FOR_OUTPUT_MSG  0x2

/***********************************************
 * modem management info table
 ***********************************************/
struct modem_mgmt_info_t_ {
    mcom_modem_state_machine_t state;
    uchar mgmt_port_status;
    ushort sequence_num;
    boolean dc_session_flg;
    uchar  clear_dc;
    modem_dc_session_t dc;
    boolean modem_in_progress;
    modem_status_info_t modem_status_info;
    modem_statistics_t modem_stats;
    semaphore lock_eventQ;
    queuetype *modem_history_eventQ;
};

/***********************************************
 * modem management packet structure
 ***********************************************/
typedef struct modem_mgmt_pkt_hdr modem_mgmt_pkt_hdr_t;
struct modem_mgmt_pkt_hdr {
    uchar modem_address;
    uchar c_field;
    uchar type;
    uchar length;
    uchar data[0]; 
};

typedef struct modem_mgmt_at_mode_block_ {
  struct modem_mgmt_at_mode_block_ *next;
  tt_soc *tty;                  /* Associated TTY socket */
  ushort slot_modem;            /* logical modem num: slot # + modem port # */
  modem_info_t *modem_info;
} modem_mgmt_at_mode_block;

typedef struct modem_mgmt_err_pkt_hdr modem_mgmt_err_pkt_hdr_t;
struct modem_mgmt_err_pkt_hdr {
    uchar modem_address;
    uchar c_field;
    uchar type;
    uchar length;
    uchar event;
};

#define GET_TTYSTR_STR_LEN 20
#define MODEM_MGMT_OVERHEADBYTES 4

#define MODEM_MGMT_HEADSTART(ptr) ((ptr)->datagramstart)
/*
 * externs
 */
extern void modem_mgmt_create_history_eventQ(modem_info_t *);
extern void modem_mgmt_exec_at_mode(modem_info_t *);
extern modem_mgmt_at_mode_block *modem_mgmt_find_active_at_mode_block(ushort, 
                                   ushort);
extern boolean modem_mgmt_check_index(ushort, char *);
extern int modem_recovery_time;
extern int modem_buffer_size;
extern int modem_poll_retry;
extern int modem_poll_time;
extern const int modem_speed[];
extern const char *const modulation_txt[];
extern const char *const protocol_txt[];
extern const char *const compress_txt[];
extern const char *const modem_state_txt[];
extern const char *const direction_txt[];
extern const char *const call_progress_txt[];
extern const char *const disc_reason_txt[];
extern modem_mgmt_at_mode_block *modem_mgmt_active_at_mode_blocks;
extern watched_boolean *at_response_ready;
extern chunk_type *modem_event_hdr_chunk;
extern chunk_type *modem_small_event_chunk;
extern chunk_type *modem_large_event_chunk;

#endif /* #__MODEM_MGMT_H__ */
