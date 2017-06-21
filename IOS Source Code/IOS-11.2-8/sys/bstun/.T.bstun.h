
bstun.h,2154
#define _BSTUN_H_95,3504
#define BSTUN_MAGIC 97,3523
#define MAXDGRAM_BSTUN 99,3551
#define BSTUN_MIN_ADDR_LENGTH 101,3621
#define BSTUN_MAX_D_OR_O_ADDR_LENGTH 102,3655
#define BSTUN_MAX_ADDR_LENGTH 103,3696
#define BSTUN_MIN_GROUP 105,3731
#define BSTUN_MAX_GROUP 106,3759
#define BSTUN_MAX_ADDRS 107,3789
#define BSTUN_PRIORITY_COUNT 109,3820
#define BEVENT_HARDWARE 111,3852
#define BEVENT_NETWORK 112,3879
#define BEVENT_TIMEOUT 113,3906
typedef struct _bstunhdrtype _bstunhdrtype116,4003
} bstunhdrtype;bstunhdrtype122,4215
#define BSTUN_OP_PASS 124,4232
#define BSTUN_OP_NOP	125,4307
#define BSTUN_OP_IGNORE	126,4357
#define BSTUN_OP_EMPTY	127,4417
#define BSTUN_OP_VREQ 128,4490
#define BSTUN_OP_VREP	129,4564
#define BSTUN_OP_ERR	132,4677
#define BSTUN_OP_BADOP	133,4727
#define BSTUN_OP_UNKPEER 134,4784
#define BSTUN_OP_UNKADDR	135,4853
#define BSTUN_MAX_QLEN 137,4918
#define BSTUN_CONFIG_MIN_QLEN 138,4984
#define BSTUN_CONFIG_MAX_QLEN 139,5056
#define BSTUN_KEEPALIVE_DEFAULT_INTERVAL 142,5155
#define BSTUN_KEEPALIVE_DEFAULT_COUNT 143,5210
#define BSTUN_KEEPALIVE_TX_PERIOD 144,5255
typedef enum _bstunstatetype _bstunstatetype152,5385
} bstunstatetype;bstunstatetype158,5657
        } bstunaddrroutestate_e;bstunaddrroutestate_e165,5993
        } bstunaddrroutestatelist_t;bstunaddrroutestatelist_t169,6102
typedef enum _bstunconnrtn _bstunconnrtn171,6140
} bstunconnrtn;bstunconnrtn175,6341
typedef enum _bstuntypetype _bstuntypetype177,6358
} bstuntypetype;bstuntypetype183,6660
typedef struct bstunpeertype_ bstunpeertype_185,6678
} bstunpeertype;bstunpeertype235,8837
typedef struct _bstun_protocol_info _bstun_protocol_info238,8856
} bstun_protocol_info;bstun_protocol_info266,10564
typedef struct _bstun_protocol_list _bstun_protocol_list271,10652
} bstun_protocol_list;bstun_protocol_list275,10777
#define BSTUN_MAX_OPEN_TRIES	283,10872
#define BSTUN_MAX_PEER_NAME	284,10903
#define B_DEBUG_EVENT 291,10992
#define B_DEBUG_PACKET 292,11027
#define B_DEBUG_GROUP_SIZE 293,11062
typedef struct _debug_group _debug_group295,11098
} debug_group;debug_group299,11205
