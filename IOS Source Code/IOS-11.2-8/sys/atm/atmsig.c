/* $Id: atmsig.c,v 3.7.10.21 1996/09/05 22:13:55 schrupp Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig.c,v $
 *---------------------------------------------------------------------------
 *
 * A T M S I G . C
 *
 * ATM Signalling Q.2931 support.
 * Conforms to ATM Forum UNI Specification, Version 3.0
 *
 * November 1993,  Percy P. Khabardar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *---------------------------------------------------------------------------
 * $Log: atmsig.c,v $
 * Revision 3.7.10.21  1996/09/05  22:13:55  schrupp
 * CSCdi68237:  ATMSIG: Dont clear vcnum when clearing vc
 * Branch: California_branch
 *
 * Revision 3.7.10.20  1996/09/04  05:10:56  schrupp
 * CSCdi67982:  ATMSIG: Dont suspend prematurely
 * Branch: California_branch
 *
 * Revision 3.7.10.19  1996/08/28  12:37:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.10.18  1996/08/01  00:35:09  jwjang
 * CSCdi64555:  atmsig is running while no qsaal pvc
 *         CSCdi64664
 * Branch: California_branch
 *
 * Revision 3.7.10.17  1996/07/10  22:20:06  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.7.10.16  1996/06/21  22:00:59  jwjang
 * CSCdi61057:  change sscop_output to normal priority
 * Branch: California_branch
 *
 * Revision 3.7.10.15  1996/06/19  00:56:32  jwjang
 * CSCdi57794:  atmsig setup message for classical ip has different IEs
 * format
 * CSCdi59379
 * Branch: California_branch
 *
 * Revision 3.7.10.14  1996/06/17  22:17:32  cakyol
 * CSCdi59963:  atm sig processes need to run in hi priority.
 *              The "sscop_input" process has been elevated
 *              to run at hi priority.
 *
 * Revision 3.7.10.13  1996/06/11  00:17:53  cakyol
 * CSCdi59963:  atm sig processes need to run in hi priority.
 *              The fix is being backed out pending further
 *              investigation.
 * Branch: California_branch
 *
 * Revision 3.7.10.12  1996/06/08  20:16:57  cakyol
 * CSCdi59963:  atm sig processes need to run in hi priority
 * Branch: California_branch
 *
 * Revision 3.7.10.11  1996/05/31  15:30:39  rzagst
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.7.10.10  1996/05/28  15:31:33  schrupp
 * CSCdi57760:  ATMSIG: Dont free party in FlushPartyQ
 * Branch: California_branch
 *
 * Revision 3.7.10.9  1996/05/24  23:32:44  achopra
 * CSCdi56238:  Linear search for glommable VCs in the vc HASH table has
 *         been changed to a binary search in a WAVL tree maintained per
 *         swidb, which is keyed on NSAPs, AAL5 encapsulation and
 *         protocol type if encapsulation is AAL5MUX.
 * Branch: California_branch
 *
 * Revision 3.7.10.8  1996/05/13  18:41:00  rzagst
 * CSCdi57247:  Burst size handled improperly between AIP and 4500
 * Branch: California_branch
 *
 * Revision 3.7.10.7  1996/05/09  14:10:57  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.10.6.2.1  1996/04/27  06:33:21  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.10.6  1996/04/25  23:02:03  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.7.10.5  1996/04/23  04:47:38  grobelch
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
 * Revision 3.7.10.4  1996/04/03  04:51:34  schrupp
 * CSCdi53125:  ATM signalling leaks memory
 * Branch: California_branch
 * Use malloc_named for ATMSIG to readily identify possible memory culprits.
 *
 * Revision 3.7.10.3.4.1  1996/04/10  23:35:23  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.10.3  1996/03/29  23:42:04  schrupp
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
 * Revision 3.7.10.2  1996/03/29  19:27:25  grobelch
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
 * Revision 3.7.10.1  1996/03/18  18:59:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.3  1996/03/16  06:27:59  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.2  1996/03/07  08:30:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  00:18:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/22  22:46:33  schrupp
 * CSCdi49275:  sscop does not get initialized properly
 *
 * Revision 3.6  1996/02/01  05:59:39  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/30  10:12:32  kartik
 * CSCdi47798:  Signalling should specify the ILMI subsystem in
 * requirement list.
 *
 * Revision 3.4  1996/01/18  19:52:47  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/22  20:44:24  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.2  1995/11/17  08:45:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/08/18  14:23:05  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.8  1995/08/11  19:46:43  schrupp
 * CSCdi38620:  Prohibit SVC opens until ATMSIG processes active
 *
 * Revision 2.7  1995/08/08  01:06:20  percyk
 * CSCdi38366:  move processes to atmsig
 *
 * Revision 2.6  1995/08/04  22:48:35  percyk
 * CSCdi38274:  delete sig processes if no atm port present.
 *
 * Revision 2.5  1995/07/26  15:13:09  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.4  1995/06/28  09:22:14  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/24  14:35:37  schrupp
 * CSCdi36344:  Dont send 2nd SETUP on T303 timeout
 *
 * Revision 2.2  1995/06/18  20:31:38  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:11:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *---------------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../atm/msg_atm.c"		/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "../if/atm.h"
#include "atmsig_api.h"
#include "../if/static_map.h"
#include "atmsig_private.h"
#include "atmsig_public.h"
#include "atmsig_smap.h"
#include "sscop_private.h"
#include "sscop_public.h"
#include "mgd_timers.h"
#include "../snmp/snmp_api.h"
#include "ilmi_api.h"

/*
 * C o n s t a n t s
 */
#define ATMSIG_TRAFFIC_INSTALL_DEFAULT 0

/* START constants for conversion between cps and kbps */

/* cps * BYTES_PER_CELL*BITS_PER_BYTE/1000 = kbps */

#define ATMSIG_BYTES_PER_CELL            53
#define ATMSIG_BITS_PER_BYTE              8
#define ATMSIG_BITS_PER_CELL_MULTIPLIER  ATMSIG_BYTES_PER_CELL
#define ATMSIG_KBPS_DIVISOR (1000/ATMSIG_BITS_PER_BYTE)

#define ATMSIG_MAX_INPUT_SIMPLE_CPS_TO_KBPS_ALG 40518559
#define ATMSIG_MAX_INPUT_SIMPLE_KBPS_TO_CPS_ALG 17179869

#define ATMSIG_KBPS_OF_MAX_INPUT_SIMPLE_CPS_TO_KBPS_ALG ATMSIG_MAX_INPUT_SIMPLE_KBPS_TO_CPS_ALG
#define ATMSIG_CPS_OF_MAX_INPUT_SIMPLE_KBPS_TO_CPS_ALG ATMSIG_MAX_INPUT_SIMPLE_CPS_TO_KBPS_ALG

/* END  constants for conversion between cps and kbps */

/*
 * G l o b a l   S t o r a g e
 */

/* call ref for all connections */
ulong callRefValue;

/* atmSig timer */
mgd_timer atmSig_wakeupTime;

/* svc hash tables */
atmSvcType *atmSig_svcHashTable[ATMSVC_HASH_SIZE];
atmSvcPartyType *atmSig_partyHashTable[ATMSVC_HASH_SIZE];

/* queue for all incoming packets from SSCOP */
watched_queue *atmSig_inputQ;

/* queue for all outgoing packets from signalling clients */
watched_queue *atmSig_outputQ;

/* all svc's with timers started are placed on this Q */
atmSvcPartyType *atmSig_timerQ;

/* permanent svc blocks */
atmSvcType *atmSig_globalSvcBlock;
atmSvcType *atmSig_staticSvcBlock;

/* atmSig process flag */
boolean atmSigRunning = FALSE;

static pid_t sscop_input_pid;
pid_t sscop_output_pid;
static pid_t sscop_timer_pid;
static pid_t atmsig_input_pid;
static pid_t atmsig_output_pid;
pid_t atmsig_timer_pid;

/* Diagnostics variables */
int atmSig_statsNoMemory;         /* no memory */
int atmSig_statsIgnored;          /* ignored frames */

static void atmSig_statusEnquiry(atmSvcType *);



/*
 * a t m S i g _ i n i t ( ).
 *
 * Initialize ATM Signalling structures, spawn processes.
 * Called at startup time.
 */
void atmSig_init (subsystype *subsys)
{

    int i;
    atmSvcPartyType *party;

    for (i = 0; i < ATMSVC_HASH_SIZE; ++i)
       atmSig_svcHashTable[i] = NULL;

    for (i = 0; i < ATMSVC_HASH_SIZE; ++i)
       atmSig_partyHashTable[i] = NULL;

    atmSig_inputQ = create_watched_queue("ATM Sig Input", 0, 0);
    atmSig_outputQ = create_watched_queue("ATM Sig Output", 0, 0);
    atmSig_timerQ = NULL;

    atmSig_debug_init();
    atmSig_parser_init();

    callRefValue = 
    atmSig_statsIgnored = 
    atmSig_statsNoMemory = 0;

    mgd_timer_init_parent(&atmSig_wakeupTime, NULL);

    /* init all atmsig parameters */
    for (i=0; i< MAX_ATM_INTFC; i++) {

      /* initialize signalling vc number */
      atm_db[i].sig_vcnum = 0;
    
      /* fill in timer values */
      atm_db[i].timer_t303   = ATMSIG_T303_DEFAULT;
      atm_db[i].timer_t308   = ATMSIG_T308_DEFAULT;
      atm_db[i].timer_t309   = ATMSIG_T309_DEFAULT;
      atm_db[i].timer_t310   = ATMSIG_T310_DEFAULT;
      atm_db[i].timer_t313   = ATMSIG_T313_DEFAULT;
      atm_db[i].timer_t316   = ATMSIG_T316_DEFAULT; 
      atm_db[i].timer_t322   = ATMSIG_T322_DEFAULT;
      atm_db[i].timer_t398   = ATMSIG_T398_DEFAULT;
      atm_db[i].timer_t399   = ATMSIG_T399_DEFAULT;

      /* default max retry count */
      atm_db[i].atmsig_retry_cnt = ATMSIG_RETRY_DEFAULT;

      /* default to user side on the router */
      atm_db[i].uni_side = ATMSIG_SIDE_USER;

      /* default to UNI 3.0 */
      atm_db[i].uni_version = ATMUNI_VERSION_30;
 
      /* default max connections pending */
      atm_db[i].max_conn_pending = ATMSIG_MAX_CONN_PENDING_DEFAULT;

      /* default strictness of traffic shaping (whether to release */
      /* if driver can't install with signalled parameters).       */
      atm_db[i].atmsig_tr_shape_strict = ATMSIG_STRICT_TRAFFIC_SHAPING_DEFAULT;
    }


    /*
     * Allocate a global svc block, used for responding to messages
     * containing global call references.
     * The global svc block is not put in svc hash table.
     */
    atmSig_globalSvcBlock = malloc( sizeof(atmSvcType) );

    if (atmSig_globalSvcBlock == NULL) {
      return;
    }

    party = malloc( sizeof(atmSvcPartyType) );

    if (party == NULL) {
      free(atmSig_globalSvcBlock);
      return;
    }

    atmSig_globalSvcBlock->rootParty = party;

    /*
     * Allocate a static svc block used for responding to unconnected
     * circuits, etc.  The static svc block is not put in hash table.
     */
    atmSig_staticSvcBlock = malloc( sizeof(atmSvcType) );

    if (atmSig_staticSvcBlock == NULL) {
      return;
    }

    party = malloc( sizeof(atmSvcPartyType) );

    if (party == NULL) {
      free(atmSig_staticSvcBlock);
      return;
    }

    atmSig_staticSvcBlock->rootParty = party;
     
    /* register with ILMI */
    atmSmap_registerIlmi();

    sscop_input_pid   =
    sscop_output_pid  =
    sscop_timer_pid   =
    atmsig_input_pid  =
    atmsig_output_pid =
    atmsig_timer_pid  = 0;

    atmsig_timer_pid = process_create(atmSig_timerProcess, "ATMSIG Timer",
                                                 NORMAL_STACK, PRIO_NORMAL);

    /* ATM Signalling subsystem not initialized, be paranoid */
    atmSigRunning = FALSE;

}




inline boolean atmSig_isV31 (atmSvcType *svc)
{
  return ( (svc->sig_idb->hwptr->sscop->uni_version == ATMUNI_VERSION_31)
	  ? TRUE: FALSE);
}

static atm_uniVersionType atmSig_getUniVersionByPak (paktype *pak)
{
  if (pak)
    return (pak->if_input->hwptr->sscop->uni_version);

  if (atmSig_debugEvent)
    buginf("\nATMSIG: null pak atmSig_checkUniVersionByPak");
  return (ATMUNI_VERSION_INVALID);
}

/*
 * a t m S i g _ f i l l T o s I n S v c ( )
 *
 * Copy type of service parameters from map block into svc block.
 */
void atmSig_fillTosInSvc (atmSvcType *svc, atm_params_type *params)
{


   if (!svc || !params)
      return;

   svc->aal5mux = params->aal5mux;  /* mux or snap vc? */
   /* svc->aal5mux ? (svc->linktype = map->protocol) : 0; */
   svc->layer2id = params->layer2id; 

   /* copy type of service parameters from map block into svc */

   /* AAL Type 5 Parameters - currently only AAL Type supported */
   svc->tos.aalType    = ATM_AALIE_TYPE5;
   svc->tos.sscsType   = ATM_AALIE_SSCS_NULL;
   svc->tos.mode       = ATM_AALIE_MODE_MSG;
   /*
   svc->tos.fwdSduSize = tos->fwdSduSize;
   svc->tos.bwdSduSize = tos->bwdSduSize;
   svc->tos.midSize    = tos->midSize;
   */

   /* AAL User Cell Rate Parameters */
   svc->tos.userCellRate.fwdPeakClp0      =
                ((params->fwpkrate0) == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) ?
                ATMSIG_TRAFFICPARM_NOT_DEFINED_API :
                atmSig_kbps_to_cps(params->fwpkrate0);
 
   svc->tos.userCellRate.bwdPeakClp0      =
                ((params->bkpkrate0) == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) ?
                ATMSIG_TRAFFICPARM_NOT_DEFINED_API :
                atmSig_kbps_to_cps(params->bkpkrate0);
 
   svc->tos.userCellRate.fwdPeakClp01     =
                ((params->fwpkrate1) == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) ?
                ATMSIG_TRAFFICPARM_NOT_DEFINED_API :
                atmSig_kbps_to_cps(params->fwpkrate1);
 
   svc->tos.userCellRate.bwdPeakClp01     =
                ((params->bkpkrate1) == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) ?
                ATMSIG_TRAFFICPARM_NOT_DEFINED_API :
                atmSig_kbps_to_cps(params->bkpkrate1);
 
   svc->tos.userCellRate.fwdSusClp0       =
                ((params->fwstrate0) == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) ?
                ATMSIG_TRAFFICPARM_NOT_DEFINED_API :
                atmSig_kbps_to_cps(params->fwstrate0);
 
   svc->tos.userCellRate.bwdSusClp0       =
                ((params->bkstrate0) == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) ?
                ATMSIG_TRAFFICPARM_NOT_DEFINED_API :
                atmSig_kbps_to_cps(params->bkstrate0);
 
   svc->tos.userCellRate.fwdSusClp01      =
                ((params->fwstrate1) == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) ?
                ATMSIG_TRAFFICPARM_NOT_DEFINED_API :
                atmSig_kbps_to_cps(params->fwstrate1);
 
   svc->tos.userCellRate.bwdSusClp01      =
                ((params->bkstrate1) == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) ?
                ATMSIG_TRAFFICPARM_NOT_DEFINED_API :
                atmSig_kbps_to_cps(params->bkstrate1);

   svc->tos.userCellRate.fwdMaxBurstClp0  = params->fwbsize0;
   svc->tos.userCellRate.bwdMaxBurstClp0  = params->bkbsize0;
   svc->tos.userCellRate.fwdMaxBurstClp01 = params->fwbsize1;
   svc->tos.userCellRate.bwdMaxBurstClp01 = params->bkbsize1;

   /* for now don't tag */
   svc->tos.userCellRate.FwdTag           = 
   svc->tos.userCellRate.BwdTag           = -1;

   /* Broadband Bearer Capability Parameters */
   /*
   svc->tos.bearerClass = tos->bearerClass;
   svc->tos.trafficType = tos->trafficType;
   svc->tos.timingRequirements = tos->timingRequirements;
   svc->tos.clipping = tos->clipping;
   svc->tos.usrPlaneAccessConfig = tos->usrPlaneAccessConfig;
   */

   /* Quality of Service Parameters */
   /*
   svc->tos.txQosClassFwd = tos->txQosClassFwd;
   svc->tos.txQosClassFwd = tos->txQosClassFwd;
   */

}



/*
 * s s c o p _ e n q u e u e ( )
 *
 * sscop queues a Q.2931 message onto the Q.2931 input queue. 
 */
void sscop_enqueue (paktype *pak, boolean poll)
{

   sscop_sdPdu *pdu;
   uchar *frameptr;
   ulong pad;

 
   /* Get the sscop header */
   frameptr = pak->datagramstart + pak->datagramsize - SSCOP_HEADER_SIZE;
   pdu = (sscop_sdPdu *)frameptr;

   /* shorten data length by pad bytes */
   pad = pdu->type_ns & SSCOP_PADBYTE_MASK;
   pak->datagramsize -= (pad >> 30);

   /*
    * shorten pak size by SSCOP header size, no need to adjust 
    * pak->datagramstart as SSCOP headers are at end of packet.
    */
   if (poll)
      pak->datagramsize -= sizeof(sscop_sdpPdu);
   else
      pak->datagramsize -= sizeof(sscop_sdPdu);

   /* hand it off to Q.2931's input process */
   process_enqueue_pak(atmSig_inputQ, pak);

}



/*
 * a t m S i g _ i n p u t P r o c e s s ( )
 *
 * This process blocks - waiting for incoming packets from SSCOP 
 */
forktype atmSig_inputProcess (void)
{
    ulong major, minor;
    paktype *pak;

    /* wait for system to initialize */
    process_wait_on_system_init();
    
    process_watch_queue(atmSig_inputQ, ENABLE, RECURRING);
    for (;;) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		while ( (pak = process_dequeue(atmSig_inputQ) ) != NULL)
		    atmSig_input(pak);
		break;
		
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }

}



/*
 * a t m S i g _ i n p u t  ( )
 *
 * main received packet processing routine.
 * pak->datagramstart points to protocol discriminator - the start
 * of the signalling message,
 */
void atmSig_input (paktype *pak)
{

    atmSig_eventtype newevent;
    atmSvcType *svc = NULL;
    hwidbtype *idb;
    boolean partyMsg;
    atmSvcPartyType *party = NULL;

 
    if (atmSig_debugAll || atmSig_debugPacket)
       atmSig_hexout(pak->datagramstart, pak->datagramsize, "ATMSIG(I)");

    idb = pak->if_input->hwptr;

    /* remember time of last input, save it in packet too */
    GET_TIMESTAMP(pak->inputtime); 
    idb->lastinput = pak->inputtime;

    if ( !atmSigRunning || !atm_interface_up(pak->if_input) ) {
       if (atmSig_debugAll || atmSig_debugEvent)
          buginf("\nATMSIG: atmSig_input: ATM Signalling %s, idb state %s",
                           atmSigRunning ? "enabled":"disabled", 
                           atm_interface_up(pak->if_input) ? "UP":"DOWN");
       datagram_done(pak);
       ++atmSig_statsIgnored;
       return;
    }

    /* determine type of message received. */
    newevent = atmSig_findMsgType(pak);

    /* msg header invalid? - free packet and return (message ignored).  */
    if (newevent >= ATMSIG_RCV_INVALIDMSG) {
       datagram_done(pak);
       ++atmSig_statsIgnored;
       return;
    }

    /* 
     * Select either global, static or dynamic svc block depending on the
     * scope of the message.
     */ 
    svc = atmSig_selectSvcBlock(pak);
    if (!svc) {
      /* 
       * SVC block not found, free packet and return - 
       */
      if (atmSig_debugAll || atmSig_debugEvent)
	buginf("\nATMSIGERR: atmSig_input: svc block not found");
      datagram_done(pak);
      return;
    }

    /*
     * We have now identified the source of the incoming frame OR
     * are using static/global SVC block should a response be needed.
     */
    svc->inbuf = pak;
    svc->event = newevent;

    /* 
     * Point-point or Multipoint Msg? 
     * PTP, use stateTable, MTP, use multiPointStateTable
     */
    partyMsg = atmSig_partyMsg(newevent);

    /* Status and Status Enq messages could be either PTP or MTP */
    if ( (newevent == ATMSIG_RCV_STATUS) ||
         (newevent == ATMSIG_RCV_STATUS_ENQUIRY) ) {
      if (svc->multiPoint) {
        partyMsg = TRUE;
      }
    }


    if (!partyMsg) {
      atmSig_stateTable(svc);
    } else {
      party = atmSig_selectPartyBlock(pak, svc);
      if (party)
	atmSig_multiPointStateTable(svc, party);
    }

    if (svc && svc->inbuf) {
       datagram_done(svc->inbuf);
       svc->inbuf = NULL;
    }

    /* localize freeing of party and svc (don't free them from everywhere) */
    if (party && party->freeParty) 
      atmSig_freePartyBlock(svc, party);

    if (svc->freeSvc)
      atmSig_freeSvcBlock(svc);

}



/*
 * a t m S i g _ o u t p u t P r o c e s s ( )
 *
 * This process blocks - waiting for outgoing packets from a higher layer
 * application.
 */
forktype atmSig_outputProcess (void)
{
    ulong major, minor;
    sig_api_msg *msg;

    /* wait for system to initialize */
    process_wait_on_system_init();
    
    process_watch_queue(atmSig_outputQ, ENABLE, RECURRING);
    for (;;) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		while ( (msg = process_dequeue(atmSig_outputQ) ) != NULL)
		    atmSig_output(msg);
		break;
		
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}



/*
 * a t m S i g _ s e l e c t S v c B l o c k ( )
 *
 * Select the SVC block depending on the scope of the message.
 * If the message has a global scope - use atmSig_globalSvcBlock.
 * If the message has local scope - try and get it from the SVC Hash Table.
 * For an invalid message - use atmSig_staticSvcBlock.
 * Returns a valid SVC block or NULL if no memory.  
 */
atmSvcType *atmSig_selectSvcBlock (paktype *pak)
{

   atmSig_msgHeader *hdr;
   atmSvcType *svc = NULL;
   boolean locallyInitiatedCall;
   ulong callref;
   atmSvcPartyType *party = NULL;
   short endpointRef;


   hdr = (atmSig_msgHeader *)pak->datagramstart;
   callref = GETLONG(&hdr->callref[0]);

   /* 
    * Message has local scope.  Find out who initiated the call - 
    * if Call Reference Flag set, we initiated the call.
    * if Call Reference Flag not set, the switch initiated the call.
    */
   if (callref & ATM_CALLREF_FLAGMASK)
      locallyInitiatedCall = TRUE;
   else
      locallyInitiatedCall = FALSE;

   /*
    * Check if Message has global scope.  (Currently only 3 messages have global 
    * scope).  A call ref of 0 is not sufficient for a msg to have global scope,
    * we also need to check Restart Indicators to truly find the scope of RESTART
    * messages.
    */

   /* check for dummy call reference */
   if ( (callref & ATM_CALLREF_VALUEMASK) == ATM_CALLREF_DUMMY ) {
     /* make sure this is UNI 3.1. printout error otherwise */
     if (atmSig_getUniVersionByPak(pak) == ATMUNI_VERSION_30) {
       if (atmSig_debugEvent)
	 buginf("\nATMSIG: i dummy call ref in uni 3.0");
     }
     return (NULL);
   }

   if ( (callref & ATM_CALLREF_VALUEMASK) == ATM_CALLREF_GLOBAL ) {
      /* 
       * Only STATUS, RESTART and RESTART ACK messages can have global call
       * reference values.
       */
      if ( (hdr->msgtype != ATMSIG_MT_RESTART)     && 
           (hdr->msgtype != ATMSIG_MT_RESTART_ACK) && 
           (hdr->msgtype != ATMSIG_MT_STATUS) ) {
         /* 
          * Invalid msg with global call ref - return a null svc block, system 
          * will ignore the packet.
          */
         return (NULL); 
      }

      /* Status messages with Call Ref = 0 always have global scope */
      if (hdr->msgtype == ATMSIG_MT_STATUS) {
         svc = atmSig_globalSvcBlock;
         svc->state = ATMSIG_ST_RESTART_REQ;
         svc->sig_idb = pak->if_input;
      }

      /*
       * Restart messages can have either local or global scope.  Restart
       * Indicator IE in Restart messages tells us the scope of the message.
       */
      if ( (hdr->msgtype == ATMSIG_MT_RESTART) || 
                      (hdr->msgtype == ATMSIG_MT_RESTART_ACK) ) {
         if ( atmSig_isMessageGlobal(hdr) ) {  
            svc = atmSig_globalSvcBlock;
            svc->state = ATMSIG_ST_RESTART_REQ;
            svc->sig_idb = pak->if_input; 
	    svc->locallyInitiatedCall = locallyInitiatedCall;
         }   
         else
            goto local_scope;
      }

      return (svc);
   }

local_scope:

   /* Try getting the svc block from the svc hash table by hashing on call ref */
   svc = atmSig_findSvcBlockByCR(callref & ATM_CALLREF_VALUEMASK, pak->if_input,
                                                        locallyInitiatedCall);

   /* No svc block found, allocate one ONLY if incoming message is a SETUP */
   if (!svc) {

      if (hdr->msgtype == ATMSIG_MT_SETUP) {
         /* We have a SETUP message - try to allocate a SVC control block. */
         svc = atmSig_allocateSvcBlock();

         if (!svc)
            return (NULL);

	 /* need to allocate party block also */
	 party = atmSig_allocatePartyBlock();

	 if (!party) {
	   free(svc);
	   return (NULL);
	 }

         /*
          * The default SDU size is the subif MTU. The default value
          * will be replaced if the AAL5 Parameters IE is included in
          * the SETUP message.
          */
         svc->tos.fwdSduSize =
         svc->tos.bwdSduSize = pak->if_input->sub_mtu + ATM_SNAP_HDRBYTES;

         svc->locallyInitiatedCall = FALSE;
         svc->callRef = callref & ATM_CALLREF_VALUEMASK;
         svc->sig_idb = pak->if_input;
         svc->state = ATMSIG_ST_CALL_PRESENT;
	 svc->rootParty = party;
	 party->svc = svc;
         atmSig_enterHashEntry(svc);

	 /* Check SETUP for EndpointRef, if so, is MPT SETUP */
	 endpointRef = atmSig_findEndpointRef(svc, pak);

	 if (endpointRef == -1) {
	   svc->multiPoint = FALSE;
	 }
	 else {
	   svc->multiPoint = TRUE;
	   party->endpointRef = endpointRef;
	   party->partyState = ATMSIG_PST_NULL;
	   atmSig_enterPartyHashEntry(svc, party);   
	   atmSig_enterPartyBlock(svc, party);    
	 }

      } else {
           /* use static svc block to tx RELEASE COMPLETE for this invalid msg */
           svc = atmSig_staticSvcBlock;
           svc->locallyInitiatedCall = locallyInitiatedCall;
           svc->callRef = callref & ATM_CALLREF_VALUEMASK;
           svc->sig_idb = pak->if_input;
           svc->state = ATMSIG_ST_NULL;
      }

   }

   return (svc);

}



/*
 * a t m S i g _ h a s h C a l l R e f ( )
 *
 * Generate a hash table index from the call reference value.  
 */
int atmSig_hashCallRef (ulong callref)
{

   uint value;

   value = (callref & 0x00ffffff);

   return (value & ATMSIG_HASH_MASK);

}



/*
 * a t m S i g _ a l l o c a t e C a l l R e f ( )
 *
 * Allocate a call reference value to a newly locally initiated call.  'callRefValue'
 * is a global variable incremented and a check is made if it's currently in use by
 * scanning the svc hash table.  This continues till a free call reference value is
 * found.
 * Returns TRUE if call ref allocated, FALSE otherwise. 
 */
boolean atmSig_allocateCallRef (atmSvcType *svc) 
{

   boolean notfound = TRUE;
   atmSvcType *tmp;
   uint maxvc;


   /* maxvc is used as a defensive mechanism to prevent infinite loops */
   maxvc = 0;

   while (notfound) {

      /* zero is a reserved call reference value for global use - don't use it!!. */
      if (callRefValue == 0x007FFFFF)
         callRefValue = 1;
      else
         ++callRefValue;

      /*
       * check if call ref value is currently being used by another svc.  If
       * so keep on trying try till we get an unused one. 
       */
      tmp = atmSig_findSvcBlockByCR(callRefValue, svc->sig_idb, TRUE);
      if (!tmp)
         notfound = FALSE;

      /*
       * this should NEVER happen as we will run out of computing resources much
       * sooner than call reference values!!  This condition probably indicates an 
       * infinite loop in the svc hash table.
       */
      if (++maxvc > ATMSIG_MAX_SVC) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMSIG: cannot find a call reference value to allocate");
         return (FALSE);
      }
 
   }

   /* found an unused call reference value!! */ 
   svc->callRef = callRefValue;
   svc->locallyInitiatedCall = TRUE;
   return (TRUE);

}


/*
 * a t m S i g _ i n i t T o s
 * initialize TOS values to don't care. These will be overwritten
 * by IE traffic parameters if they are received in an incoming SETUP.
 */
static void atmSig_initTos (atmSvcType *svc)
{
    svc->tos.userCellRate.fwdPeakClp0  = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.bwdPeakClp0  = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.fwdPeakClp01 = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.bwdPeakClp01 = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.fwdSusClp0   = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.bwdSusClp0   = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.fwdSusClp01  = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.bwdSusClp01  = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.fwdMaxBurstClp0  = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.bwdMaxBurstClp0  = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.fwdMaxBurstClp01 = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.bwdMaxBurstClp01 = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.FwdTag       = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;
    svc->tos.userCellRate.BwdTag       = ATMSIG_TRAFFICPARM_NOT_DEFINED_API;

    svc->tos.mode = ATM_AALIE_MODE_INVALID;

}


/*
 * a t m S i g _ a l l o c a t e S v c B l o c k ( )
 *
 * malloc a svc block.
 */
atmSvcType *atmSig_allocateSvcBlock (void)
{

   atmSvcType *svc = NULL;


   /* get svc block from system memory */
   svc = malloc_named( sizeof(atmSvcType), "ATMSIG-SVC");
   if (svc == NULL) {
     ++atmSig_statsNoMemory;
     return (NULL);
   }

   memset( svc, 0, sizeof(atmSvcType) );
   atmSig_initTos(svc);
   mgd_timer_init_leaf(&svc->statusEnqTimer, &atmSig_wakeupTime,
                       ATMSIG_SYNC_TIMER, svc, FALSE);
   return (svc);

}
   
 
 
/*
 * a t m S i g _ f r e e S v c B l o c k ( )
 *
 * Stop timer if running.  Free any data packet hinged onto svc.
 * Remove svc entry from the driver and from the hash table.  
 * Remove vc number from static map.  Return svc block to system memory.
 */
void atmSig_freeSvcBlock (atmSvcType *svc)
{

  atmSvcPartyType *party;


   /* sanity check */
   if (!svc) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: freeSvcBlock - null svc being freed!");
      return;
   }

   /* stop  timer as early as possible */
   mgd_timer_stop(&svc->statusEnqTimer);
   /* if any input packet hinged onto svc - free it. */
   if (svc->inbuf) {
      datagram_done(svc->inbuf);
      svc->inbuf = NULL;
   }

   /* if multipoint, free any remaining parties */
   if (svc->multiPoint)
      atmSig_freeAllParties(svc);

   party = svc->rootParty;

   /* stop any timers if active. */
   if (party) 
      atmSig_stopTimer(party);

   /* don't free static or global svc block */
   if ( (svc == atmSig_globalSvcBlock) || (svc == atmSig_staticSvcBlock) )
      return;

   /* request ATM driver to remove vc entry (if it exists) */
  atmSig_removeVcFromDriver(svc);

   /* notify client via Release Complete indication if still present */
   if (svc->clientPresent) {
      svc->state = ATMSIG_ST_RELEASE_REQ;
      atmSig_notifyReleaseCompleteIndication(svc);
   }

  /* should only have rootParty left, if multipoint clear from hash table */
  if (party) {
    if (svc->multiPoint) {
      atmSig_removePartyHashEntry(svc, party);

      /* Stop conn_pending if still active */
      if ( (party->conn_pending) &&
	  (svc->sig_idb->hwptr->atm_db->conn_pending > 0) ) {
	--svc->sig_idb->hwptr->atm_db->conn_pending;
      }
    }
    free(party);
  }
  else {
    if (atmSig_debugAll || atmSig_debugError)
      buginf("\nATMMP: NO ROOT PARTY");
  }
      
   /* stop conn pending */
   if ( (svc->conn_pending) &&
                (svc->sig_idb->hwptr->atm_db->conn_pending > 0) ) {
     --svc->sig_idb->hwptr->atm_db->conn_pending;
     svc->conn_pending = FALSE;
   }

   /* out of hash table and into free pool */
   if ( atmSig_removeHashEntry(svc) ) {
     free(svc);
   }

}


/*
 * a t m S i g _ r e m o v e V c F r o m D r i v e r ( ) 
 *
 * Remove svc entry from the driver.
 */
void atmSig_removeVcFromDriver (atmSvcType *svc)
{

  /* Disable blocking before calling driver */
  blocking_disabled = TRUE;
  
  /* request ATM driver to remove vc entry (if it exists) */
  if (svc->vcnum) {
    if (atmSig_debugEvent) {
      buginf("\nATMSIG: removeVC - vcnum = %d, vpi/vci = %d/%d", 
	     svc->vcnum, svc->vpci, svc->vci);
    }
    /* 
     * atm_remove_vc() does not return error code - so can't check if 
     * call succeeded or not.  
     */
    atm_remove_vc(svc->sig_idb->hwptr, svc->vcnum);
  }
  blocking_disabled = FALSE;
}



/*
 * s e l e c t R a t e T r a f f i c P a r m ( )
 *
 * Determine a traffic parameter cell-rate value to use for
 * installing a vc in hardware.
 * Chooses between clp0, and clp01 parameter (preferring clp01),
 * and the not-defined value. Converts cells/second to kbps,
 * which is what driver needs.
 */
static uint selectRateTrafficParm (long val_clp01, long val_clp0)
{
   if (val_clp01 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
       return(atmSig_cps_to_kbps(val_clp01));
   else { /* 01 not defined. Check 0. */
      if (val_clp0 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
          return(atmSig_cps_to_kbps(val_clp0));
   }
   /* neither clp01 nor clp0 have non-default values, return default. */
   return(ATMSIG_TRAFFIC_INSTALL_DEFAULT);
}

 
/*
 * s e l e c t B u r s t T r a f f i c P a r m ( )
 *
 * Determine a traffic parameter burst value to use for
 * installing a vc in hardware.
 * Chooses between clp0, and clp01 parameter (preferring clp01),
 * and the not-defined value. Converts cells to cells/32, which
 * is what driver needs.
 */
static uint selectBurstTrafficParm (long val_clp01, long val_clp0)
{
   if (val_clp01 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
       return(val_clp01);
   else { /* 01 not defined. Check 0. */
      if (val_clp0 != ATMSIG_TRAFFICPARM_NOT_DEFINED_API)
          return(val_clp0);
   }
   /* neither clp01 nor clp0 have non-default values, return default. */
   return(ATMSIG_TRAFFIC_INSTALL_DEFAULT);
}


/*
 * a t m S i g _ s e l e c t S h a p i n g P a r m s ( )
 *
 * Determine traffic shaping parameters to use for a vc.
 * NOTE: the input rate parameters (in tos struct) are specified
 *       in units of cells-per-second. The rate values returned by
 *       this function are specified in units of kilobits-per-second.
 */
static void atmSig_selectShapingParms (atmSvcType *svc,
                                       uint *pcr, uint *scr, uint *burst)
{
   long pcr0, pcr01, scr0, scr01, burst0, burst01;
   uint ret_pcr, ret_scr, ret_burst;
 
   if (svc->locallyInitiatedCall) {
      pcr0  = svc->tos.userCellRate.fwdPeakClp0;
      pcr01 = svc->tos.userCellRate.fwdPeakClp01;
      scr0  = svc->tos.userCellRate.fwdSusClp0;
      scr01 = svc->tos.userCellRate.fwdSusClp01;
      burst0  = svc->tos.userCellRate.fwdMaxBurstClp0;
      burst01 = svc->tos.userCellRate.fwdMaxBurstClp01;
   } else { /* we received the setup. */
      pcr0  = svc->tos.userCellRate.bwdPeakClp0;
      pcr01 = svc->tos.userCellRate.bwdPeakClp01;
      scr0  = svc->tos.userCellRate.bwdSusClp0;
      scr01 = svc->tos.userCellRate.bwdSusClp01;
      burst0  = svc->tos.userCellRate.bwdMaxBurstClp0;
      burst01 = svc->tos.userCellRate.bwdMaxBurstClp01;
   }
 
   /* now choose which cell-rates to use. Try to find a non-default value,   */
   /* else just zero (which means shaping default). In finding these values: */
   /*   - if none of pcr01/pcr0/scr01/scr0 are defined, then use default     */
   /*     shaping.                                                           */
   /*   - if pcr01 = 1, and none of pcr0, scr01, scr0 are defined, then      */
   /*     we assume that this is indicates that the pcr was signalled by us  */
   /*     as the default shaping (unspecified traffic) case.                 */
   /*   - if xcr01/xcr0 are both defined, prefer xcr01.                      */
 
   if (((pcr01 == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) ||
       (pcr01 == ATMSIG_PCR01_NOT_DEFINED_IN_SETUP_1) ||
       (pcr01 == ATMSIG_PCR01_NOT_DEFINED_IN_SETUP_ALL_1)) &&
       (pcr0  == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) &&
       (scr01 == ATMSIG_TRAFFICPARM_NOT_DEFINED_API) &&
       (scr0  == ATMSIG_TRAFFICPARM_NOT_DEFINED_API)) {
      ret_pcr   = ATMSIG_TRAFFIC_INSTALL_DEFAULT;
      ret_scr   = ATMSIG_TRAFFIC_INSTALL_DEFAULT;
      ret_burst = ATMSIG_TRAFFIC_INSTALL_DEFAULT;
   } else {
      ret_pcr   = selectRateTrafficParm(pcr01, pcr0);
      ret_scr   = selectRateTrafficParm(scr01, scr0);
      ret_burst = selectBurstTrafficParm(burst01, burst0);
   }
 
   /* try to be most accomodating in whether pcr/scr are both defined,     */
   /* though this should be ruled out in processing the ie or defining the */
   /* map-class. */
   if ((ret_pcr != ATMSIG_TRAFFIC_INSTALL_DEFAULT) &&
       (ret_scr == ATMSIG_TRAFFIC_INSTALL_DEFAULT))
      ret_scr = ret_pcr;
 
   if ((ret_scr != ATMSIG_TRAFFIC_INSTALL_DEFAULT) &&
       (ret_pcr == ATMSIG_TRAFFIC_INSTALL_DEFAULT))
      ret_pcr = ret_scr;
 
   *pcr   = ret_pcr;
   *scr   = ret_scr;
   *burst = ret_burst;
}


/*
 * a t m S i g _ r e q V c F r o m D r i v e r ( )
 *
 * Request ATM driver to create a vc.
 */
boolean atmSig_reqVcFromDriver (atmSvcType *svc, vcd_t vcnum)
{

   vc_info_t *vc_blk;
   atmSvcPartyType *party;
   uint            pcr, scr, burst;
   boolean         trafficShapingStrict;
   unsigned        flag;
   uint            params_ok;


   /* sanity check */
   if (!svc) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: reqVcFromDriver - no svc block");
      return (FALSE);
   }

   party = svc->rootParty;

   if (!party) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: reqVcFromDriver - no party block");
      return (FALSE);
   }

   atmSig_selectShapingParms(svc,&pcr,&scr,&burst);

   params_ok = (*svc->sig_idb->hwptr->atm_db->funcs.qos_params_verify)
       (svc->sig_idb->hwptr,pcr,scr,burst);
   
   flag = svc->flags | VC_FLAG_SVC;
   if (svc->multiPoint)
	flag |= VC_FLAG_MULTIPOINT;
 
   if (params_ok != ATM_TRAFFIC_SHAPING_VERIFIED) {
       /* check qos parameters and return fail if not valid
	* do not display FAILCREATEVC error message since signalling always
	* tries again with the default case.
	*/
       trafficShapingStrict =
	   svc->sig_idb->hwptr->atm_db->atmsig_tr_shape_strict;

       if (atmSig_debugAll || atmSig_debugEvent)
	   buginf("\nATMSIG: cannot create vc with non-default parms.%s",
		  trafficShapingStrict ? " Strict shaping, no defaults" : " ");

       if ((params_ok & ATM_OUT_OF_RATE_QUEUES) != 0) {
	   if (atmSig_debugAll || atmSig_debugEvent) {
	       buginf ("\nATMSIG: Interface is out of rate queues :"
		       "(%s):",svc->sig_idb->hwptr->hw_namestring,vcnum);
	   }
       }

       if ((params_ok & ATM_PEAKRATE_ERROR) != 0) {
	   if (atmSig_debugAll || atmSig_debugEvent) {
	       buginf ("\nATMSIG: Invalid peak rate of %d requested:"
		       "(%s): vc:%d",pcr,svc->sig_idb->hwptr->
		       hw_namestring,vcnum);
	   }
       }
 
       if ((params_ok & ATM_AVGRATE_ERROR) != 0) {
      if (atmSig_debugAll || atmSig_debugEvent)
	       buginf ("\nATMSIG: Invalid average rate of %d requested: (%s): "
		       "vc:%d",
		       scr,svc->sig_idb->hwptr->hw_namestring,vcnum);
       }

       if ((params_ok & ATM_BURST_ERROR) != 0) {
	   if (atmSig_debugAll || atmSig_debugEvent)
	       buginf ("\nATMSIG: Invalid burst size of %d cells requested:"
		       " (%s): vc:%d",
		       burst,svc->sig_idb->hwptr->hw_namestring,vcnum);
   }
 

       if (trafficShapingStrict) {
	   errmsg(&msgsym(UNSUPPORTED_QOS_PARAMS,ATM),
		  svc->sig_idb->hwptr->hw_namestring);
	   return(FALSE);
       }	   

       /* use default values since parameters invalid */
       pcr = 0;
       scr = 0;
       burst = 0;
   }

   vc_blk = atm_create_vc(svc->sig_idb->hwptr,
                          vcnum, svc->vpci, svc->vci,
                          party->destNsap,
                          0, 0, 0,
                          flag,
                          svc->linktype,
                          svc->user_vc_type,
                          svc->idb,
                          pcr, scr, burst, 0, 0,
                          /*ATM_VC_INARP*/ 0, /* OAM */ 0);

   /* vc created? */
   if (vc_blk) 
      return(TRUE); /* success ! */

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: reqVcFromDriver - driver unable to create vc %d."
	     ,vcnum);

     return (FALSE);

}



/*
 * a t m S i g _ f i n d S v c B l o c k B y C R ( )
 *
 * Find the svc block in the svc hash table by call reference.  
 * The call ref value is hashed to find the svc hash table index.
 */
atmSvcType *atmSig_findSvcBlockByCR (ulong callRef, idbtype *idb,
                                        boolean locallyInitiatedCall)
{

   atmSvcType *svc;
   leveltype level;
   ushort index;


   index = atmSig_hashCallRef(callRef);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: index = %d, callref = %d, lic = %s", 
             index, callRef, locallyInitiatedCall ? "TRUE":"FALSE");
 
   level = raise_interrupt_level(NETS_DISABLE);
   svc = atmSig_svcHashTable[index];

   while (svc) {
      if ( (svc->callRef == callRef) && (svc->sig_idb == idb) &&
           (svc->locallyInitiatedCall == locallyInitiatedCall) ) {
         reset_interrupt_level(level);
         return (svc);
      }
      svc = svc->next_hash;
   }

   reset_interrupt_level(level);
   return (NULL);

}



/*
 * a t m S i g _ f i n d S v c B l o c k B y A t m A d d r ( )
 *
 * Atm addresses are unique across the entire ATM network. 
 */
/*
atmSvcType *atmSig_findSvcBlockByAtmAddr (uchar *addr)
{

   int i;
   atmSvcType *svc;
   leveltype level;


   level = raise_interrupt_level(NETS_DISABLE);
   for (i = 0; i < ATMSVC_HASH_SIZE; ++i) {
      svc = atmSig_svcHashTable[i];

      while (svc) {
         if ( bcmp(party->destNsap.addr, addr, STATIONLEN_ATMNSAP) == 0 ) {
            reset_interrupt_level(level);
            return (svc);
         }
         else
            svc = svc->next_hash;
      }
   }
   reset_interrupt_level(level);

   return (NULL);

}

*/

/*
 * a t m S i g _ f i n d S v c B l o c k B y V C ( )
 *
 * Find the svc block in the svc hash table by vcnum and port number.
 */
atmSvcType *atmSig_findSvcBlockByVC (idbtype *idb, vcd_t vcnum)
{

   atmSvcType *svc;
   leveltype level;
   int i;


   level = raise_interrupt_level(NETS_DISABLE);
   for (i = 0; i < ATMSVC_HASH_SIZE; ++i) {
      svc = atmSig_svcHashTable[i];

      while (svc) {
         if ( (svc->idb == idb) && (svc->vcnum == vcnum) ) { 
            reset_interrupt_level(level);
            return (svc);
         }
         svc = svc->next_hash;
      }
   }
   reset_interrupt_level(level);
   return (NULL);

}



/*
 * a t m S i g _ e n t e r H a s h E n t r y ( )
 *
 * Enter svc block into the svc hash table.
 */
void atmSig_enterHashEntry (atmSvcType *svc)
{

   int index;
   leveltype level;

   
   if (!svc) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: enterHashEntry: null svc block");
      return;
   }

   index = atmSig_hashCallRef(svc->callRef);

   level = raise_interrupt_level(NETS_DISABLE);

   svc->next_hash = atmSig_svcHashTable[index];
   atmSig_svcHashTable[index] = svc;

   reset_interrupt_level(level);

   if (atmSig_debugAll || atmSig_debugEvent) {
      buginf("\nATMMTP: enter Hash: index = %d, call ref = %d, lic = %s", 
        index, svc->callRef, svc->locallyInitiatedCall ? "TRUE":"FALSE");
   }

}



/*
 * a t m S i g _ r e m o v e H a s h E n t r y ( )
 *
 * Removes svc entry from the hash table (if present).
 * Returns TRUE if successfully removed, FALSE otherwise.
 */
boolean atmSig_removeHashEntry (atmSvcType *target)
{

    int n;
    atmSvcType *svc, **prev;
    leveltype level;

    n = atmSig_hashCallRef(target->callRef);
    level = raise_interrupt_level(NETS_DISABLE);
    prev = &atmSig_svcHashTable[n];
    svc = *prev;

    while (svc) {
       if (svc == target) {
          *prev = svc->next_hash;
          svc->next_hash = NULL;
          reset_interrupt_level(level);
          if (atmSig_debugAll || atmSig_debugEvent)
             buginf("\nATMSIG: removeHashEntry: svc remvd from hash table");
          return (TRUE);
       }
       prev = &svc->next_hash;
       svc = svc->next_hash;
    }

    reset_interrupt_level(level);
    if (atmSig_debugAll || atmSig_debugError)
        buginf("\nATMSIG: removeHashEntry: svc %x not found", target);
    return (FALSE);

}



/*
 * a t m S i g _ v a l i d a t e S v c ( )
 *
 * Verify that a valid svc entry exists in the hash table.
 * Returns TRUE if svc found, FALSE otherwise.
 */
boolean atmSig_validateSvc (atmSvcType *target)
{

    int index;
    atmSvcType *svc;
    leveltype level;


    if (!target)
       return (FALSE);

    index = atmSig_hashCallRef(target->callRef);
    level = raise_interrupt_level(NETS_DISABLE);
    svc = atmSig_svcHashTable[index];

    while (svc) {
       if (svc == target) {
          reset_interrupt_level(level);
          return (TRUE);
       }
       svc = svc->next_hash;
    }

    reset_interrupt_level(level);
    return (FALSE);

}



/*
 * a t m S i g _ g l o b a l R e s e t ( )
 *
 * Resets ALL svc's (associated with ALL ATM interfaces).
 */
void atmSig_globalReset (void)
{

   int i;
   atmSvcType *svc, *svc1;
   leveltype level;

   
   level = raise_interrupt_level(NETS_DISABLE);
   for (i = 0; i < ATMSVC_HASH_SIZE; ++i) {

      svc = atmSig_svcHashTable[i]; 

      while (svc) {
         svc1 = svc;
         svc = svc->next_hash;
         atmSig_freeSvcBlock(svc1);
      }

   }
   reset_interrupt_level(level);

}


/*
 * a t m S i g _ i n t e r f a c e R e s e t ( )
 *
 * Resets ALL svc's associated with a particular interface.
 * This function is called when a global Restart Message is received from the
 * ATM switch or the ATM interface is being reset (e.g. changing the mtu size).
 * Note - interrupts are NOT disabled - ALL signalling processing takes place
 * at process level - processes run to completion. 
 */
void atmSig_interfaceReset (hwidbtype *hwidb)
{

   int i;
   atmSvcType **prev, *svc, *svc1;
   hwidbtype *idb;
   boolean found; 


   for (i = 0; i < ATMSVC_HASH_SIZE; ++i) {

      prev = &atmSig_svcHashTable[i];
      svc = *prev;

      while (svc) {
         /* remove svc only if there is a port match */
         if (svc->sig_idb->hwptr == hwidb) {
            svc1 = svc;
            svc = svc->next_hash;
            if (atmSig_debugAll || atmSig_debugEvent)
               buginf("\nATMSIG:atmSig_interfaceReset: removing svc %d", svc1);
            atmSig_freeSvcBlock(svc1);
         }
         else {
            prev = &svc->next_hash;
            svc = svc->next_hash;
         }
      }

   }

   /* now check if sig & sscop processes need to be started/deleted */
   found = FALSE;

   /* ATM port present? */
   FOR_ALL_HWIDBS(idb) {

      /* ATM idb? */
      if ( !(idb->status & IDB_ATM) )
         continue;

      /* atleast one ATM port exists in this system */
      found = TRUE;
      break;
   }

   if (found) {
      /* atm port present, start sig & sscop processes if not already running*/
      if ( !process_exists(sscop_input_pid) ) {
         sscop_input_pid = process_create(sscop_inputProcess, "SSCOP Input",
                                                 NORMAL_STACK, PRIO_HIGH);
         sscop_output_pid = process_create(sscop_outputProcess, "SSCOP Output",
                                                 NORMAL_STACK, PRIO_NORMAL);
         sscop_timer_pid = process_create(sscop_timerProcess, "SSCOP Timer",
                                                 NORMAL_STACK, PRIO_NORMAL);
         atmsig_input_pid = process_create(atmSig_inputProcess, "ATMSIG Input",
                                                 NORMAL_STACK, PRIO_NORMAL);
         atmsig_output_pid = process_create(atmSig_outputProcess, "ATMSIG Output",
                                                 NORMAL_STACK, PRIO_NORMAL);

	 /* ATM Signalling subsystem initialized */
	 atmSigRunning = TRUE;

      }
   }
   else {
      /* no atm port present, delete sig & sscop processes if running */
      if ( process_exists(sscop_input_pid) ) {
         process_kill(sscop_input_pid);
         process_kill(sscop_output_pid);
         process_kill(sscop_timer_pid);
         process_kill(atmsig_input_pid);
         process_kill(atmsig_output_pid);
         process_kill(atmsig_timer_pid);

	 /* ATM Signalling subsystem killed */
	 atmSigRunning = FALSE;

      }
   }

}


static void atmSig_sscopFail (hwidbtype *hwidb)
{

   int i;
   atmSvcType **prev, *svc, *svc1;

   if (atmSig_debugEvent || atmSig_debugError)
     buginf("\nATMSIG(%d), teardown non-active SVCs",
	    hwidb->hw_if_index);

   for (i = 0; i < ATMSVC_HASH_SIZE; ++i) {

      prev = &atmSig_svcHashTable[i];
      svc = *prev;

      while (svc) {
         /* remove svc only if it's not in active state */
         if ( (svc->state != ATMSIG_ST_ACTIVE) && 
	     (svc->sig_idb->hwptr == hwidb) ) {
            svc1 = svc;
            svc = svc->next_hash;
            if (atmSig_debugError)
               buginf("\nATMSIG:atmSig_sscopFail: removing svc vpi %d vci %d", 
		      svc1->vpci,svc1->vci);
            atmSig_freeSvcBlock(svc1);
         }
         else 
            svc = svc->next_hash;
      }

    }
}


/*
 * send out status enquiry message for all the SVCs
 */
static void atmSig_sscopEstablish (hwidbtype *idb)
{
   int i;
   atmSvcType **prev, *svc, *svc1;
   uchar drain_count;
   ulong delay;
  
   if (!idb) {
     buginf("\nATMSIG: atmSig_sscopEstablish: NULL idb");
     return;
   }

   drain_count = 0;

   for (i = 0; i < ATMSVC_HASH_SIZE; ++i) {

     prev = &atmSig_svcHashTable[i];
     svc = *prev;

     while (svc) {
       if (svc->sig_idb->hwptr == idb) {
         svc1 = svc;
         svc = svc->next_hash;
         drain_count++;
         if (drain_count <= (PAK_SWITCHCOUNT * 2) ) {
           atmSig_statusEnquiry(svc1);
         }
         else {
           if (atmSig_debugEvent)
             buginf("\nstatus Enquiry: %d",
                    (svc1->callRef & ATM_CALLREF_VALUEMASK) );

           delay = (drain_count/PAK_SWITCHCOUNT) * ONESEC;
           svc1->statusEnquiry = FALSE;
           mgd_timer_start(&svc1->statusEnqTimer, delay);
         }
       }
       else {
         prev = &svc->next_hash;
         svc = svc->next_hash;
       }
     }
   }
}

static void atmSig_statusEnquiry (atmSvcType *svc)
{
  if ( atmSig_txStatusEnquiryMsg(svc) )
    mgd_timer_start(&svc->statusEnqTimer,svc->sig_idb->hwptr->atm_db->timer_t322); 
}



/*
 * a t m S i g _ s u b i n t e r f a c e R e s e t ( )
 *
 * Close svc's associated with a particular sub-interface.
 *
 * This function is called when someone deletes a subinterface.  This
 * function is registered with the atm driver which invokes it on
 * removal of a subinterface.
 */
void atmSig_subinterfaceReset (idbtype *idb)
{

   int i;
   atmSvcType **prev, *svc, *svc1;


   for (i = 0; i < ATMSVC_HASH_SIZE; ++i) {

      prev = &atmSig_svcHashTable[i];
      svc = *prev;

      while (svc) {
         /* remove svc only if there is a subinterface match */
         if (svc->idb == idb) {
            svc1 = svc;
            svc = svc->next_hash;
            if (atmSig_debugEvent)
               buginf("\nATMSIG:subinterfaceReset: removing svc %d", svc1->callRef);
            atmSmap_closeSvcConnection(svc1);
         }
         else {
            prev = &svc->next_hash;
            svc = svc->next_hash;
         }
      }

   }

}



/*
 * a t m s i g _ i n t e r f a c e _ u p ( )
 *
 * Returns TRUE if both the ATM hw and signaling is up.  All
 * clients of signaling should call this function before starting
 * to send requests to signaling as signaling will toss any
 * requests if either the hw or signaling is not up fully.
 */
boolean atmsig_interface_up (idbtype *idb)
{

   sscopType *sscop;


   /* atm hw up? */
   if ( !idb || !atm_interface_up(idb) )
      return (FALSE);

   /* if no qsaal pvc, return false */
   if (idb->hwptr->atm_db->sig_vcnum == 0)
     return (FALSE);
 
   sscop = idb->hwptr->sscop;

   if ( !sscop || sscop->q93bResetFlag )
      return (FALSE);

   return (TRUE);

}
 
 

/*
 * a t m S i g _ n u l l N s a p
 * 
 * Returns TRUE if ATM nsap address is null (or not configured).
 */
boolean atmSig_nullNsap (uchar *nsap)
{

   int i;

   for (i = 0; i < STATIONLEN_ATMNSAP; ++i, ++nsap) {
      if (*nsap != 0)
	return (FALSE);
   }
 
   return (TRUE);

}
 


/*
 * a t m S i g _ n u l l P r e f i x
 * 
 * Returns TRUE if ATM esi address is null (or not configured).
 */
boolean atmSig_nullPrefix (uchar *prefix)
{

   int i;

   for (i = 0; i < ILMI_PREFIX_SIZE; ++i, ++prefix) {
      if (*prefix != 0)
	return (FALSE);
   }
 
   return (TRUE);

}
 


/*
 * a t m S i g _ n u l l E s i
 * 
 * Returns TRUE if ATM esi address is null (or not configured).
 */
boolean atmSig_nullEsi (uchar *esi)
{

   int i;

   for (i = 0; i < ILMI_ESI_SIZE; ++i, ++esi) {
      if (*esi != 0)
	return (FALSE);
   }
 
   return (TRUE);

}
 


/*
 *
 *   T  i  m  e  r      S  e  c  t  i  o  n
 * 
 */


/*
 * atmSig_process_messages
 *
 * Handle an messages from other processes.  Currently the only
 * defined messages is a RESET message.
 */
static void atmSig_process_messages (void)
{
    ulong message, value;
    void *pointer;
    sscopType *sscop;

    while (process_get_message(&message, &pointer, &value)) {
        sscop = pointer;
        if (!sscop) {
          /* fatal error */
          if (atmSig_debugError)
            buginf("\nATMSIG: sscop message - sscop block not found");
          continue;
        }
	switch (message) {
	  case ATMSIG_SSCOP_RESET:
	  if (sscop->q93bResetFlag) {
	    atmSig_interfaceReset(sscop->idb);
	    sscop->q93bResetFlag = FALSE;
	  } 
	  break;

	  case ATMSIG_SSCOP_RELEASE:  /*only from 3.1 code now */
	  if (atmSig_debugError)
	    buginf("\nATMSIG: %d sscop failure, clear all non active SVCs",
		   sscop->index);

	  atmSig_sscopFail(sscop->idb);
	  break;

          case ATMSIG_SSCOP_ESTABLISH:
	  if (atmSig_debugError)
	    buginf("\nATMSIG: %d sscop establish",sscop->index);
	  atmSig_sscopEstablish(sscop->idb);
	  break;

	  default:
	    errmsg(&msgsym(UNEXPECTEDMESSAGE,SCHED), message, pointer, value);
	    break;
	}
    }
}


/*
 * a t m S i g _ t i m e r P r o c e s s ( )
 *
 * This process blocks - waiting for svc's to timeout.
 */
forktype atmSig_timerProcess (void)
{
    ulong major, minor;

    /* wait for system to initialize */
    process_wait_on_system_init();

    process_watch_mgd_timer(&atmSig_wakeupTime, ENABLE);
    for (;;) {
      process_wait_for_event();
      while (process_get_wakeup(&major, &minor)) {
	  switch (major) {
	    case TIMER_EVENT:
	      /* process svc's which have timed out */
	      atmSig_tickTock();
	      break;

	    case MESSAGE_EVENT:
	      atmSig_process_messages();
	      break;

	    default:
	      errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
	      break;
	  }
      }
   }
}
 


/*
 * a t m S i g _ s t a r t T i m e r ( )
 *
 * Place the party at the front of the timer Q.  Indicate the timeout
 * value in the party block.
 */
void atmSig_startTimer (atmSvcPartyType *party, ulong delta)
{

   if (!party) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: startTimer: no party block");
      return;
   }

   mgd_timer_start(&party->timeoutValue, delta);

}
   


/*
 * a t m S i g _ s t o p T i m e r ( ) 
 *
 * Remove svc block from timer Q. 
 */
void atmSig_stopTimer (atmSvcPartyType *party)
{

   if (!party) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: stopTimer: no party block");
      return;
   }

   mgd_timer_stop(&party->timeoutValue);

} 



/*
 * a t m S i g _ t i c k T o c k  ( )
 *
 * scan the timer queue and process svc's which have timed out.
 */
void atmSig_tickTock (void)
{

   atmSvcType *svc = NULL;
   atmSvcPartyType *party = NULL;
   mgd_timer *expired_timer;

   while(mgd_timer_expired(&atmSig_wakeupTime)) {
     expired_timer = mgd_timer_first_expired(&atmSig_wakeupTime);
     switch (mgd_timer_type(expired_timer)) {
     case ATMSIG_SHARED_TIMER:
       party = mgd_timer_context(expired_timer);
       svc = party->svc;
       atmSig_svcTimeout(svc, party);
       break;
       
     case ATMSIG_SYNC_TIMER:
       svc = mgd_timer_context(expired_timer);
       mgd_timer_stop(expired_timer);
       if (!svc) {
	 errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), expired_timer,
		mgd_timer_type(expired_timer));
	 break;
       }

       if (!svc->statusEnquiry) {
	 /* haven't send out status enquiry message yet */
	 atmSig_statusEnquiry(svc);
       }
       else {  /* must be t322 expired, no retransmission */
	 svc->event = ATMSIG_REQ_RELEASE;
	 atmSig_stateTable(svc);
       }
       break;

     default:
       errmsg(&msgsym(UNEXPECTEDTIMER, SCHED), expired_timer,
	      mgd_timer_type(expired_timer));
       
       mgd_timer_stop(expired_timer);

     }
   }

}



/*
 * a t m S i g _ s v c T i m e o u t ( )
 *
 * Take action on a svc timeout.  The action taken depends on the timer which 
 * ticked off and the state of the svc.  The default values for the various
 * timers are recommended by the ATM Forum.
 */
void atmSig_svcTimeout (atmSvcType *svc, atmSvcPartyType *party)
{

    switch (svc->state) {

       /* 
        * Default timeout - 4 secs.  Timer notation (ATM Forum) - T303.  
        * Cause for Start - SETUP message sent.  
        * Normal Stop - CONNECT, CALL PROCEEDING or RELEASE COMPLETE msg rcvd.
        * At the first expiry - clear SVC connection.
        */
       case ATMSIG_ST_CALL_INITIATED:
          if (atmSig_debugAll || atmSig_debugEvent)
	    buginf("\nATMSIG:svc timeout, T303 ref %u, vcd %d", svc->callRef,
		   svc->vcnum);
          atmSig_freeSvcBlock(svc);
	  break;

       /* 
        * Default timeout - 10 secs.  Timer notation (ATM Forum) - T310. 
        * Cause for Start - CALL PROCEEDING message received.  
        * Normal Stop - CONNECT or RELEASE message received.
        * At the first expiry - Send RELEASE message.
        * At the second expiry - not applicable.   
        */
       case ATMSIG_ST_OUTCALL_PROCEEDING:
          if (atmSig_debugAll || atmSig_debugEvent)
            buginf("\nATMSIG:svc timeout, T310 %d, vcd %d", svc->callRef,
		   svc->vcnum);
	  atmSig_txReleaseMsg(svc, ATM_CAUSE_RCVRY_ON_TIMEREXPIRY);
          party->retryCount++;
          atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);
          svc->state = ATMSIG_ST_RELEASE_REQ; 
          break;

       /* 
        * Default timeout - 4secs.  Timer notation (ATM Forum) - T313. 
        * Cause for Start - CONNECT message sent.  
        * Normal Stop - CONNECT ACK message received.
        * At the first expiry - Send RELEASE message. 
        * At the second expiry - not applicable. 
        */ 
       case ATMSIG_ST_CONNECT_REQ:
          if (atmSig_debugAll || atmSig_debugEvent)
            buginf("\nATMSIG:svc timeout, T313 ref %u, vcd %d", svc->callRef,
		   svc->vcnum);
	  /* do it for both 3.0 and 3.1 */
	  atmSig_txReleaseMsg(svc, ATM_CAUSE_RCVRY_ON_TIMEREXPIRY);
          party->retryCount++;
          atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);
          svc->state = ATMSIG_ST_RELEASE_REQ; 
          break;

       /* 
        * Default timeout - 30secs.  Timer notation (ATM Forum) - T308. 
        * Cause for Start - RELEASE message sent.  
        * Normal Stop - RELEASE COMPLETE or RELEASE message received.
        * At the first expiry - Retransmit RELEASE message.
        * At the second expiry - clear SVC connection.
        */
       case ATMSIG_ST_RELEASE_REQ:
          if (atmSig_debugAll || atmSig_debugEvent)
            buginf("\nATMSIG:svc timeout, rel-req T308 ref %u, vcd %d", 
		   svc->callRef,svc->vcnum);
          if (party->retryCount < 1) {
             atmSig_txReleaseMsg(svc, ATM_CAUSE_NORMAL_UNSPECIFIED);
             party->retryCount++;
             atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);
          }
          else {
             atmSig_freeSvcBlock(svc);
          }
          break;

       /*
        * Default timeout - 30secs.  Timer notation (ATM Forum) - T308.
        * Cause for Start - RELEASE message received.
        * Normal Stop - RELEASE COMPLETE event from Signalling user.
        * At the first expiry - Transmit RELEASE COMPLETE message.
        * At the second expiry - not applicable.
        */
       case ATMSIG_ST_RELEASE_IND:
          if (atmSig_debugAll || atmSig_debugEvent)
            buginf("\nATMSIG:svc timeout, rel-ind T308, ref %u, vcd %d", 
		   svc->callRef,svc->vcnum);
          atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_NORMAL_UNSPECIFIED);
          atmSig_freeSvcBlock(svc);
          break;

       /* 
        * Default timeout - 2 min.  Timer notation (ATM Forum) - T316.  
        * Cause for Start - RESTART message sent.  
        * Normal Stop - RESTART ACK message received.
        * At the 1st & 2nd expiry - RESTART message may be retransmitted
        * configurable number of times. 
        */
       case ATMSIG_ST_RESTART_REQ:
          if (atmSig_debugAll || atmSig_debugEvent)  
	    buginf("\nATMSIG:svc timeout, T316, %d, vcd %d", svc->callRef
		   ,svc->vcnum);
          if (party->retryCount < party->n2) {
             atmSig_txRestartMsg(svc);
             party->retryCount++;
             atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t316);
          }
          else {
             atmSig_freeSvcBlock(svc);
          }
          break;

       /*
	* Active Link State
	* If SVC active, timer is for StatusMsg or multipoint
	* Check for multipoint msg timeout first
	*/
       case ATMSIG_ST_ACTIVE:

	  switch(party->partyState) {
	    /* 
	     * Default timeout - 4 sec.  Timer notation (ATM Forum) - T398.  
	     * Cause for Start - DROP_PARTY message sent.  
	     * Normal Stop - DROP_PARTY ACK message received.
	     * At expiry - Clear call
	     */
	  case ATMSIG_PST_DROP_PARTY_INITIATED:
	    if (atmSig_debugAll || atmSig_debugEvent)
              buginf("\nATMSIG:svc timeout, T398, ref %u, vcd %d", 
		     svc->callRef,svc->vcnum);
	    atmSig_stopTimer(party);
	    svc->causeValue = ATM_CAUSE_NORMAL_UNSPECIFIED;
	    atmSig_txDropPartyAckMsg(svc, party, svc->causeValue);
	    atmSig_notifyDropPartyEvent (svc, party);
	    atmSig_freePartyBlock(svc, party);	    

	    return;


	    /* 
	     * Default timeout - 14 sec.  Timer notation (ATM Forum) - T399.  
	     * Cause for Start - ADD_PARTY message sent.  
	     * Normal Stop - ADD_PARTY ACK message received.
	     * At expiry - Clear call
	     */
	  case ATMSIG_PST_ADD_PARTY_INITIATED:
	    if (atmSig_debugAll || atmSig_debugEvent)
              buginf("\nATMSIG:svc timeout, T399, ref %u, vcd %d", 
		     svc->callRef,svc->vcnum);
	    atmSig_stopTimer(party);
	    atmSig_notifyAddPartyRejEvent (svc, party);
	    atmSig_freePartyBlock(svc, party);

	    return;


	  default:
	    /* fall through to status message */


	  /* 
	   * Not multipoint timer, must be timer for status msg
	   * Default timeout - 4secs.  Timer notation (ATM Forum) - T322. 
	   * Cause for Start - STATUS ENQUIRY message sent.  
	   * Normal Stop - STATUS, RELEASE or RELEASE COMPLETE msg received.
	   * At the first and second expiry - STATUS ENQUIRY message may be
	   * retransmitted configurable number of times.  
	   */
	    if (atmSig_debugAll || atmSig_debugEvent)
              buginf("\nATMSIG:svc timeout, T322, ref %u, vcd %d", 
		     svc->callRef,svc->vcnum);
	    if (party->retryCount < party->n2) {
	      atmSig_txStatusEnquiryMsg(svc);
	      party->retryCount++;
	      atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t322);
	    }

	    return;


	  }

	default:
          if (atmSig_debugAll || atmSig_debugEvent)
	    buginf("\nATMSIG: invalid state %s for message timeout",
                                            atmSig_printState(svc->state) );
	  atmSig_stopTimer(party);
          break;

    } 

}



/* 
 * Multipoint specific routines start here
 */

/*
 * a t m S i g _ s e l e c t P a r t y B l o c k ( )
 *
 * Select party block for incoming MTP message.
 * Returns Party or NULL to caller.
 */
atmSvcPartyType *atmSig_selectPartyBlock (paktype *pak, atmSvcType *svc)
{

  atmSvcPartyType *party;


  /* sanity checks here */
  if (!svc) {
    if (atmSig_debugAll || atmSig_debugError)
      buginf("\nATMSIGERR: selectPartyBlock, svc = 0");
    return(NULL);
  }

  if (svc->multiPoint != TRUE) {
    if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: selectPartyBlock, %s Event ignored, svc not MTP",
	     atmSig_printEventType(svc->event) );
    return(NULL);
  }

  /* 
   * This is a MTP message, select party, only if state active 
   * or releasing.
   */
  if (! ( (svc->state == ATMSIG_ST_ACTIVE) || 
	 (svc->state == ATMSIG_ST_RELEASE_REQ) ||
	 (svc->state == ATMSIG_ST_RELEASE_IND) ) ) {

    if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: %s Event ignored, link not active or releasing",
	     atmSig_printEventType(svc->event) );
    return(NULL);
  }

  /* Find endpointRef for multipoint call */
  atmSig_findEndpointRef(svc, pak);	

  /* If new party, allocate party block */
  if (svc->event == ATMSIG_RCV_ADD_PARTY) {

    /* Verify remotely init */
    if (svc->locallyInitiatedCall != FALSE) {
      if (atmSig_debugAll || atmSig_debugEvent)
	buginf("\nATMSIG: AddParty Msg ignored, not remotely init");
      return(NULL);
    }

    /* new party */
    party = atmSig_allocatePartyBlock();
    if (!party) {
      return(NULL);
    }

    /* AddParty checks OK, add it to the call */
    party->svc = svc;
    party->endpointRef = svc->msgEndpointRef;
    party->partyState = ATMSIG_PST_NULL;
    atmSig_enterPartyHashEntry(svc, party);   
    atmSig_enterPartyBlock(svc, party);    

  } else {
    /* Not AddParty, party exists, find it in party table */
    party = atmSig_findPartyBlockByRef(svc);
    if (!party) {
      return(NULL);
    }
  }

  return(party);

}



/*
 * a t m S i g _ e n t e r P a r t y B l o c k ( )
 *
 * Enter party block into the svc party Q.
 * Returns TRUE if successful. 
 */
boolean atmSig_enterPartyBlock (atmSvcType *svc, atmSvcPartyType *party)
{

   leveltype level;

   
   if (!svc) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIGERR: enterPartyBlock: svc = 0");
      return(FALSE);
   }

   if (!party) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIGERR: enterPartyBlock: party = 0");
      return(FALSE);
   }

   level = raise_interrupt_level(NETS_DISABLE);

   /* put party block at head of svcPartyQ */
   party->next_party = svc->svcPartyQ;
   svc->svcPartyQ = party;
   
   reset_interrupt_level(level);

   if (atmSig_debugAll || atmSig_debugEvent)
     buginf("\nATMMTP: enterPartyBlock: party placed on Q, party = %x, endpointRef = %d", party, party->endpointRef);

   return(TRUE);

}



/*
 * a t m S i g _ r e m o v e P a r t y B l o c k ( )
 *
 * Removes party block entry from the svc party Q (if present).
 * If rootParty, next party = rootParty
 * Returns TRUE if successfully removed, FALSE otherwise.
 */
boolean atmSig_removePartyBlock (atmSvcType *svc, atmSvcPartyType *party)
{

  atmSvcPartyType **prev, *current, *last;
  leveltype level;


  if (!svc) {
    if (atmSig_debugAll || atmSig_debugError)
      buginf("\nATMSIGERR: removePartyBlock: no svc block");
    return(FALSE);
  }

  if (!party) {
    if (atmSig_debugAll || atmSig_debugError)
      buginf("\nATMSIGERR: removePartyBlock: no party block");
    return(FALSE);
  }

  level = raise_interrupt_level(NETS_DISABLE);
  prev = &svc->svcPartyQ;
  current = *prev;
  last = *prev;

  while (current) {
    if (current == party) {
      *prev = party->next_party;
      if ( (party == svc->rootParty) && (last != party) ) 
	svc->rootParty = last;
      party->next_party = NULL;
      reset_interrupt_level(level);
      if (atmSig_debugAll || atmSig_debugEvent)
	buginf("\nATMSIG: removePartyBlock: party %x removed from partyQ", party);
      return(TRUE);
    }
    prev = &current->next_party;
    last = current;
    current = current->next_party;
  }

  reset_interrupt_level(level);
  if (atmSig_debugAll || atmSig_debugError)
    buginf("\nATMSIGERR: removePartyBlock: party %x not found", party);
  return (FALSE);

}



/*
 * a t m S i g _ a l l o c a t e P a r t y B l o c k ( )
 *
 * Malloc a party block.
 */
atmSvcPartyType *atmSig_allocatePartyBlock (void)
{

   atmSvcPartyType *party = NULL;


   /* get party block from system memory */
   party = malloc_named( sizeof(atmSvcPartyType), "ATMSIG-PRTY");
   if (party == NULL) {
     ++atmSig_statsNoMemory;
     return (NULL);
   }

   memset( party, 0, sizeof(atmSvcPartyType) );
   mgd_timer_init_leaf(&party->timeoutValue, &atmSig_wakeupTime,
		       ATMSIG_SHARED_TIMER, party, FALSE);

   return (party);

}
   


/*
 * a t m S i g _ e n t e r P a r t y H a s h E n t r y ( )
 *
 * Enter party block into the party hash table.
 * Index is callref for 1st party of mpt call, callref + endpointref
 * for additional parties.
 */
boolean atmSig_enterPartyHashEntry (atmSvcType *svc, atmSvcPartyType *party)
{

   int index = 0;
   leveltype level;

   
   if (!svc) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: enterPartyHashEntry: svc = 0");
      return (FALSE);
   }

   if (!party) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: enterPartyHashEntry: party = 0");
      return (FALSE);
   }
   
   index = (nethash ((ulong )(svc + party->endpointRef)) ) & ATMSIG_HASH_MASK;

   level = raise_interrupt_level(NETS_DISABLE);

   party->next_hash = atmSig_partyHashTable[index];

   atmSig_partyHashTable[index] = party;

   reset_interrupt_level(level);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMMTP: enter PartyHash: party = %x, party index = %d, call ref = %d, endpoint ref = %d, lic = %s", 
	     party, index, svc->callRef, party->endpointRef, 
	     svc->locallyInitiatedCall ? "TRUE":"FALSE");

   return (TRUE);
}



/*
 * a t m S i g _ r e m o v e P a r t y H a s h E n t r y ( )
 *
 * Removes party entry from the party hash table (if present).
 * Returns TRUE if successfully removed, FALSE otherwise.
 */
boolean atmSig_removePartyHashEntry (atmSvcType *svc, atmSvcPartyType *target)
{

    int index;
    atmSvcPartyType *party, **prev;
    leveltype level;

    if (!svc) {
      if (atmSig_debugAll || atmSig_debugError)
	buginf("\nATMSIG: removePartyHashEntry: svc = 0");
      return (FALSE);
    }

    if (!target) {
      if (atmSig_debugAll || atmSig_debugError)
	buginf("\nATMSIG: removePartyHashEntry: party = 0");
      return (FALSE);
    }

    index = (nethash ((ulong )(svc + target->endpointRef))) & ATMSIG_HASH_MASK;

    level = raise_interrupt_level(NETS_DISABLE);
    prev = &atmSig_partyHashTable[index];
    party = *prev;

    while (party) {
       if (party == target) {
          *prev = party->next_hash;
          party->next_hash = NULL;
          reset_interrupt_level(level);
          if (atmSig_debugAll || atmSig_debugEvent)
	    buginf("\nATMSIG: removePartyHashEntry: party %x party index %d removed from hash table", party, index);
          return (TRUE);
       }
       prev = &party->next_hash;
       party = party->next_hash;
    }

    reset_interrupt_level(level);
    if (atmSig_debugAll || atmSig_debugError)
        buginf("\nATMSIG: removePartyHashEntry: call-ref %u, party %x not found",
	       svc->callRef, target->endpointRef);
    return (FALSE);

}



/*
 * a t m S i g _ f i n d P a r t y B l o c k B y R e f ( )
 *
 * Find the party block in the party hash table by index. 
 * Index is call reference or call ref + endpoint ref.  
 * The ref value is hashed to find the party hash table index.
 * Use this for incoming multipoint messages.
 */
atmSvcPartyType *atmSig_findPartyBlockByRef (atmSvcType * svc)
{

   atmSvcType *tmpSvc;
   atmSvcPartyType *party;
   leveltype level;
   ulong index;
   ulong callRef;
   short endpointRef;
   idbtype *idb;
   boolean locallyInitiatedCall; 


   callRef = svc->callRef;
   endpointRef = svc->msgEndpointRef;
   idb = svc->sig_idb;
   locallyInitiatedCall = svc->locallyInitiatedCall;

   index = (nethash ((ulong )(svc + endpointRef)) ) & ATMSIG_HASH_MASK;

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: findPartyBlockByRef party index = %d, callref = %d, 
                endpoint = %d, lic = %s", index, callRef, endpointRef, 
	                  locallyInitiatedCall ? "TRUE":"FALSE");
 
   level = raise_interrupt_level(NETS_DISABLE);
   party = atmSig_partyHashTable[index];
   tmpSvc = party ? party->svc : NULL;

   while ((party) && (tmpSvc)) {
      if ( (tmpSvc->callRef == callRef) && (tmpSvc->sig_idb == idb) &&
                  (tmpSvc->locallyInitiatedCall == locallyInitiatedCall) ) {
         reset_interrupt_level(level);
	 if (atmSig_debugAll || atmSig_debugEvent)
	   buginf("\nATMSIG:findPartyBlockByRef, party block found");
         return (party);
      }
      party = party->next_hash;
      tmpSvc = party ? party->svc : NULL;
   }

   reset_interrupt_level(level);
	 if (atmSig_debugAll || atmSig_debugEvent)
	   buginf("\nATMSIG:findPartyBlockByRef, party block NOT found");
   return (NULL);

}



/*
 * a t m S i g _ f i n d P a r t y B l o c k B y E n d p o i n t R e f ( )
 *
 * Find the party block in the svc party chain by Endpoint reference.  
 * Clear msgEndpointRef if found.
 */
atmSvcPartyType *atmSig_findPartyBlockByEndpointRef (atmSvcType *svc, 
						     short endpointRef)
{

   leveltype level;
   atmSvcPartyType *party;

   if (!svc) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIGERR: enterPartyBlock: svc = 0");
      return (NULL);
    }

   party = svc->svcPartyQ;

   if (!party) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIGERR: findPartyBlock: party = 0");
      return (NULL);
    }

   level = raise_interrupt_level(NETS_DISABLE);

   while (party) {
     if (party->endpointRef == endpointRef) {
       reset_interrupt_level(level);
       svc->msgEndpointRef = 0;
       return (party);
     }
     party = party->next_party;
   }

   reset_interrupt_level(level);
   return (NULL);

}



/*
 * a t m S i g _ a l l o c a t e E n d p o i n t R e f ( )
 *
 * Allocate Endpoint Ref's on a global basis, endpointRefValue is
 * maintained for this SVC.
 * Already have SVC, search SVC for Endpoint Refs.
 * EndpointRef's are unique within a point-multipoint call.
 * Returns TRUE if endpoint ref allocated, FALSE otherwise. 
 */
boolean atmSig_allocateEndpointRef (atmSvcType *svc, atmSvcPartyType *party)
{

   atmSvcPartyType *tmpParty;
   short endpointRef;
   uint maxparty;
   leveltype level;
   

   maxparty = 0;
   endpointRef = svc->lastEndpoint;


   /* Endpoint reference value of zero is reserved for the first party
    * of a multipoint call. Don't generate it automatically.
    */
   if ( (endpointRef == 0) || (endpointRef == 0x7FFF) )
     endpointRef = 1;
   else
     ++endpointRef;

   /*
    * Check if endpoint ref value is currently being used by another party 
    * of the same call (svc).  Check all parties in the call.
    * If endpointref used, bump it and try again.
    */
   tmpParty = svc->rootParty;

   level = raise_interrupt_level(NETS_DISABLE);

   while (tmpParty) {
     if ( (tmpParty->endpointRef != endpointRef) ) {
       tmpParty = tmpParty->next_party;	  
     }
     else {
       tmpParty = svc->rootParty;

       if ( (endpointRef == 0) || (endpointRef == 0x7FFF) )
	 endpointRef = 1;
       else
	 ++endpointRef;

       /*
	* Safety check. Should never happen.
	*/
       ++maxparty;
       if (++maxparty > ATMSIG_MAX_ENDPOINT) {
	 if (atmSig_debugAll || atmSig_debugError)
	   buginf("\nATMSIG: cannot allocate endpoint reference");
	 return (FALSE);
       }
       
     }
	
   }
   reset_interrupt_level(level);

   /* found an unused endpoint reference value */ 
   party->endpointRef = endpointRef;
   svc->lastEndpoint = endpointRef;
	  
   if (atmSig_debugAll || atmSig_debugEvent)
     buginf("\nATMSIG: endpoint ref allocated, callref = %d, endpoint = %d", 
	    svc->callRef, endpointRef);
 
   return (TRUE);


}



/*
 * a t m S i g _ v a l i d a t e P a r t y ( )
 *
 * Verify that a valid svc entry exists in the hash table.
 * Verify that a valid party exists for that SVC.
 * Returns TRUE if party found, FALSE otherwise.
 */
boolean atmSig_validateParty (atmSvcType *target, atmSvcPartyType *targetParty)
{

    int index;
    atmSvcType *svc;
    leveltype level;

    atmSvcPartyType *party;

    if (!target)
       return (FALSE);

    if (!targetParty)
       return (FALSE);

    index = atmSig_hashCallRef(target->callRef);
    level = raise_interrupt_level(NETS_DISABLE);
    svc = atmSig_svcHashTable[index];

    while (svc) {
      if (svc == target) {
	party = svc->svcPartyQ;

	while (party) {
	  if (party == targetParty) {
	    reset_interrupt_level(level);
	    return (TRUE);
	  }
	  party = party->next_party;
	}
      }

      svc = svc->next_hash;
    }
    reset_interrupt_level(level);
    return (FALSE);

}



/*
 * a t m S i g _ f i n d P a r t y S t a t u s ( )
 *
 * Find the status (partyState) of the party by sending a Status Enquiry message
 * to the switch.
 */
atmSig_statuscodetype atmSig_findPartyStatus (atmSvcType *svc, atmSvcPartyType *party)
{


   /* valid svc? */
   if ( !atmSig_validateSvc(svc) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: findPartystatus, no svc");
      return (ATMSIG_STATUSSVC_INVALIDSVC);
   }

   /* valid party? */
   if ( !atmSig_validateParty(svc, party) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: findPartystatus, no party");
      return (ATMSIG_STATUSSVC_INVALIDSVC);
   }

   /* transmit Status Enquiry message, transmit will fail only if no memory  */
   if ( !atmSig_txStatusEnquiryMsg(svc) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIGERR: no memory to send Status Enquiry msg, status request failed");
      return (ATMSIG_STATUSSVC_STATUSFAILED);
   }

   /* start timer */
   atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t322);
   party->retryCount = 0; 

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: status svcParty request in progress");

   return (ATMSIG_STATUSSVC_OK);

}



/*
 * a t m S i g _ f r e e P a r t y B l o c k ( )
 *
 * Stop timer if running.  
 * Remove party entry from the partyQ.
 * Remove party from static map.  
 * Return block to system memory.
 */
void atmSig_freePartyBlock (atmSvcType *svc, atmSvcPartyType *party)
{



   /* sanity check */
   if (!svc) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: freePartyBlock - null svc!");
      return;
   }

   if (!svc->multiPoint) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: freePartyBlock - svc not multiPoint!");
      return;
   }

   if ( !atmSig_validateParty(svc, party) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: freePartyBlock, party not valid!");
      return;
   }

   /* stop party timer if active. */
   atmSig_stopTimer(party);

   /* stop conn pending */
   if ( (party->conn_pending) &&
                (svc->sig_idb->hwptr->atm_db->conn_pending > 0) ) {
     --svc->sig_idb->hwptr->atm_db->conn_pending;
     party->conn_pending = FALSE;
   }

   /* remove from partyQ and free party block back to memory pool 
    * if rootParty, don't free here, let freeSvcBlock free last party
    */
   if ( atmSig_removePartyBlock(svc, party) ) {
     if ( atmSig_removePartyHashEntry(svc, party) ) {

       if (party == svc->rootParty) {
	 if (atmSig_debugAll || atmSig_debugEvent)
	   buginf("\nATMMP: DON'T FREE ROOT PARTY");
	 return;
       }
       free(party); 
     }
   }
  
  return;

}



/*
 * a t m S i g _ f l u s h P a r t y Q ( )
 *
 * Clear out all parties of an SVC partyQ
 * 
 * This function is called when a MTP call is released.
 * There should be no more parties. However, there may be more parties
 * if client or network just sent RELEASE instead of DROP_PARTY's, or
 * if there was a hardware reset.
 * Send DROP_PARTY's for all parties except last.
 * Save rootParty for the calling svc to send a RELEASE
 * Party's will be freed when call is released
 */
boolean atmSig_flushPartyQ (atmSvcType *svc)
{

   atmSvcPartyType *party;


   /* valid svc? */
   if ( !svc || !atmSig_validateSvc(svc) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: flushPartyQ, requested svc not valid");
      return (FALSE);
   }

   /* multipoint? */
   if (!svc->multiPoint) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: flushPartyQ, requested svc not multipoint");
      return (FALSE);
   }

   /* get first party */
   party = svc->svcPartyQ;

   if (!party) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: flushPartyQ, no party");
      return (FALSE);
   }

   /* If party active, send dropParty, if not active, send dropPartyAck
    * Send dropPartyAck to client. Free the block, unless it's the last one.
    */
   while ( (party) && (party != svc->rootParty) ) {

     switch (party->partyState) {

     case ATMSIG_PST_ACTIVE:
     case ATMSIG_PST_ADD_PARTY_INITIATED:
       atmSig_stopTimer(party);
       atmSig_txDropPartyMsg(svc, party, ATM_CAUSE_NORMAL_UNSPECIFIED);
       party->partyState = ATMSIG_PST_DROP_PARTY_INITIATED;
       atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t398);
       party->retryCount = 0;
       break;


     case ATMSIG_PST_ADD_PARTY_RECEIVED:
       atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_NORMAL_UNSPECIFIED);
       party->partyState = ATMSIG_PST_NULL;
       break;


     case ATMSIG_PST_DROP_PARTY_RECEIVED:
       atmSig_txDropPartyAckMsg(svc, party, ATM_CAUSE_NORMAL_UNSPECIFIED);
       party->partyState = ATMSIG_PST_NULL;
       break;


     case ATMSIG_PST_DROP_PARTY_INITIATED:
       break;

       /* unknown party state */
     default:
       break;

     }
     party = party->next_party;
   }
     
   return (TRUE);
   
}



/*
 * a t m S i g _ f r e e A l l P a r t i e s ( )
 *
 * Free all parties of an SVC partyQ. Save the rootParty for freeSvcBlock.
 * This function only called by freeSvcBlock.
 * There should be no more parties. However, we will check the partyQ and
 * free any parties remaining.
 */
boolean atmSig_freeAllParties (atmSvcType *svc)
{

   atmSvcPartyType *party, *cur_party;


   /* valid svc? */
   if ( !svc || !atmSig_validateSvc(svc) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: freeAllParties, requested svc not valid");
      return (FALSE);
   }

   /* multipoint? */
   if (!svc->multiPoint) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: freeAllParties, requested svc not multipoint");
      return (FALSE);
   }

   /* get first party */
   party = svc->svcPartyQ;

   if ( !atmSig_validateParty(svc, party) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: freeAllParties, party not valid");
      return (FALSE);
   }

   while ( (party) && (party != svc->rootParty) ) {

     cur_party = party;
     party = party->next_party;

     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG: freeAllParties, freeing Party %x", party);
     atmSig_freePartyBlock(svc, cur_party);

   }
     
   return (TRUE);
   
}



/*
 * a t m S i g _ p a r s e B r o a d b a n d L o w L a y e r I n f o ( )
 *
 * Parse Broadband Low Layer Information from setup_msg array.
 * The setup_msg was built from the BBLL IE.
 */
int atmSig_parseBroadbandLowLayerInfo (atmSvcType *svc)
{

   int length;
   int layer2id_len;
   char layer3_id;
   int layer3_ipi;
   int layer3_snap;
   int protocol;

   length = 0;
   layer2id_len = 0;
   layer3_ipi = 0;
   layer3_snap = 0;
   protocol = 0;


   /* verify blli */
   length = svc->blli[0].length;

   if (!length) { 
     return(FALSE);
   }

   /* check for layer2id */
   if (length != ATMSIG_LLAYER3_LENGTH) {

     if ( (length != ATMSIG_LLAYER_LENGTH) ||
	 (svc->blli[0].string[0] != ATMSIG_LLAYER2_ID) ) {

       if (atmSmap_debugAll || atmSmap_debugEvent)
	 buginf("\n ATMSM: parseBbLowLayer, incorrect length");

       return(FALSE);
     }
     /* have layer2id, adjust layer2id_len */
     layer2id_len = ATMSIG_LLAYER2_LENGTH;
   }

   layer3_id = svc->blli[0].string[ATMSIG_LLAYER3_ID_OFFSET + 
				    layer2id_len];
 
   if (layer3_id != ATMSIG_LLAYER3_ISO9577) {

       if (atmSmap_debugAll || atmSmap_debugEvent)
	 buginf("\n ATMSM: parseBbLowLayer, incorrect layer3_id");

     return(FALSE);
   }

   layer3_ipi = GETSHORT( & (svc->blli[0].string[ATMSIG_LLAYER3_IPI_OFFSET +
				    layer2id_len]) );

   if (layer3_ipi != ATMSIG_LLAYER3_IPI) {

       if (atmSmap_debugAll || atmSmap_debugEvent)
	 buginf("\n ATMSM: parseBbLowLayer, incorrect layer3_ipi");

     return(FALSE);
   }

   layer3_snap = GETLONG( & (svc->blli[0].string[ATMSIG_LLAYER3_SNAPID_OFFSET +
				    layer2id_len]) );

   if (layer3_snap != ATMSIG_LLAYER3_SNAPID) {

       if (atmSmap_debugAll || atmSmap_debugEvent)
	 buginf("\n ATMSM: parseBbLowLayer, incorrect layer3_snapid");

     return(FALSE);
   }

   protocol = GETSHORT( & (svc->blli[0].string[ATMSIG_LLAYER3_PID_OFFSET +
   				    layer2id_len]) );

   switch (protocol) {

      case TYPE_IP10MB:
         svc->protocol = TYPE_IP10MB;
         svc->linktype = LINK_IP; 
         break;

      case TYPE_XNS:
         svc->protocol = TYPE_XNS;
         svc->linktype = LINK_XNS;
         break;

      case TYPE_ETHERTALK:
         svc->protocol = TYPE_ETHERTALK;
         svc->linktype = LINK_APPLETALK;
         break;

      case TYPE_CLNS:
         svc->protocol = TYPE_CLNS;
         svc->linktype = LINK_CLNS;
         break;

      case TYPE_DECNET:
         svc->protocol = TYPE_DECNET;
         svc->linktype = LINK_DECNET;
         break;

      case TYPE_NOVELL1:
         svc->protocol = TYPE_NOVELL1;
         svc->linktype = LINK_NOVELL;
         break;

      case TYPE_APOLLO:
         svc->protocol = TYPE_APOLLO;
         svc->linktype = LINK_APOLLO;
         break;

      case TYPE_VINES:
         svc->protocol = TYPE_VINES;
         svc->linktype = LINK_VINES;
         break;

      default:
         svc->protocol = svc->linktype = 0;
         break;

   }

   return(protocol);

}			



/*
 * a t m S i g _ f i l l B r o a d b a n d L o w L a y e r I n f o ( )
 *
 * Fill Broadband Low Layer Information in setup_msg array.
 * It will be used to build the BBLL IE.
 */
boolean atmSig_fillBroadbandLowLayerInfo (staticmap_type *map,
					   sig_api_setup *setup_msg)
{

   int protocol;
   boolean result;
   atm_params_type *params;
   int layer2id_len;


   layer2id_len = 0;
   result = TRUE;


   if ((map->class) && (map->class->params))
     params = map->class->params;
   else
     params = map->params;

   if (!params)
     return(FALSE);

   if (params->layer2id != ATM_BLLIE) {
     setup_msg->blli[0].string[0] = params->layer2id;
     layer2id_len = ATMSIG_LLAYER2_LENGTH;
   }

   setup_msg->blli[0].length = ATMSIG_LLAYER3_LENGTH + layer2id_len;

   setup_msg->blli[0].string[ATMSIG_LLAYER3_ID_OFFSET + layer2id_len] = 
                                   ATMSIG_LLAYER3_ISO9577;

   PUTSHORT( &( setup_msg->blli[0].string[ATMSIG_LLAYER3_IPI_OFFSET +
				    layer2id_len]), ATMSIG_LLAYER3_IPI);

   PUTLONG( &( setup_msg->blli[0].string[ATMSIG_LLAYER3_SNAPID_OFFSET +
				    layer2id_len]), ATMSIG_LLAYER3_SNAPID);


   switch (map->protocol) {
  
      case LINK_IP:
         protocol = TYPE_IP10MB;
         break;

      case LINK_XNS:
         protocol = TYPE_XNS;
         break;

      case LINK_APPLETALK:
         protocol = TYPE_ETHERTALK;
         break;

      case LINK_CLNS:
      case LINK_CLNS_ALL_ES:
      case LINK_CLNS_ALL_IS:
      case LINK_CLNS_BCAST:
      case LINK_ISIS_ALL_L1_IS:
      case LINK_ISIS_ALL_L2_IS:
         protocol = TYPE_CLNS;
         break;

      case LINK_DECNET:
      case LINK_DECNET_NODE:
      case LINK_DECNET_ROUTER_L1:
      case LINK_DECNET_ROUTER_L2:
      case LINK_DECNET_PRIME_ROUTER:
         protocol = TYPE_DECNET;
         break;

      case LINK_PAD:
         protocol = 0;
         break;

      case LINK_NOVELL:
         protocol = TYPE_NOVELL1;
         break;

      case LINK_APOLLO:
         protocol = TYPE_APOLLO;
         break;

      case LINK_VINES:
         protocol = TYPE_VINES; 
         break;

      case LINK_ILLEGAL:
      default:
         protocol = 0;
	 result = FALSE;
         break;

   }

   PUTSHORT( &( setup_msg->blli[0].string[ATMSIG_LLAYER3_PID_OFFSET +
   				    layer2id_len]), protocol);

   return(result);

}


/*
 * a t m S i g _ c p s _ t o _ k b p s
 *
 *  Convert cells per second to kilo-bits per second. This gets
 *  a bit tricky towards the high end, and we don't have long long
 *  libraries.
 */
long atmSig_cps_to_kbps (long cps)
{
    long result,factor;
    /* ideally, result is cps*BITS_PER_CELL / 1000. But at the
       high-end, cps*BITS_PER_CELL exceeds 32 bits, so have to
       treat that differently. */
    if (cps <= ATMSIG_MAX_INPUT_SIMPLE_CPS_TO_KBPS_ALG) {
        result = (cps*ATMSIG_BITS_PER_CELL_MULTIPLIER)/ATMSIG_KBPS_DIVISOR;
        return(result);
    }
 
    /* number too large. Factor this number, and do it again. */
    factor = cps/ATMSIG_MAX_INPUT_SIMPLE_CPS_TO_KBPS_ALG;
    result = (factor * ATMSIG_KBPS_OF_MAX_INPUT_SIMPLE_CPS_TO_KBPS_ALG) +
             atmSig_cps_to_kbps(cps -
                (factor * ATMSIG_MAX_INPUT_SIMPLE_CPS_TO_KBPS_ALG));
 
    return(result);
}


/*
 * a t m S i g _ k b p s _ t o _ c p s
 *
 *  Convert kilobits-per-second to cells-per-second to cells per
 *  second. This gets a bit tricky towards the high end, and we
 *  don't have long long libraries.
 */
long atmSig_kbps_to_cps (long kbps)
{
    long result,intermediate,factor;
    /* ideally, result is kbps*1000/BITS_PER_CELL  But at the
       high-end, kbps*1000 exceeds 32 bits, so have to
       treat that differently. */
 
    if (kbps <= ATMSIG_MAX_INPUT_SIMPLE_KBPS_TO_CPS_ALG) {
        intermediate = kbps*ATMSIG_KBPS_DIVISOR;
        result = intermediate / ATMSIG_BITS_PER_CELL_MULTIPLIER;
        if ((intermediate % ATMSIG_BITS_PER_CELL_MULTIPLIER) != 0)
            return(result+1);
        else
            return(result);
    }
 
    /* number too large. Factor this number, and do it again. */
    factor = kbps/ATMSIG_MAX_INPUT_SIMPLE_KBPS_TO_CPS_ALG;
    result = (factor * ATMSIG_CPS_OF_MAX_INPUT_SIMPLE_KBPS_TO_CPS_ALG) +
              atmSig_kbps_to_cps(kbps -
                (factor * ATMSIG_MAX_INPUT_SIMPLE_KBPS_TO_CPS_ALG));
    return(result);
}

/*
 * ATM Signalling subsystem header
 */

#define ATMSIG_MAJVERSION      10
#define ATMSIG_MINVERSION      0
#define ATMSIG_EDITVERSION     1

SUBSYS_HEADER(atmsig, ATMSIG_MAJVERSION, ATMSIG_MINVERSION, ATMSIG_EDITVERSION,
              atmSig_init, SUBSYS_CLASS_PROTOCOL,
              NULL,
              "req: sscop");
      
