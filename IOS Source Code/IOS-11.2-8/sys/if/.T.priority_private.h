
priority_private.h,1861
#define __PRIORITY_PRIVATE_H__60,2099
#define PRIORITY_QLIMIT 73,2617
#define CUSTOM_QUEUE_SIZE 74,2646
#define CUSTOM_QUEUE_PACKET_SIZE 75,2688
#define PRIORITY_IP_ROUTECACHE 77,2757
static inline boolean priority_ip_routecache(80,2889
/* A structure for callback functions to call if an priority list is changed */ typedef void (*priority_callbackfunction)priority_callbackfunction87,3072
/* A structure for callback functions to call if an priority list is changed */ typedef void (*priority_callbackfunction) (ulong, ushort)ushort87,3072
typedef struct _priority_callback _priority_callback88,3211
} priority_callback;priority_callback93,3364
#define MAXIMUM_RED_SHIFT 95,3386
#define MINIMUM_RED_SHIFT 96,3454
#define RED_SIZE_THRESHOLD 97,3521
#define RED_STANDARD_SHIFT 98,3592
#define RED_SCALE(RED_SCALE99,3665
#define RED_RSVP_PRECEDENCE 100,3721
#define RED_PRECEDENCE_LEVELS 101,3777
struct red_parameters_ red_parameters_102,3809
#define FAIR_QUEUE_PAK_PRIORITY_WEIGHT 119,4485
#define FAIR_QUEUE_DEFAULT_BW 120,4566
#define FAIR_QUEUE_RSVP_WEIGHT 121,4642
#define FAIR_QUEUE_BASE_BURST	122,4710
#define FAIR_LINK_TYPE_HASH 123,4801
#define FAIR_QUEUE_HASH_QUEUES_DEFAULT	124,4874
#define FAIR_RSVP_QUEUES_DEFAULT	125,4917
#define FAIR_RSVP_QUEUES_RSVP	126,4952
#define FAIR_QUEUE_DYN_CONVERSATIONS	127,4987
#define FAIR_QUEUE_DEFAULT_QDEPTH 128,5054
#define FAIR_QUEUE_MAXIMUM_QDEPTH 129,5091
struct fair_queue_ fair_queue_131,5158
struct fair_queue_head_ fair_queue_head_139,5453
static inline int fair_queue_sum 161,6608
fair_queue *fair_queue_address 166,6721
boolean custom_queue_supported 227,8876
boolean priority_queue_supported 237,9101
boolean fair_queue_supported 255,9512
boolean compare_fair_queue 297,10899
boolean fair_queue_bandwidth 324,11793
boolean queuing_can_be_modifed 341,12344
