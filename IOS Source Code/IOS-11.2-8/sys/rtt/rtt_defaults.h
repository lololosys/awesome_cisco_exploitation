/* $Id: rtt_defaults.h,v 1.1.4.3 1996/06/07 22:26:04 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_defaults.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor defaults Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the default values for the
 * RTT Monitor. 
 *------------------------------------------------------------------
 * $Log: rtt_defaults.h,v $
 * Revision 1.1.4.3  1996/06/07  22:26:04  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/06/04  18:28:27  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.1  1996/05/17  11:41:09  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:41:26  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:35  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:27  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTTDEFAULTS_H_
#define _RTTDEFAULTS_H_

#include "sr_rttmibdefs.h"
#include "rtt_show.h"

extern const unsigned char rtt_arr_eyecather_str[];

#define ARR_HEADER_SIZE           12

#define RTT_VERSION               "1.0.1 Initial Round Trip Time MIB"
#define RTT_ARR_VERSION_NUM       0x01
#define RTT_ARR_EYECATCHER        rtt_arr_eyecather_str
#define RTT_ARR_LOGOFF            0x01
#define RTT_ARR_ECHO_REQUEST      0x02
#define RTT_ARR_ECHO_RESPONSE     0x03
#define RTT_ARR_SW_VERS_REQ       0x04
#define RTT_ARR_SW_VERS_RSP       0x05

#define DEF_show_appl_type             rtt_output_full
#define DEF_show_cfg_type              rtt_output_full
#define DEF_show_oper_type             rtt_output_full
#define DEF_show_stat_totals_type      rtt_output_full
#define DEF_show_stat_collect_type     rtt_output_full
#define DEF_show_stat_dist_type        rtt_output_brief
#define DEF_show_hist_type             rtt_output_brief
#define DEF_show_react_trigger_type    rtt_output_full

/* 
 * If the MIB changes (Number of RttMonProtocol items) grow larger
 *   than the following value, the following value must be increased.
 * The first entry in the array is not used, so 1..real_Max + 1.
 */
#define MAX_RTT_PROTOCOLS                  24
/*  The following is a sparce array 1..4 */
#define MAX_RTT_TYPES                      5

/* The following are taken directly from the MIB */
#define MIN_TimeTicks                                 0
#define MAX_TimeTicks                                 ((unsigned long)4294967295)

#define MIN_ApplSpecSense                             1001
#define MAX_ApplSpecSense                             2147483647

#define MIN_SenseDescription                          0
#define MAX_SenseDescription                          255

#define MIN_rttMonApplMaxPacketDataSize               0
#define MAX_rttMonApplMaxPacketDataSize               16384
#define DEF_rttMonApplMaxPacketDataSize               16384

#define MIN_rttMonApplNumCtrlAdminEntry               1
#define MAX_rttMonApplNumCtrlAdminEntry               256
#define DEF_rttMonApplNumCtrlAdminEntry               10

#define MIN_rttMonApplPreConfigedName                 0
#define MAX_rttMonApplPreConfigedName                 255
#define DEF_rttMonApplPreConfigedName                 ""

#define MIN_rttMonCtrlAdminIdx                        1
#define MAX_rttMonCtrlAdminIdx                        2147483647

#define MIN_rttMonCtrlAdminOwner                      0
#define MAX_rttMonCtrlAdminOwner                      255
#define DEF_rttMonCtrlAdminOwner                      ""

#define MIN_rttMonCtrlAdminTag                        0
#define MAX_rttMonCtrlAdminTag                        16
#define DEF_rttMonCtrlAdminTag                        ""

#define DEF_rttMonCtrlAdminRttType                    D_rttMonCtrlAdminRttType_echo

#define MIN_rttMonCtrlAdminThreshold                  0
#define MAX_rttMonCtrlAdminThreshold                  2147483647
#define DEF_rttMonCtrlAdminThreshold                  5000

#define MIN_rttMonCtrlAdminFrequency                  0
#define MAX_rttMonCtrlAdminFrequency                  604800
#define DEF_rttMonCtrlAdminFrequency                  60

#define MIN_rttMonCtrlAdminTimeout                    0
#define MAX_rttMonCtrlAdminTimeout                    604800000
#define DEF_rttMonCtrlAdminTimeout                    5000

#define DEF_rttMonCtrlAdminVerifyData                 D_rttMonCtrlAdminVerifyData_false

#define DEF_rttMonCtrlAdminStatus                     D_rttMonCtrlAdminStatus_createAndGo

#define DEF_rttMonEchoAdminProtocol                   D_rttMonEchoAdminProtocol_notApplicable

#define DEF_rttMonEchoAdminTargetAddress              ""

#define MIN_rttMonEchoAdminPktDataRequestSize         0
#define MAX_rttMonEchoAdminPktDataRequestSize         16384
#define DEF_rttMonEchoAdminPktDataRequestSize         1

#define MIN_rttMonEchoAdminPktDataResponseSize        0
#define MAX_rttMonEchoAdminPktDataResponseSize        16384
#define DEF_rttMonEchoAdminPktDataResponseSize        0

#define MIN_rttMonFileIOAdminFilePath                 0
#define MAX_rttMonFileIOAdminFilePath                 255
#define DEF_rttMonFileIOAdminFilePath                 ""

#define DEF_rttMonFileIOAdminSize                     D_rttMonFileIOAdminSize_n256

#define DEF_rttMonFileIOAdminAction                   D_rttMonFileIOAdminAction_read

#define MIN_rttMonScriptAdminName                     0
#define MAX_rttMonScriptAdminName                     255
#define DEF_rttMonScriptAdminName                     ""

#define MIN_rttMonScriptAdminCmdLineParams            0
#define MAX_rttMonScriptAdminCmdLineParams            255
#define DEF_rttMonScriptAdminCmdLineParams            ""

#define MIN_rttMonScheduleAdminRttLife                0
#define MAX_rttMonScheduleAdminRttLife                2147483647
#define DEF_rttMonScheduleAdminRttLife                3600
#define DEF_rttMonScheduleAdminRttLife_FOREVER        2147483647

#define MIN_rttMonScheduleAdminRttStartTime           MIN_TimeTicks
#define MAX_rttMonScheduleAdminRttStartTime           MAX_TimeTicks
#define DEF_rttMonScheduleAdminRttStartTime           0

#define MIN_rttMonScheduleAdminConceptRowAgeout       0
#define MAX_rttMonScheduleAdminConceptRowAgeout       2073600
#define DEF_rttMonScheduleAdminConceptRowAgeout       3600
#define CLI_DEF_rttMonScheduleAdminConceptRowAgeout   0

#define MIN_rttMonLatestRttOperApplSpecificSense      MIN_ApplSpecSense
#define MAX_rttMonLatestRttOperApplSpecificSense      MAX_ApplSpecSense

#define MIN_rttMonLatestRttOperSenseDescription       MIN_SenseDescription
#define MAX_rttMonLatestRttOperSenseDescription       MAX_SenseDescription
#define DEF_rttMonLatestRttOperSenseDescription       ""

#define DEF_rttMonReactAdminConnectionEnable          D_rttMonReactAdminConnectionEnable_false

#define DEF_rttMonReactAdminTimeoutEnable             D_rttMonReactAdminTimeoutEnable_false

#define DEF_rttMonReactAdminThresholdType             D_rttMonReactAdminThresholdType_never

#define MIN_rttMonReactAdminThresholdFalling          0
#define MAX_rttMonReactAdminThresholdFalling          2147483647
#define DEF_rttMonReactAdminThresholdFalling          3000

#define MIN_rttMonReactAdminThresholdCount            1
#define MAX_rttMonReactAdminThresholdCount            16
#define DEF_rttMonReactAdminThresholdCount            5

#define MIN_rttMonReactAdminThresholdCount2           1
#define MAX_rttMonReactAdminThresholdCount2           16
#define DEF_rttMonReactAdminThresholdCount2           5

#define DEF_rttMonReactAdminActionType                D_rttMonReactAdminActionType_none

#define MIN_rttMonStatisticsAdminNumHourGroups        0
#define MAX_rttMonStatisticsAdminNumHourGroups        512
#define DEF_rttMonStatisticsAdminNumHourGroups        2

#define MIN_rttMonStatisticsAdminNumPaths             1
#define MAX_rttMonStatisticsAdminNumPaths             128
#define DEF_rttMonStatisticsAdminNumPaths             5

#define MIN_rttMonStatisticsAdminNumHops              1
#define MAX_rttMonStatisticsAdminNumHops              512
#define DEF_rttMonStatisticsAdminNumHops              16

#define MIN_rttMonStatisticsAdminNumDistBuckets       1
#define MAX_rttMonStatisticsAdminNumDistBuckets       512
#define DEF_rttMonStatisticsAdminNumDistBuckets       1

#define MIN_rttMonStatisticsAdminDistInterval         1
#define MAX_rttMonStatisticsAdminDistInterval         100
#define DEF_rttMonStatisticsAdminDistInterval         20

#define MIN_rttMonHistoryAdminNumLives                0
#define MAX_rttMonHistoryAdminNumLives                255
#define DEF_rttMonHistoryAdminNumLives                0

#define MIN_rttMonHistoryAdminNumBuckets              1
#define MAX_rttMonHistoryAdminNumBuckets              2147483647
#define DEF_rttMonHistoryAdminNumBuckets              50

#define MIN_rttMonHistoryAdminNumSamples              1
#define MAX_rttMonHistoryAdminNumSamples              512
#define DEF_rttMonHistoryAdminNumSamples              16

#define DEF_rttMonHistoryAdminFilter                  D_rttMonHistoryAdminFilter_none

#define MIN_rttMonCtrlOperModificationTime            MIN_TimeTicks
#define MAX_rttMonCtrlOperModificationTime            MAX_TimeTicks

#define MIN_rttMonCtrlOperDiagText                    0
#define MAX_rttMonCtrlOperDiagText                    51
#define DEF_rttMonCtrlOperDiagText                    ""

#define MIN_rttMonCtrlOperResetTime                   MIN_TimeTicks
#define MAX_rttMonCtrlOperResetTime                   MAX_TimeTicks

#define DEF_rttMonCtrlOperConnectionLostOccurred      D_rttMonCtrlOperConnectionLostOccurred_false

#define DEF_rttMonCtrlOperTimeoutOccurred             D_rttMonCtrlOperTimeoutOccurred_false

#define DEF_rttMonCtrlOperOverThresholdOccurred       D_rttMonCtrlOperOverThresholdOccurred_false

#define MIN_rttMonCtrlOperNumRtts                     0
#define MAX_rttMonCtrlOperNumRtts                     2147483647

#define MIN_rttMonCrtlOperRttLife                     0
#define MAX_rttMonCrtlOperRttLife                     2147483647
#define DEF_OperRttLifeTimeChunk                      400000

#define DEF_rttMonCtrlOperState                       D_rttMonCtrlOperState_inactive

#define MIN_rttMonReactTriggerAdminRttMonCtrlAdminIdx 1
#define MAX_rttMonReactTriggerAdminRttMonCtrlAdminIdx 2147483647

#define MIN_rttMonStatsCaptureStartTimeIndex          MIN_TimeTicks
#define MAX_rttMonStatsCaptureStartTimeIndex          MAX_TimeTicks

#define MIN_rttMonStatsCapturePathIdx                 1
#define MAX_rttMonStatsCapturePathIdx                 128

#define MIN_rttMonStatsCaptureHopIdx                  1
#define MAX_rttMonStatsCaptureHopIdx                  512

#define MIN_rttMonStatsCaptureDistIdx                 1
#define MAX_rttMonStatsCaptureDistIdx                 512

#define MIN_rttMonStatsCaptureCompletions             0
#define MAX_rttMonStatsCaptureCompletions             2147483647

#define MIN_rttMonStatsCaptureOverThresholds          0
#define MAX_rttMonStatsCaptureOverThresholds          2147483647

#define MIN_rttMonStatsCollectNumDisconnects          0
#define MAX_rttMonStatsCollectNumDisconnects          2147483647

#define MIN_rttMonStatsCollectTimeouts                0
#define MAX_rttMonStatsCollectTimeouts                2147483647

#define MIN_rttMonStatsCollectBusies                  0
#define MAX_rttMonStatsCollectBusies                  2147483647

#define MIN_rttMonStatsCollectNoConnections           0
#define MAX_rttMonStatsCollectNoConnections           2147483647

#define MIN_rttMonStatsCollectDrops                   0
#define MAX_rttMonStatsCollectDrops                   2147483647

#define MIN_rttMonStatsCollectSequenceErrors          0
#define MAX_rttMonStatsCollectSequenceErrors          2147483647

#define MIN_rttMonStatsCollectVerifyErrors            0
#define MAX_rttMonStatsCollectVerifyErrors            2147483647

#define MIN_rttMonStatsTotalsInitiations              0
#define MAX_rttMonStatsTotalsInitiations              2147483647

#define MIN_rttMonHistoryCollectionLifeIdx            1
#define MAX_rttMonHistoryCollectionLifeIdx            2147483647

#define MIN_rttMonHistoryCollectionBucketIdx          1
#define MAX_rttMonHistoryCollectionBucketIdx          2147483647

#define MIN_rttMonHistoryCollectionSampleIdx          1
#define MAX_rttMonHistoryCollectionSampleIdx          512

#define MIN_rttMonHistoryCollectionApplSpecificSense  MIN_ApplSpecSense
#define MAX_rttMonHistoryCollectionApplSpecificSense  MAX_ApplSpecSense

#define MIN_rttMonHistoryCollectionSenseDescription   MIN_SenseDescription
#define MAX_rttMonHistoryCollectionSenseDescription   MAX_SenseDescription

/* The following are coding defaults */

#define MIN_rttMonTargetAddress                       0
#define MAX_rttMonTargetAddress                       92
#define MAX_rttMonTargetAddress_ipIcmpEcho            4
#define MAX_rttMonTargetAddress_ipUdpEchoAppl         4
#define MAX_rttMonTargetAddress_snaRUEcho             17
#define MAX_rttMonTargetAddress_appleTalkEcho         3
#define MAX_rttMonTargetAddress_appleTalkEchoAppl     3
#define MAX_rttMonTargetAddress_decNetEcho            2
#define MAX_rttMonTargetAddress_decNetEchoAppl        2
#define MAX_rttMonTargetAddress_ipxEcho               10
#define MAX_rttMonTargetAddress_ipxEchoAppl           10
#define MAX_rttMonTargetAddress_vinesEcho             6
#define MAX_rttMonTargetAddress_vinesEchoAppl         6
#define MAX_rttMonTargetAddress_xnsEcho               10
#define MAX_rttMonTargetAddress_xnsEchoAppl           10
#define MAX_rttMonTargetAddress_apolloEcho            10
#define MAX_rttMonTargetAddress_apolloEchoAppl        10
#define MAX_rttMonTargetAddress_netbiosEchoAppl       16
#define DEF_rttMonDependLU_App_Name                   "NSPECHO"

/* The following are the implementation Maximums */
#define SYS_MAX_rttMonApplNumCtrlAdminEntry           200
#define SYS_MAX_rttMonStatisticsAdminNumHourGroups    25
#define SYS_MAX_rttMonStatisticsAdminNumPaths         50
#define SYS_MAX_rttMonStatisticsAdminNumHops          50
#define SYS_MAX_rttMonStatisticsAdminNumDistBuckets   20
#define SYS_MAX_rttMonHistoryAdminNumLives            25
#define SYS_MAX_rttMonHistoryAdminNumBuckets          50
#define SYS_MAX_rttMonHistoryAdminNumSamples          50

#endif /* _RTTDEFAULTS_H_ */
