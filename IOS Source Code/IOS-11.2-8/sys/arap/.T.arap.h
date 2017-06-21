
arap.h,4131
#define UNLIMITED	68,2308
#define TICKLETIME	69,2362
#define MAX_UNACKED_TICKLES1 70,2419
#define MAX_UNACKED_TICKLES2 72,2576
#define MT_HEADERLEN	74,2733
#define WARNINGTIME	75,2789
#define ZONETABLESIZE	78,2868
#define MAXTEXTLENGTH	81,2956
#define ARA_LNKARB_TYPEHIGH 84,3048
#define ARA_LNKARB_TYPELOW 85,3101
#define ARA_LNKARB_RESHIGH 86,3135
#define ARA_LNKARB_RESLOW 87,3169
#define ARA_LNKARB_CRR 89,3204
#define ERR_VLD8_CALLBACK	92,3286
#define ERR_VLD8_BADVERSION	93,3319
#define ERR_VLD8_BADUSER	94,3354
#define ERR_VLD8_BADPASSWORD	95,3386
#define ERR_VLD8_BADLINK	96,3422
#define ERR_VLD8_NOCALLBACKALLOWED	97,3454
#define ERR_VLD8_ALLCBSERVERBUSY	98,3495
#define ERR_VLD8_GUESTNOTALLOWED	99,3534
#define ERR_VLD8_SERVERISIMPOSTER	100,3573
#define ERR_VLD8_LOGINNOTENABLED	101,3613
#define ERR_VLD8_MANUALPASSWORDREQUIRED	102,3652
#define ERR_CONNECTION_REFUSED 104,3699
#define NO_ERR 106,3746
#define kNoGuestAccess	109,3803
#define kMultiPortSrvr	110,3831
#define kManualPasswordEntry	111,3859
#define kContinueConnection 114,3930
#define kAbortConnection 115,3966
#define kRetryDES	116,4002
#define ARAPAK_DATAFLAG 119,4047
#define KAUTH_TWOWAYDES	122,4083
#define ARAP_LONG_SIZE 124,4110
#define ARAP_SHORT_SIZE 125,4135
#define ARAP_MAXZONESIZE 127,4162
#define ARAP_MAXNAMESIZE 128,4190
#define ARAP_MAXZONEPACKETSIZE 129,4218
#define SMARTBUFFER_VERSION 131,4254
#define SRVRVERSION1_LOW0	134,4306
#define SRVRVERSION1_LOW1	135,4334
#define SRVRVERSION1_LOW2	136,4362
#define SRVRVERSION1_LOW3	137,4392
#define SRVRVERSION2_HIGH0	140,4442
#define SRVRVERSION2_HIGH1	141,4471
#define SRVRVERSION2_HIGH2	142,4500
#define SRVRVERSION2_HIGH3	143,4530
typedef struct msg_ack_ msg_ack_145,4560
} msg_ack;msg_ack150,4662
typedef struct msg_srvrversion_ msg_srvrversion_152,4674
} msg_srvrversion;msg_srvrversion159,4861
typedef struct msg_auth_request_ msg_auth_request_161,4881
} msg_auth_request;msg_auth_request173,5263
typedef struct msg_auth_challenge_ msg_auth_challenge_175,5284
} msg_auth_challenge;msg_auth_challenge184,5568
typedef struct msg_auth_response_ msg_auth_response_186,5591
} msg_auth_response;msg_auth_response195,5873
typedef struct msg_feature_Info_ msg_feature_Info_197,5895
} msg_feature_Info;msg_feature_Info209,6307
typedef struct msg_ext_auth_version_ msg_ext_auth_version_213,6370
} msg_ext_auth_version;msg_ext_auth_version221,6630
typedef struct msg_ext_auth_result_ msg_ext_auth_result_223,6655
} msg_ext_auth_result;msg_ext_auth_result230,6871
typedef struct msg_ext_auth_load_ msg_ext_auth_load_232,6896
} msg_ext_auth_load;msg_ext_auth_load239,7115
typedef struct msg_ext_auth_exchange_ msg_ext_auth_exchange_241,7138
} msg_ext_auth_exchange;msg_ext_auth_exchange250,7402
typedef struct msg_ext_auth_abort_ msg_ext_auth_abort_252,7429
} msg_ext_auth_abort;msg_ext_auth_abort259,7644
typedef struct msg_ext_callback_ msg_ext_callback_261,7668
} msg_ext_callback;msg_ext_callback266,7810
typedef struct msg_startinfofromserver_ msg_startinfofromserver_270,7881
} msg_startinfofromserver;msg_startinfofromserver286,8459
typedef struct msg_zonelistinfo_ msg_zonelistinfo_288,8487
} msg_zonelistinfo;msg_zonelistinfo295,8634
typedef struct msg_password_change_result_ msg_password_change_result_299,8682
} msg_password_change_result;msg_password_change_result305,8852
typedef struct msg_pw_client_must_change_ msg_pw_client_must_change_307,8883
} msg_pw_client_must_change;msg_pw_client_must_change313,9049
typedef struct msg_tickle_ msg_tickle_315,9079
} msg_tickle;msg_tickle322,9255
typedef struct msg_timeleft_ msg_timeleft_324,9270
} msg_timeleft;msg_timeleft330,9413
typedef struct msg_text_alert_ msg_text_alert_332,9430
} msg_text_alert;msg_text_alert338,9572
typedef union arap_packet_ arap_packet_341,9592
} arap_packet;arap_packet361,10483
typedef struct bits64_ bits64_364,10500
} bits64;bits64367,10597
typedef struct arap_authtype_ arap_authtype_369,10608
} arap_authtype;arap_authtype377,10855
struct araptype_ araptype_380,10874
