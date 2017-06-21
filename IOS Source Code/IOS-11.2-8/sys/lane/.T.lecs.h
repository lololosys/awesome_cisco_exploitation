
lecs.h,1846
#define lecs_h310,11453
typedef struct lecs_info_t_ lecs_info_t, LecsInfo;LecsInfo352,12323
typedef struct serverAtmAddress ServerAtmAddress;ServerAtmAddress353,12374
typedef struct elanInfoBlock ElanInfoBlock;ElanInfoBlock354,12424
typedef enum RegistrationState RegistrationState366,12869
} RegistrationState;RegistrationState373,13126
typedef struct lecsAddresslecsAddress402,14188
} LecsAddress, *pLecsAddress;pLecsAddress431,14817
#define ADDR_TYPE	433,14848
#define ADDR_LEN	434,14882
#define DATA_BYTES	435,14917
#define MASK_BYTES	436,14957
#define ACTUAL_VALUE_VALID	450,15509
typedef enum callOriginatorTypecallOriginatorType461,15881
} CallOriginatorType;CallOriginatorType468,16111
typedef struct connectionconnection486,16663
} Connection, *pConnection;pConnection498,17140
typedef struct lecInfoBlocklecInfoBlock517,17858
} LecInfoBlock, *pLecInfoBlock;pLecInfoBlock523,18064
struct serverAtmAddressserverAtmAddress539,18656
#define NAMESIZE	561,19055
#define LES_TIMEOUT_DFLT 567,19206
struct elanInfoBlockelanInfoBlock569,19252
typedef struct configTableconfigTable593,20164
} ConfigTable, *pConfigTable;pConfigTable657,21720
typedef enum _callType_callType671,22202
} CallType;CallType677,22356
typedef struct globalLecsAddressglobalLecsAddress679,22369
} GlobalLecsAddress;GlobalLecsAddress697,22720
#define MAX_GLOBAL_LECS_COUNT	707,22984
struct lecs_info_t_lecs_info_t_715,23165
#define PRINTF	790,25399
#define USER_ERROR	791,25443
#define LECS_CRASH(LECS_CRASH792,25469
#define IGNORE_VCD	816,26250
boolean invalid_vcd 819,26287
#define NO_ERROR	875,28348
#define NULL_IF	876,28368
#define ALREADY_BOUND	877,28415
#define NOT_BOUND	878,28467
#define MALLOC_FAILURE	879,28514
#define REGISTRATION_ERROR	880,28564
#define FOR_ALL_LECSS(FOR_ALL_LECSS905,29389
