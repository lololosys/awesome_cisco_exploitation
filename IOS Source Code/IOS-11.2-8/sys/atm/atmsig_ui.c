/* $Id: atmsig_ui.c,v 3.8.18.5 1996/07/10 22:20:36 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_ui.c,v $
 *--------------------------------------------------------------------
 *
 * A T M S I G _ U I . C
 *
 * ATM Signalling (Q.93B) User Interface 
 *
 * November 1993,    Percy P. Khabardar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *--------------------------------------------------------------------
 * $Log: atmsig_ui.c,v $
 * Revision 3.8.18.5  1996/07/10  22:20:36  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.8.18.4  1996/05/22  19:08:33  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * cannotbe -> cannot be
 *
 * Revision 3.8.18.3  1996/05/09  14:11:22  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.18.1.18.1  1996/04/27  06:33:45  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.18.2  1996/05/06  16:43:49  dino
 * CSCdi56689:  When a multipoint-VC goes away, the IP multicast cache is
 *              not fixed
 * Branch: California_branch
 *
 * Revision 3.8.18.1  1996/03/18  18:59:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.22.1  1996/03/22  09:35:05  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.8.8.1  1996/03/05  05:48:56  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.8.2.1  1996/02/27  20:40:35  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.8  1996/02/07  16:10:09  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/01  05:59:53  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/31  00:29:42  snyder
 * CSCdi47856:  Grammar are bad in output
 *              does not exists -> does not exist
 *
 * Revision 3.5  1996/01/29  07:26:25  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4.12.1  1996/02/24  01:39:23  cakyol
 * added the "atmSig_printOpcode" user api function for
 * user debugging support.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4  1995/11/21  13:34:47  achopra
 * CSCdi39637:  Show atm vc <xxx> output should show remote not
 * destination NSAP
 *
 * Revision 3.3  1995/11/17  08:46:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:07:04  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:00:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/18  14:23:18  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.4  1995/08/01  18:41:36  percyk
 * CSCdi38028:  display cause byte as string.
 *
 * Revision 2.3  1995/07/19  21:39:38  schrupp
 * CSCdi37350:  Display protocol for ATM MUX call in show atm vc X
 *
 * Revision 2.2  1995/06/19  20:52:18  cakyol
 * CSCdi35893:  All LANE VCs should show aal5lane, not all5mux
 * - fixed.  Check the circuit for LANE first now, before
 *   proceeding to check if it is aal5mux or not.
 *
 * Revision 2.1  1995/06/07  20:12:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *--------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_atmsig.h"
#include "../if/atm.h"
#include "../if/static_map.h"
#include "../ip/ip_registry.h"
#include "atmsig_api.h"
#include "atmsig_public.h"
#include "atmsig_private.h"
#include "atmsig_smap.h"
#include "mgd_timers.h"


/*
 * a t m S i g _ i n t e r f a c e _ c o m m a n d s ( )
 *
 * Configuration for ATM Signalling.  Note all config is interface specific. 
 */
void atmSig_interface_commands (parseinfo *csb)
{

   idbtype *idb;
   hwidbtype *hwidb;
   atm_db_t *aip;


   idb = csb->interface;
   hwidb = idb->hwptr;
   aip = hwidb->atm_db;

   /* Atm Signalling configuration only allowed on ATM interfaces */
   if ( !(hwidb->status & IDB_ATM) ) {
      if (!csb->nvgen)
         printf("\n%%ATM not supported on this interface");
      return;
   }
 
   /*
    * csb->nvgen == TRUE whenever 'write [term || mem || net]' is entered.
    * Otherwise, it's false to indicate that a command is being specified
    * by the user.
    */ 
   if (csb->nvgen) {
      switch (csb->which) {

         case ATMSIG_MAX_CONN_PENDING:
            nv_write(aip->max_conn_pending != ATMSIG_MAX_CONN_PENDING_DEFAULT,
                        "%s %d", csb->nv_command, aip->max_conn_pending);
            break;

         case ATMSIG_TIMER316:
            nv_write(aip->timer_t316 != ATMSIG_T316_DEFAULT, "%s %d",
                        csb->nv_command, aip->timer_t316);
            break;

         case ATMSIG_TIMER308:
            nv_write(aip->timer_t308 != ATMSIG_T308_DEFAULT, "%s %d",
                        csb->nv_command, aip->timer_t308);
            break;

         case ATMSIG_TIMER309:
            nv_write(aip->timer_t309 != ATMSIG_T309_DEFAULT, "%s %d",
                        csb->nv_command, aip->timer_t309);
            break;

         case ATMSIG_TIMER322:
            nv_write(aip->timer_t322 != ATMSIG_T322_DEFAULT, "%s %d",
                        csb->nv_command, aip->timer_t322);
            break;

         case ATMSIG_TIMER313:
            nv_write(aip->timer_t313 != ATMSIG_T313_DEFAULT, "%s %d",
                        csb->nv_command, aip->timer_t313);
            break;

         case ATMSIG_TIMER310:
            nv_write(aip->timer_t310 != ATMSIG_T310_DEFAULT, "%s %d",
                        csb->nv_command, aip->timer_t310);
            break;

         case ATMSIG_TIMER303:
            nv_write(aip->timer_t303 != ATMSIG_T303_DEFAULT, "%s %d",
                        csb->nv_command, aip->timer_t303);
            break; 

         case ATMSIG_TIMER398:
            nv_write(aip->timer_t398 != ATMSIG_T398_DEFAULT, "%s %d",
                        csb->nv_command, aip->timer_t398);
            break; 

         case ATMSIG_TIMER399:
            nv_write(aip->timer_t399 != ATMSIG_T399_DEFAULT, "%s %d",
                        csb->nv_command, aip->timer_t399);
            break; 

         case ATMSIG_MAX_RETRY:
            nv_write(aip->atmsig_retry_cnt != ATMSIG_RETRY_DEFAULT, "%s %d",
                        csb->nv_command, aip->atmsig_retry_cnt);
            break;

         case ATMSIG_UNI_SIDE:
            nv_write(aip->uni_side != ATMSIG_SIDE_USER, "%s",
                                            "asig side network");
	    break;

         default:
            bad_parser_subcommand(csb, csb->which);
            break;

      }
      return;
   } 

   switch (csb->which) {

      case ATMSIG_MAX_CONN_PENDING:
         aip->max_conn_pending = (csb->sense ? GETOBJ(int,1) :
                                              ATMSIG_MAX_CONN_PENDING_DEFAULT);
         break;

      case ATMSIG_TIMER316:
         aip->timer_t316 = (csb->sense ? GETOBJ(int,1) : ATMSIG_T316_DEFAULT);
         break;

      case ATMSIG_TIMER308:
         aip->timer_t308 = (csb->sense ? GETOBJ(int,1) : ATMSIG_T308_DEFAULT);
         break;

      case ATMSIG_TIMER322:
         aip->timer_t322 = (csb->sense ? GETOBJ(int,1) : ATMSIG_T322_DEFAULT);
         break;

      case ATMSIG_TIMER313:
         aip->timer_t313 = (csb->sense ? GETOBJ(int,1) : ATMSIG_T313_DEFAULT);
         break;

      case ATMSIG_TIMER310:
         aip->timer_t310 = (csb->sense ? GETOBJ(int,1) : ATMSIG_T310_DEFAULT);
         break;

      case ATMSIG_TIMER303:
         aip->timer_t303 = (csb->sense ? GETOBJ(int,1) : ATMSIG_T303_DEFAULT);
         break;

      case ATMSIG_TIMER398:
         aip->timer_t398 = (csb->sense ? GETOBJ(int,1) : ATMSIG_T398_DEFAULT);
         break;

      case ATMSIG_TIMER399:
         aip->timer_t399 = (csb->sense ? GETOBJ(int,1) : ATMSIG_T399_DEFAULT);
         break;

      case ATMSIG_MAX_RETRY:
         aip->atmsig_retry_cnt = (csb->sense ? GETOBJ(int,1) : ATMSIG_RETRY_DEFAULT);
         break;

      case ATMSIG_UNI_SIDE:
         aip->uni_side = (csb->sense ? GETOBJ(int,1) : ATMSIG_SIDE_USER);
         break;

      case ATMSIG_TIMER309:
	 /* only do this if in uni 3.1 */
	 if (aip->uni_version == ATMUNI_VERSION_31) {
	   aip->timer_t309 = (csb->sense ? GETOBJ(int,1) : ATMSIG_T309_DEFAULT);
	   break;
	 }
	 /* else let it fall thru */

      default:
         bad_parser_subcommand(csb, csb->which);
         break;

   }

}


/*
 * a t m S i g _ p r i n t O p c o d e
 *
 * Text print of ATM Signalling user opcodes.
 * The api users will use this for debugging.
 * It is exposed in file "atmsig_api.h".
 */
char *atmSig_printOpcode (sig_api_opcode opcode)
{
    switch (opcode)
    {
	case ATM_LISTEN_NSAP: 
	    return "ATM_LISTEN_NSAP";

	case ATM_CANCEL_NSAP: 
	    return "ATM_CANCEL_NSAP";

	case ATM_SETUP:
	    return "ATM_SETUP";

	case ATM_CONNECT:
	    return "ATM_CONNECT";

	case ATM_CONNECT_ACK:
	    return "ATM_CONNECT_ACK";

	case ATM_RELEASE:
	    return "ATM_RELEASE";

	case ATM_RELEASE_COMPLETE:
	    return "ATM_RELEASE_COMPLETE";

	case ATM_STATUS_ENQUIRY:
	    return "ATM_STATUS_ENQUIRY";

	case ATM_STATUS:
	    return "ATM_STATUS";

	case ATM_ADD_PARTY:
	    return "ATM_ADD_PARTY";

	case ATM_ADD_PARTY_ACK:
	    return "ATM_ADD_PARTY_ACK";

	case ATM_ADD_PARTY_REJ:
	    return "ATM_ADD_PARTY_REJ";

	case ATM_DROP_PARTY:
	    return "ATM_DROP_PARTY";

	case ATM_DROP_PARTY_ACK:
	    return "ATM_DROP_PARTY_ACK";

	default:
	    return "INVALID";
    }
}


/*
 * a t m S i g _ p r i n t S t a t e
 *
 * Text print of ATM Signaling State.
 */
char *atmSig_printState (atmSig_statetype state)
{

   switch (state) {

      case ATMSIG_ST_NULL:
         return ("Null");
 
      case ATMSIG_ST_CALL_INITIATED:
         return ("Call Initiated");
 
      case ATMSIG_ST_OUTCALL_PROCEEDING:
         return ("Outgoing Call Proceeding");

      case ATMSIG_ST_CALL_PRESENT:
         return ("Call Present");

      case ATMSIG_ST_CONNECT_REQ:
         return ("Connect Request");

      case ATMSIG_ST_INCALL_PROCEEDING:
         return ("Incoming Call Proceeding");
 
      case ATMSIG_ST_ACTIVE:
         return ("Active");
 
      case ATMSIG_ST_RELEASE_REQ:
         return ("Release Request");

      case ATMSIG_ST_RELEASE_IND:
         return ("Release Indication");
 
      case ATMSIG_ST_RESTART_REQ:
         return ("Restart Request");

      case ATMSIG_PST_NULL:
         return ("Multipoint Null");
 
      case ATMSIG_PST_ACTIVE:
         return ("Multipoint Active");

       case ATMSIG_PST_ADD_PARTY_INITIATED:
         return ("Add Party Initiated");
 
       case ATMSIG_PST_ADD_PARTY_RECEIVED:
         return ("Add Party Received");
 
      case ATMSIG_PST_DROP_PARTY_INITIATED:
         return ("Drop Party Initiated");
 
      case ATMSIG_PST_DROP_PARTY_RECEIVED:
         return ("Drop Party Received");
 
      default:
         return ("Invalid");

   }

}
 


/*
 * a t m S i g _ p r i n t M s g T y p e ( )
 *
 * Text Print of Signalling Message Type.
 * Currently 15 Signalling Messages defined by ATM Forum.
 */
char *atmSig_printMsgType (int opcode)
{


   switch ( (uint)opcode ) {

      case ATMSIG_MT_CALL_PROCEEDING:
         return ("Call Proceeding");

      case ATMSIG_MT_CONNECT:
         return ("Connect");

      case ATMSIG_MT_CONNECT_ACK:
         return ("Connect Ack");

      case ATMSIG_MT_SETUP:
         return ("Setup");

      case ATMSIG_MT_RELEASE:
         return ("Release");

      case ATMSIG_MT_RELEASE_COMPLETE:
         return ("Release Complete");

      case ATMSIG_MT_RESTART:
         return ("Restart");

      case ATMSIG_MT_RESTART_ACK:
         return ("Restart Ack");

      case ATMSIG_MT_STATUS:
         return ("Status");

      case ATMSIG_MT_STATUS_ENQUIRY:
         return ("Status Enquiry");

      case ATMSIG_MT_ADD_PARTY:
         return ("Add Party");

      case ATMSIG_MT_ADD_PARTY_ACK:
         return ("Add Party Ack");

      case ATMSIG_MT_ADD_PARTY_REJ:
         return ("Add Party Rej");

      case ATMSIG_MT_DROP_PARTY:
         return ("Drop Party");

      case ATMSIG_MT_DROP_PARTY_ACK:
         return ("Drop Party Ack");

      default:
         return ("Invalid Msg");
   }

}



/*
 * a t m S i g _ p r i n t I e T y p e ( )
 *
 * Text Print of Information Element Type.
 * Currently 18 Information Elements defined by ATM Forum.
 */
char *atmSig_printIeType (uint OpCode)
{

   switch (OpCode) {

      case ATM_AAL_IE:
         return ("Aal");

      case ATM_USERCELLRATE_IE:
         return ("User Cell Rate");

      case ATM_BROADBAND_BEARERCAP_IE:
         return ("BroadBand Bearer Capability");

      case ATM_BROADBAND_HIGHLAYER_IE:
         return ("BroadBand Higher Layer");

      case ATM_BROADBAND_LOWLAYER_IE:
         return ("BroadBand Lower Layer");

      case ATM_CALLSTATE_IE:
         return ("Call State");

      case ATM_CALLEDPARTY_NUMBER_IE:
         return ("Called Party Number");

      case ATM_CALLEDPARTY_SUBADDR_IE:
         return ("Called Party SubAddress");

      case ATM_CALLINGPARTY_NUMBER_IE:
         return ("Calling Party Number");

      case ATM_CALLINGPARTY_SUBADDR_IE:
         return ("Calling Party SubAddress");

      case ATM_CAUSE_IE:
         return ("Cause");

      case ATM_CONNECTIONIDENTIFIER_IE:
         return ("Connection Identifier");

      case ATM_QOS_IE:
         return ("Qos Parameter");

      case ATM_RESTARTINDICATOR_IE:
         return ("Restart Indicator");

      case ATM_BROADBAND_SENDINGCMPLT_IE:
         return ("BroadBand Sending Complete");

      case ATM_TRANSIT_NTWRKSEL_IE:
         return ("Transit Network Selection");

      case ATM_ENDPOINT_REFERENCE_IE:
         return ("End Point Reference");

      case ATM_ENDPOINT_STATE_IE:
         return ("End Point State");

      default:
         return ("Invalid IE");

   }

}



/*
 * a t m s i g _ p r i n t A a l T y p e ( )
 *
 * Text print of AAL Type.
 */
char *atmsig_printAalType (int type)
{

   switch (type) {

      case ATM_AALIE_TYPE1:
         return ("AAL Type 1");

      case ATM_AALIE_TYPE34:
         return ("AAL Type 3/4");

      case ATM_AALIE_TYPE5:
         return ("AAL Type 5");

      case ATM_AALIE_TYPEX:
         return ("AAL Type X");

      default:
         return ("Invalid AAL Type");

   }

}



/*
 * a t m s i g _ p r i n t A a l S u b t y p e ( )
 *
 * Text print of AAL Subtype.
 */
char *atmsig_printAalSubtype (int subtype)
{

   switch (subtype) {

      case ATM_AALIE_SUBTYPE_NULL:
         return ("Null");

      case ATM_AALIE_SUBTYPE_VOICE:
         return ("Voice based");

      case ATM_AALIE_SUBTYPE_SYNC:
         return ("Sync Circuit Emulation");

      case ATM_AALIE_SUBTYPE_ASYNC:
         return ("Async Circuit Emulation");

      case ATM_AALIE_SUBTYPE_AUDIO:
         return ("High quality audio");

      case ATM_AALIE_SUBTYPE_VIDEO:
         return ("Video");

      default:
         return ("Invalid");
 
   }

}



/*
 * a t m s i g _ p r i n t A a l C o n s t B i t R a t e
 *
 * Text print of AAL Constant Bit Rate.
 */
char *atmSig_printAalConstBitRate (int rate)
{

   switch (rate) {

      case ATM_AALIE_CBR_64:
         return ("64 kbit/sec");

      case ATM_AALIE_CBR_1544:
         return ("1544 kbit/sec");

      case ATM_AALIE_CBR_6312:
         return ("6312 kbit/sec");

      case ATM_AALIE_CBR_32064:
         return ("32064 kbit/sec");

      case ATM_AALIE_CBR_44736:
         return ("44736 kbit/sec");

      case ATM_AALIE_CBR_97728:
         return ("97728 kbit/sec");

      case ATM_AALIE_CBR_2048:
         return ("2048 kbit/sec");

      case ATM_AALIE_CBR_8448:
         return ("8448 kbit/sec");

      case ATM_AALIE_CBR_34368:
         return ("34368 kbit/sec");

      case ATM_AALIE_CBR_139264:
         return ("139264 kbit/sec");

      case ATM_AALIE_CBR_N64:
         return ("n * 64 kbit/sec");

      default:
         return ("Invalid");

   }

}



/*
 * a t m S i g _ p r i n t C l k R e c o v e r y
 *
 * Text print of Clock Recovery Type
 */
char *atmSig_printClkRecovery (int type)
{

   switch (type) {

      case ATM_AALIE_CRT_NULL:
         return ("Null");

      case ATM_AALIE_CRT_SRTS:
         return ("Synchronous Residual Time Stamp");

      case ATM_AALIE_CRT_ACR:
         return ("Adaptive Clock Recovery");

      default:
         return ("Invalid");

   }

}



/*
 * a t m S i g _ p r i n t E r r o r C o r r e c t i o n ( ) 
 *
 * Text print of Error Correction. 
 */
char *atmSig_printErrorCorrection (int type)
{

   switch (type) {
  
      case ATM_AALIE_ECT_NULL:
         return ("Null");

      case ATM_AALIE_ECT_FECI:
         return ("Interleaving FEC");

      default:
         return ("Invalid");

   }

}



/*
 * a t m S i g _ p r i n t S t r u c t D a t a X f er ( )
 *
 * Text print of Error Correction.
 */
char *atmSig_printStructDataXfer (int type)
{

   switch (type) {

      case ATM_AALIE_SDT_NULL:
         return ("Null");

      case ATM_AALIE_SDT:
         return ("Structured Data Transfer");

      default:
         return ("Invalid");

   }

}



/*
 * a t m S i g _ p r i n t S s c s T y p e ( )
 *
 * Text print of SSCS type.
 */
char *atmSig_printSscsType (int type)
{

   switch (type) {

      case ATM_AALIE_SSCS_NULL:
         return ("Null");

      case ATM_AALIE_SSCS_ADATA:
         return ("Data SSCS based on SSCOP (assured operation)");

      case ATM_AALIE_SSCS_UDATA:
         return ("Data SSCS based on SSCOP (non-assured operation)");

      case ATM_AALIE_SSCS_FRAMERELAY:
         return ("Frame Relay SSCS");

      default:
         return ("Invalid");

   }

}



/*
 * a t m S i g _ p r i n t C a u s e V a l u e ( )
 *
 * Text print of Cause Value.
 */
char *atmSig_printCauseValue (int cause)
{

   switch ( (uint)cause ) {

      case ATM_CAUSE_UNALLOC:
         return ("unallocated (unassigned) number");

      case ATM_CAUSE_NOROUTE_TO_NTWRK:
         return ("no route to specified transit network");

      case ATM_CAUSE_NOROUTE_TO_DEST:
         return ("no route to destination");

      case ATM_CAUSE_VPCI_UNACCEPT:
         return ("VPCI/VCI unacceptable");

      case ATM_CAUSE_CALL_CLEARING:
	 return ("normal call clearing");

      case ATM_CAUSE_USER_BUSY:
         return ("User busy");

      case ATM_CAUSE_NOUSER_RSP:
         return ("No user responding");

      case ATM_CAUSE_CALL_REJ:
         return ("Call rejected");

      case ATM_CAUSE_NUM_CHANGED:
         return ("Number changed");

      case ATM_CAUSE_USER_REJ:
         return ("User rejects all screened numbers");

      case ATM_CAUSE_DEST_INACTIVE:
         return ("destination out of order");

      case ATM_CAUSE_INVALID_NUMBER_FMT:
         return ("invalid number format (address incomplete)");

      case ATM_CAUSE_STATUSENQ_RSP:
         return ("response to Status Enquiry Message");

      case ATM_CAUSE_NORMAL_UNSPECIFIED:
         return ("normal, unspecified");

      case ATM_CAUSE_VPCI_UNAVAIL:
         return ("requested VPCI/VCI not available");

       case ATM_CAUSE_VPCI_FAIL:
	 return ("vpci/vci assignment failure");

       case ATM_CAUSE_CELLRATE_UNAVAIL:
	 return ("user cell rate not available");

      case ATM_CAUSE_NTWRK_OUT_OF_ORDER:
         return ("network out of order");

      case ATM_CAUSE_TMP_FAILURE:
         return ("temporary failure");

      case ATM_CAUSE_ACCESS_INFO_DISCRD:
         return ("access information discarded");

      case ATM_CAUSE_NO_VPCI:
         return ("no VPCI/VCI available");

      case ATM_CAUSE_RSRC_UNAVAIL:
         return ("resource unavailable, unspecified");

      case ATM_CAUSE_QOS_UAVAIL:
         return ("Quality of Service unavailable");

      case ATM_CAUSE_USR_CELLRATE_UNAVAIL:
         return ("user cell rate not available");

      case ATM_CAUSE_BEARERCAP_UNAUTH:
         return ("bearer capability not authorized");

      case ATM_CAUSE_BEARERCAP_NOTAVAIL:
         return ("bearer capability not presently available");

      case ATM_CAUSE_SERVICE_UNAVAIL:
         return ("Service or option not available, unspecified");

      case ATM_CAUSE_BEARERCAP_UNIMP:
         return ("bearer capability not implemented");

      case ATM_CAUSE_UNSUPPORTED:
         return ("Unsupported combination of traffic parameters");

       case ATM_CAUSE_AALPARAM_NOT_SUPPORT:
	 return ("aal parameters cannot be supported");

      case ATM_CAUSE_INVALID_CALLREF:
         return ("invalid call reference value");

      case ATM_CAUSE_CHANNEL_NOTPRESENT:
         return ("identified channel does not exist");

      case ATM_CAUSE_INCOMPAT_DEST:
         return ("incompatible destination");

      case ATM_CAUSE_INVALID_ENDPOINT_REF:
         return ("invalid end point reference");

      case ATM_CAUSE_INVALID_TRNTWRK_SEL:
         return ("invalid transit network selection");

      case ATM_CAUSE_TOOMANY_ADDPARTY_REQ:
         return ("too many pending add party requests");

      case ATM_CAUSE_INVALID_AAL_PARAM:
         return ("AAL parameters cannot be supported");

      case ATM_CAUSE_MANDATORY_IE_MISSING:
         return ("mandatory information element is missing");

      case ATM_CAUSE_MSG_NOT_EXISTING:
         return ("message type non-existent or not implemented");

      case ATM_CAUSE_IE_NOT_EXISTING:
         return ("information element non-existent or not implemented");

      case ATM_CAUSE_INVALID_IE_CONTENTS:
         return ("invalid information element contents");

      case ATM_CAUSE_INCOMPAT_MSG:
         return ("message not compatible with call state");

      case ATM_CAUSE_RCVRY_ON_TIMEREXPIRY:
         return ("recovery on timer expiry");

      case ATM_CAUSE_PROTOCOL_ERROR:
         return ("protocol error, unspecified");

      default:
         return ("Invalid Cause Value");

   }

}



/*
 * a t m S i g _ p r i n t E v e n t T y p e ( )
 *
 */
char *atmSig_printEventType (atmSig_eventtype event)
{

   
   switch (event) {

      case ATMSIG_RCV_CALL_PROCEEDING:
         return ("Rcvd Call Proceeding");

      case ATMSIG_RCV_CONNECT:
         return ("Rcvd Connect");
 
      case ATMSIG_RCV_CONNECT_ACK:
         return ("Rcvd Connect Ack");

      case ATMSIG_RCV_SETUP:
         return ("Rcvd Setup");

      case ATMSIG_RCV_RELEASE:
         return ("Rcvd Release");

      case ATMSIG_RCV_RELEASE_COMPLETE:
         return ("Rcvd Release Complete");

      case ATMSIG_RCV_RESTART:
         return ("Rcvd Restart");

      case ATMSIG_RCV_RESTART_ACK:
         return ("Rcvd Restart Ack");

      case ATMSIG_RCV_STATUS:
         return ("Rcvd Status");

      case ATMSIG_RCV_STATUS_ENQUIRY:
         return ("Rcvd Status Enquiry");

      case ATMSIG_RCV_ADD_PARTY:
         return ("Rcvd Add Party");
 
      case ATMSIG_RCV_ADD_PARTY_ACK:
         return ("Rcvd Add Party Ack");
 
      case ATMSIG_RCV_ADD_PARTY_REJ:
         return ("Rcvd Add Party Rej");
 
      case ATMSIG_RCV_DROP_PARTY:
         return ("Rcvd Drop Party");
 
      case ATMSIG_RCV_DROP_PARTY_ACK:
         return ("Rcvd Drop Party Ack");

      case ATMSIG_REQ_CONNECT:
	 return ("Request Connect");

      case ATMSIG_REQ_SETUP:
	 return ("Request Setup");

      case ATMSIG_REQ_RELEASE:
	 return ("Request Release");

      case ATMSIG_REQ_RELEASE_COMPLETE:
	 return ("Request Release Complete");

      case ATMSIG_REQ_ADD_PARTY:
	 return ("Request Add Party");

      case ATMSIG_REQ_DROP_PARTY:
	 return ("Request Drop Party");

      case ATMSIG_REQ_ADD_PARTY_ACK:
	 return ("Request Add Party Ack");

      case ATMSIG_REQ_ADD_PARTY_REJ:
	 return ("Request Add Party Rej");

      case ATMSIG_REQ_DROP_PARTY_ACK:
	 return ("Request Drop Party Ack");
 
      default:
         return ("Rcvd Invalid");

   }

}



/*
 * a t m S i g _ p r i n t P r o t o c o l
 *
 * Text print of protocol of ATM Mux call
 */
char *atmSig_printProtocol (int linktype)
{

  switch (linktype) {

  case LINK_IP:
    return ("IP");

  case LINK_XNS:
    return ("XNS");

  case LINK_APPLETALK:
    return ("ATALK");

  case LINK_CLNS:
  case LINK_CLNS_ALL_ES:
  case LINK_CLNS_ALL_IS:
  case LINK_CLNS_BCAST:
  case LINK_ISIS_ALL_L1_IS:
  case LINK_ISIS_ALL_L2_IS:
    return ("CLNS");

  case LINK_DECNET:
  case LINK_DECNET_NODE:
  case LINK_DECNET_ROUTER_L1:
  case LINK_DECNET_ROUTER_L2:
  case LINK_DECNET_PRIME_ROUTER:
    return ("DECNET");

  case LINK_NOVELL:
    return ("IPX");

  case LINK_APOLLO:
    return ("APOLLO");

  case LINK_VINES:
    return ("VINES");

  case LINK_ILLEGAL:
  case LINK_PAD:
  default:
    return ("Unknown");

  }
}


/*
 * a t m S i g _ e x e c _ c o m m a n d s ( )
 *
 */
void atmSig_exec_commands (parseinfo *csb)
{

   switch (csb->which) {

      case PARSER_ATMSIG_CLOSE:
         atmSig_closeCommand( GETOBJ(idb, 1), GETOBJ(int, 2) );
         break;

      case PARSER_ATMSIG_STATUS:
         atmSig_statusCommand( GETOBJ(idb, 1), GETOBJ(int, 2) );
         break;

      default:
         bad_parser_subcommand(csb, csb->which);
         break;
   }

}



/*
 *
 * a t m S i g _ c l o s e C o m m a n d
 *
 * atmsig close <interface> <vcnum>
 * 
 * Close the specified svc on the specified port.
 * If vcnum = -1 reset all svc's on the identified ATM interface.
 * If vcnum = -2 reset all svc's on ALL ATM interfaces.
 */
void atmSig_closeCommand (idbtype *idb, int vcnum)
{

   atmSvcType *svc;


   /* reset svc's on all ATM interfaces? */
   if (vcnum == -2) {
      atmSig_globalReset();
      return;
   }

   /* requested interface ATM? */
   if ( !(idb->hwptr->status & IDB_ATM) ) {
      printf("\nrequested interface is not an ATM interface");
      return;
   }

   /* reset svc's on a particular ATM interface? */
   if (vcnum == -1) {
      atmSig_interfaceReset(idb->hwptr);
      return;
   }

   /* svc exists? */
   svc = atmSig_findSvcBlockByVC(idb, vcnum);
   if (!svc) {
      printf("\nsvc %d does not exist on interface %s\n", vcnum, 
                                                     idb->namestring);
      return;
   } 

   /* pull it down */ 
   (void)atmSmap_closeSvcConnection(svc); 

}



/*
 *
 * a t m S i g _ s t a t u s C o m m a n d
 *
 * atmsig status <interface> <vcnum>
 *
 * Find the status of a particular svc.  A Status Enquiry message is sent 
 * to the switch and the state of the svc (received in the Status message) 
 * is displayed.  The call is non-blocking. 
 */
void atmSig_statusCommand (idbtype *idb, ushort vcnum)
{

   atmSvcType *svc;
   atmSig_statuscodetype rc;


   /* requested interface ATM? */
   if ( !(idb->hwptr->status & IDB_ATM) ) {
      printf("\nrequested interface is not an ATM interface");
      return;
   }

   /* check if svc exists */
   svc = atmSig_findSvcBlockByVC(idb, vcnum);
   if (!svc) {
      printf("\nsvc %d does not exist on interface %s\n", vcnum, idb->namestring);
      return;
   }

   /* send a Status Enquiry message to the switch to find out our status */
   rc = atmSmap_findSvcStatus(svc);

   switch (rc) {
      case ATMSIG_STATUSSVC_INVALIDSVC:
         printf("\nrequested svc not present, status request ignored\n");
         break;

      case ATMSIG_STATUSSVC_STATUSFAILED:
         printf("\nno memory to send Status Enquiry msg, status request failed\n");
         break;

      default:
         break;
   } 

}



/*
 * a t m S i g _ c l e a r _ v c
 *
 * Clear an atm virtual circuit from clear command
 */
void atmSig_clear_vc (parseinfo *csb)
{

    idbtype *idb;
    int vcnum;
    atmSvcType *svc;


    idb = GETOBJ(idb,1);

    vcnum = GETOBJ(int,1);

    if ( !(idb->hwptr->status & IDB_ATM) ) {
        printf("\nInterface is not an ATM interface");
        return;
    }

    /* reset svc's on a particular ATM interface? */
   if (vcnum == -1) {
      atmSig_interfaceReset(idb->hwptr);
      return;
   }

   /* svc exists? */
   svc = atmSig_findSvcBlockByVC(idb, vcnum);
   if (!svc) {
      printf("\nsvc %d does not exist on interface %s\n", 
                             vcnum, idb->namestring);
      return;
   }

   /* pull it down */
   (void)atmSmap_closeSvcConnection(svc);
    
    /*
     * If multipoint VC, call IP multicast so the fastswitching cache can
     * be invalidated.
     */
    if (svc->multiPoint) {
	reg_invoke_ip_invalidate_mcache(idb);
    }
}
 


/*
 * a t m s i g _ s h o w _ c o m m a n d s ( )
 *
 */
#ifdef ATMSIG_NOTREQD
void atmsig_show_commands (parseinfo *csb)
{


   switch (csb->which) {

      case SHOW_ATMSIG_VC:
         atmSig_show_vc(csb);
         break;

      case SHOW_ATMSIG_SUMMARY:
         atmSig_show_summary(csb);
         break;

      default:
         /* invalid option */ 
         break;

   }

}

#endif


/*
 * a t m S i g _ s h o w _ v c ( )
 *
 * Display particulars of a given svc.
 */
void atmSig_show_vc (int vcnum, hwidbtype *hwidb)
{

   atmSvcType *svc;
   int i;
   char nsapstr[MAX_ATM_NSAP_ASCII_LEN], buffer[40];
   atmSvcPartyType *party;
   atmSvcPartyType *tmp_party;


   for (i = 0; i < ATMSVC_HASH_SIZE; ++i) {

      svc = atmSig_svcHashTable[i];

      while (svc) {
         if ( (svc->vcnum == vcnum) && (svc->idb->hwptr == hwidb) ) {
	   mem_lock(svc);		/* lock the svc for the printing */
	   mem_lock(svc->rootParty);    /* lock the root party for printf */
	   party = svc->rootParty;
	   printf("\ninterface =  %s, ", svc->idb->namestring);
	   printf("call %s initiated, ", svc->locallyInitiatedCall ? "locally":
                                                                   "remotely");
	   printf("call reference = %d", svc->callRef);
	   printf("\nvcnum = %d, vpi = %d, vci = %d, state = %s", svc->vcnum,
		  svc->vpci, svc->vci, atmSig_printState(svc->state) );
	   printf("\n ");
	   if (ISVCAAL5LANE(svc))
		printf("aal5lane vc");
	   else
		printf("%s vc", svc->aal5mux ? "aal5mux" : "aal5snap");
	   if (svc->aal5mux)
	        printf(", %s", atmSig_printProtocol(svc->linktype) );
	   printf(", %s call", svc->multiPoint ? 
		                                "multipoint": "point-to-point");
	   printf("\nRetry count: Current = %d, Max = %d",
                                           party->retryCount, party->n2);
	   sprint_ticks_in_dhms(buffer, 
			       mgd_timer_left_sleeping(&party->timeoutValue));
	   printf("\ntimer currently %s, timer value = %s", 
                 mgd_timer_running(&party->timeoutValue) ? "active" : "inactive", buffer);
	   
	   /* point to point call */
	   if (!svc->multiPoint) {
	     atm_printnsap(party->destNsap.addr, nsapstr);
	     printf("\nRemote Atm Nsap address: %s", nsapstr);
	     printf("\n");
	     free(svc->rootParty);      /* unlock root party */
	     free(svc);			/* unlock the svc */
	     return;
	   }

	   /* remotely init multipoint call, remote side is root */
	   if (!svc->locallyInitiatedCall) {
	     atm_printnsap(party->destNsap.addr, nsapstr);
	     printf("\nRoot Atm Nsap address: %s", nsapstr);
	     printf("\n");
	     free(svc->rootParty);      /* unlock root party */
	     free(svc);			/* unlock the svc */
	     return;
	   }

	   /* locally init multipoint call, we are root, others are leaves */

           /* NOTE: svc->rootParty will be locked again and freed after    */
           /* printf in the following loop for MULTIPOINT ROOT CASE.       */

	   party = svc->svcPartyQ;
	   while (party) {
	     mem_lock(party);           /* lock party for printf */
	     tmp_party = party->next_party;
	     atm_printnsap(party->destNsap.addr, nsapstr);
	     printf("\nLeaf Atm Nsap address: %s", nsapstr);
	     free(party);      		/* free party after printf */
	     party = tmp_party;
	   }
	   free(svc->rootParty);      	/* unlock root party */
	   free(svc);			/* unlock the svc */
	   return;
	 }
         else
	   svc = svc->next_hash;
       }

    }

}



/*
 * a t m S i g _ s h o w _s u m m a r y ( )
 *
 */
void atmSig_show_summary (parseinfo *csb)
{



}



/*
 * a t m S i g _ h e x o u t ( )
 *
 */
void atmSig_hexout (uchar *buf, int bc, char *src)
{


   char line[16 * 3 + 2 + 16 + 1];
   int i, j;

   if (bc == 0)          /* if none to dump, exit */
      return;

   for (i = 0; i < sizeof(line); i++) 
       line[i] = ' ';

   line[sizeof(line) - 1] = 0;

   for (i = 0; i < bc; i += 16) {

      for (j = 0; j < 16; j++) {
         line[j * 3] = tohexchar(buf[i + j] >> 4);
         line[j * 3 + 1] = tohexchar(buf[i + j]);
	 /* No ASCII dump at the end of display */
      }

      if ( (bc - i) < 16 ) {              /* if partial line */
         for (j = bc - i; j < 16; j++) {
            line[j * 3]    = ' ';
            line[j *3 + 1] = ' ';
	    /* No ASCII dump at the end of display */
         }
      }

      buginf("\n%s: %s", src, line);

   }

}
