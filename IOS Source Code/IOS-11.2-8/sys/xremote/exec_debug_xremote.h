/* $Id: exec_debug_xremote.h,v 3.2 1995/11/17 19:26:30 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/exec_debug_xremote.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ X R E M O T E . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_xremote.h,v $
 * Revision 3.2  1995/11/17  19:26:30  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:42  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/19  14:49:28  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:56:13  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:20:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug xremote xdm
 *
 */
EOLS	(debug_xr_xdm_eol, debug_command, DEBUG_XRXDM);
KEYWORD (debug_xr_xdm, debug_xr_xdm_eol, no_alt,
	 "xdm", "XREMOTE XDM client", PRIV_OPR);

/******************************************************************
 * debug xremote packet
 *
 */
EOLS	(debug_xr_packet_eol, debug_command, DEBUG_XRPACKET);
KEYWORD (debug_xr_packet, debug_xr_packet_eol, debug_xr_xdm,
	 "packet", "XREMOTE packets", PRIV_OPR);

/******************************************************************
 * debug xremote requests
 *
 */
EOLS	(debug_xr_requests_eol, debug_command, DEBUG_XRREQUESTS);
KEYWORD (debug_xr_requests, debug_xr_requests_eol, debug_xr_packet,
	 "requests", "XREMOTE requests", PRIV_OPR);

/******************************************************************
 * debug xremote replies
 *
 */
EOLS	(debug_xr_replies_eol, debug_command, DEBUG_XRREPLIES);
KEYWORD (debug_xr_replies, debug_xr_replies_eol, debug_xr_requests,
	 "replies", "XREMOTE replies", PRIV_OPR);

/******************************************************************
 * debug xremote flow
 *
 */
EOLS	(debug_xr_flow_eol, debug_command, DEBUG_XRFLOW);
KEYWORD (debug_xr_flow, debug_xr_flow_eol, debug_xr_replies,
	 "flow", "XREMOTE client flow", PRIV_OPR);

/******************************************************************
 * debug xremote files
 *
 */
EOLS	(debug_xr_files_eol, debug_command, DEBUG_XRFILES);
KEYWORD (debug_xr_files, debug_xr_files_eol, debug_xr_flow,
	 "files", "XREMOTE file access", PRIV_OPR);

/******************************************************************
 * debug xremote deltas
 *
 */
EOLS	(debug_xr_deltas_eol, debug_command, DEBUG_XRDELTA);
KEYWORD (debug_xr_deltas, debug_xr_deltas_eol, debug_xr_files,
	 "deltas", "XREMOTE delta compression", PRIV_OPR);

/******************************************************************
 * debug xremote bytestream protocol
 * debug xremote bytestream flow
 *
 */
EOLS	(debug_xr_bs_protocol_eol, debug_command, DEBUG_XRBSPRO);
KEYWORD (debug_xr_bs_protocol, debug_xr_bs_protocol_eol, no_alt,
	 "protocol", "XREMOTE bytestream protocol", PRIV_OPR);

EOLS	(debug_xr_bs_flow_eol, debug_command, DEBUG_XRBSFLO);
KEYWORD (debug_xr_bs_flow, debug_xr_bs_flow_eol, debug_xr_bs_protocol,
	 "flow", "XREMOTE bytestream state machine flow", PRIV_OPR);

KEYWORD (debug_xr_bs, debug_xr_bs_flow, debug_xr_deltas,
	 "bytestream", "XREMOTE bytestream information", PRIV_OPR);

/******************************************************************
 * debug xremote
 *
 */
KEYWORD_DEBUG(debug_xremote, debug_xr_bs, ALTERNATE,
	      OBJ(pdb,1), xr_debug_arr,
	      "xremote", "XREMOTE", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_xremote
