
rtt_lib.h,7779
#define _RTTLIB_H_66,2249
enum rtt_processHeader_msg_ids rtt_processHeader_msg_ids91,2819
    RttIPPanicMsg 92,2852
    RttEchoTargetCalled,93,2875
    RttWokeOnTimeout,94,2900
    RttWokeOnRealEvent,95,2922
    RttSetupPathInfoCalled,96,2946
    RttErrorReturnCode,97,2974
    RttCallingEndUpdatePath,98,2998
    RttPerformEchoCalled,99,3027
    RttCallingGetFirstStoredHop,100,3053
    RttReturnedFromEchoTarget,101,3086
    RttCallingUpdateResponseTimeout,102,3117
    RttCallingUpdateResponseVerifyError,103,3154
    RttCorruptedReplyToRequest,104,3195
    RttCallingUpdateResponseSequenceError,105,3227
    RttCallingUpdateResponseDropped,106,3270
    RttCallingUpdateResponseOk,107,3307
    RttNativeResponseCode,108,3339
    RttCallingGetNextStoredHop,109,3366
    RttEnteringProcess,110,3398
    RttBadRttType,111,3422
    RttCallingGetOperState,112,3441
    RttWaitingToStart,113,3469
    RttCallingTransitionState,114,3492
    RttTransitionToInactive,115,3523
    RttCallingStartLife,116,3552
    RttCallingUpdateEchoAttempts,117,3577
    RttStartingAnEchoOperation,118,3611
    RttFrequencyOverrun,119,3643
    RttCallingUpdateResponseBusy,120,3668
    RttCallingGetOperStateCheckActive,121,3702
    RttGoingToSleep,122,3741
    RttCallingGetOperStateCheckPending,123,3762
    RttTransitionToPending,124,3802
    RttCallingGetOperStateCheckDeath,125,3830
    RttOperStateNoLongerActive,126,3868
    RttCallingGetOperStateCheckInactive,127,3900
    RttFailedToStartIPEchoApplication,128,3941
    RttDependLuPanicMsg,129,3980
    RttBadTargetAddressLength,130,4005
    RttCloseIndicationCalled,131,4036
    RttSettingReceiveFinishTrue,132,4066
    RttOpenConnectionCalled,133,4099
    RttCallingLuT0orT2Open,134,4128
    RttDumpingLuT0orT2OpenResult,135,4156
    RttCallingSetConnectionHandlePlaceholder,136,4190
    RttCallingSetConnectedStateFalse,137,4236
    RttCallingSetConnectionHandle,138,4274
    RttCallingSetConnectedStateTrue,139,4309
    RttBindDataLengthShorterThanExpected,140,4346
    RttCallingSetDiagText,141,4388
    RttReceiveIndicationCalled,142,4415
    RttBufferReceiveOverflow,143,4447
    RttCallingLUReceiveContinue,144,4477
    RttSendLogoffCalled,145,4510
    RttCallingLuReceive,146,4535
    RttCallingLuSend,147,4560
    RttReceiveFinishedIsFalse,148,4582
    RttCallingUpdatePath,149,4613
    RttCallingLuClose,150,4639
    RttCallingUpdateResponseDisconnected,151,4662
    RttCallingVerifyEBCDICPortion,152,4704
    RttSettingReceiveFinishedFalse,153,4739
    RttCallingUpdateResponseNotConnected,154,4775
    RttSSCPRuPanicMsg,155,4817
    RttCallingSscpLuOpen,156,4840
    RttCallingDumpSscpLuOpenResult,157,4866
    RttCallingSscpLuReceiveContinue,158,4902
    RttCallingSscpLuSend,159,4939
    RttAckBackToHostCalled,160,4965
    RttSendingFirstAck,161,4993
    RttAck1SendFailed,162,5017
    RttSendingSecondAck,163,5040
    RttAck2SendFailed,164,5065
    RttCallingSscpLuClose,165,5088
    RttEchoTestNotGrantedBySscp,166,5115
    RttFailedToStartSscpRuEchoApplication,167,5148
    RttFailedToStartLu0EchoApplication,168,5191
    RttFailedToStartLu2EchoApplication,169,5231
    RttFindingRoute,170,5271
    RttResolutionNotSupported,171,5292
    RttAlertNotSupported172,5323
enum rtt_ErrorReturnCodes_msg_ids rtt_ErrorReturnCodes_msg_ids175,5356
    RttSetupPathInfoFName 176,5392
    RttMonUpdatePathName,177,5423
    RttPerformEchoFName,178,5449
    RttMonGetFirstStoredHopAddressName,179,5474
    RttMonUpdateResponseTimeoutName,180,5514
    RttMonUpdateResponseVerifyErrorName,181,5551
    RttMonUpdateResponseSequenceErrorName,182,5592
    RttMonUpdateResponseDroppedName,183,5635
    RttMonUpdateResponseOkName,184,5672
    RttMonMallocDataSendBuffer,185,5704
    RttIpEchoApplicationFName,186,5736
    RttMonUpdateResponseBusyName,187,5767
    RttMonOpenConnectionFName,188,5801
    RttMonSscpOpenName,189,5832
    RttMonSetConnectionHandleName,190,5856
    RttMonSetHopConnectedStateName,191,5891
    RttMonReceiveIndicationFName,192,5927
    RttMonSscpReceiveContinue,193,5961
    RttMonEchoTargetFName,194,5992
    RttMonSscpLuReceive,195,6019
    RttMonSscpLuSend,196,6044
    RttMonSetupEBCDICDataName,197,6066
    RttMonGetConnectionHandleName,198,6097
    RttMonSscpLuClose,199,6132
    RttMonUpdateResponseDisconnectedName,200,6155
    RttMonVerifyEBCDICDataName,201,6197
    RttSscpRuEchoApplicationFName,202,6229
    RttMonMallocWatchedBooleanName,203,6264
    RttMonCreateWatchedBooleanName,204,6300
    RttMonMallocDataRespBuffer,205,6336
    RttMonMallocDestOctetString,206,6368
    RttMonMallocTargetOctetString,207,6401
    RttMonMallocaHostName,208,6436
    RttMonUpdateResponseNotConnected,209,6463
    RttMonMallocMyHostName,210,6501
    RttGetMyHostNameFName,211,6529
    RttSplitLUTargetAddressFName,212,6556
    RttMonMallocPtraHostName,213,6590
    RttMonMallocPtraApplName,214,6620
    RttMonMallocPtraModeName,215,6650
    RttMonMalloceApplName,216,6680
    RttMonMalloceModeName,217,6707
    RttMonMallocInitSelfBuffer,218,6734
    RttMonMallocBindDataBuffer,219,6766
    RttMonLuT0orT2Open,220,6798
    RttMonMallocDiagTextOS,221,6822
    RttMonMallocDiagTextOSOctetPtr,222,6850
    RttMonSetDiagTextName,223,6886
    RttMonLuReceiveContinue,224,6913
    RttSendLogOffFName,225,6942
    RttMonLuReceive,226,6966
    RttMonLuSend,227,6987
    RttMonEndUpdatePathName,228,7005
    RttMonLuCloseName,229,7034
    RttDependLuApplicationFName,230,7057
    RttMonAckBackToHostFName231,7090
#define D_Return_Successfull 237,7161
#define D_Invalid_Command 238,7218
#define D_Invalid_DataPtr 239,7275
#define D_Invalid_DataSize 240,7332
#define D_Path_Too_Long 241,7389
#define D_All_Paths_In_Use 242,7446
#define D_Bad_Data_Portion 243,7503
#define D_Hop_Not_Found 244,7560
#define D_Conn_Handle_List_Invalid 245,7617
#define D_History_Buckets_Full 246,7674
#define D_Bad_Sequence_Number 247,7732
#define D_Conn_Handle_List_Full 248,7790
#define D_BadOperState 249,7848
#define D_MemoryAllocationFailed 250,7906
#define D_DataFieldIsInvalid 251,7964
#define D_InvalidParameter 252,8022
#define D_QueueFull 253,8080
#define D_EntryNotFound 254,8138
#define D_DataNotBeingStored 255,8196
#define D_OperStateInTransition 256,8254
#define D_Illegal_Transition 257,8312
#define NOT_IN_SERVICE_AND_CHANGABLE 268,8641
#define D_rttMonCtrlOperState_transition_not_active 273,8738
#define D_rttMonCtrlOperState_transition_to_pending 274,8796
#define D_rttMonCtrlOperState_transition_to_inactive 275,8854
#define D_Check_RttType 280,8958
#define D_Check_Address 281,9015
#define D_Check_Protocol 282,9072
#define D_Check_Response_Size 283,9129
#define D_VALID_RttEntry 288,9233
#define D_INVALID_RttEntry 289,9290
#define D_rttMonDataPortionCommand_logoff 294,9386
#define D_rttMonDataPortionCommand_echoRequest 295,9443
#define D_rttMonDataPortionCommand_echoResponse 296,9500
#define D_rttMonDataPortionCommand_softwareVersionReq 297,9557
#define D_rttMonDataPortionCommand_softwareVersionResp 298,9614
#define D_rttMonHistoryCollectionSense_ok 303,9708
#define D_rttMonHistoryCollectionSense_disconnected 304,9765
#define D_rttMonHistoryCollectionSense_overthreshold 305,9822
#define D_rttMonHistoryCollectionSense_timeout 306,9879
#define D_rttMonHistoryCollectionSense_busy 307,9936
#define D_rttMonHistoryCollectionSense_notconnected 308,9993
#define D_rttMonHistoryCollectionSense_dropped 309,10050
#define D_rttMonHistoryCollectionSense_sequenceerror 310,10107
#define D_rttMonHistoryCollectionSense_verifyerror 311,10164
#define D_rttMonHistoryCollectionSense_initial_flush 312,10221
#define RTT_RETVAL_NO_OP 318,10394
