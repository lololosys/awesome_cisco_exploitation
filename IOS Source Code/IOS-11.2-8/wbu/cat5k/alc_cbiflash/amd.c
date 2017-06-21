/* $Id: amd.c,v 1.1.4.1 1996/05/09 14:54:17 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cbiflash/amd.c,v $
 *-----------------------------------------------------------------
 * amd.c
 *
 * May 1995, rfa
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: amd.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:17  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:41  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:21  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:43  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:41  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:12  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.10.8.1  1996/02/16  23:53:35  rlowe
 * Remove references to Catalyst 5000 from flash drivers.
 *
 * Revision 1.10  1995/10/04  01:07:48  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.9  1995/07/27  09:02:15  rlowe
 * Restructure flash driver so that existing APIs and functionality are
 * preserved, while allowing "printf" and "yield_cpu" to be passed in to
 * the flash driver as function pointers.  This allows the exploder
 * (which uses the flash driver) to be truly independent of the Boot/Kernel
 * context (the caller provides "printf" and "yield_cpu" instead of linking
 * them in directly).
 *
 * Revision 1.8  1995/06/09  08:58:38  rlowe
 * Complete port and integration of NMP (CBI) flash driver (AMD, Intel flash
 * simms) to ATM line card (ALC).
 *
 * Revision 1.7  1995/06/08  21:28:11  rfa
 * New update flash drivers
 *
 * Revision 1.6  1995/06/08  21:26:15  rfa
 * New amd driver
 *
 * Revision 1.4  1995/06/02  02:24:09  rlowe
 * Make CBI/flash (target) code portable between NMP and ACP.
 *
 * Revision 1.3  1995/05/30  21:40:09  sgarcia
 * Decided that rob needed help here
 *
 * Revision 1.2  1995/05/30  21:14:45  rfa
 * AMD driver now handles all AMD device types
 *
 * Revision 1.1  1995/05/24  00:15:40  rfa
 * New AMD flash driver and FLASH device interface
 *

 *------------------------------------------------------------------
 * $Endlog$
 */

#include "cbi_flsh_plat.h"
#include "flash.h"

#define FORWARD	/**/

FORWARD static u_char amdquery(fdev_t *, int (*)());
FORWARD static void amdreset(fdev_t *, int (*)());
FORWARD static int amderase(fdev_t *, volatile u_long *, int (*)(), void (*)());
FORWARD static int amdprogram(fdev_t *, volatile u_long *, u_long, int (*)());
FORWARD static void amdalert(fdev_t *, int (*)());
FORWARD static int amdclean(fdev_t *, int (*)(), void (*)());

#define AMDID		0x01

/*
 * This is the driver specific minor device structure support.
 */
typedef struct amd
{
	u_short		flags;
	char		*name;		/* Device name */
	int		nblks;		/* Number of blocks */
	long		csize;		/* Chip size in bytes */
	u_char		code;		/* Chip code */
} amd_t;

#define AMD_T(f,nam,x,z,c)	{ (f), #nam, (x), ##z##L, (c) }

amd_t amddevs[] =
{
	AMD_T(0, AMD28F010A, 0, 131072, 0xA2),
	AMD_T(0, AMD28F020A, 0, 262144, 0x29),
	AMD_T(0, AMD29F010, 8, 131072, 0x20),
	AMD_T(0, AMD29F040, 8, 524288, 0xA4),
	AMD_T(0, AMD28F256A, 0, 32768, 0x2F),
	AMD_T(0, AMD28F512A, 0, 65536, 0xAE),
	AMD_T(0, AMD28F010, 0, 131072, 0xA7),
	AMD_T(0, AMD28F020, 0, 262144, 0x2A),
	AMD_T(0, AMD29F100T, 5, 65536, 0x00),
	AMD_T(0, AMD29F100B, 5, 65536, 0x00),
	AMD_T(0, AMD29F200T, 7, 131072, 0x51),
	AMD_T(0, AMD29F200B, 7, 131072, 0x57),
	AMD_T(0, AMD29F400T, 11, 262144, 0x23),
	AMD_T(0, AMD29F400B, 11, 262144, 0xAB),
	AMD_T(F_8MB, AMD29F016, 32, 2097152, 0xAD),
	AMD_T(0, AMD28F256, 0, 32768, 0xA1),
	AMD_T(0, AMD28F512, 0, 65536, 0x25),
	AMD_T(F_4MB, AMD29F080, 16, 1048576, 0xD5),
	{ 0, (char *)0, 0, 0L, (u_char)0 }
};

/*
 * This is the major device structure declaration.
 */
fdev_t amdflash =
{
	FDEV_T(F_4MB, AMDID, 0, amd)
};

/*
 * BUS cycle unlocks and command read/write operations.
 */
#define BUSADR0		0
#define BUSADR1		(u_long)(0x5555 << 2)
#define BUSADR2		(u_long)(0x2AAA << 2)
#define BUSADR3		(u_long)(0x5555 << 2)
#define BUSADR4		(u_long)(0x5555 << 2)
#define BUSADR5		(u_long)(0x2AAA << 2)
#define BUSADR6		(u_long)(0x5555 << 2)

#define BUSDAT1		0xAAAAAAAA
#define BUSDAT2		0x55555555
#define BUSDAT3		0x80808080
#define BUSDAT4		0xAAAAAAAA
#define BUSDAT5		0x55555555

#define busdelay(x) do { register int ticks = (x); while (ticks--) ; } while (0)
#define bus(v,x)	*((u_long *)((((u_long)(v))&0xFFFF0000) | BUSADR##x##))
#define buswr(v,x)	bus(v,x) = BUSDAT##x
#define busrd(v,x)	bus(v,x)
#define adr(v)		*((u_long *)(v))

/*
 * AMD Commands.
 */
#define RESET		0xF0F0F0F0		/* Reset */
#define AUTOSEL		0x90909090		/* Auto select */
#define PROGRAM		0xA0A0A0A0		/* Program location */
#define CHIPERASE	0x10101010		/* Erase entire chip */
#define SECERASE	0x30303030		/* Erase individual sector */
#define SUSPERASE	0xB0B0B0B0		/* Erase suspend */
#define RESERASE	0x30303030		/* Erase resume */

/*
 * Status polling from AMD FLASH device.
 */
#define DQ7		0x80808080
#define DQ6		0x40404040
#define DQ5		0x20202020
#define DQ4		0x10101010
#define DQ3		0x08080808
#define DQ2		0x04040404
#define DQ1		0x02020202
#define DQ0		0x01010101

#define pgmread(v)	buswr(v,1), buswr(v,2), bus(v,3) = RESET, adr(a)
#define mfgid()		adr(FLASH_START)
#define devcode()	adr(FLASH_START | (1 << 2))
#define amdstatus(d,x)	(((x) >> ((d) * 8)) & 0xFF)
#define amdisdq5(d,x)	(amdstatus(d,x) & 0x20)
#define amdisdq7(d,x)	(amdstatus(d,x) & 0x80)
#define amdbusy(d,x,v)	(amdisdq7(d,x) != amdisdq7(d,v))

#define amddone(x,v,S,F,pPrt) \
do \
{ \
	register int dev; \
\
	for (dev = 0; dev < sizeof(long); ++dev) \
	{ \
		if (amdbusy(dev,x,v)) \
		{ \
			if (amdisdq5(dev,x)) \
			{ \
				if (pPrt) \
				{ \
				    (*pPrt)("amddone: ERROR in-loop status=0x%08x val=0x%08x\n", (x), (v)); \
				    (*pPrt)("ERROR: device %d %s\n", dev, (S)); \
				} \
				return(F); \
			} \
			break; \
		} \
	} \
	if (dev == sizeof(long)) \
		return(ENOERR); \
} while (0)

static u_char amdquery(devp, pPrt)
   fdev_t *devp;
   int (*pPrt)() ;
{
	u_long id;
	u_long code;
	amd_t *amdp;
	int dev;

	wrenable();
	buswr(FLASH_START,1);
	busdelay(CF_AMD_BUSWAIT);
	buswr(FLASH_START,2);
	busdelay(CF_AMD_BUSWAIT);
	bus(FLASH_START,3) = AUTOSEL;
	busdelay(CF_AMD_BUSWAIT);
	id = mfgid();

#define byte(x,n)	((u_char *)&(x))[(n)]
#define word(x,n)	((u_short *)&(x))[(n)]

	if (word(id, 0) != word(id, 1) || byte(id, 1) != byte(id, 2))
	{
		if (!(byte(id, 0) == devp->id || byte(id, 1) == devp->id ||
		    byte(id, 2) == devp->id || byte(id, 3) == devp->id))
		{
			/*
			 * There is a problem with the flash sim
			 * not being seated properly but it is not
			 * an AMD flash so wait until it hits the
			 * right driver.
			 */
			return((u_char)0);
		}
		for (dev = 0; dev < sizeof(long); ++dev)
		{
			if (byte(id, dev) != devp->id)
				if (pPrt)
				{
					(*pPrt)("ERROR: dev %d ", dev);
					(*pPrt)("is not electrically conntected\n");
				}
		}
		return((u_char)0);
	}

	busdelay(CF_AMD_BUSWAIT);
	code = devcode();
	busdelay(CF_AMD_BUSWAIT);
	if ((id & 0xFF) == devp->id)
	{
		for (amdp = amddevs; amdp->name; ++amdp)
		{
			if ((code & 0xFF) == amdp->code)
			{
				devp->pio = (void *)amdp;
				devp->flags |= amdp->flags;
				return((u_char)(id & 0xFF));
			}
		}
		if (pPrt)
			(*pPrt)("WARNING: Invalid AMD FLASH TYPE 0x%08x\n", code);
		return((u_char)0);
	}
#ifdef COMMENT
	if (pPrt)
		(*pPrt)("amdquery: id=0x%08x code=0x%08x\n", id, code);
#endif
	wrdisable();
	return((u_char)(id & 0xFF));
}

/*
 * This causes the AMD FLASH device to reset to read mode.
 */
static void amdreset(devp, pPrt)
   fdev_t *devp;
   int (*pPrt)() ;
{
	bus(FLASH_START,0) = RESET;
}

/*
 * This routine erases the contents of a flash sector (64kbyte per device).  
 * This is a straightforward implementation of the flash erase algorithm 
 * given in the 28F008 Flash documentation.  
 * Note : 64kbyte sector from each of the 4 flash devices will be erased
 * in parallel.
 */
static int amderase(devp, segment, pPrt, pYield)
   fdev_t *devp;
   volatile u_long *segment;
   int (*pPrt)() ;
   void (*pYield)() ;
{
	register int timer, clock;
	u_long status;

	buswr(segment,1); 
	busdelay(CF_AMD_BUSWAIT); 
	buswr(segment,2); 
	busdelay(CF_AMD_BUSWAIT); 
	buswr(segment,3); 
	busdelay(CF_AMD_BUSWAIT); 
	buswr(segment,4); 
	busdelay(CF_AMD_BUSWAIT); 
	buswr(segment,5); 
	busdelay(CF_AMD_BUSWAIT);
	adr(segment) = SECERASE;
	busdelay(CF_AMD_BUSWAIT);

	for (;;)
	{
		if (pYield)
			(*pYield)();
		/*
		 * Write Read status Register Command.
		 */
		status = adr(segment);
		amddone(status, DQ7, "sector erase failed", EERASE, pPrt);
	}
	/*
	 * Should NEVER get here!!!
	 */
	return(EERASE);
}

/*
 * This routine programs a long int at a particular address; it is a simple
 * implentation of the programming algorithm given in the Intel 28F008 
 * Flash documentation. Note one byte is programmed into each of the 4
 * Flash devices.
 */
static int amdprogram(devp, addr, val, pPrt)
   fdev_t *devp;
   volatile u_long *addr;
   u_long val;
   int (*pPrt)() ;
{
	u_long status;
	int timer;

	buswr(addr,1); 
	busdelay(CF_AMD_BUSWAIT); 
	buswr(addr,2); 
	busdelay(CF_AMD_BUSWAIT); 
	bus(addr,3) = PROGRAM; 
	busdelay(CF_AMD_BUSWAIT); 
	adr(addr) = val; 
	busdelay(CF_AMD_BUSWAIT); 

	for (;;)
	{
		u_long status;

		/*
		 * Write Read status Register Command.
		 */
		status = adr(addr); 
		amddone(status, val, "programming failed", EPROG, pPrt);
	}
	/*
	 * Should NEVER get here!!!
	 */
	return(ENOERR);
}

static void amdalert(devp, pPrt)
   fdev_t *devp;
   int (*pPrt)() ;
{
	if (pPrt)
	{
		(*pPrt)("FLASH:\n\n");
		(*pPrt)("Type		Address		    Location\n");
		if (devp->id == AMDID) 
		{
		    (*pPrt)("AMD 29F080    %08x            NMP (P3) %s SIM\n\n",
			    FLASH_START, devp->flags & F_8MB ? "8MB" : "4MB");
		}
	}
}

static int amdclean(devp, pPrt, pYield)
   fdev_t *devp;
   int (*pPrt)() ;
   void (*pYield)() ;
{
	u_long status;

	buswr(FLASH_START,1); 
	busdelay(CF_AMD_BUSWAIT); 
	buswr(FLASH_START,2); 
	busdelay(CF_AMD_BUSWAIT); 
	buswr(FLASH_START,3); 
	busdelay(CF_AMD_BUSWAIT); 
	buswr(FLASH_START,4); 
	busdelay(CF_AMD_BUSWAIT); 
	buswr(FLASH_START,5);
	busdelay(CF_AMD_BUSWAIT); 
	bus(FLASH_START,6) = CHIPERASE; 
	busdelay(CF_AMD_BUSWAIT); 

	for (;;)
	{
		if (pYield)
			(*pYield)() ;
		status = adr(FLASH_START);
		amddone(status, DQ7, "chip erase failed", EERASE, pPrt);
	}
	/*
	 * Should NEVER get here!!!
	 */
	return(EERASE);
}
