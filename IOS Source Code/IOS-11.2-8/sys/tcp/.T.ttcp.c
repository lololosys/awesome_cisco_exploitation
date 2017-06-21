
ttcp.c,1591
#define DEF_BUFLEN	130,4375
#define DEF_NBUF	131,4405
#define DEF_BUFOFFSET	132,4433
#define DEF_BUFALIGN	133,4457
#define DEF_PORT	134,4490
#define DEF_SINKMODE	135,4512
#define DEF_NODELAY	136,4538
#define DEF_B_FLAG	137,4564
#define DEF_TOUCHDATA	138,4589
#define DEF_RCVWNDSIZE	139,4617
#define DEF_USE_DRIVER	140,4642
#define DEF_OQMAX	141,4671
#define DEF_SHOW_TCP	142,4693
enum { TTCP_INVALID 147,4838
enum { TTCP_INVALID = -1, TTCP_RECEIVE 147,4838
enum { TTCP_INVALID = -1, TTCP_RECEIVE = 0, TTCP_TRANSMIT 147,4838
struct ttcp_context ttcp_context149,4904
struct app_header app_header187,6006
void ttcp_receiver_readf 194,6086
void ttcp_receiver_init 209,6369
void ttcp_receiver_new_readf 215,6464
void ttcp_receiver_read_header 300,8311
void ttcp_receiver_read_data 338,9145
void ttcp_receiver_closef 391,10379
void ttcp_sender_closef 420,11286
static paktype *ttcp_getpak 431,11472
#define iswhite(iswhite449,11794
#define ttcp_isprint(ttcp_isprint451,11883
void pattern 454,11962
boolean ttcp_write_td_BLOCK 467,12129
int ttcp_write_td 478,12411
int ttcp_Nwrite 509,13256
int ttcp_Nwrite_td 524,13547
int ttyread 560,14400
int ttcp_xmit 587,14771
int ttcp_mread 787,19753
int ttcp_Nread 813,20228
int putbytes 839,20729
int ttcp_rcv 857,20927
#define TTY_BUFLEN	1009,24863
void ttcp_getnum 1012,24893
int ttcp_getparms 1031,25200
void ttcp_command 1138,28256
#define ALTERNATE 1195,29805
static void ttcp_init 1209,30105
#define TTCP_MAJVERSION 1220,30361
#define TTCP_MINVERSION 1221,30387
#define TTCP_EDITVERSION 1222,30413
