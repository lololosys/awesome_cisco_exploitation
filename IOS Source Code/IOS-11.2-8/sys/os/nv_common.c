/* $Id: nv_common.c,v 3.12.20.15 1996/09/11 15:52:06 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/nv_common.c,v $
 *------------------------------------------------------------------
 * nv_common.c -- system independent support for non-volatile
 *                configuration memory
 *
 * 30-January-1987, Kirk Lougheed
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nv_common.c,v $
 * Revision 3.12.20.15  1996/09/11  15:52:06  snyder
 * CSCdi68773:  more things const
 *              536 out of data section
 *              100 out of image
 * Branch: California_branch
 *
 * Revision 3.12.20.14  1996/09/10  02:15:58  ahh
 * CSCdi68092:  NVRAM in use errors with service compress-config
 * NVRAM access code fails to clean itself up properly when another user
 * tries to look at the config simultaneously.
 * Branch: California_branch
 *
 * Revision 3.12.20.13  1996/08/28  13:03:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.20.12  1996/08/07  09:01:53  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.12.20.11  1996/08/03  04:06:13  che
 * CSCdi65047:  Kerberos SRVTAB keys not encrypted
 * Branch: California_branch
 * Clean up Private NVRAM code.
 *
 * Revision 3.12.20.10  1996/07/24  22:05:58  widmer
 * CSCdi60592:  Error: command buffer too small [OK]  Cant save to nvram
 * Branch: California_branch
 * Malloc buffer instead of using stack
 * Improve intuitivness of logic
 *
 * Revision 3.12.20.9  1996/06/28  23:25:00  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.12.20.8  1996/06/17  08:34:47  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.12.20.7  1996/05/29  14:07:10  bew
 * CSCdi56794:  IOS private configuration not always recovered from NVRAM
 * Branch: California_branch
 * Added nv_private_getptr() to find the beginning of private NVRAM. This
 * is called by some platform-specific code as well as
 * platform-independant code.
 *
 * Revision 3.12.20.6  1996/05/21  23:41:20  widmer
 * CSCdi55809:  privilege level 6 write mem erases config
 * Branch: California_branch
 * Do write mem at level 15
 *
 * Revision 3.12.20.5  1996/05/21  09:58:19  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.12.20.4  1996/05/09  14:39:55  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.12.20.3.4.1  1996/04/27  07:17:08  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.12.20.3  1996/04/19  15:24:39  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.12.20.2  1996/04/15  14:59:09  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.12.20.1.8.1  1996/04/08  01:59:53  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.12.20.1  1996/03/18  21:30:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.12.22.1  1996/03/25  02:20:33  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.12.4.2  1996/03/22  05:25:56  cakyol
 * Found a better way of determining the version number of the
 * config file at boot time.  Leverage from parsing of the
 * "version xx.yy" command.  Define a new variable which
 * represents this value named "global_boottime_config_version".
 * Restore all the other files back to what they were before this.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.12.4.1  1996/03/20  23:50:19  cakyol
 * The 11.2 LECS parser can now FULLY recognize pre 11.2 LECS
 * commands .  As a result of this, we also have a new systemwide
 * global uint variable named "global_nvconfig_version", which
 * reflects the nvram version number in it.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.12  1996/02/04  00:48:50  foster
 * CSCdi47925:  HSA write mem cannot update slaves config file.
 * pull test for slave out.
 *
 * Revision 3.11  1996/01/30  19:15:56  foster
 * CSCdi47228:  HSA slave image needs config parsing disabled
 *
 * Revision 3.10  1996/01/22  06:56:17  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.9  1995/12/17  18:34:20  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.8  1995/12/05  07:05:17  dbath
 * CSCdi43178:  rommon boot info not kept in sync on slave
 * Ship the three boot related environment variables, and the config
 * register setting, across to the slave whenever autosync is enabled,
 * or whenever the user manually syncs the slave config.
 *
 * Revision 3.7  1995/12/01  15:56:40  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.6  1995/11/24  21:32:34  yanke
 * Raw nvram access shouldn't update rom environment variables
 *
 * CSCdi42679:  new exec command should be privileged
 *
 * Revision 3.5  1995/11/17  23:50:07  thille
 * CSCdi36960:  setup messages overwrite previous lines
 * Too many blasted bugs, last commit was to wrong bugID.
 *
 * Revision 3.4  1995/11/17  23:44:59  thille
 * CSCdi36960:  setup messages overwrite previous lines
 * Fix errors made by folks who apparently weren't aware that for some
 * reason we prepend \n on the front of lines of output instead of
 * appending it to the end.
 *
 * Revision 3.3  1995/11/17  18:50:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  21:19:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/09/25  08:37:13  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.10  1995/09/23  05:57:50  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.9  1995/09/07  03:15:13  dcoli
 * CSCdi35405:  C1000 NVRAM : simultaneous config accesses can cause crash
 * Timestamp write-opens and check it in nv_review while printing config.
 * Don't allow multiple simultaneous access if compress-config in use since
 * only a single un-compress buffer is used.
 *
 * Revision 2.8  1995/08/20  03:31:53  izhu
 * CSCdi37872:  extra char in config file on slot0: on 7K after copy
 * running startup
 * check nvptr before write to flash card to avoid duplicated write
 *
 * Revision 2.7  1995/08/15  23:24:56  tli
 * CSCdi36981:  Need to be able to generate downward compatible
 * configurations
 *
 * Revision 2.6  1995/07/22  06:05:19  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Fix over-zealousness in limiting output of "Building Configuration..."
 *
 * Revision 2.5  1995/07/15  02:37:08  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.4  1995/06/23  18:48:23  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.3  1995/06/19  06:48:49  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  19:12:08  izhu
 * CSCdi34518:  vty section gets messed up when writing config to slot0:
 * fix tested on 7000, IGS, RSP, 4000, 4500, and 2500
 *
 * Revision 2.1  1995/06/07  21:57:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "logger.h"
#include "packet.h"
#include "subsys.h"
#include "../ip/ip.h"
#include "config.h"
#include "../parser/parser_actions.h"
#include "ttysrv.h"
#include "parser.h"
#include "../parser/parser_defs_config.h"
#include "../ui/command.h"
#include "../ui/debug.h"
#include "../os/os_debug.h"
#include "../os/config_compress.h"
#include "../os/nv.h"
#include "../h/file.h"
#include "../h/ctype.h"
#include "../ip/ip_registry.h"
#include "../os/boot.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "../dev/monitor1.h"
#include "../filesys/filesys.h"
#include "../filesys/flh_config.h"
#include "../snmp/config_history.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../h/auth_registry.regh"

long nvsize, nvused;		/* bytes of non-volatile memory, bytes used */
long configbufsize;		/* Bytes in configuration buffer */
nvtype *nvptr;			/* pointer to base of nv memory */
static uchar *nvptr_uncompressed;/* used by nvptr->textbase when compressed */
boolean nv_valid = FALSE;	/* TRUE if we parsed a valid NVRAM */
static sys_timestamp nv_write_open_time;/* time of last nv_write_open call */
static parseinfo *nv_csb;
static boolean nvchecksum;

/*
 * Following variables used for the encrypted NVRAM feature.
 */
static char *nv_master_des_key_one   = NULL;

/*
 * Following variables used for the private NVRAM area feature.
 */
static long privsize;
static long *privsizeptr = &privsize;
typedef struct nv_private_area_ {
    struct nv_private_area_ *next;
    uchar *parse_command;
} nv_private_area;
static nv_private_area *nv_private_area_head = NULL;
typedef enum nv_private_status_ {
    NV_PRIVATE_SAVED,
    NV_PRIVATE_NONE,
    NV_PRIVATE_ERROR
} nv_private_status;

static inline uchar *word_align (uchar *address) {
    int modulo = (int)address  % 4;
    return (address + (modulo ? 4 - modulo : 0));
}

#define DES_KEY_LENGTH 8

static const char nvram_in_use[] =
    "\nNon-Volatile memory is in use";

static const char msg_nv_low_mem[] = 
    "\nCouldn't allocate memory for compression %s %c";

#define NAC0_VERSION    (10*256+0)

/*
 * forward declarations
 */
static void nv_line_out(const char*, const int);
static int nv_adjust_version(nvtype *, int);
static int nv_setup_uncompress(nvtype *);
static char *nv_get_master_key(int);
static uchar *nv_get_private_area(int);
static nvtype_private *nv_private_isconfig(void);
static nv_private_status nv_save_private_config(void);
static void nv_build_private_config (void);

boolean nv_check_nvptr (void)
{
    return (nvptr != NULL);
}

void nv_set_nvptr (void *ptr)
{
    nvptr = ptr;
}

void nv_clear_nvptr (void)
{
    nvptr = NULL;
}

uint nv_get_config_size (void)
{
    if (compress_config_enable) {
	if (config_bytes < (NVCOMPRESS_MULTIPLIER * nvsize))
	    return nvsize * NVCOMPRESS_MULTIPLIER;
	else
	    return config_bytes;
    }
    else
	return config_bytes - sizeof(nvtype) - ENDSTRLEN;
}

/*
 * nv_check_getptr
 *
 * This function should be called by ALL the platform specific
 * nv_getptr() functions.  If "service compress-config" is not enabled,
 * all it does basically is set configbufsize.  If it is enabled, it
 * has to allocate a bigger buffer to uncompress the config into and
 * then point nv->textbase to point at it.  This function should only
 * return 0 if it is unable to malloc the necessary memory to setup
 * the uncompress buffer.  Other errors (e.g. magic, crc) are handled
 * by the higher levels.  The platform specific nv_getptr() functions
 * should check for the error condition and return NULL. 
 */
int nv_check_getptr (nvtype *nv)
{
    long new_size;
    boolean verbose=FALSE;

    if (!nv) 
	return 0;
    
    /* If we got bad magic, but compress_config_enable is set,
     * then we need to setup the uncompress buffer and point
     * to it.
     */
    if (nv->magic != NVMAGIC) { /* invalid nvram */
        if (nvram_debug)
            buginf("\nNVRAM: nv->magic != NVMAGIC, invalid nvram.");

	nv->textbase = NV_V1_TEXTBASE(nv);
	nv->version = NVVERSION;
	if (compress_config_enable) {
	    nv->textsize = 0;
	    if (!nv_adjust_version(nv, NVCOMPRESSVERSION)) {
		printf(msg_nv_low_mem, "adjustment", '1');
		return 0;
	    }
	} else {
	    configbufsize = nvsize - sizeof(nvtype) - ENDSTRLEN;
	}

        if (nv->textsize > 0) { /* Only checksum a valid config */
	    nv->checksum = 0;  
	    nv->checksum = ipcrc((ushort *)nv,nvsize);
	}
        if (nvram_debug)
            buginf("\nNVRAM: invalid nvram, but return(1).");

	return 1;
    }

    if (nvram_debug)
        buginf("\nNVRAM: nv->magic == NVMAGIC.");

    if (nv->version != NVVERSION && nv->version != NVCOMPRESSVERSION) {
	printf("\nUnexpected version of configuration memory (%d)\n", 
               nv->version);
	configbufsize = nvsize - sizeof(nvtype) - ENDSTRLEN;
	return 1;
    }

    if (nv_badptr(nv)) {
	printf("\nBad configuration memory structure -- try rewriting");
	return 1;
    }

    /*
     * Check version, if NVCOMPRESSVERSION we must uncompress and
     * point to nvptr_uncompressed.  If NVVERSION then check if
     * compress_config_enable is set.  If so then alloc nvptr_uncompressed
     * and point txtbase to it.  Otherwise just set configbufsize to
     * the size of physical nvram - overhead.
     */
    if (nv->version == NVCOMPRESSVERSION) {
        if (nv->textsize == 0)
            return 1;

	if (!nv_setup_uncompress(nv)) {
	    return 0;
	}

	if (verbose)
	    printf("Uncompressing configuration from %ld bytes to ", 
                   nv->textsize); 
	new_size = config_decompress(NV_V2_TEXTBASE(nv),
				     nv->textsize,
				     nvptr_uncompressed,
				     configbufsize+ENDSTRLEN);
	if (new_size <= 0) {
	    printf("Couldn't uncompress config (%ld)\n", new_size);
	    free(nvptr_uncompressed); 
	    nvptr_uncompressed = NULL;
	    configbufsize = nvsize - sizeof(nvtype) - ENDSTRLEN;
	    nv->checksum = 0;  /* screw up checksum */
	    return 1;
	}
	if (verbose) printf("%ld bytes\n", new_size); 
	nv->uncompress_size = nv->textsize;
	nv->textsize = new_size;
	nv->compressed = TRUE;
    } else { /* NVVERSION */
	if (compress_config_enable) {
	    if (!nv_adjust_version(nv, NVCOMPRESSVERSION)) {
		printf(msg_nv_low_mem, "adjustment", '2');
		return 0;
	    }
	    if (nv->textsize > 0) 
		bcopy(NV_V2_TEXTBASE(nv), nvptr_uncompressed, nv->textsize);
	    nv->compressed = FALSE;
	} 
	else 
  	    configbufsize = nvsize - sizeof(nvtype) - ENDSTRLEN;
    }
    /* This checksum doesn't cover nvptr_uncompressed */    
    if (nv->textsize > 0) {
        nv->checksum = 0;
        nv->checksum = ipcrc((ushort *)nv, nvsize);
    }
    return 1;
}

/*
 * nv_check_done
 *
 * This function should be called by ALL the platform specific
 * nv_done() functions, before they actually write the data back to
 * nvram.
 */
int nv_check_done (nvtype *nv)
{
    long new_size;
    boolean verbose=FALSE;
    char *priv_start;

    if (!nv) 
	return 0;

    /* 
     * If were not actually writting, why bother with the 
     * rest of this.  So fix the textbase pointer and bail.
     */
    if (!nv_writeflag) { 
	nv_restore_nvptr(nv);
	return 1;
    }

    /*
     * After a "write erase" the CRC will be hosed. 
     */
    if (ipcrc((ushort *)nv, nvsize) != 0) { 
	if (nvptr_uncompressed) {
	    free(nvptr_uncompressed);
	    nvptr_uncompressed = NULL; 
	}
	return 1;
    }
    
    /*
     * If compress_config_enable is set then check if the ROMs "know" 
     * about compressed configs then compress it!  Otherwise 
     * if we are NVCOMPRESSVERSION adjust to NVERSION and don't 
     * compress.
     */
    if (compress_config_enable) {
	if (!nv_rom_supports_nvcompress() && nv_writeflag) {
	    printf("\nBoot ROMs do not support NVRAM compression\n");
	    printf("Config NOT written to NVRAM\n");
	    nv_restore_nvptr(nv);
	    nv_writedisable(nv);
	    return 0; 
	}
	/* 
	 * hmm, I don't think we can hit this condition
	 */
	if (nv->version != NVCOMPRESSVERSION) {
	    if (!nv_adjust_version(nv, NVCOMPRESSVERSION)) {
		printf(msg_nv_low_mem, "adjustment", '3');
		return 0;
	    }
	    bcopy(NV_V2_TEXTBASE(nv), nvptr_uncompressed, nv->textsize);
	}
	if (nv->textsize > 0) {  /* don't compress after "write erase" */
	    if (nv_writeflag || verbose) 
		printf("Compressing configuration from %ld bytes to ", nv->textsize);
	     new_size = config_compress(nvptr_uncompressed,
				       nv->textsize,
				       NV_V2_TEXTBASE(nv),
				       (nvsize-sizeof(nvtype)));
	    if (new_size <= 0) {
		printf("\nError trying to compress nvram (%ld)\n", new_size);
		if (!nv_adjust_version(nv, NVVERSION)) {
		    printf(msg_nv_low_mem, "adjustment", '4');
		    return 0;
		}
		free(nvptr_uncompressed);
		nvptr_uncompressed = NULL;
		configbufsize = nvsize - sizeof(nvtype) - ENDSTRLEN;
		nv->checksum = 0;  /* screw up checksum */
		return 0;
	    }
	    if (nv_writeflag || verbose) printf("%ld bytes\n", new_size);
	    nv->compressed = TRUE;
	    nv->compress_type = UNIX_COMPRESS;
	    nv->uncompress_size = nv->textsize;
	    nv->textsize = new_size;
	    nv->textbase = NV_V2_TEXTBASE(nv);
	    nv->checksum = 0;
	    nv->checksum = ipcrc((ushort *)nv, nvsize);
	    free(nvptr_uncompressed);
	    nvptr_uncompressed = NULL; 
	} else { /* no need to compress a zero length config */
	    /*
	     * Another condition that I don't think we can hit
	     */
	    if (!nv_adjust_version(nv, NVVERSION)) {
		printf(msg_nv_low_mem, "adjustment", '5');
		return 0;
	    }
	}
    } else {  /* !compress_config_enable */
	if (nv->version == NVCOMPRESSVERSION)
	    if (!nv_adjust_version(nv ,NVVERSION)) {
		printf(msg_nv_low_mem, "adjustment", '6');
		return 0;
	    }
    }
    if (nvptr_uncompressed) {
	free(nvptr_uncompressed);
	nvptr_uncompressed = NULL;
    }

    /*
     * Recover the private NVRAM area. It is written uncompressed, but
     * will be small and is not likely to give good compression anyway.
     */
    if (nv_private_area_head) {
        nv_build_private_config();
    } else {
    	    /* 
         * Clear out the private area magic bytes to plainly declare
         * that there is no private area.
         */
        priv_start = (char *)nv_private_getptr(nvptr);
	if (priv_start) {
	    nv_bcopy("\0\0\0\0", priv_start, 4);
	}
    }
    nv->checksum = 0;
    nv->checksum = ipcrc((ushort *)nv, nvsize);
    return 1;
}
 
int nv_adjust_version (nvtype *nv, int new_version)
{
    uchar *ptr;
    
    if (!nv) 
	return 0;
    /*
     * NVRAM configuration data directly follows the
     * nvtype structure and nv->textbase points to it.
     * Since NVCOMPRESSVERSION's nvtype structure has
     * more fields to it, we must be able to adjust between
     * the two if we want older code to be able to read
     * nvram configuration.
     */
    if ((ptr=malloc(nv->textsize+1)) == NULL) {
	return 0;
    }
    switch (new_version) {
    case NVCOMPRESSVERSION:
	if (nv->textsize > 0)  {
	    bcopy(nv->textbase, ptr, nv->textsize);
	    nv->textbase = NV_V2_TEXTBASE(nv);
	    bcopy(ptr, nv->textbase, nv->textsize);
	} else {
	    nv->textbase = NV_V2_TEXTBASE(nv);
	}
	nv->version = NVCOMPRESSVERSION;
	nv->compressed = FALSE;
	nv->compress_type = 0;
	nv->uncompress_size = nv->textsize;
	if (!nv_setup_uncompress(nv))
	    return 0;
	break;
    case NVVERSION: 
	if (nv->textsize > (nvsize - sizeof(nvtype) - ENDSTRLEN)) {
	    printf("Configuration too large to fit uncompressed in NVRAM\n");
	    if (!confirm("Truncate configuration? ")) {
		printf("\nNot saving\n");
		nv_writedisable(nv);
		nv->textbase = NV_V2_TEXTBASE(nv);
		nv->textsize = nv->uncompress_size;
		free(ptr);
		nv->checksum = 0;
		nv->checksum = ipcrc((ushort *)nv, nvsize);
		return 1;
	    }
	    else {
		printf("\nTruncating\n");
	        nv->textsize = nvsize - sizeof(nvtype) - ENDSTRLEN - 1; /* TRUNCATE */
	    }
	} 
	if (nv->textsize > 0) {
	    bcopy(nv->textbase, ptr, nv->textsize);
	    nv->textbase = NV_V1_TEXTBASE(nv);
	    bcopy(ptr, nv->textbase, nv->textsize);
	} else {
	    nv->textbase = NV_V1_TEXTBASE(nv);
	}
	nv->version = NVVERSION;
	break;
    default:
	free(ptr);
	return 0;
    }
    free(ptr);
    nv->textptr = nv->textbase + nv->textsize;
    nv->checksum = 0;
    nv->checksum = ipcrc((ushort *)nv, nvsize);
    return new_version;
}

int nv_setup_uncompress (nvtype *nv)
{
    long second, third;

    if (!nv) 
	return 0;
    
    /*
     * Don't allow multiple access to NVRAM since only a single uncompress
     * buffer is used.
     */

    if (nvptr_uncompressed) {
	printf(nvram_in_use);
	return 0;
    }

    /* 
     *  Don't use nv_get_config_size() here, because it is possible to
     *  have a compressed config without having compress_config_enable.
     */
    configbufsize = config_bytes * NVCOMPRESS_MULTIPLIER;

    /*
     * I envision this next line being used by the system code in boot
     * ROM when flash/net booting an image that supports a bigger 
     * uncompressed config.
     */
    if (nv->compressed && (nv->uncompress_size > configbufsize + ENDSTRLEN))
	configbufsize = nv->uncompress_size;

    /* 
     * Three trys at allocating some memory:
     * - 1st try full nvram * multiplier
     * - 2nd try max(nvsize, current config size)
     * - 3rd try min(nvsize, current config size)
     *
     * This could be alot smarter if we knew what nv_getptr() was
     * going to used for.  For example, the only time we need to
     * allocate the max amount of memory is when we are writting the
     * running config to nvram (since we don't know how big it will
     * be).  If we are just doing a "show config", then we know
     * exactly how much to allocate (i.e. nv->uncompress_size).  In
     * some cases we shouldn't have to allocate any memory since we
     * are mainly interested in the nvram header (e.g.
     * ok_to_write_nv() and nv_erase()).  
     */
    if (!(nvptr_uncompressed=malloc(configbufsize+ENDSTRLEN))) {
	if (config_bytes > nv->uncompress_size) {
	    second = config_bytes;
	    third = nv->uncompress_size;
	} else {
	    second = nv->uncompress_size;
	    third = config_bytes;
	}
	configbufsize = second;  /* 2nd try */
	if (!(nvptr_uncompressed = malloc(configbufsize+ENDSTRLEN))) {
	    configbufsize = third; /* 3rd try */
	    if (!(nvptr_uncompressed = malloc(configbufsize+ENDSTRLEN))) {
		printf(msg_nv_low_mem, "setup", '1');
		configbufsize = nvsize - sizeof(nvtype) - ENDSTRLEN;
		nv->checksum = 0;  
		return 0;
	    }
	}
    }
    nv->textbase = nvptr_uncompressed;

    return 1;
}

void nv_restore_nvptr (nvtype *nv)
{
    ulong temp;

    /* 
     * On some platforms nvptr is not a pointer to a buffer,
     * but a pointer to nvram.  So for those cases we must
     * repair any pointers that we have changed.
     */
    if (nv->version == NVCOMPRESSVERSION) {
	if (nv->compressed) {
	    nv->textbase = NV_V2_TEXTBASE(nv);
 	    temp = nv->textsize;
	    nv->textsize = nv->uncompress_size;
 	    nv->uncompress_size = temp;
	    nv->checksum = 0;
	    nv->checksum = ipcrc((ushort *)nv, nvsize);
	} 
	else {
	    if (!nv_adjust_version(nv, NVVERSION)) {
		/* 
		 * since we couldn't allocate memory to convert back
		 * to NVVERSION, lets just leave it and correct for
     		 * NVCOMPRESSVERSION
		 */
		printf(msg_nv_low_mem, "adjustment", '7');
		nv->textbase = NV_V2_TEXTBASE(nv);
		nv->uncompress_size = nv->textsize;
		nv->checksum = 0;
		nv->checksum = ipcrc((ushort *)nv, nvsize);
	    }
	} 
    }
    if (nvptr_uncompressed) { /* if were not writing, why compress */
	free(nvptr_uncompressed);
	nvptr_uncompressed = NULL; 
    }
}

boolean nv_rom_supports_nvcompress (void)
{
    char *rom_version, *chptr, ch;
    int version, major, minor, num;

    /* 
     * Ugly hack to take a peek at what's in ROM and
     * decide whether that version knows how to 
     * read a compressed config.
     */
    rom_version = mon_vstring_sys_rom();
    if (!rom_version || rom_version == (char *) -1)
	return FALSE;

    /*
     * Look for the version number after "Version "
     */
    chptr = strstr(rom_version,"Version ");
    if (!chptr)
	return FALSE;
    chptr = strchr(chptr,' ');  
    if (!chptr)
	return FALSE;
    chptr++; 

    /* 
     * Convert text version number into a number
     */
    major = minor = num = 0;
    ch = *chptr++;
    while ((ch >= '0') && (ch <= '9')) {
	num = num*10 + (ch - '0');
	ch = *chptr++;
	if (ch == '.') {
	    ch = *chptr++;
	    major = num;
	    num = 0;
	}
    }
    minor = num;
    version = major*256 + minor;

    /* 
     * See if its at least up to NAC0 or later. 
     * Obviously this breaks when someone decides to 
     * make the next version 1.0
     */
    if (version >= NAC0_VERSION)
	return TRUE;
    else
	return FALSE;
}


/*
 * nv_configure
 * Attempt to configure from any non-volatile memory
 */

void nv_configure (uchar priv)
{
    nvtype *nv;
    uchar *ptr;
    int bytes;
    nvtype_private *private;

    /*
     * Run some sanity checks.  Existence, prior setup, correct
     * software version, and checksum of memory contents.
     * We execute this loop at most once.  We use breaks instead of goto's.
     */
    while (TRUE) {  
	if (nvsize == 0) {
	    printf("\nNotice: NVRAM device not found.\n");
	    return;
	}

	/*
	 * Set up flash config for read.
	 */
	reg_invoke_access_flash_config(SET_READ_FLASH_CONFIG);

	nv = nv_getptr();

	if (!nv) 
	    return;
	if (nv->magic != NVMAGIC) {
	    printf("\nNotice: NVRAM invalid, possibly due to write erase.\n");
	    break;
	}
	if (nv->version != NVVERSION && nv->version != NVCOMPRESSVERSION) {
	    printf("\nUnexpected version of configuration memory");
	    break;
	}
	if (ipcrc((ushort *)nv, nvsize)) {
	    printf("\nBad checksum for configuration memory");
	    break;
	}
	if (nv_badptr(nv)) {
	    printf("\nBad configuration memory structure -- try rewriting");
	    break;
	}

	/*
	 * nv_ram could have been over-filled by old versions, correct
	 * the possibility of this problem here.  Need the 'end' command.
	 * If we have less than 5 bytes free, assume we are in trouble.
	 */
	if ((nv->textsize + sizeof(nvtype) + ENDSTRLEN) > nvsize) {
	    nv->checksum = 0;
	    ptr = nv->textbase + nv->textsize - ENDSTRLEN;
	    bytes = _doprnt(ptr, NULL, "\nend\n", NULL, FALSE);
	    nv->textptr  = ptr + bytes;
	    nv->textsize = nv->textsize - ENDSTRLEN + bytes;
	    nv->checksum = ipcrc((ushort *)nv, nvsize);
	    if (ipcrc((ushort *)nv, nvsize)) {
		printf("\nBad checksum in reduction of configuration memory");
		break;
	    }
	}

	nvused = nv->textsize;
	
	/* 
	 * Parse the private area if one exists. Do this first so that
	 * any encryption keys are in memory when needed by the public config.
	 */
	private = nv_private_getptr(nv);

	/* 
	 * If private isn't beyond nvsize and the magic is right go
	 * ahead and configure it.
	 */
	if (private && ((char *)private < ((char *)&nv->magic + nvsize)) &&
	    (private->magic == NVMAGIC_PRIV)) {
	    parse_configure((char *)private->priv_textbase, TRUE, 
			    RES_PRIVATE_NONVOL, priv);
	}

	/*
	 * Parse the text region for configuration information,
	 * then exit while loop.
	 */
	parse_configure((char *)nv->textbase,TRUE, RES_NONVOL, priv);
	if (!system_loading)
	    notify_config("memory");
	nv_valid = TRUE;
	break;
    }
    nv_done(nv);
}

/*
 * nv_review_buffer
 * Print contents of NV memory on screen
 */

static const char nvcm[] = "\n%% Non-volatile configuration memory";

nvtype *nv_review_buffer (void)
{
    nvtype *nv;

    /*
     * Set up flash config for read.
     */
    reg_invoke_access_flash_config(SET_READ_FLASH_CONFIG);

    nv = nv_getptr();
    if (!nv)
	return((nvtype *)NULL);

    if (nvsize == 0) {
	printf("%s is not present", nvcm);
        nv_done(nv);
        return((nvtype *) NULL);
    }

    if (nv->magic != NVMAGIC) {
        if (nvram_debug)
            buginf("\nNVRAM: nv->magic != NVMAGIC ");
        printf("%s has not been set up", nvcm);
        nv_done(nv);
        return((nvtype *) NULL);
    }

    if (nvram_debug)
        buginf("\nNVRAM: NVRAM has been set up properly.");

    if (nv_badptr(nv)) {
	printf("%s has funny format -- try erasing and rewriting", nvcm);
        nv_done(nv);
        return((nvtype *) NULL);
    }

    if (nvram_debug)
        buginf("\nNVRAM: Format check is good.");

    /*
     * Note that even if there is checksum error, the user
     * may still like to read the NVRAM content if the nvram
     * debug mode is on...
     * Whether to print NVRAM content will be determined in
     * nv_review().  Hence, nv is always returned no matter
     * there is checksum error or not.
     */
    if ((nvchecksum = ipcrc((ushort *)nv, nvsize)))
	printf("%s has bad checksum", nvcm);
    else if (nvram_debug)
        buginf("\nNVRAM: After format check, CRC check good, all done.\n");

    return(nv);
}

boolean flash_config_on = TRUE;	/* save config to flash enable/disable flag */

nvtype *nv_review_buffer_raw (void)
{
    nvtype *nv;

    flash_config_on = FALSE;
    nv = nv_review_buffer();
    flash_config_on = TRUE;
    return nv;
}

/*
 * nv_review
 *                            Config corrupt
 *   nvram_debug  nvchecksum  warning msg?    Display startup_config?
 *       0           0        No warning      Yes
 *       0           1        No warning       No
 *       1           0        No warning      Yes
 *       1           1        Warning         Yes, print '?' for unprintable char
 */
void nv_review (void)
{
    register uchar *cp, uch;
    register int bytes;
    nvtype *nv;
    sys_timestamp last_write_open_time;

    last_write_open_time = nv_write_open_time;

    nv = nv_review_buffer();
    if (nv == NULL)
	return;

    reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
				    CONFIG_MEDIUM_STARTUP, 
				    CONFIG_MEDIUM_COMMANDSOURCE, 
				    0, NULL, NULL);

    automore_enable(NULL);

    if (nvchecksum && nvram_debug)
        buginf("\nNVRAM: WARNING- The startup config may have been corrupted\n");

    cp = nv->textbase;
    bytes = nv->textsize;

    if (!nvchecksum || nvram_debug) {
        if (nv->version == NVCOMPRESSVERSION && nv->compressed)
	    printf("\nUsing %d out of %ld bytes, uncompressed size = %ld bytes",
	           nv->uncompress_size, nvsize, bytes);
        else
	    printf("\nUsing %d out of %ld bytes", bytes, nvsize);

        while (bytes-- > 0) {
	    if (TIMERS_NOT_EQUAL(last_write_open_time, nv_write_open_time)) {
	        printf("NVRAM opened for write by another process -- quitting\n");
	        break;
	    }
            uch = *cp++;
            if (!nvchecksum || isprint(uch) || isspace(uch))
                printc(uch);
            else if (nvram_debug)
	        printc('?');
        }
    }
    automore_disable();
    nv_done(nv);
}

/*
 * nv_write_open
 */

boolean nv_write_open (void)
{
    nvtype *nv;

    if (nvsize == 0) {
	printf("%s is not present", nvcm);
	return FALSE;
    }

    nv = nv_getptr();		/* Try to get pointer */
    if (!nv)			/* give up if no mem or if locked */
      return FALSE;

    nvptr = nv;			/* it worked, so we can use nvptr */
    nv_writeenable(nvptr);
    GET_TIMESTAMP(nv_write_open_time);
    nvptr->checksum = 0;
    nvptr->textptr  = nvptr->textbase;
    nvptr->textsize = 0;
    nvptr->magic    = NVMAGIC;
    if (nvram_debug)
        buginf("\nNVRAM: nvptr->magic = NVMAGIC.");

    if (bootstrap_enable)
	nvptr->system_version = (ushort)NULL;
    else
	nvptr->system_version = (ushort)SYSTEM_VERSION;

    /*
     * Now initialize ROM monitor BOOT variable
	 * and CONFIG variable
     */
    reg_invoke_monvar_open(ROMMON_BOOT_VAR);
    reg_invoke_monvar_open(ROMMON_CONFIG_VAR);
    reg_invoke_monvar_open(ROMMON_BOOTLDR_VAR);
    return TRUE;
}

boolean nv_write_open_raw (void)
{
    boolean retcode;

    flash_config_on = FALSE;
    retcode = nv_write_open();
    flash_config_on = TRUE;
    return retcode;
}

/*
 * nv_write_close
 */

int nv_write_close (
    boolean vocal)
{
    int left;

    nvused = nvptr->textsize;
    nvptr->checksum = ipcrc((ushort *)nvptr, nvsize);
    nv_write_summary(FALSE);
    nv_done(nvptr);
    if (vocal)
        printf("[OK]");

    /*
     *	Set up flash config for read.
     */
    reg_invoke_access_flash_config(SET_READ_FLASH_CONFIG);
    nvptr = nv_getptr();

    if (!nvptr || ipcrc((ushort *)nvptr, nvsize)) {
        if (vocal)
	    printf("\nNVRAM Verification Failed");
    }

    nv_done(nvptr);
    nvptr = NULL;

    /*
     * Now set the ROM monitor BOOT variable
     * and CONFIG variable.
     */

    left = reg_invoke_monvar_set(ROMMON_BOOT_VAR, vocal);
    left = reg_invoke_monvar_set(ROMMON_CONFIG_VAR, vocal);
    left = reg_invoke_monvar_set(ROMMON_BOOTLDR_VAR, vocal);

    /*
     * Now pass any master/slave settings down to the ROM monitor(s)
     * as required for each platform.
     */
    reg_invoke_master_slave_set(vocal);

    return nvused;
}

int nv_write_close_raw (
    boolean vocal)
{
    nvused = nvptr->textsize;
    nvptr->checksum = ipcrc((ushort *)nvptr, nvsize);
    nv_write_summary(FALSE);
    nv_done(nvptr);
    if (vocal)
        printf("[OK]");

    nvptr = nv_getptr();

    if (!nvptr || ipcrc((ushort *)nvptr, nvsize)) {
        if (vocal)
	    printf("\nNVRAM Verification Failed");
    }

    nv_done(nvptr);
    nvptr = NULL;
    return nvused;
}

/*
 * Clear NV memory
 * mark it as empty
 */

int nv_erase (boolean vocal)
{
    /*
     * Set up flash config for erase.
     */
    reg_invoke_access_flash_config(SET_ERASE_FLASH_CONFIG);
    if (nv_write_open()) {
	memset(nvptr, 0, nvsize);
	if (nvptr_uncompressed) {
	    free(nvptr_uncompressed);
	    nvptr_uncompressed = NULL;
	}
	nvptr->checksum = 1;    /* cause nv_check_done() to fail */
	nv_write_summary(TRUE);
	nv_done(nvptr);
	if (vocal)
	  printf("[OK]");
	return 0;		/* success: 0 characters written */
    }
    if (vocal)
      printf("[Failed]");
    return -1;			/* error */
}

int nv_erase_raw (boolean vocal)
{
    int retcode;

    flash_config_on = FALSE;
    retcode = nv_erase(vocal);
    flash_config_on = TRUE;
    return retcode;
}

/*
 * ok_to_write_nv
 * Verify that it is ok to write to NV memory
 *
 * If no config image is present in NVRAM, or if an invalid config image
 * is present in NVRAM, return TRUE, because no confirmation is necessary.
 *
 * If this is a full system image, and the config in NVRAM was written
 * by a system image with the same major and minor number, return TRUE.
 * Else, confirm the NVRAM config write request, because new config
 * commands will be written to NVRAM, and a user may not want this.
 * If the write mem is confirmed, return TRUE, else, return FALSE.
 *
 * ### Arguably, it might be better to confirm only if the current system
 * image is older than the image that wrote the NVRAM config.
 *
 * If the config image in NVRAM was written by a bootstrap image,
 * nv->system_version will be NULL, and confirmation is never necessary,
 * so return TRUE.
 *
 * If this is a bootstrap image, and the NVRAM image contains a config
 * written by a full system image, ie nv->system_version is not NULL,
 * verify that the user really wants to truncate his full NVRAM config.
 */

boolean ok_to_write_nv (void)
{
    nvtype *nv;
    boolean ok;

    ok = FALSE;
    nv = NULL;

    /*
     * If there is no config image in NVRAM, or if it is invalid,
     * return TRUE. We expect nv_write_open to continue to print
     * the appropriate error messages, if necessary:
     */
    if (nvsize == 0) {
	ok = TRUE;
    } else {
	
	/*
	 *	Set up flash config for read.
	 */
	reg_invoke_access_flash_config(SET_READ_FLASH_CONFIG);

	nv = nv_getptr();		/* Try to get pointer */
	if (nv) {
	    if (nv->magic != NVMAGIC) {
 		ok = TRUE;
	    }
	    if (nv->version != NVVERSION && nv->version != NVCOMPRESSVERSION) {
		ok = TRUE;
	    }
	    if (ipcrc((ushort *)nv, nvsize)) {
		ok = TRUE;
	    }
	    if (nv_badptr(nv)) {
		ok = TRUE;
	    }
	    
	    /*
	     * Is this a full system image?
	     */
	    if (bootstrap_enable) {
		/*
		 * Nope, it is a bootstrap image. What image wrote the
		 * NVRAM config?
		 */
		if (nv->system_version) {
		    /*
		     * A full system image wrote the NVRAM configuration.
		     * Confirm that the user really wants to do this:
		     */
		    printf("Warning: Attempting to overwrite "
			   "an NVRAM configuration written\n");
		    printf("by a full system image. This bootstrap"
			   " software does not support\n");
		    printf("the full configuration command set. If"
			   " you perform this command now,\n");
		    printf("some configuration commands may be lost.\n");
		    if (confirm("Overwrite the previous"
				" NVRAM configuration?")) {
			ok = TRUE;
		    }
		} else {
		    /*
		     * A bootstrap image wrote the NVRAM configuration.
		     * It is always ok to overwrite it:
		     */
		    ok = TRUE;
		}
	    } else {
		if (!reg_invoke_ok_to_write_nv_check()) {
		
		    /*
		     * Yup, this is a full system image. What image wrote the
		     * NVRAM config?
		     */
		    if (nv->system_version) {
			/*
			 * The NVRAM config was written by a full system image.
			 * What version was the system image that wrote the NVRAM
			 * config?
			 */
			if (nv->system_version == ((ushort)SYSTEM_VERSION)) {
			    ok = TRUE;
			} else {
			    /*
			     * The current system image is a different version than
			     * the image that wrote NVRAM. Confirm that the user
			     * really wants to overwrite this NVRAM config:
			     */
			    printf("Warning: Attempting to overwrite an NVRAM "
				   "configuration previously written\n");
			    printf("by a different version of the system "
				   "image.\n");
			    if (confirm("Overwrite the previous NVRAM "
					"configuration?")) {
				ok = TRUE;
			    }
			}
		    } else {
			ok = TRUE;	/* NVRAM was written by bootstrap image */
		    }
		}
	    }
	} else {
	    ok = TRUE; 
	}
    }
    if (nv)
	nv_done(nv);		/* We are done using NVRAM */
    return(ok);
}

/*
 * Create a private config header. The build_location argument is used to
 * specify someplace other than NVRAM, if need be, but using real_priv_start
 * as the base for addresses.
 */
void nv_build_private_config_header (char *real_priv_start, 
				     nvtype_private *build_location)
{
    build_location->magic = NVMAGIC_PRIV;
    build_location->version = NVVERSION_1_PRIV;
    build_location->priv_textbase = real_priv_start + sizeof(nvtype_private);
    build_location->priv_textptr = build_location->priv_textbase;
    return;
}

/*
 * Place a nvtype_private_ structure immediately following the normal config
 * data, followed by private text. Called either after saving the private
 * statements in a list when generating the configuration, or after having
 * stored them away when modifying NVRAM (e.g., copy tftp startup-config).
 */
static void nv_build_private_config (void)
{
    nvtype_private *private_area;
    nv_private_area *statement, *last_statement;
    int count, space;
    char *priv_start;

    /*
     * Sanity checking
     */
    if (!nvptr) {
	printf("%% No context available for writing private area.\n");
	flush();
	return;
    }
    /*
     * Check for enough space for header & initialize the structure.
     * Note that NVRAM must be written a byte at a time, so we create the
     * struct in RAM to be copied later.
     */
    space = nv_get_config_size() - nvptr->textsize - ENDSTRLEN;
    if (space < sizeof(nvtype_private)) {
	printf("\n%% Configuration buffer full, can't add private area");
	return;
    }
    private_area = malloc(sizeof(nvtype_private));
    if (!private_area) {
	printf("\n%% Private configuration not written!");
	flush();
	return;
    }
    priv_start = (char *)nv_private_getptr(nvptr);
    if (priv_start) {
    	nv_build_private_config_header(priv_start, private_area);
	/*
	 * Copy the list of private statements to nvram. If we run out of space,
	 * go through the rest of the list anyway to free the memory.
	 */
	space = nv_get_config_size() - nvptr->textsize - ENDSTRLEN -
		sizeof(nvtype_private);
    } else {
	printf("%% Warning: private configuration not saved!\n");
	space = 0;
    }
    statement = nv_private_area_head;
    while (statement) {
	count = strlen(statement->parse_command);
    	if (space < count) {
	    printf("\n%% Configuration buffer full, can't add private command");
	    space = 0;
    	} else {
	    count = strlen(statement->parse_command);
	    nv_bcopy(statement->parse_command, private_area->priv_textptr, 
	    	     count);
	    private_area->priv_textptr += count;
	    private_area->priv_textsize += count;
	    space -= count;
	}
	last_statement = statement;
	statement = statement->next;
	free(last_statement->parse_command);
	free(last_statement);
    } 
    nv_private_area_head = NULL;
    /*
     * Write the 'end'
     */
    count = _doprnt(private_area->priv_textptr, NULL, "\nend\n", NULL, FALSE);
    private_area->priv_textptr += count;
    private_area->priv_textsize += count;
    /*
     * Copy the private area structure into NVRAM
     */
    nv_bcopy(private_area, priv_start, sizeof(nvtype_private));
    /*
     * If there is a slave, sync it up.
     */
    reg_invoke_sync_slave_private_config(FALSE, private_area->priv_textbase,
					 private_area->priv_textsize);
    return;
}

/*
 * nv_current_common
 * Write current configuration to NV memory
 */

static void nv_current_common (boolean vocal, uchar priv,
			       boolean hide_passwords, boolean write_nvram)
{
    parseinfo *csb;

    /* Build and initialize a csb and associated structures
     * to scan the parse tree.
     */
    csb = get_csb("NV Parser CSB");
    if (csb == NULL) {
	printf(nomemory);
	return;
    }

    if (vocal) {
	printf("\nBuilding configuration...\n");
	flush();
    }

    /*
     * Clean out the rommon variables
     */
    reg_invoke_monvar_open(ROMMON_BOOT_VAR);
    reg_invoke_monvar_open(ROMMON_CONFIG_VAR);
    reg_invoke_monvar_open(ROMMON_BOOTLDR_VAR);

    csb->nvgen = TRUE;
    csb->sense = TRUE;
    csb->priv = priv;
    csb->permanent = TRUE;
    csb->down_gen_failure = FALSE;
    if (hide_passwords) {
	csb->flags |= HIDE_PASSWORDS;
    }
    if (write_nvram) {
        csb->resolvemethod = RES_WRITING_NONVOL;
    }

    csb->mode = config_mode;
    push_node(csb, get_mode_nv(csb));

    nv_csb = csb;
    parse_token(csb);
    nv_csb = NULL;

    /*
     * Declare that we are saving the private config here since
     * we have vocal, but the saving actually happens in nv_check_done()
     * after any compression or adjusting of config type is done.
     */
    if (nv_private_area_head && vocal) {
	printf("\nBuilding private configuration...\n");
        flush();
    }

    if (csb->down_gen_failure) {
	errmsg(&msgsym(CONFIG_DOWNFAIL, SYS));
	if (vocal) {
	    printf("Generated configuration not downward compatible\n");
	    flush();
	}
    }
    free_csb(&csb);
}

/*
 * nv_current_to_nv
 */

int nv_current_to_nv (boolean vocal)
{
    /*
     * Set up flash config for write.
     */
    reg_invoke_access_flash_config(SET_WRITE_FLASH_CONFIG);

    if (!nv_write_open()) {
	if (vocal)
	  printf("[Failed]");
	return 0;
    }

    note_write_mem_time();  /* Note the time. */

   /* Walk the parse tree, generating as we go */
    nv_current_common(vocal, PRIV_MAX, FALSE, TRUE);

    return nv_write_close(vocal);
}

/*
 * nv_current_to_buffer
 * Write current configuration to a text buffer (used for write terminal)
 * dest = pointer to buffer, maxbuf = size of buffer, priv = privilege level
 */

int nv_current_to_buffer (char *dest, uint maxbuf, boolean vocal,
			  uchar priv, boolean hide_passwords)
{
    nvtype fake_base;
    unsigned written;

    if (nvptr) { 		/* Check lock */
	printf(nvram_in_use);
	return 0;
    }
    nvptr = &fake_base;
    nvptr->textbase = (uchar *) dest;
    nvptr->textptr  = nvptr->textbase;
    nvptr->textsize = 0;

    configbufsize = maxbuf - sizeof(nvtype) - ENDSTRLEN; 
    /* Walk the parse tree, generating as we go */
    nv_current_common(vocal, priv, hide_passwords, FALSE);
    written = nvptr->textsize;
    nvptr = NULL;

    return(written);
}

/*
 * nv_buffer_to_nv
 * Write text directly to NV memory, do not parse text
 * do not do sanity checking, do compute checksums et al.
 * Used by copy rcp startup-config and copy tftp startup-config
 */
int nv_buffer_to_nv (char *source, uint length, boolean vocal)
{
    nv_private_status private_status;

    /*
     *	If flash config is enabled, then we redirect the config
     *	buffer to startup file pointed by CONFIG_FILE.
     */
    if (reg_invoke_flash_config_enabled() == TRUE)
    	return reg_invoke_config_buffer_to_startup(source, length, vocal);

    if (length > configbufsize - sizeof(nvtype) - 1) {
	printf("\nConfiguration too large for %s", nvcm);
	return 0;
    }
    
    /*
     * Save the private NVRAM area before writing in a new config.
     * Do this before nw_write_open() which zeros nvptr->textsize.
     */
    private_status = nv_save_private_config();
    if (private_status == NV_PRIVATE_ERROR) {
	return nv_write_close(vocal);
    }

    if (!nv_write_open()) {
	if (vocal)
	  printf("[Failed]");
	return 0;
    }

    source[length + 1] = '\0';   /* terminate the buffer */

    nv_bcopy(source, nvptr->textptr, length + 1);
    nvptr->textsize = length;

    /*
     * No need to sync the private config here. It will be done in
     * nv_check_done() via a call to nv_build_private_config().
     */
    reg_invoke_sync_slave_config(FALSE, source, length);
   
    return nv_write_close(vocal);
}


static char *nv_encrypt_getptr (int bytes, int *buffer_length)
{
    char *textptr;
    int extra;
    /*
     * Get some space to put the plaintext. Make sure it is big enough
     * for the data rounded up to the next 64bit value -- data is encrypted
     * in 64bit chunks.
     */
    extra = (bytes % 8); /* 8 bytes == 64bits */
    if (extra) {
        *buffer_length = bytes + (8 - extra);
    } else {
	*buffer_length = bytes;
    }
    textptr = malloc(*buffer_length); 
    if (textptr == NULL) {
	printf(nomemory);
    }
    return(textptr);
}

boolean nv_encrypt (char *data, int bytes)
{
    int i;
    des_state_type *des_state;
    char *master_key;

    /*
     * Encrypt the data in place.
     */
    des_state = NULL;
    if (!reg_invoke_des_init(0, &des_state)) {
	printf("\n%% Failed to initialize DES!");
	return(FALSE);
    }

    master_key = nv_get_master_key(1);
    if (master_key == NULL) {
	printf("\n%% No encryption key!");
	return(FALSE);
    }
    reg_invoke_des_setkey(des_state, master_key);
    for (i=0; i<bytes ;i+=8) {
	/*
	 * Encrypt an 8 byte chunk at a time, DES ECB mode.
	 */
        reg_invoke_des_endes(des_state, &data[i]);
    }
    reg_invoke_des_done(des_state);
    des_state = NULL;

    return(TRUE);
}

static boolean nv_do_encrypt(int *bytes, char *string, va_list args, uchar *textptr)
{
    int buffer_length;
    char *encryptptr = nv_encrypt_getptr(*bytes, &buffer_length); 

    if (!encryptptr) {
	return(FALSE);
    }

    (void) _doprnt(encryptptr, NULL, string, args, FALSE);
    if (!nv_encrypt(encryptptr, buffer_length)) {
	free(encryptptr);
	return(FALSE);
    }

    /*
     * Covert to binary coded decimal and move the encrypted data 
     * to NVRAM. Note that we copy out twice as many bytes as
     * the plaintext version of the data, rounded up the next multiple of
     * eight bytes.
     */
    *bytes = buffer_length * 2;
    bcdtochar(encryptptr, textptr, *bytes);

    free(encryptptr);

    return(TRUE);
}

/*
 * nv_decrypt() -
 *	Pass in HEXified encrypted binary string and the length of said string.
 *	Returns decrypted data in-place as well as the length of the new data.
 */
boolean nv_decrypt (char *config_data, int *expanded_bytes)
{
    int bytes, buffer_length;
    char *data;
    des_state_type *des_state;
    char *master_key;
    int i;

    /*
     * Verify first that encryption keys have been set.
     */
    if (!nv_there_are_keys()) {
	return(FALSE);
    }

    /*
     * Translate binary-to-decimal encoding, move the ciphertext into local 
     * storage, decrypt it there, then
     * copy it back to the original location. We go through this rigamorole
     * only because the DES code needs exactly 64bit data going into it, but
     * we aren't guarenteed that they ciphertext is exactly that length.
     */
    bytes = (*expanded_bytes)/2; /* size of actual unexpanded data */
    data = nv_encrypt_getptr(bytes+1, &buffer_length); /* 1 extra for '\0' */
    if (!data) {
	return(FALSE);
    }
    chartobcd(config_data, data, *expanded_bytes);

    /*
     * Decrypt the unexpanded ciphertext in place to become plaintext.
     */
    des_state = NULL;
    if (!reg_invoke_des_init(0, &des_state)) {
	printf("\n%% Failed to initialize DES!");
	free(data);
	return(FALSE);
    }

    master_key = nv_get_master_key(1);
    if (master_key == NULL) {
	printf("\n%% No encryption key!");
	free(data);
	return(FALSE);
    }
    reg_invoke_des_setkey(des_state, master_key);
    for (i=0; i<buffer_length ;i+=8) {
	/*
	 * Encrypt an 8 byte chunk at a time, DES ECB mode.
	 */
        reg_invoke_des_dedes(des_state, &data[i]);
    }
    reg_invoke_des_done(des_state);
    des_state = NULL;

    /*
     * copy the plaintext to the config_data pointer.
     * Put a NULL at the end of the actual data for string manipulation
     */
    *(data + ((*expanded_bytes)/2)) = '\0';
    memcpy(config_data, data, bytes+1);
    *expanded_bytes = bytes;

    free(data);
    return(TRUE);
}

/*
 * nv_get_private_area
 * Allocate a list element to the list of statements to be written to private
 * NVRAM. Either generated when parsing the configuration, or called from code
 * stored the current private area here while modifying NVRAM directly
 * (e.g., copy tftp startup-config). 
 */
static uchar *nv_get_private_area (int size) {
    nv_private_area *new_element, *element;

    new_element = malloc(sizeof(nv_private_area));
    if (!new_element) {
        return (NULL);
    }
    new_element->parse_command = malloc(size+1); /* add 1 for NULL */
    if (new_element->parse_command) {
	/*
	 * Link it up now
	 */
	if (nv_private_area_head) {
	    /*
	     * Put it at the end
	     */
	    element = nv_private_area_head;
	    while (element->next) {
		element = element->next;
	    }
	    element->next = new_element;
	} else {
	    /*
	     * Start the list
	     */
	    nv_private_area_head = new_element;
	}
    } else {
	/*
	 * Couldn't get memory for the data. Clean up
	 */
	free(new_element);
        return (NULL);
    }
    return (new_element->parse_command);
}

/*
 * nv_write_internal
 * Do the work of nv_write() and nv_write_encrypted().
 */
static nvtype *nv_tmp_nvptr = NULL;

static
void nv_write_internal (boolean predicate, boolean encrypt, boolean private,
			char *string, va_list args)
{
    int bytes;
    int space;
    boolean add_space = (nv_csb && (nv_csb->mode != config_mode));
    uchar *where, **textptr;
    long *textsizeptr;

    /*
     * Allow other processes to run.  Avoid being a cpu hog.
     */
    process_suspend_if_req();
  
    /*
     * This routine check if this config should be distilled.
     * It returns TRUE if yes so it won't be written to
     * nvram. It also generate the config into flash at
     * the same time.
     */
    /* When writing line configuration(i.e. when nv_tmp_nvptr!=NULL), 
     * we need to make sure it won't write to flash card when it is
     * supposed to write only to temporary buffer (nv_line_nvptr, etc.)
     * It should write line configuration to flash card 
     * only when nvptr == nv_tmp_nvptr 
     *
     * nv_tmp_nvptr is NULL when not writing line configuration.
     *
     * Don't ever write the private configuation to flash. Always write that to 
     * NVRAM.
     */
    if (!private && ((nv_tmp_nvptr == NULL) || (nv_tmp_nvptr == nvptr))) {
        if (reg_invoke_write_flash_config(TRUE, string, args)) {
	    return;
	}
    }

    bytes = _doprnt(NULL, NULLTTY, string, args, FALSE);
    if (add_space) {
        bytes++;
    }

    space = configbufsize - nvptr->textsize;
    if (space < (bytes + 1)) {
        printf("\n%% Configuration buffer full, can't add command: ");
        (void) _doprnt(NULL, stdio, string, args, FALSE);
        return;
    }
    
    /*
     * Pick the right pointer
     */
    if (private) {
        /*
         * Allocate a buffer and link it to the list of
         * private area config statements.
         *
         * 'bytes' includes add_space if necessary, but not '\n', so need
         * to add 1.
         */
        where = nv_get_private_area(bytes+1);
        if (!where) {
    	     return;
        }
        textptr = &where;
        textsizeptr = privsizeptr;
    } else {
        textptr = (uchar **) &nvptr->textptr;
        textsizeptr = &nvptr->textsize;
    }

    **textptr = '\n';
    *textptr += 1;
    *textsizeptr += 1;
    if (add_space) {
        **textptr = ' ';
        *textptr += 1;
        *textsizeptr+= 1;
    }

    if (encrypt && nv_there_are_keys()) {
        nv_do_encrypt(&bytes, string, args, *textptr);
    } else {
        bytes = _doprnt(*textptr, NULL, string, args, FALSE);
    }

    if (!private) {
        *textptr += bytes;
    }
    *textsizeptr += bytes;
}

/*
 * nv_write
 * A special form of sprintf for nv memory.
 * If predicate is non-zero, write the string, else return.
 * Keeps track of global pointers and byte counts.
 * Prepends a \n to each string.
 */
void nv_write (boolean predicate, char *string, ...)
{
    va_list args;

    va_start(args, string);
    if (nvptr && predicate) {
        nv_write_internal(predicate, FALSE, FALSE, string, args);
    }
    va_end(args);
}

/*
 * nv_write_encrypted
 * A special form of nv_write which encrypts the data using only locally
 * available keys.
 *
 * WARNING! DO NOT USE THIS TO ENCRYPT ANYTHING OTHER THAN AUTHENTICATION DATA
 *          OR CISCO WILL BE IN VIOLATION OF US GOVERNMENT EXPORT RESTRICTIONS.
 *          ACCEPTABLE USE IS RESTRICTED TO PASSWORDS, ENCRYPTION KEYS, KEY
 *          MANAGEMENT CERTIFICATES, AND THE LIKE.
 */
void nv_write_encrypted (boolean predicate, char *string, ...)
{
    va_list args;

    va_start(args, string);
    if (nvptr && predicate) {
    	nv_write_internal(predicate, TRUE, FALSE, string, args);
    }
    va_end(args);
}

/*
 * nv_write_private
 * A special form of nv_write which writes the data to RAM as a holding
 * place. After the normal configuration is genned, this data is re-written
 * as a separate parse chain in NVRAM. If the mode is writing somewhere besides
 * NVRAM (e.g., network, terminal) then it just returns.
 */
void nv_write_private (boolean predicate, char *string, ...)
{
    va_list args;

    va_start(args, string);
    /*
     * Only write out if we are configuring to memory.
     */
    if (!nv_csb || (nv_csb->resolvemethod != RES_WRITING_NONVOL)) {
	return;
    }
    if (nvptr && predicate) {
    	nv_write_internal(predicate, FALSE, TRUE, string, args);
    }
    va_end(args);
}


void nv_write_raw (boolean predicate, char *string, ...)
{
    va_list args;
    int bytes;
    int space;
    boolean add_space = (nv_csb && (nv_csb->mode != config_mode));

    va_start(args, string);
    if (nvptr && predicate) {
	bytes = _doprnt(NULL, NULLTTY, string, args, FALSE);
	if (add_space) {
	    bytes++;
	}
	space = configbufsize - nvptr->textsize;
	if (space < (bytes + 1)) {
	    printf("\n%% Configuration buffer full, can't add command: ");
	    (void) _doprnt(NULL, stdio, string, args, FALSE);
	    return;
	}
	*nvptr->textptr++ = '\n';
	nvptr->textsize++;
	if (add_space) {
	    *nvptr->textptr++ = ' ';
	    nvptr->textsize++;
	}
	bytes = _doprnt(nvptr->textptr, NULL, string, args, FALSE);
	nvptr->textptr += bytes;
	nvptr->textsize += bytes;
    }
    va_end(args);
}

/*
 * nv_add_internal
 * Do the work of nv_add(), nv_add_encrypted(), and nv_add_private().
 */

static void nv_add_internal (boolean predicate, boolean encrypt, boolean private,
		      char *string, va_list args)
{
    int bytes;
    int space;
    uchar *where, **textptr;
    long *textsizeptr;

    /*
     * This routine check if this config should be distilled.
     * It returns TRUE if yes so it won't be written to
     * nvram. It also generate the config into flash at
     * the same time.
     */

    /* When writing line configuration(i.e. when nv_tmp_nvptr!=NULL), 
     * we need to make sure it won't write to flash card when it is
     * supposed to write only to temporary buffer (nv_line_nvptr, etc.)
     * It should write line configuration to flash card 
     * only when nvptr == nv_tmp_nvptr 
     *
     * nv_tmp_nvptr is NULL when not writing line configuration.
     */
    if (!private && ((nv_tmp_nvptr == NULL) || (nv_tmp_nvptr == nvptr))) {
        if (reg_invoke_write_flash_config(FALSE, string, args)) {
	    return;
	}
    }
    
    bytes = _doprnt(NULL, NULLTTY, string, args, FALSE);
    space = configbufsize - nvptr->textsize;
    if (space < (bytes + 1))
        return;
    
    /*
     * Pick the right pointer
     */
    if (private) {
        /*
         * Allocate a buffer and link it to the list of
         * private area config statements.
         */
        where = nv_get_private_area(bytes+1);
        if (!where) {
            return;
        }
        textptr = &where;
        textsizeptr = privsizeptr;
    } else {
        textptr = (uchar **) &nvptr->textptr;
        textsizeptr = &nvptr->textsize;
    }

    if (encrypt && nv_there_are_keys()) {
        nv_do_encrypt(&bytes, string, args, *textptr);
    } else {
        bytes = _doprnt(*textptr, NULL, string, args, FALSE);
    }
    if (!private) {
        *textptr += bytes;
    }
    *textsizeptr += bytes;
}

/*
 * nv_add
 * Like nv_write(), but doesn't prepend the newline.
 */

void nv_add (boolean predicate, char *string, ...)
{
    va_list args;

    va_start(args, string);
    if (nvptr && predicate) {
    	nv_add_internal(predicate, FALSE, FALSE, string, args);
    }
    va_end(args);
}

/*
 * nv_add_encrypted
 * A special form of nv_add which encrypts the data using only locally
 * available keys.
 *
 * WARNING! DO NOT USE THIS TO ENCRYPT ANYTHING OTHER THAN AUTHENTICATION DATA
 *          OR CISCO WILL BE IN VIOLATION OF US GOVERNMENT EXPORT RESTRICTIONS.
 *          ACCEPTABLE USE IS RESTRICTED TO PASSWORDS, ENCRYPTION KEYS, KEY
 *          MANAGEMENT CERTIFICATES, AND THE LIKE.
 */

void nv_add_encrypted (boolean predicate, char *string, ...)
{
    va_list args;

    va_start(args, string);
    if (nvptr && predicate) {
    	nv_add_internal(predicate, TRUE, FALSE, string, args);
    }
    va_end(args);
}

/*
 * nv_add_private
 * A special form of nv_add which writes the data to RAM as a holding
 * place. After the normal configuration is genned, this data is re-written
 * as a separate parse chain in NVRAM.
 */
void nv_add_private (boolean predicate, char *string, ...)
{
    va_list args;

    va_start(args, string);
    /*
     * Only write out if we are configuring to memory.
     */
    if (!nv_csb || (nv_csb->resolvemethod != RES_WRITING_NONVOL)) {
	return;
    }
    if (nvptr && predicate) {
    	nv_add_internal(predicate, FALSE, TRUE, string, args);
    }
    va_end(args);
}


/*
 * nv_line
 *
 */
static const char *nv_line_type = NULL;
char *nv_line_buffer;
static int nv_line_number = 0;
static int nv_starting_line = -1;
static nvtype *nv_line_nvptr = NULL;
static long nv_tmp_configbufsize;

void nv_line (const char *typename, const int number)
{
    ulong size;

    /* check output */
    if (nv_line_nvptr != NULL) {
	nv_line_out(typename, number);
    } else {
	/* get nv structure */
	if ((nv_line_nvptr = malloc(sizeof(nvtype))) == NULL) {
	    /* just do it the old way */
	    nv_write(TRUE, "line %s%t", typename, number);
	    return;
	}
	nv_tmp_nvptr = nvptr;
	nv_tmp_configbufsize = configbufsize;
	nv_line_nvptr->magic = nvptr->magic;
	nv_line_nvptr->version = nvptr->version;
	nv_line_nvptr->checksum = 0;
	nv_line_nvptr->system_version = nvptr->system_version;
	nv_line_nvptr->textbase = NULL;
	nv_line_nvptr->textptr = NULL;
	nv_line_nvptr->textsize = 0;

	nv_starting_line = number;
    }

    if (nv_line_nvptr->textbase) {
	/*
	 * Use the old buffer
	 */
	nv_line_nvptr->textbase[0] = '\0';
	nv_line_nvptr->textptr = nv_line_nvptr->textbase;
	nv_line_nvptr->textsize = 0;
    } else {
	/*
	 * Get a new output buffer
	 */
	size = configbufsize - nvptr->textsize;
	size = (size < 1<<12) ? size : 1<<12;
	nv_line_nvptr->textsize = 0;
	if (size < 64) {
	    /*
	     * If size is < 64, then we are running out of space,
	     * so just do line NV generation the old way
	     */
	} else {
	    nv_line_nvptr->textbase = malloc(size);
	}
	if (!nv_line_nvptr->textbase) {
	    /* just do it the old way */
	    nv_line_end();
	    nv_write(TRUE, "line %s%t", typename, number);
	    return;
	}
    }

    /* now set up the variables */
    nv_line_nvptr->textptr = nv_line_nvptr->textbase;
    nvptr = nv_line_nvptr;
    configbufsize = nv_tmp_configbufsize;
    nv_line_type = typename;
    nv_line_number = number;
}

/*
 * nv_write_line
 */
static void nv_write_line (const char *type, int start,
			   const int end, char *buf)
{
    if (buf) {
	if (start == end) {
	    nv_write(TRUE, "line %s%t%s", type, end, buf);
	} else {
	    nv_write(TRUE, "line %s%t %t%s", type, start, end, buf);
	}
    }
}

/*
 * nv_line_out
 */

void nv_line_out (const char *typename, const int number)
{
    /*
     * Reset the NV pointers
     */
    nvptr = nv_tmp_nvptr;
    configbufsize = nv_tmp_configbufsize;
    if (nv_line_nvptr->textbase) {
	nv_line_nvptr->textbase[nv_line_nvptr->textsize] = '\0';
    }

    if (((int)typename == -1) ||
	(strcmp(typename, nv_line_type) != 0) ||
        (number != (nv_line_number+1))) {
        /*
         * One of two things has happened - either the line type has
         * changed or the line type is the same but there is a gap in the
         * lines. The latter can happen on some of the newer platforms
         * where slow-speed serial lines are capable of being sync or 
         * async modes. When they are in sync mode we will suppress NV
         * generation of line mode commands for that interface. When such 
         * a gap is detected, we will simply output line information until
         * that gap right away.
         */
	if ((nv_starting_line != nv_line_number) &&
	    (strcmp(nv_line_buffer, nv_line_nvptr->textbase) != 0)) {
	    /*
	     * The two previous lines of the same type are different, 
             * so output the older line output, output the newer line
	     * output, and reset the buffers
	     */
	    if (nv_line_buffer) {
		nv_write_line(nv_line_type, nv_starting_line,
			      nv_line_number-1, nv_line_buffer);
		nv_line_buffer[0] = '\0';
	    }
	    if (nv_line_nvptr->textbase) {
		nv_write_line(nv_line_type, nv_line_number, nv_line_number,
			      nv_line_nvptr->textbase);
		nv_line_nvptr->textbase[0] = '\0';
	    }
	    nv_starting_line = number;
	} else {
	    /*
	     * Either there was only one line of the previous type or 
             * the two previous lines of the same type have the same 
             * output, so output the newer line output, and reset the 
             * buffers
	     */
	    if (nv_line_buffer) {
		nv_line_buffer[0] = '\0';
	    }
	    if (nv_line_nvptr->textbase) {
		nv_write_line(nv_line_type, nv_starting_line,
			      nv_line_number, nv_line_nvptr->textbase);
		nv_line_nvptr->textbase[0] = '\0';
	    }
	    nv_starting_line = number;
	}
    } else {
	if ((strcmp(nv_line_buffer, nv_line_nvptr->textbase) != 0) ||
	    ((strcmp(typename, "vty ") == 0) && (nv_line_number == 5))) {
            char *tmp;
	    /*
	     * The line type is the same, but the current output
	     * is different from the old output or we just past
	     * the 4th vty line, so write the old output and make
	     * the current output the old output. We break up the
	     * vty lines after the 4th line so that there are no
	     * lines like "line vty 0 100" which will break older
	     * software.
	     */
	    if (nv_line_buffer &&
		((nv_line_buffer[0]) || (nv_starting_line != 0)) &&
                (nv_starting_line != nv_line_number)) {
		nv_write_line(nv_line_type, nv_starting_line,
			      nv_line_number-1, nv_line_buffer);
		nv_line_buffer[0] = '\0';
	    }
	    tmp = nv_line_buffer;
	    nv_line_buffer = nv_line_nvptr->textbase;
	    nv_line_nvptr->textbase = tmp;
	    nv_starting_line = number-1;
	} else {
	    /*
	     * Old output is equal to current output, so just
	     * reuse current output buffer for next line.
	     */
	    if (nv_line_nvptr->textbase) {
		nv_line_nvptr->textbase[0] = '\0';
	    }
	}
    }
}

/*
 * nv_line_end
 */
void nv_line_end (void)
{
    if (nv_line_nvptr) {
	/*
	 * Print out the last lines and free the buffers
	 */
	nv_line_out((const char *)-1, 0);
	free(nv_line_buffer);
	free(nv_line_nvptr->textbase);
	free(nv_line_nvptr);
    }
    /*
     * Reset the variables
     */
    nv_line_type = NULL;
    nv_line_buffer = NULL;
    nv_line_number = 0;
    nv_starting_line = -1;
    nv_line_nvptr = NULL;
    nv_tmp_nvptr = NULL;
}

void nv_store_master_keys (void)
{
    if (!nv_master_des_key_one) {
	return;
    }
    nv_write_private(TRUE, "key config-key %d %s", 1, nv_master_des_key_one);
    return;
}

boolean nv_there_are_keys (void)
{
    return(nv_master_des_key_one != NULL);
}

void nv_set_master_key (int key_number, char *key, boolean set_key)
{
    switch (key_number) {
    case 1: 
	/*
	 * If there was a key, get rid of it.
	 */
	if (nv_master_des_key_one) {
		free(nv_master_des_key_one);
	}
	if (!set_key) {
	    nv_master_des_key_one = NULL;
	    return;
	}
	/* 
	 * Copy the key in from the user or private nvram.
	 * The last byte is cleared by malloc, so string is
	 * NULL terminated. If the key was < 8 bytes, than
	 * the key is zero filled.
	 */
        nv_master_des_key_one = malloc(DES_KEY_LENGTH+1);
	if (nv_master_des_key_one) {
	    bcopy(key, nv_master_des_key_one, DES_KEY_LENGTH);
	} else {
	    printf("\n%% Configuration key configuration failed!");
	    flush();
	}
	break;
    case 2:
    case 3: 
	printf("\n%% Additional master keys not yet supported!");
	break;
    default:
        printf("\n%% Illegal master key specified.");
        break;
    }

}

static char *nv_get_master_key (int key_number)
{
    switch (key_number) {
	case 1: 
	    return(nv_master_des_key_one);
	case 2:
	case 3: 
	    printf("\n%% Additional master keys not supported yet!");
	    break;
	default:
	    printf("\n%% Illegal master key specified.");
	    break;
    }
    return (0);
}

/*
 * nv_private_getptr
 * Find the address of the private area based on an nvtype pointer.
 * The only pointer to be relied on is nv->textbase, since some platforms 
 * copy the configuration into malloc'ed memory and adjust only that pointer 
 * in the nvtype structure. The word boundary following the end of the
 * configuration is defined to be the start of the private area.
 * 
 * If the config is compressed, nv->textbase will already point to the
 * malloc'ed uncompressed config, in which case we have to look physically
 * past the nvtype struct into NVRAM.
 */
nvtype_private *nv_private_getptr (nvtype *nv)
{
    if ((nvptr_uncompressed != NULL) && (nv->version == NVCOMPRESSVERSION)) {
	if (nv->uncompress_size > 0) {
	    return (nvtype_private *) word_align(NV_V2_TEXTBASE(nv) + 
					             nv->uncompress_size);
	} else {
	    /*
	     * We never expect this condition, but need to catch it if it
	     * happens as a safeguard against overwriting the public config.
	     */
	    return (nvtype_private *)0;
	}
    } else {
        return (nvtype_private *) word_align(nv->textbase + nv->textsize);
    }
}

/*
 * nv_private_isconfig
 * Return the pointer of the private configuration if there is a
 * current valid private configuration. If not, return NULL.
 */
static nvtype_private *nv_private_isconfig (void)
{
    nvtype *nv;
    nvtype_private *private;

    if (nvptr) {
	nv = nvptr;
    } else {
    	nv = nv_getptr();
    }
    private = nv_private_getptr(nv);
    if (!private) {
	printf("%% Warning: private configuration not found!\n");
	return(0);
    }
    /* 
     * Verify that the magic number is correct.
     */
    if (private->magic == NVMAGIC_PRIV) {
	return(private);
    } else {
	return(0);
    }
}

/*
 * nv_save_private_config
 * Copies the private config in NVRAM into a holding area so that the public
 * configuration in NVRAM can be modified without affecting the private config.
 * Returns a status indicating if there was one saved or not.
 */
static nv_private_status nv_save_private_config (void)
{
    nvtype_private *private;
    char *priv_start, *where;
    nv_private_status status;

    status = NV_PRIVATE_NONE;
    private = nv_private_isconfig();
    if (private) {
	/*
	 * Allocate a private area buffer in memory, adjust the ptr to the 
	 * beginning of data, and bcopy everything but the '\nend\n'.
	 */
        where = nv_get_private_area(private->priv_textsize);
	if (where) {
	    priv_start = (char *)private + sizeof(nvtype_private);
	    nv_bcopy(priv_start, where, private->priv_textsize - ENDSTRLEN);
	    status = NV_PRIVATE_SAVED;
	} else {
	    printf("%% Private configuration can't be saved!\n");
	    status = NV_PRIVATE_ERROR;
	}
    }
    return(status);
}


static void nvram_subsys_init (subsystype *subsys)
{
    /* Initialize dirty bit that says whether config modified or not */
    nvram_config_dirty_init(); 
    nv_write_open_time = timer_no_time;
    nv_csb = NULL;
}

#define NVRAM_MAJVERSION 1
#define NVRAM_MINVERSION 0
#define NVRAM_EDITVERSION  1

SUBSYS_HEADER(nvram_common, NVRAM_MAJVERSION, NVRAM_MINVERSION, 
              NVRAM_EDITVERSION,
              nvram_subsys_init, SUBSYS_CLASS_DRIVER,
              NULL,
              NULL);
