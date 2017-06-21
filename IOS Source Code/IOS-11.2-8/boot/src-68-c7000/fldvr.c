/* $Id: fldvr.c,v 3.1 1995/11/09 09:10:13 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/fldvr.c,v $
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
 * $Log: fldvr.c,v $
 * Revision 3.1  1995/11/09  09:10:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:22  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1993/01/30  01:11:48  whp4
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
#define FLASH_BASE ADRSPC_INT_FLASH
#define DEBUG
#define XDEBUG

char not_erase_msg[] = "flash EPROMs will NOT erase...aborting\n";
int flash_size;
void assert_vpp(), deassert_vpp(), usecdelay(unsigned), wastetime(),
     flash_load(), flash_test(), notime(), rnvscope_loop(), nvscope_loop();

/*
 * Manufacturer codes table:
 */
#undef MAX_ROM_MFGS
struct prom_table proms[] = {
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
#define MAX_ROM_MFGS (sizeof(proms)/sizeof(struct prom_table))


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
get_rom_size(ushort code)
{
    int i;

    for (i = 0; i < MAX_ROM_MFGS; i++) {
#ifdef XDEBUG
	printf("comparing 0x%x and 0x%x\n",proms[i].manu_dev_code, code);
#endif
	if (proms[i].manu_dev_code == code)
	    return(proms[i].device_size);
    }
    return(0);
}


/*
 * get_rom_name
 * Given a manufacturer/device code, return its name string.
 * Return NULL for unknown devices.
 */
char 
*get_rom_name(ushort code)
{
    int i;

    for (i = 0; i < MAX_ROM_MFGS; i++)
	if (proms[i].manu_dev_code == code)
	    return(proms[i].name);
    return(NULL);
}


void 
query_roms(void)
{
    ushort manu, device, temp;
    ulong *lp;

    assert_vpp();

    lp = (ulong *)FLASH_BASE;
    *lp = RESET | (RESET << 8) | (RESET << 16) | (RESET << 24);
    usecdelay(6);
    *lp = RESET | (RESET << 8) | (RESET << 16) | (RESET << 24);
    usecdelay(6);
    *lp = READ_ID_CODES | (READ_ID_CODES << 8) | (READ_ID_CODES << 16) |
	(READ_ID_CODES << 24);
    manu = *lp & 0xff;
    device = *(lp+1) & 0xff;
#ifdef XDEBUG
    printf("Manu code = 0x%2x, device code = 0x%2x\n",manu,device);
#endif
    temp = (manu << 8) | device;

    *lp = RESET | (RESET << 8) | (RESET << 16) | (RESET << 24);

    deassert_vpp();

    if((flash_size = get_rom_size(temp)) != 0)
	printf("fprom code = 0x%4x, size = 0x%x, part = %s\n",temp,
	       flash_size, get_rom_name(temp));
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
erase_roms(void)
{
    int erasepulse, i;
    int b0_erasepulse, b1_erasepulse, b2_erasepulse, b3_erasepulse; 
    uint size, bank;
    volatile ulong *ptr, *bankstart, *end;
    ulong data, flashcmd;
    boolean retval = TRUE;  /* optimism */

#define NUMBANKS 4
#define MAXERASE 3000

    assert_vpp();

    /*
     * Program all locations to zero before erase
     */
#ifdef DEBUG
    puts("\nZeroing flash EPROMs for erase...");
#endif
    bankstart = (ulong *)FLASH_BASE;  /* base of flash EPROM */
    end = (ulong *)(FLASH_BASE + flash_size);
/* Added this code to support one bank of flash... delete when fully pop */
    size = flash_size / NUMBANKS;
    end = (ulong *)(FLASH_BASE + 4*size); /* end of current bank */
/* Added this code to support one bank of flash... delete when fully pop */
    for ( ptr = bankstart; ptr < end; ptr++ ) {
	if( (*ptr) != 0 ) {
	    if( !program_rom(0, ptr) ) {  /* || *ptr != 0 ) { */
		printf("\nflash address 0x%x would not zero for erase",ptr);
		goto badreturn;
	    }
	}
#ifdef DEBUG
	if( !((ulong)ptr % 0x10000)) putchar('z');
#endif
    }
    /*
     * Erase a bank at a time
     */
    size = flash_size / NUMBANKS;
/*    for (bank = 0; bank < NUMBANKS; bank++ ) { */
    for (bank = 0; bank < 4; bank++ ) {
#ifdef DEBUG
	printf("\nVerifying flash EPROMs bank %d...",bank);
#endif
	/* sanity check again -- should be all 0s before we erase */
	bankstart = (ulong *)(FLASH_BASE + size*bank);  /* base of this bank */
	end = (ulong *)(FLASH_BASE + (bank+1)*size);  /* end of current bank */
	for ( ptr = bankstart; ptr < end; ptr++) {
 	    data = *ptr;
 	    if ( (data) != 0 ) {
		printf("\nflash sanity error:  0x%x: expected 0x00000000   actual 0x%8x\n", 
		       ptr, data);
		printf("10 more reads from address 0x%x: ", ptr);
		for ( i = 0; i < 10; i++ ) printf("0x%8x ", *ptr);
		goto badreturn;
	    }
#ifdef DEBUG
	    if( !((ulong)ptr % 0x10000)) putchar('v');
#endif
	}

/* My extra code to stop this test before erasing begins */
/*    putchar('\n');
    *bankstart = READ_MEMORY | (READ_MEMORY << 8) | (READ_MEMORY << 16) |
	(READ_MEMORY << 24);
    deassert_vpp();
    retval = TRUE;
    return(retval); */
/* My extra code to stop this test before erasing begins */

#ifdef DEBUG
	printf("\nErasing flash EPROMs bank %d...",bank);
#endif
	erasepulse = 0;
        b0_erasepulse = b1_erasepulse = 0;
        b2_erasepulse = b3_erasepulse = 0;
        flashcmd = 0xffffffff;
	ptr = bankstart;
	while( erasepulse++ < MAXERASE ) {  /* allowable erase pulses */
	 if( !(erasepulse % 10) ) putchar('x');
	 if( !(b0_erasepulse % 10) ) putchar('0');
	 if( !(b1_erasepulse % 10) ) putchar('1');
	 if( !(b2_erasepulse % 10) ) putchar('2');
	 if( !(b3_erasepulse % 10) ) putchar('3');
	    /* write erase set-up command for entire bank */
	    *bankstart = flashcmd;
	    /* write erase command for entire bank */
	    *bankstart = flashcmd;
	    usecdelay(10000);  /* 10ms delay */
	    while( ptr < end ) {  /* verify location by location */
asm("	trighere:");
		*ptr = ERASE_VERIFY | (ERASE_VERIFY << 8) |
		    (ERASE_VERIFY << 16) | (ERASE_VERIFY << 24);
		usecdelay(6);
		data = *ptr;
                flashcmd = 0xffffffff;
		if( (data) != 0xffffffff ) {
                   if ( (data & 0xff) != 0xff) {
                      flashcmd &= 0xffffff20;
                      b0_erasepulse++;
                   }
                   if ( ( (data >> 8) & 0xff) != 0xff) {
                      flashcmd &= 0xffff20ff;
                      b1_erasepulse++;
                   }
                   if ( ( (data >> 16) & 0xff) != 0xff) {
                      flashcmd &= 0xff20ffff;
                      b2_erasepulse++;
                   }
                   if ( ( (data >> 24) & 0xff) != 0xff) {
                      flashcmd &= 0x20ffffff;
                      b3_erasepulse++;
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
badreturn:
	retval = FALSE;
        printf("retval is set to false\n");
    }
    *bankstart = READ_MEMORY | (READ_MEMORY << 8) | (READ_MEMORY << 16) |
	(READ_MEMORY << 24);
    printf("leaving erase_roms... retval is %d\n", retval);
    deassert_vpp();
    putchar('\n');
    return(retval);
}


/*
 * program_rom
 * Write the specified value to flash EPROM at the specified address
 * vpp must be asserted before calling program_rom()
 */
boolean 
program_rom(ulong data, volatile ulong *ptr)
{
    int writepulse;
    ulong readback;

    for( writepulse = 0; writepulse < 25; writepulse++) {
	*ptr = PROGRAM | (PROGRAM << 8) | (PROGRAM << 16) | (PROGRAM << 24);
	*ptr = data;
	usecdelay(10); 
	*ptr = PROGRAM_VERIFY | (PROGRAM_VERIFY << 8) | 
	    (PROGRAM_VERIFY << 16) | (PROGRAM_VERIFY << 24);
	usecdelay(6);
	readback = *ptr;
	if ( readback == data ) break;
    }
    *ptr = READ_MEMORY | (READ_MEMORY << 8) | (READ_MEMORY << 16) |
	(READ_MEMORY << 24);
    if( readback == data ) return(TRUE);
    printf("\nprogram_rom: location 0x%x wrote 0x%8x, contains 0x%8x\n",
	   ptr, data, readback);
    return(FALSE);
}


/*
 * assert_vpp
 * Assert VPP on the flash EPROMs and wait for the voltage to stabilize:
 */

void assert_vpp(void)
{
    *CONTROL_REG |= CONTROL_FLASH_VPP;
    usecdelay(100000);
#ifdef XDEBUG
    printf("\n************* VPP ON **************\n");
#endif
}


/*
 * deassert_vpp
 * Deassert VPP on the flash EPROMs.
 */

void deassert_vpp(void)
{
    *CONTROL_REG &= ~CONTROL_FLASH_VPP;
    usecdelay(100000);
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

int badpart(int address, ushort expected)
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
void usecdelay(unsigned n)
{
    unsigned short tmp;

    if( n <= 10 ) {
        if(n <= 2) return;
	wastetime(0);
        if(n <= 3) return;
	wastetime(0);
        if(n <= 4) return;
	wastetime(0);
        if(n <= 5) return;
	wastetime(0);
        if(n <= 6) return;
	wastetime(0);
        if(n <= 7) return;
	wastetime(0);
        if(n <= 8) return;
	wastetime(0);
        if(n <= 9) return;
	wastetime(0);
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
	wastetime(2);  /* give timer a moment to load */
	while(tmp > 0x4000) {  /* break if below our pad value */
	    ctaddr->control = SEL_COUNTER1 | CT_MODE4;  /* latch counter 1 */
	    wastetime(2);  /* give timer a moment to latch count */
	    tmp = (ctaddr->counter1 & 0xff);  /* get lsb */
	    tmp |= (ctaddr->counter1 << 8);  /* get msb */
	}
    }
    ctaddr->control = SEL_COUNTER1;  /* stop counting */
}

void
notime()
{
}

void
wastetime(n)
unsigned short n;
{
for(; n>0; n--);
}

/*
 * Beep the terminal 'n' times.
 */
bell(int n)
{
    while(n) {
	putchar('\007');
	usecdelay(2000);
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
void flash_test()
{
    uint i,j, size;
    char buffa[16];
    ulong *lp,t1,t2,*ptr;
    extern ulong clock;
    extern char not_erase_msg[];

#define NUMBANKS 4
#ifdef DEBUG
    printf("Flash Test!\n");
#endif

    query_roms(); 

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

    printf("\nerasing...  ");
    if ( !erase_roms() ) {
	puts(not_erase_msg);
	bell(5);
	return;
    }

/* Stopped just short of erasing the flash */
/* return; */
/* Stopped just short of erasing the flash */

    puts("\n\"data equals address\" test...  writing...  ");
    assert_vpp();

size = flash_size / NUMBANKS;
    for ( i = FLASH_BASE; i < FLASH_BASE+(4*size); i += 4 ) {
	program_rom( i, (ulong *)i ) ;
#ifdef DEBUG
	if( !(i % 0x10000)) putchar('w');
#endif
    }
    deassert_vpp();

    printf("\nverifying...  ");

    for ( i = FLASH_BASE; i < FLASH_BASE+(4*size); i += 4 ) {
	lp = (ulong *)i;
	if ( *lp != i ) {
	    printf("\nTEST VERIFICATION FAILURE!  %8x: %8x\n", 
		   i, *lp );
	    bell(5);
	    return;
	}
    }
    printf("\nTest complete, no errors.\n");
    bell(1);
}
/* #endif USEEXTENDEDTEST *********  END OF THIS */

ulong testarray1[] = {
    0x55555555, 0xaaaaaaaa,
};

ulong testarray2[] = {
    0xaaaaaaaa, 0x55555555,
};

boolean
eflash_test()
{
    extern boolean ftest();

#ifdef DEBUG
    puts("Extended Flash Test!\n");
#endif

    query_roms(); 

    if (!flash_size) {
	printf("Flash not available...aborting test\n");
	bell(5);
	return(FALSE);
    }


#ifdef NOWAY
    rdtty(buffa, 16);
    if ( (buffa[0] & 0xdf) != 'Y' ) {
	printf("flash eprom test aborted.\n");
	return(FALSE);
    }
#endif NOWAY
    
    if( !ftest(FLASH_BASE, flash_size, (long *)0, 0, "data equals address" ) ||
       !ftest(FLASH_BASE, flash_size, testarray1, 2, "0x55, 0xAA" ) ||
       !ftest(FLASH_BASE, flash_size, testarray2, 2, "0xAA, 0x55" ) ) {
	puts("\n\nTest Failed.\n");
	bell(5);
	return(FALSE);
    }
	
    puts("\n\nTest complete, no errors.\n");
    bell(1);
    return(TRUE);
}

boolean
ftest(start, size, valptr, numvals, nameptr)
    volatile ulong *start, *valptr;
    char *nameptr;
{
    volatile ulong *ptr, *end, *vptr, *vend;
    ulong rval, value;
    extern char not_erase_msg[];
    
    puts("\nerasing...  ");
    if ( !erase_roms() ) {
	puts(not_erase_msg);
	return(FALSE);
    }

    printf("\n\"%s\" test...  writing...  ", nameptr);
    assert_vpp();

    end = (ulong *)((long)start + size);
    if( numvals ) vend = valptr + numvals;
    else {
	valptr = &value;
	vend = valptr + 1;
    }

    for ( ptr = start; ptr < end; ) {
	for(vptr = valptr; vptr < vend && ptr < end; vptr++, ptr++) {
	    if( !numvals ) *vptr = (long)ptr;  /* data equals address test */
	    if( !program_rom( *vptr, ptr ) ) {
		printf("\nTEST WRITE FAILURE at location %8x\n", ptr);
		return(FALSE);
	    }
#ifdef DEBUG
	    if( !((ulong)ptr % 0x10000)) putchar('w');
#endif
	}
    }
    deassert_vpp();

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

cflash_test()    /* continuous flash test */
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
        flash_test();
    }
}

/*
 * flash_load
 * Load the boot PROMs into flash memory (this includes ROM monitor and
 * system code) and verify.
 */

void flash_load()
{
    ulong i,*lp,*ptr;
    extern ulong clock;
    extern char not_erase_msg[];
#define ROMSIZ 0x100000

    puts("\nLoad ROM to Flash w/Verify Test\n");
    query_roms(); 

    if (!flash_size) {
	puts("Flash not available...aborting.\n");
	bell(5);
	return;
    }
    if (flash_size < ROMSIZ) {
	puts("Flash too small...aborting.\n");
	bell(5);
	return;
    }

    printf("\nerasing...  ");
    if ( !erase_roms() ) {
	puts(not_erase_msg);
	return;
    }
    puts("done.\nload ROM to flash...  ");
    assert_vpp();
    ptr = (ulong *)0x1000000;   /* point to base of ROM */
    for ( i = FLASH_BASE; i < FLASH_BASE+ROMSIZ; i += 4 ) program_rom( *ptr++, (ulong *)i );
    deassert_vpp();

    puts("and verify...  ");
    ptr = (ulong *)0x1000000;   /* point to base of ROM */
    for ( i = FLASH_BASE; i < FLASH_BASE+ROMSIZ; i += 4, ptr++ )  {
	lp = (ulong *)i;
	if ( *lp != *ptr ) {
	  printf("\nTEST VERIFICATION FAILURE!  %8x: sh be:%8x  is:%8x\n",i,
		 *ptr,*lp);
	  bell(5);
	  return;
      }
    }
    puts("\nROM to Flash load complete, no errors.\n");
    bell(1);
}

void
delay_test()
{
    ulong time1 = 15;
    ulong time2 = 100;

    printf("scope triggers every %d and %d usecs.\n",time1,time2);

    while(1) {
	*(volatile ushort *)ADRSPC_CONTROL &= ~CONTROL_LAMP;
	usecdelay(time1);
	*(volatile ushort *)ADRSPC_CONTROL |= CONTROL_LAMP;
	usecdelay(time2);
    }
}
	
void
rnvscope_loop()
{
   volatile ulong *nvram, a, b, c;

   a = 0xffffffff;
   b = 0x33333333;
   nvram = (volatile ulong *) 0x11000000;
   printf("Reading NVRam from 0x%x \n", nvram);
   while(1)
     b = *nvram;
}

void
nvscope_loop()
{
   volatile ulong *nvram, a, b, c;

   a = 0xffff0000;
   b = 0x00;
   nvram = (volatile ulong *) 0x11000000;
   printf("Writing %x to NVRam\n", a);
   while(1)
     *nvram = a;
}

void
read_flash_loop()
{
 volatile ulong *flash, here;

 flash = (volatile ulong *) 0x12000000;
 while(1) {
   here = *flash;
 }
}
#ifdef DONTNEEDANYMORE
void
try_erase(void)
{
    int erasepulse, i;
    uint size, bank;
    volatile ulong *ptr, *bankstart, *end;
    ulong data;
    boolean retval = TRUE;  /* optimism */

#define NUMBANKS 1
#define MAXERASE 3000

    assert_vpp();

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

void wake_calendar()
{
ulong pattern_orig = 0xC53AA35C;
ulong pattern = 0x5CA33AC5;
/* uchar p0=0xC5, p1=3A, p2=A3, p3=5C; */
uchar test_pattern[4] = {0xC5, 0x3A, 0xA3, 0x5C};
volatile char *calendar, test, test2;
uint offset, n, k;

calendar = (char *) 0x1115FFFF;
puts("\n");
printf("pattern = %x\n", pattern);
printf("test_pattern[0] = %x\n", test_pattern[0]);
printf("test_pattern[1] = %x\n", test_pattern[1]);
printf("test_pattern[2] = %x\n", test_pattern[2]);
printf("test_pattern[3] = %x\n", test_pattern[3]);

test2 = *calendar; /* Move the calendar ptr back to beginning */

for ( k=0; k<2; k++) {
for ( n=0; n<4; n++) {
   for ( offset=0; offset<8; offset++) {
      *calendar = test_pattern[n] >> offset;
      test = test_pattern[n] >> offset;
      printf( "%d %x\t", offset, test );
   }
}
}
puts("\n");
}

void read_calendar()
{
ulong value0, value1;
char *calendar;
uint offset;

calendar = (char *) 0x1115FFFF;
value0 = 0;
value1 = 0;

wake_calendar();

puts("\n");
for ( offset=0; offset<32; offset++) {
   if ( *calendar & 01 ) {
      printf("%d\t", offset );
      value0 |= ( 1 << offset);
   }
}

puts("\n");

for ( offset=0; offset<32; offset++) {
   if ( *calendar & 01 ) {
      printf("%d\t", offset );
      value1 |= ( 1 << offset);
   }
}

printf("\nCalendar Read test...\n");
printf("\nValue0 is %x\tValue1 is %x\n", value0, value1);

}

void write_calendar()
{
ulong pattern1 = 0x11450000;
ulong pattern2 = 0x92021315;
char *calendar, test;
uint offset, n;

calendar = (char *) 0x1115FFFF;

wake_calendar();

   for ( offset=0; offset<32; offset++) {
      printf("%d\t", offset );
      *calendar = ( char ) ( ( pattern1 >> offset ) & 0xFF);
      test = ( char ) ( ( pattern1 >> offset ) & 0xFF);
      printf( "%d %x\t", offset, test );
   }

   for ( offset=0; offset<32; offset++) {
      printf("%d\t", offset );
      *calendar = ( char ) ( ( pattern2 >> offset ) & 0xFF);
      test = ( char ) ( ( pattern2 >> offset ) & 0xFF);
      printf( "%d %x\t", offset, test );
   }

printf("\nCalendar Write test...   Done.\n\n");

}

/*
 * all_flash_tests
 * this function calls all flash diagnostic routines
 */

void all_flash_tests()
{

   char *c;
   char comm;
   char old_abortstate;

   reset_io();

   puts("\nFLASH Diagnostic Routines\n");
   puts("B      UsecDelay test loop\n");
   puts("F      Flash test\n");
   puts("E      Extended flash test\n");
   puts("L      Load flash\n");
   puts("W      MB long write Scope Loop\n");
   puts("R      MB long read Scope Loop\n");
   puts("U      Assert VPP\n");
   puts("D      Deassert VPP\n");
   puts("M      Query Flash\n");
   puts("T      Loop on Flash Read\n");
   puts("1      Read Calendar Chip\n");
   puts("2      Write Calendar Chip\n");

   for (;;) {

        old_abortstate = abortstate;
        abortstate = ABORT_IGNORE;
        puts(">>");
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
	   delay_test();
           break;

	case 'F':
           flash_test();
           break;

	case 'E':
         /*  eflash_test(); */
           puts("\nNot yet supported...\n");
           break;

	case 'L':
           puts("\nNot yet supported...\n");
          /* flash_load(); */
           break;

	case 'W':
           nvscope_loop();
           break;

	case 'R':
           rnvscope_loop();
           break;

	case 'U':
           assert_vpp();
           break;

	case 'D':
           deassert_vpp();
           break;

	case 'M':
           query_roms();
           break;

	case 'T':
           puts("\nNot yet supported...\n");
          /* read_flash_loop(); */
           break;

	case '1':
           read_calendar();
           break;

	case '2':
           write_calendar();
           break;

	case 'H':
	case '?':
           puts("B      UsecDelay test loop\n");
           puts("F      Flash test\n");
           puts("E      Extended flash test\n");
           puts("L      Load flash\n");
           puts("W      MB long write Scope Loop\n");
           puts("R      MB long read Scope Loop\n");
           puts("U      Assert VPP\n");
           puts("D      Deassert VPP\n");
           puts("M      Query Flash\n");
           puts("T      Loop on Flash Read\n");
           puts("1      Read Calendar Chip\n");
           puts("2      Write Calendar Chip\n");
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


