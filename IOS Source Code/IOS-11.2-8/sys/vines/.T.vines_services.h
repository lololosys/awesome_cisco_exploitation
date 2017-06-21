
vines_services.h,3961
#define VSVC_TREE_AGE_INTERVAL 60,2260
#define VSVC_NO_ERROR	62,2305
#define VSVC_DEFERRED_REPLY	63,2330
#define VSVC_ERR_BADPROCNUM	64,2360
#define VSVC_ERR_NO_MEMORY	65,2392
#define VSVC_ERR_INVALID	66,2423
#define VSVC_ERR_TIMEOUT	67,2452
typedef enum vsvc_send_action_ vsvc_send_action_88,2955
} vsvc_send_action;vsvc_send_action91,3040
typedef enum vsvc_rcv_action_ vsvc_rcv_action_93,3061
} vsvc_rcv_action;vsvc_rcv_action99,3232
typedef uchar * (*vsvc_pack)vsvc_pack102,3253
typedef uchar * (*vsvc_pack)(uchar *packed, uchar *unpacked)unpacked102,3253
typedef ushort  (*vsvc_call)vsvc_call103,3315
typedef ushort  (*vsvc_call)(vinesipcsocket *socket, uchar *call, uchar *reply)reply103,3315
typedef void    (*vsvc_return)vsvc_return104,3396
typedef void    (*vsvc_return)(vinesipcsocket *socket, uchar *reply)reply104,3396
typedef void    (*vsvc_search)vsvc_search105,3466
			       uchar *info2, uchar *info3)info3107,3582
typedef void    (*vsvc_retadd)vsvc_retadd108,3621
			       ulong service_rev, vinesaddress *service_address)service_address109,3694
typedef void    (*void_fn)void_fn110,3755
typedef void    (*failure_fn)failure_fn111,3789
typedef void    (*failure_fn)(vinesipcsocket *socket, ushort reason)reason111,3789
typedef boolean (*ager_fn)ager_fn112,3859
typedef boolean (*ager_fn)(serverentry serverentry112,3859
typedef struct vsvc_rtn_info_ vsvc_rtn_info_117,3967
} vsvc_rtn_info;vsvc_rtn_info145,4629
typedef struct vsvc_info_ vsvc_info_150,4707
} vsvc_info;vsvc_info198,6807
typedef struct vsvc_record_ vsvc_record_205,7001
} vsvc_record;vsvc_record210,7102
#define FOR_ALL_SERVICES(FOR_ALL_SERVICES212,7118
#define VSVC_MIN_VER	215,7234
#define VSVC_MAX_VER	216,7262
#define VSVC_MAX_SLOT	218,7291
#define VSVC_MAX_LINE	219,7317
#define VSVC_IF_HDLC	231,7629
#define VSVC_IF_BA	232,7688
#define VSVC_IF_X25	233,7745
#define VSVC_IF_ETHER	234,7794
#define VSVC_IF_TOKEN	235,7846
#define VSVC_IF_FDDI	236,7899
#define VSVC_IF_ATM	237,7941
#define VSVC_IF_SMDS	238,7982
#define VSVC_IF_FRAME	239,8024
#define VSVC_IF_ISDN	240,8067
struct serverentry_ serverentry_246,8146
static inline void vsvc_make_name 270,8804
static inline void vsvc_insert_hostname 280,9002
static inline void vsvc_insert_description 285,9116
static inline uchar *vines_insert_stringz 304,9706
static inline uchar *vines_insert_string 328,10350
static inline uchar *vines_extract_string 347,10792
static inline uchar *vines_insert_uchar 365,11235
static inline uchar *vines_extract_uchar 371,11352
static inline uchar *vines_insert_ushort 377,11472
static inline uchar *vines_extract_ushort 385,11642
static inline uchar *vines_insert_ulong 393,11822
static inline uchar *vines_extract_ulong 401,11987
static inline uchar *vines_insert_char 409,12162
static inline uchar *vines_extract_char 413,12287
static inline uchar *vines_insert_short 417,12417
static inline uchar *vines_extract_short 421,12546
static inline uchar *vines_insert_long 425,12680
static inline uchar *vines_extract_long 429,12805
static inline uchar *vines_insert_boolean 433,12935
static inline uchar *vines_extract_boolean 437,13066
static inline uchar *vines_insert_address 442,13203
static inline uchar *vines_extract_address 455,13562
static inline uchar *vines_insert_macaddress 468,13924
static inline uchar *vines_extract_macaddress 483,14305
static inline uchar *vines_insert_nonce 498,14689
static inline uchar *vines_extract_nonce 515,15146
vsvc_ServerKey 543,15903
vsvc_ServerCast 555,16102
vsvc_UnprotectServer 562,16257
vsvc_ProtectServer 572,16428
vsvc_ServerProtected 582,16617
vsvc_GetNextServer 591,16811
vsvc_GetFirstServer 610,17192
vsvc_GetBestServer 620,17378
vsvc_InsertServer 630,17560
vsvc_DeleteServer 642,17812
vsvc_ReinitServer 653,18038
vsvc_CreateServer 663,18201
vsvc_FindServer 690,18803
vsvc_WalkServerTree 708,19157
vsvc_FindSelf 714,19307
