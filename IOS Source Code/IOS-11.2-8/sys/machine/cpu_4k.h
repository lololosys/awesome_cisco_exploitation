/* $Id: cpu_4k.h,v 3.7.12.4 1996/08/08 21:06:32 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/machine/cpu_4k.h,v $
 *------------------------------------------------------------------
 * cpu_4k.h - Definitions for MIPS R4X00 family processors
 *
 * October 1993, Michael Beesley
 * December 1994, Scott Mackie (moved and altered from sys/h)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpu_4k.h,v $
 * Revision 3.7.12.4  1996/08/08  21:06:32  mbeesley
 * CSCdi65492:  r4k_cpu_level format can be more efficient. Convert it
 * to a format where bits [5..3] indicate the interrupt level.
 * Branch: California_branch
 *
 * Revision 3.7.12.3  1996/06/12  19:22:19  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xxbit_word (final part)
 *
 * All good things must come to an end.  ntohl() and friends finally have
 * UNIX semantics in California releases and later.
 *
 * Branch: California_branch
 *
 * Revision 3.7.12.2  1996/04/22  07:28:27  hampton
 * Fix semaphores on MIPS platforms.  [CSCdi52328]
 * Branch: California_branch
 *
 * Revision 3.7.12.1  1996/03/18  21:11:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.2  1996/03/16  07:08:55  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.1  1996/03/07  10:06:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7  1996/02/18  19:41:52  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.6  1996/01/22  09:06:36  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.5  1995/12/21  16:29:16  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.4  1995/11/22  09:18:04  hampton
 * Fix the mips routine to read/write unaligned longlong words.  While
 * there, rework the entire family of routines to reduce overhead.  This
 * necessitates a change to code calling these routines.  [CSCdi44257]
 *
 * Revision 3.3  1995/11/17  17:50:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:58  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:40:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:15:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/07/25  17:44:48  mleelani
 * CSCdi37536:  Fix the C4700 Millisecond Ticks.
 * Add support for the 133.33 mhz pipe.
 *
 * Revision 2.3  1995/07/22  07:18:54  hampton
 * Fix the 4K disable/enable_interrupts() routine pair to remember the
 * interrupt state when the first routine is called, and reset that same
 * state when the second routine is called. [CSCdi37521]
 *
 * Revision 2.2  1995/07/14 09:35:19  schaefer
 * CSCdi36176: misalign/spurious access traceback broken for leaf functions
 * define sp, ra register numbers
 *
 * Revision 2.1  1995/06/07  21:43:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CPU_4K_H__
#define __CPU_4K_H__

#define CPU_MIPS  1
#define BIGENDIAN 1

/*
 * The following defines are used by the 64bit math package extracted
 * from libgcc2.
 */
#define BITS_PER_UNIT 8
#define WORDS_BIG_ENDIAN 1
#define __word__ SI


/*
 * Type Definitions  -- note that these are really compiler specific 
 */
#define u_short unsigned short
#define u_char unsigned char
#define u_int unsigned int
#define u_long unsigned long

/*
 * Define the NULL pointer
 */
#ifndef NULL
#define NULL ((void *) 0)
#endif /* NULL */


#define	LEVEL1	1
#define	LEVEL2	2
#define	LEVEL3	3
#define	LEVEL4	4
#define	LEVEL5	5
#define LEVEL6	6
#define LEVEL7  7

/*
 * Define the shift for the format of r4k_cpu_level (bits[5..3])
 */
#define R4K_LEVEL_SHIFT		3

/*
 * Byte swapping macros
 */
#define ntohs(a) (a)
#define htons(a) (a)
#define ntohl(a) (a)
#define htonl(a) (a)

/*
 * The following are the same definitions as above, but using the old
 * cisco nomenclature.  They should be eliminated in favor of using the
 * standard unix definitions above.  The only reason they're still here
 * is I don't want to mess up 11.2 code integration schedules.
 *
 * Someone else please clean up this last bit of mess, since I won't be
 * able to.  -- pst 6 june 1996
 */
#define	n2h(a)	 (a)
#define	h2n(a)	 (a)
#define	h2nl(a)	 (a)
#define	n2hl(a)	 (a)

#define h2big(a)  (a)
#define h2bigl(a) (a)

#define h2lit(a)  swap_16bit_word(a)
#define h2litl(a) vaxorder(a)

#define lit2h(a)  swap_16bit_word(a)
#define lit2hl(a) vaxorder(a)

#define PROCESSOR_NUM_REGS 14

#ifndef ASMINCLUDE

/*
 * R4k CPU pipeline timing variables
 */
extern ulong r4k_1ms_pipe_ticks;	/* no. of pipe ticks to waste 1ms */
extern ulong r4k_4ms_pipe_ticks;	/* no. pf pipe ticks to waste 4ms */
extern ulong r4k_pipe_speed;		/* pipeline speed in Mhz */
extern ulong r4k_count_period_ns;	/* ns per count period */

/*
 * Secondary watchdog present
 */
extern ulong r4k_second_wdog;

/*
 * Level 2 cache present boolean
 */
extern boolean level2_cache_present;

/*
 * Macro used to crash the system
 */
extern void abort(void);

#define crashpoint	abort

typedef ulong PROCESSOR_STATUS_TYPE;
#define PROCESSOR_STATUS_PAD 0
typedef long leveltype;
typedef long jmp_buf[13];

typedef volatile ulong caller_t;

extern inline caller_t cpu_caller (void)
{
    register ulong tmp;

    asm("move %0,$31" : "=r" (tmp) : );    /* read the return register  */
    if ( tmp == 0 || tmp & 0x03 ) {        /* check for odviously bogus */
        return(0);                         /* return address, return 0  */
    } else {
        return(tmp-8);                     /* return PC of caller       */
    }
}


extern inline ulong *current_stack_pointer (void)
{
    register ulong *tmp;

    asm("move %0,$sp" : "=r" (tmp) : );		/* read the stack register  */
    return(tmp);				/* return it                */
}


/*
 * Atomic increment/decrement routines.
 * We take advantage of the load linked and store conditional instructions
 * to provide atomic increment/decrement primitives. These are inlines as
 * opposed to macros to provide type checking
 */

#ifdef TARGET_UNIX

static inline void
atomic_decrement(int *p)
{
    (*p)--;
}

static inline void
atomic_increment(int *p)
{
    (*p)++;
}

#else

/*
 * atomic_decrement:
 * Atomically decrement an integer by 1
 */
static inline void
atomic_decrement(int *p)
{
    int data = 0;

    do {
	asm(".set noreorder; 
             ll %0,0(%1); 
             nop; 
             addiu %0,%2,-1; 
             sc %2,0(%1);
             .set reorder"

	    : "=r" (data) 				/* outputs */
	    : "r" (p), "0" (data)); 			/* inputs  */

    } while (!data);
}

/*
 * atomic_increment:
 * Atomically increment an integer by 1
 */
static inline void
atomic_increment(int *p)
{
    int data = 0;

    do {
	asm(".set noreorder; 
             ll %0,0(%1); 
             nop; 
             addiu %0,%2,1; 
             sc %2,0(%1);
             .set reorder"

	    : "=r" (data) 				/* outputs */
	    : "r" (p), "0" (data)); 			/* inputs  */

    } while (!data);
}

#endif

/*
 * lock_semaphore
 *
 * Attempt to lock a semaphore.  A return value of TRUE indicates
 * success. 
 */

static inline boolean lock_semaphore (semaphore *sem)
{
    ulong sem_value, temp;
    boolean result;

    asm volatile ("
	.set noreorder
     0:	ll	%0,0(%3)	# load-linked the sem (into return reg)
	li	%2,1		# load 0x01 into register
	sc	%2,0(%3)	# store conditionally to sem
	beq	%2,$0,0b	# was the ll <-> sc atomic
	nop			# branch delay
	bne	%0,$0,1f	# did we get the semaphore?
	move	%1,$0		# no, setup FALSE return value (bd slot)
	li	%1,1		# yes, setup TRUE return value
     1:
	.set reorder"
		  : "=&r"(sem_value), "=r"(result),	/* outputs */
		    "=r"(temp) /* not really output, but gcc complains */
		  : "r" (sem));				/* inputs  */
    return(result);
}

/*
 * unlock_semaphore
 *
 * Unlock a semaphore.
 */
static inline void unlock_semaphore (semaphore *sem)
{
    *sem = 0;
}	

/*
 * GET/PUT primitives
 */
static inline
ushort getshort_inline(register void const *ptr)
{
    register ushort data;

    /* 
     * Load a mis-aligned ushort
     */
    asm volatile ("ulhu %0,0(%1)"			
	: "=&r" (data)			/* output variable		*/
	: "r" (ptr));			/* input variable		*/

    return(data);
}

static inline
void putshort_inline(register void *ptr, register ushort value)
{
    /* 
     * Store a mis-aligned short
     */
    asm volatile ("ush %1,0(%0)"			
	: 				/* no outputs			*/
	: "r" (ptr), "r" (value));	/* input variables		*/
}

static inline
ulong getlong_inline(register void const *ptr)
{
    register ulong data;

    /* 
     * Load a mis-aligned ulong
     */
    asm volatile ("lwl %0,0(%1);lwr %0,3(%1)"	
	: "=&r" (data)			/* ouput variable		*/
	: "r"  (ptr));			/* input variable		*/

    return(data);
}

static inline
void putlong_inline(register void *ptr, register ulong value)
{
    /* 
     * Store a mis-aligned long
     */
    asm volatile ("swl %1,0(%0);swr %1,3(%0)"	
	:				/* no outputs			*/
	: "r" (ptr), "r" (value));	/* input variables		*/
}

static inline
void getlonglong_inline (void const *ptr, void *value)
{
    charlonglong *result = value;

    /* 
     * Load a mis-aligned ulonglong
     */
    asm volatile ("lwl %0,0(%1);lwr %0,3(%1)"	
	: "=&r" (result->d.lword[0])		/* ouput variable	*/
	: "r"  (ptr));				/* input variable	*/
    asm volatile ("lwl %0,4(%1);lwr %0,7(%1)"	
	: "=&r" (result->d.lword[1])		/* ouput variable	*/
	: "r"  (ptr));				/* input variable	*/
}

static inline
void putlonglong_inline(void *ptr, void *value)
{
    charlonglong *helper;

    /* 
     * Store a mis-aligned long
     */
    helper = value;
    asm volatile ("swl %1,0(%0);swr %1,3(%0)"	
	:					/* no outputs		*/
	: "r" (ptr), "r" (helper->d.lword[0]));	/* input variables	*/
    asm volatile ("swl %1,4(%0);swr %1,7(%0)"	
	:					/* no outputs		*/
	: "r" (ptr), "r" (helper->d.lword[1]));	/* input variables	*/
}


#define GETSHORT(ptr)		getshort_inline(ptr)
#define PUTSHORT(ptr, val)	putshort_inline((ptr), (val))
#define GETLONG(ptr)		getlong_inline(ptr)
#define PUTLONG(ptr, val)	putlong_inline((ptr), (val))
#define GETLONGLONG(ptr, val)	getlonglong_inline((ptr), (val))
#define PUTLONGLONG(ptr, val)	putlonglong_inline((ptr), (val))

/*
 * Use GET2WD and PUT2WD when retrieving a long word that is known
 * to be short aligned.  Used heavily by XNS, Novell, Apollo, and Vines.
 */
#define	GET2WD(ptr)             GETLONG(ptr)
#define PUT2WD(ptr, val)        PUTLONG(ptr, val)

/*
 * Define interrupt control macros. These are not static inlines
 * due to risk of exhausting the compilers limit on static inlines
 *
 * Note : These routines turn off all interrupts globally. They
 * are for use in cases where it is guranteed that the interrupts
 * will be turned back on very quickly IE less than 1 us (*micro*).
 * So no errmsg(), buginf() etc. while interrupts are off.
 */

#ifdef TARGET_UNIX

/*
 * MIPS based UNIX images
 */
#define GLOBAL_INTERRUPT_BLOCK(status)			\
    status = raise_interrupt_level(ALL_DISABLE);

#define GLOBAL_INTERRUPT_UNBLOCK(status)		\
    reset_interrupt_level(status)

#else

/*
 * Cisco R4600 base products
 */
#define GLOBAL_INTERRUPT_BLOCK(status) 			\
    asm volatile (".set noreorder");			\
    asm volatile (".set noat");				\
    asm volatile ("mfc0 %0, $12"			\
		  : "=&r" (status) 			\
		  : /* no inputs */ );			\
    asm volatile ("addiu $1,$0,0xfffe"			\
		  : /* no outputs */			\
		  : /* no inputs  */			\
		  : "$1" /* destroyed */);		\
    asm volatile ("and $1,$1,%0"			\
		  : /* no outputs */			\
		  : "r" (status));			\
    asm volatile ("mtc0 $1,$12");			\
    asm volatile ("nop");				\
    asm volatile (".set at");				\
    asm volatile (".set reorder");

#define GLOBAL_INTERRUPT_UNBLOCK(status) 		\
    asm volatile (".set noreorder");			\
    asm volatile ("mtc0 %0, $12" : : "r" (status));	\
    asm volatile (".set reorder");

#endif /* TARGET_UNIX */

/*
 * routines for manipulating 64bit objects
 *
 * Note: Since we are using mips2, we cannot pass back a u64 in a single
 *       register as a return value.
 */
typedef unsigned long long u64;

/*
 * use move64 for aligned accesses
 */
extern inline void
move64 (u64 *src, u64 *dst)
{
    asm volatile(".set noreorder\n"
	".set noat\n"
	".set mips3\n"
	"ld $1, 0(%0)\n"
	"nop\n"
	"sd $1, 0(%1)\n"
	".set at\n"
	".set reorder\n"
	".set mips2"	: : "r" (src), "r" (dst) : "$1");
}

/*
 * structure to map opcode encodings for R4K CPU
 */

union instr_r4k {
    unsigned int op_code;

    struct {
	unsigned op : 6;
	unsigned target : 26;
    } j_t;

    struct {
	unsigned op : 6;
	unsigned rs : 5;
	unsigned rt : 5;
	signed s_imd : 16;
    } i_t;

    struct {
	unsigned op : 6;
	unsigned rs : 5;
	unsigned rt : 5;
	unsigned u_imd : 16;
    } u_t;

    struct {
	unsigned op : 6;
	unsigned rs : 5;
	unsigned rt : 5;
	unsigned rd : 5;
	unsigned re : 5;
	unsigned func : 6;
    } r_t;

    struct {
	unsigned op : 6;
	unsigned base : 5;
	unsigned func : 3;
	unsigned cache : 2;
	signed s_imd : 16;
    } c_t;

};

struct r4k_config_t {
    unsigned cm : 1;
    unsigned ec : 3;
    unsigned ep : 4;
    unsigned sb : 2;
    unsigned ss : 1;
    unsigned sw : 1;
    unsigned ew : 2;
    unsigned sc : 1;
    unsigned sm : 1;
    unsigned be : 1;
    unsigned em : 1;
    unsigned eb : 1;
    unsigned dummy : 1;
    unsigned ic : 3;
    unsigned dc : 3;
    unsigned ib : 1;
    unsigned db : 1;
    unsigned cu : 1;
    unsigned k0 : 3;
};

struct r4k_sreg_t {
    unsigned cu : 4;
    unsigned rp : 1;
    unsigned fr : 1;
    unsigned re : 1;
    unsigned d1 : 2;
    unsigned bev : 1;
    unsigned ts : 1;
    unsigned sr : 1;
    unsigned d2 : 1;
    unsigned ch : 1;
    unsigned ce : 1;
    unsigned de : 1;
    unsigned im : 8;
    unsigned kx : 1;
    unsigned sx : 1;
    unsigned ux : 1;
    unsigned ksu : 2;
    unsigned erl : 1;
    unsigned exl : 1;
    unsigned ie : 1;
};

/*
 * Note : These structures should not be changed without
 * altering the assembly language routines for TLB manipulation
 */

struct r4k_pmask_t {
    unsigned dummy0 : 7;
    unsigned mask : 12;
    unsigned dummy1 : 13;
};

struct r4k_entryhi_t {
    unsigned vpn2 : 19;
    unsigned dummy : 5;
    unsigned asid : 8;
};

struct r4k_entrylo_t {
    unsigned dummy : 2;
    unsigned pfn : 24;
    unsigned c : 3;
    unsigned d : 1;
    unsigned v : 1;
    unsigned g : 1;
};

struct r4k_tlb_t {
    struct r4k_pmask_t pmask;
    struct r4k_entryhi_t hi;
    struct r4k_entrylo_t lo0;
    struct r4k_entrylo_t lo1;
};

/*
 * prototypes for assembly language primitives
 */
extern ulong getcp0_sreg(void);
extern ulong getcp0_config(void);
extern ulong getcp0_compare(void);
extern ulong getcp0_count(void);
extern ulong getcp0_cause(void);
extern ulong getcp0_prid(void);
extern void setcp0_sreg(ulong);
extern void setcp0_config(ulong);
extern void setcp0_compare(ulong);
extern void setcp0_count(ulong);

extern void r4k_settlb(struct r4k_tlb_t *, int, boolean);
extern boolean r4k_gettlb(struct r4k_tlb_t *, int);
extern void r4k_setasid(int);
extern int r4k_tlbprobe(struct r4k_entryhi_t *);

#endif /* ASMINCLUDE */

/*
 * adjust pointers for long bcopy's
 */
#define adjustpointer(a) a

/*
 * R4K CPU specific literals
 */
#define R4K_K0BASE           (0x80000000)
#define R4K_K0SIZE           (0x20000000)
#define R4K_K1BASE           (0xA0000000)
#define R4K_K1SIZE           (0x20000000)
#define R4K_KSBASE           (0xC0000000)
#define R4K_KSSIZE           (0x20000000)
#define R4K_K3BASE           (0xE0000000)
#define R4K_K3SIZE           (0x20000000)
#define R4K_KUBASE           (0x00000000)
#define R4K_KUSIZE           (0x80000000)

#define R4K_PHY_TO_KSEG1(x)  ((ulong)(x) | 0xA0000000)
#define R4K_PHY_TO_KSEG0(x)  ((ulong)(x) | 0x80000000)

#define R4K_COLDRESET_VECTOR	0xbfc00000

/*
 * Timing literals. Note, these are specific to the speed grade
 * of the processor. If the platform only supports one speed grade
 * of processor, the literals expand out to the relevant constants.
 * It the platform supports mulitple speed grade processors, the
 * literals expand to variables setup at init time.
 */
#define R4K_1MS_AT_100MHZ		0xC350	/* ticks per 1ms, 100mhz */
#define R4K_4MS_AT_100MHZ		0x30D40	/* ticks per 4ms, 100mhz */
#define R4K_NS_PERIOD_AT_100MHZ		20	/* ns per tick, 100mhz   */

#define R4K_1MS_AT_133MHZ               0x1046B	/* ticks per 1ms, 133.33mhz */
#define R4K_4MS_AT_133MHZ               0x411AB	/* ticks per 4ms, 133.33mhz */
#define R4K_NS_PERIOD_AT_133MHZ    	15	/* ns per tick, 133.33mhz   */

#define R4K_1MS_AT_150MHZ		0x124F8 /* ticks per 1ms, 150mhz */
#define R4K_4MS_AT_150MHZ		0x493E0 /* ticks per 4ms, 150mhz */
#define R4K_NS_PERIOD_AT_150MHZ		13	/* ns per tick, 150mhz   */

#define R4K_1MS_AT_200MHZ		0x186A0 /* ticks per 1ms, 200mhz */
#define R4K_4MS_AT_200MHZ		0x61A80 /* ticks per 4ms, 200mhz */
#define R4K_NS_PERIOD_AT_200MHZ		10	/* ns per tick, 200mhz   */

#if !defined(MULTIPLE_R4K_SPEEDS)

/*
 * Multiple speed grades are not supported. Pipe speed is 100Mhz
 */
#define R4K_COUNT_PERIOD_NS		R4K_NS_PERIOD_AT_100MHZ
#define R4K_CLOCK_1MS_INCR  		R4K_1MS_AT_100MHZ
#define R4K_CLOCK_4MS_INCR      	R4K_4MS_AT_100MHZ

#else /* MULTIPLE_R4K_SPEEDS */

/*
 * Multiple speed processors. Literals expand out to variables
 */
#define R4K_COUNT_PERIOD_NS		r4k_count_period_ns
#define R4K_CLOCK_1MS_INCR		r4k_1ms_pipe_ticks
#define R4K_CLOCK_4MS_INCR		r4k_4ms_pipe_ticks

#endif /* MULTIPLE_R4K_SPEEDS */

#ifndef ASMINCLUDE
extern inline ulong
getcp0_count_inline (void)
{
    ulong reg;

    asm volatile("mfc0 %0, $9" : "=r" (reg));
    return reg;
}

extern void msecdelay(unsigned int);
extern void usecdelay(unsigned int);

#define R4K_USEC2COUNT(usec)	((usec) * 1000 / R4K_COUNT_PERIOD_NS)
#define R4K_COUNT2USEC(count)	((count) * R4K_COUNT_PERIOD_NS / 1000)

#define USEC_TIMER_START(timestamp, usec) 				\
    do {								\
	(timestamp) = getcp0_count_inline() + R4K_USEC2COUNT(usec);	\
    } while (0)

#define USEC_GET_TIMESTAMP(timestamp)					\
	USEC_TIMER_START(timestamp, 0)

#define USEC_AWAKE(timer)						\
	((long) ((timer) - getcp0_count_inline()) <= 0)

#define USEC_CLOCK_OUTSIDE_INTERVAL(timer, usec)			\
	((getcp0_count_inline() - (timer)) >= R4K_USEC2COUNT(usec))

#define USEC_ELAPSED_TIME(timer)					\
	R4K_COUNT2USEC(getcp0_count_inline() - (timer))

#define USEC_TIMER_ADD_DELTA(timer, usec)				\
	do {								\
	    (timer) += R4K_USEC2COUNT(usec);				\
	} while (0)

typedef ulong usec_timestamp;

#endif /* ASMINCLUDE */

/*
 * Defines for the decoding opcodes
 */
#define R4K_SPEC_MASK          0x3F
#define R4K_REGIM_MASK         0x1F
#define R4K_COP0_C0MASK        0x10
#define R4K_HI_ADDR_MASK       0xF0000000

/*
 * Defines for interpreting address error and branch instructions
 */
#define OPCODE_LB	       0x20
#define OPCODE_LBU	       0x24
#define OPCODE_LHU             0x25
#define OPCODE_LH              0x21
#define OPCODE_LWU             0x27
#define OPCODE_LW              0x23
#define OPCODE_LWL	       0x22
#define OPCODE_LWR	       0x26
#define OPCODE_LD	       0x37
#define OPCODE_LDL	       0x1A
#define OPCODE_LDR	       0x1B

#define OPCODE_SH              0x29
#define OPCODE_SW              0x2B

#define OPCODE_SPECIAL         0x00
#define OPCODE_JALR            0x09
#define OPCODE_JR              0x08

#define OPCODE_REGIMM          0x01
#define OPCODE_BGEZ            0x01
#define OPCODE_BGEZAL          0x11
#define OPCODE_BLTZ            0x00
#define OPCODE_BLTZAL          0x10
#define OPCODE_BGEZL           0x03
#define OPCODE_BGEZALL         0x13
#define OPCODE_BLTZL           0x02
#define OPCODE_BLTZALL         0x12

#define OPCODE_J               0x02
#define OPCODE_JAL             0x03
#define OPCODE_BEQ             0x04
#define OPCODE_BGTZ            0x07
#define OPCODE_BLEZ            0x06
#define OPCODE_BNE             0x05
#define OPCODE_BEQL            0x14
#define OPCODE_BGTZL           0x17
#define OPCODE_BLEZL           0x16
#define OPCODE_BNEL            0x15

/*
 * Opcodes to allow virtual stack trace decoding
 */
#define OPCODE_JR_RA           0x03E00008
#define OPCODE_ADDIU           0x09

/*
 * CPU/CP0 Resources
 */
#ifndef	MIPSDEMO

#define R4K_ICACHE_LINES         0x200       /* 512 lines         */
#define R4K_ILINE_SHIFT          0x20        /* 32bytes per line  */
#define R4K_ICACHE_SIZE          0x4000      /* 16K Bytes         */

#define R4K_DCACHE_LINES         0x200       /* 512 lines         */
#define R4K_DLINE_SHIFT          0x20        /* 32bytes per line  */
#define R4K_DCACHE_SIZE          0x4000      /* 16K Bytes         */

#else	/* MIPSDEMO */

#define R4K_ICACHE_LINES         0x400       /* 1024 lines        */
#define R4K_ILINE_SHIFT          0x10        /* 16bytes per line  */
#define R4K_ICACHE_SIZE          0x4000      /* 16K Bytes         */

#define R4K_DCACHE_LINES         0x400       /* 1024 lines        */
#define R4K_DLINE_SHIFT          0x10        /* 16bytes per line  */
#define R4K_DCACHE_SIZE          0x4000      /* 16K Bytes         */

#endif	/* MIPSDEMO */

#define MAX_TLB_ENTRIES          48

/*
 * CP0 TLB Lo0 and Lo1 Register
 */
#define TLB_LO_PFNMASK          0x3FFFFFC0
#define TLB_LO_CMASK            0x00000038
#define TLB_LO_DMASK            0x00000004
#define TLB_LO_VMASK            0x00000002
#define TLB_LO_GMASK            0x00000001

#define TLB_LO_PFNSHIFT         6
#define TLB_LO_CSHIFT           3
#define TLB_LO_DSHIFT           2
#define TLB_LO_VSHIFT           1
#define TLB_LO_GSHIFT           0

#define TLB_CACHE_WT            0     /* write-through (4600 only) */
#define TLB_CACHE_WTWA          1     /* wt & write-allocate (4600 only) */
#define TLB_UNCACHED            2
#define TLB_CACHE_NONCOHERENT   3
#define TLB_CACHE_COH_EXCL      4     /* unimplemented, rsvd on R4600 */
#define TLB_CACHE_EXCL_WRITE    5     /* unimplemented, rsvd on R4600 */
#define TLB_CACHE_UPDATE_WRITE  6     /* unimplemented, rsvd on R4600 */

/*
 * CP0 TLB Hi Register
 */
#define TLB_HI_VPN2MASK         0xFFFFE000
#define TLB_HI_ASIDMASK         0x000000FF
#define TLB_MAX_ASID            0xFF

#define TLB_HI_VPN2SHIFT        13
#define TLB_HI_ASIDSHIFT        0

/*
 * CP0 TLB Page Register
 */
#define TLB_PAGE_MASK           0x01FFE000
#define TLB_PAGE_SHIFT          13

#define TLB_PAGESIZE_4K         0x000
#define TLB_PAGESIZE_16K        0x003
#define TLB_PAGESIZE_64K        0x00F
#define TLB_PAGESIZE_256K       0x03F
#define TLB_PAGESIZE_1M         0x0FF
#define TLB_PAGESIZE_4M         0x3FF
#define TLB_PAGESIZE_16M        0xFFF

#define TLB_4K_SHIFT            0
#define TLB_16K_SHIFT           2
#define TLB_64K_SHIFT           4
#define TLB_256K_SHIFT          6
#define TLB_1M_SHIFT            8
#define TLB_4M_SHIFT            10
#define TLB_16M_SHIFT           12

#define TLB_RW                  1
#define TLB_RO                  0
#define TLB_VALID               1
#define TLB_INVALID             0
#define TLB_GLOBAL              1
#define TLB_ASID                0


/*
 * CP0 TLB Index Register
 */
#define TLB_INX_INXDEXMASK          0x0000003F
#define TLB_INX_PROBEMASK           0x80000000

#define TLB_INX_INDEXSHIFT          0
#define TLB_INX_PROBESHIFT          31

/*
 * CP0 TLB Random Register
 */
#define TLB_RANDOM_MASK             0x0000003F
#define TLB_RANDOM_SHIFT            0

/*
 * CP0 TLB Wired Register
 */
#define TLB_WIRED_MASK              0x0000003F
#define TLB_WIRED_SHIFT             0

/*
 * CP0 TLB Context Register
 */
#define TLB_CTXT_BASEPTE_MASK       0xFF800000
#define TLB_CTXT_BADVPN2_MASK       0x007FFFF0

#define TLB_CTXT_BASEPTE_SHIFT      23
#define TLB_CTXT_BADVPN2_SHIFT      4

/*
 * CP0 Status Register
 */
#define SREG_CU1                    0x20000000
#define SREG_CU0                    0x10000000
#define SREG_BEV                    0x00400000
#define SREG_TS                     0x00200000
#define SREG_SR                     0x00100000
#define SREG_CH                     0x00040000
#define SREG_CE                     0x00020000
#define SREG_DE                     0x00010000
#define SREG_RP                     0x08000000
#define SREG_FR                     0x04000000
#define SREG_RE                     0x02000000
#define SREG_KX                     0x00000080
#define SREG_SX                     0x00000040
#define SREG_UX                     0x00000020
#define SREG_KSU                    0x00000018
#define SREG_ERL                    0x00000004
#define SREG_EXL                    0x00000002
#define SREG_IE                     0x00000001
#define SREG_IMASK8                 0x00000000
#define SREG_IMASK7                 0x00008000
#define SREG_IMASK6                 0x00004000
#define SREG_IMASK5                 0x00002000
#define SREG_IMASK4                 0x00001000
#define SREG_IMASK3                 0x00000800
#define SREG_IMASK2                 0x00000400
#define SREG_IMASK1                 0x00000200
#define SREG_IMASK0                 0x00000100

#define SREG_DS_MASK                0x00770000
#define SREG_CU_MASK                0xF0000000
#define SREG_IMASK                  0x0000FF00

#define SREG_CU_SHIFT               28
#define SREG_DS_SHIFT               16
#define SREG_IMASK_SHIFT            8

/*
 * CP0 Cause register
 */
#define CAUSE_BD_SLOT               0x80000000

#define CAUSE_MASK                  0x0000007C
#define CAUSE_CEMASK                0x30000000
#define CAUSE_IMASK                 0x0000FF00

#define CAUSE_SHIFT                 2
#define CAUSE_CESHIFT               28
#define CAUSE_ISHIFT                8

#define CAUSE_INTERRUPT             0
#define CAUSE_TLB_MOD               1
#define CAUSE_TLB_LOAD              2
#define CAUSE_TLB_SAVE              3
#define CAUSE_ADDR_LOAD             4
#define CAUSE_ADDR_SAVE             5
#define CAUSE_BUS_INSTR             6
#define CAUSE_BUS_DATA              7
#define CAUSE_SYSCALL               8
#define CAUSE_BP                    9
#define CAUSE_ILLOP                 10
#define CAUSE_CP_UNUSABLE           11
#define CAUSE_OVFLW                 12
#define CAUSE_TRAP                  13
#define CAUSE_VIRT_COHERENCY_INSTR  14
#define CAUSE_FPE                   15
#define CAUSE_WATCH                 23
#define CAUSE_VIRT_COHERENCY_DATA   31

#define CAUSE_IBIT7                 0x00008000
#define CAUSE_IBIT6                 0x00004000
#define CAUSE_IBIT5                 0x00002000
#define CAUSE_IBIT4                 0x00001000
#define CAUSE_IBIT3                 0x00000800
#define CAUSE_IBIT2                 0x00000400
#define CAUSE_IBIT1                 0x00000200
#define CAUSE_IBIT0                 0x00000100

#define CAUSE_IP01_MASK      (CAUSE_IBIT0|CAUSE_IBIT1)
#define CAUSE_IP3456	     (CAUSE_IBIT3|CAUSE_IBIT4|CAUSE_IBIT5|CAUSE_IBIT6)
#define CAUSE_IP23456	     (CAUSE_IP3456|CAUSE_IBIT2)

/*
 * CP0 Config Register
 */
#define CONFIG_MASTER_CHECKER       0x80000000
#define CONFIG_SYS_CLOCK            0x70000000
#define CONFIG_PATTERN              0x0F000000
#define CONFIG_CACHE2_BLOCKSIZE     0x00C00000
#define CONFIG_CACHE2_SPLIT         0x00200000
#define CONFIG_CACHE2_WIDTH         0x00100000
#define CONFIG_SYSPORT_WIDTH        0x000C0000
#define CONFIG_CACHE2_PRESENT       0x00020000
#define CONFIG_DIRTY_SHARED         0x00010000
#define CONFIG_BIG_INDIAN           0x00008000
#define CONFIG_ECC_MODE             0x00004000
#define CONFIG_BLOCK_ORDER          0x00002000
#define CONFIG_ICACHE_SIZE          0x00000E00
#define CONFIG_DCACHE_SIZE          0x000001C0
#define CONFIG_ICACHE_BLOCK         0x00000020
#define CONFIG_DCACHE_LINE          0x00000010
#define CONFIG_CACHE2_CU            0x00000008
#define CONFIG_KSEG0_COHERENCY      0x00000007

#define CONFIG_MASTER_CHECKER_SHIFT         31
#define CONFIG_SYS_CLOCK_SHIFT              28
#define CONFIG_PATTERN_SHIFT                24
#define CONFIG_CACHE2_BLOCKSIZE_SHIFT       22
#define CONFIG_CACHE2_SPLIT_SHIFT           21
#define CONFIG_CACHE2_WIDTH_SHIFT           20
#define CONFIG_SYSPORT_WIDTH_SHIFT          18
#define CONFIG_CACHE2_PRESENT_SHIFT         17
#define CONFIG_DIRTY_SHARED_SHIFT           16
#define CONFIG_BIG_INDIAN_SHIFT             15
#define CONFIG_ECC_MODE_SHIFT               14
#define CONFIG_BLOCK_ORDER_SHIFT            13
#define CONFIG_ICACHE_SIZE_SHIFT            9
#define CONFIG_DCACHE_SIZE_SHIFT            6
#define CONFIG_ICACHE_BLOCK_SHIFT           5
#define CONFIG_DCACHE_LINE_SHIFT            4
#define CONFIG_CACHE2_CU_SHIFT              3
#define CONFIG_KSEG0_COHERENCY_SHIFT        0

/*
 * CP0 PRID Register
 */
#define PRID_IMP                    0x0000FF00
#define PRID_REV                    0x000000FF

#define PRID_IMP_SHIFT              8
#define PRID_REV_SHIFT              0

/*
 * CP0 Watch Hi/Lo Registers
 */
#define WATCH_PADDR0                0xFFFFFFF8
#define WATCH_PADDR1                0x0000000F
#define WATCH_READ                  0x00000002
#define WATCH_WRITE                 0x00000001

#define WATCH_PADDR0_SHIFT          3
#define WATCH_PADDR1_SHIFT          0
#define WATCH_READ_SHIFT            1
#define WATCH_WRITE_SHIFT           0

/*
 * CP0 ECC Register
 */
#define ECC_MASK                    0x000000FF

/*
 * CP0 Cache Error Register
 */
#define CACHE_ERR_ER                0x80000000
#define CACHE_ERR_EC                0x40000000
#define CACHE_ERR_ED                0x20000000
#define CACHE_ERR_ET                0x10000000
#define CACHE_ERR_ES                0x08000000
#define CACHE_ERR_EE                0x04000000
#define CACHE_ERR_EB                0x02000000
#define CACHE_ERR_EI                0x01000000
#define CACHE_ERR_SLDX              0x003FFFF8
#define CACHE_ERR_PLDX              0x00000007

#define CACHE_ERR_ER_SHIFT          31
#define CACHE_ERR_EC_SHIFT          30
#define CACHE_ERR_ED_SHIFT          29
#define CACHE_ERR_ET_SHIFT          28
#define CACHE_ERR_ES_SHIFT          27
#define CACHE_ERR_EE_SHIFT          26
#define CACHE_ERR_EB_SHIFT          25
#define CACHE_ERR_EI_SHIFT          24
#define CACHE_ERR_SLDX_SHIFT         3
#define CACHE_ERR_PLDX_SHIFT         0

/*
 * CP0 TagLo and TagHi
 */
#define TAGLO_PTAGLO                0xFFFFFF00
#define TAGLO_PSTATE                0x000000C0
#define TAGLO_PARITY                0x00000001

#define TAGLO_PTAGLO_SHIFT          8
#define TAGLO_PSTATE_SHIFT          6
#define TAGLO_PARITY_SHIFT          0
 
/*
 * CP0 Config register masks
 */
#define CONFIG_EC_MASK		    0x70000000
#define CONFIG_EC_DIV2		    0x00000000
#define CONFIG_EC_DIV3		    0x10000000
#define CONFIG_EC_DIV4		    0x20000000
#define CONFIG_EC_DIV5		    0x30000000
#define CONFIG_EC_DIV6		    0x40000000
#define CONFIG_EC_DIV7		    0x50000000
#define CONFIG_EC_DIV8		    0x60000000
 
/****************************************************************/
/* Register names for assembler source files                    */
/****************************************************************/

/*
 * Some R4k register literals
 */
#define R4K_RA_REG	31	/* ra register number */
#define R4K_SP_REG	29	/* sp register number */
#define R4K_V0_REG	2	/* v0 register number */

#ifdef ASMINCLUDE

#define zero    $0           /* allways zero               */
#define AT      $1           /* reserved for assembler     */

#define v0      $2           /* return value               */
#define v1      $3

#define a0      $4           /* arguments 0..3             */
#define a1      $5
#define a2      $6
#define a3      $7

#define t0      $8           /* temporary registers t0-t9  */
#define t1      $9           /* saved by caller            */
#define t2      $10
#define t3      $11
#define t4      $12 
#define t5      $13
#define t6      $14
#define t7      $15
#define t8      $24
#define t9      $25

#define s0      $16          /* saved by callee            */
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define s8      $30

#define k0      $26          /* reserved for kernel        */
#define k1      $27

#define gp      $28          /* global pointer             */
#define sp      $29          /* stack pinter               */
#define ra      $31          /* return address             */

/*
 * Alternate register names
 */
#define r0	$0
#define r1	$1
#define r2	$2
#define r3	$3
#define r4	$4
#define r5	$5
#define r6	$6
#define r7	$7
#define r8	$8
#define r9	$9
#define r10	$10
#define r11	$11
#define r12	$12
#define r13	$13

#define r14	$14
#define r15	$15
#define r16	$16
#define r17	$17
#define r18	$18
#define r19	$19
#define r20	$20
#define r21	$21
#define r22	$22
#define r23	$23
#define r24	$24
#define r25	$25
#define r26	$26
#define r27	$27
#define r28	$28
#define r29	$29
#define r30	$30
#define r31	$31

/*
 * Floating point register names
 */
#define FPU_R0	$f0
#define FPU_R1	$f1
#define FPU_R2	$f2
#define FPU_R3	$f3
#define FPU_R4	$f4
#define FPU_R5	$f5
#define FPU_R6	$f6
#define FPU_R7	$f7
#define FPU_R8	$f8
#define FPU_R9	$f9
#define FPU_R10	$f10
#define FPU_R11	$f11
#define FPU_R12	$f12
#define FPU_R13	$f13
#define FPU_R14	$f14
#define FPU_R15	$f15
#define FPU_R16	$f16
#define FPU_R17	$f17
#define FPU_R18	$f18
#define FPU_R19	$f19
#define FPU_R20	$f20
#define FPU_R21	$f21
#define FPU_R22	$f22
#define FPU_R23	$f23
#define FPU_R24	$f24
#define FPU_R25	$f25
#define FPU_R26	$f26
#define FPU_R27	$f27
#define FPU_R28	$f28
#define FPU_R29	$f29
#define FPU_R30	$f30
#define FPU_R31	$f31

/*
 * CP0 Register definitions
 */
#define CP0_INDEX      $0             /* TLB Index           */
#define CP0_RANDOM     $1             /* TLB Random          */
#define CP0_TLB_LO_0   $2             /* TLB Entry Lo0       */
#define CP0_TLB_LO_1   $3             /* TLB Entry Lo1       */
#define CP0_CONTEXT    $4             /* Kernel PTE pointer  */
#define CP0_PAGEMASK   $5             /* TLB Page Mask       */
#define CP0_WIRED      $6             /* TLB Wired           */
#define CP0_BADVADDR   $8             /* Bad Virtual Address */
#define CP0_COUNT      $9             /* Count               */
#define CP0_TLB_HI     $10            /* TLB Entry Hi        */
#define CP0_COMPARE    $11            /* Timer Compare       */
#define CP0_SREG       $12            /* Status              */
#define CP0_CAUSE      $13            /* Cause               */
#define CP0_EPC        $14            /* Exception PC        */
#define CP0_PRID       $15            /* Proc Rev ID         */
#define CP0_CONFIG     $16            /* Configuration       */
#define CP0_LLADDR     $17            /* Load/Link address   */
#define CP0_WATCHLO    $18            /* Low Watch address   */
#define CP0_WATCHHI    $19            /* High Watch address  */
#define CP0_XCONTEXT   $20            /* Extended context    */
#define CP0_ECC        $26            /* ECC and parity      */
#define CP0_CACHERR    $27            /* Cache Err/Status    */
#define CP0_TAGLO      $28            /* Cache Tag Lo        */
#define CP0_TAGHI      $29            /* Cache Tag Hi        */
#define CP0_ERR_EPC    $30            /* Error exception PC  */

/*
 * Define the assembler support macros for procedure framing
 * FRAME    : sets proc as global, generates debug info with .ent
 * ENDFRAME : generates debug into with .end
 */

#define FRAME(name, frame_register, offset, return_register)  \
    .globl name;                                              \
    .ent   name;                                              \
name:                                                         \
    .frame frame_register, offset, return_register

#define ENDFRAME(name)                                        \
    .end name

#else /* ASMINCLUDE */

/*
 * disable_interrupts
 *
 * Set status register to ignore all interrupts.  Returns a value (zero) to be
 * consistent with 68K routine.
 *
 * WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
 *
 * This routine can be hazardous to the health of the router.  It
 * disables all interrupts, including the clock interrupt which is used
 * to stoke the hardware watchdog logic.  If you disable interrupts for
 * any period of time, you are likely to crash the router.  This routine
 * should only ever be used to disable and enable interrupts around one
 * or two lines of source code.  It should never be used to disable
 * interrupts around subroutine calls.
 *
 * WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
 */
static inline leveltype disable_interrupts (void)
{
    ulong status_register; 

    /*
     * Read sreg, deassert SREG_IE and write it back. We stop
     * the asm from reordering and insert a nop after the read as 
     * there is a CP0 hazard of one tick after reading a CP0 register
     * (which is not interlocked).
     *
     * Note: We put a nop after the sreg writeback as it does
     * not take effect for 1 tick. We are disabling interrupts,
     * so the very next instruction could have been in the 
     * critical section and would be unprotected.
     */
    asm volatile (".set noreorder");		/* do not let asm reorder */
    asm volatile ("mfc0 %0,$12" : "=r" (status_register) : );
    asm volatile ("nop");			/* takes one tick to read */

    asm volatile ("mtc0 %0,$12" : : "r" (status_register & ~SREG_IE));
    asm volatile ("nop");			/* sreg write takes tick */
    asm volatile (".set reorder");
    return(status_register & SREG_IE);
}
 
/*
 * enable_interrupts
 *
 * Set status register to allow interrupts.
 */
static inline void enable_interrupts (const leveltype oldlevel)
{
    ulong status_register;

    /*
     * Is this the level of code that disabled interrupts?  If not,
     * then do not re-enable them.
     */
    if (oldlevel == 0)
	return;

    /*
     * Read sreg, assert SREG_IE and write it back. We stop
     * the asm from reordering and insert a nop after the read as 
     * there is a CP0 hazard of one tick after reading a CP0 register
     * (which is not interlocked).
     *
     * Note: There is no need to put a nop after the write as
     * we are re-enabling interrupts
     */
    asm volatile (".set noreorder");		/* stop asm reordering */
    asm volatile ("mfc0 %0,$12" : "=r" (status_register) : );
    asm volatile ("nop");			/* sreg read takes a tick */

    status_register |= SREG_IE;

    asm volatile ("mtc0 %0,$12" : : "r" (status_register));
    asm volatile ("nop");			/* sreg write takes tick */
    asm volatile (".set reorder");
}

/*
 * Process level stack parameters.  Used in calls to process_create().
 */
typedef enum stack_size_t_ {
    SMALL_STACK     =  3000,		/* bytes */
    NORMAL_STACK    =  6000,		/* bytes */
    IPROUTING_STACK =  9000,		/* bytes */
    LARGE_STACK     = 12000,		/* bytes */
    HUGE_STACK      = 24000		/* bytes */
} stack_size_t;

/*
 * A hopefully temporary kludge.  Since all our current (1-Jan-92) cfork()
 * calls specify a stack size as a number rather than as a symbolic value,
 * we need some way of having the same code work correctly on both the
 * 68K (the numbers are okay) and a SPARC (the numbers are too small).
 * This code should eventually be replaced with processor specific symbols
 * for different stack sizes (see above).
 */
#define STACK_SIZE_SCALE  3

#endif /* ASMINCLUDE */

#endif /* __CPU_4K_H__ */

/* End of Module */
