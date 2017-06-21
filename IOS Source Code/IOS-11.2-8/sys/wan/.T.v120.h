
v120.h,2960
#define V120_LLIBYTES 37,1195
#define V120_CONTROLBYTES 38,1219
#define V120_ENCAPBYTES 39,1247
#define V120_UNNUMBERED_ENCAPBYTES 40,1307
#define V120_K_VALUE	42,1363
#define V120_N200	43,1386
#define V120_MAX_SEQ	44,1406
#define V120_MAX_IFRAME_SIZE 45,1431
#define V120_LLI0_MASK	48,1497
#define V120_CR_MASK	49,1525
#define V120_LLI1_MASK	52,1582
#define V120_LLI_DEFAULT 55,1669
#define V120_CONTROL_IFRAME_MASK	57,1702
#define V120_CONTROL_MASK	58,1740
#define V120_CONTROL_S_FORMAT	59,1771
#define V120_CONTROL_U_FORMAT	60,1806
#define V120_S_FORMAT_SIZE	65,1907
#define V120_U_FORMAT_SIZE	66,1936
#define V120_SUPERVISORY_PF_MASK 68,1966
#define V120_SUPERVISORY_N_MASK 69,2004
#define V120_RR_VALUE	74,2078
#define V120_RNR_VALUE	75,2105
#define V120_REJ_VALUE	76,2133
#define V120_UNNUMBERED_PF_MASK	78,2162
#define V120_SABME_VALUE	83,2278
#define V120_DM_VALUE	84,2308
#define V120_UI_VALUE	85,2336
#define V120_DISC_VALUE	86,2364
#define V120_UA_VALUE	87,2394
#define V120_FRMR_VALUE	88,2422
#define V120_FRMR_F_VALUE	90,2453
#define V120_FRMR_W_MASK	91,2523
#define V120_FRMR_X_MASK	92,2553
#define V120_FRMR_Y_MASK	93,2583
#define V120_FRMR_Z_MASK	94,2613
#define V120_HEAD_EXT	99,2678
#define V120_HEAD_BREAK	100,2705
#define V120_HEAD_C2	101,2734
#define V120_HEAD_C1	102,2760
#define V120_HEAD_BF	103,2786
#define V120_HEAD_EXT_DR	105,2813
#define V120_HEAD_EXT_SR	106,2843
#define V120_HEAD_EXT_RR	107,2873
#define V120_HEAD_EXT_ALL 108,2903
enum v120_packet_type v120_packet_type115,3163
    V120_IFRAME	116,3187
    V120_RR	117,3233
    V120_RNR	118,3272
    V120_REJ	119,3316
    V120_SABME	120,3348
    V120_DM	121,3408
    V120_UI	122,3450
    V120_DISC	123,3497
    V120_UA	124,3534
    V120_FRMR	125,3585
    V120_BAD	126,3624
enum v120_states v120_states134,3851
    V120_TEI_ASSIGNED_STATE,135,3870
    V120_AWAITING_ESTABLISHMENT_STATE,136,3899
    V120_ESTABLISHED_STATE,137,3938
    V120_TIMER_RECOVERY_STATE,138,3966
    V120_AWAITING_RELEASE_STATE,139,3997
typedef struct v120_save_type v120_save_type146,4157
} v120_save_type;v120_save_type154,4369
struct v120_info_type_ v120_info_type_156,4388
static inline boolean v120_packet_is_command 185,5623
static inline uchar v120_get_nr 194,5830
static inline uchar v120_get_ns 206,6066
static inline uchar v120_iframe_pf 218,6358
#define v120_supervisory_pf 224,6502
static inline uchar v120_unnumbered_pf 230,6622
static inline ulong v120_lli_to_num 239,6848
static inline void v120_write_lli 254,7198
static inline void v120_write_supervisory_header 270,7741
static inline void v120_write_unnumbered_header 289,8376
static inline uchar v120_increment_sequence 304,8716
static inline void v120_stop_t200 316,8935
static inline void v120_start_t200 320,9024
static inline void v120_stop_t203 324,9123
static inline void v120_start_t203 328,9212
static inline boolean v120_t200_running 333,9319
