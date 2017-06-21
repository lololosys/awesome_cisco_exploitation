
translate.h,2004
#define __TRANSLATE_H__65,2452
#define TRANS_NONE 72,2585
#define TRANS_TCP 73,2607
#define TRANS_X25 74,2627
#define TRANS_LAT 75,2647
#define TRANS_SLIP_PPP 76,2667
#define TRANS_AUTOCOMMAND 77,2692
#define TRANS_NPROT 78,2720
#define INPROT(INPROT79,2742
#define OUTPROT(OUTPROT80,2782
#define TRANSTYPE(TRANSTYPE81,2823
#define TRANS_QUEUE_MAX 82,2873
#define TRANS_LINE_COUNT 83,2900
#define MAXCHRS 84,2927
#define PT_DEFAULT_STACK_SIZE 89,3002
#define SV_STR 96,3131
#define OB_STR 104,3250
#define ND_STR 112,3369
typedef struct trans_tcpinfo_ trans_tcpinfo_115,3395
} trans_tcpinfo;trans_tcpinfo121,3537
#define TRANSF_TCP_STREAM	127,3676
#define TRANSF_TCP_ROTOR	128,3716
#define TRANSF_TCP_BINARY	129,3754
#define TRANSF_TCP_PRINTER	130,3791
#define TRANSF_TCP_IACHACK	131,3829
#define TRANSF_TCP_HOST_NAME	132,3867
typedef struct trans_x25info_ trans_x25info_134,3908
} trans_x25info;trans_x25info140,4242
#define TRANSF_X25_REVERSE 142,4260
#define TRANSF_X25_ACCEPTREVERSE 143,4304
#define TRANSF_X25_PVC 144,4348
#define TRANSF_X25_PRINTER 145,4392
#define TRANSF_X25_NOREVERSE 146,4436
#define TRANSF_X25_ALL 147,4480
typedef struct trans_latinfo_ trans_latinfo_149,4526
} trans_latinfo;trans_latinfo155,4685
#define TRANSF_LAT_BINARY 156,4702
#define TRANSF_LAT_UNADVERTISED 157,4746
typedef struct trans_slip_ppp_info_ trans_slip_ppp_info_159,4791
} trans_slip_ppp_info;trans_slip_ppp_info170,5166
#define TRANSF_SLIP_PPP_HEADERCOMPRESSED 171,5189
#define TRANSF_SLIP_PPP_ROUTING 172,5242
#define TRANSF_SLIP_PPP_PASSIVE 173,5295
#define TRANSF_PPP_AUTH 174,5348
#define TRANSF_SLIP_PPP_USE_TACACS 175,5401
typedef struct trans_autoinfo_ trans_autoinfo_177,5455
} trans_autoinfo;trans_autoinfo179,5516
struct translatetype_ translatetype_182,5537
#define TRANSF_GLOB_LOCAL 206,5976
#define TRANSF_GLOB_LOGIN 207,6019
#define TRANSF_GLOB_QUIET 208,6062
#define TRANSF_GLOB_SWAP 209,6105
#define TRANSF_GLOB_ALL 210,6148
