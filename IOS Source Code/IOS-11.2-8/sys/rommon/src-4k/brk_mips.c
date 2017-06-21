/* $Id: brk_mips.c,v 3.3.18.1 1996/03/21 23:29:57 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k/brk_mips.c,v $
 *------------------------------------------------------------------
 * brk_mips.c
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Break and Continue for MIPS/Orion.
 *------------------------------------------------------------------
 * $Log: brk_mips.c,v $
 * Revision 3.3.18.1  1996/03/21  23:29:57  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/19  01:01:21  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1995/12/01  04:22:35  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1996/02/09  00:02:12  tkam
 * CSCdi47873:  HSA ROMMON needs to add flow control to dbus download
 * Add download flow control to HSA ROMMON.
 *
 * Revision 3.2  1995/11/17  18:44:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "signal_4k.h"
#include "mon_defs.h"
#include "queryflags.h"
#include "dis_mips.h"
#include "cpu_mips.h"
#include "mon_boot.h"
#include "monitor.h"

extern int optind;
extern long memsize;
extern unsigned long entrypt;
extern r4k_context_t *kernel_context_ptr, *process_context_ptr;
extern char *optarg;
extern char *reg_names[];

struct breakpoint_t {
    unsigned long *addr;
    unsigned long instr;
    int valid;
};

static struct breakpoint_t brkpoint;
static char *optstr = "s:c";

/*
 * brk_clear : Clear the breakpoint
 */
void
brk_clear(void)
{
    if ( brkpoint.valid ) {

	/*
	 * Check that the address is readable and writable
	 */
	if (chkberr(brkpoint.addr, BW_32BITS, 0) == 0) {
	    *brkpoint.addr = brkpoint.instr;
	}
    }
    brkpoint.valid = 0;
}

/*
 * brk_set : Set the breakpoint
 */
void
brk_set(unsigned long *addr)
{
    unsigned long temp;

    if ( brkpoint.valid ) {
	if (chkberr(brkpoint.addr, BW_32BITS, 0) == 0) {
	    *brkpoint.addr = brkpoint.instr;
	}
    }

    brkpoint.valid = 0;
    if (chkberr(addr, BW_32BITS, 1) != 0) {
	printf("Error: address 0x%x is unreadable\n", addr);
	return;
    }
    temp = *addr;					/* take snapshot    */
    if (chkberr(addr, BW_32BITS, 0) != 0) {		/* check rd/wr	    */
	addr = (unsigned long *)
	    (PHY_TO_KSEG0(PHY_ADDR((int)addr)));	/* convert to kseg0 */
	if (chkberr(addr, BW_32BITS, 0) != 0) {		/* check rd/wr	    */
	    printf("Error: converted addr. 0x%x is unwritable\n", addr);
	    return;
	} else {
	    printf("Converted address to KSeg0 addr (0x%x)\n", addr);
	}
    }
    brkpoint.valid = 1;					/* set valid	    */
    brkpoint.addr = addr;				/* record addr	    */
    brkpoint.instr = temp;				/* record instr	    */
    *addr = MIPS_BREAK_POINT;				/* write break	    */
}

/*
 * break : Set/Show/Clear breakpoint. Default is to show it.
 * break [-s:c] [address]
 */
int
break_point(int argc, char *argv[])
{
    int i, clr_flag = 0;
    unsigned long *addr = 0;

    while((i = getopt(argc,argv,optstr)) >= 0) {
	switch(i) {
	case 's':  /* set the breakpoint */
	    if ( !getnum(optarg,16,(unsigned long *)&addr) )
		goto usage;
	    break;
	case 'c':  /* clear the breakpoint */
	    clr_flag = 1;
	    break;
	default:
usage:
	    printf("usage: %s [-%s]\n", argv[0], optstr);
	    return(1);    
	}
    }
    if ( argc != optind ) 
	goto usage;
    if ( clr_flag )             /* clear the break point */
	brk_clear();
    if ( addr )
	brk_set(addr);
    if ( brkpoint.valid )
	printf("breakpoint set to 0x%.8x\n", brkpoint.addr);
    else
	printf("breakpoint is cleared\n");
    cache_flush(0, -1);
    return(0);
}

/*
 * cont : Continue execution of a downloaded image. Must have a context.
 * cont [address]
 */
int
sys_cont(int argc, char *argv[])
{
    unsigned long addr = 0;

    if ((argc == 2 && getnum(argv[1],16,&addr) == 0) || 
	argc > 2 ) {
	printf("usage: %s [address]\n", argv[0]);
	return(1);
    }
    if ( !kernel_context_ptr ) {
	printf("%s: can not continue, no system kernel context\n", argv[0]);
	return(1);
    } else if ( addr ) {
	kernel_context_ptr->epc = addr;
	kernel_context_ptr->epc_hi = (addr >> 31) ? -1 : 0;
    }
    hkeepflags &= ~H_USRINT; /* clear house keeping flag upon cont */
    relaunch(kernel_context_ptr);
    kernel_context_ptr = 0;
    process_context_ptr = 0;
    return(0);
}

/*
 * sys_launch : Launch a loaded system image.
 * launch
 */
int
sys_launch(int argc, char *argv[])
{
    int launchcode = PROM2_BOOTLOAD;  /* default */

    if ( argc == 2 && strcmp(argv[1],"-d") == 0 ) {
	launchcode = PROM2_BOOTDEBUG;
    } else if ( argc > 1 ) {
	printf("usage:%s [-d]\n"
	       "-d  launch with debugger break\n", argv[0]);
	return(1);
    }
    if ( !memsize ) {
	printf("%s: can not launch, memory size is zero\n", argv[0]);
	return(1);
    }
    launch(launchcode, (void *)0, (void *)0, entrypt);
    kernel_context_ptr = 0;
    process_context_ptr = 0;
    return(0);
}

/*
 * show_context : If the system context is valid, print it out
 */
int
show_context(int argc, char *argv[])
{
    r4k_context_t *ptr;
    int i, proc;

    if (argc != 1) {
	printf("usage: %s\n", argv[0]);
	return(1);
    }

    for(proc = 0; proc < 2; proc++) {  /* do both kernel and process level */
	if (proc) {
	    ptr = process_context_ptr;
	} else {
	    ptr = kernel_context_ptr;
	}
	if ( !ptr ) {
	    printf("%s: %s context is not valid\n", argv[0],
		   proc ? "process" : "kernel");
	    return(1);
	}
	printf("%s Level Context:\n", proc ? "Process" : "Kernel");
	printf(" Reg       MSW        LSW     | Reg       MSW        LSW\n");
	printf("------  ---------- ---------- | -----   ---------- ----------\n");
	for (i=0; i<16; i++) {
	    printf("%s : %.8x   %.8x  | %s : %.8x   %.8x\n",
		   reg_names[i], ptr->regs[(i*2)], ptr->regs[(i*2)+1],
		   reg_names[i+16], ptr->regs[(i+16)*2], ptr->regs[((i+16)*2)+1]);
	}
	printf("HI     : %.8x   %.8x  | LO     : %.8x   %.8x\n",
	       ptr->mdhi_hi, ptr->mdhi,
	       ptr->mdlo_hi, ptr->mdlo);
	printf("EPC    : %.8x   %.8x  | ErrPC  : %.8x   %.8x\n",
	       ptr->epc_hi, ptr->epc,
	       ptr->err_epc_hi, ptr->err_epc);
	printf("Stat   : %.8x             | Cause  : %.8x\n\n",
	       ptr->sreg, ptr->cause);
    }
    return(0);
}

/* End of Module */
