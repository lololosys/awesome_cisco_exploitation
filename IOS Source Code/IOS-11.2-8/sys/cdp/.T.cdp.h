
cdp.h,3047
#define __CDP_H__108,3656
#define cdp_pabuginf 110,3675
#define cdp_evbuginf 111,3722
#define cdp_adjbuginf 112,3771
#define cdp_ipbuginf 113,3819
#define CDP_MAX_XMIT_INTERVAL 119,3901
#define CDP_MIN_XMIT_INTERVAL 120,3976
#define CDP_DEF_XMIT_INTERVAL 121,4051
#define CDP_MAX_TTL 123,4132
#define CDP_MIN_TTL 124,4201
#define CDP_DEF_TTL 125,4269
#define CDP_MAX_DATA_SIZE 127,4348
#define CDP_VERSION 129,4421
#define CDP_INPUT 133,4469
#define CDP_OUTPUT 134,4507
#define CDP_HDRSYNTAX 135,4557
#define CDP_CHECKSUM 136,4608
#define CDP_ENCAPSFAILED 137,4662
#define CDP_BADPACKET 138,4715
#define CDP_NOMEM 139,4772
#define CDP_FRAG 140,4826
#define CDP_MAXTRAFFIC 142,4877
#define CDP_LONGTIME 144,4927
#define CDP_NEXT_ACTIVATION 145,4987
typedef struct cdp_hdrtype_ cdp_hdrtype_150,5085
} cdp_hdrtype;cdp_hdrtype155,5329
#define CDP_MIN_HEADER_LENGTH 157,5345
#define CDP_HEADER_OVERHEAD 158,5412
#define cdpheadstart(cdpheadstart159,5442
} cdp_info_type;cdp_info_type169,5643
#define CDP_TLV_OVERHEAD 171,5661
#define CDP_DEVICE_NAME 175,5761
#define CDP_ADDRESS_TYPE 176,5829
#define CDP_PORT_ID_TYPE 177,5909
#define CDP_CAPABILITIES_TYPE 178,5987
#define CDP_VERSION_TYPE 179,6068
#define CDP_PLATFORM_TYPE 180,6122
#define CDP_IPPREFIX_TYPE 181,6201
#define CDP_MAX_TYPE 186,6331
#define CDP_TYPE_KNOWN(CDP_TYPE_KNOWN188,6371
#define CDP_NLPID_FORMAT 234,8013
#define CDP_8022_FORMAT 235,8040
#define CDP_IP_NLPID 237,8067
#define CDP_CLNS_NLPID 238,8093
#define CDP_NLPID_LEN 240,8122
#define CDP_8022_FORMAT_LEN 241,8146
#define CDP_ADDR_LEN_FIELD_LEN 242,8188
#define CDP_NLPID_INFO_OVERHEAD 245,8291
#define CDP_8022_INFO_OVERHEAD 247,8399
#define CDP_MAX_PORT_ID_STR_LEN 256,8664
#define CDP_ROUTER_DEVICE 265,8824
#define CDP_TB_DEVICE 266,8855
#define CDP_SRB_DEVICE 267,8882
#define CDP_SWITCH_DEVICE 268,8910
#define CDP_HOST_DEVICE 269,8941
#define CDP_IGMP_DEVICE 270,8970
#define CDP_REPEATER_DEVICE 271,8999
#define CDP_MAX_CAPABILITIES 273,9033
#define CDP_MAX_PLATFORM_STR_LEN 281,9223
typedef struct cdp_addr_info_ cdp_addr_info_288,9438
} cdp_addr_info;cdp_addr_info291,9560
#define CDP_ADDRESS_OVERHEAD 293,9579
typedef struct cdp_addr_structure_ cdp_addr_structure_295,9643
} cdp_addr_structure;cdp_addr_structure299,9754
#define CDP_ADDRSTRUCT_OVERHEAD 301,9777
#define CDP_CAPAB_FIELD_LEN 303,9837
typedef struct cdp_cache_type_ cdp_cache_type_308,9933
} cdp_cache_type;cdp_cache_type320,10735
typedef struct cdp_entry_link_ cdp_entry_link_322,10754
} cdp_entry_link;cdp_entry_link325,10848
typedef unsigned char	cdp_hash_type;cdp_hash_type331,10906
#define CDP_DEFAULT_DISABLE_INT(CDP_DEFAULT_DISABLE_INT349,11706
#define	CDP_DEFAULT_ENABLE_INT(CDP_DEFAULT_ENABLE_INT353,11857
typedef boolean (*cdp_add_func_t)cdp_add_func_t357,11974
typedef boolean (*cdp_add_func_t)(cdp_info_type *, idbtype *, ushort ushort357,11974
typedef void (*cdp_decode_func_t)cdp_decode_func_t358,12047
				     boolean)boolean359,12116
