
vines_srtp.h,2977
#define __VINES_SRTP_H__40,1459
struct vsrtp_header_ vsrtp_header_53,1735
#define VSRTP_HEADER_BYTES 62,1885
#define VRTP_VERSION	64,1940
#define VSRTP_VERSION	65,1969
#define VSRTP_VERSION_THRESH	66,1999
#define VSRTP_REQUEST	68,2036
#define VSRTP_UPDATE	69,2064
#define VSRTP_REDIRECT	70,2091
#define VSRTP_REINIT	71,2120
#define VSRTP_LAST	72,2147
#define VSRTP_NODE_CLIENT	81,2498
#define VSRTP_NODE_ROUTER	82,2529
#define VSRTP_FLAG_MISMATCH	84,2561
#define VSRTP_FLAG_OLD_PRESENT	85,2594
#define VSRTP_FLAG_AUTOMATIC	86,2630
struct vsrtp_request_ vsrtp_request_91,2716
#define VSRTP_REQUEST_BYTES 97,2810
#define VSRTP_REQUEST_HEADER_BYTES 98,2865
typedef enum vrtp5_request_infotype_ vrtp5_request_infotype_100,2903
} vrtp5_request_infotype;vrtp5_request_infotype107,3141
struct vsrtp_update_ vsrtp_update_112,3218
#define VSRTP_UPDATE_BYTES 122,3412
enum vsrtp_update_infotype vsrtp_update_infotype124,3468
    VSRTP_UPD_UPDATE 125,3497
    VSRTP_UPD_RESPONSE 126,3523
    VSRTP_UPD_LAST 127,3551
    VSRTP_UPD_LAST = VSRTP_UPD_RESPONSE127,3551
#define VSRTP_UPD_FLAG_NONE	130,3595
#define VSRTP_UPD_FLAG_NULL	131,3628
#define VSRTP_UPD_FLAG_BOM	132,3661
#define VSRTP_UPD_FLAG_EOM	133,3693
#define VSRTP_UPD_FLAG_BOMEOM	134,3725
#define VSRTP_UPD_FLAG_CHANGE	135,3760
#define VSRTP_UPD_FLAG_FULL	136,3795
#define VSRTP_UPD_FLAG_SYNC	137,3828
#define VSRTP_SYNC_INTERVAL	139,3862
enum vsrtp_update_type vsrtp_update_type141,3906
    VSRTP_NULL_UPDATE 142,3931
    VSRTP_CHANGE_UPDATE 143,3961
    VSRTP_FULL_UPDATE 144,3991
    VSRTP_SYNC_UPDATE 145,4021
    VSRTP_INVALID_UPDATE 146,4051
typedef struct vsrtp_entry_ vsrtp_entry_149,4084
} vsrtp_entry;vsrtp_entry155,4217
#define VSRTP_ENTRY_BYTES 157,4233
#define VSRTP_ENTRY_NONE	159,4288
#define VSRTP_ENTRY_LAN	160,4318
#define VSRTP_ENTRY_WAN	161,4348
#define VSRTP_ENTRY_NON_VINES	162,4378
#define VSRTP_ENTRY_NO_SEQ	163,4413
#define VSRTP_ENTRY_INVALID	164,4445
#define VSRTP_UPD_FRAGENTRIES	166,4479
#define VSRTP_UPD_FRAGBYTES	167,4513
#define VSRTP_UPD_TOTALBYTES	168,4585
typedef struct vsrtp_redirect_ vsrtp_redirect_173,4721
} vsrtp_redirect;vsrtp_redirect190,5152
#define VSRTP_REDIRECT_BYTES 191,5170
#define VSRTP_REQUEST_COUNT 196,5279
#define VSRTP_REQUEST_TIME 197,5310
#define VSRTP_REQUEST_SLEEP 198,5353
#define VSRTP_REINIT_CHECK	200,5398
#define VSRTP_REINIT_PAUSE	204,5641
#define VSRTP_REASSEMBLY_TIME 208,5857
enum vsrtp_seq_comp vsrtp_seq_comp210,5905
    LESS 211,5927
    EQUAL 212,5941
    ONEMORE 213,5956
    GREATER 214,5973
typedef enum vsrtp_reass_type_ vsrtp_reass_type_217,5993
} vsrtp_reass_type;vsrtp_reass_type223,6149
static inline ushort vmetric_to_srtp 238,6451
static inline ushort vmetric_from_srtp 248,6659
static inline enum vsrtp_seq_comp vsrtp_compare_seqno 265,7030
static inline enum vsrtp_seq_comp vsrtp_compare_3seqno 287,7410
static inline paktype *vsrtp_getbuffer 312,7845
