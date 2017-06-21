
rip.h,1378
#define __RIP_H__33,1057
#define	RIP_MIN_VERSION	35,1076
#define	RIP_MAX_VERSION 36,1145
#define UNSPECIFIED_FAMILY 38,1172
#define IN_FAMILY 39,1235
#define	RIP_MAXMETRIC 40,1286
#define RIP_MAXPACKETSIZE 41,1343
#define RIPHEADERBYTES 42,1410
#define MAXRIPBYTES 43,1465
#define	RIPCMD_REQUEST	45,1534
#define	RIPCMD_REPLY	46,1582
#define	RIPCMD_TRACEON	47,1631
#define	RIPCMD_TRACEOFF	48,1680
#define RIP_UPDATETIME 50,1732
#define RIP_INVALIDTIME 51,1805
#define RIP_GCTIME 52,1876
#define	RIP_JITTER	54,1947
enum ip_int_rip_cmdtypes ip_int_rip_cmdtypes59,2077
	RIP_SEND_VERS_CMD,60,2104
	RIP_RECV_VERS_CMD,61,2124
	RIP_AUTH_MODE_CMD,62,2144
	RIP_AUTH_KEY_CMD63,2164
typedef struct ripentrytype_ ripentrytype_69,2219
} ripentrytype;ripentrytype76,2357
typedef struct riptype_ riptype_78,2374
} riptype;riptype83,2485
#define	RIP_AUTH_ID	89,2611
#define	RIP_AUTH_TYPE_TEXT	90,2668
#define	RIP_AUTH_TYPE_MD	91,2723
#define	RIP_AUTH_HEADER_LEN	97,2900
#define	RIP_AUTH_TEXT_DATA_LEN	99,2952
#define	RIP_AUTH_MD5_LEN	100,3014
typedef struct ripauthtype_ ripauthtype_102,3074
} ripauthtype;ripauthtype120,3470
typedef struct ripmdsuffixtype_ ripmdsuffixtype_125,3568
} ripmdsuffixtype;ripmdsuffixtype129,3677
typedef struct ripsendtype_ ripsendtype_131,3697
} ripsendtype;ripsendtype138,3917
#define RIP_SENDQ_THRESHOLD 140,3933
