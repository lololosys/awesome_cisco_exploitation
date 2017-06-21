
probe.h,1975
#define __PROBE_H__36,1171
#define HPCODELEN 41,1228
#define HPEXTEND_PAD 42,1248
#define HPCODELEN_ARPA 43,1304
#define PROBE_VERSION 44,1384
#define HPDSN	45,1408
#define HPEXTENDEDSAP	58,1885
#define NAME_REQUEST 63,1952
#define ADDR_REQUEST 64,1979
#define WHERE_REQUEST 65,2006
#define NAME_REPLY 66,2034
#define ADDR_REPLY 67,2061
#define UNSOL_REPLY 68,2088
#define WHERE_REPLY	69,2115
#define PROXY_REQUEST	70,2140
#define PROXY_REPLY	71,2167
#define DOWN_REPLY	72,2192
#define PROBE_ARP_TYPES	79,2334
#define PROBE_PROXY_TYPES	83,2533
#define PID_ADVANCENET	88,2623
#define PID_TRANSPORT	89,2677
#define PID_IP	90,2731
#define PID_IEEE	91,2749
#define CAPA_ALL	96,2795
#define ADDR_REQLEN 101,2876
#define REPORT_LENGTH 102,2901
#define DOMAIN_LENGTH 103,2926
#define REQUEST_VERSION 104,2950
#define DOMAIN_NUMBER 105,2976
#define ADDR_REPLEN 110,3047
#define HPHDRMACRO 112,3070
#define HPHDRLENGTH 119,3360
#define HPPATHMACRO 121,3404
#define HPPATHLENGTH 142,4298
typedef struct probetype_ probetype_144,4323
} probetype;probetype151,4586
#define PROBETYPELENGTH 153,4600
typedef struct probenametype_ probenametype_155,4651
} probenametype;probenametype160,4806
#define PROBENAMETYPELENGTH 161,4823
typedef struct probenamereptype_ probenamereptype_163,4878
} probenamereptype;probenamereptype166,4946
#define PROBENAMEREPTYPELENGTH 167,4966
typedef struct probepath_ probepath_169,5034
} probepath;probepath171,5079
typedef struct probewhereis_ probewhereis_173,5093
} probewhereis;probewhereis185,5572
typedef struct probewhereisreply_ probewhereisreply_187,5589
} probewhereisreply;probewhereisreply200,6083
#define PROBEWIRLENGTH 202,6105
#define MAXPROBES 208,6219
#define DECAYCOUNT 209,6240
typedef struct probesenttype_ probesenttype_211,6262
} probesenttype;probesenttype215,6351
typedef struct probe_traffic_t_ probe_traffic_t_221,6405
} probe_traffic_t;probe_traffic_t232,6639
