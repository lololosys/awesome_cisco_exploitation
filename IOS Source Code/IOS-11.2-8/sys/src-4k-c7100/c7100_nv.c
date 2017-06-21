/* $Id: c7100_nv.c,v 3.2.56.3 1996/09/10 02:16:01 ahh Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_nv.c,v $
 *------------------------------------------------------------------
 * c7100_nv.c - Predator NVRAM support
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_nv.c,v $
 * Revision 3.2.56.3  1996/09/10  02:16:01  ahh
 * CSCdi68092:  NVRAM in use errors with service compress-config
 * NVRAM access code fails to clean itself up properly when another user
 * tries to look at the config simultaneously.
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/05/21  10:03:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.56.1  1996/03/21  23:48:05  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.4  1996/02/20  02:43:09  mbeesley
 * CSCdi49348:  IOS needs to support packed NVRAM on Predator
 * Branch: ELC_branch
 *
 * Revision 3.2.2.3  1996/01/16  02:16:49  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.2  1995/12/09  22:52:15  mbeesley
 * Branch: ELC_branch
 * Defeat OIR, ENV processes until hw is verified. Fix initial bugs:
 *         o NVRAM access must be 32 bit cycle, only using bits 7:0
 *         o Always create DRAM particle pool
 *         o Read 32 bytes, not 64 bytes from MP eeprom
 *         o Correct some misdefined hw addresses
 *         o Make packet SRAM usage on Rev 1 optional, with default as
 *           no until hw is verified (define C7100_CPU_SRAM to use it)
 *
 * Revision 3.2.2.1  1995/11/22  21:43:17  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "exception.h"
#include "packet.h"
#include "stacks.h"
#include "logger.h"
#include "config.h"
#include "parser.h"

#include "../os/free.h"
#include "../os/nv.h"

#include "../dev/monitor1.h"

#include "../filesys/filesys.h"
#include "../filesys/flh_config.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "c7100_pcmap.h"
#include "c7100_fpga.h"

/*
 * Include the config-register parser support
 */

#define ALTERNATE       NONE
#include "../parser/cfg_configreg.h"
LINK_POINT(configreg_command, ALTERNATE);
#undef  ALTERNATE

/*
 * Define alignment macro
 */
#define LONG_ALIGN(x) (((x) + sizeof(long) - 1) & ~(sizeof(long) - 1))

/*
 * Define the byte gap in NVRAM. Note, in the packed NVRAM
 * case, this is used to increment a uchar pointer (by one byte).
 * In the unpacked case it is used to increment a ulong pointer
 * (therfore incrementing by 4 bytes)
 */
#define C7100_NVRAM_BYTE_GAP	1

/*
 * Local storage
 */
static uchar *c7100_nvdram_buffer = NULL;
static tinybool c7100_packed_nvram = FALSE; 

/*
 * c7100_nvram_ptr:
 * Return a pointer to physical NVRAM at offset from start
 */
static volatile uchar *c7100_nvram_ptr (int offset)
{
    if (c7100_packed_nvram) {
	return((volatile uchar *)(ADRSPC_NVRAM + offset));
    } else {
	return((volatile uchar *)(ADRSPC_NVRAM + (offset<<2)));
    }
}

/*
 * c7100_read_nvmagic:
 * Get the pointer to the NVRAM magic number.
 */
static ushort c7100_read_nvmagic (void)
{
    ushort nvmagic;
    volatile uchar *c_ptr;
    volatile ulong *l_ptr;

    c_ptr = c7100_nvram_ptr(mon_get_mon_nvram_bytes() + NVRAM_MAGIC_OFFSET);
    if (c7100_packed_nvram) {
	nvmagic = (*c_ptr << 8);		/* first byte */
	c_ptr += C7100_NVRAM_BYTE_GAP;		/* bump pointer */
	nvmagic |= (*c_ptr);			/* or in second byte */
    } else {
	l_ptr = (volatile ulong *)c_ptr;
	nvmagic = ((*l_ptr & 0xff) << 8);	/* first byte */
	l_ptr += C7100_NVRAM_BYTE_GAP;		/* bump pointer */
	nvmagic |= (*l_ptr & 0xff);		/* or in second byte */
    }
    return(nvmagic);
}

/*
 * c7100_write_nvmagic:
 * Write the magic number to NVRAM. Must write bytes at a time.
 */
static void c7100_write_nvmagic (ushort value)
{
    volatile uchar *c_ptr;
    volatile ulong *l_ptr;

    c_ptr = c7100_nvram_ptr(mon_get_mon_nvram_bytes() + NVRAM_MAGIC_OFFSET);

    if (c7100_packed_nvram) {
	*c_ptr = (value >> 8) & 0xff;		/* write first byte */
	c_ptr += C7100_NVRAM_BYTE_GAP;		/* bump pointer */
	*c_ptr = (value & 0xff);		/* write second byte */
    } else {
	l_ptr = (volatile ulong *)c_ptr;
	*l_ptr = (value >> 8) & 0xff;		/* write first byte */
	l_ptr += C7100_NVRAM_BYTE_GAP;		/* bump pointer */
	*l_ptr = (value & 0xff);		/* write second byte */
    }
}

/*
 * c7100_get_sys_nvsize:
 * Get the NVRAM size usable by the system image
 */
static long c7100_get_sys_nvsize (void)
{
    return((long) LONG_ALIGN(mon_get_nvram_size() - 
			     mon_get_mon_nvram_bytes() -
			     NVRAM_START_OFFSET));
}

/*
 * c7100_nv_readconfig:
 * Read NVRAM configuration register
 */
static ushort c7100_nv_readconfig (void)
{
    return(mon_getconfig());
}

/*
 * c7100_load_nvram:
 * Load NVRAM contents into DRAM buffer
 */
static nvtype *c7100_load_nvram (void)
{
    nvtype *nvram;
    nvtype_private *private_nv_info;
    volatile uchar *c_ptr;
    volatile ulong *l_ptr;
    int i;

    /*
     * Get the start address of NVRAM
     */
    c_ptr = c7100_nvram_ptr(mon_get_mon_nvram_bytes() + NVRAM_START_OFFSET);

    /*
     * Make sure we have a dram buffer
     */
    if (!c7100_nvdram_buffer) {
	printf(nomemory);
	return(NULL);
    }

    /*
     * Copy the NVRAM content into the DRAM buffer and check it.
     */
    l_ptr = (volatile ulong *)c_ptr;
    for (i=0; i<nvsize; i++) {
	c7100_nvdram_buffer[i] = (*l_ptr & 0xff);	/* read a byte */
	l_ptr += C7100_NVRAM_BYTE_GAP;			/* bump pointer */
    }
    nvram = (nvtype *)c7100_nvdram_buffer;
    if ((nvram->magic == NVMAGIC) &&
	(ipcrc((ushort *)nvram, nvsize) == 0)) {
	/*
	 * Good config in NVRAM. Adjust textbase & priv_textbase to 
	 * point to our shadow NVRAM. Then recalculate checksum since
	 * we are changing textbase. Note that the pointer returned may be 
	 * NULL, notably due to an erase of NVRAM.
	 */
	nvram->textbase += (ulong) nvram;
	private_nv_info = nv_private_getptr(nvram);
	if (private_nv_info &&
	    ((char *)private_nv_info < ((char *)&nvram->magic + nvsize)) &&
	    (private_nv_info->magic == NVMAGIC_PRIV)) {
	    private_nv_info->priv_textbase += (ulong) nvram;
	}
	nvram->checksum = 0;
	nvram->checksum = ipcrc((ushort *)nvram, nvsize);
    } else {
	/*
	 * Corrupted config in NVRAM. Adjust textbase
	 * since we have a shadow NVRAM. Don't adjust priv_textbase as
	 * we can't be confident enough that we can reliably find the 
	 * private config area.
	 */
	nvram->textbase += (ulong) nvram;
    }
    return(nvram);
}

/*
 * nv_getptr:
 * Return pointer to base of non-volatile memory
 */
nvtype *nv_getptr (void)
{
    nvtype *nv_avail;

    /*
     * Make sure no one is using NVRAM
     */
    if (nvptr) {
	printf("\nNon-Volatile memory is in use");
	return(NULL);
    }

    /*
     * Open flash config for subsequent processing.
     * Please refer to filesys/flash_config.c
     */
    if (open_flash_config(&nv_avail)) {
	if (nv_avail == NULL) {
	    close_flash_config();
	}
	return(nv_avail);
    }

    /*
     * Not using flash configuration. Fire ahead and use nvram
     */
    if (!c7100_packed_nvram) {
	nv_avail = c7100_load_nvram();
    } else {
	nv_avail = (nvtype *)c7100_nvram_ptr(mon_get_mon_nvram_bytes() + 
					     NVRAM_START_OFFSET);
    }
    if (nv_check_getptr(nv_avail)) {
	nvptr = nv_avail;
	return(nv_avail);
    } else {
	return(NULL);
    }
}

/*
 * nv_getptr_raw:
 * Return a pointer to NVRAM
 */
nvtype *nv_getptr_raw (void)
{
    nvtype *nv_avail;

    flash_config_on = FALSE;
    nv_avail = nv_getptr();
    flash_config_on = TRUE;
    return(nv_avail);
}

/*
 * nv_setconfig:
 * write a number to the NVRAM configuration register
 */
void nv_setconfig (ushort number)
{
    mon_set_confreg(number);
}

#define CONFIG_MAGIC		0xf0a5

/*
 * nv_badptr:
 * Given a correctly checksummed portion of non-volatile memory, examine
 * the data structure components more closely for sanity.
 * If the configuration magic number is set, return zero.
 */
boolean nv_badptr (nvtype *ptr)
{
    ushort nv_magic = c7100_read_nvmagic();
	
    if (nv_magic == CONFIG_MAGIC) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}

/*
 * nv_done:
 * Handles writing configuration data to memory.
 */
void nv_done (nvtype *ptr)
{
    nvtype_private *private_nv_info;
    volatile ulong *l_ptr;
    volatile uchar *c_ptr;
    uchar *src_ptr;
    int i;

    /*
     * To indicate we are done with flash config.
     */
    if (close_flash_config() == TRUE) {
	if (ptr == nvptr) {		/* Free lock */
	    nv_restore_nvptr(ptr);
	    nvptr = NULL;
	}
	return;
    }

    /*
     * Write magic number in NVRAM
     */
    nv_check_done(ptr);
    c7100_write_nvmagic(CONFIG_MAGIC);
    
    /*
     * Write the data into the NVRAM
     */
    if (!c7100_packed_nvram) {
       /* 
        * If there is a private area, make priv_textbase into an offset.
	* Do this before ptr->textbase itself becomes an offset. Note that
	* the pointer returned may be NULL, notably due to an erase of NVRAM.
        */
        private_nv_info = nv_private_getptr(ptr);
        if (private_nv_info &&
	    ((char *)private_nv_info < ((char *)&ptr->magic + nvsize)) &&
            (private_nv_info->magic == NVMAGIC_PRIV)) {
            private_nv_info->priv_textbase -= (ulong) ptr;
        }
	ptr->textbase -= (ulong)ptr;	/* make it an offset */
	ptr->checksum = 0;
	ptr->checksum = ipcrc((ushort *)ptr, nvsize);
	src_ptr = (uchar *)ptr;
	c_ptr = c7100_nvram_ptr(mon_get_mon_nvram_bytes() + 
				NVRAM_START_OFFSET);
	l_ptr = (volatile ulong *)c_ptr;
	for (i=0; i<nvsize; i++) {
	    *l_ptr = *src_ptr++;		/* write byte */
	    l_ptr += C7100_NVRAM_BYTE_GAP;	/* bump pointer */
	}
    }

    if (ptr == nvptr)			/* Free lock */
	nvptr = NULL;
    nv_writedisable(ptr);
}

/*
 * nv_done_raw:
 * Finish up raw access to NVRAM
 */
void nv_done_raw (nvtype *ptr)
{
    flash_config_on = FALSE;
    nv_done(ptr);
    flash_config_on = TRUE;
}

/*
 * nv_writeenable:
 * Set the write flag
 */
void nv_writeenable (nvtype *ptr)
{
    nv_writeflag = TRUE;
}

/*
 * nv_writedisable:
 * Clear the write flag
 */
void nv_writedisable (nvtype *ptr)
{
    nv_writeflag = FALSE;
}

/*
 * nv_init:
 * Discover the size of the NVRAM
 */
void nv_init (void)
{
    /*
     * See if we have packed NVRAM or not. The rom monitor
     * will have determined if we are on a rev 2 GT64010 or not and
     * will have configured the NVRAM chip select accordingly IE packed or 
     * not packed. It will have also set NVRAM_PACKED in the IO control
     * register if we are on rev 2 (packing NVRAM)
     */
    if (C7100_IO_FPGA->io_ctrl_reg & NVRAM_PACKED) {
	c7100_packed_nvram = TRUE;
    } else {
	c7100_packed_nvram = FALSE;
    }
    nvptr = NULL;			/* make sure unlocked */
    nvsize = c7100_get_sys_nvsize();

    /*
     * If we do not have packet NVRAM, allocate a dram buffer 
     * to hold NVRAM contents
     */
    if (!c7100_packed_nvram && !c7100_nvdram_buffer) {
	c7100_nvdram_buffer = malloc(nvsize);
    }

    /*
     * Install parser stuff
     */
    reg_add_config_register_read(0, c7100_nv_readconfig, "nv_readconfig");
    parser_add_commands(PARSE_ADD_CFG_TOP_CMD, &pname(configreg_command),
                        "configreg");
}

/* 
 * platform_nvvar_support
 * NVRAM variables supported
 */
boolean platform_nvvar_support (int var)
{
    if ((var == ROMMON_BOOT_VAR) ||
        (var == ROMMON_CONFIG_VAR) ||
        (var == ROMMON_BOOTLDR_VAR))
         return (TRUE);
    else
         return (FALSE);
}

/* end of file */
