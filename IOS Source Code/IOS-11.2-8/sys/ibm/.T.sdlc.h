
sdlc.h,3751
#define __SDLC_H__107,3852
#define ISCLSSDLC(ISCLSSDLC111,3904
#define SDLC_NONE	115,3989
#define SDLC_PRIMARY	116,4011
#define SDLC_SECONDARY	117,4035
#define SDLC_PRIM_XID_POLL	118,4061
#define ISPSDLC(ISPSDLC120,4091
#define ISSSDLC(ISSSDLC125,4324
} snx_state_t;snx_state_t137,4801
#define NO_SDLC 142,4847
#define FRAS_SDLC 143,4873
#define DLSW_SDLC 144,4899
typedef struct sdlc_frametype_ sdlc_frametype_150,4965
} sdlc_frametype;sdlc_frametype153,5079
#define SDLCHEADERBYTES 155,5098
#define DEFAULT_LARGEST_SDLC_FRAME	156,5145
#define	SDLCTYPEBYTES	157,5184
#define	SDLC_FRMR_DATA	158,5221
#define sdlcdst	159,5246
#define msdlcdst 160,5285
#define msdlctype 161,5325
#define sdlcfragment 162,5366
#define	SDLC_DEFAULT_T1	164,5409
#define SDLC_DEFAULT_N1(SDLC_DEFAULT_N1165,5446
#define	SDLC_DEFAULT_N2	166,5496
#define SDLC_DEFAULT_XID0_N2 167,5523
#define	SDLC_DEFAULT_K	169,5671
#define SDLC_DEFAULT_POLL_WAIT 170,5712
#define	SDLC_DEFAULT_HOLDQ 171,5757
#define SDLC_MODULO	172,5794
#define SDLC_DEFAULT_SLF 173,5816
#define SDLC_XID_SIZE 174,5853
#define SDLC_DEFAULT_RNRLIMIT 176,5889
#define SDLC_MAX_RNRLIMIT 177,5964
#define SDLC_TXQ_HIWATER 179,6033
#define SDLC_CLS_TXQ_HIWATER 180,6087
#define SDLC_TXQ_LOWATER 181,6145
#define SDLC_CLS_FLOW_IND_DAMP 182,6197
#define SDLC_DEFAULT_SLOW_POLL 184,6257
#define SDLC_DEFAULT_POLL_PAUSE_TIMER 185,6303
#define SDLC_DEFAULT_POLL_LIMIT_VALUE 186,6344
#define SDLC_MAX_ADDRESS 187,6384
#define SDLC_MIN_ADDRESS 188,6416
#define SDLC_BCAST_ADDR 190,6449
#define SDLC_PRCMD 192,6509
#define SDLC_PRRSP 193,6530
#define DEFAULT_TEST_ITERATIONS 195,6552
#define MAX_TEST_STRING 196,6587
#define	SDLC_ISIFRAME(SDLC_ISIFRAME202,6640
#define	SDLC_ISSFRAME(SDLC_ISSFRAME203,6719
#define	SDLC_ISUFRAME(SDLC_ISUFRAME204,6798
#define SDLC_ECHO_ON 209,6928
#define SDLC_ECHO_OFF 210,6955
#define	SDLC_NOFRAME	216,7009
#define SDLC_IFRAME	217,7035
#define SDLC_IMASK 218,7060
#define SDLC_SFRAME	219,7089
#define SDLC_SMASK 220,7114
# define SDLC_RR	221,7143
# define SDLC_RNR	222,7165
# define SDLC_REJ	223,7188
#define SDLC_UFRAME	224,7211
# define SDLC_DM	225,7236
# define SDLC_SABM	226,7258
# define SDLC_SNRM	227,7282
# define SDLC_SIM	228,7306
# define SDLC_XID	229,7329
# define SDLC_TEST	230,7352
# define SDLC_CFGR	231,7376
# define SDLC_DISC	232,7400
# define SDLC_UA	233,7424
# define SDLC_RIM	234,7446
# define SDLC_FRMR	235,7469
# define SDLC_RD	236,7493
# define SDLC_BCN	237,7515
# define SDLC_UI 238,7538
#define SDLC_UPOLL 239,7569
#define SDLC_PF	240,7598
#define SDLC_FRMR_W	247,7690
#define SDLC_FRMR_X	249,7759
#define SDLC_FRMR_Y	251,7837
#define SDLC_FRMR_Z	253,7899
typedef enum _sdlc_state_t _sdlc_state_t259,8004
} sdlc_state_t;sdlc_state_t272,8258
typedef enum _cls_state_t _cls_state_t274,8275
} cls_state_t;cls_state_t286,8552
#define IS_SDLC_CONNECT(IS_SDLC_CONNECT291,8631
#define SDLLC_DEFAULT_TLF 296,8731
typedef enum _sdllc_state_t _sdllc_state_t315,9238
} sdllc_state_t;sdllc_state_t323,9831
typedef enum _sdlc_last_fail_cause_t _sdlc_last_fail_cause_t329,9884
} sdlc_last_fail_cause_t;sdlc_last_fail_cause_t338,10062
#define SDLC_THROTTLE_INTERVAL 358,10633
#define SDLC_INIT_COUNT 359,10676
#define SDLC_SEC_LINKUP_DEFAULT 360,10705
#define SDLC_FLAG_NOFRMR 374,11132
#define SDLC_FLAG_DCE 375,11213
#define SDLC_FLAG_NRM_TRNS 376,11291
#define SDLC_FLAG_TWS_FULL 377,11369
#define SDLC_FLAG_VIRTUAL_MULTIDROP 378,11447
typedef enum sdlc_oqueue_status_ sdlc_oqueue_status_381,11528
} sdlc_oqueue_status_t;sdlc_oqueue_status_t387,11684
struct sdlc_data_block_ sdlc_data_block_389,11709
static inline boolean sdlc_is_multidrop 670,25737
