/* $Id: net.h,v 1.1.68.1 1996/04/19 14:57:44 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/net.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: net.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: net.h,v $
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
 * Revision 1.1  1995/11/22  23:22:14  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************************************
*  FileName: net.h			Product Name: SecureLAN (SDU)
*  File STATUS:
*  	$Header: /SecureLAN/SDU/Include/NET.H 12    6/26/95 2:59p Venkat 
*
*  Description:
*		Some common macros that are required to be used by phybuf.c file, and
*		Ether Driver.
*
*  Copyright (c) Cylink Corporation 1994. All rights reserved.
**************************************************************************
*
* $History: NET.H 
* 
* *****************  Version 12  *****************
* User: Venkat       Date: 6/26/95    Time: 2:59p
* Updated in $/SecureLAN/SDU/Include
* Function Prototype to map portId from Port Pointer (SNMP Mgr).
* 
* *****************  Version 11  *****************
* User: Sduhost      Date: 5/16/95    Time: 4:00p
* Updated in $/SecureLAN/SDU/Include
* Made changes for DOS image.
* 
* *****************  Version 10  *****************
* User: Venkat       Date: 5/16/95    Time: 6:06p
* Updated in $/SecureLAN/SDU/Include
* Clean Up, and update for AutoSense.
* 
* *****************  Version 9  *****************
* User: Venkat       Date: 3/29/95    Time: 6:09p
* Updated in $/SecureLAN/SDU/Include
* Two MAC addresses to an SDU.
* 
* *****************  Version 8  *****************
* User: Sduhost      Date: 2/22/95    Time: 8:42a
* Updated in $/SecureLAN/SDU/Include
* Port Counters Prototype changed.
* 
* *****************  Version 7  *****************
* User: Venkat       Date: 2/16/95    Time: 3:25p
* Updated in $/SecureLAN/SDU/Include
* System Mac Address variable is available to all Modules/Tasks.
* 
* *****************  Version 6  *****************
* User: Bald_eagle   Date: 2/02/95    Time: 6:43p
* Updated in $/SecureLAN/SDU/Include
* Better Error Codes.
* 
* *****************  Version 5  *****************
* User: Venkat       Date: 1/30/95    Time: 8:41a
* Updated in $/SecureLAN/SDU/Include
* Data structure access to IO Manager.
* 
* *****************  Version 4  *****************
* User: Venkat       Date: 12/17/94   Time: 11:51a
* Updated in $/SecureLAN/SDU/Include
* Debug code is present; OK for integration, but not for full testing.
* 
* *****************  Version 3  *****************
* User: Venkat       Date: 12/08/94   Time: 8:23a
* Updated in $/SecureLAN/SDU/Include
* Release for 2 Card functional integration only.
* 
* *****************  Version 2  *****************
* User: Venkat       Date: 11/02/94   Time: 11:20a
* Updated in $/SecureLAN/SDU/Include
* Added Error Codes 
* 
* *****************  Version 1  *****************
* User: Venkat       Date: 10/19/94   Time: 3:49p
* Created in $/SecureLAN/SDU/Include
* Intial Creation for Physical Buffer Integration.
* 
*************************************************************************/
# ifndef NET_H
# define NET_H

/* physical <--> logical address conversion					 */
typedef struct {
			  ushort offset ;
			  ushort segment ;	
			}  Address ;

# define LOG2PHYS_ADDRESS( p)  p = (void *) ( 				\
						(((ulong)((Address *)&(p))->segment) * 16 ) + 	\
						((Address *)&(p))->offset 	)

/* Converts a 20-bit physical address to segment and offset; updates the argument */
# define PHYS2LOG_ADDRESS( p)  {                       \
						((Address *)&(p))->segment = (ushort)((ulong)p / 16),   \
						((Address *)&(p))->offset  = (ushort)p & 0xF; \
					}
					
# define GetSegOfsFrameP( p) 		((PhysicalBuffer *)p)->frameLogLink
# define GetSegOfsBufferP( p) 		((PhysicalBuffer *)p)->dc.logicalP
# define GetPhysicalBufferP( p) 	((PhysicalBuffer *)p)->dc.memoryP
# define GetControlArea( p)			((PhysicalBuffer *)p)->descriptorP

/* configuration errors									 */
# define NET_SYSTEM_OK			  		  0
# define NET_NO_CHANNELS				-12  /* Probably NICs are not in the SDU */
# define NOSUCH_NET_PORT				-13  /* Net API errors... */
# define NO_FREE_BUFFER					-14
# define NET_UP_REJECT					-15
# define NET_PHYS_CONN_ERROR            -16

# define NET_UP_SUCCESS 		 		  0
# define NET_DOWN_SUCCESS 	 	 		  0
# define NET_LOOP_SUCCESS 		 		  0

/* codes for driver start and stop operations */
# define REJECT_TXREQ				 	  0
# define ACCEPT_TXREQ				 	  1

/* codes for xmit function call response */
# define ACTIVE_NETPORT 				  1
# define INACTIVE_NETPORT 				  0

/* codes to specify the xmit action */
# define FRAME_REFLECT					  1
# define FRAME_FORWARD					  0

struct portCounters {
		ulong  peTxFrames ;	/* PE side Xmitted Frames */
		ulong  peRxFrames ;	/* PE side  Received Frames */
		ulong  upeTxFrames ;	/* UPE side Xmitted Frames */
		ulong  upeRxFrames ;	/* PE side Received Frames */
	} ;

/* -----------------------SDU System MAC address----------------------------- */
extern uchar systemMacAddress_g[6] ;	/* Protected Side MAC (alias System) */
extern uchar macAddressUPS_g[] ;		/* Unprotected Side MAC  */

/* external function prototypes */
extern int  taskTransmitReq( Cy2TxFrameControl *ctfp, int frameLengthBytes, int action);
extern int  netTransmitFrame( Cy2TxFrameControl *txcp) ;
extern int  netPortUp( int portIndex) ;
extern int  netPortDown( int portIndex) ;
extern int  netPortTest( int portIndex) ;
extern int  initNetChannelsControl( void) ;
extern void *getNextPort( void *p) ;
extern void *xmitPortLink( int major, int minor) ;
extern int  getNetPortId( void *p) ;
extern struct portCounters *getPortCounters( struct portCounters *pcp) ;
extern void configureNetChannels( void) ;
extern void shutdownChannels( void) ;
extern void enableNetInterrupts( void) ;
extern void disableNetInterrupts( void) ;
extern void netChannelsLoopTest( void) ; 
extern void startNetChannels( void) ;
# ifdef DOS_OS
extern int getLoopbackResult( int *portId) ;
# else
extern int getLoopbackResult( void) ;
# endif
			  
# endif   
