/* $Id: if_c3000_csu_dsu.h,v 3.2.2.2 1996/08/28 12:57:30 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_csu_dsu.h,v $
 *------------------------------------------------------------------
 * if_c3000_csu_dsu.h - Defines for Integrated CSU/DSU driver code
 *
 * March 1996, Shalabh Agarwal
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_csu_dsu.h,v $
 * Revision 3.2.2.2  1996/08/28  12:57:30  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.2.1  1996/08/07  09:01:16  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2  1996/03/29  03:49:47  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:25  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_C3000_CSU_DSU_H__
#define __IF_C3000_CSU_DSU_H__

#include "../les/parser_defs_csu_dsu.h"
/*
 * Wan Module Miscellanous Register
 */

#define MISC_MOD_TYPE_ID3_SHIFT 2
#define BOA_MOD_TYPE_ID_MASK	0x000f

/*
 * CSU/DSU ALARM and READY interrupt enable/disables
 */
#define RDY_SIGNAL_HIGH(a)              (*a & BOA_MISC_READY_STATUS) ? TRUE:FALSE
#define ALM_SIGNAL_HIGH(a)		(*a & BOA_MISC_ALARM_STATUS) ? TRUE:FALSE

#define MODULE_RESET(a)			(*a |= BOA_MISC_CSU_DSU_RESET)
#define MODULE_UNRESET(a)		(*a &= ~BOA_MISC_CSU_DSU_RESET)
	
#define BOA_DSU_IRQ_LEVEL	3
#define MAX_SERIAL_PORTS	2
/*
 * Module types detected through hardware pinouts 
 */
#define BOA_SW56_2W_ADTRAN_DSU_ID	8
#define BOA_SW56_4W_ADTRAN_DSU_ID	9
#define BOA_FT1_DIGITAL_LINK_DSU_ID	10
#define BOA_NO_MODULE			15

enum MODULE_TYPES {
	NO_MODULE,   /* Nothing in WAN slot or fixed slot 2500 */
	SERIAL_5IN1_MODULE, /* The synchronous 5-in-1 style adaptor */
	FT1_CSU_DSU, /* Integrated T1 Digital Link module */
	SW56_4W_CSU_DSU,  /* 4-wire Switched 56k Adtran module */
	SW56_2W_CSU_DSU,  /* 2-wire Switched 56k Adtran module */
	UNRECOGNISED_MODULE	
};
	
/* Driver data structures for CSU/DSUs */
enum {WAN_PORT_0 = 1,
      WAN_PORT_1};

#define W_BOOLEAN_ALM_TYPE      WAN_PORT_1+1
#define SEMAPHORE_DSU0_TYPE     W_BOOLEAN_ALM_TYPE+WAN_PORT_0
#define SEMAPHORE_DSU1_TYPE     SEMAPHORE_DSU0_TYPE+1

/*
 * CSU-DSU register read/write timing figures
 */
#define T1_RDY_INTERBYTE_WAIT           10      /* microsecs */
#define T1_INTERBYTE_READ_WAIT          10	/* microsecs */
#define T1_INTERBYTE_WRITE_WAIT         100	/* microsecs */

#define SW56_INTERBYTE_READ_WAIT        300	/* microsecs */
#define SW56_INTERBYTE_WRITE_WAIT       300	/* microsecs */

#define DSU_POST_COMMAND_FIXED_WAIT	  5     /* millisecs */
#define DSU_POST_COMMAND_WAIT           400	/* microsecs */
#define DSU_WAIT_RDY_SIGNAL_HIGH	100	/* millisecs */
#define DSU_MAX_RDY_SIGNAL_RETRIES	  5
#define DSU_MAX_POST_CMD_WAIT_RETRIES	  4
#define DSU_MAX_INTERBYTE_RDY_RETRIES	 20 


/*
 * CSU-DSU timeouts
 */
#define CSU_DSU_LOCK_WAITTIME		3*ONESEC
#define T1_RDY_MAX_REASSERT_WAIT	20	/* T1_RDY_INTERBYTE_WAITs */
#define DSU_MAX_POWERCYCLE_TIME		30	/* seconds to wait for completion */
						/* of module reset		  */
#define DSU_MAX_MODULE_RESET_RETRIES	3
#define DSU_RESET_HOLD_TIME		150
#define DSU_RESET_WAIT_TIME		400
#define DSU_SELFTEST_COMPLETION		5*ONESEC
#define DSU_REM_LOOPBACK_START		5*ONESEC
#define DSU_NUM_REG_TIMEOUTS_RESET	2

/*
 * CSU-DSU individual alarm data
 */
typedef struct dsu_alarm_data_ {
    ulong		count;
    sys_timestamp	current_duration;
    sys_timestamp	last_occurrence;
    ulong		last_duration;
} dsu_alarm_data_t;


/* 
 * CSU-DSU alarm events
 */
enum T1_ALARM_EVENTS {
        T1_BPV_THRSLD_EXCEEDED, 
	T1_CRC_THRSLD_EXCEEDED, 
	T1_OOF_THRSLD_EXCEEDED,
        T1_CARRIER_LOSS, 
	T1_FRAME_SYNC_LOSS, 
	T1_RXING_AIS, 
	T1_RXING_RAI,
        T1_INT_PROTOCOL_ERROR,
	T1_REMOTE_ACTIVATED_LOOPBACK,
	T1_TESTPATTERN_SYNC,
	T1_MAX_ALARM_EVENTS
};

enum SW56_ALARM_EVENTS {
        SW56_RXING_OOS_OOF, 
	SW56_CARRIER_LOSS, 
	SW56_CURRENT_LOSS,
        SW56_FRAME_SYNC_LOSS, 
	SW56_RATE_ADAPTING, 
	SW56_CALL_STATUS,
        SW56_CSU_DSU_TELCO_LOOPUP, 
	SW56_LOOPUP_FRM_REM_DSU,
	SW56_MAX_ALARM_EVENTS
};

#define MAX_ALARM_EVENTS	T1_MAX_ALARM_EVENTS

typedef struct csu_dsu_instance csu_dsu_instance_t;
struct csu_dsu_instance {
int			module_type;
int			interface_number;
hwidbtype		*idb;
uchar 			*wan_misc_reg;
uchar 			*command_reg;
uchar 			*data_reg;
boolean			module_init;
boolean 		module_bringup;
ushort			module_int_protocol_rev;
ushort 			mod_highest_int_protocol_rev;
watched_semaphore 	*csu_dsu_lock;
mgd_timer		dsu_diagnostic_timer;

int 			dsu_interwrite_wait_time;
int			dsu_interread_wait_time;

	/* Alarm processing stuff */
ushort 			dsu_network_alarm_status;
ushort 			dsu_alarm_mask;
char 			* const *dsu_alarm_messages;

boolean 		alarm_asserted;
sys_timestamp		last_counter_reset;
int			num_module_resets;
sys_timestamp		time_module_reset;
dsu_alarm_data_t 	alarm_counts[MAX_ALARM_EVENTS];
sys_timestamp		alarm_event_timestamp;
int			dsu_reg_access_timeouts;

	/* Diagnostics parameters */
int			current_loopback_command;
int			dsu_loopback_type;
int			current_loopback_state;
uchar			dsu_loopback_test_pattern;
ulong			dsu_loopback_user_pattern;
boolean			dsu_loopback_synced_on_pattern;
sys_timestamp		loopback_start_time;
sys_timestamp		loopback_pattern_sync_duration;
sys_timestamp 		selftest_start_time;
ulong			dsu_loopback_bit_errors;
ulong			bert_mantissa;
int			bert_exponent;
sys_timestamp		bert_remote_loopback_start;

int 			last_loopback_command;
int 			dsu_last_loopback_type;
int 			dsu_last_loopback_pattern;
sys_timestamp		last_loopback_start_time;
sys_timestamp		last_loopback_pattern_sync_duration;
ulong                   dsu_last_loopback_user_pattern;
ulong                   dsu_last_loopback_bit_errors;
boolean                 dsu_last_remote_loopback_passed;
boolean			dsu_last_loopback_synced_pattern;
sys_timestamp		last_loopback_end_time;

/* Module type specific function calls */
boolean (*dsu_get_alarm_status)(csu_dsu_instance_t *, int *, int *, 
				ushort *, ushort *);
boolean (*dsu_remote_tests)(csu_dsu_instance_t *cd_info);
boolean (*dsu_check_connection_status)(csu_dsu_instance_t *, int, int);
boolean (*dsu_set_current_config)(csu_dsu_instance_t *); 
int 	(*dsu_remote_loopback_command)(csu_dsu_instance_t *, int, int,
				   int, ulong);
void 	(*dsu_clear_alarm_data)(csu_dsu_instance_t *);
int 	(*dsu_remote_lpback_state)(csu_dsu_instance_t *);
ulong 	(*dsu_specify_reg_wait_time)(uchar);

/* Module type specific show command calls */
boolean (*dsu_show_module_config)(csu_dsu_instance_t *);
void 	(*dsu_show_network_status)(csu_dsu_instance_t *);
void 	(*dsu_show_loopback_status)(csu_dsu_instance_t *, uchar, uchar);
void 	(*dsu_show_alarm_counters)(csu_dsu_instance_t *);
boolean (*dsu_show_last_loopback)(uchar, uchar);
void 	(*dsu_show_performance_stats)(csu_dsu_instance_t *);
void 	(*dsu_show_full_performance_stats)(csu_dsu_instance_t *, uchar, uchar);
ulong	(*dsu_current_line_bandwidth)(csu_dsu_instance_t *, boolean);

/* Common user-configuration parameters */
uchar		dsu_clocksource;
uchar		dsu_networktype;
uchar		dsu_data_scrambler_mode;

/* FT1 module user-configuration parameters */
uchar		t1_backup_clocksource;
uchar		t1_line_type;
uchar		t1_framing_type;
uchar		t1_line_build_out;
uchar		t1_yellow_alarm_mode;
uchar		t1_full_bw_loopcode;
uchar		t1_fractional_bw_loopcode;
uchar		t1_loopback_test_fraction;
ulong           t1_timeslots_bitmap;
uchar		t1_timeslots_speed;

/* SW56 module user-configuration parameters */
uchar 		sw56_remote_loopback_enable;
uchar		sw56_dds_linerate;
uchar 		sw56_dte_cmd_opt;
uchar		sw56_auto_answer;
uchar		sw56_network_type;
uchar		sw56_switched_type;
uchar		dummy_longword_align1;

/* User configured parameters */
uchar           usr_cfg_clock_source;
uchar           usr_cfg_data_coding;
uchar           usr_cfg_network_type;
uchar           usr_cfg_switch_carrier;
uchar           usr_cfg_sw56_remote_loopback_mode;
uchar           usr_cfg_t1_full_remote_loopback_type;
uchar           usr_cfg_t1_fr_remote_loopback_type;
uchar           usr_cfg_framing;
uchar           usr_cfg_linecode;
uchar           usr_cfg_lbo;
uchar           usr_cfg_clock_rate;
uchar           usr_cfg_timeslot_type;
ulong           usr_cfg_timeslot_range;
uchar           usr_cfg_timeslot_speed;
boolean         usr_cfg_yellow_alarm_enable;
uchar           usr_cfg_loopback_dte_type;
uchar           usr_cfg_loopback_dte_pat_opt;
uchar           usr_cfg_loopback_dte_pattern;
uchar           usr_cfg_loopback_line_type;
uchar           usr_cfg_loopback_remote_type;
uchar           usr_cfg_loopback_remote_pat_opt;
uchar           usr_cfg_loopback_remote_pat_len;
ulong           usr_cfg_loopback_remote_pat_bmap;
};

static inline csu_dsu_instance_t *csu_dsu_get_instance(hwidbtype *idb)
{
    hd64570_instance_t *temp;

    if (idb == NULL)
	return (NULL);

    temp = INSTANCE;
    return ((csu_dsu_instance_t *) temp->csu_dsu_info);
}

/*
 * Array of CSU/DSU instances used by interrupt handler
 */
extern csu_dsu_instance_t *csu_dsu_int_array[MAX_SERIAL_PORTS];

/* CSU/DSU module register access routine returncodes */
enum REG_ACCESS_RESULTS {
	DSU_REQUEST_COMPLETE,
	DSU_REQUEST_TIMEOUT,
	DSU_BAD_INSTANCE,
	DSU_BEING_RESET,
	DSU_LOCK_OBTAIN_TIMEOUT,
	DSU_NO_MEMORY,
	DSU_WRONG_MODULE_TYPE,
	DSU_WRONG_FORMAT,
	DSU_INTERMEDIATE_REG_RW_FAILURE,
	DSU_MAX_REG_ACCESS_RESULTS
};

#define READ_REGISTER		FALSE
#define WRITE_REGISTER		TRUE

/*
 * CSU/DSU internal register maps
 * All common regs are prefixed by DSU_
 * All Fractional T1 CSU/DSU regs are prefixed by T1_
 * All Switched 56 CSU/DSU are prefixed by SW56_
 */

#define DSU_CLOCKSOURCE_INTERNAL    0
#define DSU_CLOCKSOURCE_NETWORK     1


/* The CSU/DSU vendor and type */
#define DSU_UNIT_TYPE_REG				1
#define DSU_UNIT_TYPE_LEN		1
#define DIGITAL_LINK_FT1_MODULE		0x01
#define ADTRAN_SW56_4W_MODULE		0x10
#define ADTRAN_SW56_2W_MODULE		0x11	


/* The H/W revision of module - string of characters */
#define DSU_HW_REVISION_REG				2
#define DSU_HW_REVISION_LEN		2


/* The S/W revision of module - string of characters */
#define DSU_SW_REVISION_REG				3
#define DSU_SW_REVISION_LEN		5


/* The Interface protocol revision register - a ushort */
#define DSU_INT_PROTOCOL_REV_REG			4
#define DSU_INT_PROTOCOL_REV_LEN	2
#define DIGITAL_LINK_FT1_HIGHEST_REV	0x0101
#define ADTRAN_SW56_4W_HIGHEST_REV	0x0100
#define ADTRAN_SW56_2W_HIGHEST_REV	0x0100
#define DSU_INT_PROTOCOL_LOWEST_REV	0x0100
	
/* The CSU/DSU image checksum - string of characters */
#define DSU_IMAGE_CKSUM_REG				5
#define DSU_IMAGE_CKSUM_LEN		4


/* Aggregated Alarm and network and access error retrieval register */
#define T1_ALARM_UNITSTATUS_REG				10
#define T1_ALARM_UNITSTATUS_LEN		9

/* Structure used for accessing the aggregated alarm & unit status */
struct t1_alarm_unitstatus {
ushort	t1_unit_status;
ushort	t1_receiver_status;
ushort  t1_alarm_current_status;
ushort	t1_alarm_transitions;
uchar	t1_access_error;
};

struct t1_alarm_status {
ushort  t1_alarm_current_status;
ushort  t1_alarm_transitions;
};

/* BitMask of values for FT1 unit status register. */
#define B_FT1_UNIT_STATUS_LOSS_RTS_OR_DTR      0x01
#define B_FT1_UNIT_STATUS_SENDING_YELLOW       0x02
#define B_FT1_UNIT_STATUS_SENDING_AIS          0x04
#define B_FT1_UNIT_STATUS_UNAVAIL_SIGNAL_STATE 0x08
#define B_FT1_UNIT_STATUS_EXCESS_ZEROS         0x10
#define B_FT1_UNIT_STATUS_FERR                 0x20
#define B_FT1_UNIT_STATUS_IN_TEST              0x40

/* T1 module unit status - bitmap */
#define T1_UNIT_STATUS_REG				11
#define T1_UNIT_STATUS_LEN		2
enum T1_STATUS_BITMAP {
	T1_STATUS_LOSS_DTE_PORT, T1_STATUS_TXING_YELLOW, T1_STATUS_TXING_KEEPALIVE, 
      	T1_STATUS_UNAVAILABLE_STATE, T1_STATUS_EXCESS_ZEROES, T1_STATUS_NO_CLOCK,
      	T1_STATUS_FRAME_ERR, T1_STATUS_TEST_MODE, T1_STATUS_MAX
};

/* BitMask of values for FT1 receiver status register. */
#define B_FT1_RX_STATUS_CARRIER_LOSS    0x01
#define B_FT1_RX_STATUS_LOSS_OF_SYNC    0x02
#define B_FT1_RX_STATUS_YELLOW          0x04
#define B_FT1_RX_STATUS_BLUE            0x08
#define B_FT1_RX_STATUS_SET_CODE        0x10
#define B_FT1_RX_STATUS_RESET_CODE      0x20

/* T1 module receiver status - bitmap */
#define T1_RECEIVER_STATUS_REG				12
#define T1_RECEIVER_STATUS_LEN		2
enum T1_RECEIVER_STATUS_BITMAP {
	T1_STATUS_CARRIER_LOSS, T1_STATUS_SIGNAL_LOSS, T1_STATUS_RXING_RAI,
     	T1_STATUS_RXING_AIS, T1_STATUS_RXING_LOOPUP, T1_STATUS_RXING_LOOPDOWN
};

struct sw56_alarm_loopstatus {
uchar transition_status;
uchar current_status;
};

/* BitMask of values for T1 status reg. */
#define B_FT1_ALARM_STATUS_BPV_THRESHOLD_EXCEEDED   0x0001  /* masked out */
#define B_FT1_ALARM_STATUS_CRC_THRESHOLD_EXCEEDED   0x0002  /* masked out */
#define B_FT1_ALARM_STATUS_OOF_THRESHOLD_EXCEEDED   0x0004  /* masked out */
#define B_FT1_ALARM_STATUS_NET_CARRIER_LOSS         0x0008
#define B_FT1_ALARM_STATUS_NET_SYNC_LOSS            0x0010
#define B_FT1_ALARM_STATUS_NET_AIS_RECEIVED         0x0020
#define B_FT1_ALARM_STATUS_NET_YELLOW_RECEIVED      0x0040
#define B_FT1_ALARM_STATUS_ACCESS_ERROR             0x0080  /* Oops, programmatic error accessing register */
#define B_FT1_ALARM_STATUS_NET_PAYLOAD_LOOPBACK     0x0100
#define B_FT1_ALARM_STATUS_PATTERN_IN_SYNC        0x0200

/* T1 - Alarm Status register, SW56 - Loop status register */
#define T1_ALARM_STATUS_REG				13
#define T1_ALARM_STATUS_LEN		2
#define SW56_LOOP_STATUS_REG				T1_ALARM_STATUS_REG
#define	SW56_LOOP_STATUS_LEN 		1

/* T1 - Alarm Status register bitmaps */
#define DSU_NO_ALARMS		0

/* T1 - Interface Protocol Error */
#define T1_INT_PROTOCOL_ERR_REG				14
#define T1_INT_PROTOCOL_ERR_LEN		1
enum T1_INT_PROTOCOL_ERR_BITMAP {
	T1_INVALID_REG_ADDR, T1_INVALID_REG_DATA_VAL, T1_INVALID_REG_OVERFLOW,
	T1_MAX_ACCESS_ERRORS
};

/* BitMask of values for SW56 dial status reg.*/
#define B_SW56_DIAL_STATUS_DIALLING           0x01
#define B_SW56_DIAL_STATUS_CONNECTION_ACTIVE  0x02
#define B_SW56_DIAL_STATUS_SWITCH_BUSY        0x04
#define B_SW56_DIAL_STATUS_REMOTE_END_BUSY    0x08
#define B_SW56_DIAL_STATUS_NO_ANSWER          0x10

/* SW56 Dialling Status register - bit map */
#define SW56_DIAL_STATUS_REG				15
#define SW56_DIAL_STATUS_LEN		1
enum SW56_DIAL_STATUS_BITMAP {
	SW56_STATUS_DIALLING, SW56_STATUS_CONNECTION_ACTIVE, SW56_STATUS_SWITCH_BUSY,
	SW56_STATUS_REMOTE_END_BUSY, SW56_STATUS_NO_ANSWER, SW56_MAX_DIAL_STATUS
};


/* T1 - current clock source (may change during loopbacks) */
#define T1_CURRENT_CLK_SRC_REG				19
#define T1_CURRENT_CLK_SRC_LEN		1


/* SW56 - current loop rate in DDS mode */
#define SW56_CURRENT_LINE_RATE_REG			20
#define SW56_CURRENT_LINE_RATE_LEN	1
enum DDS_LOOP_RATES {
	SW56_LINE_RATE_AUTO, SW56_LINE_RATE_2400, SW56_LINE_RATE_4800, 
	SW56_LINE_RATE_9600, SW56_LINE_RATE_19200, SW56_LINE_RATE_38400,
        SW56_LINE_RATE_56K, SW56_LINE_RATE_64K
};


/*
 * T1 - Alarm Status mask register, SW56 - Loop status mask register 
 * Bitmaps that use enum T1_ALARM_STATUS_BITMAP and enum SW56_LOOPSTATUS_BITMAP 
 */
#define T1_ALARM_STATUS_MASK_REG        		24
#define T1_ALARM_STATUS_MASK_LEN        2
#define SW56_LOOP_STATUS_MASK_REG            T1_ALARM_STATUS_MASK_REG
#define SW56_LOOP_STATUS_MASK_LEN       1

#define T1_USER_REPORTABLE_ALARMS	0x00f8
#define T1_ALARM_STATUS_MASK_DEF	0x03ff
#define T1_ALARM_STATUS_MASK_NORMAL	0x0007
#define T1_ALL_SUPPORTED_ALARMS_MASK    0x03ff


/* SW56 - Alarm Status register bitmaps */
#define SW56_USER_REPORTABLE_ALARMS     0x1f
#define SW56_4W_ALL_ALARMS_MASK 0xff
#define SW56_2W_ALL_ALARMS_MASK 0xe7
#define SW56_4W_ALARM_MASK_DDS        0xff04
#define SW56_4W_ALARM_MASK_SWITCHED   0xff00
#define SW56_2W_ALARM_MASK_SWITCHED   0xff1e


/* T1 - BPV, CRC, OOF threshold exceeded registers */
#define T1_BPV_THRESHOLD_REG				28
#define T1_CRC_THRESHOLD_REG				29
#define T1_OOF_THRESHOLD_REG				30
#define T1_THRESHOLD_REG_LEN		2


/* CSU-DSU Self Test results - bitmap */
#define DSU_SELF_TEST_RESULTS_REG			37
#define DSU_SELF_TEST_RESULTS_LEN	1
enum SELFTEST_STATUS_BITMAP {
	DSU_FLASH_CHKSUM_FAILED, DSU_EEROM_CHKSUM_FAILED, DSU_RAM_TEST_FAILED, 
        DSU_ROM_CHKSUM_FAILED, DSU_DTE_LOOP_FAILED, DSU_DTE_LOOP_WITH_TP_FAILED,
	DSU_MAX_SELF_TESTS
};

#define DSU_SELF_TEST_PASSED		0

/* Soft reset registers - enables factory power on default configuration */
#define DSU_POWER_ON_DEFAULT_REG			38
#define DSU_POWER_ON_DEFAULT_LEN	1
#define DSU_DEFAULTS_ENABLE		1


/* Test type select register */
#define DSU_TEST_TYPE_SELECT_REG			44
#define DSU_TEST_TYPE_SELECT_LEN	1
enum TEST_TYPES {
	DSU_LOOPBACK_NONE, DSU_DTE_LOOPBACK, DSU_NA_TEST, DSU_LOCAL_PAYLOAD_LOOPBACK,
	DSU_LOCAL_NET_LOOPBACK, DSU_SEND_TEST_PATTERN_TEST, DSU_REMOTE_LOOPUP,
  	DSU_REMOTE_LOOPUP_SEND_PATTERN, DSU_REMOTE_LOOPDOWN, DSU_SELF_TEST, 
  	DSU_SMART_JACK_LOOPUP, DSU_SMART_JACK_LOOPUP_SEND_PATTERN, DSU_SMART_JACK_LOOPDOWN,
	DSU_LAMP_TEST
};

/* CSU/DSU remote loopback status */
enum REM_LPBACK_STATUS {
        DSU_NO_LPBACK,
        DSU_ATTEMPTING_LOOPUP,
        DSU_ATTEMPTING_LOOPDOWN,
        DSU_FAILED_REMOTE_LOOP,
        DSU_IN_LOOPBACK,
        DSU_TEST_STATUS_UNREADABLE
};

/* Test pattern select register */
#define DSU_TEST_PATTERN_SELECT_REG			45
#define DSU_TEST_PATTERN_SELECT_LEN	1

enum T1_TEST_PATTERN_TYPES {
	T1_QRW, T1_1IN8, T1_3IN24, T1_1IN1, T1_ALL_1, T1_ALL_0, T1_1IN2, T1_1IN4,
     	T1_USER_TEST_PATTERN_1, T1_USER_TEST_PATTERN_2
};

enum SW56_TEST_PATTERN_TYPES {
	SW56_2047, SW56_511, SW56_STRESS_PATTERN_1, SW56_STRESS_PATTERN_2, 
        SW56_STRESS_PATTERN_3, SW56_STRESS_PATTERN_4
};

/* Test Status Register */
#define DSU_TEST_STATUS_REG				46
#define DSU_TEST_STATUS_LEN		1
enum T1_TEST_STATUS {
	ATTEMPTING_REMOTE_TEST, PASSED_REMOTE_TEST, FAILED_REMOTE_TEST 
};
#define NO_REMOTE_TEST		FAILED_REMOTE_TEST+1

#define SW56_LOCAL_TEST			0x01
#define SW56_TELCO_TEST			0x02
#define	SW56_LOCAL_DTE_LOOP_TEST	0x04
#define SW56_ATTEMPTING_REMOTE_LOOP	0x08
#define SW56_PASSED_REMOTE_LOOP		0x10
#define SW56_LOOPUP_FROM_REMOTE		0x20
#define SW56_SENDING_TEST_PATTERN	0x40
#define SW56_SYNCED_ON_TEST_PATTERN	0x80 

/* Bit error count - number of mismatches in test-pattern echoed back */
#define DSU_BERT_REG					47
#define DSU_BERT_LEN			4


/* SW56 - Inject bit error in test pattern */
#define SW56_INJECT_BITERR_REG				52
#define SW56_INJECT_BITERR_LEN		1
#define SW56_INJECT_BITERR_ENABLE	1


/* SW56 - Enable/Disable remote loopback */
#define SW56_REM_LOOP_DISABLE_REG			53
#define SW56_REM_LOOP_DISABLE_LEN 	1
#define SW56_DISABLE_REM_LOOPBACK	0
#define SW56_ENABLE_REM_LOOPBACK	1
#define SW56_REM_LOOP_DISABLE_DEF	SW56_ENABLE_REM_LOOPBACK


/* T1 - USER test patterns */
#define T1_USER_TEST_PATTERN1_REG			57
#define T1_USER_TEST_PATTERN2_REG			58
#define T1_USER_TEST_PATTERN1_LEN	4
#define T1_USER_TEST_PATTERN2_LEN	4
#define T1_USER_TEST_PATTERN1_DEF	0x33333333
#define T1_USER_TEST_PATTERN2_DEF	0x00ff0000


/* T1 - Test duration select */
#define T1_TEST_TIME_SELECT_REG				59
#define T1_TEST_TIME_SELECT_LEN		1
enum T1_TEST_TIME_SELECTIONS {
	T1_TEST_TIME_1MIN, T1_TEST_TIME15MIN, T1_TEST_TIME1HR,  
        T1_TEST_TIME_UNLIMITED
};
#define T1_TEST_TIME_SELECT_DEF		T1_TEST_TIME_UNLIMITED


/* T1 - Test fraction select */
#define T1_TEST_FRACTION_REG				60
#define T1_TEST_FRACTION_LEN	1
enum T1_TEST_FRACTIONS {
	T1_TEST_FRACTION_FULL_BW, T1_TEST_FRACTION_ALLOCATED_FRACTION
};
#define T1_TEST_FRACTION_DEF       	T1_TEST_FRACTION_FULL_BW


/* T1 - Full and fractional bandwidth loopcode select */
#define T1_FULL_BW_LOOPCODE_REG				61
#define T1_FRACTIONAL_LOOPCODE_REG			62
#define T1_TEST_LOOPCODE_LEN		1
enum T1_TEST_LOOPCODES {
	T1_STANDARD_LOOPCODE, T1_ALTERNATE_LOOPCODE, T1_DISABLE_REM_LOOP, 
	T1_V54_FRACTIONAL_LOOPCODE
};
#define T1_FULL_BW_LOOPCODE_DEF		T1_STANDARD_LOOPCODE
#define T1_FRACTIONAL_LOOPCODE_DEF	T1_STANDARD_LOOPCODE

 
/* T1 - DACS mode enable, SW56 - Network Type Select  */
#define T1_DACS_MODE_ENABLE_REG				68
#define T1_DACS_MODE_ENABLE_LEN		1
#define T1_DACS_MODE_DISABLED		0
#define T1_DACS_MODE_ENABLED		1
#define T1_DACS_MODE_ENABLE_DEF		T1_DACS_MODE_ENABLED
#define SW56_NETWORK_TYPE_REG				68
#define SW56_NETWORK_TYPE_LEN		1
enum SW56_NETWORK_TYPES {
	SW56_DDS_MODE, SW56_ATT_MODE, SW56_SPRINT_MODE
};

/* Main clock source */
#define DSU_CLOCK_SOURCE_REG				69
#define DSU_CLOCK_SOURCE_LEN		1
#define DSU_CLOCK_SOURCE_DEF		DSU_CLOCKSOURCE_NETWORK


/* T1 - Backup clock source */
#define T1_BACKUP_CLOCK_SOURCE_REG			70
#define T1_BACKUP_CLOCK_SOURCE_LEN	1
#define T1_BACKUP_CLOCK_SOURCE_DEF	DSU_CLOCKSOURCE_INTERNAL


/* Data Transmission options */
#define DSU_DATA_TRANSMIT_OPTIONS_REG			71
#define DSU_DATA_TRANSMIT_OPTIONS_LEN	1
enum DATA_TRANSMIT_OPTS {
	DSU_TRANSMIT_NORMAL, DSU_TRANSMIT_INVERTED_SCRAMBLED, 
	T1_TRANSMIT_56KBPS
}; 

#define DSU_DATA_TRANSMIT_OPTIONS_DEF	DSU_TRANSMIT_NORMAL


/* SW56 - Enable/Disable remote configuration */
#define SW56_REMOTE_CONFIG_DISABLE_REG			72
#define SW56_REMOTE_CONFIG_DISABLE_LEN	1
#define SW56_REMOTE_CONFIG_DISABLED	0
#define SW56_REMOTE_CONFIG_ENABLED	1
#define SW56_REMOTE_CONFIG_DEF		SW56_REMOTE_CONFIG_DISABLED

/* SW56 - Perform remote configuration */
#define SW56_REMOTE_CONFIGURATION_REG			73
#define SW56_REMOTE_CONFIGURATION_LEN	1
#define SW56_REMOTE_CONFIG_TERMINATE	0
#define SW56_REMOTE_CONFIG_INITIATE	1


/* 
 * Switched 56k module, V.35 signals configuration 
 */
#define SW56_V35_CONFIG_REG_LEN		1
enum SW56_V35_OPTIONS {SW56_CTS_OPTION_REG = 		81,
		       SW56_DSR_OPTION_REG,
		       SW56_DCD_OPTION_REG,
		       SW56_DTR_OPTION_REG,
		       SW56_RTS_TO_CTS_DELAY_REG
};


/* SW56 - V.35 Clear to send options */
enum SW56_CTS_OPTIONS {
	SW56_CTS_ALWAYS_ON, SW56_CTS_FOLLOWS_RTS, SW56_CTS_FOLLOWS_DCD,
	SW56_CTS_FOLLOWS_DCD_OR_RTS
};
#define SW56_CTS_OPTION_LEN		SW56_V35_CONFIG_REG_LEN
#define SW56_CTS_OPTION_DEF		SW56_CTS_FOLLOWS_RTS


/* SW56 - V.35 Data Set Ready options */
enum SW56_DSR_OPTIONS {
	SW56_DSR_ALWAYS_ON, SW56_DSR_OFF_WITH_OOS, SW56_DSR_OFF_WITH_TEST,
	SW56_DSR_OFF_WITH_OOS_OR_TEST
};
#define SW56_DSR_OPTION_LEN		SW56_V35_CONFIG_REG_LEN
#define SW56_DSR_OPTION_DEF		SW56_DSR_OFF_WITH_OOS_OR_TEST


/* SW56 - V.35 Data Carrier Detect options */
enum SW56_DCD_OPTIONS {
	SW56_DCD_ALWAYS_ON, SW56_DCD_ON_WITH_DATA
};
#define SW56_DCD_OPTION_LEN 		SW56_V35_CONFIG_REG_LEN
#define SW56_DCD_OPTION_DEF		SW56_DCD_ON_WITH_DATA


/* SW56 - V.35 Data Terminal Ready options */
enum SW56_DTR_OPTIONS {
	SW56_DTR_IGNORED, 
	SW56_DTR_NORMAL		/* DDS - command mode when DTR off */
};				/* Switched - on hook when DTR off */
#define SW56_DTR_OPTION_LEN		SW56_V35_CONFIG_REG_LEN
#define SW56_DTR_OPTION_DEF		SW56_DTR_IGNORED


/* SW56 - V.35 RTS to CTS delay */
enum SW56_RTS_CTS_DELAY_OPTIONS {
	SW56_SHORT_RTS_CTS_DELAY, SW56_LONG_RTS_CTS_DELAY
};
#define SW56_RTS_TO_CTS_DELAY_DEF       SW56_SHORT_RTS_CTS_DELAY


/* T1 - Network Clear channel option */
#define T1_LINE_TYPE_REG				88
#define T1_LINE_TYPE_LEN		1
#define T1_LINE_TYPE_B8ZS		0
#define T1_LINE_TYPE_AMI		1
#define T1_LINE_TYPE_DEF		T1_LINE_TYPE_B8ZS


/* T1 - Network Framing */
#define T1_FRAMING_TYPE_REG				89
#define T1_FRAMING_TYPE_LEN		1
#define T1_FRAMING_TYPE_ESF		0
#define T1_FRAMING_TYPE_SF		1
#define T1_FRAMING_TYPE_DEF		T1_FRAMING_TYPE_ESF


/* T1 - Line Build Out */
#define T1_LBO_REG					90
#define T1_LBO_LEN			1
enum {T1_LBO_0DB, T1_LBO_7_5DB, T1_LBO_15DB};
#define T1_LBO_DEF			T1_LBO_0DB


/* T1 - Yellow Alarm Enable option */
#define T1_YELLOW_ALARM_OPTION_REG			91
#define T1_YELLOW_ALARM_OPTION_LEN	1 
#define T1_YELLOW_ALARM_DISABLED	0	
#define T1_YELLOW_ALARM_ENABLED		1 
#define T1_YELLOW_ALARM_OPTION_DEF	T1_YELLOW_ALARM_DISABLED


/* T1 - Loss of DTE criterion */
#define T1_LOSS_OF_DTE_OPTION_REG			92
#define T1_LOSS_OF_DTE_OPTION_LEN	1
enum {T1_RTS_LOW, T1_DTR_LOW, T1_NONE, T1_RTS_OR_DTR_LOW};
#define T1_LOSS_OF_DTE_DEF		T1_RTS_OR_DTR_LOW


/* T1 - fraction timeslot map */
#define T1_FRACTION_TIMESLOTS_REG			100
#define T1_FRACTION_TIMESLOTS_LEN	4
#define T1_FRACTION_TIMESLOTS_DEF	0x00ffffff	/* Full 1.544Mbps */
#define T1_FRACTION_MAX_TIMESLOTS	24

/* SW56 - loop rate select in DDS mode */
#define SW56_DDS_LINE_RATE_REG      			107
#define SW56_DDS_LINE_RATE_LEN      	1
/* Uses enum DDS_LOOP_RATES */
#define SW56_DDS_LINE_RATE_DEF		SW56_LINE_RATE_AUTO


/* SW56 - Enable V.25bis dialling */
#define SW56_V25BIS_DIAL_REG				108
#define SW56_V25BIS_DIAL_LEN		1 
enum {DISABLE_V25BIS_COMMANDS, DIAL_REG_RESERVED1, ENABLE_V25BIS_COMMANDS};
#define SW56_V25BIS_DIAL_DEF		DISABLE_V25BIS_COMMANDS
      
 
/* SW56 - VLL option - Redial on call disconnect */
#define SW56_VLL_ENABLE_REG				109
#define SW56_VLL_ENABLE_LEN		1
#define SW56_VLL_DISABLED		0
#define SW56_VLL_ENABLED		1			     
#define SW56_VLL_ENABLE_DEF		SW56_VLL_DISABLED

 
/* SW56 - Select V.25bis reply mode */
#define SW56_V25BIS_RESULT_REG				110
#define SW56_V25BIS_RESULT_LEN		1
#define SW56_V25BIS_NUMERIC_REPLY	0
#define SW56_V25BIS_VERBOSE_REPLY	1	
#define SW56_V25BIS_RESULT_DEF		SW56_V25BIS_VERBOSE_REPLY


/* SW56 - Dial Command */
#define SW56_DIAL_COMMAND_REG				116
#define SW56_DIAL_COMMAND_LEN		1
#define SW56_DIAL_NUMBER		1


/* SW56 - Auto answer mode */
#define SW56_AUTO_ANSWER_MODE_REG			117
#define SW56_AUTO_ANSWER_MODE_LEN	1
#define SW56_AUTO_ANSWER_DISABLED	0
#define SW56_AUTO_ANSWER_ENABLED	1
#define SW56_AUTO_ANSWER_MODE_DEF	SW56_AUTO_ANSWER_DISABLED

/* SW56 - Answer Incoming call */
#define SW56_ANS_CALL_CMD_REG				118
#define SW56_ANS_CALL_CMD_LEN		1
#define SW56_ANSWER_INCOMING_CALL	1


/* SW56 - Hangup current call */
#define SW56_HANGUP_CALL_CMD_REG			119
#define SW56_HANGUP_CALL_CMD_LEN        1
#define SW56_HANGUP_CALL		1


/* SW56 - Dial Number - string format */
#define SW56_DIAL_NUMBER_REG				120
#define SW56_DIAL_NUMBER_LEN		32

/*
 * T1 module - T1 current and archive 15-min performance registers
 */
typedef struct t1_interval_user_data_ {
ushort		t1ES;
ushort		t1UAS;
ushort		t1BPV;
ushort		t1OOF;
ushort		t1PCV;
ushort		t1LES;
ushort		t1CSS;
ushort		t1BES;
ushort		t1SES;
ushort		t1SEFS;
ushort 		t1DM;
} t1_interval_user_data;

typedef struct t1_current_interval_user_data_ {
t1_interval_user_data		t1_current_int;
ushort				t1_seconds_elapsed;
ulong				t1_interval_BPV60;
ulong				t1_interval_CRC60;
ulong				t1_interval_OOF60;
} t1_current_interval_user_data;

typedef struct t1_total_user_data_ {
ulong		t1ES;
ulong		t1UAS;
ulong		t1BPV;
ulong		t1OOF;
ulong		t1PCV;
ulong		t1LES;
ulong		t1CSS;
ulong		t1BES;
ulong		t1SES;
ulong		t1SEFS;
ulong		t1DM;
ushort		t1_error_free_secs;
} t1_total_user_data;

#define T1_CURRENT_INTERVAL_DATA_REG			170
#define	T1_CURRENT_INTERVAL_DATA_LEN	35	
#define T1_1406_INTERVAL_DATA_LEN	22

#define T1_CURRENT_INTERVAL_SECONDS_REG			182
#define T1_CURRENT_INTERVAL_SECONDS_LEN	2

#define T1_TOTAL_DATA_REG				200
#define T1_TOTAL_DATA_LEN		46
#define T1_TOTAL_DATA_ACCESS_MSECS	44

#define T1_SELECTED_INTERVAL_DATA_REG			226

#define T1_VALID_INTERVALS_REG				228
#define T1_VALID_INTERVALS_LEN		1

#define T1_INTERVAL_SELECTOR_REG			225
#define T1_INTERVAL_SELECTOR_LEN	1
#define T1_MAX_PERFORMANCE_INTERVALS  (24*60)/15

#define T1_WHOLE_24HR_ARCHIVE_REG			227
#define T1_WHOLE_24HR_ARCHIVE_BASE_LEN	21
#define T1_WHOLE_24HR_ARCHIVE_ACCESS_MSECS	84

/* Hack until some more code cleanup */
#define T1_CURRENT_INTERVAL_DATA_NEW_REG	T1_CURRENT_INTERVAL_DATA_REG
#define T1_VALID_INTERVALS_NEW_REG		T1_VALID_INTERVALS_REG
#define T1_INTERVAL_SELECTOR_NEW_REG		T1_INTERVAL_SELECTOR_REG
#define T1_TOTAL_DATA_NEW_REG			T1_TOTAL_DATA_REG

/*
 * Function prototypes 
 */
extern boolean csu_dsu_debug;

/* les/if_c3000_csu_dsu.c */
extern int csu_dsu_register_RW (csu_dsu_instance_t *, uchar, int, void *, boolean);
extern void reset_csu_dsu(csu_dsu_instance_t *, boolean);
extern int csu_dsu_loopback_command(csu_dsu_instance_t *, int, int, int, ulong);
extern void csu_dsu_show_loopback_status(csu_dsu_instance_t *, uchar, uchar);
extern void csu_dsu_show_last_lpback(csu_dsu_instance_t *);
extern void csu_dsu_clear_counters(hwidbtype *);
extern void csu_dsu_loopback_finished(csu_dsu_instance_t *);
extern void csu_dsu_calculate_bert_test(csu_dsu_instance_t *);
extern boolean csu_dsu_selftest_results(csu_dsu_instance_t *, boolean);

/* les/if_c3000_t1_csu_dsu.c */
extern void t1_set_default_values (csu_dsu_instance_t *);
extern int t1_set_line_framing (csu_dsu_instance_t *, uchar);
extern int t1_set_line_code (csu_dsu_instance_t *, uchar);
extern int t1_set_clock_source (csu_dsu_instance_t *, uchar);
extern int t1_set_data_coding (csu_dsu_instance_t *, uchar);
extern int t1_set_timeslots (csu_dsu_instance_t *, ulong, uchar);
extern int t1_set_remote_loopcode (csu_dsu_instance_t *, int, int);
extern int t1_set_line_buildout (csu_dsu_instance_t *, uchar);
extern int t1_yellow_alarm_enable (csu_dsu_instance_t *, boolean);
extern int t1_set_test_fraction (csu_dsu_instance_t *, uchar);

/* les/if_c3000_sw56_csu_dsu.c */
extern void sw56_set_default_values (csu_dsu_instance_t *);
extern int sw56_set_network_type (csu_dsu_instance_t *, uchar, uchar);
extern int sw56_set_clock_source (csu_dsu_instance_t *, uchar);
extern int sw56_set_clock_rate (csu_dsu_instance_t *, uchar);
extern int sw56_set_data_coding (csu_dsu_instance_t *, uchar);
extern int sw56_set_RDL_enable (csu_dsu_instance_t *, boolean);

/* les/if_c3000_csu_dsu_parser.c */
extern void csu_dsu_parser_init(void);
extern ulong csu_dsu_make_bit_field(int, int);
extern ulong csu_dsu_validate_usr_pat(char *);
extern void csu_dsu_debug_init(void);
extern boolean csu_dsu_csu_dsu_installed(int, hwidbtype *);
extern void csu_dsu_service_module_command(parseinfo *);
extern boolean csu_dsu_show_cable_state(ushort, boolean);

#endif __IF_C3000_CSU_DSU_H__
