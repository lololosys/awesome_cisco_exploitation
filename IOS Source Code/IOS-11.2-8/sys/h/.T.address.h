
address.h,3440
#define _CSC_ADDRESS_99,3406
#define ADDR_ILLEGAL	108,3702
#define ADDR_IP	109,3748
#define ADDR_DECNET	110,3785
#define	ADDR_CHAOS	111,3829
#define ADDR_XNS	112,3871
#define ADDR_X121	113,3909
#define	ADDR_APPLE	114,3950
#define ADDR_CLNS	115,3996
#define ADDR_LAT	116,4042
#define ADDR_VINES	117,4084
#define ADDR_CMNS	118,4134
#define ADDR_APOLLO	119,4175
#define ADDR_STUN	120,4220
#define ADDR_NOVELL	121,4261
#define ADDR_QLLC	122,4306
#define ADDR_SNAPSHOT 123,4359
#define ADDR_ILMI 124,4433
#define ADDR_BSTUN 125,4496
#define ADDR_X25PVC	126,4562
#define	ADDR_MAX	127,4615
#define ADDRLEN_IP	133,4698
#define ADDRLEN_DECNET	134,4719
#define	ADDRLEN_CHAOS	135,4744
#define	ADDRLEN_FR 136,4768
#define ADDRLEN_XNS	137,4792
#define ADDRLEN_X121	138,4815
#define	ADDRLEN_APPLE	139,4860
#define ADDRLEN_CLNS	140,4884
#define ADDRLEN_LAT	142,4975
#define ADDRLEN_VINES	144,5057
#define ADDRLEN_CMNS	145,5081
#define ADDRLEN_APOLLO	148,5207
#define ADDRLEN_STUN	149,5232
#define ADDRLEN_NOVELL	150,5255
#define ADDRLEN_QLLC 151,5281
#define ADDRLEN_SNAPSHOT 152,5307
#define ADDRLEN_ILMI 153,5334
#define ADDRLEN_BSTUN	154,5360
#define ADDRLEN_X25PVC	155,5384
#define ADDRLEN_MAX	157,5410
    } bstun_address_t;bstun_address_t162,5472
union addresses addresses164,5535
#define ip_addr	202,6603
#define	decnet_addr	203,6636
#define	chaos_addr	204,6676
#define	xns_addr	205,6714
#define	nov_addr	206,6748
#define	x121_addr	207,6782
#define	apple_addr	208,6818
#define clns_addr	209,6856
#define lat_addr	210,6892
#define vines_addr	211,6926
#define	cmns_addr	212,6964
#define apollo_addr	213,7000
#define stun_addr	214,7040
#define qllc_addr	215,7076
#define snapshot_addr	216,7112
#define ilmi_addr 217,7156
#define bstun_addr	218,7198
struct addrtype_ addrtype_220,7237
static inline void address_copy(231,7387
} SNPAType_e;SNPAType_e276,8961
#define	STATIONLEN_X121	283,9004
#define STATIONLEN_E164 284,9032
#define STATIONLEN_X25PVC	285,9067
#define	STATIONLEN_IEEE48	286,9095
#define	STATIONLEN_IEEE16	287,9123
#define	STATIONLEN_XEROX	288,9151
#define STATIONLEN_CISCO_HDLC	289,9178
#define STATIONLEN_CISCO_TUNNEL	290,9210
#define STATIONLEN_CISCO_MLAPB	291,9244
#define STATIONLEN_LAPB	292,9277
#define STATIONLEN_SMDS48	293,9304
#define STATIONLEN_SMDS64	294,9332
#define STATIONLEN_FR10	295,9360
#define STATIONLEN_CISCO_MSDLC	296,9387
#define STATIONLEN_ULTRA	297,9420
#define STATIONLEN_PPP	298,9447
#define	STATIONLEN_ATMVC	299,9473
#define STATIONLEN_ATMNSAP 300,9500
#define STATIONLEN_ATMESI 301,9535
#define STATIONLEN_SDLC	302,9569
#define STATIONLEN_ROUTEDESC 303,9596
#define STATIONLEN_SLIP	304,9629
#define	STATIONLEN_MAX	306,9657
} SNPATarget_e;SNPATarget_e325,10208
struct hwaddrtype_ hwaddrtype_327,10225
static inline void snpa_clear 343,10627
static inline boolean snpa_is_valid(356,10880
static inline boolean snpa_is_invalid(370,11132
static inline void snpa_copy 379,11262
static inline SNPAType_e snpa_get_type(386,11391
static inline SNPATarget_e snpa_get_target_type(391,11477
static inline ushort snpa_get_length(396,11579
static inline uchar* snpa_copy_to_buffer(401,11665
typedef struct maskedAddressmaskedAddress417,12174
} MaskedAddress, *pMaskedAddress;pMaskedAddress426,12456
void clearMaskedAddress 429,12505
void MaskedAddress2hwaddrtype 442,12738
void hwaddrtype2MaskedAddress 453,12971
