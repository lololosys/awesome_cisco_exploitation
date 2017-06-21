/* $Id: nv_rp1.c,v 3.4.44.5 1996/09/10 02:15:42 ahh Exp $
 * $Source: /release/112/cvs/Xsys/hes/nv_rp1.c,v $
 *------------------------------------------------------------------
 * nv_rp1.c - CPU dependent support for non-volatile configuration memory
 *
 * January 1993, William H. Palmer
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nv_rp1.c,v $
 * Revision 3.4.44.5  1996/09/10  02:15:42  ahh
 * CSCdi68092:  NVRAM in use errors with service compress-config
 * NVRAM access code fails to clean itself up properly when another user
 * tries to look at the config simultaneously.
 * Branch: California_branch
 *
 * Revision 3.4.44.4  1996/06/28  23:06:20  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.44.3  1996/05/29  14:04:41  bew
 * CSCdi56794:  IOS private configuration not always recovered from NVRAM
 * Branch: California_branch
 * The private NVRAM structure priv_textbase wasn't properly getting turned
 * into an offset in NVRAM.
 *
 * Revision 3.4.44.2  1996/05/21  09:48:05  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.44.1  1996/03/18  19:43:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/07  09:40:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  14:19:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/17  18:27:32  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:19:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:29:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:43:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "subsys.h"
#include "../dev/monitor1.h"
#include "../os/nv.h"

#include "../../boot/src-68-c7000/pcmap.h"
#include "../../boot/src-68-c7000/confreg.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../filesys/filesys.h"
#include "../filesys/flh_config.h"

#define ALTERNATE       NONE
#include "../parser/cfg_configreg.h"
LINK_POINT(configreg_command, ALTERNATE);
#undef  ALTERNATE

#define TESTPATTERN  0x89ABCDEFL

boolean nv_writeflag;
uchar* nv_onboard;
hwidbtype *nv_idbp = NULL;		/* not used */

static long rp1_probe_nvram(void);
void rp1_buffer (uchar* base, uchar* dest, long size);

/*
 * nv_setconfig
 * write a number to the NVRAM configuration register:
 */
void nv_setconfig (
    ushort number)
{
    /* 7000 nvram is only byte addressable */
    ushort magic = CONFREG_MAGIC;
     
    if (!mon_swconfreg_support()) {
	printf("Boot ROMs do not support software config-register\n");
	return;
    }

    rp1_buffer((uchar *)&number,(uchar *)config_ptr,2);
    rp1_buffer((uchar *)&magic,(uchar *)config_magic_ptr,2);
}

/*
 * configreg_command
 * parse a number and store it as the "config register"
 */
void configreg_command (parseinfo *csb)
{
    if (csb->nvgen) {
	/* No NV generation of config register - use show hardware */
	return;
    }
    nv_setconfig(GETOBJ(int,1));
}

/*
 * nv_init
 * Initialize non-volatile memory support.
 * If Multibus memory is present, use it.
 * Only if Multibus memory is absent do we look for MCI applique memory.
 */

void nv_init (void)
{
    return;
}

static void nv_hes_init (subsystype *subsys)
{
    nvptr = NULL;		/* make sure unlocked */

    nvsize = rp1_probe_nvram();

    if (nvsize) {
	parser_add_commands(PARSE_ADD_CFG_TOP_CMD, &pname(configreg_command),
			    "configreg");
    }
}

/*
 * nv_getptr
 * Return pointer to base of a nvtype buffer which contains configuration
 */

nvtype *nv_getptr (void)
{
    uchar *ptr;
    nvtype *nv_avail;
    nvtype_private *private_nv_avail;

    if (nvptr) {  /* Check lock */
	printf("\nNon-Volatile memory is in use");
	return(NULL);
    }

    /*
     * Open flash config for subsequent processing.
     * If it returns TRUE then this is for reading
     * or for writing with error. nvp will be set
     * to NULL in error (for both read/write). In
     * this case we close flash config. It returns
     * FALSE only when write can be performed. Then
     * We continue to open nvram.
     */
    if (open_flash_config(&nv_avail) == TRUE) {
        if (nv_avail == NULL)
            close_flash_config();
        return nv_avail;
    }

    if (nv_onboard) {
	ptr = malloc(nvsize);
	if (!ptr) {
	    printf(nomemory);
	    return(NULL);
	}

	rp1_buffer((uchar *) RP1_NVBASE, ptr, nvsize);
	nvptr = (nvtype *) ptr;
	if ((nvptr->magic == NVMAGIC) && !ipcrc((ushort *)nvptr, nvsize)) {
            /*
             * Good config in NVRAM. Adjust textbase to point to
             * our shadow NVRAM. Then recalculate checksum since
             * we are changing textbase.
             */
	    nvptr->textbase += (ulong) ptr;	/* adjust to real pointer */
	    /*
	     * If there is a private area, adjust priv_textbase.
	     */
	    private_nv_avail = nv_private_getptr(nvptr);
	    if (((char *)private_nv_avail < ((char *)&nvptr->magic + nvsize))
		  && (private_nv_avail->magic == NVMAGIC_PRIV)) {
		private_nv_avail->priv_textbase += (ulong) ptr;
	    }
	    nvptr->checksum = 0;
	    nvptr->checksum = ipcrc((ushort *)nvptr, nvsize); /* fix chksum */
	}
        else {
            /*
             * Corrupted config in NVRAM. Adjust textbase anyway
             * since we have a shadow NVRAM.
             */
            nvptr->textbase += (ulong) ptr;
        }
	if (nv_check_getptr(nvptr)) {
	    return(nvptr);
	} else {
	    free(ptr);
	    nvptr = NULL;  /* release nvram */
	    return(NULL);
	}
    }
    return(NULL);
}

nvtype *nv_getptr_raw (void)
{
    nvtype *nv_avail;

    flash_config_on = FALSE;
    nv_avail = nv_getptr();
    flash_config_on = TRUE;
    return nv_avail;
}

/*
 * nv_badptr
 * Given a correctly checksummed portion of non-volatile memory, examine
 * the data structure components more closely for sanity.  Bad things happen
 * if, for example, we follow a CSC1 pointer on a CSC2 processor.
 */

boolean nv_badptr (
    nvtype* ptr)
{
    return(FALSE);
}

/*
 * nv_done
 * Handles writing configuration data to memory.
 */

void nv_done (nvtype *ptr)
{
    nvtype_private *private_nv_info;

    /*
     * To indicate we are done with the flash config.
     */
    if (close_flash_config() == TRUE) {
	if (ptr == nvptr) {		/* Free lock */
	    if (nv_onboard)
		nv_restore_nvptr(ptr);
	    nvptr = NULL;
	}
	return;
    }

    if (nv_onboard) {			/* Check for RP1 onboard NVRam */
	if (nv_check_done(ptr) && nv_writeflag) {
	    /* 
	     * If there is a private area, make priv_textbase into an offset too.
	     * Do this before ptr->textbase itself becomes an offset!
	     */
	    private_nv_info = nv_private_getptr(ptr);
	    if (((char *)private_nv_info < ((char *)&ptr->magic + nvsize)) &&
	       (private_nv_info->magic == NVMAGIC_PRIV)) {
	        private_nv_info->priv_textbase -= (ulong) ptr;
	    }
	    nvptr->textbase -= (ulong)nvptr;	 /* make into offset */
	    nvptr->checksum = 0;
	    nvptr->checksum = ipcrc((ushort *)nvptr, nvsize);
	    rp1_buffer((uchar *) nvptr, (uchar *) RP1_NVBASE, nvsize);
	}
	nv_writedisable(ptr);
	free(ptr);
	nvptr = NULL;
	return;
    }
}

void nv_done_raw (nvtype *ptr)
{
    flash_config_on = FALSE;
    nv_done(ptr);
    flash_config_on = TRUE;
}

/*
 * nv_writeenable
 * Get memory ready to be written
 */

void nv_writeenable (nvtype *ptr)
{
    if (ptr == nvptr)
	nv_writeflag = TRUE;
}

/*
 * nv_writedisable
 * Done writing memory, do hardware dependent actions.
 */

void nv_writedisable (nvtype *ptr)
{
    nv_writeflag = FALSE;
}

/*
 * nv_write_summary - Write the configuration summary
 *
 * This routine is used to write the configuration summary software
 * configuration register.
 */

void nv_write_summary (boolean eraseflag)
{
}

/*
 * nv_bcopy:
 * Used to write text directly to NVRAM
 */
void
nv_bcopy(void const *src, void *dst, int length)
{
    bcopy(src, dst, length);
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


/*
 * rp1_probe_nvram
 * Size RP1 NVRAM.  Note that it is necessary to write several different
 * locations and read them back to eliminate spurious indications due to
 * bus charging behavior.  We cannot use standard sizing loop used for
 * multibus NVRAM because RP1 NVRAM only supports byte accesses.  
 */

static long rp1_probe_nvram (void)
{
    long nvsize;
    leveltype SR;
    int i;
    volatile uchar read_verify, read_verify2, *rp1_nvptr;
    uchar orig_read, orig_read2;
    ulong testpattern = 0x3c55aa3c;
    
    nvsize = 0;
    nv_onboard = NULL;
    SR = raise_interrupt_level(ALL_DISABLE);
    rp1_nvptr = (uchar *) RP1_NVBASE;
    for (i=0; i<2; i++, rp1_nvptr++) {       
	/* check at least one longword's worth of */
	orig_read  = *rp1_nvptr;  /* data to reduce chances of random matches */
        orig_read2 = *(rp1_nvptr + 1);
        *rp1_nvptr     = (uchar) ((testpattern >> (i*8)) & 0xff);
        *(rp1_nvptr+1) = (uchar) ((testpattern >> ((i+1)*8)) & 0xff);
        read_verify  = *rp1_nvptr;
        read_verify2 = *(rp1_nvptr+1);
        *rp1_nvptr     = orig_read;
        *(rp1_nvptr+1) = orig_read2;
        if (read_verify != (uchar) ((testpattern >> (i*8)) & 0xff))
	    break;   /* if at least one of these fail, no nvram is present */
        if (read_verify2 != (uchar) ((testpattern >> ((i+1)*8)) & 0xff))
	    break;   /* if at least one of these fail, no nvram is present */
    }
    if (i == 2) {                  /* loop finished so RP1 nvram present */
	nvsize = NVMAXBYTES;
	nv_onboard = (uchar *) RP1_NVBASE;
	nv_writeflag = FALSE;
    } else
	nvsize = 0;
    reset_interrupt_level(SR);
    return (nvsize);
}


/*
 * rp1_buffer
 * Move contents of buffer to NVRam, or Vice-versa.
 * 
 * NOTE: These transfers MUST be 8-bit, because RP1 NVRam only supports
 *       8-bit reads/writes.
 */

void rp1_buffer (uchar *base, uchar *dest, long size)
{
    long local_size = 0;

    while( local_size++ < size)
       *dest++ = *base++;
}


#define NVRAM_MAJVERSION   1
#define NVRAM_MINVERSION   0
#define NVRAM_EDITVERSION  1

SUBSYS_HEADER(nvram, NVRAM_MAJVERSION, NVRAM_MINVERSION, NVRAM_EDITVERSION,
	      nv_hes_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

