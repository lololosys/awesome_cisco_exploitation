
dmdp.h,925
#define	DMDP_VERSION 26,804
#define	DMDP_ENC 32,861
#define DMDP_ACK 33,941
#define DMDP_NACK 34,1011
#define DMDP_DATA 35,1081
#define DMDP_UNROPT 36,1143
#define DMDP_NOENC 37,1208
#define DMDP_QTIME 45,1390
#define DMDP_RETRIES 46,1435
typedef struct dmdptype_ dmdptype_52,1505
} dmdptype;dmdptype59,1700
typedef struct dmdpdatatype dmdpdatatype61,1713
} dmdpdatatype;dmdpdatatype68,1912
typedef void (*receivetype)receivetype70,1929
typedef void (*receivetype) (paktype *, uchar uchar70,1929
typedef boolean (*notifytype)notifytype71,1979
typedef boolean (*notifytype) (ulong, int, ipaddrtype, boolean)boolean71,1979
typedef struct dmdplisttype_ dmdplisttype_73,2049
}dmdplisttype;dmdplisttype82,2285
#define DMDP_HDR_SIZE 84,2301
#define DMDP_RETRANS_TIME 86,2331
typedef struct dmdpblocktype dmdpblocktype91,2389
}dmdpblocktype;dmdpblocktype97,2524
#define dnsix_dmdp_buginf	99,2541
