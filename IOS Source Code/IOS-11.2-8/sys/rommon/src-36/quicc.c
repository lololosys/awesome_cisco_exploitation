/* $Id: quicc.c,v 3.2.58.1 1996/03/21 23:28:30 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36/quicc.c,v $
 *------------------------------------------------------------------
 * Nifty routines for the QUICC chip.
 *
 * November 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: quicc.c,v $
 * Revision 3.2.58.1  1996/03/21  23:28:30  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:39  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:43:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "quicc.h"
#include "bitdef.h"

void
quicc_cr_cmd(int chan_num, ushort cmd)
{
    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;
    int wd = 0;

    while (qir_p->cp_cr & QUICC_CPCR_SEMA) {
	if (wd++ > 10000)
	    /*
	     * timeout
	     */
	    puts("\n*** ERROR: timeout on QUICC CR command ***\n");
	    return;
    }
    qir_p->cp_cr = (QUICC_CPCR_OPCODE( cmd ) |
	QUICC_CPCR_CHNUM ( chan_num ) | QUICC_CPCR_SEMA);
}

struct bitdef rsr_reg_desc[] = {
    1, "(EXT) hard reset ",
    1, "(POW) power-up ",
    1, "(SWD) watchdog reset ",
    1, "(DBF) double bus fault ",
    0, "",
    1, "(LOC) loss of clock ",
    1, "(SRST) software reset ",
    1, "(SRSTP) soft reset ",
};

void
pr_reset_cause(unsigned char rsr)
{
    printf("Reset Cause: %#.8b\n", rsr_reg_desc, rsr);
}

/* end of module */
