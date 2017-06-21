
pim.h,4797
#define __PIM_H__127,4719
#define PIM_VERSION1 132,4770
#define PIM_DENSE_MODE 133,4805
#define PIM_SPARSE_MODE 134,4831
#define PIM_SPARSE_DENSE_MODE 135,4857
#define PIMV1 137,4890
#define PIMV2 138,4908
#define PIM_QUERY_INTERVAL 140,4927
#define PIM_MESSAGE_INTERVAL 141,4971
#define PIM_STACK_SIZE 142,5010
#define PIM_PERIODIC_INTVL 143,5045
#define PIM_MULTIPLIER 144,5082
#define PIM_PRUNE_DELAY 145,5114
#define PIM_JOIN_DELAY_MAX 146,5170
#define PIM_GRAFT_FREQUENCY 147,5202
#define PIM_ASSERT_FREQUENCY 148,5245
#define PIM_RP_REACH_INTERVAL 149,5288
#define PIM_RP_REACH_HOLDTIME 150,5332
#define PIM_RP_EXPIRES 151,5390
#define PIM_BRING_UP_RP 152,5433
#define PIM_NULL_REG_INTERVAL 153,5477
#define PIM_JITTER_PCT 154,5521
#define PRM_JITTER_PCT	155,5553
#define PIM_INFINITE_THRESHOLD 156,5584
#define PIM_RP_ANNOUNCE_HOLDTIME 157,5618
#define PIM_RP_DISCOVERY_HOLDTIME 158,5689
#define PIM_RP_ANNOUNCE_INTVL 159,5759
#define PIM_RP_MAPPING_INTVL 160,5798
#define PIM_QUERY_CODE 162,5838
#define PIM_REGISTER_CODE 163,5871
#define PIM_REGISTER_STOP_CODE 164,5904
#define PIM_JOIN_PRUNE_CODE 165,5937
#define PIM_RP_REACHABLE_CODE 166,5970
#define PIM_ASSERT_CODE 167,6003
#define PIM_GRAFT_CODE 168,6036
#define PIM_GRAFT_ACK_CODE 169,6069
#define PIMV2_HELLO_TYPE 174,6141
#define PIMV2_REGISTER_TYPE 175,6174
#define PIMV2_REG_STOP_TYPE 176,6207
#define PIMV2_JOIN_PRUNE_TYPE 177,6240
#define PIMV2_BOOTSTRAP_TYPE 178,6273
#define PIMV2_ASSERT_TYPE 179,6306
#define PIMV2_GRAFT_TYPE 180,6339
#define PIMV2_GRAFT_ACK_TYPE 181,6372
#define PIMV2_CAND_RP_ADV_TYPE 182,6405
#define PIM_AUTORP_V1	184,6439
#define PIM_RP_ANNOUNCE_TYPE 185,6470
#define PIM_RP_DISCOVERY_TYPE 186,6503
#define PIM_QUERY_SIZE 191,6574
#define PIM_REGISTER_STOP_SIZE 192,6607
#define PIMV2_REGISTER_SIZE 194,6641
#define PIMV2_REGISTER_STOP_SIZE 195,6715
#define PIMV2_NULL_REGISTER_BIT 197,6791
#define HOST_MASK_LEN 198,6871
typedef struct pimtype_ pimtype_200,6937
} pimtype;pimtype209,7529
typedef struct pim_grouptype_ pim_grouptype_211,7541
} pim_grouptype;pim_grouptype216,7871
typedef struct pim_addrtype_ pim_addrtype_218,7889
} pim_addrtype;pim_addrtype225,8344
#define PIM_ADDRTYPE_SIZE 231,8458
typedef struct pim_rpreach_type_ pim_rpreach_type_236,8533
} pim_rpreach_type;pim_rpreach_type242,8694
typedef struct pim_asserttype_ pim_asserttype_247,8762
} pim_asserttype;pim_asserttype253,8930
#define PIM_ASSERT_RP_BIT 255,8949
#define PIM_ASSERT_INFINITY 256,8988
typedef struct pimv2_header_ pimv2_header_261,9077
} pimv2_header;pimv2_header266,9273
struct pim_nbrtype_ pim_nbrtype_271,9325
typedef struct pim_parmtype_ pim_parmtype_282,9576
} pim_parmtype;pim_parmtype290,9939
typedef struct pim_spttype_ pim_spttype_295,10006
} pim_spttype;pim_spttype299,10233
typedef struct pim_entrytype_ pim_entrytype_304,10294
} pim_entrytype;pim_entrytype309,10589
typedef struct pim_rpfilter_ pim_rpfilter_314,10655
} pim_rpfilter;pim_rpfilter319,10822
typedef struct pim_rp_announce_type_ pim_rp_announce_type_324,10887
} pim_rp_announce_type;pim_rp_announce_type331,11342
typedef struct pim_rp_afilter_type_ pim_rp_afilter_type_333,11367
} pim_rp_afilter_type;pim_rp_afilter_type337,11503
typedef struct pim_autorp_header_ pim_autorp_header_343,11597
} pim_autorp_header;pim_autorp_header350,12022
typedef struct pim_autorp_block_ pim_autorp_block_355,12112
} pim_autorp_block;pim_autorp_block360,12449
#define AUTORP_GET_VER(AUTORP_GET_VER365,12544
typedef struct pim_encoded_group_type_ pim_encoded_group_type_370,12647
} pim_encoded_group;pim_encoded_group374,12869
typedef struct pim_rp_mapping_ pim_rp_mapping_379,12937
} pim_rp_mapping;pim_rp_mapping388,13458
typedef struct pim_rptype_ pim_rptype_393,13541
} pim_rptype;pim_rptype406,14420
#define PIM_DEBUG(PIM_DEBUG411,14468
#define PIM_BUGINF(PIM_BUGINF414,14577
#define AUTORP_BUGINF 419,14665
#define IS_DENSE_MODE(IS_DENSE_MODE425,14810
#define IS_PIM_VERSION1(IS_PIM_VERSION1432,15018
#define PIM_RANDOM_DELAY(PIM_RANDOM_DELAY437,15156
#define PIM_QUERY_GET_MODE(PIM_QUERY_GET_MODE444,15347
#define PIM_QUERY_GET_HT(PIM_QUERY_GET_HT445,15395
#define PIM_SET_REG_FLAGS(PIM_SET_REG_FLAGS452,15624
#define PIM_HANDLE_TO_BIT(PIM_HANDLE_TO_BIT458,15796
#define PIM_RP_IS_V2(PIM_RP_IS_V2463,15904
#define PIM_RP_IS_V1(PIM_RP_IS_V1464,15944
typedef struct pim_traffic_t_ pim_traffic_t_469,16045
} pim_traffic_t;pim_traffic_t492,16665
typedef struct pimv2_encoded_group_ pimv2_encoded_group_498,16760
} pimv2_encoded_group;pimv2_encoded_group504,17047
typedef struct pimv2_unicast_address_ pimv2_unicast_address_509,17146
} pimv2_unicast_address;pimv2_unicast_address513,17358
