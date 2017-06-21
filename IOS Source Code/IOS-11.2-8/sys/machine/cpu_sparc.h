/* $Id: cpu_sparc.h,v 3.7.8.2 1996/06/18 01:48:23 hampton Exp $
 * $Source: /release/112/cvs/Xsys/machine/cpu_sparc.h,v $
 *------------------------------------------------------------------
 * cpu_sparc.h - Definitions for Sparc family processors
 *
 * December 1991, Kirk Lougheed
 * December 1994, Scott Mackie (moved from sys/unix)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 *
 * WARNING! WARNING!   DANGER, WILL ROBINSON!!
 *
 * The definitions in this file are for the router being executed on
 * top of a sparc based unix system.  They are *not*, I repeat, *not*
 * definitions for a router with an embedded sparc cpu.
 *
 * WARNING! WARNING!   DANGER, WILL ROBINSON!!
 *
 *------------------------------------------------------------------
 * $Log: cpu_sparc.h,v $
 * Revision 3.7.8.2  1996/06/18  01:48:23  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.7.8.1  1996/06/12  19:22:25  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xxbit_word (final part)
 *
 * All good things must come to an end.  ntohl() and friends finally have
 * UNIX semantics in California releases and later.
 *
 * Branch: California_branch
 *
 * Revision 3.7  1996/02/28  23:11:42  hampton
 * Fix timer errors in unix images.  [CSCdi45965]
 *
 * Revision 3.6  1996/01/22  09:06:38  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.5  1995/11/24  20:50:16  jjohnson
 * Fix typos in unix ulonglong implementation [CSCdi44257]
 *
 * Revision 3.4  1995/11/22  09:18:05  hampton
 * Fix the mips routine to read/write unaligned longlong words.  While
 * there, rework the entire family of routines to reduce overhead.  This
 * necessitates a change to code calling these routines.  [CSCdi44257]
 *
 * Revision 3.3  1995/11/17  17:50:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:02:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:40:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/13  07:01:43  hampton
 * All code should use the routine GETSHORT()/PUTSHORT() instead of using
 * the routines getshort()/putshort().  [CSCdi40249]
 *
 * Revision 2.2  1995/09/12 16:24:28  hampton
 * Make the unix platform compile again.  [CSCdi39966]
 *
 * Revision 2.1  1995/06/07 21:43:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#define CPU_SPARC 1
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

#define PROCESSOR_NUM_REGS 24

typedef ulong PROCESSOR_STATUS_TYPE;
#define PROCESSOR_STATUS_PAD 0
typedef long leveltype;
typedef long jmp_buf[13];

/*
 * Atomic increment/decrement routines
 */
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

/*
 * current_stack_pointer
 *
 * Return the current stack pointer
 */
extern inline ulong *current_stack_pointer (void)
{
    register ulong *tmp;

    asm("mov %%sp,%0" : "=r" (tmp) : );		/* read the stack register  */
    return(tmp);				/* return it                */
}

/*
 * The following two routines are defined in unix/unix.c.  They are
 * defined in this location for the other CPU types.
 */
extern leveltype disable_interrupts(void);
extern void enable_interrupts(leveltype);

static inline boolean
lock_semaphore (semaphore *sem)
{
    leveltype level;
    boolean result;

    level = disable_interrupts();
    result = ((*sem) == 0);
    if (result)
	(*sem)++;
    enable_interrupts(level);
    return(result);
}

static inline void
unlock_semaphore (semaphore *sem)
{
    (*sem) = 0;
}

/*
 * Define the NULL pointer
 */
#define NULL ((void *) 0)

/*
 * Interrupt levels
 */
#define	LEVEL1	1
#define	LEVEL2	2
#define	LEVEL3	3
#define	LEVEL4	4
#define	LEVEL5	5
#define LEVEL6	6

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

/*
 * These routines are only used by the unix platform, but are found in
 * util.c for some reason.  Probably because inlining them would cause
 * code bloat. 
 */
extern ulong unix_getlong(uchar *ptr);
extern void unix_putlong(uchar *ptr, ulong data);
extern void unix_getlonglong(uchar *addr, uchar *vptr);
extern void unix_putlonglong(uchar *addr, uchar *vptr);

#define GETSHORT(ptr)       ((((uchar *)(ptr))[0]<<8)|(((uchar *)(ptr))[1]))

#define PUTSHORT(ptr, val)  (((uchar *) (ptr))[0] = (ushort) (val) >> 8, \
                ((uchar *) (ptr))[1] = (ushort) (val) & 0xff)

#define GETLONG(addr)	    unix_getlong((uchar *)(addr))
#define PUTLONG(addr, val)  unix_putlong((uchar *)(addr), (ulong)(val))
#define GETLONGLONG(addr, vptr)	unix_getlonglong((uchar *)(addr), (uchar *)(vptr))
#define PUTLONGLONG(addr, vptr)	unix_putlonglong((uchar *)(addr), (uchar *)(vptr))

/*
 * Use GET2WD and PUT2WD when retrieving a long word that is known
 * to be short aligned.  Used heavily by XNS, Novell, Apollo, and Vines.
 */
#define	GET2WD(ptr)  GETLONG(ptr)
#define PUT2WD(ptr, val)  PUTLONG(ptr, val)

/*
 * adjust pointers for long bcopy's
 */
#define adjustpointer(a) a

/*
 * Macro used to crash the system
 */
extern void abort(void);

#define crashpoint	abort

/*
 * Process level stack parameters.  Used in calls to process_create()
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

/*
 * caller support
 */
typedef ulong caller_t;
extern caller_t cpu_caller(void);
extern void msecdelay(unsigned int);
extern void usecdelay(unsigned int);
