
tunnel.h,894
#define __TUNNEL_H__52,1652
#define VISIBLE_TUNNEL_BANDWIDTH 56,1722
#define TUNNEL_DELAY 57,1778
#define	TUNNEL_HEADCALCTIME	66,2021
#define	TUNNEL_STOMPTIME	67,2063
#define	TUNNEL_BLINDTIME	68,2102
typedef	enum tunnel_mode_ tunnel_mode_70,2142
} tunnel_mode;tunnel_mode82,2486
#define	TUN_MODE_DEFAULT 84,2502
typedef enum tunnel_transport_ tunnel_transport_86,2544
} tunnel_transport;tunnel_transport89,2618
typedef paktype * (*encaps_t)encaps_t91,2639
typedef paktype * (*encaps_t)(tunnel_info *, paktype paktype91,2639
typedef void (*resync_t)resync_t92,2717
typedef void (*resync_t)(tunnel_info tunnel_info92,2717
typedef void (*cstate_encaps_t)cstate_encaps_t93,2796
typedef void (*cstate_encaps_t)(idbtype idbtype93,2796
struct tunnel_info_ tunnel_info_95,2875
#define FOR_ALL_TUNNELS(FOR_ALL_TUNNELS142,4753
#define TUNNEL_MYADDR(TUNNEL_MYADDR148,4887
