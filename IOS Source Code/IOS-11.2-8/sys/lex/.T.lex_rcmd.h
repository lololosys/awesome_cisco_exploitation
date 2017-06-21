
lex_rcmd.h,4375
#define __LEX_RCMD_H__50,1699
#define LEX_RCMD_REQ 55,1842
#define LEX_RCMD_ACK 56,1873
#define LEX_RCMD_NAK 57,1904
#define LEX_RCMD_REJ 58,1935
#define LEX_RCMD_UNSOLICITED 59,1966
#define PPP_BRD_CNTL_SIZE 61,2001
#define PPP_TYPE_FIELD_SIZE 62,2080
#define LEX_RCMD_START 74,2406
#define LEX_PROTO_FILTER 75,2435
#define LEX_ADDR_FILTER 76,2464
#define LEX_SET_PRIORITY 77,2493
#define LEX_SHUTDOWN 78,2522
#define LEX_NOSHUTDOWN 79,2551
#define LEX_RELOAD 80,2580
#define LEX_STATS 81,2609
#define LEX_DLOAD_WRQ 82,2638
#define LEX_DLOAD_DATA 83,2667
#define LEX_DLOAD_STATUS 84,2696
#define LEX_INVENTORY 85,2726
#define LEX_UNSOLICITED 86,2756
#define LEX_RCMD_END 87,2786
#define LEX_DEFAULT_CMD_RETRY 89,2830
#define LEX_DEFAULT_TIMEOUT 90,2865
#define LEX_FLASH_WRITE_TIME 96,3133
#define LEX_RCMD_HDR_LEN 107,3414
#define LEX_RCMD_OPTHDR_LEN 108,3496
#define LEX_INVENTORY_LEN 109,3551
#define LEX_PRI_ENTRY_LEN 111,3624
#define LEX_DLOAD_STATUS_LEN 113,3684
#define LEX_BLOCK_NUM_SIZE 114,3749
#define LEX_PFILTER_ENTRY_LEN 117,3902
#define LEX_AFILTER_ENTRY_LEN 118,3970
#define LEX_MAX_PROTO_FILTERS 120,4038
#define LEX_MAX_ADDR_FILTERS 121,4076
#define LEX_MAX_PRIORITY_ENTRIES 122,4114
#define LEX_RESET_SERIAL_STATS 128,4240
#define LEX_SERIAL_STATS 129,4276
#define LEX_ETHER_STATS 130,4312
#define LEX_RESET_ETHER_STATS 131,4348
#define LEX_STATS_BOTH_INTS 132,4384
#define LEX_RESET_STATS 133,4452
#define MAX_LEX_IFDESCR_LENGTH 141,4737
#define LEX_RELOAD_DEFAULT_IMAGE 147,4860
#define LEX_RELOAD_PROM_IMAGE 148,4900
} lex_interface_type;lex_interface_type153,4996
typedef struct lex_rcmdst_type_ lex_rcmdst_type_161,5149
} lex_rcmdst_type;lex_rcmdst_type174,5982
typedef struct lex_rcmd_stub_type_ lex_rcmd_stub_type_176,6002
} lex_rcmd_stub_type;lex_rcmd_stub_type180,6116
#define LEXSTATUS_ETHER_LINEPROTOCOL 183,6184
#define LEXSTATUS_RESERVED_FOR_CISCO 184,6260
#define LEXSTATUS_RESERVED_FOR_OTHERS 185,6305
#define LEX_MAX_FILENM 191,6392
#define LEX_MAX_HOSTNM 192,6424
#define LEX_CONFIRM_BUF_SIZE 193,6456
#define LEX_SEQ_ERR_RETRIES 194,6488
struct lex_dload_args lex_dload_args196,6568
#define LEX_IMAGE_OK 205,6726
#define LEX_IMAGE_BAD 206,6759
#define LEX_IMAGE_NOT_CHECKED 207,6792
#define LEX_IMAGE_NO_STATUS 208,6825
struct lex_dload_version_args lex_dload_version_args210,6859
#define LEX_IMAGE_CSUM_SIZE 217,6977
typedef struct lex_image_header lex_image_header219,7009
} lex_image_header;lex_image_header225,7202
#define LEX_BLOCK_ACK 230,7255
#define LEX_TIMEOUT_ERROR 231,7336
#define LEX_SEQ_ERROR 232,7393
#define LEX_RCMD_ABORTED 233,7470
#define LEX_LINK_DOWN 234,7551
#define LEX_FLASH_WRERR 235,7632
#define LEX_CHECKSUM_ERR 236,7689
#define LEX_NAK_ERROR 237,7746
#define LEX_REJ_ERROR 238,7803
#define LEX_MAX_OUTSTANDING_RCMDS 240,7861
#define MIN(MIN243,7910
#define lex_rcmd_headstart(lex_rcmd_headstart246,7953
struct lex_hdr lex_hdr248,8010
struct lex_option lex_option254,8078
struct lex_proto_filter_entry lex_proto_filter_entry260,8154
struct lex_proto_filter lex_proto_filter266,8243
struct lex_addr_filter_entry lex_addr_filter_entry270,8338
struct lex_addr_filter lex_addr_filter276,8453
struct lex_prty_entry lex_prty_entry280,8543
struct lex_proto_prty lex_proto_prty285,8610
struct lex_download lex_download289,8698
struct lex_download_wrq lex_download_wrq294,8779
struct lex_download_status lex_download_status300,8898
typedef struct lex_unsolicited_t lex_unsolicited_t304,8955
} lex_unsolicited_t;lex_unsolicited_t306,9008
typedef struct lex_serial_counters_t lex_serial_counters_t308,9030
} lex_serial_counters_t;lex_serial_counters_t343,10782
typedef struct lex_ether_counters_t lex_ether_counters_t345,10808
} lex_ether_counters_t;lex_ether_counters_t375,12539
typedef struct lex_stats_area_t lex_stats_area_t377,12564
} lex_stats_area_t;lex_stats_area_t382,12799
#define  LEX_RUNNING_PROM 387,12878
#define  LEX_RUNNING_FLASH 388,12915
#define  PROM_IMAGE_CKSUM_OK 389,12952
#define  FLASH_IMAGE_CKSUM_OK 390,12989
#define LEX_FLASH_HAS_FILE(LEX_FLASH_HAS_FILE392,13027
typedef struct lex_inventory_msg lex_inventory_msg395,13123
} lex_inventory_msg;lex_inventory_msg408,13586
typedef struct lex_command lex_command415,13698
} lex_command;lex_command429,14189
