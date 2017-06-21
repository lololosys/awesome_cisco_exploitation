/* $Id: synalc_nv.c,v 3.1.64.2 1996/05/21 10:03:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/synalc_nv.c,v $
 *------------------------------------------------------------------
 * synalc_nv.c -- Routines for Synergy ATM line card NVRAM manipulation
 *			(based on c3000_nv.c, xx_nv.c)
 *
 * August 31, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: synalc_nv.c,v $
 * Revision 3.1.64.2  1996/05/21  10:03:46  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.64.1  1996/05/09  14:41:14  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.92.2  1996/04/29  22:21:50  sakumar
 * Changes to Cat5K ATM HW specific code for IOS to link after latest sync.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.92.1  1996/04/27  07:25:28  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.82.1  1996/04/08  02:09:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.70.1  1996/03/22  09:43:47  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.54.1  1996/03/03  21:24:59  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.52.1  1996/03/02  01:21:14  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.36.1  1996/02/29  07:50:20  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  13:24:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.32.1  1996/02/03  07:08:37  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.2.1  1995/08/31  14:18:41  rlowe
 * Carryover useful code from old Synergy_ATMlc_branch (10.3) to new
 * Synalc2_branch (11.0).  Provide platform-dependent code for IOS
 * re-port to Synergy ATM line card.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/08/31  14:12:50  rlowe
 * Placeholders for Synalc2_branch.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "logger.h"
#include "config.h"
#include "../dev/monitor1.h"
#include "../les/if_les.h"
#include "../os/nv.h"
#include "../../boot/src-68-alc/confreg.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#define ALTERNATE       NONE
#include "../parser/cfg_configreg.h"
LINK_POINT(configreg_command, ALTERNATE);
#undef  ALTERNATE


/*
 * get_sys_nvsize: Get the NVRAM size usable by the system image
 */
long
get_sys_nvsize(void)
{
    return((long)(mon_get_nvram_size() - (ADRSPC_NVRAM_START - ADRSPC_NVRAM)));
}

/*
 * get_nvbase: Get the pointer to the base of NVRAM usable by the system image
 */
uchar *
get_nvbase(void)
{
    return((uchar *) NVBASE);
}

/*
 * get_nvmagic: Get the pointer to the NVRAM magic number
 */
ushort *
get_nvmagic(void)
{
    return((ushort *) ADRSPC_NVRAM_MAGIC);
}

/*
 * nv_getptr
 * Return pointer to base of non-volatile memory
 */

nvtype *nv_getptr (void)
{
    nvtype *p;

    if (nvptr) {  /* Check lock */
	printf("\nNon-Volatile memory is in use");
	return(NULL);
    }
    p = (nvtype *)get_nvbase();
    if (nv_check_getptr(p)) {
	return(p);
    } else {
	return(NULL);
    }
}


/*
 * nv_readconfig
 * read NVRAM configuration register
 */
ushort nv_readconfig (void)
{
    ushort number;

    number = *(ushort *)config_ptr;

    return (number);
}


/*
 * nv_setconfig
 * write a number to the NVRAM configuration register:
 */
void nv_setconfig (
    ushort number)
{
    if (mon_set_confreg(number) == -1) {
	/*
	 * Set the conf reg here if ROM monitor is old
	 */
	*(ushort *)config_ptr = number;
 	*config_magic_ptr = CONFREG_MAGIC;
    }
}

/*
 * nv_badptr
 * Given a correctly checksummed portion of non-volatile memory, examine
 * the data structure components more closely for sanity.
 * If the configuration magic number is set, return zero.
 */
boolean nv_badptr (nvtype *ptr)
{
    ushort *nv_magic_ptr = get_nvmagic();

    if (*nv_magic_ptr == CONFIG_MAGIC)
	return(FALSE);
    else
	return(TRUE);
}

/*
 * nv_done
 * Handles writing configuration data to memory.
 */

void nv_done (nvtype *ptr)
{
    ushort *nv_magic_ptr = (ushort *)get_nvmagic();
  
    *nv_magic_ptr = CONFIG_MAGIC;
    nv_check_done(ptr);
    if (ptr == nvptr)		/* Free lock */
	nvptr = NULL;
    nv_writedisable(ptr);
}

void nv_writeenable (nvtype *ptr)
{
    nv_writeflag = TRUE;
}

void nv_writedisable (nvtype *ptr)
{
    nv_writeflag = FALSE;
}

/*
 * nv_init
 * Discover the size of the NVRAM
 */

void nv_init (void)
{
    nvptr = NULL;               /* make sure unlocked */

    nvsize = get_sys_nvsize();
    reg_add_config_register_read(0, nv_readconfig, "nv_readconfig");

    parser_add_commands(PARSE_ADD_CFG_TOP_CMD, &pname(configreg_command),
                        "configreg");
}

