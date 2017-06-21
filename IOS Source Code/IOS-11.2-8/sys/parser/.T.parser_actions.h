
parser_actions.h,7472
#define	ULONG_MAX	170,6367
#define	USHORT_MAX	171,6398
#define SHORTHELP_LEN	172,6425
#define	isodigit(isodigit174,6452
#define is_terminator(is_terminator175,6504
#define CVHTOB(CVHTOB176,6571
#define	INVALID_NVGEN_CHECK\183,6720
#define NO_COMMAND_COMPLETION 197,7060
#define SUPPORT_COMMAND_COMPLETION 198,7102
#define	ADDR_SMDS	200,7145
#define	ADDR_SNPA	201,7167
#define	ADDR_PARSE(ADDR_PARSE203,7190
#define	ADDR_NAME_LOOKUP	205,7222
#define	ADDR_LIL_ENDIAN	206,7258
#define	ADDR_HOST_ADDR_ONLY	207,7294
#define	ADDR_HOST_NET_ONLY	208,7333
#define	ADDR_PROTO_ADDR	209,7371
#define	ADDR_NO_SHORT_HELP	210,7407
#define	ADDR_ALL	211,7445
#define	NUMBER_HEX	215,7525
#define	NUMBER_OCT	216,7586
#define	NUMBER_DEC	217,7642
#define	NO_NUMBER	218,7700
#define	HEX_ZEROX_OK	219,7767
#define	HEX_NO_ZEROX_OK	220,7837
#define	HEX_ZEROX_MATCHED	221,7910
#define	HEX_ZEROX_NOT_MATCHED	222,7984
#define	OCT_ZERO_OK	223,8062
#define	OCT_NO_ZERO_OK	224,8131
#define	OCT_ZERO_MATCHED	225,8203
#define	OCT_ZERO_NOT_MATCHED	226,8273
#define	SIGNED_NUMBER	228,8351
#define	NUMBER_WS_OK	230,8423
#define	NUMBER_NO_WS_OK	231,8493
#define	KEYWORD_WS_OK	232,8566
#define	KEYWORD_NO_WS_OK	233,8637
#define	NUMBER_HELP_CHECK	235,8711
#define	KEYWORD_HELP_CHECK	236,8771
#define	KEYWORD_OR_SET	238,8833
#define	KEYWORD_TEST	239,8903
#define	NUMBER_NV_RANGE	240,8973
#define	STRING_WS_OK	242,9043
#define	STRING_HELP_CHECK	243,9113
#define	STRING_QUOTED	244,9187
#define	PRIV_MIN	268,9937
#define	PRIV_NULL	269,9958
#define	PRIV_USER	270,10013
#define	PRIV_OPR	271,10064
#define	PRIV_CONF	272,10101
#define	PRIV_ROOT	273,10158
#define	PRIV_MAX	274,10213
#define	PRIV_INTERACTIVE 275,10234
#define	PRIV_CHANGED	276,10296
#define PRIV_DISTILLED	277,10362
#define	PRIV_INTERNAL	278,10432
#define	PRIV_UNSUPPORTED 279,10503
#define	PRIV_USER_HIDDEN 280,10572
#define	PRIV_SUBIF	281,10648
#define	PRIV_HIDDEN	282,10709
#define	PRIV_DUPLICATE	284,10820
#define	PRIV_NONVGEN	286,10914
#define	PRIV_NOHELP	287,10979
#define	DEFAULT_PRIV	288,11050
#define TRANS_PRIV_PRIV_MASK 290,11117
#define	TRANS_PRIV_FLAGS_OFFSET	291,11153
#define TRANS_PRIV_FLAGS_GET(TRANS_PRIV_FLAGS_GET292,11187
#define TRANS_PRIV_FLAGS_SET(TRANS_PRIV_FLAGS_SET293,11258
#define	RMATCH_IP	298,11340
#define	RMATCH_OSI	299,11370
#define	RMATCH_NOVELL	300,11401
#define	RMATCH_NONE	301,11435
#define	RMATCH_ARGS	303,11468
#define	RMATCH_NO_ARGS	304,11500
#define	RMATCH_TAG_REQUIRED	305,11535
#define	RMATCH_NO_TAG	306,11574
#define	RMATCH_NO_KEY_HELP	307,11608
#define ROARG_NONE	309,11647
#define ROARG_AS	310,11669
#define ROARG_TAG	311,11689
#define	ROARG_PID	312,11710
#define	PARSE_INTERFACE_NOFLAG	317,11766
#define	PARSE_UNIT_ONLY	318,11819
#define	PARSE_INTERFACE_KEYWORD	319,11890
#define	PARSE_CREATE_IDB	320,11977
#define CON_CONSOLE 326,12095
#define CON_AUX 327,12136
#define CON_TTY 328,12177
#define CON_VTY 329,12218
#define CON_ABSOLUTE 330,12259
#define CON_RANGE 331,12300
typedef struct trans_priv_ trans_priv_338,12502
} trans_priv;trans_priv346,12736
} general_keyword_struct;general_keyword_struct356,13166
} keyword_struct;keyword_struct362,13297
} keyword_mm_struct;keyword_mm_struct369,13444
} keyword_id_struct;keyword_id_struct377,13765
} keyword_id_mm_struct;keyword_id_mm_struct386,14152
} keyword_options;keyword_options392,14278
} keyword_option_struct;keyword_option_struct401,14475
typedef struct dynamic_number_struct_ dynamic_number_struct_403,14501
} dynamic_number_struct;dynamic_number_struct410,14838
typedef const struct dynamic_number_struct_ number_struct;number_struct412,14864
} signed_number_struct;signed_number_struct419,15168
typedef void (*parser_number_func)parser_number_func421,15193
typedef void (*parser_number_func)(parseinfo *, uint *, uint uint421,15193
} number_func_struct;number_func_struct426,15474
typedef void (*parser_number_help_func)parser_number_help_func428,15497
typedef void (*parser_number_help_func)(parseinfo *, uint *, uint uint428,15497
} number_help_func_struct;number_help_func_struct432,15769
#define PARAMS_STRUCT_COMMON 435,15798
} params_struct;params_struct449,16443
} params_struct_alist;params_struct_alist455,16653
} print_struct;print_struct459,16736
} hub_struct;hub_struct465,16935
} ttyline_struct;ttyline_struct472,17169
} help_struct;help_struct477,17262
} char_struct;char_struct481,17333
} string_struct;string_struct486,17501
} general_string_struct;general_string_struct492,17739
} set_struct;set_struct497,17885
} test_struct;test_struct506,18216
} test_bool_struct;test_bool_struct512,18451
} test_int_struct;test_int_struct518,18687
typedef const struct test_func_struct_ test_func_struct_520,18707
} test_line_struct;test_line_struct530,19147
} test_idbstatus_struct;test_idbstatus_struct535,19251
} eols_struct;eols_struct540,19347
} nvgens_struct;nvgens_struct545,19435
} interface_struct;interface_struct553,19800
} addrparse_struct;addrparse_struct560,20094
} addrparse_nf_struct;addrparse_nf_struct568,20386
} ipmask_struct;ipmask_struct573,20542
} ipaddrmask_struct;ipaddrmask_struct579,20753
} hexdata_struct;hexdata_struct586,21057
} routerprotocolinfo;routerprotocolinfo593,21309
} routerparse_struct;routerparse_struct598,21484
} char_struct2;char_struct2603,21592
} month_struct;month_struct610,21727
} day_struct;day_struct617,21858
} timesec_struct;timesec_struct624,21979
} bgp_community_struct;bgp_community_struct630,22095
} portinfo;portinfo639,22343
} portparse_struct;portparse_struct643,22454
typedef boolean (* testexpr_func)testexpr_func645,22475
typedef boolean (* testexpr_func)(parseinfo *, transition transition645,22475
} testexpr_struct;testexpr_struct649,22630
#define PMATCH_EXCLUDE	666,23292
#define PMATCH_INCLUDE	667,23355
#define PMATCH_ALL	669,23419
#define PMATCH_DGRAM	670,23464
#define PMATCH_AARP	672,23521
#define PMATCH_APOLLO	673,23567
#define PMATCH_APPLETALK	674,23595
#define PMATCH_ARP	675,23625
#define PMATCH_BRIDGE	676,23650
#define PMATCH_CDP 677,23678
#define PMATCH_CLNS	678,23715
#define PMATCH_CLNS_ALL_ES	679,23741
#define PMATCH_CLNS_ALL_IS	680,23773
#define PMATCH_COMPRESSED_TCP	681,23805
#define PMATCH_CMNS	682,23840
#define PMATCH_DECNET	683,23866
#define PMATCH_DECNET_NODE	684,23894
#define PMATCH_DECNET_ROUTER_L1	685,23926
#define PMATCH_DECNET_ROUTER_L2	686,23963
#define PMATCH_DLSW 687,24000
#define PMATCH_DECNET_PRIME_ROUTER 688,24037
#define PMATCH_IP	689,24077
#define PMATCH_LLC2	690,24101
#define PMATCH_NOVELL	691,24127
#define PMATCH_PAD	692,24155
#define PMATCH_QLLC 693,24180
#define PMATCH_RSRB	694,24217
#define PMATCH_SNAPSHOT	695,24278
#define PMATCH_STUN	696,24308
#define PMATCH_VINES	697,24354
#define PMATCH_XNS	698,24381
#define PMATCH_BSTUN 699,24406
#define PMATCH_VTP	700,24471
} protoname_struct;protoname_struct705,24673
} protoaddr_struct;protoaddr_struct711,24806
typedef struct link_name_ link_name_713,24827
} link_name;link_name720,24987
typedef struct addr_func_ addr_func_722,25001
} addr_func;addr_func726,25110
typedef struct router_name_ router_name_728,25124
} router_name;router_name731,25236
} transport_struct;transport_struct736,25420
typedef struct priv_struct_ priv_struct_738,25441
} priv_struct;priv_struct741,25515
} mode_struct;mode_struct746,25600
