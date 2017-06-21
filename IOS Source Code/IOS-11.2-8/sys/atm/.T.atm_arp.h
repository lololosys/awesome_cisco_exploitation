
atm_arp.h,1793
#define __HAVE_ATM_ARP_H__106,3610
    RFC1577_EXT_NONE,114,3848
    RFC1577_EXT_BFI,115,3889
#define	MAX_ATM_ARP_HASH	118,3969
typedef struct atm_arp_tbl atm_arp_tbl119,4020
} atm_arp_tbl_t;atm_arp_tbl_t126,4369
typedef struct atm_arpserv_entry_ atm_arpserv_entry_131,4431
} atm_arpserv_entry;atm_arpserv_entry135,4611
typedef struct atm_arp_config_ atm_arp_config_137,4633
#define ATM_ARP_SERVER ATM_ARP_SERVER139,4720
} atm_swint_config;atm_swint_config150,5113
#define	FOR_ALL_ARP_SERVERS(FOR_ALL_ARP_SERVERS158,5378
is_atm_arp 167,5608
is_atm_arp_server 176,5751
is_atm_arp_client 185,5955
#define	ATMARP_CLIENTAGE 198,6175
#define	ATMARP_SERVERAGE 199,6247
#define	ATM_ARP_MAX_SIZE	201,6320
# define ATM_ARHRD 206,6416
# define ATM_ARPPRO	207,6456
# define ATM_ARSPLN	208,6503
# define ATM_ARTPLN	209,6539
# define ATM_ARP_REQ	214,6605
# define ATM_ARP_REPLY	215,6629
# define ATM_INARP_REQ 216,6655
# define ATM_INARP_REPLY 217,6689
# define ATM_ARP_NAK	218,6723
#define	ATM_NSAPA_ADDR	223,6825
#define	ATM_E164_ADDR	224,6854
#define ATM_ADDR_LEN_MASK	225,6882
#define	ATM_ADDRLEN(ATM_ADDRLEN226,6942
#define	ATM_ADDRTYPE(ATM_ADDRTYPE227,6995
typedef struct atm_rfc_arp_ atm_rfc_arp_233,7170
} atm_rfc_arp;atm_rfc_arp256,8171
static inline uchar *arsha 261,8254
static inline uchar *arssa 265,8382
static inline uchar *arspa 269,8548
static inline uchar *artha 273,8716
static inline uchar *artsa 277,8856
static inline uchar *artpa 281,9022
#define	ATM_ARP_HDRBYTES	286,9191
extr_src_ipaddr 290,9324
extr_dst_ipaddr 295,9432
static inline int atm_arp_datasize 304,9636
#define atm_rfc_arp_headstart(atm_rfc_arp_headstart314,9927
typedef struct atm_arp_canon atm_arp_canon316,9986
} atm_arp_canon_t;atm_arp_canon_t325,10420
