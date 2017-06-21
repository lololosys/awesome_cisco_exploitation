
ipigrp2.h,1721
#define __IPIGRP2_H__98,3825
#define IPIGRP2_PROT 106,3944
#define IPIGRP2_REQUEST 108,3974
#define IPIGRP2_METRIC 109,4020
#define IPIGRP2_EXTERNAL 110,4053
enum IPIGRP2_PROT_ID IPIGRP2_PROT_ID115,4129
    NULL_PROTID,116,4152
    IGRP1_PROTID,117,4169
    IGRP2_PROTID,118,4187
    STATIC_PROTID,119,4205
    RIP_PROTID,120,4224
    HELLO_PROTID,121,4240
    OSPF_PROTID	122,4258
    ISIS_PROTID,123,4276
    EGP_PROTID,124,4293
    BGP_PROTID,125,4309
    IDRP_PROTID,126,4325
    CONNECTED_PROTID127,4342
#define IPIGRP2_AGE_DELAY 133,4400
#define INTERNAL_FLAGS_BIT 138,4514
#define CD_FLAGS_BIT 139,4546
#define IPIGRP2EXTPAKMACRO 144,4654
#define IPIGRP2MPDECODE 153,5205
typedef struct ipigrp2_mpdecode_ ipigrp2_mpdecode_157,5386
} ipigrp2_mpdecode;ipigrp2_mpdecode159,5441
typedef struct ipigrp2_metrictype_ ipigrp2_metrictype_161,5462
} ipigrp2_metrictype;ipigrp2_metrictype166,5633
#define IPIGRP2_METRICTYPEFLAGS(IPIGRP2_METRICTYPEFLAGS172,5749
typedef struct ipigrp2_externtype_ ipigrp2_externtype_174,5826
} ipigrp2_externtype;ipigrp2_externtype180,6020
typedef struct ipigrp2_extdatatype_ ipigrp2_extdatatype_182,6043
} ipigrp2_extdatatype;ipigrp2_extdatatype184,6104
typedef struct ipigrp2_address_ ipigrp2_address_189,6193
} ipigrp2_address;ipigrp2_address192,6272
typedef struct ipigrp2_worktype_ ipigrp2_worktype_197,6362
} ipigrp2_worktype;ipigrp2_worktype232,6964
typedef enum summary_revise_result_ summary_revise_result_238,7037
} summary_revise_result;summary_revise_result242,7257
typedef struct ipigrp2_traffic_t_ ipigrp2_traffic_t_247,7321
} ipigrp2_traffic_t;ipigrp2_traffic_t250,7394
#define IPIGRP2_DTARGET(IPIGRP2_DTARGET253,7417
