
mbone_mtrace.h,1380
#define __MBONE_TRACE_H__41,1405
#define MBONE_MTRACE_DEFAULT_TTL 46,1464
#define MBONE_MTRACE_MAX_TTL 47,1500
#define MBONE_MTRACE_TIMEOUT 48,1537
#define MRINFO_TIMEOUT 49,1583
#define MAXHOPS 50,1630
#define WAITTIME 52,1667
#define TR_NO_ERR 54,1744
#define TR_WRONG_IF 55,1769
#define TR_PRUNED 56,1794
#define TR_OPRUNED 57,1819
#define TR_SCOPED 58,1844
#define TR_NO_RTE 59,1869
#define TR_NO_FWD 60,1894
#define TR_HIT_RP 61,1919
#define TR_RPF_IF 62,1944
#define TR_NO_MULTI 63,1969
#define TR_NO_SPACE 65,1996
#define TR_OLD_ROUTER 66,2024
#define TR_ADMIN_PROHIB 67,2052
#define PROTO_DVMRP 69,2082
#define PROTO_MOSPF 70,2104
#define PROTO_PIM 71,2126
#define PROTO_CBT 72,2148
#define PROTO_PIM_SPECIAL 73,2170
#define PROTO_PIM_STATIC 74,2198
#define PROTO_DVMRP_STATIC 75,2225
#define NEITHER 77,2255
#define INS 78,2273
#define OUTS 79,2291
#define INOUT 80,2309
typedef struct mbone_mtraceQtype_ mbone_mtraceQtype_82,2329
} mbone_mtraceQtype;mbone_mtraceQtype91,2632
typedef struct mrinfoQtype_ mrinfoQtype_93,2654
} mrinfoQtype;mrinfoQtype101,2861
typedef struct mbone_mtrace_query_ mbone_mtrace_query_103,2877
} mbone_mtrace_query;mbone_mtrace_query108,3165
typedef struct mbone_mtrace_resp_ mbone_mtrace_resp_110,3188
} mbone_mtrace_resp;mbone_mtrace_resp122,3482
#define  QLEN 124,3504
#define  RLEN 125,3553
