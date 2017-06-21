/* $Id: csdata2.c,v 1.1.68.1 1996/04/19 14:57:58 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/task/cs/csdata2.c,v $
 *------------------------------------------------------------------
 * Cylink Source code: csdata2.c
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: csdata2.c,v $
 * Revision 1.1.68.1  1996/04/19  14:57:58  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:50:00  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:26  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Task/CS/CSDATA2.C 2     3/17/95 2:06p Tchi 
*
* DESCRIPTION:	This file was created due to Borland IDE complains about too
*               much global data defined in csdata.c
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CSDATA2.C 
* 
* *****************  Version 2  *****************
* User: Tchi         Date: 3/17/95    Time: 2:06p
* Updated in $/SecureLAN/SDU/Task/CS
* added far
* 
* *****************  Version 1  *****************
* User: Tchi         Date: 11/14/94   Time: 8:47a
* Created in $/SecureLAN/SDU/Task/CS
* created because Borland C++ complains about too many global variables
* in csdata.c
*******************************************************************************/

#define CSDATA_C

/*******************************************************************************
* INCLUDE FILES
*******************************************************************************/
#include "../crypto/sdu/include/connstup.h"

/***************************************************************************
* Pending connection list takes about 63K of memory. It needs a new data
* segment.
***************************************************************************/
#pragma option -zEPCONN_FAR_DATA -zF* -zH*
pconn_entry far		pconn_list[PCONN_ENTRIES];
short 		far  	pconn_used;             /*index linking */
short       far		pconn_free;             /*index linking */

/*---------------------------- END OF FILE -------------------------------- */
