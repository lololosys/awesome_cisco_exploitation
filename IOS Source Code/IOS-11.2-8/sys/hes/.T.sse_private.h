
sse_private.h,6837
#define SSE_REFRESH	47,1603
#define SSE_MINSLEEP	48,1679
#define SSE_UNMAPPED	49,1756
#define SSE_MAXHANG	50,1811
#define SSE_FREE_WATCHDOG 51,1878
#define SSE_SIMULATOR_ICB 53,1957
#define SSE_ICB_SHIFT	54,1988
enum SSE_INST SSE_INST60,2087
    SSE_INST_NOP	61,2103
    SSE_INST_NOP		= 0x00,61,2103
    SSE_INST_CALL	62,2129
    SSE_INST_CALL		= 0x01,62,2129
    SSE_INST_HANG	63,2156
    SSE_INST_HANG		= 0x02,63,2156
    SSE_INST_RET	64,2183
    SSE_INST_RET		= 0x03,64,2183
    SSE_INST_NEXT_DMA	65,2209
    SSE_INST_NEXT_DMA		= 0x04,65,2209
    SSE_INST_AND_PIPE	66,2240
    SSE_INST_AND_PIPE		= 0x05,66,2240
    SSE_INST_LD_COUNT1	67,2271
    SSE_INST_LD_COUNT1		= 0x06,67,2271
    SSE_INST_LD_COUNT0	68,2303
    SSE_INST_LD_COUNT0		= 0x07,68,2303
    SSE_INST_SET_DEC	69,2335
    SSE_INST_SET_DEC		= 0x08,69,2335
    SSE_INST_RST_DEC	70,2365
    SSE_INST_RST_DEC		= 0x09,70,2365
    SSE_INST_LD_SPAGE	71,2395
    SSE_INST_LD_SPAGE		= 0x0A,71,2395
    SSE_INST_XOR_SREG0	72,2426
    SSE_INST_XOR_SREG0		= 0x0B,72,2426
    SSE_INST_RD_RAND	73,2458
    SSE_INST_RD_RAND		= 0x0C,73,2458
    SSE_INST_RD_CKSUM	74,2488
    SSE_INST_RD_CKSUM		= 0x0D,74,2488
    SSE_INST_DONE	75,2519
    SSE_INST_DONE		= 0x0E,75,2519
    SSE_INST_LD_RSLT	76,2546
    SSE_INST_LD_RSLT		= 0x10,76,2546
    SSE_INST_LD_SREG	77,2606
    SSE_INST_LD_SREG		= 0x20,77,2606
    SSE_INST_RD_SREG	78,2666
    SSE_INST_RD_SREG		= 0x30,78,2666
    SSE_INST_ACC_CKSUM	79,2726
    SSE_INST_ACC_CKSUM		= 0x40,79,2726
    SSE_INST_RD_BYTE	80,2758
    SSE_INST_RD_BYTE		= 0x8080,2758
#define SSE_INST_OK	83,2791
#define SSE_INST_OUTCLASS	84,2850
#define SSE_INST_OUTICB 85,2899
#define SSE_INST_OUTRWLEN 86,2947
#define SSE_INST_OUTPTR1 87,2997
#define SSE_INST_OUTPTR2 88,3046
#define SSE_INST_OUTIPXTC 89,3095
#define SSE_INST_OUTSNAPLEN 90,3145
#define SSE_INST_OUTDST1 91,3197
#define SSE_INST_OUTDST2 92,3248
#define SSE_INST_OUTSRC1 93,3299
#define SSE_INST_OUTSRC2 94,3352
#define SSE_INST_LD_SREG_0 95,3405
#define SSE_INST_LD_SREG_1 96,3452
#define SSE_INST_LD_SREG_2 97,3503
#define SSE_INST_LD_SREG_6 98,3554
#define SSE_INST_LD_SREG_D 99,3605
#define SSE_INST_LD_SREG_E 100,3658
#define SSE_INST_LD_SREG_F 101,3711
#define SSE_INST_LD_IPDST	102,3764
#define SSE_INST_LD_IPTCPFLAGS	103,3809
#define SSE_INST_LD_IPSPORT_MSB	104,3859
#define SSE_INST_LD_IPSPORT_LSB	105,3910
#define SSE_INST_LD_IPDPORT_MSB	106,3961
#define SSE_INST_LD_IPDPORT_LSB	107,4012
#define SSE_INST_RD_SREG_0 108,4063
#define SSE_INST_RD_SREG_1 109,4110
#define SSE_INST_RD_SREG_2 110,4161
#define SSE_INST_RD_SREG_6	111,4212
#define SSE_INST_RD_SREG_A	112,4262
#define SSE_INST_RD_SREG_B	113,4314
#define SSE_INST_RD_SREG_C	114,4366
#define SSE_INST_RD_SREG_D 115,4418
#define SSE_INST_RD_SREG_E 116,4471
#define SSE_INST_RD_SREG_F 117,4524
#define SSE_INST_RD_IPTOS	118,4577
#define SSE_INST_RD_IPSRC	119,4622
#define SSE_INST_RD_IPDST	120,4667
#define SSE_INST_RD_IPXHC	121,4712
#define SSE_INST_RD_IPPRO	122,4757
#define SSE_INST_RD_IPFO_MSB	123,4802
#define SSE_INST_RD_IPFO_LSB	124,4850
#define SSE_INST_RD_IPTCPFLAGS	125,4898
#define SSE_INST_RD_IPSPORT_MSB	126,4948
#define SSE_INST_RD_IPSPORT_LSB	127,4999
#define SSE_INST_RD_IPDPORT_MSB	128,5050
#define SSE_INST_RD_IPDPORT_LSB	129,5101
enum SSE_CLASS SSE_CLASS135,5207
    SSE_CLASS_UNKNOWN	136,5224
    SSE_CLASS_FLUSH	137,5251
    SSE_CLASS_IP	138,5276
    SSE_CLASS_AUTON	139,5298
    SSE_CLASS_IPX	140,5323
    SSE_CLASS_BRIDGE	141,5346
    SSE_CLASS_SRB	142,5372
    SSE_CLASS_CLNP	143,5395
    SSE_CLASS_CLNP	= 0xB,143,5395
#define SSE_ROUTING_ATALK	150,5487
#define SSE_ROUTING_VINES	151,5518
#define SSE_ROUTING_APOLLO	152,5549
#define SSE_ROUTING_NOVELL	153,5581
#define SSE_ROUTING_IP	154,5613
#define SSE_ROUTING_DECNET	155,5642
#define SSE_ROUTING_XNS	156,5674
#define SSE_ROUTING_CLNS	157,5704
#define SSE_ROUTING_ALL	158,5734
#define SSE_MAXINDEX 177,6305
#define SSE_MAXEXTRA 178,6345
#define SSE_IP_EXTRA_INST 184,6487
#define SSE_IPX_EXTRA_INST 185,6515
#define SSE_SRB_EXTRA_INST 186,6544
#define SSE_CLNS_EXTRA_INST 187,6573
#define SSE_LOW_TREEMEM_THRESH 193,6645
#define SSE_LOW_RWMEM_THRESH 194,6681
typedef ushort treeaddrtype;treeaddrtype200,6756
typedef int    physaddrtype;physaddrtype201,6785
typedef uchar  treecmptype;treecmptype202,6814
typedef uchar  treeinsttype;treeinsttype203,6842
typedef struct shadowcelltype_ shadowcelltype_209,6897
} shadowcelltype;shadowcelltype213,6999
typedef struct shadowwordtype_ shadowwordtype_215,7018
} shadowwordtype;shadowwordtype217,7082
} spin_type;spin_type229,7257
typedef struct rwdbtype_ rwdbtype_235,7304
} rwdbtype;rwdbtype242,7629
typedef struct matchtype_ matchtype_244,7642
} matchtype;matchtype247,7773
typedef struct spin_cell_ spin_cell_249,7787
} spin_cell;spin_cell253,7926
struct spin_word spin_word255,7940
struct spin_match spin_match259,8014
typedef struct spintype_ spintype_268,8352
} spintype;spintype283,9012
#define LTADDR 285,9025
#define EQADDR 286,9067
#define GTADDR 287,9109
#define LTGRANT	288,9151
#define EQGRANT	289,9194
#define GTGRANT	290,9237
#define LTINST	291,9280
#define EQINST	292,9321
#define GTINST	293,9362
#define OUTRW 294,9403
#define EXTRA 295,9441
#define EXTRACOUNT	296,9479
#define REMAINDER	297,9524
typedef struct ssestattype_ ssestattype_302,9594
} ssestattype;ssestattype306,9688
struct ssetype_ ssetype_312,9737
#define SSE_STAT_STRING 348,11062
#define SSE_INTERN_STRING 349,11109
#define SSE_DEPTH_STRING 350,11150
typedef struct shadowlistmemtype_ shadowlistmemtype_355,11245
} shadowlistmemtype;shadowlistmemtype359,11376
#define SSE_STATE_HWRESET	366,11544
#define SSE_STATE_SWRESET	368,11636
#define SSE_STATE_INITIALIZED	370,11730
#define SSE_STATE_UCODE_ON	371,11806
#define SSE_STATE_UCODE_DISABLED 372,11861
#define SSE_STATE_ATTENTION	375,11956
} sse_tree_walk_opcodes;sse_tree_walk_opcodes387,12139
#define SSE_OFF 392,12201
#define SSE_ON	393,12257
#define SSE_INITREG 399,12309
#define SSE_REWRITE	400,12352
#define SSE_REWRITE_END 401,12394
#define SSE_TREE	402,12440
#define SSE_TREE_END	403,12479
#define SSE_STEPSIZE	404,12522
static inline void sse_init_sw 415,12628
static inline void sse_init_ucond_spin 429,13033
static inline void sse_spinref_plus1 443,13383
static inline void sse_spinref_plus2 455,13594
static inline void sse_spinref_minus1 467,13816
static inline void sse_init_spin_links 478,14015
static inline void sse_poke_treemem 492,14267
typedef spintype *(*sse_recalltype)sse_recalltype516,14752
				    uint localtime, ssestattype *stat)stat517,14818
typedef int (*sse_callback)sse_callback518,14863
			    *stat, queuetype *queue)queue519,14933
