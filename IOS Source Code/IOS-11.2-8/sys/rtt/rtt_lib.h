/* $Id: rtt_lib.h,v 1.1.4.5 1996/08/28 13:08:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_lib.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor external interface Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the function definitions for
 * external interface calls used by the components of the
 * RTT Monitor.  It also contains the return codes.
 *------------------------------------------------------------------
 * $Log: rtt_lib.h,v $
 * Revision 1.1.4.5  1996/08/28  13:08:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.4.4  1996/06/12  19:23:04  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xxbit_word (final part)
 *
 * All good things must come to an end.  ntohl() and friends finally have
 * UNIX semantics in California releases and later.
 *
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/06/07  22:26:18  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/06/04  18:28:49  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.1  1996/05/17  11:41:28  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:45  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:46  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:42  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:31  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTTLIB_H_
#define _RTTLIB_H_

#include "master.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "registry.h"
#include "../snmp/snmp_registry.regh"
#include "mgd_timers.h"
#include "rtt_defaults.h"
#include "sr_rttmibdefs.h"
#include "sr_rttmibsupp.h"
#include "rtt_util.h"
#include "rtt_global.h"
#include "rtt_support.h"
#include "rtt_registry.h"
#include "rtt_strings.h"
#include "rtt_scheduler.h"
#include "rtt_strings.h"

/*
 * The following enums maps to the array
 *  of message strings defined in 
 *  rtt_dump.c.  THEY MUST ALWAYS REMAIN
 *  SYNCRONIZED!
 */
enum rtt_processHeader_msg_ids {
    RttIPPanicMsg = 0,
    RttEchoTargetCalled,
    RttWokeOnTimeout,
    RttWokeOnRealEvent,
    RttSetupPathInfoCalled,
    RttErrorReturnCode,
    RttCallingEndUpdatePath,
    RttPerformEchoCalled,
    RttCallingGetFirstStoredHop,
    RttReturnedFromEchoTarget,
    RttCallingUpdateResponseTimeout,
    RttCallingUpdateResponseVerifyError,
    RttCorruptedReplyToRequest,
    RttCallingUpdateResponseSequenceError,
    RttCallingUpdateResponseDropped,
    RttCallingUpdateResponseOk,
    RttNativeResponseCode,
    RttCallingGetNextStoredHop,
    RttEnteringProcess,
    RttBadRttType,
    RttCallingGetOperState,
    RttWaitingToStart,
    RttCallingTransitionState,
    RttTransitionToInactive,
    RttCallingStartLife,
    RttCallingUpdateEchoAttempts,
    RttStartingAnEchoOperation,
    RttFrequencyOverrun,
    RttCallingUpdateResponseBusy,
    RttCallingGetOperStateCheckActive,
    RttGoingToSleep,
    RttCallingGetOperStateCheckPending,
    RttTransitionToPending,
    RttCallingGetOperStateCheckDeath,
    RttOperStateNoLongerActive,
    RttCallingGetOperStateCheckInactive,
    RttFailedToStartIPEchoApplication,
    RttDependLuPanicMsg,
    RttBadTargetAddressLength,
    RttCloseIndicationCalled,
    RttSettingReceiveFinishTrue,
    RttOpenConnectionCalled,
    RttCallingLuT0orT2Open,
    RttDumpingLuT0orT2OpenResult,
    RttCallingSetConnectionHandlePlaceholder,
    RttCallingSetConnectedStateFalse,
    RttCallingSetConnectionHandle,
    RttCallingSetConnectedStateTrue,
    RttBindDataLengthShorterThanExpected,
    RttCallingSetDiagText,
    RttReceiveIndicationCalled,
    RttBufferReceiveOverflow,
    RttCallingLUReceiveContinue,
    RttSendLogoffCalled,
    RttCallingLuReceive,
    RttCallingLuSend,
    RttReceiveFinishedIsFalse,
    RttCallingUpdatePath,
    RttCallingLuClose,
    RttCallingUpdateResponseDisconnected,
    RttCallingVerifyEBCDICPortion,
    RttSettingReceiveFinishedFalse,
    RttCallingUpdateResponseNotConnected,
    RttSSCPRuPanicMsg,
    RttCallingSscpLuOpen,
    RttCallingDumpSscpLuOpenResult,
    RttCallingSscpLuReceiveContinue,
    RttCallingSscpLuSend,
    RttAckBackToHostCalled,
    RttSendingFirstAck,
    RttAck1SendFailed,
    RttSendingSecondAck,
    RttAck2SendFailed,
    RttCallingSscpLuClose,
    RttEchoTestNotGrantedBySscp,
    RttFailedToStartSscpRuEchoApplication,
    RttFailedToStartLu0EchoApplication,
    RttFailedToStartLu2EchoApplication,
    RttFindingRoute,
    RttResolutionNotSupported,
    RttAlertNotSupported
    };

enum rtt_ErrorReturnCodes_msg_ids {
    RttSetupPathInfoFName = 0,
    RttMonUpdatePathName,
    RttPerformEchoFName,
    RttMonGetFirstStoredHopAddressName,
    RttMonUpdateResponseTimeoutName,
    RttMonUpdateResponseVerifyErrorName,
    RttMonUpdateResponseSequenceErrorName,
    RttMonUpdateResponseDroppedName,
    RttMonUpdateResponseOkName,
    RttMonMallocDataSendBuffer,
    RttIpEchoApplicationFName,
    RttMonUpdateResponseBusyName,
    RttMonOpenConnectionFName,
    RttMonSscpOpenName,
    RttMonSetConnectionHandleName,
    RttMonSetHopConnectedStateName,
    RttMonReceiveIndicationFName,
    RttMonSscpReceiveContinue,
    RttMonEchoTargetFName,
    RttMonSscpLuReceive,
    RttMonSscpLuSend,
    RttMonSetupEBCDICDataName,
    RttMonGetConnectionHandleName,
    RttMonSscpLuClose,
    RttMonUpdateResponseDisconnectedName,
    RttMonVerifyEBCDICDataName,
    RttSscpRuEchoApplicationFName,
    RttMonMallocWatchedBooleanName,
    RttMonCreateWatchedBooleanName,
    RttMonMallocDataRespBuffer,
    RttMonMallocDestOctetString,
    RttMonMallocTargetOctetString,
    RttMonMallocaHostName,
    RttMonUpdateResponseNotConnected,
    RttMonMallocMyHostName,
    RttGetMyHostNameFName,
    RttSplitLUTargetAddressFName,
    RttMonMallocPtraHostName,
    RttMonMallocPtraApplName,
    RttMonMallocPtraModeName,
    RttMonMalloceApplName,
    RttMonMalloceModeName,
    RttMonMallocInitSelfBuffer,
    RttMonMallocBindDataBuffer,
    RttMonLuT0orT2Open,
    RttMonMallocDiagTextOS,
    RttMonMallocDiagTextOSOctetPtr,
    RttMonSetDiagTextName,
    RttMonLuReceiveContinue,
    RttSendLogOffFName,
    RttMonLuReceive,
    RttMonLuSend,
    RttMonEndUpdatePathName,
    RttMonLuCloseName,
    RttDependLuApplicationFName,
    RttMonAckBackToHostFName
    };

/*
 *  Library Response Codes
 */
#define D_Return_Successfull                           1
#define D_Invalid_Command                              2
#define D_Invalid_DataPtr                              3
#define D_Invalid_DataSize                             4
#define D_Path_Too_Long                                5
#define D_All_Paths_In_Use                             6
#define D_Bad_Data_Portion                             7
#define D_Hop_Not_Found                                8
#define D_Conn_Handle_List_Invalid                     9
#define D_History_Buckets_Full                         10
#define D_Bad_Sequence_Number                          11
#define D_Conn_Handle_List_Full                        12
#define D_BadOperState                                 13
#define D_MemoryAllocationFailed                       14
#define D_DataFieldIsInvalid                           15
#define D_InvalidParameter                             16
#define D_QueueFull                                    17
#define D_EntryNotFound                                18
#define D_DataNotBeingStored                           19
#define D_OperStateInTransition                        20
#define D_Illegal_Transition                           21

/*
 *  An Additional RowStatus to indicate NOT_IN_SERVICE_AND_CHANGABLE
 *
 *  NOT_IN_SERVICE_AND_CHANGABLE allow all 
 *   required values to be set with good values, 
 *   but still change any value before
 *   attempting to set the conceptual row
 *   to ACTIVE.
 */
#define NOT_IN_SERVICE_AND_CHANGABLE                   99

/*
 *  operTransitionState values
 */
#define D_rttMonCtrlOperState_transition_not_active    90
#define D_rttMonCtrlOperState_transition_to_pending    91
#define D_rttMonCtrlOperState_transition_to_inactive   92

/*
 *  Protocol Validation Request Codes
 */
#define D_Check_RttType                                1
#define D_Check_Address                                2
#define D_Check_Protocol                               3
#define D_Check_Response_Size                          4

/*
 *  Protocol Validation Response Codes
 */
#define D_VALID_RttEntry                               0
#define D_INVALID_RttEntry                             1

/*
 *  setupDataPortion parameter
 */
#define D_rttMonDataPortionCommand_logoff              1
#define D_rttMonDataPortionCommand_echoRequest         2
#define D_rttMonDataPortionCommand_echoResponse        3
#define D_rttMonDataPortionCommand_softwareVersionReq  4
#define D_rttMonDataPortionCommand_softwareVersionResp 5

/*
 *  updateResponse parameter
 */
#define D_rttMonHistoryCollectionSense_ok              1
#define D_rttMonHistoryCollectionSense_disconnected    2
#define D_rttMonHistoryCollectionSense_overthreshold   3
#define D_rttMonHistoryCollectionSense_timeout         4
#define D_rttMonHistoryCollectionSense_busy            5
#define D_rttMonHistoryCollectionSense_notconnected    6
#define D_rttMonHistoryCollectionSense_dropped         7
#define D_rttMonHistoryCollectionSense_sequenceerror   8
#define D_rttMonHistoryCollectionSense_verifyerror     9
#define D_rttMonHistoryCollectionSense_initial_flush   10

/* 
 * The following is to be used as the index to RETVAL
 *   when it is used as a STUB with a return value.
 */
#define RTT_RETVAL_NO_OP                               0

/*==========================================================* 
 *==========================================================*
 * Response Time Library Calls 
 *==========================================================* 
 *==========================================================*/

/*==========================================================*
 * rttMon_get_boot_epoch
 *==========================================================*
 *
 * Description:
 *   This function returns the router boot time as an
 *     epoch structure. 
 */
extern void rttMon_get_boot_epoch SR_PROTOTYPE((clock_epoch *boot_epoch));


/*==========================================================*
 * rttMonConvertAsciiToEbcdic
 *==========================================================*
 *
 * Description:
 *   This function converts an ascii string into an 
 *     ebcdic string of the passed length.  This function
 *     removes the build dependancy on DSPU.
 */
extern void rttMonConvertAsciiToEbcdic SR_PROTOTYPE((const unsigned char *ascii,
						     unsigned char *ebcdic,
						     short length));

/*==========================================================*
 * rttMonIPToOctetString
 *==========================================================*
 *
 * Description:
 *   This function converts an Octet String into 
 *     IP Address.  This function removes the build
 *     dependancy on SNMP.
 */
extern void rttMonIPToOctetString SR_PROTOTYPE((unsigned long
						ipAddr,
						OctetString
						* OSBuff));

/*==========================================================*
 * rttMonFreeOctetStringBuffer
 *==========================================================*
 *
 * Description:
 *   This function frees an Octet String. This function 
 *     removes the build dependancy on SNMP.
 */
extern void rttMonFreeOctetStringBuffer SR_PROTOTYPE((OctetString
						      * tmpAddressBuff));

/*==========================================================*
 * rttMonPreMallocOctetString
 *==========================================================*
 *
 * Description:
 *   This function creates an Octet String with secondary
 *     storage of size length. 
 */
extern OctetString *rttMonPreMallocOctetString SR_PROTOTYPE((long
							     length));

/*==========================================================*
 * rttMonNewAddressBuffer
 *==========================================================*
 *
 * Description:
 *   This function creates an Octet String with secondary
 *     storage of the maximum size of a RTR Address. 
 */
extern OctetString *rttMonNewAddressBuffer SR_PROTOTYPE((void));

/*==========================================================*
 * getRttMonCtrlAdminRttType
 *==========================================================*
 *
 * Description:
 *   This function returns the rttMonCtrlAdminRttType for
 *     this inRttMonCtrlAdminQItem. 
 */
extern long getRttMonCtrlAdminRttType SR_PROTOTYPE((rttMonCtrlAdminQItem
						    * inRttMonCtrlAdminQItem));

/*==========================================================*
 * getRttMonEchoAdminProtocol
 *==========================================================*
 *
 * Description:
 *   This function returns the rttMonEchoAdminProtocol for
 *     this inRttMonCtrlAdminQItem. 
 */
extern long getRttMonEchoAdminProtocol SR_PROTOTYPE((rttMonCtrlAdminQItem
						     * inRttMonCtrlAdminQItem));

/*==========================================================*
 * getRttMonEchoAdminTargetAddress
 *==========================================================*
 *
 * Description:
 *   This function returns the rttMonEchoAdminTargetAddress 
 *     for this inRttMonCtrlAdminQItem. 
 */
extern OctetString *getRttMonEchoAdminTargetAddress SR_PROTOTYPE((rttMonCtrlAdminQItem
								  * inRttMonCtrlAdminQItem,
								  OctetString
								  * echoAdminTargetAddressBuff));

/*==========================================================*
 * getRttMonEchoAdminPktDataRequestSize
 *==========================================================*
 *
 * Description:
 *   This function returns the
 *     rttMonEchoAdminPktDataRequestSize for this 
 *     inRttMonCtrlAdminQItem. 
 */
extern long getRttMonEchoAdminPktDataRequestSize SR_PROTOTYPE((rttMonCtrlAdminQItem
							       * inRttMonCtrlAdminQItem));

/*==========================================================*
 * getRttMonEchoAdminPktDataResponseSize
 *==========================================================*
 *
 * Description:
 *   This function returns the
 *     rttMonEchoAdminPktDataResponseSize for this 
 *     inRttMonCtrlAdminQItem. 
 */
extern long getRttMonEchoAdminPktDataResponseSize SR_PROTOTYPE((rttMonCtrlAdminQItem
								* inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonUpdateResponse
 *==========================================================*
 *
 * Description:
 *   This function performs the table updates and threshold
 *     calcuations for each RTR operation for this 
 *     inRttMonCtrlAdminQItem. 
 *   The following command_code's are accepted:
 *     D_rttMonHistoryCollectionSense_ok
 *     D_rttMonHistoryCollectionSense_overthreshold
 *     D_rttMonHistoryCollectionSense_disconnected
 *     D_rttMonHistoryCollectionSense_timeout
 *     D_rttMonHistoryCollectionSense_busy
 *     D_rttMonHistoryCollectionSense_notconnected
 *     D_rttMonHistoryCollectionSense_dropped
 *     D_rttMonHistoryCollectionSense_sequenceerror
 *     D_rttMonHistoryCollectionSense_verifyerror
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Path_Too_Long
 *     D_Hop_Not_Found;
 *     D_All_Paths_In_Use
 *   When initialAttempt is TRUE no statistics are
 *     saved (all other actions are performed).
 *   This function calls process_may_suspend() in many
 *     locations.
 */
extern long rttMonUpdateResponse SR_PROTOTYPE((rttMonCtrlAdminQItem
					       * inRttMonCtrlAdminQItem,
					       unsigned long response_time,
					       OctetString * rttMonAddress,
					       int command_code,
					       boolean initialAttempt));

/*==========================================================*
 * rttMonCollectPath
 *==========================================================*
 *
 * Description:
 *   This function returns TRUE if path collection should
 *     be performed, else FALSE for this 
 *     inRttMonCtrlAdminQItem. 
 */
extern boolean rttMonCollectPath SR_PROTOTYPE((rttMonCtrlAdminQItem
					       * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonStartUpdatePath
 *==========================================================*
 *
 * Description:
 *   This function works in combination with the following
 *     two functions.  This function set is required to be 
 *     called before every rttMonUpdateResponse call.  It
 *     sets up where in history and statistics to store
 *     the data from the rttMonUpdateResponse call.
 */
extern void rttMonStartUpdatePath SR_PROTOTYPE((rttMonCtrlAdminQItem
						* inRttMonCtrlAdminQItem));


/*==========================================================*
 * rttMonUpdatePath
 *==========================================================*
 *
 * Description:
 *   This function works in combination with the following
 *     two functions.  This function set is required to be 
 *     called before every rttMonUpdateResponse call.  It
 *     sets up where in history and statistics to store
 *     the data from the rttMonUpdateResponse call.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Path_Too_Long
 */
extern long rttMonUpdatePath SR_PROTOTYPE((rttMonCtrlAdminQItem
					   * inRttMonCtrlAdminQItem,
					   OctetString * rttMonAddress));

/*==========================================================*
 * rttMonEndUpdatePath
 *==========================================================*
 *
 * Description:
 *   This function works in combination with the previous
 *     two functions.  This function set is required to be 
 *     called before every rttMonUpdateResponse call.  It
 *     sets up where in history and statistics to store
 *     the data from the rttMonUpdateResponse call.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_All_Paths_In_Use
 *   This function calls process_may_suspend() in many
 *     locations.
 */
extern long rttMonEndUpdatePath SR_PROTOTYPE((rttMonCtrlAdminQItem
					      * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonGetFirstStoredHopAddress
 *==========================================================*
 *
 * Description:
 *   This function works in combination with the previous
 *     UpdatePath functions.  This function fills in the
 *     rttMonAddressBuff with the first hop in the path
 *     as filled in by the first call to rttMonUpdatePath.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Hop_Not_Found -> if you ignored the pathUpdate
 */
extern long rttMonGetFirstStoredHopAddress SR_PROTOTYPE((rttMonCtrlAdminQItem
							 * inRttMonCtrlAdminQItem,
							 OctetString * rttMonAddressBuff));

/*==========================================================*
 * rttMonGetNextStoredHopAddress
 *==========================================================*
 *
 * Description:
 *   This function works in combination with the previous
 *     UpdatePath functions and the previous call to
 *     rttMonGetFirstStoredHopAddress.  This function fills 
 *     in the rttMonAddressBuff with the next hop in the path
 *     as filled in by the next call to rttMonUpdatePath.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Hop_Not_Found -> if you ignored the pathUpdate
 */
extern long rttMonGetNextStoredHopAddress SR_PROTOTYPE((rttMonCtrlAdminQItem
							* inRttMonCtrlAdminQItem,
							OctetString * rttMonAddressBuff));

/*==========================================================*
 * rttMonSetupASCIIDataPortion
 *==========================================================*
 *
 * Description:
 *   This function fills in the data payload of an echo
 *     packet with the ARR protocol data in ascii.
 *   Valid Commands are:
 *     D_rttMonDataPortionCommand_logoff
 *     D_rttMonDataPortionCommand_echoRequest
 *     D_rttMonDataPortionCommand_echoResponse
 *     D_rttMonDataPortionCommand_softwareVersionReq
 *     D_rttMonDataPortionCommand_softwareVersionResp
 *   Seqence number needs to increment for each call and
 *     is in network byte order.
 *   dataSizeofReceivedBuffer is only used on echo response.
 *   dataSizeOfSendBuffer is returned after setting up a
 *     buffer to be sent.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Invalid_DataPtr
 *     D_Invalid_DataSize
 *     D_Invalid_Command
 */
extern long rttMonSetupASCIIDataPortion SR_PROTOTYPE((rttMonCtrlAdminQItem
						      * inRttMonCtrlAdminQItem,
						      void *data,
						      long dataSizeofReceivedBuffer,
						      long *dataSizeOfSendBuffer,
						      unsigned short sequenceNumber,
						      long command));

/*==========================================================*
 * rttMonSetupEBCDICDataPortion
 *==========================================================*
 *
 * Description:
 *   This function fills in the data payload of an echo
 *     packet with the ARR protocol data in ebcdic.
 *   Valid Commands are:
 *     D_rttMonDataPortionCommand_logoff
 *     D_rttMonDataPortionCommand_echoRequest
 *     D_rttMonDataPortionCommand_echoResponse
 *     D_rttMonDataPortionCommand_softwareVersionReq
 *     D_rttMonDataPortionCommand_softwareVersionResp
 *   Seqence number needs to increment for each call and
 *     is in network byte order.
 *   dataSizeofReceivedBuffer is only used on echo response.
 *   dataSizeOfSendBuffer is returned after setting up a
 *     buffer to be sent.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Invalid_DataPtr
 *     D_Invalid_DataSize
 *     D_Invalid_Command
 */
extern long rttMonSetupEBCDICDataPortion SR_PROTOTYPE((rttMonCtrlAdminQItem
						       * inRttMonCtrlAdminQItem,
						       void *data,
						       long dataSizeofReceivedBuffer,
						       long *dataSizeOfSendBuffer,
						       unsigned short sequenceNumber,
						       long command));

/*==========================================================*
 * rttMonVerifyASCIIDataPortion
 *==========================================================*
 *
 * Description:
 *   This function verifies data payload of an echo
 *     packet with the ARR protocol data in ascii.
 *   The sequenceNumber is the expected value in
 *     host byte order.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Bad_Sequence_Number
 *     D_Bad_Data_Portion
 */
extern long rttMonVerifyASCIIDataPortion SR_PROTOTYPE((rttMonCtrlAdminQItem
						       * inRttMonCtrlAdminQItem,
						       unsigned short sequenceNumber,
						       void *data));

/*==========================================================*
 * rttMonVerifyASCIIDataPortion
 *==========================================================*
 *
 * Description:
 *   This function verifies data payload of an echo
 *     packet with the ARR protocol data in ascii.
 *   The sequenceNumber is the expected value in
 *     host byte order.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Bad_Sequence_Number
 *     D_Bad_Data_Portion
 */
extern long rttMonVerifyEBCDICDataPortion SR_PROTOTYPE((rttMonCtrlAdminQItem
							* inRttMonCtrlAdminQItem,
							unsigned short sequenceNumber,
							void *data));

/*==========================================================*
 * getRttMonCtrlAdminFrequency
 *==========================================================*
 *
 * Description:
 *   This function returns the rttMonCtrlAdminFrequency for
 *     this inRttMonCtrlAdminQItem. 
 */
extern long getRttMonCtrlAdminFrequency SR_PROTOTYPE((rttMonCtrlAdminQItem
						      * inRttMonCtrlAdminQItem));

/*==========================================================*
 * getRttMonCtrlAdminTimeout
 *==========================================================*
 *
 * Description:
 *   This function returns the rttMonCtrlAdminTimeout for
 *     this inRttMonCtrlAdminQItem. 
 */
extern long getRttMonCtrlAdminTimeout SR_PROTOTYPE((rttMonCtrlAdminQItem
						    * inRttMonCtrlAdminQItem));

/*==========================================================*
 * getRttMonOperState
 *==========================================================*
 *
 * Description:
 *   This function returns the rttMonOperState for
 *     this inRttMonCtrlAdminQItem.
 *   It returns one of the following:
 *     D_rttMonCtrlOperState_transition_to_pending
 *     D_rttMonCtrlOperState_transition_to_inactive
 *     D_rttMonCtrlOperState_pending
 *     D_rttMonCtrlOperState_inactive
 *     D_rttMonCtrlOperState_active
 */
extern long getRttMonOperState SR_PROTOTYPE((rttMonCtrlAdminQItem
					     * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonTransitionTransitionState
 *==========================================================*
 *
 * Description:
 *   This function returns the new rttMonOperState for
 *     this inRttMonCtrlAdminQItem, after performing
 *     the state transition.  When a probe sees one
 *     of the transition_to_ values in getRttMonOperState
 *     it needs to call this function to perform the 
 *     posted transition, then act according to the 
 *     return value of this call.
 *   This call should be made only for the 
 *       _transition_to_ responses above.
 *   It returns one of the following:
 *     D_rttMonCtrlOperState_pending
 *     D_rttMonCtrlOperState_inactive
 *   If this call was made out of order, and the probe
 *     should not transition, then the current value of
 *     rttMonOperState is returned.
 */
extern long rttMonTransitionTransitionState SR_PROTOTYPE((rttMonCtrlAdminQItem
							  * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonTimeStampEchoStart
 *==========================================================*
 *
 * Description:
 *   This function sets the start time of the echo.  It
 *     needs to be called immediately before the echo
 *     request.
 */
extern void rttMonTimeStampEchoStart SR_PROTOTYPE((rttMonCtrlAdminQItem
						   * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonStartLife
 *==========================================================*
 *
 * Description:
 *   This function needs to be called when the probe becomes
 *     active and starts to collect/capture data.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_BadOperState
 *     D_InvalidParameter
 *   This function calls process_may_suspend() in many
 *     locations.
 */
extern long rttMonStartLife SR_PROTOTYPE((rttMonCtrlAdminQItem
					  * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonUpdateNumberOfEchosAttempted
 *==========================================================*
 *
 * Description:
 *   This function needs to be called every time the
 *     probe initiates a request/response attempt.
 */
extern void rttMonUpdateNumberOfEchosAttempted SR_PROTOTYPE((rttMonCtrlAdminQItem
							     * inRttMonCtrlAdminQItem,
							     boolean
							     initialAttempt));

/*==========================================================*
 * rttMonHopConnected
 *==========================================================*
 *
 * Description:
 *   This function returns TRUE if 
 *     rttMonSetHopConnectedState set this hop state to TRUE,
 *     else FALSE for the same reason.
 */
extern boolean rttMonHopConnected SR_PROTOTYPE((rttMonCtrlAdminQItem
						* inRttMonCtrlAdminQItem,
						OctetString * rttMonAddress));

/*==========================================================*
 * rttMonSetHopConnectedState
 *==========================================================*
 *
 * Description:
 *   This function set the connection state of a connection
 *     to the rttMonAddres to either TRUE or FALSE.
 *   The rttMonSetConnectionHandle needs to be called 
 *     to create a location to start this state.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Hop_Not_Found
 */
extern long rttMonSetHopConnectedState SR_PROTOTYPE((rttMonCtrlAdminQItem
						     * inRttMonCtrlAdminQItem,
						     OctetString * rttMonAddress,
						     boolean state));

/*==========================================================*
 * rttMonSetConnectionHandle
 *==========================================================*
 *
 * Description:
 *   This function is called to hold the connHandle for
 *     later use by the probe.  It libraries keep one
 *     connection handle per rttMonAddress only, so the
 *     probe must reuse connections to a target.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Conn_Handle_List_Full
 */
extern long rttMonSetConnectionHandle SR_PROTOTYPE((rttMonCtrlAdminQItem
						    * inRttMonCtrlAdminQItem,
						    OctetString * rttMonAddress,
						    void *connHandle));

/*==========================================================*
 * rttMonGetConnectionHandle
 *==========================================================*
 *
 * Description:
 *   This function is retrieves the connHandle for
 *     that was previously stored.  The added benifit of
 *     using this call, is to have one connection to a target
 *     that is used by many paths.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Hop_Not_Found
 */
extern long rttMonGetConnectionHandle SR_PROTOTYPE((rttMonCtrlAdminQItem
						    * inRttMonCtrlAdminQItem,
						    OctetString * rttMonAddress,
						    void **connHandle));

/*==========================================================*
 * rttMonFreeConnectionHandleList
 *==========================================================*
 *
 * Description:
 *   This function must be called by a probe, before it
 *     exits.
 */
extern void rttMonFreeConnectionHandleList SR_PROTOTYPE((long rttMonCtrlAdminIndex,
							 connectHandleList
							 * connHandleList));

/*==========================================================*
 * rttMonNewConnectionHandleList
 *==========================================================*
 *
 * Description:
 *   This function creates a connection handle list.  It
 *     return NULL, if the call fails.
 */
extern connectHandleList *rttMonNewConnectionHandleList SR_PROTOTYPE((rttMonCtrlAdminQItem
								      * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonGetConnectionHandleList
 *==========================================================*
 *
 * Description:
 *   This function copies all of the stored connection
 *     handles into this list.  The probe should then 
 *     disconnect each handle and free the handle as
 *     appropriate.  Finally the probe will need to free
 *     the conection handle list.
 */
extern long rttMonGetConnectionHandleList SR_PROTOTYPE((rttMonCtrlAdminQItem
							* inRttMonCtrlAdminQItem,
							connectHandleList
							* connHandleListBuff));

/*==========================================================*
 * rttMonSetDiagText
 *==========================================================*
 *
 * Description:
 *   This function copies the probe specific data stored
 *     in the diagTextOctetString into the libraries
 *     diagText object.
 *   It returns one of the following:
 *     D_Return_Successfull
 *     D_Invalid_DataPtr
 */
extern long rttMonSetDiagText SR_PROTOTYPE((rttMonCtrlAdminQItem
					    * inRttMonCtrlAdminQItem,
					    OctetString
					    * diagTextOctetString));

/*==========================================================*
 * rttMonGetVerifyDataFlag
 *==========================================================*
 *
 * Description:
 *   This function returns TRUE if the customer wants the
 *     payload verified, else FALSE.
 */
extern boolean rttMonGetVerifyDataFlag SR_PROTOTYPE((rttMonCtrlAdminQItem
						     * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonGetPendingBoolean
 *==========================================================*
 *
 * Description:
 *   This function is used by a probe to get the pointer to
 *    the boolean used by the scheduler to wake the probe
 *    after the probe has suspended itself.  The probe
 *    is required to suspend itself when it is told to
 *    transition to pending.
 *
 */
extern watched_boolean *rttMonGetPendingBoolean SR_PROTOTYPE((rttMonCtrlAdminQItem
							      * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonCalculateResponseTime
 *==========================================================*
 *
 * Description:
 *   This function calculates the round trip time of the 
 *     echo.  It needs to be called immediately afer the 
 *     echo ends (with a response or not).
 */
extern unsigned long rttMonCalculateResponseTime SR_PROTOTYPE((rttMonCtrlAdminQItem
							       * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonProbePanic
 *==========================================================*
 *
 * Description:
 *   This function must be called by a probe that is 
 *     killing itself for some internal reason.  This call
 *     tells the rest of RTR that this probe is dead.
 */
extern void rttMonProbePanic SR_PROTOTYPE((rttMonCtrlAdminQItem
					   * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonProbePanicInternal
 *==========================================================*
 *
 * Description:
 *   This function must be called by a probes activation 
 *     registry call when a probe could  not be started
 *     The call tells the rest of RTR that this probe is
 *     dead.
 */
extern void rttMonProbePanicInternal SR_PROTOTYPE((rttMonCtrlAdminQItem
						   * inRttMonCtrlAdminQItem));

/*==========================================================*
 * rttMonSetSelfID
 *==========================================================*
 *
 * Description:
 *   This function is called by a probe before any debug
 *     or error messages are sent.  The value of IDofCaller
 *     must be passed on all error/trace/debug messages.
 */
extern void rttMonSetSelfID SR_PROTOTYPE((long *IDofCaller,
					  rttMonCtrlAdminQItem
					  * inRttMonCtrlAdminQItem));

#endif /* _RTTLIB_H_ */
      
