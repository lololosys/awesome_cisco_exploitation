
ttysrv.h,14038
#define __TTYSRV_H__219,8499
# define DEFAULT_NVTYS 231,8798
# define TTY_ESCAPECHAR 233,8868
# define MAXINCHARS 234,8932
# define MAXOUTCHARS 235,8998
# define TTY_MESSAGEQ 236,9046
# define TTY_ASYNCMSGQ 237,9110
# define DEF_TCP_COALESCE 238,9180
# define LOG_SYNC_DEF_SIZE 241,9293
# define LOG_SYNC_DEF_LEVEL 242,9373
# define LOG_SYNC_IS_UNLOCK 243,9451
# define LOG_SYNC_IS_LOCK 244,9521
# define LOG_SYNC_IS_REQ 245,9595
# define LOG_SYNC_IS_USED 246,9663
# define LOG_SYNC_RPRNT_REQ 247,9739
# define LOG_SYNC_RPRNT_GRANTED 248,9802
# define LOG_SYNC_RPRNT_BUFFER 249,9870
# define LOG_SYNC_RPRNT_READLINE 250,9944
# define LOG_SYNC_RPRNT_BUFSIZE 251,10020
# define LOG_SYNC_LOCK(LOG_SYNC_LOCK253,10092
# define LOG_SYNC_UNLOCK(LOG_SYNC_UNLOCK254,10162
# define LOG_SYNC_USED(LOG_SYNC_USED255,10234
# define LOG_SYNC_REQUEST(LOG_SYNC_REQUEST256,10304
# define LOG_SYNC_REPRINT_REG(LOG_SYNC_REPRINT_REG262,10547
# define LOG_SYNC_REPRINT_END(LOG_SYNC_REPRINT_END268,10723
# define AUTOSPEED 272,10831
# define AUTOSPEEDHIGH 273,10886
#define ASYNC_MINSPEED 275,10957
#define ASYNC_MAXSPEED 276,11021
#define ASYNC_DEFSPEED 277,11085
# define MDMDBG 279,11154
# define MODEMWAIT 284,11238
# define RINGWAIT 285,11300
# define IDLE_SESSION_DEFAULT 287,11362
# define SESSIONTIMEOUT_WARNING_DEFAULT 288,11430
# define SESSIONTIMEOUT_WARNING_MIN 289,11510
# define IDLE_EXEC_DEFAULT 291,11620
# define ABSOLUTE_TIMEOUT_DEFAULT 292,11694
# define LOGOUT_WARNING_DEFAULT 293,11768
# define MAX_TTY_OUTPUT_WAIT 294,11845
# define LENGTH_DEFAULT 296,11992
# define WIDTH_DEFAULT 297,12047
# define MAXTTYLENGTH 298,12100
# define MAXTTYWIDTH 299,12155
# define HISTORY_SIZE_DEFAULT	301,12210
# define MAX_HISTORY_SIZE	302,12270
# define CH_NUL 304,12328
# define CH_SOH 305,12357
# define CH_STX 306,12386
# define CH_ETX 307,12415
# define CH_EOT 308,12444
# define CH_ENQ 309,12473
# define CH_ACK 310,12502
# define BELL 311,12531
# define CH_BS 312,12575
# define CH_HT 313,12605
# define LINEFEED 314,12641
# define CH_VT 315,12694
# define CH_FF 316,12724
# define RETURN 317,12754
# define CH_SO 318,12812
# define CH_SI 319,12842
# define CH_DLE 320,12872
# define CTRLQ 321,12903
# define CH_DC2 322,12942
# define CTRLS 323,12973
# define CH_DC4 324,13013
# define CH_NAK 325,13044
# define CH_SYN 326,13075
# define CH_ETB 327,13106
# define CH_CAN 328,13137
# define CH_EM 329,13168
# define CH_SUB 330,13198
# define CH_ESC 331,13229
# define CH_FS 332,13260
# define CH_GS 333,13279
# define CH_RS 334,13298
# define CH_US 335,13317
# define CH_BREAKPOINT 336,13336
# define CH_SPACE 337,13394
# define CH_DEL 338,13430
# define CH_UNDEFINED 339,13451
#define NSPECIAL 349,13834
# define NVT_EC2	350,13901
# define NVT_SYNCH 351,13966
# define NVT_BRK 352,14027
# define NVT_IP 353,14077
# define NVT_AO 354,14129
# define NVT_AYT 355,14176
# define NVT_EOR 356,14224
# define NVT_ABORT 357,14272
# define NVT_EOF 358,14320
# define NVT_SUSP 359,14365
# define NVT_EC 360,14415
# define NVT_EL 361,14465
# define NVT_EW 362,14510
# define NVT_RP 363,14555
# define NVT_LNEXT 364,14602
# define NVT_XON 365,14657
# define NVT_XOFF 366,14704
# define NVT_FORW1 367,14750
# define NVT_FORW2 368,14813
# define NVT_MAX 370,14922
# define NVT_EL2	371,14985
# define BREAK_CHAR 375,15053
# define ESCAPED_FLAG 376,15121
# define PARITY_NO	378,15187
# define PARITY_ODD	379,15240
# define PARITY_EVEN	380,15283
# define PARITY_SPACE	381,15328
# define PARITY_MARK	382,15375
# define STOPBITS1	384,15421
# define STOPBITS2	385,15459
# define STOPBITS15	386,15498
# define DIALOUT 393,15584
# define FIRSTESC 394,15641
# define ESCAPED 395,15711
# define PSIENA	396,15774
# define RINGIN	397,15832
# define IDLE	398,15899
# define READY	399,15948
# define HANGUP	400,15999
# define CONNECTED 401,16051
# define CARDROP 402,16121
# define HASPROC 403,16183
# define LASTWASCR 404,16256
# define HOLDING 405,16327
# define WANTXON 406,16395
# define SENTXOFF 407,16466
# define AUTOBAUDING 408,16535
# define NOBANNER 409,16605
# define IDLETIMEOUT 410,16669
# define LASTWASBREAK 411,16738
# define SLIPMODE 412,16804
# define SENDINGBREAK 413,16865
# define CALLBACK_IN_PROGRESS 414,16938
# define LASTWASSPEC 415,17007
# define SENDXOFF 416,17083
# define CARDCONSOLE 417,17154
# define STOPPEDINPUT 418,17217
# define SENDBREAK 419,17290
# define CONTROLC 420,17360
# define CTRLC_ENBL 421,17436
# define PPPMODE 422,17497
# define VTYASYNC_LINE_STATE 423,17558
# define MODEM_DETECTED 424,17636
# define CTS_MODEM_DETECT_2 429,17752
# define MODEM_CONFIGURED_2 430,17826
# define MODEM_AUTOCONFING_2 431,17896
# define MODEM_SPEED_LOCKED_2 432,17967
# define BUSYOUT_2 433,18045
#define  CLAIM_TTY(CLAIM_TTY438,18160
#define  RELEASE_TTY(RELEASE_TTY439,18211
#define  TTY_IN_USE(TTY_IN_USE440,18265
#define TTY_PLATFORM_DEF_CAPABILITIES(TTY_PLATFORM_DEF_CAPABILITIES445,18379
# define STATBITS_PERM 454,18723
#define MAX_FILE_SYS_DEVICE_ID_LEN 456,18799
typedef unsigned int modem_signal;modem_signal462,18892
} modem_status;modem_status471,19265
# define PSIFLAGS 473,19282
# define LINESTATE 474,19386
# define LINESTATE_2 475,19445
#define	AC_NONE	485,19690
#define	AC_PASSWORD	486,19709
#define	AC_TACACS	487,19731
#define AC_LOCAL	488,19751
#define AAC_LOCAL	493,19817
#define AAC_TACACS	494,19837
#define AAC_SINGLELINE	495,19858
#define ROTARY_ASCENDING 500,19917
#define ROTARY_DESCENDING 501,19995
#define ACF_RELOGIN 506,20106
#define ACF_CALLBACK_NOVERIFY 507,20169
#define ACF_CALLBACK_ROTARY 508,20234
#define ACF_CALLBACK_EXEC 509,20296
#define ACF_CALLBACK_PPP 510,20348
#define ACF_CALLBACK_ARAP 511,20400
#define ACF_CALLBACK_MASK 512,20453
# define NO_FLUSH_ACTIVATE 518,20601
# define REMOTE_MOD 519,20671
# define NOTIFY	520,20737
# define NOEXEC	521,20796
# define PRIVATE_TTY 522,20862
# define TRANSPARENT 523,20936
# define SFLOWIN 524,20995
# define SFLOWOUT 525,21064
# define AUTOBAUD 526,21128
# define LOGGING 527,21204
# define HFLOWIN 528,21267
# define HFLOWOUT 529,21328
# define MODEMIN 530,21390
# define MODEMOUT 531,21452
# define MODEMCTS 532,21514
# define MODEMRI 533,21579
# define MODEMINOUT 534,21646
# define MAYSLIP 535,21725
# define MUSTSLIP 536,21791
# define AUTOHANGUP 541,21932
# define REFUSEECHO 542,21984
# define REFUSESGA 543,22041
# define REFUSEALL 544,22101
# define BREAKSYNC 545,22144
# define MODEMDTR 546,22201
# define LOCKABLE 547,22272
# define BREAKONIP 548,22340
# define OUTIDLE 549,22401
# define NOEXECBANNER 550,22474
# define INSECURE 551,22529
# define AUTOHIGH 552,22587
# define MAYARAP 553,22656
# define MUSTARAP 554,22716
# define IPONBREAK_2 561,22880
# define MA_DISCOVERY_2	562,22948
# define MA_FIXED_2	563,23022
# define NO_MODEM_ERRCNTRL_2	564,23095
# define NO_MODEM_COMPRESS_2	565,23170
# define MODEM_BUSYOUT_2	566,23239
# define BUSYOUT_OVERRIDE_2	567,23300
# define FLOWPERMANENT_2 568,23371
# define NTP_CLOCK_2	569,23438
# define PPS_CLOCK_2	570,23500
# define RCV_SUPPR_CLOCK_2	571,23569
# define NTP_BITS_2 572,23639
# define NOMOTDBANNER_2	573,23707
# define UNUSED13_2	574,23760
# define UNUSED14_2	575,23788
# define UNUSED15_2	576,23816
# define UNUSED16_2 577,23844
# define UNUSED17_2	578,23876
# define UNUSED18_2	579,23905
# define UNUSED19_2	580,23934
# define UNUSED20_2	581,23963
# define UNUSED21_2	582,23992
# define UNUSED22_2	583,24022
# define UNUSED23_2	584,24052
# define UNUSED24_2	585,24082
# define UNUSED25_2	586,24112
# define UNUSED26_2	587,24143
# define UNUSED27_2	588,24174
# define UNUSED28_2	589,24205
# define UNUSED29_2	590,24236
# define UNUSED30_2	591,24268
# define UNUSED31_2	592,24300
# define UNUSED32_2	593,24332
#define FIRST_CAP 598,24414
#define SECOND_CAP 599,24478
# define MODEMMASK 607,24630
#define MODEMTYPE_NONE	616,24784
#define MODEMTYPE_LOCAL 617,24856
#define MODEMTYPE_DIALIN 623,24978
#define MODEMTYPE_DIALOUT 624,25047
#define MODEMTYPE_DIALBOTH 625,25105
#define MODEMTYPE_HOST 626,25157
#define MODEMTYPE_OLD 627,25215
#define MODEMTYPE_LIGHTS 628,25272
#define MODEMTYPE_CTS 629,25333
#define MODEMTYPE_PRINTER 630,25396
#define MODEMSTATE_IDLE 634,25505
#define MODEMSTATE_READY 635,25567
#define MODEMSTATE_HANGUP 636,25608
#define MODEMSTATE_CARDROP 637,25684
#define MODEMSTATE_CONDROP 638,25743
#define MODEMSTATE_DIALING 639,25807
#define MODEMSTATE_WAITING 640,25867
#define MODEMSTATE_RINGING 641,25933
#define MODEMSTATE_ACTIVEI 642,25997
#define MODEMSTATE_ACTIVEU 643,26065
#define MODEMSTATE_ACTIVEC 644,26126
#define MODEMBIT_CD	652,26451
#define MODEMBIT_CTS	653,26496
#define MODEMBIT_DTR	654,26538
#define MODEMBIT_RTS	655,26581
#define MODEMBIT_CDX	657,26625
#define MODEMBIT_CTSX	658,26676
# define BAD_LINE 664,26769
# define CTY_LINE 665,26805
# define TTY_LINE 666,26845
# define VTY_LINE 667,26894
# define STR_LINE 668,26938
# define AUX_LINE 669,26989
#define MAXASCII 676,27079
struct tty_editdata_ tty_editdata_687,27529
#define TTY_DISP_DISP 698,27867
#define TTY_DISP_EDIT 699,27927
typedef struct tty_userdata_ tty_userdata_708,28295
} tty_userdata;tty_userdata744,30250
#define tty_txspeed 746,30267
#define tty_rxspeed 747,30305
#define tty_parity 748,30343
#define tty_databits 749,30379
#define tty_stopbits 750,30419
#define tty_stopchar 751,30459
#define tty_startchar 752,30499
#define tty_width 753,30541
#define tty_length 754,30575
#define tty_escapechar 755,30611
#define tty_holdchar 756,30655
#define tty_dispatchtimeout 757,30695
#define tty_editinfo 758,30749
#define tty_transport_pref 759,30789
#define tty_lat_groupmask 760,30841
#define tty_datamask 761,30891
#define tty_specialmask 762,30931
#define tty_dns 763,30977
#define tty_execmask 764,31007
#define	tty_historyon 765,31047
#define	tty_historysize 766,31089
#define	tty_editingon 767,31135
#define tty_user_server 769,31251
#define tty_slip_server 770,31297
#define	tty_help 771,31343
#define tty_ip_netmask_format 772,31375
#define tty_tcp_coalesce_size 773,31433
#define tty_passspeed 774,31491
#define tty_maxpassspeed 775,31533
#define tty_absolutetimeout 776,31581
#define tty_sessiontimeout 777,31635
#define tty_exectimeout 778,31687
#define conncount 779,31733
typedef struct texti_history_struct_ texti_history_struct_781,31763
} texti_history_struct;texti_history_struct789,31924
typedef struct texti_struct_ texti_struct;texti_struct791,31949
typedef int (*texti_function)texti_function792,31992
typedef int (*texti_function)(texti_struct texti_struct792,31992
typedef void (*rxserv_t)rxserv_t793,32039
typedef void (*rxserv_t)(tt_soc *, uchar uchar793,32039
typedef uint (*txserv_t)txserv_t794,32089
typedef uint (*txserv_t)(tt_soc *, uchar uchar794,32089
struct	texti_struct_ texti_struct_796,32140
typedef struct tty_priv_ tty_priv_839,34139
} tty_priv;tty_priv845,34433
#define SET_PRIV(SET_PRIV850,34486
#define SET_DEFPRIV(SET_DEFPRIV853,34577
#define CUR_PRIV(CUR_PRIV854,34633
#define CUR_DEFPRIV(CUR_DEFPRIV855,34673
#define NEED_TO_SET_PRIV(NEED_TO_SET_PRIV856,34715
#define SET_NEED_TO_SET_PRIV(SET_NEED_TO_SET_PRIV857,34765
struct tt_soc_ tt_soc_860,34828
#define SNMP_MODEM_INPUT_SIGNAL 1117,45424
#define SNMP_MODEM_OUTPUT_SIGNAL 1118,45502
struct snmp_signal_info_ snmp_signal_info_1120,45582
enum TTY_PROCESS TTY_PROCESS1129,45845
    TTYP_EXEC,1130,45864
    TTYP_NW_DAEMON,1131,45914
    TTYP_TELNET,1132,45987
    TTYP_CHAT,1133,46050
    TTYP_ARAP,1134,46100
    TTYP_XREMOTE,1135,46151
    TTYP_UNKNOWN1136,46205
#define MAX3270DEFNAME 1160,46790
struct tty_3270def_ tty_3270def_1161,46816
#define NULLTTY 1171,47161
#define CONTTY 1172,47193
typedef enum ntp_setuptype_ ntp_setuptype_1177,47261
} ntp_setuptype;ntp_setuptype1184,47634
    NTP_PPS_DCD_BITNUM,1192,47762
    NTP_PPS_RTS_BITNUM,1193,47786
    NTP_PPS_CTS_BITNUM,1194,47810
    NTP_PPS_DTR_BITNUM,1195,47834
    NTP_PPS_DSR_BITNUM,1196,47858
    NTP_PPS_RI_BITNUM,1197,47882
    NTP_PPS_BITNUM_COUNT	1198,47905
#define NTP_PPS_NONE	1201,47956
#define NTP_PPS_DCD	1202,47979
#define NTP_PPS_RTS	1203,48025
#define NTP_PPS_CTS	1204,48071
#define NTP_PPS_DTR	1205,48117
#define NTP_PPS_DSR	1206,48163
#define NTP_PPS_RI	1207,48209
#define NTP_PPS_INVERTED 1208,48253
typedef struct vectortype_ vectortype_1216,48378
} vectortype;vectortype1253,50559
#define NVECTORS 1255,50575
#define setup(setup1257,50654
#define getc(getc1259,50722
#define clearinput(clearinput1260,50773
#define inputpending(inputpending1261,50836
#define startinput(startinput1268,51127
#define putc(putc1269,51190
#define clearoutput(clearoutput1270,51247
#define startoutput(startoutput1271,51312
#define outputpending(outputpending1272,51377
#define outputblock(outputblock1273,51446
#define outputxoffed(outputxoffed1274,51511
#define putpkt(putpkt1275,51578
#define setspeed(setspeed1277,51676
#define auto_setspeed(auto_setspeed1278,51747
#define auto_baud(auto_baud1279,51815
#define setstopbits(setstopbits1280,51868
#define setparity(setparity1281,51938
#define setdatabits(setdatabits1282,52008
#define sendbreak(sendbreak1283,52078
#define dtrstate(dtrstate1284,52139
#define setflow(setflow1285,52206
#define setsflow(setsflow1286,52263
#define tty_portenable(tty_portenable1295,52558
#define tty_portdisable(tty_portdisable1296,52625
#define tty_dtrstate(tty_dtrstate1298,52695
#define tty_modemsignal(tty_modemsignal1299,52766
#define tty_modemchange(tty_modemchange1300,52835
#define tty_servicehook(tty_servicehook1301,52904
#define tty_noservicehook(tty_noservicehook1302,52989
#define tty_stopoutput(tty_stopoutput1303,53062
#define tty_pppmode(tty_pppmode1304,53129
#define tty_pppmap(tty_pppmap1305,53198
