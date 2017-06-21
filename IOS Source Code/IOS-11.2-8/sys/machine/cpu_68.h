/* $Id: cpu_68.h,v 3.4.44.1 1996/06/12 19:22:23 pst Exp $
 * $Source: /release/112/cvs/Xsys/machine/cpu_68.h,v $
 *------------------------------------------------------------------
 * cpu_68.h - Definitions for generic 68K family processors
 *
 * June 1987, Kirk Lougheed
 * December 1994, Scott Mackie (moved and altered from sys/h)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpu_68.h,v $
 * Revision 3.4.44.1  1996/06/12  19:22:23  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xxbit_word (final part)
 *
 * All good things must come to an end.  ntohl() and friends finally have
 * UNIX semantics in California releases and later.
 *
 * Branch: California_branch
 *
 * Revision 3.4  1995/12/21  16:29:17  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.3  1995/11/17  17:50:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:02:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:40:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:43:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#define CPU_68K 1
#define BIGENDIAN 1

/*
 * The following defines are used by the 64bit math package extracted
 * from libgcc2.
 */
#define BITS_PER_UNIT 8
#define WORDS_BIG_ENDIAN 1
#define __word__ SI


#define u_short unsigned short
#define u_char unsigned char
#define u_int unsigned int
#define u_long unsigned long

#define PROCESSOR_NUM_REGS 10

#ifndef ASMINCLUDE

typedef ushort PROCESSOR_STATUS_TYPE;
#define PROCESSOR_STATUS_PAD 2
typedef long leveltype;
typedef struct {
    void *pc;			/* PC */
    int d2;
    int d3;
    int d4;
    int d5;
    int d6;
    int d7;
    void *a2;
    void *a3;
    void *a4;
    void *a5;
    void *a6;
    void *a7;
} jmp_buf;


extern inline ulong *current_stack_pointer (void)
{
    register ulong *tmp;

    asm("movel sp,%0" : "=r" (tmp) : );		/* read the stack register  */
    return(tmp);				/* return it                */
}


/*
 * Atomic increment/decrement routines.
 * Notes : These are implemented in asm to safeguard from compiler changes
 *         in the future that may render the code non-atomic. And they are
 *         static inlines as opposed to macros to provide type checking.
 */

/*
 * atomic_decrement:
 * Atomically decrement an integer by 1
 */
static inline void
atomic_decrement(int *p)
{
    asm("subql #1,(%0)"
	:			/* outputs */
	: "a" (p));		/* inputs  */
}

/*
 * atomic_increment:
 * Atomically increment an integer by 1
 */
static inline void
atomic_increment(int *p)
{
    asm("addql #1,(%0)"
	:			/* outputs */
	: "a" (p));		/* inputs  */
}

/*
 * disable_interrupts
 *
 * Set interrupt level to disable all interrupts.  Returns current
 * contents of status register.
 *
 * WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
 *
 * This routine can be hazardous to the health of the router.  On the
 * R4000 processor, it disables all interrupts, including the clock
 * interrupt which is used to stoke the hardware watchdog logic.  If you
 * disable interrupts for any period of time, you are likely to crash the
 * router.  This routine should only ever be used to disable and enable
 * interrupts around one or two lines of source code.  It should never be
 * used to disable interrupts around subroutine calls.
 *
 * WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
 */
static inline ulong disable_interrupts (void)
{
    leveltype current;

    asm("movew sr,%0" : "=d" (current) : );
    asm("movew %0,sr" : : "d" (current | 0x0700));
    return(current);
}

/*
 * enable_interrupts
 *
 * Reset status register to value returned by disable_interrupts.
 */
static inline void enable_interrupts (const ulong oldlevel)
{
    asm("movew %0,sr" : : "d" (oldlevel));
}

/*
 * lock_semaphore
 *
 * Attempt to lock a semaphore.  A return value of TRUE indicates
 * success. 
 */
static inline boolean lock_semaphore (semaphore *sem)
{
    boolean result;

    asm ("clrl %0; tas %1@; bne 1f; moveq #1,%0; 1:"
	 : "=d" (result)
	 : "a" (sem));
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

#endif ASMINCLUDE

#define	STACKLONG	1	/* size in ints of long word on stack */
#define	STACKSHORT	1	/* size in ints of short word on stack */


/*
 * Trap locations
 */

#define SPURVector	24
#define	IRQ1Vector	25
#define	IRQ2Vector	26
#define IRQ3Vector	27
#define IRQ4Vector	28
#define IRQ5Vector	29
#define IRQ6Vector	30
#define NMIVector	31
#define TRAP0Vector	32
#define TRAP1Vector	33
#define TRAP2Vector	34
#define TRAP3Vector	35
#define TRAP4Vector	36
#define TRAP5Vector	37
#define TRAP6Vector	38
#define TRAP7Vector	39
#define TRAP8Vector	40
#define TRAP9Vector	41
#define TRAP10Vector	42
#define TRAP11Vector	43
#define TRAP12Vector	44
#define TRAP13Vector	45
#define TRAP14Vector	46
#define TRAP15Vector	47

#define IS_SPURIOUS_INTERRUPT(vect) ((vect) == SPURVector)

#define VECTORCOUNT	256	/* Number of defined vectors */
#define VECTORCOUNT_ROMMON	 64	/* Rom monitor uses partial table */

#define ILLEGAL_INSTRUCTION (0x4afc4afc)

#define	LEVEL1	1
#define	LEVEL2	2
#define	LEVEL3	3
#define	LEVEL4	4
#define	LEVEL5	5
#define LEVEL6	6
#define LEVEL7  7               /* nmi stack is for xxfddi cmt */

/*
 * Byte swapping macros
 */
#define ntohs(a) (a)		/* standard unix definitions */
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

/*
 * adjust pointers for long bcopy's
 */
#define adjustpointer(a) a

/*
 * Macro used to crash the system
 */

#define crashpoint() asm("trap #2")


#ifndef ASMINCLUDE

/*
 * Process level stack parameters.  Used in calls to process_create().
 */
typedef enum stack_size_t_ {
    SMALL_STACK     = 1000,		/* bytes */
    NORMAL_STACK    = 2000,		/* bytes */
    IPROUTING_STACK = 3000,		/* bytes */
    LARGE_STACK     = 4000,		/* bytes */
    HUGE_STACK      = 8000		/* bytes */
} stack_size_t;

/*
 * A hopefully temporary kludge.  Since all our current (1-Jan-92) cfork()
 * calls specify a stack size as a number rather than as a symbolic value,
 * we need some way of having the same code work correctly on both the
 * 68K (the numbers are okay) and a SPARC (the numbers are too small).
 * This code should eventually be replaced with processor specific symbols
 * for different stack sizes (see above).
 */
#define STACK_SIZE_SCALE  1

/*
 * CPU-specific externs
 */
typedef ulong caller_t;
extern caller_t cpu_caller(void);
extern void msecdelay(unsigned int);
extern void usecdelay(unsigned int);

#endif /* ASMINCLUDE */
