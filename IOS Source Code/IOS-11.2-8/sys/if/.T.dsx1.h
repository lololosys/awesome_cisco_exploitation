
dsx1.h,5133
#define __DSX1_H__81,3093
#define	T1_MAXCHAN	85,3184
#define	E1_MAXCHAN	86,3206
#define	T1_MAXTS	87,3228
#define	E1_MAXTS	88,3248
#define DSX1_MAX_CHAN	90,3269
#define DSX1_MAX_TS	91,3302
#define E1_ISDN_D_CHANNEL	93,3332
#define E1_ISDN_D_TIMESLOT	94,3361
#define T1_ISDN_D_CHANNEL	95,3391
#define T1_ISDN_D_TIMESLOT	96,3428
#define ISDN_TS_TO_CHAN_GROUP(ISDN_TS_TO_CHAN_GROUP98,3465
#define FOR_ALL_DSX1_CHANNELS(FOR_ALL_DSX1_CHANNELS100,3507
#define FOR_ALL_DSX1_TIMESLOTS(FOR_ALL_DSX1_TIMESLOTS103,3602
#define DEFAULT_DSX1_CTRLR_SLOTS 110,3797
#define DEFAULT_DSX1_CTRLR_INTERFACES 111,3842
#define DEFAULT_DSX1_CTRLR_UNITS 112,3908
#define DEFAULT_DSX1_CTRLR_SLOTS 114,3980
#define DEFAULT_DSX1_CTRLR_INTERFACES 115,4017
#define DEFAULT_DSX1_CTRLR_UNITS 116,4064
#define UNUSED_CHAN 119,4141
enum appl_type appl_type122,4238
    APPL_T1 123,4255
    APPL_E1_UNBAL 124,4309
    APPL_E1_BAL 125,4332
    APPL_6B 126,4353
    APPL_T1_CSU 127,4371
    APPL_NONE 128,4425
    APPL_NONE = 0xf,128,4425
enum dsx1_framing_type dsx1_framing_type134,4505
    DSX1_FRM_ESF 135,4530
    DSX1_FRM_SF 136,4552
    DSX1_FRM_E1 137,4573
    DSX1_FRM_E1_CRC 138,4594
enum dsx1_clocking_type dsx1_clocking_type140,4622
    DSX1_CLK_LINE 141,4648
    DSX1_CLK_INTERNAL 142,4671
    DSX1_CLK_THRU 143,4698
    DSX1_CLK_FREE 144,4721
enum dsx1_linecode_type dsx1_linecode_type146,4778
    DSX1_LC_B8ZS 147,4804
    DSX1_LC_HDB3 148,4826
    DSX1_LC_AMI 149,4848
enum dsx1_speed_type dsx1_speed_type151,4872
    DSX1_SPEED_8 152,4895
    DSX1_SPEED_8  = 0x01,152,4895
    DSX1_SPEED_16 153,4921
    DSX1_SPEED_16 = 0x03,153,4921
    DSX1_SPEED_24 154,4947
    DSX1_SPEED_24 = 0x07,154,4947
    DSX1_SPEED_32 155,4973
    DSX1_SPEED_32 = 0x0f,155,4973
    DSX1_SPEED_40 156,4999
    DSX1_SPEED_40 = 0x1f,156,4999
    DSX1_SPEED_48_I460 157,5025
    DSX1_SPEED_48_I460 = 0x3f,157,5025
    DSX1_SPEED_48 158,5056
    DSX1_SPEED_48 = 0x7e,158,5056
    DSX1_SPEED_56 159,5107
    DSX1_SPEED_56 = 0x7f,159,5107
    DSX1_SPEED_64 160,5171
    DSX1_SPEED_64 = 0xff,160,5171
enum dsx1_loopback_config_type dsx1_loopback_config_type163,5261
    DSX1_NO_LOOP 164,5294
    DSX1_PAYLOAD_LOOP 165,5316
    DSX1_LINE_LOOP 166,5343
    DSX1_OTHER_LOOP 167,5367
enum dsx1_loop_type dsx1_loop_type169,5395
    DSX1_LOOP_NONE 170,5417
    DSX1_LOOP_LOCAL 171,5443
    DSX1_LOOP_REMOTE 172,5469
enum dsx1_cable_length dsx1_cable_length174,5498
    DSX1_LENGTH_133 175,5523
    DSX1_LENGTH_266 176,5548
    DSX1_LENGTH_399 177,5573
    DSX1_LENGTH_533 178,5598
    DSX1_LENGTH_655 179,5623
enum dsx1_clocksource_type dsx1_clocksource_type182,5652
    DSX1_PRIMARY 183,5681
    DSX1_SECONDARY 184,5703
    DSX1_THIRD 185,5727
    DSX1_FOURTH 186,5747
    DSX1_FIFTH 187,5768
    DSX1_SIXTH 188,5788
    DSX1_SEVENTH 189,5808
    DSX1_EIGHTH 190,5830
enum dsx1_pulse_gain_type dsx1_pulse_gain_type193,5855
    DSX1_0DB 194,5883
    DSX1_75DB 195,5901
    DSX1_15DB 196,5920
    DSX1_225DB 197,5939
enum dsx1_length_type dsx1_length_type200,5963
    DSX1_SHORT 201,5987
    DSX1_LONG 202,6007
enum dsx1_gain_type dsx1_gain_type205,6030
    DSX1_GAIN26 206,6052
    DSX1_GAIN36 207,6073
enum dsx1_sig_type dsx1_sig_type210,6098
    DSX1_SIGTYPE_NOSIG 211,6119
    DSX1_SIGTYPE_EM,212,6147
enum dsx1_homologation_type dsx1_homologation_type219,6299
    DSX1_HOMOL_NORMAL,220,6329
    DSX1_HOMOL_AUSTRALIA,221,6352
enum dsx1_fdl_type dsx1_fdl_type223,6381
    DSX1_FDL_ANSI 224,6402
    DSX1_FDL_ATT 225,6425
    DSX1_FDL_NONE 226,6447
typedef unsigned long	Gauge;Gauge229,6474
typedef unsigned short	SmGauge;SmGauge230,6503
#define MAX_GAUGE_COUNT 232,6536
static inline void ADD_TO_GAUGE 234,6572
typedef struct dsx1TableType_ dsx1TableType_247,6930
} dsx1TableType;dsx1TableType260,7226
typedef struct dsx1TotalTableType_ dsx1TotalTableType_262,7244
} dsx1TotalTableType;dsx1TotalTableType273,7480
typedef struct dsx1ConfigTable_ dsx1ConfigTable_279,7551
} dsx1ConfigTable;dsx1ConfigTable293,8027
#define DSX1_NO_ALARM	295,8047
#define DSX1_RX_REM_ALARM	296,8076
#define DSX1_TX_REM_ALARM	297,8108
#define DSX1_RX_AIS	298,8140
#define DSX1_TX_AIS	299,8167
#define DSX1_RX_LOF	300,8194
#define DSX1_RX_LOS	301,8221
#define DSX1_LOOPED	302,8248
#define DSX1_TS16_AIS	303,8275
#define DSX1_RX_LOMF	304,8304
#define DSX1_TX_LOMF	305,8332
#define DSX1_HAVE_ERRORS 306,8360
#define DSX1_ALL_RX 309,8401
#define DSX1_ALL_TX 311,8523
#define QUARTER_HOURS_IN_DAY 317,8725
typedef struct dsx1MIBDataTable_ dsx1MIBDataTable_319,8758
} dsx1MIBDataTable;dsx1MIBDataTable325,9041
typedef struct dsx1info_ dsx1info_330,9122
}dsx1info;dsx1info379,11042
static inline boolean is_t1_cas(381,11054
static inline boolean is_t1_digit_channel(386,11184
#define DSX1_RBS_ESF_BITA 392,11362
#define DSX1_RBS_ESF_BITB 393,11397
#define DSX1_RBS_ESF_BITC 394,11432
#define DSX1_RBS_ESF_BITD 395,11467
#define DSX1_RBS_SF_BITA 397,11503
#define DSX1_RBS_SF_BITB 398,11538
static inline dsx1info *dsx1_find_by_unit 404,11661
