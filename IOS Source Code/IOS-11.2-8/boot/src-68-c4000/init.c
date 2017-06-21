/* $Id: init.c,v 3.3 1996/01/03 03:09:02 clev Exp $
 * $Source: /vws/aam/cherf.111/ios/boot/src-68-c4000/init.c,v $
 *------------------------------------------------------------------
 * init.c -- system hardware initialization
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: init.c,v $
 * Revision 3.3  1996/01/03  03:09:02  clev
 * CSCdi46211:  Memory sizing broke on 4000 platform
 *
 * Fix old monitor code so it doesn't get optimized out with newer
 * versions of the compiler (gcc.94q2.68k).
 *
 * Revision 3.2  1995/12/03  17:55:25  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.1  1995/11/09  09:09:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1994/03/19 02:08:45  clev
 * CSCdi17574:  Handle the split main memory on cost reduced C4000
 * Add code to handle the 8m and 32m options on the cost reduced
 * c4000
 *
 * Revision 1.1  1993/09/24  16:33:55  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.15  1993/08/19  03:51:39  hampton
 * Bring modular system to Aries 9.21(0.18) patch level.
 *
 * CSCdi09946:  Protect stack_whyreload() from bad boot_reason values
 * 
 * Doing ROM monitor changes required to support EMT_SET_CACR on all
 * 68020/68030/68040 platforms. Share setcacr() and getcacr() on all of
 * these platforms. Move all defines for bits in CACR registers to the
 * pcmap.h files for each processor so that process level code does not
 * have to have its own definition of the CACR bits.
 * 
 * Add better comments to explain memory sizing code in Merlot (IGS/TR)
 * ROM monitor.
 * 
 * Add new EMT call, EMT_WRITE_BAD_MAIN_MEMORY_PARITY, used by the new
 * 'test crash' menu in the system image. This EMT call writes a 32 bit
 * word of information into main memory with bad parity.
 * 
 * Fix EMT number collision between Clean and 9.1 for new EMT call to make
 * life easier for the 9.17 release.  Also, the numbers for all new EMT
 * calls currently defined in the 9.21 release are now also defined in the
 * 9.1 release as placeholders to prevent such EMT collision problems in
 * the future.
 *
 * Revision 1.14  1993/02/28  23:00:01  schaefer
 * Chassis MIB
 *
 * Revision 1.13  1993/01/30  01:12:17  whp4
 * Add RP1 support.
 *
 * Revision 1.12  1992/08/21  22:05:54  smackie
 * Changed the IO base address to 0x6000000 and the acu registers to the
 * correct cache mappings. Also changed the cache default to 0x2111
 *
 * Revision 1.11  1992/07/08  08:15:34  schaefer
 * remove cookie-based software capability bits
 *
 * Revision 1.10  1992/06/16  00:28:31  clev
 * Allow XX to run net booted system images with different loadpoint from
 * firmware image.
 *
 * Clean up diag messages in flash erase routine.
 *
 * Revision 1.9  1992/06/08  23:55:40  shaker
 * CSCdi05317, "default config reg setting should have NOCONFIG set".
 *
 * Revision 1.8  1992/05/28  17:21:29  kph
 * Bugfix: baseaddr wasn't set correctly (on XX) and the code attempted to
 * work around things by picking up the start address from the a.out file.
 * This doesn't work, as the ROM monitor needs to know a priori where the
 * image will go, since the bcopy() calls in setup() don't properly handle
 * address overlap.
 *
 * Revision 1.7  1992/05/16  00:26:48  clev
 * Removed flash test routines from Monitor
 *
 * Revision 1.6  1992/05/12  22:21:44  clev
 * - Add support for the monitor command "o/r" for Pan and XX.
 *
 * Revision 1.5  1992/05/11  05:58:47  schaefer
 * zero capabilities structure
 *
 * Revision 1.4  1992/05/06  22:32:58  clev
 * XX cleanup stuff:
 *
 * ../asm.S - fix clear caches in the restart routine
 *
 * ../prom2.c - handle netbooting a system image with a load point different from
 *              firmware system image
 *
 * decodebe.c & printf.c - fix reporting of bus errors and parity errors
 *
 * defs_xx.h - defined BUSERROR for the XX for conditional compiles
 *
 * fldvr.c & rom_tests.c - provide abbreviated flash test for manufacturing
 *
 * init.c - fix the clobbering of bootinfo stuff
 *
 * Revision 1.3  1992/04/18  00:00:35  clev
 * Added better support for shared memory parity errors and support for
 * EMT_ENABLE_DCACHE (and disable).  Also, added support for break enable
 * during boot and better decoding of bus errors.  Init was enhanced to
 * flash an error number in the run LED should the power-on confidence
 * test fail.  Init was also improved to initialize interrupt vectors up
 * to 0x200.
 *
 * Revision 1.2  1992/04/17  23:49:21  schaefer
 * fix typo
 * ROM monitor changes to read image capability bits from cookie PROM
 *
 * Revision 1.1  1992/04/15  02:11:41  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Modified from csc2/init.c for Pancake, Chris Shaker, September 12, 1989
 * Modified from pan/init.c for XX, Rob Clevenger, December 22, 1991
 *
 */

#include "../defs.h"
#include "../reason.h"
#include "../m68vectors.h"
#include "../i8254.h"
#include "../prom2.h"
#include "../extern.h"
#include "../stdio.h"
#include "../cpu.h"
#include "../test.h"
#include "../caps.h"

extern u_long etext;
extern char int_vecs[];
extern volatile short buserrcnt; /* Bus error count */
extern u_short flags;
extern char *edata, *end;
extern int unexp_int();

void reset_io();

struct regpair *console = (struct regpair *) &uartaddr->a; /* UART registers */
cookie_t *cookie = (cookie_t *)ADRSPC_COOKIE; /* Address of cookie */

struct mtestvals {
    long val1, val2;
    char *name;
} mtestvals[] = {
    0x0, 0x0, "data equals address",
    0x55555555, 0xaaaaaaaa, "checkerboard",
    0xaaaaaaaa, 0x55555555, "inverse checkerboard",
};
#define NUM_MTEST_VALS (sizeof(mtestvals)/sizeof(struct mtestvals))
#define STARTUP_MASK 0x00010000
#define INITIAL_CONTROL ( CONTROL_LEDS_ALL | CONTROL_PARITY_GEN | \
			 CONTROL_PARITY_CLEAR | CONTROL_RUNLAMP )
#define INITIAL_SHMEM_CONTROL (SHMEM_PARITY_ODD | SHMEM_SLOT_LEDS )
#define MMOFFSET 0x80000
#define SHMOFFSET 0x0

/*
 * This routine is invoked first. It disables all
 * timers, sets up memory mapping, exits from the
 * boot state, and sizes and configures memory. Also
 * obtains a copy of the configuration register, sets up
 * the UARTs and exception vectors, and reenables the timers.
 */

void init ()
{
    extern char *oemversion, *ciscoversion;
    extern long evec, evece;
    u_long conf;
    register int i;
    register volatile long *mptr;
    register long val1, val2, expected, readval;
    register char *cp;
    

    asm("	.globl sinit");
    asm("sinit:");
#ifdef lint
    conf = NULL;
#else
    /*
     * Intialize startup from d0, which is initialized by _start() and
     * _restart() in asm.S before entering _sinit().
     */
    asm("	movl	d0,%0" : "=g" (conf)); /* get startup flag */
#endif
    if(conf) conf = STARTUP_MASK;
    /*
     * A reset is performed in asm.S just before entering this routine.
     * Check that the control register and the shared memory control register
     * have been reset.
     * FLASHSTAT may or may not be set in the system control register.
     */
    val1 = *(volatile ulong *)ADRSPC_CONTROL;
    val1 &= CONTROL_PERR_MASK | CONTROL_TIMER_CASCADE | CONTROL_WATCHDOG |
	CONTROL_PARITY_CLEAR | CONTROL_FLASH_VPP | CONTROL_ENABLE_BERR | CONTROL_RUNLAMP |
	    CONTROL_PARITY_GEN | CONTROL_MEM_MASK | CONTROL_BOOT;
    val2 = *(volatile ulong *)ADRSPC_SHMEM_CONTROL;
    val2 &= SHMEM_PARITY_MASK | SHMEM_SLOT_RESET | SHMEM_CONFIG0 | SHMEM_CONFIG1 |
	SHMEM_PARITY_ODD | SHMEM_PERR_CLEAR;
    if( val1 || val2 ) {
	asm("    moveq #3,d0");  /* error number 3 */
	asm("    jmp lights");
    }

    asm("	.globl init_ctl_reg");
    asm("init_ctl_reg:");
    /* initialize control register and verify */
    *(volatile ulong *)ADRSPC_CONTROL = INITIAL_CONTROL; /* (odd) parity is now enabled */
    *(volatile ulong *)ADRSPC_SHMEM_CONTROL |= INITIAL_SHMEM_CONTROL;  /* enable odd parity */

    if ( (*(volatile ulong *)ADRSPC_CONTROL & 0xffff) != (INITIAL_CONTROL & 0xffff) ) {
	asm("    moveq #4,d0");  /* error number 4 */
	asm("    jmp lights");
    } 

    /*
     * Is the configuration magic value in NVRAM ok?
     */
    if (*(ushort *)config_magic_ptr == CONFREG_MAGIC)
        conf |= *(u_short *)config_ptr; /* Yes, use config value from NVRAM */
    else
	conf |= DEFAULT_CONFIG;	       /* No, use default config value */

    *(u_short *)config_ptr = conf;
    *config_magic_ptr = CONFREG_MAGIC;

    /*
     * Now initialize the UARTs themselves and print the version string
     * Appearance of the version string on the console indicates that
     * the UART is functional.
     */
    asm("	.globl init_uart");
    asm("init_uart:");
    i = ((conf & CONSOLE_MASK) >> CONSOLE_SHIFT); 
    uartaddr->a.aux2 = 0;	/* Turn off interrupts (IMR=0) */
    uartaddr->a.command = RESETMRPTR;
    uartaddr->a.mode = CHARERROR | PRTYOFF | DBITS8;
    uartaddr->a.mode = MODENORMAL | STOP16X16;
    uartaddr->a.aux1 = BRGSET1 | TIMXTAL;
    uartaddr->a.state = (RXCLOCK * UARTspeeds[i]) | (TXCLOCK * UARTspeeds[i]);
    uartaddr->a.command = STP_TXENABLE|STP_RXENABLE;
    uartaddr->b.aux2 = 0;	/* All outputs are driven by OPR */
    uartaddr->b.aux3 = CDA;	/* Set Carrier detect on Console port */

    asm("	.globl init_version");
    asm("init_version:");
    PUTSTRING(cp,(conf & OEM_MASK) ? oemversion : ciscoversion);

    /*
     * Boot State:
     *
     * In boot state (the state of the system after reset) the address
     *	0XXXXXX		reads and executes from address 1XXXXXX
     *			writes onto address 0XXXXXX,
     * in this way it is possible to initialize RAM just after reset.
     * Also, all interrupts including normally non-maskable ones are
     * disabled.
     *
     * Note that CLR instructions should not be used on RAM space in
     * boot state; this is because the CLR instructions does a read cycle
     * before writing.
     */
    if (conf & DIAG_MASK) {
	PUTSTRING(cp,"Exiting boot state - setting bus error vector...");
    }
    asm("	.globl init_exit_boot");
    asm("init_exit_boot:");
    *(volatile ulong *)ADRSPC_CONTROL |= CONTROL_BOOT;


    /*
     * Try to set the bus error vector to ignore bus errors.
     * This is the first time we touch main memory.
     */
    asm("	.globl init_bev");
    asm("init_bev:");

    /*
     * First, set up the other vectors in RAM also, since we will
     * be changing the VBR to point into RAM instead of ROM:
     */
    val1 = (long)&evece - (long)&evec;  /* calculate number of bytes to copy */
    i = (int)&evec;  /* point to source in EPROM */
    for(mptr = 0; mptr < (volatile long *)val1;)
	*mptr++ = *((volatile long *)i)++;

    /*
     * Point the bus error vector at a routine that will simply return
     * from the exception, and update the vector base register to point
     * to the new exception table in RAM:
     */
    *EVEC_BUSERR = (long)catchbe;
    asm ("movc %0, vbr" : : "r" (ADRSPC_RAM));	/* Set VBR */

    /*
     * Make sure that the bus error vector write worked:
     */
    if (*EVEC_BUSERR != (long) catchbe) {
	PUTSTRING(cp,"\r\nBad memory - unable to write low core");
	asm("    moveq #5,d0");  /* error number 5 */
	asm("    jmp lights");
    }
    if (conf & DIAG_MASK) {
	PUTSTRING(cp,"[OK]\r\n");
    }
    
    /*
     * Now run a more extensive memory test of page zero.
     * First write all zeros. Never use a clr.l instruction
     * at this point.
     *
     * 12/27/91 - Make this test better:
     * Do "data = address" test first to check for address aliasing
     * do aa/55 (checkerboard) test (causes memory data bus to alternate
     * location by location)
     * do 55/aa (inverse checkerboard) test (inverse of the above to
     * test 1's and 0's in every location)
     * also condense the error processing
     * note that this test algorithm works only if memory range is evenly
     * divisable by 8 (2 ulong's)
     */
    for(i=0; i<NUM_MTEST_VALS; i++) {
	if (conf & DIAG_MASK) {
	    PUTSTRING(cp,"Testing low memory - ");
	    PUTSTRING(cp, mtestvals[i].name);
	}
	asm("	.globl init_mtest");
	asm("init_mtest:");
	val1 = mtestvals[i].val1;
	val2 = mtestvals[i].val2;
	for( mptr = LOWCORE0; mptr < LOWCORE1; ) {
	    if(i == 0) {  /* test 0 is "data = address" test */
		*mptr++ = (ulong)mptr;
	    } else {
		*mptr++ = val1;
		*mptr++ = val2;
	    }
	}
	for( mptr = LOWCORE0; mptr < LOWCORE1; mptr++) {
	    if( i == 0 ) expected = (ulong)mptr;
	    /* test 0 is "data = address" test */
	    else {
		if((ulong)mptr & 4) expected = val2;
		else expected = val1;
	    }
	    readval = *mptr;
	    if( readval != expected ) {
		PUTSTRING(cp,"\r\nBad memory at location 0x");
		PUTHEXNUM((ulong)mptr);
		PUTSTRING(cp,": wrote 0x");
		PUTHEXNUM(expected);
		PUTSTRING(cp,", read 0x");
		PUTHEXNUM(readval);
		asm("    moveq #6,d0");  /* error number 6 */
		asm("    jmp lights");
	    }
	}
	if (conf & DIAG_MASK) {
	    PUTSTRING(cp, "...[OK]\r\n");
	}
    }

    /*
     * Size memory and initialize system control register.
     * XX control register comes up from reset set at 16mb.
     * The original XX only supports 4 and 16 megs-o-ram
     * New cost reduced XX supports 4, 8, 16 & 32mb, with a jumper
     * to indicate if the 8 and 32mb options are of the 2 bank
     * variety
     */
    if (conf & DIAG_MASK) {
	PUTSTRING(cp,"Sizing memory...");	
    }
    asm("     .globl init_sizing_memory");
    asm("init_sizing_memory:");
    val1 = CONTROL_16MEG_MEM;  /* initialize */
    val2 = 0x1000000;  /* temporary "memsize" 16mb */
    i = 0xfeedface;
    *(volatile long *)MMOFFSET = i;  /* write page 0 */
    *(volatile long *)(MMOFFSET + 4) = 0xffffffff;  /* slam the bus high */
    if(*(volatile long *)MMOFFSET != i) {  /* read page 0 back */
        PUTSTRING(cp, "Failed sizing main memory\n");
        asm("    moveq #7,d0");  /* error number 7 */
        asm("    jmp lights");
    }
    i = 0xcafebabe;  /* unique value */
    *(volatile long *)(0x400000 + MMOFFSET) = i;  /* write 4m page */
    /* protect against an 8m single bank SIMM in original (rev 0xa0) hardware */
    *(volatile long *)(0x800000 + MMOFFSET) = i;  /* write 8m page */
    if(*(volatile long *)MMOFFSET == i) {   /* read 0 page */
	/* clobbered page 0 - we have 4mb of memory */
        val1 = CONTROL_4MEG_MEM;
        val2 = 0x400000;  /* temporary "memsize" */
    }
    
    /*
    ** New for the cost reduced XX
    ** If there is 16mb of memory at this point
    ** see if expansion RAM is present
    ** This can only be 16mb more for a total of 32mb
    ** The old hardware will always bus error in the expansion
    ** memory space so don't even check
    ** If there is 4mb, see if there is 8mb
    */
    if((*(unsigned short *)ADRSPC_CONTROL & 0xff) != 0xa0) {
	/* later revision hardware */
	i = 0xdeadbeef;  /* another unique value */
	if(val2 == 0x400000) {  /* so far 4mb reported */
	    /* see if we have eight */
	    /* set for 8m so we don't bus err */
	    *(volatile ulong *)ADRSPC_CONTROL |= CONTROL_8MEG_MEM;
	    *(volatile long *)(0x400000 + MMOFFSET) = i;  /* write above 4m */
	    *(volatile long *)(0x400000) = 0xffffffff;  /* slam the bus high */
	    if(*(volatile long *)(0x400000 + MMOFFSET) == i) {   /* read back */
		/* memory is valid - we have 8mb of memory */
		val1 = CONTROL_8MEG_MEM;   /* 8mb */
		val2 = 0x800000;  /* temporary "memsize" */
	    }
	} else if(val2 == 0x1000000) {  /* 16mb */
	    /* set for 32m so we don't bus err */
	    *(volatile ulong *)ADRSPC_CONTROL |= CONTROL_32MEG_MEM;
	    *(volatile long *)(ADRSPC_EXP_RAM + MMOFFSET) = i;
	    *(volatile long *)(ADRSPC_EXP_RAM) = 0xffffffff;  /* slam the bus high */
	    if(*(volatile long *)(ADRSPC_EXP_RAM + MMOFFSET) == i) {
		/* looks like we have memory here */
		val1 = CONTROL_32MEG_MEM;  /* 32mb */
		val2 = 0x2000000;  /* 32mb */
	    }
	}
	
	/*
	** Clear the memsize bits in the control register
	*/
	
	*(volatile ulong *)ADRSPC_CONTROL = (*(volatile ulong *)ADRSPC_CONTROL & ~CONTROL_MEM_MASK);
    }
    *(volatile ulong *)ADRSPC_CONTROL |= val1;  /* write size info to sys control register */
    
    /*
     * Write all of memory outside page 0
     * in order to initialize parity bits.
     * This can't be a "clr", since that instruction reads(!)
     * before writing.  The use of 0xFFFFFFFF means that random
     * jumps will immediately cause Line 1111 Emulator traps.
     */
    
    /*
     * Copy exception table from data to bss segment.
     */
    if (conf & DIAG_MASK)
	printf("%#x bytes...[OK]\nSetting up exception vectors...",val2);
    asm("	.globl init_vectors");
    asm("init_vectors:");
    val1 = (long)&evece - (long)&evec;  /* calculate number of bytes to copy */
    i = (int)&evec;  /* point to source in EPROM */
    for(mptr = 0; mptr < (volatile long *)val1;)
	*mptr++ = *((volatile long *)i)++;
    
    /*
    ** Initialize up to &edata (0x200) with valid vectors in case NIMs cause
    ** unexpected interrupt.
    ** Some of these will be overwritten by system code when booted.
    */
    while(mptr < (volatile long *)&edata)
	*mptr++ = (long)unexp_int;

    /*
     * Copy over ourselves to reset parity.
     * First zero our .bss section.
     * Mptr equals &edata at this point.
     */
    if (conf & DIAG_MASK)
	printf("[OK]\nClearing monitor .bss area (%#x to %#x)...",&edata,&end);
    i = 0;  /* To avoid clr.x instruction */
    for(; mptr < (volatile long *)&end; *mptr++ = i);

    memsize = val2;  /* update global memsize w/real value */
    buserrhandler = 0;		/* Report bus errors */
    *(volatile ulong *)ADRSPC_CONTROL |= CONTROL_ENABLE_BERR;

    if (conf & STARTUP_MASK) {
	if (conf & DIAG_MASK) puts("[OK]\nClearing the bootstuff area...");
	/* Void the bootstuff area (reason for last reboot) */
	for(mptr = LOWCORE1;
	    mptr < (volatile long *)((long)LOWCORE1 + sizeof(bootstuff));
	    *mptr++ = i);
    }

    /*
    ** Initialize the rest of main memory with ff's
    ** New for the cost reduced XX, the possibility of 32mb
    ** in two 16mb segments
    */
    if (conf & DIAG_MASK)
	puts("[OK]\nSetting memory to all ones...");

    asm("	.globl init_parity1");
    asm("init_parity1:");

    i = 0xFFFFFFFF;  /* two "unimplemented 1111's" */
    mptr = (volatile long *)((long)LOWCORE1 + sizeof(bootstuff));
    if(val2 > 0x1000000) val1 = 0x1000000;  /* val2 is "memsize" */
    else val1 = val2;
    while(mptr < (volatile long *)val1) *mptr++ = i;
    /*
    ** Initialize the expansion RAM if present.
    */
    if(val2 > 0x1000000) {  /* val2 is "memsize" */
	val1 = ADRSPC_EXP_RAM + 0x1000000;  /* the end of expansion RAM */
	mptr = (volatile long *)ADRSPC_EXP_RAM;
	while(mptr < (volatile long *)val1) *mptr++ = i;
    }
    
    if (conf & DIAG_MASK)
	puts("[OK]\nTesting main memory parity...");
    
    asm("	.globl init_parity0");
    asm("init_parity0:");
    
    if(val2 > 0x1000000) memsize = 0x1000000;  /* fudge this for the parity test */
    else memsize = val2;
    if(parity_test(0,1) != 1) {
	puts("failed main memory parity test ... aborting\n");
	asm("    moveq #8,d0");  /* error number 8 */
	asm("    jmp lights");
    }
    memsize = val2;  /* update global memsize w/real value (again) */
    
    /*
     * Which processor are we? Delay() needs this.
     */
    asm("	.globl init_cpu_type");
    asm("init_cpu_type:");
    cpu_type = CPU_XX;
    ciscopro_flag = 0;
    if (conf & DIAG_MASK)
	printf("[OK]\nProcessor type is: %s...", cputypes[cpu_type]);
    
    /*
     * Initialize the counter/timers:
     */
    if (conf & DIAG_MASK)
	puts("[OK]\nEnabling timers...");
    asm("	.globl init_timers");
    asm("init_timers:");
    /*
     * Initialize Counter 2 for our 4 ms real time clock interrupt:
     */
    brk = 0;			/* no breaks */
    abort = FALSE;		/* no abort state */
    abortstate = ABORT_IGNORE;	/* aborts are internal */
    clock = 0;			/* zero the ms clock */
    hclock = 0;			/* zero the upper ms clock */
    ctaddr->control = SEL_COUNTER2 | RW_LSBMSB | CT_MODE2;
    ctaddr->counter2 = REFRFREQUENCY & 0xFF;
    ctaddr->counter2 = (REFRFREQUENCY >> 8) & 0xFF;

    /*
     * Initialize Counter 0 as watchdog -- 4 times refresh rate
     */
    ctaddr->control = SEL_COUNTER0 | RW_LSBMSB | CT_MODE0;
    ctaddr->counter0 = (REFRFREQUENCY*4) & 0xFF;
    ctaddr->counter0 = ((REFRFREQUENCY*4) >> 8) & 0xFF;

    /*
     * The following write clears any existing IRQ from the i8254
     * so that the timers will start interrupting us:
     */
    *(volatile u_char *)ADRSPC_TCONTROL = 0;
    
    /*
     * Perform a quick go/no go test on the 4 ms real time clock:
     */
    asm("	.globl init_tclock");
    asm("init_tclock:");
    delay(4);			/* Wait at least 4 ms */
    if (clock) {
	if (conf & DIAG_MASK)
	    puts("[OK]\n");
    } else {
	puts("real time clock FAILED to increment\n");
	asm("    moveq #9,d0");  /* error number 9 */
	asm("    jmp lights");
    }

    /*
     * Enable or disable the watchdog timer:
     * Initialize the counter timers *BEFORE* doing this. Otherwise, we are
     * vulnerable to a momentary glitch in WE (watchdog enable) which will
     * reset the box if the 8254 comes up in a funny state with Counter 0
     * out already asserted.
     */
    if (conf & WATCHDOG_MASK) {
	if(conf & DIAG_MASK)
	    puts("Watchdog timer disabled...");
    } else {
	if(conf & DIAG_MASK)
	    puts("Enabling Watchdog timer...");
	*(volatile u_char *)ADRSPC_TCONTROL |= TCONTROL_WE;
    }

    /*
     * Size NVRAM via Bus error detection. We don't want to size this memory via
     * detecting on a write, because we don't want to be left holding the bag
     * if we lose power before restoring the NVRAM location...
     * Note: NVRAM is non-cacheable.
     */
    if(conf & DIAG_MASK) puts("[OK]\nSizing NVRAM...");
    asm("	.globl init_size_nvram");
    asm("init_size_nvram:");
    buserrcnt = 0;		/* No bus error yet */
    buserrhandler = buserrnote; /* Catch and count bus errors */
    for (cp = (char *)ADRSPC_NVRAM;
	 (cp < (char *)ADRSPC_NVRAM_END) && (buserrcnt == 0); cp += 1024)
	i = *(volatile u_char *)cp;
    nvram_size = (uint)(cp - ADRSPC_NVRAM);

    buserrhandler = 0;		/* Report bus errors again */

    /*
     * Size shared memory and initialize shmem control register.
     * XX shmem control register comes up from reset set at 16mb.
     * XX only supports 1, 4, and 16 megs-o-shmem
     * Note: Shared memory is non-cacheable.
     */
    if (conf & DIAG_MASK)
	printf("%#x bytes...[OK]\nSizing shared memory...", nvram_size);
    asm("	.globl init_sizing_shmem");
    asm("init_sizing_shmem:");
    /*
    ** unreset the NIMs
    ** this is necessary to access shared memory
    */
    reset_io();

    val1 = SHMEM_16MEG;  /* initialize */
    val2 = 0x1000000;  /* temporary "shmemsize" */
    i = 0xdeadbeef;  /* unique value */
    *(volatile long *)(ADRSPC_SHAREDMEM + SHMOFFSET) = i;
    *(volatile long *)(ADRSPC_SHAREDMEM + 0x100000 + SHMOFFSET) = 0xfeedface;  /* other unique value */
    if( *(volatile long *)(ADRSPC_SHAREDMEM + SHMOFFSET) != i ) {  /* must only be 1 meg-o-ram */
	val1 = SHMEM_1MEG;
	val2 = 0x100000;  /* temporary "shmemsize" */
    } else {
	*(volatile long *)(ADRSPC_SHAREDMEM + 0x400000 + SHMOFFSET) =  0xfeedface;  /* other unique value */
	if( *(volatile long *)(ADRSPC_SHAREDMEM + SHMOFFSET) != i ) {  /* must be 4 meg-o-ram */
	    val1 = SHMEM_4MEG;
	    val2 = 0x400000;  /* temporary "shmemsize" */
	}
    }
    *(volatile ulong *)ADRSPC_SHMEM_CONTROL |= val1;  /* write size info to shmem control register */
    shmemsize = val2;
    if (conf & DIAG_MASK)
	printf("%#x bytes...[OK]\nTesting shared memory parity...",val2);
    if(parity_test(ADRSPC_SHAREDMEM,1) != 1) {
	puts("failed shared memory parity test ... aborting\n");
	asm("    moveq #10,d0");  /* error number 10 */
	asm("    jmp lights");
    }
    /*
     * Initialize shared memory for parity
     */
    if (conf & DIAG_MASK)
	puts("[OK]\nInitializing shared memory...");
    i = 0xa5a5a5a5;  /* something conspicuous and will buserr if used as pointer */
    val2 += ADRSPC_SHAREDMEM;  /* val2 is "shmemsize", make "shmem end" */
    for(mptr = (volatile long *)ADRSPC_SHAREDMEM; mptr < (volatile long *)val2;)
	*mptr++ = i;

    /*
     * Enable the processor caches, now that we've tested memory:
     */
    if (conf & DIAG_MASK)
	puts("[OK]\nEnabling processor caches - ");
    asm("	.globl init_enable_caches");
    asm("init_enable_caches:");

    setacu(0,0x02FD8507);  /* Disable any page with bit 1 set */
    setacu(1,0x08008507);  /* Disable NIM address space       */

    cacheon();		/* Enable both caches */
    cache = TRUE;		/* cache default to on */
    if (conf & DIAG_MASK)
	printf("set CACR to %#x...", ENABLE_CACHES);
    
    /*
     * Determine if a system image is present by searching for 'magic'
     * patterns in ROM address space after the end of the ROM monitor.
     * Search for the start of the system image until:
     *  o We find a system image, set prom2 to point to it, or
     *	o We encounter a bus error, or
     *	o We reach the end of the 16 MB ROM address region
     */
    if (conf & DIAG_MASK) puts("[OK]\nSearching for system code...");
    asm("	.globl init_find_system");
    asm("init_find_system:");
    buserrcnt = 0;		/* No bus error yet */
    buserrhandler = buserrnote; /* Catch and count bus errors */
    prom2 = (u_short *)NULL;
    baseaddr = (long *)0x40000;	/* default net load address */
    for (mptr = (volatile long *)&etext;
	 ((mptr < (volatile long *)ADRSPC_PROM_END) && (buserrcnt == 0));
	 (u_char *)mptr += 2) {
	switch (*(volatile u_short *)mptr) {
	case RELOC_MAGIC:
	    baseaddr = (long *)((struct relochdr *)mptr)->loadaddr;
	    mptr = (volatile long *)(((struct relochdr *)mptr)->size + (long)mptr);
	    /* fall through */
	case PROM2_MAGIC1:
	case PROM2_MAGIC2:
	case PROM2_MAGIC3:
	    prom2 = (u_short *)mptr;
	    break;
	}
	if(prom2) break;
    }
    buserrhandler = 0;		/* Report bus errors again */
    if (conf & DIAG_MASK)
	printf("found at %#x (magic = %#x, load addr = %#x)...[OK]\nSetting up monitor variables...[OK]\n",
	       prom2, *(u_short *)mptr, baseaddr);

    /*
     * Variable initializations
     */
    asm("	.globl init_variables");
    asm("init_variables:");

    iomem_base = ADRSPC_SHAREDMEM;
    *(u_short *)&config = ~conf;/* config register at startup */
    echoing = TRUE;		/* we want to echo characters */
    breakf = FALSE;		/* not inst. break state */
    breakaddr = 0;		/* first value */
    breakvalue = 0;		/* to be consistant with above */
    lastloc = 0;		/* last dep/ex. location */
    lastsize = 'L';		/* last dep/ex. size */
    lastswap = FALSE;		/* last dep/ex. byte swap action */

    vsr_set = vstring_sys_rom[0] = vstring_sys[0] = '\0';

    /*
     * Read the hardware revision register:
     */
    if (conf & DIAG_MASK)
	puts("Reading hardware revision register...[OK]");
    asm("	.globl init_rev");
    asm("init_rev:");

    /*
     * Get the hardware revision number:
     */
    revision = (*(volatile ulong *)ADRSPC_CONTROL >> 16);
    if (conf & DIAG_MASK)
	printf("\n%s processor, hardware revision = %d",
	       cputypes[cpu_type], revision);

    /*
     * Save information from restart() in bootstuff[]:
     */
    asm("	.globl init_bootstuff");
    asm("init_bootstuff:");
    mptr = LOWCORE1;
    bootstuff[BOOT_COUNT] = BOOTDATA;	/* Length of bootstuff */
    for (i = BOOT_REASON; i <= BOOTDATA; i++)
	bootstuff[i] = *mptr++;		/* Copy into bootstuff */
    
    /*
     * Clear out the cross-boot information area
     */

    bzero(boot_information, BOOTINFOSIZE);

    /*
     * Try to generate a possible parity error:
     */
    asm("	.globl init_test_parity");
    asm("init_test_parity:");
    for (i = 0; i < BUFSIZ; i++) {
	bufu.buf[i] = (char)i;
	readval = bufu.buf[i];
    }

    bzero((char*) &caps, sizeof(caps));
    caps.version = SYSCAP_VERSION;
    caps.length = SYSCAP_LENGTH;

    if (conf & DIAG_MASK)
	puts("\nExiting to monitor command level\n\n");

    /*
     * Return
     */
    asm("	.globl initc");
    asm("	jmp initc");
}

/*
** Reset all IO devices (NIMs) to a quiescent state:
** The final delay is to allow shared memory to stabilize and
** is mandatory.
*/

void reset_io ()
{
    *(volatile ulong *)ADRSPC_SHMEM_CONTROL &= ~SHMEM_SLOT_RESET;
    delay(5);			/* TMS380 needs 5 ms */
    *(volatile ulong *)ADRSPC_SHMEM_CONTROL |= SHMEM_SLOT_RESET;
    delay(5); /* shared memory needs this */
}

/*
** Reset the configuration register to default value
*/
void reset_confreg ()
{
    *(u_short *)config_ptr = DEFAULT_CONFIG | IGNORE_CONFIG;
}

/*
 * write_bad_main_memory_parity
 * Write the given data to the specified location in main memory with
 * incorrect parity, so that reading from this location will cause a
 * parity error. Do this from ROM so that we don't have to worry about
 * opcode fetches while the main memory parity setting is set wrong:
 */

int write_bad_main_memory_parity (data, location)
    uint data;
    uint *location;
{
    register uint  *reg_dptr;
    register uint  reg_d;
    volatile uint  *control_ptr;
    uint           old_cacr;
    ushort         level;

    /*
     * Disable all maskable interrupts during this code:
     */
    level = sil(7);

    /*
     * Set CACR to disable instruction and data caches while we are
     * in this subroutine:
     */
    old_cacr = setcacr(CLEAR_CACHES);

    /*
     * Get data pointer and data in registers so that we don't
     * reference the stack while parity is set wrong:
     */
    reg_dptr = location;
    reg_d = data;

    /*
     * Write data to location with odd parity to make compiler
     * put pointer and data into registers:
     */
    *reg_dptr = reg_d;
    *reg_dptr = ~reg_d;

    /*
     * Set control register for even parity in main memory
     * instead of the usual odd parity:
     */
    control_ptr = (volatile uint *)ADRSPC_CONTROL;
    *control_ptr &= ~CONTROL_PARITY_GEN;

    /*
     * Write data to location with even parity using the pointer
     * and data squirreled away in registers:
     */
    *reg_dptr = reg_d;

    /*
     * Set control register back to usual odd parity in main memory
     * to generate and check odd parity before returning:
     */
    *control_ptr |= CONTROL_PARITY_GEN;

    /*
     * Let write to control register happen while we execute code from ROM:
     */
    asm("	nop");

    /*
     * Restore the cache states:
     */
    setcacr(old_cacr);

    /*
     * Restore the previous interrupt level:
     */
    (void)sil(level);

    return(0);
}
