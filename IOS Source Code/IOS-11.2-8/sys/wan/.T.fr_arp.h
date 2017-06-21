
fr_arp.h,2639
enum FR_ARPTYPE FR_ARPTYPE41,1190
    FR_CISCO_ARP,42,1208
    FR_INARP,43,1226
    FR_RFC_ARP,44,1240
    MAX_FR_ARPTYPE_COUNT45,1256
#define FR_PR_NONE 48,1285
#define FR_PR_IP 49,1306
#define FR_PR_IP_INHERIT_COMPR 50,1329
#define FR_PR_IP_NP_COMPR 51,1378
#define FR_PR_IP_P_COMPR 52,1443
#define FR_PR_TCP_COMP 53,1503
#define FR_PR_NP_TCP_COMPR 54,1561
#define FR_PR_P_TCP_COMPR 55,1618
#define FR_PR_DECNET 56,1679
#define FR_PR_APOLLO 57,1706
#define FR_PR_XNS 58,1768
#define FR_PR_NOVELL 59,1795
#define FR_PR_APPLETALK 60,1822
#define FR_PR_VINES 61,1852
#define FR_PR_CLNS 62,1878
#define FR_ARPBYTES 63,1903
#define FR_OP_REQ 64,1952
#define FR_OP_REP 65,1972
#define FR_SERIAL 66,1992
#define FR_PL 67,2012
# define FR_IP_IN_ARP_ENABLED 72,2062
# define FR_IPX_IN_ARP_ENABLED 73,2100
# define FR_AT_IN_ARP_ENABLED 74,2138
# define FR_XNS_IN_ARP_ENABLED 75,2176
# define FR_VINES_IN_ARP_ENABLED 76,2214
# define FR_DNET_IN_ARP_ENABLED 77,2252
# define FR_ALL_IN_ARP_ENABLED 78,2290
#define FR_INARP_DLCI_GET 82,2476
#define FR_INARP_DLCI_SET 83,2504
#define FR_INARP_DLCI_GETORSET 84,2532
#define FR_ARP_INT_GET 85,2565
#define FR_ARP_INT_SET 86,2590
#define FR_ARP_INT_GETORSET 87,2615
#define FR_ARP_INTV 88,2645
struct fr_idb_arp fr_idb_arp90,2668
typedef struct fr_arptype_ fr_arptype_103,3100
} fr_arptype;fr_arptype113,3642
#define DEFAULT_INARP_INTERVAL 119,3703
#define AT_INARPBYTES 120,3737
#define XNS_INARPBYTES 121,3763
#define VINES_INARPBYTES 122,3789
#define DNET_INARPBYTES 123,3817
# define FR_ARHRD 129,3881
# define FR_ARHLN 130,3916
# define FR_INARP_REQ 131,3949
# define FR_INARP_REPLY 132,3982
#define RFBYTES 133,4015
typedef struct fr_rfc_arp_ fr_rfc_arp_135,4049
} fr_rfc_arp;fr_rfc_arp145,4593
#define fr_rfc_arp_headstart(fr_rfc_arp_headstart147,4608
typedef struct fr_xns_inarptype_ fr_xns_inarptype_149,4666
} fr_xns_inarptype;fr_xns_inarptype159,5170
typedef struct fr_novell_inarptype_ fr_novell_inarptype_161,5191
} fr_nov_inarptype;fr_nov_inarptype171,5696
typedef struct fr_at_inarptype_ fr_at_inarptype_173,5717
} fr_at_inarptype;fr_at_inarptype183,6197
typedef struct fr_decnet_inarptype_ fr_decnet_inarptype_185,6217
} fr_decnet_inarptype;fr_decnet_inarptype195,6675
typedef struct fr_vines_inarptype_ fr_vines_inarptype_197,6699
} fr_vines_inarptype;fr_vines_inarptype207,7205
typedef union fr_arp_sizer_ fr_arp_sizer_209,7228
} fr_arp_sizer;fr_arp_sizer218,7418
#define FR_ARP_MAX_SIZE 220,7435
static inline void fr_link_inarp_timer(226,7549
static inline mgd_timer *fr_get_inarp_timer(236,7793
