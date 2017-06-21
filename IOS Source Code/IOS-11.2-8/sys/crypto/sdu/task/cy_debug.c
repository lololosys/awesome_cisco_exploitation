/* $Id: cy_debug.c,v 1.1.68.1 1996/04/19 14:57:54 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/task/cy_debug.c,v $
 *------------------------------------------------------------------
 * Cylink Source code: cy_debug.c
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cy_debug.c,v $
 * Revision 1.1.68.1  1996/04/19  14:57:54  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:57  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:31  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Task/DEBUG.C 5     12/16/94 10:37a Croyle 
*
* DESCRIPTION:	Debugging print task.
*
* NOTES:		This file contains string definitions and the IQueue used by
*				tasks and interrupts to display various debugging strings.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: DEBUG.C 
* 
* *****************  Version 5  *****************
* User: Croyle       Date: 12/16/94   Time: 10:37a
* Updated in $/SecureLAN/SDU/Task
* Removed EP and CY string constants
* 
* *****************  Version 4  *****************
* User: Sduhost      Date: 3/16/94    Time: 3:17p
* Updated in $/SecureLAN/SDU/Task
* Final Update for Integration 1C+.
* 
* *****************  Version 3  *****************
* User: Sduhost      Date: 3/05/94    Time: 10:44a
* Updated in $/SecureLAN/SDU/Task
* 
* *****************  Version 2  *****************
* User: Croyle       Date: 11/02/94   Time: 11:47a
* Updated in $/SecureLAN/SDU/Task
* New Cy strings added.
*
* *****************  Version 1  *****************
* User: Croyle       Date: 11/01/94   Time: 4:00p
* Created in $/SecureLAN/SDU/Task
*******************************************************************************/

#define DEBUG_C

/*******************************************************************************
* 						INCLUDE FILES
*******************************************************************************/

#include "../crypto/sdu/include/cylink.h"
#ifndef CISCO
#include <stdio.h>		/* standard input/output routines */
#endif

/* SDU Include Files */

#include "../crypto/sdu/include/iqueue.h"

/*****************************************************************************
								MACROS
*****************************************************************************/

/* see DEBUG.H for MACROs used by TASK and INTERRUPT routines to post string */

/*****************************************************************************
						 SYSTEM GLOBAL
*****************************************************************************/

/* queues used to pass string pointers */

IQueue iq_msg_out = IQ_INI;		/* queue used by output a string */

/* CS debug strings */

const char *cs100 = "connstup:       Pending Connection List entry timed out";
const char *cs200 = "send_nnc_req:   NNC Echo Request sent";
const char *cs300 = "act_on_nnc_req: NNC Echo Reply sent";
const char *cs400 = "recv_nnc_rpy:   NNC Echo Confirm sent";

/*****************************************************************************
						 PUBLIC FUNCTION PROTOTYPES
*****************************************************************************/
/* none */
/*****************************************************************************
						 STATIC FUNCTION PROTOTYPES
*****************************************************************************/
/* none */
/*******************************************************************************
						STATIC FUNCTION DEFINITIONS
*******************************************************************************/
/* none */
/*******************************************************************************
						PUBLIC FUNCTION DEFINITIONS
*******************************************************************************/

/*------------------------- END OF FILE: debug.c ----------------------------- */

