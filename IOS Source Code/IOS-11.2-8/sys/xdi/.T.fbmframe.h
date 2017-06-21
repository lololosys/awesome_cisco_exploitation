
fbmframe.h,8680
typedef struct MACHdrStructMACHdrStruct70,2547
} MACHdrType;MACHdrType76,2817
#define	SIZE_MACHdrType	78,2832
typedef struct SMTHdrStructSMTHdrStruct87,3128
} SMTHdrType;SMTHdrType96,3546
#define	SIZE_SMTHdrType	98,3561
typedef struct SMTFrameHdrStructSMTFrameHdrStruct103,3641
} SMTFrameHdrType;SMTFrameHdrType107,3728
#define	SIZE_SMTFrameHdrType	109,3748
#define	SMT_FRAME_HDR_SIZE	114,3849
#define SMT_NSA_FC	121,4084
#define SMT_INFO_FC	122,4141
#define NIF_CLASS	132,4436
#define SIFCONFIG_CLASS	133,4478
#define SIFOP_CLASS	134,4539
#define ECF_CLASS	135,4593
#define RAF_CLASS	136,4642
#define RDF_CLASS	137,4702
#define	SRF_CLASS	138,4761
#define	PMFGET_CLASS	139,4819
#define	PMFSET_CLASS	140,4867
#define ESF_CLASS	141,4915
#define SMTANNOUNCE	146,5023
#define SMTREQUEST	147,5085
#define SMTRESPONSE	148,5141
typedef struct SMTParamHdrStructSMTParamHdrStruct164,5725
} ParamHdrType;ParamHdrType168,5877
#define	SIZE_ParamHdrType	170,5894
#define UNA_PARAM_TYPE	175,5980
#define STATION_DESC_PARAM_TYPE	176,6045
#define STATION_STATE_PARAM_TYPE	177,6110
#define MSG_TIME_STAMP_PARAM_TYPE	178,6170
#define STATION_POLICIES_PARAM_TYPE	179,6228
#define PATH_LATENCY_PARAM_TYPE	180,6302
#define	MAC_NEIGHBORS_PARAM_TYPE	181,6366
#define PATH_DESC_PARAM_TYPE	182,6426
#define MAC_STATUS_PARAM_TYPE	183,6485
#define LER_STATUS_PARAM_TYPE	184,6540
#define FRAME_COUNTERS_PARAM_TYPE	185,6608
#define FRAME_NOT_COPIED_PARAM_TYPE	186,6674
#define PRIORITY_VALUES_PARAM_TYPE	187,6747
#define EB_STATUS_PARAM_TYPE	188,6815
#define MANUFACTURER_PARAM_TYPE	189,6883
#define USER_PARAM_TYPE	190,6953
#define ECHO_PARAM_TYPE	191,7017
#define REASON_CODE_PARAM_TYPE	192,7067
#define REJECTED_FRAME_PARAM_TYPE	193,7124
#define SUPPORTED_VERSIONS_PARAM_TYPE	194,7196
#define	RESOURCE_TYPE_PARAM_TYPE	195,7270
#define	SBA_COMMAND_PARAM_TYPE	196,7330
#define	SBA_PAYLOAD_PARAM_TYPE	197,7387
#define	SBA_OVERHEAD_PARAM_TYPE	198,7452
#define	SBA_ALLOC_ADDR_PARAM_TYPE	199,7519
#define	SBA_CATEGORY_PARAM_TYPE	200,7589
#define	MAX_T_NEG_PARAM_TYPE	201,7648
#define	MIN_SEGMENT_PARAM_TYPE	202,7705
#define	SBA_ALLOCATABLE_PARAM_TYPE	203,7775
#define	AUTHORIZATION_PARAM_TYPE	204,7839
#define ESFID_PARAM_TYPE	205,7909
typedef struct UNAParamStructUNAParamStruct212,8178
} UNAParamType;UNAParamType218,8423
#define	SIZE_UNAParamType	220,8440
typedef struct StationDescParamStructStationDescParamStruct223,8497
} StationDescParamType;StationDescParamType231,8874
#define	SIZE_StationDescParamType	233,8899
typedef struct StationStateParamStructStationStateParamStruct236,8956
} StationStateParamType;StationStateParamType243,9269
#define	SIZE_StationStateParamType	245,9295
typedef struct MsgTimeStampParamStructMsgTimeStampParamStruct248,9358
} MsgTimeStampParamType;MsgTimeStampParamType253,9567
#define	SIZE_MsgTimeStampParamType	255,9593
typedef struct StationPoliciesParamStructStationPoliciesParamStruct258,9655
} StationPoliciesParamType;StationPoliciesParamType264,9937
#define	SIZE_StationPoliciesParamType	266,9966
typedef struct PathLatencyParamStructPathLatencyParamStruct269,10048
} PathLatencyParamType;PathLatencyParamType277,10446
#define	SIZE_PathLatencyParamType	279,10471
typedef struct MACNeighborsParamStructMACNeighborsParamStruct282,10530
} MACNeighborsParamType;MACNeighborsParamType290,10910
#define	SIZE_MACNeighborsParamType	292,10936
typedef struct PathPORTRecordStructPathPORTRecordStruct299,11087
} PathPORTRecordType;PathPORTRecordType307,11460
#define	SIZE_PathPORTRecordType	309,11483
typedef struct PathMACRecordStructPathMACRecordStruct311,11520
} PathMACRecordType;PathMACRecordType315,11665
#define	SIZE_PathMACRecordType	317,11687
typedef struct PathDescParamStructPathDescParamStruct319,11723
} PathDescParamType;PathDescParamType328,11944
typedef struct MACStatusStructMACStatusStruct331,11983
} MACStatusParamType;MACStatusParamType346,12756
#define	SIZE_MACStatusParamType	348,12779
typedef struct LERStatusParamStructLERStatusParamStruct351,12868
} LERStatusParamType;LERStatusParamType363,13453
#define	SIZE_LERStatusParamType	365,13476
typedef struct FrameCountersParamStructFrameCountersParamStruct368,13539
} FrameCountersParamType;FrameCountersParamType376,13942
#define	SIZE_FrameCountersParamType	378,13969
typedef struct FrameNotCopiedParamStructFrameNotCopiedParamStruct381,14043
} FrameNotCopiedParamType;FrameNotCopiedParamType388,14386
#define	SIZE_FrameNotCopiedParamType	390,14414
typedef struct PriorityValuesParamStructPriorityValuesParamStruct393,14481
} PriorityValuesParamType;PriorityValuesParamType406,15079
#define	SIZE_PriorityValuesParamType	408,15107
typedef struct EBStatusParamStructEBStatusParamStruct411,15169
} EBStatusParamType;EBStatusParamType418,15502
#define	SIZE_EBStatusParamType	420,15524
typedef struct ManufacturerParamStructManufacturerParamStruct423,15586
} ManufacturerParamType;ManufacturerParamType429,15852
#define	SIZE_ManufacturerParamType	431,15878
typedef struct UserParamStructUserParamStruct434,15935
} UserParamType;UserParamType439,16135
#define	SIZE_UserParamType	441,16153
typedef struct EchoParamStructEchoParamStruct444,16203
} EchoParamType;EchoParamType449,16408
typedef struct ReasonCodeParamStructReasonCodeParamStruct452,16444
} ReasonCodeParamType;ReasonCodeParamType457,16654
#define	SIZE_ReasonCodeParamType	459,16678
#define	REJECTED_PAD_SIZE	462,16746
typedef struct RejectedFrameParamStructRejectedFrameParamStruct463,16775
} RejectedFrameParamType;RejectedFrameParamType469,17038
typedef struct SupportedVersionsParamStructSupportedVersionsParamStruct472,17094
} SupportedVersionsParamType;SupportedVersionsParamType480,17486
typedef struct ResourceTypeParamStructResourceTypeParamStruct483,17537
} ResourceTypeParamType;ResourceTypeParamType488,17754
#define	SIZE_ResourceTypeParamType	490,17780
typedef struct SBACommandParamStructSBACommandParamStruct493,17837
} SBACommandParamType;SBACommandParamType498,18038
#define	SIZE_SBACommandParamType	500,18062
typedef struct SBAPayloadParamStructSBAPayloadParamStruct503,18125
} SBAPayloadParamType;SBAPayloadParamType508,18337
#define	SIZE_SBAPayloadParamType	510,18361
typedef struct SBAOverheadParamStructSBAOverheadParamStruct513,18425
} SBAOverheadParamType;SBAOverheadParamType518,18640
#define	SIZE_SBAOverheadParamType	520,18665
typedef struct SBAAllocAddrParamStructSBAAllocAddrParamStruct523,18732
} SBAAllocAddrParamType;SBAAllocAddrParamType529,18974
#define	SIZE_SBAAllocAddrParamType	531,19000
typedef struct SBACategoryParamStructSBACategoryParamStruct534,19059
} SBACategoryParamType;SBACategoryParamType539,19265
#define	SIZE_SBACategoryParamType	541,19290
typedef struct MaxTnegParamStructMaxTnegParamStruct544,19348
} MaxTnegParamType;MaxTnegParamType549,19544
#define	SIZE_MaxTnegParamType	551,19565
typedef struct MinSegmentParamStructMinSegmentParamStruct554,19631
} MinSegmentParamType;MinSegmentParamType559,19845
#define	SIZE_MinSegmentParamType	561,19869
typedef struct SBAAllocatableParamStructSBAAllocatableParamStruct564,19928
} SBAAllocatableParamType;SBAAllocatableParamType569,20147
#define	SIZE_SBAAllocatableParamType	571,20175
typedef struct AuthorizationParamStructAuthorizationParamStruct574,20245
} AuthorizationParamType;AuthorizationParamType579,20459
typedef struct ESFIDParamStructESFIDParamStruct582,20499
} ESFIDParamType;ESFIDParamType588,20748
#define	SIZE_ESFIDParamType	590,20767
#define	FBM_rejectA_A	601,21110
#define	FBM_rejectB_B	602,21139
#define	FBM_rejectS_S	603,21168
#define	FBM_rejectA_M	604,21197
#define	FBM_rejectB_M	605,21226
#define	FBM_rejectA_S	606,21255
#define	FBM_rejectB_S	607,21284
#define	FBM_rejectA_B	608,21313
#define	FBM_rejectB_A	609,21342
#define	FBM_rejectS_A	610,21371
#define	FBM_rejectS_B	611,21400
#define	FBM_rejectS_M	612,21429
#define	FBM_rejectM_A	613,21494
#define	FBM_rejectM_B	614,21561
#define	FBM_rejectM_S	615,21590
#define	FBM_rejectM_M	616,21619
#define	SMT_VERSION_CONSTANT	623,21794
#define	SIZE_NIF\636,22272
#define SIZE_SIFCON_REQ\643,22437
#define SIZE_SIFCON_RESP\646,22485
#define SIZE_SIFOP_REQ\659,22902
#define SIZE_SIFOP_RESP\662,22949
#define SIZE_ECF_HDR\675,23410
#define	SIZE_RAF_HDR\679,23478
#define	SIZE_RDF_HDR\682,23523
#define SIZE_ESF_HDR\688,23684
#define SIZE_PMF_GET_REQ_HDR\692,23756
#define MClearFrameBuffer(MClearFrameBuffer716,24714
#define MClearFrameBuffer(MClearFrameBuffer721,24823
#define ECHO_CLASS	738,25357
#define	PMFCHANGE_CLASS	739,25386
