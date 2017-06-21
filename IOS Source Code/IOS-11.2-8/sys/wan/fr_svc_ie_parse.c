/* $Id: fr_svc_ie_parse.c,v 3.1.64.6 1996/08/03 23:41:29 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_svc_ie_parse.c,v $
 *------------------------------------------------------------------
 * fr_svc_ie_parse.c : Parsing of incoming I frames
 *
 * May 1995, Shankar Natarajan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_svc_ie_parse.c,v $
 * Revision 3.1.64.6  1996/08/03  23:41:29  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.1.64.5  1996/06/21  22:10:16  shankar
 * CSCdi60623:  Calling party router hung in FR SVC test with Cascade
 * switch
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/06/12  14:01:26  shankar
 * CSCdi58274:  Incorrect mincir value shown in show fram svc map command
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/06/11  23:38:41  skoh
 * CSCdi58274:  Incorrect mincir value shown in show fram svc map command
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/05/22  19:06:38  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * Connectted -> Connected
 *
 * Revision 3.1.64.1  1996/04/25  23:23:37  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/24  04:11:28  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/11/09  13:49:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/01  19:43:45  shankar
 * Placeholder for arkansas feature
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "passive_timers.h"
#include "globs.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "frame_relay.h"
#include "fr_lmi.h"
#include "parser.h"
#include "fr_debug.h"
#include "../parser/parser_defs_exec.h"
#include "../wan/parser_defs_frame.h"
#include "../h/types.h"
#include "../wan/fr_svc.h"
#include "../wan/fr_svc_registry.h"

/* 
 * l3_ie_parse_process
 * A process that deals with all incoming Information (I)
 * frames. The numbered information frames contain call 
 * control messages. One such process per box.
 */
process l3_ie_parse_process (void)
{
    hwidbtype 		*hwidb;
    paktype 		*pak;
    int			count;

    iframeQ = create_watched_queue("FR SVC I Frames", 0, 0);
    process_watch_queue(iframeQ, ENABLE, RECURRING);
    count = 0;
    while (TRUE) {
	process_wait_for_event();
    	while (TRUE) {
	    pak = process_dequeue(iframeQ);
	    if (!pak)
	    	break;
	    hwidb = hwidb_or_null(pak->if_input);
	    if (!hwidb) {
	    	datagram_done(pak);
	    	continue;
	    }

	    /* 
	     * Parse the packet
	     */
	    l3_ie_parse(hwidb, pak);

	    /* 
	     * Incoming I frames are always destroyed here.
	     */
	    datagram_done(pak);

	    /* 
	     * Do not hog the processor for too long.
	     */
	    if ((++count % 10) == 0)
		process_suspend();
    	}
    }
}

/* 
 * l3_ie_parse
 * Look through the guts of the incoming packet and categorize 
 * according to the message type. Process the message. A non null
 * return value means that an abnormal situation was detected. 
 * The protocol discriminator, the call reference and the message 
 * type will be handled within this routine. The rest of the IEs will 
 * be handled by a message specific routine.
 */
void l3_ie_parse ( 
	hwidbtype *hwidb,
	paktype *pak)
{
    boolean		new_svc;
    uchar		*ie_ptr;
    uchar 		*bkp_ptr;
    uchar		msg_type;
    uint 		ret;
    uint		call_ref;
    fr_svc_info_type 	*svc;

    call_ref = 0; 

    if (L3IE_DBG)
	buginf("\n\n\nNew Incoming message on %s: ", hwidb->hw_namestring);

    new_svc = FALSE;

    /* 
     * I frames have a 4 byte header. A 2 byte dlci followed by a 
     * send sequence number followed by a receive sequence number.
     * The layer 2 code deals with them. 
     */
    ie_ptr = pak->datagramstart;

    /* 
     * We only support Q.931 messages. The protocol disciminator
     * must always have the value 0x08.
     */
    ret = check_protocol_discriminator(&ie_ptr);

    /* 
     * As per 5.8.1 of Q931, ignore any messages where the 
     * protocol discriminator is in error. i.e. act as if the 
     * message was never received.
     */
    if (ret) {
	return;
    }
	
    /* 
     * Save the ie_ptr before jumping into check_call_reference()
     * because we may need to print the call reference value but the 
     * call to check_call_reference() changes the ie_ptr pointer.
     */
    bkp_ptr = ie_ptr;
    ret = check_call_reference(&ie_ptr);

    /* 
     * As per 5.8.3 of Q931, ignore any messages where the 
     * call reference is in error. i.e. act as if the 
     * message was never received. 
     */
    if ((ret == INVALID_CALL_REF_LENGTH) || (ret == INVALID_IE_TYPE)) {
	return;
    }
    
    /* 
     * A call reference may be assigned by the user or the network.
     * Knowing that the destination side always sets the flag bit
     * to 1, a good mechanism for distinguishing between incoming 
     * and outgoing call references is to always flip the flag bit 
     * on incoming messages. So, call originating on this router 
     * will have a call reference between 1 and 0x7FFF, while incoming 
     * replies will have call references between 0x8000 and 0xFFFF.
     */
    if (ret & 0x00008000)		/* flag is set, unset it */
	ret = (ret & 0x00007FFF);
    else 				/* flag not set, set it */
	ret = (ret | 0x00008000);

    /* 
     * if the returned value is not the global call reference, we have 
     * a message directed to a specific call reference. Ignore global 
     * call references for now. shankar 5/95.
     */
    if (ret != GLOBAL_CALL_REF) {
	call_ref = ret;

	if (call_ref > MAX_CALL_REF_PER_BOX) {
	    if (L3IE_DBG) {
	    	buginf("\nL3CC IN : Call Ref %d exceeds maximum", call_ref);
		svc_ie_print("Call Reference", (char *)bkp_ptr, 
						CALL_REF_TOTAL_LENGTH, IN);
	    }
	    return;
	}
    } else {
	call_ref = 0;

	/* 
	 * A global call reference refers to ALL call references 
	 * associated with a particular data link connection. Section 4.3
	 * of Q.931.
	 */
    }

    /* 
     * Allocate the svc structure if it isn't already assigned.
     * First look it up in the IDB SVC queue. Start the call off 
     * in STATE_NULL. 
     */
    svc = fr_svc_lookup(hwidb, call_ref);
    if (!svc) {

    	svc = reg_invoke_assign_svc_mem();

	if (!svc)
	    return;

	new_svc = TRUE;
	svc->call_state = STATE_NULL;
    	svc->params.call_ref = call_ref;
    	svc->call_id = fr_svc_select_next_call_id(hwidb);
    	add_fr_svc_to_idb_queue(hwidb, svc);
    }

    svc->last_ie_seen = CALL_REF_IE_ID;
	
    /* 
     * At this point, the ie_ptr should be point at the message type.
     * switch on message type and process each message. As per 5.8.2 of
     * Q.931, if a message is too short to contain a message type IE, 
     * drop the message. Actually, if the message type is unknown, 
     * we'll drop the packet.
     */
    switch (*ie_ptr) {
	case MSG_SETUP:
	    if (L3IE_DBG)
		buginf("\n\nIncoming SETUP : ");
	    svc->last_msg_seen = MSG_SETUP;

	    if (call_ref == GLOBAL_CALL_REF) {
		/* 
		 * send INVALID CALL REF, cause 81,  Q931 5.8.3.2  section f 
		 */
		svc->params.cause = INVALID_CALL_REF_VALUE;
		fr_svc_send_msg(hwidb, svc, MSG_STATUS, call_ref, &svc->params);

	    	if (new_svc)
		    free(svc);

		return;
	    }
	    msg_type = MSG_SETUP;
	    ie_ptr++;		/* Get past the message type byte */
	    ret = process_incoming_setup_msg(pak, svc, &ie_ptr, call_ref);
	    break;
	case MSG_CALL_PROCEEDING:
	    if (L3IE_DBG)
		buginf("\n\nIncoming CALL_PROCEEDING: ");
	    svc->last_msg_seen = MSG_CALL_PROCEEDING;

	    if (call_ref == GLOBAL_CALL_REF) {
		/* 
		 * send INVALID CALL REF Q931 5.8.3.2 f 
		 */
		svc->params.cause = INVALID_CALL_REF_VALUE;
		fr_svc_send_msg(hwidb, svc, MSG_STATUS, call_ref, &svc->params);

	    	if (new_svc)
		    free(svc);

		return;
	    }
	    msg_type = MSG_CALL_PROCEEDING;
	    ie_ptr++;		/* Get past the message type byte */
	    ret = process_incoming_call_proc_msg(pak, svc, &ie_ptr, call_ref);
	    break;
	case MSG_CONNECT:
	    if (L3IE_DBG)
		buginf("\n\nIncoming CONNECT : ");
	    svc->last_msg_seen = MSG_CONNECT;

	    if (ret == GLOBAL_CALL_REF) {
		/* 
		 * send INVALID CALL REF Q931 5.8.3.2 f
		 */
		svc->params.cause = INVALID_CALL_REF_VALUE;
		fr_svc_send_msg(hwidb, svc, MSG_STATUS, call_ref, &svc->params);

	    	if (new_svc)
		    free(svc);

		return;
	    }
	    msg_type = MSG_CONNECT;
	    ie_ptr++;		/* Get past the message type byte */
	    ret = process_incoming_connect_msg(pak, svc, &ie_ptr, call_ref);
	    break;
	case MSG_DISCONNECT:
	    if (L3IE_DBG)
		buginf("\n\nIncoming DISCONNECT : ");
	    svc->last_msg_seen = MSG_DISCONNECT;

	    if (ret == GLOBAL_CALL_REF) {
		/* 
		 * send INVALID CALL REF Q931 5.8.3.2 f
		 */
		svc->params.cause = INVALID_CALL_REF_VALUE;
		fr_svc_send_msg(hwidb, svc, MSG_STATUS, call_ref, &svc->params);

	    	if (new_svc)
		    free(svc);

		return;
	    }
	    msg_type = MSG_DISCONNECT;
	    ie_ptr++;		/* Get past the message type byte */
	    ret = process_incoming_disconnect_msg(pak, svc, &ie_ptr, call_ref);
	    break;
	case MSG_RELEASE:
	    if (L3IE_DBG)
		buginf("\n\nIncoming RELEASE: ");
	    svc->last_msg_seen = MSG_RELEASE;

	    if (ret == GLOBAL_CALL_REF) {
		/* 
		 * send INVALID CALL REF Q931 5.8.3.2 f
		 */
		svc->params.cause = INVALID_CALL_REF_VALUE;
		fr_svc_send_msg(hwidb, svc, MSG_STATUS, call_ref, &svc->params);

	    	if (new_svc)
		    free(svc);

		return;
	    }
	    msg_type = MSG_RELEASE;
	    ie_ptr++;		/* Get past the message type byte */
	    ret = process_incoming_release_msg(pak, svc, &ie_ptr, call_ref);
	    break;
	case MSG_RELEASE_COMPLETE:
	    if (L3IE_DBG)
		buginf("\n\nIncoming RELEASE_COMPLETE : ");
	    svc->last_msg_seen = MSG_RELEASE_COMPLETE;

	    if (ret == GLOBAL_CALL_REF) {
		/* 
		 * send INVALID CALL REF Q931 5.8.3.2 f
		 */
		svc->params.cause = INVALID_CALL_REF_VALUE;
		fr_svc_send_msg(hwidb, svc, MSG_STATUS, call_ref, &svc->params);

	    	if (new_svc)
		    free(svc);

		return;
	    }
	    msg_type = MSG_RELEASE_COMPLETE;
	    ie_ptr++;		/* Get past the message type byte */
	    ret = process_incoming_release_comp_msg(pak, svc, &ie_ptr, call_ref);
	    break;
	case MSG_STATUS:
	    if (L3IE_DBG)
		buginf("\n\nIncoming STATUS : ");
	    svc->last_msg_seen = MSG_STATUS;

	    /* 
	     * Need some special processing. See 5.8.11, pages 153 and 154
	     * of Q.931. A global call reference is legal here!!!!!!!!
	     */

	    msg_type = MSG_STATUS;
	    ie_ptr++;		/* Get past the message type byte */
	    ret = process_incoming_status_msg(pak, svc, &ie_ptr, call_ref);
	    break;
	case MSG_STATUS_ENQUIRY:
	    if (L3IE_DBG)
		buginf("\n\nIncoming STATUS_ENQUIRY : ");
	    svc->last_msg_seen = MSG_STATUS_ENQUIRY;

	    if (ret == GLOBAL_CALL_REF) {
		/* 
		 * send INVALID CALL REF Q931 5.8.3.2 f
		 */
		svc->params.cause = INVALID_CALL_REF_VALUE;
		fr_svc_send_msg(hwidb, svc, MSG_STATUS, call_ref, &svc->params);

	    	if (new_svc)
		    free(svc);

		return;
	    }
	    svc->params.cause = RESP_TO_STATUS_ENQ;
	    msg_type = MSG_STATUS_ENQUIRY;
	    ie_ptr++;		/* Get past the message type byte */
	    ret = process_incoming_status_enquiry_msg(pak, svc, &ie_ptr, call_ref);
	    break;
	default:
	    msg_type = UNKNOWN_MSG;
	    /* 
	     * set the return value to a non-zero number so that 
	     * we won't process this message. 
	     */
	    if (L3IE_DBG)
		buginf("\nL3CC IN: Unknown incoming message %d", *ie_ptr);
	    ret = UNKNOWN_MSG;

	    if (new_svc)
		free(svc);

	    break;
    }
    if (ret) {
	if (L3IE_DBG)
	    buginf("\nL3CC IN: Error during IE processing %d, aborting", ret);
	return;
    }
    /* 
     * All the IEs have been processed. Now, go through the state 
     * handling function. 
     * svc_l3_sdl_u.c
     */
    ret = svc_process_l3_event(pak, svc, call_ref, msg_type, &svc->params, IN);
}
/* 
 * check_protocol_discriminator
 * Distinguish messages for user-network call control from other messages
 * that are yet to be defined. See Pages 53 and 54, Section 4.2, Q.931. 
 * Sanity check byte 3 of the incoming message. It must be ANSI
 * or CCITT since it is carried over DLCI 0. Ref: Q931, 5.8.1
 */
int check_protocol_discriminator (uchar **ptr) 
{
    uchar 	**ie_ptr;

    ie_ptr = ptr;
    if (**ie_ptr != ANSI_LMI_PROT_DISC) {
	return(INVALID_PROT_DISC);
    }

    if (L3IE_DBG)
	buginf("\nL3CC IN : Protocol Discriminator is %x", **ie_ptr);

    /* 
     * point to the next IE in the packet
     * 1 is the length of the protocol discriminator field.
     */
    *ie_ptr = *ie_ptr + 1;
    return(IS_OKAY);
}
	
/* 
 * check_call_reference
 * Ref: Section 4.3, Pages 55-56 of Q.931
 * The call reference is always 3 bytes long as per FRF.4. Bits 0-3 of 
 * octet 1 contain the length, 0010(binary), and the other bits contain 0.
 * Ref: Q931, 5.8.3.1
 * 
 * NOTE: while the INVALID_CALL_REF_VALUE parameter would be a more 
 * appropriate value to return in some of the cases below, it has 
 * the value 0x81 which I would expect is a very LEGAL call reference
 * value. To avoid problems with this return code, INVALID_IE_TYPE is 
 * used instead. This is just a programmer idiosyncracy and hs nothing to 
 * do with IE handling!
 * 
 * A dummy call reference is one octet long and is coded 0000 0000.
 * The use of a dummy call reference is not currently supported.
 */
int check_call_reference (uchar **ptr) 
{
    call_reference		*ie_ptr;
    ushort 			call_ref;

    ie_ptr = (call_reference *)*ptr;

    /* 
     * The FR SVC implementation agreement, section 3.1.2 specifies that 
     * the call reference IE is FIXED 3 bytes. The lower 4 bits
     * contain the length. The upper 4 bits must always be zero. 
     * As per Q.931 page 148, 5.8.3.1, if bits 5-8 are not zero,
     * ignore the message.
     */
    if (ie_ptr->length & 0xF0)
	return(INVALID_IE_TYPE);

    /* 
     * The FR SVC implementation agreement states that the 
     * length of the call reference is 2, always. By checking the 
     * length, we are precluding the use of a dummy call reference.
     */
    if ((ie_ptr->length & 0x0F) != CALL_REF_IE_LENGTH) {
	return(INVALID_CALL_REF_LENGTH);
    }

    /* 
     * bit 8 of byte 2 indicates the origination side. This is an incoming
     * message and if the bit is set, the call reference was assigned by us. 
     * If the bit is not set, the call reference was assigned by the switch.
     * A value of all zero is a global call reference. CALL_REF_FLAG_MASK is 
     * defined as 0x7F. For the record, print out who assigned the CR. 
     */
    if (ie_ptr->byte2 & ~CALL_REF_FLAG_MASK) {
	if (L3IE_DBG)
	    buginf("\nL3CC IN : Call Reference assigned by Router Side");
    } else {
	if (L3IE_DBG)
	    buginf("\nL3CC IN : Call Reference assigned by Remote Side");
    }

    /* 
     * As if things aren't complicated enough, we need to check for a 
     * global call reference. Global call references are being ignored 
     * for now. When both byte2 and byte3 are 0, we have a global 
     * call reference.
     */
    if (!(ie_ptr->byte2 & CALL_REF_FLAG_MASK) && !ie_ptr->byte3) {
	return(GLOBAL_CALL_REF);
    }

    /* 
     * The call reference is spread over two bytes. bitwise OR the bits 
     * and return the value. The flag bit is automatically included.
     */
    call_ref = ((ie_ptr->byte2  << 8) | ie_ptr->byte3);

    if (L3IE_DBG)
	buginf("\nL3CC IN : Call Reference is 0x%x", call_ref);

    /* 
     * point to the next IE. The call reference should always be 
     * 3 bytes long.
     */
    *ptr = *ptr + CALL_REF_TOTAL_LENGTH;	
    return(call_ref);
}
/* 
 * check_bearer_cap_ie
 * Used to indicate that a requested CCITT Recommendation I.233
 * bearer service is to be provided by the network. It only contains
 * info that will be used by the network. The maximum length 
 * of this IE is 5 bytes. As per Q.933, the Bearer Capability is only 
 * a part of a SETUP message. It is not contained within other messages.
 */
int check_bearer_cap_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref, 
	uchar msg_type)
{
    bearer_cap_ie 	*ie_ptr;

    ie_ptr = (bearer_cap_ie *)*ptr;

    if (ie_ptr->ie_id != BEARER_CAPABILITY_IE_ID) {
	return(INVALID_IE_TYPE);
    }

    /* 
     * Section 4.5.1, Q.931 specifies that the bearer capability IE 
     * may be repeated. Process all repeated bearer capability IEs. 
     * The <= in the line below will allow us to process all repeated IEs. 
     */
    if (svc->last_ie_seen <= BEARER_CAPABILITY_IE_ID)
    	svc->last_ie_seen = BEARER_CAPABILITY_IE_ID;
    else 
	return(INVALID_IE_TYPE);

    if (L3IE_DBG) {
	svc_ie_print("Bearer Capability : ", (char *)ie_ptr, 
					BEARER_CAP_IE_TOTAL_LENGTH, IN);
    }

    if (!((ie_ptr->length) > 0 && 
		(ie_ptr->length <= BEARER_CAP_IE_LENGTH))) {
	if (L3IE_DBG)
	    buginf("\nL3CC IN : Invalid length for Bearer Capability IE,"
					"call ref %d", call_ref);
	return(INVALID_IE_LENGTH);
    }

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + BEARER_CAP_HDR_LENGTH;

    /* 
     * Token check of the other fields in the bearer capability field. 
     * BEARER_CAP_BYTE3, BEARER_CAP_BYTE4, BEARER_CAP_BYTE5 are the 
     * hard coded values that we expect. 
     */
    if ((ie_ptr->byte3 & BEARER_CAP_BYTE3) != BEARER_CAP_BYTE3) {
	if (L3IE_DBG)
	    buginf("\nL3CC IN: Bearer Capability Byte 3 problem");
	return(UNKNOWN_BEARER_CAP_BYTE3);
    }
    if ((ie_ptr->byte4 & BEARER_CAP_BYTE4) != BEARER_CAP_BYTE4) {
	if (L3IE_DBG)
	    buginf("\nL3CC IN : Frame Mode not selected %d", ie_ptr->byte4);
	return(UNKNOWN_BEARER_CAP_BYTE4);
    }
    if ((ie_ptr->byte5 & BEARER_CAP_BYTE5_B) != BEARER_CAP_BYTE5_B) {
	if (L3IE_DBG)
	    buginf("\nL3CC IN : Illegal layer 2 information %d", ie_ptr->byte5);
	return(UNKNOWN_BEARER_CAP_BYTE5);
    }
    /* 
     * Indicate that we now have processed a valid bearer capability IE.
     */
    svc->bearer_cap_ie_present = TRUE;

    /* 
     * everything is okay!
     */
    return(IS_OKAY);
}
/* 
 * check_dlci_ie
 * The DLCI IE identifies the DLCI option, preferred or exclusive,
 * and the DLCI requested or assigned. This IE is mandatory and is 
 * always present in the SETUP message or in the first response to 
 * the SETUP message. 
 */
int check_dlci_ie (
	fr_svc_info_type *svc, 
	uchar **ptr,
	uint call_ref, 
	uchar msg_type)
{
    dlci_ie 		*ie_ptr;
    ushort 		dlci, tmp_short;

    ie_ptr = (dlci_ie *)*ptr;

    if (ie_ptr->ie_id != DLCI_IE_ID) {
	return(INVALID_IE_TYPE);
    }

    /* 
     * If the message contains repeated DLCI IEs, ignore all the 
     * the following ones. 
     */
    if (svc->dlci_ie_present) {
	*ptr = *ptr + ie_ptr->length + DLCI_IE_HDR_LENGTH;
    	return(IS_OKAY);
    }

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < DLCI_IE_ID)
    	svc->last_ie_seen = DLCI_IE_ID;
    else 
	return(INVALID_IE_TYPE);

    if (L3IE_DBG) {
	svc_ie_print("DLCI IE", (char *)ie_ptr, DLCI_IE_TOTAL_LENGTH, IN);
    }

    if (!ie_ptr->length) {
	*ptr = *ptr + DLCI_IE_HDR_LENGTH;
	return(INVALID_IE_TYPE);
    }
    /* 
     * Do a basic sanity check, The length needs to be greater than 0, 
     * and <= 2. 
     */
    dlci = 0;
    tmp_short = 0;

    /* 
     * eventhough there is a length check here, I would think that 
     * the dlci IE should be of a standard 4 byte length. 
     * Gather the DLCI bits together. Mumbo-jumbo!
     */
    if ((ie_ptr->length > 0) && (ie_ptr->length <= DLCI_IE_LENGTH)) {
    	tmp_short = ((ie_ptr->dlci_u << 4) & 0x03F0);
	dlci |= tmp_short;
	tmp_short = 0;
	tmp_short = (((ie_ptr->dlci_l) >> 3) & 0x0F);
	dlci |= tmp_short;
	svc->params.dlci = dlci;

	if (L3IE_DBG)
	    buginf("\nL3CC IN: DLCI for call %d", dlci);

    } else {
	return(INVALID_IE_TYPE);
    }

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + DLCI_IE_HDR_LENGTH;

    svc->dlci_ie_present = TRUE;
    return(IS_OKAY);
}
/* 
 * check_cg_party_number_ie
 * Check the calling party IE.
 * Identify the origin of a call. Cannot be repeated. The IE 
 * length is network dependent. The number is carried in ASCII
 * form.
 */
int check_cg_party_number_ie (
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref, 
	uchar msg_type)
{
    calling_pty_number_ie 	*ie_ptr;
    uchar 			cg_num_length;

    ie_ptr = (calling_pty_number_ie *)*ptr;
    cg_num_length = 0;

    if (!(ie_ptr->ie_id == CALLING_PARTY_NUMBER_ID))
	return(INVALID_IE_TYPE);

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < CALLING_PARTY_NUMBER_ID)
    	svc->last_ie_seen = CALLING_PARTY_NUMBER_ID;
    else 
	return(INVALID_IE_TYPE);

    if (ie_ptr->length == 0) {
	*ptr = *ptr + CALLING_PARTY_IE_HDR_LENGTH;
	return(INVALID_IE_LENGTH);
    }

    /* 
     * Initialize the length of the calling number field.
     * We need this administrative counter because we don't know what the 
     * length of the number field is. We have to effectively deduce this
     * by decrementing the non-number fields from the total IE length.
     */
    cg_num_length = ie_ptr->length;

    if (L3IE_DBG) {
	svc_ie_print("Calling Party IE", (char *)ie_ptr, ie_ptr->length + 
				CALLING_PARTY_IE_HDR_LENGTH, IN);
    }

    /* 
     * International number etc 
     * Right shift by 4 to right justify the type bits.
     */
    svc->params.svc_addr.saddr.num_type = (ie_ptr->numb_type_id & 0x70) >> 4;

    /* 
     * E164 or X121 
     * Zero out the other bits to isolate the format bits.
     * Adjust the calling address field by one byte to account 
     * for the byte that contains the plan and type.
     */
    svc->params.svc_addr.saddr.num_plan = (ie_ptr->numb_type_id & 0x0F);
    cg_num_length--;

    /* 
     * Check whether byte 3a exists. If it does, ignore the 
     * contents but adjust the calling party field length. 
     * If the field does not exist, then the address starts
     * one byte above at the indicator field.
     */
    if (ie_ptr->numb_type_id & EXT_BIT_1) {
	ie_ptr--;
    } else {
    	cg_num_length--;
    }

    /* 
     * The number is carried in IA5 format, or ASCII. 
     * The number length field is derived from the ie length.
     */
    sstrncpy((uchar *)&svc->params.svc_addr.saddr.addr, 
		(uchar *)&ie_ptr->address, cg_num_length + 1);

    svc->params.svc_addr.saddr.num_len = 
			strlen((uchar *)&svc->params.svc_addr.saddr.addr);
    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + CALLING_PARTY_IE_HDR_LENGTH;

    svc->cg_pty_num_ie_present = TRUE;
    return(IS_OKAY);	
}

/* 
 * check_cg_party_subaddr_ie
 * Check the calling party subaddress IE.
 * Used to identify a subaddress associated with the origin of a call. 
 * Not repeatable within a message. 
 */
int check_cg_party_subaddr_ie (
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref, 
	uchar msg_type)
{
    calling_pty_subaddr_ie 	*ie_ptr;

    ie_ptr = (calling_pty_subaddr_ie *)*ptr;

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < CALLING_PARTY_SUBADDR_ID)
    	svc->last_ie_seen = CALLING_PARTY_SUBADDR_ID;
    else 
	return(INVALID_IE_TYPE);

    if (L3IE_DBG) {
	svc_ie_print("Calling Pty. Subaddr", (char *)ie_ptr, ie_ptr->length + 
				CALLING_PARTY_SUBADDR_HDR_LNGTH, IN);
    }

    /* 
     * Retrieve the calling party subaddress and stuff it into the 
     * called party subaddr. Experiment with this inverse relationship.
     */
    svc->params.svc_addr.daddr.subaddr = ie_ptr->address;

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + CALLING_PARTY_SUBADDR_HDR_LNGTH;
    svc->cg_subaddr_ie_present = TRUE;
    return(IS_OKAY);
}
/* 
 * check_cd_party_number_ie
 * Check the called party IE.
 * Used to identify the called party of a call. 
 */
int check_cd_party_number_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref, 
	uchar msg_type)
{
    called_pty_ie 	*ie_ptr;
    uchar 		cd_num_length;

    ie_ptr = (called_pty_ie *)*ptr;
    cd_num_length = 0;

    if (!(ie_ptr->ie_id == CALLED_PARTY_NUMBER_ID))
	return(INVALID_IE_TYPE);

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < CALLED_PARTY_NUMBER_ID)
    	svc->last_ie_seen = CALLED_PARTY_NUMBER_ID;
    else 
	return(INVALID_IE_TYPE);

    if (ie_ptr->length == 0) {
	*ptr = *ptr + CALLING_PARTY_IE_HDR_LENGTH;
	return(INVALID_IE_LENGTH);
    }

    if (L3IE_DBG) {
	svc_ie_print("Called Party IE", (char *)ie_ptr, ie_ptr->length + 
				CALLING_PARTY_IE_HDR_LENGTH, IN);
    }

    /* 
     * Initialize the local administrative length counter.
     * We need this administrative counter because we don't know what the 
     * length of the number field is. We have to effectively deduce this
     * by decrementing the non-number fields from the total IE length.
     */
    cd_num_length = ie_ptr->length;

    /* 
     * retrieve information on number type etc. and adjust length field. 
     * String copy the address over.
     */
    svc->params.svc_addr.daddr.num_type = (ie_ptr->numb_type_id & 0x70) >> 4;
    svc->params.svc_addr.daddr.num_plan = (ie_ptr->numb_type_id & 0x0F);

    /* Account for the 1 byte of generic information */
    cd_num_length--;

    sstrncpy((uchar *)&svc->params.svc_addr.daddr.addr, 
		(uchar *)&ie_ptr->address, cd_num_length + 1);

    svc->params.svc_addr.daddr.num_len = 
			strlen((uchar *)&svc->params.svc_addr.daddr.addr);
    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + CALLING_PARTY_IE_HDR_LENGTH;

    svc->cd_pty_ie_present = TRUE;
    return(IS_OKAY);	
}
/* 
 * check_cd_party_subaddr_ie
 * Check the Called Party Subaddress IE.
 * Used to identify the subaddress of the call destination. 
 */
int check_cd_party_subaddr_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref, 
	uchar msg_type)
{
    called_pty_subaddr_ie *ie_ptr;

    ie_ptr = (called_pty_subaddr_ie *)*ptr;

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < CALLED_PARTY_SUBADDR_ID)
    	svc->last_ie_seen = CALLED_PARTY_SUBADDR_ID;
    else 
	return(INVALID_IE_TYPE);

    if (!(ie_ptr->ie_id == CALLED_PARTY_SUBADDR_ID))
	return(INVALID_IE_TYPE);

    if (L3IE_DBG) {
	svc_ie_print("Called Pty. Subaddr", (char *)ie_ptr, ie_ptr->length + 
				CALLED_PARTY_SUBADDR_HDR_LNGTH, IN);
    }

    /* 
     * Retrieve the called party subaddress and stuff it into the 
     * calling party subaddr. Experiment with this inverse relationship.
     */
    svc->params.svc_addr.saddr.subaddr = ie_ptr->address;

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + CALLED_PARTY_SUBADDR_HDR_LNGTH;

    svc->cd_subaddr_ie_present = TRUE;
    return(IS_OKAY);
}
/* 
 * check_con_party_number_ie
 * Used to identify the connected party of a call. 
 * We really do nothing with this IE. It is ignored. Basic call 
 * security/checking is done at the NLI.
 */
int check_con_party_number_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref, 
	uchar msg_type)
{
    connected_pty_ie 	*ie_ptr;
    uchar 		cn_num_length;

    ie_ptr = (connected_pty_ie *)*ptr;
    cn_num_length = 0;

    if (!(ie_ptr->ie_id == CONN_PARTY_NUMBER_ID))
	return(INVALID_IE_TYPE);

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < CONN_PARTY_NUMBER_ID)
    	svc->last_ie_seen = CONN_PARTY_NUMBER_ID;
    else 
	return(INVALID_IE_TYPE);

    if (ie_ptr->length == 0) {
	*ptr = *ptr + CONN_PARTY_IE_HDR_LENGTH;
	return(INVALID_IE_LENGTH);
    }

    if (L3IE_DBG) {
	svc_ie_print("Connected Party IE", (char *)ie_ptr, 
			ie_ptr->length + CONN_PARTY_IE_HDR_LENGTH, IN);
    }

    /* 
     * Initialize the length of the calling number field.
     * We need this administrative counter because we don't know what the 
     * length of the number field is. We have to effectively deduce this
     * by decrementing the non-number fields from the total IE length.
     */
    cn_num_length = ie_ptr->length;

    svc->params.svc_addr.caddr.num_type = (ie_ptr->numb_type_id & 0x70) >> 4;
    svc->params.svc_addr.caddr.num_plan = (ie_ptr->numb_type_id & 0x0F);

    /* Account for the type byte and the indicators byte */
    cn_num_length--;
    cn_num_length--;

    sstrncpy((uchar *)&svc->params.svc_addr.caddr.addr, 
			(uchar *)&ie_ptr->address, cn_num_length + 1);

    svc->params.svc_addr.caddr.num_len = 
			strlen((uchar *)&svc->params.svc_addr.caddr.addr);

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + CALLING_PARTY_IE_HDR_LENGTH;

    svc->cn_number_ie_present = TRUE;
    return(IS_OKAY);	
}
/* 
 * check_con_party_subaddr_ie
 * Check the Connected Party Subaddress IE.
 * Used to identify the subaddress destination to whom we actually 
 * connected. 
 * We really do nothing with this IE. It is ignored. Basic call 
 * security/checking is done at the NLI.
 */
int check_con_party_subaddr_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref, 
	uchar msg_type)
{
    connected_num_subaddr_ie 	*ie_ptr;

    ie_ptr = (connected_num_subaddr_ie *)*ptr;

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < CONN_PARTY_NUMBER_SUBADDR_ID)
    	svc->last_ie_seen = CONN_PARTY_NUMBER_SUBADDR_ID;
    else 
	return(INVALID_IE_TYPE);

    if (!(ie_ptr->ie_id == CONN_PARTY_NUMBER_SUBADDR_ID))
	return(INVALID_IE_TYPE);

    if (L3IE_DBG) {
	svc_ie_print("Connected Pty. Subaddr", (char *)ie_ptr, ie_ptr->length + 
				CALLED_PARTY_SUBADDR_HDR_LNGTH, IN);
    }

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + CONN_PARTY_SUBADDR_IE_HDR_LENGTH;

    svc->cn_subaddr_ie_present = TRUE;
    return(IS_OKAY);
}
/* 
 * check_xit_net_sel_ie
 * Check the Transit Network Selection IE. This has been designated as 
 * a Future Use only IE so ignore for now. shankar 5/95
 */
int check_xit_net_sel_ie (
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref,
	uchar msg_type)
{
    transit_network_selection_ie 	*ie_ptr;

    ie_ptr = (transit_network_selection_ie *)*ptr;

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < TRANSIT_NETWORK_IE_ID)
    	svc->last_ie_seen = TRANSIT_NETWORK_IE_ID;
    else 
	return(INVALID_IE_TYPE);

    if (L3IE_DBG) {
	svc_ie_print("Transit Network Selection", (char *)ie_ptr, ie_ptr->length + 
				TRANSIT_NETWORK_HDR_LENGTH, IN);
    }

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + TRANSIT_NETWORK_HDR_LENGTH;

    svc->xit_nwk_ie_present = TRUE;
    return(IS_OKAY);
}
/* 
 * check_llc_param_ie
 * Check the Link Core Parameters IE. Uses a different format 
 * within the SETUP than it does for the CONNECT.
 * Used to indicate the core service quality parameter 
 * values to be used for the frame mode call. 
 * Maximum length is 27 octets. Could also be 31 in the future.
 */
static int check_llc_param_ie(
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    link_core_ie 		*ie_ptr;
    fmif_ie 			*fmif_ptr;
    throughput			*thrupt_ptr;
    min_accept_throughput_ie 	*matptr;
    committed_burst_size_ie 	*cbsptr;
    excess_burst_size_ie	*ebsptr;
    cbs_mag_ie			*cbs_magptr;
    ebs_mag_ie 			*ebs_magptr;
    uchar 			*ifptr;
    int				ret;
    uchar 			llc_ie_length, tmp_c;

    ie_ptr = (link_core_ie *)*ptr;
    ret = 0;

    if (ie_ptr->ie_id != LINK_LAYER_CORE_PARAM_IE_ID) {
	if (L3IE_DBG)
	    buginf("\nL3CC IN :  Expected LLCore IE, Got %x", ie_ptr->ie_id);	
	return(INVALID_IE_TYPE);
    }

    /* 
     * If we have multiple LLC IEs, we don't want to process them all. 
     * Just skip over the repeated ones. 
     */
    if (svc->ll_core_ie_present) {
	*ptr = *ptr + ie_ptr->length + LINK_CORE_IE_HDR_LENGTH;
    	return(IS_OKAY);
    }

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < LINK_LAYER_CORE_PARAM_IE_ID)
    	svc->last_ie_seen = LINK_LAYER_CORE_PARAM_IE_ID;
    else 
	return(INVALID_IE_TYPE);

    if (!ie_ptr->length) {
	*ptr = *ptr + LINK_CORE_IE_HDR_LENGTH;
	return(INVALID_IE_TYPE);
    }
    if (ie_ptr->length > LINK_CORE_IE_MAX_LENGTH) {
	if (L3IE_DBG)
	    buginf("\nL3CC IN : Link Core IE is too long");
	return(IE_TOO_LONG);
    }

    llc_ie_length = ie_ptr->length;
    ifptr = (uchar *)ie_ptr;

    /* 
     * By incrementing twice, we are stepping past the 
     * 2 byte IE header, the type and the length fields. 
     */
    ifptr++;			/* Point to the first info field */
    ifptr++;			/* Point to the first info field */

    svc->last_llc_ie_seen = 0;

    /* 
     * Modular approach to processing optional fields in the Link Core IE.
     * If a field exists, we'll process it. At the end of processing a 
     * field, we'll point at the next major field. For each, a sanity 
     * check is made to make sure that it is present in the proper order.
     */
    while (llc_ie_length > 0) {
	switch (*ifptr) {
	    case FMIF_IE_ID:
		tmp_c = *ifptr;
		fmif_ptr = (fmif_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = FMIF_IE_ID;
		ret = check_fmif_ie(svc, (uchar **)&fmif_ptr, call_ref, 
								msg_type);
		ifptr = (uchar *)fmif_ptr;
		break;
	    case THROUGHPUT_IE_ID:
		tmp_c = *ifptr;
		thrupt_ptr = (throughput *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = THROUGHPUT_IE_ID;
		ret = check_thruput_ie(svc, (uchar **)&thrupt_ptr, call_ref, 
								msg_type);
		ifptr = (uchar *)thrupt_ptr;
		break;
	    case MIN_ACCEPTABLE_THROUGHPUT:
		tmp_c = *ifptr;
		matptr = (min_accept_throughput_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = MIN_ACCEPTABLE_THROUGHPUT;
		ret = check_min_thruput_ie(svc, (uchar **)&matptr, call_ref, 
								msg_type);
		ifptr = (uchar *)matptr;
		break;
	    case COMMITTED_BURST_SZ_IE_ID:
		tmp_c = *ifptr;
		cbsptr = (committed_burst_size_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = COMMITTED_BURST_SZ_IE_ID;
		ret = check_cbs_ie(svc, (uchar **)&cbsptr, call_ref, 
								msg_type);
		ifptr = (uchar *)cbsptr;
		break;
	    case EXCESS_BURST_SZ_IE_ID:
		tmp_c = *ifptr;
		ebsptr = (excess_burst_size_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = EXCESS_BURST_SZ_IE_ID;
		ret = check_ebs_ie(svc, (uchar **)&ebsptr, call_ref, 
								msg_type);

		/* 
		 * TEMPORARY until FRFTC95.090 gets implemented 
		 * This is a brute force approach to ending the 
		 * processing of the LLC IE. I will finesse this 
		 * in the near future. shankar 10/95.
		 */
		llc_ie_length = 0;

		ifptr = (uchar *)ebsptr;
		ret = 0;
		break;
	    case CBS_MAG_ID:
		tmp_c = *ifptr;
		cbs_magptr = (cbs_mag_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = CBS_MAG_ID;
		ret = check_cbs_mag_ie(svc, (uchar **)&cbs_magptr, call_ref,
								msg_type);
		ifptr = (uchar *)cbs_magptr;
		ret = 0;

		break;
	    case EBS_MAG_ID:
		tmp_c = *ifptr;
		ebs_magptr = (ebs_mag_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = EBS_MAG_ID;
		ret = check_ebs_mag_ie(svc, (uchar **)&ebs_magptr, call_ref,
								msg_type);
		llc_ie_length = 0;
		ifptr = (uchar *)ebs_magptr;
		break;
	    default:
		ret = INVALID_IE_TYPE;
		llc_ie_length = 0;	/* Get out of the while loop */
		if (L3IE_DBG)
		    buginf("\nL3CC in: Unknown LL Core IE %d", *ifptr);
		break;
    	}
    }

    svc->ll_core_ie_present = TRUE;

    /* 
     * If a failure was detected, ret is non NULL. 
     */
    if (ret)
	return(ret);

    /* 
     * everything is okay!
     */
    return(IS_OKAY);
}

/* 
 * check_connect_llc_param_ie
 * Used to indicate the core service quality parameter 
 * values awarded for the frame mode call. This IE follows 
 * a different format for the CONNECT message than it does for a 
 * SETUP. Hence, a different function is used.
 */
static int check_connect_llc_param_ie(
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    connect_link_core_ie 	*ie_ptr;
    fmif_ie 			*fmif_ptr;
    throughput			*thrupt_ptr;
    committed_burst_size_ie 	*cbsptr;
    excess_burst_size_ie	*ebsptr;
    cbs_mag_ie			*cbs_magptr;
    ebs_mag_ie 			*ebs_magptr;
    uchar 			*ifptr;
    int				ret;
    uchar 			llc_ie_length, tmp_c;

    ie_ptr = (connect_link_core_ie *)*ptr;
    ret = 0;

    if (ie_ptr->ie_id != LINK_LAYER_CORE_PARAM_IE_ID) {
	if (L3IE_DBG)
	    buginf("\nL3CC IN :  Expected CONNECT LLCore IE, Got %x", ie_ptr->ie_id);	
	return(INVALID_IE_TYPE);
    }

    if (svc->ll_core_ie_present) {
	*ptr = *ptr + ie_ptr->length + LINK_CORE_IE_HDR_LENGTH;
    	return(IS_OKAY);
    }

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < LINK_LAYER_CORE_PARAM_IE_ID)
    	svc->last_ie_seen = LINK_LAYER_CORE_PARAM_IE_ID;
    else 
	return(INVALID_IE_TYPE);

    if (!ie_ptr->length) {
	*ptr = *ptr + LINK_CORE_IE_HDR_LENGTH;
	return(INVALID_IE_TYPE);
    }
    if (ie_ptr->length > LINK_CORE_IE_MAX_LENGTH) {
	if (L3IE_DBG)
	    buginf("\nL3CC IN : CONNECT Link Core IE is too long");
	return(IE_TOO_LONG);
    }

    llc_ie_length = ie_ptr->length;
    ifptr = (uchar *)ie_ptr;

    /* 
     * By incrementing twice, we are stepping past the 
     * 2 byte IE header, the type and the length fields. 
     */
    ifptr++;			/* Point to the first info field */
    ifptr++;			/* Point to the first info field */

    svc->last_llc_ie_seen = 0;

    while (llc_ie_length > 0) {
	switch (*ifptr) {
	    case FMIF_IE_ID:
		tmp_c = *ifptr;
		fmif_ptr = (fmif_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = FMIF_IE_ID;
		ret = check_fmif_ie(svc, (uchar **)&fmif_ptr, call_ref, 
								msg_type);
		ifptr = (uchar *)fmif_ptr;
		break;
	    case THROUGHPUT_IE_ID:	/* should only be within the SETUP msg */
		tmp_c = *ifptr;
		thrupt_ptr = (throughput *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = THROUGHPUT_IE_ID;
		ret = check_thruput_ie(svc, (uchar **)&thrupt_ptr, call_ref, 
								msg_type);
		ifptr = (uchar *)thrupt_ptr;
		break;
	    case COMMITTED_BURST_SZ_IE_ID:
		tmp_c = *ifptr;
		cbsptr = (committed_burst_size_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = COMMITTED_BURST_SZ_IE_ID;
		ret = check_cbs_ie(svc, (uchar **)&cbsptr, call_ref, 
								msg_type);
		ifptr = (uchar *)cbsptr;
		break;
	    case EXCESS_BURST_SZ_IE_ID:
		tmp_c = *ifptr;
		ebsptr = (excess_burst_size_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = EXCESS_BURST_SZ_IE_ID;
		ret = check_ebs_ie(svc, (uchar **)&ebsptr, call_ref, 
								msg_type);

		/* TEMPORARY until FRFTC95.090 gets implemented */
		llc_ie_length = 0;	/* Get out of the while loop */

		ifptr = (uchar *)ebsptr;
		ret = 0;
		break;
	    case CBS_MAG_ID:
		tmp_c = *ifptr;
		cbs_magptr = (cbs_mag_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = CBS_MAG_ID;
		ret = check_cbs_mag_ie(svc, (uchar **)&cbs_magptr, call_ref,
								msg_type);
		ifptr = (uchar *)cbs_magptr;
		ret = 0;

		break;
	    case EBS_MAG_ID:
		tmp_c = *ifptr;
		ebs_magptr = (ebs_mag_ie *)ifptr;
		if (tmp_c < svc->last_llc_ie_seen)
		    return(INVALID_IE_TYPE);
		svc->last_llc_ie_seen = EBS_MAG_ID;
		ret = check_ebs_mag_ie(svc, (uchar **)&ebs_magptr, call_ref,
								msg_type);
		llc_ie_length = 0;
		ifptr = (uchar *)ebs_magptr;
		break;
	    default:
		ret = INVALID_IE_TYPE;
		llc_ie_length = 0;	/* Get out of the while loop */
		if (L3IE_DBG)
		    buginf("\nL3CC in: Unknown CONNECT LL Core IE %d", *ifptr);
		break;
    	}
    }

    if (ret)
	return(ret);

    svc->ll_core_ie_present = TRUE;
    return(IS_OKAY);
}
/* 
 * check_fmif_ie
 * The Frame mode IE is a part of the Link Layer Core parameters
 * IE. It contains the outgoing FMIF size and optionally, the incoming
 * FMIF size. record each value while parsing the IE. 
 * Lotsa mumbo-jumbo! See Q.933, section 4.5.19, for details of bit fields.
 */
int check_fmif_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    fmif_ie		*ie_ptr;
    uchar 		length;

    ie_ptr = (fmif_ie *)*ptr;
    length = 1;			/* Include the IE ID */

    svc->params.out_max_fmif = (((ie_ptr->outgoing_fmif_a & FMIF_UPPER_BITS) 
			<< FMIF_UPPER_BITS_SHIFT) | (ie_ptr->outgoing_fmif_b & 
							FMIF_LOWER_BITS)); 
    svc->params.out_fmif = svc->params.out_max_fmif;

    length += FMIF_IE_LENGTH;

    /* 
     * Point at byte 3c, refer Section 4.5.19 of Q.933.
     */
    *ptr = *ptr + FMIF_IE_LENGTH +1;

    /* 
     * Assume a symmetric FMIF field. 
     */
    svc->params.inc_max_fmif = svc->params.out_max_fmif;
    svc->params.inc_fmif = svc->params.out_fmif;

    /* 
     * Check whether the incoming FMIF field exists. 
     * if it does, update the incoming_max_fmif field. 
     */
    if (!(ie_ptr->outgoing_fmif_b & EXT_BIT_1_MASK)) {
	svc->params.inc_max_fmif = (((ie_ptr->incoming_fmif_a & FMIF_UPPER_BITS)
			<< FMIF_UPPER_BITS_SHIFT) | (ie_ptr->incoming_fmif_b &
							FMIF_LOWER_BITS));
	/* 
	 * since bytes 3c and 3d exist, increment by 2 so that we 
	 * now point to byte 4.
	 */
	svc->params.inc_fmif = svc->params.inc_max_fmif;
    	*ptr = *ptr + FMIF_IE_LENGTH;
	length += FMIF_IE_LENGTH;
    }

    if (L3IE_DBG) {
	svc_ie_print("FMIF IE ", (char *)ie_ptr, length, IN);
    }

return(IS_OKAY);
}

/* 
 * check_thruput_ie
 * Throughput values are included with the LLC IE. 
 * Lotsa mumbo-jumbo! See Q.933, section 4.5.19, for details of bit fields.
 */
int check_thruput_ie(
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    throughput 		*ie_ptr;
    uchar 		length;
    int			ix, tmp_mult;

    ie_ptr = (throughput *)*ptr;
    length = 1;			/* Include the IE ID */

    svc->params.out_cir_mag = (ie_ptr->out_mag_mult_a >> 4) & 0x07;
    svc->params.out_cir_mult = ((ie_ptr->out_mag_mult_a & 0x0F) << 7) | 
			(ie_ptr->out_mult_b & 0x7F);
    /* 
     * the throughput has a magnitude and a multiplier. Play games to 
     * figure out the values. 
     */
    if (svc->params.out_cir_mag) {
	tmp_mult = 1;
	for (ix= 0; ix < svc->params.out_cir_mag; ix++)
	    tmp_mult *= 10;
    	svc->params.out_cir = (tmp_mult) * (svc->params.out_cir_mult);
    } else {
	svc->params.out_cir = svc->params.out_cir_mult;
    }
    length += THROUGHPUT_IE_LENGTH;

    /* 
     * Point at byte 4c, refer section 4.5.19 of Q.933.
     */
    *ptr = *ptr + THROUGHPUT_IE_LENGTH + 1;

    svc->params.inc_cir_mag = svc->params.out_cir_mag;
    svc->params.inc_cir_mult = svc->params.out_cir_mult;

    /* 
     * The extension bit tells us whether the "outgoing" values exist.
     * the throughput has a magnitude and a multiplier. Play games to 
     * figure out the values. 
     */
    if (!(ie_ptr->out_mult_b & EXT_BIT_1_MASK)) {
    	svc->params.inc_cir_mag = (ie_ptr->inc_mag_mult_a >> 4) & 0x07;
	svc->params.inc_cir_mult = ((ie_ptr->inc_mag_mult_a & 0x0F) << 7) | 
					(ie_ptr->inc_mult_b & 0x7F);
	/* 
	 * since bytes 4c and 4d exist, increment by 2 so that we 
	 * now point to byte 5.
	 */
    	*ptr = *ptr + THROUGHPUT_IE_LENGTH;
    	length += THROUGHPUT_IE_LENGTH;
    }
    if (svc->params.inc_cir_mag) {
	tmp_mult = 1;
	for (ix = 0; ix < svc->params.inc_cir_mag; ix++)
	    tmp_mult *= 10;
    	svc->params.inc_cir = (tmp_mult) * (svc->params.inc_cir_mult);
    } else {
	svc->params.inc_cir = svc->params.inc_cir_mult;
    }

    if (L3IE_DBG) {
	svc_ie_print("Throughput", (char *)ie_ptr, length, IN);
    }

return(IS_OKAY);
}
/* 
 * check_min_thruput_ie
 * Throughput values are included with the LLC IE. 
 * Present only in the SETUP message as per Q.933. 
 * Lotsa mumbo-jumbo! See Q.933, section 4.5.19, for details 
 * of bit fields.
 */
int check_min_thruput_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    min_accept_throughput_ie	*ie_ptr;
    uchar 			length;
    int				iy, tmp_mult;

    ie_ptr = (min_accept_throughput_ie *)*ptr;
    length = 1;			/* Include the IE ID */

    /* 
     * the throughput has a magnitude and a multiplier. Play games to 
     * figure out the values. 
     */
    svc->params.min_out_cir_mag = (ie_ptr->out_mag_mult_a >> 4) & 0x07;
    svc->params.min_out_cir_mult = ((ie_ptr->out_mag_mult_a & 0x0F) << 7) | 
					(ie_ptr->out_mult_b & 0x7F);
    if (svc->params.min_out_cir_mag) {
	tmp_mult = 1;
	for (iy = 0; iy < svc->params.min_out_cir_mag; iy++)
	    tmp_mult *= 10;
    	svc->params.min_out_cir = (tmp_mult) * (svc->params.min_out_cir_mult);
    } else {
	svc->params.min_out_cir = svc->params.min_out_cir_mult;
    }
    length += MIN_THROUGHPUT_IE_LENGTH;

    /* 
     * Point at byte 5c, refer section 4.5.19 of Q.933.
     */
    *ptr = *ptr + MIN_THROUGHPUT_IE_LENGTH +1;

    svc->params.min_inc_cir_mag = svc->params.min_out_cir_mag;
    svc->params.min_inc_cir_mult = svc->params.min_out_cir_mult;

    /* 
     * The extension bit tells us whether the "outgoing" values exist.
     * the throughput has a magnitude and a multiplier. Play games to 
     * figure out the values. 
     */
    if (!(ie_ptr->out_mult_b & EXT_BIT_1_MASK)) {
    	svc->params.min_inc_cir_mag = (ie_ptr->inc_mag_mult_a >> 4) & 0x07;
	svc->params.min_inc_cir_mult = ((ie_ptr->inc_mag_mult_a & 0x0F) << 7) | 
					(ie_ptr->inc_mult_b & 0x7F);
	/* 
	 * since bytes 5c and 5d exist, increment by 2 so that we 
	 * now point to byte 6.
	 */
    	*ptr = *ptr + MIN_THROUGHPUT_IE_LENGTH;
    	length += MIN_THROUGHPUT_IE_LENGTH;
    }
    if (svc->params.min_inc_cir_mag) {
	tmp_mult = 1;
	for (iy = 0; iy < svc->params.min_inc_cir_mag; iy++)
	    tmp_mult *= 10;
    	svc->params.min_inc_cir = (tmp_mult) * (svc->params.min_inc_cir_mult);
    } else {
	svc->params.min_inc_cir = svc->params.min_inc_cir_mult;
    }

    if (L3IE_DBG) {
	svc_ie_print("Minimum Throughput", (char *)ie_ptr, length, IN);
    }

return(IS_OKAY);
}
/* 
 * check_cbs_ie
 * committed burst size values are included with the LLC IE. 
 * Lotsa mumbo-jumbo! See Q.933 for details of bit fields.
 */
int check_cbs_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    committed_burst_size_ie	*ie_ptr;
    uchar			length;

    ie_ptr = (committed_burst_size_ie *)*ptr;
    length = 1;				/* Count the IE ID */

    svc->params.out_cbs = (((ie_ptr->out_cbs_a & 0x7F) << 7) | 
				(ie_ptr->out_cbs_b & 0x7F));
    svc->params.inc_cbs = svc->params.out_cbs;
    length += CBSZ_IE_LENGTH;

    /* 
     * Point at byte 6c, refer section 4.5.19 of Q.933.
     */
    *ptr = *ptr + CBSZ_IE_LENGTH + 1;

    if (!(ie_ptr->out_cbs_b & EXT_BIT_1_MASK)) {
    	svc->params.inc_cbs = (((ie_ptr->inc_cbs_a & 0x7F) << 7) | 
				(ie_ptr->inc_cbs_b & 0x7F));
	/* 
	 * since bytes 6c and 6d exist, increment by 2 so that we 
	 * now point to byte 7.
	 */
    	*ptr = *ptr + CBSZ_IE_LENGTH;
    	length += CBSZ_IE_LENGTH;

#if 0
	/* 
	 * As per FRFTC95.029, a magnitude field may exist.
	 * Leave this inactive for now. No switch vendor implements
	 * this. yet! shankar 10/95.
	 */
	if (!(ie_ptr->inc_cbs_b & EXT_BIT_1_MASK)) {
	    svc->params.inc_cbs_mag = (((ie_ptr->cbs_mag) >> 3) & 0x07);
	    svc->params.out_cbs_mag = (ie_ptr->cbs_mag) & 0x07;
	    *ptr = *ptr + 1;
	    length += 1;
	}
#endif
    }

    if (L3IE_DBG) {
	svc_ie_print("CBS", (char *)ie_ptr, length, IN);
    }

return(IS_OKAY);
}

/* 
 * check_ebs_ie
 * excess burst size values are included with the LLC IE. 
 * Lotsa mumbo-jumbo! See Q.933 for details of bit fields.
 */
int check_ebs_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    excess_burst_size_ie	*ie_ptr;
    uchar			length;

    ie_ptr = (excess_burst_size_ie *)*ptr;
    length = 1;				/* Count the IE ID */

    svc->params.out_ebs = (((ie_ptr->out_ebs_a & 0x7F) << 7) 
				| (ie_ptr->out_ebs_b & 0x7F));
    svc->params.inc_ebs = svc->params.out_ebs;
    length += EBSZ_IE_LENGTH;

    /* 
     * Point at byte 7c, refer section 4.5.19 of Q.933.
     */
    *ptr = *ptr + EBSZ_IE_LENGTH + 1;

    if (!(ie_ptr->out_ebs_b & EXT_BIT_1_MASK)) {
    	svc->params.inc_ebs = (((ie_ptr->inc_ebs_a & 0x7F) << 7) | 
				(ie_ptr->inc_ebs_b & 0x7F));
	/* 
	 * since bytes 7c and 7d exist, increment by 2 so that we 
	 * now point to the next IE.
	 */
    	*ptr = *ptr + EBSZ_IE_LENGTH;
    	length += EBSZ_IE_LENGTH;

#if 0
	/* 
	 * As per FRFTC95.029, a magnitude field may exist.
	 * Leave this inactive for now. No switch vendor implements
	 * this. yet! shankar 10/95.
	 */
	if (!(ie_ptr->inc_ebs_b & EXT_BIT_1_MASK)) {
	    svc->params.inc_ebs_mag = (((ie_ptr->ebs_mag) >> 3) & 0x07);
	    svc->params.out_ebs_mag = (ie_ptr->ebs_mag) & 0x07;
	    *ptr = *ptr + 1;
	    length += 1;
	}
#endif
    }

    if (L3IE_DBG) {
	svc_ie_print("EBS", (char *)ie_ptr, length, IN);
    }

return(IS_OKAY);
}
/* 
 * check_cbs_mag_ie
 * excess the magnitude of the Committed burst size. 
 * See FRFTC95.090 for details. Also known as Attachment A
 * of FRF.4
 */
int check_cbs_mag_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    cbs_mag_ie			*ie_ptr;
    uchar			length;

    ie_ptr = (cbs_mag_ie *)*ptr;
    length = 1;				/* Count the IE ID */
    *ptr = *ptr + 1;

    /* 
     * As per FRFTC95.029, a magnitude field may exist.
     */
    svc->params.inc_cbs_mag = (((ie_ptr->cbs_mag) >> 3) & 0x07);
    svc->params.out_cbs_mag = (ie_ptr->cbs_mag) & 0x07;
    *ptr = *ptr + 1;
    length += 1;

    if (L3IE_DBG)
	svc_ie_print("CBS MAG", (char *)ie_ptr, length, IN);

return(IS_OKAY);
}
/* 
 * check_ebs_mag_ie
 * excess the magnitude of the Excess burst size. 
 * See FRFTC95.090 for details. 
 */
int check_ebs_mag_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    ebs_mag_ie			*ie_ptr;
    uchar			length;

    ie_ptr = (ebs_mag_ie *)*ptr;
    length = 1;				/* Count the IE ID */
    *ptr = *ptr + 1;

    /* 
     * As per FRFTC95.029, a magnitude field may exist.
     */
    svc->params.inc_ebs_mag = (((ie_ptr->ebs_mag) >> 3) & 0x07);
    svc->params.out_ebs_mag = (ie_ptr->ebs_mag) & 0x07;
    *ptr = *ptr + 1;
    length += 1;

    if (L3IE_DBG)
	svc_ie_print("EBS MAG", (char *)ie_ptr, length, IN);

return(IS_OKAY);
}
/* 
 * check_ll_comp_ie
 * The low layer compatibility IE is to provide a means which should be 
 * used for compatibility checking by an addressed entity. (e.g. a remote
 * user or an interworking unit or a high layer function network node 
 * addressed by the calling user. The maximum length is 8 octets. 
 * The IE identifier is 0x7C. Much of this IE is hardcoded.
 */
int check_ll_comp_ie(
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref, 
	uchar msg_type)
{
    low_layer_compatibility_ie 	*ie_ptr;

    ie_ptr = (low_layer_compatibility_ie *)*ptr;

    if (!(ie_ptr->ie_id == LOW_LAYER_COMPATIBILITY_IE_ID ))
	return(INVALID_IE_TYPE);

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < LOW_LAYER_COMPATIBILITY_IE_ID)
    	svc->last_ie_seen = LOW_LAYER_COMPATIBILITY_IE_ID;
    else 
	return(INVALID_IE_TYPE);

    if (!(ie_ptr->info_cdg_std & EXT_BIT_1_MASK))
	return(ILLEGAL_EXTENSION_BIT);

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + LOW_LAYER_COMP_IE_HDR_LENGTH;

    svc->ll_comp_ie_present = TRUE;
    if (L3IE_DBG) {
	svc_ie_print("Low Layer Comp", (char *)ie_ptr, 
		ie_ptr->length + LOW_LAYER_COMP_IE_HDR_LENGTH, IN);
    }
return(IS_OKAY);
}
/* 
 * check_cause_ie 
 * The cause IE describes the reason for generating certain messages, to 
 * provide diagnostic information in the event of procedural errors and to 
 * indicate the location of the cause originator.
 *
 * The cause IE can be repeated but that aspect has not been built into 
 * the code below.
 */
static int check_cause_ie(
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    cause_ie			*ie_ptr;

    ie_ptr = (cause_ie *)*ptr;

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < CAUSE_IE_ID)
    	svc->last_ie_seen = CAUSE_IE_ID;
    else 
	return(INVALID_IE_TYPE);

    svc->params.cause = (ie_ptr->cause_value) & 0x7F;

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + CAUSE_IE_HDR_LENGTH + ie_ptr->length;

    svc->cause_ie_present = TRUE;
    if (L3IE_DBG) {
	svc_ie_print("Cause", (char *)ie_ptr, 
		ie_ptr->length + CAUSE_IE_HDR_LENGTH, IN);
	buginf("\n\t\t\tCause No.%d : %s",svc->params.cause, 
	       cause_def(svc->params.cause));
    }

    return(IS_OKAY);
}

/* 
 * check user_user_ie
 * Used for conveying information between frame-relay users. This 
 * information is not interpreted by the network but is carried and
 * delivered to the remote users. 
 */
static int check_user_user_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    user_user_ie	*ie_ptr;
      
    ie_ptr = (user_user_ie *)*ptr;

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < USER_USER_IE_ID)
    	svc->last_ie_seen = USER_USER_IE_ID;
    else 
	return(INVALID_IE_TYPE);

    if (L3IE_DBG) {
	svc_ie_print("User-User", (char *)ie_ptr, ie_ptr->length + 
				CAUSE_IE_HDR_LENGTH, IN);
    }

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + USER_USER_IE_HDR_LENGTH;

    svc->user_user_ie_present = TRUE;
    return(IS_OKAY);
}
/* 
 * check_call_state_ie 
 * Describes the current state of a call.
 * Only the STATUS message has a call state IE. 
 */
static int check_call_state_ie(
	fr_svc_info_type *svc,
	uchar **ptr, 
	uint call_ref,
	uchar msg_type)
{
    call_state_ie		*ie_ptr;


    ie_ptr = (call_state_ie *)*ptr;

    /* 
     * Administrative checking to make sure that IEs are in order. 
     * The last IE seen must have a smaller ID than the one being 
     * processed. 
     */
    if (svc->last_ie_seen < CALL_STATE_IE_ID)
    	svc->last_ie_seen = CALL_STATE_IE_ID;
    else 
	return(INVALID_IE_TYPE);

    svc->call_state_inc_msg = ie_ptr->call_state;

    if (L3IE_DBG) {
	buginf("\nL3CC IN: Incoming Call state is %d", ie_ptr->call_state);
	svc_ie_print("Call State", (char *)ie_ptr, ie_ptr->length + 
				CAUSE_IE_HDR_LENGTH, IN);
    }

    /* 
     * Point to the next IE in the packet. 
     */
    *ptr = *ptr + ie_ptr->length + CALL_STATE_IE_HDR_LENGTH;

    svc->call_state_ie_present = TRUE;
    return(IS_OKAY);
}
/* 
 * process_incoming_setup_msg
 * This is a SETUP message from the switch/calling side
 * Only optional IEs are being dealt with here. 
 */
int process_incoming_setup_msg (
	paktype *pak,
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref)
{
    int 		ret;
    uchar 		*ie_ptr;
    int 		call_state;
    uchar		*paksize, check;
	
    ret = 0;
    ie_ptr  = *ptr;
    check = 0;

    paksize = pak->datagramstart + pak->datagramsize;
    call_state = svc->call_state;
    svc->ll_core_ie_present = FALSE;

    while (ie_ptr < paksize) {
	ret = check_if_ie_is_legal(svc->last_msg_seen, ie_ptr);
	if (!ret) {
	    if (++check > MAX_ILLEGAL_IE_PER_MSG)
		return(INVALID_IE_TYPE);
	    ie_ptr = fr_svc_bypass_illegal_ie(&ie_ptr);
	    if (ie_ptr)
	    	continue;
	    else
		return(INVALID_IE_TYPE);
	}
	/* 
  	 * This is a known IE, go process it. 
  	 */
    	ret = (reg_invoke_process_setup_msg(*ie_ptr, svc, &ie_ptr, 
						call_ref, MSG_SETUP));
    	if (ret) 
	    return(ret);
    }
    /* 
     * Sanity Check for mandatory IEs
     */
    if (!svc->bearer_cap_ie_present) {
	if (L3IE_DBG)
	    buginf("\nL3IE IN : NO Bearer Capability in SETUP message, ERROR");
	return(MANDATORY_IE_MISSING);
    }

    /* 
     * Initialize the SVC IE management fields. 
     */
    fr_svc_init_ie_mgmt_area(svc);

return(IS_OKAY);
}
/* 
 * process_incoming_call_proc_msg
 * This is a CALL PROCEEDING message from the switch/calling side
 * Only optional IEs are being dealt with here. 
 */
int process_incoming_call_proc_msg (
	paktype *pak,
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref)
{
    int 		ret;
    uchar 		*ie_ptr;
    uchar		*paksize, check;
	
    ret = 0;
    ie_ptr  = *ptr;
    check = 0;

    paksize = pak->datagramstart + pak->datagramsize;
    while (ie_ptr < paksize) {
	ret = check_if_ie_is_legal(svc->last_msg_seen, ie_ptr);
	if (!ret) {
	    if (++check > MAX_ILLEGAL_IE_PER_MSG)
		return(INVALID_IE_TYPE);
	    ie_ptr = fr_svc_bypass_illegal_ie(&ie_ptr);
	    if (ie_ptr)
	    	continue;
	    else
		return(INVALID_IE_TYPE);
	}
	/* 
  	 * This is a known IE, go process it. 
  	 */
    	ret = (reg_invoke_process_call_proc_msg(*ie_ptr, svc, &ie_ptr, 
					call_ref, MSG_CALL_PROCEEDING));
	if (ret)
	    return(ret);
    }
    /* 
     * Sanity Check for mandatory IEs
     * A special case is the first response to a SETUP message. 
     */
    if (!svc->dlci_ie_present && (!svc->setup_resp_count)) {
	svc->setup_resp_count++;
	if (L3IE_DBG)
	    buginf("\nL3IE IN : CALL PROCEEDING without DLCI, response to SETUP");
	return(MANDATORY_IE_MISSING);
    }

    /* 
     * Flag the user that a DLCI is missing if that is the case. 
     * Does not matter what the response number is, if dlci is not 
     * defined, we have a problem.
     */
    if (!svc->dlci_ie_present) {
	if (L3IE_DBG)
	    buginf("\nL3IE IN : CALL PROCEEDING message, response #%d, No DLCI yet?", 
						svc->setup_resp_count);
    }

    /* 
     * Initialize the SVC IE management fields. 
     */
    fr_svc_init_ie_mgmt_area(svc);

return(IS_OKAY);
}
/* 
 * process_incoming_connect_msg
 * Routine to handle MSG_CONNECT 
 * Only optional IEs are being dealt with here. 
 */
int process_incoming_connect_msg (
	paktype *pak,
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref)
{
    int 		ret;
    uchar 		*ie_ptr;
    uchar		*paksize, check;
	
    ret = 0;
    ie_ptr  = *ptr;
    check = 0;

    /* 
     * Initialize SVC IE management fields. 
     */
    svc->ll_core_ie_present = FALSE;

    paksize = pak->datagramstart + pak->datagramsize;
    while (ie_ptr < paksize) {
	ret = check_if_ie_is_legal(svc->last_msg_seen, ie_ptr);
	if (!ret) {
	    if (++check > MAX_ILLEGAL_IE_PER_MSG)
		return(INVALID_IE_TYPE);
	    ie_ptr = fr_svc_bypass_illegal_ie(&ie_ptr);
	    if (ie_ptr)
	    	continue;
	    else
		return(INVALID_IE_TYPE);
	}
	/* 
  	 * This is a known IE, go process it. 
  	 */
    	ret = (reg_invoke_process_connect_msg(*ie_ptr, svc, &ie_ptr, 
					call_ref, MSG_CONNECT));
	if (ret) return(ret);						
    }

    /* 
     * Sanity Check for mandatory IEs
     * A special case is the first response to a SETUP message. 
     */
    if (!svc->dlci_ie_present && (!svc->setup_resp_count)) {
	svc->setup_resp_count++;

	if (L3IE_DBG)
	    buginf("\nL3IE IN : CONNECT without DLCI, response to SETUP");

	return(MANDATORY_IE_MISSING);
    }

    /* 
     * Initialize the SVC IE management fields. 
     */
    fr_svc_init_ie_mgmt_area(svc);

return(IS_OKAY);
}
/* 
 * process_incoming_disconnect_msg
 * Process a DISCONNECT message 
 * Only optional IEs are being dealt with here. 
 */
int process_incoming_disconnect_msg (
	paktype *pak,
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref)
{
    int 		ret;
    uchar 		*ie_ptr;
    uchar		*paksize, check;
	
    ret = 0;
    ie_ptr  = *ptr;
    check = 0;

    paksize = pak->datagramstart + pak->datagramsize;
    while (ie_ptr < paksize) {
	ret = check_if_ie_is_legal(svc->last_msg_seen, ie_ptr);
	if (!ret) {
	    if (++check > MAX_ILLEGAL_IE_PER_MSG)
		return(INVALID_IE_TYPE);
	    ie_ptr = fr_svc_bypass_illegal_ie(&ie_ptr);
	    if (ie_ptr)
	    	continue;
	    else
		return(INVALID_IE_TYPE);
	}
	/* 
  	 * This is a known IE, go process it. 
  	 */
    	ret = (reg_invoke_process_disconnect_msg(*ie_ptr, svc, &ie_ptr, 
					call_ref, MSG_DISCONNECT));
	if (ret)
	    return(ret);
    }

    if (!svc->cause_ie_present) {
	/* 
	 * Assume that cause is NORMAL_UNSPECIFIED
	 */
	svc->params.cause = NORMAL_UNSPECIFIED;
	if (L3IE_DBG)
	    buginf("\nL3IE IN : DISCONNECT without a CAUSE IE");
    } 

    /* 
     * Initialize the SVC IE management fields. 
     */
    fr_svc_init_ie_mgmt_area(svc);

return(IS_OKAY);
}

/* 
 * process_incoming_release_msg
 * Process an incoming RELEASE message 
 * Only optional IEs are being dealt with here. 
 */
int process_incoming_release_msg (
	paktype *pak,
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref)
{
    int 		ret;
    uchar 		*ie_ptr;
    uchar		*paksize, check;
	
    ret = 0;
    ie_ptr  = *ptr;
    check = 0;

    paksize = pak->datagramstart + pak->datagramsize;
    while (ie_ptr < paksize) {
	ret = check_if_ie_is_legal(svc->last_msg_seen, ie_ptr);
	if (!ret) {
	    if (++check > MAX_ILLEGAL_IE_PER_MSG)
		return(INVALID_IE_TYPE);
	    ie_ptr = fr_svc_bypass_illegal_ie(&ie_ptr);
	    if (ie_ptr)
	    	continue;
	    else
		return(INVALID_IE_TYPE);
	}
	/* 
  	 * This is a known IE, go process it. 
  	 */
    	ret = (reg_invoke_process_release_msg(*ie_ptr, svc, &ie_ptr, 
					call_ref, MSG_RELEASE));
	if (ret)
	    return(ret);
    }

    if (!svc->cause_ie_present && !svc->call_clear_msg_count) {
	/* 
	 * Assume that cause is NORMAL_UNSPECIFIED
	 */
	svc->params.cause = NORMAL_UNSPECIFIED;
	if (L3IE_DBG)
	    buginf("\nL3IE IN : RELEASE without a CAUSE IE");
    } 

    /* 
     * keep track of incoming call clearing messages
     * The first one needs special attention.
     */
    svc->call_clear_msg_count++;

    /* 
     * Initialize the SVC IE management fields. 
     */
    fr_svc_init_ie_mgmt_area(svc);

return(IS_OKAY);
}
/* 
 * process_incoming_release_comp_msg
 * Process an incoming RELEASE COMPLETE message
 * Only optional IEs are being dealt with here. 
 */
int process_incoming_release_comp_msg (
	paktype *pak,
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref)
{
    int 		ret;
    uchar 		*ie_ptr;
    uchar		*paksize, check;
	
    ret = 0;
    ie_ptr  = *ptr;
    check = 0;

    paksize = pak->datagramstart + pak->datagramsize;
    while (ie_ptr < paksize) {
	ret = check_if_ie_is_legal(svc->last_msg_seen, ie_ptr);
	if (!ret) {
	    if (++check > MAX_ILLEGAL_IE_PER_MSG)
		return(INVALID_IE_TYPE);
	    ie_ptr = fr_svc_bypass_illegal_ie(&ie_ptr);
	    if (ie_ptr)
	    	continue;
	    else
		return(INVALID_IE_TYPE);
	}
	/* 
  	 * This is a known IE, go process it. 
  	 */
    	ret = (reg_invoke_process_release_comp_msg(*ie_ptr, svc, &ie_ptr, 
					call_ref, MSG_RELEASE_COMPLETE));
	if (ret)
	    return(ret);
    }

    if (!svc->cause_ie_present && !svc->call_clear_msg_count) {
	/* 
	 * Assume that cause is NORMAL_UNSPECIFIED
	 */
	svc->params.cause = NORMAL_UNSPECIFIED;
	if (L3IE_DBG)
	    buginf("\nL3IE IN : RELEASE COMPLETE without a CAUSE IE");
    } 

    /* 
     * keep track of incoming call clearing messages
     * The first one needs special attention.
     */
    svc->call_clear_msg_count++;

    /* 
     * Initialize the SVC IE management fields. 
     */
    fr_svc_init_ie_mgmt_area(svc);

return(IS_OKAY);
}
/* 
 * process_incoming_status_msg
 * Process an incoming STATUS message
 * This message is ssent by the user or the network in response to a 
 * STATUS ENQUIRY mesage or at any time during a call to report certain 
 * error conditions.  
 */
int process_incoming_status_msg (
	paktype *pak,
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref)
{
    int 		ret;
    uchar 		*ie_ptr;
    uchar		*paksize, check;
	
    ret = 0;
    ie_ptr  = *ptr;
    check = 0;

    paksize = pak->datagramstart + pak->datagramsize;
    while (ie_ptr < paksize) {
	ret = check_if_ie_is_legal(svc->last_msg_seen, ie_ptr);
	if (!ret) {
	    if (++check > MAX_ILLEGAL_IE_PER_MSG)
		return(INVALID_IE_TYPE);
	    ie_ptr = fr_svc_bypass_illegal_ie(&ie_ptr);
	    if (ie_ptr)
	    	continue;
	    else
		return(INVALID_IE_TYPE);
	}
	/* 
  	 * This is a known IE, go process it. 
  	 */
    	ret = (reg_invoke_process_status_msg(*ie_ptr, svc, &ie_ptr, 
					call_ref, MSG_STATUS));
	if (ret)
 	    return(ret);
    }

    if (!svc->cause_ie_present) {
	if (L3IE_DBG)
	    buginf("\nL3IE IN : STATUS without a CAUSE IE");
	return(MANDATORY_IE_MISSING);
    } 

    if (!svc->call_state_ie_present) {
	if (L3IE_DBG)
	    buginf("\nL3IE IN : STATUS without a CALL STATE IE");
	return(MANDATORY_IE_MISSING);
    } 

    /* 
     * Initialize the SVC IE management fields. 
     */
    fr_svc_init_ie_mgmt_area(svc);

return(IS_OKAY);
}
/* 
 * process_incoming_status_enquiry_msg
 * Process an incoming STATUS ENQUIRY message
 * Only optional IEs are being dealt with here. 
 */
int process_incoming_status_enquiry_msg (
	paktype *pak,
	fr_svc_info_type *svc,
	uchar **ptr,
	uint call_ref)
{
    int 		ret;
    uchar 		*ie_ptr;
    uchar		*paksize, check;
	
    ret = 0;
    ie_ptr  = *ptr;
    check = 0;
    paksize = pak->datagramstart + pak->datagramsize;

    /* 
     * Initialize all SVC IE management fields. 
     */

    while (ie_ptr <=  paksize) {
	ret = check_if_ie_is_legal(svc->last_msg_seen, ie_ptr);
	if (!ret) {
	    if (++check > MAX_ILLEGAL_IE_PER_MSG)
		return(INVALID_IE_TYPE);
	    ie_ptr = fr_svc_bypass_illegal_ie(&ie_ptr);
	    if (ie_ptr)
	    	continue;
	    else
		return(INVALID_IE_TYPE);
	}
	/* 
  	 * This is a known IE, go process it. 
  	 */
    	ret = (reg_invoke_process_status_enq_msg(*ie_ptr, svc, &ie_ptr, 
					call_ref, MSG_STATUS_ENQUIRY));
	if (ret)
	    return(ret);
    }

    if (L3IE_DBG)
	buginf("\nL3IE IN : STATUS ENQUIRY message");

    /* 
     * Initialize the SVC IE management fields. 
     */
    fr_svc_init_ie_mgmt_area(svc);

return(IS_OKAY);
}
/* 
 * fr_svc_bypass_illegal_ie
 * It could be that the IE that we are attempting to process does not 
 * meet known criteria. In such a case do a best effort at processing 
 * this message. We are generally screwed anyhow. We'll try to keep 
 * from crashing!
 */
uchar *fr_svc_bypass_illegal_ie(uchar **ptr)
{
    illegal_ie          *ie_ptr;
    uchar 		ie_type;
    uchar 		ie_length;

    ie_ptr = (illegal_ie *)*ptr;

    /* 
     * Look at the ID of the illegal IE. 
     */
    ie_type = ie_ptr->illegal_ie_id;

    /* 
     * The ID of the largest IE should be 0x7E (user-user IE).
     */
     if (ie_type > USER_USER_IE_ID) {
 	buginf("\nfr_svc_bypass_illegal_ie: UNKNOWN IE 0x%x", ie_type);
	return(NULL);
    }

    /* 
     * Look at the length of the illegal IE or if length field is too 
     * big (32 or more bytes long), return. I believe the LLCore IE can 
     * a max of 31 bytes. That is the longest known IE. 
     */
    ie_length = ie_ptr->length;
    if (!(ie_length) || (ie_length> 0x20)) {
	buginf("\nfr_svc_bypass_illegal_ie: Illegal IE length, 0x%x, %d", 
			ie_type, ie_length);
	return(NULL);
    }

    /* 
     * bypass the illegal IE, add 2 bytes for the ID and length fields
     */
    *ptr = *ptr + ie_ptr->length + 2;
    if (L3IE_DBG)
	svc_ie_print("Illegal IE", (char *)ie_ptr, ie_ptr->length + 2, IN);

return(*ptr);
}
/* 
 * svc_incoming_msg_init
 * Various initializations functions related to IE processing. 
 * Any new IE must be tacked on to the relevant message type. 
 * make sure that check_if_ie_is_legal() knows about the new IE.
 */
void svc_incoming_msg_init(void)
{
    /* 
     * Functions required for processing SETUP message 
     */
    reg_add_process_setup_msg(BEARER_CAPABILITY_IE_ID, 
			check_bearer_cap_ie, "check_bearer_cap_ie");
    reg_add_process_setup_msg(DLCI_IE_ID, check_dlci_ie, "check_dlci_ie");
    reg_add_process_setup_msg(LINK_LAYER_CORE_PARAM_IE_ID, 
			check_llc_param_ie, "check_llc_param_ie");
    reg_add_process_setup_msg(CALLING_PARTY_NUMBER_ID, 
		check_cg_party_number_ie, "check_cg_party_number_ie");
    reg_add_process_setup_msg(CALLING_PARTY_SUBADDR_ID, 
		check_cg_party_subaddr_ie, "check_cg_party_subaddr_ie");
    reg_add_process_setup_msg(CALLED_PARTY_NUMBER_ID, 
		check_cd_party_number_ie, "check_cd_party_number_ie");
    reg_add_process_setup_msg(CALLED_PARTY_SUBADDR_ID, 
		check_cd_party_subaddr_ie, "check_cd_party_subaddr_ie");
    reg_add_process_setup_msg(TRANSIT_NETWORK_IE_ID, 
		check_xit_net_sel_ie, "check_transit_net_selection_ie");
    reg_add_process_setup_msg(LOW_LAYER_COMPATIBILITY_IE_ID, 
		check_ll_comp_ie, "check_ll_comp_ie");
    reg_add_process_setup_msg(USER_USER_IE_ID, 
		check_user_user_ie, "check_user_user_ie");

    /* 
     * Functions required for processing a CALL PROCEEDING message 
     */
    reg_add_process_call_proc_msg(DLCI_IE_ID, check_dlci_ie, "check_dlci_ie");

    /* 
     * Functions required for processing a CONNECT message 
     */
    reg_add_process_connect_msg(DLCI_IE_ID, check_dlci_ie, "check_dlci_ie");
    reg_add_process_connect_msg(LINK_LAYER_CORE_PARAM_IE_ID,
		check_connect_llc_param_ie, "check_connect_llc_param_ie");
    reg_add_process_connect_msg(CONN_PARTY_NUMBER_ID,
		check_con_party_number_ie, "check_con_party_number_ie");
    reg_add_process_connect_msg(CONN_PARTY_NUMBER_SUBADDR_ID,
		check_con_party_subaddr_ie, "check_con_party_subaddr_ie");
    reg_add_process_connect_msg(USER_USER_IE_ID, 
    		check_user_user_ie, "check_user_user_ie");

    /* 
     * Functions required for processing a DISCONNECT message 
     */
    reg_add_process_disconnect_msg(CAUSE_IE_ID, 
		check_cause_ie, "check_cause_ie");

    /* 
     * Functions required for processing a RELEASE message 
     */
    reg_add_process_release_msg(CAUSE_IE_ID, 
		check_cause_ie, "check_cause_ie");

    /* 
     * Functions required for processing a RELEASE COMPLETE message 
     */
    reg_add_process_release_comp_msg(CAUSE_IE_ID, 
		check_cause_ie, "check_cause_ie");

    /* 
     * Functions required for processing a STATUS message 
     */
    reg_add_process_status_msg(CAUSE_IE_ID, check_cause_ie, "check_cause_ie");
    reg_add_process_status_msg(CALL_STATE_IE_ID, check_call_state_ie, 
				"check_call_state_ie");

    /* Functions required for processing a STATUS ENQUIRY message */
	/* 
	 * Nothing needed here
	 */
}

/* 
 * check_if_ie_is_legal
 * A simple function to decide whether an IE is a legal one or not.
 */
boolean check_if_ie_is_legal (uchar msg, uchar *ptr)
{
    switch (msg) {
	case MSG_SETUP:
    	    switch (*ptr) {
	    	case BEARER_CAPABILITY_IE_ID:
	    	case DLCI_IE_ID:
	    	case LINK_LAYER_CORE_PARAM_IE_ID:
	    	case CALLING_PARTY_NUMBER_ID:
	    	case CALLING_PARTY_SUBADDR_ID:
	    	case CALLED_PARTY_NUMBER_ID:
	    	case CALLED_PARTY_SUBADDR_ID:
	    	case LOW_LAYER_COMPATIBILITY_IE_ID:
	    	case USER_USER_IE_ID:
		    return(TRUE);
	    	default:
		    if (L3IE_DBG)
		    	buginf("\nL3CC IN: SETUP MSG :Illegal IE ID of 0x%x", *ptr);
		    return(FALSE);
    	    }
	case MSG_CALL_PROCEEDING:
	    switch (*ptr) {
	    	case DLCI_IE_ID:
		    return(TRUE);
	    	default:
		    if (L3IE_DBG)
		    	buginf("\nL3CC IN: CALL PROC :Illegal IE ID of 0x%x", *ptr);
		    return(FALSE);
	    }
	case MSG_CONNECT:
	    switch (*ptr) {
	    	case DLCI_IE_ID:
	    	case LINK_LAYER_CORE_PARAM_IE_ID:
	    	case CONN_PARTY_NUMBER_ID:
	    	case CONN_PARTY_NUMBER_SUBADDR_ID:
	    	case USER_USER_IE_ID:
		    return(TRUE);
	    	default:
		    if (L3IE_DBG)
		    	buginf("\nL3CC IN: CONNECT :Illegal IE ID of 0x%x", *ptr);
		    return(FALSE);
	    }
	case MSG_DISCONNECT:
	case MSG_RELEASE:
	case MSG_RELEASE_COMPLETE:
	    switch (*ptr) {
	    	case CAUSE_IE_ID:
		    return(TRUE);
	    	default:
		    if (L3IE_DBG)
		    	buginf("\nL3CC IN: DISC/REL/REL C :Illegal IE ID of 0x%x", *ptr);
		    return(FALSE);
	    }
	case MSG_STATUS_ENQUIRY:
	    return(TRUE);
	case MSG_STATUS:
	    switch (*ptr) {
	    	case CAUSE_IE_ID:
	    	case CALL_STATE_IE_ID:
		    return(TRUE);
		default:
		    if (L3IE_DBG)
		    	buginf("\nL3CC IN: STATUS :Illegal IE ID of 0x%x", *ptr);
		    return(FALSE);
	    }
	default:
	    if (L3IE_DBG)
		buginf("\nL3CC IN: UNKNOWN MSG :Illegal IE ID of 0x%x", *ptr);
	    return(FALSE);
    }
}
