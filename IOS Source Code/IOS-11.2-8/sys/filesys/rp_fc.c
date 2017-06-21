/* $Id: rp_fc.c,v 3.2.60.2 1996/08/07 08:58:27 snyder Exp $
 * $Source: /release/112/cvs/Xsys/filesys/rp_fc.c,v $
 *------------------------------------------------------------------
 * RP specific PCMCIA flash card support functions
 *
 * April 1995, Szewei Ju
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rp_fc.c,v $
 * Revision 3.2.60.2  1996/08/07  08:58:27  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:36:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:36:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:08:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "../../boot/src-68-c7000/pcmap.h"
#include "../dev/flash_dvr_spec.h"
#include "filesys.h"
#include "fslib_internals.h"
#include "fslib_dev_intel_series_2plus.h"
#include "rp_fc.h"

static char *const rp_slot0_status_msg[] = { "OK",
         "Flash card is half plugged in slot0",
         "Flash card is half plugged in slot0",
         "No flash card in slot0 or it's not plugged in properly",
};

/*-----------------------------------------------------------
 * Get the manufacture ID and device ID of the card
 */
static ushort rp_get_flash_card_id( ulong dev_addr)
{
    ushort id;
    volatile ushort *lp;

    lp = (volatile ushort *)dev_addr;
    *lp = IFLASH_CMD_INTELLIGENT_ID | (IFLASH_CMD_INTELLIGENT_ID << 8);
    WASTETIME(fg.fg_usec6);
    id = (*lp & 0xff) << 8;
    id += *(lp+1) & 0xff;
    *lp = IFLASH_CMD_READ_ARRAY | (IFLASH_CMD_READ_ARRAY << 8);
    WASTETIME(fg.fg_usec10); 
    return( id);
}

/*-----------------------------------------------------------
 * Figure out the size of the flash card
 */
static ulong rp_get_flash_card_size( void)
{
   ulong da;

   for ( da = ADRSPC_EXT_FLASH; \
         da < (ADRSPC_EXT_FLASH+RP_FLASH_CARD_MAX_SIZE); \
         da += INTEL_2P_FLASH_CARD_BANK_SIZE) {
       if ( rp_get_flash_card_id( da) != INTEL_2P_FLASH_CARD_ID)
           break;
   }
   return( da - ADRSPC_EXT_FLASH);
}

/*-----------------------------------------------------------
 * Analyze the flash card on RP
 */
int analyze_flash_device (format_dev_parm_t *fpp)
{
    ushort fc_id;
    uchar  fc_status;

    /*
     *  Check if card present
     *  Check if card is write protected by its external switch
     *  Check card ID if we support the card type
     *  Check the revision code of the card
     *       To be able to erase locked block, we need revision 9
     *  Check card size 
     *  Check if we can turn on Vpp (Vpp jumper is on)
     */
    if ( (fc_status = FLASH_CARD_NOT_PRESENT)) {
        printf("\n%s\n", rp_slot0_status_msg[fc_status]);
        return -1;
    }
    if ( FLASH_CARD_WRITE_PROT) {
        printf("\nThe flash card in slot0 is write protected.\n");
        return -1;
    }
    /*
     * Currently, we only support Intel series 2 plus card
     */
    if ( (fc_id=rp_get_flash_card_id( ADRSPC_EXT_FLASH)) != INTEL_2P_FLASH_CARD_ID) {
        printf("\nInvalid flash card, Manu ID = 0x%2x, device ID = 0x%2x\n",
                fc_id >>8, fc_id & 0xff);
        return -1;
    }

    fpp->device_size = rp_get_flash_card_size();

    fpp->alg_num = FSLIB_PROG_INTEL_SERIES_2PLUS_CARD;
    fpp->sector_size = INTEL_2P_FLASH_CARD_SECTOR_SIZE;
    fpp->erase_state = -1;
    strcpy(&fpp->flash_name[0], FLASH_CARD0_NAME_STR);

    return( 0);
}

/*
 *  Copy bundled monlib to a buffer.
 *  If ok, return pointer to buffer and size.
 *  Otherwise, return NULL.
 */
char *copy_bndl_monlib_to_buffer (int *size)
{
    char *bufp;

    *size = ((int)&bndl_monlib_end) - ((int)&bndl_monlib_start);
    bufp = malloc( *size);
    if (bufp == NULL) {
        return NULL;
    }
    memcpy(bufp, &bndl_monlib_start, *size);
    return bufp;
}

