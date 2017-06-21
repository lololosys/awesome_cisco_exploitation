
traffic_shape.h,1491
#define TRAFFIC_SHAPE_H__68,2685
#define	DEFAULT_SHAPE_INTERVAL	73,2792
#define	TARGET_SHAPE_INTERVAL_MAX	74,2847
#define	TARGET_SHAPE_INTERVAL_MIN	75,2904
#define	SHAPE_TARGET_BYTES	76,2960
#define	SHAPE_MIN_BITS_IN_INTERVAL	77,2993
#define	SHAPE_MIN_INTERVAL	78,3033
#define TRAFFIC_SHAPE_NOT_SUPPORTED 80,3065
enum traffic_shape_timer_types traffic_shape_timer_types86,3260
    TIMER_START_SHAPING_INTERVAL87,3293
#define FR_FECN_BIT 106,4009
struct shaping_queue_ shaping_queue_117,4262
struct shape_fair_queue_ shape_fair_queue_125,4496
struct traffic_shape_t_ traffic_shape_t_142,5421
enum traffic_shape_commands traffic_shape_commands223,8795
enum traffic_shape_commands {TRAFFIC_SHAPE_RATE,223,8795
enum traffic_shape_commands {TRAFFIC_SHAPE_RATE, TRAFFIC_SHAPE_GROUP 223,8795
shaping_queue *shaping_queue_address 226,8882
#define SHAPING_RECENT_BECN 260,9835
#define SHAPING_BECN_DECREASE_RATE 261,9868
#define SHAPING_BECN_MAINTAIN_RATE 271,10291
void traffic_shape_becn_adjust(279,10464
#define FR_NONDATA_DLCI 317,11809
#define FR_DLC_MSB_MASK 318,11838
#define FR_DLC_LSB_MASK 319,11872
static inline ushort fr_get_vc_num(324,11965
void traffic_shape_defer_sending 340,12300
ulong traffic_shape_adjust_size 362,12940
static inline ushort fr_dlci_to_vc_num(393,13502
void traffic_shape_timeslice 405,13816
traffic_shape_t *traffic_shape_required_inline 531,17923
void traffic_shape_credit 640,21275
void traffic_shape_apply_credit 659,21819
