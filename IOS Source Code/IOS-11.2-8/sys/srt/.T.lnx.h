
lnx.h,1325
#define __LNX_H__67,2390
#define LNX_BRIDGE_NUMBER 71,2442
typedef enum _lnx_state_t _lnx_state_t90,2947
} lnx_state_t;lnx_state_t100,3516
#define    LNX_INVALID	103,3550
typedef enum _lnx_net_event_t _lnx_net_event_t106,3651
    } lnx_net_event_t;lnx_net_event_t117,4113
typedef enum _lnx_dlc_event_t _lnx_dlc_event_t121,4183
    LNX_DLC_CONTACTED_IND,	/* received QUA				*/    } lnx_dlc_event_t;lnx_dlc_event_t125,4382
#define LNX_HASH_SIZE 128,4455
#define LNX_HASH_MASK 129,4515
#define LNX_XID_SIZE 132,4557
#define SNX_XID_SIZE 133,4588
#define SNX_X25_PASS_SIZE 134,4619
#define DEFAULT_SDLC_FRAG 136,4651
#define LNX_MIN_THRESHOLD 139,4686
#define LNX_MAX_THRESHOLD 140,4715
    LNX_NET_LINKUP_EVENT 147,4802
    LNX_DLC_LINKUP_EVENT,148,4832
struct lnxtype_ lnxtype_155,4956
typedef struct xidnulltype_ xidnulltype;xidnulltype234,7331
struct xidnulltype_ xidnulltype_236,7373
#define		XID0_SIZE	262,8029
#define         XID_XID1_PU2 263,8069
#define         XID_XID3 265,8134
#define         XID3_ABM 266,8193
#define		XID1_MIN_SIZE	273,8360
#define		XID1_ROLE_TWS_OFF	274,8387
#define		XID1_MAXBTU_OFF	275,8416
#define		XID1_MAX_WINDOW_OFF	276,8445
#define		XID1_PRIMARY_MASK	278,8478
typedef struct xid3type_ xid3type;xid3type280,8543
struct xid3type_ xid3type_282,8580
