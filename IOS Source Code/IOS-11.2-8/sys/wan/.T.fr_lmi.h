
fr_lmi.h,3260
# define MAX_IE_BYTES 32,1004
# define FR_REPORT_TYPE_IE_LENGTH 33,1028
# define REPORT_TYPE_IE_ID 34,1064
# define FR_REPORT_NOTSEEN 39,1144
# define FR_REPORT_KEEPALIVE_ONLY 40,1183
# define FR_REPORT_FULL_STATUS 41,1219
# define MAX_LMI_EVENTS 42,1255
# define MAX_LMI_PROCESS 43,1283
# define LMI_STATUS_ENQUIRY 47,1314
# define LMI_STATUS 48,1347
# define LMI_UPDATE_STATUS 49,1372
# define FRAME_RELAY_LMI_DBIT 50,1404
# define NDBITS(NDBITS51,1438
# define FRAME_RELAY_LMI_OVERHEADBYTES 52,1473
# define FRAME_RELAY_ANSI_LMI_OVERHEADBYTES 53,1514
# define FRAME_RELAY_LMI_HEADER_SIZE 54,1560
# define FRAME_RELAY_IE_OVERHEADBYTES 55,1599
# define IS_FRAME_RELAY_LMI_PKT(IS_FRAME_RELAY_LMI_PKT57,1640
# define IS_FRAME_RELAY_LMI_IE_PKT(IS_FRAME_RELAY_LMI_IE_PKT58,1704
# define IS_FRAME_RELAY_PVC_IE(IS_FRAME_RELAY_PVC_IE66,2018
# define ANSI_PVC_LEN 69,2115
# define CISCO_PVC_LEN 70,2139
# define MULTICAST_IE_ID 71,2164
# define FR_MULTICAST_IE_LENGTH 72,2191
# define PVC_IE_ID 73,2225
# define PVC_IE_LENGTH 74,2246
# define ANSI_PVC_IE_LENGTH 75,2271
# define KEEPALIVE_IE_ID 76,2301
# define KEEPALIVE_IE_LENGTH 77,2328
# define UNNUMBERED_FRAME_ID 79,2360
# define LMI_PROTOCOL_DISCRIMINATOR 80,2391
# define ANSI_LMI_PROTOCOL_DISCRIMINATOR 81,2432
# define LMI_PROT_DISC 82,2478
# define ANSI_LMI_PROT_DISC 83,2506
# define LMI_CALL_REF 84,2539
# define LOCK_SHIFT 85,2563
# define FR_SVC_CALL_REF 86,2588
# define INVALID_CALL_REF_VALUE	87,2623
# define FR_SVC_PID 88,2659
# define T_ACTIVITY_DEF 89,2702
# define LMI_ROUTE_INTERVAL 90,2752
enum FR_LMI_TYPE FR_LMI_TYPE97,2907
    FR_CISCO_LMI 98,2926
    FR_CCITT_LMI 99,2948
    FR_ANSI_D 100,2971
enum FR_LMI_RTN_VAL FR_LMI_RTN_VAL102,2992
    FR_LMI_FAIL,103,3014
    FR_LMI_IE_FAIL,104,3031
    FR_LMI_PASS 105,3051
enum FR_LMI_MODE FR_LMI_MODE107,3071
    FR_LMI_RCVD,108,3090
    FR_LMI_SENT109,3107
enum LMI_TIMER LMI_TIMER112,3131
    LMI_IDB_TIMER,113,3148
    LMI_ERR_TIMER,114,3167
    LMI_TUNNEL_TIMER,115,3186
    LMI_ROUTE_TIMER116,3208
typedef struct fr_lmi_protocol_discriminator_ fr_lmi_protocol_discriminator_119,3232
} fr_lmi_protocol_discriminator;fr_lmi_protocol_discriminator122,3315
typedef struct fr_lmi_stats_type_ fr_lmi_stats_type_128,3384
} fr_lmi_stats_type;fr_lmi_stats_type151,4531
typedef struct fr_report_type_struct_ fr_report_type_struct_153,4553
} fr_report_type_struct;fr_report_type_struct157,4685
struct frame_relay_keepalive_struct frame_relay_keepalive_struct159,4712
struct fr_ansi_pvc_struct fr_ansi_pvc_struct167,4860
union frame_relay_ie_types frame_relay_ie_types174,4981
struct frame_relay_lmi frame_relay_lmi183,5324
struct fr_idb_lmi fr_idb_lmi192,5550
typedef struct fr_ie_length_ fr_ie_length_243,7148
} fr_ie_length;fr_ie_length250,7299
# define CCITT_PVC_IE_ID 256,7363
# define CCITT_REPORT_IE_ID 257,7402
# define CCITT_KEEP_IE_ID 258,7441
# define FRAME_RELAY_DEFAULT_LMI 263,7525
# define DEFAULT_LMI_T392DCE 265,7566
# define DEFAULT_LMI_N391DTE 266,7606
# define DEFAULT_LMI_N392DTE 267,7646
# define DEFAULT_LMI_N393DTE 268,7686
# define DEFAULT_LMI_N392DCE 269,7726
# define DEFAULT_LMI_N393DCE 270,7766
# define lmi_idb 272,7807
