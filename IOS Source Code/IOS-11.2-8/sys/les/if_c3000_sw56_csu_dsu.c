/* $Id: if_c3000_sw56_csu_dsu.c,v 3.2.2.2 1996/08/28 12:57:34 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_sw56_csu_dsu.c,v $
 *------------------------------------------------------------------
 * if_c3000_sw56_csu_dsu.c - Switched 56k Integrated CSU/DSU driver code
 *
 * March 1996, Shalabh Agarwal
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_sw56_csu_dsu.c,v $
 * Revision 3.2.2.2  1996/08/28  12:57:34  thille
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
 * Revision 3.2  1996/03/29  03:49:59  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:28  sagarwal
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
static char *const sw56_alarm_messages[SW56_MAX_ALARM_EVENTS] = {
        "oos/oof", 			/* Alarm code received from network */
	"loss of signal",		/* 4W only - no signal 		    */
        "loss of sealing current", 	/* 4W only - no loop current 	    */
	"loss of frame", 		/* 4W only - for 64k dds            */
	"rate adaption attempts",	/* 4W only - loss of line rate for  */
					/* auto rate dds		    */
        "call connect", 		/* DSU loopback test from telco     */
	"loopback test from telco", 	/* CSU loopback test from telco	    */
	"loopback test from remote module" /* CSU loopback request from remote end  */
};

static char *const sw56_dial_messages[SW56_MAX_DIAL_STATUS] = {
        "dialling number", "active", "switch busy",
        "remote end busy", "no answer"
};

static boolean sw56_alarm_processor (csu_dsu_instance_t *cd_info, 
                                     int *low_alarm_index, int *hi_alarm_index,
                                     ushort *alarm_transitions,
                                     ushort *alarm_real)
{
    struct sw56_alarm_loopstatus sw56_alarm;
    uchar dial_bit, all_supported_alarms_mask;
    static boolean only_one_message = TRUE;

    if (!cd_info)
        return FALSE;

    if (csu_dsu_register_RW(cd_info, SW56_LOOP_STATUS_REG,
                            sizeof(sw56_alarm), &sw56_alarm,
                            READ_REGISTER) != DSU_REQUEST_COMPLETE) {
	errmsg(&msgsym(ALARMFAILURE, SERVICE_MODULE),
	       cd_info->interface_number);
        return (FALSE);
    }

    *low_alarm_index = SW56_RXING_OOS_OOF;
    *hi_alarm_index = SW56_MAX_ALARM_EVENTS;
    *alarm_transitions = (ushort) sw56_alarm.transition_status;
    *alarm_real = (ushort) sw56_alarm.current_status; 

    /* Check for any alarms we don't know about */
    if (only_one_message) {
        all_supported_alarms_mask = (cd_info->module_type == CSU_DSU_SW56_2_WIRE) ? 
				     SW56_2W_ALL_ALARMS_MASK :
                		     SW56_4W_ALL_ALARMS_MASK;
        if (sw56_alarm.current_status & ~all_supported_alarms_mask) {
	    errmsg(&msgsym(UNKNOWNALARM, SERVICE_MODULE),
		   cd_info->interface_number, sw56_alarm.current_status);
            only_one_message = FALSE;
        }
    }

    /****************************************
     * Module specific alarm processing stuff
     ****************************************/

    /*
     * In switched mode, payload line and remote loopbacks automatically
     * terminate if a call is dropped - we need to tell the driver s/w
     */
    dial_bit = 0x01 << SW56_CALL_STATUS;
    if ((cd_info->sw56_network_type == CSU_DSU_NWKTYPE_SWITCHED) &&
      (cd_info->dsu_network_alarm_status & dial_bit) && /* connected previously */
      !(sw56_alarm.current_status  & dial_bit)) {       /* but now call dropped */

      switch (cd_info->current_loopback_command) {
          case CSU_DSU_LOOPBACK_LINE:
              if (cd_info->dsu_loopback_type == CSU_DSU_LPBACK_TYPE_FR)
                  csu_dsu_loopback_command(cd_info, CSU_DSU_LPBACK_NONE, 0, 0, 0);
              break;
          case CSU_DSU_LOOPBACK_REMOTE:
              if (cd_info->current_loopback_state == DSU_IN_LOOPBACK)
                  csu_dsu_loopback_command(cd_info, CSU_DSU_LPBACK_NONE, 0, 0, 0);
              break;
          default:
              break;
      }
    }

    return (TRUE);
}

static inline boolean csu_dsu_chk_instance(csu_dsu_instance_t *cd_info)
{
    if (!cd_info)
        return (FALSE);
    else if ((cd_info->module_type != CSU_DSU_SW56_4_WIRE) &&
             (cd_info->module_type != CSU_DSU_SW56_2_WIRE))
        return (FALSE);
    else
        return (TRUE);
}

/*
 * Called after CSU/DSU reset to re configure user configurations
 */
static boolean sw56_set_dsu_current_config (csu_dsu_instance_t *cd)
{
    if (!csu_dsu_chk_instance(cd))
        return (FALSE);

    if (cd->module_type == CSU_DSU_SW56_4_WIRE) {
	sw56_set_clock_source(cd, cd->dsu_clocksource);
	sw56_set_network_type(cd, cd->sw56_network_type,
				  cd->sw56_switched_type);
	sw56_set_clock_rate(cd, cd->sw56_dds_linerate);
	sw56_set_RDL_enable(cd, cd->sw56_remote_loopback_enable);
	if (cd->sw56_network_type == CSU_DSU_NWKTYPE_DDS)
	    sw56_set_data_coding(cd, cd->dsu_data_scrambler_mode);
    }
    else {
	sw56_set_network_type(cd, cd->sw56_network_type,
                                  cd->sw56_switched_type);
	sw56_set_RDL_enable(cd, cd->sw56_remote_loopback_enable);
    }

    /* Cancel any loopbacks from previous session */
    csu_dsu_loopback_command(cd, CSU_DSU_LPBACK_NONE, 0, 0, 0);

    return (TRUE);
}

static void sw56_show_network_status (csu_dsu_instance_t *cd_info)
{
    int i;
    uchar dial_status;
    uchar bitmask = 0x01;

    if (!csu_dsu_chk_instance(cd_info))
        return;

    if (cd_info->sw56_network_type == CSU_DSU_NWKTYPE_SWITCHED) {
	printf("\nConnection state: ");
        if (csu_dsu_register_RW(cd_info, SW56_DIAL_STATUS_REG,
                                SW56_DIAL_STATUS_LEN, &dial_status,
                                READ_REGISTER) != DSU_REQUEST_COMPLETE)
	    printf("Unretrievable from module");
	else {
	    if (dial_status == 0) 
		printf("Idle ");
	    else {
	        for (i=0, bitmask = 0x01; i < SW56_MAX_DIAL_STATUS;
	     	 				i++, bitmask <<= 1)
	   	    if (dial_status & bitmask)
		        printf("%s, ", sw56_dial_messages[i]);
	    }
	}
    }

    /* Now the network receiver status */
    if (cd_info->dsu_network_alarm_status & SW56_USER_REPORTABLE_ALARMS 
					  & ~cd_info->dsu_alarm_mask) {
        printf("\nReceiver has ");
        for (i = SW56_RXING_OOS_OOF, bitmask = 0x01;
             i < SW56_CALL_STATUS; i ++, bitmask <<= 1) 
            if ((cd_info->dsu_network_alarm_status & bitmask)
                        & ~cd_info->dsu_alarm_mask)
                printf("%s, ", sw56_alarm_messages[i]);
    }
    else
	printf("\nReceiver has no alarms.");
}

/*	
 * sw56_get_current_line_speed
 */
static ulong sw56_get_current_line_speed (csu_dsu_instance_t *cd_info, 
					  boolean show_command)
{
    uchar line_rate;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    if (cd_info->module_type == CSU_DSU_SW56_2_WIRE) 
	line_rate = SW56_LINE_RATE_56K;
    else { 
        /*
         * For DDS, check line rate
         */
        if (csu_dsu_register_RW(cd_info, SW56_CURRENT_LINE_RATE_REG,
                                SW56_CURRENT_LINE_RATE_LEN, &line_rate,
                                READ_REGISTER) != DSU_REQUEST_COMPLETE)
	    line_rate = SW56_LINE_RATE_56K;
    }

    if (show_command)
	return (line_rate);

    switch (line_rate) {
            case SW56_LINE_RATE_2400:
                return (2400);
                break;
            case SW56_LINE_RATE_4800:
                return (4800);
                break;
            case SW56_LINE_RATE_9600:
                return (9600);
                break;
            case SW56_LINE_RATE_19200:
                return (19200);
                break;
            case SW56_LINE_RATE_38400:
                return (38400);
                break;
            case SW56_LINE_RATE_56K:
                return (56000);
                break;
            case SW56_LINE_RATE_64K:
                return (64000);
                break;
        default:
                return (56000);
    }
}


static boolean sw56_show_module_configuration (csu_dsu_instance_t *cd_info)
{
    ulong line_rate;

    if (!csu_dsu_chk_instance(cd_info))
	return (FALSE);
  
    printf("\nCurrent line rate is ");
    
    line_rate = sw56_get_current_line_speed(cd_info, TRUE);
    switch (line_rate) {
            case SW56_LINE_RATE_2400:
            	printf("2.4 ");
		break;
            case SW56_LINE_RATE_4800:
            	printf("4.8 ");
		break;
            case SW56_LINE_RATE_9600:
		printf("9.6 ");
            	break;
            case SW56_LINE_RATE_19200:
		printf("19.2 ");
            	break;
            case SW56_LINE_RATE_38400:
		printf("38.4 ");
            	break;
            case SW56_LINE_RATE_56K:
		printf("56 ");
            	break;
            case SW56_LINE_RATE_64K:
		printf("64 ");
		break;
            default:
		printf("unknown ");
    }
    printf("Kbits/sec");

    return (TRUE);
}

static void sw56_show_alarm_counters (csu_dsu_instance_t *cd_info)
{
    uchar bitmask;
    int i;

    if (!csu_dsu_chk_instance(cd_info))
        return;

    for (i = SW56_RXING_OOS_OOF, bitmask = 0x01; 
	 i < SW56_CALL_STATUS; i++, bitmask <<= 1) {
	if (bitmask & cd_info->dsu_alarm_mask)
	    continue;
	printf("\n    %22s:%-5d, ", sw56_alarm_messages[i], 
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

/*
 *	PARSER CONFIGURATION COMMANDS
 */
static boolean sw56_chk_rem_activated_lpbacks (csu_dsu_instance_t *cd_info) 
{
   
    if (cd_info->dsu_network_alarm_status & (0x01 << SW56_CSU_DSU_TELCO_LOOPUP)) 
        return (TRUE);

    return (FALSE);
}

static boolean sw56_chk_connection_active (csu_dsu_instance_t *cd_info, 
				           int loopback_command, int loopback_type) 
{
    uchar dial_status;
    uchar bitmask = 0x01;

    if (!csu_dsu_chk_instance(cd_info))
        return (TRUE);

    if ((cd_info->module_type == CSU_DSU_SW56_4_WIRE) && 
	 (cd_info->sw56_network_type == CSU_DSU_NWKTYPE_DDS))
	return (TRUE);

    switch (loopback_command) {
	case CSU_DSU_LOOPBACK_LINE:
	    if (loopback_type == CSU_DSU_LPBACK_TYPE_DEF)
		return (TRUE);
	    break;
	case CSU_DSU_LOOPBACK_REMOTE:
	    break;
	default:
	    return (TRUE);
    }

    if (csu_dsu_register_RW(cd_info, SW56_DIAL_STATUS_REG,
                            SW56_DIAL_STATUS_LEN, &dial_status,
                            READ_REGISTER) != DSU_REQUEST_COMPLETE)
        return (FALSE);

    bitmask <<= SW56_STATUS_CONNECTION_ACTIVE;
    if (dial_status & bitmask) 
	return (TRUE);
    else
	return (FALSE);
}

static boolean sw56_set_v35_signals (csu_dsu_instance_t *cd_info, 
				     uchar dsu_network_type)
{
    boolean dds_mode;
    boolean configuration_success = TRUE;
    uchar write_value;

    if ((dsu_network_type == SW56_ATT_MODE) || 
	(dsu_network_type == SW56_SPRINT_MODE)) 
        dds_mode = FALSE;
    else
	dds_mode = TRUE;

    /*
     * For both DDS and Switched mode, V.35 DTR option must be normal
     */
    write_value = (dds_mode ? SW56_DTR_IGNORED : SW56_DTR_NORMAL);
    if (csu_dsu_register_RW(cd_info, SW56_DTR_OPTION_REG,
			    SW56_DTR_OPTION_LEN, &write_value, 
			    WRITE_REGISTER) != DSU_REQUEST_COMPLETE) 
        configuration_success = FALSE;

     /*
      * For both DDS and Switched mode, V.35 DCD should be high only with data
      */
     write_value = SW56_DCD_ON_WITH_DATA;
     if (csu_dsu_register_RW(cd_info, SW56_DCD_OPTION_REG,
                             SW56_DCD_OPTION_LEN, &write_value,
                             WRITE_REGISTER) != DSU_REQUEST_COMPLETE)
         configuration_success = FALSE;


    /*
     * If Switched mode, enable v.25bis dialling else disable it
     */
    write_value = (dds_mode ? DISABLE_V25BIS_COMMANDS : ENABLE_V25BIS_COMMANDS);
    if (csu_dsu_register_RW(cd_info, SW56_V25BIS_DIAL_REG,
                            SW56_V25BIS_DIAL_LEN, &write_value,
                            WRITE_REGISTER) != DSU_REQUEST_COMPLETE)
        configuration_success = FALSE;

    /*
     * If switched mode, disable the auto answer function to prevent incoming calls
     * when IDBS_ADMINDOWN
     */
    write_value = SW56_AUTO_ANSWER_DISABLED;
    if (csu_dsu_register_RW(cd_info, SW56_AUTO_ANSWER_MODE_REG,
                            SW56_AUTO_ANSWER_MODE_LEN, &write_value,
                            WRITE_REGISTER) != DSU_REQUEST_COMPLETE)
          configuration_success = FALSE;

    return (configuration_success);
}

/*
 * sw56_set_network_type - selects whether in dds (leased line) mode or 
 *			   switched mode (dialup mode)
 */
int sw56_set_network_type (csu_dsu_instance_t *cd_info, uchar parser_network_type, 
			   uchar parser_switched_carrier) 
{
    boolean result;
    ushort alarm_mask;
    uchar dsu_network_type, dsu_alarm_mask;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);
    if (parser_network_type == CSU_DSU_NWKTYPE_DDS) {
	if (cd_info->module_type == CSU_DSU_SW56_2_WIRE) {
	    printf("\nCannot select DDS for the 2-wire 56k service-module");
	    return (DSU_WRONG_FORMAT);
	}
 	dsu_network_type = SW56_DDS_MODE;	
    } 
    else if (parser_network_type == CSU_DSU_NWKTYPE_SWITCHED) { 	
	/* 
	 * If clocking is set to internal, reject command
	 */
	if (cd_info->dsu_clocksource == CSU_DSU_CLK_SRC_INT) {
	    printf("\n%% Have to use line clocking for switched mode");
	    return (DSU_WRONG_FORMAT);
	}

       /*
        * If were using subrate DDS and then going to switched mode, force
        * user to change clock rate to 56 or auto
        */
       if ((cd_info->sw56_dds_linerate != CSU_DSU_CLK_RATE_56) &&
           (cd_info->sw56_dds_linerate != CSU_DSU_CLK_RATE_AUTO)) {
           printf("\n%% Have to use 56k or auto clock rate for switched mode");
           return (DSU_WRONG_FORMAT);
       }


	switch (parser_switched_carrier) {
	    case CSU_DSU_SW_CARRIER_ATT:
		dsu_network_type = SW56_ATT_MODE;
		break;
	    case CSU_DSU_SW_CARRIER_SPRINT:
		dsu_network_type = SW56_SPRINT_MODE;
		break;
	    case CSU_DSU_SW_CARRIER_OTHER:
		/*
		 * 4-wire should default to AT&T (i.e. disable echo canceller tones)
		 * Although sending echo canceller tones (Sprint mode) is safest, 
		 * calls then take at least 3-4 seconds - a big performance hit
	         */
		if (cd_info->module_type == CSU_DSU_SW56_4_WIRE)
		    dsu_network_type = SW56_ATT_MODE;
		else
		    dsu_network_type = SW56_SPRINT_MODE;
		break;
	    default:
		return (DSU_WRONG_FORMAT);
		break;
	}
    }
    else 
        return (DSU_WRONG_FORMAT); 

    /*
     * If changing between dds and switched on 4-wire, all loopbacks
     * get cancelled - let driver know about that
     */
    if ((cd_info->module_type == CSU_DSU_SW56_4_WIRE) &&
        (parser_network_type != cd_info->sw56_network_type))
        csu_dsu_loopback_command(cd_info, CSU_DSU_LPBACK_NONE, 0, 0, 0);

    /*
     * Now actually change the network type
     */
    if ((result = csu_dsu_register_RW(cd_info, SW56_NETWORK_TYPE_REG,
				      SW56_NETWORK_TYPE_LEN, &dsu_network_type,
				      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE) {
	cd_info->sw56_network_type = parser_network_type;
	cd_info->sw56_switched_type = parser_switched_carrier;
    }
    else
	return (result);

    /*
     * If 4-wire, then need to enable sealing current alarm if in switched mode
     * Sealing current is supplied on telco provided 4-wire networks
     * For dds, permanently disabled because could be running back-to-back
     */
    if (cd_info->module_type == CSU_DSU_SW56_4_WIRE) {
      alarm_mask = (parser_network_type == CSU_DSU_NWKTYPE_DDS) ?
                   SW56_4W_ALARM_MASK_DDS :
                   SW56_4W_ALARM_MASK_SWITCHED;
      dsu_alarm_mask = (uchar) (alarm_mask & 0x00ff);

      if (csu_dsu_register_RW(cd_info, SW56_LOOP_STATUS_MASK_REG,
                              SW56_LOOP_STATUS_MASK_LEN, &dsu_alarm_mask,
                              WRITE_REGISTER) == DSU_REQUEST_COMPLETE)
          cd_info->dsu_alarm_mask = alarm_mask;
    }

    /*
     * Now check dialling setup
     */
    if (!sw56_set_v35_signals(cd_info, dsu_network_type))
	return (DSU_INTERMEDIATE_REG_RW_FAILURE);
    else
	return (DSU_REQUEST_COMPLETE);
}

/*
 * sw56_set_clock_source - only valid for 4-wire CSU/DSU
 *                         sets network or internal clocking 
 */
int sw56_set_clock_source (csu_dsu_instance_t *cd_info, uchar parser_clocksource_value)
{
    boolean result;
    uchar clocksource_value;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    if (cd_info->module_type == CSU_DSU_SW56_2_WIRE)
        return (DSU_WRONG_MODULE_TYPE);

    switch (parser_clocksource_value) {
	case CSU_DSU_CLK_SRC_INT:
	    clocksource_value = DSU_CLOCKSOURCE_INTERNAL;
	    break;
	case CSU_DSU_CLK_SRC_LINE:
	    clocksource_value = DSU_CLOCKSOURCE_NETWORK;
	    break;
	default:
	    return (DSU_WRONG_FORMAT);
    }

    /*
     * If trying to set internal clocking in switched mode, reject command
     */ 
    if ((clocksource_value == DSU_CLOCKSOURCE_INTERNAL) && 
	(cd_info->sw56_network_type != CSU_DSU_NWKTYPE_DDS)) {
	printf("\nIn switched mode, have to use line clocking");
	return (DSU_WRONG_FORMAT);
    }

    if ((result = csu_dsu_register_RW(cd_info, DSU_CLOCK_SOURCE_REG, 
		   		      DSU_CLOCK_SOURCE_LEN, 
				      &clocksource_value, 
				      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE) 
	cd_info->dsu_clocksource = parser_clocksource_value;

    return (result);
}

/*
 * sw56_set_clock_rate - only valid for 4-wire CSU/DSU
 *                       sets a subrate line speed when in DDS mode 
 */
int sw56_set_clock_rate (csu_dsu_instance_t *cd_info, uchar parser_clockrate_value)
{
    uchar clockrate_value;
    boolean result;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    if (cd_info->module_type == CSU_DSU_SW56_2_WIRE)
        return (DSU_WRONG_MODULE_TYPE);
 
    if (cd_info->sw56_network_type == CSU_DSU_NWKTYPE_SWITCHED)
        if ((parser_clockrate_value != CSU_DSU_CLK_RATE_AUTO) &&
            (parser_clockrate_value != CSU_DSU_CLK_RATE_56)) {
            printf("\n%% Cannot set subrate line speed in switched network mode");
            return (DSU_WRONG_FORMAT);
        }

    switch (parser_clockrate_value) {
	case CSU_DSU_CLK_RATE_AUTO:
	    clockrate_value = SW56_LINE_RATE_AUTO;
	    /* if in back-to-back CSU/DSU mode, cannot use this option */
	    printf("\n%% WARNING - auto rate will not work in back-to-back DDS.");
	    break;
	case CSU_DSU_CLK_RATE_2_4:
	    clockrate_value = SW56_LINE_RATE_2400;
	    break; 
	case CSU_DSU_CLK_RATE_4_8:
	    clockrate_value = SW56_LINE_RATE_4800;
	    break;
	case CSU_DSU_CLK_RATE_9_6:
            clockrate_value = SW56_LINE_RATE_9600;
	    break; 
	case CSU_DSU_CLK_RATE_19_2:
	    clockrate_value = SW56_LINE_RATE_19200;
	    break;
	case CSU_DSU_CLK_RATE_38_4:
	    clockrate_value = SW56_LINE_RATE_38400;
	    break;
	case CSU_DSU_CLK_RATE_56:
            clockrate_value = SW56_LINE_RATE_56K;
	    break;
	case CSU_DSU_CLK_RATE_64:
	    clockrate_value = SW56_LINE_RATE_64K;
           /*
            * This is only subrate that cannot be used in back to back mode
            */
           printf("\n%% WARNING - 64k mode will not work in back-to-back DDS.");
	   break;
	default:
	    return (DSU_WRONG_FORMAT);
    }

    if ((result = csu_dsu_register_RW(cd_info, SW56_DDS_LINE_RATE_REG,
                                      SW56_DDS_LINE_RATE_LEN,
                                      &clockrate_value,
                                      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE) 
        cd_info->sw56_dds_linerate = parser_clockrate_value;

    return (result);
}

/*
 * sw56_set_data_coding - only valid for 4-wire CSU/DSU
 *                        allows scrambling data in DDS 64K mode to prevent
 *			  network control code replication
 */
int sw56_set_data_coding (csu_dsu_instance_t *cd_info, uchar data_coding_enable)
{
    uchar actual_dds_linerate;
    uchar dsu_data_coding;
    boolean result;
    char *data_coding_errmsg = "Can configure scrambler only in 64k speed DDS mode";
 
    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    if (cd_info->module_type == CSU_DSU_SW56_2_WIRE)
        return (DSU_WRONG_MODULE_TYPE);

    if (cd_info->sw56_network_type != CSU_DSU_NWKTYPE_DDS) {
	printf("\n%s", data_coding_errmsg);
	return (DSU_WRONG_FORMAT);
    }

    if (data_coding_enable == CSU_DSU_DCODE_NORMAL) 
	dsu_data_coding = DSU_TRANSMIT_NORMAL;
    else if (data_coding_enable == CSU_DSU_DCODE_SCRAMBLED) {
	dsu_data_coding = DSU_TRANSMIT_INVERTED_SCRAMBLED;
	/* Check if line rate is 64k or not */
	switch (cd_info->sw56_dds_linerate) {
	    case CSU_DSU_CLK_RATE_64:
		break;
	    case CSU_DSU_CLK_RATE_AUTO:
		/* Need to find out what the actual line rate is */
		if (csu_dsu_register_RW(cd_info, SW56_CURRENT_LINE_RATE_REG,
		     SW56_CURRENT_LINE_RATE_LEN, &actual_dds_linerate,
		     READ_REGISTER) != DSU_REQUEST_COMPLETE) {
		        printf("\n Couldn't discover dds line rate");
			return (DSU_INTERMEDIATE_REG_RW_FAILURE);
		}
		if (actual_dds_linerate != SW56_LINE_RATE_64K) {
			printf("\n%s", data_coding_errmsg);
			return (DSU_WRONG_FORMAT);
		}
		break;
	    default:
		printf("\n%s", data_coding_errmsg);
		return (DSU_WRONG_FORMAT);	   
	}
    }
    else 
	return (DSU_WRONG_FORMAT);

   if ((result = csu_dsu_register_RW(cd_info, DSU_DATA_TRANSMIT_OPTIONS_REG,
                                     DSU_DATA_TRANSMIT_OPTIONS_LEN,
                                     &dsu_data_coding,
                                     WRITE_REGISTER)) == DSU_REQUEST_COMPLETE) 
        cd_info->dsu_data_scrambler_mode = data_coding_enable;

   return (result);
}
		    	 
/*
 * sw56_set_RDL_enable - enables/prevents servicing of remote loopback requests
 */
int sw56_set_RDL_enable (csu_dsu_instance_t *cd_info, boolean RDL_enable)
{
    uchar dsu_RDL_enable;
    boolean result;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    dsu_RDL_enable = RDL_enable ? SW56_ENABLE_REM_LOOPBACK :
				  SW56_DISABLE_REM_LOOPBACK;

    if ((result = csu_dsu_register_RW(cd_info, SW56_REM_LOOP_DISABLE_REG,
                                      SW56_REM_LOOP_DISABLE_LEN,
                                      &dsu_RDL_enable,
                                      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE) 
        cd_info->sw56_remote_loopback_enable = RDL_enable;

   return (result);
}

/*
 * sw56_get_rem_lpback_state
 */
static int sw56_get_rem_lpback_state (csu_dsu_instance_t *cd_info)
{
    uchar current_loopback;
    uchar test_status;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

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
	    break;
	default:
	    return (NO_REMOTE_TEST);
    }

    if (csu_dsu_register_RW(cd_info, DSU_TEST_STATUS_REG,
                            DSU_TEST_STATUS_LEN, &test_status,
                            READ_REGISTER) != DSU_REQUEST_COMPLETE)
        return (DSU_TEST_STATUS_UNREADABLE);

    if (test_status & SW56_ATTEMPTING_REMOTE_LOOP)
	return (ATTEMPTING_REMOTE_TEST);
    else if (test_status & SW56_PASSED_REMOTE_LOOP)
	return (PASSED_REMOTE_TEST);
    else if ((test_status & SW56_SENDING_TEST_PATTERN) ||
	     (test_status & SW56_SYNCED_ON_TEST_PATTERN))
	return (PASSED_REMOTE_TEST);
    else 
	return (FAILED_REMOTE_TEST);

    return (test_status);
}
     
/*
 * sw56_set_test_pattern - sets up the CSU/DSU for a user selected test pattern to use
 *                       for a remote loopback test
 */
static int sw56_set_test_pattern (csu_dsu_instance_t *cd_info, int test_pattern,
                                  ulong user_pattern)
{
    int result;
    uchar dsu_test_pattern;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    switch (test_pattern) {
	case CSU_DSU_LPBACK_PAT_NOSEL:
	    return (DSU_REQUEST_COMPLETE);
	    break;
	case CSU_DSU_LPBACK_PAT_2047:
	    dsu_test_pattern = SW56_2047;
	    break;
	case CSU_DSU_LPBACK_PAT_511:
	    dsu_test_pattern = SW56_511;
	    break;
	case CSU_DSU_LPBACK_PAT_STR_PAT:
	    switch (user_pattern) {
		case 1:
		    dsu_test_pattern = SW56_STRESS_PATTERN_1;
		    break;
		case 2:
		    dsu_test_pattern = SW56_STRESS_PATTERN_2;
		    break;
		case 3:
		    dsu_test_pattern = SW56_STRESS_PATTERN_3;
		    break;
		case 4:
		    dsu_test_pattern = SW56_STRESS_PATTERN_4;
		    break;
		default:
		    printf("\nSelected test pattern not supported");
		    return (DSU_WRONG_FORMAT);
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
 * sw56_remote_loopback_command - sets parameters for remote loopback
 *				  and awakens background process
 */
static int sw56_remote_loopback_command (csu_dsu_instance_t *cd_info,
                                         int loopback_command, int loopback_type,
                                         int test_pattern, ulong user_pattern)
{
    uchar dsu_loopback_command;
    uchar loopdown_test_status = 0;
    int result;
    ulong bit_errors;

    if (!csu_dsu_chk_instance(cd_info))
        return (DSU_BAD_INSTANCE);

    if (loopback_command == CSU_DSU_LPBACK_NONE) {
        dsu_loopback_command = DSU_LOOPBACK_NONE;
    }
    else if (loopback_command == CSU_DSU_LOOPBACK_REMOTE) {

	if (test_pattern == CSU_DSU_LPBACK_PAT_NOSEL)
	    dsu_loopback_command = DSU_REMOTE_LOOPUP;
	else {
	    dsu_loopback_command = DSU_REMOTE_LOOPUP_SEND_PATTERN;
	    if (sw56_set_test_pattern(cd_info, test_pattern, 
				      user_pattern) != DSU_REQUEST_COMPLETE)
		return (DSU_WRONG_FORMAT);
	}
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

    if (loopback_command == CSU_DSU_LPBACK_NONE) {
        /*
         * Bit errors registers clears as soon as loopdown is selected -
         * cache the value before selecting the test
         */
        if (csu_dsu_register_RW(cd_info, DSU_BERT_REG,
                                DSU_BERT_LEN, &bit_errors,
                                READ_REGISTER) != DSU_REQUEST_COMPLETE)
            bit_errors = 0;

        /*
         * Cache the test status at this point - used when looping down
         * to figure out whether CSU/DSU was in sync and hence if bit error
         * count is valid
         */
        csu_dsu_register_RW(cd_info, DSU_TEST_STATUS_REG,
                            DSU_TEST_STATUS_LEN, &loopdown_test_status,
                            READ_REGISTER);
    }

    /* Now actually start the remote loopback test */
    if ((result = csu_dsu_register_RW(cd_info, DSU_TEST_TYPE_SELECT_REG,
                                      DSU_TEST_TYPE_SELECT_LEN,
                                      &dsu_loopback_command,
                                      WRITE_REGISTER)) != DSU_REQUEST_COMPLETE)  {
        printf("\nLoopback test failed");
        return (result);
    }

    if (loopback_command == CSU_DSU_LOOPBACK_REMOTE) {
        cd_info->current_loopback_command = CSU_DSU_LOOPBACK_REMOTE;
        cd_info->current_loopback_state =   DSU_ATTEMPTING_LOOPUP;
        cd_info->dsu_loopback_type = loopback_type;
        cd_info->dsu_loopback_test_pattern = test_pattern;
        cd_info->dsu_loopback_user_pattern = user_pattern;
	
        /*
         * Now start the diagnostic timer so background process can
         * check on loopback status
         */
        mgd_timer_start(&cd_info->dsu_diagnostic_timer, 5*ONESEC);

    }
    else if (loopback_command == CSU_DSU_LPBACK_NONE) {
	/*
	 * For the SW56 modules, there is no way of telling if
  	 * loopdown was successful (unlike the FT1) so just assume
	 * it will happen
	 */
	cd_info->dsu_loopback_bit_errors = bit_errors;
	csu_dsu_calculate_bert_test(cd_info);
	cd_info->dsu_loopback_synced_on_pattern =
			(loopdown_test_status & SW56_SYNCED_ON_TEST_PATTERN) ?
			TRUE : FALSE;
	csu_dsu_loopback_finished(cd_info);
	cd_info->dsu_last_remote_loopback_passed = TRUE;
	cd_info->current_loopback_command = CSU_DSU_LPBACK_NONE;
	cd_info->current_loopback_state = NO_REMOTE_TEST;
    }
    
    return (DSU_REQUEST_COMPLETE);

}

/*
 * Initializes driver data structure parameters to defaults
 */
void sw56_set_default_values (csu_dsu_instance_t *cd_info)
{

    if (!cd_info)
        return;

    if (cd_info->module_type == CSU_DSU_SW56_4_WIRE) {
        cd_info->sw56_network_type =            CSU_DSU_SW56_4W_NWKTYPE_DEFAULT;
        cd_info->sw56_switched_type =           CSU_DSU_SW56_4W_CARRIER_DEFAULT;
        cd_info->sw56_dds_linerate =            CSU_DSU_CLK_RATE_56;
	cd_info->dsu_alarm_mask =		SW56_4W_ALARM_MASK_DDS;
        cd_info->module_int_protocol_rev =      ADTRAN_SW56_4W_HIGHEST_REV;
        cd_info->mod_highest_int_protocol_rev = ADTRAN_SW56_4W_HIGHEST_REV;
    }
    else if (cd_info->module_type == CSU_DSU_SW56_2_WIRE) {
        cd_info->sw56_network_type =            CSU_DSU_SW56_2W_NWKTYPE_DEFAULT;
        cd_info->sw56_switched_type =           CSU_DSU_SW56_2W_CARRIER_DEFAULT;
	cd_info->sw56_dds_linerate =            CSU_DSU_CLK_RATE_56;
	cd_info->dsu_alarm_mask = 		SW56_2W_ALARM_MASK_SWITCHED;
        cd_info->module_int_protocol_rev =      ADTRAN_SW56_2W_HIGHEST_REV;
        cd_info->mod_highest_int_protocol_rev = ADTRAN_SW56_2W_HIGHEST_REV;
    } else
        return;

    /* Common user configuration parameters */
    cd_info->dsu_clocksource =                  CSU_DSU_CLK_SRC_DEFAULT;
    cd_info->dsu_data_scrambler_mode =          CSU_DSU_DCODE_DEFAULT;
    cd_info->sw56_auto_answer =                 SW56_AUTO_ANSWER_MODE_DEF;
    cd_info->sw56_remote_loopback_enable =      CSU_DSU_RDL_MODE_DEFAULT;
    cd_info->dsu_network_alarm_status = 	DSU_NO_ALARMS;
    cd_info->usr_cfg_sw56_remote_loopback_mode= CSU_DSU_RMT_LPBACK_MODE_DEF;
    cd_info->dsu_get_alarm_status =		sw56_alarm_processor;
    cd_info->dsu_alarm_messages =		sw56_alarm_messages;
    cd_info->dsu_remote_tests =                 sw56_chk_rem_activated_lpbacks; 
    cd_info->dsu_check_connection_status =      sw56_chk_connection_active;
    cd_info->dsu_remote_loopback_command =      sw56_remote_loopback_command;
    cd_info->dsu_remote_lpback_state = 		sw56_get_rem_lpback_state;
    cd_info->dsu_current_line_bandwidth =	sw56_get_current_line_speed;
    cd_info->dsu_clear_alarm_data = 		NULL;
    cd_info->dsu_set_current_config =           sw56_set_dsu_current_config;
    cd_info->dsu_show_network_status =          sw56_show_network_status;
    cd_info->dsu_show_module_config = 		sw56_show_module_configuration;
    cd_info->dsu_show_alarm_counters = 		sw56_show_alarm_counters;
    cd_info->dsu_show_full_performance_stats =	NULL;

    /*
     * Setup register access timing values
     */
    cd_info->dsu_interread_wait_time = timer_calibrate(SW56_INTERBYTE_READ_WAIT,
						       FALSE);
    cd_info->dsu_interwrite_wait_time = timer_calibrate(SW56_INTERBYTE_WRITE_WAIT,
							FALSE);
    cd_info->dsu_specify_reg_wait_time = NULL;
}
