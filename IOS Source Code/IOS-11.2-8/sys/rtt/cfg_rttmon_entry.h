/* $Id: cfg_rttmon_entry.h,v 1.1.4.2 1996/06/07 22:25:45 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/cfg_rttmon_entry.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor parse chain configuration entry header 
 *  file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the definitions for
 * the RTT Monitor configuration entry parse chain.
 *------------------------------------------------------------------
 * $Log: cfg_rttmon_entry.h,v $
 * Revision 1.1.4.2  1996/06/07  22:25:45  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:40:41  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:09  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:12  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:26  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:20  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 *  [ no ] type { echo | pathEcho }  protocol 
 *     { ipIcmpEcho { <ip-address> | <ip-host-name> } |
 *       snaRUEcho <sna-host-name> | 
 *       snaLU0EchoAppl <sna-host-name> 
 *                      [ application-name <sna-application-name> ]
 *                      [ mode-name <sna-mode-name> ] | 
 *       snaLU2EchoAppl <sna-host-name> 
 *                      [ application-name <sna-application-name> ]
 *                      [ mode-name <sna-mode-name> ] }
 *       ...
 *  [ no ] owner <owner-string>
 *  [ no ] tag <tag-string>
 *  [ no ] threshold <threshold-millisecond-value>
 *  [ no ] frequency <frequency-second-value>
 *  [ no ] timeout <timeout-millisecond-value>
 *  [ no ] verify-data
 *  [ no ] request-data-size <payload-data-value>
 *  [ no ] response-data-size <payload-data-value>
 *  [ no ] hours-of-statistics-kept <hour-size>
 *  [ no ] paths-of-statistics-kept <path-size>
 *  [ no ] hops-of-statistics-kept <hop-size>
 *  [ no ] distributions-of-statistics-kept <distribution-size>
 *  [ no ] statistics-distribution-interval <dist-interval-size>
 *  [ no ] lives-of-history-kept <life-size>
 *  [ no ] buckets-of-history-kept <bucket-size>
 *  [ no ] samples-of-history-kept <sample-size>
 *  [ no ] filter-for-history { none | all | 
 *                              overThreshold | failures }
 */

/*
 * Make protocol and address expandable 
 */

LINK_TRANS(config_rttmon_entry_internal_extend_here, ALTERNATE);

/******************************************************************
 *  [ no ] type { echo | pathEcho }  protocol 
 *     { ipIcmpEcho { <ip-address> | <ip-host-name> } |
 *       snaRUEcho <sna-host-name> | 
 *                      [ application-name <sna-application-name> ]
 *                      [ mode-name <sna-mode-name> ] | 
 *       snaLU2EchoAppl <sna-host-name> 
 *                      [ application-name <sna-application-name> ]
 *                      [ mode-name <sna-mode-name> ] }
 *
 *  OBJ(int,1)    = #defined value for
 *                    = echo
 *                    = pathEcho
 *  OBJ(int,2)    = #defined value for
 *                    = ipIcmpEcho
 *                    = snaRUEcho
 *                    = snaLU0EchoAppl
 *                    = snaLU2EchoAppl
 *  OBJ(string,1)  = <ip-address | ip-host-name | sna-host-name>
 *  OBJ(int,3)     = 0 = sna-application-name present
 *                   1 = sna-application-name is not present
 *  OBJ(string,2)  = <sna-application-name>
 *  OBJ(int,4)     = 0 = sna-mode-name present
 *                   1 = sna-mode-name is not present
 *  OBJ(string,3)  = <sna-mode-name>
 */

pdecl(config_entry_rtr_protocol_snaLUEchoAppl_loop);

EOLS(config_entry_rtr_type_eol,
     rttmon_config_entry_command,
     RTR_ENTRY_CFG_TYPE);

SET(config_entry_rtr_protocol_snaLUEchoAppl_mode_name_valid,
    config_entry_rtr_protocol_snaLUEchoAppl_loop,
    OBJ(int, 4), 1);

SET(config_entry_rtr_protocol_snaLUEchoAppl_mode_name_not_valid,
    NONE,
    OBJ(int, 4), 0);

STRING(config_entry_rtr_protocol_snaLUEchoAppl_mode_name_value,
       config_entry_rtr_protocol_snaLUEchoAppl_mode_name_valid,
       config_entry_rtr_protocol_snaLUEchoAppl_mode_name_not_valid,
       OBJ(string, 3), "SNA Mode");

KEYWORD(config_entry_rtr_protocol_snaLUEchoAppl_mode_name,
	config_entry_rtr_protocol_snaLUEchoAppl_mode_name_value,
	config_entry_rtr_protocol_snaLUEchoAppl_mode_name_not_valid,
	D_rttMonEchoAdminProtocolSnaModeName_string,
	"SNA Mode",
	PRIV_CONF);

EQUAL(config_entry_rtr_protocol_snaLUEchoAppl_mode_name_assert,
      config_entry_rtr_protocol_snaLUEchoAppl_mode_name,
      config_entry_rtr_type_eol,
      OBJ(int, 4), 0);

SET(config_entry_rtr_protocol_snaLUEchoAppl_appl_name_valid,
    config_entry_rtr_protocol_snaLUEchoAppl_loop,
    OBJ(int, 3), 1);

SET(config_entry_rtr_protocol_snaLUEchoAppl_appl_name_not_valid,
    NONE,
    OBJ(int, 3), 0);

STRING(config_entry_rtr_protocol_snaLUEchoAppl_appl_name_value,
       config_entry_rtr_protocol_snaLUEchoAppl_appl_name_valid,
       config_entry_rtr_protocol_snaLUEchoAppl_appl_name_not_valid,
       OBJ(string, 2), "SNA Application Name on Host");

KEYWORD(config_entry_rtr_protocol_snaLUEchoAppl_appl_name,
	config_entry_rtr_protocol_snaLUEchoAppl_appl_name_value,
	config_entry_rtr_protocol_snaLUEchoAppl_appl_name_not_valid,
	D_rttMonEchoAdminProtocolSnaApplName_string,
	"SNA Application Name on Host",
	PRIV_CONF);

EQUAL(config_entry_rtr_protocol_snaLUEchoAppl_appl_name_assert,
      config_entry_rtr_protocol_snaLUEchoAppl_appl_name,
      config_entry_rtr_protocol_snaLUEchoAppl_mode_name_assert,
      OBJ(int, 3), 0);

NOP(config_entry_rtr_protocol_snaLUEchoAppl_loop,
    config_entry_rtr_protocol_snaLUEchoAppl_appl_name_assert,
    NONE);

STRING(config_entry_rtr_protocol_snaLUEchoAppl_host_name,
       config_entry_rtr_protocol_snaLUEchoAppl_loop,
       NONE,
       OBJ(string, 1), "Configure SNA Host Name");

SET(config_entry_rtr_protocol_snaLU2EchoAppl_valid,
    config_entry_rtr_protocol_snaLUEchoAppl_host_name,
    OBJ(int, 2),
    D_rttMonEchoAdminProtocol_snaLU2EchoAppl);

KEYWORD(config_entry_rtr_protocol_snaLU2EchoAppl_kwd,
	config_entry_rtr_protocol_snaLU2EchoAppl_valid,
	NONE,
	D_rttMonEchoAdminProtocol_snaLU2EchoAppl_string,
	"Use SNA LU2 Echo Application",
	PRIV_CONF);

TEST_FUNC(config_entry_rtr_protocol_snaLU2EchoAppl,
	  config_entry_rtr_protocol_snaLU2EchoAppl_kwd,
	  NONE,
	  NONE, is_rttmon_snaLU2EchoAppl_configured);

SET(config_entry_rtr_protocol_snaLU0EchoAppl_valid,
    config_entry_rtr_protocol_snaLUEchoAppl_host_name,
    OBJ(int, 2),
    D_rttMonEchoAdminProtocol_snaLU0EchoAppl);

KEYWORD(config_entry_rtr_protocol_snaLU0EchoAppl_kwd,
	config_entry_rtr_protocol_snaLU0EchoAppl_valid,
	config_entry_rtr_protocol_snaLU2EchoAppl,
	D_rttMonEchoAdminProtocol_snaLU0EchoAppl_string,
	"Use SNA LU0 Echo Application",
	PRIV_CONF);

TEST_FUNC(config_entry_rtr_protocol_snaLU0EchoAppl,
	  config_entry_rtr_protocol_snaLU0EchoAppl_kwd,
	  config_entry_rtr_protocol_snaLU2EchoAppl,
	  NONE, is_rttmon_snaLU0EchoAppl_configured);

STRING(config_entry_rtr_protocol_snaRUEcho_host_name,
       config_entry_rtr_type_eol,
       NONE,
       OBJ(string, 1), "Configured SNA Host Name");

SET(config_entry_rtr_protocol_snaRUEcho_valid,
    config_entry_rtr_protocol_snaRUEcho_host_name,
    OBJ(int, 2),
    D_rttMonEchoAdminProtocol_snaRUEcho);

KEYWORD(config_entry_rtr_protocol_snaRUEcho_kwd,
	config_entry_rtr_protocol_snaRUEcho_valid,
	config_entry_rtr_protocol_snaLU0EchoAppl,
	D_rttMonEchoAdminProtocol_snaRUEcho_string,
	"Use SNA RU Test",
	PRIV_CONF);

TEST_FUNC(config_entry_rtr_protocol_snaRUEcho,
	  config_entry_rtr_protocol_snaRUEcho_kwd,
	  config_entry_rtr_protocol_snaLU0EchoAppl,
	  NONE, is_rttmon_snaRUEcho_configured);

STRING(config_entry_rtr_protocol_ip_address,
       config_entry_rtr_type_eol,
       NONE,
       OBJ(string, 1), "IP Address or IP HostName");

SET(config_entry_rtr_protocol_ipIcmpEcho_valid,
    config_entry_rtr_protocol_ip_address,
    OBJ(int, 2),
    D_rttMonEchoAdminProtocol_ipIcmpEcho);

KEYWORD(config_entry_rtr_protocol_ipIcmpEcho_kwd,
	config_entry_rtr_protocol_ipIcmpEcho_valid,
	config_entry_rtr_protocol_snaRUEcho,
	D_rttMonEchoAdminProtocol_ipIcmpEcho_string,
	"Use IP/ICMP",
	PRIV_CONF);

TEST_FUNC(config_entry_rtr_protocol_ipIcmpEcho,
	  config_entry_rtr_protocol_ipIcmpEcho_kwd,
	  config_entry_rtr_protocol_snaRUEcho,
	  NONE, is_rttmon_ipIcmpEcho_configured);

KEYWORD(config_entry_rtr_protocol,
	config_entry_rtr_protocol_ipIcmpEcho,
	NONE,
	D_rttMonEchoAdminProtocol_string,
	"Protocol to Use for Operations",
	PRIV_CONF);

SET(config_entry_rtr_type_pathEcho_valid,
    config_entry_rtr_protocol,
    OBJ(int, 1),
    D_rttMonCtrlAdminRttType_pathEcho);

KEYWORD(config_entry_rtr_type_pathEcho,
	config_entry_rtr_type_pathEcho_valid,
	NONE,
	D_rttMonCtrlAdminRttType_pathEcho_string,
	"Perform Path Discovered Echo Operations",
	PRIV_CONF);

SET(config_entry_rtr_type_echo_valid,
    config_entry_rtr_protocol,
    OBJ(int, 1),
    D_rttMonApplSupportedRttTypes_echo);

KEYWORD(config_entry_rtr_type_echo,
	config_entry_rtr_type_echo_valid,
	config_entry_rtr_type_pathEcho,
	D_rttMonCtrlAdminRttType_echo_string,
	"Perform Point to Point Echo Operations",
	PRIV_CONF);

NVGENS(config_entry_rtr_type_nvgen,
       config_entry_rtr_type_echo,
       rttmon_config_entry_command,
       RTR_ENTRY_CFG_TYPE);

KEYWORD(config_entry_rtr_type,
	config_entry_rtr_type_nvgen,
	config_rttmon_entry_internal_extend_here,
	D_rttMonCtrlAdminRttType_string,
	"Type of Entry",
	PRIV_CONF);

/******************************************************************
 *  [ no ] owner <owner-string>
 *
 *  OBJ(string,1) = <owner-string>
 */
EOLS(config_entry_rtr_owner_eol,
     rttmon_config_entry_command,
     RTR_ENTRY_CFG_OWNER);

TEXT(config_entry_rtr_owner_value,
     config_entry_rtr_owner_eol,
     NONE, OBJ(string, 1),
     "Owner String");

NOPREFIX(config_entry_rtr_owner_no,
	 config_entry_rtr_owner_value,
	 config_entry_rtr_owner_eol);

NVGENS(config_entry_rtr_owner_nvgen,
       config_entry_rtr_owner_value,
       rttmon_config_entry_command,
       RTR_ENTRY_CFG_OWNER);

KEYWORD(config_entry_rtr_owner,
	config_entry_rtr_owner_nvgen,
	config_entry_rtr_type,
	D_rttMonCtrlAdminOwner_string,
	"Owner of Entry",
	PRIV_CONF);

/******************************************************************
 *  [ no ] tag <tag-string>
 *
 *  OBJ(string,1) = <tag-string>
 */
EOLS(config_entry_rtr_tag_eol,
     rttmon_config_entry_command,
     RTR_ENTRY_CFG_TAG);

TEXT(config_entry_rtr_tag_value,
     config_entry_rtr_tag_eol,
     NONE, OBJ(string, 1),
     "Tag String");

NOPREFIX(config_entry_rtr_tag_no,
	 config_entry_rtr_tag_value,
	 config_entry_rtr_tag_eol);

NVGENS(config_entry_rtr_tag_nvgen,
       config_entry_rtr_tag_value,
       rttmon_config_entry_command,
       RTR_ENTRY_CFG_TAG);

KEYWORD(config_entry_rtr_tag,
	config_entry_rtr_tag_nvgen,
	config_entry_rtr_owner,
	D_rttMonCtrlAdminTag_string,
	"User Defined Tag",
	PRIV_CONF);

/******************************************************************
 *  [ no ] threshold <threshold-millisecond-value>
 *
 *  OBJ(int,1) = <threshold-millisecond-value>
 */
PARAMS_KEYONLY(config_entry_rtr_threshold,
	       config_entry_rtr_tag,
	       D_rttMonCtrlAdminThreshold_string,
	       OBJ(int, 1), MIN_rttMonCtrlAdminThreshold,
	       MAX_rttMonCtrlAdminThreshold,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_THRESHOLD,
	       "Operation Threshold in Milliseconds",
	       "Millisecond Threshold Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] frequency <frequency-second-value>
 *
 *  OBJ(int,1) = <frequency-second-value>
 */
PARAMS_KEYONLY(config_entry_rtr_frequency,
	       config_entry_rtr_threshold,
	       D_rttMonCtrlAdminFrequency_string,
	       OBJ(int, 1), MIN_rttMonCtrlAdminFrequency,
	       MAX_rttMonCtrlAdminFrequency,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_FREQUENCY,
	       "Operation Frequency Value in Seconds",
	       "Second Frequency Interval Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] timeout <timeout-millisecond-value>
 *
 *  OBJ(int,1) = <timeout-millisecond-value>
 */
PARAMS_KEYONLY(config_entry_rtr_timeout,
	       config_entry_rtr_frequency,
	       D_rttMonCtrlAdminTimeout_string,
	       OBJ(int, 1), MIN_rttMonCtrlAdminTimeout,
	       MAX_rttMonCtrlAdminTimeout,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_TIMEOUT,
	       "Operation Timeout Value in Milliseconds",
	       "Millisecond Timeout Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] verify-data
 */
EOLS(config_entry_rtr_verify_data_eol,
     rttmon_config_entry_command,
     RTR_ENTRY_CFG_VERIFY_DATA);

KEYWORD(config_entry_rtr_verify_data,
	config_entry_rtr_verify_data_eol,
	config_entry_rtr_timeout,
	D_rttMonCtrlAdminVerifyData_string,
	"Verify Data",
	PRIV_CONF);

/******************************************************************
 *  [ no ] request-data-size <payload-data-value>
 *
 *  OBJ(int,1) = <payload-data-value>
 */
PARAMS_KEYONLY(config_entry_rtr_request_data,
	       config_entry_rtr_verify_data,
	       D_rttMonEchoAdminPktDataRequestSize_string,
	       OBJ(int, 1), MIN_rttMonEchoAdminPktDataRequestSize,
	       MAX_rttMonEchoAdminPktDataRequestSize,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_REQUEST_SIZE,
	       "Requested Request Payload Size",
	       "Request Payload Size",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] response-data-size <payload-data-value>
 *
 *  OBJ(int,1) = <payload-data-value>
 */
PARAMS_KEYONLY(config_entry_rtr_response_data,
	       config_entry_rtr_request_data,
	       D_rttMonEchoAdminPktDataResponseSize_string,
	       OBJ(int, 1), MIN_rttMonEchoAdminPktDataResponseSize,
	       MAX_rttMonEchoAdminPktDataResponseSize,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_RESPONSE_SIZE,
	       "Requested Response Payload Size",
	       "Response Payload Size",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] hours-of-statistics-kept <hour-size>
 *
 *  OBJ(int,1) = <hour-size>
 */
PARAMS_KEYONLY(config_entry_rtr_stats_num_hours,
	       config_entry_rtr_response_data,
	       D_rttMonStatisticsAdminNumHourGroups_string,
	       OBJ(int, 1), MIN_rttMonStatisticsAdminNumHourGroups,
	       SYS_MAX_rttMonStatisticsAdminNumHourGroups,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_HOURS_SIZE,
	       "Maximum Number of Statistics Hour Groups to Capture",
	       "Hour Groups Size Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] paths-of-statistics-kept <path-size>
 *
 *  OBJ(int,1) = <path-size>
 */
PARAMS_KEYONLY(config_entry_rtr_stats_num_paths,
	       config_entry_rtr_stats_num_hours,
	       D_rttMonStatisticsAdminNumPaths_string,
	       OBJ(int, 1), MIN_rttMonStatisticsAdminNumPaths,
	       SYS_MAX_rttMonStatisticsAdminNumPaths,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_PATHS_SIZE,
	       "Maximum Number of Statistics Paths to Capture",
	       "Paths Size Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] hops-of-statistics-kept <hop-size>
 *
 *  OBJ(int,1) = <hop-size>
 */
PARAMS_KEYONLY(config_entry_rtr_stats_num_hops,
	       config_entry_rtr_stats_num_paths,
	       D_rttMonStatisticsAdminNumHops_string,
	       OBJ(int, 1), MIN_rttMonStatisticsAdminNumHops,
	       SYS_MAX_rttMonStatisticsAdminNumHops,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_HOPS_SIZE,
	       "Maximum Number of Statistics Hops to Capture",
	       "Hops Size Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] distributions-of-statistics-kept <distribution-size>
 *
 *  OBJ(int,1) = <distribution-size>
 */
PARAMS_KEYONLY(config_entry_rtr_stats_num_dists,
	       config_entry_rtr_stats_num_hops,
	       D_rttMonStatisticsAdminNumDistBuckets_string,
	       OBJ(int, 1), MIN_rttMonStatisticsAdminNumDistBuckets,
	       SYS_MAX_rttMonStatisticsAdminNumDistBuckets,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_DIST_SIZE,
	     "Maximum Number of Statistics Distribution Buckets to Capture",
	       "Distribution Bucket Size Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] statistics-distribution-interval <dist-interval-size>
 *
 *  OBJ(int,1) = <dist-interval-size>
 */
PARAMS_KEYONLY(config_entry_rtr_stats_dist_interval,
	       config_entry_rtr_stats_num_dists,
	       D_rttMonStatisticsAdminDistInterval_string,
	       OBJ(int, 1), MIN_rttMonStatisticsAdminDistInterval,
	       MAX_rttMonStatisticsAdminDistInterval,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_DIST_INTERVAL_SIZE,
	       "Statistics Distribution Interval Size",
	       "Distribution Interval Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] lives-of-history-kept <life-size>
 *
 *  OBJ(int,1) = <life-size>
 */
PARAMS_KEYONLY(config_entry_rtr_hist_num_lives,
	       config_entry_rtr_stats_dist_interval,
	       D_rttMonHistoryAdminNumLives_string,
	       OBJ(int, 1), MIN_rttMonHistoryAdminNumLives,
	       SYS_MAX_rttMonHistoryAdminNumLives,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_LIFE_SIZE,
	       "Maximum Number of History Lives to Collect",
	       "Life Size Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] buckets-of-history-kept <bucket-size>
 *
 *  OBJ(int,1) = <bucket-size>
 */
PARAMS_KEYONLY(config_entry_rtr_hist_num_buckets,
	       config_entry_rtr_hist_num_lives,
	       D_rttMonHistoryAdminNumBuckets_string,
	       OBJ(int, 1), MIN_rttMonHistoryAdminNumBuckets,
	       SYS_MAX_rttMonHistoryAdminNumBuckets,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_BUCKET_SIZE,
	       "Maximum Number of History Buckets to Collect",
	       "Bucket Size Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] samples-of-history-kept <sample-size>
 *
 *  OBJ(int,1) = <sample-size>
 */
PARAMS_KEYONLY(config_entry_rtr_hist_num_samples,
	       config_entry_rtr_hist_num_buckets,
	       D_rttMonHistoryAdminNumSamples_string,
	       OBJ(int, 1), MIN_rttMonHistoryAdminNumSamples,
	       SYS_MAX_rttMonHistoryAdminNumSamples,
	       rttmon_config_entry_command,
	       RTR_ENTRY_CFG_SAMPLE_SIZE,
	       "Maximum Number of History Samples to Collect",
	       "Sample Size Value",
	       PRIV_CONF);

/******************************************************************
 *  [ no ] filter-for-history { none | all | 
 *                              overThreshold | failures }
 *
 *  OBJ(int,1) = #defined value for
 *                 = none
 *                 = all
 *                 = overThreshold
 *                 = failures
 */
EOLS(config_entry_rtr_hist_filter_eol,
     rttmon_config_entry_command, RTR_ENTRY_CFG_HIST_FILTER);

SET(config_entry_rtr_hist_filter_failures_yes,
    config_entry_rtr_hist_filter_eol,
    OBJ(int, 1),
    D_rttMonHistoryAdminFilter_failures);

KEYWORD(config_entry_rtr_hist_filter_failures,
	config_entry_rtr_hist_filter_failures_yes, NONE,
	D_rttMonHistoryAdminFilter_failures_string,
	"Collect Operations that Fail in History", PRIV_CONF);

SET(config_entry_rtr_hist_filter_overThreshold_yes,
    config_entry_rtr_hist_filter_eol,
    OBJ(int, 1),
    D_rttMonHistoryAdminFilter_overThreshold);

KEYWORD(config_entry_rtr_hist_filter_overThreshold,
	config_entry_rtr_hist_filter_overThreshold_yes,
	config_entry_rtr_hist_filter_failures,
	D_rttMonHistoryAdminFilter_overThreshold_string,
	"Collect Operations that are Over Threshold in History",
	PRIV_CONF);

SET(config_entry_rtr_hist_filter_all_yes,
    config_entry_rtr_hist_filter_eol,
    OBJ(int, 1),
    D_rttMonHistoryAdminFilter_all);

KEYWORD(config_entry_rtr_hist_filter_all,
	config_entry_rtr_hist_filter_all_yes,
	config_entry_rtr_hist_filter_overThreshold,
	D_rttMonHistoryAdminFilter_all_string,
	"Collect Every Operation in History",
	PRIV_CONF);

SET(config_entry_rtr_hist_filter_none_yes,
    config_entry_rtr_hist_filter_eol,
    OBJ(int, 1),
    D_rttMonHistoryAdminFilter_none);

KEYWORD(config_entry_rtr_hist_filter_none,
	config_entry_rtr_hist_filter_none_yes,
	config_entry_rtr_hist_filter_all,
	D_rttMonHistoryAdminFilter_none_string,
	"Shutoff History Collection", PRIV_CONF);

NVGENS(config_entry_rtr_hist_filter_nvgen,
       config_entry_rtr_hist_filter_none,
       rttmon_config_entry_command, RTR_ENTRY_CFG_HIST_FILTER);

KEYWORD(config_entry_rtr_hist_filter,
	config_entry_rtr_hist_filter_nvgen,
	config_entry_rtr_hist_num_samples,
	D_rttMonHistoryAdminFilter_string,
	"Add Operation to History when...",
	PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	config_entry_rtr_hist_filter
