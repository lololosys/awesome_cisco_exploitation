/* $Id: flash.h,v 1.1.4.1 1996/05/09 15:01:45 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/flash.h,v $
 *------------------------------------------------------------------
 * flash.h:  Synergy flash driver definitions
 *
 *		It is assumed that there are 4 flash devices (32 bit).
 *		All drivers will erase or write to all 4 devices in
 *		parallel. This will speed up download.
 *
 * March 1995, 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:45  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:40  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:37  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:51  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:34  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:48  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.7  1995/10/04  01:07:25  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.6  1995/07/27  09:11:40  rlowe
 * Restructure flash driver so that existing APIs and functionality are
 * preserved, while allowing "printf" and "yield_cpu" to be passed in to
 * the flash driver as function pointers.  This allows the exploder
 * (which uses the flash driver) to be truly independent of the Boot/Kernel
 * context (the caller provides "printf" and "yield_cpu" instead of linking
 * them in directly, or using HW-specific macros).
 *
 * Revision 1.5  1995/06/09  08:52:38  rlowe
 * Complete port and integration of NMP (CBI) flash driver (AMD, Intel flash
 * simms) to ATM line card (ALC).
 *
 * Revision 1.4  1995/06/08  21:28:57  rfa
 * New FDDI ILC download messages; new flash header
 *
 * Revision 1.2  1995/06/02  02:23:59  rlowe
 * Make CBI/flash (target) code portable between NMP and ACP.
 *
 * Revision 1.1  1995/05/30  20:52:49  rfa
 * Moved from syn_core/nmp/nmpflash/h
 *
 * Revision 1.6  1995/05/30  20:51:41  rfa
 * Added device functionality to the flash device structure
 *
 * Revision 1.5  1995/05/24  00:14:54  rfa
 * Moved device specified constants to each of the flash drivers in
 * syn_common/syndnld
 *
 * Revision 1.4  1995/04/17  17:00:59  rfa
 * New command support
 *
 * Revision 1.3  1995/03/31  22:27:20  rfa
 * release
 *
 * Revision 1.2  1995/03/24  01:10:41  rfa
 * new timing delays for flash programming from non boot NMP code
 *
 * Revision 1.1  1995/03/09  18:46:47  rfa
 * These header files support the new Combined Binary Linked downloader.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#define F_4MB		0x0000
#define F_8MB		0x0001

typedef struct fdev
{
	u_short		flags;
	u_char		id;			/* Manufacturer ID code */
	u_char		align;			/* Spare field for alignment. */
	void		*pio;			/* Physical I/O pointer */
	u_char		(*query)(struct fdev *, int (*)());
	void		(*reset)(struct fdev *, int (*)());
	int		(*erase)(struct fdev *, volatile u_long *, int (*)(), void (*)());
	int		(*program)(struct fdev *, volatile u_long *, u_long, int (*)());
	void		(*alert)(struct fdev *, int (*)());
	int		(*suspend)(struct fdev *, int (*)());
	int		(*resume)(struct fdev *, int (*)());
	int		(*chip)(struct fdev *, int (*)());
} fdev_t;

#define FDEV_T(f,id,p,name) \
	(u_short)(f), (u_char)(id), (u_char)0, (void *)(p), \
	name##query, name##reset, name##erase, name##program, name##alert, \
	(int (*)(fdev_t *, int (*)()))0, (int (*)(fdev_t *, int (*)()))0, \
	(int (*)(fdev_t *, int (*)()))0

#define	FBLKSIZ		0x040000		/* 64kbyte * 4 = 256kbyte */

/*
 * Error codes.
 */
#define	ENOERR			0
#define	EPROG			1	/* Flash program failed */
#define	EERASE			2	/* Flash erase failed */
#define ESUSP			3	/* Erase suspended */

#ifndef	usec10
static int usecv ;

#ifdef BOOT
#define usec10() do { ++usecv; ++usecv; } while (0)
#else /* BOOT */
#define usec10() do { ++usecv; ++usecv; ++usecv; ++usecv; } while (0)
#endif /* BOOT */
#endif	/* usec10 */

#ifndef	msec
#ifdef BOOT
#define msec(x) \
	do { register int clock; \
		for (clock = (x) * 125; clock; --clock); } while (0)
#else /* BOOT */
#define msec(x) \
	do { register int clock; \
		for (clock = (x) * (125 * 2); clock; --clock); } while (0)
#endif /* BOOT */
#endif	/* msec */

#ifndef	CF_AMD_BUSWAIT
#define	CF_AMD_BUSWAIT	(5)	/* Buzz delay when accessing AMD flashes. */
#endif

/*
 * Control Register 2 macros.
 */
extern uInt16 cntl2_shadow;

/*
 * Don't starve the (NMP) cpu, watchdog.
 */
extern void yield_cpu(void);

/*
 * Allow macro definition overrides by platform dependent header files.
 * These definitions apply to the NMP.
 */

#ifndef	wrenable
#define wrenable()	setbcntl2(FLASH_WR_ENB)
#endif
#ifndef	wrdisable
#define wrdisable()	clrbcntl2(FLASH_WR_ENB)
#endif
#define setbcntl2(x) \
	do { cntl2_shadow |= (x); wrcntl2(cntl2_shadow); } while (0)
#define clrbcntl2(x) \
	do { cntl2_shadow &= ~(x); wrcntl2(cntl2_shadow); } while (0)
#define wrcntl2(x) *((uInt16 *)CNTL2_REG) = (x)

#ifndef	CBIF_RecordPgmFailure
#define	CBIF_RecordPgmFailure()	\
	do {	\
		++ (((struct logNVRAM *)(LOGNVRAM_START)) -> \
						nmp_flash_pgm_failures) ; \
	} while (0)
#endif

extern int load(int, u_long *, int);
extern int load2(int, u_long *, int, int (*)(), void (*)());
extern int erase(int, int) ;
extern int erase2(int, int, int (*)(), void (*)()) ;
extern int record(int, u_long *, int) ;
extern int record2(int, u_long *, int, int (*)(), void (*)()) ;

