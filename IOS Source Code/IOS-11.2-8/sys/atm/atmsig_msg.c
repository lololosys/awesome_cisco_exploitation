/* $Id: atmsig_msg.c,v 3.3.40.7 1996/07/23 13:20:07 thille Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_msg.c,v $
 *--------------------------------------------------------------------
 *
 * A T M S I G _ M S G . C
 *
 * ATM Signalling (Q.93B) Message support 
 *
 * November 1993,    Percy P. Khabardar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *--------------------------------------------------------------------
 * $Log: atmsig_msg.c,v $
 * Revision 3.3.40.7  1996/07/23  13:20:07  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.40.6  1996/07/10  22:20:22  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.40.5  1996/06/19  02:35:41  jwjang
 * CSCdi60754:  atmsig setup message length is wrong
 * Branch: California_branch
 *
 * Revision 3.3.40.4  1996/06/19  00:56:45  jwjang
 * CSCdi57794:  atmsig setup message for classical ip has different IEs
 * format
 * CSCdi59379
 * Branch: California_branch
 *
 * Revision 3.3.40.3  1996/05/09  14:11:11  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.40.2.14.1  1996/04/27  06:33:34  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.40.2.4.1  1996/04/10  23:35:34  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.40.2  1996/03/29  23:42:28  schrupp
 * CSCdi53085:  ATMSIG  11.0, 11.1 bugfixes for 11.2
 * Branch: California_branch
 * Bugfixes from 11.0, 11.1 into 11.2
 *
 *  CSCdi35689 shut down an ATM ARP
 *  CSCdi46600 Concurrent 1577 and LANE crashes in ATMSIG upon reboot
 *  CSCdi47523 Crash in ATM Signalling code
 *  CSCdi48594 %SUBSYS-2-NOTFOUND: atmsig needs atmilmi on boothelper
 *  images
 *  CSCdi49728 bus error on 7000 running ATM,ATALK,OSPF,SNMP,IPX with 11.0
 *  CSCdi50591 ATMSIG:  release complete uses invalid cause value 80h
 *  CSCdi51037 IBM 25 Mbits ATM-ARP connectivity pb / rfc1755 support
 *  CSCdi52025 router crash at atmSig_callPresentState
 *
 * Revision 3.3.40.1  1996/03/29  19:28:04  grobelch
 *   Provide traffic shaping for SVCs when possible.
 *   Also:
 *   - Specify map-class rate parameters in kbps.
 *   - Provide a per-interface command to indicate
 *     whether SVC should be established if signalled
 *     parameters cannot be installed.
 *   - Don't attach ATM submaps to map-class block
 *   - Check for map-class params prior to using
 *     them (they can be null).
 *   - Fix help-strings for "clp1" parameters to
 *     indicate for clp0&1 flows.
 *   - Signal best effort if only pcr for clp0&1 are
 *     specified.
 *   - map-class parameters override map-list where
 *     they overlap.
 *   - Fix a couple of erroneous variable setting/usages
 *     in static-map handling.
 * CSCdi51550:  ATMSIG: SVCs should attempt to use traffic shaping
 * Branch: California_branch
 *
 * Revision 3.3  1995/12/05  19:46:39  schrupp
 * CSCdi43528:  router fails to respond to status request after clear int
 * Handle RCV_STATUS in NULL state.
 *
 * Revision 3.2  1995/11/17  08:46:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:51:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/23  21:00:15  schrupp
 * CSCdi39217:  ATM Process UCR params in Setup message
 *
 * Revision 2.4  1995/08/18  14:23:10  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.3  1995/08/07  19:14:25  percyk
 * CSCdi38338:  q messages on the correct output process.
 *
 * Revision 2.2  1995/07/08  00:33:12  schrupp
 * CSCdi36706:  Reject ATM CBR Setup with cause QOS unavailable
 *
 * Revision 2.1  1995/06/07  20:11:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *--------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "mgd_timers.h"
#include "address.h"
#include "../if/atm.h"
#include "atmsig_api.h"
#include "atmsig_private.h"
#include "atmsig_public.h"
#include "sscop_private.h"
#include "sscop_public.h"



/*
 * a t m S i g _ f i n d M s g T y p e ( )
 *
 * Validates Message Header and returns message type (op code) of the Signalling 
 * packet received.
 */
atmSig_eventtype atmSig_findMsgType (paktype *pak)
{

   atmSig_msgHeader *hdr;
   int opcode;


   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* check if message is atleast the size of the message header */
   if (pak->datagramsize < ATMSIG_HDRSIZE) {
      if (atmSig_debugAll || atmSig_debugEvent) 
         buginf("\nATMSIG: incoming message rcvd %d smaller than message header %d",
                                         pak->datagramsize, ATMSIG_HDRSIZE);
      return (ATMSIG_RCV_TOOSMALL);
   } 

   /* valid Q.93B protocol discriminator? */
   if (hdr->proto_discr != Q93B_PROTO_DISCR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: incoming msg has invalid protocol discriminator %x",
                                                              hdr->proto_discr);
      return (ATMSIG_RCV_INVPROTODISCR);
   }

   /* valid compatibility instruction indicator? */
   if (hdr->cii != ATMSIG_CII) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: incoming msg has invalid compatibility instr indicator %x",
                                                                            hdr->cii);
      return (ATMSIG_RCV_INVCII);
   }

   /* 
    * The length of the message is embedded in the message (excludes message header),
    * check if packet size matches message size?  If not, ignore pak. 
    */ 
   if ( pak->datagramsize != (ATMSIG_HDRSIZE + GETSHORT(&hdr->length[0])) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf( "\nATMSIG: incoming pak size %d does not match embedded msg size %d",
                  pak->datagramsize, ATMSIG_HDRSIZE + GETSHORT(&hdr->length[0]) ); 
      return (ATMSIG_RCV_INVMSGSIZE);
   }

   /* message header valid, now find message opcode */
   switch (hdr->msgtype) {

      /* Call Proceeding message */
      case ATMSIG_MT_CALL_PROCEEDING:
         opcode = ATMSIG_RCV_CALL_PROCEEDING;
         break;

      /* Connect message */
      case ATMSIG_MT_CONNECT:
         opcode = ATMSIG_RCV_CONNECT;
         break;

      /* Connect Acknowledge message */
      case ATMSIG_MT_CONNECT_ACK:
         opcode = ATMSIG_RCV_CONNECT_ACK;
         break;

      /* Setup message */
      case ATMSIG_MT_SETUP:
         opcode = ATMSIG_RCV_SETUP;
         break;

      /* Release message */
      case ATMSIG_MT_RELEASE:
         opcode = ATMSIG_RCV_RELEASE;
         break;

      /* Release Complete message */
      case ATMSIG_MT_RELEASE_COMPLETE:
         opcode = ATMSIG_RCV_RELEASE_COMPLETE;
         break;

      /* Restart message */
      case ATMSIG_MT_RESTART:
         opcode = ATMSIG_RCV_RESTART;
         break;

      /* Restart Acknowledge message */
      case ATMSIG_MT_RESTART_ACK:
         opcode = ATMSIG_RCV_RESTART_ACK;
         break;

      /* Status message */
      case ATMSIG_MT_STATUS:
         opcode = ATMSIG_RCV_STATUS;
         break;

      /* Status Enquiry message */
      case ATMSIG_MT_STATUS_ENQUIRY:
         opcode = ATMSIG_RCV_STATUS_ENQUIRY;
         break;

      /* Add Party message */
      case ATMSIG_MT_ADD_PARTY:
         opcode = ATMSIG_RCV_ADD_PARTY;
         break;

      /* Add Party Acknowledge message */
      case ATMSIG_MT_ADD_PARTY_ACK:
         opcode = ATMSIG_RCV_ADD_PARTY_ACK;
         break;

      /* Add Party Reject message */
      case ATMSIG_MT_ADD_PARTY_REJ:
         opcode = ATMSIG_RCV_ADD_PARTY_REJ;
         break;

      /* Drop Party message */
      case ATMSIG_MT_DROP_PARTY:
         opcode = ATMSIG_RCV_DROP_PARTY;
         break;

      /* Drop Party Acknowledge message */
      case ATMSIG_MT_DROP_PARTY_ACK:
         opcode = ATMSIG_RCV_DROP_PARTY_ACK;
         break;

      /* unknown message type */
      default:
         opcode = ATMSIG_RCV_BADMT;
         if (atmSig_debugAll || atmSig_debugEvent)
            buginf("\nATMSIG: unknown incoming message %x", opcode);
         break;
   }

   return (opcode);

}



/*
 * a t m S i g _ i s M e s s a g e G l o b a l ( )
 *
 * Only used by Restart messages.  Restart messages can have local or global
 * scope.  This info is needed as global messages use the Global SVC block and
 * local messages use SVC's from the SVC Hash Table. 
 * Restart Indicator IE in Restart Messages tells us if the message applies to
 * all or only one VC. 
 * Returns TRUE if the scope of the message is global, FALSE otherwise.
 */
boolean atmSig_isMessageGlobal (atmSig_msgHeader *hdr)
{

   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;


   /* length of message - excluding message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* 
    * We are only interested in processing the Restart Indicator IE - any
    * other IE is skipped over.
    */
   while (msglength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

         case ATM_RCV_RESTARTINDICATOR:
            frameptr = (uchar *)ieHdr;
            frameptr += ATMSIG_IEHDRSIZE;
            if (*frameptr & ATM_RESTART_CLASS_MASK & ATMSIG_EXTENSION_MASK)
               return (TRUE);
            else 
               return (FALSE);  
            break;

         /* skip over any other IE!! */
         default:
            break;

      }

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugEvent)
            buginf("\nnew length %d of global message is more than total length %d",
                                                             newlength, msglength);
         return(TRUE);
      }      

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;

   }

   /* 
    * Restart Indicator not present in RESTART or RESTART ACK Messages.  Fatal
    * error!!  Treat is as a global message, the Global SVC block will discard
    * the message when it finds out that Restart Indicator is not present.
    */
   return (TRUE);
 

}


 
/*
 * a t m S i g _ p r o c e s s S e t u p M s g ( )
 *
 * Process an incoming SETUP message.  The info learned from the IE's is stored in 
 * the SVC block which could be retrieved by show commands.
 * Returns ATMSIG_MSG_OK if message valid/accepted, error code otherwise.  
 */
int atmSig_processSetupMsg (atmSvcType *svc)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   boolean UserCellRateIe = FALSE;    /* mandatory IE */
   boolean BrodBndBerCapIe = FALSE;   /*     "        */
   boolean CalledNumIe = FALSE;       /*     "        */
   boolean QosIe = FALSE;             /*     "        */
   boolean ConnectionIdIe = FALSE;    /*     "        */
   boolean EndpointRefIe = FALSE;
   int result = ATMSIG_MSG_OK;
   atmSvcPartyType *party;

     
   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: Process Setup Msg, No party");
     svc->freeSvc = TRUE;
     return (ATMSIG_IE_FATAL_ERROR);
   }

   pak = svc->inbuf; 
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of SETUP message - excluding msg header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;
  
   /* Start processing the various IE's in the SETUP message */
   while (newlength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

         case ATM_RCV_AAL:
            result |= atmSig_processAalIe(svc, ieHdr); 
            break;

         /* mandatory IE for SETUP */
         case ATM_RCV_USERCELLRATE:
            result |= atmSig_processUserCellRate(svc, ieHdr); 
            UserCellRateIe = TRUE;
            break;

         /* mandatory IE for SETUP */
         case ATM_RCV_BROADBAND_BEARERCAP:
            result |= atmSig_processBbearerCapability(svc, ieHdr); 
            BrodBndBerCapIe = TRUE;
            break;

         case ATM_RCV_BROADBAND_HIGHLAYER:
            result |= atmSig_processBroadbandHighLayerInfo(svc, ieHdr); 
            break;

         case ATM_RCV_BROADBAND_LOWLAYER:
            result |= atmSig_processBroadbandLowLayerInfo(svc, ieHdr); 
            break;

         /* mandatory IE for SETUP */
         case ATM_RCV_CALLEDPARTY_NUMBER:
            result |= atmSig_processCalledPartyNumber(svc, ieHdr); 
            CalledNumIe = TRUE;
            break;

         case ATM_RCV_CALLEDPARTY_SUBADDR:
            result |= atmSig_processCalledPartySubAddr(svc, ieHdr); 
            break;

         case ATM_RCV_CALLINGPARTY_NUMBER:
            result |= atmSig_processCallingPartyNumber(svc, ieHdr, party); 
            break;

         case ATM_RCV_CALLINGPARTY_SUBADDR:
            result |= atmSig_processCallingPartySubAddr(svc, ieHdr, party);
            break;

         case ATM_RCV_CONNECTIONIDENTIFIER:
            result |= atmSig_processConnectionIdentifier(svc, ieHdr);
            ConnectionIdIe = TRUE; 
            break;

         /* mandatory IE for SETUP */
         case ATM_RCV_QOS:
            result |= atmSig_processQosParameter(svc, ieHdr);
            QosIe = TRUE;
            break;

         case ATM_RCV_BROADBAND_SENDINGCMPLT:
            result |= atmSig_processBroadbandSendCmplt(svc, ieHdr);
            break;

         case ATM_RCV_TRANSIT_NTWRKSEL:
            result |= atmSig_processTransitNtwrkSel(svc, ieHdr);
            break;

         /* mandatory IE for multipoint */
         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
            EndpointRefIe = TRUE;
            break;

         case ATM_RCV_REPEATINDICATOR_IE:
            result |= atmSig_processBbandRepeatIndicator(svc, ieHdr);
            break;

         /* IE not required for SETUP message - skip it over!! */       
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            if (atmSig_debugAll || atmSig_debugEvent)
               buginf("\nunknown ie %x rcvd in Setup msg", ieOpCode);
            break;
      }

      /* decrement Message length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nnew length %d of Setup message is more than total length %d", 
                                                             newlength, msglength);
         return(ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE; 
   }


   /* All Mandatory IE's present in SETUP? */
   if (!UserCellRateIe || !BrodBndBerCapIe || !CalledNumIe || !QosIe || 
                                                               !ConnectionIdIe) {
      if (atmSig_debugAll || atmSig_debugEvent) {
         if (!UserCellRateIe)
            buginf("\nATMSIG: mandatory User Cell Rate IE missing in Setup message");
         if (!BrodBndBerCapIe)
            buginf("\nATMSIG: mandatory Broadband Bearer Cap IE missing in Setup msg");
         if (!CalledNumIe)
            buginf("\nATMSIG: mandatory Called Party IE missing in Setup message");
         if (!QosIe)
            buginf("\nATMSIG: mandatory Quality of Service IE missing in Setup msg");
         if (!ConnectionIdIe)
            buginf("\nATMSIG: mandatory Connection Id IE missing in Setup message");
      } 
      result |= ATMSIG_MANDATORY_IE_MISSING;
   }

   /* Now take action if Message contains any errors!! */

   if (result & ATMSIG_IE_VPCI_REUSED) {
     result |= ATMSIG_IE_FATAL_ERROR; /* so that svc will be freed later */
     atmSig_txReleaseCompleteMsg(svc,ATM_CAUSE_VPCI_UNAVAIL);
     return (result);
   }

   /*
    * If a SETUP message is received which has one or more mandatory IE's
    * missing, a RELEASE COMPLETE message with cause "mandatory IE is missing"
    * shall be returned.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in Setup message");
      atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * If a SETUP message is received which has one or more mandatory IE's
    * with invalid content, a RELEASE COMPLETE message with cause "invalid IE
    * contents" shall be returned.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in Setup message");
      atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a SETUP message is received which has one or more non-mandatory IE's
    * with invalid content, a STATUS message with cause "invalid IE contents"
    * shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in Setup message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*  
    * If SETUP is for Multipoint, need EndpointRefIe
    * Need to check usercellrate bkwd params zero
    *
    */
   if ( (EndpointRefIe) &&
       ( (svc->tos.usrPlaneAccessConfig & ATM_UPAC_MASK) != ATMSIG_MULTIPOINT ) ) {
     result |= ATMSIG_IE_FATAL_ERROR;       
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG: invalid user plane config in multipoint Setup message");
     atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
     return (result);
   }

   if ( (svc->tos.usrPlaneAccessConfig & ATM_UPAC_MASK) == ATMSIG_MULTIPOINT ) {

     if (!EndpointRefIe) {
       result |= ATMSIG_MANDATORY_IE_MISSING;       
       if (atmSig_debugAll || atmSig_debugEvent)
	 buginf("\nATMSIG: mandatory Endpoint Ref Id IE missing in Setup message");
       atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_MANDATORY_IE_MISSING);
       return (result);
     }

     if ( svc->tos.userCellRate.bwdPeakClp01) {
       result |= ATMSIG_IE_FATAL_ERROR;       
       if (atmSig_debugAll || atmSig_debugEvent)
	 buginf("\nATMSIG: non-zero Bkwd UCR in multipoint Setup message");
       atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_UNSUPPORTED_TRAFFIC_PARAMS);
       return (result);
     }

     svc->multiPoint = TRUE;
   }


   return (result);

}



/*
 * a t m S i g _ p r o c e s s C a l l P r o c e e d i n g M s g ( )
 *
 * Process an incoming Call Proceeding message.  The info learned from the IE's 
 * is stored in the SVC block which could be retrieved by show commands.
 * Returns ATMSIG_MSG_OK if message is valid/accepted, error code otherwise.
 */
int atmSig_processCallProceedingMsg (atmSvcType *svc)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   int result = ATMSIG_MSG_OK;
   boolean ConnectionIdPresent = FALSE;         /* mandatory IE */
   boolean EndpointRefIe = FALSE;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: Process Call Proc Msg, No party");
     svc->freeSvc = TRUE;
     return (ATMSIG_IE_FATAL_ERROR);
   }

   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of CALL PROCEEDING message - excluding message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the CALL PROCEEDING message */
   while (newlength) {
   
      ieHdr = (atmSig_ieHeader *)frameptr;    
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

         case ATM_RCV_CONNECTIONIDENTIFIER:
            result |= atmSig_processConnectionIdentifier(svc, ieHdr);
            ConnectionIdPresent = TRUE;
            break;

         /* mandatory IE for multipoint */
         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
            EndpointRefIe = TRUE;
            break;

         /* IE not required for CALL PROCEEDING - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;          

      }

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError) 
            buginf("\nATMSIG: new length %d of Call Proceeding message is more than total length %d", 
                                                           newlength, msglength); 
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;
   }

   if (!ConnectionIdPresent) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory Connection Id IE missing in Call Proceeding msg");
      result |= ATMSIG_MANDATORY_IE_MISSING;
   }

   if (result & ATMSIG_IE_VPCI_REUSED) {
     result |= ATMSIG_IE_FATAL_ERROR; /* so that svc will be freed later */
     atmSig_txReleaseCompleteMsg(svc,ATM_CAUSE_VPCI_UNAVAIL);
     return (result);
   }

   /* If SETUP is for Multipoint, need EndpointRefIe */
   if ( (svc->multiPoint) && (!EndpointRefIe) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory EndpointRef IE missing in Call Proceeding msg");
      result |= ATMSIG_MANDATORY_IE_MISSING;       
    }

   if ( (svc->multiPoint) && (svc->msgEndpointRef != svc->rootParty->endpointRef) ) {
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG: invalid EndpointRef IE in Call Proceeding msg");
     result |= ATMSIG_IE_FATAL_ERROR;
   }

   /* Now take action if Message contains any errors */

   /*
    * CALL PROCEEDING message is received which has one or more mandatory IE's
    * missing, a STATUS message with cause "mandatory IE is missing" shall be
    * returned.  No action should be taken.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in Call Proceeding message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * CALL PROCEEDING message is received which has one or more mandatory IE's
    * with invalid content, a STATUS message with cause "invalid IE contents"
    * shall be returned.  No action should be taken.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in Call Proc message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * CALL PROCEEDING message is received which has a non-mandatory IE with
    * invalid content, a STATUS message with cause "invalid IE contents"
    * shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in Call Proc message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * CALL PROCEEDING message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: Call Proceeding message rcvd with unrecognized IE");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result);

}



/*
 * a t m S i g _ p r o c e s s C o n n e c t M s g ( )
 *
 * Process an incoming CONNECT message.  The info learned from the IE's is 
 * stored in the SVC block which could be retrieved by show commands.
 * Returns ATMSIG_MSG_OK if message is valid/accepted, error code otherwise.
 */
int atmSig_processConnectMsg (atmSvcType *svc)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   int result = ATMSIG_MSG_OK;
   boolean EndpointRefIe = FALSE;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: Process Conn Msg, No party");
     svc->freeSvc = TRUE;
     return (ATMSIG_IE_FATAL_ERROR);
   }

   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of CONNECT message - excluding message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the CONNECT message */
   while (newlength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

         case ATM_RCV_AAL:
            result |= atmSig_processAalIe(svc, ieHdr);
            break;

         case ATM_RCV_CONNECTIONIDENTIFIER:
            result |= atmSig_processConnectionIdentifier(svc, ieHdr);
            break;

         case ATM_RCV_BROADBAND_LOWLAYER:
            result |= atmSig_processBroadbandLowLayerInfo(svc, ieHdr);
            break; 

         /* mandatory IE for multipoint */
         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
            EndpointRefIe = TRUE;
            break;

         /* IE not required for CONNECT message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;

      } 

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new lngth %d of Connect msg is more than total lngth %d", 
                                                            newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;
   }

   if (result & ATMSIG_IE_VPCI_REUSED) {
     result |= ATMSIG_IE_FATAL_ERROR; /* so that svc will be freed later */
     atmSig_txReleaseCompleteMsg(svc,ATM_CAUSE_VPCI_UNAVAIL);
     return (result);
   }

   /* If SETUP is for Multipoint, need EndpointRefIe */
   if ( (svc->multiPoint) && (!EndpointRefIe) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory EndpointRef IE missing in Connect msg");
      result |= ATMSIG_MANDATORY_IE_MISSING;       
    }

   if ( (svc->multiPoint) && (svc->msgEndpointRef != svc->rootParty->endpointRef) ) {
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG: invalid EndpointRef IE in Connect msg");
     result |= ATMSIG_IE_FATAL_ERROR;
   }

   /* Now take action if Message contains any errors */

   /*
    * CONNECT message received which has one or more mandatory IE's missing,
    * a STATUS message with cause "mandatory IE is missing" shall be returned.
    * No action should be taken.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory IE missing in Connect message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * CONNECT message is received which has one or more mandatory IE's with
    * invalid content, a STATUS message with cause "invalid IE contents" shall
    * be returned.  No action should be taken.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory IE with invalid content in Connect message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * CONNECT message is received which has a non-mandatory IE with invalid
    * content, a STATUS message with cause "invalid IE contents" shall be retunred.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory IE with invalid content in Connect message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * CONNECT message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: Connect message received with unrecognized ie");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result);

}



/*
 * a t m S i g _ p r o c e s s C o n n e c t A c k M s g ( )
 *
 * Process an incoming CONNECT ACK message.  The info learned from the IE's is 
 * stored in the SVC block which could be retrieved by show commands.
 * Returns ATMSIG_MSG_OK if message is valid/accepted, error code otherwise.
 */
int atmSig_processConnectAckMsg (atmSvcType *svc)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   int result;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: Process Conn Ack Msg, No party");
     svc->freeSvc = TRUE;
     return (ATMSIG_IE_FATAL_ERROR);
   }

   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* CONNECT ACK msg contains only msg header, no IE's, i.e. hdr->length = 0  */
   if ( GETSHORT(&hdr->length[0]) )
      result = ATMSIG_MSG_INVALID;
   else
      result = ATMSIG_MSG_OK;

   /* Now take action if Message contains any errors */

   /*
    * CONNECT ACK message is received which has an IE, a STATUS message with
    * cause "invalid IE contents" shall be returned.  No action should be taken
    * on the message.
    */
   if (result != ATMSIG_MSG_OK) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: Connect Ack message received with an IE"); 
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
   }

   return (result);

}



/*
 * a t m S i g _ p r o c e s s R e l e a s e M s g ( )
 *
 * Process an incoming RELEASE message.  The info learned from the IE's is 
 * stored in the SVC block which could be retrieved by show commands.
 * Returns ATMSIG_MSG_OK if message is valid/accepted, error code otherwise.
 */
int atmSig_processReleaseMsg (atmSvcType *svc)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   int result = ATMSIG_MSG_OK;
   boolean CauseIePresent = FALSE;         /* mandatory IE */
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: Process Release Msg, No party");
     svc->freeSvc = TRUE;
     return (ATMSIG_IE_FATAL_ERROR);
   }

   pak = (paktype *)svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of RELEASE message - excluding the message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the RELEASE message */
   while (newlength) {
      
      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

         case ATM_RCV_CAUSE:
            result |= atmSig_processCause(svc, ieHdr);
            CauseIePresent = TRUE;
            break;

         /* IE not required for RELEASE message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;

      }

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new lngth %d of Release msg is more than total lngth %d", 
                                                              newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;
 
   }

   if (!CauseIePresent)
      result |= ATMSIG_MANDATORY_IE_MISSING;

   /* Now take action if Message contains any errors */

   /*
    * If a RELEASE message is received which has one or more mandatory IE's
    * missing, a RELEASE COMPLETE message with cause "mandatory IE is missing"
    * shall be returned.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in Release message");
      atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * If a RELEASE message is received which has one or more mandatory IE's
    * with invalid content, a RELEASE COMPLETE message with cause "invalid IE
    * contents" shall be returned.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in Release message");
      atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a RELEASE message is received which has one or more non-mandatory IE's
    * with invalid content, a STATUS message with cause "invalid IE contents"
    * shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in Release message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a RELEASE message is received which has an unrecognized IE, a RELEASE 
    * COMPLETE with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: Release message received with unrecognized IE");
      atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result); 

}



/*
 * a t m S i g _ p r o c e s s R e l e a s e C o m p l e t e M s g ( )
 *
 * Process an incoming RELEASE COMPLETE message.  The info learned from the IE's
 * stored in the SVC block which could be retrieved by show commands.
 * Returns ATMSIG_MSG_OK if message is valid/accepted, error code otherwise.
 */
int atmSig_processReleaseCompleteMsg (atmSvcType *svc)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   int result = ATMSIG_MSG_OK;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: Process Release Complete Msg, No party");
     svc->freeSvc = TRUE;
     return (ATMSIG_IE_FATAL_ERROR);
   }

   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of RELEASE COMPLETE message - excluding the message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the SETUP message */
   while (newlength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

         case ATM_RCV_CAUSE:
            result |= atmSig_processCause(svc, ieHdr);
            break;

         /* IE not required for RELEASE COMPLETE message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;

      }       

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new lngth %d of Rel Compl is more than total lngth %d", 
                                                                 newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;

   }

   return (result); 

}



/*
 * a t m S i g _ p r o c e s s S t a t u s M s g ( )
 *
 * Process an incoming STATUS message.  The info learned from the IE's is
 * stored in the SVC block which could be retrieved by show commands.
 * Returns ATMSIG_MSG_OK if message is valid/accepted, error code otherwise.
 */
int atmSig_processStatusMsg (atmSvcType *svc, int *callState, int *endpointState)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   int result = ATMSIG_MSG_OK;
   boolean CallStateIePresent = FALSE;           /* mandatory IE */
   boolean CauseIePresent = FALSE;               /*      "       */
   atmSvcPartyType *party;
   boolean EndrefIePresent = FALSE;
   boolean EndstatIePresent = FALSE;

   svc->statusEnquiry = FALSE;
   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: Process Status Msg, No party");
     svc->freeSvc = TRUE;
     return (ATMSIG_IE_FATAL_ERROR);
   }

   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of STATUS message - excluding message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the STATUS message */
   while (newlength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

         /* mandatory IE */
         case ATM_RCV_CALLSTATE:
            result |= atmSig_processCallState(svc, ieHdr, callState);
            CallStateIePresent = TRUE;
            break;

         /* mandatory IE */
         case ATM_RCV_CAUSE:
            result |= atmSig_processCause(svc, ieHdr);
            CauseIePresent = TRUE;
            break;

         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
	    EndrefIePresent = TRUE;
            break;

         case ATM_RCV_ENDPOINT_STATE:
            result |= atmSig_processEndPointState(svc, ieHdr, party);
	    EndstatIePresent = TRUE;
            break;         

         /* IE not required for STATUS message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;

      }

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new length %d of Status msg is more than total length %d", 
                                                              newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;
   }

   if ( (svc->multiPoint && (!EndrefIePresent || !EndstatIePresent)) ||
       !CallStateIePresent || !CauseIePresent ) {
     if (atmSig_debugEvent)
       buginf("\nATMSIG: mandatory ie missing in Status message");

     result |= ATMSIG_MANDATORY_IE_MISSING;
   }

   /* Now take action if Message contains any errors */

   /*
    * STATUS message received which has one or more mandatory IE's missing,
    * a STATUS message with cause "mandatory IE is missing" shall be returned.
    * No action should be taken.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in Status message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * STATUS message is received which has one or more mandatory IE's with
    * invalid content, a STATUS message with cause "invalid IE contents" shall
    * be returned.  No action should be taken.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in Status message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * STATUS message is received which has a non-mandatory IE with invalid
    * content, a STATUS message with cause "invalid IE contents" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in Status message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * STATUS message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: Status message received with unrecognized ie");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result);

}



/*
 * a t m S i g _ p r o c e s s S t a t u s E n q u i r y M s g ( )
 *
 * Process an incoming STATUS ENQUIRY message.  The info learned from the IE's is 
 * stored in the SVC block which could be retrieved by show commands.
 * Returns ATMSIG_MSG_OK if message is valid/accepted, error otherwise.
 */
int atmSig_processStatusEnquiryMsg (atmSvcType *svc)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   int result = ATMSIG_MSG_OK;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: Process Status Enq Msg, No party");
     svc->freeSvc = TRUE;
     return (ATMSIG_IE_FATAL_ERROR);
   }

   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of STATUS ENQUIRY message - excludes message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the STATUS ENQUIRY message */
   while (newlength) {
       
      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
            break;

         /* IE not required for STATUS ENQUIRY message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;
       
      }

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new lngth %d of Status Enq is more than total lngth %d", 
                                                                newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;
   }

   /* Now take action if Message contains any errors */

   /*
    * STATUS ENQ message received which has one or more mandatory IE's missing,
    * a STATUS message with cause "mandatory IE is missing" shall be returned.
    * No action should be taken.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in Status Enquiry message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * STATUS ENQ message is received which has one or more mandatory IE's with
    * invalid content, a STATUS message with cause "invalid IE contents" shall
    * be returned.  No action should be taken.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in Status Enquiry message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * STATUS ENQ message is received which has a non-mandatory IE with invalid
    * content, a STATUS message with cause "invalid IE contents" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in Status Enquiry msg");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * STATUS ENQ message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: Status Enquiry message received with unrecognized ie");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result);

}



/*
 * a t m S i g _ p r o c e s s R e s t a r t M s g ( )
 *
 * Process an incoming RESTART message.  The info learned from the IE's is stored  
 * in the SVC block which could be retrieved by show commands.
 * Returns ATMSIG_MSG_OK if message is valid/accepted, error code otherwise.
 */
int atmSig_processRestartMsg (atmSvcType *svc)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *IeHdr;
   ushort msglength, newlength, IeOpCode;
   uchar *frameptr;
   boolean RestartIndicatorIe = FALSE;     /* mandatory IE */
   int result = ATMSIG_MSG_OK;


   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of RESTART message - excludes message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the RESTART message */
   while (newlength) {

      IeHdr = (atmSig_ieHeader *)frameptr;
      IeOpCode = atmSig_findIeOpCode(IeHdr);

      switch (IeOpCode) {

         case ATM_RCV_CONNECTIONIDENTIFIER:
            result |= atmSig_processConnectionIdentifier(svc, IeHdr);
            break;

         /* mandatory IE */
         case ATM_RCV_RESTARTINDICATOR:
            result |= atmSig_processRestartIndicator(svc, IeHdr);
            RestartIndicatorIe = TRUE;
            break;

         /* IE not required for RESTART message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;

      }

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&IeHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new lngth %d of Release msg is more than total lngth %d", 
                                                             newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)IeHdr;
      frameptr += GETSHORT(&IeHdr->length[0]) + ATMSIG_IEHDRSIZE;

   }

   /* mandatory IE present? */
   if (!RestartIndicatorIe)
      if (atmSig_debugAll || atmSig_debugEvent) {
         buginf("\nATMSIG: mandatory Restart Indicator IE missing in Restart message");
      result |= ATMSIG_MANDATORY_IE_MISSING;
   }

   /* Now take action if Message contains any errors */

   /*
    * RESTART message received which has one or more mandatory IE's missing,
    * a STATUS message with cause "mandatory IE is missing" shall be returned.
    * No action should be taken.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in Restart message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * RESTART message is received which has one or more mandatory IE's with
    * invalid content, a STATUS message with cause "invalid IE contents" shall
    * be returned.  No action should be taken.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in Restart message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * RESTART message is received which has a non-mandatory IE with invalid
    * content, a STATUS message with cause "invalid IE contents" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in Restart message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * RESTART message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: Restart message with unrecognized ie");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result);

}



/*
 * a t m S i g _ p r o c e s s R e s t a r t A c k M s g ( )
 *
 * Process an incoming RESTART ACK message.  The info learned from the IE's is
 * stored in the SVC block which could be retrieved by show commands.
 * Returns ATMSIG_MSG_OK if message is valid/accepted, error code otherwise.
 */
int atmSig_processRestartAckMsg (atmSvcType *svc)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *IeHdr;
   ushort msglength, newlength, IeOpCode;
   uchar *frameptr;
   int result = ATMSIG_MSG_OK;


   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of RESTART ACK message - excludes message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the RESTART ACK message */
   while (newlength) {

      IeHdr = (atmSig_ieHeader *)frameptr;
      IeOpCode = atmSig_findIeOpCode(IeHdr);

      switch (IeOpCode) {

         case ATM_RCV_CONNECTIONIDENTIFIER:
            result |= atmSig_processConnectionIdentifier(svc, IeHdr);
            break;

         case ATM_RCV_RESTARTINDICATOR:
            result |= atmSig_processRestartIndicator(svc, IeHdr);
            break;

         /* IE not required for RESTART ACK message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED; 
            break;

      }

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&IeHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new lngth %d of Restart Ack is more than total lngth %d", 
                                                              newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)IeHdr;
      frameptr += GETSHORT(&IeHdr->length[0]) + ATMSIG_IEHDRSIZE;

   }

   /* Now take action if Message contains any errors */

   /*
    * RESTART ACK message received which has one or more mandatory IE's missing,
    * a STATUS message with cause "mandatory IE is missing" shall be returned.
    * No action should be taken.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in Restart Ack message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * RESTART ACK message is received which has one or more mandatory IE's with
    * invalid content, a STATUS message with cause "invalid IE contents" shall
    * be returned.  No action should be taken.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in Restart Ack message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * RESTART ACK message is received which has a non-mandatory IE with invalid
    * content, a STATUS message with cause "invalid IE contents" shall be retunred.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in Restart Ack msg");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * RESTART ACK message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: Restart Ack message with unrecognized ie");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result);

}



/*
 * a t m S i g _ b u i l d M s g H e a d e r ( )
 *
 * Message Header is odd byte in length - must use PUTSHORT, GETSHORT, PUTLONG,
 * and GETLONG to avoid pad bytes getting into transmit frames. 
 */
void atmSig_buildMsgHeader (atmSvcType *svc, uchar *frameptr, uint opcode, uint size)
{

   atmSig_msgHeader *hdr;


   hdr = (atmSig_msgHeader *)frameptr;

   /* fill in protocol discriminator */
   hdr->proto_discr = Q93B_PROTO_DISCR;

   /* fill in call reference values - call ref length, flag & value */
   if (svc->locallyInitiatedCall)
      PUTLONG(&hdr->callref[0], ATMSIG_CALLREF_LNGTH | svc->callRef);
   else
      PUTLONG(&hdr->callref[0], ATMSIG_CALLREF_LNGTH | svc->callRef |
                                                          ATMSIG_CALLREF_FLAG); 

   /* fill in opcode */
   hdr->msgtype = (uchar)opcode;

   /* fill in compatibility instruction indicator */
   hdr->cii = ATMSIG_CII;

   /*
    * fill in message length. 
    * 'size' is the length of the message excluding the message header length. 
    * The calling function must ensure that 'size' does not include the length
    * of the message header.
    */ 
   PUTSHORT(&hdr->length[0], size); 

}


/*
 * a t m S i g _ u p d a t e D e f a u l t C e l l R a t e s ( )
 *
 * If all cell-rate parameters are default, reflect what we signal as
 * default.
 */
static void atmSig_updateDefaultCellRates (atmSvcType *svc)
{
   boolean allDefault = TRUE;
   userCellRateType *ucr;

   ucr = &(svc->tos.userCellRate);
 
   if (ucr->fwdPeakClp0 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
      allDefault = FALSE;
 
   if (ucr->bwdPeakClp0 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
      allDefault = FALSE;
 
   if (ucr->fwdPeakClp01 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
      allDefault = FALSE;
 
   if (ucr->bwdPeakClp01 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
      allDefault = FALSE;
 
   if (ucr->fwdSusClp0 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
      allDefault = FALSE;
 
   if (ucr->bwdSusClp0 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
      allDefault = FALSE;
 
   if (ucr->fwdSusClp01 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
      allDefault = FALSE;
 
   if (ucr->bwdSusClp01 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
      allDefault = FALSE;
 
   if (ucr->fwdMaxBurstClp0 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
      allDefault = FALSE;
 
   if (ucr->bwdMaxBurstClp01 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
      allDefault = FALSE;
 
   if ( (ucr->FwdTag != ATMSIG_TRAFFICPARM_NOT_DEFINED_API) ||
        (ucr->BwdTag != ATMSIG_TRAFFICPARM_NOT_DEFINED_API) )
      allDefault = FALSE;
 
   if (allDefault) { /* all parameters defaulted, substitute signalled pcr01 */
      ucr->fwdPeakClp01 = ATMSIG_PCR01_NOT_DEFINED_IN_SETUP_ALL_1;
      ucr->bwdPeakClp01 = ATMSIG_PCR01_NOT_DEFINED_IN_SETUP_ALL_1;
   }
}


/*
 * a t m S i g _ t x S e t u p M s g ( )
 *
 * Builds and sends a SETUP message to SSCOP.  Returns FALSE if buffer unavailable.
 */
boolean atmSig_txSetupMsg (atmSvcType *svc)
{

   uchar *frameptr;
   paktype *pak;
   int size, pad;
   int ucr_size, ll_size;
   atmSvcPartyType *party;
   ushort aal5IeSize = 0;

   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: txSetupMsg, No party");
     svc->freeSvc = TRUE;
     return (FALSE);
   }

   size = 0;   /* size ==> length of entire SETUP msg (including msg header) */

   /*
    * first, update cell-rate parameters if they are all don't care,
    * to indicate default signalled parameters.
    * size up the User Cell Rate IE (mandatory IE) which is variable length 
    * depending on the number of parameters the user configures.
    */
   atmSig_updateDefaultCellRates(svc);
   ucr_size = atmSig_sizeupUserCellRate(svc);

   /*
    * size up Lower Layer IE (optional IE) which is variable length depending
    * on if the vc is mux and/or layer 2 id is to be included.  Even though the
    * IE is optional we include it.
    */
   ll_size = svc->blli[0].length;

   /* sizeup the entire SETUP message - first the mandatory IE's */
   size = ATMSIG_HDRSIZE + ATMSIG_BBEARERCAP_IESIZE +
          ATMSIG_CALLEDPARTY_IESIZE + ATMSIG_QOS_IESIZE + ucr_size +
          (4 * ATMSIG_IEHDRSIZE);

   /* Calling Party IE is optional - client may not want to send it */
   if (svc->srcNsap.length > 0)
      size += ATMSIG_IEHDRSIZE + ATMSIG_CALLINGPARTY_IESIZE;

   /* optional IE - but we send it! */
   if (svc->tos.aalType == ATM_AALIE_TYPE34)
      size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE34_IESIZE;
   else {
      if (svc->tos.aalType == ATM_AALIE_TYPE5) {
	if ( atmSig_isV31(svc) )
         size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE5_IESIZE_V31;
       else {
	 /* this is really ugly!! */
	 if (svc->tos.mode !=  ATM_AALIE_MODE_INVALID) 
	   size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE5_IESIZE;
	 else
	   size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE5_IESIZE - 2;
       }
      }
   }

   /* LL IE (optional) to be included? */
   if (ll_size)
      size += ATMSIG_IEHDRSIZE + ll_size;

   /* multipoint call? if so, add End Point Ref IE size */
   if (svc->multiPoint) 
     size += ATMSIG_IEHDRSIZE + ATMSIG_ENDPOINT_REF_IESIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0;

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet for sscop headers cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header first */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_SETUP, 
                                   size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);


   /* Build User Cell Rate IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildUserCellRate(svc, frameptr, ucr_size);

   /* Build Broadband Bearer Capability IE */
   frameptr += ATMSIG_IEHDRSIZE + ucr_size;
   atmSig_buildBBearerCapability(svc, frameptr);

   /* Build Called Party Number IE */
   frameptr += ATMSIG_IEHDRSIZE + ATMSIG_BBEARERCAP_IESIZE;
   atmSig_buildCalledPartyNumber(svc, frameptr, party);

   /* Build Calling Party Number IE ONLY if client wants to */
   frameptr += ATMSIG_IEHDRSIZE + ATMSIG_CALLEDPARTY_IESIZE;
   if (svc->srcNsap.length > 0) {
      atmSig_buildCallingPartyNumber(svc, frameptr);
      frameptr += ATMSIG_IEHDRSIZE + ATMSIG_CALLINGPARTY_IESIZE;
   }

   /* Build QOS Parameter IE */
   atmSig_buildQosParameter(svc, frameptr);

   /* If multipoint, Build the EndPoint Ref IE */
   if (svc->multiPoint) {
     frameptr += ATMSIG_IEHDRSIZE + ATMSIG_QOS_IESIZE;
     atmSig_buildEndpointReference(svc, frameptr, party);
   }

   /* Bump frameptr depending on multipoint */
   if (svc->multiPoint)
     frameptr += ATMSIG_IEHDRSIZE + ATMSIG_ENDPOINT_REF_IESIZE;
   else
     frameptr += ATMSIG_IEHDRSIZE + ATMSIG_QOS_IESIZE;

   /* Build AAL IE */
   if (svc->tos.aalType == ATM_AALIE_TYPE34) {
     atmSig_buildAalType34(svc, frameptr); 
     frameptr += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE34_IESIZE;
   }
   else if (svc->tos.aalType == ATM_AALIE_TYPE5) {
     aal5IeSize = atmSig_buildAalType5(svc, frameptr); 
     frameptr += ATMSIG_IEHDRSIZE + aal5IeSize;
   }
  
   /* Build Broadband Lower Layer IE for mux vc's */
   if (ll_size) {
     atmSig_buildBBandLowLayerInfo(svc, frameptr); 
     frameptr += ATMSIG_HDRSIZE + ll_size;
   }

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o Setup msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(svc->state), pak->datagramsize, svc->callRef, pad); 

   /* send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}



/*
 * a t m S i g _ t x C a l l P r o c e e d i n g M s g ( )
 *
 * Builds and sends CALL PROCEEDING Message to SSCOP.  Returns FALSE if buffer 
 * unavailable. 
 */
boolean atmSig_txCallProceedingMsg (atmSvcType *svc)
{

   uchar *frameptr;
   paktype *pak;
   uint size, pad;
   atmSvcPartyType *party;

   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: tx Call Proc Msg, No party");
     svc->freeSvc = TRUE;
     return (FALSE);
   }

   /* sizeup the entire CALL PROCEEDING message - first the mandatory IE */
   size = ATMSIG_HDRSIZE + ATMSIG_IEHDRSIZE + ATMSIG_CONNECTIONID_IESIZE;

   /* check for multipoint, if so, add ATMSIG_ENDPOINT_REF_IESIZE */
   if (svc->multiPoint) 
     size += ATMSIG_IEHDRSIZE + ATMSIG_ENDPOINT_REF_IESIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header first */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_CALL_PROCEEDING, 
                                  size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);

   /* Build the Connection Identifier IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildConnectionId(svc, frameptr);

   /* If multipoint, Build the EndPoint Ref IE */
   if (svc->multiPoint) {
     frameptr += ATMSIG_IEHDRSIZE + ATMSIG_CONNECTIONID_IESIZE;
     atmSig_buildEndpointReference(svc, frameptr, party);
   }

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o Call Proc msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(svc->state), pak->datagramsize, svc->callRef, pad);

   /* send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}



/*
 * a t m S i g _ t x C o n n e c t M s g ( ) 
 *
 * Builds and send CONNECT Message to SSCOP.  Returns FALSE if buffer unavailable.
 */
boolean atmSig_txConnectMsg (atmSvcType *svc)
{

   uchar *frameptr;
   paktype *pak;
   uint size, pad, aal5IeSize;
   atmSvcPartyType *party;
   

   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: tx Connect Msg, No party");
     svc->freeSvc = TRUE;
     return (FALSE);
   }

   /* sizeup the entire CONNECT message - first the mandatory IE */
   size = ATMSIG_HDRSIZE + ATMSIG_IEHDRSIZE + ATMSIG_CONNECTIONID_IESIZE;

   if (!svc->multiPoint) {
     /*
      * aalParam ie should not be present if we have
      * non-zero endpoing ref value in received setup.
      * Otherwise, send the aalparam ie if we received
      * it in the setup message.
      */
     if (svc->tos.aalType == ATM_AALIE_TYPE34)
        size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE34_IESIZE;
     else if (svc->tos.aalType == ATM_AALIE_TYPE5) {
       if ( atmSig_isV31(svc) )
	 size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE5_IESIZE_V31;
       else {
	 /* this is really ugly!! */
	 if (svc->tos.mode !=  ATM_AALIE_MODE_INVALID) 
	   size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE5_IESIZE;
	 else
	   size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE5_IESIZE - 2;
       }
     }
   }

   /* should blli ie be sent? */
   if (svc->blli[0].length)
      size += ATMSIG_IEHDRSIZE + svc->blli[0].length; 
  
   /* check for multipoint, if so, add ATMSIG_ENDPOINT_REF_IESIZE */
   if (svc->multiPoint) 
     size += ATMSIG_IEHDRSIZE + ATMSIG_ENDPOINT_REF_IESIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_CONNECT, 
                                   size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);
   frameptr += ATMSIG_HDRSIZE;

   /* Build the Connection Identifier IE */
   atmSig_buildConnectionId(svc, frameptr);
   frameptr += ATMSIG_IEHDRSIZE + ATMSIG_CONNECTIONID_IESIZE; 

   if (!svc->multiPoint) {
      if (svc->tos.aalType == ATM_AALIE_TYPE34) {
         atmSig_buildAalType34(svc, frameptr);
         frameptr += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE34_IESIZE;
      } else if (svc->tos.aalType == ATM_AALIE_TYPE5) {
         aal5IeSize = atmSig_buildAalType5(svc, frameptr);
         frameptr += ATMSIG_IEHDRSIZE + aal5IeSize;
      }
   }

   /* Build BLL-IE if needed */
   if (svc->blli[0].length) {
      atmSig_buildBBandLowLayerInfo(svc, frameptr);
      frameptr += ATMSIG_IEHDRSIZE + svc->blli[0].length;
   }

   /* If multipoint, Build the EndPoint Ref IE */
   if (svc->multiPoint) {
     atmSig_buildEndpointReference(svc, frameptr, party);
   }

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o Connect msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(svc->state), pak->datagramsize, svc->callRef, pad);

   /* Transmit it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}



/*
 * a t m S i g _ t x C o n n e c t A c k M s g ( )
 *
 * Builds and sends CONNECT ACK Message to SSCOP.  Returns FALSE if buffer 
 * unavailable.
 */
boolean atmSig_txConnectAckMsg (atmSvcType *svc)
{

   uchar *frameptr;
   paktype *pak;
   uint size, pad;


   /* sizeup the entire CONNECT ACK message - no IE's */
   size = ATMSIG_HDRSIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_CONNECT_ACK, 0);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o Connect Ack msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(svc->state), pak->datagramsize, svc->callRef, pad);

   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);
 
   return (TRUE);

}



/*
 * a t m S i g _ t x R e l e a s e M s g ( )
 *
 * Builds and sends RELEASE Message to SSCOP.  Returns FALSE if buffer unavailable.
 */
boolean atmSig_txReleaseMsg (atmSvcType *svc, uchar cause)
{

   uchar *frameptr;
   paktype *pak;
   uint size, pad;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: tx Release Msg, No party");
     svc->freeSvc = TRUE;
     return (FALSE);
   }

   
   /* sizeup the entire RELEASE message - one mandatory IE */
   size = ATMSIG_HDRSIZE + ATMSIG_IEHDRSIZE + ATMSIG_CAUSE_IESIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_RELEASE, 
                                  size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);

   /* Build the Cause IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildCause(svc, frameptr, cause);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o Release msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(svc->state), pak->datagramsize, svc->callRef, pad);

   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}



/*
 * a t m S i g _ t x R e l e a s e C o m p l e t e M s g ( )
 *
 * Builds and sends RELEASE COMPLETE Message to SSCOP.  Returns FALSE if buffer 
 * unavailable.
 */
boolean atmSig_txReleaseCompleteMsg (atmSvcType *svc, uchar cause)
{

   uchar *frameptr;
   paktype *pak;
   uint size, pad;
   


   /* sizeup the entire RELEASE COMPLETE message - one mandatory IE */
   size = ATMSIG_HDRSIZE + ATMSIG_IEHDRSIZE + ATMSIG_CAUSE_IESIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header first */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_RELEASE_COMPLETE, 
                                    size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);

   /* Build the Cause IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildCause(svc, frameptr, cause);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o Rel Complete msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(svc->state), pak->datagramsize, svc->callRef, pad);

   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}



/*
 * a t m S i g _ t x S t a t u s M s g ( )
 *
 * Builds and sends STATUS Message to SSCOP.  Returns FALSE if buffer unavailable.
 */
boolean atmSig_txStatusMsg (atmSvcType *svc, uchar cause)
{

   uchar *frameptr;
   paktype *pak;
   uint size, pad;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: tx Status Msg, No party");
     svc->freeSvc = TRUE;
     return (FALSE);
   }

   /* sizeup the entire STATUS message - first the mandatory IE's */
   size = ATMSIG_HDRSIZE + (2 * ATMSIG_IEHDRSIZE) + ATMSIG_CAUSE_IESIZE +
                                                        ATMSIG_CALLSTATE_IESIZE;

   /* check for multipoint, if so, add ATMSIG_ENDPOINT_REF_IESIZE and 
                                                  ATMSIG_ENDPOINT_STATE_IESIZE */
   if (svc->multiPoint) 
     size += ( (2 * ATMSIG_IEHDRSIZE) + ATMSIG_ENDPOINT_REF_IESIZE + 
	      ATMSIG_ENDPOINT_STATE_IESIZE);

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_STATUS, 
                                       size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);

   /* Build the Cause IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildCause(svc, frameptr, cause);

   /* Build the Call State IE */
   frameptr += (ATMSIG_IEHDRSIZE + ATMSIG_CAUSE_IESIZE);
   atmSig_buildCallState(svc, frameptr);

   /* If multipoint, Build the EndPoint Ref IE and Endpoint State IE*/
   if (svc->multiPoint) {
     frameptr += ATMSIG_IEHDRSIZE + ATMSIG_CALLSTATE_IESIZE;
     atmSig_buildEndpointReference(svc, frameptr, party);

     /* Build the EndPoint State IE */
     frameptr += ATMSIG_IEHDRSIZE + ATMSIG_ENDPOINT_REF_IESIZE;
     atmSig_buildEndpointState(svc, frameptr, party);
   }

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o Status msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(svc->state), pak->datagramsize, svc->callRef, pad);

   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);
}



/*
 * a t m S i g _ t x S t a t u s E n q u i r y M s g ( )
 *
 * Builds and sends STATUS ENQUIRY Message to SSCOP.  Returns FALSE if buffer 
 * unavailable.
 */
boolean atmSig_txStatusEnquiryMsg (atmSvcType *svc)
{

   uchar *frameptr;
   paktype *pak;
   uint size, pad;
   atmSvcPartyType *party;


   /* if we are pending for the staus response, return here */
   if (svc->statusEnquiry)
     return (FALSE);

   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: tx Status Enq Msg, No party");
     svc->freeSvc = TRUE;
     return (FALSE);
   }

   /* sizeup the entire STATUS ENQUIRY message - no IE's */
   size = ATMSIG_HDRSIZE;

   /* check for multipoint, if so, add ATMSIG_ENDPOINT_REF_IESIZE */
   if (svc->multiPoint) 
     size += ATMSIG_IEHDRSIZE + ATMSIG_ENDPOINT_REF_IESIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_STATUS_ENQUIRY, 0);

   /* If multipoint, Build the EndPoint Ref IE */
   if (svc->multiPoint) {
     frameptr += ATMSIG_HDRSIZE;
     atmSig_buildEndpointReference(svc, frameptr, party);
   }

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o Status Enquiry msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(svc->state), pak->datagramsize, svc->callRef, pad);

   svc->statusEnquiry = TRUE;
   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}



/*
 * a t m S i g _ t x R e s t a r t M s g ( )
 *
 * Builds and sends RESTART Message to SSCOP.  Returns FALSE if buffer unavailable.
 */
boolean atmSig_txRestartMsg (atmSvcType *svc)
{

   uchar *frameptr;
   paktype *pak;
   uint size, pad;

   /* sizeup the entire RESTART message - first the mandatory IE's */
   size = ATMSIG_HDRSIZE +  (2 * ATMSIG_IEHDRSIZE) + ATMSIG_CONNECTIONID_IESIZE +
                                                   ATMSIG_RESTARTINDICATOR_IESIZE; 

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header first */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_RESTART, 
                                      size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);

   /* Build the Connection Id IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildConnectionId(svc, frameptr);

   /* Build the Restart Indicator IE */
   frameptr += (ATMSIG_IEHDRSIZE + ATMSIG_CONNECTIONID_IESIZE);
   atmSig_buildRestartIndicator(svc, frameptr);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o Restart msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(svc->state), pak->datagramsize, svc->callRef, pad);

   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);
   
   return (TRUE);

}



/*
 * a t m S i g _ t x R e s t a r t A c k M s g ( )
 *
 * Builds and sends RESTART ACK Message to SSCOP.  Returns FALSE if buffer 
 * unavailable. 
 */
boolean atmSig_txRestartAckMsg (atmSvcType *svc)
{

   uchar *frameptr;
   paktype *pak;
   uint size, pad;


   /* sizeup the entire RESTART ACK message - first the mandatory IE's */
   size = ATMSIG_HDRSIZE +  (2 * ATMSIG_IEHDRSIZE) + ATMSIG_CONNECTIONID_IESIZE +
                                                   ATMSIG_RESTARTINDICATOR_IESIZE;
   
   /* Q.93 messages need to align to long word boundary for SSCOP */  
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_RESTART_ACK, 
                                  size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE); 

   /* Build the Connection Id IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildConnectionId(svc, frameptr);

   /* Build the Restart Indicator IE */
   frameptr += (ATMSIG_IEHDRSIZE + ATMSIG_CONNECTIONID_IESIZE);
   atmSig_buildRestartIndicator(svc, frameptr);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o Restart Ack msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(svc->state), pak->datagramsize, svc->callRef, pad);

   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}



/*
 * a t m S i g _ s i z e u p U s e r C e l l R a t e
 *
 * Find the number of bytes required for the User Cell Rate IE to be sent in
 * the SETUP message.  The size is not fixed as the user may not configure all
 * the Cell Rate Parameters.
 * We wish to signal best-effort if at all possible,  but can only have
 * specified fwd/bwd CLP01 pcr if so.
 */
int atmSig_sizeupUserCellRate (atmSvcType *svc)
{
   boolean canDoBestEffort = TRUE;
   int size = 0;


   /* Forward Peak Cell Rate (CLP = 0) configured? */
   if (svc->tos.userCellRate.fwdPeakClp0 != -1) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_LENGTH;
   }

   /* Backward Peak Cell Rate (CLP = 0) configured? */
   if (svc->tos.userCellRate.bwdPeakClp0 != -1) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_LENGTH;
   }

   /* Forward Peak Cell Rate (CLP = 0 + 1) configured? */
   if (svc->tos.userCellRate.fwdPeakClp01 != -1) 
      size += ATMSIG_UCRID_LENGTH;

   /* Backward Peak Cell Rate (CLP = 0 + 1) configured? */
   if (svc->tos.userCellRate.bwdPeakClp01 != -1) 
      size += ATMSIG_UCRID_LENGTH; 

   /* Forward Sustainable Cell Rate (CLP = 0) configured? */
   if (svc->tos.userCellRate.fwdSusClp0 != -1) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_LENGTH;
   }

   /* Backward Sustainable Cell Rate (CLP = 0) configured? */
   if (svc->tos.userCellRate.bwdSusClp0 != -1) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_LENGTH;
   }

   /* Forward Sustainable Cell Rate (CLP = 0 + 1) configured? */
   if (svc->tos.userCellRate.fwdSusClp01 != -1) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_LENGTH;
   }

   /* Backward Sustainable Cell Rate (CLP = 0 + 1) configured? */
   if (svc->tos.userCellRate.bwdSusClp01 != -1) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_LENGTH;
   }

   /* Forward Maximum Burst Size (CLP = 0) configured? */
   if (svc->tos.userCellRate.fwdMaxBurstClp0 != -1) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_LENGTH;
   }

   /* Backward Maximum Burst Size (CLP = 0) configured? */
   if (svc->tos.userCellRate.bwdMaxBurstClp0 != -1) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_LENGTH;
   }

   /* Forward Maximum Burst Size (CLP = 0 + 1) configured? */
   if (svc->tos.userCellRate.fwdMaxBurstClp01 != -1) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_LENGTH;
   }

   /* Backward Maximum Burst Size (CLP = 0 + 1) configured? */
   if (svc->tos.userCellRate.bwdMaxBurstClp01 != -1) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_LENGTH;
   }

   /* Traffic Management Options configured? */
   if ( (svc->tos.userCellRate.FwdTag != -1) || 
                                 (svc->tos.userCellRate.BwdTag != -1) ) {
      canDoBestEffort = FALSE;
      size += ATMSIG_UCRID_TRMGMT_LENGTH;
   }

   /*
    * If none of the User Cell Rate parameters are configured, then insert
    * "Best Effort Indicator" in the IE for the SETUP message.
    */
   if (canDoBestEffort) {
      size = ATM_UCR_BESTEFFORT_LNGTH;
      svc->tos.userCellRate.bestEffortIndicator = 1;
   }

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: User Cell Rate IE size = %d", size);
   return (size);

}



/*
 * a t m S i g _ s i z e u p L o w e r L a y e r I e
 *
 * Find the size of the Broadband Lower Layer Ie to be included in the
 * Setup message.
 */
int atmSig_sizeupLowerLayerIe (atmSvcType *svc)
{

   int size;


   size = 0;

   /* layer 2 id to be included? */
   if (svc->layer2id != -1)
      size = ATMSIG_LLAYER2_LENGTH;

   /* mux vc? */
   if (svc->aal5mux)
      size += ATMSIG_LLAYER3_LENGTH;

   /* default is SNAP vc */
   if (!size) {
      svc->layer2id = 0x0C;
      size = ATMSIG_LLAYER2_LENGTH;
   }

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nsizeupLowerLayerIe: size = %d", size);

   return (size);

}



/*
 * a t m S i g _ s e n d T o S s c o p
 *
 * Queue the packet on sscop's transmit queue.
 */
void atmSig_sendToSscop (atmSvcType *svc, paktype *pak, long pad)
{

   idbtype *idb;
   sscop_sdPdu *pdu;


   idb = pak->if_output = svc->sig_idb;

   /* form Sd hdr to insert pad bytes - SSCOP hdrs are at the end of packet */
   pdu = (sscop_sdPdu *)(pak->datagramstart + pak->datagramsize - 
			                             SSCOP_HEADER_SIZE);
   pdu->type_ns = pad;

   /* send to sscop. different mailboxes between UNI 3.0 and UNI 3.1 */
   process_enqueue_pak(sscop_outputQ, pak); 

   if (atmSig_debugAll || atmSig_debugPacket) 
     atmSig_hexout(pak->datagramstart, pak->datagramsize - 
		   sizeof(sscop_sdPdu), "ATMSIG(O)"); 


}



/*
 * M u l t i p o i n t  M e s s a g e s 
 */


/*
 * a t m S i g _ p a r t y M s g ( )
 *
 * Check incoming message for multipoint type of message.
 * Return TRUE if multipoint.
 */
boolean atmSig_partyMsg (atmSig_eventtype newevent)
{

  boolean partyMsg;

  switch(newevent) {
    /*
     * Point to point messages 
     */
  case ATMSIG_RCV_CALL_PROCEEDING:
    partyMsg = FALSE;
    break;
  case ATMSIG_RCV_CONNECT:
    partyMsg = FALSE;
    break;
  case ATMSIG_RCV_CONNECT_ACK:
    partyMsg = FALSE;
    break;
  case ATMSIG_RCV_SETUP:
    partyMsg = FALSE;
    break;
  case ATMSIG_RCV_RELEASE:
    partyMsg = FALSE;
    break;
  case ATMSIG_RCV_RELEASE_COMPLETE:
    partyMsg = FALSE;
    break;
  case ATMSIG_RCV_RESTART:
    partyMsg = FALSE;
    break;
  case ATMSIG_RCV_RESTART_ACK:
    partyMsg = FALSE;
    break;
  case ATMSIG_RCV_STATUS:
    partyMsg = FALSE; 
    break;
  case ATMSIG_RCV_STATUS_ENQUIRY:
    partyMsg = FALSE; 
    break;

    /*
     * Multipoint messages 
     */
  case ATMSIG_RCV_ADD_PARTY_ACK:
    partyMsg = TRUE;
    break;
  case ATMSIG_RCV_ADD_PARTY_REJ:
    partyMsg = TRUE;
    break;
  case ATMSIG_RCV_DROP_PARTY:
    partyMsg = TRUE;
    break;
  case ATMSIG_RCV_DROP_PARTY_ACK:
    partyMsg = TRUE;
    break;
  case ATMSIG_RCV_ADD_PARTY:
    partyMsg = TRUE;
    break;

  default:
    partyMsg = FALSE;
  }
  
  return(partyMsg);

}



/*
 * a t m S i g _ f i n d E n d p o i n t R e f ( )
 *
 * Parse incoming message for Endpoint Ref.
 * Find party by Endpoint. 
 */
short atmSig_findEndpointRef (atmSvcType *svc, paktype *pak)
{

   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   boolean EndpointRefIe = FALSE;
   int result;
   short endpointRef;


   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of message - excluding msg header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;
  
   /* Start processing the various IE's in the message, find the EndpointRefIe */
   while (newlength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

         /* 
	  * EndpointRef plugged into svc->msgEndpointRef 
	  * in processEndPointReference
	  */
         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
            EndpointRefIe = TRUE;
            goto found_endpointRef;

	    /* No action taken for other IE's */
	  default:
	    break;
	  }

      /* decrement Message length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nnew length %d of message is more than total length %d", 
                                                             newlength, msglength);
         return(-1);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE; 
    }

found_endpointRef:
   /* endpointRef found, multipoint call */
   if ( EndpointRefIe ) {
     endpointRef = svc->msgEndpointRef;
     return (endpointRef);
   }
   /* endpointRef not found, pt-pt call */
   endpointRef = -1;
   return (endpointRef);


}



/*
 * a t m S i g _ p r o c e s s A d d P a r t y M s g 
 *
 * Process AddParty Message
 */
int atmSig_processAddPartyMsg (atmSvcType *svc, atmSvcPartyType *party)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   boolean EndpointRefIe = FALSE;     /* mandatory IE */
   boolean CalledNumIe = FALSE;       /*     "        */
   int result = ATMSIG_MSG_OK;

     
   pak = svc->inbuf; 
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of ADD_PARTY message - excluding msg header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;
  
   /* Start processing the various IE's in the ADD_PARTY message */
   while (newlength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

         case ATM_RCV_AAL:
            result |= atmSig_processAalIe(svc, ieHdr); 
            break;

         case ATM_RCV_BROADBAND_HIGHLAYER:
            result |= atmSig_processBroadbandHighLayerInfo(svc, ieHdr); 
            break;

         case ATM_RCV_BROADBAND_LOWLAYER:
            result |= atmSig_processBroadbandLowLayerInfo(svc, ieHdr); 
            break;

         /* mandatory IE for ADD_PARTY */
         case ATM_RCV_CALLEDPARTY_NUMBER:
            result |= atmSig_processCalledPartyNumber(svc, ieHdr); 
            CalledNumIe = TRUE;
            break;

         case ATM_RCV_CALLEDPARTY_SUBADDR:
            result |= atmSig_processCalledPartySubAddr(svc, ieHdr); 
            break;

         case ATM_RCV_CALLINGPARTY_NUMBER:
            result |= atmSig_processCallingPartyNumber(svc, ieHdr, party); 
            break;

         case ATM_RCV_CALLINGPARTY_SUBADDR:
            result |= atmSig_processCallingPartySubAddr(svc, ieHdr, party);
            break;

         case ATM_RCV_BROADBAND_SENDINGCMPLT:
            result |= atmSig_processBroadbandSendCmplt(svc, ieHdr);
            break;

         case ATM_RCV_TRANSIT_NTWRKSEL:
            result |= atmSig_processTransitNtwrkSel(svc, ieHdr);
            break;

         /* mandatory IE for ADD_PARTY */
         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
            EndpointRefIe = TRUE;
            break;

         /* IE not required for ADD_PARTY message - skip it over!! */       
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            if (atmSig_debugAll || atmSig_debugEvent)
               buginf("\nunknown ie %x rcvd in AddParty msg", ieOpCode);
            break;
      }

      /* decrement Message length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nnew length %d of AddParty message is more than total length %d", 
                                                             newlength, msglength);
         return(ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE; 
   }


   /* All Mandatory IE's present in ADD_PARTY? */
   if (!CalledNumIe || !EndpointRefIe ) {
      if (atmSig_debugAll || atmSig_debugEvent) {
         if (!CalledNumIe)
            buginf("\nATMSIG: mandatory Called Party IE missing in AddParty message");
         if (!EndpointRefIe)
            buginf("\nATMSIG: mandatory Endpoint Ref IE missing in AddParty msg");
      } 
      result |= ATMSIG_MANDATORY_IE_MISSING;
   }

   /* Now take action if Message contains any errors!! */

   /*
    * If a ADD_PARTY message is received which has one or more mandatory IE's
    * missing, an ADD_PARTY_REJ message with cause "mandatory IE is missing"
    * shall be returned.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in AddParty message");
      atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * If a ADD_PARTY message is received which has one or more mandatory IE's
    * with invalid content, a RELEASE COMPLETE message with cause "invalid IE
    * contents" shall be returned.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in AddParty message");
      atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a ADD_PARTY message is received which has one or more non-mandatory IE's
    * with invalid content, a STATUS message with cause "invalid IE contents"
    * shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in AddParty message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a ADD_PARTY message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: AddParty message received with unrecognized ie");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result);

}



/*
 * a t m S i g _ p r o c e s s A d d P a r t y A c k M s g 
 *
 * Process AddPartyAck Message 
 */
int atmSig_processAddPartyAckMsg (atmSvcType *svc, atmSvcPartyType *party)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   boolean EndpointRefIe = FALSE;     /* mandatory IE */
   int result = ATMSIG_MSG_OK;


   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of ADD_PARTY_ACK message - excluding the message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the ADD_PARTY_ACK message */
   while (newlength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

        /* mandatory IE for ADD_PARTY_ACK */
         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
            EndpointRefIe = TRUE;
            break;

         /* IE not required for ADD_PARTY_ACK message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;

      }       

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new lngth %d of AddPartyAck is more than total lngth %d", 
                                                                 newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;

   }

   /* All Mandatory IE's present in ADD_PARTY_ACK? */
   if (!EndpointRefIe ) {
      if (atmSig_debugAll || atmSig_debugEvent)
	buginf("\nATMSIG: mandatory Endpoint Ref IE missing in AddPartyAck msg");
      result |= ATMSIG_MANDATORY_IE_MISSING;
   }

   /* Now take action if Message contains any errors!! */

   /*
    * If a ADD_PARTY_ACK message is received which has one or more mandatory IE's
    * missing, a ADD_PARTY_REJ message with cause "mandatory IE is missing"
    * shall be returned.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in AddPartyAck message");
      atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * If a ADD_PARTY_ACK message is received which has one or more mandatory IE's
    * with invalid content, a ADD_PARTY_REJ message with cause "invalid IE
    * contents" shall be returned.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in AddPartyAck message");
      atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a ADD_PARTY_ACK message is received which has one or more non-mandatory IE's
    * with invalid content, a STATUS message with cause "invalid IE contents"
    * shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in AddPartyAck message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a ADD_PARTY_ACK message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: AddPartyAck message received with unrecognized ie");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result); 

}


/*
 * a t m S i g _ p r o c e s s A d d P a r t y R e j M s g 
 *
 * Process AddPartyRej Message 
 */
int atmSig_processAddPartyRejMsg (atmSvcType *svc, atmSvcPartyType *party)
{

   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   boolean EndpointRefIe = FALSE;     /* mandatory IE */
   boolean CauseIe = FALSE;     /* mandatory IE */
   int result = ATMSIG_MSG_OK;


   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of ADD_PARTY_REJ message - excluding the message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the ADD_PARTY_REJ message */
   while (newlength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

        /* mandatory IE for ADD_PARTY_REJ */
         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
            EndpointRefIe = TRUE;
            break;

        /* mandatory IE for ADD_PARTY_REJ */
         case ATM_RCV_CAUSE:
            result |= atmSig_processCause(svc, ieHdr);
	    CauseIe = TRUE;
            break;

         /* IE not required for ADD_PARTY_REJ message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;

      }       

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new lngth %d of AddPartyRej is more than total lngth %d", 
                                                                 newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;

   }

   /* All Mandatory IE's present in ADD_PARTY_REJ? */
   if (!EndpointRefIe || !CauseIe) {
      if (atmSig_debugAll || atmSig_debugEvent) {
	if (!EndpointRefIe)
	  buginf("\nATMSIG: mandatory Endpoint Ref IE missing in AddPartyRej msg");
	if (!CauseIe)
	  buginf("\nATMSIG: mandatory Cause IE missing in AddPartyRej msg");
      }
      result |= ATMSIG_MANDATORY_IE_MISSING;
   }

   /* Now take action if Message contains any errors!! */

   /*
    * If a ADD_PARTY_REJ message is received which has one or more mandatory IE's
    * missing, a ADD_PARTY_REJ message with cause "mandatory IE is missing"
    * shall be returned.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in AddPartyRej message");
      atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * If a ADD_PARTY_REJ message is received which has one or more mandatory IE's
    * with invalid content, a ADD_PARTY_REJ message with cause "invalid IE
    * contents" shall be returned.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in AddPartyRej message");
      atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a ADD_PARTY_REJ message is received which has one or more non-mandatory IE's
    * with invalid content, a STATUS message with cause "invalid IE contents"
    * shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in AddPartyRej message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a ADD_PARTY_REJ message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: AddPartyRej message received with unrecognized ie");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result); 

}




/*
 * a t m S i g _ p r o c e s s D r o p P a r t y M s g 
 *
 * Process DropParty Message 
 */
int atmSig_processDropPartyMsg (atmSvcType *svc, atmSvcPartyType *party)
{


   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   boolean CauseIe = FALSE;     /* mandatory IE */
   boolean EndpointRefIe = FALSE;     /* mandatory IE */
   int result = ATMSIG_MSG_OK;


   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of DROP_PARTY message - excluding the message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the DROP_PARTY message */
   while (newlength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

        /* mandatory IE for DROP_PARTY */
         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
            EndpointRefIe = TRUE;
            break;

        /* mandatory IE for DROP_PARTY */
         case ATM_RCV_CAUSE:
            result |= atmSig_processCause(svc, ieHdr);
	    CauseIe = TRUE;
            break;
 
        /* IE not required for DROP_PARTY message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;
      }       

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new lngth %d of DropParty is more than total lngth %d", 
                                                                 newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;

   }

   /* All Mandatory IE's present in DROP_PARTY? */
   if (!EndpointRefIe || !CauseIe) {
      if (atmSig_debugAll || atmSig_debugEvent) {
	if (!EndpointRefIe)
	  buginf("\nATMSIG: mandatory Endpoint Ref IE missing in AddPartyRej msg");
	if (!CauseIe)
	  buginf("\nATMSIG: mandatory Cause IE missing in AddPartyRej msg");
      }
      result |= ATMSIG_MANDATORY_IE_MISSING;
   }

   /* Now take action if Message contains any errors!! */

   /*
    * If a DROP_PARTY message is received which has one or more mandatory IE's
    * missing, a ADD_PARTY_REJ message with cause "mandatory IE is missing"
    * shall be returned.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in DropParty message");
      atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * If a DROP_PARTY message is received which has one or more mandatory IE's
    * with invalid content, a ADD_PARTY_REJ message with cause "invalid IE
    * contents" shall be returned.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in DropParty message");
      atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a DROP_PARTY message is received which has one or more non-mandatory IE's
    * with invalid content, a STATUS message with cause "invalid IE contents"
    * shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in DropParty message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a DROP_PARTY message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: DropParty message received with unrecognized ie");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result); 

}


/*
 * a t m S i g _ p r o c e s s D r o p P a r t y A c k M s g 
 *
 * Process DropPartyAck Message 
 */
int atmSig_processDropPartyAckMsg (atmSvcType *svc, atmSvcPartyType *party)
{


   paktype *pak;
   atmSig_msgHeader *hdr;
   atmSig_ieHeader *ieHdr;
   ushort msglength, newlength, ieOpCode;
   uchar *frameptr;
   boolean EndpointRefIe = FALSE;     /* mandatory IE */
   int result = ATMSIG_MSG_OK;


   pak = svc->inbuf;
   hdr = (atmSig_msgHeader *)pak->datagramstart;

   /* length of DROP_PARTY_ACK message - excluding the message header */
   msglength = newlength = GETSHORT(&hdr->length[0]);

   /* move pointer to first IE */
   frameptr = (uchar *)hdr + ATMSIG_HDRSIZE;

   /* Start processing the various IE's in the DROP_PARTY_ACK message */
   while (newlength) {

      ieHdr = (atmSig_ieHeader *)frameptr;
      ieOpCode = atmSig_findIeOpCode(ieHdr);

      switch (ieOpCode) {

        /* mandatory IE for DROP_PARTY_ACK */
         case ATM_RCV_ENDPOINT_REFERENCE:
            result |= atmSig_processEndPointReference(svc, ieHdr);
            EndpointRefIe = TRUE;
            break;

         case ATM_RCV_CAUSE:
            result |= atmSig_processCause(svc, ieHdr);
            break;
 
         /* IE not required for DROP_PARTY_ACK message - skip it over!! */
         default:
            result |= ATMSIG_IE_NOTREQUIRED;
            break;

      }       

      /* decrement total length by IE size */
      newlength -= (GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE);

      /* sanity check */
      if (newlength > msglength) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: new lngth %d of DropPartyAck is more than total lngth %d", 
                                                                 newlength, msglength);
         return (ATMSIG_IE_FATAL_ERROR);
      }

      /* move to next IE in the Message */
      frameptr = (uchar *)ieHdr;
      frameptr += GETSHORT(&ieHdr->length[0]) + ATMSIG_IEHDRSIZE;

   }

   /* All Mandatory IE's present in DROP_PARTY_ACK? */
   if (!EndpointRefIe ) {
      if (atmSig_debugAll || atmSig_debugEvent)
	buginf("\nATMSIG: mandatory Endpoint Ref IE missing in DropPartyAck msg");
      result |= ATMSIG_MANDATORY_IE_MISSING;
   }

   /* Now take action if Message contains any errors!! */

   /*
    * If a DROP_PARTY_ACK message is received which has one or more mandatory IE's
    * missing, a ADD_PARTY_REJ message with cause "mandatory IE is missing"
    * shall be returned.
    */
   if (result & ATMSIG_MANDATORY_IE_MISSING) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie missing in DropPartyAck message");
      atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_MANDATORY_IE_MISSING);
      return (result);
   }

   /*
    * If a DROP_PARTY_ACK message is received which has one or more mandatory IE's
    * with invalid content, a ADD_PARTY_REJ message with cause "invalid IE
    * contents" shall be returned.
    */
   if (result & ATMSIG_IE_FATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: mandatory ie with invalid content in DropPartyAck message");
      atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a DROP_PARTY_ACK message is received which has one or more non-mandatory IE's
    * with invalid content, a STATUS message with cause "invalid IE contents"
    * shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: non-mandatory ie with invalid content in DropPartyAck message");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   /*
    * If a DROP_PARTY_ACK message is received which has an unrecognized IE, a STATUS
    * message with cause "IE non-existent or not implemented" shall be returned.
    */
   if (result & ATMSIG_IE_NONFATAL_ERROR) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: DropPartyAck message received with unrecognized ie");
      atmSig_txStatusMsg(svc, ATM_CAUSE_INVALID_IE_CONTENTS);
      return (result);
   }

   return (result); 

}




/*
 * a t m S i g _ t x A d d P a r t y M s g 
 *
 * Transmit AddParty Message
 */
boolean atmSig_txAddPartyMsg (atmSvcType *svc, atmSvcPartyType *party)
{

   ushort aal5IeSize = 0;
   uchar *frameptr;
   paktype *pak;
   int size, pad;
   int ll_size;


   size = 0;   /* size ==> length of the entire SETUP msg (including msg header) */

   /*
    * size up Lower Layer IE (optional IE) which is variable length depending
    * on if the vc is mux and/or layer 2 id is to be included.
    */
   ll_size = svc->blli[0].length;

   /* sizeup the entire ADDPARTY message - first the mandatory IE's */
   size = ATMSIG_HDRSIZE + ATMSIG_CALLEDPARTY_IESIZE  +
          ATMSIG_ENDPOINT_REF_IESIZE + (2 * ATMSIG_IEHDRSIZE);

   /* Calling Party IE is optional - client may not want to send it */
   if (svc->srcNsap.length > 0)
      size += ATMSIG_IEHDRSIZE + ATMSIG_CALLINGPARTY_IESIZE;

   /* optional IE - but we send it! */
   /* must be same as in original party SETUP */
   if (svc->tos.aalType == ATM_AALIE_TYPE34)
      size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE34_IESIZE;
   else if (svc->tos.aalType == ATM_AALIE_TYPE5) {
     if ( atmSig_isV31(svc) )
       size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE5_IESIZE_V31;
       else {
	 /* this is really ugly!! */
	 if (svc->tos.mode !=  ATM_AALIE_MODE_INVALID) 
	   size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE5_IESIZE;
	 else
	   size += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE5_IESIZE - 2;
       }
   }

   /* LL IE (optional) to be included? */
   /* must be same as in original party SETUP */
   if (ll_size)
      size += ATMSIG_IEHDRSIZE + ll_size;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0;

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet for sscop headers cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header first */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_ADD_PARTY, 
                                   size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);


   /* Build the EndPoint Ref IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildEndpointReference(svc, frameptr, party);

   /* Build Called Party Number IE */
   frameptr += ATMSIG_IEHDRSIZE + ATMSIG_ENDPOINT_REF_IESIZE;
   atmSig_buildCalledPartyNumber(svc, frameptr, party);

   /* Build Calling Party Number IE ONLY if client wants to */
   frameptr += ATMSIG_IEHDRSIZE + ATMSIG_CALLEDPARTY_IESIZE;
   if (svc->srcNsap.length > 0) {
      atmSig_buildCallingPartyNumber(svc, frameptr);
      frameptr += ATMSIG_IEHDRSIZE + ATMSIG_CALLINGPARTY_IESIZE;
   }

   /* Build AAL IE */
   if (svc->tos.aalType == ATM_AALIE_TYPE34) {
     atmSig_buildAalType34(svc, frameptr); 
     frameptr += ATMSIG_IEHDRSIZE + ATMSIG_AALTYPE34_IESIZE;
   }
   else if (svc->tos.aalType == ATM_AALIE_TYPE5) {
     aal5IeSize = atmSig_buildAalType5(svc, frameptr); 
     frameptr += ATMSIG_IEHDRSIZE + aal5IeSize;
   }
  
   /* Build Broadband Lower Layer IE for mux vc's */
   if (ll_size) {
     atmSig_buildBBandLowLayerInfo(svc, frameptr); 
     frameptr += ATMSIG_HDRSIZE + ll_size;
   }

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o AddParty msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(party->partyState), pak->datagramsize, svc->callRef, pad); 

   /* send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}


/*
 * a t m S i g _ t x A d d P a r t y A c k M s g 
 *
 * Transmit AddPartyAck Message
 */
boolean atmSig_txAddPartyAckMsg (atmSvcType *svc, atmSvcPartyType *party)
{


   uchar *frameptr;
   paktype *pak;
   uint size, pad;

   /* sizeup the entire ADD_PARTY_ACK message - mandatory IE */
   size = ATMSIG_HDRSIZE + ATMSIG_IEHDRSIZE + ATMSIG_ENDPOINT_REF_IESIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_ADD_PARTY_ACK, 
                                   size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);

   /* Build the EndPoint Ref IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildEndpointReference(svc, frameptr, party);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o AddPartyAck msg, %s state, length %d, call ref %d, pad %d",
           atmSig_printState(party->partyState), pak->datagramsize, svc->callRef, pad);

   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}


/*
 * a t m S i g _ t x A d d P a r t y R e j M s g 
 *
 * Transmit AddPartyRej Message
 */
boolean atmSig_txAddPartyRejMsg (atmSvcType *svc, atmSvcPartyType *party, uchar cause)
{


   uchar *frameptr;
   paktype *pak;
   uint size, pad;

   /* sizeup the entire ADD_PARTY_REJ message - mandatory IEs */
   size = ATMSIG_HDRSIZE + (2 * ATMSIG_IEHDRSIZE) + ATMSIG_ENDPOINT_REF_IESIZE + 
     ATMSIG_CAUSE_IESIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_ADD_PARTY_REJ, 
                                   size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);

   /* Build the Cause IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildCause(svc, frameptr, cause);

   /* Build the EndPoint Ref IE */
   frameptr += ATMSIG_IEHDRSIZE + ATMSIG_CAUSE_IESIZE;
   atmSig_buildEndpointReference(svc, frameptr, party);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o AddPartyRej msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(party->partyState), pak->datagramsize, svc->callRef, pad);

   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}


/*
 * a t m S i g _ t x D r o p P a r t y M s g 
 *
 * Transmit DropParty Message
 */
boolean atmSig_txDropPartyMsg (atmSvcType *svc, atmSvcPartyType *party, uchar cause)
{


   uchar *frameptr;
   paktype *pak;
   uint size, pad;

   /* sizeup the entire DROP_PARTY message - mandatory IEs */
   size = ATMSIG_HDRSIZE + (2 * ATMSIG_IEHDRSIZE) + ATMSIG_ENDPOINT_REF_IESIZE + 
     ATMSIG_CAUSE_IESIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_DROP_PARTY, 
                                   size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);

   /* Build the Cause IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildCause(svc, frameptr, cause);

   /* Build the EndPoint Ref IE */
   frameptr += ATMSIG_IEHDRSIZE + ATMSIG_CAUSE_IESIZE;
   atmSig_buildEndpointReference(svc, frameptr, party);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o DropParty msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(party->partyState), pak->datagramsize, svc->callRef, pad);

   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}





/*
 * a t m S i g _ t x D r o p P a r t y A c k M s g 
 *
 * Transmit DropPartyAck Message
 */
boolean atmSig_txDropPartyAckMsg (atmSvcType *svc, atmSvcPartyType *party, uchar cause)
{


   uchar *frameptr;
   paktype *pak;
   uint size, pad;

   /* sizeup the entire DROP_PARTY_ACK message - mandatory IEs */
   size = ATMSIG_HDRSIZE + (2 * ATMSIG_IEHDRSIZE) + ATMSIG_ENDPOINT_REF_IESIZE + 
     ATMSIG_CAUSE_IESIZE;

   /* Q.93 messages need to align to long word boundary for SSCOP */
   pad = (size % SSCOP_PADBYTES) ? ( SSCOP_PADBYTES - (size % SSCOP_PADBYTES) ) : 0; 

   /*
    * we also need to take into account the sscop header size here as sscop headers
    * are at the END of the message.  The traditional method of using the ENCAP area
    * of the packet cannot be used for Q.93B/SSCOP.
    */
   size += sizeof(sscop_sdPdu);

   /* get a buffer */
   pak = getbuffer(size + pad);
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = size + pad;
   frameptr = pak->datagramstart;

   /* Build the Message Header */
   atmSig_buildMsgHeader(svc, frameptr, ATMSIG_MT_DROP_PARTY_ACK, 
                                   size - ATMSIG_HDRSIZE - SSCOP_HEADER_SIZE);

   /* Build the Cause IE */
   frameptr += ATMSIG_HDRSIZE;
   atmSig_buildCause(svc, frameptr, cause);

   /* Build the EndPoint Ref IE */
   frameptr += ATMSIG_IEHDRSIZE + ATMSIG_CAUSE_IESIZE;
   atmSig_buildEndpointReference(svc, frameptr, party);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: o DropPartyAck msg, %s state, length %d, call ref %d, pad %d",
            atmSig_printState(party->partyState), pak->datagramsize, svc->callRef, pad);

   /* Send it to SSCOP!! */
   atmSig_sendToSscop(svc, pak, (long)pad);

   return (TRUE);

}

 
