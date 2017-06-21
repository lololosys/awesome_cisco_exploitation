/* $Id: sierra_cycles.c,v 3.2.58.1 1996/03/21 23:31:01 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/sierra_cycles.c,v $
 *------------------------------------------------------------------
 * sierra_cycles.c
 *
 * December 1993, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Dispatch all possible legal and illegal cycles to the Sierra
 * hardware.
 *------------------------------------------------------------------
 * $Log: sierra_cycles.c,v $
 * Revision 3.2.58.1  1996/03/21  23:31:01  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:23:28  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:45:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "cpu_mips.h"
#include "nvmonvars.h"
#include "mon_defs.h"
#include "queryflags.h"
#include "signal_4k.h"

/*
 * Local definitions
 */
#define SECTION_DRAM	0x01
#define SECTION_IO	0x02
#define SECTION_SHMEM	0x04

#define BERR_NONE	0x00
#define BERR_RDONLY	0x01
#define BERR_WRONLY	0x02
#define BERR_RDWR	0x03

#define M128MEG    	0x8000000
#define M64MEG     	0x4000000
#define M32MEG     	0x2000000
#define M16MEG     	0x1000000
#define M8MEG      	0x800000
#define M4MEG      	0x400000
#define M2MEG      	0x200000
#define M1MEG      	0x100000
#define M512K      	0x80000
#define M256K      	0x40000
#define M128K      	0x20000
#define M64K       	0x10000

/*
 * External variables and functions
 */
extern int optind;
extern char *optarg;
extern volatile unsigned long berr_addr;

static char *optstr = "idsq";
static char *dram_sec = "DRAM";
static char *shmem_sec = "SHMEM";
static char *io_sec = "IO";
static char *eprom_sec = "EPROM";
static char *nvram_sec = "NVRAM";
static char *flash_sec = "FLASH";
static char *bflash_sec = "Boot FLASH";
static char *uart_sec = "UART";
static char *nev_sec = "NEVADA";
static char *iack_sec = "IACK";
static char *nim_sec = "NIM";
 
/*
 * test_results: 
 * Print out an error message if appropriate
 */
static void
test_results(int val, int res, char *str, char *size, unsigned long addr)
{
    if ((val & BERR_RDONLY) != (res & BERR_RDONLY)) {

	/*
	 * Discrepancy to do with read access
	 */
	if (val & BERR_RDONLY) {
	    printf("Error : unexpected %s bus error on %s read at 0x%x\n",
		   str, size, addr);
	} else {
	    printf("Error : expected %s bus error on %s read at 0x%x\n",
		   str, size, addr);
	}
    }

    if ((val & BERR_WRONLY) != (res & BERR_WRONLY)) {

	/*
	 * Discrepancy to do with write access
	 */
	if (val & BERR_WRONLY) {
	    printf("Error : unexpected %s bus error on %s write at 0x%x\n",
		   str, size, addr);
	} else {
	    printf("Error : expected %s bus error on %s write at 0x%x\n",
		   str, size, addr);
	}
    }
}

/*
 * test_16bit_memory:
 * Test out memory using 1-2 byte access types
 */
static void
test_16bit_memory(unsigned long addr, char *str, int res)
{
    int retval;

    retval = chkberr(addr, BW_8BITS, 0);
    test_results(retval, res, str, "8bit", addr);
    retval = chkberr(addr, BW_16BITS, 0);
    test_results(retval, res, str, "16bit", addr);
}

/*
 * test_32bit_memory:
 * Test out memory using 1-4 byte access types
 */
static void
test_32bit_memory(unsigned long addr, char *str, int res)
{
    int retval;

    retval = chkberr(addr, BW_8BITS, 0);
    test_results(retval, res, str, "8bit", addr);
    retval = chkberr(addr, BW_16BITS, 0);
    test_results(retval, res, str, "16bit", addr);
    retval = chkberr((addr & ~0x03) + 2, BW_24BITS, 0);
    test_results(retval, res, str, "24bit", addr);
    retval = chkberr((addr & ~0x03) + 1, BW_24BITS, 0);
    test_results(retval, res, str, "24bit", addr);
    retval = chkberr(addr, BW_32BITS, 0);
    test_results(retval, res, str, "32bit", addr);
}

/*
 * test_64bit_memory:
 * Test out memory using 5-8 byte access types
 */
static void
test_64bit_memory(unsigned long addr, char *str, int res)
{
    int retval;

    retval = chkberr((addr & ~0x7) + 5, BW_40BITS, 0);
    test_results(retval, res, str, "40bit", addr);
    retval = chkberr((addr & ~0x7) + 3, BW_40BITS, 0);
    test_results(retval, res, str, "40bit", addr);
    retval = chkberr((addr & ~0x7) + 6, BW_48BITS, 0);
    test_results(retval, res, str, "48bit", addr);
    retval = chkberr((addr & ~0x7) + 2, BW_48BITS, 0);
    test_results(retval, res, str, "48bit", addr);
    retval = chkberr((addr & ~0x7) + 7, BW_56BITS, 0);
    test_results(retval, res, str, "56bit", addr);
    retval = chkberr((addr & ~0x7) + 1, BW_56BITS, 0);
    test_results(retval, res, str, "56bit", addr);
    retval = chkberr((addr & ~0x7), BW_64BITS, 0);
    test_results(retval, res, str, "64bit", addr);
}

/*
 * test_cached_read:
 * Test out memory using cached accesses
 */
static void
test_cached_read(unsigned long addr, char *str, int res)
{
    int retval;

    /*
     * Flush the caches and read the address
     */
    cache_flush(0, -1);
    addr = KSEG1_TO_KSEG0(addr);
    retval = chkberr((addr & ~0x3), BW_32BITS, 1);
    test_results(retval, res, str, "32bit", addr);
}

/*
 * test_wberr_register:
 * Test out the section write bus error register
 * The bus error register should handle 1-4byte reads. It should
 * bus error on all writes, and 5-8,cached reads.
 */
static void
test_wberr_register(unsigned long addr, char *str, int verbose)
{
    if (verbose)
	printf("Testing %s write bus error register\n", str);
    test_32bit_memory(addr, str, BERR_WRONLY);
    test_64bit_memory(addr, str, BERR_RDWR);
    test_cached_read(addr, str, BERR_RDONLY);
}

/*
 * get_bank_size: 
 * Return the bank size in bytes for a flash simm
 */
static int
get_bank_size(int flag)
{
    switch (flag) {
    case 0x00:
	return(M1MEG);
    case 0x01:
	return(M2MEG);
    case 0x02:
	return(M4MEG);
    case 0x03:
	return(M8MEG);
    case 0x04:
	return(M16MEG);
    case 0x05:
	return(M32MEG);
    default:
	return(0);
    }
}

/*
 * get_flash_size:
 * Calculate the flash size from the status registers
 */
static int
get_flash_size(void)
{
    int i = 0;

    if ( KSEG1_IO_ASIC->sys_stat0 & SS0_FSIM1_PRES )
	return(0);

    if ( KSEG1_IO_ASIC->sys_stat0 & SS0_FSIM1_2BNK )
	i = 1;
    else
	i = 2;

    if (!(KSEG1_IO_ASIC->sys_stat0 & SS0_FSIM2_PRES)) {	
	if ( KSEG1_IO_ASIC->sys_stat0 & SS0_FSIM2_2BNK )
	    i += 1;
	else
	    i += 2;
    }
    return( i * get_bank_size((KSEG1_IO_ASIC->sys_stat0 >> SS0_FLSIZ_SH) & 0x7) );
}

/*
 * get_bflash_size:
 * Calculate the boot flash size from the status registers
 */
static int
get_bflash_size(void)
{
    int i = 0;

    if ( KSEG1_IO_ASIC->sys_stat0 & SS0_BSIMM_PRES )
	return(i);

    if ( KSEG1_IO_ASIC->sys_stat0 & SS0_BSIMM_2BNK )
	i = 1;
    else
	i = 2;

    return( i * get_bank_size((KSEG1_IO_ASIC->sys_stat0 >> SS0_BSSIZ_SH) & 0x7) );
}

/*
 * test_nim_slot:
 * Test out the NIM registers and IACK register
 */
static void
test_nim_slot(int slot, unsigned long addr, unsigned long iack, int absent, int v)
{
    int berr_flag;

    if (v)
	printf("Testing NIM %d\n", slot);
    if (absent)
	berr_flag = BERR_RDWR;
    else
	berr_flag = BERR_NONE;

    test_32bit_memory(addr, nim_sec, berr_flag);
    test_64bit_memory(addr, nim_sec, BERR_RDWR);
    test_cached_read(addr, nim_sec, BERR_RDONLY);

    test_32bit_memory(iack, iack_sec, berr_flag);
    test_64bit_memory(iack, iack_sec, BERR_RDWR);
    test_cached_read(iack, iack_sec, BERR_RDONLY);
}

/*
 * test_dram_cycles:
 * Test all possible DRAM cycles
 * DRAM supports : Memory - 1-8 byte read/write, cache read/write
 *      	   Write Berr Register - 1-4 byte read only
 */
static void
test_dram_cycles(int v)
{
    unsigned short sys_ctl = KSEG1_IO_ASIC->sys_ctl;

    printf("Test DRAM section\n");

    /*
     * We'll test start of DRAM first
     */
    if (v)
	printf("Testing normal cycles\n");
    test_32bit_memory(KSEG1_ADRSPC_RAM, dram_sec, BERR_NONE);
    test_64bit_memory(KSEG1_ADRSPC_RAM, dram_sec, BERR_NONE);
    test_cached_read(KSEG1_ADRSPC_RAM, dram_sec, BERR_NONE);

    /*
     * Now, test DRAM sizing
     */
    if (v)
	printf("Testing main memory sized to 128M\n");
    KSEG1_IO_ASIC->sys_ctl = (KSEG1_IO_ASIC->sys_ctl & ~CONTROL_MEM_MASK) | 
	(CONTROL_128MEG_MEM << CONTROL_MEM_SHIFT);
    flush_io_wb();
    test_32bit_memory(KSEG1_ADRSPC_RAM + M128MEG - 4, dram_sec, BERR_NONE);

    if (v)
	printf("Testing main memory sized to 32M\n");
    KSEG1_IO_ASIC->sys_ctl = (KSEG1_IO_ASIC->sys_ctl & ~CONTROL_MEM_MASK) | 
	(CONTROL_32MEG_MEM << CONTROL_MEM_SHIFT);
    flush_io_wb();
    test_32bit_memory(KSEG1_ADRSPC_RAM + M32MEG - 4, dram_sec, BERR_NONE);
    test_32bit_memory(KSEG1_ADRSPC_RAM + M32MEG, dram_sec, BERR_RDWR);

    /*
    ** Rev 0 boards do not have an 16m option
    */
    if((KSEG1_IO_ASIC->sys_stat0 & SS0_HWREV_BITS) > 0) {
	if (v)
	    printf("Testing main memory sized to 16M\n");
	KSEG1_IO_ASIC->sys_ctl = (KSEG1_IO_ASIC->sys_ctl & ~CONTROL_MEM_MASK) | 
	    (CONTROL_16MEG_MEM << CONTROL_MEM_SHIFT);
	flush_io_wb();
	test_32bit_memory(KSEG1_ADRSPC_RAM + M16MEG - 4, dram_sec, BERR_NONE);
	test_32bit_memory(KSEG1_ADRSPC_RAM + M16MEG, dram_sec, BERR_RDWR);
    }

    if (v)
	printf("Testing main memory sized to 8M\n");
    KSEG1_IO_ASIC->sys_ctl = (KSEG1_IO_ASIC->sys_ctl & ~CONTROL_MEM_MASK) | 
	(CONTROL_8MEG_MEM << CONTROL_MEM_SHIFT);
    flush_io_wb();
    test_32bit_memory(KSEG1_ADRSPC_RAM + M8MEG - 4, dram_sec, BERR_NONE);
    test_32bit_memory(KSEG1_ADRSPC_RAM + M8MEG, dram_sec, BERR_RDWR);
    
    /*
     * Now, test the write bus error register
     */
    test_wberr_register(KSEG1_ADRSPC_DRAM_WBERR, dram_sec, v);

    /*
     * Finished
     */
    KSEG1_IO_ASIC->sys_ctl = sys_ctl;	/* restore sys_ctl register */
    flush_io_wb();
}

/*
 * test_io_cycles:
 * Test all possible IO cycles
 */
static void
test_io_cycles(int v)
{
    unsigned short sys_ctl = KSEG1_IO_ASIC->sys_ctl;
    unsigned long flash_size, bflash_size;
    int flash_berr, bflash_berr;

    printf("Test IO section\n");
    /*
     * Test NVRAM sizing and normal cycles
     */
    if (v)
	printf("Testing NVRAM normal cycles\n");
    test_32bit_memory(KSEG1_ADRSPC_NVRAM, nvram_sec, BERR_NONE);
    test_64bit_memory(KSEG1_ADRSPC_NVRAM, nvram_sec, BERR_RDWR);
    test_cached_read(KSEG1_ADRSPC_NVRAM, nvram_sec, BERR_RDONLY);

    if (v)
	printf("Testing NVRAM sized to 512K\n");
    KSEG1_IO_ASIC->sys_ctl = (KSEG1_IO_ASIC->sys_ctl & ~NVRAM_SIZE_128K);
    flush_io_wb();
    test_32bit_memory(KSEG1_ADRSPC_NVRAM + M512K - 4, nvram_sec, BERR_NONE);
    
    if (v)
	printf("Testing NVRAM sized to 128K\n");
    KSEG1_IO_ASIC->sys_ctl = (KSEG1_IO_ASIC->sys_ctl | NVRAM_SIZE_128K);
    flush_io_wb();
    test_32bit_memory(KSEG1_ADRSPC_NVRAM + M128K - 4, nvram_sec, BERR_NONE);
    test_32bit_memory(KSEG1_ADRSPC_NVRAM + M128K, nvram_sec, BERR_RDWR);

    /*
     * Test EPROM sizing and normal cycles
     */
    if (v)
	printf("Testing EPROM normal cycles\n");
    test_32bit_memory(KSEG1_ADRSPC_PROM, eprom_sec, BERR_NONE);
    test_64bit_memory(KSEG1_ADRSPC_PROM, eprom_sec, BERR_RDWR);
    test_cached_read(KSEG1_ADRSPC_PROM, eprom_sec, BERR_RDONLY);

    if (v)
	printf("Testing EPROM sized to 512K\n");
    KSEG1_IO_ASIC->sys_ctl = (KSEG1_IO_ASIC->sys_ctl & ~EPROMSZ_BITS) | 
	(EPROM_512K << EPROM_SHIFT);
    flush_io_wb();
    test_32bit_memory(KSEG1_ADRSPC_PROM + M512K - 4, eprom_sec, BERR_NONE);
    test_32bit_memory(KSEG1_ADRSPC_PROM + M512K, eprom_sec, BERR_RDWR);
    
    if (v)
	printf("Testing EPROM sized to 256K\n");
    KSEG1_IO_ASIC->sys_ctl = (KSEG1_IO_ASIC->sys_ctl & ~EPROMSZ_BITS) | 
	(EPROM_256K << EPROM_SHIFT);
    flush_io_wb();
    test_32bit_memory(KSEG1_ADRSPC_PROM + M256K - 4, eprom_sec, BERR_NONE);
    test_32bit_memory(KSEG1_ADRSPC_PROM + M256K, eprom_sec, BERR_RDWR);

#ifdef TEST_EPROM_128K  /* This code is bigger than 128k */
    if (v)
	printf("Testing EPROM sized to 128K\n");
    KSEG1_IO_ASIC->sys_ctl = (KSEG1_IO_ASIC->sys_ctl & ~EPROMSZ_BITS) | 
	(EPROM_128K << EPROM_SHIFT);
    flush_io_wb();
    test_32bit_memory(KSEG1_ADRSPC_PROM + M128K - 4, eprom_sec, BERR_NONE);
    test_32bit_memory(KSEG1_ADRSPC_PROM + M128K, eprom_sec, BERR_RDWR);
#endif

    /*
     * Test flash
     */
    flash_size = get_flash_size();
    if (v) 
	printf("Testing FLASH address space, with 0x%x bytes\n", flash_size);
    if (flash_size) {
        flash_berr = BERR_NONE;
    } else {
	flash_berr = BERR_RDWR;
    }
    test_32bit_memory(KSEG1_ADRSPC_FLASH, flash_sec, flash_berr);
    test_64bit_memory(KSEG1_ADRSPC_FLASH, flash_sec, flash_berr);
    test_cached_read(KSEG1_ADRSPC_FLASH, flash_sec, BERR_RDONLY);

    test_32bit_memory(KSEG1_ADRSPC_FLASH + flash_size - 4, flash_sec, flash_berr);
    test_32bit_memory(KSEG1_ADRSPC_FLASH + flash_size, flash_sec, BERR_RDWR);

    /*
     * Test boot flash normal cycles and sizing
     */
    bflash_size = get_bflash_size();
    if (v) 
	printf("Testing Boot FLASH address space, with 0x%x bytes\n", 
	       bflash_size);
    if (bflash_size) {
        bflash_berr = BERR_NONE;
    } else {
	bflash_berr = BERR_RDWR;
    }
    test_32bit_memory(KSEG1_ADRSPC_BFLASH, bflash_sec, bflash_berr);
    test_64bit_memory(KSEG1_ADRSPC_BFLASH, bflash_sec, bflash_berr);
    test_cached_read(KSEG1_ADRSPC_BFLASH, bflash_sec, BERR_RDONLY);

    test_32bit_memory(KSEG1_ADRSPC_BFLASH + bflash_size - 4, bflash_sec, 
		      bflash_berr);
    test_32bit_memory(KSEG1_ADRSPC_BFLASH + bflash_size, bflash_sec, BERR_RDWR);

    /*
     * Test Nevada ASIC
     */
    if (v)
	printf("Testing NEVADA address space\n");
    test_32bit_memory(KSEG1_ADRSPC_NEV_ASIC, nev_sec, BERR_NONE);
    test_64bit_memory(KSEG1_ADRSPC_NEV_ASIC, nev_sec, BERR_NONE);
    test_cached_read(KSEG1_ADRSPC_NEV_ASIC, nev_sec, BERR_RDONLY);

    /*
     * Test the write bus error register
     */
    test_wberr_register(KSEG1_ADRSPC_IO_WBERR, io_sec, v);

    /*
     * Finished
     */
    KSEG1_IO_ASIC->sys_ctl = sys_ctl;
    flush_io_wb();
}

/*
 * test_shmem_cycles:
 * Test all possible Shared Memory cycles
 */
static void
test_shmem_cycles(int v)
{
    unsigned short shmem_ctl;
    int nim_absent;

    reset_io();
    shmem_ctl = KSEG1_IO_ASIC->shmem_ctl;
    printf("Test Shared Memory section\n");

    /*
     * We'll test start of Shared Memory first
     */
    if (v)
	printf("Testing normal cycles\n");
    test_32bit_memory(KSEG1_ADRSPC_SHAREDMEM, shmem_sec, BERR_NONE);
    test_64bit_memory(KSEG1_ADRSPC_SHAREDMEM, shmem_sec, BERR_NONE);
    test_cached_read(KSEG1_ADRSPC_SHAREDMEM, shmem_sec, BERR_NONE);

    /*
     * Now, test shared memory sizing
     */
    if (v)
	printf("Testing shared memory sized to 16M\n");
    KSEG1_IO_ASIC->shmem_ctl = (KSEG1_IO_ASIC->shmem_ctl & ~SHMEM_SIZE_BITS) | 
	(SHMEM_16MEG << SHMEM_SIZ_SHIFT);
    flush_io_wb();
    test_32bit_memory(KSEG1_ADRSPC_SHAREDMEM + M16MEG - 4, shmem_sec, BERR_NONE);

    /*
    ** Rev 0 boards do not have an 8m option
    */
    if((KSEG1_IO_ASIC->sys_stat0 & SS0_HWREV_BITS) > 0) {
	if (v)
	    printf("Testing shared memory sized to 8M\n");
	KSEG1_IO_ASIC->shmem_ctl = (KSEG1_IO_ASIC->shmem_ctl & ~SHMEM_SIZE_BITS) | 
	    (SHMEM_8MEG << SHMEM_SIZ_SHIFT);
	flush_io_wb();
	test_32bit_memory(KSEG1_ADRSPC_SHAREDMEM + M8MEG - 4, shmem_sec, BERR_NONE);
	test_32bit_memory(KSEG1_ADRSPC_SHAREDMEM + M8MEG, shmem_sec, BERR_RDWR);
    }

    if (v)
	printf("Testing shared memory sized to 4M\n");
    KSEG1_IO_ASIC->shmem_ctl = (KSEG1_IO_ASIC->shmem_ctl & ~SHMEM_SIZE_BITS) | 
	(SHMEM_4MEG << SHMEM_SIZ_SHIFT);
    flush_io_wb();
    test_32bit_memory(KSEG1_ADRSPC_SHAREDMEM + M4MEG - 4, shmem_sec, BERR_NONE);
    test_32bit_memory(KSEG1_ADRSPC_SHAREDMEM + M4MEG, shmem_sec, BERR_RDWR);

    /*
     * Now, test the write bus error register
     */
    test_wberr_register(KSEG1_ADRSPC_SHMEM_WBERR, shmem_sec, v);

    /*
     * Test each NIM slot
     */
    nim_absent = (KSEG1_IO_ASIC->nim_stat & NMS_NIM1_PRES);
    test_nim_slot(1, KSEG1_ADRSPC_SLOT_1, KSEG1_ADRSPC_IACK_1, nim_absent, v);
    nim_absent = (KSEG1_IO_ASIC->nim_stat & NMS_NIM2_PRES);
    test_nim_slot(2, KSEG1_ADRSPC_SLOT_2, KSEG1_ADRSPC_IACK_2, nim_absent, v);
    nim_absent = (KSEG1_IO_ASIC->nim_stat & NMS_NIM3_PRES);
    test_nim_slot(3, KSEG1_ADRSPC_SLOT_3, KSEG1_ADRSPC_IACK_3, nim_absent, v);

    /*
     * Finished 
     */
    KSEG1_IO_ASIC->shmem_ctl = shmem_ctl;
    flush_io_wb();
}

/*
 * hw_cycles : Excercise the hardware with all possible cycles
 * cycles [-i|-s|-d|-q]
 */
int
hw_cycles(int argc, char *argv[])
{
    int verbose = 1, sections = 0, i;
    unsigned long sreg, shmem_ctl;
    SIG_PF savfcn;

    while((i = getopt(argc,argv,optstr)) >= 0) {
	switch(i) {
	case 'i': 	/* IO section */
	    sections |= SECTION_IO;
	    break;
	case 'd':	/* DRAM section */
	    sections |= SECTION_DRAM;
	    break;
	case 's':	/* Shared Memory section */
	    sections |= SECTION_SHMEM;
	    break;
	case 'q':
	    verbose = 0;
	    break;
	default:
usage:
	    printf("usage: %s [-%s]\n", argv[0], optstr);
	    return(1);    
	}
    }
    if ( argc != optind ) 
	goto usage;
    if (!sections) 
	sections = SECTION_IO|SECTION_DRAM|SECTION_SHMEM;
    
    /*
     * Check that KSeg0 is set as cacheable
     */
    if ((getcp0_config() & CONFIG_KSEG0_COHERENCY) != TLB_CACHE_NONCOHERENT) {
	printf("%s: KSeg0 must be set as cacheable for this test\n", argv[0]);
	return(1);
    }

    /*
     * Before we test anything, we need to setup for bootstrap
     * vectors so the DRAM test does not destroy the exception table
     * and capture SIGINT. We then need to disable sh mem parity
     * and CPU parity exceptions.
     */
    savfcn = signal(SIGINT, SIG_IGN);
    move_vectors(1);
    shmem_ctl = KSEG1_IO_ASIC->shmem_ctl;
    KSEG1_IO_ASIC->shmem_ctl = (shmem_ctl & ~SHMEM_PERR_CLEAR);
    flush_io_wb();
    sreg = getcp0_sreg();
    setcp0_sreg(sreg | SREG_DE);

    /*
     * Test the various sections
     */
    if (sections & SECTION_DRAM) {
	test_dram_cycles(verbose);
    }
    if (sections & SECTION_SHMEM) {
	test_shmem_cycles(verbose);
    }
    if (sections & SECTION_IO) {
	test_io_cycles(verbose);
    }
    setcp0_sreg(sreg);
    KSEG1_IO_ASIC->shmem_ctl = shmem_ctl;
    flush_io_wb();
    signal(SIGINT, savfcn);
}

/* End of file */


