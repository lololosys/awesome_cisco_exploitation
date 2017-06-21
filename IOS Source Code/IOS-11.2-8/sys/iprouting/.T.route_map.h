
route_map.h,4534
#define __ROUTE_MAP_H__92,3491
#define ROUTE_MAP_INIT_SEQ 97,3568
#define ROUTE_MAP_MAX_COMMUNITY 98,3598
struct route_map_headtype_ route_map_headtype_103,3699
typedef struct route_map_interface_ route_map_interface_115,3951
} route_map_interface;route_map_interface118,4066
typedef struct route_map_addrtype_ route_map_addrtype_120,4090
} route_map_addrtype;route_map_addrtype123,4195
typedef struct route_map_community_ route_map_community_128,4268
} route_map_community;route_map_community131,4368
typedef struct route_map_match_value_ route_map_match_value_133,4392
} route_map_match_value;route_map_match_value136,4515
typedef struct route_map_match_acl_ route_map_match_acl_138,4541
} route_map_match_acl;route_map_match_acl141,4658
typedef struct route_map_matchtype_ route_map_matchtype_143,4682
} route_map_matchtype;route_map_matchtype159,5326
#define ROUTE_MAP_INTERNAL 164,5393
#define ROUTE_MAP_EXTERNAL 165,5422
#define ROUTE_MAP_LEVEL1 166,5451
#define ROUTE_MAP_LEVEL2 167,5480
#define	ROUTE_MAP_LOCAL	168,5509
#define ROUTE_MAP_EX_TYPE1 169,5538
#define ROUTE_MAP_EX_TYPE2 170,5567
typedef struct route_map_metrictype_ route_map_metrictype_174,5651
} route_map_metrictype;route_map_metrictype187,5904
typedef struct route_map_dampening_ route_map_dampening_192,5960
}route_map_dampening;route_map_dampening197,6105
typedef struct route_map_settype_ route_map_settype_202,6179
} route_map_settype;route_map_settype226,6943
#define ROUTE_MAP_METRIC_SET 231,7034
#define ROUTE_MAP_METRICTYPE_SET 232,7079
#define ROUTE_MAP_TAG_SET 233,7124
#define ROUTE_MAP_AUTOTAG_SET 234,7169
#define ROUTE_MAP_LEVEL_SET 235,7214
#define ROUTE_MAP_DEST_SET 236,7259
#define ROUTE_MAP_LOCALPREF_SET 237,7304
#define ROUTE_MAP_WEIGHT_SET 238,7349
#define ROUTE_MAP_NEXT_HOP_SET 239,7394
#define ROUTE_MAP_ORIGIN_SET 240,7440
#define ROUTE_MAP_ASPATH_PND_SET 241,7485
#define	ROUTE_MAP_COMMUNITY_ADD	242,7530
#define ROUTE_MAP_ASPATH_TAG_SET 243,7575
#define ROUTE_MAP_DEFAULT_NEXT_HOP_SET 244,7620
#define ROUTE_MAP_DEFAULT_INTERFACE_SET 245,7671
#define ROUTE_MAP_INTERFACE_SET	246,7722
#define ROUTE_MAP_QOS_SET	247,7766
#define ROUTE_MAP_PRECEDENCE_SET	248,7804
#define ROUTE_MAP_COMMUNITY_SET	249,7848
#define ROUTE_MAP_DAMPENING_SET	250,7892
#define ROUTE_MAP_DESTPREF_SET 251,7936
#define ROUTE_MAP_METRIC_ADD	253,7988
#define ROUTE_MAP_METRIC_SUB	254,8023
#define ROUTE_MAP_MAX_ASPATH	255,8058
#define ROUTE_MAP_SET_TYPE1 260,8170
#define ROUTE_MAP_SET_TYPE2 261,8203
#define ROUTE_MAP_SET_INTERNAL 262,8236
#define ROUTE_MAP_SET_EXTERNAL 263,8269
#define ROUTE_MAP_SET_LEVEL1 264,8302
#define ROUTE_MAP_SET_LEVEL2 265,8335
#define ROUTE_MAP_SET_LEVEL12 266,8368
#define ROUTE_MAP_SET_STUB 267,8401
#define ROUTE_MAP_SET_BACKBONE 268,8434
struct route_maptype_ route_maptype_278,8777
#define ROUTE_MAP_MATCH_IPADDRESS 295,9258
#define ROUTE_MAP_MATCH_IPNEXTHOP 296,9306
#define ROUTE_MAP_MATCH_IPROUTESOURCE 297,9354
#define ROUTE_MAP_MATCH_CLNSADDRESS 298,9402
#define ROUTE_MAP_MATCH_CLNSNEXTHOP 299,9450
#define ROUTE_MAP_MATCH_CLNSROUTESOURCE 300,9498
#define ROUTE_MAP_MATCH_INTERFACE 301,9546
#define ROUTE_MAP_MATCH_METRIC 302,9595
#define ROUTE_MAP_MATCH_ROUTETYPE 303,9643
#define ROUTE_MAP_MATCH_TAG 304,9698
#define ROUTE_MAP_MATCH_ASPATH 305,9746
#define ROUTE_MAP_MATCH_COMMUNITY	306,9794
#define ROUTE_MAP_MATCH_EXACT_COMMUNITY	307,9837
#define ROUTE_MAP_MATCH_LENGTH	308,9885
#define ROUTEMAP_MATCH_COMMARGS_BEGIN	313,9984
#define ROUTEMAP_MATCH_COMMARGS	314,10024
#define ROUTE_MAP_ADD	320,10117
#define ROUTE_MAP_DELETE	321,10142
#define ROUTE_MAP_MODIFY	322,10169
struct route_map_nametype_ route_map_nametype_324,10197
#define ROUTE_MAP_IP_CONNECTED_MATCHES 336,10397
#define ROUTE_MAP_IP_STATIC_MATCHES 339,10499
#define ROUTE_MAP_OSPF_MATCHES 343,10664
#define ROUTE_MAP_IGRP1_MATCHES 348,10921
#define ROUTE_MAP_IGRP2_MATCHES 353,11129
#define ROUTE_MAP_RIP_MATCHES 358,11387
#define ROUTE_MAP_HELLO_MATCHES 360,11444
#define ROUTE_MAP_EGP_MATCHES 362,11501
#define ROUTE_MAP_BGP_MATCHES 364,11558
#define ROUTE_MAP_BGP_NETWORK_MATCHES 368,11731
#define ROUTE_MAP_BGP_ATTRIBUTE_MATCHES 372,11899
#define ROUTE_MAP_CLNS_STATIC_MATCHES 375,11999
#define ROUTE_MAP_ISIS_MATCHES 379,12144
#define ROUTE_MAP_ISO_IGRP_MATCHES 386,12520
#define ROUTE_MAP_ODR_MATCHES 391,12731
typedef struct route_map_userinfo_ route_map_userinfo_398,12931
} route_map_userinfo;route_map_userinfo403,13050
