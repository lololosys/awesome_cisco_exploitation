
signal_4k.h,1770
#define __SIGNAL_4K_H__31,1036
#define SIGCACHE 39,1172
#define SIGWBERR 40,1211
#define SIGERROR 41,1264
#define SIGRELOAD 42,1334
#define REG_INDEX(REG_INDEX47,1449
#define R4K_EXCEPTION_TABLE	52,1546
typedef struct r4k_context_t_ r4k_context_t_66,1717
} r4k_context_t;r4k_context_t77,2185
#define SP_REGNUM	82,2249
#define RA_REGNUM	83,2271
#define V0_REGNUM	84,2292
#define A0_REGNUM	85,2312
#define A1_REGNUM	86,2332
#define A2_REGNUM	87,2353
#define A3_REGNUM	88,2374
#define RESET_COLD 96,2557
#define RESET_WARM 97,2580
#define RESET_WDOG 98,2603
#define RESET_NMI 99,2626
typedef int (*SIG_PF)SIG_PF106,2710
#define MAX_SIGNAL_NUMBER	108,2744
#define EXCPT_FRAME 123,3089
#define SP_OFS(SP_OFS129,3248
#define REG_ZERO	134,3326
#define REG_AT	135,3345
#define REG_V0	136,3363
#define REG_V1	137,3381
#define REG_A0	138,3399
#define REG_A1	139,3417
#define REG_A2	140,3435
#define REG_A3	141,3453
#define REG_T0	142,3471
#define REG_T1	143,3489
#define REG_T2	144,3507
#define REG_T3	145,3526
#define REG_T4	146,3545
#define REG_T5	147,3564
#define REG_T6	148,3583
#define REG_T7	149,3602
#define REG_S0	150,3621
#define REG_S1	151,3640
#define REG_S2	152,3659
#define REG_S3	153,3678
#define REG_S4	154,3697
#define REG_S5	155,3716
#define REG_S6	156,3735
#define REG_S7	157,3754
#define REG_T8	158,3773
#define REG_T9	159,3792
#define REG_K0	160,3811
#define REG_K1	161,3830
#define REG_GP	162,3849
#define REG_SP	163,3868
#define REG_S8	164,3887
#define REG_RA	165,3906
#define REG_SREG 167,3926
#define REG_LO 168,3951
#define REG_HI 169,3971
#define REG_BADVADDR	170,3998
#define REG_CAUSE 171,4022
#define REG_EPC	172,4049
#define REG_ERR_EPC 173,4069
#define R4K_CONTEXT_FRAME	179,4185
