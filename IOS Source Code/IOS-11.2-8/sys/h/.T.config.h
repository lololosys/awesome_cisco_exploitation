
config.h,4271
#define __CONFIG_H__137,4840
#define SABUFSIZE 142,4906
#define TOKLEN 143,4970
#define WORD 144,5017
#define LINE 145,5071
#define CONFIG_TERM	151,5162
#define CONFIG_ABORT	152,5228
#define CONFIG_DONE	153,5292
#define	CONFIG_HTTP	154,5345
#define	KEYWORD_MATCHED	155,5405
#define	COMMAND_AUTHORIZE	156,5467
#define	COMMAND_FUNCTION	157,5541
#define	HIDE_PASSWORDS	158,5613
#define	PARSE_EMPTY_STRING	159,5688
#define NOMEANSZERO	161,5759
#define NONULL	162,5812
#define NOMEANSMINUS	163,5858
#define NOMEANSNULL	164,5913
#define NULLMEANSDONE	165,5969
#define IN	167,6028
#define OUT	168,6068
enum RES RES175,6299
    RES_NULL,176,6310
    RES_HEURISTIC,177,6345
    RES_RARP,178,6400
    RES_BOOTP,179,6434
    RES_CONFIG,180,6471
    RES_MANUAL,181,6514
    RES_NONVOL,182,6551
    RES_PRIVATE_NONVOL,183,6602
    RES_WRITING_NONVOL	184,6668
#define NVMAGIC	191,6790
#define NVVERSION 192,6819
#define NVCOMPRESSVERSION 193,6848
#define UNIX_COMPRESS 195,6878
struct nvtype_ nvtype_197,6908
#define NVMAGIC_PRIV	218,8011
#define NVVERSION_1_PRIV 219,8039
struct nvtype_private_ nvtype_private_221,8071
typedef struct namedthingtype_ namedthingtype_238,8502
} namedthingtype;namedthingtype242,8608
#define PARSEBUF	250,8856
#define MAXTOKLEN	251,8903
#define MAXPROMPTLEN	252,8951
struct rtrname rtrname257,9069
typedef struct save_buf_ save_buf_268,9498
} save_buf;save_buf272,9677
typedef struct ambig_struct_ ambig_struct_277,9700
} ambig_struct;ambig_struct281,9812
typedef void (*trans_func)trans_func297,10579
typedef void (*trans_func)(parseinfo *, transition transition297,10579
struct transition_ transition_298,10654
typedef struct dynamic_transition_ dynamic_transition;dynamic_transition308,10857
struct dynamic_transition_ dynamic_transition_309,10912
typedef void (*command_func)command_func316,11061
typedef void (*command_func)(parseinfo parseinfo316,11061
#define UIARRAY_SIZE 326,11465
struct parseinfo_ parseinfo_327,11527
#define csb_start_data 491,18501
#define csb_end_data 492,18558
#define PFLAG_in_help	494,18624
#define PFLAG_nvgen	495,18671
#define PFLAG_sense	496,18716
#define	PFLAG_which	497,18760
#define	PTXT_0 499,18805
#define PTXT_1 500,18834
#define PTXT_2 501,18857
#define PTXT_3 502,18880
#define PTXT_4 503,18903
#define PTXT_5 504,18926
#define PTXT_6 505,18949
#define	PTXT_prompt	506,18972
#define	PTXT_line	507,19000
#define	PTXT_break_chars	508,19024
#define	PCHAR_lastchar	510,19062
#define PIDB_current 512,19096
#define PIDB_1 513,19127
#define	PCDB_current	515,19148
#define	PCDB_1	516,19180
#define	PMXINT_current	518,19202
#define	PMXINT_1	519,19237
#define PPDB_current 521,19262
#define PPDB_1 522,19292
#define PROUTEMAP_1 523,19312
#define PUDB_current1 525,19343
#define PUDB_current2 526,19382
#define PUDB_1 527,19421
#define PUDB_2 528,19441
#define PTTY_current 530,19462
#define PTYPE_int(PTYPE_int532,19488
#define	PTYPE_char(PTYPE_char533,19536
#define PTYPE_string(PTYPE_string534,19574
#define PTYPE_paddr(PTYPE_paddr535,19626
#define PTYPE_hwaddr(PTYPE_hwaddr536,19690
#define PTYPE_maskedAddr(PTYPE_maskedAddr537,19757
#define PTYPE_idb(PTYPE_idb538,19830
#define	PTYPE_cdb(PTYPE_cdb539,19897
#define	PTYPE_mxint(PTYPE_mxint540,19965
#define PTYPE_pdb(PTYPE_pdb541,20038
#define PTYPE_routemap(PTYPE_routemap542,20104
#define PTYPE_udb(PTYPE_udb544,20182
#define PTYPE_tty(PTYPE_tty546,20253
#define PTYPE_flag(PTYPE_flag547,20306
#define PPTR_int549,20364
#define PPTR_string550,20381
#define PPTR_paddr 551,20401
#define PPTR_hwaddr 552,20422
#define PPTR_maskedAddr 553,20444
#define PPTR_idb554,20470
#define PPTR_cdb555,20487
#define PPTR_mxint556,20504
#define PPTR_pdb557,20523
#define PPTR_routemap558,20540
#define PPTR_udb559,20562
#define PPTR_flag560,20579
#define PPTR_tty561,20597
#define GETOBJ(GETOBJ563,20615
#define	SETOBJ(SETOBJ564,20698
#define ISOBJ(ISOBJ565,20760
typedef boolean (*match_gen_func)match_gen_func612,22489
				  int, uint, struct idbtype_ idbtype_613,22559
typedef char * (*short_help_func)short_help_func614,22596
typedef char * (*short_help_func)(parseinfo *, uint, struct idbtype_ idbtype_614,22596
