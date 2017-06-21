/* $Id: breakpt.c,v 3.1.72.1 1996/06/16 21:18:37 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68/breakpt.c,v $
 *------------------------------------------------------------------
 * breakpt.c -- The software breakpoint mechanism.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: breakpt.c,v $
 * Revision 3.1.72.1  1996/06/16  21:18:37  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/03  21:19:54  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "types.h"
#include "mon_boot.h"
#include "signal_4k.h"
#include "monlib.h"
#include "mon_defs.h"
#include "extern.h"
#include "exception.h"
#include "context.h"
#include "stack.h"
#include "pcmap.h"
#include "as5200_pcmap.h"
#include "reason.h"
#include "signal_4k.h"
#include "console.h"

/*
** software breakpoint mechanism
** 
** break <addr>
** sets breakpoint(s) - check to see if the address bus errs first
**
** break -d 1 | 2 | 3 | 4 | all
** clears breakpoint(s)
**
** break
** displays the breakpoint(s)
*/

struct break_point {
    unsigned short *addr;
    unsigned short loc_sav;
};

#define MAX_BREAKPOINTS 4
static struct break_point break_points[MAX_BREAKPOINTS];

static unsigned char brkpt_flags;
#define BRKPT_NULL          0x00
#define BRKPT_REQUESTED     0x01
#define BRKPT_SET           0x02
#define BRKPT_ENCOUNTERED   0x04
#define BRK_KEY_ENCOUNTERED 0x80  /* user hit the break key */

#define TRAP_1 0x4e41  /* this is our software breakpoint trap */

void
display_brkpts(char *str, unsigned char ourflag)
{
    int j;
    int banner_printed = 0;

    if(ourflag & (BRKPT_REQUESTED | BRKPT_SET)) {
	for(j=0; j<MAX_BREAKPOINTS; j++) {
	    if(break_points[j].addr) {
		if(!banner_printed) {
		    printf("%s breakpoint(s) at:\n",str);
		    banner_printed = 1;
		}
		printf("(%d) %#.8x  ", j+1, break_points[j].addr);
	    }
	}
    }
    if(!banner_printed) printf("no breakpoints %s\n",str);
    else putchar('\n');
}

int
is_brkpt_addr(unsigned short *addr)
{
    int j;

    for(j=0; j<MAX_BREAKPOINTS; j++) {
	if(break_points[j].addr == addr)
	    return(1);
    }
    return(0);
}

/*
** Return the breakpoint address we are at or zero
** if none.
*/
unsigned long
at_brkpt(void)
{
    if(brkpt_flags & (BRK_KEY_ENCOUNTERED| BRKPT_ENCOUNTERED))
       return(entry_context->esf.pc);
    else return(0);
}

int
set_brkpts(int ourflag)
{
    int j;

    /* clear all existing break/watch flags (except REQUESTED) */
    brkpt_flags &= ~(BRKPT_SET | BRKPT_ENCOUNTERED | BRK_KEY_ENCOUNTERED);
    if(ourflag & BRKPT_REQUESTED) {
	for(j=0; j<MAX_BREAKPOINTS; j++) {
	    if(break_points[j].addr) {
#ifdef DEBUG
printf("set_brkpt: at location %#.8x  contents %#.4x\n",
       break_points[j].addr, *break_points[j].addr);
#endif
		break_points[j].loc_sav = *break_points[j].addr;
		/* use software trap #1 instruction */
		*break_points[j].addr = TRAP_1;
		brkpt_flags |= BRKPT_SET;
	    }
	}
    }
    display_brkpts("set", ourflag);
    return(brkpt_flags & BRKPT_SET);
}

void
clr_brkpts(int ourflag)
{
    int j;

    if(ourflag & BRKPT_SET) {
	if(brkpt_flags |= BRKPT_SET) {
	    for(j=0; j<MAX_BREAKPOINTS; j++) {
		if(break_points[j].addr) {
#ifdef DEBUG
		    printf("clr_brkpt: at location %#.8x  contents %#.4x\n",
			   break_points[j].addr, break_points[j].loc_sav);
#endif
		    *break_points[j].addr = break_points[j].loc_sav;
		}
	    }
	    brkpt_flags &= ~BRKPT_SET;
	}
    }
    display_brkpts("clear", ourflag);
}


int
break_point(int argc, char *argv[])
{
    int i,j;
    unsigned short *addr, addr_contents;
    long temp;
    int set_em = 0;
    int addr_valid = 0;
    static char *optstr = "scd:";
    extern char *optarg;

    while((i = getopt(argc,argv,optstr)) >= 0) {
	switch(i) {
	case 'd':  /* delete the breakpoint */
	    if(*optarg == 'a') {  /* 'a' is for all */
		for(j=0; j<MAX_BREAKPOINTS; j++) {
		    break_points[j].addr = (unsigned short *)0;
		}
		brkpt_flags &= ~BRKPT_REQUESTED;
		break;
	    }
	    while(*optarg) {
		j = getnum(optarg, 10, &temp);
		if(j <= 0) {
ill_op_arg:
		    printf("illegal option argument \"%c\"\n", *optarg);
usage:
		    printf("usage: %s [-%s] [<addr>]\n"
			   "  -s   set breakpoint\n"
			   "  -c   clear breakpoint\n"
			   "  -d   delete breakpoint request\n",
			   argv[0], optstr);
		    return(1);    
		}
		optarg += j;  /* bump to next */
		temp -= 1;    /* convert to usable index */
		if(temp >= 0 || temp < MAX_BREAKPOINTS) {
		    if(break_points[temp].addr) {
			printf("breakpoint at location %#x is cleared\n",
			       break_points[temp].addr);
			break_points[temp].addr = (unsigned short *)0;
		    } else {
			printf("breakpoint %d is null, "
			       "ignoring clear command\n", temp+1);
		    }
		}
		if(*optarg == '\0') break;    /* we are done */
		if(*optarg == ',') optarg++;  /* bump past and continue */
		else goto ill_op_arg;         /* illegal character */
	    }
	    break;
	case 'c':  /* clear the breakpoints */
	    clr_brkpts(BRKPT_SET);
	    return(0);
	case 's':  /* set the breakpoints */
	    set_em = 1;
	    break;
	default:
	    goto usage;
	}
    }

    if(optind < argc) {
	if(getnum(argv[optind++],16,&addr) <= 0) {
	    printf("illegal address\n");
	    goto usage;
	}
	if((long)addr & 1) {
	    printf("breakpoint address must be on even boundary\n");
	    goto usage;
	}
	addr_valid = 1;
    }

    if(argc != optind) goto usage;

    if(addr_valid) {
	if(chkberr((volatile unsigned long *)addr, BW_16BITS, 1) == 0) {
	    /*
	    ** We just verified that we can read the address
	    ** now verify we can write it.
	    */
	    addr_contents = *addr;  /* save contents */
	    if(chkberr((volatile unsigned long *)addr, BW_16BITS, 0) != 0)
		goto addr_berr;
	    *addr = addr_contents;  /* restore contents */
	    /*
	    ** also need to make sure this is a proper
	    ** instruction boundary?  how?
	    */
	    for(j=0; j<MAX_BREAKPOINTS; j++) {
		if(!break_points[j].addr) {
		   break_points[j].addr = addr;
		   break_points[j].loc_sav = TRAP_1;
		   brkpt_flags |= BRKPT_REQUESTED;
		   addr_valid = 0;
		   break;
		}
	    }
	    if(addr_valid) {
		printf("sorry, no more room in the breakpoint table\n");
	    }
	} else {
addr_berr:
	    printf("a bus error results from accessing this address\n");
	    return(1);
	}
    }
    if(set_em) set_brkpts(BRKPT_REQUESTED);
    else display_brkpts("requested", BRKPT_REQUESTED);
    return(0);
}

/*
 * brkpt_isr
 * Process a software breakpoint instruction, a hardware watchpoint,
 * the user break, and a step trace break.
 */
void
breakpt_isr(unsigned long stack_frame)
{
    int vectnum;
    cpu_context_t *ccp = (cpu_context_t *)&stack_frame;

#ifdef DEBUG
/*
 * Note - enabling this DEBUG will cause the HammerBreak (tm) test to fail.
 */
printf("\nMON breakpt_isr: vector number is: %d, pc is %#x, a6 is %#x\n",
       ccp->esf.offset >> 2, ccp->esf.pc, ccp->a6);
#endif

    entry_context = ccp;  /* set global pointer for other utilities */
    /* this is a live stack - get stack info from actual stack */
    entry_stack_sav_p = (stack_trace_info_t *)0;

    if(ccp->a6 == 0) {    /* null frame pointer */
	monitor(1);       /* invoke monitor first time (reset) */
	return;           /* should never get here */
    }

    uartaddr->a.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;
    if(brkpt_flags & BRKPT_SET)
	clr_brkpts(BRKPT_SET);

    vectnum = ccp->esf.offset >> 2;

    switch(vectnum) {
    case VN_TRAP1:
	/* a software breakpoint */
	if(is_brkpt_addr((ushort *)(ccp->esf.pc - 2))) {
	    ccp->esf.pc -= 2;  /* back up over brkpt instruction */
	    printf("Software breakpoint reached at location %#x\n",
		   ccp->esf.pc);
	} else {
	    printf("Unexpected software breakpoint encountered "
		   "at location %#x\n", ccp->esf.pc);
	}
	/* fall thru */
    case VN_TRACE:
	/* this is a single step */
	brkpt_flags |= BRKPT_ENCOUNTERED;
	ccp->esf.sr &= ~SR_TRACE_MSK;  /* turn off trace bits */
	break;
      case VN_TRAP0:
	/* user break from system image via trap 0 */
	log_sys_ret(EXC_ABORT, ccp);  /* log sys info here */
	/* fall thru */
      case VN_AVECT7:
	/* user break in monitor when sys_state != MONITOR_RUNNING */
	brkpt_flags |= BRK_KEY_ENCOUNTERED;
	printf("\nUser break detected at location %#x\n", ccp->esf.pc);
	break;
    }
    /* restore the monitor state */
    setvbr(mon_vbr_sav); /* use monitor vector base register */
    /* use monitor interrupt priority level */
    chgint((mon_sr_sav & SR_INT_PRI_MSK) >> SR_INT_PRI_SH);
    sys_state = MONITOR_RUNNING;
    signal(SIGINT, SIG_DFL);  /* OK to process user breaks again */
    monitor(0);  /* invoke monitor in non-reset mode */
    /* monitor never returns */
}

/*
** sys_cont
** Continue execution from a software breakpoint
*/
static char *no_brkpt_msg = "Sorry, you must be at a breakpoint to %s\n";
int
sys_cont(int argc, char *argv[])
{
    char setb = 0;

    if(argc > 2) {
usage:
	printf("usage: %s [-b]\n"
	       "  -b  continue with break/watchpoints\n",
	       argv[0]);
	return(1);
    }
    if(argc == 2) {
	if(strcmp(argv[1],"-b") != 0) goto usage;
	else setb = 1;
    }
    if(at_brkpt()) {
	/* unset flags */
	brkpt_flags &= ~(BRKPT_ENCOUNTERED | BRK_KEY_ENCOUNTERED);
	if(setb) set_brkpts(BRKPT_REQUESTED);
	cont_brkpt();  /* this is in assembler in start.S */
	return(0);     /* you should never really get here */
    }
    printf(no_brkpt_msg, "continue");
    return(1);
}

/*
** Step trace routine
** Allow single step and step to branch.
** Disassemble instruction at current PC on single step.
*/
int
sys_step(int argc, char *argv[])
{
    ushort trace_type = SR_TRACE_SGL;  /* single step is default */

    if(argc > 2) {
usage:
	printf("usage: %s [-b]\n", argv[0]);
	return(1);
    }
    if(argc == 2) {
	if(strcmp(argv[1],"-b") != 0) goto usage;
	else trace_type = SR_TRACE_BRA;  /* step to branch */
    }
    if(at_brkpt()) {
	/* unset flags */
	brkpt_flags &= ~(BRKPT_ENCOUNTERED | BRK_KEY_ENCOUNTERED);
	/* display disassembled instruction at PC */
	dinstr(entry_context->esf.pc, 1);
	putchar('\n');
	entry_context->esf.sr |= trace_type;  /* set trace bits */
	cont_brkpt();  /* this is in assembler in start.S */
	return(0);     /* you should never really get here */
    }
    printf(no_brkpt_msg, "step");
    return(1);
}    

/*
 * sys_launch : Launch a loaded system image.
 * launch
 */
int
sys_launch(int argc, char *argv[])
{
    int launchcode = PROM2_BOOTLOAD;  /* default */
    struct pib pib;
    unsigned long addr = 0;
    int i;
    char setb = 0;
    static char *optstr = "bd";

    while((i = getopt(argc,argv,optstr)) >= 0) {
	switch(i) {
	case 'b':  /* launch with breakpoints */
	    setb = 1;
	    break;
	case 'd':
	    launchcode = PROM2_BOOTDEBUG;
	    break;
	default:
usage:
	    printf("usage:%s [-%s] [<addr>]\n"
		   "-d  launch with debugger break\n"
		   "-b  launch with break/watchpoints\n", argv[0], optstr);
	    return(1);    
	}
    }

    if(optind < argc) {
	if(getnum(argv[optind++],16,&addr) <= 0)
	    goto usage;
    }

    /*
    ** May want to allow passing args to the launched program here.
    ** Will need to shift out above args and do other work if we do.
    */
    if(argc != optind) goto usage;

    if ( !memsize ) {
	printf("%s: can not launch, memory size is zero\n", argv[0]);
	return(1);
    }
    if(!addr && !entrypt) {
	printf("%s: can not launch, no entry point\n", argv[0]);
	return(1);
    }
    pib.mifp = mifp;
    pib.argc = argc;
    pib.argv = argv;
    if(setb) set_brkpts(BRKPT_REQUESTED);
    launch(launchcode, (struct plb *)0, &pib, addr ? addr : entrypt);
    return(0);
}

/* end of module */
