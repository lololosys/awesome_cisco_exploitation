/* $Id: flashdrvr.c,v 3.2.58.2 1996/06/01 02:46:22 clev Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-rsp/flashdrvr.c,v $
 *------------------------------------------------------------------
 * flashdrvr.c
 *
 * Mar 1995, Steve J. Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Platform specific flash routines.  Derived from DiagMon code for
 * XX.
 *------------------------------------------------------------------
 * $Log: flashdrvr.c,v $
 * Revision 3.2.58.2  1996/06/01  02:46:22  clev
 * CSCdi55487:  Add support for AMD 008 and 016 flash SIMMs
 * Branch: California_branch
 *
 * Fujitsu chips too.
 *
 * Revision 3.2.58.1  1996/03/21  23:32:13  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:05  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:46:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Platform specific flash routines.
*/

#include "monitor.h"
#include "mon_defs.h"
#include "flash.h"

#define FLASH_VPP_ON()       (REG_FPGA->sys_ctrl_reg &= ~FLASH_PROG_MASK)
#define FLASH_VPP_OFF()      (REG_FPGA->sys_ctrl_reg |= FLASH_PROG_MASK)

/*
 * Turn on the flash programming voltage VPP
 */
void
flash_prog_en(void)
{
    int i;
    extern long tenmsecs;

    FLASH_VPP_ON();
    flush_io_wb();
#ifdef DEBUG
    printf("\n***************VPP ON ***************\n");
#endif
    for(i=0; i<10; i++) wastetime(tenmsecs);
}

/*
 * Turn off flash programming
 */
void
flash_prog_dis(void)
{
    int i;
    extern long tenmsecs;

    FLASH_VPP_OFF();
    flush_io_wb();
#ifdef DEBUG
    printf("\n*************VPP OFF *************\n");
#endif
    for(i=0; i<10; i++) wastetime(tenmsecs);
}

/*
** Set the address of boot flash and return one or zero if it does
** not exist on this platform.
*/
int
bootflash_addr(long *addrptr, int *fcc_install)
{
    *fcc_install = 1;		/* For the time being, use RXBOOT flash */
    *addrptr = (long)KSEG1_ADRSPC_RXFLASH;
    return(1);
}

unsigned
get_flash_base(void)
{
    return(0xffffffff);
}
  
long
bflash_siz()
{
    int fcc_install;
    struct flash_dvr_info fdi[MAXFLASHSETS];

    if(!bootflash_addr(&fdi[0].flash_base, &fcc_install)) {
	printf("boot flash not available\n");
	return(0);
    }
    return(flash_siz(fdi));
}

/* end of module */
