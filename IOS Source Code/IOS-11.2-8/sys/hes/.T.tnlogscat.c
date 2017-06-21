
tnlogscat.c,7627
#define min(min46,1237
typedef struct _tnuint1_ _tnuint1_48,1284
typedef struct _tnuint1_ { unsigned char num; } _tnuint1;_tnuint148,1284
typedef struct _tnuint2_ _tnuint2_49,1342
typedef struct _tnuint2_ { unsigned short PACKED(num); } _tnuint2;_tnuint249,1342
typedef struct _tnbiguint1_ _tnbiguint1_50,1409
typedef struct _tnbiguint1_ { unsigned char num; } _tnbiguint1;_tnbiguint150,1409
typedef struct _tnbiguint2_ _tnbiguint2_51,1473
typedef struct _tnbiguint2_ { unsigned short PACKED(num); } _tnbiguint2;_tnbiguint251,1473
typedef struct _tnuint4_ _tnuint4_52,1546
typedef struct _tnuint4_ { unsigned long PACKED(num); } _tnuint4;_tnuint452,1546
typedef struct _tnbiguint4_ _tnbiguint4_53,1612
typedef struct _tnbiguint4_ { unsigned long PACKED(num); } _tnbiguint4;_tnbiguint453,1612
typedef struct _tnptr_ _tnptr_54,1684
typedef struct _tnptr_ { unsigned short PACKED(pktLength); unsigned short PACKED(len); unsigned char ptr[1]; } _tnptr;_tnptr54,1684
typedef struct _tnint1_ _tnint1_55,1803
typedef struct _tnint1_ { char num; } _tnint1;_tnint155,1803
typedef struct _tnint2_ _tnint2_56,1850
typedef struct _tnint2_ { short PACKED(num); } _tnint2;_tnint256,1850
typedef struct _tnint4_ _tnint4_57,1906
typedef struct _tnint4_ { long PACKED(num); } _tnint4;_tnint457,1906
static void tn_scatter_adapter 59,1962
static void tn_scatter_cpNameLcl 66,2069
static void tn_scatter_cpNameRem 73,2178
static void tn_scatter_idBlkNum 80,2287
static void tn_scatter_ipAddrLcl 87,2412
static void tn_scatter_snaSense 94,2528
static void tn_scatter_systemStateTransition 101,2650
static void tn_scatter_tcpPortLcl 108,2778
static void tn_scatter_tgNumber 115,2906
#define tn_max_entrynum 125,3058
struct copy_fld_def copy_fld_def126,3086
typedef struct TnScatBind TnScatBind131,3172
} TnScatBind;TnScatBind134,3241
typedef struct TnScatBindEntry TnScatBindEntry136,3256
} TnScatBindEntry;TnScatBindEntry139,3357
typedef struct TnFixedFields TnFixedFields143,3447
} TnFixedFields;TnFixedFields147,3581
enum TnLogFldNames TnLogFldNames149,3599
  TLFID_entryName 150,3620
  TLFID_timeTicks 151,3696
  TLFID_subTicks 152,3772
  TLFID_puNameLclAdmin 153,3848
  TLFID_luNameSna 154,3924
  TLFID_tn3270eDeviceType 155,4000
  TLFID_tn3270eDeviceName 156,4076
  TLFID_tn3270eSubErr 157,4152
  TLFID_ipAddrLcl 158,4228
  TLFID_ipAddrClient 159,4304
  TLFID_tcpPortLcl 160,4380
  TLFID_tcpPortClient 161,4456
  TLFID_msgLength 162,4532
  TLFID_msgFragment 163,4608
  TLFID_puIndex 164,4684
  TLFID_luLocaddr 165,4760
  TLFID_logRingNbr 166,4836
  TLFID_logEntrySize 167,4912
  TLFID_logNumEntries 168,4988
  TLFID_lucOldState 169,5064
  TLFID_lucEvent 170,5140
  TLFID_lucNewState 171,5216
  TLFID_connectReasonCode 172,5292
  TLFID_ruDump 173,5368
  TLFID_connectSpecificP 174,5444
  TLFID_ipPoolNumActivePus 175,5520
  TLFID_systemStateTransition 176,5596
  TLFID_tnetIndex 177,5675
  TLFID_puSnmpState 178,5751
  TLFID_listenPointStateExtern 179,5827
  TLFID_luType 180,5907
  TLFID_snaMaxRuIn 181,5983
  TLFID_snaMaxRuOut 182,6059
  TLFID_snaPaceIn 183,6135
  TLFID_snaPaceOut 184,6211
  TLFID_snaChainIn 185,6287
  TLFID_snaRspMode 186,6363
  TLFID_puOldState 187,6439
  TLFID_puNewState 188,6515
  TLFID_puEvent 189,6591
  TLFID_snaSense 190,6667
  TLFID_cpNameLcl 191,6743
  TLFID_cpNameRem 192,6819
  TLFID_adapter 193,6895
  TLFID_tgNumber 194,6971
  TLFID_idBlkNum 195,7047
  TLFID_luSnaState 196,7123
  TLFID_luStateFlags 197,7199
  TLFID_linkName 198,7275
  TLFID_oldState 199,7351
  TLFID_event 200,7427
  TLFID_linkState 201,7503
  TLFID_maxData 202,7579
  TLFID_sense 203,7655
  TLFID_offset 204,7731
  TLFID_count 205,7807
  TLFID_lSap 206,7883
  TLFID_adapType 207,7959
  TLFID_adapNo 208,8035
  TLFID_length 209,8111
  TLFID_unbindType 210,8187
  TLFID_luName 211,8263
  TLFID_netName 212,8339
  TLFID_modeName 213,8415
  TLFID_peerName 214,8491
  TLFID_tgNum 215,8567
  TLFID_luDiscReason 216,8643
enum TnLogEntryNames TnLogEntryNames219,8722
  TLEID_LogGrowth 220,8745
  TLEID_SystemUp 221,8821
  TLEID_PuFsm 222,8897
  TLEID_PuExternalState 223,8973
  TLEID_PuSnaBadMsg 224,9049
  TLEID_LuPacingAddCredits 225,9125
  TLEID_LuPacingFlush 226,9201
  TLEID_LuSendPsid 227,9277
  TLEID_LuFsm 228,9353
  TLEID_BindParm 229,9429
  TLEID_LuTnetRcvInvalidData 230,9505
  TLEID_BadBind 231,9583
  TLEID_BadTnetConnect 232,9659
  TLEID_NoLuSessions 233,9735
  TLEID_Connected 234,9811
  TLEID_LuEnterSpec 235,9887
  TLEID_LuBadRequest 236,9963
  TLEID_LuBadResponse 237,10039
  TLEID_PacketLuIn 238,10115
  TLEID_PacketLuOut 239,10191
  TLEID_PacketPuIn 240,10267
  TLEID_PacketPuOut 241,10343
  TLEID_PacketTnetIn 242,10419
  TLEID_PacketTnetOut 243,10495
  TLEID_TnetState 244,10571
  TLEID_NoLuMemory 245,10647
  TLEID_SystemDown 246,10723
  TLEID_APPNProtErr 247,10799
  TLEID_DLUSIsnt 248,10875
  TLEID_DLUSXSubNet 249,10951
  TLEID_APPNBindFail 250,11027
  TLEID_XIDRej 251,11103
  TLEID_BadPIU 252,11179
  TLEID_NoEventBufs 253,11255
  TLEID_XIDRejID 254,11331
  TLEID_NoPsidRspRcvd 255,11407
  TLEID_NoNtfyAvRspRcvd 256,11483
  TLEID_NoBindReqRcvd 257,11559
  TLEID_NoSdtReqRcvd 258,11635
  TLEID_NoSdtTmarkRcvd 259,11711
  TLEID_NoUnbindTmarkRcvd 260,11791
  TLEID_NoNtfyUaRcvd 261,11874
  TLEID_NoDynActluReqRcvd 262,11950
  TLEID_NoUnbindRspRcvd 263,12028
  TLEID_LuStateFlags 264,12104
  TLEID_LinkStateChange 265,12181
  TLEID_LinkAccept 266,12257
  TLEID_LinkRspDisc 267,12333
  TLEID_LinkXIDErrCV 268,12409
  TLEID_LinkRetries 269,12485
  TLEID_DroppedFrame 270,12567
  TLEID_SapRetries 271,12643
  TLEID_AdaptRetries 272,12724
  TLEID_LinkXIDErrLen 273,12809
  TLEID_LU62FrameRej 274,12885
  TLEID_BadFrame 275,12961
  TLEID_RejDLURBind 276,13037
  TLEID_AccCPBind 277,13123
  TLEID_RejCPBind 278,13199
  TLEID_CPBindAcc 279,13286
  TLEID_CPBindRej 280,13371
  TLEID_CPConWinUnbnd 281,13447
  TLEID_CPConLosUnbnd 282,13523
  TLEID_CPSendResReg 283,13599
  TLEID_CPReply2Caps 284,13685
  TLEID_CPReplyRecd 285,13768
  TLEID_AnswerLocate 286,13854
  TLEID_RejRecdLocate 287,13935
  TLEID_RecdRoute2DLUS 288,14011
  TLEID_NoRoute2DLUS 289,14089
  TLEID_CPSendBind 290,14185
  TLEID_Link2SrvrLost 291,14261
  TLEID_SendTGTDU2DLUS 292,14337
  TLEID_LinkUp 293,14413
  TLEID_LinkDown 294,14489
  TLEID_Link2DLUSLost 295,14565
  TLEID_SndVRNTDU2DLUS 296,14641
  TLEID_SndREQACTPU 297,14717
  TLEID_SndTDUs2DLUS 298,14793
  TLEID_BindAccdByDLUS 299,14869
  TLEID_BindRejByDLUS 300,14945
  TLEID_ConWinUnbDLUS 301,15021
  TLEID_ConLosUnbDLUS 302,15097
  TLEID_SndLocDLUS 303,15173
  TLEID_SndLocBUDLUS 304,15249
  TLEID_SndBind2DLUS 305,15325
  TLEID_GotREQACTPU 306,15401
  TLEID_GotREQDACTPU 307,15477
  TLEID_UnknownPuFr 308,15553
  TLEID_DLUSProtErr 309,15629
  TLEID_DLURRejBind 310,15705
  TLEID_AccBindFrDLUS 311,15781
  TLEID_PoolExhausted 312,15857
  TLEID_SndREQDACTPU 313,15933
  TLEID_LuDisconnected 314,16009
  TLEID_LuContention 315,16085
  TLEID_LuUnexpectedRsp 316,16161
  TLEID_LuConnected 317,16237
  TLEID_LuUnbind 318,16313
  TLEID_UnusedEntry 319,16389
int InitScatBindingsTn 322,16468
unsigned long APPNBindFailScatter 349,17447
unsigned long APPNProtErrScatter 387,18668
unsigned long BadPIUScatter 425,19888
unsigned long DLUSIsntScatter 466,21243
unsigned long DLUSXSubNetScatter 501,22320
unsigned long NoLuSessionsScatter 536,23400
unsigned long SystemScatter 571,24486
unsigned long XIDRejScatter 603,25465
unsigned long XIDRejIDScatter 644,26816
