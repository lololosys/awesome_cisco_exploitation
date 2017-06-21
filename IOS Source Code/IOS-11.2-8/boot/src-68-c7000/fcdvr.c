/* $Id: fcdvr.c,v 3.1 1995/11/09 09:10:12 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/fcdvr.c,v $
 *------------------------------------------------------------------
 * mcdvr.c - MC and MC+ card Flash and NVRAM driver support
 *
 *
 *      July 1991, Howard Straus
 *      Aug 1991, Darren Kerr
 *
 *           MCI-specific flash routines pulled from Chris Shaker
 *           and Steve Elias' "mc.c" (now "mc.c" contains only
 *           high-level flash routines, while "mcdvr.c" and "ec.c"
 *           contain low-level flash drivers).
 *
 *	Copyright (c) 1991-1993 by cisco Systems, Inc.
 *	All rights reserved.
 *------------------------------------------------------------------
 * $Log: fcdvr.c,v $
 * Revision 3.1  1995/11/09  09:10:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:20  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1993/01/30  01:11:47  whp4
 * Add RP1 support.
 *
 *
 * $Endlog$
 */

#include "../stdio.h"
#include "../defs.h"
#include "../i8254.h"
#include "fldvr.h"
#include "../reason.h"
#include "../extern.h"
#include "../ctype.h"
#include "pcmap.h"

#define CONTROL_REG (ushort *)ADRSPC_CONTROL
#define FLASH_BASE ADRSPC_EXT_FLASH
#define DEBUG
#define XDEBUG

char fc_not_erase_msg[] = "flash EPROMs will NOT erase...aborting\n";
int flash_size;
void fc_assert_vpp(), fc_deassert_vpp(), fc_usecdelay(unsigned), fc_wastetime(),
     fc_flash_load(), fc_flash_test(), fc_notime(), fc_rnvscope_loop(), fc_nvscope_loop();

/*
 * Manufacturer codes table:
 */
#undef MAX_ROM_MFGS
struct prom_table fc_proms[] = {
    {(ushort)0x89B4, 0x200000L, "INTEL 28F010"}, 
    {(ushort)0x89BD, 0x400000L, "INTEL 28F020"}, 
    {(ushort)0x01A7, 0x200000L, "AMD 28F010"}, 
    {(ushort)0x012A, 0x400000L, "AMD 28F020"}, 
    {(ushort)0x1CD0, 0x400000L, "M5M28F101P"}, 
};
/*
 * codes table with bits reversed the way the processor really
 * sees them
 */
#ifdef DONTREALLYNEEDTHIS
struct prom_table bwkproms[] = {
    {(ushort)0x912d, 0x200000L, "INTEL 28F010"}, 
    {(ushort)0x91BD, 0x400000L, "INTEL 28F020"}, 
    {(ushort)0x80E5, 0x200000L, "AMD 28F010"}, 
    {(ushort)0x8054, 0x400000L, "AMD 28F020"},
    {(ushort)0x1CD0, 0x400000L, "M5M28F101P  "},
};
#endif
#define MAX_ROM_MFGS (sizeof(fc_proms)/sizeof(struct prom_table))


/***************************************************************************
 *
 *                Flash Support Routines
 *
 ***************************************************************************/
    

/*
 * get_rom_size
 * Given a manufacturer/device code, return its size.
 * Return 0 for unknown devices.
 */

long 
fc_get_rom_size(ushort code)
{
    int i;

    for (i = 0; i < MAX_ROM_MFGS; i++) {
#ifdef XDEBUG
	printf("comparing 0x%x and 0x%x\n",fc_proms[i].manu_dev_code, code);
#endif
	if (fc_proms[i].manu_dev_code == code)
	    return(fc_proms[i].device_size);
    }
    return(0);
}


/*
 * get_rom_name
 * Given a manufacturer/device code, return its name string.
 * Return NULL for unknown devices.
 */
char 
*fc_get_rom_name(ushort code)
{
    int i;

    for (i = 0; i < MAX_ROM_MFGS; i++)
	if (fc_proms[i].manu_dev_code == code)
	    return(fc_proms[i].name);
    return(NULL);
}


void 
fc_query_roms(void)
{
    ushort manu, device, temp;
    ushort *lp;

    fc_assert_vpp();

    lp = (ushort *)FLASH_BASE;
    *lp = RESET | (RESET << 8);
    fc_usecdelay(6);
    *lp = RESET | (RESET << 8);
    fc_usecdelay(6);
    *lp = READ_ID_CODES | (READ_ID_CODES << 8);
    manu = *lp & 0xff;
    device = *(lp+1) & 0xff;
#ifdef XDEBUG
    printf("Manu code = 0x%2x, device code = 0x%2x\n",manu,device);
#endif
    temp = (manu << 8) | device;

    *lp = RESET | (RESET << 8);

    fc_deassert_vpp();

    if((flash_size = fc_get_rom_size(temp)) != 0)
	printf("fprom code = 0x%4x, size = 0x%x, part = %s\n",temp,
	       flash_size, fc_get_rom_name(temp));
    else {
	printf("invalid fprom code 0x%4x\n",temp);
	return;
    }
    return;
}


/*
 * erase_roms
 * Erase the flash EPROMs.
 * Return TRUE if EPROMs are successfully erased, else, return FALSE.
 */

boolean 
fc_erase_roms(void)
{
    int erasepulse, i;
    int b0_erasepulse, b1_erasepulse, b2_erasepulse, b3_erasepulse; 
    uint size, bank;
    volatile ushort *ptr, *bankstart, *end;
    ushort data, flashcmd;
    boolean retval = TRUE;  /* optimism */

#define FC_NUMBANKS 8
#define MAXERASE 3000

    fc_assert_vpp();

    /*
     * Program all locations to zero before erase
     */
#ifdef DEBUG
    puts("\nZeroing flash EPROMs for erase...");
#endif
    bankstart = (ushort *)FLASH_BASE;  /* base of flash EPROM */
    end = (ushort *)(FLASH_BASE + flash_size);
/* Added this code to support one bank of flash... delete when fully pop */
/*    size = flash_size / FC_NUMBANKS; */
/*    end = (ushort *)(FLASH_BASE + 4*size); */ /* end of current bank */
/* Added this code to support one bank of flash... delete when fully pop */
    for ( ptr = bankstart; ptr < end; ptr++ ) {
	if( (*ptr & 0xffff) != 0 ) {
	    if( !fc_program_rom(0, ptr) ) {  /* || *ptr != 0 ) { */
		printf("\nflash address 0x%x would not zero for erase",ptr);
		goto fc_badreturn;
	    }
	}
#ifdef DEBUG
	if( !((ulong)ptr % 0x10000)) putchar('z');
#endif
    }
    /*
     * Erase a bank at a time
     */

/* lets stop here before verifying */
/* return; */
/* lets stop here before verifying */

    size = flash_size / FC_NUMBANKS;
    for (bank = 0; bank < FC_NUMBANKS; bank++ ) {
/*    for (bank = 0; bank < 4; bank++ ) { */
#ifdef DEBUG
	printf("\nVerifying flash EPROMs bank %d...",bank);
#endif
	/* sanity check again -- should be all 0s before we erase */
	bankstart = (ushort *)(FLASH_BASE + size*bank);  /* base of this bank */
	end = (ushort *)(FLASH_BASE + (bank+1)*size);  /* end of current bank */
	for ( ptr = bankstart; ptr < end; ptr++) {
 	    data = *ptr;
 	    if ( (data & 0xffff) != 0 ) {
		printf("\nflash sanity error:  0x%x: expected 0x00000000   actual 0x%8x\n", 
		       ptr, data);
		printf("10 more reads from address 0x%x: ", ptr);
		for ( i = 0; i < 10; i++ ) printf("0x%8x ", *ptr);
		goto fc_badreturn;
	    }
#ifdef DEBUG
	    if( !((ulong)ptr % 0x10000)) putchar('v');
#endif
	}

/* My extra code to stop this test before erasing begins */
/*    putchar('\n');
    *bankstart = READ_MEMORY | (READ_MEMORY << 8);
    fc_deassert_vpp();
    retval = TRUE;
    return(retval); */
/* My extra code to stop this test before erasing begins */

#ifdef DEBUG
	printf("\nErasing flash EPROMs bank %d...",bank);
#endif
	erasepulse = 0;
        b0_erasepulse = 0;
        b1_erasepulse = 0;
        flashcmd = 0xffff;
	ptr = bankstart;
	while( erasepulse++ < MAXERASE ) {  /* allowable erase pulses */
#ifdef DEBUG
	 if( !(erasepulse % 10) ) putchar('x');
	 if( !(b0_erasepulse % 10) ) putchar('0');
	 if( !(b1_erasepulse % 10) ) putchar('1');
#endif
	    /* write erase set-up command for entire bank */
	    *bankstart = flashcmd;
	    /* write erase command for entire bank */
	    *bankstart = flashcmd;
	    fc_usecdelay(10000);  /* 10ms delay */
	    while( ptr < end ) {  /* verify location by location */
asm("	trighere:");
		*ptr = ERASE_VERIFY | (ERASE_VERIFY << 8);
		fc_usecdelay(6);
		data = *ptr;
                flashcmd = 0xffff;
		if( (data & 0xffff) != 0xffff ) {
                   if ( (data & 0xff) != 0xff) {
                      flashcmd &= 0xff20;
                      b0_erasepulse++;
                   }
                   if ( ( (data >> 8) & 0xff) != 0xff) {
                      flashcmd &= 0x20ff;
                      b1_erasepulse++;
                   }

                   break;  /* verify failed */
                }
#ifdef DEBUG
		if( !((ulong)ptr % 0x10000)) putchar('e');
#endif
		ptr++;
	    }
	    if(ptr == end) {
               printf("bank successfully erased\n");
               break;  /* this bank successfully erased */
            }
	}
	if(ptr != end) {
          printf("erasepulse = %d\n", erasepulse);
          break;  /* we ran out of erase pulses */
        }
    }
    if(ptr != end) {
	printf("\nerase failed at location 0x%x",ptr);
        printf("\nend of bank 0 I think... bank = %d\n", bank);
fc_badreturn:
	retval = FALSE;
        printf("retval is set to false\n");
    }
    *bankstart = READ_MEMORY | (READ_MEMORY << 8);
    printf("leaving erase_roms... retval is %d\n", retval);
    fc_deassert_vpp();
    putchar('\n');
    return(retval);
}


/*
 * program_rom
 * Write the specified value to flash EPROM at the specified address
 * vpp must be asserted before calling program_rom()
 */
boolean 
fc_program_rom(ushort data, volatile ushort *ptr)
{
    int writepulse;
    ushort readback;

    for( writepulse = 0; writepulse < 25; writepulse++) {
	*ptr = PROGRAM | (PROGRAM << 8);
	*ptr = data;
	fc_usecdelay(10); 
	*ptr = PROGRAM_VERIFY | (PROGRAM_VERIFY << 8);
	fc_usecdelay(6);
	readback = *ptr;
	if ( (readback & 0xffff) == (data & 0xffff) ) break;
    }
    *ptr = READ_MEMORY | (READ_MEMORY << 8);
    if( (readback & 0xffff) == (data & 0xffff) ) return(TRUE);
    printf("\nprogram_rom: location 0x%x wrote 0x%8x, contains 0x%8x\n",
	   ptr, data, readback);
    return(FALSE);
}


/*
 * assert_vpp
 * Assert VPP on the flash EPROMs and wait for the voltage to stabilize:
 */

void fc_assert_vpp(void)
{
    *CONTROL_REG |= CONTROL_FLASH_VPP;
    fc_usecdelay(100000); /* Delay 100ms to allow VPP to ramp up */
#ifdef XDEBUG
    printf("\n************* VPP ON **************\n");
#endif
}


/*
 * deassert_vpp
 * Deassert VPP on the flash EPROMs.
 */

void fc_deassert_vpp(void)
{
    *CONTROL_REG &= ~CONTROL_FLASH_VPP;
    fc_usecdelay(100000); /* Delay 100ms to allow VPP to ramp up */
#ifdef XDEBUG
    printf("\n************* VPP OFF *************\n");
#endif
}

#ifdef LATERMAYBE
/*
 * badpart(addr, expected)
 * print out (U-something) for a possible bad flash part.
 * return the starting address of the next rom or rom pair.
 */

int fc_badpart(int address, ushort expected)
{
}
#endif

/*
 * wait at least n microseconds 
 * by watching 8254 timer counter 1 while looping.
 * the sequence of instructions is critical in this module due to the
 * slow internal workings of the 8254 chip.
 * especially important is the time between writing the control register
 * and reading something back.
 */
void fc_usecdelay(unsigned n)
{
    unsigned short tmp;

    if( n <= 10 ) {
        if(n <= 2) return;
	fc_wastetime(0);
        if(n <= 3) return;
	fc_wastetime(0);
        if(n <= 4) return;
	fc_wastetime(0);
        if(n <= 5) return;
	fc_wastetime(0);
        if(n <= 6) return;
	fc_wastetime(0);
        if(n <= 7) return;
	fc_wastetime(0);
        if(n <= 8) return;
	fc_wastetime(0);
        if(n <= 9) return;
	fc_wastetime(0);
        if(n <= 10) return;
    }
    n -= 10;  /* subtract some overhead amount */
    while(n) {
	tmp = n & 0x7fff;
	if(tmp == 0) tmp = 0x8000;
	n -= tmp;
/*	if(tmp < 0) continue; */
/* tmp = ((tmp >> 1) + (tmp >> 2)); */  /* take ~75% of the value */
	tmp += 0x4000;  /* pad this amount so timer never times out */
	ctaddr->control = SEL_COUNTER1 | RW_LSBMSB | CT_MODE4;
	ctaddr->counter1 = tmp;  /* program lsb */
	ctaddr->counter1 = tmp >> 8;  /* program msb */
	fc_wastetime(2);  /* give timer a moment to load */
	while(tmp > 0x4000) {  /* break if below our pad value */
	    ctaddr->control = SEL_COUNTER1 | CT_MODE4;  /* latch counter 1 */
	    fc_wastetime(2);  /* give timer a moment to latch count */
	    tmp = (ctaddr->counter1 & 0xff);  /* get lsb */
	    tmp |= (ctaddr->counter1 << 8);  /* get msb */
	}
    }
    ctaddr->control = SEL_COUNTER1;  /* stop counting */
}

void
fc_notime()
{
}

void
fc_wastetime(n)
unsigned short n;
{
for(; n>0; n--);
}

/*
 * Beep the terminal 'n' times.
 */
fc_bell(int n)
{
    while(n) {
	putchar('\007');
	fc_usecdelay(2000);
	n--;
    }
}

/*
 * flash_test
 * Test the flash EPROMs. Do read/write test. Let user know stats on each
 * part.  Provide simple "data equals address" test routine (flash_test)
 * and an extended routine with all ones and all zeros tests (eflash_test).
 */
/* #ifdef USEEXTENDEDTEST  *** WHY IS THIS HERE !!!!! **** */
void fc_flash_test()
{
    uint i,j, size;
    char buffa[16];
    ushort *lp,t1,t2,*ptr;
    extern ulong clock;
    extern char fc_not_erase_msg[];

#define FC_NUMBANKS 8

#ifdef DEBUG
    printf("Flash Test!\n");
#endif

    fc_query_roms(); 

    if (!flash_size) {
	printf("Flash not available...aborting test\n");
	bell(5);
	return;
    }


#ifdef NOWAY
    rdtty(buffa, 16);
    if ( (buffa[0] & 0xdf) != 'Y' ) {
	printf("flash eprom test aborted.\n");
	return;
    }
#endif NOWAY

/* return after checking manu id from flash card */
/* return; */
/* return after checking manu id from flash card */

    printf("\nerasing...  ");
    if ( !fc_erase_roms() ) {
	puts(fc_not_erase_msg);
	fc_bell(5);
	return;
    }

/* Stopped just after erasing the flash */
/* return; */
/* Stopped just after erasing the flash */

    puts("\n\"data equals address\" test...  writing...  ");
    fc_assert_vpp();

/* size = flash_size / FC_NUMBANKS; */
    for ( i = FLASH_BASE; i < FLASH_BASE+flash_size; i += 2 ) {
	fc_program_rom((short)(i & 0xffff), (ushort *)i ) ;
#ifdef DEBUG
	if( !(i % 0x10000)) putchar('w');
#endif
    }
    fc_deassert_vpp();

    printf("\nverifying...  ");

    for ( i = FLASH_BASE; i < FLASH_BASE+flash_size; i += 2 ) {
/*   for ( i = FLASH_BASE; i < FLASH_BASE+(4*size); i += 2 ) { */
	lp = (ushort *)i;
	if ( (*lp & 0xffff) != (i & 0xffff) ) {
	    printf("\nTEST VERIFICATION FAILURE!  %8x: %8x\n", 
		   i, (*lp & 0xffff) );
	    fc_bell(5);
	    return;
	}
    }
    printf("\nTest complete, no errors.\n");
    fc_bell(1);

}
/* #endif USEEXTENDEDTEST *********  END OF THIS */

ulong fc_testarray1[] = {
    0x55555555, 0xaaaaaaaa,
};

ulong fc_testarray2[] = {
    0xaaaaaaaa, 0x55555555,
};

boolean
fc_eflash_test()
{
    extern boolean ftest();

#ifdef DEBUG
    puts("Extended Flash Test!\n");
#endif

    fc_query_roms(); 

    if (!flash_size) {
	printf("Flash not available...aborting test\n");
	fc_bell(5);
	return(FALSE);
    }


#ifdef NOWAY
    rdtty(buffa, 16);
    if ( (buffa[0] & 0xdf) != 'Y' ) {
	printf("flash eprom test aborted.\n");
	return(FALSE);
    }
#endif NOWAY
    
    if( !fc_ftest(FLASH_BASE, flash_size, (long *)0, 0, "data equals address" ) ||
       !fc_ftest(FLASH_BASE, flash_size, fc_testarray1, 2, "0x55, 0xAA" ) ||
       !fc_ftest(FLASH_BASE, flash_size, fc_testarray2, 2, "0xAA, 0x55" ) ) {
	puts("\n\nTest Failed.\n");
	fc_bell(5);
	return(FALSE);
    }
	
    puts("\n\nTest complete, no errors.\n");
    fc_bell(1);
    return(TRUE);
}

boolean
fc_ftest(start, size, valptr, numvals, nameptr)
    volatile ushort *start, *valptr;
    char *nameptr;
{
    volatile ushort *ptr, *end, *vptr, *vend;
    ushort rval, value;
    extern char fc_not_erase_msg[];
    
    puts("\nerasing...  ");
    if ( !fc_erase_roms() ) {
	puts(fc_not_erase_msg);
	return(FALSE);
    }

    printf("\n\"%s\" test...  writing...  ", nameptr);
    fc_assert_vpp();

    end = (ushort *)((long)start + size);
    if( numvals ) vend = valptr + numvals;
    else {
	valptr = &value;
	vend = valptr + 1;
    }

    for ( ptr = start; ptr < end; ) {
	for(vptr = valptr; vptr < vend && ptr < end; vptr++, ptr++) {
	    if( !numvals ) *vptr = (long)ptr;  /* data equals address test */
	    if( !fc_program_rom( *vptr, ptr ) ) {
		printf("\nTEST WRITE FAILURE at location %8x\n", ptr);
		return(FALSE);
	    }
#ifdef DEBUG
	    if( !((ulong)ptr % 0x10000)) putchar('w');
#endif
	}
    }
    fc_deassert_vpp();

    puts("\nverifying...  ");

    for ( ptr = start; ptr < end; ) {
	for(vptr = valptr; vptr < vend && ptr < end; vptr++, ptr++) {
	    if( !numvals ) *vptr = (long)ptr;  /* data equals address test */
	    rval = *ptr;
	    if( rval != *vptr ) {
		printf("\nTEST VERIFICATION FAILURE at location %8x: expected %8x, is %8x\n",
		       ptr, *vptr, rval);
		return(FALSE);
	    }
#ifdef DEBUG
	    if( !((ulong)ptr % 0x10000)) putchar('v');
#endif
	}
    }
    return(TRUE);
}

fc_cflash_test()    /* continuous flash test */
{
    int pass = 0;
    int error = 0;

    while(1) {
	printf("Continuous flash test pass: %d, errors: %d\n", ++pass, error);
        /*
         * Cant do this until flash_test returns a boolean value
         * Figure out if you need to do this.....
         *
         * if( flash_test() == FALSE ) error++;
         * just put a single call to flash_test() in its place...
         */
        fc_flash_test();
    }
}

/*
 * flash_load
 * Load the boot PROMs into flash memory (this includes ROM monitor and
 * system code) and verify.
 */

void fc_flash_load()
{
    ushort i,*lp,*ptr;
    extern ulong clock;
    extern char fc_not_erase_msg[];
#define ROMSIZ 0x100000

    puts("\nLoad ROM to Flash w/Verify Test\n");
    fc_query_roms(); 

    if (!flash_size) {
	puts("Flash not available...aborting.\n");
	fc_bell(5);
	return;
    }
    if (flash_size < ROMSIZ) {
	puts("Flash too small...aborting.\n");
	fc_bell(5);
	return;
    }

    printf("\nerasing...  ");
    if ( !fc_erase_roms() ) {
	puts(fc_not_erase_msg);
	return;
    }
    puts("done.\nload ROM to flash...  ");
    fc_assert_vpp();
    ptr = (ushort *)0x1000000;   /* point to base of ROM */
    for ( i = FLASH_BASE; i < FLASH_BASE+ROMSIZ; i += 4 ) program_rom( *ptr++, (ulong *)i );
    fc_deassert_vpp();

    puts("and verify...  ");
    ptr = (ushort *)0x1000000;   /* point to base of ROM */
    for ( i = FLASH_BASE; i < FLASH_BASE+ROMSIZ; i += 4, ptr++ )  {
	lp = (ushort *)i;
	if ( *lp != *ptr ) {
	  printf("\nTEST VERIFICATION FAILURE!  %8x: sh be:%8x  is:%8x\n",i,
		 *ptr,*lp);
	  fc_bell(5);
	  return;
      }
    }
    puts("\nROM to Flash load complete, no errors.\n");
    fc_bell(1);
}

void
fc_delay_test()
{
    ulong time1 = 15;
    ulong time2 = 100;

    printf("scope triggers every %d and %d usecs.\n",time1,time2);

    while(1) {
	*(volatile ushort *)ADRSPC_CONTROL &= ~CONTROL_LAMP;
	fc_usecdelay(time1);
	*(volatile ushort *)ADRSPC_CONTROL |= CONTROL_LAMP;
	fc_usecdelay(time2);
    }
}
	
void
fc_rnvscope_loop()
{
   volatile uchar *nvram, a, b, c;

   a = 0xff;
   b = 0x33;
   nvram = (volatile uchar *) 0x11140000;
   printf("Reading NVRam from 0x%x \n", nvram);
   while(1)
     b = *nvram;
}

void
fc_nvscope_loop()
{
   volatile uchar *nvram, a, b, c;

   a = 0xff;
   b = 0x00;
   nvram = (volatile uchar *) 0x11140000;
   printf("Writing %x to NVRam\n", a);
   while(1)
     *nvram = a;
}

void
fc_read_flash_loop()
{
 volatile ushort *flash, here;

 flash = (volatile ushort *) 0x13000000;
 while(1) {
   here = *flash;
 }
}
#ifdef DONTNEEDANYMORE
void
fc_try_erase(void)
{
    int erasepulse, i;
    uint size, bank;
    volatile ulong *ptr, *bankstart, *end;
    ulong data;
    boolean retval = TRUE;  /* optimism */

#define FC_NUMBANKS 1
#define MAXERASE 3000

    fc_assert_vpp();

    /*
     * Program all locations to zero before erase
     */
#ifdef DEBUG
    puts("\nZeroing flash EPROMs for erase...");
#endif

    ptr = bankstart = (volatile ulong *)ADRSPC_FLASH_BASE;
    while(1) {
	/* write erase set-up command for entire bank */
	*bankstart = ERASE | (ERASE << 8) | (ERASE << 16) | (ERASE << 24);
	/* write erase command for entire bank */
	*bankstart = ERASE | (ERASE << 8) | (ERASE << 16) | (ERASE << 24);
	*ptr = ERASE_VERIFY | (ERASE_VERIFY << 8) |
	    (ERASE_VERIFY << 16) | (ERASE_VERIFY << 24);
	data = *ptr++;
	if( data != 0xfffffff1 ) ptr=bankstart;  /* verify failed */
    }
}
#endif DONTNEEDANYMORE


/*
 * all_flash_tests
 * this function calls all flash diagnostic routines
 */

void fc_all_flash_tests()
{

   char *c;
   char comm;
   char old_abortstate;

   reset_io();

   puts("\nFLASH CARD Diagnostic Routines\n");
   puts("B      UsecDelay test loop\n");
   puts("F      Flash test\n");
   puts("E      Extended flash test\n");
   puts("L      Load flash\n");
   puts("N      NV write Scope Loop\n");
   puts("R      NV read Scope Loop\n");
   puts("U      Assert VPP\n");
   puts("D      Deassert VPP\n");
   puts("M      Query Flash\n");
   puts("T      Loop on Flash Read\n");

   for (;;) {

        old_abortstate = abortstate;
        abortstate = ABORT_IGNORE;
        puts("i>>");
        gets(bufu.buf);
        abortstate = old_abortstate;

        c = bufu.buf;
        while (*c && isspace(*c))
            c++;                /* skip leading spaces */
        if (*c == '\0')
            continue;
        comm = *c;
        /* find command argument if present */
        while (*c && !isspace(*c) && *c != '/')
            c++;                /* skip rest of command */
        while (*c && isspace(*c))
            c++;                /* skip leading spaces */

        switch (comm) {

        case 'B':
	   fc_delay_test();
           break;

	case 'F':
           fc_flash_test();
           break;

	case 'E':
           fc_eflash_test();
           break;

	case 'L':
           fc_flash_load();
           break;

	case 'N':
           fc_nvscope_loop();
           break;

	case 'R':
           fc_rnvscope_loop();
           break;

	case 'U':
           fc_assert_vpp();
           break;

	case 'D':
           fc_deassert_vpp();
           break;

	case 'M':
           fc_query_roms();
           break;

	case 'T':
           fc_read_flash_loop();
           break;

	case 'H':
	case '?':
           puts("B      UsecDelay test loop\n");
           puts("F      Flash test\n");
           puts("E      Extended flash test\n");
           puts("L      Load flash\n");
           puts("N      NV write Scope Loop\n");
           puts("R      NV read Scope Loop\n");
           puts("U      Assert VPP\n");
           puts("D      Deassert VPP\n");
           puts("M      Query Flash\n");
           puts("T      Loop on Flash Read\n");
	   break;
	case 'Q':
	   return;
	   break;
	default:
           puts(illarg);
	   break;
        }

	flush();        /* Make sure that the UART is drained */
   }
}


