/* $Id: lecs.h,v 3.5.28.11 1996/09/11 17:27:17 cakyol Exp $
 * $Source: /release/112/cvs/Xsys/lane/lecs.h,v $
 *------------------------------------------------------------------
 * The lane lecs implementation header file
 *
 * November 8, 1994
 *
 * Copyright (c) 1994-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lecs.h,v $
 * Revision 3.5.28.11  1996/09/11  17:27:17  cakyol
 * CSCdi68348:  The LECS needs to LOCALLY maintain the calling party
 * address of conn
 * Branch: California_branch
 *
 * Revision 3.5.28.10  1996/08/28  12:55:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.28.9  1996/08/26  19:46:10  cakyol
 * CSCdi66421:  the well known LECS address needs to follow the master LECS
 * Branch: California_branch
 *
 * Revision 3.5.28.8  1996/06/29  06:33:51  cakyol
 * CSCdi61448:  Router crashes after changing master LECS addr in ls1010
 * table
 * Change the printf strategy in "lecs_interface_show".  The problem
 * arises when performing a "show lane config" at the same instant
 * that a connection obkject is being deleted.
 * Branch: California_branch
 *
 * Revision 3.5.28.7  1996/06/21  20:54:57  cakyol
 * CSCdi60981:  more than one LECS address for the same LECS breaks
 * redundancy
 * Branch: California_branch
 *
 * Revision 3.5.28.6  1996/06/08  20:45:48  cakyol
 * CSCdi59043:  %LANE-3-LECS_ERROR: processSetupIndication: NULL pinfo ...
 * Branch: California_branch
 *
 * Revision 3.5.28.5  1996/06/06  04:31:34  cakyol
 * CSCdi59510:  format string in inline wastes space.
 *              offending macro obliterated.
 * Branch: California_branch
 *
 * Revision 3.5.28.4  1996/05/25  09:48:21  cakyol
 * CSCdi58685:  router crashed when LS100 system call errors occured.
 *              forgot to lock/unlock connection objects while
 *              printing them out.
 * Branch: California_branch
 *
 * Revision 3.5.28.3  1996/05/09  14:33:16  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.28.2.2.1  1996/04/27  07:07:06  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.28.2  1996/04/25  23:15:06  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.5.28.1.8.5  1996/04/16  02:45:10  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.28.1.8.4  1996/04/11  18:20:12  cakyol
 * amendments to the latest sync:
 * - take some unnecessary crashdump's out of lane server.
 * - correct possible input buffer loss at LECS input processing
 *   for token ring lan type.
 * - debugging messages needed tidying up.
 * - some but not all white space cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.28.1.8.3  1996/04/11  05:34:40  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.5.28.1.8.2  1996/04/08  14:52:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.28.1.8.1  1996/04/08  01:57:29  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.28.1  1996/03/18  20:33:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.32.5  1996/04/05  03:20:43  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.32.4  1996/04/02  10:10:03  cakyol
 * - perform re-verifications ONLY on completely operational clients.
 * - tidy up initializations, they were scattered all over the place.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.32.3  1996/03/27  22:57:53  cakyol
 * Added some more 11.2 backward compatibility code that I missed.
 * This involves the "name NAME server-atm-address ADDRESS" lane
 * database command.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.32.2  1996/03/25  02:20:06  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.32.1  1996/03/22  22:55:31  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.12.2  1996/03/20  23:48:14  cakyol
 * The 11.2 LECS parser can now FULLY recognize pre 11.2 LECS
 * commands .  As a result of this, we also have a new systemwide
 * global uint variable named "global_nvconfig_version", which
 * reflects the nvram version number in it.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.12.1  1996/02/27  21:10:54  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/01/19  02:46:17  cakyol
 * CSCdi47155:  Removing a LANE database while editing it can crash router
 * - fixed.  a simple locking mechanism now takes care of it.
 *
 * Revision 3.4.2.9  1996/02/22  21:08:26  cakyol
 * new scheduling paradigm expects a process to watch only
 * on a single managed timer.  So, convert multiple timers
 * to work under one master timer.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.2.8  1996/02/22  09:05:37  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.2.7  1996/02/08  00:04:09  cakyol
 * save a lot of string space by assigning __FILE__ to a
 * static char* at the beginning of the file and use that
 * throughout instead.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.2.6  1996/02/06  22:19:37  cakyol
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
 * Revision 3.4.2.5  1996/01/29  22:06:18  cakyol
 * - as per the signalling group's request, eliminate the
 *   direct invasion of their private data structures by
 *   using:
 *     atmSig_callId2CallingPartyAddress   and
 *     atmSig_sameCallingPartyAddress
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.2.4  1996/01/26  02:02:47  cakyol
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
 * Revision 3.4.2.3.2.2  1996/03/19  00:30:43  kjadams
 * Branch: TRlane_branch
 * Add support for new config commands for TR LANE
 *
 * Revision 3.4.2.3.2.1  1996/02/26  23:08:58  ddecapit
 * Branch: TRlane_branch
 *
 * Revision 3.4.2.3  1996/01/19  19:37:05  cakyol
 * - lock a config table to make it accessible by only a
 *   single session at a time (this was bug CSCdi47155
 *   in 11.0 and 11.1)
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.2.2  1996/01/11  22:00:43  cakyol
 * somehow "timer.h" got in here again.  Should
 * NOT be here
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.2.1  1996/01/11  01:46:51  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4  1996/01/05  04:56:23  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3.10.2  1996/01/03  22:52:53  cakyol
 * - divided up debug categories to: all, events and packets
 *   for the LECS.  There were too many debugging messages
 *   for a single debug flag.
 *
 * - updated some of the outdated comments.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3.10.1  1995/12/29  01:07:06  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/17  17:51:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:51:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:23:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/08/04  06:43:58  hampton
 * sched_private.h included too many places  [CSCdi38246]
 *
 * Revision 2.6  1995/07/13 21:39:48  cakyol
 * CSCdi37017:  LECS should advertise its coming up and going down
 * fixed.
 *
 * CSCdi37071:  lecs should migrate to "malloc_named"
 * fixed.
 *
 * Revision 2.5  1995/07/13  01:07:12  cakyol
 * CSCdi37016:  lecs access policy should allow for restricted elans
 * - done.  The main point is that restricted elans cannot now be
 *   joined by name only.  Explicit atm address or mac address
 *   is required.
 *
 * Revision 2.4  1995/06/29  23:15:35  cakyol
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
 * Revision 2.3  1995/06/20  18:13:04  cakyol
 * CSCdi35998:  no error while configuring LEC on subif missing LECS NSAP
 * fixed
 *
 * Revision 2.2  1995/06/16  01:09:25  cakyol
 * CSCdi35938:  lecs should show as down if none of its adrrs are
 * registered
 * - fixed.  To be operational now, at least one nsap address
 *   must show as fully registered both with ilmi & signalling
 *
 * Revision 2.1  1995/06/07  21:20:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lecs_h
#define lecs_h

/* out of our control */

#include "master.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "mgd_timers.h"
#include "logger.h"
#include "ttysrv.h"
#include "../os/chunk.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../if/network.h"
#include "../ui/debug.h"
#include "../os/clock.h"
#include "../os/signal.h"
#include "../atm/atmsig_api.h"
#include "../atm/ilmi_api.h"
#include "config.h"
#include "globs.h"

/* in our control */

#include "parser_defs_lane.h"
#include "generic_objects.h"
#include "lane.h"
#include "lane_debug.h"
#include "lane_private.h"
#include "lane_ilmi.h"
#include "lane_signal.h"
#include "lane_registry.h"
#include "lecs_finder.h"

/* forward type declarations */

typedef struct lecs_info_t_ lecs_info_t, LecsInfo;
typedef struct serverAtmAddress ServerAtmAddress;
typedef struct elanInfoBlock ElanInfoBlock;

/******************************************************************
*******************************************************************
**			Lecs Nsap Address object
*******************************************************************
******************************************************************/

/*
** at what stage is an address registration ?
** this applies both to ilmi and signalling.
*/
typedef enum RegistrationState {

    DEREGISTERED = 0,	/* address completely deregistered */
    REGISTERING,	/* in the process of registering */
    REGISTERED,		/* address completely registered */
    DEREGISTERING	/* in the process of deregistering */

} RegistrationState;

/*
** This definitively represents an LECS nsap address.
**
** "maskedAddr" holds the data & mask and is used to
** regenerate the address AS THE USER TYPED it, as well as
** in comparisons.
**
** "actualValue" holds an absolute address which is derived
** from the masked address and the ilmi prefix etc, so that
** it can be fed to the signalling api.
**
** The "formationType" field indicates how this address
** was formed from the command line, so that we can 
** re-create the exact command syntax when we hit the nvgen 
** stuff.  This could be "fixed", "auto" or manually specified.
**
** The "addrFlags" field indicates whether this address is
** registered with certain services, namely signalling and
** ilmi.
**
** the signature value indicates whether the structure has
** been freed from under us.  This can occur on return from
** a signalling registration with an error.  We have to check
** if the structure still contains valid data.  The signature
** is set to the actual pointer value of the lecs address structure.
*/

typedef struct lecsAddress
{
    /*
    ** this HAS to be the FIRST field, 
    ** to make a list of these guys
    */
    ListEntry links;

    /* the masked combination address */
    MaskedAddress maskedAddr;

    /* the LecsInfo object, to which this addr is bound to */
    LecsInfo *lecsInfo;

    /*
    ** this is the current snapshot, with prefix etc 
    ** only valid at one particular instance in time
    */
    hwaddrtype actualValue;

    /* registration states */
    RegistrationState ilmiState;
    RegistrationState sigState;

    /* frills */
    uchar formationType;
    uchar addrFlags;
    int snmpIndex;

} LecsAddress, *pLecsAddress;

#define ADDR_TYPE	maskedAddr.type
#define ADDR_LEN	maskedAddr.length
#define DATA_BYTES	maskedAddr.dataBytes
#define MASK_BYTES	maskedAddr.maskBytes

/* possible values for "addrFlags" */

/*
** this bit indicates that an "absolute" nsap value has been
** obtained and placed into the actualValue field of the lecs
** nsap address structure.  This was (or SHOULD have been) obtained
** by using the prefix, mac address and selector appendage routine.
** This could NOT have been done until the prefix was obtained
** from the ilmi.  For all practical purposes, "actualValue" is the 
** value that gets registered with the ilmi and signalling 
** subsystems.
*/
#define ACTUAL_VALUE_VALID		0x1

/******************************************************************
*******************************************************************
**			Connection object
*******************************************************************
******************************************************************/

/*
** who the call is originating from 
*/
typedef enum callOriginatorType
{
    NO_CALLER =	0,		/* make sure this one is 0 */
    CALLER_IS_LEC,		/* caller is a LANE client */
    CALLER_IS_LES,		/* caller is a LANE server */
    CALLER_IS_LECS,		/* caller is an lecs */

} CallOriginatorType;

/*
** this structure identifies an svc in the
** router.  The "callId" is returned as a
** handle from the signalling API.
**
** Note that if a connection is made from an LES,
** then the "lesAddress" pointer points to that 
** address object, from which the elan can be found.
**
** Alternatively, if the connection is from an LECS,
** then the "slecsAddress" pointer points to the
** corresponding switch lecs address object.
**
** If the connection is from neither or cannot be
** determined, both these pointers are NULL.
*/
typedef struct connection
{
    ListEntry links;			/* to use it in a list */
    void *callId;			/* signalling handle */
    vcd_t vcd;				/* vcd of the circuit */
    LecsInfo *lecsInfo;			/* on which interface */
    LecsAddress *calledParty;		/* THIS address */
    hwaddrtype callingParty;		/* OTHER end */
    CallOriginatorType callerType;	/* type of the caller */
    ServerAtmAddress *lesAddress;	/* if caller is an LES */
    int index;				/* if caller is an LECS */

} Connection, *pConnection;

/******************************************************************
*******************************************************************
**			LecInfoBlock object
*******************************************************************
******************************************************************/

/*
** basic storage unit in the database, everything else
** points to these guys.  These are used as reference from
** an ELAN membership list as individual members of an ELAN 
** or from whatever you want to reference them from.
**
** the "parentElan" field is a cross reference to the elan object,
** that I am a member of.  It is there so that I can find my
** own ELAN immediately.
*/

typedef struct lecInfoBlock
{
    MaskedAddress address;		/* LEC mac/atm address */
    sys_timestamp lastAccessTime;	/* sys up time stamp */
    struct elanInfoBlock *parentElan;	/* elan it belongs to */

} LecInfoBlock, *pLecInfoBlock;

/******************************************************************
*******************************************************************
**			ElanInfoBlock object
*******************************************************************
******************************************************************/

/*
** this structure represents one server atm address for an elan
** (elanInfoBlock).  An elan can have MANY server atm addresses
** for redundancy.
**
** If a connection from an LES is indeed made, the connPtr pointer
** points to that connection object.
*/
struct serverAtmAddress
{
    /* to make a list */
    ListEntry links;

    /* which elan this server atm address belongs to */
    ElanInfoBlock *elanPtr;

    /* the atm address itself */
    hwaddrtype lesAddr;

    /* priority */
    int priority;

    /* reference to connection object, IF it exists */
    Connection *connPtr;

    /* the grace period timer */
    mgd_timer graceTimer;

};

#define NAMESIZE		33

/*
** the grace time in secs, given to an LES to reconnect back to
** the LECS, before LECS switches the elan to another LES.
*/
#define LES_TIMEOUT_DFLT                0
	  
struct elanInfoBlock
{
    ListEntry links;			/* to make a list */
    char elanName [NAMESIZE];		/* name (null terminated) */
    uint elanNameSize;			/* length in bytes */
    boolean restricted;			/* access class */
    sys_timestamp lastAccessTime;	/* sys up time stamp */
    struct configTable *cfgTable;	/* my config table */
    uint local_segment_id;		/* TR LANE local segment id */

    /* the rest is for redundant LES implementation */

    ListPtr lesAddressesList;		/* list of LES addresses */
    ServerAtmAddress *activeLes;	/* currently active LES */
    int lesTimeout;			/* grace timeout */

};

/******************************************************************
*******************************************************************
**			ConfigTable object
*******************************************************************
******************************************************************/

typedef struct configTable
{
    /* part of a linked list */
    ListEntry links;

    /* the name of this guy */
    char tableName [NAMESIZE];

    /* default elan for this table, if any */
    ElanInfoBlock *defaultElan;

    /* how many interfaces are bound to this config table ? */
    int bindCount;

    /* stuff used to maintain the lec info database */

    /* ALL the LEC storages come from here */
    chunk_type *lecTable;

    /*
    ** index to search LECs by mac address.
    ** Provides VERY FAST lookup of mac address to LECs
    */
    IndexPtr macAddressIndex;

    /*
    ** index to search LECs by UNmasked nsap address.
    ** Provides VERY FAST lookup of unmasked atm address to LECs
    */
    IndexPtr UNmaskedAtmAddressIndex;

    /* index to search by pointer value (for MASKED addresses) */
    IndexPtr maskedAtmAddressIndex;

    /*
    ** add support for TR LANE.
    ** index to search by route descriptor 
    */
    IndexPtr rdIndex; 

    /* to chain the elans in this table together */
    ListPtr elanList;

    /*
    ** index to search elans by name.
    ** Provides VERY FAST lookup of elans by ascii name
    */
    IndexPtr elanIndexByName;

    /*
    ** index to search server atm addresses for elans by 
    ** atm address.  Provides VERY FAST lookup of server 
    ** atm address by atm address
    */
    IndexPtr serverAtmAddressesIndex;

    /* 
    ** info needed to "lock" this structure 
    ** for single threaded access
    */
    pid_t pid;
    signal_handler dflt_exit_handler;
    boolean locked;

} ConfigTable, *pConfigTable;

/******************************************************************
*******************************************************************
**			LecsInfo object
**		This is an extension of the idb and
**		the lane_info objects which hold lecs
**		specific stuff.
*******************************************************************
******************************************************************/

/*
** possible call types for inter LECS connexions
*/
typedef enum _callType
{
    NO_CALL = 0,
    INCOMING_CALL,	/* always from higher LECSs to me */
    OUTGOING_CALL,	/* always from me to lower LECSs */

} CallType;

typedef struct globalLecsAddress
{
    hwaddrtype hwaddr;		/* actual address */
    CallType callType;		/* in or out call */

    /* this is used only for outgoing calls */
    boolean connected;

    union
    {
	/* for an incoming call from a higher LECS */
	Connection *pconn;

	/* for an outgoing call to a lower LECS */
	void *callId;

    } u;

} GlobalLecsAddress;

/* 
** total number of LECSs in the system,
** for the LECS redundancy feature.  I
** think this is big enuf.  It is very
** unlikely that there will be more than
** this many.  Most switches will probably
** not support this many anyway.
*/
#define MAX_GLOBAL_LECS_COUNT	16

/*
** LecsInfo:
** the main structure that hangs off the "lane_info"
** on the swidb and contains all the details of the
** LECS on that swidb.
*/
struct lecs_info_t_
{
    /* part of a linked list */
    ListEntry links;

    /* indicates interface shutdown or not */
    boolean shutDown;

    /* list and number of the global LECSs in the system */
    boolean locked;
    int globalLecsCount;
    GlobalLecsAddress globalLecsTable [MAX_GLOBAL_LECS_COUNT];

    /* indicates whether there is a valid lecs here or not */
    boolean lecsRunning;

    /* indicates whether this is a master LECS or not */
    boolean thisLecsIsTheMaster;

    /* pointer back to the hardware idb */
    hwidbtype *hwidb;

    /* 
    ** actual name of interface, this is used to search
    ** in the inactive list, in case resurrection is needed.
    ** It is identical to the name in the hwidb.
    */
    char name [NAMESIZE];

    /* pointer back to the lane_info */
    lane_info_t *lane_info;
    
    /* list of all the available nsap addresses for THIS lecs */
    ListPtr lecsAddressesList;

    /* all the svc connections to this interface */
    ListPtr connectionList;

    /* which config table is this guy bound to; if any */
    ConfigTable *cfgTable;

    /* connection pointers, indexed by vcd */
    Connection *connexions [ATM_MAX_VC_DEFAULT];

    /* statistics counters */
    sys_timestamp lecsUpTime;
    int lecsInConfigReqs;
    int lecsInConfigErrors;
    int lecsOutConfigFails;
    int causeOfLastFailure;
    uchar lastOffender [STATIONLEN_ATMNSAP];

};

/******************************************************************
*******************************************************************
**		    Global variables & constants
*******************************************************************
******************************************************************/

extern ListPtr G_LecsInfosList;
extern ListPtr G_ConfigTablesList;
extern int G_lecsServerPid;
extern uchar FIXED_LECS_NSAP [];

/******************************************************************
*******************************************************************
**			Shared routines etc.
*******************************************************************
******************************************************************/

/* debug stuff */

extern void lecs_crash(char *fileName, int lineNumber);

#define PRINTF		if (lane_lecs_debug) printf
#define USER_ERROR	printf
#define LECS_CRASH()	lecs_crash(ThisFileName, __LINE__)

/*********** helpers and various address objects ***********/

extern boolean initWatchedQueue 
    (watched_queue **wq, char *qName);
extern boolean ILMI_Error
    (ilmiClientReq *response);
extern void maskedAddress_setMacValues 
    (MaskedAddress *maddr, uchar *bytes);
extern void maskedAddress_setAtmValues
    (MaskedAddress *maddr, uchar *dataBytes, uchar *maskBytes);
extern void maskedAddress_setRDValues 
    (MaskedAddress *maddr, ushort *bytes);
extern boolean lecsAddressIsWKA 
    (LecsAddress *lecsAddr);

/*********** connection object management ***********/

extern void sendRelease 
    (boolean releaseComplete, void *callId, 
     sig_api_cause_code cause, idbtype *swidb);

/* an invalid vcd value */
#define IGNORE_VCD	0

static inline 
boolean invalid_vcd (vcd_t vcd)
{ return ((vcd < 1) || (vcd >= ATM_MAX_VC_DEFAULT)); }
 
/*********** config table object management ***********/

extern ConfigTable *ConfigTable_alloc 
    (char *name);
extern ConfigTable *getConfigTableByName 
    (char *name);
extern boolean ConfigTable_destroyByPtr 
    (ConfigTable*);
extern ElanInfoBlock *ConfigTable_elanCreate
    (ConfigTable*, char *elanName, hwaddrtype *lesAddr, 
     int priority, ElanRestrictedStatus elanStatus);
extern boolean deleteElan_AndOr_ServerAtmAddress
    (ConfigTable *pCfg, char *elanName, hwaddrtype *lesAddr);
extern void elanSetLesTimeout 
    (ConfigTable *pCfg, char *name, int lesTimeout);
extern void renameElan 
    (ConfigTable *pCfg, char *oldName, char *newName);
extern ElanInfoBlock *getElanByName 
    (ConfigTable *pCfg, char *elanName, int elanNameSize);
extern void setElanRestrictedStatus 
    (ConfigTable *pCfg, char *name, ElanInfoBlock *pElan, 
     ElanRestrictedStatus elanStatus);
extern boolean ConfigTable_destroyElanByName
    (ConfigTable *pCfg, char *elanName);
extern boolean ConfigTable_setDefaultElan 
    (ConfigTable*, char *elanName);
extern LecInfoBlock *ConfigTable_addLec
    (ConfigTable*, char *elanName, MaskedAddress *lecAddr);
extern void ConfigTable_destroyLecByAtmAddr 
    (ConfigTable*, MaskedAddress *atmAddrPtr);
extern void ConfigTable_destroyLecByMacAddr 
    (ConfigTable*, MaskedAddress *macAddrPtr);
extern void ConfigTable_bindToLecsInfo 
    (ConfigTable*, LecsInfo*);
extern void ConfigTable_destroyLecByRouteDesc
    (ConfigTable*, char *elanName, MaskedAddress *rd_ptr);

/*********** LecsInfo object management ***********/

extern void LecsInfo_destroy 
    (LecsInfo *pinfo);
extern LecsAddress *searchLecsAddress 
    (LecsInfo*, MaskedAddress *lecsAddr, int formationType, 
     int snmpIndex);
extern LecsAddress *searchLecsNsapActualValues
    (LecsInfo *pinfo, uchar *addrDataBytes);

/* 
** "addLecsAddress" and "deleteLecsAddress"
** can return any one of those; granularity is needed by 
** the snmp stuff.
*/

#define NO_ERROR		0
#define NULL_IF			1	/* null LecsInfo object */
#define ALREADY_BOUND		2	/* address already here */
#define NOT_BOUND		3	/* not bound to delete */
#define MALLOC_FAILURE		4	/* need I say more ? */
#define REGISTRATION_ERROR	5	/* ilmi could NOT register */

extern int addLecsAddress 
    (idbtype *swidb, MaskedAddress *lecsAddr,
     int formationType, int snmpIndex);
extern int deleteLecsAddress 
    (idbtype *swidb, MaskedAddress *lecsAddr,
     int formationType, int snmpIndex);
extern LecsInfo *buildLecsPointer 
    (idbtype *swidb);
extern LecsInfo *getLecsPointer 
    (idbtype *swidb);
extern void exposeThisLecsAddress
    (idbtype *swidb, MaskedAddress *lecsAddr, int formationType);
extern void unExposeThisLecsAddress 
    (idbtype *swidb, MaskedAddress *lecsAddr);
extern boolean lecsIsReachable
    (LecsInfo *pinfo);
extern boolean hintOfLecs
    (LecsInfo *pinfo);
extern int myHighestRank 
    (LecsInfo *pinfo, LecsAddress **highest);
extern int timeRemainingToUpdateGlobalLecsAddresses
    (void);

#define FOR_ALL_LECSS(pinfo)	\
    for (pinfo = List_head(G_LecsInfosList); \
	 pinfo != NULL;  \
	 pinfo = List_next(pinfo))

#endif /* lecs_h */

