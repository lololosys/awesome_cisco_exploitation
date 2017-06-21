
if_vip_s4t68360.h,1486
#define S4T_POWERUP 127,4462
#define S4T_STARTUP 128,4529
#define S4T_TX_RING_ENTRIES	136,4673
#define S4T_TX_RING_INDEX_MASK	137,4706
#define S4T_RX_RING_ENTRIES	143,4824
#define S4T_RX_RING_INDEX_MASK	144,4856
#define S4T_TX_RING_ENTRIES	152,4992
#define S4T_TX_RING_INDEX_MASK	153,5024
#define S4T_RX_RING_ENTRIES	159,5140
#define S4T_RX_RING_INDEX_MASK	160,5172
#define S4T_TXPOLL_HIGH	167,5295
#define S4T_TXPOLL_LOW	168,5345
#define S4T_RX_RING_ATTRIBUTE	173,5430
#define S4T_FAST_PARTICLE_POOL	178,5510
#define S4T_SLOW_PARTICLE_POOL	179,5566
#define S4T_PARTICLE_SIZE	186,5742
#define S4T_RX_MAX_SPIN	191,5835
#define S4T_TX_MAX_SPIN	192,5880
#define S4T_BUF_ADDR(S4T_BUF_ADDR197,5970
#define S4T_CNTL_ADDR(S4T_CNTL_ADDR198,6016
#define S4T_MAX_PORTS 200,6056
#define S4T_MAX_MTU 202,6081
#define S4T_MIN_MTU 203,6141
#define S4T_PCIMEM_CACHE_ATTRIBUTE 204,6200
#define S4T_DS_SIZE 205,6251
#define S4T_DSCRPTR_SIZE 206,6296
              S4T_PORT_3 = 3} S4T_PORTS;S4T_PORTS212,6452
#define S4T_PKT_STATS	217,6562
#define S4T_THROTTLING	222,6670
#define SERIAL_QUEUE_POLICY 223,6706
static inline particletype *s4t_get_particle_inline 231,6848
static inline void s4t_fastsend_stats 268,7578
static inline boolean s4t_dfs_enabled 281,7950
static inline boolean s4t_ip_dfs 290,8133
static inline boolean s4t_encap_dfs_switchable 301,8370
static inline void s4t_process_receive_packet 313,8634
static inline void s4t_output_enqueue 357,9779
