/* $Id: breakpt.c,v 3.3.12.1 1996/03/21 23:28:26 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36/breakpt.c,v $
 *------------------------------------------------------------------
 * breakp.c
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The software breakpoint mechanism.
 *------------------------------------------------------------------
 * $Log: breakpt.c,v $
 * Revision 3.3.12.1  1996/03/21  23:28:26  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/19  01:01:08  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1995/12/01  04:21:35  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1996/02/17  05:49:03  smaser
 * CSCdi46943:  break command in Rom Monitor causes image checksum
 * validation to fail
 *
 * Revision 3.2  1995/11/17  18:43:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "mon_boot.h"
#include "signal_4k.h"
#include "monlib.h"
#include "mon_defs.h"
#include "extern.h"
#include "exception.h"
#include "context.h"
#include "stack.h"
#include "quicc.h"
#include "signal_4k.h"

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

/*
 * In order to set break points in the system image, one must
 * fist disable system image checksum validation. To do this,
 * the system image calls mon_dont_validate()
 * before validating the image checksum.
 * mon_dont_validate() makes the EMT call EMT_MON_DONT_VALIDATE
 * which returns the state of the sys_chksum_disable flag.
 *
 * sys_chksum_disable
 * Defaults to 0. Note: once set this is not set back to
 * zero. This follows the operation of the dont_validate flag
 * in the system image. IOTW, once this flag gets set, image
 * checksum validation is permanently disabled.
 */
int sys_chksum_disable;

#define MAX_BREAKPOINTS 4
static struct break_point break_points[MAX_BREAKPOINTS];

static unsigned char brkpt_flags;
#define BRKPT_NULL          0x00
#define BRKPT_REQUESTED     0x01
#define BRKPT_SET           0x02
#define BRKPT_ENCOUNTERED   0x04
#define WATCHPT_REQUESTED   0x10
#define WATCHPT_SET         0x20
#define WATCHPT_ENCOUNTERED 0x40
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
    if(ourflag & (WATCHPT_REQUESTED | WATCHPT_SET) &&
       brkpt_flags & WATCHPT_REQUESTED) {
	printf("%s watchpoint at: %#.8x\n",str,
	       ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->sim_bkar);
    } else printf("no watchpoint %s\n",str);
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
    if(brkpt_flags & (WATCHPT_ENCOUNTERED | BRKPT_ENCOUNTERED |
		      BRK_KEY_ENCOUNTERED))
       return(entry_context->esf.pc);
    else return(0);
}

int
set_brkpts(int ourflag)
{
    int j;

    /* clear all existing break/watch flags (except REQUESTED) */
    brkpt_flags &= ~(BRKPT_SET | BRKPT_ENCOUNTERED |
		     WATCHPT_SET | WATCHPT_ENCOUNTERED |
		     BRK_KEY_ENCOUNTERED);
    if(ourflag & BRKPT_REQUESTED) {
	for(j=0; j<MAX_BREAKPOINTS; j++) {
	    if(break_points[j].addr) {
#ifdef DEBUG
printf("set_brkpt: at location %#.8x  contents %#.4x\n",
       break_points[j].addr, *break_points[j].addr);
#endif
		/*
	 	 * Turn off checksum validation of system image.
	 	 */
        	sys_chksum_disable = 1;
		break_points[j].loc_sav = *break_points[j].addr;
		/* use software trap #1 instruction */
		*break_points[j].addr = TRAP_1;
		brkpt_flags |= BRKPT_SET;
	    }
	}
    }
    if(ourflag & WATCHPT_REQUESTED && brkpt_flags & WATCHPT_REQUESTED) {
	((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->sim_bkcr |= 
	    QUICC_BKCR_AS_VALID;
	brkpt_flags |= WATCHPT_SET;
#ifdef DEBUG
printf("bkcr is %#.8x\n",
       ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->sim_bkcr);
#endif
    }
    display_brkpts("set", ourflag);
    return(brkpt_flags & (WATCHPT_SET | BRKPT_SET));
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
    if(ourflag & WATCHPT_SET && brkpt_flags & WATCHPT_SET) {
	((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->sim_bkcr &= 
	    ~QUICC_BKCR_AS_VALID;
	brkpt_flags &= ~WATCHPT_SET;
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
** brkpt_isr
** Process a software breakpoint instruction, a hardware watchpoint,
** the user break, and a step trace break.
*/
void
breakpt_isr(unsigned long stack_frame)
{
    int vectnum;
    cpu_context_t *ccp = (cpu_context_t *)&stack_frame;

#ifdef DEBUG
/*
** Note - enabling this DEBUG will cause the HammerBreak (tm) test to fail.
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

    if(brkpt_flags & (WATCHPT_SET | BRKPT_SET))
	clr_brkpts(WATCHPT_SET | BRKPT_SET);

    vectnum = ccp->esf.offset >> 2;

    switch(vectnum) {
    case VN_HDWRE_BRK:
	/* the hardware watchpoint mechanism */
	printf("Watchpoint reached at location %#x,  PC is %#x\n",
	       ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->sim_bkar,
	       ccp->esf.pc);
	brkpt_flags |= WATCHPT_ENCOUNTERED;
	break;
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
    setvbr(mon_vbr_sav);  /* use monitor vector base register */
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
	brkpt_flags &= ~(WATCHPT_ENCOUNTERED | BRKPT_ENCOUNTERED |
			 BRK_KEY_ENCOUNTERED);  /* unset flags */
	if(setb) set_brkpts(BRKPT_REQUESTED | WATCHPT_REQUESTED);
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
	brkpt_flags &= ~(WATCHPT_ENCOUNTERED | BRKPT_ENCOUNTERED |
			 BRK_KEY_ENCOUNTERED);  /* unset flags */
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
    if(setb) set_brkpts(BRKPT_REQUESTED | WATCHPT_REQUESTED);
    launch(launchcode, (struct plb *)0, &pib, addr ? addr : entrypt);
    return(0);
}

/*
** Support the 360 hardware breakpoint mechanism.
** We call this the watchpoint mechanism.
**
** watch [-nrws:b:m:] <address>
**
*/

int
watch_point(int argc, char *argv[])
{
    int i;
    unsigned long addr;
    long temp;
    unsigned long bkcr;
    int set_em = 0;
    int addr_valid = 0;
    static char *optstr = "csdnrwu:b:m:";
    extern char *optarg;

    bkcr = (QUICC_BKCR_RW | QUICC_BKCR_SIZM);  /* defaults */
    while((i = getopt(argc,argv,optstr)) >= 0) {
	switch(i) {
	case 'c':  /* clear the watchpoint */
	    clr_brkpts(WATCHPT_SET);
	    return(0);
	case 'd':  /* delete the watchpoint request */
	    brkpt_flags &= ~WATCHPT_REQUESTED;
	    break;
	case 'n':
	    bkcr |= QUICC_BKCR_NEG;    /* use negative block logic */
	    break;
	case 'r':
	    bkcr &= ~QUICC_BKCR_RW_MSK;
	    bkcr |= QUICC_BKCR_READ;   /* only read accesses */
	    break;
	case 's':  /* set the watchpoint */
	    set_em = 1;  /* set the flag to do it later */
	    break;
	case 'w':
	    bkcr &= ~QUICC_BKCR_RW_MSK;
	    bkcr |= QUICC_BKCR_WRITE;  /* only write accesses */
	    break;
	case 'u':
	    if(getnum(optarg, 10, &temp) != 1) {
ill_op_arg:
		printf("illegal option argument \"%c\"\n", *optarg);
usage:
		printf("usage: %s [-%s] [<addr>]\n", argv[0], optstr);
		printf("  -c     clear watchpoint\n"
		       "  -s     set watchpoint\n"
		       "  -d     delete watchpoint request\n"
		       "  -n     block negative logic\n"
		       "  -r     read cycles only\n"
		       "  -w     write cycles only\n"
		       "  -u 1 | 2 | 3 | 4\n"
		       "         specify unit size in bytes\n"
		       "  -b 2 | 8 | 32\n"
		       "         specify block size in K\n"
		       "  -m ndma | ncpu | nsprog | nsdat | nuprog | nudat\n"
		       "         specify address space to mask out\n"
		       "  defaults to longword access, all cycles,\n"
		       "  all address spaces, not a block\n");
		return(1);    
	    }
	    switch(temp) {
	    case 1:
		bkcr |= QUICC_BKCR_SIZ_B;   /* byte size */
		break;
	    case 2:
		bkcr |= QUICC_BKCR_SIZ_W;   /* word size */
		break;
	    case 3:
		bkcr |= QUICC_BKCR_SIZ_3B;  /* 3 byte size */
		break;
	    case 4:
		bkcr |= QUICC_BKCR_SIZ_LW;  /* longword size */
		break;
	    default:
		goto ill_op_arg;
	    }
	    bkcr &= ~QUICC_BKCR_SIZM;  /* size bits are valid */
	    break;
	    
	case 'b':
	    if(getnum(optarg, 10, &temp) <= 0) goto ill_op_arg;
	    switch(temp) {
	    case 2:
		bkcr |= QUICC_BKCR_MA_2K;   /* 2k block size */
		break;
	    case 8:
		bkcr |= QUICC_BKCR_MA_8K;  /* 8k block size */
		break;
	    case 32:
		bkcr |= QUICC_BKCR_MA_32K;  /* 32k block size */
		break;
	    default:
		goto ill_op_arg;
	    }
	    break;

	case 'm':
	    if(strcmp(optarg, "ndma") == 0) {
		bkcr |= QUICC_BKCR_AS_DMA;
		break;
	    }
	    if(strcmp(optarg, "ncpu") == 0) {
		bkcr |= QUICC_BKCR_AS_CPU;
		break;
	    }
	    if(strcmp(optarg, "nsprog") == 0) {
		bkcr |= QUICC_BKCR_AS_SUPPROG;
		break;
	    }
	    if(strcmp(optarg, "nsdat") == 0) {
		bkcr |= QUICC_BKCR_AS_SUPDAT;
		break;
	    }
	    if(strcmp(optarg, "nuprog") == 0) {
		bkcr |= QUICC_BKCR_AS_USRPROG;
		break;
	    }
	    if(strcmp(optarg, "nudat") == 0) {
		bkcr |= QUICC_BKCR_AS_USRDAT;
		break;
	    }
	    /* fall through */

	default:
	    goto usage;
	}
    }

    if(argc == optind) {
	if(!set_em) {
	    /* display watchpoint (and breakpoints) */
	    display_brkpts("requested", WATCHPT_REQUESTED);
	    return(0);
	}
    } else {
	if(getnum(argv[optind++],16,&addr) <= 0) {
	    printf("illegal address\n");
	    goto usage;
	} else addr_valid = 1;
    }
    if(argc != optind) goto usage;

    if(addr_valid)
	((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->sim_bkar = addr;

    ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->sim_bkcr = bkcr;
    brkpt_flags |= WATCHPT_REQUESTED;    
    if(set_em) set_brkpts(WATCHPT_REQUESTED);
    else display_brkpts("requested", WATCHPT_REQUESTED);
#ifdef DEBUG
printf("bkcr reg is %#.8x\n",
       ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->sim_bkcr);
#endif
    return(0);
}

/* end of module */
