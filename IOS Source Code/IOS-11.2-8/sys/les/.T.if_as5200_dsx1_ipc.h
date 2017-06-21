
if_as5200_dsx1_ipc.h,5230
#define HAVE_IF_AS5200_DSX1_IPCH57,2086
#define SRAM_PAGESIZE	59,2120
#define SRAM_PAGEMASK	60,2182
#define MAX_DIGITS 62,2228
#define MAX_T1	63,2255
#define MAX_DSX1	64,2273
#define FW_VERSION_EXT 65,2292
typedef volatile uchar vuchar;vuchar67,2322
typedef volatile ushort vushort;vushort68,2353
typedef ushort LineStatus;LineStatus69,2386
typedef struct neat2mpu_mail_type neat2mpu_mail_type75,2477
} neat2mpu_mail_type;neat2mpu_mail_type85,2771
typedef struct carrier_mail_type carrier_mail_type87,2794
} carrier_mail_type;carrier_mail_type91,2913
typedef struct dtmfmail_from_mpu dtmfmail_from_mpu99,3136
} dtmfmail_from_mpu;dtmfmail_from_mpu104,3439
typedef struct dtmfmail_to_mpu dtmfmail_to_mpu113,3745
} dtmfmail_to_mpu;dtmfmail_to_mpu118,4032
#define MSG_DATA_SIZE	120,4052
typedef struct _neat_ipc_message _neat_ipc_message126,4250
} neat_ipc_message;neat_ipc_message134,4743
#define IPCMSGFLAG_DATA_EXT 137,4804
	MSG_NEAT_NULL 143,4975
	MSG_NEAT_MIB,144,4995
	MSG_NEAT_BUGINFO,145,5035
	MSG_NEAT_ERROR_REPORT,146,5078
 	MSG_NEAT_RX_RBS_DATA,147,5121
 	MSG_NEAT_TX_RBS_DATA,148,5189
        MSG_NEAT_RBS2CSM_HI,149,5257
        MSG_NEAT_RBS2CSM_LO,150,5336
	MSG_NEAT_MAX	151,5415
	MSG_MPU_NULL 156,5522
	MSG_MPU_SET_CT1_CHANNELS,157,5566
        MSG_MPU_SHOW_RBS,158,5629
        MSG_MPU_CSM2RBS,159,5707
	MSG_MPU_MAX	160,5772
#define MAX_DSX1_CHANNELS 163,5802
} msg_ct1_channels;msg_ct1_channels169,5961
} msg_show_rbs_resp;msg_show_rbs_resp175,6067
typedef struct _ipc_msg_queue _ipc_msg_queue177,6089
} ipc_msg_queue;ipc_msg_queue187,6582
#define IPCQ_OWNER_NONE	189,6600
#define IPCQ_OWNER_NEAT	190,6654
#define IPCQ_OWNER_CORE	191,6719
#define WHO_NEAT 193,6775
#define WHO_CORE 194,6814
#define WHO_INIT 195,6853
#define WHO_CHECK 196,6892
#define WHO_MSG_SEND 197,6931
#define WHO_GET 198,6970
#define IPC_MSG_SIZE	200,7010
#define TOTAL_MSG_BUFFERS 206,7195
typedef struct _ipc_section _ipc_section209,7284
} ipc_section;ipc_section228,8247
#define DIALTONE_ADDR	233,8307
#define BUSYTONE_ADDR	234,8348
#define IPC_MSG_BUFF_ADDR	235,8389
#define CARRIER_TO_MPU(CARRIER_TO_MPU236,8434
#define MAIL_FROM_MPU(MAIL_FROM_MPU237,8506
#define MSG_FROM_MPU	238,8574
#define MSG_TO_MPU	239,8619
#define FIRMWARE_VER_ADDR	240,8660
#define MPU_RELOAD_DONE_ADDR	241,8713
#define FW_VERSION 243,8764
#define MPU_RELOAD_DONE 244,8811
#define MAIL_TO_MPU(MAIL_TO_MPU247,8881
#define DTMF_TO_MPU(DTMF_TO_MPU248,8940
#define DTMF_FROM_MPU(DTMF_FROM_MPU249,9002
#define LOCAL_STACK_POINTER	256,9174
#define IPC_AREA	257,9210
#define IPC_MSG_AREA	259,9323
#define DYNAMIC_MEM_AREA 262,9438
} neat_ipc_msg_priority;neat_ipc_msg_priority268,9588
} neat_ipc_msg_ret;neat_ipc_msg_ret275,9722
typedef struct neat_ipc_info_ neat_ipc_info_283,9870
} neat_ipc_info;neat_ipc_info291,10137
#define IPC_RESET	295,10180
#define	IPC_INITED	296,10221
#define NEAT_PAGE_BITS_SHIFT 300,10308
#define DSX1_MAIL_FROM_C(DSX1_MAIL_FROM_C304,10528
#define MSG_TYPE_NONE 309,10728
#define MSG_TYPE_FULL 310,10758
#define MSG_LOOPBACK 311,10788
#define MSG_ERROR 312,10818
#define MSG_CARRIER 313,10848
#define MSG_DTMF0 314,10878
#define MSG_DTMF1 315,10909
#define BOARD_STATUS_OFFSET 317,10941
#define OTHER_STATUS_OFFSET 319,11091
#define BPV_COUNT_OFFSET 321,11245
#define PCV_COUNT_OFFSET 323,11399
#define CRC_COUNT_OFFSET 325,11553
#define FBE_COUNT_OFFSET 327,11707
#define LOOPBACK_OFFSET 329,11861
#define ERRORS_OFFSET 331,12015
#define REM_LPBK_PASS 336,12240
#define REM_LPBK_FAIL 337,12274
#define LINE_LP_NET 338,12308
#define NO_LINE_LP_NET 339,12342
#define PAYLD_LP_NET 340,12376
#define NO_PAYLD_LP_NET 341,12410
#define DSX1_CARRIER_AVAIL(DSX1_CARRIER_AVAIL344,12522
#define CARRIER_UP 348,12652
#define CARRIER_DOWN 349,12680
#define CARRIER_MASK 350,12708
#define BUS_ERROR 351,12736
#define ILLEGAL 352,12770
#define NEAT_ERR_MASK 353,12804
#define DSX1_MAIL_TO_C(DSX1_MAIL_TO_C356,12908
#define MPU_MSG_DISABLED 359,13029
#define MPU_MSG_FRAME 360,13095
#define MPU_MSG_LINECODE 361,13152
#define MPU_MSG_LPBK_LOC 362,13212
#define MPU_MSG_LPBK_REM 363,13278
#define MPU_MSG_CLK_SOURCE 364,13344
#define MPU_MSG_FDL_ATT 365,13413
#define MPU_MSG_FDL_ANSI 366,13464
#define MPU_MSG_FDL_NONE 367,13516
#define MPU_MSG_FDL 368,13568
#define DSX1_FW_VERSION 370,13606
#define DSX1_RELOAD_DONE 371,13695
#define DSX1_FROM_DTMF(DSX1_FROM_DTMF374,13848
#define MSG_RDY_DIGCOL 379,14011
#define MSG_1STDIG_TO 380,14041
#define MSG_1STDIG_RCV 381,14071
#define MSG_DIG_DONE 382,14101
#define DSX1_TO_DTMF(DSX1_TO_DTMF385,14199
#define RESET_REQ 387,14257
#define READY_COL 388,14285
#define FIRST_DIGIT_TO 389,14313
#define FIRST_DIGIT_RCV 390,14341
#define ALL_DIGIT_RCV 391,14369
typedef void (*neat_ipc_msg_function_t)neat_ipc_msg_function_t394,14431
typedef void (*neat_ipc_msg_function_t)(uchar,uchar394,14431
typedef struct csm_message_t_ csm_message_t_396,14488
} csm_message_t;csm_message_t413,15306
#define NEAT_LONGWORD_WR(NEAT_LONGWORD_WR420,15376
#define NEAT_LONGWORD_RD(NEAT_LONGWORD_RD426,15527
	MSG_POOL_RCV 430,15641
	MSG_POOL_XMT	431,15706
