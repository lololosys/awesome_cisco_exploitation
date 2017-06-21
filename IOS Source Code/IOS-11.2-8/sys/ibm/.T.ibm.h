
ibm.h,2427
#define _IBM_H_66,2378
#define STUN_MAGIC 70,2419
typedef struct _stunhdrtype _stunhdrtype73,2515
} stunhdrtype;stunhdrtype79,2726
#define STUN_MIN_GROUP 81,2742
#define STUN_MAX_GROUP 82,2769
#define STUN_OP_PASS	84,2799
#define STUN_OP_NOP	85,2869
#define STUN_OP_IGNORE	86,2916
#define STUN_OP_EMPTY	87,2973
#define STUN_OP_VREQ 88,3043
#define STUN_OP_VREP	89,3115
#define STUN_OP_LINKUP_REQ 91,3249
#define STUN_OP_LINKUP_RSP 92,3314
#define STUN_OP_LINKDOWN_REQ 93,3380
#define STUN_OP_LINKDOWN_RSP 94,3448
#define STUN_OP_ABORT_REQ 95,3517
#define STUN_OP_ABORT_RSP 96,3572
#define STUN_OP_RD_REQ 97,3627
#define STUN_OP_DATA	98,3692
#define STUN_OP_QUENCH_ON 99,3741
#define STUN_OP_QUENCH_OFF 100,3817
#define STUN_OP_SIM 101,3893
#define STUN_OP_LINK_ESTAB 102,3968
#define STUN_OP_REROUTE 103,4043
#define STUN_OP_OPCODE	105,4126
#define STUN_OP_TO_SDLC	110,4331
#define STUN_OP_ERR	113,4438
#define STUN_OP_BADOP	114,4480
#define STUN_OP_UNKPEER 115,4529
#define STUN_OP_UNKADDR	116,4591
#define STUN_MAX_QLEN 118,4656
#define STUN_CONFIG_MAX_QLEN 119,4713
#define STUN_CONFIG_MIN_QLEN 120,4785
typedef enum _stunstatetype _stunstatetype128,4920
} stunstatetype;stunstatetype135,5242
typedef enum _slackstatetype_slackstatetype140,5291
} slackstatetype;slackstatetype154,6080
typedef struct _slackstatistics_slackstatistics156,6099
} slackstatistics;slackstatistics169,6939
#define STUN_KEEPALIVE_DEFAULT_INTERVAL 172,6986
#define STUN_KEEPALIVE_DEFAULT_COUNT 173,7040
#define STUN_KEEPALIVE_TX_PERIOD 174,7084
#define STUN_SLACK_FSM_TIMEOUT_DEFAULT 175,7138
#define STUN_SERIAL_MAXBYTES	184,7443
typedef enum _stuntypetype _stuntypetype186,7478
} stuntypetype;stuntypetype193,7847
typedef enum _stunconnrtn _stunconnrtn195,7864
} stunconnrtn;stunconnrtn200,8123
#define STUN_MIN_ADDR_LENGTH 202,8139
#define STUN_MAX_D_OR_O_ADDR_LENGTH 203,8172
#define STUN_MAX_ADDR_LENGTH 204,8212
#define STUN_PRIORITY_COUNT 205,8245
struct stunpeertype_ stunpeertype_207,8276
typedef struct _stun_protocol_info _stun_protocol_info299,11798
} stun_protocol_info;stun_protocol_info334,14009
typedef struct _stun_tg_info _stun_tg_info339,14092
} stun_tg_info;stun_tg_info345,14453
#define STUN_MAX_OPEN_TRIES	349,14530
#define STUN_REMOPEN_DEFTMO	350,14560
#define MAX_PEER_NAME	351,14602
#define SDLC_PTYPE 353,14629
#define GENERIC_PTYPE 354,14653
