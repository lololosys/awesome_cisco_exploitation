
srb_multiring.h,1455
#define __SRB_MULTIRING_H__26,827
#define MULTIRING_BIT_NONE 31,881
#define MULTIRING_BIT_APOLLO 32,921
#define MULTIRING_BIT_APPLE 33,961
#define MULTIRING_BIT_CLNS 34,1001
#define MULTIRING_BIT_DECNET 35,1041
#define MULTIRING_BIT_IP 36,1081
#define MULTIRING_BIT_NOVELL 37,1121
#define MULTIRING_BIT_OTHER 38,1161
#define MULTIRING_BIT_VINES 39,1201
#define MULTIRING_BIT_XNS 40,1241
#define MULTIRING_BITS_ALL 41,1281
static inline ulong srb_get_multiring_bits 44,1323
static inline void srb_set_multiring_bits 50,1428
static inline ulong srb_get_allroutes_bits 55,1542
static inline void srb_set_allroutes_bits(61,1647
static inline boolean srb_multiring_any_enabled(67,1761
static inline boolean srb_multiring_none_enabled(73,1892
static inline boolean srb_multiring_apollo_enabled(79,2024
static inline boolean srb_multiring_apple_enabled(85,2159
static inline boolean srb_multiring_clns_enabled(91,2292
static inline boolean srb_multiring_decnet_enabled(97,2423
static inline boolean srb_multiring_ip_enabled(103,2558
static inline boolean srb_multiring_novell_enabled(109,2685
static inline boolean srb_multiring_other_enabled(115,2820
static inline boolean srb_multiring_vines_enabled(121,2953
static inline boolean srb_multiring_xns_enabled(127,3086
static inline boolean srb_multiring_all_enabled(133,3215
static inline void srb_multiring_enable_all(139,3345
static inline void srb_multiring_disable_all(144,3462
