/* $Id: exec_debug_channel.h,v 3.2.60.1 1996/05/29 18:44:58 motto Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_debug_channel.h,v $
 *------------------------------------------------------------------
 * parser/exec_debug_channel.h: DEBUG command parsing code
 *
 * June 1994, William H. Palmer
 *
 * Copyright (c) 1994-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_channel.h,v $
 * Revision 3.2.60.1  1996/05/29  18:44:58  motto
 * CSCdi46832:  vencap failed debug messages displayed for non cip
 * explorers
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:27:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/05  17:34:24  motto
 * Close window on BADVCN error message.
 * Fix SNMP query of CIP daughter boards. Was returning the virtual
 * interface as a daughter board. Remove 'debug channel trap' from
 * parser. This was not implemented to do anything.
 * CSCdi39794:  %MEMD-3-BADVCN error messages after CIP mic reload
 *
 * Revision 2.2  1995/07/13  19:42:24  motto
 * Fixed all channel tcp/ip show commands to be consistent and work
 * properly.
 * CSCdi36252:  show ex ch <slot>/<port> tcp-connections needs parms to be
 * optional
 *
 * Revision 2.1  1995/07/02  10:53:20  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.3  1995/06/19  14:47:35  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/14  15:18:33  anke
 * CSCdi35807:  Yet more spellink errors
 *
 * cls/dlcqllc.c:         Unexepcted -> Unexpected
 * ipc/msg_ipc.c:         availble -> available
 * exec_debug_channel.h:  debuggeries -> debugging
 * filesys/fslib.c:       inconsistant -> inconsistent
 * atm/sscop.c:           initalize -> initialize
 * filesys/fslib_print.c: progresss -> progress
 * ts/callback.c:         unnexpected -> unexpecte
 *
 * Revision 2.1  1995/06/07  22:17:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#undef  CHANNEL_NEXT_DEBUG
#define CHANNEL_NEXT_DEBUG no_alt
/******************************************************************
 * debug channel sysmgt
 */
EOLS	(debug_channel_sysmgt_eol, debug_command, DEBUG_CHANNEL_SYSMGT);
KEYWORD	(debug_channel_sysmgt, debug_channel_sysmgt_eol, CHANNEL_NEXT_DEBUG,
	"sysmgt", "Channel System Management Events", PRIV_OPR);

#undef  CHANNEL_NEXT_DEBUG
#define CHANNEL_NEXT_DEBUG debug_channel_sysmgt
/******************************************************************
 * debug channel ilan [ <ack> | <nack> | <filtered> ]
 */
EOLS	(debug_channel_ilan_eol, debug_command, DEBUG_CHANNEL_ILAN);
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN debug_channel_ilan_eol

EOLS	(debug_channel_ilan_filter_eol, debug_command, DEBUG_CHANNEL_ILAN_FILT);
KEYWORD	(debug_channel_ilan_filter, debug_channel_ilan_filter_eol, CHANNEL_NEXT_IN_CHAIN,
	"filtered", "Channel Internal LAN MAC filter debug", PRIV_OPR);
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN debug_channel_ilan_filter
  
EOLS	(debug_channel_ilan_nack_eol, debug_command, DEBUG_CHANNEL_ILAN_NACK);
KEYWORD	(debug_channel_ilan_nack, debug_channel_ilan_nack_eol, CHANNEL_NEXT_IN_CHAIN,
	"nack", "Send local cfg negative ack", PRIV_HIDDEN|PRIV_NOHELP);
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN debug_channel_ilan_nack

EOLS	(debug_channel_ilan_ack_eol, debug_command, DEBUG_CHANNEL_ILAN_ACK);
KEYWORD	(debug_channel_ilan_ack, debug_channel_ilan_ack_eol, CHANNEL_NEXT_IN_CHAIN,
	"ack", "Send local cfg positive ack", PRIV_HIDDEN|PRIV_NOHELP);
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN debug_channel_ilan_ack

KEYWORD	(debug_channel_vlan, CHANNEL_NEXT_IN_CHAIN, CHANNEL_NEXT_DEBUG,
	"vlan", "Virtual Lan debugging", PRIV_HIDDEN|PRIV_NOHELP);
#undef  CHANNEL_NEXT_DEBUG
#define CHANNEL_NEXT_DEBUG debug_channel_vlan

KEYWORD	(debug_channel_ilan, CHANNEL_NEXT_IN_CHAIN, CHANNEL_NEXT_DEBUG,
	"ilan", "Channel Internal LAN debug", PRIV_OPR);
#undef  CHANNEL_NEXT_DEBUG
#define CHANNEL_NEXT_DEBUG debug_channel_ilan
/******************************************************************
 * debug channel love
 */
EOLS	(debug_channel_love_eol, debug_command, DEBUG_CHANNEL_LOVE);
KEYWORD	(debug_channel_love, debug_channel_love_eol, CHANNEL_NEXT_DEBUG,
	"love", "Channel love letters", PRIV_OPR);
#undef  CHANNEL_NEXT_DEBUG
#define CHANNEL_NEXT_DEBUG debug_channel_love
/******************************************************************
 * debug channel encaps
 */
EOLS	(debug_channel_encaps_eol, debug_command, DEBUG_CHANNEL_ENCAPS);
KEYWORD	(debug_channel_encaps, debug_channel_encaps_eol, CHANNEL_NEXT_DEBUG,
	"encaps", "Channel encaps", PRIV_OPR);
#undef  CHANNEL_NEXT_DEBUG
#define CHANNEL_NEXT_DEBUG debug_channel_encaps
/******************************************************************
 * debug channel events
 */
EOLS    (debug_channel_event_sna_eol, debug_command, DEBUG_CHANNEL_EVENT_SNA);
EOLS	(debug_channel_event_eol, debug_command, DEBUG_CHANNEL);
KEYWORD (debug_channel_event_sna, debug_channel_event_sna_eol, 
         debug_channel_event_eol, "sna", "Channel CSNA/LLC2 events", PRIV_OPR);
KEYWORD	(debug_channel_event, debug_channel_event_sna, CHANNEL_NEXT_DEBUG,
	"events", "Channel events", PRIV_OPR);
#undef  CHANNEL_NEXT_DEBUG
#define CHANNEL_NEXT_DEBUG debug_channel_event
/******************************************************************
 * debug channel blocks
 */
EOLS	(debug_channel_blocks_eol, debug_command, DEBUG_CHANNEL_BLOCKS);
KEYWORD	(debug_channel_blocks, debug_channel_blocks_eol, CHANNEL_NEXT_DEBUG,
	"blocks", "Channel data blocks", PRIV_OPR);
#undef  CHANNEL_NEXT_DEBUG
#define CHANNEL_NEXT_DEBUG debug_channel_blocks
/******************************************************************
 * debug channel packets
 */
EOLS	(debug_channel_packet_eol, debug_command, DEBUG_CHANNEL_PKTS);
KEYWORD	(debug_channel_packet, debug_channel_packet_eol, CHANNEL_NEXT_DEBUG,
	"packets", "Channel packets", PRIV_OPR);

/******************************************************************
 * debug channel
 *
 */
KEYWORD_DEBUG(debug_channel, debug_channel_packet, ALTERNATE,
	      OBJ(pdb,1), channel_debug_arr,
	      "channel", "Channel interface information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_channel
