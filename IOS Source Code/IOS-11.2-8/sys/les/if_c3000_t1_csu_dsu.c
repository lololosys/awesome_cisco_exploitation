/* $Id: if_c3000_t1_csu_dsu.c,v 3.2.2.2 1996/08/28 12:57:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_t1_csu_dsu.c,v $
 *------------------------------------------------------------------
 * if_c3000_t1_csu_dsu.c - Fractional T1 Integrated CSU/DSU driver code
 *
 * March 1996, Shalabh Agarwal
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_t1_csu_dsu.c,v $
 * Revision 3.2.2.2  1996/08/28  12:57:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.2.1  1996/08/07  09:01:17  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2  1996/03/29  03:50:00  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:29  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../les/msg_csu_dsu.c"		/* Not a typo, see logger.h */
#include "mgd_timers.h"
#include "../les/if_les.h"
#include "../les/if_les_hd64570.h"
#include "../os/timer_generic.h"
#include "../les/if_c3000_csu_dsu.h"
#include "../parser/parser_defs_parser.h"

/*
 * Array used for printing debug messages in alarm handler
 */
static char *const t1_alarm_messages[T1_MAX_ALARM_EVENTS] = {
        "bpv threshold exceeded alarm", "crc threshold exceeded alarm",
        "oof threshold exceeded alarm", "loss of signal", "loss of frame",
        "AIS alarm", "Remote alarm", "Module access errors",
	"Loopback test", "Sync on test-pattern"
};

/*
 * Array used for printing debug messages in alarm handler -
 * "" are status bits we don't care about or are available in the alarm status
 */
static char *const unit_status_messages[T1_STATUS_MAX] = {
        "", "", "",
        "", "excess zeroes", "no clock",
        "", ""
};

static char *const t1_access_errors[T1_MAX_ACCESS_ERRORS] = {
	"bad register address supplied",
	"bad register value supplied",
	"register overflow"
};


static boolean t1_alarm_processor (csu_dsu_instance_t *cd_info, 
			           int *low_alarm_index, int *hi_alarm_index,
			           ushort *alarm_transitions, 
			           ushort *alarm_real)
{
    int i;
    ushort bitmask;
    struct t1_alarm_unitstatus dsu_status;
    boolean pattern_sync_status;
    static boolean only_one_message = TRUE;

    if (!cd_info)
        return FALSE;

    if (csu_dsu_register_RW(cd_info, T1_ALARM_UNITSTATUS_REG,
                            T1_ALARM_UNITSTATUS_LEN, &dsu_status,
                            READ_REGISTER) != DSU_REQUEST_COMPLETE) {
	errmsg(&msgsym(ALARMFAILURE, SERVICE_MODULE), 
	       cd_info->interface_number);
        return (FALSE);
    }

    GET_TIMESTAMP(cd_info->alarm_event_timestamp);
    *low_alarm_index = T1_BPV_THRSLD_EXCEEDED;
    *hi_alarm_index = T1_MAX_ALARM_EVENTS;
    *alarm_transitions = dsu_status.t1_alarm_transitions;
    *alarm_real = dsu_status.t1_alarm_current_status;

    /*
     * First process any unit status changes
     * Right now, only unavailable state, excess zeroes and no clock
     * status are interesting - rest are insignificant
     */
    bitmask = 0x0001 << T1_STATUS_EXCESS_ZEROES;
    for (i = T1_STATUS_EXCESS_ZEROES; i < T1_STATUS_FRAME_ERR;
         i ++, bitmask <<= 1) 
        if (csu_dsu_debug && (bitmask & dsu_status.t1_unit_status))
            buginf("\nSERVICE_MODULE(%d): Unit has %s", 
		   cd_info->interface_number, unit_status_messages[i]);

    /* Check if any alarms we don't know of */
    if (only_one_message) 
        if ((dsu_status.t1_alarm_current_status & 
                      ~T1_ALL_SUPPORTED_ALARMS_MASK) != DSU_NO_ALARMS) {
	    errmsg(&msgsym(UNKNOWNALARM, SERVICE_MODULE),
	     	   cd_info->interface_number, dsu_status.t1_alarm_current_status);
            only_one_message = FALSE;
        }

    /*
     * Check for module access errors and print tracebacks
     */
    if (dsu_status.t1_alarm_current_status & (0x01 << T1_INT_PROTOCOL_ERROR)) {
	switch (dsu_status.t1_access_error) {
	    case T1_INVALID_REG_ADDR:
	    case T1_INVALID_REG_DATA_VAL:
	    case T1_INVALID_REG_OVERFLOW:
		errmsg(&msgsym(ACCESSERROR, SERVICE_MODULE), 
		       cd_info->interface_number, 
		       t1_access_errors[dsu_status.t1_access_error]);
		break;
	    default:
		errmsg(&msgsym(ACCESSERROR, SERVICE_MODULE),
		       cd_info->interface_number,
		       "Unknown error type returned"); 
    	}
    }

    /*
     * If we're in the middle of a remote-loopback with test-pattern
     * check for in-sync and out-of-sync conditions on test-pattern
     * This is important to obtain the correct duration for bit errors
     */
    if ((cd_info->current_loopback_command == CSU_DSU_LOOPBACK_REMOTE) &&
        (cd_info->dsu_loopback_test_pattern != CSU_DSU_LPBACK_PAT_NOSEL))
        if ((cd_info->current_loopback_state == DSU_IN_LOOPBACK) ||
            (cd_info->current_loopback_state == DSU_ATTEMPTING_LOOPUP)) {
            pattern_sync_status = dsu_status.t1_alarm_current_status &
                                  B_FT1_ALARM_STATUS_PATTERN_IN_SYNC;
            if (pattern_sync_status && (!cd_info->dsu_loopback_synced_on_pattern)) {
                GET_TIMESTAMP(cd_info->loopback_pattern_sync_duration);
            }
            else if ((!pattern_sync_status) && cd_info->dsu_loopback_synced_on_pattern)  {
                TIMER_STOP(cd_info->loopback_pattern_sync_duration);
            }

            cd_info->dsu_loopback_synced_on_pattern = pattern_sync_status;
        }

    return (TRUE);
}

static inline boolean csu_dsu_chk_instance(csu_dsu_instance_t *cd_info)
{
    if (!cd_info)
        return (FALSE);
    else if (cd_info->module_type != CSU_DSU_FT1)
        return (FALSE);

    return (TRUE);

}

/*
 * Called to clear all T1 performance statistics
 * Called on image launch and clear counters to reset 24-hr statistics 
 */
static void t1_clear_alarm_data (csu_dsu_instance_t *cd_info)
{
    ushort current_seconds_remaining;
    uchar num_valid_intervals;
    ushort t1_alarm_mask;

    if (!csu_dsu_chk_instance(cd_info))
        return;

    /* First the 24hr archive */
    num_valid_intervals = 0;
    if (csu_dsu_register_RW(cd_info, T1_VALID_INTERVALS_REG,
                            T1_VALID_INTERVALS_LEN,
                            &num_valid_intervals,
                            WRITE_REGISTER) != DSU_REQUEST_COMPLETE)
	if (csu_dsu_debug) 
	    buginf("\nSERVICE_MODULE(%d): Couldn't clear T1 performance data",
		   cd_info->interface_number);

    current_seconds_remaining = 0;
    if (csu_dsu_register_RW(cd_info, T1_CURRENT_INTERVAL_SECONDS_REG,
                            T1_CURRENT_INTERVAL_SECONDS_LEN,
                            &current_seconds_remaining,
                            WRITE_REGISTER) != DSU_REQUEST_COMPLETE)
	if (csu_dsu_debug)
            buginf("\nSERVICE_MODULE(%d): Couldn't clear T1 current interval data",
		   cd_info->interface_number);

    /* Set up the T1 alarm mask */
    t1_alarm_mask = T1_ALARM_STATUS_MASK_NORMAL;
    if (csu_dsu_register_RW(cd_info, T1_ALARM_STATUS_MASK_REG,
                            T1_ALARM_STATUS_MASK_LEN,
                            &t1_alarm_mask,
                            WRITE_REGISTER) == DSU_REQUEST_COMPLETE) 
        cd_info->dsu_alarm_mask = t1_alarm_mask;
    else if (csu_dsu_debug)
        buginf("\nSERVICE_MODULE(%d): Cannot enable module network alarm interrupts",
	       cd_info->interface_number); 
}

/*
 * t1_show_network_status - for the show service-module 
 *			    exec level command
 */
static void t1_show_network_status (csu_dsu_instance_t *cd_info)
{
    ushort unit_status;
    ulong bitmask;
    int bitnum;

    if (!csu_dsu_chk_instance(cd_info))
        return;

    /* What are we transmitting on line */
    unit_status = 0;
    if (csu_dsu_register_RW(cd_info, T1_UNIT_STATUS_REG, 
			    T1_UNIT_STATUS_LEN, &unit_status,
			    READ_REGISTER) == DSU_REQUEST_COMPLETE) {
	if (unit_status & (bitmask = 0x0001 << T1_STATUS_TXING_YELLOW))
	    printf("\nTransmitter is sending remote alarm.");
	else if (unit_status & (bitmask = 0x0001 << T1_STATUS_TXING_KEEPALIVE))
	    printf("\nTransmitter is sending AIS.");
	else if (unit_status & (bitmask = 0x0001 << T1_STATUS_NO_CLOCK))
	    printf("\nTransmitter has no clock");
    }

    /* Now the network receiver status */
    if (cd_info->dsu_network_alarm_status & T1_USER_REPORTABLE_ALARMS &
					    ~cd_info->dsu_alarm_mask) {
        printf("\nReceiver has ");
        for (bitnum = T1_BPV_THRSLD_EXCEEDED, bitmask = 0x0001;
             bitnum < T1_MAX_ALARM_EVENTS; bitnum ++, bitmask <<= 1) 
            if ((cd_info->dsu_network_alarm_status & bitmask)
                        & ~cd_info->dsu_alarm_mask)
                printf("%s, ", t1_alarm_messages[bitnum]);
    }
    else
	printf("\nReceiver has no alarms.");
} 

static int t1_num_fractional_timeslots (ulong timeslots_bitmap)
{
    ulong timeslots_bitmask;
    int i, num_timeslots = 0;

    for (i = 0, timeslots_bitmask = 0x01;  i < T1_FRACTION_MAX_TIMESLOTS;
                                           i++, timeslots_bitmask <<= 1) 
        if (timeslots_bitmap & timeslots_bitmask)
            num_timeslots ++;
    
    return (num_timeslots);
}

static ulong t1_current_line_speed (csu_dsu_instance_t *cd_info, boolean show_command)
{
    ulong line_speed;

    line_speed = ((cd_info->t1_timeslots_speed == CSU_DSU_TS_SPEED_64) ? 64 : 56) 
		   * t1_num_fractional_timeslots(cd_info->t1_timeslots_bitmap);
    
    if (show_command)
	return (line_speed);
    else
        return (line_speed * 1000);
}
    

	
static boolean t1_show_module_configuration (csu_dsu_instance_t *cd_info)
{
    uchar linecode_value, framing_value, data_coding;
    uchar clocksource_value;
    ulong timeslots_bitmap;

    if (!csu_dsu_chk_instance(cd_info))
        return (FALSE);

    printf("\nFraming is ");
    if (csu_dsu_register_RW(cd_info, T1_FRAMING_TYPE_REG,
                            T1_FRAMING_TYPE_LEN, &framing_value,
			    READ_REGISTER) != DSU_REQUEST_COMPLETE)
	return (FALSE);

    switch (framing_value) {
	case T1_FRAMING_TYPE_SF:
	    printf("SF, ");
	    break;
	case T1_FRAMING_TYPE_ESF:
	    printf("ESF, ");
	    break;
	default:
	    printf("Unknown, ");
    }

    printf("Line Code is ");
    if (csu_dsu_register_RW(cd_info, T1_LINE_TYPE_REG,
                            T1_LINE_TYPE_LEN, &linecode_value,
                            READ_REGISTER) != DSU_REQUEST_COMPLETE)  
	return (FALSE);

    switch (linecode_value) {
	case T1_LINE_TYPE_AMI:
	    printf("AMI, ");
	    break;
	case T1_LINE_TYPE_B8ZS:
	    printf("B8ZS, ");
	    break;
	default:
	    printf("Unknown, ");
    }

    printf("Current clock source is ");
    if (csu_dsu_register_RW(cd_info, T1_CURRENT_CLK_SRC_REG,
                            T1_CURRENT_CLK_SRC_LEN, &clocksource_value,
                            READ_REGISTER) != DSU_REQUEST_COMPLETE) 
	return (FALSE);
    
    switch (clocksource_value) {
	case DSU_CLOCKSOURCE_INTERNAL:
	    printf("internal, ");
	    break;
	case DSU_CLOCKSOURCE_NETWORK:
	    printf("line, ");
	    break;
	default:
	    printf("Unknown, ");
    }

    printf("\nFraction has ");
    if (csu_dsu_register_RW(cd_info, T1_FRACTION_TIMESLOTS_REG,
                            T1_FRACTION_TIMESLOTS_LEN, &timeslots_bitmap, 
                            READ_REGISTER) != DSU_REQUEST_COMPLETE)
	return (FALSE);
    
    printf("%d timeslots ", t1_num_fractional_timeslots(timeslots_bitmap));

    if (csu_dsu_register_RW(cd_info, DSU_DATA_TRANSMIT_OPTIONS_REG,
                            DSU_DATA_TRANSMIT_OPTIONS_LEN, &data_coding,
                            READ_REGISTER) != DSU_REQUEST_COMPLETE)
	return (FALSE);

    if (data_coding == T1_TRANSMIT_56KBPS) 
	printf("(56 Kbits/sec each), ");
    else 
	printf("(64 Kbits/sec each), ");
   
    printf("Net bandwidth is %d Kbits/sec.", t1_current_line_speed(cd_info, TRUE));

    return (TRUE);
}
    	
static void t1_show_alarm_counters (csu_dsu_instance_t *cd_info)
{
    int i;

    if (!csu_dsu_chk_instance(cd_info))
	return;

    for (i = T1_CARRIER_LOSS; i < T1_REMOTE_ACTIVATED_LOOPBACK; i ++) {
	printf("\n    %22s:%-5d, ", t1_alarm_messages[i], 
				    cd_info->alarm_counts[i].count);
	if (cd_info->alarm_counts[i].count) 
	    if (TIMER_RUNNING(cd_info->alarm_counts[i].current_duration)) {
	        printf("current duration ");
	        print_dhms(cd_info->alarm_counts[i].current_duration);
	    }
	    else {
		printf("last occurred ");
		print_dhms(cd_info->alarm_counts[i].last_occurrence);
	    }
    }
}

static void t1_show_interval_data (t1_interval_user_data *tbl)
{
    if (!tbl)
	return;

    printf("\n    %d Line Code Violations, %d Path Code Violations",
           tbl->t1BPV, tbl->t1PCV);

    printf("\n    %d Slip Secs, %d Fr Loss Secs, "
           "%d Line Err Secs, %d Degraded Mins",
           tbl->t1CSS, tbl->t1SEFS, tbl->t1LES, tbl->t1DM);

    printf("\n    %d Errored Secs, %d Bursty Err Secs, "
           "%d Severely Err Secs, %d Unavail Secs",
           tbl->t1ES, tbl->t1BES, tbl->t1SES, tbl->t1UAS);

}
 
static void t1_show_total_data (t1_total_user_data *tbl)
{
    if (!tbl)
        return;

    printf("\n    %d Line Code Violations, %d Path Code Violations",
           tbl->t1BPV, tbl->t1PCV);

    printf("\n    %d Slip Secs, %d Fr Loss Secs, "
           "%d Line Err Secs, %d Degraded Mins",
           tbl->t1CSS, tbl->t1SEFS, tbl->t1LES, tbl->t1DM);

    printf("\n    %d Errored Secs, %d Bursty Err Secs, "
           "%d Severely Err Secs, %d Unavail Secs",
           tbl->t1ES, tbl->t1BES, tbl->t1SES, tbl->t1UAS);

}

static void t1_show_total_performance_stats (csu_dsu_instance_t *cd_info)
{
    t1_current_interval_user_data *current;
    t1_total_user_data *total;
    uchar valid_intervals;

    if (!csu_dsu_chk_instance(cd_info))
        return;

    if (!(current = malloc(sizeof(t1_current_interval_user_data))))
	return;
 
    if (!(total = malloc(sizeof(t1_total_user_data)))) {
	free (current);
	return;
    }
	
   if (csu_dsu_register_RW(cd_info, T1_VALID_INTERVALS_REG,
                           T1_VALID_INTERVALS_LEN, &valid_intervals,
                           READ_REGISTER) != DSU_REQUEST_COMPLETE)
       printf("\nTotal Data (unknown number of intervals):");
   else
       printf("\nTotal Data (last %d 15 minute intervals):", valid_intervals);

   if (csu_dsu_register_RW(cd_info, T1_TOTAL_DATA_REG,
                           sizeof(t1_total_user_data), total,
                           READ_REGISTER) != DSU_REQUEST_COMPLETE) 
       printf("\nFailed to read total data");
   else
        t1_show_total_data(total);
   
    if (csu_dsu_register_RW(cd_info, T1_CURRENT_INTERVAL_DATA_REG,
			    sizeof(t1_current_interval_user_data), current,
			    READ_REGISTER) != DSU_REQUEST_COMPLETE) 
	printf("\nFailed to read current interval data");
    else {
        printf("\nData in current interval (%d seconds elapsed):",
		current->t1_seconds_elapsed);
    	t1_show_interval_data(&current->t1_current_int);
   }

   free(current);
   free(total);
}

static void t1_show_24hr_perf_stat (csu_dsu_instance_t *cd_info, 
				    uchar start_interval, uchar end_interval)
{ 
    uchar num_valid_intervals;
    int  num_intervals_read;
    int i;
    t1_interval_user_data *t1_24hr;
    t1_interval_user_data *t1_next_interval;
	

    if (!csu_dsu_chk_instance(cd_info))
        return;

   if (csu_dsu_register_RW(cd_info, T1_VALID_INTERVALS_REG, 
                           T1_VALID_INTERVALS_LEN,
                           &num_valid_intervals,
                           READ_REGISTER) != DSU_REQUEST_COMPLETE) {
       /* if can't retrieve valid intervals, pointless to show anything */
       printf("\nCouldn't read module statistics archive");
       return;
   }

   if (start_interval > num_valid_intervals) {
       printf("\nStart interval out of range - total number of intervals %d", num_valid_intervals);
       return;
   }

    /* First show total and current interval data */
    t1_show_total_performance_stats(cd_info);

   /* Perform range checks */
   start_interval = (start_interval <= 0) ? 1 : start_interval;
   end_interval = (end_interval > num_valid_intervals) ? num_valid_intervals :
							   end_interval;
   num_intervals_read = end_interval - start_interval + 1;
    
    t1_24hr = malloc(num_intervals_read * sizeof(t1_interval_user_data));
    if (!t1_24hr) 
	return;

    /* First tell CSU/DSU which interval to start from */
    if (csu_dsu_register_RW(cd_info, T1_INTERVAL_SELECTOR_REG, 
			    T1_INTERVAL_SELECTOR_LEN,
			    &start_interval, 
			    WRITE_REGISTER) != DSU_REQUEST_COMPLETE) {
	printf("\nCouldn't read module statistics archive");
	free(t1_24hr);
	return;
    }

    /* Here goes the biggest read in the T1 module */
    if (csu_dsu_register_RW(cd_info, T1_WHOLE_24HR_ARCHIVE_REG,
			    (num_intervals_read * sizeof(t1_interval_user_data)),
			    t1_24hr,
			    READ_REGISTER) != DSU_REQUEST_COMPLETE) {
	printf("\nCouldn't read module statistics archive");
	free(t1_24hr);
	return;
    }

    process_sleep_for(ONESEC/2);

    t1_next_interval = t1_24hr;
    for (i = start_interval; i <= end_interval; i++) {
	printf("\nData in Interval %d:", i);
	t1_show_interval_data(t1_next_interval ++);
    }

    free(t1_24hr);
}

/*
 *	PARSER CONFIGURATION COMMANDS
 */

static boolean t1_chk_rem_activated_lpbacks (csu_dsu_instance_t *cd_info)
{
    if (cd_info->dsu_network_alarm_status & (0x0001 << T1_REMOTE_ACTIVATED_LOOPBACK))
	return (TRUE);
    else
	return (FALSE);
}
 
/*
 * t1_set_test_fraction - selects full or fractional bandwidth for the 
 *			  loopbacks - DTE, network or remote
 */
int t1_set_test_fraction (csu_dsu_instance_t *cd_info, uchar test_bandwidth)
{
    int result;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    if ((test_bandwidth != T1_TEST_FRACTION_FULL_BW) && 
	 (test_bandwidth != T1_TEST_FRACTION_ALLOCATED_FRACTION))
	return (DSU_WRONG_FORMAT);

    result = csu_dsu_register_RW(cd_info, T1_TEST_FRACTION_REG,
                                 T1_TEST_FRACTION_LEN, &test_bandwidth,
                                 WRITE_REGISTER);
    return (result);
}

    
/*
 * t1_set_line_framing - sets the SF or ESF framing format for the line
 */
int t1_set_line_framing (csu_dsu_instance_t *cd_info, uchar parser_framing_value)
{
    int result;
    uchar framing_value;
    
    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    switch (parser_framing_value) {
	case CSU_DSU_FRAMING_SF:
	    framing_value = T1_FRAMING_TYPE_SF;
	    break;
	case CSU_DSU_FRAMING_ESF:
	    framing_value = T1_FRAMING_TYPE_ESF;
	    break;
	default:
	    return (DSU_WRONG_FORMAT);
    }

    if ((result = csu_dsu_register_RW(cd_info, T1_FRAMING_TYPE_REG,
				      T1_FRAMING_TYPE_LEN, &framing_value,
				      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE)  
	cd_info->t1_framing_type = parser_framing_value;

    return (result);
}

/*
 * t1_set_line_code - sets the AMI or B8ZS channel capability for the line
 */
int t1_set_line_code (csu_dsu_instance_t *cd_info, uchar parser_linecode_value)
{
    int result;
    uchar linecode_value;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    switch (parser_linecode_value) {
	case CSU_DSU_LINECODE_AMI:
	    linecode_value = T1_LINE_TYPE_AMI;
	    /*
	     * If AMI mode is being selected (i.e. no clear channel capability)
	     * and not transmitting with bit 8 forced to 1, give warning to user
	     */
	    if (cd_info->dsu_data_scrambler_mode != 
				CSU_DSU_DCODE_TIMESLOT_56KBPS) {
		printf("\n%% WARNING - timeslot 64kbps and AMI mode selected");
	    }
	    break;
	case CSU_DSU_LINECODE_B8ZS:
	    linecode_value = T1_LINE_TYPE_B8ZS;
	    break;
	default:
            return (DSU_WRONG_FORMAT);
    }

    if ((result = csu_dsu_register_RW(cd_info, T1_LINE_TYPE_REG,
                                      T1_LINE_TYPE_LEN, &linecode_value,
                                      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE)  
        cd_info->t1_line_type = parser_linecode_value;

    return (result);
}

/*
 * t1_set_clock_source - sets network or internal line clocking
 */
int t1_set_clock_source (csu_dsu_instance_t *cd_info, uchar parser_clocksource_value)
{
    int result;
    uchar clocksource_value;
    uchar backup_clocksource;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    /* Setup the backup clocksource to opposite of the main source */
    switch (parser_clocksource_value) {
	case CSU_DSU_CLK_SRC_INT:
    	    clocksource_value = DSU_CLOCKSOURCE_INTERNAL;
	    backup_clocksource = DSU_CLOCKSOURCE_NETWORK;
	    break;
	case CSU_DSU_CLK_SRC_LINE:
            clocksource_value = DSU_CLOCKSOURCE_NETWORK;
	    backup_clocksource = DSU_CLOCKSOURCE_INTERNAL;
	    break;
	default:
	    return (DSU_WRONG_FORMAT);
    }

    if (csu_dsu_register_RW(cd_info, T1_BACKUP_CLOCK_SOURCE_REG,
                            T1_BACKUP_CLOCK_SOURCE_LEN, &backup_clocksource,
                            WRITE_REGISTER) != DSU_REQUEST_COMPLETE) 
        return (DSU_INTERMEDIATE_REG_RW_FAILURE);

    /* Now write the main clocksource value */
    if ((result = csu_dsu_register_RW(cd_info, DSU_CLOCK_SOURCE_REG,
                                      DSU_CLOCK_SOURCE_LEN,
                                      &clocksource_value,
                                      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE) {
        cd_info->dsu_clocksource = parser_clocksource_value;
    }

    return (result);
}

/*
 * t1_set_data_coding - sets the line data coding
 *                      called from parser and timeslots command
 *                      Parser - only values are NORMAL & INVERTED
 *                      Timeslots - only NORMAL & FORCED (56kbps)
 */
int t1_set_data_coding (csu_dsu_instance_t *cd_info, uchar parser_data_coding)
{
    int result;
    uchar data_coding;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    switch (parser_data_coding) {
	case CSU_DSU_DCODE_NORMAL:
            data_coding = DSU_TRANSMIT_NORMAL;
            if (cd_info->t1_timeslots_speed == CSU_DSU_TS_SPEED_56) 
                /* This should never happen */
                return (DSU_REQUEST_COMPLETE);
            break;
	case CSU_DSU_DCODE_INVERTED:
            data_coding = DSU_TRANSMIT_INVERTED_SCRAMBLED;
            if (cd_info->t1_timeslots_speed == CSU_DSU_TS_SPEED_56) {
                printf("\nCannot choose inverted data mode if timeslot speed is 56kbps");
                return (DSU_WRONG_FORMAT);
	    }
            break;
	case CSU_DSU_DCODE_TIMESLOT_56KBPS:
            data_coding = T1_TRANSMIT_56KBPS;
            break;
        default:
            return (DSU_WRONG_FORMAT);
    }

    if ((result = csu_dsu_register_RW(cd_info, 
				      DSU_DATA_TRANSMIT_OPTIONS_REG,
                                      DSU_DATA_TRANSMIT_OPTIONS_LEN,
                                      &data_coding, 
				      WRITE_REGISTER)) == 
						DSU_REQUEST_COMPLETE)
        cd_info->dsu_data_scrambler_mode = parser_data_coding;
    
    return (result);
}

/*
 * t1_set_timeslots - sets the number and position of each ds0 timeslot
 *		      that comprises the t1 interface
 *		      Also sets the individual ds0 speed 
 */
int t1_set_timeslots (csu_dsu_instance_t *cd_info, ulong timeslots_bitmap, 
		      uchar parser_timeslots_speed)
{
    int result;
    uchar old_timeslots_speed;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);
     
    /*
     * First process the timeslot speed command - if invalid
     * exit (without configuring the timeslots) so user can rethink
     * ds0 allocation strategy
     */
    old_timeslots_speed = cd_info->t1_timeslots_speed;

    switch (parser_timeslots_speed) {
	case CSU_DSU_TS_SPEED_64:

	    /* If line code is AMI give warning */
	    if (cd_info->t1_line_type == CSU_DSU_LINECODE_AMI)
		printf("\n%% WARNING - Each timeslot = 64kpbs and AMI mode selected");

	    /*
	     * If speed is already 64kpbs, we don't call t1_set_data_coding again
	     * because it would reset data coding mode to normal if set to inverted
	     */ 
	    if (cd_info->dsu_data_scrambler_mode == CSU_DSU_DCODE_TIMESLOT_56KBPS) {
		/*
		 * Make t1_set_data_coding think we're already operating at 64kpbs
		 */
		cd_info->t1_timeslots_speed = CSU_DSU_TS_SPEED_64;
		if (t1_set_data_coding(cd_info, CSU_DSU_DCODE_NORMAL) !=
		     				DSU_REQUEST_COMPLETE) {
		    cd_info->t1_timeslots_speed = old_timeslots_speed;
		    return (DSU_INTERMEDIATE_REG_RW_FAILURE);
		}
	     }
	     break;
	case CSU_DSU_TS_SPEED_56:
	   if (cd_info->dsu_data_scrambler_mode == 
					CSU_DSU_DCODE_INVERTED) {
	        printf("\nMust disable inverted data-coding before 56kbpbs/timeslot");
	       	return (DSU_WRONG_FORMAT);
	   }

           if (t1_set_data_coding(cd_info, CSU_DSU_DCODE_TIMESLOT_56KBPS) != 
                                                DSU_REQUEST_COMPLETE) 
                    return (DSU_INTERMEDIATE_REG_RW_FAILURE);
	   cd_info->t1_timeslots_speed = CSU_DSU_TS_SPEED_56;
	   break; 
	default:
	   return (DSU_WRONG_FORMAT);
    }

    /*
     * Now process the individual timeslots that comprise the T1 
     */
    if (timeslots_bitmap == 0) 
	printf("\nWARNING - no timeslots allocated to interface");

    if ((result = csu_dsu_register_RW(cd_info, T1_FRACTION_TIMESLOTS_REG, 
				      T1_FRACTION_TIMESLOTS_LEN,
				      &timeslots_bitmap, 
				      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE) 
	cd_info->t1_timeslots_bitmap = timeslots_bitmap;
    else 
	printf("\nWARNING - timeslots command not accepted by service-module");

    return (result);
}

/*
 * t1_set_remote_loopcode - sets the loopcode used in initiating/terminating
 *			    remote loopbacks
 *			    Full b/w loopcodes - standard, alternate only
 *			    Fractional loopcodes - standard, alternate, v.54 only
 */
int t1_set_remote_loopcode (csu_dsu_instance_t *cd_info, int parser_bandwidth, 
			    int parser_loopcode)
{
    int result;
    uchar reg_addr;
    uchar *cd_param_ptr;
    uchar loopcode;
    int bandwidth;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    switch (parser_loopcode) {
	case CSU_DSU_RMT_LPBACK_TYPE_DEF:
	    loopcode = T1_STANDARD_LOOPCODE;
	    break;
	case CSU_DSU_RMT_LPBACK_TYPE_ALT:
	    loopcode = T1_ALTERNATE_LOOPCODE;
	    break;
	case CSU_DSU_RMT_LPBACK_TYPE_V54:
	    loopcode = T1_V54_FRACTIONAL_LOOPCODE;
	    break;
	case CSU_DSU_RMT_LPBACK_TYPE_DISABLE:
	    loopcode = T1_DISABLE_REM_LOOP;
	    break;
	default:	
	    return (DSU_WRONG_FORMAT);
    }

    switch (parser_bandwidth) {
	case CSU_DSU_RMT_LPBACK_MODE_FULL:
	    bandwidth = T1_TEST_FRACTION_FULL_BW;
	    if (loopcode == T1_V54_FRACTIONAL_LOOPCODE) {
		printf("\nV.54 loopcodes only for fractional b/w loopbacks");
		return (DSU_WRONG_FORMAT);
	    }
	    reg_addr = T1_FULL_BW_LOOPCODE_REG;
	    cd_param_ptr = &cd_info->t1_full_bw_loopcode;
	    break;
	case CSU_DSU_RMT_LPBACK_MODE_FRACT:
	    bandwidth = T1_TEST_FRACTION_ALLOCATED_FRACTION;
	    reg_addr = T1_FRACTIONAL_LOOPCODE_REG;
	    cd_param_ptr = &cd_info->t1_fractional_bw_loopcode;
	    break;
	default:
	    return (DSU_WRONG_FORMAT);
    }

    if ((result = csu_dsu_register_RW(cd_info, reg_addr, 
                                      T1_TEST_LOOPCODE_LEN, &loopcode, 
				      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE)
	*cd_param_ptr = parser_loopcode;

    return (result);
}
/*
 * t1_set_test_pattern - sets up the CSU/DSU for a user selected test pattern to use
 *			 for a remote loopback test
 */
static int t1_set_test_pattern (csu_dsu_instance_t *cd_info, int test_pattern, 
				ulong user_pattern)
{
    int result;
    uchar dsu_test_pattern;
    ulong dsu_user_pattern = 0;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    switch (test_pattern) {
	case CSU_DSU_LPBACK_PAT_NOSEL:
	    return (DSU_REQUEST_COMPLETE);
	    break;
	case CSU_DSU_LPBACK_PAT_QRW:
	    dsu_test_pattern = T1_QRW;
	    break;
	case CSU_DSU_LPBACK_PAT_1IN8:
	    dsu_test_pattern = T1_1IN8;
	    break;
	case CSU_DSU_LPBACK_PAT_3in24:
	    dsu_test_pattern = T1_3IN24;
	    break;
	case CSU_DSU_LPBACK_PAT_1IN2:
	    dsu_test_pattern = T1_1IN1;
	    break;
	case CSU_DSU_LPBACK_PAT_1IN1:
	    dsu_test_pattern = T1_ALL_1;
	    break;
	case CSU_DSU_LPBACK_PAT_0IN1:
	    dsu_test_pattern = T1_ALL_0;
	    break;
	case CSU_DSU_LPBACK_PAT_1IN3:
	    dsu_test_pattern = T1_1IN2;
	    break;
	case CSU_DSU_LPBACK_PAT_1IN5:
	    dsu_test_pattern = T1_1IN4;
	    break;
	case CSU_DSU_LPBACK_PAT_UP:
	    /*
	     * If user pattern selected tell CSU/DSU about it 
	     * Need to swap bytes of user pattern from MSB to LSB for this to work
	     */
	    dsu_test_pattern = T1_USER_TEST_PATTERN_1;
	    dsu_user_pattern = h2litl(user_pattern);
	    if (csu_dsu_register_RW(cd_info, T1_USER_TEST_PATTERN1_REG,
				    T1_USER_TEST_PATTERN1_LEN,
				    &dsu_user_pattern,
				    WRITE_REGISTER) != DSU_REQUEST_COMPLETE) {
	        printf("\nFailed to write User selected pattern");
		return (DSU_INTERMEDIATE_REG_RW_FAILURE);
	    } 
	    break;
	default:
	    printf("\nSelected test pattern not supported");
	    return (DSU_WRONG_FORMAT);
    }

    result = csu_dsu_register_RW(cd_info, DSU_TEST_PATTERN_SELECT_REG,
                                 DSU_TEST_PATTERN_SELECT_LEN,
                                 &dsu_test_pattern, WRITE_REGISTER);

    return (result);
}

/*
 * t1_remote_loopback command - sets parameters for remote loopback
 *				and awakens background process 
 */
static int t1_remote_loopback_command (csu_dsu_instance_t *cd_info, 
				       int loopback_command, int loopback_type,
				       int test_pattern, ulong user_pattern)
{
    uchar dsu_loopback_command;
    uchar test_fraction;
    ulong bit_errors;
    int result;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    if (loopback_command == CSU_DSU_LPBACK_NONE) {
	switch (cd_info->dsu_loopback_type) {
	    case CSU_DSU_LPBACK_TYPE_SJ:
		dsu_loopback_command = DSU_SMART_JACK_LOOPDOWN;
            	break;
	    case CSU_DSU_LPBACK_TYPE_DEF:
	    case CSU_DSU_LPBACK_TYPE_FR:
	    default:
                /*
                 * When we're looping down as part of the csu/dsu reset command
                 * the csu/dsu forgets about what type of loopback it was performing
                 * hence - we need to tell it
                 */
                test_fraction = (cd_info->dsu_loopback_type == CSU_DSU_LPBACK_TYPE_FR) ?
                                 T1_TEST_FRACTION_ALLOCATED_FRACTION :
                                 T1_TEST_FRACTION_FULL_BW;
                if (t1_set_test_fraction(cd_info, test_fraction) != DSU_REQUEST_COMPLETE)
                    return (DSU_INTERMEDIATE_REG_RW_FAILURE);
		dsu_loopback_command = DSU_REMOTE_LOOPDOWN;
		break;
	}
     }
     else if (loopback_command == CSU_DSU_LOOPBACK_REMOTE) {
	switch (loopback_type) {
	    case CSU_DSU_LPBACK_TYPE_DEF:
	    case CSU_DSU_LPBACK_TYPE_FR:
		test_fraction = (loopback_type == CSU_DSU_LPBACK_TYPE_FR) ?
				 T1_TEST_FRACTION_ALLOCATED_FRACTION :
				 T1_TEST_FRACTION_FULL_BW;
		if (t1_set_test_fraction(cd_info, test_fraction) != DSU_REQUEST_COMPLETE)
		    return (DSU_INTERMEDIATE_REG_RW_FAILURE);

		/*
		 * See if we need to send a test pattern
		 */
		dsu_loopback_command = (test_pattern == CSU_DSU_LPBACK_PAT_NOSEL) ?
				        DSU_REMOTE_LOOPUP :
					DSU_REMOTE_LOOPUP_SEND_PATTERN;
		break;
	     case CSU_DSU_LPBACK_TYPE_SJ:
		dsu_loopback_command = (test_pattern == CSU_DSU_LPBACK_PAT_NOSEL) ?
					DSU_SMART_JACK_LOOPUP :
				        DSU_SMART_JACK_LOOPUP_SEND_PATTERN;
		break;
	     default:
		return (DSU_WRONG_FORMAT);
        }
	
	if (t1_set_test_pattern(cd_info, test_pattern, user_pattern) != DSU_REQUEST_COMPLETE) 
	    return (DSU_INTERMEDIATE_REG_RW_FAILURE);
    }
    else {
	printf("\nUnsupported loopback type");
	return (DSU_WRONG_FORMAT);
    }

    /* If loopback timer is being used, bail out right now */
    if (mgd_timer_running(&cd_info->dsu_diagnostic_timer)) {
   	printf("\nLoopback failed - timer being used");
	return (DSU_NO_MEMORY);
    }

    /*
     * Bit errors registers clears as soon as loopdown is selected -
     * cache the value before selecting the test
     */
    if (loopback_command == CSU_DSU_LPBACK_NONE) {
        if (csu_dsu_register_RW(cd_info, DSU_BERT_REG,
                                DSU_BERT_LEN, &bit_errors,
                                READ_REGISTER) != DSU_REQUEST_COMPLETE)
            bit_errors = 0;
    }

    /*
     * Mask alarms off to prevent race with in sync on test pattern interrupt
     * As soon as we select loopdown, module will go out of sync but we need to
     * preserve the value of the dsu_loopback_synced_on_pattern flag
     */
    BOA_MASK_ALM_INTERRUPT(cd_info->wan_misc_reg);
 
    /* Now actually start the remote loopback test */
    if ((result = csu_dsu_register_RW(cd_info, DSU_TEST_TYPE_SELECT_REG,
                                      DSU_TEST_TYPE_SELECT_LEN,
                                      &dsu_loopback_command,
                                      WRITE_REGISTER)) != DSU_REQUEST_COMPLETE)  {
	printf("\nLoopback test failed");
	BOA_ENABLE_ALM_INTERRUPT(cd_info->wan_misc_reg);
	return (result);
    }

    if (loopback_command == CSU_DSU_LOOPBACK_REMOTE) { 
        cd_info->current_loopback_command = CSU_DSU_LOOPBACK_REMOTE;
        cd_info->current_loopback_state =   DSU_ATTEMPTING_LOOPUP;
	cd_info->dsu_loopback_synced_on_pattern = FALSE;
	TIMER_STOP(cd_info->loopback_pattern_sync_duration);
        cd_info->dsu_loopback_type = loopback_type;
        cd_info->dsu_loopback_test_pattern = test_pattern;
        cd_info->dsu_loopback_user_pattern = user_pattern;
    }
    else {
	/* 
	 * For loopdowns, we still want to make router think we're in
	 * remote loopback	
	 */
	cd_info->dsu_loopback_bit_errors = bit_errors;
	cd_info->current_loopback_state = DSU_ATTEMPTING_LOOPDOWN;
    }

    BOA_ENABLE_ALM_INTERRUPT(cd_info->wan_misc_reg);

    /* 
     * Now start the diagnostic timer so background process can 
     * check on loopback status
     */
    mgd_timer_start(&cd_info->dsu_diagnostic_timer, 5*ONESEC);

    return (DSU_REQUEST_COMPLETE);
}

/*
 * t1_get_rem_lpback_state
 */
static int t1_get_rem_lpback_state (csu_dsu_instance_t *cd_info)
{
    uchar current_loopback;
    uchar test_status;

    if (!csu_dsu_chk_instance(cd_info))
        return (NO_REMOTE_TEST);

    /*
     * First check the test status register - if no remote loopback
     * related test ongoing say so
     */
    if (csu_dsu_register_RW(cd_info, DSU_TEST_TYPE_SELECT_REG,
                            DSU_TEST_TYPE_SELECT_LEN, &current_loopback,
                            READ_REGISTER) != DSU_REQUEST_COMPLETE) 
        return (NO_REMOTE_TEST);

    switch (current_loopback) {
	case DSU_REMOTE_LOOPUP:
	case DSU_REMOTE_LOOPUP_SEND_PATTERN:
	case DSU_REMOTE_LOOPDOWN:
	case DSU_SMART_JACK_LOOPUP:
	case DSU_SMART_JACK_LOOPUP_SEND_PATTERN:
	case DSU_SMART_JACK_LOOPDOWN:
	    /* Only in these cases will the test  */
	    /* status register have a valid value */
	    break;
	default:
	    return (NO_REMOTE_TEST);
    }

    if (csu_dsu_register_RW(cd_info, DSU_TEST_STATUS_REG,
			    DSU_TEST_STATUS_LEN, &test_status,
			    READ_REGISTER) != DSU_REQUEST_COMPLETE)
	return (DSU_TEST_STATUS_UNREADABLE);

    switch (test_status) {
	case ATTEMPTING_REMOTE_TEST:
	case PASSED_REMOTE_TEST:
	case FAILED_REMOTE_TEST:
	    break;
	default:
	    if (csu_dsu_debug)
	        buginf("\nSERVICE_MODULE(%d):Bad register value-cancelling remote loopback",
			cd_info->interface_number);
	    return (NO_REMOTE_TEST);
    }

    return (test_status);
}


/*
 * t1_set_line_buildout - sets the attenuation loss because of cable length
 *			  and distance to next telco repeater
 */
int t1_set_line_buildout (csu_dsu_instance_t *cd_info, uchar parser_lbo_value)
{
    uchar lbo_value;
    int result;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    switch (parser_lbo_value) {
	case CSU_DSU_LBO_NONE:
	    lbo_value = T1_LBO_0DB;
	    break;
	case CSU_DSU_LBO_7_5_DB:
	    lbo_value = T1_LBO_7_5DB;
	    break;
	case CSU_DSU_LBO_15DB:
	    lbo_value = T1_LBO_15DB;
	    break;
     	default:
	    return (DSU_WRONG_FORMAT);
    }

    if ((result = csu_dsu_register_RW(cd_info, T1_LBO_REG, T1_LBO_LEN,
                                      &lbo_value,  
				      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE)
        cd_info->t1_line_build_out = parser_lbo_value;

    return (result);
}

/*
 * t1_yellow_alarm_enable - enables yellow alarm detection and transmission
 *                          yellow alarms are disabled by default
 */
int t1_yellow_alarm_enable (csu_dsu_instance_t *cd_info, boolean yellow_enable)
{
    int result;
    uchar yellow_enable_value;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    yellow_enable_value = (yellow_enable) ? T1_YELLOW_ALARM_ENABLED :
					    T1_YELLOW_ALARM_DISABLED;

    if ((result = csu_dsu_register_RW(cd_info, T1_YELLOW_ALARM_OPTION_REG,
				      T1_YELLOW_ALARM_OPTION_LEN, 
                                      &yellow_enable_value,
                                      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE) 
        cd_info->t1_yellow_alarm_mode = yellow_enable;

    return (result);
}

static ulong t1_specify_nondefault_access_time (uchar register_addr) 
{
    switch (register_addr) {
	case T1_TOTAL_DATA_REG:
	    return (T1_TOTAL_DATA_ACCESS_MSECS);
	    break;
	case T1_WHOLE_24HR_ARCHIVE_REG:
	    return (T1_WHOLE_24HR_ARCHIVE_ACCESS_MSECS);
	    break;
	default:
	    return (0);
    }

    return (0);
}

/*
 * Called after CSU/DSU reset to re configure user configurations
 */
static boolean t1_set_dsu_current_config (csu_dsu_instance_t *cd)
{
    if (!csu_dsu_chk_instance(cd))
        return (FALSE);

    /* Call all configuration routines with parameters */
    t1_set_data_coding(cd, cd->dsu_data_scrambler_mode);
    t1_set_timeslots(cd, cd->t1_timeslots_bitmap,
                         cd->t1_timeslots_speed); 
    t1_set_line_framing(cd, cd->t1_framing_type);
    t1_set_line_code(cd, cd->t1_line_type);
    t1_set_clock_source(cd, cd->dsu_clocksource);
    t1_set_remote_loopcode(cd, CSU_DSU_RMT_LPBACK_MODE_FULL,
                               cd->t1_full_bw_loopcode);
    t1_set_remote_loopcode(cd, CSU_DSU_RMT_LPBACK_MODE_FRACT,
                               cd->t1_fractional_bw_loopcode);
    t1_set_line_buildout(cd, cd->t1_line_build_out);
    t1_yellow_alarm_enable(cd, cd->t1_yellow_alarm_mode);

    /* Cancel any loopbacks from previous session */
    csu_dsu_loopback_command(cd, CSU_DSU_LPBACK_NONE, 0, 0, 0);

    return (TRUE);
}

/*
 * Initializes driver data structure parameters to defaults
 */
void t1_set_default_values (csu_dsu_instance_t *cd_info)
{
    if (!cd_info)
        return;

    if (cd_info->module_type == CSU_DSU_FT1) {
        cd_info->t1_framing_type =              CSU_DSU_FRAMING_DEFAULT;
        cd_info->t1_line_type =                 CSU_DSU_LINECODE_DEFAULT;
        cd_info->dsu_clocksource =              CSU_DSU_CLK_SRC_DEFAULT;
        cd_info->dsu_data_scrambler_mode =      CSU_DSU_DCODE_DEFAULT;
        cd_info->t1_timeslots_speed =           CSU_DSU_TS_SPEED_DEFAULT;
        cd_info->t1_timeslots_bitmap =          T1_FRACTION_TIMESLOTS_DEF;
        cd_info->t1_full_bw_loopcode =          CSU_DSU_RMT_LPBACK_TYPE_DEFAULT;
        cd_info->t1_fractional_bw_loopcode =    CSU_DSU_RMT_LPBACK_TYPE_DEFAULT;
        cd_info->t1_line_build_out =            CSU_DSU_LBO_DEFAULT;
        cd_info->t1_yellow_alarm_mode =         CSU_DSU_YELLOW_ALARM_MODE_DEF;
	cd_info->dsu_network_alarm_status = 	DSU_NO_ALARMS;
	cd_info->dsu_alarm_mask = 		T1_ALARM_STATUS_MASK_DEF;
        cd_info->module_int_protocol_rev =      DIGITAL_LINK_FT1_HIGHEST_REV;
        cd_info->mod_highest_int_protocol_rev = DIGITAL_LINK_FT1_HIGHEST_REV;
	cd_info->dsu_get_alarm_status = 	t1_alarm_processor;
	cd_info->dsu_alarm_messages =		t1_alarm_messages;
        cd_info->dsu_remote_tests =             t1_chk_rem_activated_lpbacks;
        cd_info->dsu_check_connection_status =  NULL;
        cd_info->dsu_remote_loopback_command =  t1_remote_loopback_command;
	cd_info->dsu_remote_lpback_state = 	t1_get_rem_lpback_state;
	cd_info->dsu_current_line_bandwidth  =  t1_current_line_speed;
	cd_info->dsu_clear_alarm_data =		t1_clear_alarm_data;
        cd_info->dsu_set_current_config =       t1_set_dsu_current_config;
        cd_info->dsu_show_network_status =      t1_show_network_status;
	cd_info->dsu_show_alarm_counters = 	t1_show_alarm_counters;
	cd_info->dsu_show_module_config =	t1_show_module_configuration;
	cd_info->dsu_show_performance_stats =	t1_show_total_performance_stats;
	cd_info->dsu_show_full_performance_stats = t1_show_24hr_perf_stat;
        cd_info->usr_cfg_t1_full_remote_loopback_type = CSU_DSU_RMT_LPBACK_TYPE_DEF;
        cd_info->usr_cfg_t1_fr_remote_loopback_type = CSU_DSU_RMT_LPBACK_TYPE_DEF;

 	/*
	 * Setup register access timing values
	 */
	cd_info->dsu_interread_wait_time = timer_calibrate(T1_INTERBYTE_READ_WAIT,
							   FALSE);
	cd_info->dsu_interwrite_wait_time = timer_calibrate(T1_INTERBYTE_WRITE_WAIT,
							    FALSE);
	cd_info->dsu_specify_reg_wait_time = t1_specify_nondefault_access_time;

    }
}
 
	 
