
ipfast_vip.h,2776
#define IPFLOWOK 122,4520
#define IPFLOWERR_NOTCONF 123,4554
#define IPFLOWERR_CKS 124,4588
#define IPFLOWERR_NOCACHE 125,4622
#define IPFLOWERR_MTU 126,4656
#define IPFLOWERR_TQL 127,4690
#define IPFLOWERR_UNSUP 128,4724
#define IPFLOWERR_ACCESS 129,4758
#define IPFLOWERR_ACCT 130,4792
#define IPFLOWTBL_SIZE 132,4827
#define IPFLOWTBL_MASK 133,4887
#define IPFLOW_FREE_RESERVE 134,4931
#define IPFLOW_ALLOC_MAX 135,4963
#define IPFLOW_IDLE_TIMEOUT 136,5001
#define IPFLOWSHOWMAX 137,5041
#define IPFLOW_ALLOC_MAX_HUGE 138,5066
#define IPFLOW_ALLOC_MAX_LARGE 139,5122
#define IPFLOW_ALLOC_MAX_MEDIUM 140,5182
#define IPFLOW_ALLOC_MAX_SMALL 141,5237
#define IPFLOWSTAT_TCP_TELNET 147,5368
#define IPFLOWSTAT_TCP_FTP 148,5403
#define IPFLOWSTAT_TCP_FTPD 149,5438
#define IPFLOWSTAT_TCP_WWW 150,5473
#define IPFLOWSTAT_TCP_SMTP 151,5508
#define IPFLOWSTAT_TCP_X 152,5543
#define IPFLOWSTAT_TCP_OTHER 153,5578
#define IPFLOWSTAT_UDP_TFTP 154,5613
#define IPFLOWSTAT_UDP_OTHER 155,5648
#define IPFLOWSTAT_ICMP 156,5683
#define IPFLOWSTAT_IGMP 157,5719
#define IPFLOWSTAT_IPINIP 158,5754
#define IPFLOWSTAT_GRE 159,5789
#define IPFLOWSTAT_IP_OTHER 160,5824
#define IPFLOWSTAT_TOTAL 161,5859
#define TCP_FLAGS_FIN 163,5895
#define TCP_FLAGS_RST 164,5922
#define TCP_FLAGS_ACK 165,5949
#define TCP_FLAGS_EST 166,5976
#define X_PORT 171,6096
#define DFS_SUPPORTED_OUTPUT_FLAGS(DFS_SUPPORTED_OUTPUT_FLAGS177,6199
#define DFS_SUPPORTED_CRYPTO(DFS_SUPPORTED_CRYPTO186,6473
} crypto_tag_t;crypto_tag_t199,6768
} ipflow_key_t;ipflow_key_t227,7677
#define fl_ports 229,7694
#define fl_dport 230,7722
#define fl_sport 231,7761
#define fl_misc 232,7800
#define fl_prot 233,7826
#define fl_tos 234,7855
#define fl_input 235,7883
typedef struct ipflow_t_ ipflow_t_252,8501
} ipflow_t;ipflow_t264,9313
typedef struct ipflowshow_t_ ipflowshow_t_270,9388
} ipflowshow_t;ipflowshow_t281,9612
typedef struct ipflowtbl_t_ ipflowtbl_t_288,9788
} ipflowtbl_t;ipflowtbl_t300,10098
typedef struct ipflow_stats_t_ ipflow_stats_t_305,10175
} ipflow_stats_t;ipflow_stats_t312,10330
static inline void update_rx_ip_cntrs 316,10380
static inline void update_tx_ip_cntrs 326,10612
static inline void encap_rewrite_generic 345,11139
static inline encapvectortype get_ipencaps_vector 392,12276
static inline void assign_encaps_and_hwstring 416,12827
static inline uchar ipflow_key_get 473,14285
static inline ulong ipflow_hash 518,15461
static inline ipflow_t *ipflow_hash_lookup 532,15822
static inline ipflow_t *ipflow_alloc 594,17958
static inline void bind_ipcache_to_flow 614,18407
static inline void free_ipcache_from_flow 625,18643
static inline void ipflow_free 637,18859
static inline void ipflow_hash_insert 648,19082
