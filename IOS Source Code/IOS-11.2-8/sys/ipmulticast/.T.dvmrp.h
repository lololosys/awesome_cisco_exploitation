
dvmrp.h,2697
#define __DVMRP_H__111,3947
#define DVMRP_MAX_SIZE 116,3995
#define DVMRP_REPORT_INTERVAL 117,4029
#define DVMRP_PROBE_CODE 119,4074
#define DVMRP_REPORT_CODE 120,4106
#define DVMRP_ASK_NBR_CODE 121,4138
#define DVMRP_NBR_CODE 122,4170
#define DVMRP_ASK_NBR2_CODE 123,4202
#define DVMRP_NBR2_CODE 124,4234
#define DVMRP_PRUNE_CODE 125,4266
#define DVMRP_GRAFT_CODE 126,4298
#define DVMRP_GRAFT_ACK_CODE 127,4330
#define DVMRP_NBR_EXPIRATION 129,4363
#define DVMRP_ENTRY_SIZE 130,4421
#define DVMRP_INFINITY 131,4453
#define DVMRP_END 132,4486
#define DVMRP_MAX_METRIC 133,4521
#define DVMRP_MAX_FILTER 134,4554
#define DVMRP_EXPIRATION_TIME 135,4587
#define DVMRP_PRUNE_TIME 136,4645
#define DVMRP_MIN_PRUNE_TIME 137,4692
#define DVMRP_GRAFT_FREQUENCY 138,4742
#define DVMRP_AGING_RATE 139,4785
#define DVMRP_DEFAULT_METRIC_OFFSET_IN 140,4818
#define DVMRP_DEFAULT_METRIC_OFFSET_OUT 141,4860
#define DVMRP_CLEAR_ROUTES_RCVD 142,4902
#define DVMRP_DEFAULT_COMPLAINT_COUNT 143,4958
#define DVMRP_DEFAULT_SUMMARY_METRIC 144,5002
#define DVMRP_TUNNEL_FLAG 149,5074
#define DVMRP_TUNNEL_SR 150,5106
#define DVMRP_PIM_FLAG 151,5175
#define DVMRP_DOWN_FLAG 152,5242
#define DVMRP_DISABLE_FLAG 153,5274
#define DVMRP_QUERIER_FLAG 154,5306
#define DVMRP_LEAF 155,5338
typedef struct dvmrp_asknbrtype_ dvmrp_asknbrtype_160,5459
} dvmrp_asknbrtype;dvmrp_asknbrtype166,5828
#define DVMRP_ASKNBR_SIZE 168,5849
typedef struct dvmrp_prunetype_ dvmrp_prunetype_173,5966
} dvmrp_prunetype;dvmrp_prunetype177,6073
typedef struct dvmrp_grafttype_ dvmrp_grafttype_182,6149
} dvmrp_grafttype;dvmrp_grafttype185,6228
typedef struct dvmrp_metrictype_ dvmrp_metrictype_191,6375
} dvmrp_metrictype;dvmrp_metrictype198,6791
typedef struct dvmrp_filter_ dvmrp_filter_204,6929
} dvmrp_filter;dvmrp_filter209,7231
typedef struct dvmrp_sumtype_ dvmrp_sumtype_214,7288
} dvmrp_sumtype;dvmrp_sumtype221,7631
typedef struct dvmrptype_ dvmrptype_228,7843
} dvmrptype;dvmrptype238,8445
#define DVMRP_BUGINF 243,8494
#define DVMRP_IN_BUGINF(DVMRP_IN_BUGINF244,8539
#define DVMRP_OUT_BUGINF(DVMRP_OUT_BUGINF248,8714
#define DVMRP_PRUNING_BUGINF 252,8890
#define DVMRP_TUNNEL(DVMRP_TUNNEL255,9001
#define DVMRP_ROUTES 257,9121
#define DVMRP_VALID(DVMRP_VALID258,9156
#define DVMRP_RPF_NBR(DVMRP_RPF_NBR259,9219
#define DVMRP_LEAF_CAPABLE 267,9486
#define DVMRP_PRUNE_CAPABLE 268,9526
#define DVMRP_GENID_CAPABLE 269,9566
#define DVMRP_MTRACE_CAPABLE 270,9606
#define DVMRP_SNMP_CAPABLE 271,9646
#define DVMRP_AUTORP_CAPABLE 272,9686
#define DVMRP_CISCO_VERSION 274,9764
#define DVMRP_PRUNER(DVMRP_PRUNER277,9890
#define DVMRP_34(DVMRP_34281,10050
