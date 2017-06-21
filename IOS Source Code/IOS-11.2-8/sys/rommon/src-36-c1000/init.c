/* $Id: init.c,v 3.4.10.1 1996/03/21 23:29:04 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/init.c,v $
 *------------------------------------------------------------------
 * init.c
 *
 * November 1994, Rob Clevenger
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Platform specific initialization routine.
 *------------------------------------------------------------------
 * $Log: init.c,v $
 * Revision 3.4.10.1  1996/03/21  23:29:04  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:15  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1995/12/11  19:14:48  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:59  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.4  1996/02/23  17:15:53  cshen
 * CSCdi48454:  Need to support on-board flash on c1005 derivative
 *
 * Revision 3.3  1995/12/03  17:59:57  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  18:44:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:24:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/23  05:58:02  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.2  1995/09/22  04:36:20  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.1  1995/06/07  22:39:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "quicc.h"
#include "clpd6720.h"
#include "extern.h"
#include "signal_4k.h"
#include "monlib.h"
#include "confreg.h"
#include "mon_defs.h"
#include "console_36.h"
#include "exception.h"
#include "context.h"
#include "tp3420.h"
#include "init.h"
#include "dev.h"
#include "mon_boot.h"
#include "flash.h"

#include "../../../boot/cpu.h"

char *errlog_start = ((char *)(MONITOR_FREE_MEM_BASE+1));
extern volatile short buserrcnt; /* Bus error count */  
extern char *etext, *edata, *end;
extern long *_start, *evec, *evece;
extern unsigned char UARTspeeds[];
extern char *banner_string;
extern int total_flash;

struct nvram *nvram_ptr = (struct nvram *)ADRSPC_NVRAM;

struct mtestvals {
    long val1, val2;
    char *name;
} mtestvals[] = {
    0x0, 0x0, "data equals address",
    0x55555555, 0xaaaaaaaa, "checkerboard",
    0xaaaaaaaa, 0x55555555, "inverse checkerboard",
};
#define NUM_MTEST_VALS (sizeof(mtestvals)/sizeof(struct mtestvals))
#define RAM_TEST_START ((unsigned long *)ADRSPC_RAM)
#define RAM_TEST_END   ((unsigned long *)MONITOR_STACK)

/*
** Stuff that is in the nvram structure on some platforms is in
** bss on Sapphire due to small nvram size.
*/
char aliastbl[ALSSIZ];             /* our monitor alias table */
unsigned char conbaud;             /* the console baud rate (future use) */
unsigned char killchar;            /* the kill line character (future)*/
unsigned char erasechar;           /* the erase character (future) */
unsigned char diagflag;            /* LOOPONERR | STOPONERR | CONTINUOUS, etc. */
int memop_siz;                     /* operation size for memory routines */
unsigned long memop_srcaddr;       /* source address for the menu diags */
unsigned long memop_destaddr;      /* destination address for the menu diags */
unsigned long memop_start;         /* start address for the menu diags */
unsigned long memop_length;        /* length for the menu diags */
unsigned long memop_value;         /* data value for the menu diags */
unsigned long memop_trigger;       /* scope trigger address */
char diaglist[32];                 /* storage for our diag groups */
static char sap_aliasinit[] = {
  "r\0repeat\0h\0history\0?\0help\0b\0boot\0ls\0dir\0i\0reset\0k\0stack\0"
      "br\0break\0s\0step\0\0"
};


struct mon_iface *mifp;  /* our global pointer */
char *platform_string = "C1000";
char *default_monlib_string = "%c1000-fslib-m";
int cpu_type = CPU_SAPPHIRE;
char ciscopro_flag;  /* if non-zero, then this is a CiscoPro platform */
char *errlog_start;
static long pcmcia_siz;
int sys_state;
char hw_rev;

/*
** Note - checksum is initialized so it goes into text segment
*/ 
static unsigned int checksum = 0;

/*
 * pin initialization table
 */

static quicc_pin_init_t pin_init_array[] =
{ 
    /* 
     * default (just init console pins everything else an input)
     */

    { CAT_QU_PT_A_DIR_INIT,	/* padir */
      CAT_QU_PT_A_PAR_INIT,	/* papar */
      CAT_QU_PT_A_ODR_INIT,	/* paodr */
      (0),			/* padat */

      CAT_QU_PT_B_DIR_INIT,	/* pbdir */
      CAT_QU_PT_B_PAR_INIT,	/* pbdat */
      CAT_QU_PT_B_ODR_INIT,	/* pbodr */
      (SAP_QU_PT_B_PAR_WRX_LED | 
       SAP_QU_PT_B_PAR_WTX_LED |
       SAP_QU_PT_B_PAR_RUNLED),	/* pbdat */

     CAT_QU_PT_C_DIR_INIT,	/* pcdir */
     CAT_QU_PT_C_PAR_INIT,	/* pcpar */
     CAT_QU_PT_C_SO_INIT,	/* pcso */
     (SAP_QU_PT_C_PAR_CON_DSR) }, /* pcdat */

    /* 
     * Pin initialization for sapphire 
     */

    { SAP_QU_PT_A_DIR_INIT,	/* padir */
      SAP_QU_PT_A_PAR_INIT,	/* papar */
      SAP_QU_PT_A_ODR_INIT,	/* paodr */
      (SAP_QU_PT_A_PAR_ET_FDX),	/* padat */

      SAP_QU_PT_B_DIR_INIT,	/* pbdir */
      SAP_QU_PT_B_PAR_INIT,	/* pbdat */
      SAP_QU_PT_B_ODR_INIT,	/* pbodr */
      (SAP_QU_PT_B_PAR_10B_AUT | 
       SAP_QU_PT_B_PAR_ISDN_CS |
       SAP_QU_PT_B_PAR_WRX_LED | 
       SAP_QU_PT_B_PAR_WTX_LED |
       SAP_QU_PT_B_PAR_RUNLED),	/* pbdat */

     SAP_QU_PT_C_DIR_INIT,	/* pcdir */
     SAP_QU_PT_C_PAR_INIT,	/* pcpar */
     SAP_QU_PT_C_SO_INIT,	/* pcso */
     (SAP_QU_PT_C_PAR_TPSQEL | 
      SAP_QU_PT_C_PAR_TPAPCE | 
      SAP_QU_PT_C_PAR_CON_DSR) }, /* pcdat */
   
      /* 
       * Pin initialization for catwoman 
       */

    { CAT_QU_PT_A_DIR_INIT,	/* padir */
      CAT_QU_PT_A_PAR_INIT,	/* papar */
      CAT_QU_PT_A_ODR_INIT,	/* paodr */
      (0),			/* padat */

      CAT_QU_PT_B_DIR_INIT,	/* pbdir */
      CAT_QU_PT_B_PAR_INIT,	/* pbdat */
      CAT_QU_PT_B_ODR_INIT,	/* pbodr */
      (SAP_QU_PT_B_PAR_WRX_LED | 
       SAP_QU_PT_B_PAR_WTX_LED |
       SAP_QU_PT_B_PAR_RUNLED),	/* pbdat */

     CAT_QU_PT_C_DIR_INIT,	/* pcdir */
     CAT_QU_PT_C_PAR_INIT,	/* pcpar */
     CAT_QU_PT_C_SO_INIT,	/* pcso */
     (SAP_QU_PT_C_PAR_CON_DSR) } /* pcdat */
   
};
/* forward declarations */
void pet_the_dog(void);   /* strobe the watchdog timer */
int init_quicc(int wd_on);
void bzero(char *addr, int cnt);
void bcopy(char *src, char *dest, int size);
void wr_ena_nvram(void);
void wr_prot_nvram(void);
void nv_wr_confreg(unsigned short val);
void quicc_pin_initialization(int hardware_type);
#ifdef ONBOARD_FLASH
void fixed_flash_init(void);
int sizeflash(void);
#endif

/*
** Initialize the hardware
** Note that you can use the stack in this routine, the stack has been
** established in the QUICC chip DPRAM
** The watchdog timer gets initialized early on in the init sequence.
** The routine pet_the_dog() must be called periodically until the
** PIT timer gets kicked off.
*/
/*
** There is additional code now to support boards with on-board flash
** and NO PCMCIA. This is for NEC's UMI and OKI's HANA projects.
** The on-board flash specific initialization fixed_flash_init() is
** called only when in partner-specific build.
*/

int
init(void)
{
    int i;
    long temp;
    long msize;
    unsigned short confreg;
    unsigned long zero;
    unsigned long *mptr, expected, readval, val1, val2;
    int rsr_save;
    struct cookie *cookiep;
    unsigned char *addrptr;
    unsigned int tmp;
    unsigned short sum;
    char newconf;
    volatile int hardware_type;

    rsr_save = init_quicc(1);  /* enable watchdog */

    /*
    ** Is the configuration magic value in NVRAM ok?
    */
    if(NV_VALID_CONFREG) {
	confreg = (NVRAM_SAV)->confreg;
	newconf = 0;
    } else {
	confreg = DEFAULT_CONFIG; /* No, use default config value */
	newconf = 1;
    }

    /* init console port */
    init_con((confreg & CONSOLE_MASK) >> CONSOLE_SHIFT);

    /* 
    ** sign-on message
    ** Say hello to the world, but avoid using the standard printf mechanism
    ** until our bss section is initialized
    */
    PUTSTRING("\r\n\n");
    PUTSTRING(banner_string);

    if (confreg & DIAG_MASK) {
	PUTSTRING("\nSizing main memory ...");
    }
    msize = memsizing();

    if(msize == 0) {
ponct_fail:
	PUTSTRING("\r\naborting confidence test ... stack left in DPRAM\r\n");
	return(1);
    }
    /*
    ** Test and initialize low core
    ** From the base of memory to the top of our (future) stack
    ** This includes our bss area
    */
    if (confreg & DIAG_MASK) {
	PUTSTRING(" found 0x");
	PUTHEXNUM(msize);
	PUTSTRING(" bytes");
    }
    pet_the_dog();

    /*
    ** Low core test.
    **
    ** data = address : Check address aliasing
    ** AA's/55's : Checkerboard
    ** 55's/AA's : Inverse checkerboard
    ** Note, memory range must be divisable by 8
    */
    for(i=0; i<NUM_MTEST_VALS; i++) {
	if (confreg & DIAG_MASK) {
	    PUTSTRING("\r\nTesting lower main memory - ");
	    PUTSTRING(mtestvals[i].name);
	}
	val1 = mtestvals[i].val1;
	val2 = mtestvals[i].val2;
	for(mptr = RAM_TEST_START; mptr < RAM_TEST_END; ) {
	    if(i == 0) {  /* test 0 is "data = address" test */
		*mptr++ = (long)mptr;
	    } else {
		*mptr++ = val1;
		*mptr++ = val2;
	    }
	}
	for(mptr = RAM_TEST_START; mptr < RAM_TEST_END; mptr++) {
	    if( i == 0 )  {
		/* test 0 is "data = address" */
		expected = (long)mptr;
	    } else {
		if((long)mptr & 4)
		    expected = val2;
		else 
		    expected = val1;
	    }
	    readval = *mptr;
	    if( readval != expected ) {
		PUTSTRING("\r\nBad RAM at location 0x");
		PUTHEXNUM((long)mptr);
		PUTSTRING(": wrote 0x");
		PUTHEXNUM(expected);
		PUTSTRING(", read 0x");
		PUTHEXNUM(readval);
		goto ponct_fail;
	    }
	}
    }

    zero = 0;  /* now zero low core including bss */
    for(mptr = RAM_TEST_START; mptr < RAM_TEST_END; ) *mptr++ = zero;
    envflag = INPONCT;

    /*
    ** We can use our bss now
    ** This means we can start using puts and printf, etc.
    */

    if (confreg & DIAG_MASK) {
	printf("\nChecksumming Monitor image ...");
    }

    pet_the_dog();
    sum = 0;         /* initialize */
    tmp = checksum;  /* checksum is static var defined above */

    addrptr = (unsigned char *)&_start;

    while(addrptr < (unsigned char *)&etext) {
	sum += *addrptr++;	
    }

    /* we must subtract out the original checksum */
    for(i=0 ; i<sizeof(checksum); i++) {
	sum -= (unsigned short)(tmp & 0xff);
	tmp = tmp >> 8;  /* get next byte */
    }

    if(sum != (unsigned short)checksum){
	printf("\nWARNING: Monitor image chksum failed - expected %#x, got %#x\n",
	      (unsigned short)checksum, sum);
	/* what to do here? */
    }

    if (confreg & DIAG_MASK) {
	printf("\nInitializing main memory ...");
    }

    pet_the_dog();
    temp = msize;
    temp += ADRSPC_RAM;  /* make this into an end address */
    while(mptr < (unsigned long *)temp) *mptr++ = zero;  /* zero the rest of RAM */

    pet_the_dog();
    memsize = msize;
    shmemsize = 0;  /* no shared memory on this platform */
    shmemaddr = 0;
    
    /*
    ** Get the hardware rev of the board.
    ** This info is in Port B of the QUICC chip.
    */
    hw_rev = SAP_QU_PT_B_HWREV(((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->pip_pbdat);
    if (confreg & DIAG_MASK) {
	printf("\nHardware rev is %#x", hw_rev);
    }

    if (confreg & DIAG_MASK) {
	puts("\nSizing NVRAM ...");
    }
    temp = size_nvram();
    pet_the_dog();

    /* if NVRAM sizes ok write our confreg back out */
    if(newconf && temp) {
	NV_WR_CONFREG(confreg);
    }

    if (confreg & DIAG_MASK) {
	printf(" found %#x bytes\nReading NVRAM ...", temp);
    }
    nvraminit();
    pet_the_dog();

    /*
    ** Since Sapphire does not keep the alias table in NVRAM we must
    ** initialize this separately.
    ** FIXME - use standard aliasinit table from nvram.c?
    */
    movbyte(sap_aliasinit,ALIAS_TABLE,sizeof(sap_aliasinit));

    if (confreg & DIAG_MASK) {
	puts("\nReading cookie ...");
    }
    cookiep = get_cookie();

    /*
    ** Special code for manufacturing.
    ** New platforms will have the cookie information programmed into
    ** the beginning of NVRAM during ICT.  This code makes sure the
    ** backup copy is valid too.
    */
    if(nvcookie_valid(&(NVRAM)->nvcookie) &&
	bcmp(&(NVRAM)->nvcookie, &(NVRAM_SAV)->nvcookie_bkup,
	     sizeof(struct nvcookie)) != 0)
	put_cookie(cookiep);

    ciscopro_flag = (cookiep->processor & CISCOPRO_FLAG);
    cookiep->processor &= ~CISCOPRO_FLAG; /* un-mask the real processor type */

    /* 
     * if we have a bad cookie make us boot to the rommon.
     */
    if (get_1000_hardware_type((struct cookie *)
				&(NVRAM_SAV)->nvcookie.cookie) <= 0) {
	printf(" unable to init 360 pins -- please set up cookie info\n");
	confreg = 0x0;		/* force us to go into ROMMON instead */
				/* of booting */
	NV_WR_CONFREG(confreg);
	(NVRAM)->confreg = confreg;
    }

    (NVRAM)->testarea = 0x0;
    hkeepflags = 0x0;       /* clear housekeeping flags */
    monjmpptr = 0;          /* initialize monjmp pointer */
    sys_state = MONITOR_RUNNING;
    initsigs();             /* set all signals to default */
    clrerrlog();

    /*
    ** Log the reason for getting here
    */
    if(rsr_save & QUICC_RSR_SW) {
	/* this is a software watchdog reset */
	log_sys_ret(EXC_WATCHDOG, (cpu_context_t *)0);
    } else if(rsr_save & (QUICC_RSR_POW | QUICC_RSR_EXT)) {
	/* power-up or reset condition */
	log_sys_ret(EXC_RESET, (cpu_context_t *)0);
	clr_autoboot_state();
    }
    pet_the_dog();


#ifndef ONBOARD_FLASH
    /*
    ** Set up the PCMCIA interface chip.
    ** Use socket 1 and window 0.
    */
    if (confreg & DIAG_MASK) {
      puts("\nInitializing the PCMCIA controller ...");
    }
    pcmcia_init();
   
    clpd6720_set_window(1, 0, 0x0000000, /* start */
                        0x3ff0000,       /* offset */
                        0x800000,        /* pcmcia size */
                        CLPD6720_COMMON_MEM,
                        CLPD6720_WIDTH_16_BIT,
                        CLPD6720_READ_WRITE);

    clpd6720_enable_window(1, 0);
    clpd6720_enable_socket(1);
    pcmcia_siz = 0x800000;  /* FIXME hard coded size - same as above */
#endif

    if (confreg & DIAG_MASK){ 
	puts("\nInitializing the PIT timer ...");
    }
    timer_init();

    /*
    ** No need to pet the dog any more since the timer routine
    ** will do this
    */

#ifdef FIXME_TP3420_TEST
    if (confreg & DIAG_MASK){ 
	puts("\nTesting the tp3420 ...");
    }
    if(tp3420_test() < 0)
	puts(" TEST FAILED");
#endif /* FIXME_TP3420_TEST */
    
    /*
    ** copy the exception vectors to the base of RAM
    ** for use by the system image.
    */
    if (confreg & DIAG_MASK){ 
	puts("\nInitializing the RAM vector table ...");
    }
    bcopy((char *)&evec, (char *)ADRSPC_RAM,
	  (int)((long)&evece - (long)&evec));

    /*
    ** Note the Sapphire monitor does not use main.o
    ** All initiallization is done here and the monitor
    ** is invoked from brkpt_isr() (trap #1)
    */
    if (confreg & DIAG_MASK){ 
	puts("\nInitializing monitor variables ...");
    }
    initdevtbl();
    init_ml_iface();  /* init the monlib interface */

    /*
    ** We are doing fixed_flash_init() here because we need monitor variables 
    ** UMI is the Sapphire with on-board flash size 4 MB fixed.
    ** This initialization is UMI hardware specific
    */
#ifdef ONBOARD_FLASH
    if (confreg & DIAG_MASK) {
      puts("\nInitializing the on-board flash ...");
    }
    fixed_flash_init();
#endif

    if (confreg & DIAG_MASK){ 
	putchar('\n');
    }

    /*
    ** We leave interrupts disabled in the monitor since
    ** we don't use them
    */

    if (confreg & DIAG_MASK) {
	pr_reset_cause(rsr_save);
    }
    return(0);
}

/*
** Pet the [watch]dog so he doesn't bark
*/
void
pet_the_dog(void)
{
    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    qir_p->sim_swsr = QUICC_SWSR_VAL1;
    qir_p->sim_swsr = QUICC_SWSR_VAL2;
}

/*
** Init the quicc chip
** This is done once from init()
** Many of the init values are defined in pcmap.h
*/
int
init_quicc(int wd_on)
{
    int rsr_save;
    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;
    int hardware_type;

    qir_p->sim_mcr = QUICC_MCR_INIT_VAL;  /* see pcmap.h file for defines */
    qir_p->sim_pllcr = QUICC_MODCLK1_1_VAL;
    qir_p->sim_swiv = QUICC_SWIV_VAL;     /* default (uninitialized vector) */
    if(wd_on) {                              /* turn the watchdog on (8 sec) */
	pet_the_dog();                       /* pet him before turning on */
	qir_p->sim_pitr = QUICC_PITR_SWP(1); /* prescaler on */
	qir_p->sim_sypcr = QUICC_SYPCR_VAL;  /* see pcmap.h */
	pet_the_dog();                       /* and after */
    } else {
	qir_p->sim_sypcr = QUICC_SYPCR_NWD_VAL;  /* no WD timer */
    }
    qir_p->cp_cr = (QUICC_CPCR_RST | QUICC_CPCR_SEMA);
    qir_p->sim_pepar = QUICC_PEPAR_VAL;
    qir_p->sim_avr = QUICC_AVR_VAL;
    qir_p->memc_gmr = QUICC_GMR_1M_VAL;

    qir_p->memc_or1 = QUICC_OR1_4MEG_VAL;  /* DRAM 02000000 - 023fffff 4 Meg (int DSACK) */
    qir_p->memc_br1 = QUICC_BR1_VAL;

    qir_p->memc_or2 = QUICC_OR2_VAL;  /* upper 4m of 8m SIMM */
    qir_p->memc_br2 = QUICC_BR2_VAL;

    /* chip select 3 is not used on this platform */

    qir_p->memc_or4 = QUICC_OR4_VAL;  /* PCMCIA Register space (06000000 - 0600ffff) */
    qir_p->memc_br4 = QUICC_BR4_VAL;

    qir_p->memc_or5 = QUICC_OR5_VAL;  /* PCMCIA common memory (08000000 - 08ffffff) */
    qir_p->memc_br5 = QUICC_BR5_VAL;

    qir_p->memc_or6 = QUICC_OR6_VAL;  /* PCMCIA I/O space (0C000000 - 0C00ffff) */
    qir_p->memc_br6 = QUICC_BR6_VAL;

    qir_p->memc_or7 = QUICC_OR7_32K_VAL;  /* NVRAM space (0E000000 - 0E007fff) */
    qir_p->memc_br7 = QUICC_BR7_VAL;

    qir_p->memc_br0 = QUICC_BR0_VAL;
    qir_p->memc_or0 = QUICC_OR0_VAL;  /* ROM  00000000 - 1ffff (internal DSACK) */
    qir_p->cpic_cicr = QUICC_CICR_VAL;

    /* 
     *   retrieve and clear the reset status register
     */
    rsr_save = qir_p->sim_rsr;
    qir_p->sim_rsr = 0xff;  /* clear all bits */
    
    /*
     * reset the cp
     */
    qir_p->cp_cr = (QUICC_CPCR_RST | QUICC_CPCR_SEMA);
    while(qir_p->cp_cr & QUICC_CPCR_SEMA) ;  /* FIXME - time this */
    
    qir_p->sdma_sdcr = QUICC_SDCR_VAL;
    /* sdma arbitration id 4 */
    
    qir_p->cpic_cimr = 0;    		/* mask all intr's */
    qir_p->cpic_cipr = 0xffffffff;	/* clear intr pendings */
    qir_p->cpic_cisr = 0xffffffff;	/* clear intr service */
    qir_p->si_sicr = 0x00;		/* reset clock routing pins */

    /* 
     * do pin initializationn
     * first check for valid main cookie 
     */
    hardware_type = CONSOLE_PIN_INIT;
    if (nvcookie_valid(&(NVRAM_SAV)->nvcookie)) {
	hardware_type = get_1000_hardware_type((struct cookie *)
					       &(NVRAM_SAV)->nvcookie.cookie);
    } else {	/* whoops main copy of cookie is bad lets try secondary copy */
	if (nvcookie_valid(&(NVRAM_SAV)->nvcookie_bkup)) {
	    hardware_type = 
		get_1000_hardware_type((struct cookie *)
				       &(NVRAM_SAV)->nvcookie_bkup.cookie);
	}
    }
    quicc_pin_initialization(hardware_type);

    /* make sure the hardware break(watch)point register is clear */
    qir_p->sim_bkcr = 0;
    return(rsr_save);
}

/*
** Routines to save and restore the state of the System OK LED
** on the front panel.
*/
static unsigned sys_ok_led;

void
sav_sys_ok_led(void)
{
    sys_ok_led = (((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->pip_pbdat &
	   SAP_QU_PT_B_PAR_RUNLED);
}

void
restore_sys_ok_led(void)
{
    ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->pip_pbdat = 
	(((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->pip_pbdat &
	    ~SAP_QU_PT_B_PAR_RUNLED) | sys_ok_led;
}

/*
** Sapphire has the ability to write protect NVRAM.
** We take advantage of this and only write enable it when
** necessary.
*/
void
wr_ena_nvram(void)
{
    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    qir_p->memc_br7 &= ~QUICC_BR_WR_PROT;
}

void
wr_prot_nvram(void)
{
    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    qir_p->memc_br7 |= QUICC_BR_WR_PROT;
}

/*
** Write the confreg value to NVRAM.  Do *not* update the
** in-core copy.
** The tmp_nv structure herein mirrors the relevant portion of the
** actual nvram structure.
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
** Dummy up a cache_flush routine.
*/
void
cache_flush(void)
{
    /* this processor has no cache */
    return;
}

/*
** Clear the pending interrupts on the tp3420
** ISDN iface chip
*/
void
clr_tp3420_ints(void)
{
    spi_open();
    spi_cmd(TP3420_CRF_NOP);
    spi_close();
}

int sid_int_count;
/*
** Provide an ISR for the tp3420 chip
** We cannot disable interrupts on this puppy (stupid hardware)
** so we must be prepared to handle an interrupt
*/
void
sid_int(void)
{
    clr_tp3420_ints();
    sid_int_count++;
}

static struct reset_item {
    unsigned char chan;
    unsigned char cmd;
} reset_info[] = {
    QUICC_CHAN_SCC1, QUICC_CPC_TX_STOP,
    QUICC_CHAN_SCC1, QUICC_CPC_RX_CLOSE_BD,
    QUICC_CHAN_SCC2, QUICC_CPC_TX_STOP,
    QUICC_CHAN_SCC2, QUICC_CPC_RX_CLOSE_BD,
    QUICC_CHAN_SPI,  QUICC_CPC_RX_CLOSE_BD,
    QUICC_CHAN_SCC3, QUICC_CPC_TX_STOP,
    QUICC_CHAN_SCC3, QUICC_CPC_RX_CLOSE_BD,
    QUICC_CHAN_SCC4, QUICC_CPC_TX_STOP,
    QUICC_CHAN_SCC4, QUICC_CPC_RX_CLOSE_BD,
};
#define RSTCMDSIZ (sizeof(reset_info)/sizeof(struct reset_item))

void
reset_io(void)
{
    int i;
    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    /* kill all SCC transmitters and receivers */
    for(i=0; i<RSTCMDSIZ; i++)
	quicc_cr_cmd( reset_info[i].chan, reset_info[i].cmd);

    for(i=0; i<4; i++)
	qir_p->scc[i].scc_gsmrl &= ~(QUICC_SCC_GSMRL_ENR | QUICC_SCC_GSMRL_ENT);

    /* mask all QUICC interrupt sources */
    qir_p->cpic_cimr = 0;

    /* hammer any possible existing ints */
    qir_p->cpic_cisr = qir_p->cpic_cisr;

#ifndef ONBOARD_FLASH
    /* turn off interrupts at the pcmcia controller chip */
        clpd6720_disable_ints();
#endif
}

long
sizepcmcia(void)
{
    if(chkberr(ADRSPC_PCMCIA_MEM, BW_8BITS, 1) != 0)
	return(0);
    return(pcmcia_siz);
}

/* dummy for the new compilers */
__main()
{
    return;
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

/*
 * put_int_handler:
 * install the handler desired at the vector number desired
 * Input:
 *      dpr: pointer to the quicc dual port ram
 *      int_number: this is the offset from the vector base which is stored in
 *                  cpic_cicr register
 *      int_handler: the handler routine in .S file
 */
void
put_int_handler(quicc_dpr_t *dpr, int int_number, long *int_handler)
{
    quicc_internal_regs_t *qir_p; /* quicc internal reg */
    ulong int_vec_base;
    int vbr_location;
    int int_vector;
    long *int_handler_location;

    qir_p = &(dpr->regs);
    int_vec_base = qir_p->cpic_cicr & QUICC_VBA_MASK; /* MC68360 manual page 7-370 */
    int_vector   = int_vec_base + int_number;   /* get interrupt vector */
    vbr_location = getvbr();   /* starting location of exception vector table */
    int_handler_location = (long *)(vbr_location + (sizeof(long) * int_vector));
    *int_handler_location = (long)int_handler; /* store the handler */
}

void quicc_pin_initialization(int hardware_type)
{
    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;
   /*
    ** Initialize the QUICC ports
    ** See pcmap.h for platform specific defines.
    */
    
    qir_p->pio_papar = pin_init_array[hardware_type].papar;
    qir_p->pio_padat = pin_init_array[hardware_type].padat;
    qir_p->pio_padir = pin_init_array[hardware_type].padir;
    qir_p->pio_paodr = pin_init_array[hardware_type].paodr;
    
    qir_p->pip_pbpar = pin_init_array[hardware_type].pbpar;
    qir_p->pip_pbdir = pin_init_array[hardware_type].pbdir;
    qir_p->pip_pbdat = pin_init_array[hardware_type].pbdat;
    qir_p->pip_pbodr = pin_init_array[hardware_type].pbodr;

    qir_p->pio_pcpar = pin_init_array[hardware_type].pcpar;
    qir_p->pio_pcdir = pin_init_array[hardware_type].pcdir;
    qir_p->pio_pcso  = pin_init_array[hardware_type].pcso;
    qir_p->pio_pcdat = pin_init_array[hardware_type].pcdat;
}
/*
 * get_1000_hardware_type
 * 
 * return hardware type based on cookie
 */
int get_1000_hardware_type(struct cookie *cookie) 
{
    if (cookie) {
	switch(cookie->interfaces) {
	  case INTERFACE_1E1T:
	    return(CATWOMAN_HARDWARE);
	  case INTERFACE_1E1B:
	  case INTERFACE_1E1N:
	    return(SAPPHIRE_HARDWARE);
	  default:
	    return(CONSOLE_PIN_INIT);
	}
    } 
    return(CONSOLE_PIN_INIT);
}

#ifdef ONBOARD_FLASH

/* Umi - onboard flash specific code begins here. So does Hana */

/* The flash: device is replaced by the following device */

struct devstruct onboard_flash = {
    DEV_FLASH_ID, "flash", (char *)ADRSPC_FLASH_BASE, 0,
         (char *)0, (unsigned)DEV_FS_CAPABLE, sizeflash,
         FILE_ACCESS_FLASH
};

/*  Initialization for onboard-flash                                  */
/* The Umi specific part is the definition of BR/OR and port C pins   */
/* which are dependent on Hardware design. see pcmap.h                */


void fixed_flash_init(void)
{
    struct devstruct *devptr;
    int size,mask;
    int i,len;
    int hardware_type;

    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    hardware_type = get_1000_hardware_type((struct cookie *)
                                               &(NVRAM_SAV)->nvcookie.cookie);

    qir_p->memc_or5 = QUICC_FIXED_OR5_VAL; /* Flash (08000000-08ffffff) */
    qir_p->memc_br5 = QUICC_FIXED_BR5_VAL;

    switch(hardware_type) {
      case SAPPHIRE_HARDWARE: /* UMI */
        qir_p->pio_pcdat |= QU_PT_C_FIXED_VPP_CTRL_UMI;
        qir_p->pio_pcdir |= QU_PT_C_FIXED_VPP_CTRL_UMI;
        break;
      case CATWOMAN_HARDWARE: /* HANA */
        qir_p->pio_pcdat |= QU_PT_C_FIXED_VPP_CTRL_HANA;
        qir_p->pio_pcdir |= QU_PT_C_FIXED_VPP_CTRL_HANA;
        break;
      default:
        break;
      }


    /* Find the flash: and replace it with onboard flash routines */

    devptr = devtbl;
    for(i=0; i<numdevstructs; i++) {
        len = strlen(devptr->devid);
        if(strncmp(devptr->devid, onboard_flash.devid, len) == 0) {
             break;
        }
        devptr++;
    }
    bcopy((unsigned char *)&onboard_flash, (unsigned char *)devptr, sizeof(struct devstruct));

}
#endif

#if defined(FUTURE)
#include "../les/if_table.h"

int check_interface(unsigned short interface, char *loadname)
{
    unsigned short cookie_int;
    struct cookie *cookiep;

    if (strncmp(loadname, DEV_EPROM_ID, strlen (DEV_EPROM_ID)) != 0) {
	cookiep = get_cookie();
	cookie_int = interface_bit_table[cookiep->interfaces];
#if defined(DEBUG)
	printf("cookie_int = 0x%x interface = 0x%x\n",cookie_int,interface);
#endif /* defined(DEBUG) */
	if (interface == cookie_int) {
	    return(0);
	}else {
	    return(1);
	}
    } else {
#if defined(DEBUG)
	printf("booting eprom image of code no interface check\n");
#endif /* defined(DEBUG) */
	return(0);
    }
}
#endif /* defined(FUTURE) */

/* end of module */
