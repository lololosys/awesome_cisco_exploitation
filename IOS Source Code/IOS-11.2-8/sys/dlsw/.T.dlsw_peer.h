
dlsw_peer.h,3030
#define _DLSW_PEER_H_191,7534
#define PDEBUG 206,7894
#define PDEBUG_FSEQ 207,7938
#define PDEBUG_FPAK 208,7990
#define PDEBUG_FERR 209,8043
#define DLSW_RD_PORT 214,8160
#define DLSW_WR_PORT 215,8197
#define TCP_DEFAULT_RETRY_TIME 220,8271
#define TCP_PASSIVE_OPEN_WINDOW 221,8318
#define TCP_RETRY_THOLD_1 222,8364
#define TCP_RETRY_THOLD_2 223,8440
#define TCP_RETRY_THOLD_3 224,8516
#define DLSW_PRIORITY_COUNT 225,8593
#define DLSW_FCT_HASH_SIZE 230,8666
#define DLSW_FCT_HASH_MASK 231,8731
#define DLSW_FCT_CLEAN_INT 232,8789
#define DLSW_FCT_LIFE 233,8829
#define DLSW_FST_SEQNUM_MAX 235,8876
#define DLSW_FST_HALF_SEQ_RANGE 236,8912
#define DLSW_FST_ORDER_WIN 237,8972
#define DLSW_FST_OUT_OF_WIN_CNT 238,9032
typedef enum _peerstatetype _peerstatetype243,9112
} peerstatetype;peerstatetype249,9461
typedef enum _peereventtype _peereventtype255,9512
} peereventtype;peereventtype267,9759
typedef enum rd_state_t rd_state_t269,9777
} rd_state_t;rd_state_t273,9845
typedef enum _border_peer_ret_t _border_peer_ret_t275,9860
} border_peer_ret_t;border_peer_ret_t279,9933
typedef enum _peer_t _peer_t281,9955
} peer_t;peer_t287,10257
typedef enum _peer_status_t _peer_status_t289,10268
} peer_status_t;peer_status_t293,10362
#define SAP_CV_BYTES 298,10406
#define MIN_SAP_IN_BYTE 299,10442
#define MAX_SAP_IN_BYTE 300,10478
#define ODD_SAP_INDICATOR 301,10514
#define FST_RSRV_PEERS 302,10550
#define PEER_AUTOMORE_LINES 303,10586
#define PEER_CONN_TIMER 304,10622
#define PEER_KEEP_TIMER 305,10658
#define PEER_MAX_PAK 306,10694
typedef struct _peercap_t _peercap_t321,11081
} peercap_t;peercap_t354,11867
typedef boolean (*fast_wan2lan_type)fast_wan2lan_type358,11911
typedef boolean (*fast_wan2lan_type)(hwidbtype *, fast_cache_t *, paktype paktype358,11911
typedef boolean (*fast_hes_wan2lan_type)fast_hes_wan2lan_type359,11989
					 trrif_t *, uchar uchar360,12059
struct fast_cache_t_ fast_cache_t_365,12129
typedef int     (*fast_lan2wan_type)fast_lan2wan_type383,12779
typedef int     (*fast_lan2wan_type)(hwidbtype *, fast_cache_t *, paktype paktype383,12779
typedef int (*fast_les_lan2wan_type)fast_les_lan2wan_type384,12857
typedef int (*fast_les_lan2wan_type)(hwidbtype *, fast_cache_t *, paktype *, ulong)ulong384,12857
typedef int (*fast_hes_lan2wan_type)fast_hes_lan2wan_type385,12942
typedef int (*fast_hes_lan2wan_type)(hwidbtype *, fast_cache_t *, uchar *, ulong)ulong385,12942
struct peerentry_ peerentry_390,13054
typedef struct _localpeer_t _localpeer_t551,18510
} localpeer_t;localpeer_t566,19270
typedef struct _hdr_info_t _hdr_info_t568,19286
} hdr_info_t;hdr_info_t573,19391
typedef union _peer_lsap_u _peer_lsap_u578,19474
} peer_lsap_u;peer_lsap_u581,19538
static inline int peer_get_pri_by_cat 740,25475
static inline boolean dlsw_fst_acceptor 766,26061
static inline void dlsw_fst_incr_send 844,28672
static inline ulong dlsw_get_seq_num 854,28932
static inline fast_cache_t *dlsw_id2fct 862,29086
