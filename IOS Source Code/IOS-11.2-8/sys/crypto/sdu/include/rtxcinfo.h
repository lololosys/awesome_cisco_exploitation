/* $Id: rtxcinfo.h,v 1.1.68.1 1996/04/19 14:57:44 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/rtxcinfo.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: rtxcinfo.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rtxcinfo.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:44  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:42  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:20  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/RTXCINFO.H 24    8/18/95 11:01a Peacock 
*
* DESCRIPTION:  RTXC related information header
*
* NOTES:
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: RTXCINFO.H 
* 
* *****************  Version 24  *****************
* User: Peacock      Date: 8/18/95    Time: 11:01a
* Updated in $/SecureLAN/SDU/Include
* Adjusted task stacks to good conservative value.
* 
* *****************  Version 23  *****************
* User: Peacock      Date: 8/18/95    Time: 10:56a
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 22  *****************
* User: Tchi         Date: 7/19/95    Time: 5:44p
* Updated in $/SecureLAN/SDU/Include
* removed CFGMGR_SEMA and DOS_RES
* 
* *****************  Version 21  *****************
* User: Bilal        Date: 5/03/95    Time: 11:19a
* Updated in $/SecureLAN/SDU/Include
* Added the serial interface task, eliminated user task.
* 
* *****************  Version 20  *****************
* User: Tchi         Date: 4/20/95    Time: 7:44p
* Updated in $/SecureLAN/SDU/Include
* CS config data resouce locking; csAvail to check if CS can start
* 
* *****************  Version 19  *****************
* User: Sduhost      Date: 3/21/95    Time: 10:03a
* Updated in $/SecureLAN/SDU/Include
* Rearragne Task Defintions - conditional use of some tasks.
* 
* *****************  Version 18  *****************
* User: Sduhost      Date: 3/17/95    Time: 8:07p
* Updated in $/SecureLAN/SDU/Include
* Defintions to let files use DOS resource locking.
* 
* *****************  Version 17  *****************
* User: Tchi         Date: 3/16/95    Time: 5:20p
* Updated in $/SecureLAN/SDU/Include
* added one resouce; removd user_task due to kb_hit
* 
* *****************  Version 16  *****************
* User: Sduhost      Date: 3/15/95    Time: 9:20a
* Updated in $/SecureLAN/SDU/Include
* Changes to 've an Idle Task and a separate Initialization task.
* 
* *****************  Version 15  *****************
* User: Venkat       Date: 3/15/95    Time: 9:40a
* Updated in $/SecureLAN/SDU/Include
* Another module ID is added(CCC).
* 
* *****************  Version 14  *****************
* User: Sduhost      Date: 3/03/95    Time: 7:34p
* Updated in $/SecureLAN/SDU/Include
* Safe Stack Sizes!!!
* 
* *****************  Version 13  *****************
* User: Sduhost      Date: 2/28/95    Time: 2:00p
* Updated in $/SecureLAN/SDU/Include
* Changed again Stack Sizes...venkat
* 
* *****************  Version 12  *****************
* User: Sduhost      Date: 2/27/95    Time: 6:18p
* Updated in $/SecureLAN/SDU/Include
* Change in Stack Sizes for various tasks...Venkat
* 
* *****************  Version 11  *****************
* User: Bald_eagle   Date: 2/14/95    Time: 5:07p
* Updated in $/SecureLAN/SDU/Include
* Addition of SDUinit Module ID.
* 
* *****************  Version 10  *****************
* User: Moe          Date: 2/10/95    Time: 1:26p
* Updated in $/SecureLAN/SDU/Include
* Changed extra declaration of PA_MODULE to EP_MODULE
* 
* *****************  Version 9  *****************
* User: Venkat       Date: 2/09/95    Time: 8:44a
* Updated in $/SecureLAN/SDU/Include
* Module Ids are added which are to be unique along with task #s; To
* communicate Errors with IO manager.
* 
* *****************  Version 8  *****************
* User: Bald_eagle   Date: 2/01/95    Time: 3:01p
* Updated in $/SecureLAN/SDU/Include
* Moved IO manager Q definitions from rtxcinfo.c
* 
* *****************  Version 7  *****************
* User: Moe          Date: 1/26/95    Time: 4:02p
* Updated in $/SecureLAN/SDU/Include
* No change
******************************************************************************/

/* RTXC information header */

#ifndef RTXCINFO_H
#define RTXCINFO_H

/* numbers of system objects */
#define NTASKS		9				/* number of tasks */
#define NSEMAS		10				/* number of semaphores in system */
#define NRES		3				/* number of resources */
#define NPARTS		0				/* number of memory partitions */
#define DNPARTS		0				/* number of dynamic partitions */
#define NMBOXES		0				/* number of mailboxes available */
#define NQUEUES		1				/* Number of Queues */
#define NTMRS		15				/* number of timers */

/* task stack size for each task... */
#define STKSIZE1	0x0800			/* init task */
#define STKSIZE2	0x0800			/* conn setup task */
#define STKSIZE3	0x0200      	/* EP custodian */
#define STKSIZE4	0x0400			/* show task - should be eliminated */
#define STKSIZE5	0x0400 			/* user task - should be eliminated */
#define STKSIZE6	0x0400      	/* auth task - nimble footed */
#define STKSIZE7	0x0800      	/* mibmgr - snmp stack depth require more...toolkit api */
#define STKSIZE8	0x0400      	/* medium? */
#define STKSIZE9	0x0800      	/* iomanager -- ?? */

#define SELFTASK	0				/* reference calling task */
#define CONNSTUP	1				/* Connection Setup */
#define EPCUSTOD	2				/* EP Custodian */
#define MIBMGR		3				/* MIB Manager Task */
#define AUTH_TASK	4				/* Authentication task */
#define BOOTPTASK	5				/* BOOTPc */
#define IOMGRTASK	6				/* IO Manager */
#define INITTASK	7				/* initialization task */
#define SHOWTASK	8				/* Status/Message display task */
#define MTR_TASK    9               /* Serial Monitor task */

/* ---------------------- Module IDs -------------------------------- */
/* ***** TO BE Unique #s along wth Task IDs defined above **** */
#define NET_DRIVER		  10
#define PA_MODULE	 	  11
#define EP_MODULE	 	  12
#define CY_MODULE	 	  13
#define SDU_INIT		  14
#define CCC_MODULE		  15

/* resource definitions */
#define CCC_TOOLKIT			1		/* CCC Toolkit request interface */
#define DISP_BUF_RES		2       /* Display Buffer - nothing to do with DOS */
#define	CS_CONFIG_RES		3		/* CS config resource (protocol table, socket table...) */

/* Queue definitions for IO Manager */
# define IOMGR_Q			1
# define IOMGR_QSIZE		4
# define IOMGR_QNUM       100

/* semaphore definitions */

# define FRMARC_SEMA		1		/* frame arch semaphore */
# define CS_FRM_SEMA		2		/* CS entry */
# define EPCUST_SEMA		3		/* EP Custodian entry */
# define MIBMGR_SEMA		4		/* Mib Manager */
# define TKREQ_SEMA			5		/* CCC Toolkit request complete semaphore */
# define TIMER_AUTH_SEMA	6		/* Authentication - Time */
# define VOICE_AUTH_SEMA	7		/* Authentication - Voice */
# define IOMGR_SEMA			8		/* IO Manager Task */
# define MTR_SEMA           9      	/* Serial Monitor Interface */
# define EPC_SEMA			10		/* EP custodian semaphore */

/* clock macros */

#define CLKRATE		100				/* clock tick interval (ms) */
#define CLKTICK		10				/* divider to get # ticks */

/* Critical region facility macros for DOS access */
/*
# ifdef RXTC_DOS_RES_LOCK
	# define  LOCK_DOS_RES()   	KS_lockw( DOS_RES)
	# define  UNLOCK_DOS_RES() 	KS_unlock( DOS_RES)
	# define  UWL_DOS_RES()	 {	KS_unlock( DOS_RES);	\
							KS_delay(SELFTASK, 3);  \
							KS_lockw( DOS_RES);     \
							}
	# define  DELAY_DOS_RES() 	KS_delay(SELFTASK, 3);
# else
	# define  LOCK_DOS_RES()
	# define  UNLOCK_DOS_RES()
	# define  UWL_DOS_RES()
	# define  DELAY_DOS_RES()
# endif
*/

#endif

