/* $Id: msg_lane.c,v 3.2.56.4 1996/09/13 23:50:14 hampton Exp $
 * $Source: /release/112/cvs/Xsys/lane/msg_lane.c,v $
 *------------------------------------------------------------------
 * msg_lane.c -- LAN Emulation Message file
 *
 * October 1994, Brad Benson
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_lane.c,v $
 * Revision 3.2.56.4  1996/09/13  23:50:14  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.56.3  1996/08/28  12:55:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.56.2  1996/08/26  19:46:20  cakyol
 * CSCdi66421:  the well known LECS address needs to follow the master LECS
 * Branch: California_branch
 *
 * Revision 3.2.56.1  1996/05/09  14:33:27  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.84.1  1996/04/27  07:07:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.74.3  1996/04/16  02:45:23  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.74.2  1996/04/08  14:52:35  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.74.1  1996/04/08  01:57:39  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.62.1  1996/03/22  22:55:44  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.46.1  1996/03/05  06:43:49  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.34.1  1996/02/27  21:11:05  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.12.2  1996/02/06  22:19:50  cakyol
 * cleanup for the last code review:
 * - get rid of "#ifdef PARANOID...#endif".  Leave some in
 *   permanently, take some out completely.
 * - get rid of all "if 0....endif" type constructs.
 * - debug messages speeded up by using variable argument
 *   macro expansion.
 * - most fatal type errors are converted to crashdump with
 *   __FILE__ and __LINE__ printing.
 * - in the LECS, do *NOT* update the table of global LECS
 *   addresses, if any outstanding signalling requests remain.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.12.1.2.1  1996/03/23  21:14:20  kjadams
 * Branch: TRlane_branch
 * LES/BUS support for token ring LANE has been added.
 * TR LEC now has a new state for registration - to register the virtual
 *    ring number and bridge (route descriptor) with the LES.
 * BLLI code points for token ring have also been added for TR BUS
 *    connections.
 *
 * Revision 3.2.12.1  1996/01/11  01:47:01  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:07:15  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:51:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/07/24  04:55:54  nfinn
 * CSCdi37072:  LECS access code uses only the first LECS address
 * found via ILMI.  Also, sevel "show lane" and LANE error messages
 * need cleaning up.  A small memory leak occurred whenever LANE was
 * configured and the interface was destroyed.
 *
 * Revision 2.7  1995/07/13  21:39:54  cakyol
 * CSCdi37017:  LECS should advertise its coming up and going down
 * fixed.
 *
 * CSCdi37071:  lecs should migrate to "malloc_named"
 * fixed.
 *
 * Revision 2.6  1995/07/07  05:42:35  bbenson
 * CSCdi35012:  Server and Client should errmsg when operState goes down.
 * - Consolidated message to conform to the link and lineproto "UPDOWN"s.
 *   As Norm would say, slicker than...
 *
 * Revision 2.5  1995/06/30  22:02:23  bbenson
 * CSCdi36650:  LEC Join Failure error code could be more descriptive
 * - Removed coercion of mallocs in the bargain.
 *
 * Revision 2.4  1995/06/30  00:46:13  nfinn
 * CSCdi35012:  Server and Client should errmsg when operState goes down.
 *
 * Revision 2.3  1995/06/28  22:42:55  cakyol
 * CSCdi35007:  Server should toggle state upon change on elan name.
 * - done.
 *
 * Revision 2.2  1995/06/26  23:22:01  nfinn
 * CSCdi35235:  LANE config server access sequencing incorrect
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(LANE);				/* Define the LANE facility */
msgdef_section("LAN emulation error messages");


/*
 * Common LANE debug/event messages.
 */
msgdef(BADTYPE, LANE, LOG_ERR, 0, "LAN Emulation supports only Ethernet type");
msgdef_explanation(
	"The user specified a Token-Ring-emulated LAN, which is not currently
	supported.");
msgdef_recommended_action(
	"Specify an Ethernet-emulated LAN. Future software releases will
	support Token Ring ELANs.");
msgdef_ddts_component("");


msgdef(BADNAME, LANE, LOG_ERR, 0, "ELAN name must be 1-32 characters long");
msgdef_explanation(
	"An emulation LAN (ELAN) name was entered in a command that is longer
	than 32 characters. The LANE standard v1.0 requires a name that is 32
	characters or less in length.");
msgdef_recommended_action(
	"Re-enter a shorter name.");
msgdef_ddts_component("");


msgdef(UPDOWN, LANE, LOG_NOTICE, 0, "%s %s %s: %s changed state to %s");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LANE_ERROR, LANE, LOG_ERR, 0, "%s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


/*
 * LEC debug/event messages.
 */
msgdef(LEC_INFO, LANE, LOG_INFO, 0, "%s");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LEC_ERR, LANE, LOG_ERR, 0, "%s", 10*ONESEC);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LEC_CONTROL_MSG, LANE, LOG_ERR, 0, 
	     "Received bad control message on interface %s", 10*ONESEC);
msgdef_explanation(
	"The client received a control message over one of its control virtual
	channel connections (VCCs) that failed the header check.");
msgdef_recommended_action(
	"This message indicates a noninteroperable LANE configuration server,
	LANE server, or LANE client implementation on the emulated LAN.");
msgdef_ddts_component("");


msgdef(LEC_BADAD, LANE, LOG_ERR, 0, "%s LEC unable to compute ATM address");
msgdef_explanation(
	"The client cannot obtain an ATM address from the given configuration,
	or auto-assignment failed to get an ATM prefix from the switch.");
msgdef_recommended_action(
	"If you expect the switch to assign the prefix to the router, check
	that the Interim Local Management Interface (ILMI) is functional and
	the switch has a prefix configured. Otherwise, confirm that the client
	has an address in the NVRAM configuration.");
msgdef_ddts_component("");


msgdef_limit(LEC_SIGNAL, LANE, LOG_ERR, 0, 
	     "Unexpected SigAPI event %d", 10*ONESEC);
msgdef_explanation(
	"ATM signaling sent the client an unexpected message type.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LEC_LISTEN, LANE, LOG_ERR, 0, "%s listen failed on %s");
msgdef_explanation(
	"The client failed an attempt to listen on a particular ATM address.");
msgdef_recommended_action(
	"Reconfigure the client ATM address. It is conflicting with another
	component on the ATM Interface Processor (AIP) for the same ATM
	address.");
msgdef_ddts_component("");


msgdef(LEC_JOIN, LANE, LOG_ERR, 0, "%s JOIN failed, %s (LES returned %d)");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LEC_CFG, LANE, LOG_ERR, 0, "%s CFG_REQ failed, %s (LECS returned %d)");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LEC_BAD_CTL_DIST_TYPE, LANE, LOG_ERR, 0, "%s CTL DIST blli type wrong");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


/*
 * LES debug/event messages.
 */
msgdef(NAMECHG, LANE, LOG_ERR, 0, "A LANE Client is already running here using another elan name");
msgdef_explanation(
	"Both the LANE client and the LANE server/BUS may have an emulated LAN
	name specified in the configuration via the lane server-bus and lane
	client commands. If both specify the emulated LAN name, then the name
	cannot be changed by re-entering just one of the commands.");
msgdef_recommended_action(
	"Use the no lane server-bus command to delete the server-bus or the no
	lane client command to delete the client. Then, the other of the two
	commands can be re-entered with another emulated LAN name. Finally,
	re-enter the deleted command with the new emulated LAN name.");
msgdef_ddts_component("");


msgdef(TYPECHG, LANE, LOG_ERR, 0, "A LANE Client is already running here using another elan type");
msgdef_explanation(
	"Both the LANE client and the LANE Server/BUS may have an ELAN type
	specified in the configuration via the lane server-bus and lane client
	commands as either Ethernet or Token Ring. If both, specify the type;
	the type cannot be changed by re-entering just one of the commands.");
msgdef_recommended_action(
	"Use the no lane server-bus command to delete the server-bus or the no
	lane client command to delete the client. Then, the other of the two
	commands can be re-entered with a new type. Finally, re-enter the
	deleted command with the new type.");
msgdef_ddts_component("");


msgdef_limit(LSV_LECID, LANE, LOG_WARNING, 0,
             "%s elan %s LECIDs exhausted", 10*ONESEC);
msgdef_explanation(
	"The LANE server specified has 65279 clients. No more can be added.");
msgdef_recommended_action(
	"Change the configurations to avoid trying to have more than 65279 LANE
	clients on this server.");
msgdef_ddts_component("");


msgdef(LSV_DUPID, LANE, LOG_ERR, 0, "%s elan %s duplicate LECID %d");
msgdef_explanation(
	"An attempt was made to define two different server-direct PVCs both on
	the same subinterface (same LANE server) and both for the same client
	(LECID).");
msgdef_recommended_action(
	"Delete one of the lane pvc commands that has this duplicated LECID.");
msgdef_ddts_component("");


msgdef(LSV_CRASH, LANE, LOG_ERR, MSG_TRACEBACK,
       "%s elan %s unrecoverable error %s");
msgdef_explanation(
	"An internal error occurred.");
msgdef_recommended_action(
	"This error should not occur. Download the system error log, then call
	your technical support representative and provide the log, the error
	message, and the router configuration.");
msgdef_ddts_component("");


msgdef(LSV_ADDR, LANE, LOG_ERR, 0,
       "%s elan %s unable to register LES/BUS ATM addr");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LSV_BADAD, LANE, LOG_ERR, 0,
       "%s elan %s unable to compute %s ATM address");
msgdef_explanation(
	"Software was unable to compute an ATM address based on the ATM address
	template given. The characters in chars (see above display) will be
	either "BUS" (broadcast-and-unknown), "server", or "config server" in
	the actual error message.");
msgdef_recommended_action(
	"Enter a correct ATM address template for the LANE component ("BUS",
	"server", or "config server") named in the error message."); 
msgdef_ddts_component("");


msgdef(LSV_MCAST, LANE, LOG_ERR, 0,
       "%s elan %s unable to create %s MC group");
msgdef_explanation(
	"An internal error occurred.");
msgdef_recommended_action(
	"This error should not occur. Download the system error log, then call
	your technical support representative and provide the log, the error
	message, and the router configuration.");
msgdef_ddts_component("");


msgdef(NOVCD, LANE, LOG_ERR, 0, "VCDs exhausted");
msgdef_explanation(
	"When trying to create PVC 0/17, the router ran out of PVC
	resources.");
msgdef_recommended_action(
	"The number of PVCs can be increased with the atm maxvc command.");
msgdef_ddts_component("");


msgdef(LSV_DUPMAC, LANE, LOG_ERR, 0,
       "%s elan %s client %d duplicate MAC address registration %e");
msgdef_explanation(
	"An attempt was made by the specified LANE client to register a MAC
	address and ATM address pair that duplicated a MAC address already
	registered to another LANE client.");
msgdef_recommended_action(
	"Use the show lane server command to list the registered addresses in
	the LANE server reporting the error. Correct one of the two LANE
	clients that is trying in error to use the same MAC address.");
msgdef_ddts_component("");


msgdef(LSV_DUPATM, LANE, LOG_ERR, 0,
       "%s elan %s client %d duplicate ATM address registration %s");
msgdef_explanation(
	"An attempt was made by the specified LANE client to register a MAC
	address and ATM address pair that duplicated an ATM address already
	registered to another LANE client.");
msgdef_recommended_action(
	"Use the show lane server command to list the registered addresses in
	the LANE server reporting the error. Correct one of the two LANE
	clients that is trying in error to use the same ATM address.");
msgdef_ddts_component("");


msgdef(LSV_DUPRD, LANE, LOG_ERR, 0,
       "%s elan %s client %d duplicate route descriptor registration %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(LSV_NOREG, LANE, LOG_ERR, 0,
       "%s elan %s client %d unable to register MAC address %e");
msgdef_explanation(
	"An internal error occurred.");
msgdef_recommended_action(
	"This error should not occur. Download the system error log, then call
	your technical support representative and provide the log, the error
	message, and the router configuration.");
msgdef_ddts_component("");


msgdef(NOINT, LANE, LOG_ERR, 0, "%s still destroying old sub-interface");
msgdef_explanation(
	"An attempt was made to create a new subinterface while a previous
	version of the subinterface was still being destroyed.");
msgdef_recommended_action(
	"Wait a minute and try again. If failure still occurs, try shutting
	down the main interface. Rebooting may be required to recover in
	extreme cases.");
msgdef_ddts_component("");


msgdef(NOILMI, LANE, LOG_ERR, MSG_TRACEBACK,
       "ILMI service registration failed");
msgdef_explanation(
	"The Interim Local Management Interface (ILMI) feature is not included
	in the software image running on the router.");
msgdef_recommended_action(
	"Reload router with software that includes the ILMI feature.");
msgdef_ddts_component("");


msgdef(BADILMI, LANE, LOG_ERR, MSG_TRACEBACK, "Unexpected ILMI event %d");
msgdef_explanation(
	"An internal error occurred.");
msgdef_recommended_action(
	"This error should not occur. Download the system error log, then call
	your technical support representative and provide the log, the error
	message, and the router configuration.");
msgdef_ddts_component("");


msgdef(REGSTATE, LANE, LOG_ERR, MSG_TRACEBACK, "ILMI event in state %d");
msgdef_explanation(
	"An internal error occurred.");
msgdef_recommended_action(
	"This error should not occur. Download the system error log, then call
	your technical support representative and provide the log, the error
	message, and the router configuration.");
msgdef_ddts_component("");


msgdef(NOREGILMI, LANE, LOG_ERR, 0, "%s %s cannot register %Cj with ILMI");
msgdef_explanation(
	"An attempt to register the given ATM address with the ATM switch
	failed. This failure can occur because the ATM switch is down or
	misconfigured. The registration will be retried.");
msgdef_recommended_action(
	"If the situation persists after a number of attempts, and the switch
	is up, the configuration of the switch and router must be examined. The
	most likely cause is that ATM address being registered is not
	compatible with the parameters - for example, the ATM address prefix
	- configured in the switch. Correct the configuration.");
msgdef_ddts_component("");


msgdef(NEWPREF, LANE, LOG_ERR, 0, "%s elan %s ATM address prefix %s");
msgdef_explanation(
	"Entering a new ATM address template for some LANE component caused the
	specified LANE server or bus to drop all clients and restart.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(LES_ERROR, LANE, LOG_ERR, 0, "%s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(LSV_BADSEG, LANE, LOG_ERR, 0,
       "TokenRing ELAN segment id must be 1-4096");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


/*
 * LECS debug/event messages.
 */
msgdef(LECS_ERROR, LANE, LOG_ERR, 0, "%s");
msgdef_explanation(
	"An internal consistency error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LECS_WARNING, LANE, LOG_WARNING, 0, "%s");
msgdef_explanation(
	"The LANE Configuration Server address could not be registered with or
	de-registered from the switch, using Interim Local Management Interface
	(ILMI).

	Possible causes:

	The switch prefix is not available.

	The address is not acceptable to the switch (the prefix does not
	conform).

	The interface over which this operation should take place is down.

	The ILMI subsystem on the router is not completely initialized.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LECS_INFO, LANE, LOG_INFO, 0, "%s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


/*
 * LANE signaling debug/event messages
 */
msgdef(LANE_SIG_ERR, LANE, LOG_ERR, MSG_TRACEBACK, "%s");
msgdef_explanation(
	"An internal error occurred while system routines were handling ATM
	signaling messages for LANE.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LANE_SIG_CHAIN, LANE, LOG_ERR, MSG_TRACEBACK,
       "not in %s chain (%x %x)");
msgdef_explanation(
	"An internal error occurred while system routines were handling ATM
	signaling messages for LANE.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef(LANE_SIG_INV_MSG, LANE, LOG_ERR, 0,
       "invalid msg received (%x %x %x)");
msgdef_explanation(
	"An internal error occurred while system routines were handling ATM
	signaling messages for LANE.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef_limit(LANE_SIG_INV_SDU, LANE, LOG_ERR, 0,
	     "invalid SDU received in  SETUP"
	     "\nCall from: ATM NSAP - %Ch"
	     "\nreceived forward SDU size is %d"
	     "\nreceived backward SDU size is %d" 
	     "\nSDU size on the interface %s is %d"
	     "\nCall Rejected\n", MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"Incoming SDU size did not match the SDU size of the client.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LANE_SIG_INV_STATE, LANE, LOG_ERR, MSG_TRACEBACK,
       "invalid %s req in state %d (%x)");
msgdef_explanation(
	"An internal error occurred while system routines were handling ATM
	signaling messages for LANE.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

/*******************************************************************
**
**		more serious error reporting  support
**
*******************************************************************/

/* more serious errors/warnings */
extern void lane_error (const message *msg, const char *fmt, ...);

/* lecs info output */
#define LECS_INFO(fmt, args...)		\
    lane_error(&msgsym(LECS_INFO, LANE), fmt, ## args)

/* lecs warning */
#define LECS_WARN(fmt, args...)		\
    lane_error(&msgsym(LECS_WARNING, LANE), fmt, ## args)

/* lecs errors */
#define LECS_ERR(fmt, args...)		\
    lane_error(&msgsym(LECS_ERROR, LANE), fmt, ## args)

/* les errors */
#define LES_ERR(fmt, args...)		\
    lane_error(&msgsym(LES_ERROR, LANE), fmt, ## args)

/* general lane error */
#define LANE_ERR(fmt, args...)		\
    lane_error(&msgsym(LANE_ERROR, LANE), fmt, ## args)
    
