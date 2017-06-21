
isdn_bri.h,4461
#define	ISDN_BRI_H141,5257
#define SERIALS_PER_BRI	143,5277
enum Chan Chan147,5406
enum Chan { ISDN_Dchan 147,5406
enum Chan { ISDN_Dchan = -1, B1,147,5406
enum Chan { ISDN_Dchan = -1, B1, B2,147,5406
enum Chan { ISDN_Dchan = -1, B1, B2, B3,147,5406
enum Chan { ISDN_Dchan = -1, B1, B2, B3, B4,147,5406
enum Chan { ISDN_Dchan = -1, B1, B2, B3, B4, B5,147,5406
enum Chan { ISDN_Dchan = -1, B1, B2, B3, B4, B5, B6,147,5406
enum Chan { ISDN_Dchan = -1, B1, B2, B3, B4, B5, B6, B7,147,5406
enum Chan { ISDN_Dchan = -1, B1, B2, B3, B4, B5, B6, B7, B8,147,5406
enum Chan { ISDN_Dchan = -1, B1, B2, B3, B4, B5, B6, B7, B8, B9,147,5406
enum Chan { ISDN_Dchan = -1, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10,147,5406
enum Chan { ISDN_Dchan = -1, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11,147,5406
		B12,148,5481
		B12, B13,148,5481
		B12, B13, B14,148,5481
		B12, B13, B14, B15,148,5481
		B12, B13, B14, B15, B16,148,5481
		B12, B13, B14, B15, B16, B17,148,5481
		B12, B13, B14, B15, B16, B17, B18,148,5481
		B12, B13, B14, B15, B16, B17, B18, B19,148,5481
		B12, B13, B14, B15, B16, B17, B18, B19, B20,148,5481
		B12, B13, B14, B15, B16, B17, B18, B19, B20, B21,148,5481
		B12, B13, B14, B15, B16, B17, B18, B19, B20, B21, B22,148,5481
		B12, B13, B14, B15, B16, B17, B18, B19, B20, B21, B22, B23,148,5481
		B24,149,5543
		B24, B25,149,5543
		B24, B25, B26,149,5543
		B24, B25, B26, B27,149,5543
		B24, B25, B26, B27, B28,149,5543
		B24, B25, B26, B27, B28, B29,149,5543
		B24, B25, B26, B27, B28, B29, B30,149,5543
		B24, B25, B26, B27, B28, B29, B30, B31 149,5543
enum Enab Enab151,5588
enum Enab { disable 151,5588
enum Enab { disable = 0, enable}151,5588
enum Loopback Loopback153,5661
        LOOPBACK_DISABLED 154,5677
        SYSTEM_LOOPBACK_ENABLED 155,5756
        REMOTE_LOOPBACK_ENABLED 156,5832
        PLUG_LOOPBACK_ENABLED 157,5908
enum BriMsgType BriMsgType160,5988
	ISDN_CALL,162,6071
	ISDN_CALL, ISDN_HANGUP,162,6071
	ISDN_CONNECT,166,6206
	ISDN_CONNECT, ISDN_INCOMING,166,6206
	ISDN_CONNECT, ISDN_INCOMING, ISDN_DISCON,166,6206
        VOICE_IDLE,169,6328
        VOICE_ANS,170,6348
        VOICE_ISDNCALL,171,6413
        VOICE_DIGIT,172,6485
        VOICE_HANGUP 173,6528
enum activation activation176,6608
enum activation {DEACTIVATED 176,6608
enum activation {DEACTIVATED = 0, PENDING,176,6608
enum activation {DEACTIVATED = 0, PENDING, ACTIVATED}176,6608
enum EncodeType EncodeType178,6664
		U_LAW_ENCODING 179,6682
		A_LAW_ENCODING 180,6704
#define MAX_NUM_LEN 183,6729
} l1_prim_element_t;l1_prim_element_t199,7119
enum msg_for_modem msg_for_modem205,7235
    DEV_IDLE 206,7256
    DEV_INCALL,207,7293
    DEV_SETUP_ACK,208,7336
    DEV_CALL_PROC,209,7399
    DEV_CONNECTED 210,7441
enum state_of_pots state_of_pots216,7580
    POTS_IDLE_ST 217,7601
    POTS_OFFHOOK_ST,218,7655
    POTS_SETUP_ACKP_ST,219,7715
    POTS_RINGING_ST,220,7796
    POTS_DIALING_ST,221,7856
    POTS_CONN_ACKP_ST,222,7928
    POTS_CALL_PROC_ST,223,8006
    POTS_CONNECTED_ST 224,8072
#define ISDN_BRI_L1	230,8179
#define BRI_MAX_L1_PRIM	231,8243
#define PRI_MAX_L1_PRIM	233,8339
#define NO_BUSY_CAUSE_SPECIFIED 236,8435
} l1_state_t;l1_state_t260,9622
typedef struct isdn_l1_primitive isdn_l1_primitive_t;isdn_l1_primitive_t266,9698
struct isdn_l1_primitive isdn_l1_primitive267,9752
typedef struct call_id_string call_id_string_t;call_id_string_t275,10016
struct call_id_string call_id_string277,10065
typedef struct v120_parameters_type v120_parameters_type295,10743
} v120_parameters_type;v120_parameters_type306,11238
typedef struct llc_parse_type llc_parse_type308,11263
} llc_parse_type;llc_parse_type314,11524
typedef struct call_time_check call_time_check_t;call_time_check_t321,11747
struct call_time_check call_time_check323,11798
#define NTT_REDIAL_TIMEOUT	329,11914
enum rem_src rem_src337,12172
    ISDN_SRC_UNKNOWN,338,12187
    ISDN_SRC_ISDN,339,12209
    ISDN_SRC_DIALER340,12228
struct isdn_bri_t_ isdn_bri_t_344,12274
#define IS_BRI(IS_BRI424,16303
#define IS_PRI(IS_PRI425,16383
#define ISBRILEASED(ISBRILEASED430,16550
static inline boolean hwidb_is_bri_leased_line_inline 432,16669
#define b1_chan_idb	441,16889
#define b2_chan_idb	442,16925
#define BRI_LBL1	444,16962
#define BRI_LBL2	445,17019
#define BRI_DUAL_LINE 446,17076
#define BRI_LBS	447,17155
#define BRI_LBB1	448,17216
#define BRI_LBB2	449,17276
