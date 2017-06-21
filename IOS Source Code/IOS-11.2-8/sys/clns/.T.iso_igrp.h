
iso_igrp.h,1941
#define __ISO_IGRP_H__35,1166
#define ISO_IGRP_VERSION 37,1190
#define ISO_IGRP_CYCLETIME 57,1919
#define ISO_IGRP_ROUTEFLUSHTIME 58,1985
#define ISO_IGRP_BLINDTIME 59,2059
#define ISO_IGRP_HELLOTIME 60,2137
#define ISO_IGRP_HELLO_DEF_HOLD_TIME 62,2216
#define ISO_IGRP_OPCODE_STATION 64,2279
#define ISO_IGRP_OPCODE_AREA 65,2313
#define ISO_IGRP_OPCODE_HELLO 66,2344
#define ISO_IGRP_OPCODE_QUERY 67,2378
#define ISO_IGRP_LEVEL1_ONLY 69,2414
#define ISO_IGRP_LEVEL2_ONLY 70,2448
#define ISO_IGRP_LEVEL1_LEVEL2 71,2482
#define ISO_IGRP_AREALIST 76,2583
#define ISO_IGRP_AREAROUTE 77,2655
#define ISO_IGRP_STATIONROUTE 78,2725
#define ISO_IGRP_PREFIXROUTE 79,2798
#define ISO_IGRP_PARAMS 80,2868
typedef struct iso_igrp_hdrtype_ iso_igrp_hdrtype_90,3126
} iso_igrp_hdrtype;iso_igrp_hdrtype100,3646
#define iso_igrp_headstart(iso_igrp_headstart102,3667
typedef struct iso_igrp_info_type_ iso_igrp_info_type_111,4004
} iso_igrp_info_type;iso_igrp_info_type115,4236
#define ISO_IGRP_TLV_HDR_LENGTH 116,4258
typedef struct iso_igrp_metrictype_ iso_igrp_metrictype_123,4401
} iso_igrp_metrictype;iso_igrp_metrictype132,4853
#define ISO_IGRP_METRIC_HOPCOUNT_LENGTH 134,4877
typedef struct iso_igrp_prefixroute_ iso_igrp_prefixroute_142,5151
} iso_igrp_prefixroute;iso_igrp_prefixroute149,5367
#define ISO_IGRP_K1_DEFAULT 154,5461
#define ISO_IGRP_K2_DEFAULT 155,5526
#define ISO_IGRP_K3_DEFAULT 156,5579
#define ISO_IGRP_K4_DEFAULT 157,5633
#define ISO_IGRP_K5_DEFAULT 158,5690
typedef struct iso_igrp_hello_hdrtype_ iso_igrp_hello_hdrtype_178,6451
} iso_igrp_hello_hdrtype;iso_igrp_hello_hdrtype195,7330
#define ISO_IGRP_HELLO_HDRSIZE 200,7418
typedef struct iso_igrp_area_addr_type_ iso_igrp_area_addr_type_203,7458
} iso_igrp_area_addr_type;iso_igrp_area_addr_type206,7644
typedef struct iso_igrp_param_type_ iso_igrp_param_type_209,7673
} iso_igrp_param_type;iso_igrp_param_type219,7870
