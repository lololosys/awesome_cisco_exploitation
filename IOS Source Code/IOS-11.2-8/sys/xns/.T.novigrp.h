
novigrp.h,1516
#define __NOVIGRP_H__83,3005
#define IPX_EIGRP_SAP_SPLIT_HORIZON_DEFAULT 91,3136
#define IPX_EIGRP_SAP_DEFER_COUNT 96,3263
#define NOVIGRP_PROT 101,3366
#define NOVIGRP_REQUEST 103,3422
#define NOVIGRP_METRIC 104,3455
#define NOVIGRP_EXTERNAL 105,3488
enum NOVIGRP_PROT_ID NOVIGRP_PROT_ID110,3564
    NULL_PROTID,111,3587
    IGRP_PROTID,112,3604
    STATIC_PROTID,113,3621
    RIP_PROTID,114,3640
    CONNECTED_PROTID,115,3656
    ISIS_PROTID,116,3678
    NLSP_PROTID,117,3738
    INTERNAL_PROTID 118,3798
#define NOVIGRPEXTPAKMACRO 125,3938
typedef struct novigrp_metrictype_ novigrp_metrictype_134,4401
} novigrp_metrictype;novigrp_metrictype140,4607
typedef struct novigrp_externtype_ novigrp_externtype_142,4630
} novigrp_externtype;novigrp_externtype149,4859
typedef struct novigrp_extdatatype_ novigrp_extdatatype_151,4882
} novigrp_extdatatype;novigrp_extdatatype153,4943
typedef struct novigrp_worktype_ novigrp_worktype_155,4967
} novigrp_worktype;novigrp_worktype182,5391
typedef struct novigrp_sapq_entry_ novigrp_sapq_entry_186,5448
} novigrp_sapq_entry;novigrp_sapq_entry192,5755
#define NOVIGRP_SAPQ_CHUNK_COUNT 196,5810
#define NOVIGRP_DOING_INCREMENTAL_SAP(NOVIGRP_DOING_INCREMENTAL_SAP200,5945
#define NOVIGRP_DTARGET(NOVIGRP_DTARGET230,7344
#define NOVIGRP_DETARGET(NOVIGRP_DETARGET235,7518
static inline novpdbtype *find_novigrp_pdb 244,7749
static inline void ipx_igrp2_take_nbrs_down 263,8291
static inline boolean ipx_igrp2_exists_iidb 278,8747
