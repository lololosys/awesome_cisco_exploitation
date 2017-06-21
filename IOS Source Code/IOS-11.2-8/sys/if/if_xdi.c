/* $Id: if_xdi.c,v 3.4.42.5 1996/08/28 12:48:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/if_xdi.c,v $
 *------------------------------------------------------------------
 * Platform independent XDI SMT interface software.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *      This module includes all or part of the XDI interface files
 *      which contain platform specific code:
 *
 *              cspsysif.c,
 *              csplocal.c,
 *              cspmsgif.c,
 *              fbmintif.c,
 *              fbmbufif.c,
 *              fbmesfif.c,
 *              fbmlocal.c,
 *              fbmmsgif.c,
 *              fbmrafif.c,
 *              fbmsysif.c,
 *              mibinim.c,
 *              mibintif.c,
 *              miblocal.c,
 *              mibmsgif.c,
 *              mibsysif.c
 *
 *------------------------------------------------------------------
 * $Log: if_xdi.c,v $
 * Revision 3.4.42.5  1996/08/28  12:48:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.42.4  1996/05/18  08:34:08  fsunaval
 * CSCdi57554:  FDDI PA needs OIR support
 * Branch: California_branch
 * 1.1F OIR support and patch for CSCdi57554.
 *
 * Revision 3.4.42.3  1996/04/30  07:00:54  fsunaval
 * CSCdi51250:  SW-VIP1: Host MIB updating needs to be completed
 * Branch: California_branch
 * 1.Code changes for formation and transfer of SMT Messages to RSP
 *   in form of love letters.
 *
 * Revision 3.4.42.2  1996/03/21  22:48:52  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.4.42.1  1996/03/18  20:13:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/01/15  22:57:13  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/23  04:40:26  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.4  1996/01/05  10:17:17  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  09:29:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:04  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  11:43:46  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 20:53:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***********************************************************************
                                Includes
***********************************************************************/
#include	"master.h"
#include	"logger.h"
#include	"../les/msg_fddi.c"	/* Not a typo, see logger.h */
#include	"../xdi/cschdrsif.h"
#include	"mgd_timers.h"

#include	"../xdi/xdihdrsif.h"
#include	"../xdi/fddihdr.h"
#include	"../xdi/csphdr.h"
#include	"../xdi/cspglbl.h"
#include	"../xdi/smtmsg.h"
#include	"../xdi/cspproto.h"
#include	"../xdi/smterror.h"
#include	"../xdi/cspglbl.h"
#include	"../xdi/fbmframe.h"
#include	"../xdi/fbmhdr.h"
#include	"../xdi/fbmglbl.h"
#include	"../xdi/fbmproto.h"
#include	"../xdi/mibglbl.h"
#include	"../xdi/mibproto.h"

#include	"if_fddi.h"
#include	"if_fddi_public.h"

/*********************************************************************
	Global Variables
*********************************************************************/
extern queuetype	smtbufQ;
extern ulong		next_timeout,next_timeout_h;

/* FIXME: MIF68840 */
extern void SendCSP_loveletter(SMTMessage * msg);

void
#ifdef __STDC__
SendMAPMessage (SMTMessage * msg)
#else
SendMAPMessage (msg)
		SMTMessage             *msg;
#endif
/*********************************************************************
Function:	Send a message from MAP to another process.
Parameters:	msg	= address of buffer containing message.
Input:		msg	= contains SMT message structure that defines the
					message to send.
Output:		None.
Return:		None.
Notes:		In systems, where all Manager tasks (CSP, FBM, MIB and
			possibly MAP), this routine may call the destination
			task's ProcessXXXMessage() function directly.
Modification History:
*********************************************************************/
{
	leveltype	SR;

	switch (msg->destination)
	{
	case CSP_MSG_ID:
		ProcessCSPMessageMulti (msg);
		if (!EmptySignalQueue()) {
		    MDisableCSPInterrupts(&SR);
		    ProcessCSP(&SR);
		    MRestoreCSPInterrupts(&SR);
		}
		break;

	case MIB_MSG_ID:
		ProcessMIBMessageMulti (msg);
		break;

	case FBM_MSG_ID:
		ProcessFBMMessageMulti (msg);
		break;

	default:
		break;
	}

	return;
}

void
#ifdef __STDC__
ProcessMAPMessageMulti (SMTMessage * smtmsg)
#else
ProcessMAPMessageMulti (smtmsg)
                SMTMessage             *smtmsg;
#endif
/*********************************************************************
Function:       Process an SMT message.
Parameters:     smtmsg  = address of message buffer.
Input:          smtmsg  = contains message to process.
Output:         Changes global data and/or posts signals to the signal
                        queue depending upon the message.
Return:         None.
Notes:          None.
Modification History:
*********************************************************************/
{
	return;
}

#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_cspsysif.c if7_v2.2.1:cset.0036:4:4 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Connection Services Process Module

	System Interface Routines

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_cspsysif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0036		02/10/93
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	All functions listed here are used by CSP to access
	environment capabilities.

	Modification History:
        *** SMT 7.2 ***
		Based on 6.2.3 multistation code.
	930203.007	LJP
		The ANSI definition of ConfigureMACMulti() did not use the
		correct parameter names. They now match the traditional C
		parameters.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"cspmacro.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"smtmsg.h"
#include	"cspproto.h"
*/


/*********************************************************************
        PATH Scrubbing Interface Routines
*********************************************************************/

void
#ifdef __STDC__
StartScrubbingMulti (uInt16 stnID, uInt16 pathID)
#else
StartScrubbingMulti (stnID, pathID)
                uInt16                  stnID;
                uInt16                  pathID;
#endif
/*********************************************************************
Function:       Implement a particular scrubbing mechanism on the given path.
Parameters:     stnID   = index of station to use.
                        pathID  = CSP index of path to scrub.
Input:
Output:
Return:         None.
Notes:          The default (and probably easiest) mechanism for scrubbing
                        is to source IDLE symbols from a PORT on the given path.
                        This mechanism is handled by the PCM state machine by
                        sending a SIG_PC_Scrub_Start signal to any PORT on the
                        path. The PCM state machine handles the timing and stopping
                        of the scrub function. Other mechanisms may be used,
                        however, their implementation is left to the implementer.
                                Also, the PCM state machine only allows one PORT
                        on the local path at any given time. Therefore, the local
                        path does not need to be scrubbed.
Modification History:
*********************************************************************/
{
uInt16                  i;                              /* loop counter */
Flag                    portFound;              /* flag if port found on path */
CSPPORTInfoType        *portPtr;

        /* find the first PORT on the path */
        portFound = FALSE;
        for (i = 0, portPtr = &(CSPDATA (stnID, portData[0]));
                i < MAX_PORT_COUNT;
                i++, portPtr++)
        {
                /* if this PORT is ACTIVE */
                if (portPtr->pcState == PC_ACTIVE)
                {
                        /*
                        * If PORT is on the requested path or not local
                        * path and active attachment port is concatenated.
                        */
                        if ((portPtr->cpIndex == pathID)
                                || ((pathID < PII_local)
                                        && (portPtr->currentPath
                                                == CP_concatenated)))
                        {
                                /* this port is on the path */
                                SendLineStateMulti (stnID, portPtr->ID, ILS);
                                SetCSPTimerMulti (stnID, T_Scrub, PCM_SIGNALS,
                                        portPtr->ID, &portPtr->TScrub);
                                break;
                        }
                }
        }

        /*
        * If no ports on the path, then the path is completely internal
        * to the station. Since the Cisco stations' internal path is so
        * short (not a single frame can fit in it), internal scrubbing
        * will not be performed.
        */

        return;
}

#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_csplocal.c if7_v2.2.1:cset.0009:3:3 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Connection Services Process Module
	
	CSP Local Processing Module

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.
	
	File:			if7_csplocal.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED
	
	This module contains functions relevant to a particular
	implementation. The implementor may provide any additional
	support and functionality required for the product. Those
	areas in the CSP that have specific local action
	references call functions located in this file.

	Modification History:
        *** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smterror.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"cspmacro.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"cspproto.h"
*/


/*********************************************************************
	Local Processing Functions
*********************************************************************/

void
#ifdef __STDC__
LocalCSPMessages (SMTMessage * smtmsg)
#else
LocalCSPMessages (smtmsg)
		SMTMessage             *smtmsg;
#endif
/*********************************************************************
Function:	Process messages not handled through standard CSP
			message processing.
Parameters:	smtmsg	= address of SMT message buffer.
Input:		smtmsg	= contains message to process.
Output:		Implementor defined.
Return:		None.
*********************************************************************/
{
	return;
}

void
#ifdef __STDC__
LocalSignalQueueFull (struct SigQueueEntry q[], uInt16 * headIndex,
	uInt16 * tailIndex)
#else
LocalSignalQueueFull (q, headIndex, tailIndex)
		struct SigQueueEntry    q[];
		uInt16                 *headIndex;
		uInt16                 *tailIndex;
#endif
/*********************************************************************
Function:	Handle a signal queue full condition.
Parameters:	q			= pointer to signal queue array.
			headIndex	= pointer to index of queue head entry.
			tailIndex	= pointer to index of queue tail entry.
Input:		q			= current signal queue contents.
Output:		Implementer defined.
Return:		None.
Notes:		This routine is provided to allow implementers to handle
			possibly fatal signal queue full conditions. This routine
			will probably be most useful during debugging to indicate
			an insufficient queue size or misbehaving hardware.
			Refer to the XLNT Manager Integration Guide for possible
			solutions to queue full conditions.
*********************************************************************/
{
	return;
}

void
#ifdef __STDC__
LocalTimerListFull (struct TimerListEntry list[], uInt16 ID,
	uInt16 count, uInt16 index)
#else
LocalTimerListFull (list, ID, count, index)
		struct TimerListEntry   list[];
		uInt16                  ID;
		uInt16                  count;
		uInt16                  index;
#endif
/*********************************************************************
Function:	Handle a timer list full condition.
Parameters:	list	= pointer to timer list array.
			ID		= current event ID.
			count	= # of active events in the list.
			index	= array index of event in the system timer.
Input:		list	= current timer list.
Output:		Implementer defined.
Return:		None.
Notes:		This routine is provided to allow implementers to handle
			the fatal timer list full condition. This routine
			will probably be most useful during debugging to indicate
			an insufficient list size.
			Refer to the XLNT Manager Integration Guide for possible
			solutions to timer list full conditions.
*********************************************************************/
{
	return;
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_cspmsgif.c if7_v2.2.1:cset.0009:3:3 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Connection Services Process Module

	Message Interface Routines

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_cspmsgif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module provides the interface routines to handle message
	passing between CSP and the other processes in the SMT system.

	Modification History:
        *** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smterror.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"cspmacro.h"
#include	"cspproto.h"
*/


/*********************************************************************
	Message Interface Routines
*********************************************************************/

void
#ifdef __STDC__
SendCSPMessage (SMTMessage * msg)
#else
SendCSPMessage (msg)
		SMTMessage             *msg;
#endif
/*********************************************************************
Function:	Send a message from CSP to another process.
Parameters:	msg	= address of buffer containing message.
Input:		msg	= contains SMT message structure that defines the
					message to send.
Output:		None.
Return:		None.
Notes:		In systems, where all Manager tasks (CSP, FBM, MIB and
			possibly MAP), this routine may call the destination
			task's ProcessXXXMessage() function directly.
Modification History:
*********************************************************************/
{
	switch (msg->destination)
	{
	case MIB_MSG_ID:
		ProcessMIBMessageMulti (msg);
		break;

	case MAP_MSG_ID:
		ProcessMAPMessageMulti (msg);
		break;

	case FBM_MSG_ID:
		ProcessFBMMessageMulti (msg);
		break;

	default:
		break;
	}

#if defined(RVIP) || defined(SVIP)
	SendCSP_loveletter(msg);
#endif

	return;
}

uInt32
#ifdef __STDC__
InitCSPMessageMulti (uInt16 stnID)
#else
InitCSPMessageMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Initialize system for sending and receiving SMT messages.
Parameters:	None.
Input:		None.
Output:		Set system message processing.
Return:		0 if successful, error number if failed.
*********************************************************************/
{
	return (0);
}

/*********************************************************************
	MIB Interface Routines

	The MIB interface routines are placed with the message
	interface routines because the MIB interface may require
	strong support from the message passing facilities provided
	by the system. In cases where this process is separate and
	unique from the MIB process, the message passing facilities
	will implement some form of interprocess communication.

	The strong support will be required when this process
	is waiting for a response from the MIB. In this case, any other
	messages will need to be held off or enqueued for storage
	until the MIB response is received.

	CSP does not set MIB attributes through the standard
	change interface. Therefore, only the Get operation
	needs to be implemented.
*********************************************************************/

uInt32
#ifdef __STDC__
CSPGetMIBAttrMulti (uInt16 stnID, uInt16 bufSize, uChar * bufData)
#else
CSPGetMIBAttrMulti (stnID, bufSize, bufData)
		uInt16                  stnID;
		uInt16                  bufSize;
		uChar                  *bufData;
#endif
/*********************************************************************
Function:	Get information from the MIB. This function provides
			CSP with access to the MIB.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							requested attribute information
							and used to hold attribute value returned.
Input:		bufData		= contains the requested attribute ID and
							object index (if required) using
							the MIB TLV encoding for a Get
							Request (see SMT standard).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT standard.
Return:		Returns the same values as specified by GetXMMIBAttrMulti().
			If messages are used, then the return value is given by
			the result member of the MIBMsgType structure.
Notes:		CSP is initialized to know the index of entity the MIB's
			object arrays. This avoids a table lookup and several
			array references.  MSP provides a special function to
			support this quick access.

			In cases where MSP and CSP reside in the same
			executable image, this routine can simply call
			GetXMMIBAttrMulti().

			If CSP and MSP are separated, this function will
			perform the necessary communication functions to
			access the MIB and wait for a reply. The message
			type to use is MIB_ACTION_SHORT_GET_ATTR.  The
			operation of this function must cause CSP to block
			(or effectively stop processing) until the response
			is received from the MIB. This routine is only called
			during initialization so responsiveness is not a major
			issue.
*********************************************************************/
{
extern uInt32
#ifdef __STDC__
GetXMMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData);
#else
GetXMMIBAttrMulti ();
#endif

	return (GetXMMIBAttrMulti (stnID, bufSize, bufData));
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_fbmintif.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Frame Based Management Module

	Interrupt/Event/Invocation Interface Routines

	THIS MODULE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_fbmintif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module provides the interface routines to handle event
	conditions that must be reported to FSP.

	Modification History:
        *** SMT 7.2 ***
                Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smtmacro.h"
#include	"smterror.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"fbmmacro.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmglbl.h"
#include	"fbmproto.h"
*/


/*********************************************************************
	Interrupt Interface Routines
*********************************************************************/

uInt32
#ifdef __STDC__
InitFBMInterruptsMulti (uInt16 stnID)
#else
InitFBMInterruptsMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Initialize system for processing FBM interrupts.
Parameters:	stnID		= index of station to use.
Input:		None.
Output:		Set system signal processing.
Return:		0 if successful, error number if failed.
*********************************************************************/
{
	/*
	* There are no FBM interrupts in this implementation.
	*/
	return (0);
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_fbmbufif.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Frame Based Management Module

	Frame Buffer Management Routines
	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_fbmbufif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	The FSP generates only one transmit frame at a time. To reduce
	memory management overhead, FSP uses one buffer to build
	out-going SMT frames throughout its execution. This buffer may
	be statically defined or dynamically allocated during FSP
	initialization. The buffer must be at least MAX_FRAME_SIZE
	bytes long and the first byte must start on a 32-bit boundary.

	Management of the received frame buffers is left to the
	frame reception interface. Since FSP does not know how frames
	are received, nor the best method for handling these buffers,
	no management method is specified. When an SMT frame needs to be
	processed, the system passes a pointer to this frame to FSP.
	FSP only reads data from this buffer, no modification is done
	on the frame data or the buffer.

	When FSP is done with the received frame buffer, it calls the
	interface routine FreeFrameBuffer() to release the buffer.

	Modification History:
        *** SMT 7.2 ***
                Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smtmacro.h"
#include	"smterror.h"
#include	"fddihdr.h"
#include	"fbmmacro.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmglbl.h"
#include	"smtmsg.h"
#include	"fbmproto.h"
*/


/*********************************************************************
	Frame Buffer Routines
*********************************************************************/

void
#ifdef __STDC__
FreeFrameBuffer (Void_p buf)
#else
FreeFrameBuffer (buf)
		Void_p                  buf;
#endif
/*********************************************************************
Function:	Releases the memory used by a frame buffer containing
			a RECEIVED SMT frame.
Parameters:	buf	= address of buffer to free.
Input:		None.
Output:		Frame buffer is returned to SMT free buffer queue.
Return:		No value returned.
*********************************************************************/
{
	*(ulong *)buf = 0;
	p_enqueue(&smtbufQ, buf);
	return;
}

void
#ifdef __STDC__
ClearFrameBuffer (Void_p buf)
#else
ClearFrameBuffer (buf)
		Void_p                  buf;
#endif
/*********************************************************************
Function:	Clears the contents of a frame buffer to 0s.
Parameters:	buf	= address of buffer to clear.
Input:		None.
Output:		Clears contents of buffer to all 0s.
Return:		No value returned.
*********************************************************************/
{
	MEMZERO (buf, MAX_FRAME_SIZE);
	return;
}

uInt32
#ifdef __STDC__
InitFBMFrameBuffer (Void_p * bufptr)
#else
InitFBMFrameBuffer (bufptr)
		Void_p                 *bufptr;
#endif
/*********************************************************************
Function:	Initialize the frame buffer management interface.
Parameters:	bufptr	= address of pointer to hold address of buffer.
Input:		Local option.
Output:		bufptr	= set to address of frame buffer.
Return:		0 return if successful, otherwise an error code is
			returned.
Notes:		The frame buffer must be at least MAX_FRAME_SIZE bytes
			in length. It must also start on a 32-bit boundary.
*********************************************************************/
{
	*bufptr = (Void_p) GETMEMORY (MAX_FRAME_SIZE);
	if (*bufptr)
		return (0);
	else
		return (1);
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_fbmesfif.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Frame Based Management Module

	Local ESF Processing Module

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_fbmesfif.c
	Created:		09/05/90

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module contains all implementer routines for handling
	Extended Service Frames.

	Modification History:
	*** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smterror.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmmacro.h"
#include	"fbmproto.h"
*/


/*********************************************************************
	ESF Processing Routines
*********************************************************************/

void
#ifdef __STDC__
LocalESFAnnouncementMulti (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
LocalESFAnnouncementMulti (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16                  stnID;
		Void_p                  memPtr;
		uInt16                  frameLen;
		uInt16                  MACNum;
		uChar                   EACbits;
#endif
/*********************************************************************
Function:	This function is called from the ProcessESFAnnouncement()
		routine. When this function is called, the frame is
		known to be good.
Parameters:	stnID		= index of station to use.
		memPtr		= address of frame buffer.
		frameLen	= length of frame (in bytes).
		MACNum		= MAC index that received the frame.
		EACbits		= EAC indicators as received by the MAC.
Input:		memPtr		= contains frame to process. This must
				include the three pad bytes at the start
				of the frame.
Output:		Local option.
Return:		None.
Modification History:
*********************************************************************/
{
	return;
}

uInt32
#ifdef __STDC__
LocalESFRequestMulti (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
LocalESFRequestMulti (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16                  stnID;
		Void_p                  memPtr;
		uInt16                  frameLen;
		uInt16                  MACNum;
		uChar                   EACbits;
#endif
/*********************************************************************
Function:	This function is called from the ProcessESFRequest()
			routine. When this function is called, the frame is
			known to be good. If this routine returns any value
			other than RC_Success, then the calling routine generates
			an RDF using the value returned as the reason code.
Parameters:	stnID		= index of station to use.
			memPtr		= address of frame buffer.
			frameLen	= length of frame (in bytes).
			MACNum		= MAC index that received the frame.
			EACbits		= EAC indicators as received by the MAC.
Input:		memPtr		= contains frame to process. This must
							include the three pad bytes at the start
							of the frame.
Output:		Local option.
Return:		The following values may be returned:
				RC_Success
					if this frame was accepted or processed.
				RC_FrameClassNotSupported
					if this frame is not supported.
				Other reason code
					if this frame is not part of the local ESF
					protocol. Used by calling routine to generate an RDF.
Modification History:
*********************************************************************/
{
	return (RC_FrameClassNotSupported);
}

void
#ifdef __STDC__
LocalESFResponseMulti (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
LocalESFResponseMulti (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16                  stnID;
		Void_p                  memPtr;
		uInt16                  frameLen;
		uInt16                  MACNum;
		uChar                   EACbits;
#endif
/*********************************************************************
Function:	This function is called from the ProcessESFRequest()
			routine. When this function is called, the frame is
			known to be good.
Parameters:	stnID		= index of station to use.
			memPtr		= address of frame buffer.
			frameLen	= length of frame (in bytes).
			MACNum		= MAC index that received the frame.
			EACbits		= EAC indicators as received by the MAC.
Input:		memPtr		= contains frame to process. This must
							include the three pad bytes at the start
							of the frame.
Output:		Local option.
Return:		None.
Modification History:
*********************************************************************/
{
	return;
}

uInt32
#ifdef __STDC__
InitESFLocalMulti (uInt16 stnID)
#else
InitESFLocalMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Provide initialization for local ESF support.
Parameters:	stnID		= index of station to use.
Input:		Implementer defined.
Output:		Implementer defined.
Return:		0 if initialization is successful, otherwise an error
		code is returned.
*********************************************************************/
{
	return (0);
}

#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_fbmlocal.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Frame Based Management Module

	Local Processing Module

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_fbmlocal.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module contains all implementer initialization and
	enhancment routines.

	Modification History:
        *** SMT 7.2 ***
                Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smterror.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmmacro.h"
#include	"fbmproto.h"
*/


/*********************************************************************
	Local FBM Processing Routines
*********************************************************************/

void
#ifdef __STDC__
LocalFBMMessages (SMTMessage * smtmsg)
#else
LocalFBMMessages (smtmsg)
		SMTMessage             *smtmsg;
#endif
/*********************************************************************
Function:	Process messages not handled through standard FBM
			message processing.
Parameters:	smtmsg	= address of SMT message buffer.
Input:		smtmsg	= contains message to process.
Output:		Implementor defined.
Return:		None.
*********************************************************************/
{
	return;
}

void
#ifdef __STDC__
LocalFBMNotificationMulti (uInt16 stnID, TLVParamType * mibPtr,
	ECEntryType * ecPtr)
#else
LocalFBMNotificationMulti (stnID, mibPtr, ecPtr)
		uInt16                  stnID;
		TLVParamType           *mibPtr;
		ECEntryType            *ecPtr;
#endif
/*********************************************************************
Function:	Set up the TLVParamType structure to get notification
			report for a vendor-specific notification.
Parameters:	stnID	= index of station to use.
			mibPtr	= buffer for parameter info.
			ecPtr	= element with notification to retrieve.
Input:		ecPtr	= list element from SRP.
Output:		mibPtr	= TLV encoding to retrieve the formatted information
			associated with the notification specified by ecPtr.
			If mibPtr->paramType is zero, then this notification
			is NOT included in the SRF.
Return:		None.
*********************************************************************/
{
	mibPtr->paramType = 0;
	return;
}

void
#ifdef __STDC__
LocalFBMFramesMulti (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
LocalFBMFramesMulti (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16                  stnID;
		Void_p                  memPtr;
		uInt16                  frameLen;
		uInt16                  MACNum;
		uChar                   EACbits;
#endif
/*********************************************************************
Function:	Local processing of valid received SMT frames. This
			routine is called after the standard SMT processing
			has occurred. The frame is known to be a valid SMT
			frame and all standard protocol processing has already
			been handled by ProcessFBMFrame(). This routine can be
			used to implement a frame trace utility, accumulate SMT
			frame statistics, etc.
Parameters:	memPtr		= pointer to buffer containing frame
			frameLen        = length of frame data
			MACNum		= index of MAC that received frame
			EACbits		= received E, A, and C indicators
Input:		memPtr		= contains frame including pad bytes
Output:		Implementor defined.
Return:		None.
*********************************************************************/
{
	return;
}

void
#ifdef __STDC__
LocalAttrNetOrder (TLVParamType * param, uInt16 netOrder,
	uInt16 ptype, uInt16 plen)
#else
LocalAttrNetOrder (param, netOrder, ptype, plen)
		TLVParamType           *param;
		uInt16                  netOrder;
		uInt16                  ptype;
		uInt16                  plen;
#endif
/*********************************************************************
Function:	Provide vendor-specific parameter attribute conversion
			from host byte order to network order byte.
Parameters:	param		= buffer containing parameter.
			netOrder	= direction to convert parameter.
			ptype		= parameter type in host byte order.
			plen		= parameter length in host byte order.
Input:		param		= buffer containing complete TLV encoding
			netOrder	= set to:
							HOST_ORDER convert to host byte order.
							NET_ORDER convert to network byte order.
Output:		param		= TLV encoded parameter in specifed byte order.
Return:		None.
Notes:		This routine only needs to be implemented if host byte order
			is little endian.
*********************************************************************/
{
	return;
}

void
#ifdef __STDC__
ProcessLocalTickMulti (uInt16 stnID)
#else
ProcessLocalTickMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Provide local timed processing support. This routine
			is called every FBM_CLOCK_TICK seconds.
Parameters:	stnID		= index of station to use.
Input:		Local option.
Output:		Local option.
Return:		None.
*********************************************************************/
{
	return;
}

uInt32
#ifdef __STDC__
InitFBMLocalMulti (uInt16 stnID)
#else
InitFBMLocalMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Provide local initialization support. Such support can
			include hardware initialization, local data
			initialization, and system interfaces.
Parameters:	stnID		= index of station to use.
Input:		None.
Output:		None.
Return:		0 if initialization is successful, otherwise an error
			code is returned.
*********************************************************************/
{
uInt32                  result;

        /*
        * In systems where MSP is known to be initialized prior to FBM,
        * the FBM initialization process may be completed here by
        * calling InitFBMMIBDataMulti(). This routine reads the necessary
        * FBM data from the MIB.
        */
        result = InitFBMMIBDataMulti (stnID);

	/*
	 * Allocate SMT buffers for installed FDDI interfaces now
	 * Allocate more when new cards are discovered
	 */
	if (get_smt_bufs(fddi_smt_bufs_needed()))
	    errmsg(&msgsym(NOMEMORY, FDDI),
        	   HWIDBDATA (stnID, 0, unit), "smt main mem buffers");

	return (result);
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_fbmmsgif.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Frame Based Management Module
	
	Message Interface Routines

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_fbmmsgif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module provides the interface routines to handle message
	passing between FBM and the other processes in the SMT system.

	Modification History:
        *** SMT 7.2 ***
                Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smtmacro.h"
#include	"smterror.h"
#include	"fddihdr.h"
#include	"fbmmacro.h"
#include	"smtmsg.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmproto.h"
*/


/*********************************************************************
	Message Interface Routines
*********************************************************************/

void
#ifdef __STDC__
SendFBMMessage (SMTMessage * msg)
#else
SendFBMMessage (msg)
		SMTMessage             *msg;
#endif
/*********************************************************************
Function:	Send a message from FSP to another process.
Parameters:	msg	= address of buffer containing message.
Input:		msg	= contains SMT message structure that defines the
					message to send.
Output:		None.
Return:		None.
Notes:		In systems, where all Manager tasks (CSP, FBM, MIB and
			possibly MAP), this routine may call the destination
			task's ProcessXXXMessage() function directly.
Modification History:
*********************************************************************/
{
	leveltype	SR;

	switch (msg->destination)
	{
	case MIB_MSG_ID:
		ProcessMIBMessageMulti (msg);
		break;

	case MAP_MSG_ID:
		ProcessMAPMessageMulti (msg);
		break;

	case CSP_MSG_ID:
		ProcessCSPMessageMulti (msg);
		if (!EmptySignalQueue()) {
		    MDisableCSPInterrupts(&SR);
		    ProcessCSP(&SR);
		    MRestoreCSPInterrupts(&SR);
		}
		break;

	default:
		break;
	}

	return;
}

void
#ifdef __STDC__
FreeMessageBuffer (SMTMessage * buf)
#else
FreeMessageBuffer (buf)
		SMTMessage             *buf;
#endif
/*********************************************************************
Function:	Release the memory used by a message received by FSP.
Parameters:	buf	= pointer to message buffer.
Input:		None.
Output:		Implementor defined.
Return:		None.
*********************************************************************/
{
	return;
}

uInt32
#ifdef __STDC__
InitFBMMessageMulti (uInt16 stnID)
#else
InitFBMMessageMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Initialize system for sending and receiving SMT messages.
Parameters:	stnID	= index of station to use.
Input:		None.
Output:		Set system message processing.
Return:		0 if successful, error number if failed.
*********************************************************************/
{
	return (0);
}

/*********************************************************************
		MIB Interface Routines

		The MIB interface routines are placed with the message
		interface routines because the MIB interface may require
		strong support from the message passing facilities provided
		by the system. In cases where this process is separate and
		unique from the MIB process, the message passing facilities
		will implement some form of interprocess communication.

		The strong support will be required when this process
		is waiting for a response from the MIB. In this case, any other
		messages will need to be held off or enqueued for storage
		until the MIB response is received.

		FSP requires three interfaces to the MIB. Two routines provide
		the basic Get and Set operations defined for the MIB. The third
		routine performs the fast access into the MIB as done in CSP.
*********************************************************************/

uInt32
#ifdef __STDC__
FBMGetMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData)
#else
FBMGetMIBAttrMulti (stnID, bufSize, bufData)
		uInt16                  stnID;
		uInt16                  bufSize;
		Void_p                  bufData;
#endif
/*********************************************************************
Function:	Get information from the MIB. This function provides
			FSP with access to the MIB.
Parameters:	stnID	= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the requested
							attribute information and used to hold
							attribute value returned.
Input:		bufData		= contains the requested attribute ID and object
							index (if required) using the MIB TLV encoding
							for a Get Request (see SMT standard).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
Return:		Returns the same values as specified by GetXMMIBAttrMulti().
			If messages are used, then the return value is given by
			the result member of the MIBMsgType structure.
Notes:		FSP is initialized to know the index of entity the MIB's
			object arrays. This avoids a table lookup and several
			array references.  MSP provides a special function to
			support this quick access.

			In cases where the MSP and FSP reside in the same
			executable image, this routine can simply call
			GetXMMIBAttrMulti().

			If FSP and MSP are separated, this function will
			perform the necessary communication functions to
			access the MIB and wait for a reply. The message
			type to use is MIB_ACTION_SHORT_GET_ATTR.  The
			operation of this function must cause FSP to block
			(or effectively stop processing) until the response
			is received from MSP. Responsiveness is more of an
			issue here than in CSP because the values retrieved
			are used to build frames for the SMT protocols. However,
			FSP is not very time critical. This allows reasonable
			response times to be acceptable.
*********************************************************************/
{
extern uInt32
#ifdef __STDC__
GetXMMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData);
#else
GetXMMIBAttrMulti ();
#endif

	return (GetXMMIBAttrMulti (stnID, bufSize, bufData));
}

uInt32
#ifdef __STDC__
FBMMultiGetMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData,
	TLVSetCountType * setCount)
#else
FBMMultiGetMIBAttrMulti (stnID, bufSize, bufData, setCount)
		uInt16                  stnID;
		uInt16                  bufSize;
		Void_p                  bufData;
		TLVSetCountType        *setCount;
#endif
/*********************************************************************
Function:	Get information from the MIB. This function provides
			FSP with access to the MIB. In cases where the MSP and
			FSP reside in the same executable image, this function
			can simply call MultiGetMIBAttr(). If FSP and MSP are
			separated, this function will perform the necessary
			communication functions to access the MIB and wait for
			a reply.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							requested attribute information
							and used to hold attribute value returned.
			setCount	= pointer to buffer to hold current value
							of the fddiSMTSetCountAttribute
							(may be NULL if current SetCount
							is not required).
Input:		bufData		= contains the requested attribute ID and
							object index (if required) using
							the MIB TLV encoding for a Get
							Request (see SMT standard).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
		setCount	= if not NULL, this buffer gets set to the
						current value of fddiSMTSetCount,
						also using the MIB TLV encoding.
Return:		Returns the same values as specified by MultiGetMIBAttrMulti().
			If messages are used, then the return value is given by
			the result member of the MIBMsgType structure.
Note:		The operation of this function must cause FSP to block
			(or effectively stop processing) until the response
			is received from the MIB. This routine is called
			throughout the protocol processing of FBM.
*********************************************************************/
{
extern uInt32
#ifdef __STDC__
MultiGetMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData,
	TLVSetCountType * setCount);
#else
MultiGetMIBAttrMulti ();
#endif

	return (MultiGetMIBAttrMulti (stnID, bufSize, bufData, setCount));
}


uInt32
#ifdef __STDC__
FBMChangeMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData,
	TLVSetCountType * setCount, SMTStationIdType * stationID)
#else
FBMChangeMIBAttrMulti (stnID, bufSize, bufData, setCount, stationID)
		uInt16                  stnID;
		uInt16                  bufSize;
		Void_p                  bufData;
		TLVSetCountType        *setCount;
		SMTStationIdType       *stationID;
#endif
/*********************************************************************
Function:	FSP access routine to set a MIB attribute.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							new attribute information
							and used to hold attribute value returned.
			setCount	= pointer to buffer holding the current value
							of the fddiSMTSetCount attribute
			stationID	= pointer to buffer holding ID of station
							requesting the change.
Input:		bufData		= contains the new attribute value using
							the MIB TLV encoding for a Get
							Request (see SMT document).
			setCount	= contains the verification value for fddiSMTSetCount.
			stationID	= contains station ID of the station
							requesting the attribute change.
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
			setCount	= set to the value of fddiSMTSetCount after
							attempting the attribute change,
							also using the MIB TLV encoding.
Return:		Returns the same values as specified by ChangeMIBAttrMulti().
			If messages are used, then the return value is given by
			the result member of the MIBMsgType structure.
Note:		The operation of this function must cause FSP to block
			(or effectively stop processing) until the response
			is received from the MIB. This routine is called
			throughout the protocol processing of FBM.
*********************************************************************/
{
extern uInt32
#ifdef __STDC__
ChangeMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData,
	TLVSetCountType * setCount, SMTStationIdType * stationID);
#else
ChangeMIBAttrMulti ();
#endif

	return (ChangeMIBAttrMulti (stnID, bufSize, bufData,
			setCount, stationID));
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_fbmrafif.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Frame Based Management Module

	Local RAF Processing Module

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_fbmrafif.c
	Created:		07/28/92

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module contains all implementor routines for handling
	Resource Allocation Frames for resources not supported by
	the XLNT Manager core routines.

	Modification History:
        *** SMT 7.2 ***
                Based on 6.2.3 multistation code file fbmesfif.c.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smterror.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmmacro.h"
#include	"fbmproto.h"
*/


/*********************************************************************
	RAF Processing Routines
*********************************************************************/

uInt32
#ifdef __STDC__
LocalRAFRequestMulti (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
LocalRAFRequestMulti (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16                  stnID;
		Void_p                  memPtr;
		uInt16                  frameLen;
		uInt16                  MACNum;
		uChar                   EACbits;
#endif
/*********************************************************************
Function:	This function is called from the ProcessRAFRequest()
			routine. When this function is called, the frame is
			known to be good. If this routine returns any value
			other than RC_Success, then the calling routine generates
			an RDF using the value returned as the reason code.
Parameters:	stnID		= index of station to use.
			memPtr		= address of frame buffer.
			frameLen	= length of frame (in bytes).
			MACNum		= MAC index that received the frame.
			EACbits		= EAC indicators as received by the MAC.
Input:		memPtr		= contains frame to process. This must
							include the three pad bytes at the start
							of the frame.
Output:		Local option.
Return:		The following values may be used:
				RC_Success
					if this frame was accepted or processed.
				RC_FrameClassNotSupported
					this frame is not supported
				Other reason code
					if this frame is not part of the local RAF
					protocol. Used by calling routine to generate an RDF.
Modification History:
*********************************************************************/
{
	return (RC_FrameClassNotSupported);
}

void
#ifdef __STDC__
LocalRAFResponseMulti (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
LocalRAFResponseMulti (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16                  stnID;
		Void_p                  memPtr;
		uInt16                  frameLen;
		uInt16                  MACNum;
		uChar                   EACbits;
#endif
/*********************************************************************
function:	This function is called from the ProcessRAFRequest()
			routine. When this function is called, the frame is
			known to be good.
Parameters:	stnID		= index of station to use.
			memPtr		= address of frame buffer.
			frameLen	= length of frame (in bytes).
			MACNu		= MAC index that received the frame.
			EACbits		= EAC indicators as received by the MAC.
Input:		memPtr		= contains frame to process. This must include the three pad bytes at the start of the frame.
Output:		Local option.
Return:		None.
Modification History:
*********************************************************************/
{
	return;
}

uInt32
#ifdef __STDC__
InitRAFLocalMulti (uInt16 stnID)
#else
InitRAFLocalMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Provide initialization for local RAF support.
Parameters:	stnID	= index of station to use.
Input:
Output:
Return:		0 if initialization is successful, otherwise an error
		code is returned.
*********************************************************************/
{
	return (0);
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_fbmsysif.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Frame Based Management Module
	
	System Environment Interface Routines

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_fbmsysif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED
	
	This module provides the interface routines to services
	provided by the system environment.

	Modification History:
        *** SMT 7.2 ***
                Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smtmacro.h"
#include	"smtmsg.h"
#include	"fddihdr.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmmacro.h"
#include        "fbmproto.h"
*/


/*********************************************************************
	System Interface Routines
*********************************************************************/

static void
#ifdef __STDC__
ServiceFBMTimer (uInt16 *stnIDPtr,  uChar *nullPtr1, uChar *nullPtr2)
#else
ServiceFBMTimer (stnIDPtr, nullPtr1, nullPtr2)
		uInt16                  *stnIDPtr;
		uChar                   *nullPtr1;
		uChar                   *nullPtr2;
#endif
/*********************************************************************
Function:	Handle FBM timer tick.
Parameters:	None.
Input:		None.
Output:		None.
Return:		void.
*********************************************************************/
{
	/* Report FBM Timer tick event */
	PostFBMQueueMulti (*stnIDPtr, FBM_Q_TIMER, 0, 0, 0, 0);
	process_wakeup(smt_process_id);
}

/*********************************************************************
 Handle FBM timer tick.
*********************************************************************/
static process ServiceFBMTimer_process (void)
{
    while (TRUE) {
	/* Sleep until there is a FBM Timer tick event */
	process_sleep_on_timer(&fbmTimer_parent);
	
	while (mgd_timer_expired(&fbmTimer_parent)) {
	    mgd_timer *expired_timer;
	    ushort stnID;
	    expired_timer =  mgd_timer_first_expired(&fbmTimer_parent);
	    mgd_timer_update(expired_timer, FBM_CLOCK_TICK*ONESEC);
	    stnID = mgd_timer_type(expired_timer);

	    ServiceFBMTimer(&stnID, NULL, NULL);
	}
    }
}
uInt32
#ifdef __STDC__
InitFBMTimerMulti (uInt16 stnID)
#else
InitFBMTimerMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Initialize the FSP system timer.
Parameters:	None.
Input:		None.
Output:		None.
Return:		0 if successful, otherwise an error code.
*********************************************************************/
{
	static uInt16 StnIndex[MAX_STATION_COUNT];
	uInt16 i;

        /* Initialize local variables */
        for (i=0; i<MAX_STATION_COUNT; i++)
	    StnIndex[i] = i;

	/* Create FBM timer process and parent timer if not done so yet */
	if (!process_exists(ServiceFBMTimer_process_id)) {
	    mgd_timer_init_parent(&fbmTimer_parent, NULL);
	    ServiceFBMTimer_process_id =
		process_create(ServiceFBMTimer_process, "FBM Timer",
			       NORMAL_STACK, PRIO_HIGH);
	}

	/* Create FBM tick timer and start it */
	mgd_timer_init_leaf(&(STADATA(stnID, fbmTimer)), &fbmTimer_parent, stnID, NULL, TRUE);

	mgd_timer_start(&(STADATA(stnID, fbmTimer)), FBM_CLOCK_TICK*ONESEC);

	return (0);
}


/*********************************************************************
	Authorization Interface Routines
*********************************************************************/

#define	AUTHORIZE_STR	"XDI SMT 7.2 "
#define	AUTHORIZE_LEN	(strlen (AUTHORIZE_STR))

Flag
#ifdef __STDC__
VerifyAuthorizationMulti (uInt16 stnID, uChar * aBuffer, uInt16 aLen,
	Void_p memPtr, uInt16 frameLen, uInt16 macID)
#else
VerifyAuthorizationMulti (stnID, aBuffer, aLen, memPtr, frameLen, macID)
		uInt16                  stnID;
		uChar                  *aBuffer;
		uInt16                  aLen;
		Void_p                  memPtr;
		uInt16                  frameLen;
		uInt16                  macID;
#endif
/*********************************************************************
Function:	Takes the contents of the Authorization parameter and
			checks to see if it passes the authentication routine.
			The authentication process may be based on and of the
			following: "presence, absence, or contents of the
			Authorization parameters; the presence or absence of
			a SetCount parameter; the soiurce station ID; the requested
			operation; the target attribute; or any combination of
			these or any other criteria." (see SMT 7.2 section 8.4.1.2)
Parameters:	stnID		= index of station to use.
			aBuffer		= pointer to start of Authorization value.
			aLen		= length (in bytes) of aBuffer.
			memPtr		= start of frame buffer (including PAD bytes).
			frameLen	= length of frame (excluding PAD bytes).
			macID		= index of MAC that received the frame.
Input:		aBuffer		= contains the octet string of the Authorization
							parameter.
Output:		None.
Return:		SET		= authentication passed.
			CLEAR	= authentication failed.
*********************************************************************/
{
	if (aLen == AUTHORIZE_LEN)
	{
		if (strcmp ((char *)aBuffer, AUTHORIZE_STR) == 0)
			return (SET);
	}

	return (CLEAR);
}

uInt16
#ifdef __STDC__
BuildAuthorizationMulti (uInt16 stnID, uChar * aBuffer,
	SMTFrameHdrType * frameHdr, uInt16 maxLen, uInt16 macID)
#else
BuildAuthorizationMulti (stnID, aBuffer, frameHdr, maxLen, macID)
		uInt16                  stnID;
		uChar                  *aBuffer;
		SMTFrameHdrType        *frameHdr;
		uInt16                  maxLen;
		uInt16                  macID;
#endif
/*********************************************************************
Function:	Creates the contents of the Authorization parameter.
Parameters:	stnID		= index of station to use.
			aBuffer		= points to start of Authorization value in
							parameter.
			frameHdr	= pointer to start of frame.
			maxLen		= maximum length for Authorization parameter.
			macID		= index of MAC sending this frame.
Input:		None.
Output:		aBuffer	= contains octet string of the Authorization value.
Return:		Length (in bytes) of the contents in aBuffer.
Notes:		Length must be a multiple of 4. The frame header is passed
			to this routine to allow the authentication process to use
			the frame contents in determining the authorization parameter.
*********************************************************************/
{
	/* this example just puts a fixed string into the parameter */
	MEMCOPY (aBuffer, AUTHORIZE_STR, AUTHORIZE_LEN);
	return (AUTHORIZE_LEN);
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_mibinim.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Management Information Base Module

	Implementation-dependent system initialization routines.

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_mibinim.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module contains implementation-dependent initialization
	routines. These routines allocate memory used to store the MIB
	data base and initialized implementation-dependent value in the
	MIB. All of these routines are called from InitMIB() in the
	mibinit.c core file.

	Modification History:
	*** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"fddihdr.h"
#include	"smtmacro.h"
#include	"mibdefs.h"
#include	"mibvend.h"
#include	"mibtypes.h"
#include	"mibglbl.h"
#include	"smtmsg.h"
#include	"mibproto.h"
*/


/*********************************************************************
	Initialization Routines
*********************************************************************/

uInt32
#ifdef __STDC__
InitMIBBuffer (Void_p * bufPtr, uInt16 * bufLen)
#else
InitMIBBuffer (bufPtr, bufLen)
		Void_p                 *bufPtr;
		uInt16                 *bufLen;
#endif
/*********************************************************************
Function:	Allocate a block of memory large enough to
			hold a maximum sized request buffer. This should be
			at least as large as the maximum transmit SMT INFO length
			plus one extra header to mark the end of the buffer during
			Get operations (MAX_SMT_XMIT_LEN + SIZE_DEFAULT). The buffer
			must start on a 32-bit boundary.
Parameters:	bufPtr	= address of pointer to hold buffer location.
			bufLen	= address of pointer to hold size of buffer.
Input:		None.
Output:		Allocates memory.
			bufPtr	= address of buffer location.
			bufLen	= size (in bytes) of buffer location.
Return:		0	if successful, any other value indicates an error.
Notes:		One buffer will be used for all stations.
Modification History:
*********************************************************************/
{
	*bufLen = MAX_SMT_INFO_LEN + SIZE_DEFAULT;
	*bufPtr = (Void_p) GETMEMORY (*bufLen);
	if (*bufPtr)
		return (0);
	else
		return (1);
}

MIBType                *
#ifdef __STDC__
CreateMIBMulti (uInt16 stnID)
#else
CreateMIBMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Allocate and clear a block of memory large enough to
			hold a MIB structure.
Parameters:	stnID	= index of station to use.
Input:		None.
Output:		Allocates memory and clears all locations to 0.
Return:		Pointer to start of memory block if successful.
			NULL if MIB memory could not be allocated.
Modification History:
*********************************************************************/
{
MIBType                *mibptr;

	mibptr = (MIBType *) GETMEMORY (sizeof (MIBType));
	if (mibptr)
		MEMZERO (mibptr, sizeof (MIBType));

	return (mibptr);
}

uInt32
#ifdef __STDC__
BuildSMTMIBMulti (uInt16 stnID, MIBType * mib)
#else
BuildSMTMIBMulti (stnID, mib)
		uInt16                  stnID;
		MIBType                *mib;
#endif
/*********************************************************************
Function:	Fill in implementation-specific SMT MIB attribute values.
Parameters:	stnID	= index of station to use.
			mib		= pointer to a MIB structure.
Input:		None.
Output:		The following MIB attribute values for the SMT object must
			be specified:
			fddiSMTStationId, fddiSMTUserData, and
			fddiSMTManufacturerData.

			These attributes have default values set by the Manager. The
			implementer may change these to match node requirements or
			capabilities. See file fddihdr.h for default values.
			Attribute:					Default:
			fddiSMTAvailablePaths		AP_primary
			fddiSMTConfigCapabilities	Cap_None
			fddiSMTConfigPolicy			CFP_None
			fddiSMTConnectionPolicy		Con_XDI_Default
			fddiSMTT-Notify				T_Notify_Default
			fddiSMTStatRptPolicy		TRUE
			fddiSMTTrace_MaxExpiration	Trace_Max_Default
			fddiSMTBypassPresent		FALSE
			xdiSMTRMTDupCapabilities	RMT_DAC_LEAVE
			xdiSMTConnectMax			0
			xdiSMTMaxMasterCanConnect	0
			xdiSMTGracefulMax			0
			xdiSMTGracefulInsCapabilities	GIC_None
			xdiSMTGracefulInsMode		GIM_off
Return:		0				if successful.
			EMIB_INIT_MIB	if error initializing MIB.
Notes:		The sample configuration given represents a SM-DAS.
Modification History:
*********************************************************************/
{
	/*
	*       Initialize implementation-specific SMT attributes
	*/
	MCopyAddress (&mib->smtData.StationId.longaddress[0],
            HWIDBDATA (stnID, 0, bit_swapped_hardware));
	bcopy (SMTCFG (stnID,manufacturerOUI),
	    mib->smtData.ManufacturerData.manufacturerOUI, 3);
	strcpy ((char *)mib->smtData.ManufacturerData.manufacturerData,
	    (char *)SMTCFG (stnID, pmanufacturerData));
	strcpy ((char *)mib->smtData.UserData,
	    (char *)SMTCFG (stnID, pUserData));

	/*
	*	Initialize optional SMT attributes
	*/
	mib->smtData.AvailablePaths = SMTCFG (stnID, AvailablePaths);
	mib->smtData.ConfigCapabilities = SMTCFG (stnID, ConfigCapabilities);
	mib->smtData.ConfigPolicy = SMTCFG (stnID, ConfigPolicy);
	mib->smtData.ConnectionPolicy = SMTCFG (stnID, ConnectionPolicy);
	mib->smtData.T_Notify = SMTCFG (stnID, T_Notify);
	mib->smtData.StatRptPolicy = SMTCFG (stnID, StatRptPolicy);
	mib->smtData.Trace_MaxExpiration = MUsecs2Fddi (SMTCFG (stnID, Trace_MaxExpiration));
	mib->smtData.BypassPresent = SMTCFG (stnID, BypassPresent);
	mib->smtData.xdi.RMTDupCapabilities = SMTCFG (stnID, xdi.RMTDupCapabilities);

	/*
	* Set supported packages
	*/
	mib->smtData.xdi.packages = SMTCFG (stnID, xdi.packages);

	return (0L);
}

uInt32
#ifdef __STDC__
BuildMACMIBMulti (uInt16 stnID)
#else
BuildMACMIBMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Fill in implementation-specific MAC MIB attribute values.
Parameters:	stnID	= index of station to use.
Input:		Requires MIB to have been created and SMT values initialized.
Output:		The implementer must add manageable MAC entities using
			the AddMACObject() routine first. Upon successful completion
			of AddMACObject(), the implementer must specify the following
			attribute values: any vendor attributes.

			The implementer may optionally specify the following attributes
			to change their default values:
			Attribute:					Default:
			fddiMACRequestedPaths		as specified in standard
			fddiMACFrameErrorThreshold	0
			fddiMACMA_UnitdataEnable	TRUE
			fddiMACNotCopiedThreshold	0

			After adding the manageable objects, the implementer inserts
			physically present MACs into the MIB using InsertMACObject().
			Upon successful completion of InsertMACObject(), the
			implementer must specify the following attribute values:
			fddiMACT_MaxCapability, fddiMACTVXCapability, and
			fddiMACSMTAddress.

			The following optional attributes must be specified if they
			are supported: fddiMACLongGrpAddress and fddiMACShortGrpAddress.

			The implementer may optionally specify the following attributes
			to change their default values:
			Attribute:					Default:
			fddiMACFrameStatusFunctions	FS_None
			fddiMACAvailablePaths		AP_primary
			fddiMACBridgeFunctions		BT_None
			xdiMACRMTDupPolicy			RMT_DUP_LEAVE
			xdiMACLocalPath				PI_local

			The sequence for building the MAC objects may be either:
				Add all MACs, then insert or MACs
			or
				Add and insert each MAC in sequence.

Return:		0				if successful.
			EMIB_INIT_MIB	if error initializing MIB.
Notes:		The sample configuration given represents a SM-DAS.
Modification History:
*********************************************************************/
{
MACDataPtr              macPtr;			/* MIB element for MAC */
uInt32                  result;			/* return value */
uInt16			i;			/* auxiliar index */

	/*
	*	Initialize all station MACs
	*/
	for (i=0; i<MACCFG(stnID,MacCt); i++) {
	    /*
	    * Add MAC to MIB.
	    */
	    result = AddMACObjectMulti (stnID, MACCFG(stnID,mac[i].MacIndex), &macPtr);
	    if (result != 0)
		    return (result);

	    /*
	    * Add any vendor specific attributes.
	    */

	    /*
	    * Initialize optional MAC attributes.
	    */
	    macPtr->baseData.RequestedPaths.reqPaths = MACCFG(stnID,mac[i].RequestedPaths.reqPaths);
	    macPtr->baseData.FrameErrorThreshold = MACCFG(stnID,mac[i].FrameErrorThreshold);
	    macPtr->baseData.MA_UnitdataEnable = MACCFG(stnID,mac[i].MA_UnitdataEnable);
	    macPtr->baseData.NotCopiedThreshold = MACCFG(stnID,mac[i].NotCopiedThreshold);

	    /*
	    * Insert physically present MACs.
	    */
	    result = InsertMACObjectMulti (stnID, MACCFG(stnID,mac[i].MacIndex), &macPtr);
	    if (result != 0)
		    return (result);

	    /*
	    * Initialize required MAC attributes.
	    */
	    macPtr->hwData.T_MaxCapability = MACCFG(stnID,mac[i].T_MaxCapability);
	    macPtr->hwData.TVXCapability = MACCFG(stnID,mac[i].TVXCapability);
	    MCopyAddress (macPtr->hwData.SMTAddress, 
		HWIDBDATA (stnID, macPtr->baseData.xdi.xdiIndex, bit_swapped_hardware));

	    /*
	    * Initialize Long and Short Group Addresses if supported.
	    */

	    /*
	    * Initialize optional MAC attributes.
	    */
	    macPtr->hwData.FrameStatusFunctions = MACCFG(stnID,mac[i].FrameStatusFunctions);
	    macPtr->hwData.AvailablePaths = MACCFG(stnID,mac[i].AvailablePaths);
	    macPtr->hwData.BridgeFunctions = MACCFG(stnID,mac[i].BridgeFunctions);

	    /*
	    * Set supported packages
	    */
	    macPtr->baseData.xdi.packages = MACCFG(stnID,mac[i].xdi.packages);
	}

	return (0);
}

uInt32
#ifdef __STDC__
BuildPATHMIBMulti (uInt16 stnID)
#else
BuildPATHMIBMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Fill in implementation-specific PATH MIB attribute values.
Parameters:	stnID	= index of station to use.
Input:		Requires MIB to have been created and SMT values initialized.
Output:		The implementer must add  >>> ALL <<<  PATH entities using
			the AddPATHObject() routine. Upon successful completion
			of AddPATHObject(), the implementer must specify the following
			attribute values: fddiPATHTVXLowerBound, fddiPATHT_MaxLowerBound,
			fddiPATHMaxT_Req and any vendor attributes.

			The implementer may optionally specify fddiPATHRingLatency
			if it is supported.

Notes:		The sample configuration given represents a SM-DAS.
Return:		0				if successful.
			EMIB_INIT_MIB	if error initializing MIB.
Modification History:
*********************************************************************/
{
PATHDataPtr             pathPtr;		/* MIB element for PATH */
uInt32                  result;			/* return value */
uInt16			i;			/* auxiliar index */

	/*
	*	Initialize all station PATHs
	*/
	for (i=0; i<PATHCFG(stnID,PathCt); i++) {
	    /*
	    * Add PATH to MIB.
	    */
	    result = AddPATHObjectMulti (stnID, PATHCFG(stnID,path[i].PathIndex), &pathPtr);
	    if (result != 0)
		    return (result);

	    /*
	    * Initialize PATH attributes.
	    */
	    pathPtr->baseData.TVXLowerBound = PATHCFG(stnID,path[i].TVXLowerBound);
	    pathPtr->baseData.T_MaxLowerBound = PATHCFG(stnID,path[i].T_MaxLowerBound);
	    pathPtr->baseData.MaxT_Req = PATHCFG(stnID,path[i].MaxT_Req);

	    /*
	    * Set supported packages
	    */
	    pathPtr->baseData.xdi.packages = PATHCFG(stnID,path[i].xdi.packages);
	}

	return (0);
}

uInt32
#ifdef __STDC__
BuildPORTMIBMulti (uInt16 stnID)
#else
BuildPORTMIBMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Fill in implementation-specific PORT MIB attribute values.
Parameters:	stnID	= index of station to use.
Input:		Requires MIB to have been created and SMT values initialized.
Output:		The implementer must add manageable PORT entities using
			the AddPORTObject() routine first. Upon successful completion
			of AddPORTObject(), the implementer must specify the following
			attribute values: any vendor attributes.

			The implementer may optionally specify the following attributes
			to change their default values:
			Attribute:					Default:
			fddiPORTConnectionPolicies	CP_None
			fddiPORTRequestedPaths		as specified in standard
			fddiPORTLer_Cutoff			LER_Cutoff_Default
			fddiPORTLer_Alarm			LER_Alarm_Default
			fddiPORTMACLoop-Time		0
			fddiPORTMaint-LS			LS_qls

			After adding the manageable objects, the implementer inserts
			physically present PORTs into the MIB using InsertPORTObject().
			Upon successful completion of InsertPORTObject(), the
			implementer need not specify any values as all attributes
			have default values.

			The implementer may optionally specify the following attributes
			to change their default values:
			Attribute:						Default:
			fddiPORTAvailablePaths
				(M or S)					AP_primary
				(A or B)					AP_primary | AP_secondary
			fddiPORTPMDClass				PMD_multimode
			fddiPORTConnectionCapabilities	CP_None
			xdiPORTLocalPath				PI_local

			The sequence for building the PORT objects may be either:
				Add all PORTs, then insert or PORTs
			or
				Add and insert each PORT in sequence.

Notes:		The sample configuration given represents a SM-DAS.
Return:		0				if successful.
			EMIB_INIT_MIB	if error initializing MIB.
Modification History:
*********************************************************************/
{
PORTDataPtr             portPtr;		/* MIB element for PATH */
uInt32                  result;			/* return value */
uInt16                  i;                      /* auxiliar index */

        /*
        *       Initialize all station PORTs
        */
        for (i=0; i<PORTCFG(stnID,PortCt); i++) {
	    /*
	    * Add PORT to MIB.
	    */
	    result = AddPORTObjectMulti (stnID, PORTCFG(stnID,port[i].PortIndex),
		    PORTCFG(stnID,port[i].My_Type), &portPtr);
	    if (result != 0)
		    return (result);

	    /*
	    * Initialize vendor attributes.
	    */

	    /*
	    * Initialize optional PORT attributes.
	    */
	    portPtr->baseData.ConnectionPolicies = PORTCFG(stnID,port[i].ConnectionPolicies);
	    portPtr->baseData.RequestedPaths.reqPaths[RP_none] = PORTCFG(stnID,port[i].RequestedPaths.reqPaths[RP_none]);
	    portPtr->baseData.RequestedPaths.reqPaths[RP_tree] = PORTCFG(stnID,port[i].RequestedPaths.reqPaths[RP_tree]);
	    portPtr->baseData.RequestedPaths.reqPaths[RP_peer] = PORTCFG(stnID,port[i].RequestedPaths.reqPaths[RP_peer]);
	    portPtr->baseData.Ler_Cutoff = PORTCFG(stnID,port[i].Ler_Cutoff);
	    portPtr->baseData.Ler_Alarm = PORTCFG(stnID,port[i].Ler_Alarm);
	    portPtr->baseData.MACLoop_Time = PORTCFG(stnID,port[i].MACLoop_Time);
	    portPtr->baseData.Maint_LS = PORTCFG(stnID,port[i].Maint_LS);

	    /*
	    * Insert PORT into MIB.
	    */
	    result = InsertPORTObjectMulti (stnID, PORTCFG(stnID,port[i].PortIndex), &portPtr);
	    if (result != 0)
		    return (result);

	    /*
	    * Initialize optional PORT attributes.
	    */
	    portPtr->hwData.AvailablePaths = PORTCFG(stnID,port[i].AvailablePaths);
	    portPtr->hwData.PMDClass = PORTCFG(stnID,port[i].PMDClass);
	    portPtr->hwData.ConnectionCapabilities = PORTCFG(stnID,port[i].ConnectionCapabilities);

	    /*
	    * Set supported packages
	    */
	    portPtr->baseData.xdi.packages = PORTCFG(stnID,port[i].xdi.packages);
	}
	return (0);
}

uInt32
#ifdef __STDC__
BuildConfigMulti (uInt16 stnID)
#else
BuildConfigMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Create the path configuration for each PATH in the MIB.
Parameters:	stnID	= index of station to use.
Input:		Requires MIB to have been created and SMT values initialized.
Output:		The implementer must create the path configuration lists
			in the MIB. This is done by calling AddObjectPATH() for each
			object on each path.
Return:		0				if successful.
			EMIB_INIT_MIB	if error initializing MIB.
Modification History:
*********************************************************************/
{
uInt32                  result = 0;		/* return value */
uInt16			i, j;			/* auxiliar indexes */

	/*
	*	Initialize all station PATHs' configuration.
	*/
	for (i=0; i<PATHCFG(stnID,PathCt); i++) {
	    /*
	    *	Include all path resources
	    */
	    for (j=0; j<PATHRESCFG(stnID,i,ResCt); j++) {
		result = AddObjectPATHMulti (stnID, PATHCFG(stnID,path[i].PathIndex),
		    PATHRESCFG(stnID,i,res[j].ResIndex),	/* resource index */
		    PATHRESCFG(stnID,i,res[j].ResType),		/* resource type */
		    PATHRESCFG(stnID,i,res[j].OutResIndex),	/* output resource index */
		    PATHRESCFG(stnID,i,res[j].OutResType),	/* output resource type */
		    PATHRESCFG(stnID,i,res[j].InResIndex),	/* input resource index */
		    PATHRESCFG(stnID,i,res[j].InResType));	/* input resource type */
		if (result != 0) break;
	    }
	}

	return (result);
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_mibintif.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Management Services Process Module
	
	Interrupt/Event/Invocation Interface Routines

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_mibintif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module provides the interface routines to handle event
	conditions that must be reported to MIB.

	Modification History:
	*** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"mibtypes.h"
#include	"mibproto.h"
*/


/*********************************************************************
	Interrupt External Declarations
*********************************************************************/


/*********************************************************************
	Interrupt Interface Defined Values
*********************************************************************/


/*********************************************************************
	Interrupt Interface Routines
*********************************************************************/

void
#ifdef __STDC__
MIBInterruptHandler (uInt16 stnID)
#else
MIBInterruptHandler (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	This routine may do any number of things depending
			upon the implementation. Generally, MSP does not handle
			asynchronous events, but it is valid to do so if this
			capabilities is necessary.
Parameters:	Implementor defined.
Input:		Implementor defined.
Output:		Implementor defined.
Return:		None.
*********************************************************************/
{
	return;
}

uInt32
#ifdef __STDC__
InitMIBInterruptsMulti (uInt16 stnID)
#else
InitMIBInterruptsMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Initialize system for processing MIB interrupts.
Parameters:	None.
Input:		None.
Output:		Set system signal processing.
Return:		0 if successful, error number if failed.
*********************************************************************/
{
	return (0);
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_miblocal.c if7_v2.2.1:cset.0009:3:3 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Management Information Base Module

	Local Processing Module

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_miblocal.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module contains all implementor initialization and
	enhancment routines.

	Modification History:
	*** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smterror.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"mibtypes.h"
#include	"mibproto.h"
*/


/*********************************************************************
	Local MIB Routines
*********************************************************************/

uInt32
#ifdef __STDC__
InitMIBLocalMulti (uInt16 stnID)
#else
InitMIBLocalMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Provide local initialization support. Such support can
			include hardware initialization, local data
			initialization, and system interfaces.
Parameters:	stnID	= index of station to use.
Input:		Implementor-defined.
Output:		Implementor-defined.
Return:		0 if initialization is successful, otherwise an error
			code is returned.
*********************************************************************/
{
	return (0);
}

void
#ifdef __STDC__
LocalMIBMessages (SMTMessage * smtmsg)
#else
LocalMIBMessages (smtmsg)
		SMTMessage             *smtmsg;
#endif
/*********************************************************************
Function:	Process messages not handled through standard MIB
			message processing.
Parameters:	smtmsg	= address of SMT message buffer.
Input:		smtmsg	= contains message to process.
Output:		Implementor defined.
Return:		None.
*********************************************************************/
{
	/*
         * If the mib structure hasn't been initialized, that means
         * SMT isn't running yet, so don't try to do anything.  When SMT
         * starts up, the correct data will be copied from the idb.
         */
        if (MIBDATA(smtmsg->stnID, mib) == NULL)
	    return;

        switch (smtmsg->type)
        {
        case MIB_ACTION_CHANGE_MACADDR:

            /* check for valid MAC entity */
            if (smtmsg->stnID < MAX_STATION_COUNT &&
		smtmsg->entity < MAX_MAC_COUNT) {

                /* Update MIB variable and its copies (CSP & FBM) */
                MCopyAddress (MIBDATA(smtmsg->stnID,
				      mib->macBase[smtmsg->entity].hwData.SMTAddress),
			      &smtmsg->data.addr);
                MCopyAddress (MIBDATA(smtmsg->stnID,
				      mib->smtData.StationId.longaddress),
			      &smtmsg->data.addr);
                MCopyAddress (FBMDATA(smtmsg->stnID,
				      fbmMACData[smtmsg->entity].SMTAddress),
			      &smtmsg->data.addr);
                MCopyAddress (FBMDATA(smtmsg->stnID,
				      fbmStationData.stationID.longaddress),
			      &smtmsg->data.addr);
                MCopyAddress (CSPDATA(smtmsg->stnID,
				      macData[smtmsg->entity].SMTAddress),
			      &smtmsg->data.addr);
            }
        }

	return;
}

void
#ifdef __STDC__
LocalMIBEventMulti (uInt16 stnID, MIBType * mibPtr, uInt16 paramType,
        uInt16 xdiIndex, uInt32 vendorID)
#else
LocalMIBEventMulti (stnID, mibPtr, paramType, xdiIndex, vendorID)
                uInt16           stnID;
                MIBType         *mibPtr;
                uInt16           paramType;
                uInt16           xdiIndex;
                uInt32           vendorID;
#endif
/*********************************************************************
Function:       Handle event messages from CSP and FSP that update
                        MIB attribute values. The implementer may ignore this
                        information or perform some statistical accumulation
                        and reporting.
Parameters:     stnID           = index of station to use.
                        mibPtr          = pointer to this station's MIB database.
                        paramType       = MIB attribute type
                        xdiIndex        = array index of object affected. This va
lue
                                                        will be 0 for the SMT obj
ect.
                        vendorID        = if paramType indicates a vendor-specifi
c
                                                        notification, then this p
arameter holds the
                                                        actual vendor notificatio
n ID. Otherwise, this
                                                        value is 0.
Input:          mibPtr          = contains current MIB information.
Output:         Vendor-specific action.
Return:         No value is returned through this function.
Notes:          The following attributes are reported when changed by
                        some internal action based on SMT processing:
                        fddiSMT
                                SetCount
                                LastSetStationId
                                ECMState
                                CF_State
                                HoldState
                                PeerWrapFlag
                                RemoteDisconnectFlag
                        fddiMAC
                                CurrentPath
                                UpstreamNbr     (implies OldUpstreamNbr also)
                                DownstreamNbr   (implies OldDownstreamNbr also)
                                DupAddressTest
                                RMTState
                                DA_Flag
                                UNDA_Flag
                                SMTAddress
                                FrameErrorFlag
                                mspMAC (XDI attributes)
                                                ConfigurationFailed
                                                SM_MA_AvailFlag
                        fddiPATH
                                TraceStatus
                        fddiPORT
                                NeighborType
                                MACIndicated
                                CurrentPath
                                BS_Flag
                                LCTFail_Ct
                                Ler_Estimate
                                Lem_Reject_Ct
                                Lem_Ct
                                ConnectState
                                PCMState
                                PC_Withhold
                                LerFlag
                                EBErrorCondition (no flag attribute)
                Non-XDI vendor-specific event changes are not reported
                here by the core code. However, other interface routines
                may call LocalMIBEventMulti() to report these vendor-
                specific changes.
*********************************************************************/
{
        return;
}

#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_mibmsgif.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Management Services Process Module
	
	Message Interface Routines

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_mibmsgif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED
	
	This module provides the interface routines to handle message
	passing between MIB and the other processes in the SMT system.

	Modification History:
	*** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smterror.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"mibtypes.h"
#include	"mibproto.h"
*/


/*********************************************************************
	Message External Declarations
*********************************************************************/


/*********************************************************************
	Message Interface Defined Values
*********************************************************************/


/*********************************************************************
	Message Interface Global Variables
*********************************************************************/


/*********************************************************************
	Message Interface Routines
*********************************************************************/

void
#ifdef __STDC__
SendMIBMessage (SMTMessage * msg)
#else
SendMIBMessage (msg)
		SMTMessage             *msg;
#endif
/*********************************************************************
Function:	Send a message from MSP to another process.
Parameters:	msg	= address of buffer containing message.
Input:		msg	= contains SMT message structure that defines the
					message to send.
Output:		None.
Return:		None.
Notes:		In systems, where all Manager tasks (CSP, FBM, MIB and
			possibly MAP), this routine may call the destination
			task's ProcessXXXMessage() function directly.
Modification History:
*********************************************************************/
{
	leveltype	SR;

	switch (msg->destination)
	{
	case CSP_MSG_ID:
		ProcessCSPMessageMulti (msg);
		if (!EmptySignalQueue()) {
		    MDisableCSPInterrupts(&SR);
		    ProcessCSP(&SR);
		    MRestoreCSPInterrupts(&SR);
		}
		break;

	case MAP_MSG_ID:
		ProcessMAPMessageMulti (msg);
		break;

	case FBM_MSG_ID:
		ProcessFBMMessageMulti (msg);
		break;

	default:
		break;
	}
	return;
}

uInt32
#ifdef __STDC__
InitMIBMessageMulti (uInt16 stnID)
#else
InitMIBMessageMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Initialize system for sending and receiving SMT messages.
Parameters:	None.
Input:		None.
Output:		Set system message processing.
Return:		0 if successful, error number if failed.
*********************************************************************/
{
	return (0);
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_mibsysif.c if7_v2.2.1:cset.0009:3:3 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
	Management Information Base Module

	System Interface Module

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_mibsysif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module is responsible for interfacing to the system
	environment for providing information used and supplied by
	the Management Information Base (MIB). This module provides
	routines to access hardware- and system-level data.

	Modification History:
	*** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"fddihdr.h"
#include	"smtmacro.h"
#include	"mibdefs.h"
#include	"mibvend.h"
#include	"mibtypes.h"
#include	"mibglbl.h"
#include	"smtmsg.h"
#include	"mibproto.h"
*/


/*********************************************************************
	Defined Values
*********************************************************************/


/*********************************************************************
	External Variables
*********************************************************************/


/*********************************************************************
	System Interface Functions.
*********************************************************************/

uInt32
#ifdef __STDC__
InitTimeStampMulti (uInt16 stnID)
#else
InitTimeStampMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	This function initializes the SMT time stamp function.
			The SMT time stamp is a 64-bit value measuring the
			number of 80 ns intervals since the station started.
			This function is called at station start up and should
			initialize the starting time stamp to 0.
			The accuracy of the time stamp is not critical. However,
			consecutive calls to the time stamp function should
			unique, increasing values (until the the count wraps
			around). The maximum granularity for this time stamp
			is 100,000 units (8 ms).
Parameters:	stnID	= index of station to use.
Input:		None.
Output:		None.
Return:		0 if successful, otherwise an error code is returned.
*********************************************************************/
{
	/* Log system time when interface is coming up */
	GET_TIMESTAMP(STADATA (stnID, StartupTime));
	return (0);
}

void
#ifdef __STDC__
GetTimeStampMulti (uInt16 stnID, FDDITimeStamp * buf)
#else
GetTimeStampMulti (stnID, buf)
		uInt16                  stnID;
		FDDITimeStamp          *buf;
#endif
/*********************************************************************
Function:	This function generates the next SMT time stamp.
			The SMT time stamp is a 64-bit value measuring the
			number of 80 ns intervals since the station started.
Parameters:	buf	= buffer to hold time stamp value.
Input:		Implementer defined.
Output:		Implementer defined.
Return:		No value returned.
*********************************************************************/
{
	/*
	 * The station time stamp is obtained by subtracting the
	 * station's startup time from the current system clock.
	 * Since all this times are given in milliseconds, conversion
	 * to multiples of 80ns is required, which can be obtained
	 * by multiplying the millisecond value by 12,500 (almost
	 * the same as multiplying it by 16,384 - 4,096 = 12,288,
	 * which is much easier to implement).
	 *
	 * The standard also requires that successive frames carry
	 * a different time stamp. If two frames are sent out in
	 * a single millisecond interval (in fact the system time
	 * is updated only every 4ms), frames may carry the same
	 * time stamp. To prevent this problem, an internal counter
	 * is maintained and incremented every time this function
	 * is called, ensuring that each function call will return
	 * a distinct time stamp value.
	 */
	ulong loTS, uniqloTS, hiTS, loTS16k, hiTS16k, loTS4k, hiTS4k;
	static int timestampCtr = 0;
	sys_deltatime elapsed;

	/* Get time since station started */
	elapsed = ELAPSED_TIME64(STADATA (stnID, StartupTime));
	hiTS = (elapsed >> 32) & 0xFFFFFFFF;
	loTS = (elapsed & 0xFFFFFFFF);

	/* Obtain time stamp in multiples of 80ns (approximate) */
	hiTS16k = (hiTS << 14) | (loTS >> 18);
	loTS16k = loTS << 14;
	hiTS4k = (hiTS << 12) | (loTS >> 20);
	loTS4k = loTS << 12;
	hiTS = hiTS16k - hiTS4k;
	loTS = loTS16k - loTS4k;
	if (loTS16k < loTS4k)
	    hiTS--;

	/* Ensure each time stamp is unique */
	uniqloTS = loTS + timestampCtr++;
	if (uniqloTS < loTS )
	    ++hiTS;

	/* Return time stamp */
	buf->hiword = hiTS;
	buf->loword = uniqloTS;
}


/*********************************************************************
	System Interface Functions.
*********************************************************************/

uInt32
#ifdef __STDC__
SetSMTDataMulti (uInt16 stnID, uInt16 attr, SMTBaseAttrType * smtPtr,
        TLVParamType * p)
#else
SetSMTDataMulti (stnID, attr, smtPtr, p)
                uInt16           stnID;
                uInt16           attr;
                SMTBaseAttrType *smtPtr;
                TLVParamType    *p;
#endif
/*********************************************************************
Function:       Set an SMT attribute.
                        Although ALL SMT attributes are managed through the MIB                        database, an interface routine is required for storing
                        the SMT attribute values into a non-volatile memory
                        location. This routine DOES NOT change the MIB itself.
                        It only provides a place for storing SMT attributes
                        permanently in the system.
Parameters:     stnID   = index of station to use.
                        attr    = SMT attribute to set.
                        smtPtr  = pointer to MIB SMT object that has changed.
                        p               = pointer to a buffer holding the data v
alue.
Input:          p               = attribute in TLV encoding
Output:         Non-volatile memory is changed to store the new value. This
                        action is optional and all changes to standard and XDI S
MT
                        attributes may be ignored by this routine.
Return:         This routine returns RC_Success.
*********************************************************************/
{

        switch (attr)
        {
        case fddiSMTUserData:
        case fddiSMTConfigPolicy:
        case fddiSMTConnectionPolicy:
        case fddiSMTT_Notify:
        case fddiSMTStatRptPolicy:
        case fddiSMTTrace_MaxExpiration:
        case fddiSMTStationAction:
                /*
                 * Handle fddiStationSMTAction for action value (p->SMTPARAM16)of
                 * SMTAction_self_Test
                 */
                break;

        case fddiSMTVendorAttrib:
                /* check which vendor */
                if ((p->SMTVENDORID & VENDOR_MASK) == fddiXDI)
                {
                        /* store XDI attribute values */
                        if (p->SMTVENDORID == xdiSMTGracefulInsMode)
                        {
                        }

                        else if (p->SMTVENDORID == xdiSMTMaxMasterCanConnect)
                        {
                        }

                        else if (p->SMTVENDORID == xdiSMTGIMACID)
                        {
                        }

#ifdef XM_USE_VSO_MASIC
                        else if (p->SMTVENDORID == calmSMTMASICSecureRingPolicy)
                        {
                        }
#endif                                                  /* XM_USE_VSO_MASIC */
                }

                /* handle other vendors with else-ifs */

                break;
        }

        return (RC_Success);
}

