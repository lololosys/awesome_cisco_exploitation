/* $Id: init.c,v 1.1.68.1 1996/06/16 21:18:50 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/init.c,v $
 *------------------------------------------------------------------
 * init.c -- Platform specific initialization.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: init.c,v $
 * Revision 1.1.68.1  1996/06/16  21:18:50  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:16:29  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Includes
 */
#include "pcmap.h"
#include "as5200_pcmap.h"
#include "cpu.h"
#include "nvmonvars.h"
#include "extern.h"
#include "mon_defs.h"
#include "mon_plat_defs.h"
#include "console.h"
#include "reason.h"
#include "exception.h"
#include "context.h"
#include "setjmp.h"
#include "monitor.h"
#include "m68vectors.h"
#include "cookie.h"
#include "confreg.h"

/*
 * Defines
 */
#define NUM_MTEST_VALS (sizeof(mtestvals)/sizeof(struct mtestvals))
#define RAM_TEST_START ((unsigned long *)ADRSPC_RAM)
#define RAM_TEST_END   ((unsigned long *)(ADRSPC_RAM+MONITOR_STACK))
#define RAM_TEST_SIZE  (MONITOR_STACK - ADRSPC_RAM)

/*
 * Externs
 */
extern char *banner_string;
extern char *etext, *edata, *end;
extern long *_start, *evec, *evece;

/*
 * Misc declarations
 */
struct nvram *nvram_ptr = (struct nvram *)ADRSPC_NVRAM;
char *errlog_start = ((char *)(MONITOR_FREE_MEM_BASE+1));
static char brasil_aliasinit[] = {
  "r\0repeat\0h\0history\0?\0help\0b\0boot\0ls\0dir\0i\0reset\0k\0stack\0"
      "br\0break\0s\0step\0\0"
};
struct mtestvals {
    long val1, val2;
    char *name;
} mtestvals[] = {
    0x0, 0x0, "data equals address",
    0x55555555, 0xaaaaaaaa, "checkerboard",
    0xaaaaaaaa, 0x55555555, "inverse checkerboard",
};


struct mon_iface *mifp;  /* our global pointer */
char *platform_string = "AS5200";
char *default_monlib_string = "%as5200-fslib-m";
int cpu_type = CPU_CANCUN;
char *errlog_start;
int sys_state;
unsigned int hw_rev;
struct cookie *cookie_p;
char brk;
unsigned long clock, hclock;
/*
 * Note - checksum is initialized so it goes into text segment
 */ 
static unsigned int checksum = 0;

/*
 * forward declarations
 */
void bzero(char *addr, int cnt);
void bcopy(char *src, char *dest, int size);
int wr_ena_nvram(void);
int wr_prot_nvram(void);
void nv_wr_confreg(unsigned short val);
void reset_io(void);
void mon_chksum(void);
void cp_intvec_toram(int mem_location);

/*
 * Initialize the hardware
 */
int
init(void)
{
    /*
     * Note that the MC68EC030 does not have any
     * internal memory so until we test low memory, only
     * registers are available for variable storage.
     */
    char *cp;
    unsigned short confreg;
    volatile unsigned long *mptr;
    long  i, val1, val2, expected, readval, start_reason;

    /*
     * The following entry point is here so the assemly language
     * jmp in start.S can bypass the link instruction (causing
     * stack operations) associated with a subroutine call.
     */
    asm("       .globl __init");
    asm("__init:");

    /*
     * Is the configuration magic value in NVRAM ok?
     */
    if(NV_VALID_CONFREG) {
        confreg = (NVRAM_SAV)->confreg;
    } else {
        confreg = DEFAULT_CONFIG; /* No, use default config value */
    }

    /*
     * Init the console port
     */
    i = ((confreg & CONSOLE_MASK) >> CONSOLE_SHIFT);
    uartaddr->a.aux2 = 0;       /* Turn off interrupts (IMR=0) */
    uartaddr->a.command = RESETMRPTR;
    uartaddr->a.mode = CHARERROR | PRTYOFF | DBITS8;
    uartaddr->a.mode = MODENORMAL | STOP16X16;
    uartaddr->a.aux1 = BRGSET1 | TIMXTAL;
    uartaddr->a.state = (RXCLOCK * UARTspeeds[i]) | (TXCLOCK * UARTspeeds[i]);
    uartaddr->a.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;
    uartaddr->b.aux2 = 0;       /* All outputs are driven by OPR */
    uartaddr->b.aux3 = CDA;     /* Set Carrier detect on Console port */

    /*
     * sign-on message
     * Say hello to the world, but avoid using the standard printf mechanism
     * until our bss section is initialized
     */
    PUTSTRING(cp, banner_string);

    /*
     * In the boot state (the state of the system after reset) the address
     *  0XXXXXX		reads and executes from address 1XXXXXX and
     *			writes onto address 0XXXXXX
     */
    PUT_DIAG_STR("Exiting boot state ...");
    *(volatile unsigned short *)ADRSPC_CONTROL &= ~CONTROL_BOOT;

    /*
     * Set the ROM size
     */
    PUT_DIAG_STR("\r\nSetting ROM size, 16 bit wide EPROM, 4 Mbit EPROMs ...");
    *(volatile unsigned short *)ADRSPC_CONTROL2 =
		(CONTROL2_BROMSIZE_1MB << CONTROL2_BROMSIZE_SHIFT);

    /*
     * Test and initialize low core
     * From the base of memory to the top of our (future) stack
     * This includes our bss area
     *
     * Initially configure ASIC memory size bits for max possible, 16MB
     */
    PUT_DIAG_STR("\r\nSetting RAM size, 32 bit wide EPROM ...");
    i = *(volatile ushort *)ADRSPC_CONTROL2;
    i |= (CONTROL2_MSIZE_16MB << CONTROL2_MSIZE_SHIFT);
    *(volatile ushort *)ADRSPC_CONTROL2 = i;

    /*
     * Low core test.
     *
     * data = address : Check address aliasing
     * AA's/55's : Checkerboard
     * 55's/AA's : Inverse checkerboard
     * Note, memory range must be divisable by 8
     */
    for(i=0; i<NUM_MTEST_VALS; i++) {
        PUT_DIAG_STR("\r\nTesting Main Memory from 0x0 to 0x4000: ");
        PUT_DIAG_STR(mtestvals[i].name);
        val1 = mtestvals[i].val1;
        val2 = mtestvals[i].val2;
        for( mptr = (volatile unsigned long *)RAM_TEST_START;
            mptr < (volatile unsigned long *)RAM_TEST_END; ) {
                if(i == 0) {  /* test 0 is "data = address" test */
                    *mptr++ = (long)mptr;
                } else {
                    *mptr++ = val1;
                    *mptr++ = val2;
                }
        }
        for( mptr = (volatile unsigned long *)RAM_TEST_START;
            mptr < (volatile unsigned long *)RAM_TEST_END;
	    mptr++) {
                if( i == 0 ) expected = (long)mptr;
                /* test 0 is "data = address" test */
                else {
                    if((long)mptr & 4) expected = val2;
                    else expected = val1;
            }
            readval = *mptr;
            if( readval != expected ) {
                PUTSTRING(cp,"\r\nBad RAM at location 0x");
                PUTHEXNUM((long)mptr);
                PUTSTRING(cp,": wrote 0x");
                PUTHEXNUM(expected);
                PUTSTRING(cp,", read 0x");
                PUTHEXNUM(readval);
                asm("    moveq #1,d0");  /* ERROR #1 */
        	asm("       jmp lights");
            }
        }
    }

    /*
     * zero low core including the bss
     */
    if (confreg & DIAG_MASK) {
	printf("\nZeroing low core ...");
    }
    for(mptr = RAM_TEST_START; mptr < RAM_TEST_END; )
            *mptr++ = 0x0;

    /*
     * We can use our bss now
     * This means we can start using puts and printf, etc.
     */
    envflag = INPONCT;  /* in power-on confidence test */

    /*
     * Size memory for real.
     */
    memsize = memsizing();
    if(memsize == 0) {
	printf("\naborting confidence test ...\n");
	asm("    moveq #2,d0");  /* ERROR #2 */
        asm("       jmp lights");
    }
    if (confreg & DIAG_MASK) {
	printf("memsizing() returned 0x%x ...", memsize);
    }

    /*
     * Checksum the Monitor image
     */
    if (confreg & DIAG_MASK) {
        printf("\nChecksumming Monitor image ...");
    }
    mon_chksum();

    /*
     * Initialize main memory.
     */
    if (confreg & DIAG_MASK)
        printf("\nInitializing main memory from 0x%x to 0x%x ...",
		ADRSPC_RAM, (ADRSPC_RAM+memsize));
    setcacr(CLEAR_CACHES); /* Clear the cache */
    setcacr(ENABLE_CACHES); /* Turn on caches to gain speed */
    while(mptr < (unsigned long *)(ADRSPC_RAM+memsize))
            *mptr++ = 0x0;
    setcacr(0); /* Disable the caches */
    setcacr(CLEAR_CACHES); /* Clear the cache */

    /*
     * Play the reset game: a3 is used to store a value
     * to differentiate a EXC_RESET from other warm starts.
     * a3 was chosen as, until we size pkt memory, it is unused.
     */
    asm("movl    a3,%0" : "=g" (start_reason)); /* Initialize start_reason */

    /*
     * Size Packet memory.
     */
    shmemsize = shmemsizing();
    if(shmemsize == 0) {
        printf("\naborting confidence test ...\n");
	asm("    moveq #3,d0");  /* ERROR #3 */
        asm("       jmp lights");
    }
    if (confreg & DIAG_MASK) {
        printf("sizeshmem() returned 0x%x ...", shmemsize);
    }

    /*
     * Now, test I/O memory since we've yet only tested CPU memory.
     */
    for(i=0; i<NUM_MTEST_VALS; i++) {
	if (confreg & DIAG_MASK) {
	    printf("\nTesting Packet Memory from 0x%x to 0x%x: ",
		ADRSPC_BRASIL_PKT_MEM, (ADRSPC_BRASIL_PKT_MEM+RAM_TEST_SIZE));
	    printf("%s", mtestvals[i].name);
	}
        val1 = mtestvals[i].val1;
        val2 = mtestvals[i].val2;
        for(mptr = (volatile unsigned long *)ADRSPC_BRASIL_PKT_MEM;
            mptr < (volatile unsigned long *)(ADRSPC_BRASIL_PKT_MEM+RAM_TEST_SIZE); ) {
                if(i == 0) {  /* test 0 is "data = address" test */
                    *mptr++ = (long)mptr;
                } else {
                    *mptr++ = val1;
                    *mptr++ = val2;
                }
        }
        for( mptr = (volatile unsigned long *)ADRSPC_BRASIL_PKT_MEM;
            mptr < (volatile unsigned long *)(ADRSPC_BRASIL_PKT_MEM+RAM_TEST_SIZE);
            mptr++) {
                if( i == 0 ) expected = (long)mptr;
                /* test 0 is "data = address" test */
                else {
                    if((long)mptr & 4) expected = val2;
                    else expected = val1;
            }
            readval = *mptr;
            if( readval != expected ) {
		printf("\nBad RAM at location 0x%x", (long)mptr);
		printf(": wrote 0x%x, read 0x%x", expected, readval);
		asm("    moveq #4,d0");  /* ERROR #4 */
        	asm("       jmp lights");
            }
        }
    }

    /*
     * Read the hardware revision register:
     */
    if (confreg & DIAG_MASK)
        puts("\nReading hardware revision register ...");
    hw_rev = ((*(volatile unsigned short *)ADRSPC_HW_REVISION >> 8) & 0xFF);
    if (confreg & DIAG_MASK)
        printf(" hardware revision = %d", hw_rev);

    /*
     * Size NVRAM via Bus error detection. We don't want to size memory via
     * detecting on a write, because we don't want to be left holding the bag
     * if we lose power before restoring the NVRAM location.
     */
    if (confreg & DIAG_MASK)
        puts("\nSizing NVRAM ...");
    val1 = size_nvram();
    if (confreg & DIAG_MASK)
        printf("found %dK bytes of NVRAM ...", val1/1024);

    /*
     * If NVRAM sizes ok write our confreg back out
     */
    if (val1) {
        nv_wr_confreg(confreg);
    }
    nvraminit();

    /*
     * Initialize the alias table.
     */
    movbyte(brasil_aliasinit,ALIAS_TABLE,sizeof(brasil_aliasinit));

    /*
     * Get the cookie.
     */
    if (confreg & DIAG_MASK) {
        puts("\nReading cookie ... ");
    }
    cookie_p = (struct cookie *)get_cookie();
    if (confreg & DIAG_MASK) {
        puts("Done.");
    }

    /*
     * Init/print processor type
     */
    switch (cookie_p->processor) {
	case PROCESSOR_CANCUN:
	    cpu_type = CPU_CANCUN;
	    if (confreg & DIAG_MASK)
		puts("\nProcessor type is: AS5200 ...");
	    break;
	default:
	    cpu_type = CPU_UNKNOWN;
	    if (confreg & DIAG_MASK)
		puts("\nWARNING: Unknown Processor type found in cookie ...");
	    break;
    }


    (NVRAM)->testarea = 0x0;
    hkeepflags = 0x0;       /* clear housekeeping flags */
    monjmpptr = 0;      /* initialize monjmp pointer */
    sys_state = MONITOR_RUNNING;
    initsigs();                  /* set all signals to default */
    clrerrlog();

    /*
     * Log the reason for getting here
     */
    if (start_reason == EXC_RESET) {
        log_sys_ret(EXC_RESET, (cpu_context_t *)0);
	clr_autoboot_state();
    }

    /*
     * Initialize the timer
     */
    if (confreg & DIAG_MASK)
        puts("\nInitializing the PIT timer ...");
    clock = 0;                  /* zero the ms clock */
    hclock = 0;                 /* zero the upper ms clock */
    timer_init();

    /*
     * Brasil does not support parity.
     */
    *((unsigned short *)ADRSPC_CONTROL) &= ~CANCUN_PARITY_ENABLE;

    /*
     * Set the default Brasil Flash Vpp operational mode
     */
    *((ushort *)ADRSPC_BRASIL_MB_CTRL2_REG) = BRASIL_FLASH_VPP_MODE;

    /*
     * copy the exception vectors to the base of RAM
     * for use by the system image.
     */
    if (confreg & DIAG_MASK){
        puts("\nInitializing the RAM vector table ...");
    }
    bcopy((char *)&evec, (char *)ADRSPC_RAM,
          (int)((long)&evece - (long)&evec));

    /*
     * Variable initializations
     */
    if (confreg & DIAG_MASK)
        puts("\nInitializing monitor variables ...");
    initdevtbl();
    init_ml_iface();  /* init the monlib interface */

    /*
     * Enable caches.
     */
    if (confreg & DIAG_MASK)
        puts("\nEnabling processor caches, ");
    setcacr(CLEAR_CACHES); /* Clear the caches */
    setcacr(ENABLE_CACHES); /* Enable both caches */
    if (confreg & DIAG_MASK)
        printf("set CACR to %#x ...", ENABLE_CACHES);

    /*
     * Take the interfaces out of reset
     */
    reset_io();

    /*
     * Light the run lamp:
     */
    *(volatile unsigned short *)ADRSPC_CONTROL |= CONTROL_RUNLAMP;
 
    if (confreg & DIAG_MASK)
        puts("\nExiting to monitor command level\n");

    /*
     * Return
     */
    asm("       .globl initdone");
    asm("       jmp initdone");

}

/*
 * Routine to checksum the monitor image
 */
void
mon_chksum(void)
{
    unsigned int tmp;
    unsigned short sum;
    unsigned char *addrptr;
    long i;

    sum = 0;         /* initialize */
    tmp = checksum;  /* checksum is static var defined above */
    addrptr = (unsigned char *)&_start;
 
    while(addrptr < (unsigned char *)&etext) {
        sum += *addrptr++;
    }
 
    /*
     * subtract out the original checksum
     */
    for(i=0 ; i<sizeof(checksum); i++) {
        sum -= (unsigned short)(tmp & 0xff);
        tmp = tmp >> 8;  /* get next byte */
    }

    /*
     * Check it.
     */
    if(sum != (unsigned short)checksum){
        printf("\nWARNING: Monitor image chksum failed - expected %#x, got %#x\n",
              (unsigned short)checksum, sum);
        /* what to do here? */
    }

}
/*
 * Brasil does not have the ability to write protect NVRAM.
 */
int
wr_ena_nvram(void)
{
    return;
}

int
wr_prot_nvram(void)
{
    return;
}

/*
 * Write the confreg value to NVRAM.  Do *not* update the
 * in-core copy.
 * The tmp_nv structure herein mirrors the relevant portion of the
 * actual nvram structure.
 */
void
nv_wr_confreg(unsigned short val)
{
    struct {
	unsigned short confreg;
	unsigned short rconfreg;
	long magic;
    } tmp_nv;

    tmp_nv.confreg = val;
    tmp_nv.rconfreg = ~val;
    tmp_nv.magic = NVMAGIC;
    nvwrite((unsigned char *)&tmp_nv,
	    (unsigned char *)&(NVRAM_SAV)->confreg, sizeof(tmp_nv));
}

/*
** Nothing has been done to speed enhance this routine.  If
** it is used for very big blocks then it should be rewritten
** for speed.
*/
void
bzero (char *addr, int cnt)
{
    char zero = 0;
    
    while (cnt--)
	*addr++ = zero;
}

/*
** Nothing has been done to speed enhance this routine.  If
** it is used for very big blocks then it should be rewritten
** for speed.
*/

static void
bcopy_down_body(char *src, char *dest, int size)
{
    char *end;

    end = src + size;
    while(src < end) *dest++ = *src++;
}

static void
bcopy_up_body(char *src, char *dest, int size)
{
    char *end;

    end = src;
    src += (size - 1);  /* start from the other end */
    dest += (size - 1);
    while(size--) *dest-- = *src--;
}

void
bcopy(char *src, char *dest, int size)
{
    void (*routine)();

#ifdef DEBUG
printf("bcopy: src = %#x, dest = %#x, size = %#x\n",src, dest, size);
#endif

    if(src > dest)
	routine = &bcopy_down_body;
    else
	routine = &bcopy_up_body;

    (*routine)(src, dest, size);
}

/*
 * Dummy up cache_flush and clear cache routines.
 */
void
cache_flush(void)
{
    /* Do we need this??? */
    return;
}

void
clearcaches(void)
{
    setcacr(CLEAR_CACHES); /* Clear the cache */
}

void
reset_io(void)
{
    short ctrl4;
    /*
     * Reset channels
     */
    *(volatile unsigned short *)ADRSPC_RESET |= ALL_NETWORK_RESET;
    BUSERR_NOP_X4();                  /* Delay... */
    *(volatile unsigned short *)ADRSPC_RESET &= ~ALL_NETWORK_RESET;

    /*
     * Reset slots
     */
    ctrl4 = *(volatile unsigned short *)ADRSPC_BRASIL_MB_CTRL4_REG; 
    ctrl4 &= ~ALL_SLOTS_RESET;
    *(volatile unsigned short *)ADRSPC_BRASIL_MB_CTRL4_REG = ctrl4;
    BUSERR_NOP_X4();
    ctrl4 |= ALL_SLOTS_RESET;
    *(volatile unsigned short *)ADRSPC_BRASIL_MB_CTRL4_REG = ctrl4;
}

/* dummy for the new compilers */
__main()
{
}
char *auxstr = "\nSorry, the aux software is not yet implemented on this platfor
m\n";
 
void
initaux(void)
{
    puts(auxstr);
}
 
int
auxstat(void)
{
    puts(auxstr);
}
 
int
auxin(void)
{
    puts(auxstr);
}
 
void
auxout(int c)
{
    puts(auxstr);
}
 
/*
 * cp_intvec_toram:
 * Copy the interrupt vector table from PROM into RAM
 * Set the VBR point to location in RAM
 * Input:
 *        Memory location where the interrupt vector table desired
 */
void
cp_intvec_toram(int mem_location)
{
    bcopy((char *)&evec, (char *)mem_location,
	  (int)((long)&evece - (long)&evec));
 
    setvbr(mem_location);  /* now vbr is pointed to ram location */
}

/* end of module */
