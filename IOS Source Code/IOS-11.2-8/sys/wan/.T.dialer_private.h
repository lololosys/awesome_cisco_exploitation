
dialer_private.h,3629
#define __DIALER_PRIVATE_H__90,3251
#define DIALERHASH 92,3281
#define DIALER_ODDPARITY_MAXCHAR 93,3303
#define DIALER_IDLE_TIME	95,3341
#define DIALER_FAST_IDLE_TIME	96,3379
#define DIALER_ENABLE_TIME	97,3421
#define DIALER_CARRIER_TIME	98,3460
#define DIALER_HOLDQ_TIME	99,3500
#define DIALER_LOAD_TIME	100,3576
#define DIALER_DEBOUNCE_TIME	101,3612
#define DIALER_DEFAULT_PULSEINTERVAL 102,3653
#define DIALER_MAXLIST	104,3702
#define DIALER_MAX_CALLER 105,3755
#define DIALER_MAX_NUM_HOLDQ 106,3830
#define DIALER_CRN_LEN	108,3867
struct dialer_call_tab_ dialer_call_tab_114,3985
struct dialermaptype_ dialermaptype_127,4391
typedef struct dialerentrytype_ dialerentrytype_137,4693
} dialerentrytype;dialerentrytype151,5366
#define DIALER_HOLD_PAK(DIALER_HOLD_PAK153,5386
#define DIALER_NEW_ADDR_LINKTYPE(DIALER_NEW_ADDR_LINKTYPE154,5461
struct dialergrouptype_ dialergrouptype_159,5590
struct dialerpaktype_ dialerpaktype_176,6406
typedef struct dialerpoolheadtype_ dialerpoolheadtype_193,7044
} dialerpoolheadtype;dialerpoolheadtype200,7297
struct dialerpooltype_ dialerpooltype_205,7386
enum DIALERLIST DIALERLIST221,7780
    DIALLST_NONE,222,7798
    DIALLST_DENY,223,7851
    DIALLST_PERMIT,224,7906
    DIALLST_PROLIST,225,7965
    DIALLST_LIST	226,8025
union dllist dllist232,8110
typedef struct dlisttype_ dlisttype_237,8213
} dlisttype;dlisttype242,8338
struct dialerlisttype_ dialerlisttype_244,8352
struct dialer_holdq_type_ dialer_holdq_type_249,8482
static inline void init_dialer_holdq_timer 259,8732
static inline void start_dialer_holdq_timer 264,8839
static inline void stop_dialer_holdq_timer 270,8983
static inline boolean dialer_holdq_timer_running 275,9090
boolean dialer_holdq_timer_running_and_awake 281,9228
typedef struct callback_type callback_type292,9527
} callback_type;callback_type301,10023
static inline void start_callback_timer 303,10041
static inline void stop_callback_timer 308,10167
typedef struct dialer_params_type_ dialer_params_type_316,10313
} dialer_params_type;dialer_params_type328,10875
#define GET_NETDDB(GET_NETDDB333,10929
#define V25BIS_DIALER(V25BIS_DIALER334,10974
#define DTR_DIALER(DTR_DIALER335,11039
#define ASYNC_DIALER(ASYNC_DIALER336,11099
#define ISDN_DIALER(ISDN_DIALER337,11164
#define IS_PROFILE(IS_PROFILE338,11220
#define IS_SPOOFING_INT(IS_SPOOFING_INT339,11286
#define IS_SPOOFING_PROT(IS_SPOOFING_PROT340,11344
#define IS_SPOOFING(IS_SPOOFING341,11404
#define DIALER_HOLDQ_EMPTY(DIALER_HOLDQ_EMPTY343,11478
#define IS_CALL_ALLOWED(IS_CALL_ALLOWED344,11541
#define CONNECTED_GROUP(CONNECTED_GROUP346,11633
#define SLAVE_DIALER(SLAVE_DIALER347,11691
static inline void start_idle_timer 349,11770
#define START_IDLE_TIMER(START_IDLE_TIMER355,11941
static inline void set_dialer_state 357,11994
#define FOR_ALL_HUNTDDBS_ON_MASTER(FOR_ALL_HUNTDDBS_ON_MASTER367,12215
#define FOR_ALL_CONNECTEDGROUPS_ON_MASTER(FOR_ALL_CONNECTEDGROUPS_ON_MASTER375,12448
#define FOR_ALL_MEMBERS_IN_GROUP(FOR_ALL_MEMBERS_IN_GROUP383,12654
#define FOR_ALL_MEMBERS_IN_POOL(FOR_ALL_MEMBERS_IN_POOL391,12887
#define FOR_ALL_PROFILES_IN_POOL(FOR_ALL_PROFILES_IN_POOL399,13091
#define FOR_ALL_POOLS_IN_INTERFACE(FOR_ALL_POOLS_IN_INTERFACE407,13305
#define FOR_ALL_HEAD_OF_POOLS(FOR_ALL_HEAD_OF_POOLS412,13451
#define DDR_FASTSWITCH(DDR_FASTSWITCH420,13654
static inline dialerdbtype *set_output_dialer_ddb 466,15095
static inline void dialer_holdq_enqueue 514,17226
static inline void dialer_increment_call 534,17727
static inline void dialer_decrement_call 555,18392
