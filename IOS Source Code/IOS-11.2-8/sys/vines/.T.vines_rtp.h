
vines_rtp.h,1218
struct vrtp_type_ vrtp_type_46,1516
#define VRTP_REQUEST	54,1630
#define VRTP_UPDATE	55,1654
#define VRTP_RESPONSE	56,1677
#define VRTP_REDIRECT	57,1702
#define VRTP_LAST	58,1727
#define VRTP_NODE_CLIENT 67,2075
#define VRTP_NODE_ROUTER 68,2102
#define VRTP_CONT_SINGLE 70,2130
#define VRTP_CONT_MULTI	71,2157
#define VRTP_MACH_SLOW	73,2185
#define VRTP_MACH_FAST	74,2213
#define VRTP_MACH_TCPIP	75,2241
#define VRTP_COMPATABILITY 76,2270
typedef struct vrtp_entry_ vrtp_entry_81,2383
} vrtp_entry;vrtp_entry84,2459
#define VRTP_MAX_ENTRIES 86,2474
struct vrtp_redirecttype_ vrtp_redirecttype_91,2581
#define V_REDIR_DATAMAX 113,3039
#define VRTP_RC_INITIAL_TIME	130,3880
#define VRTP_RC_TIME	131,3922
#define VRTP_RC_RETRY_TIME	132,3954
typedef enum vrtp_rc_states_ vrtp_rc_states_134,3996
} vrtp_rc_states;vrtp_rc_states140,4151
static inline ushort vmetric_to_rtp 154,4451
static inline ushort vmetric_from_rtp 168,4723
static inline void vrtp_buginf_metric 181,5020
static inline void vrtp_rc_timer_initial_enqueue 210,5716
static inline void vrtp_rc_timer_enqueue 221,6047
static inline void vrtp_rc_timer_enqueue_retry 232,6362
static inline paktype *vrtp_getbuffer 256,7008
