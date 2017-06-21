
ipoptions.h,4345
#define __IPOPTIONS_H__71,2512
#define PAKOPT_HASSEC	77,2652
#define PAKOPT_HASLSR	78,2717
#define PAKOPT_HASSSR	79,2775
#define PAKOPT_HASRRT	80,2834
#define PAKOPT_HASSID	81,2886
#define PAKOPT_HASTSTMP	82,2935
#define PAKOPT_SRNOTEND	83,2987
#define PAKOPT_SECSTRIP	84,3054
#define PAKOPT_SECADD	85,3116
#define PAKOPT_HASALERT	86,3173
#define PAKOPT_HASCIPSO	87,3237
#define PAKOPT_HASROUTE 89,3307
#define	IPOPT_FRAGCOPY	131,5620
#define	IPOPT_CLASS	132,5678
#define	  IPOPT_CLASS_CONTROL	133,5733
#define	  IPOPT_CLASS_RESERVED	134,5768
#define	  IPOPT_CLASS_DEBUG	135,5804
#define	  IPOPT_CLASS_RESERVED2	136,5837
#define	IPOPT_NUMBER	137,5874
#define	IPOPT_TLV(IPOPT_TLV143,6011
#define IPOPT_END	145,6063
#define IPOPT_NOP	146,6109
#define IPOPT_BASSEC	147,6146
#define IPOPT_LSR	148,6195
#define IPOPT_TSTMP	149,6245
#define   IPOPT_TSTMP_TYPE0	150,6297
#define   IPOPT_TSTMP_TYPE1	151,6353
#define   IPOPT_TSTMP_TYPE2	152,6414
#define IPOPT_EXTSEC	153,6470
#define	IPOPT_CIPSO	154,6522
#define IPOPT_RRT	155,6575
#define IPOPT_SID	156,6619
#define IPOPT_SSR	157,6660
#define	IPOPT_ZSU	158,6711
#define	IPOPT_MTUP	159,6763
#define	IPOPT_MTUR	160,6815
#define	IPOPT_FINN	161,6867
#define	IPOPT_VISA	162,6917
#define	IPOPT_ENCODE	163,6969
#define	IPOPT_IMITD	164,7016
#define	IPOPT_EIP	165,7066
#define	IPOPT_TR	166,7105
#define	IPOPT_ADDEXT	167,7146
#define IPOPT_ALERT	168,7198
#define PADDING	170,7245
typedef struct ipopt_routetype_ ipopt_routetype_172,7266
} ipopt_routetype;ipopt_routetype177,7502
#define IPOPT_ROUTEHEADERSIZE 179,7522
#define IPOPT_ROUTEMINPTR 180,7554
#define IPOPT_ROUTEMINLEN 181,7582
#define SECURITY_SIZE 183,7654
typedef struct ipopt_bassectype_ ipopt_bassectype_185,7702
} ipopt_bassectype;ipopt_bassectype190,7821
#define RESERVED4 196,7979
#define TOPSECRET 197,8016
#define SECRET 198,8053
#define CONFIDENTIAL 199,8090
#define RESERVED3 200,8127
#define RESERVED2 201,8164
#define UNCLASSIFIED 202,8201
#define RESERVED1 203,8238
#define SECLVL_RESERVED4 204,8275
#define SECLVL_TOPSECRET 205,8309
#define SECLVL_SECRET 206,8343
#define SECLVL_CONFIDENTIAL 207,8377
#define SECLVL_RESERVED3 208,8411
#define SECLVL_RESERVED2 209,8445
#define SECLVL_UNCLASSIFIED 210,8479
#define SECLVL_RESERVED1 211,8513
#define SECINSERT 212,8547
#define SECNONE 213,8586
#define AUTH_GENSER 217,8645
#define AUTH_SIOP_ESI 218,8674
#define AUTH_SCI 219,8703
#define AUTH_NSA 220,8732
#define	AUTH_DOE	221,8761
#define AUTH_CONT 222,8783
#define AUTH_NONE 223,8812
#define AUTH_UNSET 224,8841
#define AUTH_MASK 225,8870
#define MAX_AUTH_STRING 226,8949
#define inclass(inclass228,8977
#define inauthl(inauthl229,9065
#define inauthh(inauthh230,9151
#define SECSTYLE_NONE 234,9283
#define SECSTYLE_ADD 235,9320
#define SECSTYLE_STRIP 236,9357
#define SECSTYLE_FIRST 237,9394
#define SECSTYLE_IGNOREAUTH	238,9431
#define SECSTYLE_EXTSECOK	239,9464
#define SECRESP_BLACKHOLE 244,9563
#define SECRESP_PROHIB 245,9591
#define SECRESP_REQUIRED 246,9619
#define SECRESP_REQNOROOM 247,9647
#define SECRESP_ERROR 248,9675
typedef struct ipopt_extsectype_ ipopt_extsectype_251,9705
} ipopt_extsectype;ipopt_extsectype259,9874
typedef struct ipopt_cipsotype_ ipopt_cipsotype_261,9895
} ipopt_cipsotype;ipopt_cipsotype269,10046
typedef struct ipopt_alerttype_ ipopt_alerttype_271,10066
} ipopt_alerttype;ipopt_alerttype275,10154
#define IPOPT_ALERTSIZE 277,10174
typedef struct ipopt_sidtype_ ipopt_sidtype_279,10201
} ipopt_sidtype;ipopt_sidtype283,10290
#define IPOPT_SIDSIZE 285,10308
typedef struct ipopt_tstmptype_ ipopt_tstmptype_287,10333
} ipopt_tstmptype;ipopt_tstmptype300,10808
#define IPOPT_TSTMPHEADERSIZE 302,10828
#define MAX_NLESO 307,10893
#define MAX_COMPARTMENT_BYTES 309,10975
#define IP_ESO_DENY	313,11057
#define IP_ESO_IGNORE	314,11127
#define IP_ESO_CHECK	315,11195
#define IP_CIPSO_DENY	320,11309
#define IP_CIPSO_IGNORE	321,11373
typedef struct ip_sec_compartment_info_ ip_sec_compartment_info_323,11436
} ip_sec_compartment_info;ip_sec_compartment_info327,11544
enum NLESO_RANGE NLESO_RANGE329,11572
    NLESO_MIN,330,11591
    NLESO_MAX331,11606
typedef struct ip_sec_eso_source_ ip_sec_eso_source_334,11624
} ip_sec_eso_source;ip_sec_eso_source340,11853
