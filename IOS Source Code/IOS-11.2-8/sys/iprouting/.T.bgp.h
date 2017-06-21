
bgp.h,1317
#define __BGP_H__52,1778
#define BGP_VERSION	59,1894
#define BGP_MAX_VERSION 60,1916
#define BGP_MIN_VERSION 61,1942
#define	BGP_MIN_WEIGHT	67,2037
#define	BGP_MAX_WEIGHT	68,2065
#define	BGP_SOURCED_WEIGHT 69,2101
#define	BGP_LEARNED_WEIGHT 70,2134
#define BGP_MAX_COMMONADMIN 71,2163
#define ORIGIN_IGP 77,2260
#define ORIGIN_EGP 78,2289
#define ORIGIN_INCOMPLETE 79,2318
typedef enum bgp_show_parser_cons_ bgp_show_parser_cons_84,2399
} bgp_show_parser_cons;bgp_show_parser_cons96,2636
typedef enum bgp_clear_parser_cons_ bgp_clear_parser_cons_98,2661
} bgp_clear_parser_cons;bgp_clear_parser_cons110,2970
#define BGP_SHOW_COMCOUNT 112,2996
#define	BGP_DEF_LOCALPREF	117,3088
#define	BGP_FAST_RESET	118,3116
#define	BGP_REDIST_IBGP	119,3142
#define BGP_ROUTER_ID	120,3169
#define BGP_ALWAYS_COMPARE_MED	121,3194
#define	BGP_CONFED_ID	122,3227
#define	BGP_CLUSTER_ID	123,3252
#define BGP_RMAP_DAMPENING	124,3285
#define BGP_CLIENT_REFLECTION 125,3314
#define BGP_DPA_ENABLED	126,3348
#define BGP_DETERM_MED	127,3375
typedef enum bgp_show_neighbor_cons_ bgp_show_neighbor_cons_131,3453
} bgp_show_neighbor_cons;bgp_show_neighbor_cons137,3611
#define BGP_MAX_HALFLIFE	142,3702
#define BGP_MAX_PENALTY	143,3730
#define BGP_TRAP_ESTAB	148,3829
#define BGP_TRAP_BACKTRANS	149,3855
