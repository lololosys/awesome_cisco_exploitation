/* $Id: cfg_rttmon.h,v 1.1.4.4 1996/06/07 22:25:42 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/cfg_rttmon.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor parse chain configuration header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the definitions for
 * the RTT Monitor configuration parse chain.
 *------------------------------------------------------------------
 * $Log: cfg_rttmon.h,v $
 * Revision 1.1.4.4  1996/06/07  22:25:42  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/06/04  18:28:13  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.2  1996/05/22  17:51:31  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * Occurances -> Occurrences
 * Occurences -> Occurrences
 * hundreths  -> hundredths
 * "unknown -> unkown" -> unknown
 * Varify -> Verify
 * Currrent -> Current
 * Occured -> Occurred
 * Sequece -> Sequence
 * millliseconds -> milliseconds
 * unkown -> unknown
 * soure -> source
 * Unkown -> Unknown
 *
 * Revision 1.1.4.1  1996/05/17  11:40:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:04  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:11  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:25  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:18  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(config_rttmon_extend_here, NONE);

/******************************************************************
 * rtr reset
 * [ no ] rtr schedule <entry-number> [ life <seconds> ]
 *         [ start-time { pending | now |  
 *                        hh:mm [{Mon dd | dd Mon }] } ]
 *         [ ageout <seconds> ]
 * [ no ] rtr reaction-configuration <entry-number>
 *         [ connection-loss-enable ] 
 *         [ timeout-enable ]
 *         [ threshold-falling <falling-value> ]
 *         [ threshold-type { never | immediate | 
 *                            consecutive 
 *                             [ <consecutive-occurances> ] | 
 *                            xOfy [ <x-value> <y-value> ] | 
 *                            average [<n-attempts>] }
 *         [ action-type { none | trapOnly | nmvtOnly | 
 *                         triggerOnly | trapAndNmvt | 
 *                         trapAndTrigger | nmvtAndTrigger | 
 *                         trapNmvtAndTrigger }
 * [ no ] rtr reaction-trigger <entry-number> <target-entry-number>
 * [ no ] rtr <entry-number>
 */

/******************************************************************
 * rtr reset
 */
EOLS(config_rtr_reset_eol,
     rttmon_config_base_command, RTR_CFG_RESET_CMD);
KEYWORD(config_rtr_reset,
	config_rtr_reset_eol,
	config_rttmon_extend_here,
	D_rttMonApplReset_reset_string,
	"RTR Reset", PRIV_CONF);

/******************************************************************
 * [ no ] rtr schedule <entry-number> [ life <seconds> ]
 *         [ start-time { pending | now | 
 *                        hh:mm [{Mon dd | dd Mon }] } ]
 *         [ ageout <seconds> ]
 *
 *
 *  OBJ(int,1)   = <entry-number>
 *  OBJ(int,2)   = <life-seconds>
 *  OBJ(int,3)   = 0 = no life seconds
 *               = 1 = life seconds valid
 *  OBJ(int,4)   = 0 = no start-time given
 *                 1 = now
 *                 2 = date-time
 *                 3 = pending
 *  OBJ(int,5)   = hours
 *  OBJ(int,6)   = min
 *  OBJ(int,7)   = month or 0 if date not specified
 *  OBJ(int,8)   = day
 *  OBJ(int,9)   = <ageout-seconds>
 *  OBJ(int,10)  = 0 = no ageout seconds
 *               = 1 = ageout seconds valid 
 */

pdecl(config_rtr_schedule_loop);

EOLS(config_rtr_schedule_eol,
     rttmon_config_base_command, RTR_CFG_SCHEDULE_CMD);

SET(config_rtr_schedule_valid_ageout_seconds,
    config_rtr_schedule_loop,
    OBJ(int, 10), 1);

SET(config_rtr_schedule_no_ageout_seconds,
    NONE,
    OBJ(int, 10), 0);

NUMBER(config_rtr_schedule_ageout_seconds,
       config_rtr_schedule_valid_ageout_seconds,
       config_rtr_schedule_no_ageout_seconds, OBJ(int, 9),
       MIN_rttMonScheduleAdminConceptRowAgeout,
       MAX_rttMonScheduleAdminConceptRowAgeout, "Ageout Seconds");

KEYWORD(config_rtr_schedule_ageout,
	config_rtr_schedule_ageout_seconds,
	config_rtr_schedule_no_ageout_seconds,
	D_rttMonScheduleAdminConceptRowAgeout_string,
	"How long to keep this Entry when inactive",
	PRIV_CONF);

EQUAL(config_rtr_schedule_ageout_assert,
      config_rtr_schedule_ageout,
      config_rtr_schedule_eol,
      OBJ(int, 10), 0);

SET(config_rtr_schedule_start_time_pending_flag_valid,
    config_rtr_schedule_loop,
    OBJ(int, 4), 3);

SET(config_rtr_schedule_start_time_not_valid,
    NONE,
    OBJ(int, 4), 0);

KEYWORD(config_rtr_schedule_start_time_pending,
	config_rtr_schedule_start_time_pending_flag_valid,
	config_rtr_schedule_start_time_not_valid,
	D_rttMonScheduleAdminRttStartTime_pending_string,
	"Start Pending", PRIV_CONF);

SET(config_rtr_schedule_start_time_now_flag_valid,
    config_rtr_schedule_loop,
    OBJ(int, 4), 1);

KEYWORD(config_rtr_schedule_start_time_now,
	config_rtr_schedule_start_time_now_flag_valid,
	config_rtr_schedule_start_time_pending,
	D_rttMonScheduleAdminRttStartTime_now_string,
	"Start Now", PRIV_CONF);

SET(config_rtr_schedule_start_time_date_time_valid,
    config_rtr_schedule_loop,
    OBJ(int, 4), 2);

/******************************************************************
 * End     hh:mm [{Mon dd | dd Mon }] } ]
 */
NUMBER(config_rtr_schedule_start_time_date_day_2,
       config_rtr_schedule_start_time_date_time_valid, NONE,
       OBJ(int, 8), 1, 31,
       "Day of the month");

SPLIT(config_rtr_schedule_start_time_date_test,
      config_rtr_schedule_start_time_date_day_2,
      config_rtr_schedule_start_time_date_time_valid,
      OBJ(int, 8), 0);

SET(config_rtr_schedule_start_time_no_date,
    config_rtr_schedule_start_time_date_time_valid,
    OBJ(int, 7), 0);

MONTH(config_rtr_schedule_start_time_date_month,
      config_rtr_schedule_start_time_date_test,
      config_rtr_schedule_start_time_no_date,
      OBJ(int, 7),
      "Month of the year");

NUMBER(config_rtr_schedule_start_time_date_day,
       config_rtr_schedule_start_time_date_month,
       config_rtr_schedule_start_time_date_month,
       OBJ(int, 8), 1, 31,
       "Day of the month");

TIME(config_rtr_schedule_start_time_date_time,
     config_rtr_schedule_start_time_date_day,
     config_rtr_schedule_start_time_now,
     OBJ(int, 5), OBJ(int, 6), "Start Time (hh:mm)");

/******************************************************************
 * Start   hh:mm [{Mon dd | dd Mon }] } ]
 */

KEYWORD(config_rtr_schedule_start_time,
	config_rtr_schedule_start_time_date_time,
	config_rtr_schedule_start_time_not_valid,
	D_rttMonScheduleAdminRttStartTime_string,
	"When to start this entry",
	PRIV_CONF);

EQUAL(config_rtr_schedule_start_time_assert,
      config_rtr_schedule_start_time,
      config_rtr_schedule_ageout_assert,
      OBJ(int, 4), 0);

SET(config_rtr_schedule_valid_life_seconds,
    config_rtr_schedule_loop,
    OBJ(int, 3), 1);

SET(config_rtr_schedule_no_life_seconds,
    NONE,
    OBJ(int, 3), 0);

NUMBER(config_rtr_schedule_life_seconds,
       config_rtr_schedule_valid_life_seconds,
       config_rtr_schedule_no_life_seconds, OBJ(int, 2),
       MIN_rttMonScheduleAdminRttLife,
       MAX_rttMonScheduleAdminRttLife, "Life Seconds");

KEYWORD(config_rtr_schedule_life,
	config_rtr_schedule_life_seconds,
	config_rtr_schedule_no_life_seconds,
	D_rttMonScheduleAdminRttLife_string,
	"Length of time to execute in seconds",
	PRIV_CONF);

EQUAL(config_rtr_schedule_life_assert,
      config_rtr_schedule_life,
      config_rtr_schedule_start_time_assert,
      OBJ(int, 3), 0);

NOP(config_rtr_schedule_loop,
    config_rtr_schedule_life_assert,
    NONE);

NOPREFIX(config_rtr_schedule_no,
	 config_rtr_schedule_loop,
	 config_rtr_schedule_eol);

NUMBER(config_rtr_schedule_entry,
       config_rtr_schedule_no,
       NONE, OBJ(int, 1),
       MIN_rttMonApplNumCtrlAdminEntry,
       MAX_rttMonApplNumCtrlAdminEntry, "Entry Number");

NVGENS(config_rtr_schedule_nvgen,
       config_rtr_schedule_entry,
       rttmon_config_base_command, RTR_CFG_SCHEDULE_CMD);

KEYWORD(config_rtr_schedule,
	config_rtr_schedule_nvgen,
	config_rtr_reset,
	D_rttMonSchedule_string,
	"RTR Entry Scheduling", PRIV_CONF);

/******************************************************************
 * [ no ] rtr reaction-configuration <entry-number>
 *         [ connection-loss-enable ] 
 *         [ timeout-enable ] 
 *         [ threshold-falling <falling-value> ]
 *         [ threshold-type { never | immediate | 
 *                            consecutive 
 *                             [ <consecutive-occurances> ] | 
 *                            xOfy [ <x-value> <y-value> ] | 
 *                            average [<n-attempts>] }
 *         [ action-type { none | trapOnly | nmvtOnly | 
 *                         triggerOnly | trapAndNmvt | 
 *                         trapAndTrigger | nmvtAndTrigger | 
 *                         trapNmvtAndTrigger }
 *
 *
 *  OBJ(int,1) = <entry-number>
 *  OBJ(int,2) = 1 = connection-loss-enable exists
 *               0 = connection-loss-enable does not exists 
 *  OBJ(int,3) = #defined value for
 *                = no connection-loss-enable
 *                = connection-loss-enable
 *  OBJ(int,4) = 1 = timeout-enable exists
 *               0 = timeout-enable does not exists
 *  OBJ(int,5) = #defined value for
 *                = no timeout-enable
 *                = timeout-enable
 *  OBJ(int,6) = 1 = threshold-falling exists
 *               0 = threshold-falling does not exist
 *  OBJ(int,7) = falling-value
 *  OBJ(int,8) = 1 = threshold-type exists
 *               0 = threshold-type does not exist
 *  OBJ(int,9) = #defined value for
 *                = never
 *                = immediate
 *                = consecutive
 *                = xOfy
 *                = average
 *  OBJ(int,10) = 1 = consecutive-occurences |
 *                   x-value | n-attempts exists
 *                0 = does not exist
 *  OBJ(int,11) = consecutive-occurences |
 *                 x-value | n-attempts
 *  OBJ(int,12) = 1 = y-value exists
 *                0 = does not exists
 *  OBJ(int,13) = y-value
 *  OBJ(int,14) = 1 = action-type exists
 *                0 = action-type does not exists
 *  OBJ(int,15) = #defined value for 
 *                = none
 *                = trapOnly
 *                = nmvtOnly
 *                = triggerOnly
 *                = trapAndNmvt
 *                = trapAndTrigger
 *                = nmvtAndTrigger
 *                = trapNmvtAndTrigger
 *                 
 *
 */

pdecl(config_rtr_reaction_configuration_loop);

EOLS(config_rtr_reaction_configuration_eol,
     rttmon_config_base_command, RTR_CFG_REACT_CONFIG_CMD);
/*
 *  End    [ action-type { none | trapOnly | nmvtOnly | 
 *                         triggerOnly | trapAndNmvt | 
 *                         trapAndTrigger | nmvtAndTrigger | 
 *                         trapNmvtAndTrigger }
 */
SET(config_rtr_reaction_action_exists,
    config_rtr_reaction_configuration_loop,
    OBJ(int, 14), 1);

SET(config_rtr_reaction_action_doesnt_exist,
    NONE,
    OBJ(int, 14), 0);

SET(config_rtr_reaction_action_trapNmvtAndTrigger_flag,
    config_rtr_reaction_action_exists,
    OBJ(int, 15),
    D_rttMonReactAdminActionType_trapNmvtAndTrigger);

KEYWORD(config_rtr_reaction_action_trapNmvtAndTrigger,
	config_rtr_reaction_action_trapNmvtAndTrigger_flag,
	NONE,
	D_rttMonReactAdminActionType_trapNmvtAndTrigger_string,
	"Trap, SNA Nmvt and Trigger Action",
	PRIV_CONF);

SET(config_rtr_reaction_action_nmvtAndTrigger_flag,
    config_rtr_reaction_action_exists,
    OBJ(int, 15),
    D_rttMonReactAdminActionType_nmvtAndTrigger);

KEYWORD(config_rtr_reaction_action_nmvtAndTrigger,
	config_rtr_reaction_action_nmvtAndTrigger_flag,
	config_rtr_reaction_action_trapNmvtAndTrigger,
	D_rttMonReactAdminActionType_nmvtAndTrigger_string,
	"SNA Nmvt and Trigger Action",
	PRIV_CONF);

SET(config_rtr_reaction_action_trapAndTrigger_flag,
    config_rtr_reaction_action_exists,
    OBJ(int, 15),
    D_rttMonReactAdminActionType_trapAndTrigger);

KEYWORD(config_rtr_reaction_action_trapAndTrigger,
	config_rtr_reaction_action_trapAndTrigger_flag,
	config_rtr_reaction_action_nmvtAndTrigger,
	D_rttMonReactAdminActionType_trapAndTrigger_string,
	"Trap and Trigger Action", PRIV_CONF);

SET(config_rtr_reaction_action_trapAndNmvt_flag,
    config_rtr_reaction_action_exists,
    OBJ(int, 15),
    D_rttMonReactAdminActionType_trapAndNmvt);

KEYWORD(config_rtr_reaction_action_trapAndNmvt,
	config_rtr_reaction_action_trapAndNmvt_flag,
	config_rtr_reaction_action_trapAndTrigger,
	D_rttMonReactAdminActionType_trapAndNmvt_string,
	"Trap and SNA Nmvt Action", PRIV_CONF);

SET(config_rtr_reaction_action_TriggerOnly_flag,
    config_rtr_reaction_action_exists,
    OBJ(int, 15),
    D_rttMonReactAdminActionType_triggerOnly);

KEYWORD(config_rtr_reaction_action_triggerOnly,
	config_rtr_reaction_action_TriggerOnly_flag,
	config_rtr_reaction_action_trapAndNmvt,
	D_rttMonReactAdminActionType_triggerOnly_string,
	"Trigger Only Action", PRIV_CONF);

SET(config_rtr_reaction_action_nmvtOnly_flag,
    config_rtr_reaction_action_exists,
    OBJ(int, 15),
    D_rttMonReactAdminActionType_nmvtOnly);

KEYWORD(config_rtr_reaction_action_nmvtOnly,
	config_rtr_reaction_action_nmvtOnly_flag,
	config_rtr_reaction_action_triggerOnly,
	D_rttMonReactAdminActionType_nmvtOnly_string,
	"SNA Nmvt Only Action", PRIV_CONF);

SET(config_rtr_reaction_action_trapOnly_flag,
    config_rtr_reaction_action_exists,
    OBJ(int, 15),
    D_rttMonReactAdminActionType_trapOnly);

KEYWORD(config_rtr_reaction_action_trapOnly,
	config_rtr_reaction_action_trapOnly_flag,
	config_rtr_reaction_action_nmvtOnly,
	D_rttMonReactAdminActionType_trapOnly_string,
	"Trap Only Action", PRIV_CONF);

SET(config_rtr_reaction_action_none_flag,
    config_rtr_reaction_action_exists,
    OBJ(int, 15),
    D_rttMonReactAdminActionType_none);

KEYWORD(config_rtr_reaction_action_none,
	config_rtr_reaction_action_none_flag,
	config_rtr_reaction_action_trapOnly,
	D_rttMonReactAdminActionType_none_string,
	"Take No Action", PRIV_CONF);

KEYWORD(config_rtr_reaction_action,
	config_rtr_reaction_action_none,
	config_rtr_reaction_action_doesnt_exist,
	D_rttMonReactAdminActionType_string,
	"RTR Reaction Action Type", PRIV_CONF);

EQUAL(config_rtr_reaction_action_assert,
      config_rtr_reaction_action,
      config_rtr_reaction_configuration_eol,
      OBJ(int, 14), 0);

/*
 * Start   [ action-type { none | trapOnly | nmvtOnly | 
 *                         triggerOnly | trapAndNmvt | 
 *                         trapAndTrigger | nmvtAndTrigger | 
 *                         trapNmvtAndTrigger }
 */
/*
 *  End    [ threshold-type { never | immediate | 
 *                            consecutive 
 *                             [ <consecutive-occurances> ] | 
 *                            xOfy [ <x-value> <y-value> ] | 
 *                            average [<n-attempts>] }
 */
SET(config_rtr_reaction_threshold_type_exists,
    config_rtr_reaction_configuration_loop,
    OBJ(int, 8), 1);

SET(config_rtr_reaction_threshold_type_doesnt_exist,
    NONE,
    OBJ(int, 8), 0);

SET(config_rtr_reaction_threshold_average_flag,
    config_rtr_reaction_threshold_type_exists,
    OBJ(int, 9),
    D_rttMonReactAdminThresholdType_average);

SET(config_rtr_reaction_threshold_average_value_exists,
    config_rtr_reaction_threshold_average_flag,
    OBJ(int, 10), 1);

SET(config_rtr_reaction_threshold_average_value_doesnt_exists,
    config_rtr_reaction_threshold_average_flag,
    OBJ(int, 10), 0);

NUMBER(config_rtr_reaction_threshold_average_N_attempts,
       config_rtr_reaction_threshold_average_value_exists,
       config_rtr_reaction_threshold_average_value_doesnt_exists,
       OBJ(int, 11), MIN_rttMonReactAdminThresholdCount,
       MAX_rttMonReactAdminThresholdCount,
       "N Value");

KEYWORD(config_rtr_reaction_threshold_average,
	config_rtr_reaction_threshold_average_N_attempts,
	NONE,
	D_rttMonReactAdminThresholdType_average_string,
	"Average over N Attempts", PRIV_CONF);

SET(config_rtr_reaction_threshold_xOfy_flag,
    config_rtr_reaction_threshold_type_exists,
    OBJ(int, 9),
    D_rttMonReactAdminThresholdType_xOfy);

SET(config_rtr_reaction_threshold_xOfy2_value_exists,
    config_rtr_reaction_threshold_xOfy_flag,
    OBJ(int, 12), 1);

SET(config_rtr_reaction_threshold_xOfy2_value_doesnt_exists,
    config_rtr_reaction_threshold_xOfy_flag,
    OBJ(int, 12), 0);

SET(config_rtr_reaction_threshold_xOfy_value_exists,
    config_rtr_reaction_threshold_xOfy2_value_exists,
    OBJ(int, 10), 1);

SET(config_rtr_reaction_threshold_xOfy_value_doesnt_exists,
    config_rtr_reaction_threshold_xOfy2_value_doesnt_exists,
    OBJ(int, 10), 0);

NUMBER(config_rtr_reaction_threshold_xOfy_y_value,
       config_rtr_reaction_threshold_xOfy_value_exists,
       NONE,
       OBJ(int, 13), MIN_rttMonReactAdminThresholdCount2,
       MAX_rttMonReactAdminThresholdCount2,
       "Y Value");

NUMBER(config_rtr_reaction_threshold_xOfy_x_value,
       config_rtr_reaction_threshold_xOfy_y_value,
       config_rtr_reaction_threshold_xOfy_value_doesnt_exists,
       OBJ(int, 11), MIN_rttMonReactAdminThresholdCount,
       MAX_rttMonReactAdminThresholdCount,
       "X Value");

KEYWORD(config_rtr_reaction_threshold_xOfy,
	config_rtr_reaction_threshold_xOfy_x_value,
	config_rtr_reaction_threshold_average,
	D_rttMonReactAdminThresholdType_xOfy_string,
	"X out of Y Occurrences",
	PRIV_CONF);

SET(config_rtr_reaction_threshold_consecutive_flag,
    config_rtr_reaction_threshold_type_exists,
    OBJ(int, 9),
    D_rttMonReactAdminThresholdType_consecutive);

SET(config_rtr_reaction_threshold_consecutive_value_exists,
    config_rtr_reaction_threshold_consecutive_flag,
    OBJ(int, 10), 1);

SET(config_rtr_reaction_threshold_consecutive_value_doesnt_exists,
    config_rtr_reaction_threshold_consecutive_flag,
    OBJ(int, 10), 0);

NUMBER(config_rtr_reaction_threshold_consecutive_occurances,
       config_rtr_reaction_threshold_consecutive_value_exists,
       config_rtr_reaction_threshold_consecutive_value_doesnt_exists,
       OBJ(int, 11), MIN_rttMonReactAdminThresholdCount,
       MAX_rttMonReactAdminThresholdCount,
       "Number of Occurrences");

KEYWORD(config_rtr_reaction_threshold_consecutive,
	config_rtr_reaction_threshold_consecutive_occurances,
	config_rtr_reaction_threshold_xOfy,
	D_rttMonReactAdminThresholdType_consecutive_string,
	"Consecutive Occurrences",
	PRIV_CONF);

SET(config_rtr_reaction_threshold_immediate_flag,
    config_rtr_reaction_threshold_type_exists,
    OBJ(int, 9),
    D_rttMonReactAdminThresholdType_immediate);

KEYWORD(config_rtr_reaction_threshold_immediate,
	config_rtr_reaction_threshold_immediate_flag,
	config_rtr_reaction_threshold_consecutive,
	D_rttMonReactAdminThresholdType_immediate_string,
	"React Immediately", PRIV_CONF);

SET(config_rtr_reaction_threshold_never_flag,
    config_rtr_reaction_threshold_type_exists,
    OBJ(int, 9),
    D_rttMonReactAdminThresholdType_never);

KEYWORD(config_rtr_reaction_threshold_never,
	config_rtr_reaction_threshold_never_flag,
	config_rtr_reaction_threshold_immediate,
	D_rttMonReactAdminThresholdType_never_string,
	"Never React", PRIV_CONF);

KEYWORD(config_rtr_reaction_threshold_type,
	config_rtr_reaction_threshold_never,
	config_rtr_reaction_threshold_type_doesnt_exist,
	D_rttMonReactAdminThresholdType_string,
	"RTR Reaction Threshold Type", PRIV_CONF);

EQUAL(config_rtr_reaction_threshold_type_assert,
      config_rtr_reaction_threshold_type,
      config_rtr_reaction_action_assert,
      OBJ(int, 8), 0);

/*
 *  Start  [ threshold-type { never | immediate | 
 *                            consecutive 
 *                             [ <consecutive-occurances> ] | 
 *                            xOfy [ <x-value> <y-value> ] | 
 *                            average [<n-attempts>] }
 */
/*
 *  End    [ threshold-falling <falling-value> ]
 */
SET(config_rtr_reaction_threshold_falling_given,
    config_rtr_reaction_configuration_loop,
    OBJ(int, 6), 1);

SET(config_rtr_reaction_threshold_falling_skipped,
    NONE,
    OBJ(int, 6), 0);

NUMBER(config_rtr_reaction_threshold_falling_value,
       config_rtr_reaction_threshold_falling_given,
       config_rtr_reaction_threshold_falling_skipped,
       OBJ(int, 7), MIN_rttMonReactAdminThresholdFalling,
       MAX_rttMonReactAdminThresholdFalling,
       "Falling Value in Milliseconds");

KEYWORD(config_rtr_reaction_threshold_falling,
	config_rtr_reaction_threshold_falling_value,
	config_rtr_reaction_threshold_falling_skipped,
	D_rttMonReactAdminThresholdFalling_string,
	"RTR Falling Threshold Value", PRIV_CONF);

EQUAL(config_rtr_reaction_threshold_falling_assert,
      config_rtr_reaction_threshold_falling,
      config_rtr_reaction_threshold_type_assert,
      OBJ(int, 6), 0);

/*
 *  Start  [ threshold-falling <falling-value> ]
 */
/*
 *  End    [ timeout-enable ] 
 */
SET(config_rtr_reaction_timeout_enable_given,
    config_rtr_reaction_configuration_loop,
    OBJ(int, 4), 1);

SET(config_rtr_reaction_timeout_enable_skipped,
    NONE,
    OBJ(int, 4), 0);

KEYWORD_ID(config_rtr_reaction_timeout_enable,
	   config_rtr_reaction_timeout_enable_given,
	   config_rtr_reaction_timeout_enable_skipped,
	   OBJ(int, 5), TRUE,
	   D_rttMonReactAdminTimeoutEnable_string,
	   "RTR Enable Timeout Reaction", PRIV_CONF);

EQUAL(config_rtr_reaction_timeout_assert,
      config_rtr_reaction_timeout_enable,
      config_rtr_reaction_threshold_falling_assert,
      OBJ(int, 5), FALSE);

/*
 *  Start  [ timeout-enable ] 
 */
/*
 *  End    [ connection-loss-enable ] 
 */
SET(config_rtr_reaction_conn_loss_given,
    config_rtr_reaction_configuration_loop,
    OBJ(int, 2), 1);

SET(config_rtr_reaction_conn_loss_skipped,
    NONE,
    OBJ(int, 2), 0);

KEYWORD_ID(config_rtr_reaction_conn_loss_enable,
	   config_rtr_reaction_conn_loss_given,
	   config_rtr_reaction_conn_loss_skipped,
	   OBJ(int, 3), TRUE,
	   D_rttMonReactAdminConnectionEnable_string,
	   "RTR Enable Connection Loss Reaction", PRIV_CONF);

EQUAL(config_rtr_reaction_conn_loss_enable_assert,
      config_rtr_reaction_conn_loss_enable,
      config_rtr_reaction_timeout_assert,
      OBJ(int, 3), FALSE);

/*
 *  Start  [ connection-loss-enable ] 
 */
NOP(config_rtr_reaction_configuration_loop,
    config_rtr_reaction_conn_loss_enable_assert,
    NONE);

NOPREFIX(config_rtr_reaction_config_no,
	 config_rtr_reaction_configuration_loop,
	 config_rtr_reaction_configuration_eol);

NUMBER(config_rtr_reaction_config_entry,
       config_rtr_reaction_config_no,
       NONE, OBJ(int, 1),
       MIN_rttMonApplNumCtrlAdminEntry,
       MAX_rttMonApplNumCtrlAdminEntry, "Entry Number");

NVGENS(config_rtr_reaction_config_nvgen,
       config_rtr_reaction_config_entry,
       rttmon_config_base_command, RTR_CFG_REACT_CONFIG_CMD);

KEYWORD(config_rtr_reaction_configuration,
	config_rtr_reaction_config_nvgen,
	config_rtr_schedule,
	D_rttMonReactAdmin_string,
	"RTR Reaction Configuration", PRIV_CONF);


/******************************************************************
 * [ no ] rtr reaction-trigger <entry-number> <target-entry-number>
 *
 *  OBJ(int,1) = <entry-number>
 *  OBJ(int,2) = <target-entry-number>
 */
EOLS(config_rtr_reaction_trigger_eol,
     rttmon_config_base_command, RTR_CFG_REACT_TRIG_CMD);

NUMBER(config_rtr_reaction_trigger_target_entry,
       config_rtr_reaction_trigger_eol,
       NONE, OBJ(int, 2), MIN_rttMonApplNumCtrlAdminEntry,
       MAX_rttMonApplNumCtrlAdminEntry,
       "Target Entry Number");

NUMBER(config_rtr_reaction_trigger_entry,
       config_rtr_reaction_trigger_target_entry,
       NONE, OBJ(int, 1), MIN_rttMonApplNumCtrlAdminEntry,
       MAX_rttMonApplNumCtrlAdminEntry, "Entry Number");

NVGENS(config_rtr_reaction_trigger_nvgen,
       config_rtr_reaction_trigger_entry,
       rttmon_config_base_command, RTR_CFG_REACT_TRIG_CMD);

KEYWORD(config_rtr_reaction_trigger,
	config_rtr_reaction_trigger_nvgen,
	config_rtr_reaction_configuration,
	D_rttMonReactTriggerAdmin_string,
	"RTR Trigger Assignment", PRIV_CONF);


/******************************************************************
 * [ no ] rtr <entry-number>
 *
 *  OBJ(int,1) = <entry-number>
 */
EOLS(config_rtr_entry_eol, rttmon_config_base_command,
     RTR_CFG_ENTRY_CMD);

NUMBER(config_rtr_entry, config_rtr_entry_eol,
       config_rtr_reaction_trigger, OBJ(int, 1),
       MIN_rttMonApplNumCtrlAdminEntry,
       MAX_rttMonApplNumCtrlAdminEntry, "Entry Number");

NVGENS(config_rtr_entry_nvgen, config_rtr_entry,
       rttmon_config_base_command, RTR_CFG_ENTRY_CMD);


/******************************************************************
 * rtr ...
 */
KEYWORD(config_rttmon, config_rtr_entry_nvgen, ALTERNATE,
	D_rttMon_string, "RTR Base Configuration", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	config_rttmon
