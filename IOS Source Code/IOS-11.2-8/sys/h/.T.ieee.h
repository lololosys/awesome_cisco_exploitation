
ieee.h,6632
#define __IEEE_H__132,4626
#define IEEEBYTES 154,5360
#define IEEEWORDS 155,5413
#define BITSWAP(BITSWAP156,5466
typedef struct ieee_addrs_ ieee_addrs_158,5503
} ieee_addrs;ieee_addrs161,5588
typedef struct sap_hdr_ sap_hdr_180,6278
} sap_hdr;sap_hdr185,6472
#define SAP_RESPONSE	190,6510
#define SAP_GROUP	191,6576
#define SAP_USERDEF	192,6639
#define SAP_STANDARD	193,6704
#define SAP_NULL	198,6819
#define SAP_SMF	199,6841
#define SAP_SMF_GROUP	200,6863
#define SAP_IP	201,6890
#define SAP_ISA_NETMAN	202,6911
#define SAP_SPAN	203,6939
#define SAP_EIA_RS511	204,6961
#define SAP_ISA_STAMAN	205,6988
#define SAP_SNAP	206,7016
#define SAP_CLNS	207,7038
#define SAP_GLOBAL	208,7060
#define SAP_SDE 209,7084
#define SAP_IBM_SNA	214,7190
#define SAP_ULTRA_RTE 215,7241
#define SAP_CONS 216,7311
#define SAP_CMNS 217,7377
#define SAP_3COM	218,7457
#define SAP_NOVELL	219,7506
#define SAP_VINES	220,7559
#define SAP_NETBIOS 221,7611
#define SAP_IBMNM	222,7673
#define SAP_EXTENDED	223,7722
#define SAP_NOVELL_ETHER 224,7774
#define DLSW_SAP 225,7834
#define LLC1_CLASS	238,8223
#define LLC1_CLASS_I1	239,8247
#define LLC1_CLASS_S	240,8274
#define LLC1_CLASS_I2	241,8300
#define LLC1_CLASS_U	242,8327
#define LLC1_UI	244,8354
#define LLC1_XID	245,8376
#define LLC1_TEST	246,8398
#define LLC1_P	247,8421
#define XID_FMT_STD_LEN 249,8443
#define XID_FMT_STD	250,8469
#define XID_CLASS_TYPE1	251,8518
#define XID_CLASS_TYPE2	252,8574
#define XID_WIN_TYPE1	253,8630
#define XID_WIN_TYPE2 254,8696
#define XID_LLC_TYPE	255,8766
#define XID_RCV_WIN	256,8792
#define XID_FMT_MASK	258,8818
#define XID_FMT_0	259,8844
#define XID_FMT_1	260,8867
#define XID_FMT_2	261,8890
#define XID_FMT_3	262,8913
#define XID_FMT_8	263,8936
#define XID_TYPE_MASK	265,8960
#define XID_TYPE_1	266,8987
#define XID_TYPE_2	267,9011
#define XID_TYPE_4	268,9035
#define XID_ERROR_CV 270,9060
#define XID_TYPE0_FMT2 273,9134
#define SAP_HDRBYTES 278,9188
#define SAP_LONGHDRBYTES 279,9254
#define SAPSAP_NOVELL	280,9320
#define SAPSAP_CLNS	281,9396
#define SNAPSNAP	282,9470
#define SAPSAP_SDE 283,9516
#define SAPSAP_NULL	284,9567
#define SAPSAP_VINES	285,9616
#define SAPSAP_NOVELL_ETHER 286,9668
#define SAPSAP_NETBIOS	287,9741
#define SAPSAP_IP	288,9799
#define SAP_CTL_HIZER 293,9922
typedef struct snap_hdr_ snap_hdr_301,10167
} snap_hdr;snap_hdr308,10447
#define SNAP_HDRBYTES 310,10460
#define SNAP_HDRWORDS 311,10485
#define SNAP_OUIBYTES 312,10510
# define TYPE_PUP	322,10722
# define TYPE_XEROX_ARP	323,10760
# define TYPE_XNS	324,10823
# define TYPE_IP10MB 325,10861
# define TYPE_CHAOS	326,10916
# define TYPE_RFC826_ARP 327,10970
# define TYPE_FR_ARP 328,11017
# define TYPE_VINES	329,11073
# define TYPE_VINES_LOOP 330,11118
# define TYPE_VINES_ECHO 331,11182
# define TYPE_MOP_BOOT	332,11233
# define TYPE_MOP_CONSOLE 333,11297
# define TYPE_DECNET	334,11362
# define TYPE_LAT	335,11427
# define TYPE_HP_PROBE	336,11481
# define TYPE_REVERSE_ARP 337,11529
# define TYPE_DEC_SPANNING 338,11581
# define TYPE_ETHERTALK 339,11639
# define TYPE_AARP	340,11695
# define TYPE_APOLLO	341,11744
# define TYPE_VINES2	342,11795
# define TYPE_VINES_ECHO2 343,11865
# define TYPE_NOVELL1	344,11939
# define TYPE_CPP	345,11988
# define TYPE_LOOP	346,12051
# define TYPE_ETHER_ESMP 361,12549
# define TYPE_HDLC_ESMP 372,12989
# define TYPE_HDLC_TAGSWITCH 373,13068
# define TYPE_HDLC_DRIP 374,13128
# define TYPE_HDLC_VSI 375,13198
# define TYPE_HDLC_PAGP 376,13269
# define TYPE_NOVELL2 378,13381
# define TYPE_CLNS 379,13453
# define TYPE_ESIS 380,13504
# define TYPE_IEEE_SPANNING 381,13556
# define TYPE_DLSW 382,13623
# define TYPE_RSRB 383,13678
# define TYPE_STUN 384,13733
# define TYPE_BSTUN 385,13793
# define TYPE_UNCOMPRESSED_TCP 386,13859
# define TYPE_COMPRESSED_TCP 387,13944
# define TYPE_CDP 393,14187
# define TYPE_CGMP 394,14258
# define TYPE_COMPRESSED_FR 395,14336
# define TYPE_VTP	396,14411
# define TYPE_DISL 397,14472
# define TYPE_CATALYST_IPC 398,14550
# define TYPE_CATALYST_SYNC 400,14690
# define TYPE_BRIDGE 403,14827
# define TYPE_NHRP	410,15036
# define TYPE_NMP 415,15119
# define TYPE_L2_8022_PAD 420,15251
# define TYPE_TAG 426,15454
# define TYPE_MTAG 427,15491
#define CISCOOUI 429,15529
#define ETHEROUI 430,15594
#define OLD_ETHEROUI 431,15669
#define SNAP_CTL_OUI 432,15701
#define OUI_MASK 433,15767
#define DEFAULT_OUI 434,15838
#define LLC_DSAP_OFFSET 452,16380
#define LLC_SSAP_OFFSET 453,16406
#define LLC_CONTROL_OFFSET 454,16432
#define LLC_ORG_OFFSET 455,16461
#define LLC_TYPE_OFFSET 456,16486
#define LLC_LSAP_OFFSET 457,16512
#define LLC_DATA_OFFSET 458,16552
typedef struct _llc1_hdr_t _llc1_hdr_t460,16579
} llc1_hdr_t;llc1_hdr_t467,16858
#define TYPECODE_MIN_ACL 469,16873
#define TYPECODE_MAX_ACL 470,16905
#define MACADDR_MIN_STD_ACL 471,16937
#define MACADDR_MAX_STD_ACL 472,16969
#define MACADDR_MIN_EXT_ACL 473,17001
#define MACADDR_MAX_EXT_ACL 474,17033
static inline void ieee_copy 485,17263
static inline void ieee_move 496,17464
static inline void ieee_copy_sans_rii 510,17686
static inline void ieee_zero 519,17862
static inline void ieee_copy_oui 528,17982
static inline void ieee_swap 537,18130
static inline boolean ieee_equal 550,18363
static inline boolean ieee_equal_sans_rii 566,18788
static inline int ieee_equal_oui 582,19198
static inline int ieee_compare 595,19478
static inline boolean is_ieee_zero 612,19785
static inline boolean is_ieee_bcast 620,19907
static inline void ieee_add 635,20148
static inline void ieee_subtract 655,20463
static inline void ieee_mask 675,20798
#define SAP_TEST_AUTORESPONSE	719,21717
#define SAP_IEEE_XID_AUTORESPONSE	720,21755
#define SAP_SNA_XID_AUTORESPONSE	721,21796
#define SAP_TEST_FORWARD	727,21923
#define SAP_IEEE_XID_FORWARD	728,21956
#define SAP_SNA_XID_FORWARD	729,21993
#define SAP_CLS_ACT_SAP 731,22030
#define SAP_PRE_REG_SAP 732,22110
#define MAX_SAP_VALUE	734,22193
#define MAX_SAP_ENTRIES	735,22219
typedef void (*sap_handle_test_t)sap_handle_test_t739,22312
typedef void (*sap_handle_test_t)(idbtype *idb, paktype *pak, boolean usesa)usesa739,22312
typedef void (*sap_handle_ieee_xid_t)sap_handle_ieee_xid_t740,22390
    (idbtype *idb, paktype *pak, boolean usesa)usesa741,22428
typedef void (*sap_handle_sna_xid_t)sap_handle_sna_xid_t742,22477
	void *usapid, boolean isresponse)isresponse743,22543
typedef struct sap_entry_t_sap_entry_t_745,22580
} sap_entry_t;sap_entry_t767,23315
struct sap_registry_t_sap_registry_t_769,23331
