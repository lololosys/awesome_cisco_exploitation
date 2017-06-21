/* $Id: uc.h,v 1.1.68.1 1996/04/19 14:57:45 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/uc.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: uc.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: uc.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:45  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:43  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:21  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/UC.H 10    6/01/95 4:58p Sduhost 
*
* DESCRIPTION	This file has the utility card macros and constants.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: UC.H 
* 
* *****************  Version 10  *****************
* User: Sduhost      Date: 6/01/95    Time: 4:58p
* Updated in $/SecureLAN/SDU/Include
* Watchdog I/O address is changed.
* 
* *****************  Version 9  *****************
* User: Sduhost      Date: 3/02/95    Time: 2:34p
* Updated in $/SecureLAN/SDU/Include
* Added WDOG Parameters.
* 
* *****************  Version 8  *****************
* User: Sduhost      Date: 2/27/95    Time: 6:18p
* Updated in $/SecureLAN/SDU/Include
* Change Memory Address to 0xD800
* 
* *****************  Version 7  *****************
* User: Croyle       Date: 2/15/95    Time: 9:30a
* Updated in $/SecureLAN/SDU/Include
* Added NVRAM_PAGES constant
* 
* *****************  Version 6  *****************
* User: Croyle       Date: 2/12/95    Time: 4:17p
* Updated in $/SecureLAN/SDU/Include
* Modified NOVRAM macro to NVRAM
* 
* *****************  Version 5  *****************
* User: Moe          Date: 2/02/95    Time: 1:24p
* Updated in $/SecureLAN/SDU/Include
* Added Front Panel port assignments.
* 
* *****************  Version 4  *****************
* User: Bald_eagle   Date: 2/01/95    Time: 3:17p
* Updated in $/SecureLAN/SDU/Include
* changed UC_PAGE and UC_MEMORY
* 
* *****************  Version 3  *****************
* User: Tchi         Date: 2/01/95    Time: 2:29p
* Updated in $/SecureLAN/SDU/Include
* changed from 0xD800 to 0xD000
* 
* *****************  Version 2  *****************
* User: Tchi         Date: 1/11/95    Time: 10:14a
* Updated in $/SecureLAN/SDU/Include
* utility card address has been changed to D800:0000h
* 
* *****************  Version 1  *****************
* User: Tchi         Date: 12/21/94   Time: 6:34p
* Created in $/SecureLAN/SDU/Include
* SDU startup code
*******************************************************************************/

#ifndef UC_H
#define UC_H

/*******************************************************************************
						   UTILITY CARD MACROS
*******************************************************************************/

/*----------------------------------------------------------------------------- */
/*          			Front Panel Port Assignments */
/*----------------------------------------------------------------------------- */

#define UC_SWITCH			0x0300			/* Front switch and keypad  port */
#define UC_LCD_CTRL		0x0302			/* Front Panel Select Control Port */
#define UC_LCD_DATA		0x0303			/* Front Panel Data Port */

#define UC_SEGMENT		0xD800			/* segment at which UC memory begins: 0xD800 */
#define UC_PAGE			0x0304			/* NonVolatile RAM Page selection */
#define UC_SIZE			0x800				/* size of a page - 2K */

#define UC_MEMORY			0x306				/* Memory Bank Selection I/O call */
#define EPROM					0				/* EPROM memory bank */
#define FLASH1					1				/* Flash memory bank #1 */
#define FLASH2					2				/* Flash memory bank #2 */
#define NVRAM					3				/* Non-volatile memory bank */
#define WDOG_DEVICE	  		0x30A           	/* watch dog device */

#define NVRAM_PAGES	128						/* number of nvram pages */

/*------------------------------ macros --------------------------------------- */

#define UC_PTR		MK_FP(UC_SEGMENT,0)		/*point to the 2K window */

#endif
/*----------------------------- END OF FILE ---------------------------------- */
