
csphdr.h,3501
#define	CSP_PORT_COUNT	59,1792
typedef struct CSPStationInfoCSPStationInfo74,2219
} CSPStationInfoType;CSPStationInfoType120,4383
typedef struct CSPPATHInfoCSPPATHInfo127,4488
} CSPPATHInfoType;CSPPATHInfoType141,5000
#define LEM_WindowSize	153,5284
struct TimeCountTimeCount161,5526
typedef struct CSPPORTInfoCSPPORTInfo167,5675
} CSPPORTInfoType;CSPPORTInfoType254,10201
typedef struct GIPortNodeGIPortNode264,10443
} GIPortNode;GIPortNode268,10533
typedef struct CSPMACInfoCSPMACInfo274,10628
} CSPMACInfoType;CSPMACInfoType350,14169
struct CSPAllInfoCSPAllInfo355,14214
struct SigQueueEntrySigQueueEntry370,14619
struct TimerListEntryTimerListEntry383,15011
#define	TIMER_SIGNAL	407,15933
#define	ECM_SIGNALS	409,15960
#define	SIG_Connect	410,15987
#define	SIG_Disconnect	411,16029
#define	SIG_Trace_Prop	412,16073
#define	SIG_Path_Test	413,16117
#define	SIG_EC_PCM_RETRY	414,16160
#define	SIG_EC_Timer	415,16224
#define	PCM_SIGNALS	417,16275
#define	SIG_PC_Start	418,16302
#define	SIG_PC_Maint	419,16344
#define	SIG_PC_Trace	420,16386
#define	SIG_PC_Stop	421,16428
#define	SIG_PC_Signal	422,16470
#define	SIG_PC_PDR	423,16513
#define	SIG_PC_Join	424,16554
#define	SIG_Line_State	425,16596
#define	SIG_Noise_Event	426,16640
#define	SIG_Noise_Timer	427,16685
#define	SIG_PC_Scrub_Complete\428,16730
#define	SIG_PC_Enable	430,16788
#define	SIG_PC_Disable	431,16831
#define	SIG_PC_LEM_Fail	432,16875
#define	SIG_PC_WC_Flag	433,16920
#define	SIG_PC_RETRY	434,16995
#define	SIG_PC_Timer	435,17053
#define	CFM_SIGNALS	437,17104
#define	SIG_CF_Loop	438,17131
#define	SIG_CF_Join	439,17173
#define SIG_CF_Change	440,17215
#define SIG_CF_Timer	441,17275
#define	RMT_SIGNALS	443,17326
#define	SIG_RM_Loop	444,17353
#define	SIG_RM_Join	445,17395
#define	SIG_Ring_OP	446,17437
#define	SIG_MAC_Interrupt\447,17503
#define	SIG_Dup_Addr	449,17583
#define	SIG_RM_DA_Flag	450,17654
#define	SIG_RM_MA_Unitdata\451,17722
#define	SIG_RM_Available\453,17805
#define	SIG_RM_Timer	455,17888
#define	LEM_SIGNALS	457,17939
#define	SIG_LEM_Start	458,17966
#define	SIG_LEM_Stop	459,18036
#define	SIG_LCT_Start	460,18104
#define	SIG_LCT_Stop	461,18163
#define	SIG_LEM_Threshhold\462,18220
#define	SIG_LE_Timer	464,18300
#define LMM_SIGNALS	467,18386
#define SIG_Abort_GI	468,18413
#define SIG_Join_Primary	469,18486
#define SIG_Join_Secondary	470,18557
#define SIG_Leave_Primary	471,18632
#define SIG_Leave_Secondary	472,18702
#define SIG_Change_Complete	473,18774
#define SIG_Run_Diagnostics	474,18844
#define SIG_Loop_Req	475,18919
#define SIG_Claim_Complete	476,18980
#define SIG_LM_Timer 477,19055
#define	PORT_INT_NONE	487,19333
#define	PORT_INT_QLS	488,19390
#define	PORT_INT_HLS	489,19447
#define	PORT_INT_MLS	490,19504
#define	PORT_INT_SILS	491,19561
#define	PORT_INT_NOISE	492,19620
#define	PORT_INT_EB	493,19689
#define	MAC_My_Claim	498,19774
#define	MAC_My_Beacon	499,19827
#define	MAC_Other_Beacon	500,19882
#define	MAC_TRT_In_T4T5	501,19942
#define	MAC_Rmode	502,20005
#define	MAC_TBid_TReq_Mismatch	503,20067
#define	MCF_WRAP_AB(MCF_WRAP_AB510,20309
#define	EMPTY_PORT	516,20466
#define	SMT_Broadcast_DA	537,20943
#define	NULL_ADDRESS	538,20986
#define	SMT_Unknown_Address	539,21021
#define	SMT_Directed_Beacon_DA	540,21065
#define	SMT_SRF_DA	541,21108
#define	BROADCAST_ADDRESS	558,21509
#define	UNKNOWN_ADDRESS	559,21553
#define	DIR_BCN_ADDRESS	560,21599
#define	SRF_MULTICAST_ADDRESS	561,21648
