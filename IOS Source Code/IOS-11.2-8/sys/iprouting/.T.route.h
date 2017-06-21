
route.h,12065
#define __ROUTE_H__266,10017
#define CLASS_A_BIT 270,10073
#define CLASS_B_BIT 271,10144
#define CLASS_C_BIT 272,10212
#define CLASS_D_BIT 273,10278
#define CLASSA_BIT 275,10334
#define CLASSB_BIT 276,10405
#define CLASSC_BIT 277,10473
#define CLASSD_BIT 278,10536
#define CLASS_A_MASK 280,10589
#define CLASS_B_MASK 281,10649
#define CLASS_C_MASK 282,10709
#define CLASS_D_MASK 283,10769
#define	CLASS_A_SHIFT	285,10833
#define	CLASS_B_SHIFT	286,10891
#define	CLASS_C_SHIFT	287,10949
#define CLASSD_AND_GREATER(CLASSD_AND_GREATER289,11007
#define MAXROUTES 291,11079
#define DEFMAXROUTES 292,11134
#define MAXDEFNETWORKS 293,11183
#define METRIC_DIRECT 295,11242
#define METRIC_INACCESSIBLE 296,11293
#define METRIC_UNKNOWN 297,11361
#define ISIS_MAX_LINK_METRIC 298,11415
#define CONNECTED_DISTANCE 304,11524
#define STATIC_DISTANCE 305,11558
#define IGRP2_SUMMARY_DISTANCE 306,11592
#define BGP_EXTERNAL_DISTANCE	307,11672
#define IGRP2_DISTANCE 308,11743
#define IGRP_DISTANCE 309,11778
#define OSPF_DISTANCE 310,11814
#define ISIS_DISTANCE 311,11850
#define RIP_DISTANCE 312,11886
#define unused1_DISTANCE 313,11922
#define EGP_DISTANCE 314,11958
#define unused2_DISTANCE 315,11994
#define ODR_DISTANCE 316,12030
#define IGRP2_EX_DISTANCE 317,12066
#define BGP_LOCAL_DISTANCE 318,12146
#define BGP_INTERNAL_DISTANCE	319,12216
#define UNKNOWN_DISTANCE 320,12250
#define MINDISTANCE 322,12287
#define MAXDISTANCE 323,12321
typedef struct backuptype_ backuptype_331,12497
} backuptype;backuptype339,12877
#define backup_context_info 341,12892
#define isis_backup_context_info 342,12948
typedef struct masktype_ masktype_348,13038
} masktype;masktype351,13165
#define MAXMASKS	353,13178
typedef masktype masktabletype[masktabletype355,13199
#define sdbtype 586,22722
struct ndbtype_ ndbtype_588,22747
#define ndb_area_id 627,24499
#define ndb_isis_backup 628,24542
#define PROC_ILLEGAL	634,24646
#define PROC_CONNECTED	635,24674
#define PROC_STATIC	636,24704
#define PROC_BGP	637,24731
#define PROC_unused1	638,24755
#define PROC_MOBILE	639,24783
#define PROC_RIP	640,24810
#define PROC_NHRP	641,24834
#define PROC_IGRP	642,24859
#define PROC_EGP	643,24884
#define PROC_OSPF	644,24908
#define PROC_ISIS 645,24933
#define PROC_IGRP2 646,24964
#define	PROC_ISOIGRP	647,24995
#define	PROC_NLSP	648,25023
#define PROC_FSTATIC 649,25048
#define PROC_ODR	650,25079
#define	PROC_MAXPROC	652,25104
#define MAX_IP_PROCESS	654,25135
#define MAXREDIST	656,25164
#define PROC_REDISTMASK 658,25185
#define PROC_NOSPLITMASK 661,25341
#define	PROC_NEWSCHEDULER	666,25465
#define	PROC_NONETWORKCMD	671,25608
#define	PROC_NOSUPERNETS	676,25719
#define PROC_NOAGERT 681,25846
#define PROC_MAINTAINDEFAULT	687,26019
#define PROC_DISCSUBNETMASK 692,26129
#define PROC_NONPERIODIC 697,26244
#define PROC_NOHOLDDOWN 702,26387
#define INDEX2MASK(INDEX2MASK704,26456
#define INDEX2PDB(INDEX2PDB705,26485
#define NDB2PROCTYPE(NDB2PROCTYPE706,26524
#define NDBSUBNETED(NDBSUBNETED707,26586
#define OSPF_IROUTE_TYPES 712,26702
#define OSPF_EROUTE_TYPES 713,26756
#define OSPF_AROUTE_TYPES 714,26814
enum pdb_stack pdb_stack716,26881
    PDB_UNKNOWN,717,26898
    PDB_IP,718,26930
    PDB_OSI,719,26953
    PDB_ATALK,720,26978
    PDB_NOVELL,721,27011
    PDB_ALL	722,27042
enum DISTTYPE DISTTYPE725,27084
    IP_DIST_BETTER,726,27100
    IP_DIST_WORSE,727,27149
    IP_DIST_SAME	728,27196
#define	NET_SUPERNET	739,27472
#define NET_FLOATING_STATIC 740,27526
#define NET_STATIC 741,27585
#define NET_IGRP	742,27649
#define NET_RIP 743,27674
#define NET_ATTACHED	744,27736
#define NET_NLSP 745,27809
#define NET_ISIS 746,27872
#define NET_XROUTE 747,27946
#define NET_CONNECTED 748,28011
#define	NET_PERMANENT	749,28084
#define NET_INTERIOR 750,28157
#define NET_SYSTEM 751,28216
#define NET_VARIABLE 752,28273
#define NET_SUBNETTED 753,28344
#define NET_DEXTERIOR 754,28399
#define NET_SEXTERIOR 755,28475
#define NET_NOREDIST 756,28550
#define NET_SUBNET 757,28627
#define NET_SUMMARY 758,28679
#define	NET_TARGET 759,28732
#define NET_INTERNAL 760,28807
#define NET_PER_USER 761,28881
#define NET_EXTERIOR	763,28957
#define NET_PERMATTR	764,29008
#define MASK_UNKNOWN	770,29142
#define LONGEST_MASK(LONGEST_MASK772,29204
#define SHORTEST_MASK(SHORTEST_MASK773,29238
#define ROBIN_DEFAULT 779,29309
#define IP_RDB_SNAPSHOT_ROUTE 866,32175
#define IP_RDB_ISIS	867,32247
#define	IP_RDB_PERMANENT 868,32305
#define IP_RDB_OSPF	869,32367
struct rdbtype_ rdbtype_871,32426
#define rdb_delay 905,33752
#define rdb_bandwidth 906,33796
#define rdb_mtu 907,33848
#define rdb_nexthop_metric 908,33888
#define rdb_routetimer 909,33950
#define isis_rdb_ndb 913,34033
#define isis_rdb_linkage 914,34077
#define	RDB_CHUNK_MAX	916,34132
#define ospf_rdb_lsdb 919,34186
#define ospf_rdb_area 920,34232
#define MAXPEERS 927,34348
#define MAXNETWORKS 928,34368
#define NETF_PASSIVE 930,34393
#define NETF_BACKDOOR 931,34465
#define MINOUTDELAY 933,34540
#define MAXOUTDELAY 934,34604
typedef struct peertype_ peertype_936,34669
} peertype;peertype943,34834
typedef struct re_metrictype re_metrictype946,34848
} re_metrictype;re_metrictype965,35354
typedef struct re_criteria re_criteria967,35372
} re_criteria;re_criteria970,35477
typedef struct summarytype_ summarytype_976,35620
} summarytype;summarytype983,35853
typedef struct sum_idbtype_ sum_idbtype_985,35869
} sum_idbtype;sum_idbtype989,36029
typedef struct offsettype_ offsettype_994,36092
}offsettype;offsettype997,36164
struct ipinfotype_ ipinfotype_1002,36231
typedef void (*majorupdatecbtype)majorupdatecbtype1053,38018
				   sourcemask)sourcemask1056,38182
typedef void (*add_routecbtype)add_routecbtype1057,38203
				 ipaddrtype mask, backuptype *backup)backup1058,38271
typedef void (*lost_routecbtype)lost_routecbtype1059,38314
typedef void (*lost_routecbtype) (pdbtype *pdb, ndbtype *ndb)ndb1059,38314
typedef boolean (*route_agercbtype)route_agercbtype1060,38377
typedef boolean (*route_agercbtype) (pdbtype *pdb, ndbtype *ndb)ndb1060,38377
typedef void (*redist_callbackcbtype)redist_callbackcbtype1061,38443
				       event, int flag)flag1062,38514
typedef void (*networkcbtype)networkcbtype1063,38544
			       int area_id_format, boolean sense)sense1065,38664
typedef void (*redistributecbtype)redistributecbtype1066,38710
				    sense)sense1067,38783
typedef void (*processcbtype)processcbtype1068,38800
typedef void (*processcbtype) (pdbtype *pdb)pdb1068,38800
typedef void (*cleanupcbtype)cleanupcbtype1069,38846
typedef void (*cleanupcbtype) (pdbtype *pdb)pdb1069,38846
typedef void (*neighborcbtype)neighborcbtype1070,38892
				*csb, boolean sense)sense1071,38968
typedef void (*areacbtype)areacbtype1072,38994
typedef void (*areacbtype) (pdbtype *pdb, parseinfo *csb, boolean sense)sense1072,38994
typedef boolean (*feasiblecbtype)feasiblecbtype1073,39068
                                   ulong bestmetric)bestmetric1074,39131
typedef boolean (*holddowncbtype)holddowncbtype1075,39185
				   *rdb, ulong newmetric)newmetric1076,39256
typedef void (*querycbtype)querycbtype1077,39287
typedef void (*querycbtype) (pdbtype *pdb, idbtype *idb)idb1077,39287
typedef void (*showipprotocbtype)showipprotocbtype1078,39345
typedef void (*showipprotocbtype) (pdbtype *pdb)pdb1078,39345
typedef void (*exteriorcbtype)exteriorcbtype1079,39395
typedef void (*exteriorcbtype) (pdbtype *pdb, ndbtype *ndb, boolean sense)sense1079,39395
typedef int (*distancecbtype)distancecbtype1080,39471
			       int *pdb_peerindex)pdb_peerindex1082,39575
typedef void (*summaryaddresscbtype)summaryaddresscbtype1083,39606
typedef void (*summaryaddresscbtype) (pdbtype *pdb, boolean sense)sense1083,39606
typedef void (*buildbackupinfocbtype)buildbackupinfocbtype1084,39674
				       boolean replaced)replaced1086,39791
typedef void (*rdbcleanupcbtype)rdbcleanupcbtype1087,39821
typedef void (*rdbcleanupcbtype) (pdbtype *pdb, rdbtype *rdb)rdb1087,39821
typedef void (*ndbcleanupcbtype)ndbcleanupcbtype1088,39884
typedef void (*ndbcleanupcbtype) (pdbtype *pdb, ndbtype *ndb)ndb1088,39884
typedef void (*backupcleanupcbtype)backupcleanupcbtype1089,39947
typedef void (*backupcleanupcbtype) (pdbtype *pdb, backuptype *backup)backup1089,39947
typedef void (*ndbinitcbtype)ndbinitcbtype1090,40019
typedef void (*ndbinitcbtype) (pdbtype *pdb, ndbtype *ndb, boolean new_ndb)new_ndb1090,40019
typedef void (*rdbinitcbtype)rdbinitcbtype1091,40096
typedef void (*rdbinitcbtype) (pdbtype *pdb, ndbtype *ndb, rdbtype *rdb)rdb1091,40096
typedef void (*ndbcopycbtype)ndbcopycbtype1092,40170
typedef void (*ndbcopycbtype) (pdbtype *pdb, ndbtype *oldndb, ndbtype *newndb)newndb1092,40170
typedef void (*ndbpdbchangecbtype)ndbpdbchangecbtype1093,40250
				    boolean now_yours)now_yours1094,40328
typedef int (*rdbprintcbtype)rdbprintcbtype1095,40356
typedef int (*rdbprintcbtype) (pdbtype *pdb, rdbtype *rdb, char *bp)bp1095,40356
typedef void (*ndbprintcbtype)ndbprintcbtype1096,40426
typedef void (*ndbprintcbtype) (pdbtype *pdb, ndbtype *ndb)ndb1096,40426
typedef enum DISTTYPE (*distancecmptype)distancecmptype1097,40487
					  ulong, ushort)ushort1098,40558
typedef void (*aclchangecbtype)aclchangecbtype1099,40581
				 boolean sense)sense1100,40655
typedef void (*hopschangecbtype)hopschangecbtype1101,40676
typedef void (*hopschangecbtype) (pdbtype *pdb, uchar)uchar1101,40676
typedef void (*metriccmdcbtype)metriccmdcbtype1102,40732
typedef void (*metriccmdcbtype) (pdbtype *pdb)pdb1102,40732
typedef void (*defmetriccmdcbtype)defmetriccmdcbtype1103,40780
typedef void (*defmetriccmdcbtype) (pdbtype *pdb, boolean sense)sense1103,40780
typedef void (*routeadjcbtype)routeadjcbtype1104,40846
typedef void (*routeadjcbtype) (pdbtype *pdb, idbtype *idb)idb1104,40846
typedef void (*showsumcbtype)showsumcbtype1105,40907
typedef void (*showsumcbtype) (pdbtype *pdb)pdb1105,40907
typedef void (*autosumcmdcbtype)autosumcmdcbtype1106,40953
typedef void (*autosumcmdcbtype) (pdbtype *pdb)pdb1106,40953
union aclU aclU1107,41002
struct pdbtype_ pdbtype_1112,41107
#define	RTYPE_OTHER	1394,54996
#define	RTYPE_INTRA	1399,55076
#define	RTYPE_INTER	1400,55130
#define	RTYPE_EXTERN1	1401,55193
#define	RTYPE_EXTERN2	1402,55253
#define RTYPE_L1 1407,55343
#define RTYPE_L2	1408,55404
#define RTYPE_IGRP2_INT 1414,55489
#define RTYPE_IGRP2_EXT 1415,55550
#define	RTYPE_BGP_INT	1420,55644
#define	RTYPE_BGP_EXT	1421,55706
#define	RTYPE_BGP_LOC	1422,55768
#define RTYPE_NSSA1	1427,55875
#define RTYPE_NSSA2	1428,55943
#define	ROUTE_UP	1432,56033
#define	ROUTE_DOWN	1433,56078
#define	ROUTE_MODIF	1434,56128
#define	DBASE_INACTIVE 1438,56195
#define	DBASE_ACTIVE 1439,56223
#define	DBASE_FREEZE 1440,56251
#define	DBASE_INVALID 1441,56279
#define DEFAULT_ROUTE	1445,56344
typedef struct ipstatictype_ ipstatictype_1452,56442
} ipstatictype;ipstatictype1464,56800
typedef struct addrsumtype_ addrsumtype_1470,56855
} addrsumtype;addrsumtype1485,57255
#define	ADDRSUM_VALID	1487,57271
#define	ADDRSUM_HIDDEN	1488,57325
#define	ADDRSUM_AS_SET	1489,57381
#define	ADDRSUM_AUTOFLTR	1490,57436
#define ADDRSUM_NOT_ADV	1491,57494
#define ADDRSUM_EXACT_MATCH	1492,57558
#define ADDRSUM_OSPF_REDIST	1493,57625
#define ADDRSUM_OSPF_TYPE7	1495,57713
typedef struct spawntype_ spawntype_1503,58001
} spawntype;spawntype1512,58212
#define OSPF_AREA_ID_FORMAT_IGNORE 1514,58226
#define OSPF_AREA_ID_FORMAT_IP	1515,58268
#define OSPF_AREA_ID_FORMAT_INT	1516,58302
typedef struct ndbstattype_ ndbstattype_1523,58441
} ndbstattype;ndbstattype1526,58531
typedef ndbtype *(*table_walk_proc_type)table_walk_proc_type1531,58599
typedef ndbtype *(*table_walk_proc_type)(ndbtype *, ndbtype *, ndbtype ndbtype1531,58599
typedef boolean (*validtesttype)validtesttype1538,58749
				 *ndb)ndb1539,58821
