
if_c4000_mcn.h,2168
#define IF_C4000_MCN_H74,2647
#define BITS(BITS78,2706
#define BITL(BITL79,2765
#define MAX_MCN_SUBCONT_PER_SLOT 81,2824
#define C4000_MCN_UNITS 82,2865
#define MCN_REG_OFFSET 86,3016
#define MCN_COUNTER_REG_OFFSET	87,3055
#define MCN_M32_REG_OFFSET(MCN_M32_REG_OFFSET88,3093
#define MCN_MBRI_REG_OFFSET	89,3185
#define MCN_EMPTY 94,3276
#define MCN_PA_BRI_SINGLE 95,3350
#define MCN_PA_T1_U1 96,3416
#define MCN_PA_E1_U1 97,3480
#define MCN_PA_BRI_DUAL 98,3544
#define MCN_PA_T1 100,3612
#define MCN_PA_E1 101,3672
#define MCN_M32_RESET_MASK 106,3771
#define MCN_M32_ENABLE_MASK 107,3806
#define MCN_ADAPTER1_RESET_MASK 108,3841
#define MCN_ADAPTER1_ENABLE_MASK 109,3880
#define MCN_ADAPTER2_RESET_MASK 110,3919
#define MCN_ADAPTER2_ENABLE_MASK 111,3958
#define MCN_ENABLE_MASK	112,3997
#define MCN_LED_DISABLE 113,4034
typedef struct mcn_block mcn_block_t;mcn_block_t116,4107
struct mcn_block mcn_block117,4145
#define MCN_COUNTER_ENABLE 131,4636
#define MCN_COUNTER_DISABLE 132,4694
#define MCN_MAX_TIMER_ENTRIES 133,4753
typedef struct mcn_counter_block mcn_counter_block_t;mcn_counter_block_t138,4894
struct mcn_counter_block mcn_counter_block139,4948
typedef struct mcn_timerbuf mcn_timerbuf_t;mcn_timerbuf_t152,5481
struct mcn_timerbuf mcn_timerbuf153,5525
typedef struct m32_instance m32_instance_t;m32_instance_t164,5981
typedef struct mb_instance  mb_instance_t;mb_instance_t165,6025
typedef struct dsx1_instance  dsx1_instance_t;dsx1_instance_t166,6068
typedef struct m32_block    m32_block_t;m32_block_t168,6116
typedef struct mb_block     mb_block_t;mb_block_t169,6157
typedef struct mcn_instance mcn_instance_t;mcn_instance_t172,6235
typedef struct mcn_subinstance_t_ mcn_subinstance_t_174,6280
} mcn_subinstance_t;mcn_subinstance_t180,6558
struct mcn_instance mcn_instance182,6580
inline static void mcn_reset_m32(195,7048
#define SINGLE_SUBCONT 207,7359
#define MCN_INSTANCE(MCN_INSTANCE208,7393
#define M32_INSTANCE(M32_INSTANCE209,7436
#define MB_INSTANCE(MB_INSTANCE211,7579
#define DSX1_INSTANCE(DSX1_INSTANCE213,7726
#define DELAY_ACTION_REQUEST(DELAY_ACTION_REQUEST215,7872
