
cpu_sparc.h,1738
#define CPU_SPARC 75,2715
#define BIGENDIAN 76,2735
#define BITS_PER_UNIT 82,2850
#define WORDS_BIG_ENDIAN 83,2874
#define __word__ 84,2901
#define u_short 90,3001
#define u_char 91,3032
#define u_int 92,3061
#define u_long 93,3088
#define PROCESSOR_NUM_REGS 95,3118
typedef ulong PROCESSOR_STATUS_TYPE;PROCESSOR_STATUS_TYPE97,3149
#define PROCESSOR_STATUS_PAD 98,3186
typedef long leveltype;leveltype99,3217
typedef long jmp_buf[jmp_buf100,3241
atomic_decrement(106,3333
atomic_increment(112,3394
extern inline ulong *current_stack_pointer 122,3507
lock_semaphore 138,3947
unlock_semaphore 152,4184
#define NULL 160,4273
#define	LEVEL1	165,4328
#define	LEVEL2	166,4345
#define	LEVEL3	167,4362
#define	LEVEL4	168,4379
#define	LEVEL5	169,4396
#define LEVEL6	170,4413
#define ntohs(ntohs175,4462
#define htons(htons176,4483
#define ntohl(ntohl177,4504
#define htonl(htonl178,4525
#define	n2h(n2h189,4938
#define	h2n(h2n190,4958
#define	h2nl(h2nl191,4978
#define	n2hl(n2hl192,4999
#define h2big(h2big194,5021
#define h2bigl(h2bigl195,5043
#define h2lit(h2lit197,5066
#define h2litl(h2litl198,5103
#define lit2h(lit2h200,5134
#define lit2hl(lit2hl201,5171
#define GETSHORT(GETSHORT213,5569
#define PUTSHORT(PUTSHORT215,5647
#define GETLONG(GETLONG218,5785
#define PUTLONG(PUTLONG219,5841
#define GETLONGLONG(GETLONGLONG220,5913
#define PUTLONGLONG(PUTLONGLONG221,5996
#define	GET2WD(GET2WD227,6227
#define PUT2WD(PUT2WD228,6261
#define adjustpointer(adjustpointer233,6349
#define crashpoint	240,6444
typedef enum stack_size_t_ stack_size_t_245,6547
} stack_size_t;stack_size_t251,6785
#define STACK_SIZE_SCALE 261,7218
typedef ulong caller_t;caller_t266,7272
