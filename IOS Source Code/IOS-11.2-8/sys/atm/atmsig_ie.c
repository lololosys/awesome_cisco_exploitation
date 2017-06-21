/* $Id: atmsig_ie.c,v 3.3.48.9 1996/07/10 22:20:18 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_ie.c,v $
 *------------------------------------------------------------------
 *
 * A T M S I G _ I E . C
 *
 * ATM Signalling Information Element Handler 
 *
 * November 1993,   Percy P. Khabardar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_ie.c,v $
 * Revision 3.3.48.9  1996/07/10  22:20:18  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.48.8  1996/06/21  02:08:37  jwjang
 * CSCdi60829:  unknow ie in connect messae
 *         also added txQ count to show sscop
 * Branch: California_branch
 *
 * Revision 3.3.48.7  1996/06/19  00:56:37  jwjang
 * CSCdi57794:  atmsig setup message for classical ip has different IEs
 * format
 * CSCdi59379
 * Branch: California_branch
 *
 * Revision 3.3.48.6  1996/05/09  14:11:06  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.48.5  1996/04/29  10:01:55  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.3.48.4.2.1  1996/04/27  06:33:29  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.48.4  1996/04/25  23:02:17  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.48.3  1996/04/23  04:47:40  grobelch
 * - When we initiate an SVC with no traffic parameters specified,
 *     use a default value of all-ones. Still recognize 1 as a
 *     default.
 *   - Use the correct id-value to specify the FWD/BWD max-burst
 *     for CLP0&1 in the UCR IE. Use B0/B1, the old/bad values
 *     were C0/C1.
 *   - Correct error in initiating an SVC using the forward SCR
 *     value for CLP0 in UCR
 *
 *   ** Also fix for CSCdi55344:
 *   - Signalling uses whole-cell rates in installing VCs/specifying
 *      map-class.
 * CSCdi55196:  ATMSIG: UserCellRate Information Element changes needed
 * Branch: California_branch
 *
 * Revision 3.3.48.2.4.1  1996/04/10  23:35:31  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.48.2  1996/03/29  23:42:22  schrupp
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
 * Revision 3.3.48.1  1996/03/29  19:27:47  grobelch
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
 * Revision 3.3  1995/11/22  20:44:31  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.2  1995/11/17  08:46:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/01  17:48:28  percyk
 * CSCdi39722:  set sdu sizes per connection - part 2
 *
 * Revision 2.3  1995/08/23  21:00:09  schrupp
 * CSCdi39217:  ATM Process UCR params in Setup message
 *
 * Revision 2.2  1995/07/08  00:33:10  schrupp
 * CSCdi36706:  Reject ATM CBR Setup with cause QOS unavailable
 *
 * Revision 2.1  1995/06/07  20:11:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "address.h"
#include "../if/atm.h"
#include "atmsig_api.h"
#include "atmsig_private.h"


#define ATMSIG_BEST_EFFORT_DEFAULT_CR 1

/*
 * a t m s i g _ f i n d I e O p C o d e ( )
 *
 * Find the type of Information Element received.
 */
ushort atmSig_findIeOpCode (atmSig_ieHeader *ieHdr)
{

   ushort opcode;


   switch (ieHdr->identifier) {

      /* ATM Adaption Layer IE */
      case ATM_AAL_IE:
         opcode = ATM_RCV_AAL;
         break;

      /* User Cell Rate IE */ 
      case ATM_USERCELLRATE_IE:
         opcode = ATM_RCV_USERCELLRATE;
         break;

      /* Broadband Bearer Capability */ 
      case ATM_BROADBAND_BEARERCAP_IE:
         opcode = ATM_RCV_BROADBAND_BEARERCAP;
         break;
 
      /* Broadband Higher Layer IE */ 
      case ATM_BROADBAND_HIGHLAYER_IE:
         opcode = ATM_RCV_BROADBAND_HIGHLAYER;
         break;

      /* Broadband Lower Layer IE */
      case ATM_BROADBAND_LOWLAYER_IE:
         opcode = ATM_RCV_BROADBAND_LOWLAYER;
         break;
 
      /* Call State IE */ 
      case ATM_CALLSTATE_IE:
         opcode = ATM_RCV_CALLSTATE;
         break;

      /* Called Party Address IE */ 
      case ATM_CALLEDPARTY_NUMBER_IE: 
         opcode = ATM_RCV_CALLEDPARTY_NUMBER;
         break;

      /* Called Party Subaddress IE */ 
      case ATM_CALLEDPARTY_SUBADDR_IE:
         opcode = ATM_RCV_CALLEDPARTY_SUBADDR;
         break;
 
      /* Calling Party Address IE */ 
      case ATM_CALLINGPARTY_NUMBER_IE:
          opcode = ATM_RCV_CALLINGPARTY_NUMBER;
         break;

      /* Calling Party Subaddress IE */ 
      case ATM_CALLINGPARTY_SUBADDR_IE:
         opcode = ATM_RCV_CALLINGPARTY_SUBADDR;
         break;

      /* Cause IE */ 
      case ATM_CAUSE_IE:
         opcode = ATM_RCV_CAUSE;
         break;

      /* Connection Identifier IE */ 
      case ATM_CONNECTIONIDENTIFIER_IE:
         opcode = ATM_RCV_CONNECTIONIDENTIFIER;
         break;

      /* Quality of Service IE */
      case ATM_QOS_IE:
         opcode = ATM_RCV_QOS;
         break;

      /* Restart Indicator IE */ 
      case ATM_RESTARTINDICATOR_IE:
         opcode = ATM_RCV_RESTARTINDICATOR;
         break;

      /* Broadband Sending Complete IE */ 
      case ATM_BROADBAND_SENDINGCMPLT_IE:
         opcode = ATM_RCV_BROADBAND_SENDINGCMPLT;
         break;

      /* Transit Network Selection IE */
      case ATM_TRANSIT_NTWRKSEL_IE:
         opcode = ATM_RCV_TRANSIT_NTWRKSEL;
         break;

      /* End Point Reference IE */ 
      case ATM_ENDPOINT_REFERENCE_IE:
         opcode = ATM_RCV_ENDPOINT_REFERENCE;
         break;

      /* End Point State IE */
      case ATM_ENDPOINT_STATE_IE:
         opcode = ATM_RCV_ENDPOINT_STATE;
         break;

      /* Broadband Repeat Indicator IE */
      case ATM_BBAND_REPEATINDICATOR_IE:
         opcode = ATM_RCV_REPEATINDICATOR_IE;
         break;

      /* Unknown/Invalid IE */
      default:
         buginf("\ninvalid ie opcode = %x", ieHdr->identifier);
         opcode = ATM_RCV_INVALID_IE;
         break;
    
   }

   return (opcode);

}
 


/*
 * a t m S i g _ p r o c e s s A a l I e ( )
 * 
 * Process ATM Adaption Layer (AAL) Information Element.
 * This is an optional IE in setup/add-party, we should not
 * return ATMSIG_IE_FATAL_ERROR from this function.
 */
int atmSig_processAalIe (atmSvcType *svc, atmSig_ieHeader *iehdr)
{

   uchar *frameptr;
   int length, type; 
   int result = ATMSIG_IE_OK;


   type = 0;
   frameptr = (uchar *)iehdr;

   /* sanity check - this should never happen! */
   if (iehdr->identifier != ATM_AAL_IE)
      return (ATMSIG_IE_NONFATAL_ERROR);

   /* AAL IE length */
   length = GETSHORT(&iehdr->length[0]);

   if (length > ATM_AAL_MAXSIZE) {
      /* length specified more than spec'd in the UNI spec */ 
      return (ATMSIG_IE_NONFATAL_ERROR);
   }

   /* Message Compatibility Instruction Indicator */
   if ( iehdr->cii != (ATMSIG_EXTENSION_BIT | ATM_CII_CCITT | ATM_CII_NOTSIG | 
                                                           ATM_CII_CLEARCALL) ) {
      /* invalid cii - should log it!! */
      return (ATMSIG_IE_NONFATAL_ERROR);
   }

   /* Check AAL Type if present. */
   if (length) {
      frameptr += ATMSIG_IEHDRSIZE;
      type = atmSig_findAALType(*frameptr);
      if (type == ATM_AALIE_TYPEINVLD) {
         /* invalid AAL type - should log it!! */
         return (ATMSIG_IE_NONFATAL_ERROR);
      }
      else
         svc->tos.aalType = type;
      --length;
   }

   /* reached end of IE? */
   if (!length)
      return (ATMSIG_IE_OK);

   /*++frameptr;*/  /*now to the meat of the IE!! */

   switch (type) {

      /* Type 1 AAL */
      case ATM_AALIE_TYPE1:
         result = atmSig_processAALType1Ie(svc, frameptr, length);
         break;

      /* Type 3/4 AAL */
      case ATM_AALIE_TYPE34:
         result = atmSig_processAALType34Ie(svc, frameptr, length);
         break;

      /* Type 5 AAL */
      case ATM_AALIE_TYPE5:
         result = atmSig_processAALType5Ie(svc, frameptr, length);
         break;

      /* Type X AAL - user defined */
      case ATM_AALIE_TYPEX:
         result = atmSig_processAALTypeXIe(svc, frameptr, length);
         break;

      /* Invalid/Not-supported AAL - should never come here!! */
      default:
         break;
   }
   return (result);

}



/*
 * a t m S i g _ f i n d A A L T y p e ( )
 *
 * Find AAL type (in AAL Information Element).
 */
int atmSig_findAALType (uchar type)
{

   /* Mask of the extension bit before checking for AAL type */
   switch (type & ATMSIG_EXTENSION_MASK) {

      /* AAL Type 1 */
      case ATM_AALIE_TYPE1:
         return (ATM_AALIE_TYPE1);

      /* AAL Type 3/4 */
      case ATM_AALIE_TYPE34:
         return (ATM_AALIE_TYPE34);

      /* AAL Type 5 */
      case ATM_AALIE_TYPE5:
         return (ATM_AALIE_TYPE5);

      /* user defined AAL */
      case ATM_AALIE_TYPEX:
         return (ATM_AALIE_TYPEX);

      /* invalid AAL */
      default:
         return (ATM_AALIE_TYPEINVLD);
   }

}



/*
 * a t m S i g _ p r o c e s s A A L T y p e 1 I e ( )
 *
 * Process AAL Type 1 Information Element.  
 * Save all the AAL 1 parameters in the SVC block.
 */
int atmSig_processAALType1Ie (atmSvcType *svc, uchar *frameptr, int length)
{

   atmSig_aalType1Ie *ie;


   ie = (atmSig_aalType1Ie *)frameptr;

   /* sanity check */
   if (!length)
      return (ATMSIG_IE_OK);

   /* Subtype */
   if (ie->subtype_id == ATM_AALIE_SUBTYPE_ID) {
      switch (ie->subtype) {

         /* Null subtype */
         case ATM_AALIE_SUBTYPE_NULL:
            svc->tos.subType = ATM_AALIE_SUBTYPE_NULL;
            break;

         /* Voice-band based on 64 kbit/s */
         case ATM_AALIE_SUBTYPE_VOICE:
            svc->tos.subType = ATM_AALIE_SUBTYPE_VOICE;
            break;

         /* Circuit Emulation - synchronous */
         case ATM_AALIE_SUBTYPE_SYNC:
            svc->tos.subType = ATM_AALIE_SUBTYPE_SYNC;
            break;

         /* High-quality audio subtype */
         case ATM_AALIE_SUBTYPE_AUDIO:
            svc->tos.subType = ATM_AALIE_SUBTYPE_AUDIO;
            break;

         /* Video subtype */
         case ATM_AALIE_SUBTYPE_VIDEO:
            svc->tos.subType = ATM_AALIE_SUBTYPE_VIDEO;
            break;

         /* Circuit Emulation - asynchronous */
         case ATM_AALIE_SUBTYPE_ASYNC:
	    if ( !atmSig_isV31(svc) ) {
	      svc->tos.subType = ATM_AALIE_SUBTYPE_ASYNC;
	      break;
	    }  /* let it fall thru */

         /* Invalid subtype */
         default:
            /* should log invalid subtype */
            return (ATMSIG_IE_FATAL_ERROR);
            break;
      }
      length -= 2;
   }

   /* more IE to process? */
   if (!length)
      return (ATMSIG_IE_OK);

   /* Constant Bit Rate */
   if (ie->cbr_id == ATM_AALIE_CBR_IDENT) {
      switch (ie->cbr_rate) {

         /* 64 kbit/sec */
         case ATM_AALIE_CBR_64:
            svc->tos.constantBitRate = ATM_AALIE_CBR_64;
            break;
      
         /* 1544 kbit/sec (DS1) */ 
         case ATM_AALIE_CBR_1544:
            svc->tos.constantBitRate = ATM_AALIE_CBR_1544;
            break;

         /* 6312 kbit/sec (DS2) */
         case ATM_AALIE_CBR_6312:
            svc->tos.constantBitRate = ATM_AALIE_CBR_6312;
            break;

         /* 32064 kbit/sec */
         case ATM_AALIE_CBR_32064:
            svc->tos.constantBitRate = ATM_AALIE_CBR_32064;
            break;

         /* 44736 kbit/sec */
         case ATM_AALIE_CBR_44736:
            svc->tos.constantBitRate = ATM_AALIE_CBR_44736;
            break;

         /* 97728 kbit/sec */
         case ATM_AALIE_CBR_97728:
            svc->tos.constantBitRate = ATM_AALIE_CBR_97728;
            break;

         /* 2048 kbit/sec (E1) */
         case ATM_AALIE_CBR_2048:
            svc->tos.constantBitRate = ATM_AALIE_CBR_2048;
            break;

         /* 8448 kbit/sec (E2) */
         case ATM_AALIE_CBR_8448:
            svc->tos.constantBitRate = ATM_AALIE_CBR_8448;
            break;

         /* 34368 kbit/sec (E3) */
         case ATM_AALIE_CBR_34368:
            svc->tos.constantBitRate = ATM_AALIE_CBR_34368;
            break;

         /* 139264 kbit/sec */
         case ATM_AALIE_CBR_139264:
            svc->tos.constantBitRate = ATM_AALIE_CBR_139264;
            break;

         /* n * 64 kbit/sec, n => multiplier */
         case ATM_AALIE_CBR_N64:
            svc->tos.constantBitRate = ATM_AALIE_CBR_N64;
            break;

	 case ATM_AALIE_CBR_N8:   /* for UNI 3.1 only */
	    if ( atmSig_isV31(svc) ) {
	      svc->tos.constantBitRate = ATM_AALIE_CBR_N8;
	      break;
	    } /* else let it fall thru */

         default:
            return (ATMSIG_IE_FATAL_ERROR);
            break;
      }
      length -= 2;
   }

   /* more IE to process? */
   if (!length)
      return (ATMSIG_IE_OK);

   /* Multiplier present? - only for n x 64 kbit/s CBR rate */
   if ( ((svc->tos.constantBitRate == ATM_AALIE_CBR_N64) ||
	 (svc->tos.constantBitRate == ATM_AALIE_CBR_N8)) &&
                  (ie->multiplier_id == ATM_AALIE_MULTIPLIER_ID) ) {
      svc->tos.multiplier = ie->multiplier;
      length -= 3;
   }

   /* more IE to process? */
   if (!length)
      return (ATMSIG_IE_OK);

   /* Clock Recovery */
   if (ie->clkrcvry_id == ATM_AALIE_CRT_ID) {
      switch (ie->clkrcvry_type) {

         /* Null - timing is derived from physical interface */
         case ATM_AALIE_CRT_NULL:
            svc->tos.clockRecoveryType = ATM_AALIE_CRT_NULL;
            break;

         /* Synchronous Residual Time Stamp */
         case ATM_AALIE_CRT_SRTS:
            svc->tos.clockRecoveryType = ATM_AALIE_CRT_SRTS;
            break;

         /* Adaptive Clock Recovery */
         case ATM_AALIE_CRT_ACR:
            svc->tos.clockRecoveryType = ATM_AALIE_CRT_ACR;
            break;

         default:
            return (ATMSIG_IE_FATAL_ERROR);
            break;
      }
      length -= 2;
   }

   /* more IE to process? */
   if (!length)
      return (ATMSIG_IE_OK);

   /* Error Correction Type */
   if (ie->error_corr_id == ATM_AALIE_ECT_ID) {
      switch (ie->error_correction) {

         /* Null */
         case ATM_AALIE_ECT_NULL:
            svc->tos.errorCorrection = ATM_AALIE_ECT_NULL;
            break;

         /* Interleaving FEC */
         case ATM_AALIE_ECT_FECI:
            svc->tos.errorCorrection = ATM_AALIE_ECT_FECI;
            break;

         case ATM_AALIE_ECT_FECD:
	    if ( atmSig_isV31(svc) ) {
	      svc->tos.errorCorrection = ATM_AALIE_ECT_FECD;
	      break;
	    }

         default:
            return (ATMSIG_IE_FATAL_ERROR);
            break;
      }
      length -= 2;
   }

   /* more IE to process? */
   if (!length)
      return (ATMSIG_IE_OK);

   /* Structured Data Transfer */
   if (ie->struct_data_id == ATM_AALIE_SDT_ID) {
     if ( atmSig_isV31(svc) ) {
       svc->tos.structuredDataXfer = GETSHORT(
	    &((atmSig_aalType1IeV31*)ie)->struct_data_xfer);
       length -= 3;     
     }
     else     {   /* UNI 3.0 */
       switch (ie->struct_data_xfer) {

         /* Null */
         case ATM_AALIE_SDT_NULL:
            svc->tos.structuredDataXfer = ATM_AALIE_SDT_NULL;
            break;

         /* Structured Data Transfer */
         case ATM_AALIE_SDT:
            svc->tos.structuredDataXfer = ATM_AALIE_SDT;
            break;

         default:
            return (ATMSIG_IE_FATAL_ERROR);
            break;
       }
       length -= 2;
     }   /* UNI 3.0 */
   }

   /* more IE to process? */
   if (!length)
      return (ATMSIG_IE_OK);

   /* Partially Filled Cells */
   if (ie->partfilledcells_id == ATM_AALIE_PFC_ID)
      svc->tos.partiallyFilledCells = ie->partially_filled_cells;

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s A A L T y p e 3 4 I e ( )
 *
 * Process AAL Type 3/4 Information Element.  
 * Save all the AAL 3/4 parameters in the SVC block.
 */
int atmSig_processAALType34Ie (atmSvcType *svc, uchar *frameptr, int length)
{

   atmSig_aalType34Ie *ie;


   ie = (atmSig_aalType34Ie *)frameptr;

   /* sanity check */
   if (!length)
      return (ATMSIG_IE_OK);

   /* forward maximum CPCS-SDU size */
   if (ie->fwdSduSizeId == ATM_AALIE_FWD_SDUSZ_ID) {
      svc->tos.fwdSduSize = GETSHORT(&ie->fwdSdu[0]);
      length -= 3;
   }

   /* backward maximum CPCS-SDU size */
   if (ie->bwdSduSizeId == ATM_AALIE_BWD_SDUSZ_ID) {
      svc->tos.bwdSduSize = GETSHORT(&ie->bwdSdu[0]);
      length -= 3;
   }

   /* more IE to process? */
   if (!length)
      return (ATMSIG_IE_OK);

   /* MID size */
   if (ie->midSizeId_midRangeId == ATM_AALIE_MID_SIZE_ID) {
     if ( atmSig_isV31(svc) ) {
       svc->tos.svcTos_lowestMid = ie->midSize_lowestMid;
       svc->tos.highestMid = (ie->modeId_highestMidHi << 8) | ie->mode_highestMidLo;
       length -= 5;
     }
     else     {
       /* get MID */
       svc->tos.svcTos_midSize = ie->midSize_lowestMid;
       length -= 3;
       /* get Mode */
       if (ie->modeId_highestMidHi == ATM_AALIE_MODE_ID) {
	 svc->tos.mode = ie->mode_highestMidLo;
	 length -= 2;
       }
     }
   }

   /* more IE to process? */
   if (!length)
      return (ATMSIG_IE_OK);

   /* SSCS-Type Identifier */
   if (ie->sscsTypeId == ATM_AALIE_SSCSTYPE_ID) {
      switch (ie->sscsType) {

         /* Null */
         case ATM_AALIE_SSCS_NULL:
            svc->tos.sscsType = ATM_AALIE_SSCS_NULL;
            break;

         /* Data SSCS based on SSCOP (assured operation) */
         case ATM_AALIE_SSCS_ADATA:
            svc->tos.sscsType = ATM_AALIE_SSCS_ADATA;
            break;

         /* Data SSCS based on SSCOP (non-assured operation) */
         case ATM_AALIE_SSCS_UDATA:
            svc->tos.sscsType = ATM_AALIE_SSCS_UDATA;
            break;

         /* Frame Relay SSCS */
         case ATM_AALIE_SSCS_FRAMERELAY:
            svc->tos.sscsType = ATM_AALIE_SSCS_FRAMERELAY;
            break;

         default:
            break;
      }
   }

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s A A L T y p e 5 I e ( )
 *
 * Process AAL Type 5.  
 * Save all the AAL 5 parameters in the SVC block.
 */
int atmSig_processAALType5Ie (atmSvcType *svc, uchar *frameptr, int length)
{

   atmSig_aalType5Ie *ie;
   ushort sscs;
   
   ie = (atmSig_aalType5Ie *)frameptr;

   /* sanity check */
   if (!length)
      return (ATMSIG_IE_NONFATAL_ERROR);

   /* forward maximum CPCS-SDU size */
   if (ie->fwdSduSizeId == ATM_AALIE_FWD_SDUSZ_ID) {
      svc->tos.fwdSduSize = GETSHORT(&ie->fwdSdu[0]);
      length -= 3;
      /* more IE to process? */
      if (!length)
	return (ATMSIG_IE_OK);
   }

   /* backward maximum CPCS-SDU size */
   if (ie->bwdSduSizeId == ATM_AALIE_BWD_SDUSZ_ID) {
      svc->tos.bwdSduSize = GETSHORT(&ie->bwdSdu[0]);
      length -= 3;
      /* more IE to process? */
      if (!length)
	return (ATMSIG_IE_OK);
   }

   /* Mode Identifier, only uni 3.0 */
   if ( ie->modeId_sscsId == ATM_AALIE_MODE_ID ) {
     if (atmSig_isV31(svc) )  /* well, should not exist in 3.1 */
       return (ATMSIG_IE_NONFATAL_ERROR);

      if ( (ie->mode_sscs != ATM_AALIE_MODE_MSG) &&
	  (ie->mode_sscs != ATM_AALIE_MODE_STREAM) )
	return (ATMSIG_IE_NONFATAL_ERROR);

      svc->tos.mode = ie->mode_sscs;
      length -= 2;
     /* more IE to process? */
     if (!length)
       return (ATMSIG_IE_OK);
   }

   /* SSCS-Type Identifier */
   if ( atmSig_isV31(svc) && (ie->modeId_sscsId == ATM_AALIE_SSCSTYPE_ID) )
     sscs = ie->mode_sscs;
   else if ( !atmSig_isV31(svc) && (ie->sscsId == ATM_AALIE_SSCSTYPE_ID) )
     sscs = ie->sscs;
   else  /* this is an optional IE, it's ok to return ATMSIG_IE_OK */
     return (ATMSIG_IE_OK);

   switch (sscs) {

     /* Null */
   case ATM_AALIE_SSCS_NULL:
     svc->tos.sscsType = ATM_AALIE_SSCS_NULL;
     break;

     /* Data SSCS based on SSCOP (assured operation) */
   case ATM_AALIE_SSCS_ADATA:
     svc->tos.sscsType = ATM_AALIE_SSCS_ADATA;
     break;

     /* Data SSCS based on SSCOP (non-assured operation) */
   case ATM_AALIE_SSCS_UDATA:
     svc->tos.sscsType = ATM_AALIE_SSCS_UDATA;
     break;

     /* Frame Relay SSCS */
   case ATM_AALIE_SSCS_FRAMERELAY:
     svc->tos.sscsType = ATM_AALIE_SSCS_FRAMERELAY;
     break;
     
   default:
     break;
   }

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s A A L T y p e X I e ( )
 * 
 * Process AAL Type X (User defined AAL)
 */
int atmSig_processAALTypeXIe (atmSvcType *svc, uchar *frameptr, int length)
{

   atmSig_aalTypeXIe *ie;


   ie = (atmSig_aalTypeXIe *)frameptr;

   /* sanity check */
   if (!length)
      return (ATMSIG_IE_OK);

   /* user info can be anywhere from 1 to 4 bytes */
   switch (length) {

      case 1:
         svc->tos.userInfo1 = ie->user_info1;
         break;

      case 2:
         svc->tos.userInfo1 = ie->user_info1;
         svc->tos.userInfo2 = ie->user_info2;
         break;

      case 3:
         svc->tos.userInfo1 = ie->user_info1;
         svc->tos.userInfo2 = ie->user_info2;
         svc->tos.userInfo3 = ie->user_info3;
         break;

      case 4:
         svc->tos.userInfo1 = ie->user_info1;
         svc->tos.userInfo2 = ie->user_info2;
         svc->tos.userInfo3 = ie->user_info3;
         svc->tos.userInfo4 = ie->user_info4;
         break;

       default:
          break;
   }

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s U s e r C e l l R a t e ( )
 *
 * Process User Cell Rate Information Element.
 */
boolean atmSig_processUserCellRate (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   uchar *frameptr;
   int length;
   atmSig_userCellRate *ucr;

   
   frameptr = (uchar *)hdr;
   length = GETSHORT(&hdr->length[0]); 

   /* check valid length */
   if ( !length  || (length > ATM_USERCELLRATE_MAXSIZE) ) {
      /* User Cell Rate IE has invalid length - fatal error!! */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   /* move to the meat of the IE!! */
   frameptr += ATMSIG_IEHDRSIZE;

   /* User Cell Rate IE can have the various Cell Rate's in ANY order */
   while (length) {

      switch (*frameptr) {

         /* Forward Peak Cell Rate (CLP = 0) */
         case ATM_OP_FWD_PEAK_CLP0:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.fwdPeakClp0 = GETLONG(&ucr->cellRate[0])
                                                    & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);             
            break;

         /* Backward Peak Cell Rate (CLP = 0) */ 
         case ATM_OP_BWD_PEAK_CLP0:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.bwdPeakClp0 = GETLONG(&ucr->cellRate[0])
                                                    & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);
            break;

         /* Forward Peak Cell Rate (CLP = 0 + 1) */
         case ATM_OP_FWD_PEAK_CLP01:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.fwdPeakClp01 = GETLONG(&ucr->cellRate[0])
                                                    & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);
            break;

         /* Backward Peak Cell Rate (CLP = 0 + 1) */ 
         case ATM_OP_BWD_PEAK_CLP01:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.bwdPeakClp01 = GETLONG(&ucr->cellRate[0]) 
                                                    & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);
            break;

         /* Forward Sustainable Cell Rate (CLP = 0) */ 
         case ATM_OP_FWD_SUS_CLP0:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.fwdSusClp0 = GETLONG(&ucr->cellRate[0]) 
                                                  & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate); 
            break;

         /* Backward Sustainable Cell Rate (CLP = 0) */ 
         case ATM_OP_BWD_SUS_CLP0:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.bwdSusClp0 = GETLONG(&ucr->cellRate[0])
                                                  & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);
            break;

         /* Forward Sustainable Cell Rate (CLP = 0 + 1) */
         case ATM_OP_FWD_SUS_CLP01:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.fwdSusClp01 = GETLONG(&ucr->cellRate[0])
                                                   & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);
            break;
 
         /* Backward Sustainable Cell Rate (CLP = 0 + 1) */ 
         case ATM_OP_BWD_SUS_CLP01:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.bwdSusClp01 = GETLONG(&ucr->cellRate[0])
                                                   & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);
            break;
  
         /* Forward Maximum Burst Size (CLP = 0) */ 
         case ATM_OP_FWD_BURST_CLP0:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.fwdMaxBurstClp0 = GETLONG(&ucr->cellRate[0])
                                                       & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);
            break;
 
         /* Backward Maximum Burst Size (CLP = 0) */
         case ATM_OP_BWD_BURST_CLP0:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.bwdMaxBurstClp0 = GETLONG(&ucr->cellRate[0])
                                                       & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);
            break;
 
         /* Forward Maximum Burst Size (CLP = 0 + 1) */ 
         case ATM_OP_FWD_BURST_CLP01:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.fwdMaxBurstClp01 = GETLONG(&ucr->cellRate[0])
                                                        & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);
            break;
 
         /* Backward Maximum Burst Size (CLP = 0 + 1) */ 
         case ATM_OP_BWD_BURST_CLP01:
            ucr = (atmSig_userCellRate *)frameptr;
            svc->tos.userCellRate.bwdMaxBurstClp01 = GETLONG(&ucr->cellRate[0])
                                                        & ATM_UCR_VALUE_SIZE;
            length -= sizeof(atmSig_userCellRate);
            if (length)
               frameptr += sizeof(atmSig_userCellRate);
            break;
 
         /* Best Effort Indicator */ 
         case ATM_UCR_BEST_EFFORT_IND:
            svc->tos.userCellRate.bestEffortIndicator = 1;
            --length;
            if (length)
               ++frameptr;
            break;

         /* Traffic Management Options Indicator */ 
         case ATM_UCR_TRMGMT_OPTIONS_ID:
            ++frameptr;
            --length;
            if (length) {
               if (*frameptr == ATM_UCR_TAGGING_FWD)
                  svc->tos.userCellRate.FwdTag = 1;
               if (*frameptr == ATM_UCR_TAGGING_BWD)
                  svc->tos.userCellRate.BwdTag = 1;
            }
            ++frameptr;
            --length; 
            break;

         /* Unknown Id - fatal error!! */
         default:
            return (ATMSIG_IE_FATAL_ERROR);

      }

   }

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s B b e a r e r C a p a b i l i t y ( )
 *
 * Process Broadband Bearer Capability Information Element.  Mandatory 
 * IE in SETUP messages.
 */
int atmSig_processBbearerCapability (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   ushort length;
   uchar *frameptr;


   frameptr = (uchar *)hdr;
   length = GETSHORT(&hdr->length[0]);

   /* IE info present? */
   if ( !length || (length > ATM_BBAND_BEARERCAP_MAXSIZE) ) {
      /* BroadBand Bearer Capability IE has invalid length - fatal error!! */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   /* move to the meat of the IE!! */
   frameptr += ATMSIG_IEHDRSIZE;

   /* Bearer Class */
   svc->tos.bearerClass =  *frameptr & ATM_BCLASS_MASK;
   --length;
   ++frameptr;

   /* Traffic Type & Timing Requirements should be present for Bearer Class X */
   if (svc->tos.bearerClass == ATM_BCLASS_BCOBX) {
      if (length) {
         svc->tos.trafficType = *frameptr & ATM_TRAFFICTYPE_MASK;
         svc->tos.timingRequirements = *frameptr & ATM_TIMINGREQ_MASK;
	 --length;
	 ++frameptr;
      }
      else {
         /* Traffic Type and Timing Requirements missing for Bearer Class X */
         return (ATMSIG_IE_FATAL_ERROR);
      }
   }

   /* more IE to process? still have clipping and user plane */
   if (!length)
     return (ATMSIG_IE_FATAL_ERROR);

   /* Susceptibility to clipping */
   svc->tos.clipping = *frameptr & ATM_CLIPPING_MASK;

   /* User Plane Access Configuration */
   svc->tos.usrPlaneAccessConfig = *frameptr & ATM_UPAC_MASK;

   --length;

   /* No more IE bytes should be present at this time.  If present, fatal error!! */
   if (length) 
      return (ATMSIG_IE_FATAL_ERROR);
   else
      return (ATMSIG_IE_OK);


}



/*
 * a t m S i g _ p r o c e s s B r o a d b a n d H i g h L a y e r I n f o ( )
 *
 * Process Broadband High Layer Information Element.
 * The purpose of this IE is to provide a means which should be used for compatibility
 * checking by an addressed entity.  This IE is transparently by the ATM network
 * between the call originating entity and the addressed entity.  For the Public UNI
 * the availability of this IE must be negotiated with the network provider.  At
 * the Public UNI, the network provider has option of NOT supporting this IE in the
 * SETUP message.  At the Private UNI, support for this IE is mandatory.
 */
int atmSig_processBroadbandHighLayerInfo (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   uchar *frameptr;
   ushort length;
   ushort i = 0;


   frameptr = (uchar *)hdr;

   /* sanity check - should never happen!! */
   if (hdr->identifier != ATM_BROADBAND_HIGHLAYER_IE)
      return (ATMSIG_IE_FATAL_ERROR);

   length = GETSHORT(&hdr->length[0]);

   /* valid length? */
   if ( !length || (length > ATM_BBAND_HILAYER_MAXSIZE) ) {
      /* invalid length present in IE - non fatal - log it */
      return (ATMSIG_IE_OK);
   }

   /* High Layer Information Type */
   svc->tos.broadBandHLType = *frameptr & ATMSIG_EXTENSION_MASK;
   --length;

   /* more IE? */
   if (!length) {
      /* High Layer Info missing - non-fatal - log it */
      return (ATMSIG_IE_OK);
   }

   /* High Layer Info should not exceed 8 bytes - truncate if longer! */
   if (length > 8) {
      /* info truncated - log it!! */
      length = 8;
   }

   /* copy High Layer Information into SVC */
   while (length--) 
      svc->tos.highLayerInfo[i++] = (*frameptr)++;

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s B r o a d b a n d L o w L a y e r I n f o ( )
 *
 * Process Broadband Lower Layer Information Element.
 * The logic here is to simply pass the entire IE (except header) to the
 * client since BLL-IE info is only meaningful to Sig clients.  The Sig
 * client can decide to terminate the call if there's something in BLL-IE
 * which it does not like.
 */
boolean atmSig_processBroadbandLowLayerInfo (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   ushort length;
   uchar *frameptr;
   int i; 


   frameptr = (uchar *)hdr;

   /* sanity check - should never happen!! */
   if (hdr->identifier != ATM_BROADBAND_LOWLAYER_IE)
      return (ATMSIG_IE_FATAL_ERROR);

   length = GETSHORT(&hdr->length[0]);

   /* valid length? */
   if ( !length || (length > ATM_BBAND_LOLAYER_MAXSIZE) ) {
      /* invalid length in Broadband Lower Layer IE - log it */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   frameptr += ATMSIG_IEHDRSIZE;

   /* empty IE contents in svc's blli field */
   svc->blli[0].length = GETSHORT(&hdr->length[0]);
   for (i = 0; i < length; ++i)
   svc->blli[0].string[i] = *frameptr++;

  if (length >= ATMSIG_LLAYER3_LENGTH)
    svc->aal5mux = TRUE;

   return (ATMSIG_IE_OK); 

}



/*
 * a t m S i g _ p r o c e s s B b a n d R e p e a t I n d i c a t o r ( )
 *
 * Process Broadband Repeat Indicator Information Element.
 */
boolean atmSig_processBbandRepeatIndicator (
                                  atmSvcType *svc, atmSig_ieHeader *hdr)
{

   uchar *frameptr;
   ushort length = 0;


   frameptr = (uchar *)hdr;

   length = GETSHORT(&hdr->length[0]);

   /* valid length? */
   if ( !length || (length > ATM_REPEAT_IND_MAXSIZE) ) {
      /* invalid length in Repeat Indicator IE - log it */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   frameptr += ATMSIG_IEHDRSIZE;

   if ( *frameptr != (ATMSIG_EXTENSION_BIT | ATMSIG_REPEAT_INDICATION) )
      return (ATM_CAUSE_INVALID_IE_CONTENTS);

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s C a l l e d P a r t y N u m b e r ( )
 *
 * Process Called Party Number Information Element.
 * Identifies the destination of the call.  Mandatory in SETUP messages.
 */
int atmSig_processCalledPartyNumber (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   uchar *frameptr;
   ushort length = 0;
   ushort i = 0;


   frameptr = (uchar *)hdr;

   /* sanity check - should never happen!! */
   if (hdr->identifier != ATM_CALLEDPARTYIE_ID)
      return (ATMSIG_IE_FATAL_ERROR);

   length = GETSHORT(&hdr->length[0]);

   /* valid length? */
   if ( !length || (length > ATM_CALLEDPARTY_MAXSIZE) ) {
      /* invalid length in Called Party Number IE - log it */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   frameptr += ATMSIG_IEHDRSIZE;

   /* Type of number */
   svc->tos.numberType = *frameptr & ATM_CALLEDPARTY_NUM_MASK;

   /* Addressing/numbering plan identification */
   svc->tos.numberingPlan = *frameptr & ATM_CALLEDPARTY_NUMPLAN_MASK;

   --length;
   ++frameptr;

   /* Called Party Number present? */
   if (!length) {
      /* Called Party Number absent - log it!! */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   /* extract the Called Party Number for comparison with interface NSAP addr */
   for (i = 0; i < length; ++i, ++frameptr)
      svc->srcNsap.addr[i] = *frameptr;

   svc->srcNsap.length = ATM_NSAP_ADDR_LNGTH;

   return (ATMSIG_IE_OK);


}



/*
 * a t m S i g _ p r o c e s s C a l l e d P a r t y S u b A d d r ( )
 *
 * Process Called Party Subaddress Information Element.
 */
boolean atmSig_processCalledPartySubAddr (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   uchar *frameptr;
   ushort i, length;


   frameptr = (uchar *)hdr;

   length = GETSHORT(&hdr->length[0]);

   /* valid length? */
   if ( !length || (length > ATM_CALLEDPARTY_SUBADDR_MAXSIZE) ) {
      /* invalid length in Called Party Subaddress IE - log it */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   /* move to the meat of the IE! */
   frameptr += ATMSIG_IEHDRSIZE;

   /* Type of Subaddress */
   svc->subAddressType = *frameptr & ATMSIG_EXTENSION_MASK;

   --length;

   /* more IE? */
   if (!length) {
      /* Called Party Subaddress missing - log it!! */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   ++frameptr;

   svc->callingPartySubAddrLngth = length;

   /* store the Called Party Subaddress in the SVC block */
   for (i = 0; i < length; ++i, ++frameptr);
      svc->callingPartySubaddress[i] = *frameptr;

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s C a l l i n g P a r t y N u m b e r ( )
 *
 * Process Calling Party Number Information Element.
 * Identifies the origin of the call.
 */
int atmSig_processCallingPartyNumber (atmSvcType *svc, atmSig_ieHeader *hdr, 
				      atmSvcPartyType *party)
{

   uchar *frameptr;
   ushort length = 0;
   ushort i = 0;
   boolean preIndPresent;


   frameptr = (uchar *)hdr;

   /* sanity check - should never happen!! */
   if (hdr->identifier != ATM_CALLINGPARTYIE_ID)
      return (ATMSIG_IE_FATAL_ERROR);

   length = GETSHORT(&hdr->length[0]);

   /* valid length? */
   if ( !length || (length > ATM_CALLINGPARTY_MAXSIZE) ) {
      /* invalid length in Calling Party Number IE - log it */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   /* move to the meat of the IE! */
   frameptr += ATMSIG_IEHDRSIZE;

   /* Type of number */
   svc->tos.numberType = *frameptr & ATM_CALLEDPARTY_NUM_MASK;

   /* Addressing/Numbering plan identification */
   svc->tos.numberingPlan = *frameptr & ATM_CALLEDPARTY_NUMPLAN_MASK;

   /* _next_ byte i.e. Presentation/Screening Indicator byte present? */
   if (*frameptr & ATMSIG_EXTENSION_BIT)
      preIndPresent = FALSE;
   else
      preIndPresent = TRUE;

   --length;
   ++frameptr;

   /* more IE? */
   if (!length) {
      /* Calling Party Address missing - log it!! */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   if (preIndPresent) {
      /* Presentation Indicator */
      svc->tos.presentationIndicator = *frameptr & ATM_CALLINGPARTYIE_PRESIND_MASK;

      /* Screening Indicator */
      svc->tos.screeningIndicator = *frameptr & ATM_CALLINGPARTYIE_SCREENIND_MASK;
   
      --length;
      ++frameptr;

      /* more IE? */
      if (!length) {
         /* Calling Party Address missing - log it!! */
         return (ATMSIG_IE_FATAL_ERROR);
      }
   }

   party->destNsap.length = ATM_NSAP_ADDR_LNGTH;
   party->destNsap.type = STATION_ATMNSAP;

   /* store Calling Party Number in svc block */
   for (i = 0; i < length; ++i, ++frameptr)
      party->destNsap.addr[i] = *frameptr;

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s C a l l i n g P a r t y S u b A d d r ( )
 *
 * Process Calling Party Subaddress Information Element.
 */
boolean atmSig_processCallingPartySubAddr (atmSvcType *svc, atmSig_ieHeader *hdr, 
					   atmSvcPartyType *party)
{

   uchar *frameptr;
   ushort i, length;


   frameptr = (uchar *)hdr;

   length = GETSHORT(&hdr->length[0]);

   /* valid length? */
   if ( !length || (length > ATM_CALLINGPARTY_SUBADDR_MAXSIZE) ) {
      /* invalid length in Calling Party Subaddress IE - log it */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   /* move to the meat of the IE! */
   frameptr += ATMSIG_IEHDRSIZE;

   /* Type of subaddress */
   svc->subAddressType = *frameptr & ATMSIG_EXTENSION_MASK;

   --length;

   /* more IE? */
   if (!length) {
      /* Calling Party Subaddress missing - log it!! */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   ++frameptr;

   party->calledPartySubAddrLngth = length;

   /* store the Calling Party Subaddress in the SVC */
   for (i = 0; i < length; ++i, ++frameptr);
      party->calledPartySubaddress[i] = *frameptr;

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s C o n n e c t i o n I d e n t i f e r ( )
 *
 * Process Conncetion Identifier Information Element.
 * Mandatory in SETUP, CALL PROCEEDING and CONNECT messages.
 */
int atmSig_processConnectionIdentifier (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   uchar *frameptr;
   atmSig_connectionIdIe *connId;
   ushort length;
   vc_info_t *vc_info;

   frameptr = (uchar *)hdr;

   /* sanity check - this should NEVER happen!! */
   if (hdr->identifier != ATM_CONNECTIONIDENTIFIER_IE)
      return (ATMSIG_IE_FATAL_ERROR);

   /* Connection Identifier IE length */
   length = GETSHORT(&hdr->length[0]);

   /* Message Compatibility Instruction Indicator */
   if (hdr->cii != ATMSIG_CII) {
      /* invalid cii - should log it!! */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   if (length == ATMSIG_CONNECTIONID_IESIZE) { 
      frameptr += ATMSIG_IEHDRSIZE;
      connId = (atmSig_connectionIdIe *)frameptr;
      svc->tos.assocSig = connId->assoc_sig;
      svc->vpci = GETSHORT(&connId->vpci[0]);
      svc->vci = GETSHORT(&connId->vci[0]);

      /*
       * this should also apply to 3.0  code once we are confident
       * it works
       */
      /* check whether or not vpci/vci are not used. see 5.5.2.3 */
      if ( atmSig_isV31(svc) ) {
	if ( (vc_info = atm_vpivci_to_vc(svc->sig_idb->hwptr->atm_db,
				     svc->vpci,svc->vci)) != NULL) {
	  if (atmSig_debugAll || atmSig_debugError)
	    buginf("\nATMSIG: vpci %d, vci %d are used",svc->vpci,svc->vci);
	  return (ATMSIG_IE_VPCI_REUSED);
	}
      }
   }
   else {
      /* log - invalid Connection Identifier IE */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s Q o s P a r a m e t e r ( )
 *
 * Process Quality of Service Information Element.
 * The purpose of QOS is to request and indicate the Quality of Service
 * Class for a connection.  Mandatory in SETUP messages.
 */
int atmSig_processQosParameter (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   ushort length;
   uchar *frameptr;
   atmSig_qosIe *qos;


   /* IE header */
   frameptr = (uchar *)hdr;
 
   length = GETSHORT(&hdr->length[0]);

   /* sanity check - should NEVER happen! */
   if (hdr->identifier != ATM_QOS_IE)
      return (ATMSIG_IE_FATAL_ERROR);

   /* QOS must have 2 bytes of info in it's IE. */ 
   length = GETSHORT(&hdr->length[0]);
   if ( length != ATMSIG_QOS_IESIZE ) {
      /* invalid QOS length */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   frameptr += ATMSIG_IEHDRSIZE;
   qos = (atmSig_qosIe *)frameptr;

   if ( atmSig_isV31(svc) ) {
     char codingStandard;
     char Qos;
     int result;

     codingStandard = (hdr->cii) & ATMSIG_IECODING_MASK;
     Qos = qos->qosFwd & ATMSIG_EXTENSION_MASK; 
     if ((result=atmSig_checkQosClass(codingStandard, Qos)) != ATMSIG_IE_OK)
       return (result);
     svc->tos.rcvQosClassFwd = Qos;
     Qos = qos->qosBackwd & ATMSIG_EXTENSION_MASK;

     /* now process the backward qos */
     if ((result=atmSig_checkQosClass(codingStandard, Qos)) != ATMSIG_IE_OK)
       return (result);
     Qos = qos->qosBackwd & ATMSIG_EXTENSION_MASK;

     return (ATMSIG_IE_OK);      
   }

   /* extract the Forward and Backward QOS parameters and save in SVC */
   svc->tos.rcvQosClassFwd = qos->qosFwd & ATMSIG_EXTENSION_MASK; 
   svc->tos.rcvQosClassBackwd = qos->qosBackwd & ATMSIG_EXTENSION_MASK;


   return (ATMSIG_IE_OK); 

}



/*
 * a t m S i g _ p r o c e s s B r o a d b a n d S e n d C m p l t ( )
 *
 * Process Broadband Send Complete Information Element.
 * Indicate completion of the called party number - provides compatibility
 * with certain public networks.  It is optional for user equipment to send,
 * shall be ignored by network equipment when received.
 */
int atmSig_processBroadbandSendCmplt (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   ushort length;
   uchar *frameptr;


   /* IE header */
   frameptr = (uchar *)hdr;

   length = GETSHORT(&hdr->length[0]);

   /* sanity check - should NEVER happen! */  
   if (hdr->identifier != ATM_BBAND_SENDCMPLTIE_ID)
      return (ATMSIG_IE_FATAL_ERROR);

   /* ONLY one byte of information for this IE */
   length = GETSHORT(&hdr->length[0]);
   if (length != 1) 
      return (ATMSIG_IE_FATAL_ERROR);

   /* move to the meat of the IE */
   frameptr += ATMSIG_IEHDRSIZE;

   /* IE content */
   if (*frameptr != (ATM_BBAND_SENDCMPLT_IND | ATMSIG_EXTENSION_BIT))
      return (ATMSIG_IE_FATAL_ERROR);

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s T r a n s i t N t w r k S e l ( )
 *
 * Process Transit Network Selection Information Element.
 * Identifies one requested transit network.
 */
int atmSig_processTransitNtwrkSel (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   uchar *frameptr;
   int i, length;


   frameptr = (uchar *)hdr;

   length = GETSHORT(&hdr->length[0]);

   if (!length)
      return (ATMSIG_IE_OK);

   if (length > ATM_TRANSIT_NTWRKSEL_MAXSIZE)
      return (ATMSIG_IE_FATAL_ERROR);
 
   frameptr += ATMSIG_IEHDRSIZE;

   /* Note - this check takes into account the extension bit */ 
   if ( (*frameptr & (ATM_TRANSIT_NETID | ATM_TRANSIT_NETID_PLAN)) !=
                            (ATM_TRANSIT_NETID | ATM_TRANSIT_NETID_PLAN) )
      return (ATMSIG_IE_FATAL_ERROR);

   --length;

   /* check if Network Identfication field exists */
   if (!length)
      return (ATMSIG_IE_OK); 

   ++frameptr;

   /* 
    * For now we assume that the Network identification length is no more
    * than 20.  We could change this if we need to.
    */ 
   if (length > ATM_TRANSIT_NETID_LENGTH)
      length = ATM_TRANSIT_NETID_LENGTH;

   svc->netIdLength = length;

   for (i = 0; i < length; ++i, ++frameptr)
      svc->netId[i] = *frameptr;

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s C a l l S t a t e ( )
 * 
 * Process the Call State Information Element.  
 * The Call State IE contains the state of the SVC.  A higher level application 
 * may need this info which is contained in the "state" variable.  Mandatory in
 * STATUS messages. 
 */
int atmSig_processCallState (atmSvcType *svc, atmSig_ieHeader *hdr, int *state)
{

   ushort length;
   uchar *frameptr;

 
   frameptr = (uchar *)hdr;

   /* Call State IE should have ONLY one byte of information */ 
   length = GETSHORT(&hdr->length[0]);
   if (length != ATMSIG_CALLSTATE_IESIZE) {
      /* Invalid length of Call State IE! */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   frameptr += ATMSIG_IEHDRSIZE;
   *state = *frameptr;

   return (ATMSIG_IE_OK); 

}



/*
 * a t m S i g _ p r o c e s s C a u s e ( )
 *
 * Process Cause Information Element.  
 * Describes the reason for generating certain messages, providing diagnostic 
 * information in the event of procedural errors and indicates the location of
 * the cause originator.
 * Mandatory in STATUS messages.
 */
int atmSig_processCause (atmSvcType *svc, atmSig_ieHeader *hdr)
{

   uchar *frameptr;
   ushort length;

   
   frameptr = (uchar *)hdr;

   /* sanity check - should never happen!! */
   if (hdr->identifier != ATM_CAUSE_IE)
      return (ATMSIG_IE_FATAL_ERROR);

   length = GETSHORT(&hdr->length[0]); /* length does not include IE header */

   if ( !length || (length > ATM_CAUSE_MAXSIZE) ) {
      /* Cause IE invalid length - log it */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   /* move to the meat of the IE! */
   frameptr += ATMSIG_IEHDRSIZE;

   /* Location byte - mask of the extension bit */
   svc->causeLocation = *frameptr & ATMSIG_EXTENSION_MASK;
   --length;

   /* Cause Value present? */
   if (!length) {
      /* No Cause Value - log it!! */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   /* Cause Value - mask of the extension bit */
   ++frameptr;
   svc->causeValue = *frameptr & ATMSIG_EXTENSION_MASK;

   --length;
  
   if (atmSig_debugAll || atmSig_debugEvent) {
      buginf("\nATMSIG: cause = %s, vcnum = %d, call ref = %d",
          atmSig_printCauseValue(svc->causeValue), svc->vcnum, svc->callRef);
   }

   /* Diagnostic present? - ok if not there */ 
   if (!length) {
      return (ATMSIG_IE_OK);
   }

   /* Diagnostic info present? */


   return (ATMSIG_IE_OK);


}



/*
 * a t m S i g _ p r o c e s s R e s t a r t I n d i c a t o r ( )
 *
 * Process Restart Indicator Information Element. 
 * The purpose of this IE is to identify the class of the facility to be
 * restarted.  Only the indicated VC or ALL VC's controlled by the
 * Signalling entity would need to be restarted.
 */
int atmSig_processRestartIndicator (atmSvcType *svc, atmSig_ieHeader *ieHdr)
{

   ushort length;
   uchar *frameptr;


   frameptr = (uchar *)ieHdr;

   length = GETSHORT(&ieHdr->length[0]);
   if (length != ATMSIG_RESTARTINDICATOR_IESIZE) {
      /* Incorrect size of Restart Indicator IE - log it */
      return (ATMSIG_IE_FATAL_ERROR);
   }

   frameptr += ATMSIG_IEHDRSIZE;
   svc->restartClass = *frameptr & ATM_RESTART_CLASS_MASK & ATMSIG_EXTENSION_MASK;

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s E n d P o i n t R e f e r e n c e ( )
 *
 * Process End Point Reference Information Element. 
 * Mandatory in Multipoint and SETUP Messages for Multipoint calls.
 * Currently not used as point to multipoint support is not required.
 */
int atmSig_processEndPointReference (atmSvcType *svc, atmSig_ieHeader *ieHdr)
{

   uchar *frameptr;
   ushort length;
   atmSig_endpointRefIe *endpointRef;


   frameptr = (uchar *)ieHdr;

   /* sanity check - should never happen!! */
   if (ieHdr->identifier != ATM_ENDPOINT_REFERENCE_IE)
      return (ATMSIG_IE_FATAL_ERROR);

   length = GETSHORT(&ieHdr->length[0]);

   if (length != ATMSIG_ENDPOINT_REF_IESIZE)
      return (ATMSIG_IE_FATAL_ERROR);

   frameptr += ATMSIG_IEHDRSIZE;
   endpointRef = (atmSig_endpointRefIe *)frameptr;

   svc->msgEndpointRef = 
     (GETSHORT(&endpointRef->endpointRefId[0]) ) &  ATMSIG_ENDPOINT_REF_MASK;

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ p r o c e s s E n d P o i n t S t a t e ( )
 *
 * End Point State IE is used for point to multipoint signalling.  Currently
 * not supported.  Needed for the Multicast Server (NBMA) project. 
 * Currently not used as point to multipoint support is not required.
 */
int atmSig_processEndPointState(atmSvcType *svc, atmSig_ieHeader *ieHdr, 
				atmSvcPartyType *party)
{

   uchar *frameptr;
   ushort length;


   frameptr = (uchar *)ieHdr;

   /* sanity check - should never happen!! */
   if (ieHdr->identifier != ATM_ENDPOINT_STATE_IE)
      return (ATMSIG_IE_FATAL_ERROR);

   length = GETSHORT(&ieHdr->length[0]);

   if (length != ATMSIG_ENDPOINT_STATE_IESIZE)
      return (ATMSIG_IE_FATAL_ERROR);

   frameptr += ATMSIG_IEHDRSIZE;
   party->partyState = *frameptr & ATMSIG_ENDPOINT_STATE_IE_MASK;

   return (ATMSIG_IE_OK);

}



/*
 * a t m S i g _ b u i l d U s e r C e l l R a t e ( )
 *
 * Build User Cell Rate Information Element.
 * 'size' is the IE byte length (excluding the IE header). 
 */
void atmSig_buildUserCellRate (atmSvcType *svc, uchar *frameptr, int size)
{

   atmSig_ieHeader *hdr;
   atmSig_userCellRate *ucr = NULL;
   boolean offset_moved;


   offset_moved = FALSE;

   /* IE Header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_USERCELLRATE_IE;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],size);

   frameptr += ATMSIG_IEHDRSIZE;

   /* IE contents */

   /* 
    * no type of service specified - make it a best effort connection.
    * Best effort requires us to specify fwd & bwd peak cell rates CLP 0 + 1.
    * Specify dummy values to satisfy the spec that something should be put in,
    * unless they were specified.
    * If multipoint, zero bkwdPeakClp01 
    */
   if ( (svc->tos.userCellRate.bestEffortIndicator) && 
                                   (size == ATM_UCR_BESTEFFORT_LNGTH) ) {
       /* If best-effort, we now signal all-ones, so no need to assign */
       /* a value (default in tos is -1). */
      if (svc->multiPoint) 
	svc->tos.userCellRate.bwdPeakClp01 = 0;

      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], ( ATM_UCR_FWD_PEAK_CLP01 |
                 (svc->tos.userCellRate.fwdPeakClp01 & ATM_UCR_VALUE_SIZE) ));
      frameptr += sizeof(atmSig_userCellRate);
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], ( ATM_UCR_BWD_PEAK_CLP01 |
                    (svc->tos.userCellRate.bwdPeakClp01 & ATM_UCR_VALUE_SIZE) ));
      frameptr += sizeof(atmSig_userCellRate); 
      *frameptr = ATM_UCR_BEST_EFFORT_IND; 
      return;
   }

   /* Forward Peak Cell Rate (CLP = 0) configured? */
   if (svc->tos.userCellRate.fwdPeakClp0 != -1) {
      ucr = (atmSig_userCellRate *)frameptr;
      offset_moved = TRUE;
      PUTLONG(&ucr->cellRate[0], ( ATM_UCR_FWD_PEAK_CLP0 |
                 (svc->tos.userCellRate.fwdPeakClp0 & ATM_UCR_VALUE_SIZE) ));
   }
  
   /* Backward Peak Cell Rate (CLP = 0) configured? */ 
   if (svc->tos.userCellRate.bwdPeakClp0 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE;
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], ( ATM_UCR_BWD_PEAK_CLP0 |
                   (svc->tos.userCellRate.bwdPeakClp0 & ATM_UCR_VALUE_SIZE) ));
   }
 
   /* Forward Peak Cell Rate (CLP = 0 + 1) configured? */ 
   if (svc->tos.userCellRate.fwdPeakClp01 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE;
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], ( ATM_UCR_FWD_PEAK_CLP01 |
                (svc->tos.userCellRate.fwdPeakClp01 & ATM_UCR_VALUE_SIZE) ));
   }

   /* Backward Peak Cell Rate (CLP = 0 + 1) configured? */ 
   if (svc->tos.userCellRate.bwdPeakClp01 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE; 
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], ( ATM_UCR_BWD_PEAK_CLP01 |
               (svc->tos.userCellRate.bwdPeakClp01 & ATM_UCR_VALUE_SIZE) ));
   }

   /* Forward Sustainable Cell Rate (CLP = 0) configured? */ 
   if (svc->tos.userCellRate.fwdSusClp0 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE;
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], ( ATM_UCR_FWD_SUS_CLP0 |
                (svc->tos.userCellRate.fwdSusClp0 & ATM_UCR_VALUE_SIZE) )); 
   }

   /* Backward Sustainable Cell Rate (CLP = 0) configured? */ 
   if (svc->tos.userCellRate.bwdSusClp0 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE; 
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], (ATM_UCR_BWD_SUS_CLP0 |
                 (svc->tos.userCellRate.bwdSusClp0 & ATM_UCR_VALUE_SIZE) ));
   }

   /* Forward Sustainable Cell Rate (CLP = 0 + 1) configured? */
   if (svc->tos.userCellRate.fwdSusClp01 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE; 
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], (ATM_UCR_FWD_SUS_CLP01 |
              (svc->tos.userCellRate.fwdSusClp01 & ATM_UCR_VALUE_SIZE) ));
   }

   /* Backward Sustainable Cell Rate (CLP = 0 + 1) configured? */
   if (svc->tos.userCellRate.bwdSusClp01 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE; 
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], (ATM_UCR_BWD_SUS_CLP01 |
                (svc->tos.userCellRate.bwdSusClp01 & ATM_UCR_VALUE_SIZE) ));
   }

   /* Forward Maximum Burst Size (CLP = 0) configured? */ 
   if (svc->tos.userCellRate.fwdMaxBurstClp0 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE; 
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], (ATM_UCR_FWD_BURST_CLP0 |
               (svc->tos.userCellRate.fwdMaxBurstClp0 & ATM_UCR_VALUE_SIZE) ));
   }

   /* Backward Maximum Burst Size (CLP = 0) configured? */ 
   if (svc->tos.userCellRate.bwdMaxBurstClp0 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE; 
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], (ATM_UCR_BWD_BURST_CLP0 |
               (svc->tos.userCellRate.bwdMaxBurstClp0 & ATM_UCR_VALUE_SIZE) ));
   }
 
   /* Forward Maximum Burst Size (CLP = 0 + 1) configured? */ 
   if (svc->tos.userCellRate.fwdMaxBurstClp01 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE; 
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], (ATM_UCR_FWD_BURST_CLP01 |
               (svc->tos.userCellRate.fwdMaxBurstClp01 & ATM_UCR_VALUE_SIZE) ));
   }

   /* Backward Maximum Burst Size (CLP = 0 + 1) configured? */ 
   if (svc->tos.userCellRate.bwdMaxBurstClp01 != -1) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      offset_moved = TRUE; 
      ucr = (atmSig_userCellRate *)frameptr;
      PUTLONG(&ucr->cellRate[0], (ATM_UCR_BWD_BURST_CLP01 |
               (svc->tos.userCellRate.bwdMaxBurstClp01 & ATM_UCR_VALUE_SIZE) ));
   }

   /* Traffic Management Options configured? */
   if ( (svc->tos.userCellRate.FwdTag != -1) || 
                          (svc->tos.userCellRate.FwdTag != -1) ) {
      frameptr += offset_moved ? sizeof(atmSig_userCellRate) : 0;
      *frameptr++ = ATM_UCR_TRMGMT_OPTIONS_ID;
      *frameptr = (ATM_UCR_TAGGING_FWD | ATM_UCR_TAGGING_BWD);
   }

}



/*
 * a t m S i g _ b u i l d B B e a r e r C a p a b i l i t y ( )
 *
 * Build BroadBand Bearer Capability Information Element.
 * The purpose of the Broadband bearer capability IE is to indicate a 
 * requested broadband connection oriented bearer service (see CCITT
 * Recommendation F.811) to be provided by the network.
 */
void atmSig_buildBBearerCapability (atmSvcType *svc, uchar *frameptr)
{

   atmSig_ieHeader *hdr;
   atmSig_bBearerCapability *broad;

   
   /* IE Header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_BROADBAND_BEARERCAP_IE;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],ATMSIG_BBEARERCAP_IESIZE); 

   frameptr += ATMSIG_IEHDRSIZE;
   broad = (atmSig_bBearerCapability *)frameptr;
 
   broad->bearerClass = ATM_BCLASS_BCOBX;
   broad->trafficType = ATMSIG_EXTENSION_BIT; 

   if (svc->multiPoint)
     broad->clipping = svc->tos.clipping | ATMSIG_MULTIPOINT | 
                                                        ATMSIG_EXTENSION_BIT;
   else
     broad->clipping = svc->tos.clipping | ATMSIG_EXTENSION_BIT;

}



/*
 * a t m S i g _ b u i l d C a l l e d P a r t y N u m b e r ( )
 *
 * Build Called Party Number Information Element.
 * The purpose of the Called Party number IE is to identify the called
 * party of a call.
 */
void atmSig_buildCalledPartyNumber (atmSvcType *svc, uchar *frameptr,
				    atmSvcPartyType *party)
{

   atmSig_ieHeader *hdr;
   int i;


   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_CALLEDPARTY_NUMBER_IE;
   hdr->cii = ATMSIG_CII;

   /* 1 => Type of number and Addressing plan */ 
   PUTSHORT(&hdr->length[0],ATMSIG_CALLEDPARTY_IESIZE);

   frameptr += ATMSIG_IEHDRSIZE;

   /* Type of Number/Numbering Plan Id */
   *frameptr = ATM_CALLEDPARTY_NUMPLAN_NSAP | ATMSIG_EXTENSION_BIT;
   ++frameptr;

   /* copy called party address from svc into the message */
   for (i = 0; i < STATIONLEN_ATMNSAP; ++i, ++frameptr)
      *frameptr = party->destNsap.addr[i];

}



/*
 * a t m S i g _ b u i l d C a l l i n g P a r t y N u m b e r ( )
 *
 * Build Calling Party Number Information Element.
 * The purpose of this IE is to identify the origin of the call.
 */
void atmSig_buildCallingPartyNumber (atmSvcType *svc, uchar *frameptr)
{

   atmSig_ieHeader *hdr;
   int i;


   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_CALLINGPARTY_NUMBER_IE;
   hdr->cii = ATMSIG_CII;

   /* 2 => Type of number/Addr Plan + Presentation Ind/Screening Ind */
   PUTSHORT(&hdr->length[0],ATMSIG_CALLINGPARTY_IESIZE);

   frameptr += ATMSIG_IEHDRSIZE;

   *frameptr = ATM_CALLEDPARTY_NUMPLAN_NSAP;
   ++frameptr;

   *frameptr = svc->tos.presentationIndicator | svc->tos.screeningIndicator
                                                   | ATMSIG_EXTENSION_BIT;
   ++frameptr;

   /* copy nsap address of source into the IE */
   for (i = 0; i < ATM_NSAP_ADDR_LNGTH; ++i, ++frameptr)
      *frameptr = svc->srcNsap.addr[i];

}



/*
 * a t m S i g _ b u i l d Q o s P a r a m e t e r ( )
 *
 * Build Quality of Service Parameter Information Element.
 */
void atmSig_buildQosParameter (atmSvcType *svc, uchar *frameptr)
{

   atmSig_ieHeader *hdr;
   atmSig_qosIe *qos;


   /* IE header */ 
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_QOS_IE;

   frameptr += ATMSIG_IEHDRSIZE;
  
   /* IE contents */ 
   qos = (atmSig_qosIe *)frameptr;
   qos->qosFwd = svc->tos.txQosClassFwd;
   qos->qosBackwd = svc->tos.txQosClassBackwd;
   if ( atmSig_isV31(svc) ) {
     if (qos->qosFwd == ATM_QOSIE_UNSPECD || 
	 qos->qosBackwd == ATM_QOSIE_UNSPECD)
       hdr->cii = ATMSIG_QOS_CII_ITU;   
     else
       hdr->cii = ATMSIG_QOS_CII;     
   }
   else
     hdr->cii = ATMSIG_QOS_CII;     

   PUTSHORT(&hdr->length[0],sizeof(atmSig_qosIe));
}



/*
 * a t m S i g _ b u i l d A a l T y p e 3 4 ( )
 * 
 * Build AAL Type 3/4 Information Element.
 */
void atmSig_buildAalType34 (atmSvcType *svc, uchar *frameptr)
{

   atmSig_ieHeader *hdr;
   atmSig_aalType34Ie *aal34;

   
   hdr = (atmSig_ieHeader *)frameptr;

   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_AAL_IE;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],sizeof(atmSig_aalType34Ie));

   frameptr += ATMSIG_IEHDRSIZE;
   aal34 = (atmSig_aalType34Ie *)frameptr;

   /* AAL Type */
   aal34->aalType = ATM_AALIE_TYPE34;

   /* forward maxmimum CPCS-SDU size */
   aal34->fwdSduSizeId = ATM_AALIE_FWD_SDUSZ_ID;
   PUTSHORT(&aal34->fwdSdu[0], svc->tos.fwdSduSize);

   /* backward maxmimum CPCS-SDU size */
   aal34->bwdSduSizeId = ATM_AALIE_BWD_SDUSZ_ID;
   PUTSHORT(&aal34->bwdSdu[0], svc->tos.bwdSduSize);

   /* SSCS type */
   aal34->sscsTypeId = ATM_AALIE_SSCSTYPE_ID;
   aal34->sscsType = svc->tos.sscsType;
   
   /* MID common fields */
   aal34->midSizeId_midRangeId = ATM_AALIE_MID_SIZE_ID;
   aal34->midSize_lowestMid = svc->tos.svcTos_lowestMid;

   if ( atmSig_isV31(svc) ) {
     aal34->modeId_highestMidHi = svc->tos.highestMid >> 8;
     aal34->mode_highestMidLo = svc->tos.highestMid;
   }
   else { /* must be 3.0 */
     /* Mode */
     aal34->modeId_highestMidHi = ATM_AALIE_MODE_ID;
     aal34->mode_highestMidLo = svc->tos.mode;
   }


}



/*
 * a t m S i g _ b u i l d A a l T y p e 5 ( )
 *
 * Build AAL Type 5 Information Element.
 */
ushort atmSig_buildAalType5 (atmSvcType *svc, uchar *frameptr)
{

   atmSig_ieHeader *hdr;
   atmSig_aalType5Ie *aal5;
   int size;

   hdr = (atmSig_ieHeader *)frameptr;

   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_AAL_IE;
   hdr->cii = ATMSIG_CII;

   frameptr += ATMSIG_IEHDRSIZE;
   aal5 = (atmSig_aalType5Ie *)frameptr;

   /* AAL Type */
   aal5->aalType = ATM_AALIE_TYPE5;
   size = 1;

   /* forward maxmimum CPCS-SDU size */
   aal5->fwdSduSizeId = ATM_AALIE_FWD_SDUSZ_ID;
   PUTSHORT(&aal5->fwdSdu[0], svc->tos.fwdSduSize);

   /* backward maxmimum CPCS-SDU size */
   aal5->bwdSduSizeId = ATM_AALIE_BWD_SDUSZ_ID;
   PUTSHORT(&aal5->bwdSdu[0], svc->tos.bwdSduSize);
   
   size += 6;
   /* for uni 3.1, there's no mode-id & mode. we push the
      sscs type up 2 bytes so that we don't need a new data
      structure */
   if ( atmSig_isV31(svc) ) {
     /* SSCS type */
     aal5->modeId_sscsId = ATM_AALIE_SSCSTYPE_ID;
     aal5->mode_sscs = svc->tos.sscsType;
     size += 2;
   }
   else {
     if (svc->tos.mode !=  ATM_AALIE_MODE_INVALID) {
       aal5->modeId_sscsId = ATM_AALIE_MODE_ID;
       aal5->mode_sscs = svc->tos.mode;
       /* SSCS type */
       aal5->sscsId = ATM_AALIE_SSCSTYPE_ID;
       aal5->sscs = svc->tos.sscsType;
       size += 4;
     }
     else {
       aal5->modeId_sscsId = ATM_AALIE_SSCSTYPE_ID;
       aal5->mode_sscs = svc->tos.sscsType;
       size += 2;
     }
   }

   PUTSHORT(&hdr->length[0],size);
   return (size);
}





/*
 * a t m S i g _ b u i l d B r o a d b a n d S e n d C m p l t ( )
 *
 * Build BroadBand Send Completion Information Element.
 * Indicate completion of the called party number - provides compatibility
 * with certain public networks.  It is optional for user equipment to send,
 * shall be ignored by network equipment when received.
 */
void atmSig_buildBroadbandSendCmplt (atmSvcType *svc, uchar *frameptr)
{

   atmSig_ieHeader *hdr;


   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_BBAND_SENDCMPLTIE_ID;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],1);    /* ONLY one byte of information!! */

   frameptr += ATMSIG_IEHDRSIZE;

   /* IE contents */
   *frameptr = ATM_BBAND_SENDCMPLT_IND;

}



/*
 * a t m S i g _ b u i l d C a l l S t a t e ( )
 *
 * Build Call State Information Element.
 * The purpose of the Call State IE is to describe the current status of a
 * call or a global interface state.
 */
void atmSig_buildCallState (atmSvcType *svc, uchar *frameptr)
{

   atmSig_ieHeader *hdr;


   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_CALLSTATE_IE;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],ATMSIG_CALLSTATE_IESIZE);  

   frameptr += ATMSIG_IEHDRSIZE;

   /* IE content */
   switch (svc->state) {

      case ATMSIG_ST_NULL:
         *frameptr = ATM_CALLSTATE_NULL;
         break;

      case ATMSIG_ST_CALL_INITIATED:
         *frameptr = ATM_CALLSTATE_CALLINIT;
         break;

      case ATMSIG_ST_OUTCALL_PROCEEDING:
         *frameptr = ATM_CALLSTATE_OUTCALLPROC;
         break;

      case ATMSIG_ST_CALL_PRESENT:
	 *frameptr = ATM_CALLSTATE_CALLPRESENT;
	 break;

      case ATMSIG_ST_CONNECT_REQ:
         *frameptr = ATM_CALLSTATE_CONNECTREQ;
         break;

      case ATMSIG_ST_INCALL_PROCEEDING:
	 *frameptr = ATM_CALLSTATE_INCALLPROC;
	 break;

      case ATMSIG_ST_ACTIVE:
         *frameptr = ATM_CALLSTATE_ACTIVE;
         break;

      case ATMSIG_ST_RELEASE_REQ:
         *frameptr = ATM_CALLSTATE_RELREQ;
         break;

      case ATMSIG_ST_RELEASE_IND:
	 *frameptr = ATM_CALLSTATE_RELIND;
	 break;

      case ATMSIG_ST_RESTART_REQ:
	 *frameptr = ATM_CALLSTATE_REST1;
	 break;

	 
      default:
	 if (atmSig_debugEvent || atmSig_debugError)
	   buginf("\nSIGEVNT: o build call state IE in an undefined state %s",
		  atmSig_printState(svc->state));
         break;

   }

}


/*
 * a t m S i g _ b u i l d R e s t a r t I n d i c a t o r ( )
 *
 * Build Restart Indicator Information Element.
 */
void atmSig_buildRestartIndicator (atmSvcType *svc, uchar *frameptr)
{

   atmSig_ieHeader *hdr;
  

   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_RESTARTINDICATOR_IE;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],ATMSIG_RESTARTINDICATOR_IESIZE);

   frameptr += ATMSIG_IEHDRSIZE;
   *frameptr = svc->restartClass | ATMSIG_EXTENSION_BIT; 

}


/*
 * a t m S i g _ b u i l d C a u s e ( )
 *
 * Build Cause Information Element.
 * The Cause IE describes the reason for generating certain messages,
 * providing diagnostic information in the event of procedural errors.
 */
void atmSig_buildCause (atmSvcType *svc, uchar *frameptr, uchar cause)
{

   atmSig_ieHeader *hdr;
   atmSig_causeIe *causeIe;


   /* If no cause at this point, use ATM_CAUSE_NORMAL_UNSPECIFIED */
   if (!cause)
     cause = ATM_CAUSE_NORMAL_UNSPECIFIED;

   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_CAUSE_IE;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],ATMSIG_CAUSE_IESIZE);

   frameptr += ATMSIG_IEHDRSIZE;
   causeIe = (atmSig_causeIe *)frameptr;

   causeIe->location = ATM_CAUSELOC_USER | ATMSIG_EXTENSION_BIT;
   causeIe->causeValue = cause | ATMSIG_EXTENSION_BIT; 

}


/*
 * a t m S i g _ b u i l d C o n n e c t i o n I d ( )
 *
 * Build Connection Identifier Information Element.
 * The Connection Identifier IE identifies the local ATM connection resources
 * on the interface.
 */
void atmSig_buildConnectionId (atmSvcType *svc, uchar *frameptr)
{

   atmSig_ieHeader *hdr;
   atmSig_connectionIdIe *connid;


   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_CONNECTIONIDENTIFIER_IE;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],ATMSIG_CONNECTIONID_IESIZE);

   frameptr += ATMSIG_IEHDRSIZE;
   connid = (atmSig_connectionIdIe *)frameptr;

   connid->assoc_sig = ATM_VPASSOC_SIG | ATMSIG_EXTENSION_BIT; 
   PUTSHORT(&connid->vpci[0], svc->vpci);
   PUTSHORT(&connid->vci[0], svc->vci);

}



/*
 * a t m S i g _ b u i l d B B a n d L o w L a y e r I n f o ( )
 *
 * Build Broadband Low Layer Information Element.
 * The purpose of the Broadband low layer information element is to provide
 * a means which should be used for compatibility checking by an addressed
 * entity. 
 */
void atmSig_buildBBandLowLayerInfo (atmSvcType *svc, uchar *frameptr)
{

   atmSig_ieHeader *hdr;
   int i;


   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_BROADBAND_LOWLAYER_IE;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],svc->blli[0].length);

   frameptr += ATMSIG_IEHDRSIZE;

   /* empty contents from svc's blli field into IE */
   for (i = 0; i < svc->blli[0].length; ++i, ++frameptr)
      *frameptr = svc->blli[0].string[i];

}



/*
 * a t m S i g _ b u i l d E n d P o i n t R e f e r e n c e ( )
 *
 * Build End Point Reference Information Element. 
 * Mandatory in Multipoint Messages and SETUP Messages for Multipoint calls.
 */
void atmSig_buildEndpointReference (atmSvcType *svc, uchar *frameptr, 
				    atmSvcPartyType *party)
{

   atmSig_ieHeader *hdr;
   atmSig_endpointRefIe *endpointRef;
   short reference;

   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_ENDPOINT_REFERENCE_IE;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],ATMSIG_ENDPOINT_REF_IESIZE);

   frameptr += ATMSIG_IEHDRSIZE;
   endpointRef = (atmSig_endpointRefIe *)frameptr;

   *frameptr = ATMSIG_ENDPOINT_REF_TYPE;
   
   if ( atmSig_isV31(svc) ) {
     if (svc->locallyInitiatedCall)
       /* set flag bit to 0 because we are the originate side */
       reference = party->endpointRef & ATMSIG_ENDPOINT_REF_MASK;
     else   /* set the flag bit to 1 */
       reference = party->endpointRef | ~ATMSIG_ENDPOINT_REF_MASK;  
   }
   else
       reference = party->endpointRef;

   PUTSHORT(&endpointRef->endpointRefId[0], reference);


}


/*
 * a t m S i g _ b u i l d E n d P o i n t S t a t e ( )
 *
 * Build End Point State Information Element. 
 * Mandatory in Multipoint Status Messages.
 */
void atmSig_buildEndpointState (atmSvcType *svc, uchar *frameptr, atmSvcPartyType *party)
{

   atmSig_ieHeader *hdr;
   uchar endpointState = 0;


   /* IE header */
   hdr = (atmSig_ieHeader *)frameptr;
   hdr->identifier = ATM_ENDPOINT_STATE_IE;
   hdr->cii = ATMSIG_CII;
   PUTSHORT(&hdr->length[0],ATMSIG_ENDPOINT_STATE_IESIZE);

   frameptr += ATMSIG_IEHDRSIZE;

   switch(party->partyState) {
   case ATMSIG_PST_NULL:
	endpointState = ATM_ENDPOINTSTATE_NULL;
	break;

   case ATMSIG_PST_ADD_PARTY_INITIATED:
	endpointState = ATM_ENDPOINTSTATE_ADDPRTYINIT;
	break;

   case ATMSIG_PST_ADD_PARTY_RECEIVED:
	endpointState = ATM_ENDPOINTSTATE_ADDPRTYRCVD;
	break;

   case ATMSIG_PST_DROP_PARTY_INITIATED:
	endpointState = ATM_ENDPOINTSTATE_DRPPRTYINIT;
	break;

   case ATMSIG_PST_DROP_PARTY_RECEIVED:
	endpointState = ATM_ENDPOINTSTATE_DRPPRTYRCVD;
	break;

   case ATMSIG_PST_ACTIVE:
	endpointState = ATM_ENDPOINTSTATE_ACTIVE;
	break;

   default:
	break;
   }

   /* Only 1 byte in endpointState */
   *frameptr = endpointState & ATMSIG_ENDPOINT_STATE_IE_MASK;

}



/*
 * this function checks whether or not the Qos class in the IE
 * matches its coding standard. It only applies to UNI 3.1
 */
int atmSig_checkQosClass (char codingStandard, unsigned char Qos)
{

  switch (Qos) {
  case ATM_QOSIE_UNSPECD:
    if (codingStandard == ATMSIG_IECODING_NET) {
      if (atmSig_debugError)
	buginf("\nATMSIG:Qos class 0 in standard defined for the network");
      
      return (ATMSIG_IE_FATAL_ERROR);
    }

    break;

  case ATM_QOSIE_CLASS1:
  case ATM_QOSIE_CLASS2:
  case ATM_QOSIE_CLASS3:
  case ATM_QOSIE_CLASS4:
    if (codingStandard == ATMSIG_IECODING_ITU) {
      if (atmSig_debugError)
	buginf("\nATMSIG:Qos class %d in coding standard ITU-T",
	       Qos);
      
      return (ATMSIG_IE_FATAL_ERROR);
    }
    
    break;

  case ATM_QOSIE_RES:
    /* there is no individual qos parameter defined in UNI 3.1. They are only
       defined in UNI 4.0. */
    return (ATMSIG_IE_FATAL_ERROR);

  default:
    return (ATMSIG_IE_FATAL_ERROR);    
  }

  return (ATMSIG_IE_OK); 

}

