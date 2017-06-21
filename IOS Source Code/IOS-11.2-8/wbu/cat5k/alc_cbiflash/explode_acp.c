/* $Id: explode_acp.c,v 1.1.4.1 1996/05/09 14:54:18 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cbiflash/explode_acp.c,v $
 *------------------------------------------------------------------
 * explode_acp.c -- Self-contained exploder code for burning ACP
 *		    downloads (BCS combined image format) into flash.
 *		    (Adapted from RFA's NMP CBI tools.)
 *
 * July 24, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: explode_acp.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:18  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:43  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:23  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:44  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:43  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:14  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.5  1995/10/13  06:07:00  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 * Revision 1.4  1995/07/29  06:52:25  rlowe
 * Mask out interrupts while burning flash, to cover the kernel case.
 * Do not want interrupts, etc. interfering with critical flash timing
 * loops.
 *
 * Revision 1.3  1995/07/27  22:06:50  rlowe
 * Add ALC combined binary image support to BDG (Boot download glue).
 *
 * Revision 1.2  1995/07/27  10:35:56  rlowe
 * Include file "acbl.h" is now in include directory.
 *
 * Revision 1.1  1995/07/27  10:24:00  rlowe
 * Initial check-in; the ATM line card embedded exploder for downloaded
 * images.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "cbi_defs.h"
#include "acbl.h"
#include "libcom.h"
#include "flash.h"
#include "mc68ec030_defs.h"


#define FORWARD	/**/

FORWARD void	CBI_CopyRightToLeft () ;
FORWARD u_int32	CBI_VerifyFlash () ;

#ifndef	NULL
#define	NULL	((void *) 0)
#endif


/*
 * Main entry point for the FLASH image exploder (formerly parser).
 * Returns 0 on success.
 */
long CBI_Explode (long IF_rev, char * pBuf, u_long DnldSz,
			int (*pLogErr)(), void (*pCallBack)())
{
	register ocs_t *	ocsp	= (ocs_t *) pBuf ;
	u_long	index ;
	u_long	BlkSz ;
	u_long	BkNbr ;
	char *	magic ;
	u_int32 CCtl = 0 ;
	u_int32 CACU = 0 ;
	u_long	Old_SR ;
	long	RC   = (-1) ;		/* Assume failure for now. */


#ifdef DEBUG
	if (pLogErr != NULL)
	{
		(*pLogErr)("\nOCS\tbuf = %#x, len = %#x, siz = %#x, mods = %d\n",
				pBuf, DnldSz, ocsp->cbi_size, ocsp->cbi_mods) ;
		(*pLogErr)("\tichk = %#x, hchk = %#x, ifst = %#x, bfst = %#x\n",
				ocsp->cbi_cksum, ocsp->ocs_cksum, ocsp->ImageOfst, ocsp->BCS_Ofst) ;
		(*pLogErr)("\texpb = %#x, exps = %#x, expm = %#x, expo = %#x\n",
				ocsp->ExplBase, ocsp->ExplSize, ocsp->ExplEnAddr, ocsp->ExplOfst) ;
		(*pLogErr)("\tver = %d, tag = %s\n\n", ocsp->Version,ocsp->tag);
	}
#endif /* DEBUG */


	/*
	 *	Check for synergy tag first.
	 */
	for (magic = ABL_MAGIC, index = 0 ; magic [index] ; ++ index)
	{
		if (magic [index] != ocsp->tag [index])
		{
			if (pLogErr != NULL)
				(*pLogErr) ("ERROR: not C5kALC image\n") ;

			return (-1) ;
		}
	}


	/*
	 *	Write the flash here.  Do it in two chunks:  middle-end
	 *	and start-middle.  This is because the BCS headers go at
	 *	the start of flash, but are written at the end of the
	 *	download image/buffer.
	 *
	 *	Map the flash area as uncacheable using ACU.
	 *	Enable and flush all CPU caches.
	 *	Buzz loop timing requires I/D-caches enabled.
	 *	Flush caches after changing ACU state.
	 */
	if (pCallBack != NULL)
		(*pCallBack) () ;
	CACU = Sys_SetCpuACUx (ACU_FLASH_CTL, CBI_FLASH_ACU) ;
	CCtl = Sys_SetCpuCacheX (CPU030_ENAB_CACHES | CPU030_FLUSH_CACHES) ;

	BlkSz = ocsp->cbi_size - ocsp->BCS_Ofst ;
	BkNbr = (BlkSz + (FBLKSIZ) - 1) / (FBLKSIZ) ;
	index = (BkNbr * (FBLKSIZ)) - BlkSz + ocsp->ImageOfst ;

#ifdef	DEBUG
	if (pLogErr != NULL)
		(*pLogErr)("\nBCS Sz %#x, blks %d, frag %#x, imof %#x, indx %#x\n",
				BlkSz, BkNbr, index - ocsp->ImageOfst,
				ocsp->ImageOfst, index) ;
#endif	/* DEBUG */

	Old_SR = spl7 () ;
	do {
		if (pCallBack != NULL)
			(*pCallBack) () ;

		if (! load2 (BkNbr + CBI_FIRST_FLASH_BLK,
				(u_long *) & pBuf [index],
				ocsp->BCS_Ofst - index, pLogErr, pCallBack))
			break ;

		if (pCallBack != NULL)
			(*pCallBack) () ;

		if (pLogErr != NULL)
			(*pLogErr)("CBI_Explode: flash burn-1 succeeded\n") ;

		if (pCallBack != NULL)
			(*pCallBack) () ;

		if (! CBI_VerifyFlash (( (BkNbr + CBI_FIRST_FLASH_BLK) *
					 (FBLKSIZ) + (ACP_HW_FLASH_BASE)),
					(u_long *) & pBuf [index],
					ocsp->BCS_Ofst - index,
					pLogErr, pCallBack))
			break ;

		if (pCallBack != NULL)
			(*pCallBack) () ;

		if (pLogErr != NULL)
			(*pLogErr)("CBI_Explode: flash verify-1 passed\n") ;

		if (pCallBack != NULL)
			(*pCallBack) () ;

		CBI_CopyRightToLeft (& pBuf [ocsp->ImageOfst],
					& pBuf [ocsp->ImageOfst + BlkSz],
					index - ocsp->ImageOfst) ;

		if (pCallBack != NULL)
			(*pCallBack) () ;

		CBI_CopyRightToLeft (& pBuf [ocsp->BCS_Ofst],
					& pBuf [ocsp->ImageOfst], BlkSz) ;

		if (pCallBack != NULL)
			(*pCallBack) () ;

		if (! load2 ((CBI_FIRST_FLASH_BLK),
				(u_long *) & pBuf [ocsp->ImageOfst],
				BkNbr * (FBLKSIZ), pLogErr, pCallBack))
			break ;

		if (pCallBack != NULL)
			(*pCallBack) () ;

		if (pLogErr != NULL)
			(*pLogErr)("CBI_Explode: flash burn-2 succeeded\n") ;

		if (pCallBack != NULL)
			(*pCallBack) () ;

		if (! CBI_VerifyFlash ( ((CBI_FIRST_FLASH_BLK) * (FBLKSIZ) +
							(ACP_HW_FLASH_BASE)),
					(u_long *) & pBuf [ocsp->ImageOfst],
					BkNbr * (FBLKSIZ), pLogErr, pCallBack))
			break ;

		if (pCallBack != NULL)
			(*pCallBack) () ;

		if (pLogErr != NULL)
			(*pLogErr)("CBI_Explode: flash verify-2 passed\n") ;

		RC = 0 ;		/* Success ! */

	} while (FALSE) ;


		/* Flush caches after changing ACU state. */
	(void) splx (Old_SR) ;
	if (pCallBack != NULL)
		(*pCallBack) () ;
	(void) Sys_SetCpuACUx (ACU_FLASH_CTL, CACU) ;
	(void) Sys_SetCpuCacheX (CCtl | CPU030_FLUSH_CACHES) ;


	if (pCallBack != NULL)
		(*pCallBack) () ;

	return (RC) ;

}	/* CBI_Explode */


/*
 *	Copy long words from source to destination, using right-to-left
 *	policy within a block.  This is to help handle overlapping blocks.
 */
void CBI_CopyRightToLeft (u_long * pSrc, u_long * pDest, u_long ByteCount)
{
	register u_long *	pTo ;
	register u_long *	pFrom ;
	register u_long		LongCount ;


	LongCount = ((ByteCount + 3) >> 2) ;

	pTo   = pDest + LongCount ;
	pFrom = pSrc  + LongCount ;

	for (; LongCount > 0 ; -- LongCount)
		*--pTo = *--pFrom ;

}	/* CBI_CopyRightToLeft */

/**/
/* --------------------------------------------------------------------	*/
/*	Verify that DRAM contents match flash contents (after burn).	*/
/*	Returns TRUE on success, otherwise FALSE.			*/
/* --------------------------------------------------------------------	*/
u_int32 CBI_VerifyFlash (register u_int32 * pFlash, register u_int32 * pRam,
			 register u_int32 Len, int (*pLogErr)(),
			 register void (*pCallBack)())
{
	register u_int32	BlkSz ;
	register u_int32	Count ;


		/* Ensure Length is multiple of (long) word size. */
		/* Assume that we can process a whole flash block */
		/* between watchdog punches.                      */
	if (pCallBack)
		(*pCallBack) () ;

	Len   = ((Len + 3) & ~ 3L) ;
	BlkSz = (ACP_HW_FLASH_SEGSIZE) ;


	while (Len > 0)
	{
			/* Look for (last) block using less than phys max. */
		if (Len < BlkSz)
			BlkSz = Len ;

		for (Count = BlkSz ; Count > 0 ; Count -= sizeof (*pFlash))
			if (*pFlash++ != *pRam++)
			{
#ifdef	DEBUG
				if (pCallBack)
					(*pCallBack) () ;

				-- pFlash ;
				-- pRam ;

				if (pLogErr)
					(*pLogErr)
		("\r\nCBI_VerifyFlash:  mismatch $%x = %08x, $%x = %08x\r\n",
					pFlash, *pFlash, pRam, *pRam) ;
#endif

				if (pCallBack)
					(*pCallBack) () ;

				return (FALSE) ;
			}

		if (pCallBack)
			(*pCallBack) () ;

		Len -= BlkSz ;
	}
	
	return (TRUE) ;

}	/* CBI_VerifyFlash */

