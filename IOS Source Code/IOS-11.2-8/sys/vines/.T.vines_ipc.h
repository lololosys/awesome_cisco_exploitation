
vines_ipc.h,3258
struct vinesipctype_ vinesipctype_46,1397
typedef struct vinesipc2type_ vinesipc2type_59,1666
} vinesipc2type;vinesipc2type65,1828
typedef struct vinesspptype_ vinesspptype_77,2131
} vinesspptype;vinesspptype88,2358
#define VIPC_PORT_UNKNOWN	100,2613
#define VIPC_PORT_FIRST_WKP	101,2663
#define VIPC_PORT_LAST_WKP	102,2701
#define VIPC_PORT_FIRST_AVAIL	103,2738
#define VIPC_PORT_LAST_AVAIL	104,2778
#define VIPC_PORT_TRANSIENT	105,2817
#define VIPC_PORT_ECHO	110,2908
#define VIPC_PORT_ROUTER	111,2942
#define VIPC_PORT_BACKEND	112,2977
#define VIPC_PORT_MAIL	113,3013
#define VIPC_PORT_FTP	114,3047
#define VIPC_PORT_FILE	115,3080
#define VIPC_PORT_SERVER	116,3114
#define VIPC_PORT_BKGND	117,3149
#define VIPC_PORT_ST	118,3184
#define VIPC_PORT_TALK	119,3216
#define VIPC_PORT_NM	120,3250
#define VIPC_PORT_VS	121,3282
#define VIPC_PORT_TUNNEL_IP	122,3314
#define VIPC_PORT_TUNNEL_ARP	123,3352
#define VIPC_PORT_STDA	124,3391
#define VIPC_PORT_STDA_LISTEN	125,3425
#define VIPC_PORT_NSM	126,3465
#define VIPC_PORT_TUNNEL_ATALK	127,3498
#define VIPC_PORT_TUNNEL_IPX	128,3539
#define VIPC_PORT_VINES_ASSISTANT 129,3578
#define VIPC_PORT_BIG_EAR_LISTENER 130,3620
#define VIPC_PORT_MAXCOUNTED	136,3760
#define VIPC_PORT_CISCO_PROXY	137,3823
#define FOR_ALL_VIPC_PORTS_COUNTED(FOR_ALL_VIPC_PORTS_COUNTED139,3882
#define VIPC_DATAGRAM	145,4022
#define VIPC_DATA	146,4078
#define VIPC_ERROR	147,4122
#define VIPC_DISC	148,4171
#define VIPC_PROBE	149,4216
#define VIPC_ACK	150,4257
#define VIPC_LAST	151,4302
#define VIPC_CTL_ACK	156,4364
#define VIPC_CTL_BOMEOM 157,4390
#define VIPC_CTL_EOM	158,4419
#define VIPC_CTL_BOM	159,4445
#define VIPC_CTL_ABORT	160,4471
#define VIPC_CTL_NONE	161,4499
#define VIPC_CONN_FIRST	166,4567
#define VIPC_CONN_LAST	167,4598
#define VIPC_MAXDATA_ONE 172,4672
#define VIPC_MAXDATA 173,4704
typedef struct vinesproxytype_ vinesproxytype_189,5133
} vinesproxytype;vinesproxytype195,5490
#define VIPC_RETRY_COUNT 200,5546
enum SOCKETSTATE SOCKETSTATE202,5579
    SS_NULL,203,5598
    SS_CONNECTING,204,5626
    SS_CONNECTED,205,5674
    SS_IDLE,206,5729
    SS_DEAD	207,5763
struct vinesipcsocket_ vinesipcsocket_210,5820
#define VIPC_NEED_NOTHING 244,6916
#define VIPC_NEED_LAST_HOP 245,6954
#define VIPC_NEED_VALID_METRIC 246,7023
#define VIPC_DEFAULT_METRIC 248,7093
#define retransmit_count 251,7187
#define retransmit_time 252,7227
#define FOR_ALL_VIPC_SOCKETS(FOR_ALL_VIPC_SOCKETS254,7262
#define FOR_ALL_VIPC_SOCKETS_W_NEXT(FOR_ALL_VIPC_SOCKETS_W_NEXT257,7380
#define FOR_ALL_VIPC_SOCKETS_W_PREV(FOR_ALL_VIPC_SOCKETS_W_PREV260,7501
typedef void (vipc_upcall)vipc_upcall265,7715
typedef void (vipc_upcall)(vinesipcsocket *, uchar uchar265,7715
typedef struct vipc_port_info_ vipc_port_info_272,7947
} vipc_port_info;vipc_port_info279,8124
#define FOR_ALL_VIPC_PORTS(FOR_ALL_VIPC_PORTS281,8143
#define FOR_ALL_VIPC_PORTS_W_PREV(FOR_ALL_VIPC_PORTS_W_PREV284,8246
static inline paktype *vipc_getbuffer 295,8608
static inline paktype *vipc_short_getbuffer 307,8896
static inline int vmetric_to_ipc 324,9279
static inline vinesidbtype *vipc_send_datagram 338,9629
static inline vinesidbtype *vipc_send_datagram_alt_src 350,9896
