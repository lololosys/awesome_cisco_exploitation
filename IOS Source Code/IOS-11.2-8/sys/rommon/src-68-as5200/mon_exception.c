/* $Id: mon_exception.c,v 1.1.68.1 1996/06/16 21:18:52 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/mon_exception.c,v $
 *------------------------------------------------------------------
 * mon_exception.c Exception processing routines for Brasil.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mon_exception.c,v $
 * Revision 1.1.68.1  1996/06/16  21:18:52  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:16:30  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Handle all of the exceptions for a MC68EC030 chip.
 * The MC68EC030 has a "type a|b" stack frame.
 */

#include "setjmp.h"
#include "pcmap.h"
#include "nvmonvars.h"
#include "monitor.h"
#include "signal_4k.h"
#include "mon_defs.h"
#include "mon_plat_defs.h"
#include "bitdef.h"
#include "exception.h"
#include "context.h"
#include "stack.h"
#include "reason.h"
#include "mon_boot.h"
#include "extern.h"

unsigned short mstat_snapshot;
volatile unsigned berr_addr;
extern SIG_PF sigbusfcn;

static cpu_context_t context_sav;
static stack_trace_info_t stack_sav[NUM_STACK_FRAMES];
cpu_context_t *entry_context;
stack_trace_info_t *entry_stack_sav_p;
long mon_vbr_sav;
short mon_sr_sav;

static char unassigned[] = "unassigned, reserved";
static char *exstr[] = {
  "",
  "",
  "BUS ERROR",
  "ADDRESS ERROR",
  "illegal instruction",
  "zero divide",
  "CHK instruction",
  "TRAPV instruction",
  "privilege violation",
  "trace",
  "line 1010 emulator",
  "line 1111 emulator",
  unassigned,
  unassigned,
  unassigned,
  "uninitialized",
};

void
exception(unsigned long stack_frame)
{
    cpu_context_t *ccp = (cpu_context_t *)&stack_frame;
    struct excpt_stack_frame *esfptr = &ccp->esf;
    int exceptnum = (esfptr->offset >> 2);
    char *str = exstr[exceptnum];
    unsigned long pc = esfptr->pc;
    unsigned short ssw;
    static int abort_task();
    int reason = 0;  /* init */

    entry_context = ccp;  /* set global pointer for other utilities */
    if(exceptnum < 4) {   /* bus error or address error */
	hkeepflags |= H_BUSERR;     /* set bus error flag */
	/* MC68EC030 has a type a|b buserr stack frame */
	berr_addr = esfptr->stack.typeb.faddr;
	ssw = esfptr->stack.typeb.ssw;  /* pick up special status word */
	if(exceptnum == 2) {  /* bus error */
	  /*
	   * Unset the rerun flags on bus error
	   */
	  esfptr->stack.typeb.ssw = ssw & 0xceff;
#ifdef DEBUG
printf("bus error at (return) pc %#x, faulted addr %#x\n", pc, berr_addr);
putchar('\n');
#endif
	}

	switch((int)sigbusfcn) {
	case SIG_IGN:
	    return;
	case SIG_DFL:
	    break;
	default:
	    if((*sigbusfcn)((int)esfptr) == (int)SIG_DFL) break;
	    return;
	}
    }

    /*
     * If we reached here due to a system fault record the facts
     */
    if(sys_state == SYSTEM_RUNNING) {
	switch(exceptnum) {
	case VN_BERR: 
		reason = EXC_BUSERR;
	    break;
	case VN_ADDR_ERR:
	    reason = EXC_ADRERR;
	    break;
	case VN_TRAP2:
	    str = "software forced crash";
	    reason = (int)str;
	    break;
	default:
	    reason = EXC_UNEXP_INT;
	    break;
	}
	re_init_con();  /* re-init the console */
	/* restore the monitor state */
	ccp->esf.sr = mon_sr_sav;  /* use monitor status register */
	ccp->vbr = mon_vbr_sav;  /* restore the monitor vbr */
	sys_state = MONITOR_RUNNING;
	log_sys_ret(reason, ccp);
    }

    /*
    ** Print out the exception info for the user
    */
    printf("\n*** %s ***\n", exceptnum < 4 ? str : "EXCEPTION");

    copy_context(ccp);  /* save our context here */

    if(exceptnum >= 4) {  /* neither bus nor address error */
	if(exceptnum < (sizeof(exstr)/sizeof(char *))) {
	    printf("%s interrupt\n", str);
	} else {
	    if(exceptnum >= VN_AVECT1 && exceptnum <= VN_AVECT7)
		printf("auto vector interrupt level %d\n",
		       exceptnum - (VN_AVECT1 - 1));
	    else if(exceptnum == VN_TRAP2)
		printf("%s\n", str);
	    else if(exceptnum >= VN_TRAP0 && exceptnum <= VN_TRAP15)
		printf("user trap # %d\n", exceptnum - VN_TRAP0);
	    else {
		switch(exceptnum) {
		case VN_SPUR_INT:
		    puts("spurious interrupt\n");
		    break;
		case 48:
		    printf("%s interrupt\n",unassigned);
		    break;
		case 64:
		    puts("unexpected vectored interrupt\n");
		    break;
		default:	
		    printf("exception number = %d\n", exceptnum);
		    break;
		}
	    }
	}
    } else {  /* either bus or address error */
	printf("Bus error at (return) pc 0x%08x, faulted addr 0x%08x",
		pc, berr_addr);
	printf(", scr1 0x%04x\n", *(ushort *)ADRSPC_CONTROL);
    }

    /*
     * Check to see if we should abort due to this exception.
     * Note that we do not simply longjmp(monjmpptr,1),
     * we set it up so that the longjmp happens after we execute
     * a rte (return from exception) so that the processor state
     * is restored.
     * Address errors are almost impossible to recover from so
     * abort whenever possible.
     */
    if(monjmpptr &&
       (exceptnum == 3 || envflag == INMON || stoponerr())) {
	esfptr->pc = (long)&abort_task;
	/* 
	 * If address error, remake the frame as Format $0
	 * Don't have to worry about rewinding the stack as abort_task
	 * is going to longjump which will set the SP correctly
	 */
        esfptr->type = 0;   /* Fake short frame so 68k uses our abort pc at RTE */
    }
}

void
copy_context(cpu_context_t *ccp)
{
    /* make a copy of our context here for later use as debugging aid */
    bcopy((uchar *)ccp, (uchar *)&context_sav, sizeof(cpu_context_t));
    /* and save our stack trace */
    save_stack_trace((ulong *)ccp->a6, stack_sav, NUM_STACK_FRAMES);
    entry_context = &context_sav;  /* set global pointer for other utilities */
    entry_stack_sav_p = stack_sav;
}

static
abort_task()
{
    longjmp(monjmpptr,2);
}

/*
 * log_sys_ret : log the system return info for future use
 */
void
log_sys_ret(int reason, cpu_context_t *ccp)
{
    /*
    ** Only log errors from healthy target images or reset
    */
    if(reason != EXC_RESET && !boot_satisfied) return;

    /* init area with zeroes */
    bzero((NVRAM)->sys_ret_info, sizeof((NVRAM)->sys_ret_info));

    if(ccp) {
	save_stack_trace(ccp->a6,
			 (stack_trace_info_t *)(&(NVRAM)->sys_ret_info[BOOT_STACK_TRACE]),
			 BOOT_STACK_FRAMES);

	(NVRAM)->sys_ret_info[BOOT_PC] = ccp->esf.pc;

	if(reason == EXC_BUSERR) {
	    (NVRAM)->sys_ret_info[BOOT_ADDRESS] = ccp->esf.stack.typeb.faddr;
	} else if(ccp->esf.type == 2) {    /* six word stack frame */
	    (NVRAM)->sys_ret_info[BOOT_ADDRESS] = ccp->esf.stack.type2.instaddr;
	} else {                           /* four word stack frame */
	    (NVRAM)->sys_ret_info[BOOT_ADDRESS] = ccp->esf.pc;
	}
    }

    (NVRAM)->sys_ret_info[BOOT_REASON] = reason;
    (NVRAM)->sys_ret_info[BOOT_COUNT] = BOOTDATA;
    savenv();  /* write it out to NVRAM */
}

/* End of Module */

