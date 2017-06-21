/* $Id: rtt_strings.h,v 1.1.4.1 1996/05/17 11:41:48 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_strings.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor strings Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the default values for the
 * RTT Monitor strings used in the CLI. 
 *------------------------------------------------------------------
 * $Log: rtt_strings.h,v $
 * Revision 1.1.4.1  1996/05/17  11:41:48  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:52  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:57  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:52  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:38  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTTSTRINGS_H_
#define _RTTSTRINGS_H_

#define D_rttMon_string                                          "rtr"
#define D_rttMon_ConfigSubmode_string                            "config-rtr"
#define D_rttMonApplReset_reset_string                           "reset"
#define D_rttMonScheduleAdminConceptRowAgeout_string             "ageout"
#define D_rttMonScheduleAdminRttStartTime_now_string             "now"
#define D_rttMonScheduleAdminRttStartTime_pending_string         "pending"
#define D_rttMonScheduleAdminRttStartTime_string                 "start-time"
#define D_rttMonScheduleAdminRttLife_string                      "life"
#define D_rttMonSchedule_string                                  "schedule"
#define D_rttMonReactAdminActionType_string                      "action-type"
#define D_rttMonReactAdminThresholdType_string                   "threshold-type"
#define D_rttMonReactAdminThresholdFalling_string                "threshold-falling"
#define D_rttMonReactAdminTimeoutEnable_string                   "timeout-enable"
#define D_rttMonReactAdminConnectionEnable_string                "connection-loss-enable"
#define D_rttMonReactAdmin_string                                "reaction-configuration"
#define D_rttMonReactTriggerAdmin_string                         "reaction-trigger"
#define D_rttMonEchoAdminProtocol_string                         "protocol"
#define D_rttMonEchoAdminProtocolSnaApplName_string              "application-name"
#define D_rttMonEchoAdminProtocolSnaModeName_string              "mode-name"
#define D_rttMonCtrlAdminRttType_string                          "type"
#define D_rttMonCtrlAdminOwner_string                            "owner"
#define D_rttMonCtrlAdminTag_string                              "tag"
#define D_rttMonCtrlAdminThreshold_string                        "threshold"
#define D_rttMonCtrlAdminFrequency_string                        "frequency"
#define D_rttMonCtrlAdminTimeout_string                          "timeout"
#define D_rttMonCtrlAdminVerifyData_string                       "verify-data"
#define D_rttMonEchoAdminPktDataRequestSize_string               "request-data-size"
#define D_rttMonEchoAdminPktDataResponseSize_string              "response-data-size"
#define D_rttMonStatisticsAdminNumHourGroups_string              "hours-of-statistics-kept"
#define D_rttMonStatisticsAdminNumPaths_string                   "paths-of-statistics-kept"
#define D_rttMonStatisticsAdminNumHops_string                    "hops-of-statistics-kept"
#define D_rttMonStatisticsAdminNumDistBuckets_string             "distributions-of-statistics-kept"
#define D_rttMonStatisticsAdminDistStats_string                  "distributions-statistics"
#define D_rttMonStatisticsAdminCollStats_string                  "collection-statistics"
#define D_rttMonStatisticsAdminTotalsStats_string                "totals-statistics"
#define D_rttMonStatisticsAdminDistInterval_string               "statistics-distribution-interval"
#define D_rttMonHistoryAdminNumLives_string                      "lives-of-history-kept"
#define D_rttMonHistoryAdminNumBuckets_string                    "buckets-of-history-kept"
#define D_rttMonHistoryAdminNumSamples_string                    "samples-of-history-kept"
#define D_rttMonHistoryAdminFilter_string                        "filter-for-history"
#define D_rttMonHistoryAdmin_string                              "history"
#define D_rttMonDebugError_string                                "error"
#define D_rttMonDebugTrace_string                                "trace"
#define D_rttMonShowTabular_string                               "tabular"
#define D_rttMonShowFull_string                                  "full"
#define D_rttMonShowRaw_string                                   "raw"
#define D_rttMonAppl_string                                      "application"
#define D_rttMonCtrlOper_string                                  "operational-state"
#define D_rttMonCtrlAdmin_string                                 "configuration"

#define D_rttMonCtrlAdminRttType_echo_string                     "echo"
#define D_rttMonCtrlAdminRttType_pathEcho_string                 "pathEcho"
#define D_rttMonCtrlAdminRttType_fileIO_string                   "fileIO"
#define D_rttMonCtrlAdminRttType_script_string                   "script"
#define D_Unknown_string                                         "Unknown"

#define D_rttMonEchoAdminProtocol_notApplicable_string           "notApplicable"
#define D_rttMonEchoAdminProtocol_ipIcmpEcho_string              "ipIcmpEcho"
#define D_rttMonEchoAdminProtocol_ipUdpEchoAppl_string           "ipUdpEchoAppl"
#define D_rttMonEchoAdminProtocol_snaRUEcho_string               "snaRUEcho"
#define D_rttMonEchoAdminProtocol_snaLU0EchoAppl_string          "snaLU0EchoAppl"
#define D_rttMonEchoAdminProtocol_snaLU2EchoAppl_string          "snaLU2EchoAppl"
#define D_rttMonEchoAdminProtocol_snaLU62Echo_string             "snaLU62Echo"
#define D_rttMonEchoAdminProtocol_snaLU62EchoAppl_string         "snaLU62EchoAppl"
#define D_rttMonEchoAdminProtocol_appleTalkEcho_string           "appleTalkEcho"
#define D_rttMonEchoAdminProtocol_appleTalkEchoAppl_string       "appleTalkEchoAppl"
#define D_rttMonEchoAdminProtocol_decNetEcho_string              "decNetEcho"
#define D_rttMonEchoAdminProtocol_decNetEchoAppl_string          "decNetEchoAppl"
#define D_rttMonEchoAdminProtocol_ipxEcho_string                 "ipxEcho"
#define D_rttMonEchoAdminProtocol_ipxEchoAppl_string             "ipxEchoAppl"
#define D_rttMonEchoAdminProtocol_isoClnsEcho_string             "isoClnsEcho"
#define D_rttMonEchoAdminProtocol_isoClnsEchoAppl_string         "isoClnsEchoAppl"
#define D_rttMonEchoAdminProtocol_vinesEcho_string               "vinesEcho"
#define D_rttMonEchoAdminProtocol_vinesEchoAppl_string           "vinesEchoAppl"
#define D_rttMonEchoAdminProtocol_xnsEcho_string                 "xnsEcho"
#define D_rttMonEchoAdminProtocol_xnsEchoAppl_string             "xnsEchoAppl"
#define D_rttMonEchoAdminProtocol_apolloEcho_string              "apolloEcho"
#define D_rttMonEchoAdminProtocol_apolloEchoAppl_string          "apolloEchoAppl"
#define D_rttMonEchoAdminProtocol_netbiosEchoAppl_string         "netbiosEchoAppl"

#define D_rttMonCtrlAdminStatus_active_string                    "active"
#define D_rttMonCtrlAdminStatus_notInService_string              "notInService"
#define D_rttMonCtrlAdminStatus_notReady_string                  "notReady"
#define D_rttMonCtrlAdminStatus_createAndGo_string               "createAndGo"
#define D_rttMonCtrlAdminStatus_createAndWait_string             "createAndWait"
#define D_rttMonCtrlAdminStatus_destroy_string                   "destroy"

#define D_rttMonReactAdminThresholdType_never_string             "never"
#define D_rttMonReactAdminThresholdType_immediate_string         "immediate"
#define D_rttMonReactAdminThresholdType_consecutive_string       "consecutive"
#define D_rttMonReactAdminThresholdType_xOfy_string              "xOfy"
#define D_rttMonReactAdminThresholdType_average_string           "average"

#define D_rttMonReactAdminActionType_none_string                 "none"
#define D_rttMonReactAdminActionType_trapOnly_string             "trapOnly"
#define D_rttMonReactAdminActionType_nmvtOnly_string             "nmvtOnly"
#define D_rttMonReactAdminActionType_triggerOnly_string          "triggerOnly"
#define D_rttMonReactAdminActionType_trapAndNmvt_string          "trapAndNmvt"
#define D_rttMonReactAdminActionType_trapAndTrigger_string       "trapAndTrigger"
#define D_rttMonReactAdminActionType_nmvtAndTrigger_string       "nmvtAndTrigger"
#define D_rttMonReactAdminActionType_trapNmvtAndTrigger_string   "trapNmvtAndTrigger"

#define D_rttMonHistoryAdminFilter_none_string                   "none"
#define D_rttMonHistoryAdminFilter_all_string                    "all"
#define D_rttMonHistoryAdminFilter_overThreshold_string          "overThreshold"
#define D_rttMonHistoryAdminFilter_failures_string               "failures"

#define D_rttMonCtrlOperState_reset_string                       "reset"
#define D_rttMonCtrlOperState_orderlyStop_string                 "orderlyStop"
#define D_rttMonCtrlOperState_immediateStop_string               "immediateStop"
#define D_rttMonCtrlOperState_pending_string                     "pending"
#define D_rttMonCtrlOperState_inactive_string                    "inactive"
#define D_rttMonCtrlOperState_active_string                      "active"

#define D_rttMonLatestRttOperSense_ok_string                     "ok"
#define D_rttMonLatestRttOperSense_disconnected_string           "disconnected"
#define D_rttMonLatestRttOperSense_overThreshold_string          "overThreshold"
#define D_rttMonLatestRttOperSense_timeout_string                "timeout"
#define D_rttMonLatestRttOperSense_busy_string                   "busy"
#define D_rttMonLatestRttOperSense_notConnected_string           "notConnected"
#define D_rttMonLatestRttOperSense_dropped_string                "dropped"
#define D_rttMonLatestRttOperSense_sequenceError_string          "sequenceError"
#define D_rttMonLatestRttOperSense_verifyError_string            "verifyError"
#define D_rttMonLatestRttOperSense_applicationSpecific_string    "applicationSpecific"

#define D_rttMonReactTriggerOperState_active_string              "active"
#define D_rttMonReactTriggerOperState_pending_string             "pending"

#endif /* _RTTSTRINGS_H_ */
