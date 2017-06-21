/* $Id: msg_x25.c,v 3.3.62.2 1996/09/14 00:10:19 hampton Exp $
 * $Source: /release/112/cvs/Xsys/x25/msg_x25.c,v $
 *------------------------------------------------------------------
 * msg_x25.c - Message file for x25 facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_x25.c,v $
 * Revision 3.3.62.2  1996/09/14  00:10:19  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/08/28  13:23:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  18:06:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:51:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:52:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  20:38:55  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:21:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(X25);
msgdef_section("X.25 error messages");


#ifdef X25_COMPLAIN_LEVEL
#undef X25_COMPLAIN_LEVEL
#endif
#define X25_COMPLAIN_LEVEL (5*ONESEC) /* No more than one per 5 sec */

msgdef(ILLP4, X25, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Interface %s, Illegal state %s when P4");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NOLCI, X25, LOG_ERR, 0,
       "Interface %s, Delete: lci %d not found in %s table");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NOTFINDBH, X25, LOG_ERR, 0,
       "Can't find bfe host address %i to delete");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(SPURD1, X25, LOG_ERR, 0,
       "Interface %s, Spurious D1 timer wakeup on LCI %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(INTIMEQ, X25, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Interface %s, LCN %d already in timer queue, new time %#Ta");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADCONFIG, X25, LOG_ERR, 0,
       "Interface %s, X.25 %s, current config. retained");
msgdef_explanation(
	"The interface's X.25 configuration is not valid; the existing
	configuration was not changed.");
msgdef_recommended_action(
	"Verify the following:

	The interface's switched virtual circuit ranges do not overlap; for
	nonzero values, lic <= hic < ltc <= htc < loc <= hoc. (For more
	information, refer to "Configuring Virtual Circuit Ranges" in the
	Router Products Configuration Guide.)

	If an incoming-only range is defined, check that it is numerically less
	than the two-way range.

	If an outgoing-only range is defined, check that it is numerically
	greater than the two-way range.

	No virtual circuit high/low parameter is zero unless its partner
	low/high parameter is also zero.

	The default window sizes are less than the interface modulo.");
msgdef_ddts_component("");


msgdef(PVCBAD, X25, LOG_ERR, 0,
       "Interface %s, PVC %d, %s");
msgdef_explanation(
	"A switched PVC cannot be connected. The final [chars] text string in
	the message represents the state of the PVC. These text strings can
	also appear in show x25-vc output. For more information, refer to the
	show x25-vc documentation in the Router Products Command Reference
	publication.");
msgdef_recommended_action(
	"Validate the configuration of the PVCs at both ends of the connection,
	and reconfigure them if necessary.");
msgdef_ddts_component("");


msgdef(XOTPROTOCOL, X25, LOG_ERR, 0,
       "XOT protocol error from %i: %s");
msgdef_explanation(
	"A protocol error was detected over an X.25-over-TCP (XOT) connection,
	causing the connected VC to close. A flaw may exist in the remote
	host's XOT implementation.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(VCLOSTSYNC, X25, LOG_WARNING, 0,
       "Interface %s, VC %d TCP connection %s");
msgdef_explanation(
	"An X.25 circuit that is being tunneled over a TCP connection between
	routers was cleared ungracefully. The last [chars] text string in the
	message can take one of two forms. If the text string is \"closed
	unexpectedly,\" the router at the remote end of the connection was
	rebooted, or the connection failed. If the text string is \"connection
	corrupted,\" it is likely that the connection was made to a router
	running an older software release.");
msgdef_recommended_action(
	"If the text string is \"closed unexpectedly,\" reestablish the
	connection after the remote router or intervening network is
	operational. If the text string is \"connection corrupted,\" upgrade
	the router at the other end of the connection to a more recent software
	release.");
msgdef_ddts_component("");


msgdef(BADUPCALL, X25, LOG_WARNING, 0,
       "Interface %s, Unexpected call (%s) from LLC2"
       " DTE=%02x%02x.%02x%02x.%02x%02x.");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(DEBUG_LCI, X25, LOG_WARNING, MSG_TRACEBACK,
       "LCI deleted at interrupt time");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(RANGEUNUSED, X25, LOG_WARNING, 0,
       "Interface %s, %s VC range unused; set limits to zero");
msgdef_explanation(
	"An outgoing-only or incoming-only virtual circuit range exactly
	overlaps the two-way virtual circuit range. This was the former method
	for specifying that a virtual circuit range was not used. The current
	method is to set the range limits to zero.");
msgdef_recommended_action(
	"Use the privileged copy running-config startup-config command to write
	the configuration to memory. This will store the modified (zero) range
	limits.");
msgdef_ddts_component("");


msgdef(LARGEPKSIZE, X25, LOG_WARNING, 0,
       "Interface %s, %s packet sizes %d/%d too large;"
       " lower values will be negotiated");
msgdef_explanation(
	"The interface's default packet sizes or outgoing packet size facility
	values are too large to be carried by Link Access Procedure, Balanced
	(LAPB).");
msgdef_recommended_action(
	"The router will negotiate lower packet size values automatically. The
	LAPB N1 value should be increased. For example, for an X.25 maximum
	packet size of 1024, modulo 8 X.25 operation will add 3 bytes of
	overhead and modulo 8 LAPB operation will add 4 bytes of overhead (the
	two bytes of CRC are defined to be included). This means that N1 must
	be at least 1031 bytes * 8 = 8248.");
msgdef_ddts_component("");


msgdef(PARTIALMAP, X25, LOG_WARNING, 0,
       "Interface %s, %s encapsulation call from %s partially mapped to %s");
msgdef_explanation(
	"An incoming X.25 call specified a protocol that was assigned to a
	multiprotocol map; the VC will not be able to carry the full set of
	protocols specified by the map.");
msgdef_recommended_action(
	"Modify the configuration of the router or the far host to match the
	encapsulation method used.");
msgdef_ddts_component("");


msgdef_limit(NOBUF, X25, LOG_NOTICE, 0, "Interface %s LCI %d %s",
	X25_COMPLAIN_LEVEL);
msgdef_explanation(
	"This is a warning only (the compressed packet was not lost). However,
	an X.25 packet layer RNR (wait) was generated and packet-level
	transmission on the LCN will not resume (RR issued) until a
	decompression buffer becomes available.");
msgdef_recommended_action(
	"If this message occurs frequently, you may need to modify the buffer
	pool. Copy the message exactly as it appears, and report it your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(PBPHOLDQ, X25, LOG_NOTICE, 0,
       "Interface %s LCI %d X25 Hold queue needs to be at least default size",
	X25_COMPLAIN_LEVEL);
msgdef_explanation(
	"An attempt was made to configure the X.25 hold queue to less than the
	minimum value.");
msgdef_recommended_action(
	"Configure the X.25 hold queue to be ten or more.");
msgdef_ddts_component("");


msgdef_limit(PBPDBIT, X25, LOG_NOTICE, 0,
       "Interface %s LCI %d PBP Compression does not support X25 D bit",
	X25_COMPLAIN_LEVEL);
msgdef_explanation(
	"Compression is not enabled because D-bit support is configured for the
	LCI.");
msgdef_recommended_action(
	"Reconfigure without D-bit support, or do not attempt to run with
	compression.");
msgdef_ddts_component("");


msgdef_limit(SYNCBAD, X25, LOG_ERR, 0,
       "Interface %s LCI %d Decompression sync error. expected %d got %d",
	X25_COMPLAIN_LEVEL);
msgdef_explanation(
	"An internal software error occurred. The PBP header has a sync field.
	The sync field (8 bits) is checked on each received packet.");
msgdef_recommended_action(
	"Contact your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(VERSIONBAD, X25, LOG_ERR, 0,
	"Interface %s LCI %d Compression Version mismatch "
		"expected %x received %x", X25_COMPLAIN_LEVEL);
msgdef_explanation(
	"An internal software error occurred. The PBP header has a mismatched
	version field. The version is checked on each received packet.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef_limit(SIZEBAD, X25, LOG_ERR, 0,
       "Interface %s LCI %d Decompression size error", X25_COMPLAIN_LEVEL);
msgdef_explanation(
	"An internal software error occurred. The PBP header has an
	uncompressed packet size field. Each uncompressed packet size is
	checked against this field.");
msgdef_recommended_action(
	"Contact your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(PBPNOEND, X25, LOG_NOTICE, 0,
	"Interface %s LCI %d End system not configured for X25 PBP",
	X25_COMPLAIN_LEVEL);
msgdef_explanation(
	"A compression or decompression operation was attempted by a router
	other than an end router. Only end routers can perform compression and
	decompression.");
msgdef_recommended_action(
	"Reconfigure the routers so that only the end routers in the network
	perform compression and decompression.");
msgdef_ddts_component("");


msgdef_limit(COMPERR, X25, LOG_ERR, 0,
	"Interface %s LCI %d low-level stac compression error",
	X25_COMPLAIN_LEVEL);
msgdef_explanation(
	"A Stacker compression algorithm internal error or shortage of buffer
	space was encountered during a compression operation. This should never
	happen.");
msgdef_recommended_action(
	"Report the error to your technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LCIBOGUS, X25, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
	" in x25_timer NULL lci_idb %x", X25_COMPLAIN_LEVEL);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef_limit(BADMBIT, X25, LOG_WARNING, 0,
       "Interface %s, VC %d, partial data packet has M-bit set", (10 * ONESEC));
msgdef_explanation(
	"This message occurs when a virtual circuit is set up using the default
	maximum packet sizes, but the DTE and DCE are not configured with the
	same default values. A packet was received that had the M-bit set,
	signifying that more data follows, but the packet did not carry the
	maximum number of bytes allowed. Such partial data packets must have
	the M-bit cleared. The resulting loss of a message boundary might cause
	problems for the data recipient.");
msgdef_recommended_action(
	"Verify the default flow control parameters of the DTE and the DCE. If
	this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef_limit(ADDRESSBAD, X25, LOG_ERR, 0,
	"Interface %s, x25 address %s subaddress %c is too long", 
	X25_COMPLAIN_LEVEL);
msgdef_explanation(
	"This message occurs when a subaddress is configured. When this
	subaddress is combined with the interface's X.121 address, the total
	address exceeds the limit of 15 characters.");
msgdef_recommended_action(
	"Reconfigure the subaddress so that this does not happen.");
msgdef_ddts_component("");
