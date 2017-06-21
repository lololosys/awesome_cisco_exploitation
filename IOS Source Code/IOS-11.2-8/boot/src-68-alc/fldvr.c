/* $Id: fldvr.c,v 3.1.58.1 1996/05/09 13:54:22 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/fldvr.c,v $
 *------------------------------------------------------------------
 * fldvr.c -- Synergy ATM line card IOS ROM Monitor flash support
 *	      and test routines (based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fldvr.c,v $
 * Revision 3.1.58.1  1996/05/09  13:54:22  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:21  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:34:51  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.2  1996/03/23  04:43:12  rlowe
 * Fix includes to look inside wbu top-level directory.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.64.1  1996/03/22  09:07:17  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:16  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:15  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:17  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:08:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:54  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:50  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.2  1995/09/02  00:01:52  rlowe
 * Fix includes so that makefile can give search paths, rather than
 * hardcoding them.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:42  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.2  1995/06/02  00:33:26  rlowe
 * Roll in ACP Rev 2 HW map.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.1  1995/04/19  02:28:48  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:05  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.2  1995/02/09  04:55:54  rlowe
 * Initial port of IOS kernel to SYNALC.
 *
 * Revision 1.1  1995/02/01  10:59:55  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.1  1995/01/11  23:04:35  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:49  rahuja
 * IPS ver 10.2 snapshot
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Based on code from Chris Shaker and Steve Elias
 */

#include "../stdio.h"
#include "../defs.h"
#ifndef	SYNALC
#include "../i8254.h"
#endif
#include "mc.h"
#include "pcmap.h"
#ifdef	SYNALC
#include "../../wbu/cat5k/alc_include/mfp_acp_defs.h"
#endif

#define FLASH_BASE ADRSPC_FLASH_BASE
#define NUMBANKS 4
#define DEBUG

char not_erase_msg[] = "flash EPROMs will NOT erase...aborting\n";
int flash_size;
char flash_stat;
static int num_flsh_banks;
void assert_vpp(), deassert_vpp(), usecdelay(unsigned), 
     wastetime();
#ifndef	SYNALC
void usecdelay_brut(unsigned);
#endif

/*
 * Manufacturer codes table:
 */
#undef MAX_ROM_MFGS
#ifdef DONTREALLYNEEDTHIS
struct prom_table fwdproms[] = {
    {(ushort)0x89B4, 0x10000L, "INTEL 28F010"}, 
    {(ushort)0x89BD, 0x20000L, "INTEL 28F020"}, 
    {(ushort)0x01A7, 0x10000L, "AMD 28F010"}, 
    {(ushort)0x012A, 0x20000L, "AMD 28F020"}, 
    {(ushort)0x1CD0, 0x20000L, "M5M28F101P"}, 
    {(ushort)0x89A2, 0x40000L, "INTEL 28F008SA"}, 
};
#endif
/*
 * codes table with bits reversed the way the processor really
 * sees them
 */
struct prom_table proms[] = {
    {(ushort)0x912d, 0x80000L, "INTEL 28F010"}, 
    {(ushort)0x91BD, 0x100000L, "INTEL 28F020"}, 
    {(ushort)0x80E5, 0x80000L, "AMD 28F010"}, 
    {(ushort)0x8054, 0x100000L, "AMD 28F020"},
    {(ushort)0x1CD0, 0x100000L, "M5M28F101P  "},
    {(ushort)0x9145, 0x200000L, "INTEL 28F008SA"}, 
};
#define MAX_ROM_MFGS (sizeof(proms)/sizeof(struct prom_table))

#define INTEL28F00SA 0x9145
#define FLBLOCKSIZE  0x10000

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
	printf("comparing %#x and %#x\n",proms[i].manu_dev_code, code);
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
    ushort manu, device, man_dev;
    int bank, banksiz;
    ulong *lp;

    assert_vpp();

    lp = (ulong *)FLASH_BASE;
    *lp = RESET | (RESET << 8) | (RESET << 16) | (RESET << 24);
    usecdelay(6);
    *lp = RESET | (RESET << 8) | (RESET << 16) | (RESET << 24);
    usecdelay(6);
    *lp = READ_ID_CODES | (READ_ID_CODES << 8) | (READ_ID_CODES << 16) |
	(READ_ID_CODES << 24);
    usecdelay(6);
    manu = *lp & 0xff;
    device = *(lp+1) & 0xff;
#ifdef XDEBUG
    printf("Manu code = %#2x, device code = %#2x\n",manu,device);
#endif
    man_dev = (manu << 8) | device;

    *lp = RESET | (RESET << 8) | (RESET << 16) | (RESET << 24);

    deassert_vpp();

    if((banksiz = get_rom_size(man_dev)) == 0) {
	printf("invalid fprom code %#4x\n",man_dev);
	return;
    }

    /* check to see which banks are stuffed */
    for (bank = 0; bank < NUMBANKS; bank++ ) {
	/* check for bus error at base of flash bank */
	if(chk_buserr(FLASH_BASE + (bank * banksiz)) != 0) break;
    }

    num_flsh_banks = bank;
    flash_size = (bank * banksiz);
    /* Some newer flash parts have a status register */
    if(man_dev == INTEL28F00SA) flash_stat = 1;
    else flash_stat = 0;
    printf("fprom code = %#4x, part = %s\n\
bank size = %#x, banks = %d, total size = %#x\n",
	   man_dev, get_rom_name(man_dev), banksiz, bank, flash_size);
    return;
}

/*
** The newer flash parts have status registers.  This routine polls
** the address passed until it sees the status passed.
*/
unsigned long
poll_flash_stat(addr, stat)
volatile unsigned long *addr;
unsigned long stat;
{
    *addr = READ_STAT_REG | (READ_STAT_REG << 8) |
	(READ_STAT_REG << 16) | (READ_STAT_REG << 24);
    usecdelay(6);
    while(1) {
	if ((*addr & stat) == stat) return(*addr);
    }
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
    uint banksize, bank;
    volatile ulong *ptr, *bankstart, *end;
    ulong data, erasecmd;
    boolean retval = TRUE;  /* optimism */

#define MAXERASE 3000

    assert_vpp();
    bankstart = (ulong *)FLASH_BASE;  /* base of flash EPROM */
    end = (ulong *)(FLASH_BASE + flash_size);

    if(flash_stat) {  /* we are using technology with a status register */
	/* erase entire array a block at a time */
	for ( ptr = bankstart; ptr < end; ptr += (FLBLOCKSIZE / sizeof(* ptr))) {
#ifdef DEBUG
	    printf("\rErasing flash EPROM block at %#x...",ptr);
#endif
	    *ptr = ERASE | (ERASE << 8) | (ERASE << 16) | (ERASE << 24);
	    *ptr = ERASE_CONFIRM | (ERASE_CONFIRM << 8) |
		    (ERASE_CONFIRM << 16) | (ERASE_CONFIRM << 24);
	    poll_flash_stat(ptr, WR_STAT_RDY | (WR_STAT_RDY << 8) |
			    (WR_STAT_RDY << 16) | (WR_STAT_RDY << 24));
	    if ( *ptr & (ERASE_STAT | (ERASE_STAT << 8) |
			 (ERASE_STAT << 16) | (ERASE_STAT << 24))) {  /* erase error? */
		printf("block erase failed at location %#x\n",ptr);
		*ptr = CLEAR_STAT_REG;  /* clear error */
		goto badreturn;
	    }
	}
	goto returnok;
    }
    /*
     * Program all locations to zero before erase
     */
#ifdef DEBUG
    puts("\nZeroing flash EPROMs for erase...");
#endif
    bankstart = (ulong *)FLASH_BASE;  /* base of flash EPROM */
    end = (ulong *)(FLASH_BASE + flash_size);
    for ( ptr = bankstart; ptr < end; ptr++ ) {
	if( *ptr != 0 ) {
	    if( !program_rom(0, ptr) || *ptr != 0 ) {
		printf("\nflash address %#x would not zero for erase",ptr);
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
    banksize = flash_size / num_flsh_banks;  /* get bank size */
    for (bank = 0; bank < num_flsh_banks; bank++ ) {
#ifdef DEBUG
	printf("\nVerifying flash EPROMs bank %d...",bank);
#endif
	/* sanity check again -- should be all 0s before we erase */
	bankstart = (ulong *)(FLASH_BASE + banksize*bank);  /* base of this bank */
	end = (ulong *)(FLASH_BASE + (bank+1)*banksize);  /* end of current bank */
	for ( ptr = bankstart; ptr < end; ptr++) {
 	    data = *ptr;
 	    if ( data != 0 ) {
		printf("\nflash sanity error:  %#x: expected 0x00000000   actual %#8x\n", 
		       ptr, data);
		printf("10 more reads from address %#x: ", ptr);
		for ( i = 0; i < 10; i++ ) printf("%#8x ", *ptr);
		goto badreturn;
	    }
#ifdef DEBUG
	    if( !((ulong)ptr % 0x10000)) putchar('v');
#endif
	}

#ifdef DEBUG
	printf("\nErasing flash EPROMs bank %d...",bank);
#endif
	erasepulse = 0;
	/* write erase command for entire bank */
	erasecmd = ERASE | (ERASE << 8) | (ERASE << 16) | (ERASE << 24);
	ptr = bankstart;
	while( erasepulse++ < MAXERASE ) {  /* allowable erase pulses */
	    /* write erase set-up command */
	    *bankstart = erasecmd;
	    /* write erase command */
	    *bankstart = erasecmd;
	    usecdelay(10000);  /* 10ms delay */
	    while( ptr < end ) {  /* verify location by location */
		*ptr = ERASE_VERIFY | (ERASE_VERIFY << 8) |
		    (ERASE_VERIFY << 16) | (ERASE_VERIFY << 24);
		usecdelay(6);
		data = *ptr;
		if( data != 0xffffffff ) {  /* verify failed */
		    /*
		     * Important: We must re-erase only the byte that
		     * needs re-erasing.  If you re-erase a location
		     * that is already erased the part may be destroyed.
		     * This code sets up the erasecmd for the next erasepulse
		     * substituting the READ_MEMORY command for the bytes
		     * that do not need re-erasing.
		     */
		    erasecmd = READ_MEMORY | (READ_MEMORY << 8) |
			(READ_MEMORY << 16) | (READ_MEMORY << 24);  /* initialize */
		    for( i=0; i<32; i+=8 ) {
			if((( data >> i ) & 0xff ) != 0xff )  /* location not erased */
			    erasecmd |= ( ERASE << i );
		    }
#ifdef XDEBUG
		printf("erase command = %#x, at location %#x\n",erasecmd,ptr);
#endif
		    break;
		}
#ifdef DEBUG
		if( !((ulong)ptr % 0x10000)) putchar('e');
#endif
		ptr++;
	    }
	    if(ptr == end) break;  /* this bank successfully erased */
	}
	if(ptr != end) break;  /* we ran out of erase pulses */
    }
    if(ptr != end) {
	printf("\nerase failed at location %#x",ptr);
badreturn:
	retval = FALSE;
    }
returnok:
    *bankstart = RESET | (RESET << 8) | (RESET << 16) |	(RESET << 24);
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
	if(flash_stat) {  /* we are using technology with a status register */
	    poll_flash_stat(ptr, WR_STAT_RDY | (WR_STAT_RDY << 8) |
			    (WR_STAT_RDY << 16) | (WR_STAT_RDY << 24));
	    if ( *ptr & (BYTE_WR_STAT | (BYTE_WR_STAT << 8) |
			 (BYTE_WR_STAT << 16) | (BYTE_WR_STAT << 24))) {  /* erase error? */
		printf("write operation failed at location %#x\n",ptr);
		*ptr = CLEAR_STAT_REG;  /* clear error */
		return(FALSE);
	    }
	    break;
	}
	usecdelay(10); 
	*ptr = PROGRAM_VERIFY | (PROGRAM_VERIFY << 8) | 
	    (PROGRAM_VERIFY << 16) | (PROGRAM_VERIFY << 24);
	usecdelay(6);
	readback = *ptr;
	if ( readback == data ) break;
    }
    *ptr = RESET | (RESET << 8) | (RESET << 16) | (RESET << 24);
    usecdelay(6);
    readback = *ptr;
    if(readback == data) return(TRUE);
    printf("\nprogram_rom: location %#x wrote %#8x, contains %#8x\n",
	   ptr, data, readback);
    return(FALSE);
}


/*
 * assert_vpp
 * Assert VPP on the flash EPROMs and wait for the voltage to stabilize:
 */

void assert_vpp(void)
{
    *(ushort *)ADRSPC_CONTROL2 |= CONTROL2_VPPEN;
    usecdelay(100000);
#ifdef XDEBUG
    printf("\n***************VPP ON ***************\n");
#endif
}


/*
 * deassert_vpp
 * Deassert VPP on the flash EPROMs.
 */

void deassert_vpp(void)
{
    *(ushort *)ADRSPC_CONTROL2 &= ~CONTROL2_VPPEN;
    usecdelay(100000);
#ifdef XDEBUG
    printf("\n*************VPP OFF *************\n");
#endif
}

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
#ifdef	SYNALC
	MFP_Poll_uSec (n) ;
#else
    unsigned short tmp;

    if( n <= 9 ) 
        return;  /* subroutine overhead is 9us on Merlot */

    if (cookie->processor == PROCESSOR_BRUT) {
        usecdelay_brut(n);
        return;
    }
    n -= 10;  /* subtract some overhead amount */
    while(n) {
	tmp = n & 0x7fff;
	if(tmp == 0) tmp = 0x8000;
	n -= tmp;
	if(tmp < 5) continue;
	tmp += 0x4000;  /* pad this amount so timer never times out */
	ctaddr->control = SEL_COUNTER1 | RW_LSBMSB | CT_MODE4;
	ctaddr->counter1 = tmp;  /* program lsb */
	ctaddr->counter1 = tmp >> 8;  /* program msb */
	wastetime();  /* give timer a moment to load */
	while(tmp > 0x4000) {  /* break if below our pad value */
	    ctaddr->control = SEL_COUNTER1 | CT_MODE4;  /* latch counter 1 */
	    wastetime();  /* give timer a moment to latch count */
	    tmp = (ctaddr->counter1 & 0xff);  /* get lsb */
	    tmp |= (ctaddr->counter1 << 8);  /* get msb */
	}
    }
    ctaddr->control = SEL_COUNTER1;  /* stop counting */
#endif
}

#ifndef	SYNALC
/*
 * wait at least n microseconds 
 * by watching 8254 timer counter 1 while looping.
 * the sequence of instructions is critical in this module due to the
 * slow internal workings of the 8254 chip.
 * especially important is the time between writing the control register
 * and reading something back.
 */

void usecdelay_brut(unsigned n)
{
    unsigned short tmp;

    if( n <= 18 ) 
        return;  /* subroutine overhead is 9us on Merlot */
    n -= 19;  /* subtract some overhead amount */
    while(n) {
	tmp = n & 0x7fff;
	if(tmp == 0) tmp = 0x8000;
	n -= tmp;
	if(tmp < 5) continue;
	tmp += 0x4000;  /* pad this amount so timer never times out */
	ctaddr->counter1 = tmp;  /* program timer */
	wastetime();  /* give timer a moment to load */
	while(tmp > 0x4000) {  /* break if below our pad value */
	    wastetime();  /* give timer a moment to latch count */
	    tmp = (ctaddr->counter1);  /* get timer value */
	}
    }
}
#endif

void
wastetime(void)
{
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
 * part.  Provide simple "data equals address" test routine (flash_test(0))
 * and an extended routine with all ones and all zeros tests (flash_test(1)).
 */

ulong testarray1[] = {
    0x55555555, 0xaaaaaaaa,
};

ulong testarray2[] = {
    0xaaaaaaaa, 0x55555555,
};

boolean
flash_test(int testlength)
{
    extern setcacr();
    extern boolean ftest();
    int origcache;
    boolean retval = FALSE;  /* pessimistic? */

#ifdef DEBUG
    puts("Extended Flash Test!\n");
#endif

    query_roms(); 

    if (!flash_size) {
	printf("Flash not available...aborting test\n");
	bell(5);
	return(FALSE);
    }

    origcache = setcacr(CLEAR_CACHES); 

    if( !ftest(FLASH_BASE, flash_size, (long *)0, 0, "data equals address" ) ) {
testfail:
	puts("\n\nTest Failed.\n");
	bell(5);
	goto ret;
    }

    if(testlength > 0) {
	if(!ftest(FLASH_BASE, flash_size, testarray1, 2, "0x55, 0xAA" ) ||
	   !ftest(FLASH_BASE, flash_size, testarray2, 2, "0xAA, 0x55" ) )
	    goto testfail;
    }
	
    puts("\n\nTest complete, no errors.\n");
    bell(1);
    retval = TRUE;
ret:
    setcacr(origcache);
    return(retval);
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

void
cflash_test(void)    /* continuous flash test */
{
    int pass = 0;
    int error = 0;

    while(1) {
	printf("Continuous flash test pass: %d, errors: %d\n", ++pass, error);
	if( flash_test(1) == FALSE ) error++;
    }
}

/*
 * flash_load
 * Load the boot PROMs into flash memory (this includes ROM monitor and
 * system code) and verify.
 */

void flash_load(void)
{
    ulong i,*lp,*ptr;
    int origcache;
    extern ulong clock;
    extern setcacr();
    extern char not_erase_msg[];
#ifdef	SYNALC
#define	ROMSIZ	(ADRSPC_PROM_END - ADRSPC_PROM)
#else
#define ROMSIZ 0x100000
#endif

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

    origcache = setcacr(CLEAR_CACHES);

    printf("\nerasing...  ");
    if ( !erase_roms() ) {
	puts(not_erase_msg);
	goto ret;
    }
    puts("done.\nload ROM to flash...  ");
    assert_vpp();
    ptr = (ulong *)ADRSPC_PROM;   /* point to base of ROM */
    for ( i = FLASH_BASE; i < FLASH_BASE+ROMSIZ; i += 4 ) program_rom( *ptr++, (ulong *)i );
    deassert_vpp();

    puts("and verify...  ");
    ptr = (ulong *)ADRSPC_PROM;   /* point to base of ROM */
    for ( i = FLASH_BASE; i < FLASH_BASE+ROMSIZ; i += 4, ptr++ )  {
	lp = (ulong *)i;
	if ( *lp != *ptr ) {
	  printf("\nTEST VERIFICATION FAILURE!  %8x: sh be:%8x  is:%8x\n",i,
		 *ptr,*lp);
	  bell(5);
	  goto ret;
      }
    }
    puts("\nROM to Flash load complete, no errors.\n");
    bell(1);
 ret:
    setcacr(origcache);
}
