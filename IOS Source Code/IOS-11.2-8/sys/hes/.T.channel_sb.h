
channel_sb.h,994
#define __CHANNEL_SB_H__38,1193
#define NO_CHANNEL_VC 46,1274
enum CIP_VLAN_MEDIA CIP_VLAN_MEDIA53,1364
     CIP_VLAN_UNKNOWN 54,1386
     CIP_VLAN_ETHER 55,1414
     CIP_VLAN_TOKEN 56,1442
     CIP_VLAN_FDDI 57,1470
typedef ushort (*channel_ilan_vc_from_macaddr_t)channel_ilan_vc_from_macaddr_t65,1551
typedef ushort (*channel_ilan_vc_from_macaddr_t)(hwidbtype *, uchar *, uchar uchar65,1551
typedef hwidbtype *(*channel_ilan_hwidb_from_vc_t)channel_ilan_hwidb_from_vc_t66,1632
typedef hwidbtype *(*channel_ilan_hwidb_from_vc_t)(hwidbtype *, ushort)ushort66,1632
struct channel_sb_t_ channel_sb_t_82,1965
static inline channel_sb_t *channel_create_sb 108,2474
static inline channel_sb_t *channel_get_sb 154,3414
channel_sb_t *channel_get_or_create_sb 168,3692
static inline boolean channel_destroy_sb 185,3983
struct cip_idb_info *channel_sb_get_cip_info 216,4467
boolean channel_sb_cip_info_is_null 232,4757
enum CIP_VLAN_MEDIA channel_sb_get_emulated_media 247,5014
