/* $Id: image.ld,v 3.1.58.1 1996/05/09 13:54:44 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/obj-68-alc/image.ld,v $
 *------------------------------------------------------------------
 * Gnu-ld linker script for Synergy ATM Line Card (ALC) IOS bootstrap
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: image.ld,v $
 * Revision 3.1.58.1  1996/05/09  13:54:44  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:10  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:34:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.1  1996/03/22  09:07:07  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:06  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:03  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:04  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:07:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:45  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/10/03  07:04:46  rlowe
 * Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  19:14:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.3  1995/07/27  07:00:58  rlowe
 * Remove redundant line(s).
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.2  1995/05/17  00:27:16  rlowe
 * Support downloading to Rev 2 ATM line cards.  Move downloaded code base
 * from $40001000 to $40004000.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.1  1995/04/19  02:31:54  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:16:00  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

SECTIONS
{
  .text :
  {
    CREATE_OBJECT_SYMBOLS
    *(.text)
    . = ALIGN(4) ;
    _etext = .;
  }
  .data . : AT (SIZEOF(.text) + ADDR(.text))
  {
    _bdata = . ;	/* RKL */
    *(.data)
    CONSTRUCTORS
    . = ALIGN(4) ;
    _edata  =  .;
  }

  _ImageSize = SIZEOF(.text) + SIZEOF(.data) ;
  _EndImage  = ADDR(.text) + _ImageSize ;

  .bss SIZEOF(.data) + ADDR(.data) :
  {
    __bss_start = .;
   *(.bss)
   *(COMMON)
/*    . = ALIGN(4) ;		RKL */
   . = ALIGN(1024) ;
   _end = . ;
   __end = . ;
  }
}
