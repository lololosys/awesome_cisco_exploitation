/* $Id: fr_svc_ie_setup.c,v 3.1.64.3 1996/07/03 01:57:51 chifei Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_svc_ie_setup.c,v $
 *------------------------------------------------------------------
 * fr_svc_ie_setup.c : Construction of outgoing I frames
 *
 * May 1995, Shankar Natarajan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_svc_ie_setup.c,v $
 * Revision 3.1.64.3  1996/07/03  01:57:51  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.1.64.2  1996/05/24  21:21:32  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/25  23:23:40  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/24  04:11:29  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/11/09  13:49:09  shaker
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
#include "../h/globs.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "frame_relay.h"
#include "fr_debug.h"
#include "../if/network.h"
#include "../ui/parse_util.h" 
#include "../parser/parser_defs_exec.h"
#include "../wan/parser_defs_frame.h"
#include "../if/static_map.h"
#include "fr_svc.h"
#include "fr_l2.h"
#include "../wan/fr_svc_registry.h"

/* 
 * add_prot_disc_ie
 * Tack on a 1 byte protocol discriminator header. 
 * 
 * A Protocol discriminator is used to distinguish messages for 
 * user-network call control from other messages that have not 
 * been defined yet. Pg 53, section 4.2, Q.931. A value of 0x08 
 * indicates a Q931 message type.
 */
void add_prot_disc_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    uchar *ie_ptr;

    ie_ptr = (uchar *)*ptr;
    *ie_ptr = PROT_DISC_IE_ID;	/* ANSI protocol discriminator */

    if (L3IE_DBG)
	svc_ie_print("Prot Disc", ie_ptr, PROT_DISC_IE_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. ie. call reference. This is a 1 byte IE.
     */
    *ptr = *ptr + 1; 	
}

/* 
 * add_call_ref_ie
 * Section 3.1.2 of the FR SVC Implementation Agreement specifies that this 
 * field should be 3 bytes long. ref: 3.1.2, 3.1.3, FRF.4
 *
 * Tack on a 3 byte call reference. We are assuming that the call reference
 * has already been assigned by the time we get to this point. Call References
 * in the range 0 to 0x7FFF are generated within this box. Call references 
 * greater than 0x8000 belong to calls generated at the other end.
 *
 * The call reference is used to identify the call at the local user-network
 * interface. It does not have end-end significance. 
 */
static void add_call_ref_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    call_reference 	*ie_ptr;
    uint		call_ref;

    call_ref = params->call_ref;

    ie_ptr = (call_reference *)*ptr;
    ie_ptr->length = CALL_REF_IE_LENGTH;	/* length is 2 */

    /* 
     * We need to split the call reference into two parts. Since the Call 
     * reference can take on values from 0 to 0x7FFF, the upper byte is masked 
     * with 0x7F and the lower byte is masked with 0xFF. 
     *
     * Leave the highest bit the way it came in. First separate the upper 8 bits.
     * Move them into the "byte2" field. The "& 0xFF" does routine cleanup of the 
     * upper byte.
     */
    ie_ptr->byte2 = (char)(((call_ref & 0xFF00) >> 8) & 0xFF);

    /* 
     * Lower 8 bits of the call reference. "byte3"! what an appropriate name! 
     */
    ie_ptr->byte3 = (char)(call_ref & 0x00FF);

    if (L3IE_DBG)
	svc_ie_print("Call Ref", (char *)ie_ptr, CALL_REF_TOTAL_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. CALL_REF_TOTAL_LENGTH is 3.
     */
    *ptr =  *ptr + CALL_REF_TOTAL_LENGTH;
}

/* 
 * add_bearer_cap_ie
 * Section 4.5.5 in Q.933. 
 * Bearer capability is used to request a certain type of service from the 
 * network. It contains information that MAY be used by the network. If this 
 * IE is missing, a default may not be assumed.
 * Tack on a 4 byte bearer capability. Q.922 is implied. 
 */
static void add_bearer_cap_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    bearer_cap_ie 	*ie_ptr;

    ie_ptr = (bearer_cap_ie *)*ptr;
    ie_ptr->ie_id = BEARER_CAPABILITY_IE_ID;	/* ID is 4 */
    ie_ptr->length = BEARER_CAP_IE_LENGTH;	/* length is 3 */

    /* 
     * BEARER_CAP_BYTE3 is hardcoded as 0x88.
     * 0x1cciiiii, where 1 is the extension bit, cc is the coding standard. 
     * 00 for cc means CCITT standard coding, 01000 for iiiii means 
     * unrestricted digital information.
     */
    ie_ptr->byte3 = BEARER_CAP_BYTE3;

    /* 
     * BEARER_CAP_BYTE4 is hardcoded as 0xA0. It signifies frame mode for the 
     * transfer. 
     */
    ie_ptr->byte4 = BEARER_CAP_BYTE4;

    /* 
     * BEARER_CAP_BYTE5_B is hardcoded as 0xCF. The layer 2 protoocol
     * follows core aspects of Q.922 Annex A. 
     */
    ie_ptr->byte5 = BEARER_CAP_BYTE5_B;

    if (L3IE_DBG)
	svc_ie_print("Bearer Capability", (char *)ie_ptr, 
				BEARER_CAP_IE_TOTAL_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. BEARER_CAP_IE_TOTAL_LENGTH is hardcoded as 5. I 
     * do no expect it to change. BEARER_CAP_IE_TOTAL_LENGTH is 5.
     */
    *ptr =  *ptr + BEARER_CAP_IE_TOTAL_LENGTH;

    if (svc->fr_svc_b2b) {
	if (L3IE_DBG)
	    buginf("\nBACK to BACK setup, Adding DLCI IE to message, using a preset DLCI");
	add_dlci_ie(svc, ptr, params);
    }
}

/* 
 * add_msg_type_ie
 * Tack on a 1 byte message type. Legal messages are MSG_SETUP, MSG_CALL_PROCEEDING,
 * MSG_CONNECT, MSG_DISCONNECT, MSG_RELEASE, MSG_RELEASE_COMPLETE, MSG_STATUS and
 * MSG_STATUS_ENQUIRY. 
 */
static void add_msg_type_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    uchar 	*ie_ptr;
    uchar 	msg_type;

    msg_type = params->msg_type;

    ie_ptr = (uchar *)*ptr;
    *ie_ptr = msg_type;

    /* 
     * Message type has a length of 1, hardcoded below as "+ 1"
     * when the IE pointer gets updated. This "IE" has no length 
     * field.
     */
    if (L3IE_DBG)
	svc_ie_print("Message type", ie_ptr, 1, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. Message type is a 1 byte field.
     */
    *ptr = *ptr + 1;
}

/* 
 * add_dlci_ie
 * Tack on a DLCI IE. Maximum length is 4 octets. As per the FR SVC 
 * implementation agreement, we'll follow a 2 byte DLCI format which
 * implies a 10 bit DLCI.  Also, refer Section 4.5.15 in Q.933.
 */
void add_dlci_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    dlci_ie 	*ie_ptr;
    ushort	dlci;
    ushort	dlci_type;

    /* 
     * for calls originating on this router, the dlci field will 
     * be zero. 
     */
    dlci = params->dlci;
    if (svc->fr_svc_b2b) {
    	dlci = 456;
	if (L3IE_DBG)
	    buginf("\nadd_dlci_ie: This is a BACK to BACK SVC, DLCI = %d", dlci);
    }

    /* 
     * The preferred/exclusive bit is set to 0. We'll accept any DLCI
     * that the network assigns to us. The "dlci_type" is currently not being
     * used. I intended it to be used if the user configuration ever allows
     * a specific DLCI to be requested.
     */
    dlci_type = params->req_svc_type;

    ie_ptr = (dlci_ie *)(*ptr);
    ie_ptr->ie_id = DLCI_IE_ID;			/* ID is 0x19 */
    ie_ptr->length = DLCI_IE_LENGTH;		/* length is 2 */

    /* 
     * This field holds the upper 6 bits of the dlci, the 
     * preferred/exclusive bit and the extension bit.
     * Get rid of the 4 lowest bits, they belong in the 
     * next byte. dlci is 10 bits long. dlci>>4 gives us the
     * upper 6 bits.
     */
    ie_ptr->dlci_u = ((dlci >> 4) & 0x003F) | DLCI_EXCLUSIVE;

    /* 
     * Lower 4 bits of the DLCI, 3 spare bits and an extension bit.  
     * don't set anything in the spare bits area. Set the extension bit.
     */
    ie_ptr->dlci_l = ((dlci & 0x000F) << 3) | EXT_BIT_1;

    if (L3IE_DBG)
	svc_ie_print("DLCI", (char *)ie_ptr, DLCI_IE_TOTAL_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. DLCI_IE_TOTAL_LENGTH is 4.
     */
    *ptr =  *ptr + DLCI_IE_TOTAL_LENGTH;
}

/* 
 * add_link_lyr_core_ie
 * add a link layer core parameter IE. You'll need a copy of Q.933
 * before wading into this routine. The byte names are derived from the 
 * picture on Page 32 of Q.933, Section 4.5.19.
 * 
 * Some fields hold the actual value, while others hold the value in 
 * magnitude * (10 ** multiplier) form. Hope you remember your FORTRAN. 
 * "*" means multiply, "**" means exponent.
 * 
 * Warning: If you are subject to nausea, please tread carefully!!!!!!
 */
void add_link_lyr_core_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    link_core_ie *ie_ptr;
    uint 		magnitude;
    uint		multiplier;

    multiplier = 0;
    ie_ptr = (link_core_ie *)(*ptr);
    ie_ptr->ie_id = LINK_LAYER_CORE_PARAM_IE_ID;	/* ID is 0x48 */

    /*  
     * The -4 in the statement relates to FTFTC/96-025 extension to FRF.4
     * The FRF has specified an extension to FRF.4 to support high 
     * speed interfaces. This extension is not currently supported 
     * by the switch vendors.
     */
    ie_ptr->length = LINK_CORE_IE_MAX_LENGTH - 4;

    ie_ptr->max_fmif_size_field_id = MAX_FMIF_ID;

    /* 
     * The upper 7 bits of the fmif go into byte 3a, the lower 7 bits go 
     * into byte 3b. Many of the bitwise ANDs are used for cleanup and 
     * do not directly relate to functionality.
     */
    ie_ptr->max_out_fmif_size_a = ((params->out_fmif & 0x7F80) >> 7) & 0x7F;
    ie_ptr->max_out_fmif_size_b = (params->out_fmif & 0x7F) & 0x7F;

    ie_ptr->max_inc_fmif_size_a = ((params->inc_fmif & 0x7F80) >> 7) & 0x7F;
    ie_ptr->max_inc_fmif_size_b = (params->inc_fmif & 0x007F) | EXT_BIT_1;

    /* 
     * To understand the following mumbo jumbo, please see section 4.5.19
     * Of the Q.933 specification. Basic expectation is that 
     * the magnitude holds a power of 10 and the multiplier holds a number 
     * that is not divisible by 10.
     * 
     * Take the user input, convert it into the required format. To do this,
     * we must successively divide by 10 until we divide no more. At this
     * time we have a magnitude and a multiplier.
     */
    magnitude = 0;
    multiplier =  params->out_cir;
    fr_svc_determine_mag_and_mult(&magnitude, &multiplier);

    ie_ptr->thruput_field_id = THROUGHPUT_ID;
    /* 
     * The top 4 multiplier bits belong in byte 4a of picture in Q.933. 
     */
    ie_ptr->out_thruput_a = ((magnitude << 4) | ((multiplier >> 7) & 0x000F)) 
						& 0x7F;
    ie_ptr->out_multiplier = (multiplier & 0x007F) & 0x7F;

    magnitude = 0;
    multiplier =  params->inc_cir;
    fr_svc_determine_mag_and_mult(&magnitude, &multiplier);

    /* 
     * The top 4 multiplier bits belong in byte 4c of picture in Q.933. 
     * Section 4.5.19, Q.933
     */
    ie_ptr->inc_thruput_b = ((magnitude << 4) | 
			((multiplier >> 7) & 0x000F)) & 0x7F;
    ie_ptr->inc_multiplier = (multiplier & 0x007F) | EXT_BIT_1;

    magnitude = 0;
    multiplier =  params->min_out_cir;
    fr_svc_determine_mag_and_mult(&magnitude, &multiplier);

    ie_ptr->minimum_thruput_id = MIN_ACCEPTABLE_THROUGHPUT;
    ie_ptr->out_min_thruput_a = ((magnitude << 4) | 
			((multiplier >> 7) & 0x000F)) & 0x7F;
    ie_ptr->out_min_thruput_mult = (multiplier & 0x007F) & 0x7F;

    magnitude = 0;
    multiplier =  params->min_inc_cir;
    fr_svc_determine_mag_and_mult(&magnitude, &multiplier);

    ie_ptr->inc_min_thruput_a = ((magnitude << 4) | 
			((multiplier >> 7) & 0x000F)) & 0x7F;
    ie_ptr->inc_min_thruput_mult = (multiplier & 0x007F) | EXT_BIT_1;

    /* 
     * This field is distributed over 2 bytes. Separate the bits
     * so that 7 bits belong in the upper byte and 7 bits belong in
     * the lower byte. Use bitwise ANDs generously to cleanup.
     * Section 4.5.19, Q.933
     */
    ie_ptr->cbs_id = COMMITED_BURST_SIZE_ID;
    ie_ptr->out_cbs_a = (((params->out_cbs >> 7) & 
					0x007F) & 0x7F);
    ie_ptr->out_cbs_b = (params->out_cbs & 0x007F) & 0x7F;

    ie_ptr->inc_cbs_a = ((params->inc_cbs >> 7) & 0x007F) & 0x7F;
    ie_ptr->inc_cbs_b = ((params->inc_cbs & 0x007F) & 0x7F) | EXT_BIT_1;

    /* 
     * This field is distributed over 2 bytes. Separate the bits
     * so that 7 bits belong in the upper byte and 7 bits belong in
     * the lower byte. Use bitwise ANDs generously to cleanup.
     * Section 4.5.19, Q.933
     */
    ie_ptr->ebs_id = EXCESS_BURST_SIZE_ID;
    ie_ptr->out_ebs_a = ((params->out_ebs >> 7) & 0x007F) & 0x7F;
    ie_ptr->out_ebs_b = (params->out_ebs & 0x007F) & 0x7F;

    ie_ptr->inc_ebs_a = ((params->inc_ebs >> 7) & 0x007F) & 0x7F;
    ie_ptr->inc_ebs_b = ((params->inc_ebs & 0x007F) & 0x7F) | EXT_BIT_1;

    /* 
     * An extension to the Link Core IE that didn't get 
     * defined until mid-1995. Both Hughes and NT do NOT support these
     * extensions. If the user configures values that do not fit in the
     * original definitions then this extension will be included.
     * If it is included, the IE length needs to be adjusted accordingly.
     * Attachment A, FRF.4
     */
    if (params->inc_cbs_mag || params->out_cbs_mag) {
    	ie_ptr->cbs_mag_id = CBS_MAG_ID;
    	ie_ptr->cbs_magnitude = (((params->inc_cbs_mag << 3) | 
			(params->out_cbs_mag)) & 0x3F) | EXT_BIT_1; 
    	ie_ptr->length++; 
    	ie_ptr->length++; 
    }

    /* 
     * An extension to the Link Core IE that didn't get 
     * defined until mid-1995. Both Hughes and NT do NOT support these
     * extensions. If the user configures values that do not fit in the
     * original definitions then this extension will be included.
     * If it is included, the IE length needs to be adjusted accordingly.
     * Attachment A, FRF.4
     */
    if (params->inc_ebs_mag || params->out_ebs_mag) {
    	ie_ptr->ebs_mag_id = EBS_MAG_ID;
    	ie_ptr->ebs_magnitude = (((params->inc_ebs_mag << 3) | 
			(params->out_ebs_mag)) & 0x3F) | EXT_BIT_1;
    	ie_ptr->length++; 
    	ie_ptr->length++; 
    }

    if (L3IE_DBG)
	svc_ie_print("Link Lyr Core", (char *)ie_ptr, ie_ptr->length + 
					LINK_CORE_IE_HDR_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. 
     */
    *ptr =  *ptr + ie_ptr->length + LINK_CORE_IE_HDR_LENGTH;
}
/* 
 * add_connect_link_lyr_core_ie
 * SPECIAL link_lyr_core_ie for a CONNECT message.
 * Rather than dirty up the add_link_lyr_core_ie routine with all sorts
 * of conditional statements, a new routine has been added. The intent is to
 * add a link layer core parameter IE. You'll need a copy of Q.933
 * before wading into this routine. The byte names are derived from the 
 * picture in Section 4.5.19 on Page 32 of Q.933. 
 * 
 * Some fields hold the actual value, while others hold the value in 
 * magnitude * (10 ** multiplier) form.
 * 
 * Warning: If you are subject to nausea, please tread carefully!!!!!!
 */
void add_connect_link_lyr_core_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    connect_link_core_ie 	*ie_ptr;
    uint 			magnitude;
    uint			multiplier;

    multiplier = 0;
    ie_ptr = (connect_link_core_ie *)(*ptr);
    ie_ptr->ie_id = LINK_LAYER_CORE_PARAM_IE_ID;	/* ID is 0x48 */

    /*  
     * The -4 in the statement relates to FTFTC/96-025 extension to FRF.4
     * The FRF has specified an extension to FRF.4 to support high 
     * speed interfaces. This extension is not currently supported 
     * by the switch vendors.
     */
    ie_ptr->length = CONNECT_LINK_CORE_IE_MAX_LENGTH - 4;

    ie_ptr->max_fmif_size_field_id = MAX_FMIF_ID;

    /* 
     * The upper 7 bits of the fmif go into byte 3a, the lower 7 bits go 
     * into byte 3b. 
     */
    ie_ptr->max_out_fmif_size_a = ((params->out_fmif & 0x7F80) >> 7) & 0x7F;
    ie_ptr->max_out_fmif_size_b = (params->out_fmif & 0x7F) & 0x7F;

    ie_ptr->max_inc_fmif_size_a = ((params->inc_fmif & 0x7F80) >> 7) & 0x7F;
    ie_ptr->max_inc_fmif_size_b = (params->inc_fmif & 0x007F) | EXT_BIT_1;

    /* 
     * To understand the following mumbo jumbo, please see section 4.5.19
     * Of the FR SVC implementation agreement. Basic expectation is that 
     * the magnitude holds a power of 10 and the multiplier holds a number 
     * that is not divisible by 10.
     * 
     * Take the user input, convert it into the required format. To do this,
     * we must successively divide by 10 until we divide no more. At this
     * time we have a magnitude and a multiplier.
     */
    magnitude = 0;
    multiplier =  params->out_cir;
    fr_svc_determine_mag_and_mult(&magnitude, &multiplier);

    ie_ptr->thruput_field_id = THROUGHPUT_ID;
    /* 
     * The top 4 multiplier bits belong in byte 4a. 
     */
    ie_ptr->out_thruput_a = ((magnitude << 4) | ((multiplier >> 7) & 0x000F)) 
						& 0x7F;
    ie_ptr->out_multiplier = (multiplier & 0x007F) & 0x7F;

    magnitude = 0;
    multiplier =  params->inc_cir;
    fr_svc_determine_mag_and_mult(&magnitude, &multiplier);

    /* 
     * The top 4 multiplier bits belong in byte 4c. 
     */
    ie_ptr->inc_thruput_b = ((magnitude << 4) | 
			((multiplier >> 7) & 0x000F)) & 0x7F;
    ie_ptr->inc_multiplier = (multiplier & 0x007F) | EXT_BIT_1;

    /* 
     * This field is distributed over 2 bytes. Separate the bits
     * so that 7 bits belong in the upper byte and 7 bits belong in
     * the lower byte. Use bitwise ANDs generously to cleanup.
     */
    ie_ptr->cbs_id = COMMITED_BURST_SIZE_ID;
    ie_ptr->out_cbs_a = (((params->out_cbs >> 7) & 
					0x007F) & 0x7F);
    ie_ptr->out_cbs_b = (params->out_cbs & 0x007F) & 0x7F;

    ie_ptr->inc_cbs_a = ((params->inc_cbs >> 7) & 0x007F) & 0x7F;
    ie_ptr->inc_cbs_b = ((params->inc_cbs & 0x007F) & 0x7F) | EXT_BIT_1;

    /* 
     * This field is distributed over 2 bytes. Separate the bits
     * so that 7 bits belong in the upper byte and 7 bits belong in
     * the lower byte. Use bitwise ANDs generously to cleanup.
     */
    ie_ptr->ebs_id = EXCESS_BURST_SIZE_ID;
    ie_ptr->out_ebs_a = ((params->out_ebs >> 7) & 0x007F) & 0x7F;
    ie_ptr->out_ebs_b = (params->out_ebs & 0x007F) & 0x7F;

    ie_ptr->inc_ebs_a = ((params->inc_ebs >> 7) & 0x007F) & 0x7F;
    ie_ptr->inc_ebs_b = ((params->inc_ebs & 0x007F) & 0x7F) | EXT_BIT_1;

    /* 
     * An extension to the Link Core IE that didn't get 
     * defined until mid-1995. Both Hughes and NT do NOT support these
     * extensions. If the user configures values that do not fit in the
     * original definitions then this extension will be included.
     * If it is included, the IE length needs to be adjusted accordingly.
     * Attachment A, FRF.4
     */
    if (params->inc_cbs_mag || params->out_cbs_mag) {
    	ie_ptr->cbs_mag_id = CBS_MAG_ID;
    	ie_ptr->cbs_magnitude = (((params->inc_cbs_mag << 3) | 
			(params->out_cbs_mag)) & 0x3F) | EXT_BIT_1; 
    	ie_ptr->length++; 
    	ie_ptr->length++; 
    }

    /* 
     * An extension to the Link Core IE that didn't get 
     * defined until mid-1995. Both Hughes and NT do NOT support these
     * extensions. If the user configures values that do not fit in the
     * original definitions then this extension will be included.
     * If it is included, the IE length needs to be adjusted accordingly.
     * Attachment A, FRF.4
     */
    if (params->inc_ebs_mag || params->out_ebs_mag) {
    	ie_ptr->ebs_mag_id = EBS_MAG_ID;
    	ie_ptr->ebs_magnitude = (((params->inc_ebs_mag << 3) | 
			(params->out_ebs_mag)) & 0x3F) | EXT_BIT_1;
    	ie_ptr->length++; 
    	ie_ptr->length++; 
    }

    if (L3IE_DBG)
	svc_ie_print("Link Lyr Core", (char *)ie_ptr, ie_ptr->length + 
					LINK_CORE_IE_HDR_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. 
     */
    *ptr =  *ptr + ie_ptr->length + LINK_CORE_IE_HDR_LENGTH;

}
/* 
 * fr_svc_determine_mag_and_mult
 * The coding of the link core IE is such that all values are 
 * broken down into a magnitude and a multiplier. However, since 
 * the user has entered a bps number, it needs to be converted 
 * to the required format. 
 */
void fr_svc_determine_mag_and_mult (uint *magnitude, uint *multiplier)
{
    uint 	temp, temp2; 

    temp = *multiplier;
    while (temp && (!(temp % 10))) {
	(*magnitude)++;
	*multiplier = (temp)/10;
	temp = *multiplier;
    }
    temp2 = temp;
}
/* 
 * fr_svc_strncpy
 * A special form of sstrncpy that will mimic the unix strncpy. 
 * max characters are copied over, there is no NULL padding. 
 * I need this because all address information is stored in string
 * form but when it is copied into an IE, the null padding is not 
 * needed.
 */
static void fr_svc_strncpy (char *dst, char const *src, ulong max)
{
    while ((max-- > 0) && (*src)) {
	*dst = *src;
	dst++;
	src++;
    }
}

/* 
 * add_calling_pty_ie
 * Section 4.5.10 of Q.931.
 * Each subinterface should have a X.121/E.164 address assigned to it. 
 * We have assigned a maximum 20 byte slot to put the addresses into. 
 * The actual length will get adjusted based on the length of the 
 * address string. 
 */
static void add_calling_pty_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    calling_pty_number_ie 	*ie_ptr;

    ie_ptr = (calling_pty_number_ie *)*ptr;
    ie_ptr->ie_id = CALLING_PARTY_NUMBER_ID;	/* ID is 0x6C */

    /* 
     * Section 4.5.10 of Q.931. Calling party IE details. 
     * E164, X121, or private plan, 
     * National, International, network specific, unknown, etc. 
     */
    ie_ptr->numb_type_id = ((params->svc_addr.saddr.num_plan) |
		(params->svc_addr.saddr.num_type << 4)) & 0x7F;

    /* 
     * User provided, not screened, no indicators set 
     */
    ie_ptr->indicators = 0 | EXT_BIT_1;

    /* 
     * Make sure that the number is stored as ASCII and that the
     * string is NOT terminated with a NULL. Adjust the IE length
     * based on address length.
     * 
     * For example: 
     * 	20 bytes are allocated for address field to cover maximum size.
     * 	Let's say the user enters a address string with 11 characters. 
     * 	Then, the unused portion of the allocated buffer will be : 
     *			11 - 20	= -9
     * This is what the (strlen((uchar *)saddr) - MAX_ADDRESS_LENGTH) is.		
     */
    fr_svc_strncpy( (uchar *)&ie_ptr->address, (uchar *)&params->svc_addr.saddr,
			strlen((uchar *)&params->svc_addr.saddr));
    ie_ptr->length = sizeof(calling_pty_number_ie) - 
		CALLING_PARTY_IE_HDR_LENGTH + 
		(strlen((uchar *)&params->svc_addr.saddr) - MAX_ADDRESS_LENGTH);

    if (L3IE_DBG)
	svc_ie_print("Calling Party", (char *)ie_ptr, ie_ptr->length + 
				CALLING_PARTY_IE_HDR_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. CALLING_PARTY_IE_HDR_LENGTH is 2.
     */
    *ptr = *ptr + ie_ptr->length + CALLING_PARTY_IE_HDR_LENGTH;
}

/* 
 * add_calling_pty_subaddr_ie
 * User assigned subaddress, used when multiple SVCs are setup between the
 * same two endpoints. Section 4.5.11 of Q.931.
 */
static void add_calling_pty_subaddr_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    calling_pty_subaddr_ie 	*ie_ptr;

    ie_ptr = (calling_pty_subaddr_ie *)*ptr;
    ie_ptr->ie_id = CALLING_PARTY_SUBADDR_ID;	/* ID is 0x6D */
    ie_ptr->length = sizeof(calling_pty_subaddr_ie) - 
				CALLING_PARTY_SUBADDR_HDR_LNGTH;
    ie_ptr->type_of_addr = USER_SPECIFIED_SUBADDR;

    /* 
     * only 1 byte allocated for subaddress
     */
    memset(&ie_ptr->address, 0, 1);	
    ie_ptr->address = params->svc_addr.saddr.subaddr;

    if (L3IE_DBG)
	svc_ie_print("Calling Party Subaddr", (char *)ie_ptr, 
		ie_ptr->length + CALLING_PARTY_SUBADDR_HDR_LNGTH, 
					OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. CALLING_PARTY_SUBADDR_HDR_LNGTH is 2.
     */
    *ptr = *ptr + ie_ptr->length + CALLING_PARTY_SUBADDR_HDR_LNGTH;
}

/* 
 * add_called_pty_ie
 * Section 4.5.8 of Q.931
 * We have assigned a maximum 20 byte slot to put the addresses into. 
 * The actual length will get adjusted based on the length of the 
 * address string. 
 */
static void add_called_pty_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    called_pty_ie *ie_ptr;

    ie_ptr = (called_pty_ie *)(*ptr);
    ie_ptr->ie_id = CALLED_PARTY_NUMBER_ID;	/* ID is 0x70 */

    /* 
     * Section 4.5.8 of Q.931. Called party IE details. 
     * The user configures a lot of this information.
     * E164, X121, or private plan, National, International, 
     * network specific, unknown, etc. 
     */
    ie_ptr->numb_type_id = ((params->svc_addr.daddr.num_plan) | 
		(params->svc_addr.daddr.num_type << 4)) | EXT_BIT_1;

    /* 
     * Make sure that the number is stored as ASCII and that the
     * string is NOT terminated with a NULL. Adjust the IE length
     * based on address length.
     * 
     * For example: 
     * 	20 bytes are allocated for address field to cover maximum size.
     * 	Let's say the user enters a address string with 11 characters. 
     * 	Then, the unused portion of the allocated buffer will be : 
     *			11 - 20	= -9
     * This is what the (strlen((uchar *)daddr) - MAX_ADDRESS_LENGTH) is.		
     */
    fr_svc_strncpy((uchar *)&ie_ptr->address, (uchar *)&params->svc_addr.daddr, 
		strlen((uchar *)&params->svc_addr.daddr));
    ie_ptr->length = sizeof(called_pty_ie) - 
		CALLED_PARTY_HDR_LENGTH  + 
		(strlen((uchar *)&params->svc_addr.daddr) - MAX_ADDRESS_LENGTH);

    if (L3IE_DBG)
	svc_ie_print("Called Party", (char *)ie_ptr, ie_ptr->length + 
					CALLED_PARTY_HDR_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. 
     */
    *ptr = *ptr + ie_ptr->length + CALLING_PARTY_IE_HDR_LENGTH;
}

/* 
 * add_called_pty_subaddr_ie
 * User assigned subaddress, used when multiple SVCs are setup between the
 * same two endpoints. Section 4.5.9 of Q.931.
 */
static void add_called_pty_subaddr_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    called_pty_subaddr_ie *ie_ptr;

    ie_ptr = (called_pty_subaddr_ie *)(*ptr);
    ie_ptr->ie_id = CALLED_PARTY_SUBADDR_ID;		/* ID is 0x71 */
    ie_ptr->length = sizeof(called_pty_subaddr_ie) - 
				CALLED_PARTY_SUBADDR_HDR_LNGTH;

    ie_ptr->type_of_addr = USER_SPECIFIED_SUBADDR;

    /* 
     * only 1 byte allocated for subaddress
     */
    memset(&ie_ptr->address, 0, 1); 
    ie_ptr->address = params->svc_addr.daddr.subaddr;

    if (L3IE_DBG)
	svc_ie_print("Called Party Subaddr", (char *)ie_ptr, 
			ie_ptr->length + CALLED_PARTY_SUBADDR_HDR_LNGTH, 
			OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. 
     */
    *ptr = *ptr + ie_ptr->length + CALLING_PARTY_IE_HDR_LENGTH;
}
/* 
 * add_connected_pty_ie
 * This IE indicates the number which is connected to a call.
 * The connected number may be different from the called party
 * number because of changes during the lifetime of a call.
 * Reference: Q.951
 * Once a call gets setup, we do not actually care what the 
 * connected party number is. Some primitive checking is done
 * before the call is awarded.
 */
static void add_connected_pty_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    connected_pty_ie *ie_ptr;

    ie_ptr = (connected_pty_ie *)(*ptr);
    ie_ptr->ie_id = CONN_PARTY_NUMBER_ID;

    ie_ptr->numb_type_id = ((params->svc_addr.daddr.num_plan) | 
		(params->svc_addr.daddr.num_type << 4));

    /* 
     * User provided, not screened, no indicators set 
     */
    ie_ptr->indicators = 0 | EXT_BIT_1;

    /* 
     * Make sure that the number is stored as ASCII and that the
     * string is NOT terminated with a NULL. Adjust the IE length
     * based on address length.
     */
    fr_svc_strncpy((uchar *)&ie_ptr->address, 
		(uchar *)&params->svc_addr.daddr,
		strlen((uchar *)&params->svc_addr.daddr));
    ie_ptr->length = sizeof(connected_pty_ie) - 
		CONN_PARTY_IE_HDR_LENGTH + 
		(strlen((uchar *)&params->svc_addr.daddr) - MAX_ADDRESS_LENGTH);

    if (L3IE_DBG)
	svc_ie_print("Connected Party", (char *)ie_ptr, ie_ptr->length + 
					CONN_PARTY_IE_HDR_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. 
     */
    *ptr = *ptr + ie_ptr->length + CALLING_PARTY_IE_HDR_LENGTH;
}

/* 
 * add_connected_pty_subaddr_ie
 * An extension to the connected_pty_ie. Currently not used to 
 * covey subaddresses. The field is always zero.
 */
static void add_connected_pty_subaddr_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    connected_num_subaddr_ie *ie_ptr;

    ie_ptr = (connected_num_subaddr_ie *)(*ptr);
    ie_ptr->ie_id = CONN_PARTY_NUMBER_SUBADDR_ID;
    ie_ptr->length = sizeof(calling_pty_subaddr_ie) - 
				CONN_PARTY_SUBADDR_IE_HDR_LENGTH;

    ie_ptr->type_of_addr = USER_SPECIFIED_SUBADDR;

    /* 
     * Subaddress field is currently not used. Fill 
     * it with 0s.
     */
    memset(&ie_ptr->address, 0, 1); 

    if (L3IE_DBG)
	svc_ie_print("Connected Party Subaddr", (char *)ie_ptr, 
			ie_ptr->length + CONN_PARTY_SUBADDR_IE_HDR_LENGTH, 
						OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. 
     */
    *ptr = *ptr + ie_ptr->length + CONN_PARTY_SUBADDR_IE_HDR_LENGTH;
}

/* 
 * add_llc_ie
 *
 * Provides a means which should be used for compatibility
 * checking by an addressed entity i.e. a remote user or
 * an interworking unit or a high layer function network node
 * addressed by the calling user.
 */
void add_llc_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    low_layer_compatibility_ie *ie_ptr;

    ie_ptr = (low_layer_compatibility_ie *)(*ptr);
    ie_ptr->ie_id = LOW_LAYER_COMPATIBILITY_IE_ID;
    ie_ptr->length = LOW_LAYER_COMP_IE_LENGTH;

    /*   
     * The following is best explained when looking at Section 4.5.21 or
     * pages 40-46 of Q.933. Most switches should not care what we stuff 
     * in these fields. 
     */
    ie_ptr->info_cdg_std = LLC_BYTE3;
    ie_ptr->xfer_mode = FRAME_XFER_MODE;
    ie_ptr->user_info_l2_prot = LLC_IE_CCITT_Q22;

    if (L3IE_DBG)
	svc_ie_print("Low Lyr Comp", (char *)ie_ptr, ie_ptr->length + 
				LOW_LAYER_COMP_IE_HDR_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. 
     */
    *ptr = *ptr + LOW_LAYER_COMP_IE_TOTAL_LENGTH;
}

/* 
 * add_user_user_ie
 * Used to convey information between users.
 * Currently cisco's implementation does not pass any information 
 * using this IE. Possibly for future use.
 */
void add_user_user_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    user_user_ie *ie_ptr;

    ie_ptr = (user_user_ie *)(*ptr);
    ie_ptr->ie_id = USER_USER_IE_ID;		/* ID is 0x7E */
    ie_ptr->length = USER_USER_IE_LENGTH;	/* length is 2 */

    ie_ptr->prot_disc = 0x00; 			/* user defined protocol */
    ie_ptr->user_info = 0x00;			/* no useful info */

    if (L3IE_DBG)
	svc_ie_print("User to User", (char *)ie_ptr, ie_ptr->length + 
					USER_USER_IE_HDR_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. 
     */
    *ptr = *ptr + USER_USER_IE_HDR_LENGTH + USER_USER_IE_LENGTH;
}

/* 
 * add_cause_ie
 * Although the cause IE can be repeated, cisco's implementation allows only 
 * one cause element at a time. If more than one needs to go out, 
 * formulate more than one message. I notice that NT also only sends out 
 * one cause IE at a time.
 */
void add_cause_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    cause_ie *ie_ptr;

    ie_ptr = (cause_ie *)(*ptr);
    ie_ptr->ie_id = CAUSE_IE_ID;		/* ID is 0x7E */
    ie_ptr->length = CAUSE_IE_LENGTH;		/* length is 2 */

    /*
     * See section 10 (4.5.11, Q.931) of the SVC implementation agreement 
     * for more details.
     */
    ie_ptr->cdg_std_location = CODING_STD_LOCATION;

    /* 
     * params->cause is set by the L3 SDL code.
     */
    ie_ptr->cause_value = (params->cause) | EXT_BIT_1; 
    ie_ptr->diagnostics = 0;

    if (L3IE_DBG) {
	svc_ie_print("Cause", (char *)ie_ptr, ie_ptr->length + 
						CAUSE_IE_HDR_LENGTH, OUT);
	buginf("\n\t\t\tCause No.%d : %s",params->cause, 
	       cause_def(params->cause));
    }
    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. 
     */
    *ptr = *ptr + CAUSE_IE_HDR_LENGTH + CAUSE_IE_LENGTH;
}

/* 
 * add_cs_ie
 * Call State IE. See Section 4.5.7, page 74 of Q.931. Typically, we will 
 * assume CCITT standardized coding and so, the coding_standard field will 
 * be zero.
 */
void add_cs_ie (
	fr_svc_info_type *svc,
	uchar **ptr, 
	struct fr_svc_params *params)
{
    call_state_ie *ie_ptr;

    ie_ptr = (call_state_ie *)(*ptr);
    ie_ptr->ie_id = CALL_STATE_IE_ID;
    ie_ptr->length = 1;			/* This is a 1 byte IE */

    /* 
     * We need to make sure that we populate the call state field
     * so that appropriate checks can be performed. This is the only IE that 
     * is defined using bitfields as a test to see how different platforms
     * handle bitfields. May need to remove later.
     */
    ie_ptr->call_state = svc->call_state;

    if (L3IE_DBG)
	svc_ie_print("Call State", (char *)ie_ptr, ie_ptr->length + 
					CALL_STATE_IE_HDR_LENGTH, OUT);

    /* 
     * Increment the IE pointer so that we'll now point to the 
     * next IE. 
     */
    *ptr = *ptr + CALL_STATE_IE_HDR_LENGTH + ie_ptr->length;
}

/* 
 * fr_svc_send_msg
 * Main routine that coordinates the sending of messages to the network. 
 * datagramsize must be set after the message gets formulated.
 */
void fr_svc_send_msg (
	hwidbtype *hwidb,
	fr_svc_info_type *svc,
	ushort msg_type,
	uint call_ref,
	struct fr_svc_params *params)
{
    paktype		*pak;
    uchar		ctmp;
    uchar		*cptr;

    /* 
     * Need to setup some variables in the params structure so that 
     * the info can be printed out. 
     */
    params->msg_type = msg_type;
    params->call_ref = call_ref;
    params->cause = svc->params.cause;
    pak = NULL;

    /* 
     * The largest message is the SETUP message. Invoke a buffer of
     * size setup_msg for all messages, then only use as much as 
     * required. 
     */
	    pak = getbuffer(sizeof(svc_setup_msg) );
	    if (!pak)
		return;
	    pak->datagramstart = pak->network_start;
	    cptr = (uchar *)(pak->datagramstart );
	    memset((uchar *)pak->datagramstart, 0, sizeof(svc_setup_msg));

    switch (msg_type) {
	case MSG_SETUP:
	    if (L3IE_DBG)
		buginf("\n\nOutgoing MSG_SETUP ");
	    reg_invoke_form_setup_msg(svc, &cptr, params);
	    pak->datagramsize = cptr - pak->datagramstart;
	    break;
	case MSG_CALL_PROCEEDING:
	    if (L3IE_DBG)
		buginf("\n\nOutgoing MSG_CALL_PROCEEDING");
	    reg_invoke_form_call_proc_msg(svc, &cptr, params);
	    pak->datagramsize = cptr - pak->datagramstart;
	    break;
	case MSG_CONNECT:
	    if (L3IE_DBG)
		buginf("\n\nOutgoing MSG_CONNECT");
    	    reg_invoke_form_connect_msg(svc, &cptr, params);
	    pak->datagramsize = cptr - pak->datagramstart;
	    break;
	case MSG_DISCONNECT:
	    if (L3IE_DBG)
		buginf("\n\nOutgoing MSG_DISCONNECT");
    	    reg_invoke_form_disc_msg(svc, &cptr, params);
	    pak->datagramsize = cptr - pak->datagramstart;
	    break;
	case MSG_RELEASE:
	    if (L3IE_DBG)
		buginf("\n\nOutgoing MSG_RELEASE");
	    reg_invoke_form_release_msg(svc, &cptr, params);
	    pak->datagramsize = cptr - pak->datagramstart;
	    break;
	case MSG_RELEASE_COMPLETE:
	    if (L3IE_DBG)
		buginf("\n\nOutgoing MSG_RELEASE_COMPLETE");
    	    reg_invoke_form_release_comp_msg(svc, &cptr, params);
	    pak->datagramsize = cptr - pak->datagramstart;
	    break;
	case MSG_STATUS_ENQUIRY:
	    ctmp = svc->num_status_enq_sent++;
	    if (ctmp > T322_RETRY_COUNT) {
		svc->params.cause = TEMPORARY_FAILURE;
		fr_svc_send_msg(hwidb, svc, MSG_RELEASE, call_ref, &svc->params);
	    }
	    if (L3IE_DBG)
		buginf("\n\nOutgoing MSG_STATUS_ENQUIRY");
    	    reg_invoke_form_status_enq_msg(svc, &cptr, params);
	    pak->datagramsize = cptr - pak->datagramstart;
	    /* Start the T322 timer */
	    break;
	case MSG_STATUS:
	    svc->num_status_enq_sent = 0;
	    if (L3IE_DBG)
		buginf("\n\nOutgoing MSG_STATUS");
    	    reg_invoke_form_status_msg(svc, &cptr, params);
	    pak->datagramsize = cptr - pak->datagramstart;
	    break;
	default:
    	    if (L3IE_DBG)
    	    	buginf("\nwithin fr_svc_send_msg bogus msg %d", msg_type);
	    break;
    }

     pak->if_output = hwidb->firstsw;
     pak->linktype = LINK_ADDRESS;
     pak->flags |= PAK_PRIORITY;
     fr_svc_l3_to_l2(FR_DL_DATA_REQ, hwidb->frame_relay_stuff, pak);
}

/* 
 * svc_outgoing_msg_init
 * The FR SVC implementation agreement specifies the various IEs 
 * that are icluded within each message. Use Q.933 as the base 
 * reference and use the Implementation agreement as the FR SVC 
 * specific reference. 
 */
void svc_outgoing_msg_init ()
{

    /* SETUP Message */
    reg_add_form_setup_msg(add_prot_disc_ie, "add_prot_disc_ie");
    reg_add_form_setup_msg(add_call_ref_ie, "add_call_ref_ie");
    reg_add_form_setup_msg(add_msg_type_ie, "add_msg_type_ie");
    reg_add_form_setup_msg(add_bearer_cap_ie, "add_bearer_cap_ie");
    reg_add_form_setup_msg(add_link_lyr_core_ie, "add_link_lyr_core_ie");
    reg_add_form_setup_msg(add_calling_pty_ie, "add_calling_pty_ie");
    reg_add_form_setup_msg(add_calling_pty_subaddr_ie, 
					"add_calling_pty_subaddr_ie");
    reg_add_form_setup_msg(add_called_pty_ie, "add_called_pty_ie");
    reg_add_form_setup_msg(add_called_pty_subaddr_ie, 
					"add_called_pty_subaddr_ie");
    reg_add_form_setup_msg(add_llc_ie, "add_llc_ie");
    reg_add_form_setup_msg(add_user_user_ie, "add_user_user_ie");

    /* CALL PROCEEDING message */
    reg_add_form_call_proc_msg(add_prot_disc_ie, "add_prot_disc_ie");
    reg_add_form_call_proc_msg(add_call_ref_ie, "add_call_ref_ie");
    reg_add_form_call_proc_msg(add_msg_type_ie, "add_msg_type_ie");
    reg_add_form_call_proc_msg(add_dlci_ie, "add_dlci_ie");

    /* CONNECT message */
    reg_add_form_connect_msg(add_prot_disc_ie, "add_prot_disc_ie");
    reg_add_form_connect_msg(add_call_ref_ie, "add_call_ref_ie");
    reg_add_form_connect_msg(add_msg_type_ie, "add_msg_type_ie");
    reg_add_form_connect_msg(add_dlci_ie, "add_dlci_ie");
    reg_add_form_connect_msg(add_connect_link_lyr_core_ie, 
				"add_connect_link_lyr_core_ie");
    reg_add_form_connect_msg(add_connected_pty_ie, "add_connected_pty_ie");
    reg_add_form_connect_msg(add_connected_pty_subaddr_ie, 
						"add_ctd_pty_subaddr_ie");
    reg_add_form_connect_msg(add_user_user_ie, "add_user_user_ie");

    /* DISCONNECT message */
    reg_add_form_disc_msg(add_prot_disc_ie, "add_prot_disc_ie");
    reg_add_form_disc_msg(add_call_ref_ie, "add_call_ref_ie");
    reg_add_form_disc_msg(add_msg_type_ie, "add_msg_type_ie");
    reg_add_form_disc_msg(add_cause_ie, "add_cause_ie");

    /* RELEASE message */
    reg_add_form_release_msg(add_prot_disc_ie, "add_prot_disc_ie");
    reg_add_form_release_msg(add_call_ref_ie, "add_call_ref_ie");
    reg_add_form_release_msg(add_msg_type_ie, "add_msg_type_ie");
    reg_add_form_release_msg(add_cause_ie, "add_cause_ie");

    /* RELEASE COMPLETE message */
    reg_add_form_release_comp_msg(add_prot_disc_ie, "add_prot_disc_ie");
    reg_add_form_release_comp_msg(add_call_ref_ie, "add_call_ref_ie");
    reg_add_form_release_comp_msg(add_msg_type_ie, "add_msg_type_ie");
    reg_add_form_release_comp_msg(add_cause_ie, "add_cause_ie");

    /* STATUS message */
    reg_add_form_status_msg(add_prot_disc_ie, "add_prot_disc_ie");
    reg_add_form_status_msg(add_call_ref_ie, "add_call_ref_ie");
    reg_add_form_status_msg(add_msg_type_ie, "add_msg_type_ie");
    reg_add_form_status_msg(add_cause_ie, "add_cause_ie");
    reg_add_form_status_msg(add_cs_ie, "add_cs_ie");

    /* STATUS ENQUIRY message */
    reg_add_form_status_enq_msg(add_prot_disc_ie, "add_prot_disc_ie");
    reg_add_form_status_enq_msg(add_call_ref_ie, "add_call_ref_ie");
    reg_add_form_status_enq_msg(add_msg_type_ie, "add_msg_type_ie");
}
