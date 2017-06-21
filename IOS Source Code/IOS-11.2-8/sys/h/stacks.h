/* $Id: stacks.h,v 3.2.60.2 1996/04/19 15:15:34 che Exp $
 * $Source: /release/112/cvs/Xsys/h/stacks.h,v $
 *------------------------------------------------------------------
 * stack.h -- Interrupt level stack definitions
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Stack specific structures extracted from sched.h.
 *------------------------------------------------------------------
 * $Log: stacks.h,v $
 * Revision 3.2.60.2  1996/04/19  15:15:34  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.2.68.2  1996/04/16  03:53:47  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.2.68.1  1996/04/09  20:20:10  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.2.60.1  1996/03/18  19:37:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/09  05:05:08  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  09:37:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:17:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:13:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/14  09:34:55  schaefer
 * CSCdi36176: misalign/spurious access traceback broken for leaf functions
 * add an extern
 *
 * Revision 2.1  1995/06/07  20:36:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __STACK_H__
#define __STACK_H__

/*
 * Data structure describing an interrupt level stack
 */
typedef struct intstacktype_ {
    uchar **base;		/* base address */
    uchar **stack;		/* stack address */
    uchar **routine;		/* address of routine */
    ulong *count;		/* interrupt count */
    ulong size;			/* stack size in bytes */
    ulong low;			/* low water stack size */
    void *reent_block;		/* library reentrancy block */
    char *desc;			/* stack description */
} intstacktype;


/*
 * Process level stack parameters
 */

#define STACK_SIZE_DEFAULT	1000	/* words */
#define EXEC_STACK_SIZE		2000	/* words */
#define KERBEROS_STACK_SIZE	2750	/* words */

/*
 * Data structure for holding stack_analyze() results
 * Used to determine worse case process usage of a stack
 */
typedef struct stacktype_ {
    struct stacktype_ *next;
    int size;
    int free;
    char *name;
} stacktype;

/*
 * Interrupt level stack parameters
 */

#define	STACK_INT_THRESHOLD 50  /* complain if stack unused is this low */
#define STACK_INT_NUMBER 7	/* total number of interrupt level stacks */
#define STACK_SIZE_INTERRUPT  	(STACK_SIZE_SCALE * 2000) /* bytes */


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * os/stacks.c
 */
extern intstacktype intstacks[];

extern int stackleft(ulong *, ulong);
extern void changelevel(int, void fn(void), char *);
extern void createlevel(int, void fn(void), char *);
extern boolean level_exists(int);
extern void level_init(void);
extern void make_reason(void);
extern void stack_analyze(sprocess *);
extern void stack_check(void);
extern void stack_hardware_init(void);
extern void stack_setup_interrupt(int, void *);
extern void stack_setup_process(sprocess *, long, void *, long);
extern void stack_printtrace(sprocess *);
extern int stack_remaining(void);
extern void stack_whyreload(void);

#endif		/* __STACK_H__ */
