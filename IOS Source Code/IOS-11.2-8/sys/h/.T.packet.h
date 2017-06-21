
packet.h,3735
#define __PACKET_H__338,12909
#define ENCAPBYTES 363,13762
#define ENCAPWORDS 364,13827
#define ENC_NOARPENTRY	372,14115
#define ENC_FAILED_ONCE 378,14295
#define ENC_SRCMAC_OVERRIDE 383,14441
#define ENC_DLSW 388,14524
#define PAK_X25_QBIT 393,14568
#define PAK_X25_DBIT 394,14591
#define PAK_X25_ENC 395,14614
#define PAK_FR_FECN 399,14664
#define PAK_FR_BECN 400,14691
#define PAK_FR_DE 401,14718
#define PAK_FR_IETF 402,14745
#define DEFERED_LATE_Q_COMPRESSION 403,14772
#define PAK_FR_BROADCAST 404,14812
#define PAK_DDR_INTERESTING	408,14883
#define PAK_DDR_UNINTERESTING	409,14953
#define PAK_DDR_CALL_PENDING	410,15027
#define PAK_DDR_NO_HOLDQ	411,15104
#define PAK_VPN_USED 416,15186
typedef ulong buffer_flags_t;buffer_flags_t418,15213
typedef uint flags_t;flags_t419,15243
} protocol_event_t;protocol_event_t424,15325
#define set_major_protocol_event(set_major_protocol_event426,15346
#define set_minor_protocol_event(set_minor_protocol_event427,15422
#define get_major_protocol_event(get_major_protocol_event428,15498
#define get_minor_protocol_event(get_minor_protocol_event429,15562
typedef struct pakdata_ pakdata_434,15679
} pakdata;pakdata437,15762
struct particletype_ particletype_443,15828
struct paktype_ paktype_460,16628
typedef struct paktype_offset_ paktype_offset_651,25563
} paktype_offset;paktype_offset662,25843
static inline void pak_delta_adjust_start(672,26058
# define DEF_HUGEDATA 689,26313
# define HUGEDATA 690,26376
# define LARGEDATA 691,26449
# define VERYBIGDATA 692,26499
# define BIGDATA 693,26566
# define MEDDATA 694,26612
# define SMALLDATA 695,26661
# define MAXDATAGRAMSIZE 697,26713
#define PAK_SWITCHCOUNT 704,26918
#define MINELEMENTS 711,26989
typedef struct elementtype_ elementtype_713,27014
} elementtype;elementtype716,27096
# define PAK_BUF_PERMANENT 729,27607
# define PAK_BUF_IGNOREQ 730,27684
# define PAK_BUF_USE_DIRTY 731,27762
# define PAK_BUF_DIRTY 732,27840
# define PAK_BUF_ACCT_DONE 733,27919
# define PAK_BUF_DO_NOT_Q 734,27994
# define PAK_BROADCAST 745,28254
# define PAK_NOREDIRECT 746,28330
# define PAK_ALIAS	747,28402
# define PAK_DLQ	748,28476
# define PAK_PRIORITY	749,28545
# define PAK_CONTROL	750,28621
# define PAK_HEARSELF	751,28696
# define PAK_MCICHECKED	752,28773
# define PAK_ABORT	753,28852
# define PAK_INPUTQ	754,28922
# define PAK_MBIT	755,28998
# define PAK_DO_ISL_CRC	756,29072
# define PAK_IS_ISL 758,29179
# define PAK_SENDSELF	759,29251
# define PAK_MULTICAST 760,29328
# define PAK_PRIORITIZED 761,29408
# define PAK_XMITBCAST	762,29486
# define PAK_RSP_QUEUE	764,29564
# define PAK_MFS_SWITCHED 765,29633
# define PAK_IPXCOMPRESS 766,29713
# define PAK_SHAPED 767,29789
# define PAK_RSRB_EXPLORER 768,29856
# define PAK_SRB_IN	769,29939
# define PAK_SRB_OUT	770,30007
# define PAK_SRB_EXPLORER 771,30081
# define PAK_TBRIDGE_IN 772,30157
# define PAK_LACK_IN 774,30273
# define PAK_LACK_OUT 775,30350
# define PAK_SSCOP_RTS 776,30430
# define PAK_ENCAPS_ONLY 777,30492
# define PAK_SRB_EXP_FAST 778,30572
# define PAK_SRB_EXP_SENT 779,30647
#define pak_lock(pak_lock786,30868
#define pak_unlock(pak_unlock787,30910
#define pak_shared(pak_shared791,31021
static inline void platform_pak_set_dirty_inline 809,31743
#define pak_center(pak_center821,31982
#define ALIGN_DATAGRAM_CMD(ALIGN_DATAGRAM_CMD832,32283
#define ALIGN_DATAGRAM_CMD(ALIGN_DATAGRAM_CMD834,32321
#define BUFFER_WORD_ALIGN 840,32420
#define BUFFER_LONGWORD_ALIGN 841,32461
pak_queue_get_head 925,35678
pak_queue_get_tail 931,35862
pak_queue_depth 937,36043
pak_queue_max_depth 942,36130
pak_queue_set_max_depth 947,36222
pak_on_data_link_queue 957,36546
