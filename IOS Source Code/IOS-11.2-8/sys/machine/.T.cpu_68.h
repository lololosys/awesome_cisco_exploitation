
cpu_68.h,2527
#define CPU_68K 44,1532
#define BIGENDIAN 45,1550
#define BITS_PER_UNIT 51,1665
#define WORDS_BIG_ENDIAN 52,1689
#define __word__ 53,1716
#define u_short 56,1739
#define u_char 57,1770
#define u_int 58,1799
#define u_long 59,1826
#define PROCESSOR_NUM_REGS 61,1856
typedef ushort PROCESSOR_STATUS_TYPE;PROCESSOR_STATUS_TYPE65,1907
#define PROCESSOR_STATUS_PAD 66,1945
typedef long leveltype;leveltype67,1976
} jmp_buf;jmp_buf82,2198
extern inline ulong *current_stack_pointer 85,2211
atomic_decrement(106,2774
atomic_increment(118,2962
static inline ulong disable_interrupts 143,3821
static inline void enable_interrupts 157,4109
static inline boolean lock_semaphore 168,4324
static inline void unlock_semaphore 183,4565
#define	STACKLONG	190,4657
#define	STACKSHORT	191,4718
#define SPURVector	198,4810
#define	IRQ1Vector	199,4832
#define	IRQ2Vector	200,4854
#define IRQ3Vector	201,4876
#define IRQ4Vector	202,4898
#define IRQ5Vector	203,4920
#define IRQ6Vector	204,4942
#define NMIVector	205,4964
#define TRAP0Vector	206,4985
#define TRAP1Vector	207,5008
#define TRAP2Vector	208,5031
#define TRAP3Vector	209,5054
#define TRAP4Vector	210,5077
#define TRAP5Vector	211,5100
#define TRAP6Vector	212,5123
#define TRAP7Vector	213,5146
#define TRAP8Vector	214,5169
#define TRAP9Vector	215,5192
#define TRAP10Vector	216,5215
#define TRAP11Vector	217,5239
#define TRAP12Vector	218,5263
#define TRAP13Vector	219,5287
#define TRAP14Vector	220,5311
#define TRAP15Vector	221,5335
#define IS_SPURIOUS_INTERRUPT(IS_SPURIOUS_INTERRUPT223,5360
#define VECTORCOUNT	225,5420
#define VECTORCOUNT_ROMMON	226,5476
#define ILLEGAL_INSTRUCTION 228,5545
#define	LEVEL1	230,5587
#define	LEVEL2	231,5604
#define	LEVEL3	232,5621
#define	LEVEL4	233,5638
#define	LEVEL5	234,5655
#define LEVEL6	235,5672
#define LEVEL7 236,5689
#define ntohs(ntohs241,5787
#define htons(htons242,5841
#define ntohl(ntohl243,5862
#define htonl(htonl244,5883
#define	n2h(n2h255,6296
#define	h2n(h2n256,6316
#define	h2nl(h2nl257,6336
#define	n2hl(n2hl258,6357
#define h2big(h2big260,6379
#define h2bigl(h2bigl261,6401
#define h2lit(h2lit263,6424
#define h2litl(h2litl264,6461
#define lit2h(lit2h266,6492
#define lit2hl(lit2hl267,6529
#define adjustpointer(adjustpointer272,6603
#define crashpoint(crashpoint278,6673
typedef enum stack_size_t_ stack_size_t_286,6809
} stack_size_t;stack_size_t292,7042
#define STACK_SIZE_SCALE 302,7475
typedef ulong caller_t;caller_t307,7535
