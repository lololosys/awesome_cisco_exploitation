
clsi_msg.h,14766
#define __CLSI_MSG_H__136,4842
} CLSDLCType_e;CLSDLCType_e163,5424
#define CLSI_VERSION 169,5479
typedef ReqOpnStnLLCParms_t ReqOpnStnNCIAParms_t;ReqOpnStnNCIAParms_t183,5813
typedef ReqOpnStnLLCParms_t ReqOpnStnSDLCParms_t;ReqOpnStnSDLCParms_t184,5863
typedef ReqOpnStnLLCParms_t ReqOpnStnVDLCParms_t;ReqOpnStnVDLCParms_t185,5932
} CLSIdentifierType_e;CLSIdentifierType_e204,6317
#define CLS_NO_CEP 210,6462
#define TR_MAX_RIF_LENGTH 213,6488
} CLSSelectorCode_e;CLSSelectorCode_e222,6604
#define CLS_COMMAND_MASK 228,6827
#define CLS_TYPE_MASK 229,6905
} CLSPrimCodeType_e;CLSPrimCodeType_e241,7273
} CLSPrimCodeCommandType_e;CLSPrimCodeCommandType_e276,8075
#define DLC_CONGESTION_INCR 281,8150
#define DLC_CONGESTION_LOW 282,8218
#define DLC_CONGESTION_MEDIUM 283,8254
#define DLC_CONGESTION_HIGH 284,8290
#define DLC_CONGESTION_MAX 285,8326
#define CLS_MOD_MAXOUT	291,8458
#define CLS_MOD_MAXIFIELD	292,8512
#define CLS_MOD_T1	293,8571
#define CLS_MOD_MAXRETRY	294,8624
#define CLS_MOD_OPENBUSY 295,8682
#define CLS_MOD_QUEUING	297,8780
#define CLS_ACT_SAP_REQ_HDR_SZ 304,9010
#define CLS_ACT_SAP_CNF_HDR_SZ 305,9070
#define CLS_DEACT_SAP_REQ_HDR_SZ 306,9130
#define CLS_DEACT_SAP_CNF_HDR_SZ 307,9192
#define CLS_ENABLE_REQ_HDR_SZ 308,9254
#define CLS_ENABLE_CNF_HDR_SZ 309,9314
#define CLS_DISABLE_REQ_HDR_SZ 310,9374
#define CLS_DISABLE_CNF_HDR_SZ 311,9435
#define CLS_FLOW_REQ_HDR_SZ 312,9496
#define CLS_FLOW_IND_HDR_SZ 313,9554
#define CLS_OPEN_STN_REQ_HDR_SZ 314,9612
#define CLS_OPEN_STN_CNF_HDR_SZ 315,9673
#define CLS_REQ_OPNSTN_REQ_HDR_SZ 316,9734
#define CLS_REQ_OPNSTN_CNF_HDR_SZ 317,9797
#define CLS_CLOSE_STN_REQ_HDR_SZ 318,9860
#define CLS_CLOSE_STN_CNF_HDR_SZ 319,9922
#define CLS_CONNECT_REQ_HDR_SZ 320,9984
#define CLS_CONNECT_CNF_HDR_SZ 321,10045
#define CLS_CONNECT_IND_HDR_SZ 322,10106
#define CLS_CONNECT_RSP_HDR_SZ 323,10167
#define CLS_CONNECTED_IND_HDR_SZ 324,10228
#define CLS_DISCONNECT_REQ_HDR_SZ 325,10291
#define CLS_DISCONNECT_CNF_HDR_SZ 326,10355
#define CLS_DISCONNECT_IND_HDR_SZ 327,10419
#define CLS_DISCONNECT_RSP_HDR_SZ 328,10483
#define CLS_ID_REQ_HDR_SZ 329,10547
#define CLS_ID_CNF_HDR_SZ 330,10603
#define CLS_ID_IND_HDR_SZ 331,10659
#define CLS_ID_RSP_HDR_SZ 332,10715
#define CLS_ID_STN_IND_HDR_SZ 333,10771
#define CLS_ID_STN_RSP_HDR_SZ 334,10830
#define CLS_SIGNAL_STN_REQ_HDR_SZ 335,10889
#define CLS_SIGNAL_STN_CNF_HDR_SZ 336,10952
#define CLS_DATA_REQ_HDR_SZ 337,11015
#define CLS_DATA_CNF_HDR_SZ 338,11073
#define CLS_DATA_IND_HDR_SZ 339,11131
#define CLS_ACT_RING_REQ_HDR_SZ 341,11190
#define CLS_ACT_RING_CNF_HDR_SZ 342,11250
#define CLS_DEACT_RING_REQ_HDR_SZ 343,11310
#define CLS_DEACT_RING_CNF_HDR_SZ 344,11372
#define CLS_ID_STN_REQ_HDR_SZ 345,11434
#define CLS_ID_STN_CNF_HDR_SZ 346,11493
#define CLS_UDATA_REQ_HDR_SZ 347,11552
#define CLS_UDATA_IND_HDR_SZ 348,11611
#define CLS_UDATA_CNF_HDR_SZ 349,11670
#define CLS_TEST_REQ_HDR_SZ 350,11729
#define CLS_TEST_RSP_HDR_SZ 351,11787
#define CLS_TEST_IND_HDR_SZ 352,11845
#define CLS_TEST_CNF_HDR_SZ 353,11903
#define CLS_TEST_STN_REQ_HDR_SZ 354,11961
#define CLS_TEST_STN_RSP_HDR_SZ 355,12022
#define CLS_TEST_STN_IND_HDR_SZ 356,12083
#define CLS_TEST_STN_CNF_HDR_SZ 357,12144
#define CLS_UDATA_STN_REQ_HDR_SZ 358,12205
#define CLS_UDATA_STN_IND_HDR_SZ 359,12267
#define CLS_UDATA_STN_CNF_HDR_SZ 360,12329
#define CLS_CONNECT_STN_IND_HDR_SZ 361,12391
#define CLS_PONG_REQ_HDR_SZ 362,12455
#define CLS_PONG_CNF_HDR_SZ 363,12513
#define CLS_MODIFY_STN_REQ_HDR_SZ 364,12571
#define CLS_MODIFY_STN_CNF_HDR_SZ 365,12634
} ClsErrorCodeT;ClsErrorCodeT401,13575
#define CLS_PRIVATE_SIZE 408,13759
#define CLSI_HEADER 410,13835
struct TCLSIMsg_TCLSIMsg_424,14579
} CLSPortIdentifierType;CLSPortIdentifierType435,14851
} CLSPortID;CLSPortID442,14972
} Status_t;Status_t463,15748
} TCLSICnfMsg;TCLSICnfMsg473,16035
} EnableReqh_t;EnableReqh_t483,16272
} EnableReq_t;EnableReq_t496,16809
} EnableTRParms_t;EnableTRParms_t510,17229
} EnableSDLCParms_t;EnableSDLCParms_t519,17369
} EnableQLLCParms_t;EnableQLLCParms_t528,17511
#define CLS_RSRB_DEFAULT_BRIDGE_NUM 540,17726
#define CLS_RSRB_ENABLE_LACK_ON 545,17808
} EnableRSRBParms_t;EnableRSRBParms_t556,18330
} EnableVDLCParms_t;EnableVDLCParms_t565,18565
} EnableNCIAParms_t;EnableNCIAParms_t575,18994
} EnableTBRIDGEParms_t;EnableTBRIDGEParms_t612,20555
} EnableFRParms_t;EnableFRParms_t618,20721
} EnableCnfh_t;EnableCnfh_t628,20992
} EnableCnf_t;EnableCnf_t640,21527
} DisableReqh_t;DisableReqh_t650,21765
} DisableReq_t;DisableReq_t664,22310
} DisableCnfh_t;DisableCnfh_t673,22578
} DisableCnf_t;DisableCnf_t685,23122
} ActSapReqh_t;ActSapReqh_t698,23524
} ActSapReq_t;ActSapReq_t712,24116
#define CLS_TEST_RESPONSE 725,24462
#define CLS_IEEE_XID_RESPONSE 726,24538
#define CLS_XID_RESPONSE 727,24617
#define CLS_TEST_NOTIFY 733,24826
#define CLS_IEEE_XID_NOTIFY 734,24902
#define CLS_XID_NOTIFY 735,24982
} ActSapLLCParms_t;ActSapLLCParms_t764,26364
} ActSapSDLCParms_t;ActSapSDLCParms_t770,26527
} ActSapVDLCParms_t;ActSapVDLCParms_t779,26679
} ActSapNCIAParms_t;ActSapNCIAParms_t785,26764
#define	QLLC_PVC	797,27050
#define	QLLC_VMACS	798,27074
#define	QLLC_PARTNER	799,27100
#define QLLC_PROXYXID	800,27128
#define QLLC_TR_SAPS	801,27157
#define QLLC_REMOTE	802,27185
#define QLLC_TIMERS	803,27212
#define QLLC_NPSI_POLL	804,27239
#define QLLC_DLSW	805,27269
struct QSapPVCRangeT_ QSapPVCRangeT_807,27317
struct QTimersT_ QTimersT_812,27383
struct QSapT_ QSapT_827,27712
} ActSapQLLCParms_t;ActSapQLLCParms_t866,28799
} ActSapCnfh_t;ActSapCnfh_t880,29248
} ActSapCnf_t;ActSapCnf_t895,29953
} DeactSapReqh_t;DeactSapReqh_t909,30372
} DeactSapReq_t;DeactSapReq_t919,30785
} DeactSapCnfh_t;DeactSapCnfh_t932,31168
} DeactSapCnf_t;DeactSapCnf_t941,31524
#define CLS_ACTION_STOP_FLOW 953,31828
#define CLS_ACTION_RESUME_FLOW 954,31870
} FlowReqh_t;FlowReqh_t956,31985
} FlowReq_t;FlowReq_t965,32339
} FlowIndh_t;FlowIndh_t977,32713
} FlowInd_t;FlowInd_t986,33067
} OpenStnReqh_t;OpenStnReqh_t999,33482
} OpenStnReq_t;OpenStnReq_t1008,33841
} OpenStnCnfh_t;OpenStnCnfh_t1021,34293
} OpenStnCnf_t;OpenStnCnf_t1030,34652
} ReqOpnStnReqh_t;ReqOpnStnReqh_t1044,35141
} CLSIQueuingType;CLSIQueuingType1052,35272
struct ReqOpnStnReq_t_ReqOpnStnReq_t_1054,35292
struct ReqOpnStnLLCParms_t_ReqOpnStnLLCParms_t_1073,36116
#define CLS_L3PID_FID4 1107,37843
#define CLS_L3PID_FID2 1108,37897
#define CLS_L3PID_APPN 1109,37954
#define CLS_L3PID_NETBIOS 1110,38005
#define CLS_BAN_FLAG 1111,38051
#define EO_RIF_SPECIFIED 1125,38707
#define EO_VALIDATE_RIF 1131,38935
#define EO_EXPLORER_ARE 1142,39328
#define EO_EXPLORER_TEST 1146,39491
#define EO_TRANSPARENT_FIRST 1148,39595
#define EO_LOCAL_LAN 1153,39825
#define EO_OUTBOUND_DEFAULT 1169,40439
#define EO_INBOUND_DEFAULT 1172,40559
} ReqOpnStnQLLCParms_t;ReqOpnStnQLLCParms_t1251,43763
} ReqOpnStnCnfh_t;ReqOpnStnCnfh_t1264,44225
} ReqOpnStnCnf_t;ReqOpnStnCnf_t1273,44587
#define CLS_ACTION_NORMAL 1287,45009
#define CLS_ACTION_FORCED 1288,45051
} CloseStnReqh_t;CloseStnReqh_t1290,45166
} CloseStnReq_t;CloseStnReq_t1299,45527
} CloseStnCnfh_t;CloseStnCnfh_t1311,45913
} CloseStnCnf_t;CloseStnCnf_t1320,46274
} ConnectReqh_t;ConnectReqh_t1333,46684
} ConnectReq_t;ConnectReq_t1345,47247
} ConnectCnfh_t;ConnectCnfh_t1357,47630
} ConnectCnf_t;ConnectCnf_t1368,48048
} ConnectIndh_t;ConnectIndh_t1380,48382
} ConnectInd_t;ConnectInd_t1391,48799
#define CLS_ACTION_CONNECT 1403,49110
#define CLS_ACTION_NO_CONNECT 1404,49152
} ConnectRsph_t;ConnectRsph_t1407,49341
} ConnectRsp_t;ConnectRsp_t1421,50036
} ConnectedIndh_t;ConnectedIndh_t1433,50422
} ConnectedInd_t;ConnectedInd_t1443,50865
} DisconnectReqh_t;DisconnectReqh_t1455,51218
} DisconnectReq_t;DisconnectReq_t1464,51582
} DisconnectCnfh_t;DisconnectCnfh_t1476,51971
} DisconnectCnf_t;DisconnectCnf_t1485,52335
#define CLS_NORMAL_DISC 1497,52652
#define CLS_ERROR_DISC 1498,52702
#define CLS_TIMEOUT_DISC 1499,52744
} DisconnectIndh_t;DisconnectIndh_t1501,52859
} DisconnectInd_t;DisconnectInd_t1510,53223
#define CLS_ACTION_ACCEPT 1522,53540
#define CLS_ACTION_REJECT 1523,53596
} DisconnectRsph_t;DisconnectRsph_t1525,53676
} DisconnectRsp_t;DisconnectRsp_t1534,54040
} IdReqh_t;IdReqh_t1546,54388
struct IdReq_t_IdReq_t_1548,54401
} IdStnReqh_t;IdStnReqh_t1573,55557
}IdStnReq_t;IdStnReq_t1595,56789
} IdCnfh_t;IdCnfh_t1607,57163
} IdCnf_t;IdCnf_t1621,57851
} IdStnCnfh_t;IdStnCnfh_t1633,58290
} IdStnCnf_t;IdStnCnf_t1648,59151
} IdIndh_t;IdIndh_t1660,59494
} IdInd_t;IdInd_t1676,60339
} IdStnIndh_t;IdStnIndh_t1689,60776
#define CLS_XID_CMD 1699,61210
#define CLS_XID_RSP 1700,61235
#define CLS_XID_POLL_ON 1704,61421
#define CLS_XID_POLL_OFF 1705,61451
} IdStnInd_t;IdStnInd_t1733,63406
} IdRsph_t;IdRsph_t1745,63749
struct IdRsp_t_IdRsp_t_1747,63762
} IdStnRsph_t;IdStnRsph_t1771,64866
}IdStnRsp_t;IdStnRsp_t1792,66059
#define CLS_ACTION_START 1838,67475
#define CLS_ACTION_STOP_ORDERLY 1839,67529
#define CLS_ACTION_STOP_IMMEDIATE 1840,67590
} SignalStnReqh_t;SignalStnReqh_t1842,67677
} SignalStnReq_t;SignalStnReq_t1851,68039
} SignalStnCnfh_t;SignalStnCnfh_t1863,68375
} SignalStnCnf_t;SignalStnCnf_t1872,68737
} DataReqh_t;DataReqh_t1884,69126
struct DataReq_t_DataReq_t_1886,69141
} DataCnfh_t;DataCnfh_t1918,70436
} DataCnf_t;DataCnf_t1932,71064
} DataIndh_t;DataIndh_t1944,71448
struct DataInd_t_DataInd_t_1946,71463
  } PongReqh_t;PongReqh_t1975,72535
PongReq_t;PongReq_t1983,72743
} PongCnfh_t;PongCnfh_t1994,73051
} PongCnf_t;PongCnf_t2001,73221
static inline TCLSIMsg*  CLSIMsgCreate(2020,73602
static inline TCLSIMsg* CLSIMsgGetLink 2148,78638
static inline void CLSIMsgSetLink 2153,78736
static inline hword CLSIMsgGetPrimitiveType 2158,78845
static inline hword CLSIMsgGetPrimitiveCommand 2163,78978
static inline hword CLSIMsgGetPrimitiveCode 2168,79118
static inline void CLSIMsgSetPrimitiveType 2173,79235
static inline boolean	CLSIMsgIsRequest 2179,79396
static inline boolean	CLSIMsgIsConfirm 2187,79559
static inline boolean	CLSIMsgIsIndication 2195,79722
static inline boolean	CLSIMsgIsResponse 2203,79891
static inline void CLSIMsgSetReturnCode 2211,80056
static inline word CLSIMsgGetReturnCode 2216,80190
static inline void CLSIMsgSetDestination(2221,80310
static inline CLSIdentifierType_e CLSIMsgGetIdentifierType(2229,80489
static inline word CLSIMsgGetIdentifier(2234,80610
static inline byte* CLSIIdStnGetLocalMac(2245,80881
static inline byte* CLSIIdStnGetRoute(2263,81370
static inline byte* CLSIIdStnGetXID(2282,81860
} ActRngReqh_t;ActRngReqh_t2307,82430
} ActRngReq_t;ActRngReq_t2323,83215
} ActRngTRParms_t;ActRngTRParms_t2345,84452
} ActRngLLCParms_t;ActRngLLCParms_t2371,85907
typedef ActRngLLCParms_t  ActRngSDLCParms_t;ActRngSDLCParms_t2373,85928
typedef ActSapQLLCParms_t ActRngQLLCParms_t;ActRngQLLCParms_t2374,85973
typedef ActSapVDLCParms_t ActRngVDLCParms_t;ActRngVDLCParms_t2375,86018
typedef ActSapNCIAParms_t ActRngNCIAParms_t;ActRngNCIAParms_t2376,86063
} ActRngCnfh_t;ActRngCnfh_t2391,86590
} ActRngCnf_t;ActRngCnf_t2405,87212
} DeActRngReqh_t;DeActRngReqh_t2421,87556
} DeActRngReq_t;DeActRngReq_t2429,87779
} DeActRngTRParms_t;DeActRngTRParms_t2456,89255
} DeActRngCnfh_t;DeActRngCnfh_t2472,89773
} DeActRngCnf_t;DeActRngCnf_t2481,90003
} TestParms_t;TestParms_t2493,90618
} TestReqh_t;TestReqh_t2504,90903
} TestReq_t;TestReq_t2515,91373
} TestStnReqh_t;TestStnReqh_t2526,91658
} TestStnReq_t;TestStnReq_t2546,92660
} TestRsph_t;TestRsph_t2557,92945
} TestRsp_t;TestRsp_t2575,93851
} TestStnRsph_t;TestStnRsph_t2585,94135
} TestStnRsp_t;TestStnRsp_t2603,95040
} TestIndh_t;TestIndh_t2615,95351
} TestInd_t;TestInd_t2627,95860
} TestStnIndh_t;TestStnIndh_t2637,96060
#define CLS_TEST_CMD 2648,96370
#define CLS_TEST_RSP 2649,96396
#define CLS_TEST_POLL_ON 2653,96584
#define CLS_TEST_POLL_OFF 2654,96615
} TestStnInd_t;TestStnInd_t2679,98253
} TestStnParms_t;TestStnParms_t2688,98831
} TestCnfh_t;TestCnfh_t2699,99113
} TestCnf_t;TestCnf_t2711,99507
} TestStnCnfh_t;TestStnCnfh_t2721,99708
#define CLS_TEST_CMD 2731,100017
#define CLS_TEST_RSP 2732,100043
#define CLS_TEST_POLL_ON 2736,100231
#define CLS_TEST_POLL_OFF 2737,100262
} TestStnCnf_t;TestStnCnf_t2762,101820
} UDataReqh_t;UDataReqh_t2789,103382
} UDataReq_t;UDataReq_t2802,103935
} UDataStnReqh_t;UDataStnReqh_t2830,105516
} UDataStnReq_t;UDataStnReq_t2852,106591
} UDataIndh_t;UDataIndh_t2870,107542
} UDataInd_t;UDataInd_t2885,108278
} UDataStnIndh_t;UDataStnIndh_t2895,108480
} UDataStnInd_t;UDataStnInd_t2928,110401
} UDataCnfh_t;UDataCnfh_t2950,111501
} UDataCnf_t;UDataCnf_t2962,112055
} UDataStnCnfh_t;UDataStnCnfh_t2984,113154
} UDataStnCnf_t;UDataStnCnf_t2996,113714
} ConnectStnIndh_t;ConnectStnIndh_t3006,113921
#define CLS_CONNECT_POLL_ON 3016,114236
#define CLS_CONNECT_POLL_OFF 3017,114270
} ConnectStnInd_t;ConnectStnInd_t3043,116070
} ModifyStnReqh_t;ModifyStnReqh_t3057,116528
} ModifyStnReq_t;ModifyStnReq_t3069,117095
} ModifyStnCnfh_t;ModifyStnCnfh_t3081,117483
} ModifyStnCnf_t;ModifyStnCnf_t3090,117846
static inline byte* CLSIDataIndGetData(3093,117866
static inline byte* CLSIIdIndGetData(3102,118164
static inline int CLSIMsgGetNextWindow(3110,118428
static inline byte* CLSIConnectStnGetLocalMac(3121,118695
static inline boolean QTestPvc 3148,119427
static inline void QSetPvc 3150,119510
static inline void QClearPvc 3152,119581
static inline boolean QTestVmac 3157,119669
static inline void QSetVmac 3159,119755
static inline void QClearVmac 3161,119829
static inline boolean QTestPartner 3166,119923
static inline void QSetPartner 3168,120014
static inline void QClearPartner 3170,120093
static inline boolean QTestProxyXid 3175,120194
static inline void QSetProxyXid 3177,120287
static inline void QClearProxyXid 3179,120368
static inline boolean QTestTRSaps 3184,120477
static inline void QSetTRSaps 3186,120567
static inline void QClearTRSaps 3188,120645
static inline boolean QTestRemote 3193,120749
static inline void QSetRemote 3195,120838
static inline void QClearRemote 3197,120915
static inline boolean QTestTimers 3202,121010
static inline void QSetTimers 3204,121099
static inline void QClearTimers 3206,121176
static inline boolean QTestNpsiPoll 3210,121273
static inline void QSetNpsiPoll 3212,121367
static inline void QClearNpsiPoll 3214,121449
static inline boolean QTestDlsw 3219,121547
static inline void QClearAllFlags 3224,121650
