
ntp_request.h,5359
struct req_pkt req_pkt199,8951
#define	REQ_LEN_MAC	215,9532
#define	REQ_LEN_NOMAC	216,9577
#define	RESP_HEADER_SIZE	223,9798
#define	RESP_DATA_SIZE	224,9827
struct resp_pkt resp_pkt226,9858
#define	INFO_OKAY	240,10255
#define	INFO_ERR_IMPL	241,10275
#define	INFO_ERR_REQ	242,10333
#define	INFO_ERR_FMT	243,10383
#define	INFO_ERR_NODATA	244,10425
#define	INFO_ERR_AUTH	245,10482
#define	MAXSEQ	250,10571
#define	RESP_BIT	256,10643
#define	MORE_BIT	257,10665
#define	ISRESPONSE(ISRESPONSE259,10688
#define	ISMORE(ISMORE260,10748
#define INFO_VERSION(INFO_VERSION261,10804
#define	INFO_MODE(INFO_MODE262,10871
#define	RM_VN_MODE(RM_VN_MODE264,10921
#define	INFO_IS_AUTH(INFO_IS_AUTH269,11057
#define	INFO_SEQ(INFO_SEQ270,11115
#define	AUTH_SEQ(AUTH_SEQ271,11160
#define	INFO_ERR(INFO_ERR273,11235
#define	INFO_NITEMS(INFO_NITEMS274,11305
#define	ERR_NITEMS(ERR_NITEMS275,11374
#define	INFO_MBZ(INFO_MBZ278,11478
#define	INFO_ITEMSIZE(INFO_ITEMSIZE279,11541
#define	MBZ_ITEMSIZE(MBZ_ITEMSIZE280,11605
#define	IMPL_UNIV	286,11743
#define	IMPL_XNTPD	287,11763
#define	INFO_TS_MAXSKEW_UI	294,11968
#define	REQ_PEER_LIST	303,12101
#define	REQ_PEER_LIST_SUM	304,12153
#define	REQ_PEER_INFO	305,12221
#define	REQ_PEER_STATS	306,12285
#define	REQ_SYS_INFO	307,12341
#define	REQ_SYS_STATS	308,12394
#define	REQ_IO_STATS	309,12442
#define REQ_MEM_STATS	310,12486
#define	REQ_LOOP_INFO	311,12550
#define	REQ_TIMER_STATS	312,12607
#define	REQ_CONFIG	313,12656
#define	REQ_UNCONFIG	314,12706
#define	REQ_SET_SYS_FLAG	315,12766
#define	REQ_CLR_SYS_FLAG	316,12817
#define	REQ_MONITOR	317,12870
#define	REQ_NOMONITOR	318,12916
#define	REQ_GET_RESTRICT	319,12972
#define	REQ_RESADDFLAGS	320,13027
#define	REQ_RESSUBFLAGS	321,13088
#define	REQ_UNRESTRICT	322,13154
#define	REQ_MON_GETLIST	323,13219
#define	REQ_RESET_STATS	324,13286
#define	REQ_RESET_PEER	325,13340
#define	REQ_REREAD_KEYS	326,13398
#define	REQ_DO_DIRTY_HACK	327,13463
#define	REQ_DONT_DIRTY_HACK	328,13518
#define	REQ_TRUSTKEY	329,13561
#define	REQ_UNTRUSTKEY	330,13610
#define	REQ_AUTHINFO	331,13664
#define REQ_TRAPS	332,13722
#define	REQ_ADD_TRAP	333,13777
#define	REQ_CLR_TRAP	334,13819
#define	REQ_REQUEST_KEY	335,13863
#define	REQ_CONTROL_KEY	336,13924
#define	REQ_GET_CTLSTATS	337,13985
#define	REQ_GET_LEAPINFO	338,14053
#define	REQ_GET_CLOCKINFO	339,14108
#define	REQ_SET_CLKFUDGE	340,14165
#define REQ_GET_KERNEL	341,14223
#define	REQ_GET_CLKBUGINFO	342,14287
#define	REQ_SET_PRECISION	343,14348
#define	REQ_MON_GETLIST_1	344,14403
#define	INFO_FLAG_CONFIG	349,14522
#define	INFO_FLAG_SYSPEER	350,14551
#define INFO_FLAG_UNUSED	351,14581
#define	INFO_FLAG_REFCLOCK	352,14610
#define	INFO_FLAG_PREFER	353,14641
#define	INFO_FLAG_AUTHENABLE	354,14671
#define	INFO_FLAG_SEL_CANDIDATE	355,14705
#define	INFO_FLAG_SHORTLIST	356,14742
#define INFO_FLAG_BCLIENT	361,14826
#define INFO_FLAG_AUTHENTICATE	362,14856
#define INFO_FLAG_PLL	363,14891
#define INFO_FLAG_PPS	364,14918
#define INFO_FLAG_PLL_SYNC	365,14958
#define INFO_FLAG_PPS_SYNC	366,14990
#define INFO_FLAG_MONITOR	367,15022
#define INFO_FLAG_FILEGEN	368,15053
struct info_peer_list info_peer_list374,15230
struct info_peer_summary info_peer_summary385,15507
struct info_peer info_peer404,16065
struct info_peer_stats info_peer_stats456,17646
struct info_loop info_loop487,18660
struct info_sys info_sys499,18863
struct info_sys_stats info_sys_stats524,19754
struct old_info_sys_stats old_info_sys_stats542,20443
struct info_mem_stats info_mem_stats559,21079
struct info_io_stats info_io_stats573,21354
struct info_timer_stats info_timer_stats592,21973
struct conf_peer conf_peer603,22236
#define	CONF_FLAG_AUTHENABLE	615,22702
#define CONF_FLAG_PREFER	616,22735
struct conf_unpeer conf_unpeer623,22916
struct conf_sys_flags conf_sys_flags630,23029
#define	SYS_FLAG_BCLIENT	637,23113
#define	SYS_FLAG_AUTHENTICATE	638,23142
#define SYS_FLAG_PLL	639,23176
#define SYS_FLAG_PPS	640,23202
#define SYS_FLAG_MONITOR	641,23228
#define SYS_FLAG_FILEGEN	642,23258
struct info_restrict info_restrict647,23345
struct conf_restrict conf_restrict659,23627
struct info_monitor_1 info_monitor_1670,23854
struct info_monitor info_monitor687,24477
struct old_info_monitor old_info_monitor701,25012
struct reset_flags reset_flags714,25457
#define	RESET_FLAG_ALLPEERS	718,25498
#define	RESET_FLAG_IO	719,25531
#define	RESET_FLAG_SYS	720,25559
#define	RESET_FLAG_MEM	721,25588
#define	RESET_FLAG_TIMER	722,25617
#define	RESET_FLAG_AUTH	723,25647
#define	RESET_FLAG_CTL	724,25677
#define	RESET_ALLFLAGS 726,25707
struct info_auth info_auth734,25940
struct info_trap info_trap750,26479
struct conf_trap conf_trap764,26980
struct info_control info_control775,27245
struct info_leap info_leap797,28193
#define	INFO_LEAP_MASK	811,28775
#define	INFO_LEAP_SEENSTRATUM1	812,28835
#define	INFO_LEAP_OVERRIDE	813,28902
struct info_clock info_clock818,29035
struct conf_fudge conf_fudge839,29376
#define	FUDGE_TIME1	846,29475
#define	FUDGE_TIME2	847,29497
#define	FUDGE_VAL1	848,29519
#define	FUDGE_VAL2	849,29540
#define	FUDGE_FLAGS	850,29561
#define	NUMCBUGVALUES	856,29645
#define	NUMCBUGTIMES	857,29670
struct info_clkbug info_clkbug859,29695
struct info_kernel info_kernel872,29932
