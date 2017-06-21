
types.h,4251
#define __TYPES_H__369,13367
typedef unsigned char *caddr_t;caddr_t379,13650
typedef struct queuetype_ queuetype_385,13751
} queuetype;queuetype390,13937
typedef struct nexthelper_ nexthelper_392,13951
} nexthelper;nexthelper395,14043
#define STRUCTURE_REF(STRUCTURE_REF401,14117
typedef const struct transition_ transition;transition697,28614
typedef struct radix_node	rntype;rntype703,28769
typedef struct radix_node_head	rnhtype;rnhtype704,28803
#define _SIZE_T715,29006
#define _GCC_SIZE_T718,29039
typedef __typeof__ (sizeof sizeof719,29059
typedef unsigned int size_t;size_t721,29105
typedef long long longlong;longlong726,29179
typedef unsigned short ushort;ushort727,29207
typedef unsigned char uchar;uchar728,29238
typedef unsigned int uint;uint729,29267
typedef unsigned long ulong;ulong730,29294
typedef unsigned long long ulonglong;ulonglong731,29323
typedef signed char        int8;int8736,29394
typedef signed short       int16;int16737,29427
typedef signed long        int32;int32738,29461
typedef signed long long   int64;int64739,29495
typedef unsigned char      uint8;uint8740,29529
typedef unsigned short     uint16;uint16741,29563
typedef unsigned long      uint32;uint32742,29598
typedef unsigned long long uint64;uint64743,29633
typedef ulong ipaddrtype;ipaddrtype745,29669
typedef char *va_list;va_list747,29696
typedef int boolean;boolean748,29737
typedef unsigned char tinybool;tinybool749,29758
typedef void forktype;forktype750,29790
typedef int pid_t;pid_t751,29813
} fuzzy;fuzzy757,29981
typedef void process;process759,29991
typedef process (process_t)process_t760,30013
typedef forktype (forkproc)forkproc761,30048
typedef boolean (blockproc)blockproc762,30094
typedef boolean (blockproc)(queuetype queuetype762,30094
typedef void (void_dontcare)void_dontcare763,30136
typedef boolean (boolean_dontcare)boolean_dontcare764,30179
typedef void (*timer_func)timer_func765,30228
typedef boolean (*fastswitch_type)fastswitch_type767,30314
typedef boolean (*fastswitch_type)(hwidbtype hwidbtype767,30314
typedef boolean (*fastswitch_type)fastswitch_type769,30369
typedef boolean (*fastswitch_type)(paktype paktype769,30369
#define NULL 778,30535
#define TRUE 784,30595
#define FALSE 785,30610
#define PACKED(PACKED791,30703
#define PACKED(PACKED793,30760
#define ALIGN_2(ALIGN_2812,31421
#define ALIGN_4(ALIGN_4813,31478
#define ALIGN_8(ALIGN_8814,31535
#define ALIGN_16(ALIGN_16815,31592
#define ALIGN_32(ALIGN_32816,31651
#define ALIGN_2(ALIGN_2818,31716
#define ALIGN_4(ALIGN_4819,31743
#define ALIGN_8(ALIGN_8820,31770
#define ALIGN_16(ALIGN_16821,31797
#define ALIGN_32(ALIGN_32822,31825
#define PRINTF_FUNCTION(PRINTF_FUNCTION828,31904
typedef struct charlong_ charlong_837,32114
} charlong;charlong843,32210
typedef struct charlonglong_ charlonglong_850,32313
} charlonglong;charlonglong857,32438
typedef volatile ulong semaphore;semaphore863,32577
typedef longlong sys_deltatime;sys_deltatime869,32658
typedef struct sys_timestamp_ sys_timestamp_870,32690
} sys_timestamp;sys_timestamp878,32810
#define timer_no_time 879,32827
#define TS_VALUE(TS_VALUE880,32881
#define TS_VALUE32(TS_VALUE32881,32915
typedef struct mgd_timer_ mgd_timer_918,34688
} mgd_timer;mgd_timer933,35424
#define MT_INTRPT_EXCLUDE_DIRECT 935,35438
#define MT_INTRPT_EXCLUDE_INHERITED 936,35510
#define MT_INITIALIZED_MAGIC 937,35586
#define MGD_TIMER_EXTENDED(MGD_TIMER_EXTENDED939,35659
typedef enum ipc_error_code_ ipc_error_code_944,35828
} ipc_error_code;ipc_error_code954,36039
typedef void (*ipc_callback)ipc_callback956,36059
typedef void (*ipc_callback)(ipc_message *, void *, ipc_error_code)ipc_error_code956,36059
typedef uint ipc_port_id;ipc_port_id957,36128
typedef ushort ipc_port;ipc_port958,36154
typedef uint ipc_seat;ipc_seat959,36179
typedef ushort ipc_sequence;ipc_sequence960,36202
typedef ushort ipc_message_type;ipc_message_type961,36231
typedef ushort ipc_size;ipc_size962,36264
typedef ushort ipc_flags;ipc_flags963,36289
enum HEADTAIL HEADTAIL965,36316
    HEAD,966,36332
    TAIL967,36342
#define	MIN(MIN970,36355
#define	MAX(MAX971,36398
