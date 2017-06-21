
if_as5200_mcn.h,1703
#define IF_AS5200_MCN_H52,1849
#define BITS(BITS56,1895
#define BITL(BITL57,1954
#define MAX_DTMF_UNIT_PER_SLOT 59,2013
#define MAX_MCN_SUBCONT_PER_SLOT 60,2054
#define AS5200_MCN_UNITS 61,2095
#define MCN_REG_OFFSET 65,2248
#define MCN_M32_REG_OFFSET(MCN_M32_REG_OFFSET66,2291
#define MCN_EMPTY 71,2414
#define MCN_PA_T1_U2 72,2491
#define MCN_PA_E1_U2 73,2569
#define MCN_PA_T1 75,2648
#define MCN_PA_E1 76,2710
#define MCN_M32_RESET_MASK 81,2811
#define MCN_M32_ENABLE_MASK 82,2846
#define MCN_ENABLE_MASK	83,2881
#define MCN_FRAMING_ENABLE_MASK 84,2922
#define MCN_LED_DISABLE 85,2963
#define MCN_M32_1_INTERRUPRT 87,3005
#define MCN_M32_2_INTERRUPRT 88,3045
#define MCN_IDLE_STATE	94,3193
#define MCN_PRI_IN_LOOP_STATE	95,3219
#define MCN_SEC_IN_LOOP_STATE	96,3251
typedef struct mcn_block mcn_block_t;mcn_block_t99,3319
struct mcn_block mcn_block100,3357
typedef struct m32_instance m32_instance_t;m32_instance_t111,3855
typedef struct dsx1_instance  dsx1_instance_t;dsx1_instance_t112,3899
typedef struct m32_block    m32_block_t;m32_block_t113,3946
typedef struct dtmf_instance  dtmf_instance_t;dtmf_instance_t114,3987
typedef struct mcn_instance mcn_instance_t;mcn_instance_t117,4072
typedef struct mcn_subinstance_t_ mcn_subinstance_t_119,4117
} mcn_subinstance_t;mcn_subinstance_t124,4320
struct mcn_instance mcn_instance126,4342
inline static void mcn_reset_m32(140,4881
#define SINGLE_SUBCONT 152,5193
#define MCN_INSTANCE(MCN_INSTANCE153,5227
#define M32_INSTANCE(M32_INSTANCE154,5270
#define DSX1_INSTANCE(DSX1_INSTANCE157,5496
#define DTMF_INSTANCE(DTMF_INSTANCE160,5725
#define DELAY_ACTION_REQUEST(DELAY_ACTION_REQUEST164,5940
