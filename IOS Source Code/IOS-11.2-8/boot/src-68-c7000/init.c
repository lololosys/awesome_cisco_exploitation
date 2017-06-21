/* $Id: init.c,v 3.2 1995/12/03 17:55:30 thille Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/init.c,v $
 *------------------------------------------------------------------
 * init.c -- system hardware initialization
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: init.c,v $
 * Revision 3.2  1995/12/03  17:55:30  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.1  1995/11/09  09:10:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/12  17:30:21  sthormod
 * CSCdi35688:  some RPs hang in the rom monitor after a power cycle
 * Invalidate caches.
 *
 * Revision 2.2  1995/06/12  17:08:12  sthormod
 * CSCdi35550:  64MB RP shows up as 16MB with 11.0 boot ROMs
 * Use volatiles so that the compiler doesn't optimize away the read
 * back.  Also check for old RPs that don't have the 64MB address decode
 * PAL.
 *
 * Revision 2.1  1995/06/07  19:17:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.7  1995/06/07 09:05:11  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.5.18.2  1995/05/05  21:39:41  vandys
 * Branch: Arizona_branch
 * Update Arizona_branch to Arizona_baseline_950504
 *
 * Revision 1.5.18.1  1995/05/02  23:22:31  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.6  1995/05/03  21:29:55  sthormod
 * CSCdi30508:  ROM monitor reset_io does not work on the C7000
 * Do a dbus reset to every occupied slot.
 *
 * Revision 1.5  1995/03/01  02:47:02  sthormod
 * CSCdi30241:  64MB RP takes too long to boot
 * Speed up memory initialization.  Now the bulk of the time is spent
 * checksumming ROM (~6 seconds for 4MB of ROM).
 *
 * Revision 1.4  1994/10/14  00:20:22  sthormod
 * CSCdi25683:  high-end 10.2 rom monitor broken
 *              The 94q2 compiler uses "moveb" instead of "clrb" in
 *              plogic_test().  This causes a parity error.
 *
 * Revision 1.3  1994/02/18  22:20:18  sthormod
 * CSCdi16289:  16mb of memory not enough for proper operation of the
 * router
 * Change memory sizing to allow for either 16 or 64mb on RP1.
 *
 * Revision 1.2  1993/12/16  12:05:39  sthormod
 * ROM monitor support for nvram config compress & s/w config register.
 *
 * Revision 1.1  1993/09/24  16:34:25  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.3  1993/08/19  03:51:34  hampton
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
 * Revision 1.2  1993/02/28  22:59:56  schaefer
 * Chassis MIB
 *
 * Revision 1.1  1993/01/30  01:11:52  whp4
 * Add RP1 support.
 *
 *
 * $Endlog$
 */

#include "../defs.h"
#include "../reason.h"
#include "../m68vectors.h"
#include "../ctype.h"
#include "../i8254.h"
#include "../prom2.h"
#include "../extern.h"
#include "../stdio.h"
#include "../cpu.h"
#include "../test.h"
#include "../caps.h"

extern volatile int memory_test_cell;

extern u_long etext;
extern char int_vecs[];

extern ulong _end;

#define MEMTEST_MAGIC -559038737
#define STACK_MAGIC   -1161905474

struct regpair *console = (struct regpair *) &uartaddr->a; /* UART registers */
volatile u_short controlreg_snapshot; /* temporary sys. cntrl reg. */

/*
 * This routine is invoked first. It disables all
 * timers, sets up memory mapping, exits from the
 * boot state, and sizes and configures memory. Also
 * obtains a copy of the configuration register, sets up
 * the UARTs and exception vectors, and reenables the timers.
 */

init ()
{
    register long startup;	/* MAGIC - see asm code below */
    register long i, j;
    register u_short conf;	/* temporary config register */
    register long *et;
    register long msize;
    register long adrspc_prom1; /* start of bank 1 */
    register char *cp;
    register u_char processor_version;
    extern long evec[], evece;
    extern char *oemversion, *ciscoversion;
    volatile ushort *env_control;

#ifdef lint
    startup = NULL;
#else
    asm("	.globl sinit");
    asm("sinit:");
    /*
     * Intialize startup from d0, which is initialized by _start() and
     * _restart() in locore.s before entering _sinit().
     */
    asm("	movl	d0,%0" : "=g" (startup)); /* Initialize startup */
#endif
    
    /*
     * Light lamps and reset MB
     */
    *(volatile u_short *)ADRSPC_CONTROL |=
	(CONTROL_LAMP|CONTROL_RUNLAMP|CONTROL_MBRESET|CONTROL_MSIZE_64MB);

    /*
     * But remember to unreset MB
     */
    *(volatile u_short *)ADRSPC_CONTROL &= ~CONTROL_MBRESET;

    /*
     * Save a copy of the config register
     */
    cp = (char *)config_magic_ptr;   /* byte access nvram config-reg */
    conf = (*cp++ << 8);
    conf |= *cp;
    if (conf == CONFREG_MAGIC) {
	cp = (char *)config_ptr;    
	conf = (*cp++ << 8);
	conf |= *cp;
    }
    else {
	conf = DEFAULT_CONFIG;      
    }

    cp = (char *)config_ptr;        /* write back conf */
    *cp++ = (char)(conf >> 8);
    *cp = (char)conf;

    cp = (char *)config_magic_ptr;  /* write back config-reg magic */
    *cp++ = (char)(CONFREG_MAGIC >> 8);
    *cp = (char)CONFREG_MAGIC;

    conf = ~conf;
    
    /*
     * Now initialize the UARTs themselves and print version string
     * Appearance of the version string on the console indicates that
     * the UART and timer chips are functional.
     */
    i = ((~conf & CONSOLE_MASK) >> CONSOLE_SHIFT); 
    uartaddr->a.ccr = CCR_TCMD | CCR_DISABLE;
    uartaddr->a.ccr = CCR_RCMD | CCR_DISABLE;
    uartaddr->a.cmr1 = CMR1_ASYNC;
    uartaddr->a.tpr = TPR_2_STOP_BITS | TPR_8_DATA_BITS;
    uartaddr->a.ttr = TTR_CLOCKBRG | UARTspeeds[i];
    uartaddr->a.rpr = RPR_8_DATA_BITS;
    uartaddr->a.rtr = RTR_CLOCKBRG | UARTspeeds[i];
    /*
     * Funny initialization dance to avoid going
     * into a non-resetable break state.
     */
    uartaddr->a.cmr2 = CMR2_LOOP | CMR2_POLLED;
    uartaddr->a.ccr = CCR_TCMD | CCR_ENABLE;
    uartaddr->a.ccr = CCR_TCMD | CCR_TBREAK;
    for (i = 0; i < 1000000; i++)
	if (uartaddr->a.trsr & TRSR_BREAK_ACK)
	    break;
    if (i >= 1000000)
	asm("    jmp lights");
    uartaddr->a.ccr = CCR_TCMD | CCR_DISABLE;
    uartaddr->a.cmr2 = CMR2_POLLED;
    uartaddr->a.omr = 2;		/* raise bit 1 - DTR */
    uartaddr->a.ccr = CCR_TCMD | CCR_RESET;
    uartaddr->a.ccr = CCR_RCMD | CCR_RESET;
    uartaddr->a.ccr = CCR_TCMD | CCR_ENABLE;
    uartaddr->a.ccr = CCR_RCMD | CCR_ENABLE;
    asm("	.globl init_version");
    asm("init_version:");
    PUTSTRING(cp,(~conf & OEM_MASK) ? oemversion : ciscoversion);
    
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
    
    if (~conf & DIAG_MASK) {
	PUTSTRING(cp,"Exiting boot state - setting bus error vector..");
    }
    asm("	.globl init_exit_boot");
    asm("init_exit_boot:");
    *(volatile u_short *)ADRSPC_CONTROL &= ~CONTROL_BOOT;
    
    /*
     * Try to set the bus error vector to ignore bus errors.
     * This is the first time we touch main memory.
     */
    *EVEC_BUSERR = (long)catchbe;

    asm ("movc %0, vbr" : : "r" (ADRSPC_RAM));        /* Set VBR */

    if (*EVEC_BUSERR != (long) catchbe) {
	PUTSTRING(cp,"\r\nBad memory - unable to write low core");
    }
    if (~conf & DIAG_MASK) {
	PUTSTRING(cp,"[OK]\r\n");
    }

    /*
     * Now run a more extensive memory test of page zero.
     * Do this only on a hardware reset/powerup.
     * First write all zeros. Never use a clr.l instruction
     * at this point.
     */
    if (~conf & DIAG_MASK) {
	PUTSTRING(cp,"Testing low memory...0's...");
    }
    asm("	.globl init_low_mem_test_zeros");
    asm("init_low_mem_test_zeros:");
    i = 0;	/*hope compiler stays dumb*/
    for (et = LOWCORE0; et < LOWCORE1; et++)
	*et = i;
    for (et = LOWCORE0; et < LOWCORE1; et++) {
	i = *et;
	if (i != 0) {
	    PUTSTRING(cp,"\r\nBad memory - wrote zero, read 0x");
	    PUTHEXNUM(i);
	    PUTSTRING(cp," at location 0x");
	    PUTHEXNUM((unsigned)et);
	}
    }
    
    /*
     * Test all ones
     */
    if (~conf & DIAG_MASK) {
	PUTSTRING(cp,"1's...");
    }
    asm("	.globl init_low_mem_test_ones");
    asm("init_low_mem_test_ones:");
    /*
     * According to lister, if we do "*et = 0xFFFFFFFF" the compiler
     * will reload -1 for every iteration of the loop.  But if we 
     * assign it to a variable, then the compiler keeps the value in 
     * a register.
     */
    j = 0xFFFFFFFF;  
    for (et = LOWCORE0; et < LOWCORE1; et++)
	*et = j;     
    for (et = LOWCORE0; et < LOWCORE1; et++) {
	i = *et;
	if (i != j) {
	    PUTSTRING(cp,"\r\nBad memory - wrote all ones, read 0x");
	    PUTHEXNUM(i);
	    PUTSTRING(cp," at location 0x");
	    PUTHEXNUM((unsigned)et);
	}
    }
    
    /*
     * Test address patterns
     */
    if (~conf & DIAG_MASK) {
	PUTSTRING(cp,"address+1...");
    }
    asm("	.globl init_low_mem_test_address");
    asm("init_low_mem_test_address:");
    for (et = LOWCORE0; et < LOWCORE1; et++)
	*et = (long) (et + 1);
    for (et = LOWCORE0; et < LOWCORE1; et++) {
	j = (long) (et + 1);
	i = *et;
	if (i != (long)j) {
	    PUTSTRING(cp,"\r\nBad memory - wrote 0x");
	    PUTHEXNUM(j);
	    PUTSTRING(cp,", read 0x");
	    PUTHEXNUM(i);
	    PUTSTRING(cp," at location 0x");
	    PUTHEXNUM((unsigned)et);
	}
    }
    
    /*
     * Size and initialize memory on powerup or hard reset.
     * Note we are using the stack for the first time.
     */
    if (~conf & DIAG_MASK) {
	PUTSTRING(cp,"[OK]\r\nSizing memory...");	
    }
    asm("	.globl init_sizing_memory");
    asm("init_sizing_memory:");

    /* 
     * Test for 16 or 64 megabyte RP.  
     *
     * Try writing a known value to &mem_test_cell+16MB.  If we are
     * on a 16MB RP, then the write will wrap around and clobber the
     * zero written to mem_test_cell.  
     * 
     * Some older RPs don't have the updated 64MB address decoding
     * PAL.  In those cases, the write will cause a bus error instead
     * of wrapping around.  So, we must first check for older RPs by
     * looking for a bus error, otherwise check for a clobber.
     *
     * Using volatiles is key here.  Without it the compiler will
     * often optimize away the operation altogether.  Double check
     * the assembly listing.
     */

#define CELL_PLUS16MB ((volatile long *)((uchar *)&memory_test_cell + MSIZE_16MB))

    memory_test_cell = 0;

    abortstate = ABORT_INTERNAL;
    *EVEC_BUSERR = evec[2];   
    buserrhandler = buserrnote;
    buserrcnt = 0;
    *CELL_PLUS16MB = MEMTEST_MAGIC;   /* this may cause a bus error */
    *EVEC_BUSERR = (long)catchbe;    

    if (buserrcnt) {
	/*
	 * RP without updated address decode PAL.
	 */
	if (~conf & DIAG_MASK) {
	    PUTSTRING(cp,"touch, setting to 16MB...");
	}
	*(volatile u_short *)ADRSPC_CONTROL &= ~CONTROL_MSIZE_64MB;
    } else {
	if (memory_test_cell == 0 && *CELL_PLUS16MB == MEMTEST_MAGIC) { 
	    if (~conf & DIAG_MASK) {
		PUTSTRING(cp, "no wrap, setting to 64MB...");
	    }
	    /* already set to 64mb, no need to reset it */
	} else {
	    if (~conf & DIAG_MASK) {
		PUTSTRING(cp, "wrap, setting to 16MB...");
	    }
	    *(volatile u_short *)ADRSPC_CONTROL &= ~CONTROL_MSIZE_64MB;
	}
    }
    
    if (~conf & DIAG_MASK) 
	PUTSTRING(cp, "[OK]\r\nPage check...");
    /* 
     * Now sanity check things by calling meminit().  This is also the
     * 1st function call.
     */
    msize = meminit(~conf & DIAG_MASK) << PAGESHIFT;

    /* 
     * Now that we have done some memory testing, let's enable 
     * instruction caching before we initialize the memory.  
     * 
     * The variable "cpu_type" hasn't been set yet, so we'll do the
     * equivalent of setcacr(ICACHE_040) in assembly.   
     *
     * Do NOT enable data caching.
     */
    if (~conf & DIAG_MASK)
 	PUTSTRING(cp,"[OK]\r\nInvalidating caches...");
    asm(".word 0xf4d8");      /* cinva bc */

    if (~conf & DIAG_MASK)
 	PUTSTRING(cp,"[OK]\r\nEnabling instruction caching...");
    asm("movc %0, cacr" : : "r" (ICACHE_040));
    
    /*
     * Write all of memory outside page 0
     * in order to initialize parity bits.
     * This can't be a "clr", since that instruction reads(!)
     * before writing.  The use of 0xFFFFFFFF means that random
     * jumps will immediately cause Line 1111 Emulator traps.
     */
    if (~conf & DIAG_MASK)
	puts("[OK]\nSetting memory to all ones...");
    asm("	.globl init_initializing_memory");
    asm("init_initializing_memory:");
    /*
     * Copy over ourselves to reset parity
     */
    if (startup) {
	/*
	 * To avoid clr.x instruction,
	 * need stupid compiler!
	 */
	i = 0;
	for (et = LOWCORE1; et < LOWCORE1 + sizeof(bootstuff); et++)
	    *et = i;
    }

    i = 0xFFFFFFFF;  /* two "unimplemented 1111's" */
    for (et = LOWCORE1 + sizeof(bootstuff); et < (long *)msize; et++)
	*et = i;

    if (~conf & DIAG_MASK)
	puts("[OK]\n");
    
    /*
     * Copy exception table from data to bss segment.
     */
    if (~conf & DIAG_MASK)
	puts("Setting up exception vectors...");
    asm("	.globl init_vectors");
    asm("init_vectors:");
    buserrhandler = 0;		/* Report bus errors */
    for (et = &evec[0]; et < &evece; et++)
	*(long *)((u_long)et - (u_long)&evec[0]) = *et;

    /*
     * Set the CPU type from the processor version number
     * so that delay() can use it:
     */
    processor_version = sys_status_reg->version;
    switch(processor_version) {
      case VER_RP1:
	cpu_type = CPU_RP1;
	break;
      default:
	cpu_type = 0;
    }
    ciscopro_flag = 0;

    if (~conf & DIAG_MASK)
	puts("[OK]\nEnabling timers...");
    asm("	.globl init_start_timers");
    asm("init_start_timers:");

    /*
     * Initialize the env_status register & initialize a couple variables
     */
    env_control = (ushort *) ADRSPC_ENV_CTL;
    *env_control = (ushort) 0xff11; /* to initialize and ramp up ADC */
    delay(10);   /* let the value ramp up */
    pf_info.pf_addr_save = 0; /* Initialize to zero */
    pf_info.pf_addr = 0; /* Initialize to zero; only sys. code can */
                          /* set this to the proper value */
    pf_info.intermittent_pf_count = 0; /* Initialize to NO intermittents */

    /*
     * Initialize Counter 2 for our 4 ms real time clock interrupt:
     */
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
    asm("	.globl init_test_clock");
    asm("init_test_clock:");
    delay(4);			/* Wait at least 4 ms */
    if (clock) {
	if (~conf & DIAG_MASK)
	    puts("[OK]\n");
    } else
	printf("real time clock FAILED to increment\n");

    /*
     * Enable or disable the watchdog timer:
     * Initialize the counter timers *BEFORE* doing this. Otherwise, we are
     * vulnerable to a momentary glitch in WE (watchdog enable) which will
     * reset the box if the 8254 comes up in a funny state with Counter 0
     * out already asserted.
     */
    if (~conf & WATCHDOG_MASK) {
	if (~conf & DIAG_MASK)
	    puts("Watchdog timer disabled...");
    } else {
	if (~conf & DIAG_MASK)
	    puts("Enabling Watchdog timer...");
	*(volatile u_char *)ADRSPC_TCONTROL |= TCONTROL_WE;
    }

    /*
     * Test Memory Parity Circuitry
     */
    asm("	.globl test_mem_parity");
    asm("test_mem_parity:");
    plogic_test(conf);

    /*
     * Determine if the second prom set is present. Reading
     * from the second prom's location should yield 0x4E56, the
     * link a6, instruction. Otherwise the location should read
     * as garbage.
     */
    if (~conf & DIAG_MASK)
	puts("Searching for system code...");
    asm("	.globl init_find_system");
    asm("init_find_system:");

    /*
     * This should probably be changed back to a CONSTANT
     * seeing we support the universal PAL now...
     */
    adrspc_prom1 = ADRSPC_PROM1_8MB;
    prom2 = (u_short *)NULL;

    if (*(u_short *)adrspc_prom1 == PROM2_MAGIC2 ||
        *(u_short *)adrspc_prom1 == PROM2_MAGIC3) 
	prom2 = (u_short *)adrspc_prom1;
    else {
	for (et = (long *)&etext; et <= (long *)adrspc_prom1;
	     (u_char *)et += 2) {
	    if (*(u_short *)et == PROM2_MAGIC2 ||
		*(u_short *)et == PROM2_MAGIC3) {
		prom2 = (u_short *)et;
		break;
	    }
	}
    }

    /*
     * Variable initializations:
     */
    if (~conf & DIAG_MASK)
	puts("[OK]\nSetting up monitor variables...");
    asm("	.globl init_variables");
    asm("init_variables:");
    memsize = msize;		/* memory size */
    *(u_short *)&config = conf;	/* config register at startup */
    echoing = TRUE;		/* we want to echo characters */
    brk = 0;			/* no breaks */
    abort = FALSE;		/* no abort state */
    abortstate = ABORT_IGNORE;	/* aborts are internal */
    buserrhandler = 0;		/* routine to handle bus errors */
    breakf = FALSE;		/* not inst. break state */
    breakaddr = 0;		/* first value */
    breakvalue = 0;		/* to be consistant with above */
    lastloc = 0;		/* last dep/ex. location */
    lastsize = 'L';		/* last dep/ex. size */
    lastswap = FALSE;		/* last dep/ex. byte swap action */
    cache = TRUE;		/* cache in 68020 default to on */
    i_cache = TRUE;		/* i_cache in 68040 set to TRUE */
    d_cache = TRUE;		/* d_cache in 68040 set to TRUE */
    baseaddr = LOWCORE1;	/* default net load address */
    boot_satisfied = FALSE;     /* nothing booted yet */
    monlib_initialized = FALSE; /* monlib not loaded yet */
    sys_dirty = FALSE;          /* system ready for booting */
    system_state = SYSTATE_MONITOR; /* Note we are in the monitor */ 

    vsr_set = vstring_sys_rom[0] = vstring_sys[0] = '\0';

    bzero((char*) &caps, sizeof(caps));
    caps.version = SYSCAP_VERSION;
    caps.length = SYSCAP_LENGTH;

    /*
     * Save information from restart() in bootstuff[]:
     */
    et = LOWCORE1;
    bootstuff[BOOT_COUNT] = BOOTDATA;	/* Length of bootstuff */
    for (i = BOOT_REASON; i <= BOOTDATA; i++)
	bootstuff[i] = *et++;		/* Copy into bootstuff */
    
    /*
     * Clear out the cross-boot information area
     */

    bzero(boot_information, BOOTINFOSIZE);

    /*
     * Try to generate a possible parity error:
     */
    asm("	.globl init_test_memory_parity");
    asm("init_test_memory_parity:");
    for (i = 0; i < BUFSIZ; i++) {
	bufu.buf[i] = (char)i;
	j = bufu.buf[i];
    }

    if (~conf & DIAG_MASK)
	puts("[OK]\nChecking environment variables...");
    nvraminit(startup);

    /*
     * Plug a known value at the end of the bss section.  Check it on
     * emt call to verify that the stack hasn't clobbered our data.
     */
    _end = STACK_MAGIC;

    /*
     * Delight lamp
     */
    *(volatile u_short *)ADRSPC_CONTROL &= ~CONTROL_LAMP;

    if (~conf & DIAG_MASK)
	puts("[OK]\nExiting to monitor command level\n\n");

    /*
     * Return
     */
    asm("	.globl initc");
    asm("	jmp initc");
}

/*
 * Reset all IO devices to a quiescent state
 */
reset_io ()
{
    /*
     * Reset the Multibus:
     */
    *(volatile u_short *)ADRSPC_CONTROL |= CONTROL_MBRESET;
    
    /*
     * Unreset the Multibus:
     */
    *(volatile u_short *)ADRSPC_CONTROL &= ~CONTROL_MBRESET;

    /*
     * Reset the devices on each slot
     */
    reset_dgbus();
}

/*
 * Parity error check buserror handler     
 */
void pbuserr ()
{
    buserrhandler = 0;

    /*
     * the next read operation will take a snapshot of the
     * control register and then hardware will clear all
     * eight bits so that a succeeding error is not confused
     * with one before the status was read
     */
    controlreg_snapshot = *(volatile u_short *)ADRSPC_CONTROL;

    if (controlreg_snapshot & CONTROL_PARITY)
       buserrcnt++;  /* Parity error bit was set (parity error detected) */
    else
       puts("\npbuserr: [FAILURE] Parity Error bit not detected\n");
    asm("nop");      /* used to flush pipe so buserrcnt settles */
}

/*
 * Parity circuitry test
 */
int plogic_test (conf)
ushort conf;
{
    volatile long l;
    volatile short s[2];
    volatile char c[4];
    volatile long i,j;
    volatile short s0;
    volatile char c0;
    uchar failed = 0;

#define ODD       *(volatile u_short *)ADRSPC_CONTROL &= ~CONTROL_EVENPAR
#define EVEN      *(volatile u_short *)ADRSPC_CONTROL |= CONTROL_EVENPAR

    if (~conf & DIAG_MASK)
	puts("[OK]\nTesting main memory parity logic...");

    EVEN;
    l = 0x12345679;
    ODD;
    buserrcnt = 0; buserrhandler = pbuserr;
    asm("nop"); /* to flush pipe so buserrhandler settles */
    i = l;      /* parity err */
    asm("nop"); /* nop's are provided so we can keep buserror */
    asm("nop"); /* handler ignorant so it doesn't have to attempt */
    asm("nop"); /* to fix up the return address */
    if (buserrcnt != 1) failed |= 1;
    l = 1;  /* write over memory with correct parity */

    EVEN;
    s[0] = 0x1234;
    ODD; s[1] = 0;
    buserrcnt = 0; buserrhandler = pbuserr;
    asm("nop");
    s0 = s[0];  /* parity err */
    asm("nop"); asm("nop"); asm("nop");
    if (buserrcnt != 1) failed |= 128;
    s[0] = 1;

    EVEN;
    s[1] = 0x1234;
    ODD; s[0] = 0;
    buserrcnt = 0; buserrhandler = pbuserr;
    asm("nop");
    s0 = s[1];  /* parity err */
    asm("nop"); asm("nop"); asm("nop");
    if (buserrcnt != 1) failed |= 64;
    s[1] = 1;

    EVEN;
    c[0] = 0x13;
    ODD; 
    c[1] = 0; c[2] = 0; c[3] = 0;
    buserrcnt = 0; buserrhandler = pbuserr;
    asm("nop");
    c0 = c[0];  /* parity err */
    asm("nop"); asm("nop"); asm("nop");
    if (buserrcnt != 1) failed |= 32;
    c[0] = 1;

    EVEN;
    c[1] = 0x13;
    ODD; 
    c[0] = 0; c[2] = 0; c[3] = 0;
    buserrcnt = 0; buserrhandler = pbuserr;
    asm("nop");
    c0 = c[1];  /* parity err */
    asm("nop"); asm("nop"); asm("nop");
    if (buserrcnt != 1) failed |= 16;
    c[1] = 1;

    EVEN;
    c[2] = 0x13;
    ODD; 
    c[0] = 0; c[1] = 0; c[3] = 0;
    buserrcnt = 0; buserrhandler = pbuserr;
    asm("nop");
    c0 = c[2];  /* parity err */
    asm("nop"); asm("nop"); asm("nop");
    if (buserrcnt != 1) failed |= 8;
    c[2] = 1;

    EVEN;
    c[3] = 0x13;
    ODD; 
    c[0] = 0; c[1] = 0; c[2] = 0;                
    buserrcnt = 0; buserrhandler = pbuserr;
    asm("nop");
    c0 = c[3];  /* parity err */
    asm("nop"); asm("nop"); asm("nop");
    if (buserrcnt != 1) failed |= 4;
    c[3] = 1;

    if ( !failed && ~conf & DIAG_MASK ) 
	puts("[OK]\n");

    if ( failed ) {
	puts("[FAILURE]\nMain mem parity logic failure.\n");
        printf("Cryptic error code %x\n",failed);
    } 
}
    
/*
 * Set instruction and data cache in 68040
 */
void setcache (cp)
char *cp;
{
    int change = 0;
    char comm = *cp;

    while (*cp && !isspace(*cp))
        cp++;                /* skip rest of command */
    while (*cp && isspace(*cp))
        cp++;                /* skip leading spaces */

    switch (comm) {
    case 'I':
	change = 1;
	i_cache = !i_cache;
	break;
    case 'D':
	change = 1;
	d_cache = !d_cache;
	break;
    case '\0':
        change = 1;
	i_cache = !i_cache;
	d_cache = !d_cache;
	break;
    case '?':
        puts("[I]      Toggle instruction cache\n");
        puts("[D]      Toggle data cache\n");
        puts("[none]   Toggle both instruction and data cache\n");
	break;
    default:
        puts(illarg);
	break;
    }
    if (change) {
	change = ICACHE_040*i_cache | DCACHE_040*d_cache;
	printf("Setting cacr to %x",change);
	setcacr(change);
	if (getcacr() == change)
            puts(" [OK]\n");
        else
            puts(" [FAILED]\n");
    }
	printf("Instruction cache is %s\n", 
               i_cache ? "enabled" : "disabled");
	printf("Data cache is %s\n", 
               d_cache ? "enabled" : "disabled");
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
    register uint   *reg_dptr;
    register uint   reg_d;
    volatile ushort *control_ptr;
    uint            old_cacr;
    ushort          level;
    
    /*
     * Disable all maskable interrupts during this code:
     */
    level = sil(7);
    
    /*
     * Get the current CACR value so that we can restore it:
     */
    old_cacr = getcacr();
    
    /*
     * This is a 68040 processor:
     */
    
    /*
     * Set CACR to disable instruction and data caches while we are
     * in this subroutine, then flush any dirty lines from either cache:
     */
    (void)setcacr(0);
    
    /*
     * CPUSH to flush both caches before parity changes:
     */
    asm(".word	0xf4f8");
    
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
    control_ptr = (volatile ushort *)ADRSPC_CONTROL;
    *control_ptr |= CONTROL_EVENPAR;
    
    asm("nop");		/* Flush 68040 pipe */
    
    /*
     * Write data to location with even parity using the pointer
     * and data squirreled away in registers:
     */
    *reg_dptr = reg_d;
    
    asm("nop");		/* Flush 68040 pipe */
    
    /*
     * Set control register back to usual odd parity in main memory
     * to generate and check odd parity before returning:
     */
    *control_ptr &= ~CONTROL_EVENPAR;
    
    
    /*
     * Let write to control register happen while we execute code from ROM:
     */
    asm("nop");
    
    /*
     * Restore the cache states:
     */
    (void)setcacr(old_cacr);
    
    /*
     * Restore the previous interrupt level:
     */
    (void)sil(level);
    
    return(0);
}

ushort read_nv_short (ushort *address)
{
    ushort value;

    bcopy(address, &value, sizeof(ushort));
    return value;
}

void write_nv_short (ushort *address, ushort value)
{
    bcopy(&value, address, sizeof(ushort));
}

void reset_confreg (void)
{
    write_nv_short((ushort *)config_ptr, DEFAULT_CONFIG | IGNORE_CONFIG);
}

boolean check_stack_corruption (boolean vocal)
{
    if (vocal)
	printf("0x%x = 0x%x\t", &_end, _end);
    if (_end == STACK_MAGIC) {
	if (vocal) printf("[OK]\n");
	return FALSE;
    } else {
	if (vocal) printf("[Possible Stack Overflow]\n");
	return TRUE;
    }
}
