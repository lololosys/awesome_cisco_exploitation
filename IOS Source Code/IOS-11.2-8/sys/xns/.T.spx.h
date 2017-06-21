
spx.h,1869
struct spxsocktype_ spxsocktype_36,990
typedef struct spxsocktype_ spxsocktype;spxsocktype46,1467
typedef nov_spx_header spxhdr;spxhdr47,1508
struct spxcontype_ spxcontype_53,1585
typedef struct spxcontype_ spxcontype;spxcontype101,3793
typedef boolean (*spx_listen_cb)spx_listen_cb102,3832
typedef boolean (*spx_listen_cb)(spxsocktype *, spxcontype spxcontype102,3832
typedef boolean (*spx_close_cb)spx_close_cb103,3895
typedef boolean (*spx_close_cb)(spxcontype spxcontype103,3895
enum SPXCON_STATES SPXCON_STATES109,4027
    SPXCON_ST_INITIAL,110,4048
    SPXCON_ST_OPEN,111,4071
    SPXCON_ST_CLOSING,112,4091
    SPXCON_ST_PURGING113,4114
#define SPX_MINSIZE 119,4165
#define SPXCON_MAX_OUT_PENDING 120,4230
#define SPXCON_PACKET_IO 124,4349
enum SPX_SHOW_TYPES SPX_SHOW_TYPES129,4480
    SHOW_SPX_CON,130,4502
    SHOW_SPX_ALL131,4520
#define FOR_ALL_SPX_SOCKETS(FOR_ALL_SPX_SOCKETS137,4563
#define FOR_ALL_SPXCON_ON_SOCK(FOR_ALL_SPXCON_ON_SOCK141,4706
#define SPX_CONCTL_XHD 153,4968
#define SPX_CONCTL_RES1 154,5033
#define SPX_CONCTL_NEG_SIZE 155,5088
#define SPX_CONCTL_SPXII 156,5155
#define SPX_CONCTL_EOM 158,5223
#define SPX_CONCTL_ATTN 159,5293
#define SPX_CONCTL_NEED_ACK 160,5355
#define SPX_CONCTL_SYS 161,5427
#define SPX_CONCTL_NEWCON 163,5487
#define SPX_CONCTL_WATCHDOG 164,5555
#define SPX_DATASTREAM_EOM_REQ 169,5665
#define SPX_DATASTREAM_EOM_ACK 170,5735
#define SPX_CONID_CONREQUEST 172,5800
#define SPX_ALLOCATION_UNITS 174,5869
#define SPX_VERIFY_WATCHDOG_TIMER 181,6089
#define SPX_ABORT_WATCHDOG_TIMER 182,6162
#define SPX_PURGE_TIMER 183,6235
enum SPX_RET_CODES SPX_RET_CODES188,6346
    SPX_RET_NODATA 189,6367
    SPX_RET_DATA 190,6391
    SPX_RET_OK=191,6413
    SPX_RET_CONCLOSED 192,6432
    SPX_RET_NOMEM 193,6459
    SPX_RET_BADCON 194,6482
    SPX_RET_FAILED 195,6506
