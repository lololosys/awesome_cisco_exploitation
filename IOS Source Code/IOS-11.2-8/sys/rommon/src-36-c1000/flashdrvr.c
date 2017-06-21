/* $Id: flashdrvr.c,v 3.2.10.1 1996/03/21 23:29:02 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/flashdrvr.c,v $
 *------------------------------------------------------------------
 * Flash specific driver routines 
 *
 * Sept 1995, Eric Fronberg
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flashdrvr.c,v $
 * Revision 3.2.10.1  1996/03/21  23:29:02  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.2  1996/03/19  01:01:13  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.4.1  1995/12/01  04:21:58  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1996/02/23  17:15:51  cshen
 * CSCdi48454:  Need to support on-board flash on c1005 derivative
 *
 * Revision 3.1  1995/11/09  13:07:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/22  04:36:18  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.1  1995/09/22  00:50:55  enf
 * Placeholders for frankenstorm/umi
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Platform specific flash routines.
*/

#define __mips

#include "monitor.h"
#include "mon_defs.h"
#include "flash.h"
#include "quicc.h"
#include "nvmonvars.h"
#include "init.h"

/*
** Turn on the flash programming voltage VPP
*/
flash_prog_en()
{
    int i, hardware_type;
    extern long tenmsecs;

    hardware_type = get_1000_hardware_type((struct cookie *)
                                             &(NVRAM_SAV)->nvcookie.cookie);
    switch(hardware_type) {
      case SAPPHIRE_HARDWARE: /* UMI */
        ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->pio_pcdat &=
            ~QU_PT_C_FIXED_VPP_CTRL_UMI;
        break;
      case CATWOMAN_HARDWARE: /* HANA */
        ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->pio_pcdat |=
            QU_PT_C_FIXED_VPP_CTRL_HANA;
        break;
      default:
        break;
      }

#ifdef DEBUG
    printf("\n***************VPP ON ***************\n");
#endif
    for(i=0; i<10; i++) 
        sleep(10); /* 100 msec delay for voltage to ramp up */ 
}

/*
** Turn off flash programming
*/
flash_prog_dis()
{
    int i, hardware_type;
    extern long tenmsecs;

    hardware_type = get_1000_hardware_type((struct cookie *)
                                             &(NVRAM_SAV)->nvcookie.cookie);
    switch(hardware_type) {
      case SAPPHIRE_HARDWARE: /* UMI */
        ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->pio_pcdat |=
            QU_PT_C_FIXED_VPP_CTRL_UMI;
        break;
      case CATWOMAN_HARDWARE: /* HANA */
        ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)->pio_pcdat &=
            ~QU_PT_C_FIXED_VPP_CTRL_HANA;
        break;
      default:
        break;
      }

#ifdef DEBUG
    printf("\n*************VPP OFF *************\n");
#endif
    for(i=0; i<10; i++)  
        sleep(10); /*  100 msec delay for voltage to ramp down */ 
}

/*
** Return the bus width of given flash.
*/
get_flash_width(volatile unsigned long *addr)
{
    unsigned long temp, temp1;
    extern long sixusecs;

    *addr = F_RESET | (F_RESET << 8) | (F_RESET << 16) | (F_RESET << 24);
    wastetime(sixusecs);
    *addr = F_RESET | (F_RESET << 8) | (F_RESET << 16) | (F_RESET << 24);
    wastetime(sixusecs);
    *addr = F_READ_ID_CODES | (F_READ_ID_CODES << 8) | (F_READ_ID_CODES << 16) |
	(F_READ_ID_CODES << 24);
    wastetime(sixusecs);
    temp = *addr;
    temp1 = *(addr+1);
    *addr = F_RESET | (F_RESET << 8) | (F_RESET << 16) | (F_RESET << 24);
#ifdef DEBUG
    printf("\nget_flash_width 32: read %#x & %#x\n",temp, temp1);
#endif
    if((temp >> 16) == (temp & 0xffff)) return(BW_32BITS);  /* 32 bit flash bus (4 byte) */
#ifdef DEBUG
    printf("\nget_flash_width 16: read %#x & %#x\n",temp, temp1);
#endif

    if((temp >> 24) == ((temp >> 16) & 0xff)) return(BW_16BITS);  /* 16 bit bus */
#ifdef DEBUG
    printf("\nget_flash_width 8: read %#x & %#x\n",temp, temp1);
#endif

    return(BW_8BITS);  /* eight bit bus */
}

/*
** Set the address of boot flash and return one or zero if it does
** not exist on this platform.
*/
int
bootflash_addr(long *addrptr, int *fcc_install)
{
    *fcc_install = 0;           /* No credit card */
    *addrptr = (long)0xffffffff; /* No boot flash return that value */
    return(1);
}

int wastetime(int duration)
{
    volatile int timepass;
    int i;

    timepass = 0;
    for(i=0; i < duration; i++) {
        timepass = timepass + 1;
    }
}
