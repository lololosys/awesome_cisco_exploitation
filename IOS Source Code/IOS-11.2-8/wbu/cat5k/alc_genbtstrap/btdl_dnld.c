/* $Id: btdl_dnld.c,v 1.1.4.1 1996/05/09 14:55:15 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_genbtstrap/btdl_dnld.c,v $
 *------------------------------------------------------------------
 * btdl_dnld.c -- Boot download glue code to support ALC BCS/combined
 *		  binary images.
 *
 * July 27, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: btdl_dnld.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:15  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:43  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:26  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:52  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:43  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:11  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.5  1995/10/21  04:27:18  vinod
 * Extracted checksum part of BDG_HasExploder into BDG_HaspExploderChecksumOnly,
 *
 * Revision 1.4  1995/10/13  06:06:47  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 * Revision 1.3  1995/08/31  12:13:04  rlowe
 * Changes to work with IOS 11.0 and higher.
 * Fix bug where exploder was allowing interrupts after burning flash;
 * but IOS system tick (4ms) interrupt will call flash (monitor) code,
 * thereby accessing invalid code and causing various exceptions.
 *
 * Revision 1.2  1995/08/03  02:03:58  rlowe
 * Add OCS tag/signature string checking, to validate download image
 * as being for ATM line card.  Clean up some buginf/printf messages.
 * Cleanup code that was #if'd out.
 *
 * Revision 1.1  1995/07/27  22:13:50  rlowe
 * Add ALC combined binary image support to BDG (Boot download glue).
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "lo_memblks.h"
#include "acp_hw_defs.h"
#include "acp_c_defs.h"
#include "acp_image_map.h"
#include "bdg_if.h"
#include "mc68ec030_defs.h"
#include "cbi_defs.h"
#include "flash.h"
#include "libcom.h"
#include "acbl.h"


extern void buginf () ;


/**/
/* --------------------------------------------------------------------	*/
/*	Determine the size of the downloaded image.			*/
/* --------------------------------------------------------------------	*/
u_int32 BDG_GetDownldSize (u_int32 Base)
{
	extern u_int32	image_len ;	/* From downloader/ipc module. */
	extern u_int8 *	image_buf ;	/* From downloader/ipc module. */

#ifdef	DEBUG
	buginf ("\r\nBDG_GetDownldSize:  len 0x%x, ptr 0x%x, base 0x%x\r\n",
#ifdef	BDG_FLASH_TEST_PATTERN
		ACP_HW_FLASH_SIZE, image_buf, Base) ;
#else
		image_len, image_buf, Base) ;
#endif
#endif

#ifdef	BDG_FLASH_TEST_PATTERN
	return (ACP_HW_FLASH_SIZE) ;
#else
	return (((u_int32) image_buf) - Base) ;
#endif

}	/* BDG_GetDownldSize */


u_int32 BDG_GetDownldSizeX ()
{
	return (BDG_GetDownldSize ((UINT32)(ARAM_DOWNLOAD_BUFR))) ;

}	/* BDG_GetDownldSizeX */

#ifdef	BDG_FLASH_TEST_PATTERN
/**/
/* --------------------------------------------------------------------	*/
/*	Create a test pattern in RAM for burning into flash.		*/
/* --------------------------------------------------------------------	*/
const u_int32 BDG_TestIData [] =
{
	0x00112233, 0x11223344, 0x22334455, 0x33445566,	/* entry 1 */
	0x44556677, 0x55667788, 0x66778899, 0x778899aa,
	0x8899aabb, 0x99aabbcc, 0xaabbccdd, 0xbbccddee,
	0xccddeeff, 0xddeeff00, 0xeeff0011, 0xff001122,

	0x00112233, 0x11223344, 0x22334455, 0x33445566,	/* entry 2 */
	0x44556677, 0x55667788, 0x66778899, 0x778899aa, /* for wraps */
	0x8899aabb, 0x99aabbcc, 0xaabbccdd, 0xbbccddee,
	0xccddeeff, 0xddeeff00, 0xeeff0011, 0xff001122,
} ;

s_int32 BDG_TestImage (register u_int32 * pRam, register u_int32 Len,
							u_int8 Ofst)
{
	register u_int32	BlkSz ;
	register u_int32	Count ;
	register u_int32 *	pTab ;
	u_int32			OldCache = 0 ;


                /* Enable I/D-caches.  Also flush caches in case   */
                /* they are not already enabled (by debug jumper). */
	PUNCH_WD ;
        OldCache = Sys_SetCpuCacheX (CPU030_ENAB_CACHES | CPU030_FLUSH_CACHES) ;

		/* Ensure Length is multiple of table entry size. */
		/* Assume that we can process a whole flash block */
		/* between watchdog punches.                      */
	PUNCH_WD ;
	Len   = ((Len + 63) & ~ 63L) ;
	BlkSz = (ACP_HW_FLASH_SEGSIZE) ;
	Ofst &= 0x0f ;
	pTab  = & BDG_TestIData [Ofst] ;

	PUNCH_WD ;
	while (Len > 0)
	{
			/* Look for (last) block using less than phys max. */
		if (Len < BlkSz)
			BlkSz = Len ;

		for (Count = BlkSz ; Count > 0 ; Count -= (16 * sizeof (*pRam)))
		{
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;
			*pRam++ = *pTab++ ;

			pTab -= 16 ;
		}
#ifdef	DEBUG
		PUNCH_WD ;
		buginf ("BDG_TestImage:  fill ptr $%x\n", pRam) ;
#endif

		PUNCH_WD ;
		Len -= BlkSz ;
	}

	PUNCH_WD ;
	(void) Sys_SetCpuCacheX (OldCache | CPU030_FLUSH_CACHES) ;

	return (TRUE) ;

}	/* BDG_TestImage */
#endif

/**/
/* --------------------------------------------------------------------*/
/*	Handle downloads that include exploders.  Validate download         */
/*	checksums and signature tag                                         */
/* --------------------------------------------------------------------*/
u_int8 BDG_HasExploderChecksumOnly ()
{
  s_int32 RC   = 0 ;
  u_int32 DlSz = 0 ;
  u_int32 Valu = 0 ;
  ocs_t * pH   = NULL ;
  long	(*pFcn) () ;


  do {
    /* How many bytes did the downloader catch ? */
    PUNCH_WD ;
    DlSz = BDG_GetDownldSize ((u_int32) (ARAM_DOWNLOAD_BUFR)) ;

    /* Checksum the OCS header in the download. */
    PUNCH_WD ;
    pH = (ocs_t *) (ARAM_DOWNLOAD_BUFR) ;
    Valu = checksum2 ((u_long *) pH,
		      ((sizeof(*pH) - sizeof(pH->ocs_cksum) + 3) >> 2),
		      ABL_CKSUM_SEED, (int (*)()) (buginf)) ;

    PUNCH_WD ;
    if (Valu != pH->ocs_cksum) {
      buginf ("\nBDG_HasExploderChecksumOnly:  hdr cksum error, got %#x expected %#x\n",
	      Valu, pH->ocs_cksum) ;
      RC = (-1) ;
      break ;
    }
    buginf ("\nBDG_HasExploderChecksumOnly:  hdr cksum passed, got %#x\n", Valu) ;
    
    /* Verify the signature tag string in the OCS header. */
    PUNCH_WD ;
    if (strncmp (ABL_MAGIC, pH->tag, sizeof (pH->tag) - 1))	{
      buginf ("\nBDG_HasExploderChecksumOnly:  invalid image signature %s\n",
	      pH->tag) ;
      RC = (-1) ;
      break ;
    }
    buginf ("\nBDG_HasExploderChecksumOnly:  good image signature %s\n", pH->tag) ;
    
    /* Checksum the download body. */
    PUNCH_WD ;
    Valu = checksum2 ((u_long *)(pH + 1),
		      ((pH->cbi_size - sizeof (*pH) + 3) >> 2),
		      ABL_CKSUM_SEED, (int (*)()) (buginf)) ;

    PUNCH_WD ;
    if (Valu != pH->cbi_cksum) {
      buginf ("\nBDG_HasExploderChecksumOnly:  body cksum error, got %#x expected %#x\n",
	      Valu, pH->cbi_cksum) ;
      RC = (-1) ;
      break ;
    }
    buginf ("\nBDG_HasExploderChecksumOnly:  body cksum passed, got %#x\n", Valu) ;

  } while (FALSE) ;

  return (RC != 0) ;			/* Return FALSE for success. */

}	/* BDG_HasExploderChecksumOnly */


/**/
/* --------------------------------------------------------------------	*/
/*	Handle downloads that include exploders. use embedded exploder  */
/*      to burn the combined binary image into flash.  We do not burn	*/
/*	flash from here when an exploder is present.			*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_HasExploder (void (*pCallBack)(void))
{
	s_int32	RC   = 0 ;
	u_int32	DlSz = 0 ;
	u_int32 Valu = 0 ;
	ocs_t *	pH   = NULL ;
	long	(*pFcn) () ;


    do {
		/* How many bytes did the downloader catch ? */
	PUNCH_WD ;
	DlSz = BDG_GetDownldSize ((u_int32) (ARAM_DOWNLOAD_BUFR)) ;

	PUNCH_WD ;
	pH = (ocs_t *) (ARAM_DOWNLOAD_BUFR) ;

		/* Copy embedded exploder to its execution buffer. */
	PUNCH_WD ;
	if ((pH->ExplBase < ARAM_EXPLODER_BUFR) ||
	    (pH->ExplBase + pH->ExplSize > ARAM_2ND_BOOT_DATA))
	{
		buginf ("\nBDG_HasExploder:  exploder posn error %#x len %#x\n",
			pH->ExplBase, pH->ExplSize) ;
		buginf ("BDG_HasExploder:  expected %#x len %#x\n",
			ARAM_EXPLODER_BUFR,
			ARAM_2ND_BOOT_DATA - ARAM_EXPLODER_BUFR) ;
		RC = (-1) ;
		break ;
	}
	buginf ("\nBDG_HasExploder:  copying exploder to %#x, len %#x\n",
		pH->ExplBase, pH->ExplSize) ;

	PUNCH_WD ;
	A_BlockFill ((u_int8 *)(ARAM_EXPLODER_BUFR),
		     ARAM_2ND_BOOT_DATA - ARAM_EXPLODER_BUFR,
		     0, AR_SvcWatchdog) ;

	PUNCH_WD ;
	A_BlockCopy (((u_int8 *) pH) + pH->ExplOfst,
			(u_int8 *)(pH->ExplBase),
			pH->ExplSize, AR_SvcWatchdog) ;

		/* Jump to the embedded exploder to burn flash. */
	PUNCH_WD ;
	if ((pH->ExplEnAddr < ARAM_EXPLODER_BUFR) ||
	    (pH->ExplEnAddr >= ARAM_2ND_BOOT_DATA))
	{
	buginf ("\nBDG_HasExploder:  exploder entry %#x bad range %#x len %#x\n",
			pH->ExplEnAddr,
			ARAM_EXPLODER_BUFR,
			ARAM_2ND_BOOT_DATA - ARAM_EXPLODER_BUFR) ;
		RC = (-1) ;
		break ;
	}
	buginf ("\nBDG_HasExploder:  jumping to exploder at %#x\n",
		pH->ExplEnAddr) ;

	PUNCH_WD ;
	pFcn = (long (*)())(pH->ExplEnAddr) ;
	Valu = (u_long) (*pFcn) (CBI_BOOT_IF_REV, pH, DlSz,
				 buginf, pCallBack) ;

	PUNCH_WD ;
	if (Valu)
	{
		buginf ("\nBDG_HasExploder:  exploder failed\n") ;
		RC = (-1) ;
		break ;
	}
	buginf ("\nBDG_HasExploder:  exploder succeeded\n") ;

    } while (FALSE) ;

    return (RC != 0) ;			/* Return FALSE for success. */

}	/* BDG_HasExploder */


/**/
/* --------------------------------------------------------------------	*/
/*	Complete download by verifying image and burning it to flash.	*/
/*	Actually, we setup the embedded exploder and let it burn to	*/
/*	flash.								*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_SetupExploder (u_int8 fReset, void (*pCallBack)(void))
{
	s_int32	RC   = 0 ;
	u_int32	DlSz = 0 ;
	u_int32	CCtl = 0 ;
	u_int32	CACU = 0 ;
	u_int32 Valu = 0 ;
	ocs_t *	pH   = NULL ;
	u_long	Old_SR ;
	long	(*pFcn) () ;


		/* Try to burn the downloaded image to flash.    */
		/* Map the flash area as uncacheable using ACU.  */
		/* Enable and flush all CPU caches.              */
		/* Buzz loop timing requires I/D-caches enabled. */
		/* Flush caches after changing ACU state.        */
		/* Mask interrupts to avoid IOS SW watchdog or   */
		/* kernel tick (4ms) interrupts that will access */
		/* IOS monitor (boot, flash-resident) code.      */
	PUNCH_WD ;
	Old_SR = spl7 () ;
	CACU = Sys_SetCpuACUx (ACU_FLASH_CTL, CBI_FLASH_ACU) ;
        CCtl = Sys_SetCpuCacheX (CPU030_ENAB_CACHES | CPU030_FLUSH_CACHES) ;


		/* Use embedded exploder in download. */
	PUNCH_WD ;
	RC = BDG_HasExploder (pCallBack) ;


		/* Flush caches after changing ACU state. */
	PUNCH_WD ;
	(void) Sys_SetCpuACUx (ACU_FLASH_CTL, CACU) ;
	(void) Sys_SetCpuCacheX (CCtl | CPU030_FLUSH_CACHES) ;


		/* Force reset if download okay.  Don't worry about intr mask.*/
	if ((! RC) && (fReset))
		A_ResetCPU () ;


		/* Unmask interrupts if we are returning. */
	PUNCH_WD ;
	(void) splx (Old_SR) ;
	return (! RC) ;			/* Return TRUE for success. */

}	/* BDG_SetupExploder */

/**/
/* --------------------------------------------------------------------	*/
/*	Run a downloaded image.						*/
/* --------------------------------------------------------------------	*/
void BDG_RunLoad ()
{
	void	(* pFcn) () ;


#ifdef	DEBUG
	PUNCH_WD ;
	buginf ("\r\nBDG_RunLoad: jumping to new image...\r\n") ;
#endif

	PUNCH_WD ;
	pFcn = (p_void) (ARAM_DOWNLOAD_BUFR) ;
	(* pFcn) (0, 0, 0, 0) ;

}	/* BDG_RunLoad */

