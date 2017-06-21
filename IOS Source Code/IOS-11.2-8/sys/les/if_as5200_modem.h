/* $Id: if_as5200_modem.h,v 3.1.68.7 1996/09/10 03:28:57 elem Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_modem.h,v $
 *------------------------------------------------------------------
 * if_as5200_modem.h -- Brasil Modem pool and modem information
 *
 * November, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_modem.h,v $
 * Revision 3.1.68.7  1996/09/10  03:28:57  elem
 * CSCdi54417:  modem busyout generates report as cleanup pending.
 * Delaying DTR
 * Branch: California_branch
 * The setting of the MODEM_AUTOCONFIG_2 flag to hold off the modem
 * state machine is moved off to modem_lock_tty().  This will prevent
 * the cleanup pending message in all situations when the CSM needs
 * to lock the tty line -- not just during modem maintainence activity.
 *
 * Revision 3.1.68.6  1996/09/09  20:56:45  jchlin
 * CSCdi66525:  ct1/rbs cannot busy back the CO trunk when theres no free
 * modem
 * Branch: California_branch
 * o       The bugfix will busy dsx0s back to CO thru RBS when there's
 *         no free modem to accept incoming calls.
 * o       The modem pool is changed to have multiple pools for different
 *         types of modems.
 * o       Add hidden debug commands.
 *
 * Revision 3.1.68.5  1996/09/02  08:37:05  styang
 * CSCdi61183:  Cannot tell hardware revision of cards from command line
 * Branch: California_branch
 * To show hardware version info for the modem card and T1/E1 card.
 *
 * Revision 3.1.68.4  1996/08/28  12:57:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.68.3  1996/08/26  23:08:49  wmay
 * CSCdi66907:  ISDN incoming calls getting rejected - enforce call ranges
 * for modem and dialer type calls
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/07/15  20:36:10  towu
 * CSCdi62430:  CT1 RBS is not enabled
 * Branch: California_branch
 *         o Code cleanup.
 *         o Create msg_dsx0.c for supporting CT1 RBS.
 *
 * Revision 3.1.68.1  1996/06/16  21:14:54  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:12:27  jchlin
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#if !defined(__IF_AS5200_MODEM_H__)
#define __IF_AS5200_MODEM_H__

#define MODEM_BITMAP_SIZE         16 
#define MODEM_PORT_OFFSET(port)   (port / MODEM_BITMAP_SIZE)
#define MODEM_PORT_BITMAP(port)   (1 << (port % MODEM_BITMAP_SIZE))
#define MAX_MODEM_BITMAP(modem)   (((modem)-1)/MODEM_BITMAP_SIZE + 1)

#define MAX_SLOTS                  3
#define MAX_M32_PER_SLOT           2
#define MAX_MODEM_MODULE_PER_SLOT  2
#define MAX_AM_SLOTS		   2
#define MAX_MODEM_PER_SLOT         128
#define MAX_DIGIT_COLLECT_PER_SLOT 2
#define MAX_MODEM_BITMAP_PER_SLOT  MAX_MODEM_BITMAP(MAX_MODEM_PER_SLOT)
#define MAX_TOTAL_M32              (MAX_SLOTS * MAX_M32_PER_SLOT)
#define MAX_TOTAL_SIGNAL_CHANNEL   ((MAX_SLOTS * MAX_M32_PER_SLOT) + MAX_SLOTS)
#define MAX_TOTAL_MODEM            (MAX_SLOTS * MAX_MODEM_PER_SLOT)
#define MAX_TOTAL_DIGIT_COLLECT    (MAX_SLOTS * MAX_DIGIT_COLLECT_PER_SLOT)
#define MAX_PHONE_NUM_LEN          65         /* rfc1696 */
#define DEFAULT_FREE_MODEM_THRESHOLD   6
#define OOB_PORTS_PER_SLOT	   4
#define AT_CMD_ANSWER_CALL         "ata\n"
#define CSM_EVENT_QUEUE_SIZE       500

/***********************************************
 * Configuration bit masks
 ***********************************************/
#define CFG_MODEM_STATUS_POLL      0x01
#define CFG_MODEM_DC_SESSION       0x02
#define CFG_MODEM_DEBUG_OOB        0x04
#define CFG_MODEM_DEBUG_CSM        0x08
#define CFG_MODEM_BAD              0x10
#define CFG_MODEM_HOLD_RESET       0x20
#define CFG_MODEM_DEBUG_TRACE      0x40
#define CFG_MODEM_SHUTDOWN         0x80
 
#define CFG_MODEM_CONFIG_DEFAULT   0x03
#define BRASIL_MODEM               0x41
 
/*
 * CSM definitions
 */
#define CSM_MODEM_UNIT(slot, port) (slot*MAX_MODEM_PER_SLOT + port)
#define CSM_MODEM_INFO(slot,port)  (modem_pool->modem_slot[slot]->modem_info[port])
#define CSM_MODEM_INFO_OR_NULL(slot,port) ((modem_pool && \
                                            modem_pool->modem_slot[slot]) ? \
                                           CSM_MODEM_INFO(slot,port) : NULL)
#define CSM_MODEM_SLOT(modem_info) ((modem_info)->modem_chnl.slot)
#define CSM_MODEM_PORT(modem_info) ((modem_info)->modem_chnl.channel)
#define CSM_MODEM_ADDRESS(modem_info)  ((modem_info)->modem_chnl.channel+1)
#define CSM_MODEM_ADDR_TO_PORT(modem_address) (modem_address - 1)

#define CSM_MODEM_POOL_CHECK_TIME            (6*ONESEC)
#define CSM_PERIODICAL_BACKGROUND_CHECK_TIME (ONEMIN/CSM_MODEM_POOL_CHECK_TIME)
#define CSM_DEFAULT_WATCH_DOG_TIME_OUT       (6 * ONEMIN)
#define CSM_MODEM_RESET_MINIMUM_TIME         (2 * ONESEC)   /* 2 secs */

#define PARK_ONLY_MODEM_TX        FALSE
#define PARK_BOTH_MODEM_TX_RX     TRUE

#define RC_SUCCESS                TRUE
#define RC_FAILURE                FALSE

#define CSM_BUSYOUT_DSX0         TRUE
#define CSM_UNBUSYOUT_DSX0       FALSE

#define BITS(pos)                  (((ushort) 1) << (pos))
#define BITL(pos)                  (((ulong) 1) << (pos))
#define CSM_STATUS_NONE           0
#define CSM_STATUS_UNLOCKED       0
#define CSM_STATUS_ACTIVE_WDT     BITL(0)   /* the csm wdt is enable */
#define CSM_STATUS_ACTIVE_CALL    BITL(1)   /* lock modem_info for active call */
#define CSM_STATUS_BUSYOUT_REQ    BITL(2)   /* whenever a modem state changes
                                             * request to BUSY_OUT during active 
                                             * call process, this flag will be set 
                                             * to indicate that the request is pending. 
                                             * After the modem becomes on-hook, the modem 
                                             * state will enter into BUSY_OUT state. 
                                             */
#define CSM_STATUS_BAD_MODEM      BITL(3)   /* lock a bad modem */
#define CSM_STATUS_BACK2BACK_TEST BITL(4)   /* lock modems in back to back test mode */
#define CSM_STATUS_MODEM_RESET    BITL(5)   /* the modem is under reset */
#define CSM_STATUS_DOWNLOAD_FILE  BITL(6)   /* the modem is downloading the firmware */
#define CSM_STATUS_DOWNLOAD_FAIL  BITL(7)   /* the firmware downloading fails */
#define CSM_STATUS_SHUTDOWN       BITL(8)   /* the modem is in shutdown state */
#define CSM_STATUS_BUSY           BITL(9)   /* internal csm lockout */
#define CSM_STATUS_MAX_LENGTH     10

#define CSM_IS_IDLE_STATE(modem_info)     ((modem_info)->csm_state == CSM_IDLE_STATE)
#define CSM_IS_CONNECTED_STATE(modem_info) (((modem_info)->csm_state == CSM_IC3_CONNECTED) || ((modem_info)->csm_state == CSM_OC6_CONNECTED))
#define CSM_IS_IC_STATE(modem_info)       ((modem_info)->csm_state & CSM_IC_STATE)
#define CSM_IS_OC_STATE(modem_info)       ((modem_info)->csm_state & CSM_OC_STATE)
#define CSM_IS_IC_OC_STATE(modem_info)    ((modem_info)->csm_state & (CSM_OC_STATE | CSM_IC_STATE))
#define CSM_IS_UNLOCKED(modem_info)       (!(modem_info)->csm_status)
#define CSM_IS_LOCKED(modem_info)         ((modem_info)->csm_status)
#define CSM_IS_ACTIVE_WDT(modem_info)     ((modem_info)->csm_status & CSM_STATUS_ACTIVE_WDT)
#define CSM_IS_ACTIVE_CALL(modem_info)    ((modem_info)->csm_status & CSM_STATUS_ACTIVE_CALL)
#define CSM_IS_BUSY_OUT(modem_info)       ((modem_info)->csm_status & CSM_STATUS_BUSYOUT_REQ)
#define CSM_IS_MODEM_BAD(modem_info)      ((modem_info)->csm_status & CSM_STATUS_BAD_MODEM)
#define CSM_IS_BACK2BACK_TEST(modem_info) ((modem_info)->csm_status & CSM_STATUS_BACK2BACK_TEST)
#define CSM_IS_MODEM_RESET(modem_info)    ((modem_info)->csm_status & CSM_STATUS_MODEM_RESET)
#define CSM_IS_DOWNLOAD_FILE(modem_info)  ((modem_info)->csm_status & CSM_STATUS_DOWNLOAD_FILE)
#define CSM_IS_DOWNLOAD_FAIL(modem_info)  ((modem_info)->csm_status & CSM_STATUS_DOWNLOAD_FAIL)
#define CSM_IS_SHUTDOWN(modem_info)       ((modem_info)->csm_status & CSM_STATUS_SHUTDOWN)
#define CSM_IS_BUSY(modem_info)           ((modem_info)->csm_status & CSM_STATUS_BUSY)

/*
 * Special derived information macros
 */
#define CSM_IS_MODEM_DEAD(modem_info)     ((modem_info)->csm_status &                       \
                                           (CSM_STATUS_BAD_MODEM | CSM_STATUS_MODEM_RESET | \
                                            CSM_STATUS_DOWNLOAD_FILE |                      \
                                            CSM_STATUS_DOWNLOAD_FAIL))
#define CSM_IS_MODEM_ALIVE(modem_info)    (!CSM_IS_MODEM_DEAD(modem_info))

#define CSM_IS_IDLE_READY(modem_info)     (CSM_IS_UNLOCKED(modem_info)       \
                                           && CSM_IS_IDLE_STATE(modem_info))
#define CSM_IS_IDLE_LOCKED(modem_info)    (CSM_IS_LOCKED(modem_info)       \
                                           && CSM_IS_IDLE_STATE(modem_info))
#define CSM_IS_BUSY_OUT_READY(modem_info) (CSM_IS_BUSY_OUT(modem_info)       \
                                           && CSM_IS_IDLE_STATE(modem_info))
#define CSM_IS_BUSY_OUT_PENDING(modem_info) (CSM_IS_BUSY_OUT(modem_info)     \
                                             && CSM_IS_IC_OC_STATE(modem_info))

#define CSM_DEBUG_ENABLED(modem_info)     (modem_info->modem_config & CFG_MODEM_DEBUG_CSM)

/*
 * CSM common control flags
 */
#define CSM_FLAG_USE_AT_CMD_ANSWER        BITS(0)
#define CSM_FLAG_USE_PRI_LINE             BITS(1)
#define CSM_FLAG_V110_MODEM               BITS(2)
#define CSM_FLAG_FAST_RING                BITS(3)
#define CSM_USE_AT_CMD_ANSWER(modem_info) (modem_info->csm_flags & CSM_FLAG_USE_AT_CMD_ANSWER)
#define CSM_USE_PRI_LINE(modem_info)     (modem_info->csm_flags & CSM_FLAG_USE_PRI_LINE)
#define CSM_USE_CT1_LINE(modem_info)     (!(modem_info->csm_flags & CSM_FLAG_USE_PRI_LINE))
#define CSM_IS_V110_MODEM(modem_info)    (modem_info->csm_flags & CSM_FLAG_V110_MODEM)
#define CSM_IS_FAST_RING(modem_info)     ((modem_info->csm_flags & \
					   CSM_FLAG_FAST_RING) &&  \
					  (modem_fast_answer))

/*
 * TTY flags
 */
#define MODEM_TTY_IN_USE(modem_info)     ((modem_info->modem_tty) ? \
                                          TTY_IN_USE(modem_info->modem_tty) \
                                          : FALSE)

/*
 * Modem Control Flags
 */
#define MODEM_CTRL_USE_ROUND_ROOBIN       BITL(0)
#define MODEM_CTRL_PRE_INIT_DONE          BITL(1)

/*
 * ISDN and NEAT related message and event constants.
 */
#define NO_CAUSE           0
#define INVALID_B_CHANNEL -1
#define UNKNOWN_B_CHANNEL  0xff
#define CSM_PRI_SLOT       0xff

typedef enum {
    DC_RESET_REQ = 1,
    DC_READY_RSP,
    DC_ABORT_RSP,
    DC_FIRST_DIGIT_RSP,
    DC_ALL_DIGIT_RSP,
    DC_EVENT_MAX_LENGTH
} dc_event_t;
 
/********************************************************************
 *
 */

typedef enum {
    CSM_CAUSE_SHUTDOWN_INTERFACE=BITS(15),
    CSM_CAUSE_SIGNAL_CHANNEL_NOT_UP,
    CSM_CAUSE_NO_DATA_CHANNEL
} csm_cause_t;

typedef enum csm_state_t_ {
    CSM_IDLE_STATE = BITS(8),
    
    CSM_IC_STATE   = BITS(9),
            CSM_IC1_RINGING, CSM_IC2_WAIT_FOR_CARRIER, CSM_IC3_CONNECTED,
            CSM_IC4_DISCONNECTING, CSM_IC_END_STATE,
            CSM_IC_STATE_MAX_LENGTH=(CSM_IC_END_STATE&~CSM_IC_STATE),
    
    CSM_OC_STATE   = BITS(10),
            CSM_OC1_REQUEST_DIGIT, CSM_OC2_COLLECT_1ST_DIGIT,
            CSM_OC3_COLLECT_ALL_DIGIT, CSM_OC4_DIALING,
            CSM_OC5_WAIT_FOR_CARRIER, CSM_OC6_CONNECTED,
            CSM_OC7_BUSY_ERROR, CSM_OC8_DISCONNECTING,
            CSM_OC_END_STATE,   
            CSM_OC_STATE_MAX_LENGTH=(CSM_OC_END_STATE&~CSM_OC_STATE)

} csm_state_t;

typedef enum csm_event_t_ {
   CSM_EVENT_NONE=0,
   /*
    * Error Handling Events
    */
   CSM_EVENT_WDT_TIMEOUT,

   /*
    * ISDN Events 
    */
   CSM_EVENT_ISDN_CALL,
   CSM_EVENT_ISDN_CONNECTED,
   CSM_EVENT_ISDN_DISCONNECTED,
   CSM_EVENT_ISDN_BCHAN_ASSIGNED,

   /*
    * T1 DSX0 Voice Events 
    */
   CSM_EVENT_DSX0_ENABLE_CHANNEL,
   CSM_EVENT_DSX0_DISABLE_CHANNEL,
   CSM_EVENT_DSX0_CALL,
   CSM_EVENT_DSX0_CONNECTED,
   CSM_EVENT_DSX0_DISCONNECTED,
   CSM_EVENT_DSX0_BCHAN_ASSIGNED,
   CSM_EVENT_DSX0_START_TX_TONE,
   CSM_EVENT_DSX0_START_RX_TONE,
   /*
    * Modem Events
    */
   CSM_EVENT_MODEM_OFFHOOK,
   CSM_EVENT_MODEM_ONHOOK,

   /*
    * Digit Collection Events
    */
   CSM_EVENT_DIGIT_COLLECT_READY,
   CSM_EVENT_GET_1ST_DIGIT,
   CSM_EVENT_GET_ALL_DIGITS,
   CSM_EVENT_ABORT_DIGIT_COLLECT,

   /*
    * Special Events
    */
   CSM_EVENT_SHUTDOWN_INTERFACE,
   /*
    * Event Length
    */
   CSM_EVENT_MAX_LENGTH
} csm_event_t;

typedef enum {
    CSM_FAIL_NONE=0,    /* should be zero */
    CSM_FAIL_WDT_TIMEOUT,
    CSM_FAIL_UNEXPECTED_ON_HOOK,
    CSM_FAIL_MISS_EXPECTED_ON_HOOK,
    CSM_FAIL_NO_DC_READY_RSP,
    CSM_FAIL_DC_ABORT_RSP,
    CSM_FAIL_SHUTDOWN_INTERFACE,
    CSM_FAIL_ISDN_CAUSE,
    CSM_FAIL_NO_SIGANL_CHANNEL,
    CSM_FAIL_BUSY_TONE,
    CSM_FAIL_RINGING,
    CSM_FAIL_MAX_LENGTH
} csm_fail_reason_code_t;

typedef enum {
    CSM_ELOG_PHONE_NUMBER_COLLECTED=0
} csm_event_log_t;

typedef enum {
   RING_STOP,
   RING_ON,
   RING_OFF_BEGIN,
   RING_OFF_END
} ring_state_t;

typedef enum {
   DC_STATE_IDLE,
   DC_STATE_READY_RSP_PENDING,
   DC_STATE_READY_RSP_RECEIVED
} digit_collect_state_t;

typedef enum {
   CSM_FREE_MODEM=0, /* normal free modems, must be 0 */
   CSM_FREE_V110_MODEM,
   MAX_CSM_FREE_MODEM_LIST
} free_modem_list_index_t;

/*****************************************
 * CT1 definitions:
 *   The changes in this area should also reflect in
 *   the file robbed_bit.h of NEAT firmware source code.
 */
typedef enum {
    EVENT_BIT_STATE_CHANGE=0,
    EVENT_ENABLE_CHANNEL,
    EVENT_DISABLE_CHANNEL,
    EVENT_CALL_IDLE,
    EVENT_CALL_DIAL_IN,
    EVENT_CALL_DIAL_OUT,
    EVENT_CHANNEL_LOCK,
    EVENT_CHANNEL_CONNECTED,
    EVENT_START_RX_TONE,
    EVENT_START_TX_TONE,
    EVENT_VOICE_ANS,
    EVENT_RBS_DEBUG_ON,
    EVENT_RBS_DEBUG_OFF,
    EVENT_MAX_INDEX
} signal_event_t;

/****************************************/

STRUCTURE_REF(modem_info_t);    /* les/if_as5200_modem.h */
STRUCTURE_REF(modem_pool_t);
typedef ushort (*csm_state_function_t) (modem_info_t*);
typedef volatile ushort io_reg16_t;

#define MDB_CNTL1_MDB_ENABLE              BITS(0)
#define MDB_CNTL1_ENABLE_BYTE_ACCESS      BITS(2)
#define MDB_CNTL1_CODEC_AU_LAW            BITS(3)
#define MDB_CNTL1_MDB1_LED                BITS(4)
#define MDB_CNTL1_MDB2_LED                BITS(5)

#define MDB_ISR1_INTERRUPT_PENDING        BITS(0)
#define MDB_ISR1_MDB1_INTERRUPT_PENDING   BITS(1)
#define MDB_ISR1_MDB2_INTERRUPT_PENDING   BITS(2)

#define RTEN_MDB1_ENABLE_20HZ_RING_PULSE  BITS(0)
#define RTEN_MDB2_ENABLE_20HZ_RING_PULSE  BITS(1)

typedef struct csm_event_queue_t_ {
    modem_info_t *modem_info;
    csm_event_t   csm_event;
    ushort        cause;
} csm_event_queue_t;

typedef struct modem_reg_t_{
         /* 
          * When updating this structure, please make sure that
          * all of offsets stay the same!!!
          *
          * slot base byte offset 0x0 
          */
	 io_reg16_t mdb_id;
	 io_reg16_t mdb_rev;
         io_reg16_t res1;
         io_reg16_t mdb_control1;
         io_reg16_t res2;
         io_reg16_t mdb_isr1; /* modem board interrupt status register1 */
         io_reg16_t rten;     /* ring tone enable register */
         io_reg16_t res3[4];
         io_reg16_t mdb_eepm;
         io_reg16_t res4[4];

         /* 
          * slot base byte offset 0x20 
          */
         io_reg16_t ring_indicator[MAX_MODEM_BITMAP_PER_SLOT];
         const io_reg16_t oh_state[MAX_MODEM_BITMAP_PER_SLOT];
         io_reg16_t oh_int_status[MAX_MODEM_BITMAP_PER_SLOT];
         io_reg16_t oh_int_enable[MAX_MODEM_BITMAP_PER_SLOT];
         io_reg16_t modem_reset[MAX_MODEM_BITMAP_PER_SLOT];
} modem_reg_t;

#define VENDOR_BANNER_CHARLEN			24
#define MODEM_MODULE_VER_NUM_LEN                4
#define MODEM_MODULE_VENDER_CODE_LEN            2
#define MAX_MODEM_MODULE_COOKIE_LEN             32  
/* 32 is the size of modem module cookie, can get */
/* from strlen(modem_module_cookie_t)             */

#define VENDOR_CODEC_U_LAW_HIGH			0x01

typedef struct modem_carrier_cookie_ {
    uchar	number_of_ports;
    uchar	max_modules;
    uchar	max_oob_ports;
    uchar	reserved[29];
} modem_carrier_cookie_t;

typedef struct modem_module_cookie_ {
    uchar	number_of_modems;
    uchar	option_bits;
    char	rev_num[4];
    char	vendor_model_number[2];
    /* add extra byte for null terminator; OK since it's last element */
    char	vendor_banner[VENDOR_BANNER_CHARLEN + 1];
} modem_module_cookie_t;

typedef struct modem_cookie_ {
    modem_carrier_cookie_t	carrier;
    modem_module_cookie_t	module[0];
} modem_cookie_t;

/*
 * Character string limits
 */
#define MAI_USER_INPUT_STRLEN	80
#define MAI_MAX_MODEM_LIST	(MAI_USER_INPUT_STRLEN / 2)
#define MAI_MODEM_RSPLEN	256
/* replystrg length with 10 byte to save AT^Pn string */
#define MAI_REPLYSTRGLEN	(MAI_MODEM_RSPLEN + 10) 

/*
 * Processing limits
 */
#define MAI_MAX_RUNS		20
#define MAI_TIMEOUT_CHECK_INTERVAL	(ONESEC/4)
#define MAI_TIMEOUT_ATD_ATA_CONNECT	(90*ONESEC)
#define MAI_TIMEOUT_ATA_HEADSTART	(2*ONESEC)
#define MAI_TIMEOUT_B2B_DATA_TRANSFER	(5*ONESEC)
#define MAI_TIMEOUT_DL_WAIT_RETRY	(5*ONESEC)
#define MAI_TIMEOUT_DL_TX_COMPLETE	(10*ONESEC)

#define MAI_TIMEOUT_NORMAL_AT_CMD	(3*ONESEC)
#define MAI_TIMEOUT_NOT_CONNECTED_AT_CMD	(5*ONESEC)
#define MAI_TIMEOUT_WAIT_NEXTSTEP	(ONESEC)

#define MAI_TIMEOUT_QUICK_WAIT		(MAI_TIMEOUT_CHECK_INTERVAL)
#define MAI_TIMEOUT_ECHO_RESPONSE	(ONESEC)
#define MAI_TIMEOUT_INTER_CHAR		(ONESEC/2)

#define MAI_AT_MAX_RETRY	3
#define MAI_AT_DL_MAX_RETRY     4
#define MAI_DL_MAX_RETRY        3
#define MAI_AT_AFTER_RESET	20
#define MAI_ATP_MAX_RETRY	5

/*
 * Buffer Size for testing tftp and file presence first
 *  - basically 2 TFTP packets
 */
#define MAI_MAX_DUMMYDATA	(TFTP_MAXDATA * 2)

/*
 * Buffer Size to hold download firmware file
 *   Assumes:  128K Modem Flash * 2 char/byte
 *             +16 for record headers
 */
#define MAI_MAX_BUFFSIZE	((256 + 16) * ONE_K)
#define MAI_MAX_DLBUFFERS	4

/*
 * Modem Command Limits
 */

typedef enum mai_cmd_ {
    MAI_NONE_PENDING,
    MAI_PENDING_DL,
    MAI_PENDING_B2B,
    MAI_PENDING_PWRON,
    MAI_PENDING_PWRON_B2B,
    MAI_PENDING_RESET,
    MAI_MAX_PENDING_MAINTN,
} mai_cmd_t;

#define MAI_COMMON_TASK		0x80

/*
 * States action processing
 */
typedef enum mai_state_ {
    MAI_IDLE,
    /* Downloading */
    MAI_DL_INIT,
    MAI_DL_INIT_MODEM,
    MAI_DL_COMPLETE,

    /* Power ON/Reset Initialization */
    MAI_PWRON_INIT,

    /* Recover from faliure by resetting */
    MAI_RESET_MODEM,

    MAI_MAX_STATE,

    /* Generic Ops Sub-States */

    /* Sending AT command */
    MAI_CM_WAIT_CR = MAI_COMMON_TASK,
    MAI_CM_WAIT_CLR,
    MAI_CM_WAIT_ECHO,
    MAI_CM_WAIT_RESP,

	/* Back2Back test */
    MAI_B2B_INIT,
    MAI_B2B_CONNECTING,
    MAI_B2B_DATA,

    MAI_COMMON_MAX_STATE_FTN
} mai_state_t;

/* Always the first step */
#define MAI_STP_BEGIN		0

/* statbits definitions */
#define MAI_USER_BUSYOUT	0x0001
#define MAI_MAINTN_PENDING	0x0002
#define MAI_RX_SIGNAL_REQ 	0x0004  /* tty rsp signal requested */
#define MAI_B2B_ATD		0x0008  /* the dialer */
#define MAI_B2B_ATA	 	0x0010  /* the answer state */	
#define MAI_FAILED_LAST_B2B 	0x0020  /* failed last b2b modem test */
#define MAI_PWRON_B2B_SUCCESS 	0x0040  /* succeeded power-on b2b modem test */
#define MAI_STRAY_QUEUE_EVENT 	0x0080  /* stray response event in queue */

typedef struct mai_range_ {
    ushort	startslot, startport;	/* starting modem address */
    ushort	endslot, endport;	/* ending modem address */
} mai_range_t;

/* Data buffer to hold firmware code */
typedef struct mai_dl_buffer_ {
    struct mai_dl_buffer_ *nextbuf;
    int		bytecnt;
    uchar	data[MAI_MAX_BUFFSIZE];
} mai_dl_buffer_t;

/*
 * Download Information
 *   - includes the flash system's data structure because
 *     of interface to flash.
 */
typedef struct mai_dl_info_ {
    struct mai_dl_info_ *nextq;
    flash_copy_parms_t fparms;
    queuetype	bufqueue;
    int		bsize;
    int		modemcnt;
} mai_dl_info_t;

/*
 * Queue event message
 */
typedef struct modem_q_info_t_ {
    struct modem_q_info_t	*next;
    modem_info_t		*modem_info;
    uint			type;
} modem_q_info_t;

/*
 * Vendor specific parameters
 */
typedef struct modem_vendor_defs_ {
    char	banner[VENDOR_BANNER_CHARLEN + 1];
    char	model_number[3];
    boolean	ring_tone_enabled;
    void	(*init_ftn)(modem_info_t *);
    boolean	(*(*maintn_ftn)[])(modem_info_t *, int*);
} modem_vendor_defs_t;

/* use motherboard id address for identifying BRASIL platorm */
#define IS_BRASIL_PLATFORM  (cpu_type == CPU_CANCUN && \
			touch_device((uchar *)ADRSPC_BRASIL_MB_CONFIG_REG))
#define MODEM_VENDOR_NAME(modem_info)	((modem_info)->cookie->vendor_banner)

/*
 * X2444 Serial Eeprom Defs
 */
#define MODEM_X2444_DI		0x0001
#define MODEM_X2444_DO		0x0002
#define MODEM_X2444_SK		0x0004
#define X2444_CE_CARRIER	0x0008
#define X2444_CE_MODEMBD1	0x0010
#define X2444_CE_MODEMBD2	0x0018

#define MAX_WORDS_PER_X2444	16
#define X2444_NUM_CMDBITS_OUT	8
#define X2444_NUM_DATABITS_IN	16
#define X2444_NO_DATABITS_IN	0

typedef enum modem_vendor_type {
    MODEM_MICROCOM_ID = 1,
    MODEM_ROCKWELL_ID,
    MODEM_USR_ID,
    MODEM_NEC_ID,
    MODEM_TELEBIT_ID
} modem_vendor_type_t;

/***********************************/
/* MODEM DOWNLOAD SPECIFIC COUNTERS*/
/***********************************/

typedef union modem_dl_counters_t_ {
    struct modem_microcom_counters_ {
        uchar    mai_dl_retry_stp_ru_there;     /* download retry counter */
        uchar    mai_dl_retry_stp_xfer_done;    /* download retry counter */
        uchar    mai_dl_retry_xfer_timedout;    /* download retry counter */
        uchar    mai_dl_retry_stp_version_done; /* download retry counter */
        uchar    mai_dl_retry_stp_atp_done;     /* download retry counter */
        uchar    mai_dl_ppp_mode;               /* PPP mode at dl time    */
    } modem_microcom_counters;
} modem_dl_counters_t;

#define MCOM_MODEM_CTR(m)     ((m)->mai_dl_counters).modem_microcom_counters

extern modem_pool_t *modem_pool;

extern modem_info_t *modem_ttynum_to_modem_info(int);

typedef struct dchan_info_t_ {
    /* 
     * dchan_idb contains pri slot(0..2) and pri number(0..1) 
     * if dchan_idb == NULL, it's a channelized T1 voice and
     * then use slot info in this structure to identify the
     * signaling entity.
     */
    hwidbtype        *dchan_idb;
    ushort            current_oc_call_id;    /* value ranges from 0xa000 to 0xafff */

    /* 
     * used for channelized T1 RBS voice 
     */
    ushort            slot;               
    ushort            num_of_channel;     
    ushort            num_of_busyout;     
} dchan_info_t;

typedef struct digit_collect_t_ {
    digit_collect_state_t collecting_digit;
    uchar                 message_id;
    modem_info_t         *modem_info;
    ushort                wdt_timeout_count;
    ch_info_t             dc_chnl;
} digit_collect_t;

struct modem_info_t_ {

/* safetess way to get hw_idb is from tty structure */ 
#define tty_hwidb	modem_tty->tty_idb

    tt_soc           *modem_tty;
    tt_soc           *mgmt_tty;
    boolean          initialized;
    modem_pool_t     *modem_pool;
    modem_vendor_defs_t *vendor_info;
    ushort            unit;     
    uchar	      module_num;     

    /**************************************
     * CSM Data area
     */
    modem_info_t     *remote_b2b_modem_info;
    digit_collect_t  *dc;
    ushort            modem_mask;
    ushort            modem_port_offset;
    csm_state_t       csm_state;
    csm_state_function_t csm_event_proc;
    ulong             csm_status;
    ushort            invalid_event_count;
    ushort            wdt_timeout_count;
    sys_timestamp     wdt_timestamp_started;
    sys_timestamp     modem_reset_started;
    ch_info_t         pri_chnl, modem_chnl;
    ushort            csm_flags;

    /*
     * List links
     */
    list_element      atcmd_answer_link;
    list_element      waiting_dial_link;
    list_element      waiting_bchan_link;
    list_element      free_modem_link;
    list_header      *free_modem_header_ptr;

    /* 
     * The nice busy out event process
     */
    watched_queue    *busyout_rsp_queue;
    void             *busyout_rsp_msg;   /* requester supplies the response message */

    /*
     * The following variables are valid only in the duration of the call 
     * processing and connection.
     *
     * dchan_idb_start_index and dchan_idb_index are not used for incoming call
     */
    ushort            dchan_idb_start_index;
    ushort            dchan_idb_index; 
    hwidbtype        *dchan_idb;
    hwidbtype        *bchan_idb;
    ushort            call_id;
    ushort            dsx0_slot;        /* used for CT1 */
    ushort            dsx0_controller;  /* used for CT1 */
    short             bchan_num;
    csm_event_t       csm_event;
    ushort            cause;

    /* 
     * null terminated ascii string phone numbers
     */
    uchar             calling_party_num[MAX_PHONE_NUM_LEN];
    uchar             called_party_num[MAX_PHONE_NUM_LEN];
    
    /*
     * Modem Statistics maintained by CSM
     */
    csm_fail_reason_code_t  connection_fail_reason;
    csm_state_t       fail_csm_state;
    ulong             stat_ring_no_answer;
    ulong             stat_ic_failure;
    ulong             stat_ic_complete;
    ulong             stat_dial_failure;
    ulong             stat_oc_failure;
    ulong             stat_oc_complete;
    ulong             stat_oc_busy;
    ulong             stat_oc_no_dial_tone;
    ulong             stat_oc_dial_timeout;
    ulong             stat_remote_link_disc;
    ulong             stat_busyout;
    ulong             stat_modem_reset;
    sys_timestamp     stat_call_duration_started;
    sys_timestamp     stat_call_duration_ended;
    sys_timestamp     stat_total_call_duration;

    /**************************************
     * Modem Management Async information
     */

    modem_module_cookie_t	*cookie;

    modem_q_info_t	rsp_info;
    modem_q_info_t	pwron_b2b_qlink;

    mai_cmd_t		maintn_head;
    mai_cmd_t		maintn_pending[MAI_MAX_PENDING_MAINTN];

    ushort		mai_statbits;

    mai_state_t		mai_state;
    mai_state_t		mai_laststate;	/* save last state before doing */
    uchar		mai_step;

    uchar               mai_retry;	     /* retry counter          */
    modem_dl_counters_t mai_dl_counters;    /* Pointer to modem specific retry counters */
    
    list_element	tmonext;	/* time-out linklist */
    sys_timestamp	dallytimer;

			/* modem output string info */
    int			save_numchar;
    char		*save_ptr;

    int			save_txspeed;
    int			save_rxspeed;
    int			save_stopbits;
    int			save_databits;
    int			save_parity;
    ulong		save_capabilities;
    ulong		save_capabilities2;

    			/* rxservice input info */
    int			rsp_bcount;
    char		*cur_rsptr;
    char		*replystrg;
    int			rsp_waittime;

			/* initializier function */
    void		(*do_init_ftn)(modem_info_t *);

			/* state handler functions */
    boolean		(*(*do_ftn)[])(modem_info_t *, int*);

    mai_dl_info_t	*download_info;	/* ptr to download info */
    mai_dl_buffer_t	*dlbufptr;	/* ptr to download text storage */
    uchar		*cur_txptr;
    int			tx_bcount;

    modem_info_t	*alt_modem;
    int			mai_b2b_max;
    int			mai_b2b_good;
    int			mai_b2b_cnt;

    /**************************************
     * Modem Management information
     */
     uchar             modem_config;
     uchar             trace_on;
     uchar             last_called_number[MAX_PHONE_NUM_LEN];
     uchar             last_calling_number[MAX_PHONE_NUM_LEN];
     uchar             last_call_direction;
     modem_mgmt_info_t *modem_mgmt_info;
     sys_timestamp     last_clear_modem_counter;
     boolean           mgmt_support;

};

typedef struct modem_slot_t_ {
    modem_reg_t      *modem_reg;
    modem_info_t     *modem_info[MAX_MODEM_PER_SLOT];
    ushort            modem_exist_mask[MAX_MODEM_BITMAP_PER_SLOT];
    ushort            ring_map[MAX_MODEM_BITMAP_PER_SLOT];
    ushort            ring_map_ignored[MAX_MODEM_BITMAP_PER_SLOT];
    ushort            total_modem_num;
    ushort            total_tty_ports;
    ushort            total_bitmap_modem_num;
    sys_timestamp     modem_reset_minimum_ticks;
    modem_cookie_t    *modem_cookie;    
    tt_soc	      *mgmt_oob_tty[OOB_PORTS_PER_SLOT];
    int		      good_modem_count[MAX_MODEM_MODULE_PER_SLOT];
    int		      mai_pwron_b2b_total;
    int		      mai_pwron_b2b_undetermined;
    int		      mai_pwron_b2b_max_modems;
    int		      mai_pwron_b2b_good_held;
    modem_info_t      *mai_pwron_b2b_next_modem;
    queuetype	      mai_pwron_b2b_good_queue;
    queuetype	      mai_pwron_b2b_recheck_queue;

} modem_slot_t;

struct modem_pool_t_ {
    ulong             modem_control_flags;
    boolean           modem_mgmt_subsys_running;
    int		      max_slots;

    /**************************************
     * CSM Data area
     */
    modem_slot_t     *modem_slot[MAX_SLOTS];
    dchan_info_t     *dchan_info[MAX_TOTAL_SIGNAL_CHANNEL];
    ushort            current_dchan_idb_index;
    ushort            max_dchan_idb_index;
    ushort            max_dchan_idb_num;
    ulong             wdt_timeout_val;     /* in system clock ticks */
    timer_t          *wdt;
    pid_t             background_check_task_pid;
    ushort            next_slot_search;
    ushort            next_port_search;    
    ushort            background_check_countdown;
    ushort            min_free_modem_threshold;
    ushort            total_free_rbs_timeslot;
    ushort            total_busy_rbs_timeslot; /* busy out */

    /*
     * ring indicator control
     * ring_map is the same mapping used in the modem cards. ring_map's bit will be 
     * set when the corresponding modem needs a ring indication.
     */
    timer_t          *ring_timer; /* a single timer to control all of RI pins to modems. */
    ring_state_t      ring_state;
    ushort            ringer_num;    /* the number of modems are ringing */

    /*
     * dialing control:
     *   since digit controller is a very limitted resource,
     *   digit_collect_pool will be pre-allocated.
     */
    digit_collect_t   digit_collect_pool[MAX_SLOTS][MAX_DIGIT_COLLECT_PER_SLOT];
    digit_collect_t  *digit_collect_list[MAX_TOTAL_DIGIT_COLLECT];
    ushort            max_digit_collect_num[MAX_SLOTS];;
    ushort            current_digit_collect_index;
    ushort            max_digit_collect_index;
    ushort            free_digit_collector_num;
    
    /*
     * List headers
     */
    list_header       atcmd_answer_header;
    list_header       wait_for_dialing_header;
    list_header       wait_for_bchan_header;
    list_header       free_modem_header[MAX_CSM_FREE_MODEM_LIST];

    /**************************************
     * Modem Management Async information
     */
    ulong             ttybase;

    ushort            encoding;
    pid_t		mai_maintn_process_pid;
    int			mai_maintn_cnt;
    sys_timestamp	mai_global_timer;
    watched_queue	*mai_command_queue;
    list_header		mai_tmolist;

    /* to quickly find modem_info given the tty structure */
    modem_info_t	*(*tty2modem)[];
};

/***********************************************
 * modem group structure
 ***********************************************/
typedef struct modem_group modem_group_t;
struct modem_group {
    hwidbtype *hwidb;
    modem_group_t *next;
};

/* modem management process status */
#define MGMT_STATUS_GOOD_DATA	0
#define MGMT_STATUS_NO_ERROR	0
#define MGMT_STATUS_CRC_ERROR   1
#define MGMT_STATUS_DATA_ERROR  2
#define MGMT_STATUS_NO_DATA	3

#define MGMT_OK			0 	/* transmit ok */
#define MGMT_FAIL		-1	/* transmit fail */

#define MAI_B2B_MAX_WAIT		(3*8)		/* 3 secs */

#define MGMT_MODEM_MODULE_0_OK  0x0010
#define MGMT_MODEM_MODULE_1_OK  0x0020

static inline modem_vendor_type_t get_modem_vendor_type_inline (modem_info_t *modem_info)
{
    return (modem_vendor_type_t) (((modem_info->cookie->vendor_model_number[0]-'0')*10)+
                                (modem_info->cookie->vendor_model_number[1]-'0'));
}


extern void csm_modem_interrupt(int slot);
extern void dsx1_mail_from_neat(ushort event, uchar mid, uchar slot, uchar unit);

extern boolean csm_set_busy_out_status(modem_info_t *modem_info, watched_queue *event_queue,
                                       void *busyout_rsp);
extern boolean csm_clear_busy_out_status(modem_info_t *modem_info);
extern boolean csm_set_download_file_status(modem_info_t *modem_info);
extern boolean csm_clear_download_file_status(modem_info_t *modem_info);
extern boolean csm_set_download_fail_status(modem_info_t *modem_info);
extern boolean csm_clear_download_fail_status(modem_info_t *modem_info);
extern boolean csm_set_bad_modem_status(modem_info_t *modem_info);
extern boolean csm_clear_bad_modem_status(modem_info_t *modem_info);
extern boolean csm_set_modem_reset_status(modem_info_t *modem_info);
extern boolean csm_clear_modem_reset_status(modem_info_t *modem_info, boolean wait_for_reset);
extern boolean csm_set_shutdown_status(modem_info_t *modem_info);
extern boolean csm_clear_shutdown_status(modem_info_t *modem_info);
extern boolean csm_set_busy_status(modem_info_t *modem_info);
extern boolean csm_clear_busy_status(modem_info_t *modem_info);
extern boolean csm_set_modem_back2back_test(modem_info_t *modem_info1, 
                                            modem_info_t *modem_info2);
extern boolean csm_clear_modem_back2back_test(modem_info_t *modem_info1, 
                                              modem_info_t *modem_info2);

extern void csm_show_modem_info(modem_info_t *modem_info);
extern void csm_clear_modem_info_counter(modem_info_t *modem_info);
extern boolean csm_do_pending_request(modem_info_t *modem_info);
extern void csm_setup_capability_flags(modem_info_t *modem_info);
extern void dsx1_mail_from_neat (ushort event, uchar mid, uchar slot, uchar unit);

extern void mai_modem_pool_init(modem_slot_t *modem_slot);
extern void mai_line_init(void);
extern void mai_busyout_a_modem(modem_info_t *modem_info,
                                boolean busyout_requested);
extern void mai_restart_modem(modem_info_t *modem);
extern boolean mai_back2back_test(modem_info_t *modem1, modem_info_t *modem2);
extern boolean mai_recover_modem_from_failure(modem_info_t *modem_info);
extern int modem_oob_request_send( modem_info_t *modem, paktype *req_buf,
                        paktype **rsp_buf, int *status, watched_boolean **event);
extern void modem_oob_request_reset(modem_info_t *modem);
extern void modem_lock_tty(modem_info_t *modem_info);
extern void modem_unlock_tty(modem_info_t *modem_info);
extern boolean modem_shutdown(modem_info_t *modem_info, boolean sense);
extern boolean modem_bad(modem_info_t *modem_info, boolean sense);
extern boolean modem_reset(modem_info_t *modem_info, boolean sense, 
                           boolean wait_for_reset);
extern boolean modem_mgmt_start_closing_dc_session(boolean, ushort, ushort, modem_info_t *);
extern boolean modem_startup_test;
extern boolean modem_fast_answer;
extern boolean mai_clear_maintns(modem_info_t *);
extern void mai_enable_poweron_b2b_test(void);

extern int modem_mgmt_csm_debug;
extern modem_group_t *modem_group_list;
extern modem_vendor_defs_t mai_modem_vendor_specific_table[];

#endif /* !defined(__IF_AS5200_MODEM_H__) */
