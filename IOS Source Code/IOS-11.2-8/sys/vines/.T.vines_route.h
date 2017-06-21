
vines_route.h,2979
typedef struct vroute_fn_data_ vroute_fn_data_81,2958
} vroute_fn_data;vroute_fn_data88,3128
} vroute_msg;vroute_msg96,3256
#define VINES_MAGIC_PATH 102,3404
#define VINES_MAGIC_NEIGHBOR 103,3444
#define VINES_MAGIC_ROUTE 104,3484
#define VINES_MAGIC_SERVER 105,3524
#define VINES_MAGIC_INVALID 106,3564
#define V_METRIC_FDDI	113,3777
#define V_METRIC_ETHER	114,3830
#define V_METRIC_16MBTR 115,3864
#define V_METRIC_4MBTR	116,3899
#define V_METRIC_CUTOFF 117,3933
#define V_METRIC_T1	118,3968
#define V_METRIC_56K	119,4019
#define V_METRIC_9600	120,4051
#define V_METRIC_4800	121,4084
#define V_METRIC_2400	122,4117
#define V_METRIC_1200	123,4170
#define V_METRIC_X25 125,4204
#define V_METRIC_BRIDGE 126,4239
#define VINES_INFINITY	128,4275
#define V_NET_ME	140,4501
#define V_NET_STATIC	141,4551
#define V_NET_RTP	142,4611
#define V_NET_IGRP	143,4666
#define V_NET_TEST	144,4723
#define V_NET_REDIRECT	145,4784
struct vinesroutetype_ vinesroutetype_155,5279
struct vinesnettype_ vinesnettype_180,6136
struct routingdecision_ routingdecision_197,6699
#define VRTP_DEF_WAKEUP_INTERVAL 211,7092
#define VRTP_DEF_UPDATE_COUNT 212,7137
#define VRTP_HELLOTIME(VRTP_HELLOTIME214,7173
#define VRTP_HELLOINVALID(VRTP_HELLOINVALID215,7237
#define VRTP_HELLOEXPIRE(VRTP_HELLOEXPIRE216,7301
#define VRTP_QUERYINTERVAL(VRTP_QUERYINTERVAL218,7383
#define VRTP_SUPPRESSTIME 220,7449
#define VRTP_HOLDTIME(VRTP_HOLDTIME222,7496
#define VRTP_MOVETIME(VRTP_MOVETIME223,7535
#define VRTP_ROUTEEXPIRE(VRTP_ROUTEEXPIRE224,7574
#define VRTP_INVALIDTIME(VRTP_INVALIDTIME225,7613
#define VRTP_CHANGETIME 228,7712
#define VRTP_FLASHUPDATE	230,7771
vroute_EntryKey 254,8378
vrouteCast 266,8572
vroute_UnprotectNetwork 273,8727
vroute_ProtectNetwork 283,8906
vroute_NetworkProtected 293,9103
vroute_GetNextNetwork 302,9311
vroute_GetFirstNetwork 321,9696
vroute_GetBestNetwork 331,9894
vroute_InsertEntry 341,10077
vroute_DeleteEntry 353,10331
vroute_ReinitNetwork 364,10565
vroute_CreateEntry 374,10732
vroute_FindEntry 404,11391
vroute_WalkTree 422,11731
vroute_WalkPartialTree 433,11983
vroute_FindSelf 443,12278
#define FOR_ALL_ROUTES(FOR_ALL_ROUTES463,12719
#define FOR_ALL_ROUTES_W_PREV(FOR_ALL_ROUTES_W_PREV466,12841
#define FOR_ALL_SERVERS(FOR_ALL_SERVERS472,13076
static inline boolean vmetric_valid 484,13416
static inline char *vmetric_to_seconds 492,13507
static inline char *vmetric_to_config 502,13685
static inline ushort vmetric_from_config 514,13924
static inline void vines_set_server_metrics(522,14059
static inline void vines_bump_changed_timer 539,14523
static inline void vines_set_changetime 546,14686
static inline void vines_set_suppresstime 552,14840
static inline vinesroutetype *vines_get_route 569,15336
static inline vinesroutetype *vines_get_route_by_idb 587,15760
static inline vinesroutetype *vines_get_route_by_gw 599,16028
static inline int vines_server_total_paths 611,16297
