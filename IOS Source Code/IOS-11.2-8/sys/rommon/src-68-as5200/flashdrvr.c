/* $Id: flashdrvr.c,v 1.1.68.1 1996/06/16 21:18:49 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/flashdrvr.c,v $
 *------------------------------------------------------------------
 * flashdrvr.c -- Platform specific flash routines.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flashdrvr.c,v $
 * Revision 1.1.68.1  1996/06/16  21:18:49  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:16:28  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Platform specific flash routines.
*/

#include "mon_defs.h"
#include "pcmap.h"
#include "as5200_pcmap.h"
#include "flash.h"
#include "mon_plat_defs.h"

/*
 * Define a pointer to the motherboard control register for VPP control
 */
#define FLASH_CTRL_REG          ((unsigned short *)ADRSPC_BRASIL_MB_CTRL2_REG)
#define FLASH_VPP_ON()       (*FLASH_CTRL_REG = CONTROL_FLASH_VPP)
#define FLASH_VPP_OFF()      (*FLASH_CTRL_REG = BRASIL_FLASH_VPP_MODE)

/*
 * Global
 */
unsigned short save_cacr_state;

/*
 * Externs
 */
extern long total_flash;

/*
** Turn on the flash programming voltage VPP
*/
flash_prog_en()
{
    int i;
    extern long tenmsecs;

    save_cacr_state = setcacr(0); /* Disable cache */
    setcacr(CLEAR_CACHES); /* Clear the cache */
    FLASH_VPP_ON();
#ifdef DEBUG
    printf("\n***************VPP ON ***************\n");
#endif
    for(i=0; i<10; i++) wastetime(tenmsecs);
}

/*
** Turn off flash programming
*/
flash_prog_dis()
{
    int i;
    extern long tenmsecs;

    FLASH_VPP_OFF();
    setcacr(CLEAR_CACHES); /* Clear the cache */
    setcacr(save_cacr_state); /* Restore cacr contents */
#ifdef DEBUG
    printf("\n*************VPP OFF *************\n");
#endif
    for(i=0; i<10; i++) wastetime(tenmsecs);
}

unsigned int
get_flash_base(void)
{
    return(ADRSPC_BRASIL_FLASH_BASE);
}

/*
** Set the address of boot flash 
*/
int
bootflash_addr(long *addrptr, int *fcc_install)
{
    *addrptr = (long)ADRSPC_BOOT_FLASH_BASE;
    return(1);
}

/*
 * convenient routines for getting the size of bflash
 */
long
bflash_siz()
{
    struct flash_dvr_info fdi[MAXFLASHSETS];
 
    bootflash_addr(&fdi[0].flash_base, 0);
    if(!get_flash_info(fdi)) {
        printf("cannot read bootflash info\n");
        return(0);
    }
    return(total_flash);
}

long
getflashsiz(void)
{
    struct flash_dvr_info fdi[MAXFLASHSETS];
 
    fdi[0].flash_base = ADRSPC_BRASIL_FLASH_BASE;  /* addr of sys flash start */
    return(flash_siz(fdi));
}


/* end of module */
