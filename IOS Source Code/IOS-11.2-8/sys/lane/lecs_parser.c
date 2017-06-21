/* $Id: lecs_parser.c,v 3.5.18.12 1996/09/11 17:28:07 cakyol Exp $
 * $Source: /release/112/cvs/Xsys/lane/lecs_parser.c,v $
 *------------------------------------------------------------------
 * LECS parser routines
 *
 * November, 1994  Cihangir Akyol(Gee)
 *
 * Copyright (c) 1994-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lecs_parser.c,v $
 * Revision 3.5.18.12  1996/09/11  17:28:07  cakyol
 * CSCdi68348:  The LECS needs to LOCALLY maintain the calling party
 * address of conn
 * Branch: California_branch
 *
 * Revision 3.5.18.11  1996/08/28  12:55:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.18.10  1996/08/26  19:46:17  cakyol
 * CSCdi66421:  the well known LECS address needs to follow the master LECS
 * Branch: California_branch
 *
 * Revision 3.5.18.9  1996/06/29  06:34:01  cakyol
 * CSCdi61448:  Router crashes after changing master LECS addr in ls1010
 * table
 * Change the printf strategy in "lecs_interface_show".  The problem
 * arises when performing a "show lane config" at the same instant
 * that a connection obkject is being deleted.
 * Branch: California_branch
 *
 * Revision 3.5.18.8  1996/06/21  20:55:13  cakyol
 * CSCdi60981:  more than one LECS address for the same LECS breaks
 * redundancy
 * Branch: California_branch
 *
 * Revision 3.5.18.7  1996/05/31  23:24:15  cakyol
 * CSCdi58685:  router crashed when LS100 system call errors occured.
 *              this really had nothing to do with the atm switch.
 *              it was all a coincidence.  forgot locking the elements
 *              while printing them.  this is a much better fix.
 * Branch: California_branch
 *
 * Revision 3.5.18.6  1996/05/29  01:05:46  cakyol
 * CSCdi58893:  an unsigned-signed comparison is NOT caught in
 * lecs_parser.c.
 * Branch: California_branch
 *
 * Revision 3.5.18.5  1996/05/25  09:48:24  cakyol
 * CSCdi58685:  router crashed when LS100 system call errors occured.
 *              forgot to lock/unlock connection objects while
 *              printing them out.
 * Branch: California_branch
 *
 * Revision 3.5.18.4  1996/05/17  21:31:41  cakyol
 * CSCdi58042:  print out a proper error string for last LECS config
 * failure
 * Branch: California_branch
 *
 * Revision 3.5.18.3  1996/05/09  14:33:21  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.18.2.2.1  1996/04/27  07:07:10  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.18.2  1996/04/25  23:15:10  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.5.18.1.8.13  1996/04/24  18:47:31  cakyol
 * show the local-seg-id only ONCE per elan during
 * the "show lane database" command.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.12  1996/04/17  01:27:11  cakyol
 * - indicate so if an LEC is connected.
 * - white space cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.11  1996/04/16  02:45:14  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.10  1996/04/11  18:20:15  cakyol
 * amendments to the latest sync:
 * - take some unnecessary crashdump's out of lane server.
 * - correct possible input buffer loss at LECS input processing
 *   for token ring lan type.
 * - debugging messages needed tidying up.
 * - some but not all white space cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.9  1996/04/11  05:34:49  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.5.18.1.8.8  1996/04/10  23:47:58  cakyol
 * change misleading variable name
 * from "global_boottime_config_version"
 * to "global_parsed_swversion".
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.7  1996/04/10  21:27:16  cakyol
 * make backward compatible LECS command work when
 * configured from the network (via "config net")
 * as well.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.6  1996/04/09  00:52:05  cakyol
 * oops, my mistake, pull the "ushort"s into the "if 0" section.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.5  1996/04/09  00:05:47  cakyol
 * eliminate another rdIndex access (not initted).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.4  1996/04/08  21:33:52  cakyol
 * correct sync problems:
 * - more unprotected buginfs.
 * - do NOT access uninitialized index "rdIndex".
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.3  1996/04/08  20:47:06  cakyol
 * sync corrections:
 * - value "LANE_LECS_BIND_NSAP" should not be there.
 * - parser user errors should emit a "%" as the first character.
 * - white space cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.2  1996/04/08  14:52:29  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.18.1.8.1  1996/04/08  01:57:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.18.1  1996/03/18  20:33:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.22.4  1996/04/05  03:20:50  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_ATMNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_ATMNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.22.3  1996/03/27  22:58:12  cakyol
 * Added some more 11.2 backward compatibility code that I missed.
 * This involves the "name NAME server-atm-address ADDRESS" lane
 * database command.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.22.2  1996/03/25  02:20:10  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.22.1  1996/03/22  22:55:37  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.2.5  1996/03/22  05:25:38  cakyol
 * Found a better way of determining the version number of the
 * config file at boot time.  Leverage from parsing of the
 * "version xx.yy" command.  Define a new variable which
 * represents this value named "global_boottime_config_version".
 * Restore all the other files back to what they were before this.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.2.4  1996/03/20  23:48:28  cakyol
 * The 11.2 LECS parser can now FULLY recognize pre 11.2 LECS
 * commands .  As a result of this, we also have a new systemwide
 * global uint variable named "global_nvconfig_version", which
 * reflects the nvram version number in it.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.2.3  1996/03/19  20:47:31  cakyol
 * make "lane config ...." command backward compatible with
 * the older versions.  So, now we accept both forms of
 * "lane config [database] NAME" but we only nvgen the new one,
 * "lane config database NAME".  As a result of this however, now
 * one can NOT use a NAME which matches with any of the keywords
 * "database", "auto-config-atm-address", "fixed-config-atm-address"
 * and "config-atm-address" in any beginning character positions.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.2.2  1996/02/29  22:16:13  cakyol
 * correct some slight merge errors and clean up stuff related
 * to config access, which somehow must not have been totally
 * cleaned before.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.2.1  1996/02/27  21:10:59  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/02/07  16:13:42  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/01/19  02:46:20  cakyol
 * CSCdi47155:  Removing a LANE database while editing it can crash router
 * - fixed.  a simple locking mechanism now takes care of it.
 *
 * Revision 3.3.14.9  1996/02/22  09:05:41  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.8  1996/02/08  00:04:18  cakyol
 * save a lot of string space by assigning __FILE__ to a
 * static char* at the beginning of the file and use that
 * throughout instead.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.7  1996/02/06  22:19:42  cakyol
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
 * Revision 3.3.14.6  1996/02/02  02:16:32  cakyol
 * - much more robust procesing of calling party address on
 *   the LECS now.  If a call from an already existing caller
 *   is received, the old one is maintained, the new one is
 *   rejected.  Before, the new one bumped the old one off.
 * - More efficient lecs finder:
 *         - if noone wants to know the master LECS address,
 *           this step is completely eliminated saving cycles.
 *         - as soon as the global LECS addresses are determined,
 *           the users awaiting only for those are responded to
 *           immediately.  Before, they had to wait until the
 *           master LECS was determined as well.  This could take
 *           a long time if signalling was slow.
 *         - safety timer to protect against ILMI hangups is
 *           added for debugging only.
 *         - the "#ifdef TESTING" section is now taken out.
 * - long printf string cleanup done, concatenation used now.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.5  1996/01/30  23:56:34  cakyol
 * - last touches on the 3 additional atm sig api
 *   routines as recommended by Schrupp.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.4  1996/01/29  22:06:23  cakyol
 * - as per the signalling group's request, eliminate the
 *   direct invasion of their private data structures by
 *   using:
 *     atmSig_api_callId2CallingPartyAddress   and
 *     atmSig_api_sameCallingPartyAddress
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.3  1996/01/26  02:02:53  cakyol
 * more code review mods (still more to come):
 *
 * - "show lane config" now shows all the details of the
 *   connected LESs, such as elan name, relative priority
 *   and whether active or not.
 * - in the lecs finder, protect against getnext going past
 *   the end of the lecs addresses table, by checking the
 *   incoming oid every time.
 * - in the lecs finder, add a parameter to the user interface
 *   limiting the number of lecs addresses the caller is
 *   interested in.
 * - use a static global buffer for debugging messages; saves
 *   stack space.
 * - make "les-timeout" and "lane global-lecs-address" commands
 *   hidden.
 * - prepend "atmSig_" to the 2 new atm signalling api function
 *   names.
 * - prepend "lsv_" to some LES functions for consistency.
 * - more white space, if...else formatting cleanup in some
 *   of the files.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.2.2.1  1996/03/19  00:32:24  kjadams
 * Branch: TRlane_branch
 * Add support for new config commands for TR LANE
 * 
 * Revision 3.3.14.2  1996/01/19  19:37:08  cakyol
 * - lock a config table to make it accessible by only a
 *   single session at a time (this was bug CSCdi47155
 *   in 11.0 and 11.1)
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.1  1996/01/11  01:46:56  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.10.2  1996/01/03  22:53:01  cakyol
 * - divided up debug categories to: all, events and packets
 *   for the LECS.  There were too many debugging messages
 *   for a single debug flag.
 *
 * - updated some of the outdated comments.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3.10.1  1995/12/29  01:07:10  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/17  17:51:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:51:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:23:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/08/30  02:34:14  bbenson
 * CSCdi37992:  LANE packet count can go negative
 * - Change appropriate format chars from d to u.  AKA gratuitous commit
 *   to impress the boss.
 *
 * Revision 2.13  1995/07/13  21:39:51  cakyol
 * CSCdi37017:  LECS should advertise its coming up and going down
 * fixed.
 *
 * CSCdi37071:  lecs should migrate to "malloc_named"
 * fixed.
 *
 * Revision 2.12  1995/07/13  01:07:14  cakyol
 * CSCdi37016:  lecs access policy should allow for restricted elans
 * - done.  The main point is that restricted elans cannot now be
 *   joined by name only.  Explicit atm address or mac address
 *   is required.
 *
 * Revision 2.11  1995/07/11  00:08:34  cakyol
 * CSCdi36888:  lecs should display admin state
 * - lecs now displays the admin state
 * - lecs uses "waitingIlmi" instead of "ilmi-"
 * - also uses "waitingListen" instead of "sig-"
 * - main lecs process name is now "LANE LECS Server"
 * - process name for config access is now "LANE CFG Access"
 *
 * Revision 2.10  1995/07/07  22:59:19  cakyol
 * CSCdi36829:  lecs configured AFTER boot doesnt register with ilmi
 * - fixed.  Note down the fact that a prefix is received whether
 *   an lecs is configured or not on an interface.
 *
 * Revision 2.9  1995/06/29  23:15:42  cakyol
 * CSCdi36582:  lecs should register with ilmi at init stage
 * - major cleanup occured.  The lecs now registers with
 *   ilmi at init time.  Also, lecs does not attempt to
 *   register with the ilmi until at least one prefix add
 *   message from the ilmi is seen.  Lastly, if any registration
 *   or deregistration fails, the LECS will only bitch once
 *   and shut up for good afterwards.  So, it is now the
 *   user's responsibility to make sure to record offending
 *   addresses.
 *
 * Revision 2.8  1995/06/28  22:33:15  cakyol
 * CSCdi36542:  add a label client-atm-address to the LEC address in lane
 * database
 * done. Added "client-mac-address" and "clkient-atm-address" in front
 * of appropriate outputs.
 *
 * Revision 2.7  1995/06/20  18:13:14  cakyol
 * CSCdi35998:  no error while configuring LEC on subif missing LECS NSAP
 * fixed
 *
 * Revision 2.6  1995/06/16  21:59:08  cakyol
 * CSCdi36001:  show lane shows lecs packet counts even tho no LECS exists
 * on interf
 * - fixed.
 *
 * Revision 2.5  1995/06/16  01:09:31  cakyol
 * CSCdi35938:  lecs should show as down if none of its adrrs are
 * registered
 * - fixed.  To be operational now, at least one nsap address
 *   must show as fully registered both with ilmi & signalling
 *
 * Revision 2.4  1995/06/14  23:55:03  cakyol
 * CSCdi35849:  LECS shold reject names > 32 chars
 * - fixed
 *
 * Revision 2.3  1995/06/13  22:37:39  cakyol
 * CSCdi35793:  <show lane config> should show counts if no active vcs
 * - fixed.  Now, they are suppressed only if "brief" is entered.
 *
 * Revision 2.2  1995/06/08  20:41:52  cakyol
 * CSCdi35435:  a quick shut- no shut sequence may lock an addr
 * unregistered
 * fixed by ensuring that at every down->up transition, mark all relevant
 * addresses as unregistered with ILMI, so that they get re-registered
 * when coming up finishes
 *
 * CSCdi35366:
 * also fixed this one.
 *
 * Revision 2.1  1995/06/07  21:20:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

static char *ThisFileName = __FILE__;

#include "lecs.h"
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "parser.h"

/* 
** check length of names
*/
static boolean invalidNameLength (char *name)
{
    int len;

    if (name == NULL) return TRUE;
    len = strlen(name);
    if ((len <= 0) || (len >= NAMESIZE))
    {
	USER_ERROR("%% name should be 1 - 32 characters in length\n");
	return TRUE;
    }
    return FALSE;
}

/*
** write the config tables out exactly in the format
** that the parser would understand.  This is part
** of the nvgens creation.  The "createString" is
** the first parser string to create the config table
** (csb->nv_command)
*/
static void ConfigTables_regenerate (char *createString)
{
    ConfigTable *pCfg;
    ElanInfoBlock *pElan;
    LecInfoBlock *pLec;
    IndexPtr ind;
    int i, count;
    ServerAtmAddress *psaa;

    if ((createString == NULL) || (createString [0] == 0)) 
	LECS_CRASH();
    pCfg = List_head(G_ConfigTablesList);
    while (pCfg != NULL)
    {

    /* re-create the config table */
	nv_write(TRUE, "!\n%s %s", createString, pCfg->tableName);

    /* re-create the elans */
	pElan = List_head(pCfg->elanList);
	while (pElan != NULL)
	{
	    /* write out all the server atm addresses */
	    psaa = List_head(pElan->lesAddressesList);
	    while (psaa != NULL)
	    {
		nv_write(TRUE, 
		    "  name %s server-atm-address %Ch", 
		    pElan->elanName, &psaa->lesAddr);
		psaa = List_next(psaa);
	    }

	    /* write out the local segment id if one is set */
	    if ((pElan->local_segment_id > 0) &&
	       (pElan->local_segment_id <= LANE_MAX_SEGMENTID))
		   nv_write(TRUE, "  %s %s %s %d",
			"name", pElan->elanName,
			"local-seg-id", pElan->local_segment_id);
 
	    /* write out restricted status if needed */
	    if (pElan->restricted)
		nv_write(TRUE, "  name %s restricted", 
		    pElan->elanName);

	    /* write out les timeout if needed */
	    if (pElan->lesTimeout != LES_TIMEOUT_DFLT)
		nv_write(TRUE, "  name %s les-timeout %d",
		    pElan->elanName, pElan->lesTimeout);

	    /* next elan */
	    pElan = List_next(pElan);
	}

    /* set the default elan */
	if (pCfg->defaultElan)
	    nv_write(TRUE, "  %s %s", 
		"default-name", pCfg->defaultElan->elanName);

    /* add UNMASKED nsap addressed LEC's to elans */
	ind = pCfg->UNmaskedAtmAddressIndex;
	count = Index_numberOfElements(ind);
	for (i=0; i<count; i++)
	{
	    pLec = Index_getElementPointer(ind, i);
	    if (pLec)
	    {
		nv_write(TRUE, "    %s %Cw %s %s",
		    "client-atm-address", &pLec->address, 
		    "name", pLec->parentElan->elanName);
	    }
	    else
		break;
	}

    /* add MASKED nsap addressed LEC's to elans */
	ind = pCfg->maskedAtmAddressIndex;
	count = Index_numberOfElements(ind);
	for (i=0; i<count; i++)
	{
	    pLec = Index_getElementPointer(ind, i);
	    if (pLec)
	    {
		nv_write(TRUE, "    %s %Cw %s %s",
		    "client-atm-address", &pLec->address, 
		    "name", pLec->parentElan->elanName);
	    }
	    else
		break;
	}

    /* add mac addressed LEC's to elans */
	ind = pCfg->macAddressIndex;
	count = Index_numberOfElements(ind);
	for (i=0; i<count; i++)
	{
	    pLec = Index_getElementPointer(ind, i);
	    if (pLec)
	    {
		nv_write(TRUE, "    %s %Cw %s %s",
		    "mac-address", &pLec->address, 
		    "name", pLec->parentElan->elanName);
	    }
	    else
		break;
	}

    /* add route descriptor LEC's to elans */
	ind = pCfg->rdIndex;
	count = Index_numberOfElements(ind);
	for (i=0; i< count; i++)
	{
	    pLec = Index_getElementPointer(ind, i);
	    if (pLec)
	    {
		ushort segment;
		ushort bridge;

		segment = ((ushort)(pLec->address.dataBytes[0])) << 4;
		segment |= ((pLec->address.dataBytes[1] & 0xF0) >> 4);
		bridge = (ushort)(pLec->address.dataBytes[1] & 0x0F);
		nv_write(TRUE, "    %s %d %s %d %s %s", "route-desc",
		    segment, "bridge", bridge, "name",
		    pLec->parentElan->elanName);
	    }
	    else
		break;
	}

    /* next config table */
	pCfg = List_next(pCfg);
    }
}

/*
** debug handler
*/
void lecs_exec_debug_all (parseinfo *csb)
{ lecs_debug_all(csb->sense); }

/*
** exit signal handler for database manipulation.
**
** this is used for dead telnet sessions to make sure
** that a database is not left around locked.  The
** process id that is exiting must be matched to the
** database table, and the table unlocked.
*/
static void lane_db_exit_handler (int signal, int subcode,
    void *info, char *addr)
{
    pid_t pid;
    ConfigTable *pCfg;

    if (lane_lecs_debug)
	buginf("\nLECS: lane_db_exit_handler activated");

    if (!process_get_pid(&pid)) {
	LECS_ERR("lane_db_exit_handler: process_get_pid failed");
	return;
    }

    pCfg = List_head(G_ConfigTablesList);
    while (pCfg) {
	if ((pCfg->pid == pid) && pCfg->locked) {
	    pCfg->pid = NO_PROCESS;
	    pCfg->locked = FALSE;
	    break;
	}
	pCfg = List_next(pCfg);
    }
}

/*
** needed for parser context change: exiting
*/
void *cfgtable_context_exit (parseinfo *csb)
{
    ConfigTable *pCfg = csb->laneLecsCfgTbl;

    csb->laneLecsCfgTbl = NULL;
    if (pCfg) {
	pCfg->locked = FALSE;
	pCfg->pid = NO_PROCESS;

	/* restore the original exit handler */
	signal_oneshot(SIGEXIT, pCfg->dflt_exit_handler);
    }
    return pCfg;
}

/*
** needed for parser context change: entering
*/
void cfgtable_context_enter (parseinfo *csb, void *var)
{
    ConfigTable *pCfg = var;

    csb->laneLecsCfgTbl = pCfg;
    if (!pCfg) return;
    pCfg->locked = TRUE;
    if (!process_get_pid(&pCfg->pid)) {
	LECS_ERR("cfgtable_context_enter: process_get_pid failed");
    }

    /*
    ** remember the original exit handler 
    ** while installing the new one
    */
    pCfg->dflt_exit_handler = signal_oneshot(SIGEXIT, 
	lane_db_exit_handler);
}

/*****************************************************************
**
** lane lecs config table manipulation commands
** This routine handles the global config commands:
** 
** [no] lane database TABLE_NAME
**
**	subcommands of the above mode:
**
**	name ELAN-NAME server-atm-address NSAP-ADDRESS \
**		[restricted] [index PRIORITY]
**	name ELAN-NAME ( restricted | un-restricted )
**	name ELAN-NAME new-name NEW-ELAN-NAME
**	name ELAN-NAME les-timeout TIMEOUT
**	name ELAN-NAME local-seg-id SEGMENT-NUMBER
**
**	no name ELAN-NAME [local-seg-id SEGMENT-NUMBER]
**	no server-atm-address NSAP-ADDRESS
**
**	[no] mac-address MAC-ADDRESS name ELAN-NAME
**	[no] client-atm-address NSAP-ADDRESS[/NSAP-MASK] \
**		name ELAN-NAME
**	[no] route-descriptor ROUTE-DESCRIPTOR name ELAN-NAME
**	[no] default-name ELAN-NAME
**	display-databases
*/
void lecs_cfgtbl_function (parseinfo *csb)
{
    char *name, *newName;
    ConfigTable *pCfg;
    MaskedAddress masked, *maskedPtr;
    hwaddrtype *hwaddr;
    int priority, timeout;
    ElanRestrictedStatus elanStatus;
    ElanInfoBlock *pElan;
    ushort   route_desc;
    ushort   segment_id;
    ushort   bridge_num;

    /* nvgen stuff */
    if (csb->nvgen)
    {
	switch (csb->which)
	{
	    /* create and populate all config tables */
	    case LANE_LECS_CMD_CFGTBL:
		ConfigTables_regenerate(csb->nv_command);
		break;

	    default:
		bad_parser_subcommand(csb, csb->which);
		break;
	}
	return;
    }

    /* NON nvgen stuff */
    switch (csb->which)
    {
	/* main config table command */
	case LANE_LECS_CMD_CFGTBL:

	    name = GETOBJ(string, 1);
	    if (invalidNameLength(name)) return;
	    pCfg = getConfigTableByName(name);

	    /* create/select a config table */
	    if (csb->sense)
	    {
		if (pCfg == NULL)
		{
		    pCfg = ConfigTable_alloc(name);
		    if (pCfg == NULL)
		    {
			LECS_ERR("lecs_cfgtbl_function: "
			    "ConfigTable_alloc failed");
			return;
		    }
		    PRINTF("lecs config table '%s' created\n", name);
		}

		/* if table already in use, too bad */
		if (pCfg->locked)
		{
		    USER_ERROR("%% config table '%s' is "
			"currently in use", name);
		    return;
		}

		/*
		** change the mode to the config table editing mode
		** and if the change succeeds, store the context.
		*/
		if (set_mode_byname(&csb->mode, "lane", MODE_VERBOSE))
		    cfgtable_context_enter(csb, pCfg);
	    }
	    else
	    {
		/* delete a config table */
		if (pCfg == NULL)
		    USER_ERROR("%% config table '%s' does NOT exist",
			name);
		else
		{
		    if (pCfg->locked)
		    {
			USER_ERROR("%% config table '%s' is in use",
			    name);
			return;
		    }
		    ConfigTable_destroyByPtr(pCfg);
		    PRINTF("config table '%s' is deleted\n", name);
		}
	    }
	    break;

	/* create/modify/delete an elan */
	case LANE_LECS_CMD_ELAN_CREATE:

	    /* these are all the needed parameters */
	    pCfg = (ConfigTable*) csb->laneLecsCfgTbl;
	    name = GETOBJ(string, 1);
	    if (invalidNameLength(name)) return;
	    hwaddr = GETOBJ(hwaddr, 1);
	    priority = GETOBJ(int, 2);
	    elanStatus = GETOBJ(int, 1);
	    ConfigTable_elanCreate
		(pCfg, name, hwaddr, priority, elanStatus);
	    break;

	case LANE_LECS_CMD_ELAN_SET_TIMEOUT:

	    pCfg = (ConfigTable*) csb->laneLecsCfgTbl;
	    name = GETOBJ(string, 1);
	    if (invalidNameLength(name)) return;
	    timeout = GETOBJ(int, 1);
	    elanSetLesTimeout(pCfg, name, timeout);
	    break;
	
	/* rename an elan */
	case LANE_LECS_CMD_ELAN_RENAME:

	    pCfg = (ConfigTable*) csb->laneLecsCfgTbl;
	    name = GETOBJ(string, 1);
	    if (invalidNameLength(name)) return;
	    newName = GETOBJ(string, 2);
	    if (invalidNameLength(newName)) return;
	    renameElan(pCfg, name, newName);
	    break;

	/* set elan restricted status of an elan */
	case LANE_LECS_CMD_ELAN_SETR:

	    pCfg = (ConfigTable*) csb->laneLecsCfgTbl;
	    name = GETOBJ(string, 1);
	    if (invalidNameLength(name)) return;
	    elanStatus = GETOBJ(int, 1);
	    setElanRestrictedStatus(pCfg, name, NULL, elanStatus);
	    break;

	/* set/clear local segment id */
        case LANE_LECS_CMD_ELAN_SET_SEGMENT:

            pCfg = (ConfigTable*) csb->laneLecsCfgTbl;
            name = GETOBJ(string, 1);
            if (invalidNameLength(name)) return;
            pElan = getElanByName(pCfg, name, strlen(name));
            if (pElan != NULL)
	    {
		if (csb->sense)
		{
		    pElan->local_segment_id = GETOBJ(int, 1);
		}
		else
		{
		    PRINTF("removing segment id %d from elan '%s'\n",
			pElan->local_segment_id, pElan->elanName);
		    pElan->local_segment_id = 0;
		}
	    }
	    else
		USER_ERROR("%% elan '%s' does not exist", name);
	    break;

	/* delete a server atm address from an elan */
	case LANE_LECS_SAA_DELETE:

	    pCfg = (ConfigTable*) csb->laneLecsCfgTbl;

	    /* get and analyze the "name" variable */
	    name = GETOBJ(string, 1);
	    if (name && (name [0] == 0)) name = NULL;
	    if (name && invalidNameLength(name)) return;

	    /* get and analyze the "hwaddr" variable */
	    hwaddr = GETOBJ(hwaddr, 1);
	    if (! lane_addr_valid(hwaddr)) hwaddr = NULL;

	    /* ok do the deed */
	    deleteElan_AndOr_ServerAtmAddress(pCfg, name, hwaddr);
	    break;

	/* add/delete a LEC to/from an elan using its mac address */
	case LANE_LECS_CMD_ADD_MAC:

	    pCfg = (ConfigTable*) csb->laneLecsCfgTbl;
	    hwaddr = GETOBJ(hwaddr, 1);
	    maskedAddress_setMacValues(&masked, hwaddr->addr);

	    /* add a LEC identified by a mac address to an elan */
	    if (csb->sense)
	    {
		name = GETOBJ(string, 1);
		if (invalidNameLength(name)) return;
		ConfigTable_addLec(pCfg, name, &masked);
	    }
	    else
	    {
		/* delete a LEC identified by a mac address */
		ConfigTable_destroyLecByMacAddr(pCfg, &masked);
	    }
	    break;

	/* add/delete a LEC to/from an elan using its nsap address */
	case LANE_LECS_CMD_ADD_NSAP:

	    pCfg = (ConfigTable*) csb->laneLecsCfgTbl;
	    maskedPtr = GETOBJ(maskedAddr, 1);

	    /* add a LEC identified by an nsap address to an elan */
	    if (csb->sense)
	    {
		name = GETOBJ(string, 1);
		if (invalidNameLength(name)) return;
		ConfigTable_addLec(pCfg, name, maskedPtr);
	    }
	    else
	    {
		/* delete a LEC identified by an nsap address */
		ConfigTable_destroyLecByAtmAddr(pCfg, maskedPtr);
	    }
	    break;

	/* add/delete a LEC to/from an elan using its route descriptor */
	case LANE_LECS_CMD_ADD_RD:

	    name = GETOBJ(string, 1);
	    if (invalidNameLength(name)) return;
	    pCfg = (ConfigTable*) csb->laneLecsCfgTbl;
	    segment_id = GETOBJ(int, 1);
	    bridge_num = GETOBJ(int, 2);
	    route_desc = (segment_id << 4) | bridge_num;
	    maskedAddress_setRDValues (&masked, &route_desc);

	    /* add a LEC identified by a route descriptor to an elan */
	    if (csb->sense)
	    {
		ConfigTable_addLec(pCfg, name, &masked);
	    }
	    else
	    {
		/* delete a LEC identified by a route descriptor */
		ConfigTable_destroyLecByRouteDesc(pCfg, name, &masked); 
	    }
	    break;

	/* make this elan the default elan for the config table */
	case LANE_LECS_CMD_DFLT:

	    pCfg = (ConfigTable*) csb->laneLecsCfgTbl;
	    if (csb->sense)
	    {
		name = GETOBJ(string, 1);
		if (invalidNameLength(name)) return;
	    }
	    else
		name = NULL;
	    ConfigTable_setDefaultElan(pCfg, name);
	    break;

	/* display the names of all config tables */
	case LANE_LECS_CFGTBL_DISPLAY:

	    pCfg = List_head(G_ConfigTablesList);
	    while (pCfg != NULL)
	    {
		printf("  %s", pCfg->tableName);
		if (pCfg == (ConfigTable*) csb->laneLecsCfgTbl)
		    printf("   <----- config table in context");
		printf("\n");
		pCfg = List_next(pCfg);
	    }
	    break;

	/* atm-nsap-exercise command */
	case LANE_LECS_NSAP_TEST:
	    {
		MaskedAddress *maddr;

		maddr = GETOBJ(maskedAddr, 1);
		printf("\n**** MaskedAddress ****\n");
		printf("type = %d\n", maddr->type);
		printf("length = %d\n", maddr->length);
		printf("data bytes = %Cj", maddr->dataBytes);
		printf("\nmask bytes = %Cj", maddr->maskBytes);
		printf("**** end of MaskedAddress ****\n");
	    }
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }
}

/*****************************************************************
**
** exit from "lane config-table" top level config mode
**
*/
void lecs_cfgtblexit_function (parseinfo *csb)
{
    exit_config_submode(csb);
    cfgtable_context_exit(csb);
}

/*****************************************************************
**
** nvgens support for lecs nsap address binding.
**
*/
static void LecsAddresses_regenerate (idbtype *swidb,
    LecsInfo *pinfo, char *createString, int variation,
    boolean toLecs, boolean dbKwParsed)
{
    LecsAddress *pa;
    char buf [ATM_MAX_NSAP_STRING];
    lane_info_t *linfo;
    MaskedAddress *maddr;

    if ((createString == NULL) || (createString [0] == 0))
	LECS_CRASH();

    /* command is issued on the main interface */
    if (toLecs) {

	if (!pinfo) return;

	pa = List_head(pinfo->lecsAddressesList);
	while (pa != NULL) {
	    if (pa->formationType == variation) {
		buf [0] = 0;
		if (variation == LANE_LECS_BIND_ADDR)
		    sprintf(buf, "%Cw", &pa->maskedAddr);
		nv_write(TRUE, "%s %s", createString, buf);
	    }
	    pa = List_next(pa);
	}

    /* command is issued on the sub interface */
    } else {

	linfo = swidb->lane_info;
	if (linfo == NULL) return;
	if (linfo->lecs_nsap_type != variation) return;
	maddr = &linfo->lecs_nsap;
	if ((maddr->length <= 0) || (maddr->type == STATION_ILLEGAL))
	    return;

	/* for fixed and auto */
	buf [0] = 0;

	/* for manually typed ones */
	if (variation == LANE_LECS_BIND_ADDR)
	    sprintf(buf, "%Cw", maddr);

	/* now write it out */
	nv_write(TRUE, "%s %s", createString, buf);
    }
}

/*****************************************************************
**
** [no] lane config TABLE_NAME 
**
**	The above command runs in the interface (NOT sub-interface)
**	config mode.  It binds or unbinds an lecs config table 
**	to/from an interface.
**
** [no] lane global-lecs-address ADDRESS
**
**	The above command runs in the interface (NOT sub-interface)
**	config mode.  It adds/deletes a global LECS address to/from
**	the interface.
**
** [no] lane [config] { config-atm-address ADDRESS_TEMPLATE |
**			fixed-config-atm-address | 
**			auto-config-atm-address }
**
**	The above command can run BOTH in the interface AND the
**	subinterface contexts.  When it is run in the interface 
**	context, the keyword "config" can be used to target
**	the address type to the LECS itself.  In the subif mode,
**	the command is targetted at LANE entities OTHER than the
**	LECS itself and the "config" keyword cannot be used.
*/
void lecs_interface_function (parseinfo *csb)
{
    idbtype *swidb = csb->interface;
    int command = csb->which;
    boolean toLecs = GETOBJ(int, 1);
    int variation = GETOBJ(int, 2);
    boolean dbKwParsed = GETOBJ(int, 3);
    char *name = GETOBJ(string, 1);
    hwaddrtype *hwaddr = GETOBJ(hwaddr, 1);
    MaskedAddress masked = *(GETOBJ(maskedAddr, 1));
    LecsInfo *pinfo = getLecsPointer(swidb);
    ConfigTable *pCfg = NULL;
    boolean parsingConfigFile;

/* california version release number */
#define ELEVEN_TWO	(11*256 + 2)

    /*
    ** internal logic checks:  if the command is intended
    ** for the LECS itself, we MUST be on the major interface.
    ** Also, if the command is a "bind config table" command,
    ** then it could only be intended for the LECS.
    */
    if (toLecs && is_subinterface(swidb)) LECS_CRASH();
    if ((command == LANE_LECS_BIND_TABLE) && (! toLecs))
	LECS_CRASH();

    /* are we parsing this command from a config file ? */
    parsingConfigFile = csb->resolvemethod != RES_MANUAL;

    /* make masked address structure usable */
    masked.type = STATION_ATMNSAP;
    masked.length = STATIONLEN_ATMNSAP;

    /* nvgen stuff */
    if (csb->nvgen)
    {
	switch (command)
	{
	    case LANE_LECS_GLOBAL_ADDR:
		globalLecsAddresses_regenerate(swidb, 
		    csb->nv_command);
		break;

	    case LANE_LECS_BIND_TABLE:

		/* no lecs configured */
		if (! pinfo) break;
		pCfg = pinfo->cfgTable;
		if (! pCfg) break;

		/*
		** Only write the new form out.
		** This is the one which has the "database"
		** keyword in the syntax.
		*/
		nv_write(dbKwParsed, "%s %s", 
		    csb->nv_command, pCfg->tableName);
		break;

	    case LANE_LECS_BIND_ADDR:
		LecsAddresses_regenerate(swidb, pinfo,
		    csb->nv_command, variation, 
		    toLecs, dbKwParsed);
		break;

	    default:
		bad_parser_subcommand(csb, csb->which);
		break;
	}
	return;
    }

    /* normal stuff */
    switch (command)
    {
	/* add/delete a global LECS address to/from interface */
	case LANE_LECS_GLOBAL_ADDR:
	    if (csb->sense)
		switchLecsAddress_add(swidb, TRUE, hwaddr);
	    else
		switchLecsAddress_delete(swidb, hwaddr);
	    break;

	/* bind the interface to a config table */
	case LANE_LECS_BIND_TABLE:
	    
	    if (csb->sense)
	    {
		if (invalidNameLength(name)) return;
		pCfg = getConfigTableByName(name);
		if (pCfg == NULL)
		{
		    USER_ERROR("%% non existent config table");
		    return;
		}

		/* correct place to get/create the LecsInfo* */
		pinfo = buildLecsPointer(swidb);
		if (pinfo == NULL)
		{
		    LECS_ERR("lecs_interface_function: "
			"buildLecsPointer failed");
		    return;
		}
		
		/* 
		** check that nothing is bound or the same 
		** config table is not bound already
		*/
		if (pinfo->cfgTable != NULL)
		{
		    if (pinfo->cfgTable == pCfg)
		    {
			USER_ERROR("%% this config table is already");
			USER_ERROR(" bound to this interface\n");
			return;
		    }

		    USER_ERROR("%% config table '%s' is ",
			pinfo->cfgTable->tableName);
		    USER_ERROR("already bound to this interface\n");
		    USER_ERROR("%% un-bind it before re-binding ");
		    USER_ERROR("the new table\n");
		    return;
		}

		/* register config table with the interface */
		PRINTF("bound table '%s' to interface %s\n",
		    name, pinfo->name);
		ConfigTable_bindToLecsInfo(pCfg, pinfo);
	    }
	    else
	    {
		pinfo = getLecsPointer(swidb);
		if (pinfo == NULL)
		{
		    USER_ERROR("%% LANE Config Server is NOT ");
		    USER_ERROR("configured on this interface");
		    return;
		}
		if (pinfo->cfgTable == NULL)
		{
		    USER_ERROR("%% there is already no config ");
		    USER_ERROR("table bound to this interface");
		    return;
		}

		/* print BEFORE the pointer is cleared */
		PRINTF("un-bound table '%s' from interface %s\n",
		    pinfo->cfgTable->tableName, pinfo->name);

		/* unbind from interface */
		ConfigTable_bindToLecsInfo(NULL, pinfo);
	    }
	    break;

	/* bind an lecs nsap to interface */
	case LANE_LECS_BIND_ADDR:

	    /* 
	    ** While we are processing a configuration file, if we
	    ** are parsing a pre 11.2 lecs address bind command
	    ** on the MAJOR interface, then also make the command
	    ** directed to the LECS itself.  This is to preserve 
	    ** the backward compatibility with the pre 11.2 config 
	    ** files.
	    */
	    if (parsingConfigFile && !is_subinterface(swidb))
	    {
		if (global_parsed_swversion < ELEVEN_TWO)
		    toLecs = TRUE;
	    }

	    /* prepare data & mask bytes based on variation */
	    switch (variation)
	    {
		/* already done in the init section */
		case LANE_LECS_BIND_ADDR:
		    break;

		case LANE_LECS_BIND_AUTO:
		    memset(masked.dataBytes, 0, STATIONLEN_ATMNSAP);
		    memset(masked.maskBytes, 0, STATIONLEN_ATMNSAP);
		    break;

		case LANE_LECS_BIND_FIXED:
		    bcopy(FIXED_LECS_NSAP, masked.dataBytes, 
			STATIONLEN_ATMNSAP);
		    bcopy(ALL_ONES_MASK, masked.maskBytes, 
			STATIONLEN_ATMNSAP);
		    break;

		default:
		    bad_parser_subcommand(csb, variation);
		    return;
	    }

	    /* process depending on where it is targetted */
	    if (toLecs)
	    {
		pinfo = buildLecsPointer(swidb);
		if (!pinfo) {
		    LECS_ERR("cannot build the Lecs structure");
		    return;
		}
		if (csb->sense) {
		    addLecsAddress(swidb, &masked, variation, 0);
		} else {
		    deleteLecsAddress(swidb, &masked, variation, 0);
		}
	    }
	    else
	    {
		if (csb->sense)
		    exposeThisLecsAddress(swidb, &masked, variation);
		else
		    unExposeThisLecsAddress(swidb, &masked);
	    }

	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }
} 

/*
** used for the show command for the config table.
** If tableName is NULL, all the tables (NOT detailed)
** are shown.  Otherwise only the specific table is shown.
** Furthermore, if the detail is set, then all the LEC
** members in each elan will also be displayed.
*/
static void ConfigTable_show (char *tableName, int detail)
{
    LecsInfo *pinfo;
    ConfigTable *pcfgFirst, *pcfgLast, *pCfg;
    ElanInfoBlock *pElan;
    LecInfoBlock *pLec;
    int i, count, n;
    IndexPtr ind;
    ServerAtmAddress *psaa;

    /* set boundary pointers */
    if (tableName == NULL)
    {
	pcfgFirst = List_head(G_ConfigTablesList);
	pcfgLast = List_tail(G_ConfigTablesList);
    }
    else
    {
	pcfgFirst = getConfigTableByName(tableName);
	if (pcfgFirst == NULL)
	{
	    USER_ERROR("%% specified config table does NOT exist");
	    return;
	}
	pcfgLast = pcfgFirst;
    }

    pCfg = pcfgFirst;
    while (pCfg != NULL)
    {
	int noHeader = 1;

	/* print if table is bound to any interface/s */
	printf("\n");
	printf("\nLANE Config Server database table '%s'", 
	    pCfg->tableName);
	pinfo = List_head(G_LecsInfosList);
	while (pinfo != NULL)
	{
	    if (pinfo->cfgTable == pCfg)
	    {
		if (noHeader)
		{
		    printf(" bound to interface/s:");
		    noHeader = 0;
		}
		printf(" %s", pinfo->name);
	    }
	    pinfo = List_next(pinfo);
	}
	printf("\n");

	/* print the default elan */
	if (pCfg->defaultElan == NULL) printf("no ");
	printf("default elan");
	if (pCfg->defaultElan != NULL)
	    printf(": %s", pCfg->defaultElan->elanName);
	printf("\n");

	/* print the elan list */
	pElan = List_head(pCfg->elanList);
	while (pElan != NULL)
	{
	    /* restricted status */
	    printf("elan '%s': %s", pElan->elanName,
		pElan->restricted ? "restricted" : "un-restricted");

	    /* les timeout */
	    if (pElan->lesTimeout != LES_TIMEOUT_DFLT)
		printf(", les-timeout %d", pElan->lesTimeout);

	    /* for token ring elans */
	    if ((pElan->local_segment_id > 0) &&
		(pElan->local_segment_id < LANE_MAX_SEGMENTID))
		    printf(", local-segment-id %d", 
			pElan->local_segment_id);

	    /* server atm addresses */
	    printf("\n");
	    psaa = List_head(pElan->lesAddressesList);
	    while (psaa != NULL)
	    {
		printf("  server %Ch (prio %d)",
		    &psaa->lesAddr, psaa->priority);
		if (psaa->connPtr || 
		    mgd_timer_running(&psaa->graceTimer))
		{
		    printf((pElan->activeLes == psaa) ?
			" active" : " backup");
		    if (mgd_timer_running(&psaa->graceTimer))
		    {
			unsigned int timeout;
			timeout = mgd_timer_left_sleeping
				    (&psaa->graceTimer);
			timeout /= 1000;
			printf(" %u secs to timeout", timeout);
		    }
		}
		printf("\n");

		psaa = List_next(psaa);
	    }

	    /* if detail is set print all the LEC's in this elan */
	    if (detail)
	    {
		/* display the mac address defined LECs */
		ind = pCfg->macAddressIndex;
		count = Index_numberOfElements(ind);
		n = 0;
		for (i=0; i<count; i++)
		{
		    pLec = Index_getElementPointer(ind, i);
		    if (pLec != NULL)
		    {
			if (pLec->parentElan == pElan)
			{
			    n++;
			    printf("    mac client %Cw\n",
				&pLec->address);
			}
		    }
		    else
			break;
		}

		/* display the UNMASKED nsap address defined LECs */
		ind = pCfg->UNmaskedAtmAddressIndex;
		count = Index_numberOfElements(ind);
		n = 0;
		for (i=0; i<count; i++)
		{
		    pLec = Index_getElementPointer(ind, i);
		    if (pLec != NULL)
		    {
			if (pLec->parentElan == pElan)
			{
			    n++;
			    printf("    atm client %Cw\n",
				&pLec->address);
			}
		    }
		    else
			break;
		}

		/* display the MASKED nsap address defined LECs */
		ind = pCfg->maskedAtmAddressIndex;
		count = Index_numberOfElements(ind);
		n = 0;
		for (i=0; i<count; i++)
		{
		    pLec = Index_getElementPointer(ind, i);
		    if (pLec != NULL)
		    {
			if (pLec->parentElan == pElan)
			{
			    n++;
			    printf("    atm client %Cw\n",
				&pLec->address);
			}
		    }
		    else
			break;
		}

		/*
		** for TR LANE, also add a display of the
		** route descriptor defined LECs
		*/
		ind = pCfg->rdIndex;
		count = Index_numberOfElements(ind);
		n = 0;
		for (i=0; i< count; i++)
		{
		    pLec = Index_getElementPointer(ind, i);
		    if (pLec != NULL)
		    {
			if (pLec->parentElan == pElan)
			{
			    n++;
			    printf("    client-route-descriptor %Cw\n", 
				&pLec->address);
			}
		    }
		    else
			break;
		 }
	    }

	    /* next elan in line */
	    pElan = List_next(pElan);
	}

	/* was this the last one */
	if (pCfg == pcfgLast) break;
	pCfg = List_next(pCfg);
    }
}

/*
** show the redundancy info.
*/
static void showGlobalLecsInfo (LecsInfo *pinfo, boolean brief)
{
    GlobalLecsAddress *glecs;
    int i, vcd;
    int timeLeft;

    if (! pinfo) return;
    timeLeft = timeRemainingToUpdateGlobalLecsAddresses();
    if (pinfo->globalLecsCount <= 0)
    {
	printf("no global LECS addresses exist");
	if (!brief)
	    printf(" (%d seconds to update)", timeLeft);
	printf("\n");
	return;
    }
    printf("list of global LECS addresses");
    if (!brief)
	printf(" (%d seconds to update)", timeLeft);
    printf(":\n");
    for (i=0; i<pinfo->globalLecsCount; i++)
    {
	glecs = &pinfo->globalLecsTable [i];
	printf("%Ch ", &glecs->hwaddr);
	if (! brief)
	{
	    switch (glecs->callType)
	    {
		case NO_CALL:
		    break;
		case INCOMING_CALL:
		    vcd = glecs->u.pconn->vcd;
		    printf("incoming call (vcd %d)", vcd);
		    break;
		case OUTGOING_CALL:
		    if (glecs->connected)
		    {
			vcd = atmSig_api_callId2vcd(glecs->u.callId);
			printf("connected outgoing call (vcd %d)",
			    vcd);
		    }
		    else
			printf("outgoing call placed");
		    break;
	    }

	    /* if this is one of my addresses, notify */
	    if (searchLecsNsapActualValues(pinfo, glecs->hwaddr.addr))
		printf(" <-------- me");
	}
	printf("\n");
    }
}

/*
** printf info about one particular connection object.
**
** the reason we do the "printf" operation in a single
** atomic way at the end is becoz we do NOT want to
** context switch in the middle of this somewhere.
** Once the context changes in the middle, a real
** possibility of losing the "pconn" exists.  That
** is what we are trying to avoid.
*/
static void Connection_display (LecsInfo *pinfo, 
    Connection *pconn)
{
    char extra [64];
    int offset;
    unsigned ipkts, opkts;
    ElanInfoBlock *pElan;

    /* get packet counts */
    atm_getvc_counts(pinfo->hwidb, pconn->vcd, 
	&ipkts, &opkts);

    /* print out the entity type connected */
    if (pconn->callerType == CALLER_IS_LES) {
	offset = sprintf(extra, "LES");
	pElan = NULL;
	if (pconn->lesAddress)
	    pElan = pconn->lesAddress->elanPtr;
	if (pElan) {
	    offset += sprintf(extra+offset, " %s %d ", 
		pElan->elanName, 
		pconn->lesAddress->priority);
	    if (pElan->activeLes == pconn->lesAddress)
		strcat(extra+offset, "active");
	    else
		strcat(extra+offset, "backup");
	}
    } else if (pconn->callerType == CALLER_IS_LECS) {
	sprintf(extra, "LECS");
    } else if (pconn->callerType == CALLER_IS_LEC) {
	    sprintf(extra, "LEC");
    } else {
	sprintf(extra, "UNKNOWN");
    }
    
    printf("%-4u %-6u %-6u  %Ch %s\n", 
	pconn->vcd, ipkts, opkts, &pconn->callingParty, extra);
}

/*
** used for show command for the lecs interface.
** If the interface is NULL, all the connections
** on all the nsap addresses on that interface
** are shown.  Otherwise only the specific one is
** shown
*/
void lecs_interface_show (idbtype *swidb, boolean brief)
{
    LecsInfo *first, *last, *pinfo;
    LecsAddress *pa;
    Connection *pconn, **pconnPtr;
    int vcPrinted, vcd;
    boolean showDetail = ! brief;
    boolean operational;
    boolean reachable;
    boolean lecsHint;
    boolean master;
    int num_addrs = 0;

    if (swidb == NULL)
    {
	first = List_head(G_LecsInfosList);
	last = List_tail(G_LecsInfosList);
    }
    else
    {
	if (swidb->lane_info == NULL) return;
	if (swidb->lane_info->lecs_info == NULL) return;
	first = last = swidb->lane_info->lecs_info;
    }

    /* display the state of the LECS server */
    if (lane_lecs_debug)
    {
	printf("\nLECS config server process is ");
	if (G_lecsServerPid != NO_PROCESS)
	    printf("UP (pid=%d)", G_lecsServerPid);
	else
	    printf("DOWN");
	printf("\n");
    }

    pinfo = first;
    while (pinfo != NULL)
    {
	/* how many addresses are bound to it ? */
	num_addrs = List_size(pinfo->lecsAddressesList);

	/* is the lecs operational on this interface ? */
	operational = pinfo->lecsRunning;

	/* is it reachable */
	reachable = operational && lecsIsReachable(pinfo);

	/* is it the master LECS ? */
	master = pinfo->thisLecsIsTheMaster;

	/* there is a hint of lecs on this interface */
	lecsHint = hintOfLecs(pinfo);

	/* 
	** display some lecs info if there is at least a "hint" 
	** of it on this interface.  
	*/
	if (lecsHint)
	{
	    printf("\nLE Config Server %s", pinfo->name);
	    printf(" config table: %s\n", 
		pinfo->cfgTable ? 
		    pinfo->cfgTable->tableName: "");
	    printf("Admin: %s  State: ",
		pinfo->shutDown ? "down" : "up");
	    if (operational)
	    {
		printf("operational");
		if (! reachable) printf(" (maybe unreachable)");
		printf("\n");
		printf("LECS Mastership State: %s\n",
		    master ? "active master" : "backup");

		/* global lecs list info */
		showGlobalLecsInfo(pinfo, brief);
	    }
	    else
	    {
		printf("down\n");
		printf("down reasons:");
		if (pinfo->cfgTable == NULL)
		    printf(" NO-config-table;");
		if (num_addrs <= 0)
		    printf(" NO-nsap-address;");
		if (pinfo->shutDown)
		    printf (" NO-interface-up;");
		printf("\n");
	    }

	    /* debugging stuff */
	    if (lane_lecs_debug)
	    {
		printf("interface object creation time: %#Ta\n",
			    pinfo->lecsUpTime);
		printf("interface burnt in mac address: %e\n",
			    pinfo->hwidb->bia);
		printf("interface hardware mac address: %e\n",
			    pinfo->hwidb->hardware);
		printf("interface forced mac address  : %e\n",
			    pinfo->hwidb->forced_mac);

		if (operational)
		{
		    printf("up reasons:");
		    if (pinfo->cfgTable != NULL)
			printf(" config-table;");
		    if (num_addrs > 0)
			printf(" nsap-address;");
		    if (! pinfo->shutDown)
			printf (" interface-up;");
		    printf("\n");
		}
	    }
	}

	/*
	** display all the bound nsap addresses (if any)
	** AND the connections to these (if any).  Note that
	** in displaying these, show the ACTUAL final computed
	** nsap address and NOT what the user typed UNLESS the
	** address is not registered with the signalling or the
	** ilmi, in which case, print the user typed string.
	*/
	pa = List_head(pinfo->lecsAddressesList);
	while (pa != NULL)
	{
	    printf("ATM Address of this LECS: ");

	    /* the WKA should be treated in a special way */
	    if (lecsAddressIsWKA(pa)) {
		if (!operational || !master) {
		    printf("well known LECS address "
			"(inactive until master)\n");
		    goto SKIP;
		}
	    }
	    /* 
	    ** print the "actual" value ONLY if that value is set.
	    */
	    if (pa->addrFlags & ACTUAL_VALUE_VALID)
		printf("%Ch", &pa->actualValue);
	    else
		printf("%43s", 
		    "EXACT ADDRESS NOT YET SET (NO PREFIX ?)");

	    switch (pa->formationType)
	    {
		case LANE_LECS_BIND_FIXED: 
		    printf(" (well known)");
		    break;
		case LANE_LECS_BIND_AUTO:
		    printf(" (auto)");
		    break;
		default:
	    }

	    /* show failed registrations */
	    if (pa->ilmiState != REGISTERED)
	    {
		printf("\n             ");
		printf("the above address has NOT yet "
		    "been registered with ILMI");
	    }
	    if (pa->sigState != REGISTERED)
	    {
		printf("\n             ");
		printf("the above address has NOT yet "
		    "been registered with ATM signalling");
	    }
	    printf("\n");

	    /*
	    ** if ANY the registrations has failed, OR the actual
	    ** value has not ever been set, then show the original
	    ** user typed form so that it can be identified more 
	    ** easily by the user.
	    */
	    if ((pa->ilmiState != REGISTERED) ||
		(pa->sigState != REGISTERED) ||
		! (pa->addrFlags & ACTUAL_VALUE_VALID))
	    {
		if (! allOnesMask(pa->MASK_BYTES, 
			    STATIONLEN_ATMNSAP) ||
		    ! (pa->addrFlags & ACTUAL_VALUE_VALID))
		{
		    printf("  actual user specified form: %Cw\n",
			&pa->maskedAddr);
		}
	    }

	    /* 
	    ** Display live SVC connections targetted at THIS nsap.
	    **
	    ** The reason the static "connexions" table is searched
	    ** instead of the linked list "connectionList" is to
	    ** avoid the list being changed from under us while we are
	    ** in the process of printing and the printf causing
	    ** a context change and hence creating that nasty
	    ** opportunity.
	    */
	    if (showDetail) {
		vcPrinted = vcd = 0;
		pconnPtr = &pinfo->connexions [0];
		while (vcd < ATM_MAX_VC_DEFAULT) {

		    /*
		    ** display connection ONLY if not NULL AND 
		    ** also targetted at this LECS address.
		    */
		    pconn = *pconnPtr;
		    if (pconn && (pconn->calledParty == pa)) {
			if ((vcPrinted % 20) == 0)
			    printf(" vcd  rxCnt  txCnt  callingParty\n");
			vcPrinted++;
			Connection_display(pinfo, pconn);
		    }
		    vcd++;
		    pconnPtr++;
		}
	    }

	SKIP:

	    pa = List_next(pa);
	}

	/*
	** display statistics if there is a hint of lecs
	*/
	if (lecsHint && showDetail)
	{
	    printf("cumulative total number of ");
	    printf("unrecognized packets received so far: %u\n",
		pinfo->lecsInConfigErrors);
	    printf("cumulative total number of config ");
	    printf("requests received so far: %u\n",
		pinfo->lecsInConfigReqs);
	    printf("cumulative total number of config ");
	    printf("failures so far: %u\n",
		pinfo->lecsOutConfigFails);
	    if (pinfo->lecsOutConfigFails > 0)
	    {
		printf("    cause of last failure: %s\n",
		    lane_pr_status(pinfo->causeOfLastFailure));
		printf("    culprit for the last failure: %Cj", 
		    pinfo->lastOffender);
		printf("\n");
	    }
	}

	if (pinfo == last) break;
	pinfo = List_next(pinfo);
    }
}

/*****************************************************************
**
** exec level show commands
**
** show lane config [interface INTERFACE]
** show lane database [TABLE_NAME [brief]]
*/

void lecs_show_function (parseinfo *csb)
{
    char *name;

    /* enable paginated output */
    automore_enable(NULL);

    switch (csb->which) 
    {
	/* show lecs connexion table */
	case SHOW_LANE_LECS:
        
            lecs_interface_show
		((GETOBJ(int, 2) & SHOW_LANE_INT) ?
		    GETOBJ(idb, 1) : NULL,
		    GETOBJ(int, 2) & SHOW_LANE_BRIEF);
	    break;

	/* show lecs config table */
	case SHOW_LANE_CFGTBL:

            if (GETOBJ(int, 2) & SHOW_LANE_SPECIFIC) 
	    {
                name = GETOBJ(string, 1);
		if (invalidNameLength(name)) return;
            }
	    else
		name = NULL;
            ConfigTable_show(name, 
		!(GETOBJ(int, 2) & SHOW_LANE_BRIEF));
            break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }

    /* disable paginated output */
    automore_disable();
} 

