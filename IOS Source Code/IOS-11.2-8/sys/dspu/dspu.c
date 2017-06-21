/* $Id: dspu.c,v 3.2.60.3 1996/08/28 12:41:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu.c,v $
 *------------------------------------------------------------------
 * Dummy placeholder for Downstream PU (DSPU)
 *
 * April 94, Claude Alan Cartee
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dspu.c,v $
 * Revision 3.2.60.3  1996/08/28  12:41:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.2  1996/05/27  06:01:28  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Also fix a bunch of ByteToHexAscii invocations since they are mostly
 * colocated with errmsg invocations
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/05/17  10:47:17  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/03/30  08:10:29  doner
 * CSCdi52657:  IBU modularity - DSPU conversion to use new IOS scheduler
 * Branch: IbuMod_Calif_branch
 * Update to new scheduler interface
 *
 * Revision 3.2  1995/11/17  09:04:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/20  17:02:37  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.2  1995/09/14 06:15:06  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.1  1995/06/07  20:28:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "logger.h"
#include "msg_dspu.c"			/* Not a typo, see logger.h */

#include "dspu.h"
#include "dspu_cps.h"
#include "bfrprim.h"
#include "dspu_adaptor.h"
#include "dspu_ls.h"
#include "dspu_sap.h"
#include "dspu_util.h"

/* DSPU provides 2 levels of decoding, the first level (DspuReceive) determines
 * which "entity" is responsible for processing the incoming signal.
 * The two procedures defined below take care of signals for the adaptor and
 * sap. Signals destined for the link station are sent directly to the link
 * station signal decoder (& ls fsm input encoder): LsReceive.
 */

static void
ProcessSap (TCLSIMsg *clsiMsgP);

static void
ProcessAdaptor (TCLSIMsg *clsiMsgP);



/*=====================================================================*
* DspuReceive
*======================================================================*
*
* Description:
* This routine reads the input message and processes it.
*
**/

void
DspuReceive (TCLSIMsg* clsiMsgP)

{
  boolean error = FALSE;


    if (clsiMsgP->fHeaderLength < sizeof(TCLSIMsg)) {
       errmsg(&msgsym(CLSIBadLength, DSPU),
               ByteToHexAscii((byte*) clsiMsgP, sizeof(TCLSIMsg)));
       error = TRUE;
       }
    else {
       if (CLSIMsgIsConfirm (clsiMsgP) || CLSIMsgIsIndication (clsiMsgP)) {
          switch (clsiMsgP->fIdentifierType) {
 
             case CLS_U_CEP_ID:
                LsReceive (clsiMsgP);
                break;

             case CLS_U_SAP_ID:
                ProcessSap (clsiMsgP);
                break;
 
             case CLS_ENTITY_MGR:
                ProcessAdaptor (clsiMsgP);
                break;
 
             default:
                errmsg(&msgsym(CLSIBadIdentifierType, DSPU),
                        ByteToHexAscii((byte*) clsiMsgP,
                                        sizeof(TCLSIMsg)));
                error = TRUE;
                break;
          }
       }
       else {
          errmsg(&msgsym(CLSIBadPrimitiveType, DSPU),
                  ByteToHexAscii((byte*) clsiMsgP, sizeof(TCLSIMsg)));
          error = TRUE;
       }   
    }

    if (error)
       CLSIMsgDestroy (clsiMsgP);

}  /* DspuReceive */


/*=====================================================================*
* ProcessSap
*======================================================================*
*
* Description:
*    Decode data before passed to Sap.
**/

static void
ProcessSap (TCLSIMsg *clsiMsgP)

{

   Sap *sapP;
   boolean confirm, indication;
   boolean error = FALSE;
   

   confirm = CLSIMsgIsConfirm (clsiMsgP);
   indication = CLSIMsgIsIndication (clsiMsgP);
   sapP = (Sap *) SapFind (CLSIMsgGetIdentifier (clsiMsgP));
   if (!sapP) {
      errmsg(&msgsym(CLSIBadIdentifier, DSPU),
              ByteToHexAscii((byte*) clsiMsgP, sizeof(TCLSIMsg)));
      error = TRUE;
   }      
   else {
      switch (CLSIMsgGetPrimitiveCommand (clsiMsgP)) {
         
         case  CLS_ACTIVATE_SAP:
           if (confirm)
              SapReceive (sapP, SapI_ActivateSapCnf, clsiMsgP);
           else {
              errmsg(&msgsym(CLSIBadPrimitiveType, DSPU),
                      ByteToHexAscii((byte*) clsiMsgP,
                                      sizeof(TCLSIMsg)));
              error = TRUE;
           }
           break;

        case  CLS_DEACTIVATE_SAP:
           if (confirm)
              SapReceive (sapP, SapI_DeactiveSapCnf, clsiMsgP); 
           else {
              errmsg(&msgsym(CLSIBadPrimitiveType, DSPU),
                      ByteToHexAscii((byte*) clsiMsgP,
                                      sizeof(TCLSIMsg)));
              error = TRUE;
           }
           break;
           
        case CLS_ID_STN:
           if (indication)
              SapReceive (sapP, SapI_IdStnInd, clsiMsgP);
           else {
              errmsg(&msgsym(CLSIBadIdentifier, DSPU),
              ByteToHexAscii((byte*) clsiMsgP, sizeof(TCLSIMsg)));
              error = TRUE;
           }
           break;

        default:
           errmsg(&msgsym(CLSIBadPrimitiveCmd, DSPU),
                   ByteToHexAscii((byte*) clsiMsgP, sizeof(TCLSIMsg)));
           error = TRUE;
           break;
        }
      }

   if (error)
      CLSIMsgDestroy (clsiMsgP);
         
}  /* ProcessSap. */


/*=====================================================================*
* ProcessAdaptor
*======================================================================*
*
* Description:
*    Decode data before passed to Adaptor.
*    Note that ReceiveAdaptor does not have the class object as a 
* parameter. This is due to the fact the adapter identifier is not at the
* LSA identifier. We therefore let the ReceiveAdaptor to process the
* packet.
* NOTE: AdaptorReceive does not destroy the CLSI message buffer.
*  
**/

static void
ProcessAdaptor (TCLSIMsg *clsiMsgP)

{
   Adaptor* adaptorP;
   hword adaptorIndex;


   adaptorIndex = (hword) CLSIMsgGetCorrelator (clsiMsgP);
   if ((adaptorP = GetChild(GetAdaptorRoot(), adaptorIndex)) != 0)
      AdaptorReceive (adaptorP, clsiMsgP);
   else
      errmsg(&msgsym(CLSIBadIdentifier, DSPU),
              ByteToHexAscii((byte*) clsiMsgP, sizeof(TCLSIMsg)));

   CLSIMsgDestroy (clsiMsgP);

}

