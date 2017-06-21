/* $Id: qllc.c,v 3.10.8.16 1996/09/15 02:56:00 rbatz Exp $
 * $Source: /release/112/cvs/Xsys/srt/qllc.c,v $
 *------------------------------------------------------------------
 * qllc.c -- Manage qllc data structures.
 *
 * November 1992,    Percy P. Khabardar
 * September 1993,   Phil Morton
 * September 1993,   Ramin Naderi
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qllc.c,v $
 * Revision 3.10.8.16  1996/09/15  02:56:00  rbatz
 * CSCdi60999:  QLLC: PS/2 using Link Station of Negotiable cannot connect.
 * Branch: California_branch
 *
 * Revision 3.10.8.15  1996/09/03  22:27:37  rbatz
 * CSCdi67189:  Multiple connections for reverse QLLC (DLSW+) crashes.
 * Branch: California_branch
 *
 * Revision 3.10.8.14  1996/08/28  13:15:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.8.13  1996/08/21  14:52:58  rbatz
 * CSCdi66340:  QLLC over PVCs displays infinite error messages.
 *              - Do not try to requue same qllc control block.
 *              - Do not respond to a Reset with a Reset
 *                because this can result in a protocol loop.
 * Branch: California_branch
 *
 * Revision 3.10.8.12  1996/08/21  13:15:16  rbatz
 * CSCdi62155:  LNX timers not working correctly. Result is TEST
 *              and NULL XID frames sent in the middle of
 *              XID(3) Negotiation.
 * Branch: California_branch
 *
 * Revision 3.10.8.11  1996/08/08  19:27:47  rbatz
 * CSCdi64333:  Serial input queue gets wedged because QLLC was
 *              not freeing QSM, QDISC, and QUA packets, plus
 *              clear_if_input() was not called.
 * Branch: California_branch
 *
 * Revision 3.10.8.10  1996/08/07  18:23:07  pmorton
 * CSCdi61562:  45k crash in QllcTestStnReq()
 * Branch: California_branch
 * Test whether Virtual Mac Address Range exists before using.
 *
 * Revision 3.10.8.9  1996/08/07  09:02:28  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.10.8.8  1996/08/03  23:37:26  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.10.8.7  1996/07/08  01:11:04  rbatz
 * CSCdi60002:  Npsi-poll and proxy-xid do not work for DLSw+/QLLC.
 * Branch: California_branch
 *
 * Revision 3.10.8.6  1996/05/29  22:51:23  rbatz
 * CSCdi55749:  Multiple "qllc dlsw..." commands do not work.
 * Branch: California_branch
 *
 * Revision 3.10.8.5  1996/05/23  06:42:21  pmorton
 * CSCdi58502:  Fix for CSCdi42369 broke FRAD image
 * Branch: California_branch
 * Use 'debug sdlc qllc' instead of 'debug qllc sdlc'
 *
 * Revision 3.10.8.4  1996/05/22  05:00:42  pmorton
 * CSCdi42369:  sdlc-qllc conversion, no auto-reconnect
 * Branch: California_branch
 * 1. When qllc link state changes propagate to SDLC-QLLC conversion (SNX)
 * 2. Add 'debug qllc sdlc' to track this
 * 3. Correct SNX state names
 *
 * Revision 3.10.8.3  1996/05/21  10:05:22  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.8.2  1996/05/17  12:13:09  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.2.4  1996/04/26  07:58:19  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.9.2.3  1996/04/06  01:06:01  pmorton
 * CSCdi45577:  QLLC w/PVCs regression with QLLC in CLS.
 * Build a map for managing PVCs, so that they can be claimed either by
 * CLS or by RSRB
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.2.2  1996/04/03  21:58:56  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.2.1  1996/03/17  18:49:40  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.10.8.1  1996/03/18  22:10:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.4  1996/03/16  07:37:55  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.3  1996/03/13  02:02:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.2  1996/03/07  10:50:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  18:47:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/28  09:34:09  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.9  1996/02/17  16:47:52  rbatz
 * CSCdi44435:  Outbound connection from FEP to QLLC device fails.
 *              Respond to XID Commands with XID Responses.
 *
 * Revision 3.8  1996/02/13  19:18:49  lpereira
 * CSCdi45471:  appn/qllc pvc broken
 *
 * Revision 3.7  1996/02/01  06:10:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/30  21:57:32  pmorton
 * CSCdi36695: QLLC disards XID packet (assuming that end station will
 * retransmit)
 * Hold the XID packet and send it to remote x.25 station after receiving
 * Call Accept
 *
 * Revision 3.5  1996/01/22  07:16:24  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/17  18:38:49  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/21  00:04:16  rbatz
 * CSCdi40851:  SegV exception in qllc_x25_macaddr due to race
 *              condition and a NULL pointer.
 *
 * Revision 3.2  1995/11/17  18:54:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/08/08  16:49:21  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.7  1995/08/07 05:43:50  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.6  1995/07/31  18:59:16  ppearce
 * CSCdi37939:  Eliminate use of CLS_P_TYPE_NUMBERS from DLSw+
 *
 * Revision 2.5  1995/07/20  07:56:01  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Assignment for proxy XID value was wrong
 *
 * Revision 2.4  1995/07/19  21:06:30  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add services to reducce dependency on idb.
 * Install 'qllc dlsw' command.
 * Build QLLC Activate/Deactivate Ring CLSI message.
 * Consolidate qllc fields from swidb to a single data structure (subblock)
 *
 * Revision 2.3  1995/06/21  09:01:03  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/20  07:19:02  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:00:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "srt_registry.h"
#include "interface_generic.h"
#include "packet.h"
#include "../os/buffers.h"
#include "mgd_timers.h"
#include "linktype.h"
#include "logger.h"
#include "../srt/msg_qllc.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../x25/parser_defs_x25.h"
#include "../llc2/llc2.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../x25/x25_proto.h"
#include "../util/avl.h"
#include "../ibm/sdlc.h"
#include "../ibm/sna_util.h"
#include "../cls/cls_private.h"
#include "qllc.h"
#include "qllc_private.h"
#include "../srt/lnx.h"
#include "parser_defs_qllc.h"
#include "../ui/debug.h"
#include "../srt/qllc_debug.h"
#include "qllc_event.h"
#include "../ibm/libibm_externs.h"

/*
 * Constants
 */

static const QTimersT QDefaultTimers = {
    DEFAULT_X25_TIMER,			/* X25 Timer */
    DEFAULT_QL_TIMER,			/* QLLC Timer */
    DEFAULT_QL_RETRIES			/* Logical Link Layer retries */
} ;
    
/*
 * Global storage
 */

/* process id's */
queuetype qllc_cleanupQ;
queuetype QCepFreeupQ;

static watched_queue *qllc_inputQ;


qllctype *qllcstationptr;

static int qllc_running = FALSE;

static qllcmap_t		*qllcmap_table[QLLC_HASH_SIZE];


/* local functions - registry entries */
static void qllc_enqueue(paktype *pak);
static boolean qllc_x25_macaddr(paktype *pak, long address,
				  addrtype *proto_addr, lcitype **lci,
				  boolean *broadcast);
static void qllc_x25_lci_notify_event(lcitype *lci, uchar  event, 
				  uchar cause, uchar diag, paktype *pak);
static boolean qllc_x25_address_map_enter(x25map *map);
static void qllc_x25_address_map_delete(addrtype *proto_addr);
static void qllc_x25_lci_busy(addrtype *proto_addr, boolean *result);
static boolean qllc_x25_pvc_setup(x25map *map, lcitype *lci, boolean sense);
static void qllc_send_quench(paktype *pak);

static void qllc_sprintf_address_link(char *buffer, addrtype *addr);
static boolean qllc_x25_incoming_call(paktype *pak, lcitype *lci,
	uchar *cause, uchar *diagnostic, uchar **reason);

static QLInputT QMapToCsliEvent(int event);

/* other local functions */

static qllctype *QCepCreate(QPSapT *qpsap, qllctype *qllc,
			    QLLCReqOpenStnT *qllcReqOpenStn,
			    uchar *vmac, char *x121address);

static QPSapT *QSRSearchByPvc(idbtype *idb, int lcn);
static boolean QCepListWalk(QPSapT *qpsap, QCepWalkT proc, ...);
static qllctype **QCepPvcFindSapEntry(QPSapT* qpsap, qllctype *qllc);
static qllctype *QPvcLookupUnclaimed(qllctype *qllc);
static boolean QCepInPvcList(QPSapT* qpsap, qllctype *qllc);
static enum avl_compare QLcnCompare(avl_node_type *node1,
				     avl_node_type *node2);

/* FSMs */
static void QX25PacketInput(qllctype *qllc, int event);
static void QX25EventInput(qllctype *qllc, uchar event);

static boolean QCepIncomingCall(paktype *pak, lcitype *lci, uchar *cause, uchar *diag, uchar **reason);
static int find_qllcpaktype (paktype *pak, QLInputT *qEvent, qllc_frmr_t *q_frmr);
static int QSapFindFreeVmac (QPSapT *qpsap, uchar *vmacaddr);
static boolean QSapVmacMapInUse(QPSapT *qpsap, int ix);
static void QSapVmacMapAllocate(QPSapT *qpsap, int ix);
static void QSapVmacMapFree(QPSapT *qpsap, int ix);

static boolean QCepDetachSap (qllctype *qllc, va_list);
static boolean QCepPvcWalk(QPSapT *qpsap, QCepWalkT proc, va_list argptr);
static void    QllcPrPacket(qllctype *qllc, paktype *pak, char direction);

static mgd_timer	QllcMasterTimer;

/* diagnostics */
#ifdef NOBODY_READS_ME
int qllc_stats_ignored;          /* ignored frames */
#endif
/* Now we don't reply to connection request till we have QUA, but
 * we don't send QSM till we get SABME/called by la2_qllc_open
 */
 
#define WHICHSTREAM_STARTUP(qllc) \
      if (qllc->lnx->configured_prtnr ) \
          lnx_initiate(qllc->lnx)    /* generate explorer and  go */
 


static void UpdateQllcRxStat(qllctype* qllc, int event);

/* 
 * qllc_findfree.
 * Allocates a new qllctype struct.
 * Returns NULL if no memory.
 */
qllctype *qllc_findfree (void)
{

    qllctype *qllc;
    
    qllc = malloc(sizeof(qllctype));

    if (!qllc) {
	return(NULL);
    }
        
    /* Initialize qllctype data structures */
    memset(qllc, 0, sizeof(qllctype));
    
    qllc->qllc_state    = QST_ADM;
    qllc->n2       = QRETRY_COUNT;

    /*
     * Create some managed timers.
     * For CLSI-compliant DLUs, QCepCreate will change the type of 
     * fTimer from QLNX_TIMER to QX25_TIMER.
     */
    mgd_timer_init_leaf(&qllc->fTimer, &QllcMasterTimer, QLNX_TIMER, qllc, TRUE);

    /* statistic that needs to be zeroed out */
    qllc->qllcStat.qllc_xidin            =   0;
    qllc->qllcStat.qllc_xidout           =   0;
    qllc->qllcStat.qllc_testin           =   0;
    qllc->qllcStat.qllc_testout          =   0;
    qllc->qllcStat.qllc_quenchoff        =   0;
    qllc->qllcStat.qllc_quenchon         =   0;
    qllc->qllcStat.qllc_inpaks           =   0;
    qllc->qllcStat.qllc_outpaks          =   0;
    qllc->qllcStat.qllc_inbytes          =   0;
    qllc->qllcStat.qllc_outbytes         =   0;
    qllc->qllcStat.qllc_numrcvqsms       =   0;
    qllc->qllcStat.qllc_numsndqsms       =   0;
    qllc->qllcStat.qllc_numrcvdiscs      =   0;
    qllc->qllcStat.qllc_numsnddiscs      =   0;
    qllc->qllcStat.qllc_numrcvdms        =   0;
    qllc->qllcStat.qllc_numsnddms        =   0;
    qllc->qllcStat.qllc_numrcvfrmrs      =   0;
    qllc->qllcStat.qllc_numsndfrmrs      =   0;
    qllc->qllcStat.qllc_numdrops         =   0;
    qllc->qllcStat.qllc_numerrs          =   0;
    
    queue_init(&qllc->qllc_holdQ, 0);
    queue_init(&qllc->qllc_lnxQ, 0);
    queue_init(&qllc->qllc_reassmQ, 0);

    /*
     * Although these are CLSI-specific, it is beneficial
     * for PVCs to have these always initialized.
     */
    queue_init(&qllc->fFreeEventQ, MAX_QLLC_EVENTS);
    queue_init(&qllc->fQLeventQ, MAX_QLLC_EVENTS);
    queue_init(&qllc->fQXeventQ, MAX_QLLC_EVENTS);   
    return (qllc);
}


/*
 * qllc_freeup
 */
void qllc_freeup (qllctype *qllc)
{    
    paktype *pak;
    leveltype level;

    if (qllc) {
        pak = qllc_get_stored_xid3(qllc);        
        if (pak) {
            datagram_done(pak);
        }

        mgd_timer_stop(&qllc->fTimer);

        /* Clean up back pointers */
	if (qllc->lci)
	    ((lcitype *)(qllc->lci))->qllc = NULL;

	if (qllc->lnx) {
	    qllc->lnx->qllc = NULL;
	}
	
        if (qllc->sdlcp)
            qllc->sdlcp->qllcp = NULL;
		
	while ((pak = p_dequeue(&qllc->qllc_holdQ)) != NULL)
		datagram_done(pak);

        while ((pak = p_dequeue(&qllc->qllc_lnxQ)) != NULL)
            datagram_done(pak);

	level = raise_interrupt_level(ALL_DISABLE); /* are we paranoid - you bet */
	while ((pak = p_dequeue(&qllc->qllc_reassmQ)) != NULL)
	    datagram_done(pak);
	reset_interrupt_level(level);

	free(qllc);
    }
    else
        if (lnx_error_debug)
	 buginf("\nQLLC: qllc_freeup - NULL qllc pointer!!");
}


/*******************************************************************
 *
 * 
Registry routines
 *
 */

static boolean qllc_x25_macaddr (paktype *pak, long address,
				  addrtype *proto_addr, lcitype **lci,
				  boolean *broadcast)
{
    proto_addr->type = ADDR_QLLC;
    proto_addr->length = ADDRLEN_QLLC;
    ieee_copy( ((addrtype *)address)->qllc_addr, proto_addr->qllc_addr);
    *broadcast = FALSE;
    return(TRUE);
}

static void qllc_sprintf_address_link (char *buffer, addrtype *addr)
{
    sprintf(buffer, "%e", addr->qllc_addr);
    return;
}

static boolean qllc_x25_address_map_enter (x25map *map)
{
    uchar *vmac;

    if (map->map_services & X25_MAPS_QLLC_NATIVE) {
	/* no vmac - no mapping */
	return(TRUE);
    }
	vmac =  &map->map_address[X25_MAP_SECONDARY_ADDR_INX].addr.qllc_addr[0];
    if (lnx_by_macaddr(vmac) != NULL) {
	printf("\nVirtual Mac address %e already in use", vmac);
	return(FALSE);
    }
    if (qllcmap_add(vmac, map) != NULL) {
	return(TRUE);
    }
    return(FALSE);
}

static void qllc_x25_address_map_delete (addrtype *addr)
{
    qllcmap_t	*qllcmap;
    uchar	*macaddr;

    macaddr = &addr->qllc_addr[0];
   
    qllcmap = qllcmap_lookup(macaddr);
    if (qllcmap == NULL) {
	return;
    }
    lnx_x25_map_delete(qllcmap->lnx);

    qllcmap_remove(macaddr);		/* remove the map entry */
}

static void qllc_x25_lci_busy (addrtype *proto_addr, boolean *result)
{
    *result = lnx_busy(proto_addr);
}

/* qllc_x25_pvc_setup
 * 
 * Come here when [no] x25 pvc <n> configured
 */

static boolean qllc_x25_pvc_setup (x25map *map,
				   lcitype *lci, boolean sense)
{
    qllctype	*qllc;
    uchar	*vmac;
    int		lcn;
    QPSapT	*qpsap;
    qllcmap_t	*qllcmap;

    lcn = x25_get_lcn(lci);
    
    qllcmap = NULL;
    if (map == NULL) {
	vmac = NULL;
    }
    else if (map->map_services & X25_MAPS_QLLC_NATIVE) {
	vmac = NULL;
    } else {
	vmac =  &map->map_address[X25_MAP_SECONDARY_ADDR_INX].
	    addr.qllc_addr[0];
	qllcmap = qllcmap_lookup(vmac);
	if (qllcmap == NULL) {
	    if (lnx_error_debug) {
		buginf("\nQLLC Map entry for %e missing", vmac);
	    }
	    return(FALSE);
	}
    }
    qllc = lci->qllc;
    if (sense) {
	if (qllc == NULL) {
            qllc_start();
	    qllc = qllc_findfree();
	    if (qllc == NULL) {
		return(FALSE);
	    }
	    QCepBindLci(qllc, lci);
	    qllc->qllc_ispvc = TRUE;
	    qllc->idb = lci->lci_idb;
	}
	if (qllcmap != NULL) {
	    qllcmap->qllc = qllc;
	}
	if (qllc->lnx != NULL) {
	    return(TRUE);
	}
	/* has some CLS SAP already claimed this lcn */
	qpsap = QSRSearchByPvc(lci->lci_idb, lcn);
	if (qpsap != NULL) {
            if (QCepInPvcList(qpsap, qllc)) {
                printf("\nQLLC: internal error: Entry for lcn %d already exists", lcn);
  		return(FALSE);
  	    }
            QCepCreate(qpsap, qllc, NULL, vmac, NULL);
            CepInit(&qllc->fCep, &lci->lci_holdQ, lci->lci_holdQ.maximum, 
                    QLLC_TXQ_LOW_WATER, QLLC_TXQ_HIGH_WATER, QLLC_CLS_FLOW_IND_DAMP);
	} else {
	    QPvcAddUnclaimed(qllc);
	}
	return(TRUE);

    } else {				/* sense is FALSE */
        if (qllc == NULL) {
            return (TRUE);
        }

    	if (QPvcLookupUnclaimed(qllc)) {
            qllc_clean_qllc_inputQ(qllc->lci);
	    qllc_freeup(qllc);
	    return(TRUE);
	}

        if (QllcIsCls(qllc)) {
            QCepPvcListDelete(qllc->fQSap, qllc);
        } else if (qllc->lnx) {
	    lnx_x25_map_delete(qllc->lnx);
            qllc->qllc_ispvc = FALSE;
	}
	qllcmap_remove(vmac);
	return(TRUE);
    }
}

/*
 * Manage PVC;s
 *
 * There's an AVL tree hanging off the QData that owns the unclaimed
 * ones.
 *
 * Routines exist to insert, delete, search the tree.
 */

/* QPVcAddUnclaimed
 * 
 * Adds qllc to the unclaimed list,  removes prior existing qllctype
 */

void QPvcAddUnclaimed (qllctype *qllc)
{
    QDataT	*qData;
    qllctype	*qPrior;
    boolean	balancing;

    qData = qllc_idb_get_data(qllc->idb, TRUE);
    if (qData == NULL) {
	printf(nomemory);
	qllc_freeup(qllc);
	return;
    }
    qPrior = (qllctype *)avl_search(&qData->fUnclaimedPvcs->avl_node,
				    &qllc->avl_node, QLcnCompare);
    if (qPrior != NULL) {
	QPvcRemoveUnclaimed(qPrior);
    }
   avl_insert(((avl_node_type **)(&qData->fUnclaimedPvcs)),
	       &qllc->avl_node, &balancing, QLcnCompare);
}
void QPvcRemoveUnclaimed (qllctype *qllc)

{
    QDataT	*qData;
    boolean	balancing;

    qData = qllc_idb_get_data(qllc->idb, FALSE);
    if (qData == NULL) {
	return;
    }
    avl_delete(((avl_node_type **)(&qData->fUnclaimedPvcs)),
	       &qllc->avl_node, &balancing, QLcnCompare);
}

static qllctype *QPvcLookupUnclaimed (qllctype *qllc)
{
    QDataT	*qData;

    if (qllc == NULL) {
        return (NULL);
    }

    qData = qllc_idb_get_data(qllc->idb, FALSE);
    if (qData == NULL) {
	return(NULL);
    }

    return( (qllctype *)avl_search(&qData->fUnclaimedPvcs->avl_node,
				   &qllc->avl_node, QLcnCompare));
}

static enum avl_compare QLcnCompare (avl_node_type *node1,
				     avl_node_type *node2)
{
    qllctype	*qllc1;
    qllctype	*qllc2;

    ushort	lcn1;
    ushort	lcn2;

    qllc1 = (qllctype *)node1;
    qllc2 = (qllctype *)node2;

    lcn1 = qllc1->lci ? x25_get_lcn(qllc1->lci) : 0;
    lcn2 = qllc2->lci ? x25_get_lcn(qllc2->lci) : 0;

    if (lcn1 < lcn2) {
	return(AVL_LT);
    } else if (lcn1 == lcn2) {
	return(AVL_EQ);
    } else {
	return(AVL_GT);
    }
}

/* PHIL 
 * IF WE GET HERE WE'RE ALMOST DEAD, 
 * IN SPITE OF OUR EFFORTS WE'VE OVERLFOWED THE lci_holdq
 * DROPPING A QLLC PACKET IS *NOT* OK
 *
 *	????	can we figure out anything to requeue the packets in qllc
 *		LATER
 */


static void qllc_send_quench (paktype *pak)
{
    lcitype *lci = pak->x25_cb;
    reg_invoke_media_x25_sendcause(lci->lci_idb->hwptr, lci, X25_RESET,
				   X25_RESET_NETWORK_CONGESTION, 
				   X25_DIAG_NO_INFO, NULL);

}


/*****************************

	QLLC PACKET INPUTS

*****************************/

/*
 * qllc_enqueue
 *
 * just the raw_enqueue's way of passing us  data
 */

static void qllc_enqueue (paktype *pak)
{
    process_enqueue(qllc_inputQ, pak);
    return;
}

/*
 * qllc_get_stored_xid3
 *
 * Pull the stored XID(3) packet from the qllc 
 * clear the packet from the control block,
 * and return the packet
 *
 * Since this routine can be called at process and 
 * interrupt level, it opens a window for race conditions.
 * Use raise_interrupt_level to shut this window tight.
 */
paktype * qllc_get_stored_xid3 (qllctype *qllc)
{
    leveltype level;
    paktype *pak;
    
    level = raise_interrupt_level(NETS_DISABLE);
    pak = qllc->saved_xid3pak;
    qllc->saved_xid3pak = NULL;
    reset_interrupt_level(level);
    return(pak);
}

/*
 * qllc_input_proc
 * This high priority process blocks - waiting for packets from x.25 
 */
static process qllc_input_process (void)
{
    paktype *pak;
    boolean have_event;
    ulong major, minor;

    process_wait_on_system_init();
    process_watch_queue(qllc_inputQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&QllcMasterTimer, ENABLE);
    
    for (;;) {
        process_wait_for_event();
        while ((have_event = process_get_wakeup(&major, &minor)) == TRUE) {
            switch (major) {

            case QUEUE_EVENT:
                switch (minor) {
                case QLLC_DATAQ_EVENT:                  
                    /* 
                     * Process data packets from X.25.
                     */
                    while ((pak = process_dequeue(qllc_inputQ)) != NULL)  {
                        qllc_input(pak);
                    }
                    break;
                default:
                    break;
                }
                break;

            case TIMER_EVENT:
                /* 
                 * Managed timers.
                 */
                while (mgd_timer_expired(&QllcMasterTimer)) {
                    mgd_timer	*qTimer;
                    qllctype	*qllc;

                    qTimer = mgd_timer_first_expired(&QllcMasterTimer);
                    mgd_timer_stop(qTimer);  /* Make it go away! */
                    qllc = mgd_timer_context(qTimer);

                    switch (mgd_timer_type(qTimer)) {
                    case QX25_TIMER:
                        if (QllcIsCls(qllc)) { 
                            if ((qllc->out_of_order_pvc) || 
                                (qllc->idb->hwptr->x25_upstate == FALSE)) {
                                QXFsmExecute(qllc, QX25OutOfOrderTimer);            
                            } else {
                                QXFsmExecute(qllc, QX25TimerExpires);
                            }
                        }
                        break;
                    case QLLL_TIMER:
                        if (QllcIsCls(qllc)) { 
                            qllc->fRetries++;
                            if (qllc->fRetries < qllc->fTimers.fQLRetries) {
                                QLFsmExecute(qllc, QLTimerT1, NULL);
                            } else {
                                QLFsmExecute(qllc, QLTimerT1N2, NULL);
                            }   
                        }
                        break;
                    case QLNX_TIMER:
                        if (qllc->lnx) {
                            qllc_periodic(qllc);
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;

            default:
                break;
            }
        }
    }
}



/*
 * qllc_input.
 * Come here with qllc packets.
 * pak->datagramstart points to the beginning of the qllc frame,
 * pak->datagramsize is the size of the qllc frame.
 *
 * lapb and x.25 headers have been removed.
 */
void qllc_input (paktype *pak)
{
    register int 	newevent;
    QLInputT 		qEvent;
    qllctype *qllc = NULL;
    idbtype *idb;
    hwidbtype *hwidb;
    lcitype *lci;
    qllc_frmr_t q_frmr;
    paktype *tpak;

    lci  = pak->x25_cb;      /* set by x.25 */

    idb = pak->if_input;
    hwidb = hwidb_or_null(idb);
    if (hwidb == NULL) {
	datagram_done(pak);
	return;
    }

    if ( (!qllc_running) || (hwidb->state != IDBS_UP) || (lci == NULL) ) {
    	datagram_done(pak);
	return;
    }

    /* get the qllc cntrl block - if it doesn't exist we shouldn't have
     * got any packet in the first place
     * For an SVC we allocate qllc at qllc_lci_notify, or qllc_open...
     * For a PVC we allocate during configuration
     */
    qllc = lci->qllc;
    if (qllc == NULL) {
	/* use station component in ADM state to send DM RSP */
	qllc        = qllcstationptr;
	qllc->idb   = pak->if_input;
	qllc->lci   = lci;
        qllc->qllcStat.qllc_numsnddms++;
	qllc_sendcontrol( qllc, QDM, FALSE );
	datagram_done(pak);
	return;
    }
    
    switch (reg_invoke_media_x25_reassemble(&pak, &qllc->qllc_reassmQ)) {
      case X25_QUEUED:
      	return;		/* packet has M bit, handling complete for now */
	
      case X25_NOPACKET:	/* reassembly failed: couldn't get output packet */
        errmsg(&msgsym(NO_QLLCBUFFER, QLLC)); 
	return;

      case X25_PACKET:
      	break;
    }
    	
    if (qllc_packet_debug) {
	QllcPrPacket(qllc, pak, 'I');
    }

    idb            = pak->if_input;
    GET_TIMESTAMP(idb->hwptr->lastinput);	      /* remember time of last input */
    GET_TIMESTAMP(pak->inputtime);		      /* save it in packet too */

    q_frmr.q_ctl	= 0;
    q_frmr.q_cmdrsp	= 0;
    q_frmr.q_WXYZ	= 0;
    
    /* 
     * Determine which qllc packet was rcvd - find_qllcpaktype() is only used
     * for QLLC cntrl packets (q bit set).
     * If Q bit on the packet is set - pak->qllc has PAK_X25_QBIT set.
     */
    if (pak->x25_flags & PAK_X25_QBIT) {
	newevent = find_qllcpaktype(pak, &qEvent, &q_frmr);
    	if (newevent == REC_QXID_CMD) {
	    qllc->qllc_xid_rsp_pending = TRUE;
	}
        tpak = qllc_get_stored_xid3(qllc);
        if (tpak) {
            datagram_done(tpak);
            if (QllcIsCls(qllc)) {
                mgd_timer_start(&qllc->fTimer, qllc->fTimers.fQLTimer);
            } else {
                mgd_timer_start(&qllc->fTimer, QT1_DEFAULT);
            }
        }
    } else {
	newevent = REC_QIFRAME;
	if ((qllc = lci->qllc) != 0){
	    qllc->qllcStat.qllc_inpaks++;
	    qllc->qllcStat.qllc_inbytes += pak->datagramsize;
	}
    }
   
    if (newevent == REC_FRMR_EVENT)
   	qllc->qllc_frmr = &q_frmr;
    else
   	qllc->qllc_frmr = NULL;

    /*  
     * we have now identified the source of the incoming frame 
     * OR are using the station component should a response be needed.
     */
    qllc->inbuf = pak;
    UpdateQllcRxStat(qllc, newevent);
    
    /* hi-jack the rest of this code for CLSI handling if suitable */
    if (QllcIsCls(qllc)) {
        clear_if_input(pak);    /* Packet should no longer be charged to X.25 input */
	QX25PacketInput(qllc, newevent);
	return;
    }

    /* process xid and test no matter what state you're in. */
    if ( (newevent == REC_QXID_CMD) || (newevent == REC_QTEST_CMD) ||
       (newevent == REC_QXID_RSP) || (newevent == REC_QTEST_RSP) )
       qllc_type1event(qllc, newevent);
    else 
     {
      qllc_stateswitch(qllc, newevent);
      if (newevent == REC_QSM_CMD && qllc->sdlcp)
       qllc_fix_sdlc(qllc);

    }

    if (qllc->inbuf) {
	if (pak->refcount >= 1)
	    datagram_done(pak);

	qllc->inbuf = NULL;
    }
    
}


/*
 * qllc_extract_ctl
 *
 * returns the control field, or different values for data packets
 * illegal codes, and short packets get the value BAD_PACKET
 *
 */

uchar qllc_extract_ctl (paktype *pak)
{
    qllc_header *qllchdr = (qllc_header *)pak->datagramstart;
    
    if (pak->datagramsize < QLLC_HDRSIZE)
	return ( TOO_SHORT );
    
    if (pak->x25_flags & PAK_X25_QBIT)
	return ( qllchdr->cntrl );
    
    return (DATA_PACKET);
}

/* UpdateQllcRxStat
 *  update Qllc Input events
 */

static void UpdateQllcRxStat (qllctype* qllc, int event)
{
  switch(event){
    case REC_QXID_CMD:
    case REC_QXID_RSP:
         qllc->qllcStat.qllc_xidin++;
         break;

    case REC_QTEST_CMD:
    case REC_QTEST_RSP:
         qllc->qllcStat.qllc_testin++;
         break;

    case REC_QFRMR_RSP:
         qllc->qllcStat.qllc_numrcvfrmrs++;
         break;

    case REC_QSM_CMD:
         qllc->qllcStat.qllc_numrcvqsms++;
         break;

    case REC_QDISC_CMD:
         qllc->qllcStat.qllc_numrcvdiscs++;
         break;

    case REC_QDM_RSP:
         qllc->qllcStat.qllc_numrcvdms++;
         break;

    case REC_FRMR_EVENT:
         qllc->qllcStat.qllc_numerrs++;
         break;
     }

}

/* 
 * find_qllcpaktype
 * Returns the type of QLLC control packet received.
 * This routine is only used if the packet has the Q bit set - i.e. qllc
 * control packets.  I-frames (with no q bit set) don't come in here.
 *
 * Also returns the event code needed for the Logical Link Layer FSM
 */
static int find_qllcpaktype (paktype *pak, QLInputT *qEvent, qllc_frmr_t *q_frmr)
{
    uchar  cntrl = qllc_extract_ctl(pak);
    uchar  addr  = ((qllc_header *)pak->datagramstart)->addr;
    int    oktype;
    
    *qEvent = FSM_NO_EVENT;

    if (pak->datagramsize < QLLC_HDRSIZE)
	return(REC_TOO_QSMALL);

    q_frmr->q_ctl = cntrl;

    if (addr == QLLC_CMD) {
 
	/* qllc commands */  
	switch (cntrl) {
	case QSM:
	    oktype = REC_QSM_CMD;
	    *qEvent = QLSM;
	    break;
	    
	case QDISC:
	    oktype = REC_QDISC_CMD;
	    *qEvent = QLDISC;
	    break;
	    
	case QXID:
	    oktype = REC_QXID_CMD;
	    *qEvent = QLXID;
	    break;
	    
	case QTEST:
	    oktype = REC_QTEST_CMD;
	    *qEvent = QLTest;
	    break;
	    
	case QRR:
	    oktype = REC_QRR_CMD;
	    *qEvent = QLRR;
	    break;
	    
        /*
         * A QUA should only come in as a Response.  However, a few 
         * QLLC devices will send a QUA as a Command (duh!).  NPSI accepts 
         * this, so we must also (or we won't replace NPSI in those cases).
         * To the rest of the QLLC code, lie and say we received a QUA response.
         */
	case QUA:
            oktype = REC_QUA_RSP;
            *qEvent = QLUA;
            break;   

	default:
	    q_frmr->q_WXYZ = Q_W;
	    *qEvent = QLFRMR;
            return(REC_FRMR_EVENT);		/* that'll do */
	}
    }
    else {
	
	/* qllc responses */
	q_frmr->q_cmdrsp = Q_RSP;
	
	switch (cntrl) {
	case QXID:
            oktype = REC_QXID_RSP;
	    *qEvent = QLXID;
	    break;
	    
	case QTEST:
            oktype = REC_QTEST_RSP;
	    *qEvent = QLTest;
	    break;
	    
        case QRD:
            oktype = REC_QRD_RSP;
	    *qEvent = QLRD;
	    break;
	    
	case QUA:
            oktype = REC_QUA_RSP;
	    *qEvent = QLUA;
	    break;
	    
	case QDM:
            oktype = REC_QDM_RSP;
	    *qEvent = QLDM;
	    break;
	    
	case QFRMR:
            oktype = REC_QFRMR_RSP;
	    *qEvent = QLFRMR;
	    break;
	    
	default:
	    q_frmr->q_WXYZ = Q_W;
	    *qEvent = QLFRMR;
            return(REC_FRMR_EVENT);		/* that'll do */
	}
    }
    
    /* now check the length
     *
     * XID and TEST can have I fields
     * FRMR has exactly five bytes
     * everything else MUST be exactly two bytes
     */
    
    switch (oktype) {
	
    case REC_QXID_CMD:
    case REC_QTEST_CMD:
    case REC_QXID_RSP:
    case REC_QTEST_RSP:
	break;
	
    case REC_QFRMR_RSP:
	if (pak->datagramsize != ( QLLC_HDRSIZE + QFRMR_SIZE) ) {
	    q_frmr->q_WXYZ = Q_WX;
	    *qEvent = QLFRMR;
	    return (REC_FRMR_EVENT);	
 	}
				  
    case REC_QSM_CMD:
    case REC_QDISC_CMD:
    case REC_QRD_RSP:
    case REC_QUA_RSP:
    case REC_QDM_RSP:
	
    case REC_QRR_CMD:
	if (pak->datagramsize != QLLC_HDRSIZE) {
	    q_frmr->q_WXYZ = Q_WX;
	    *qEvent = QLFRMR;
	    return (REC_FRMR_EVENT);	
	}
	break;
    }
    return (oktype);
}



/*
 * qllc_init.
 *
 * initialize global variables.  Don't commit any resources (process ID
 * malloc etc.)
 */
void qllc_init (void)
{
    register int i;

    qllc_running      = FALSE;
    qllc_event_debug  = FALSE;
    qllc_packet_debug = FALSE;
    qllc_state_debug  = FALSE;
    qllc_timer_debug  = FALSE;
    qllc_x25_debug    = FALSE;
    
    lnx_debug         = FALSE;

    queue_init(&qllc_cleanupQ, 0);
    queue_init(&QCepFreeupQ, 0);

    qllc_debug_init();
    
	/* add registry services */
	
    reg_add_raw_enqueue(LINK_QLLC, qllc_enqueue, "qllc_enqueue");
    reg_add_media_x25_macaddr(LINK_QLLC, qllc_x25_macaddr, "qllc_x25_macaddr");
    reg_add_media_x25_notify_event(LINK_QLLC, qllc_x25_lci_notify_event, 
    	"qllc_x25_lci_notify_event");

    reg_add_x25_incoming_call(LINK_QLLC, qllc_x25_incoming_call,
	"qllc_x25_incoming_call");

    reg_add_media_x25_address_map_enter(LINK_QLLC, qllc_x25_address_map_enter, 
    	"qllc_x25_address_map_enter");
    reg_add_media_x25_pvc_setup(LINK_QLLC, qllc_x25_pvc_setup, 
    	"qllc_x25_pvc_setup");

    reg_add_media_x25_address_map_delete(LINK_QLLC, 
        qllc_x25_address_map_delete, "qllc_x25_address_map_delete");
    reg_add_media_x25_lci_busy(LINK_QLLC, qllc_x25_lci_busy, 
    	"qllc_x25_lci_busy");
    reg_add_send_quench(LINK_QLLC, qllc_send_quench, "qllc_send_quench");
    reg_add_sprintf_address_link(LINK_QLLC, qllc_sprintf_address_link,
        "qllc_sprintf_address_link");

    reg_add_dlc_wput(CLS_QLLC, qllc_wput, "qllc_wput");

    for (i = 0; i < QLLC_HASH_SIZE; i++) {
        qllcmap_table[i] = NULL;
    }

}

/* scheduler test for the qllc cleanup process. If there are QLLC
  * entries waiting to be cleaned up, the cleanup process gets unblocked
  */
  
boolean qllc_cleanupBLOCK (void)
{
    if (!QUEUEEMPTY(&QCepFreeupQ))
 	return (FALSE);
 
    if (QUEUEEMPTY(&qllc_cleanupQ))
 	return (TRUE);
    return (FALSE);
}

/*
 * qllc_cleanup_proc
 */
static forktype qllc_cleanup_proc (void)
{
    qllctype *qllc;
        
    /*
     * Process the cleanup queue.
     * We got put on the cleanup queue by goto_adm().
     *
     * This cleanup process got got assigned another task;
     * It take QLLC Ceps for ther QCepFreeup Q, so that they can
     * be deallocated at process level.
     */

    for (;;) {
	edisms( (blockproc *)qllc_cleanupBLOCK, 0);
	while ((qllc = p_dequeue(&QCepFreeupQ)) != NULL) {
  	     QCepDestroy(qllc);
  	 }
 	  
 	 while ((qllc = p_dequeue(&qllc_cleanupQ)) != NULL ) {
 		
 	 /* If in adm state - make  sure that the virtual circuit is done
  	  * release all the associated resources
  	  */
  	     if (qllc->qllc_state == QST_ADM) {
  		 lnx_qllc_notify(qllc->lnx, LNX_DLC_LINKDOWN_IND);

 		 /* Tell someone that we're going away */
  		 if (qllc_event_debug)
  		     buginf("\nQLLC: cleanup qllc, reason=%d", qllc->adm_reason);
 
                  /* Clear the adm reason field so qllc_open does not
                     block on any subsequent reconnection attempts */

		 qllc->adm_reason = 0;
		 qllc_kill_lci(qllc); /* clear/reset lci */

		 /* 
		  * SVCs - remove all traces
		  * Don't want to do this for PVCs - that should only happen
		  * If there's an X25_DELETE_EVENT
		  */
		if (!qllc->qllc_ispvc) {
                    if (qllc->sdlcp)
                        snx_clean_sdlc(qllc);

		     qllc_freeup(qllc);
                } else if ((qllc->idb->hwptr->x25_upstate) && 
                            (!qllc->out_of_order_pvc)) {
                    /* 
                     * PVC - good cause, go ahead and startup 
                     *       the lnx side.
                     */
                    lnx_qllc_notify(qllc->lnx, LNX_DLC_LINKUP_IND);
                    WHICHSTREAM_STARTUP(qllc);
  		}
            } 
	 }
     }
}

/* disconnect/clear when QLLC session is completed */

void qllc_kill_lci (qllctype *qllc)
{
     lcitype   	*lci =  (lcitype *)qllc->lci;
     leveltype	level;
     
     if (lci && qllc->vc_kill_flag) {
        if (qllc_event_debug) {
            buginf("\nQLLC:  kill lci 0x%lx, qllc 0x%lx", lci,  qllc);
        }

       /* 
        * SVC: Send Clear Packet.
        * PVC: Send Reset Packet.
        */
        level = raise_interrupt_level(NETS_DISABLE);
	x25_called_from_api = TRUE;
	if (qllc->qllc_ispvc) {
	    reg_invoke_media_x25_sendcause(qllc->idb->hwptr, lci, X25_RESET,
	    		X25_RESET_DTE_ORIGINATED, X25_DIAG_NO_INFO, NULL);
	}
	else {
	    reg_invoke_media_x25_sendcause(qllc->idb->hwptr, lci, X25_CLEAR,
	    		X25_CLEAR_DTE_ORIGINATED, X25_DIAG_NO_INFO, NULL);
	}
	x25_called_from_api = FALSE;
	reset_interrupt_level(level);
    }
    return;    
}


/*
 * qllc_periodic - 
 *
 * Wakes up once ten seconds and scans the qllc control blocks.
 * Since this idle timer is mostly used for keepalives (no data transfer
 * is dependent on it), a ten second granularity is sufficient.
 *
 * It's driven by a timer set up in start srb_background
 */

void qllc_periodic (qllctype *qllc)
{
    lcitype	*lci;
    paktype     *xid_pak = NULL;

    lci = qllc->lci;

    if (qllc_timer_debug)
	buginf("\nQllc timer lci %d, state %s retry count %d",
	       lci ? lci->lci_lci: -1, 
	       qllc_prstate(qllc->qllc_state),
	       qllc->retry_count);
    
    if ( !qllc->idb->hwptr->x25_upstate) {
	if ( qllc->qllc_state != QST_ADM) {
	    qllc_goto_adm(qllc, QLS_INTDOWN, FALSE);
	}
	return;
    }
    
    /* probe the x25 to make sure that we're not holding back
     * when the x.25 is clear to go
     */
    if (qllc->qllc_x25_busy)
	qllc_psend( qllc, (paktype *)NULL);

    if (qllc->retry_count < qllc->n2) {
	
	switch (qllc->qllc_state) {
	    
	case QST_ADM:
	    if (qllc->sdlcp)
		return;
	    
	    /* as we're not on the cleanup queue do a startup thing,
	     * if we're in ADM state, then we ned to go through a full blown
	     * start (for security reasons at the very least)
	     *
	     * WHICHSTREAM_STARTUP will exchange explorers, and XIDs, this
	     * should result in getting the host to send us a null XID, and
	     * verifying the XID0 we send in reply.
	     */
	    if (!checkqueue(&qllc_cleanupQ, qllc) ) {
                /* 
                 * We may have a situation where the XID(3)
                 * negotiation with a PUT2.1 configured as 
                 * Link "Negotiable" on QLLC needs a "nudge".
                 * Retransmit the XID(3) once in this case.
                 */        
                xid_pak = qllc_get_stored_xid3(qllc);
                if (xid_pak) {
                    qllc_fwd_xid(qllc, xid_pak);
                } else if (qllc->qllc_ispvc && qllc->out_of_order_pvc) {
                    /*
                     * PVC may be out of order.  We have no real way of
                     * telling since the PVC may be up, but the remote device
                     * is not really there.  So we send a QTEST packet.  If we
                     * receive another RESET with an OUT_OF_ORDER cause, we know
                     * not to try to activate the link to the host; otherwise, we
                     * will activate upstream next time the timer pops.
                     */
                    qllc->out_of_order_pvc = FALSE;
                    qllc_sendcontrol(qllc, QTEST, TRUE);
                } else {
                    WHICHSTREAM_STARTUP(qllc);
                }
            }
	    break;
	    
	case QST_SETUP:
            qllc->qllcStat.qllc_numsndqsms++;
            qllc->primaryRole = TRUE;
	    qllc_sendcontrol(qllc, QSM, TRUE);
	    break;
	    
	case QST_DISC:
            qllc->qllcStat.qllc_numsnddiscs++;
	    qllc_sendcontrol(qllc, QDISC, TRUE);
	    break;
	    
	case QST_NORMAL:
	    if (qllc->sdlcp)
		return;	/* ignore all timeouts */
	    qllc->retry_count = 0;
	    /* we shouldn't get bumped out of here */
	    break;

	default:
            errmsg(&msgsym(BADQLLCSTATE, QLLC), qllc_prstate(qllc->qllc_state) ); 
	    break;
	}
	qllc->retry_count++;
	mgd_timer_start(&qllc->fTimer, QT1_DEFAULT);
    }
    else {			/* retry_count blown */
	if (qllc_event_debug)
	    buginf("\nQLLC: qllc_periodic: qllc n2 retries");

        /* NmEvent Hook 3 */
        QllcEvent(qllc, N2_RetryCountExpired);

	qllc_goto_adm(qllc, QLS_TIMEOUT, TRUE); /* resets retry count */
    }
}

/* X.25 Virtual Circuit Handlers
 *
 * Find 'em, start 'em, stop 'em, and reset 'em
 */

/* QllcIsLegacy
 *
 * figure out if the Incoming Call is a legacy (lnx) connection
 *
 * Return TRUE if it is; FALSE for a CLS connection
 *
 * First, look at the x25 address map; if none exists
 * 	x25 code messed up.
 *
 * If there's a virtual macaddress in the map entry, look for the
 * qllcmap, which ought to exist.  If it does, and there's an lnxtype,
 * it's been configured for legacy, else it's CLS 
 */

static boolean QllcIsLegacy (lcitype *lci, paktype *pak)
{
    x25map		*x25map;
    uchar		*vmac;
    qllcmap_t		*qllcmap;

    x25map = lci->lci_map;
    if (x25map->map_addr_count > X25_MAP_SECONDARY_ADDR_INX) {
	/* a x25 map qllc statement was configured */
	vmac = (uchar *)
	    &x25map->map_address[X25_MAP_SECONDARY_ADDR_INX].addr.qllc_addr;

	qllcmap = qllcmap_lookup(vmac);	/* has other uses */
	
	if (qllcmap == NULL) {
	    errmsg(&msgsym(QLLCMAP_ERR, QLLC), vmac);
	} else {
	    if (qllcmap->lnx != NULL) {
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * qllc_x25_incoming_call
 *
 * Handle Incoming Call.
 * Returns TRUE if QLLC is ready to accept the call, 
 * FALSE otherwise
 *
 * It has the option of setting cause diagnostic, and con-
 * textual explanation as needed
 */

static boolean qllc_x25_incoming_call (paktype *pak, lcitype *lci,
	uchar *cause, uchar *diagnostic, uchar **reason)
{
    qllctype 		*qllc;

    if (qllc_x25_debug)
	buginf("\nQLLC Incoming Call %d", lci->lci_lci);

    if (QllcIsLegacy(lci, pak)) {
	if ((qllc = qllc_setup(lci)) ) {
	    WHICHSTREAM_STARTUP(qllc);
	    return(TRUE);
	} else {
	    return(FALSE);
	}
    } else {
	return (QCepIncomingCall(pak, lci, cause, diagnostic, reason));
    }
}

/*
 * Character strings for event codes, must be in the same order as the
 * enumerated constants in x25/x25.h
 */

static char *const x25_event_name[X25_NUM_EVENTS] = {
   "X25_BOGUS",
    "X25_INCALL",
    "X25_ACCEPT",
    "X25_CLEAR_IND",
    "X25_CLEAR_CONF",
    "X25_RESET_IND",
    "X25_RESET_CONF",
    "X25_RR",
    "X25_RNR",
    "X25_UNQUENCH",
    "X25_QUENCH",
    "X25_DELETE",
};

/* pr_x25_event
 *
 * Convert the input event to text
 */

#define MAXSTRING	20

static char *pr_x25_event (uchar event)
{
    static char output[MAXSTRING];

    if (event >=  X25_NUM_EVENTS) {
	sprintf(output,"?%d?", event);
	return(output);    
    } else {
	return(x25_event_name[event]);
    }
}
/*
 * qllc_lci_notify
 *
 * process events from X.25 services.
 * This lets X.25 notify us about interesting things,
 * i.e. CONNECTED, CLEARED, RESET, UNQUENCHED
 */

static void 
qllc_x25_lci_notify_event (lcitype *lci, uchar  event,
			   uchar cause, uchar diag, paktype *pak)
{
    qllctype *qllc;
    lnxtype *lnxp = NULL;
    boolean kill_it;        

    qllc = lci->qllc;
    
    if (qllc_x25_debug || sdlc_qllc_debug) {
	buginf("\nQLLC X25 notify qllc %x lci %d event %s", 
	       qllc, lci->lci_lci, 
	       pr_x25_event(event));
	if (sdlc_qllc_debug && qllc != NULL && qllc->sdlcp != NULL) {
	    buginf("\n sdlcp %x ", qllc->sdlcp);
	}
    }
    if (qllc == NULL) {
	return;
    }

    /*
     * Do this here since CLS should use this also.
     */
    if (event == X25_RESET_IND_EVENT) {
        if ((cause == X25_RESET_NETWORK_OUT_OF_ORDER) ||
            (cause == X25_RESET_OUT_OF_ORDER)) {
            qllc->out_of_order_pvc = TRUE;
        } else {
            qllc->out_of_order_pvc = FALSE;
        }
    }

    /* Get CLS Events out of the main path */

    if (QllcIsCls(qllc)) { 
	QX25EventInput(qllc, event);	/* CLS routine */
	return;
    }
    
    	/* 'Classic' QLLC */
	
    switch(event)
    {

	/* clear - can only happen on an SVC - it means that we know no more
	 * packets are going to go through.  DO things to stop us from even 
	 * trying.
	 *
	 * Reset handling is bad enough,
	 * since it breaks the integrity of the sequence numbering, and
	 * the qllc and llc2 connections have to be taken down.
	 */

    case X25_CLEAR_IND_EVENT:
	CLSASSERT(qllc != NULL);

        if (!qllc->qllc_ispvc) {
	    qllc->lci = NULL;		/* inhibit reuse of lci */
	    lci->qllc = NULL;		/* remove the back-pointer */
	    
	}		/* note fall through */

    case X25_RESET_IND_EVENT:

	if (!(lnx_running || snx_running))
	    break;

	if (qllc == NULL)
	    break;		/* no qllc left for this lci */

        if (qllc->qllc_ispvc) {
            /* 
             * For PVCs, we set the "kill_flag" to FALSE
             * to prevent problem with infinite loops 
             * of RESET packets (CSCdi66340).           
             */
            kill_it = FALSE;
        } else if (event == X25_RESET_IND_EVENT) {
            /* 
             * Rare case - Reset on an SVC.
             * Need to send a CLEAR packet by setting
             * "kill_flag" to TRUE, or the VC could 
             * get stuck in limbo.
             */
            kill_it = TRUE;
        } else {
            /*
             * CLEAR on an SVC.
             * Don't respond to a CLEAR with a CLEAR.
             */
            kill_it = FALSE;
        }
        qllc_goto_adm(qllc, QLS_DISCONNECT, kill_it); /* cleans things up nicely */

	/* clean up the input queue; remove  pending packets for this lci */
        qllc_clean_qllc_inputQ(lci);
	break;
	
    case X25_ACCEPT_EVENT:

	if (!(lnx_running  || snx_running)) {
	    lci->qllc = NULL;
	    reg_invoke_media_x25_sendcause(lci->lci_idb->hwptr, lci, 
		X25_CLEAR, X25_CLEAR_DTE_ORIGINATED, X25_DIAG_NO_INFO, NULL);
	}

	if (qllc->sdlcp) /* Things are already in order for sdlc-qllc business*/
         break;
	
	/*
	 * qllc already exists; this means that the router issued a
	 * Call Request, and has just received the Call Accept
	 * Send any pending XID packet
	 */


	lnxp = qllc->lnx;
	if (lnxp == NULL) {
	    if (lnx_error_debug) {
		buginf("\nQLLC: Call Accept: No lnx pointer for qllctype");
	    }
            break;
	}

        if (lnxp->lnx_xid_pak) {
	    if (qllc_event_debug) {
		buginf("\nQLLC processing held-over XID");
	    }
	    lnx_forward_held_xid(lnxp, lnxp->lnx_xid_pak);
	    lnxp->lnx_xid_pak = NULL;      
	    lnx_setstate(lnxp, LNX_AW_NET_QLLC_SEC);
	    break;
        }

	WHICHSTREAM_STARTUP(qllc); /* assume qllc to llc2 conversion, AND send
				    * TEST or NULL XID
				    */
        lnxp = qllc->lnx;
        lnx_setstate(lnxp, LNX_AW_NET_QLLC_SEC); 
      	break;

    case X25_UNQUENCH_EVENT:
	if (qllc != NULL) {
	    qllc_psend(qllc, (paktype *)NULL);	
	    /* restart pending x.25 traffic */
	}
	break;
    case X25_DELETE_EVENT:	/* lci is just about be deleted */
    	if (qllc != NULL) {
	    qllc->lci = NULL;
            qllc_goto_adm(qllc, QLS_DISCONNECT, FALSE);
	    qllc_clean_qllc_inputQ(lci);
	    if (qllc->sdlcp) {
		snx_clean_sdlc(qllc);
	    }
	}
	break;
    default:
        if (lnx_error_debug)
         buginf("\nBad Input qllc_x25_lci_notify_event %d", event);
	break;
    }

    return;
}

/*
 * qllc_clean_qllc_inputQ
 *
 * pull all inbound packets for a given lci from the qllc input queue
 */
 
void qllc_clean_qllc_inputQ (lcitype *lci)
{
    paktype *pak;
    paktype *head_pak = NULL;
    leveltype level;
 
    level = raise_interrupt_level(ALL_DISABLE);
    while ((pak = process_dequeue(qllc_inputQ)) != NULL)  {
        if (pak == head_pak) {
	    /* 
	     * We been through all the packets once.
	     * break out of the loop
	     */
            process_requeue(qllc_inputQ, pak);
            break;
        }
        if (pak->x25_cb == lci) {
            datagram_done(pak);
        } else {
            process_enqueue(qllc_inputQ, pak);
            if (head_pak == NULL) {
                head_pak = pak;	/* mark the header to end the loop */
            }
        }
    }
    reset_interrupt_level(level);
}


/*
 * qllc_setup
 *
 * create a qllc block
 */
 
qllctype *qllc_setup (lcitype *lci)
{
    x25map *map;
    uchar *macaddr;
    qllctype *qllc;
    lnxtype *lnx;

    if (!lci) {
        if (lnx_error_debug)
	    buginf("\nqllcsetup: NULL LCI");
	return (NULL);
    }

    map = lci->lci_map;
    macaddr = map->map_address[X25_MAP_SECONDARY_ADDR_INX].addr.qllc_addr;
    
    if ( !(lnx = lnx_by_macaddr(macaddr)) ) {
    	if (lnx_error_debug)
		buginf("\nQLLC: %e not configured for QLLC", macaddr);
	return (NULL);
    }
    
    
    if (qllc_event_debug)
	buginf("\nQLLC: allocating  new qllc lci %d", lci->lci_lci);

    if ( !(qllc = qllc_findfree()) ) {
	if (lnx_error_debug)
	    buginf("\nQLLC: couldn't allocate memory for control block");
	return (NULL);
    }

    lci->qllc	        = qllc;
    qllc->lci           = lci;
    qllc->lnx           = lnx;
    qllc->idb           = lnx->idb;
    lnx_fix_qllc(qllc, macaddr);
    return (qllc);
}

/*
 *
 * qllc_quench
 *
 * flow control - handle back pressure from the llc2 side
 *		- exert back pressure on X.25
 *
 * (an unlikely story - but watch for SDLC-QLLC flow control
 * issues)
 *
 * Also needed to make the QLLC DLC conform to LAK2 model
 * PHIL 1/17/94
 */
 

void qllc_quench (qllctype *qllc, boolean quench_on)
{
    paktype     *pak;


    if (qllc_event_debug)
	buginf("\nQLLC: X25 QUENCH %s", quench_on ? "ON": "OFF");
    if ( !qllc ){
        errmsg(&msgsym(NULLPTR, QLLC),"qllc"); 
    }
    qllc->qllc_net_busy = quench_on;

    if (quench_on)
        qllc->qllcStat.qllc_quenchon++;
    else
        qllc->qllcStat.qllc_quenchoff++;
 
    /* if we're not quenched start removing packets and passing them to
     * lnx_output
     */
    while ( !qllc->qllc_net_busy ) {
        pak = p_dequeue(&qllc->qllc_lnxQ) ;
 
        if (pak)
            lnx_output(pak, qllc);
        else
            break;
    }
 
}

/*
 *  Hash routines        
 */

/*
 * qllc_by_dte
 * Find entry in the dte hash table given the dte string and interface.
 * The dte address stored is the virtual mac address of the x.25 sna device.
 * This routine is used when a packet from the llc2 side needs to go to
 * the x.25 side and the qllc block needs to be found.
 * A packet from the x.25 side going to the tr side pick up the qllc block
 * from the lci structure.
 */
qllctype *qllc_by_dte (uchar *macaddr)
{

    lnxtype *lnx;
    
    if (! (lnx = lnx_by_macaddr(macaddr)) ){
               errmsg(&msgsym(NOMACADDR, QLLC), macaddr);
	return (NULL);
    }
    return (lnx->qllc);
}

/*
 * qllc_validate
 * Verify that an qllc exists
 */
boolean qllc_validate (qllctype *qllc)
{
    if (qllc) {
	if (qllc->fUCepId) {
	    return(TRUE);
	} else {
	    return (lnx_validate(qllc->lnx));
	}
    } else {
	return (FALSE);
    }
}



/* 
 * qllc_openBLOCK
 *
 * Wait for open event to occur - happens when a QUA arrives on the line.
 */
static boolean qllc_openBLOCK (qllctype *qllc)
{
    if ( (qllc->open_block != QLS_WAITING) || (qllc->adm_reason) ) {
       if (qllc->adm_reason)
          qllc->open_block = 0;       /* clear block */
       return (FALSE);
    }

    return (TRUE);                    /* continue to block */

}



/********************************
 * External interface routines. *
 ********************************/

/*
 * Opens up first an x.25 connection and then a QLLC connection with
 * the x.25 SNA device.  This routine is called in response to establishing
 * an llc2 session with the TR host.
 * vmacaddr   - virtual mac address of the x.25 sna device.
 * idb        - idb of the x.25 link
 *              blocking state.
 * blocking   - TRUE if blocking open, FALSE if non-blocking open.
 */ 
qllctype *qllc_open (uchar *vmacaddr, idbtype *idb, boolean blocking)
{

    qllctype *qllc;

    if (qllc_event_debug)
	buginf("\nQLLC: qllc_open : vmac %e", vmacaddr);

    /* check if the qllc control block is currently active */
    if ( (qllc = qllc_by_dte( vmacaddr) ) == NULL ) {
            if (lnx_error_debug)
		buginf("\nQLLC: qllc_open qllc block is missing");
	  return (NULL);

    } 

    if (qllc->qllc_state != QST_ADM)  {
	if (qllc_event_debug)
	    buginf("\nQLLC: qllc_open: failed: dte already in use");
        qllc_close (qllc);
	return (NULL);      	/* already in use */
      }

    qllc->adm_reason  = 0;
    qllc->retry_count = 0;
    qllc->qllcStat.qllc_numsndqsms++;
    qllc->primaryRole = TRUE;
    qllc_sendcontrol(qllc,  QSM, TRUE);
    qllc_setstate ( qllc, QST_SETUP );
    qllc->blocking   = blocking;
    /* 
     * If non-blocking, return immediately, else wait for something to
     * happen.
     */
    if (!blocking)
	return(qllc);

    qllc->open_block = QLS_WAITING;

    /* await arrival of QUA rsp OR session timeout */
    edisms( (blockproc *)qllc_openBLOCK, (ulong)qllc);

    if (qllc->open_block == QLS_SUCCESS)
	return(qllc);

    /* open failed; close it down again */
    qllc_close(qllc);
    return(NULL);
}

/* q l l c _ c o n t a c t _ a c c e p t
 *
 * Received a QSM some while ago - send the (deferred QUA)
 * Do any necessary housekeeping here
 */

void qllc_contact_accept (qllctype *qllc)
{
    if (!qllc_validate(qllc) ) {
	if (lnx_error_debug)
	    buginf("\nQLLC: qllc_contact_accept: invalid qllc pointer");
    return;
    }
    qllc->primaryRole = FALSE;
    qllc_sendcontrol(qllc, QUA, FALSE);
    qllc_setstate(qllc, QST_NORMAL);
    return;
}

/* q l l c _ c o n t a c t _ r e j e c t
 *
 * Received a QSM some while ago - send QDM to show that we can't accept the call
 * Do any necessary housekeeping here
 */

void qllc_contact_reject (qllctype *qllc)
{
    if ( !qllc_validate(qllc) ) {
	if (lnx_error_debug)
	    buginf("\nQLLC: qllc_contact_reject: invalid qllc pointer");
    return;
    }
    qllc->qllcStat.qllc_numsnddms++;
    qllc_sendcontrol(qllc, QDM, FALSE);
    qllc_goto_adm(qllc, QLS_DISCONNECT, TRUE);
    return;
}

/*
 * qllc_close - close a QLLC connection. 
 * This can be called from an interrupt; 
 * it sets up qllc in the QDISC state (unless it's in ADM)
 * Returns FALSE if error, else TRUE.
 */
boolean qllc_close (qllctype *qllc)
{
    /* Verify this is a valid qllc */
    if ( !qllc_validate(qllc) ) {
	if (qllc_event_debug)
	    buginf("\nQLLC: attempt to close with invalid qllc=0x%x", qllc);
	return(FALSE);
    }

    if ((qllc->qllc_state != QST_ADM)&&(qllc->qllc_state != QST_DISC)) {
        if (qllc->idb->hwptr->x25_upstate == TRUE){
            qllc->qllcStat.qllc_numsnddiscs++;
  	    qllc_sendcontrol(qllc,QDISC,TRUE);
            qllc_setstate( qllc, QST_DISC );
        }

	qllc->retry_count = 0;
    }
    return(TRUE);
}

void qllc_detach (qllctype *qllc)
{
    lnxtype 	*lnx;
    lcitype 	*lci;
    leveltype	level;

    if (!qllc)
	return;

    level = raise_interrupt_level(NETS_DISABLE);
    lnx = qllc->lnx;
    if (lnx_validate(qllc->lnx) ) {
    	lnx->qllc = NULL;
    }    
    qllc->lnx = NULL;

    lci = qllc->lci;
    if (lci) {
	qllc_clean_qllc_inputQ(lci);
        qllc->vc_kill_flag = TRUE;
	qllc_kill_lci(qllc);
    }
    reset_interrupt_level(level);

    qllc_goto_adm(qllc, QLS_DETACHING, FALSE);
    return;
}

/* q l l c _ f w d _ x i d
 *
 * pass an XID out to the end station
 * This is used to support NT2.1
 * pak comes in pointing to the frame control field
 */
 
boolean qllc_fwd_xid (qllctype *qllc, paktype *pak)
{
    /* add in an address byte - the opposite of qllc_type1event */
    pak->datagramstart --;
    pak->datagramsize++;
    if (qllc->qllc_xid_rsp_pending) {
	*(pak->datagramstart) = QLLC_RSP;
	qllc->qllc_xid_rsp_pending =FALSE;
    } else
	*(pak->datagramstart) = QLLC_CMD;
 
    pak->x25_flags |= PAK_X25_QBIT;
    return (qllc_psend(qllc, pak));
}

/*
 * qllc_send
 * Convenience routine to send an I frame.
 * Entry: pak->datagramstart = beginning of data
 *
 * Returns: FALSE if failure, pak disposed of.
 */
void qllc_send (void *cookie, paktype *pak)
{
    qllctype *qllc;
    int len;

    qllc = cookie;
    len = pak->datagramsize;

    /* Verify this is a valid qllc */
    if (!qllc_validate(qllc)) {
	if (qllc_event_debug)
           buginf("\nQLLC: attempt to send with invalid qllc=0x%x", qllc);
        datagram_done(pak);
	return;
    }

    qllc->qllcStat.qllc_outpaks++;
    qllc->qllcStat.qllc_outbytes += len;
    pak->datagramsize = len;

    /* send date to x.25 only if qllc state is NORMAL, otherwise free pak
     *
     * Also if x.25 is unable to tx the packet, the llc2 session should be
     * pulled down.  Not being done yet.
     */
    if ( (qllc->fLState == QLOpened) || (qllc->qllc_state == QST_NORMAL)) {
    	pak->x25_flags &= ~PAK_X25_QBIT;
	qllc_psend(qllc, pak);
	return;
    }
    else {
       if (qllc_event_debug)
          buginf("\nQLLC: qllc_send: state is %s", qllc->qllc_state);
       datagram_done(pak);
       return;
    }

    return;
}


/* qllc_psend
 *
 * common send routine used both for data and control packets,
 *
 * also called by qllc_lci_notify if X.25 empties its hold queue
 *	(in this case a NULL packet is used)
 *
 * main purpose is to support the qllc hold queue
 *
 * If anything's in the hold queue it has priority.
 *
 * PAK_X25_QBIT is used to tell whether it's a q bit packet
 *
 * Have to use encaps for QBIT packets because x25_vencapsulate doesn't
 * allow the user to set the Q_bit
 *
 * If lci's hold queue is full: don't try to encapsulate 
 *	just put the packet at the tail end of the qllc_holdQ (unencapsulated)
 *	make sure that the quench on message goes out, unless already sent
 */

boolean qllc_psend (qllctype *qllc, paktype *pak)
{
    x25map *map;
    lcitype *lci;

    /* Check the pointers here - qllc->lci can be NULL */
    if (!qllc) {
        if (lnx_error_debug)
            buginf("\nqllc_psend: Bad qllc ptr - packet dropped.");
	if (pak)
 	    datagram_done(pak);
        return(FALSE);
    }
    
    if (qllc->idb->hwptr->x25_upstate == FALSE) {
	if(QllcIsCls(qllc)) {
	    /* X.25 link went down - treat as if we got Clear Indication */
	    QXFsmExecute(qllc, QX25ClearIndication);
	} else {
	    qllc_goto_adm(qllc, QLS_INTDOWN, FALSE);
	    if (pak)
		datagram_done(pak);
	}
	return (FALSE);
    }

    lci = qllc->lci;
    if (!lci) {
        if (lnx_error_debug)
            buginf("\nqllc_psend: Bad qllc->lci ptr - packet dropped.");
	if (pak)
	    datagram_done(pak);
        return(FALSE);
    }

    map = lci->lci_map;
    if (!map) {
        if (lnx_error_debug)
            buginf("\nqllc_psend: Bad lci->map ptr - packet dropped.");
	if (pak)
	    datagram_done(pak);
        return(FALSE);
    }

    if (pak) {
	pak->linktype     = LINK_QLLC;
	pak->if_output    = qllc->idb;
	pak->x25_cb	  = qllc->lci;
    }
 
    if (qllc_packet_debug) {
	QllcPrPacket(qllc, pak, 'O');
    }

    /* handle the best case, where we don't queue up in qllc or lci */
    
    if (pak && QUEUEEMPTY(&qllc->qllc_holdQ) &&
	!QUEUEFULL(&lci->lci_holdQ)) {	/* simple case wrap it and go */

	if ( !reg_invoke_media_x25_encaps(pak,
		(long)(&map->map_address[X25_MAP_SECONDARY_ADDR_INX].addr),
		NULL) )
	    {
            errmsg(&msgsym(GENERRMSG, QLLC),
		"qllc_psend - failed to send pak to x.25"); 
	    datagram_done (pak);	/* just good housekeeping */
	    return (FALSE);
	}

	reg_invoke_media_x25_output(qllc->idb->hwptr, pak, TAIL);

    } else {

	  /* straightforward, put the pak at the BACK of qllc's holdQ
       				before handling the rest of qllc_holdQ */
				
        if (pak)
	    p_enqueue( &qllc->qllc_holdQ, pak);
				
	while (!QUEUEEMPTY(&qllc->qllc_holdQ) && !QUEUEFULL(&lci->lci_holdQ)) {
		
	    pak = p_dequeue(&qllc->qllc_holdQ);

	    if ( !reg_invoke_media_x25_encaps(pak,
		    (long)(&map->map_address[X25_MAP_SECONDARY_ADDR_INX].addr),
		    NULL) )
		{
                errmsg(&msgsym(GENERRMSG, QLLC),
			"qllc_psend - failed to send pak to x.25"); 
		datagram_done(pak); /* just good housekeeping */
		  return (FALSE);
	    }
	    reg_invoke_media_x25_output(qllc->idb->hwptr, pak, TAIL);
	}
    }

    /* if we reach any threshold values, send appropriate messages */
    if (QllcIsCls(qllc)) {
	CepFlowControlInd(qllc->fUCepId, &qllc->fCep);
	return(TRUE);
    }

    if (QUEUEFULL(&lci->lci_holdQ) && !qllc->qllc_x25_busy) {
        qllc->qllc_x25_busy = TRUE;
        lnx_quench (qllc->lnx, TRUE);
}
 
    if (QUEUEEMPTY(&lci->lci_holdQ) && qllc->qllc_x25_busy) {
        qllc->qllc_x25_busy = FALSE;
        lnx_quench (qllc->lnx, FALSE);
    }
	
    return (TRUE);
}



/*
 * qllc_start
 * Called when qllc is configured.
 */
void qllc_start (void)
{
    static pid_t qllc_cleanup_pid = NO_PROCESS;
    static pid_t qllc_input_pid = NO_PROCESS;
    qllctype *qllc;
    
    if (qllc_running)
       return; 

    if (!qllc_inputQ) {
        qllc_inputQ = create_watched_queue("QLLC InputQ", 0,
                                            QLLC_DATAQ_EVENT);
        if (!qllc_inputQ) {
            return;
        }
    }

    if (qllc_input_pid == NO_PROCESS) {
        qllc_input_pid = process_create(qllc_input_process, "QLLC Input",
                                        NORMAL_STACK, PRIO_HIGH);
        if (qllc_input_pid == NO_PROCESS) {
            delete_watched_queue(&qllc_inputQ);
	    return;
	}
    }

    if (qllc_cleanup_pid == NO_PROCESS) {

    /*
     * Create structures for the QLLC cleanup process.
     *     
     * **************** NOTE ****************
     *
     * This continues to use the obsolete processor code.
     * That wil lget cleaned up when the XPE code is folded in.
     *
     * **************** NOTED ***************
     */
 	qllc_cleanup_pid =
 	    cfork(qllc_cleanup_proc, 0L, 0, "QLLC Cleanup", 0);
  	if (qllc_cleanup_pid == NO_PROCESS) {
  	    return;
  	}
    }

    /*
     * Initialize the QLLC Parent Timer.
     */    
    mgd_timer_init_parent(&QllcMasterTimer, NULL);

    /*
     * Allocate a static block used for responding to unconnected
     * circuits, etc.  Some things like the timer defaults won't
     * get initialized.
     */
    if ( (qllc = qllc_findfree() ) == NULL ) {
        errmsg(&msgsym(NOMEM, QLLC)); 
	return; 
    }
    
    qllcstationptr = qllc;
    qllc_running = TRUE;
}

/* qllc_ind_connect
 *
 * qllc opening exchange completed;
 * EITHER qllc and lnx upstream X.25 device downstream
 * OR X.25 device (FEP) upstream and snx (PU2.0) on SDLC
 *
 * We figure out which is which by looking at sdlcp
 */


void qllc_ind_connect (qllctype *qllc)
{
    if (qllc->sdlcp)
	qllc_ind_connect_snx(qllc);
    else if(qllc->lnx)
	qllc_ind_connect_lnx(qllc);
    else
	return;

}


/* q l l c _ i n d _ c o n n e c t _ s n x
 *
 * qllc just got connected, i.e. received QUA to its QSM,
 * If the opener is waiting on this event 
 * (i.e. called qllc_open in blocking mode)
 * we don't have to do anything here, just return;
 *
 * Else make an upcall to lnx to let it know that something 
 * interesting happened
 */
 
void qllc_ind_connect_snx (qllctype *qllc)
{
    lnxtype   *lnx;

    if (!qllc)
	return;

    lnx = qllc->lnx;
    
    if (lnx_running && qllc_idb_test_lnx_active(qllc->idb)) {
	/*
	 * This interface is running because of QLLC.
	 */
	switch(lnx->lnx_state) {
	  case LNX_AW_QLLC_PRI:
	    lnx_setstate(lnx, LNX_CONNECT);
	    lnx_quench(lnx, FALSE);		/* unquench */
	    break;

	  case LNX_AW_QLLC_SEC:
	    lnx_setstate(lnx, LNX_AW_NET);
	    break;

	  case LNX_DISCONNECT:
	  case LNX_DW_NET:
	  case LNX_DW_QLLC:
	  case LNX_AW_NET:
	  case LNX_CONNECT:
	  default:
	    if (lnx_error_debug)
		buginf("\nQLLC: qllc_ind_connect in lnx state %s: %s %e",
		       lnx_prstate(lnx->lnx_state), qllc->idb->namestring, 
		       lnx->vmac_addr);
	    lnx_setstate(lnx, LNX_DISCONNECT);
	    qllc_close(qllc);
	    break;
	}
    }
}



/* q l l c _ i n d _ c o n n e c t _ l n x
 *
 * qllc just got connected, i.e. received QUA to its QSM,
 * If the opener is waiting on this event 
 * (i.e. called qllc_open in blocking mode)
 * we don't have to do anything here, just return;
 *
 * Else make an upcall to lnx to let it know that something
 * interesting happened
 */

void qllc_ind_connect_lnx (qllctype *qllc)
{
    lnxtype   *lnx;
 
    if (!qllc)
        return;
 
    if ( !(lnx = qllc->lnx) ) { /* what !!! lnx went away ??? */
        qllc_close(qllc);
        return;
    }
 
    if ( qllc->blocking )
        qind_concnf(qllc, QLS_SUCCESS); /* unblock qllc_open() */
 
    else {
        qllc->status_flags |= LLCSF_OPEN;
        lnx_dlc_ind_connect(lnx);       /* tell lnx we're up */
    }
}


 
/*
 * qllc_open_upstream_snx
 *
 * Upstream QLLC support.           
 * 3174 -SDLC- <> Router <> -QLLC- Host         
 *
 * Opens up first an x.25 connection and then a QLLC connection with
 * the x.25 SNA Host.  This routine is called after establishing
 * the SDLC connection.
 * vmacaddr   - virtual mac address of the x.25 sna device.
 * idb        - idb of the x.25 link
 *              blocking state.
 * blocking   - TRUE if blocking open, FALSE if non-blocking open.
 * sdbp       - sdlc pointer.
 * pw         - password.
 */
qllctype *
qllc_open_upstream_snx (uchar *vmacaddr, idbtype *idb, boolean blocking, 
     sdlc_data_block *sdb, uchar *pw)
{
 addrtype proto_addr;
 qllctype *qllc;
 lcitype  *lci = NULL;
 lnxtype *lnx;

 uint passwd_size;
/* set the cud to null since x25 will interogate the link */
/* type and insert the proper cud for qllc*/
 uint     cudlen = 0;
 uchar    *cud = NULL;

 if ( !(lnx = lnx_by_macaddr(vmacaddr)) )
  {
    if (lnx_error_debug)
     buginf("\nQLLC: %e: no LNX", vmacaddr);
    return (NULL);
  }
  /* check if the qllc control block is currently active */
  if ( (qllc = qllc_by_dte(vmacaddr) ) != NULL ) {
    /* 
     * if we are already active, either other side is doing cleaning or
     * we are trying after connection is established, ignore(safe gaurd).
     */
    return (NULL);       
    if (qllc->qllc_state != QST_ADM) {  /* this code would never execute */
      return (NULL);         /* already in use */
     }
  } else {
    if ( (qllc = qllc_findfree() ) == NULL ) {
     return (NULL);         /* no memory */
     }
    qllc->idb = lnx->idb;
   }

  /* open the x.25 virtual circuit with the x.25 SNA Host */
  memset(&proto_addr, 0, sizeof(proto_addr));
  ieee_copy(vmacaddr, &proto_addr.qllc_addr[0]);
  proto_addr.type   = ADDR_QLLC;
  proto_addr.length = ADDRLEN_QLLC;

  if (pw != NULL)
     passwd_size = SNX_X25_PASS_SIZE;
  else
     passwd_size = 0;

  lci = reg_invoke_media_x25_place_call(idb, &proto_addr, NULL,
		X25_UNASSIGNED_PREC, LINK_QLLC, cud, cudlen);
                                         
  /*
   * If x.25 connection fails to open free the qllc control block.
   */
  if (!lci)
   {
    if (lnx_error_debug)
     buginf("\nQLLC: cannot set x.25 connection with the x.25 sna device");
    qllc_freeup(qllc);
    return(NULL);
   }
 
  qllc->lci    	   = lci;
  lci->qllc        = qllc;

  qllc->blocking   = blocking;

  qllc_setstate( qllc, QST_ADM );
	 
  qllc->sdlcp = sdb;
 
  /*
   * If non-blocking, return immediately, else wait for something to
   * happen.
   */
  if (!blocking)
   return(qllc);
	 
  qllc->open_block = QLS_WAITING;
	 
  /* await arrival of QUA rsp OR session timeout */
  /* await QSM from the host */
  edisms( (blockproc *)qllc_openBLOCK, (ulong)qllc);
	 
  if (qllc->open_block == QLS_SUCCESS)
   return(qllc);

  /* free the qllc control block */
  qllc_close(qllc);
  return(NULL);
	 
}
	 
 
/*
 * qllc_fix_sdlc()
 *
 * Enable the SDLC Primary side.
 * We have a link end to end, ready for session establishment.
 */
void qllc_fix_sdlc (qllctype *qllc)
{
 sdlc_data_block *sdb;
 lnxtype *lnxp;

 
 /* Let them be a free terminal */
 sdb = qllc->sdlcp;

 lnxp = qllc->lnx;
 
 /* update in case it has changed */
 if (!lnxp->qllc_set_largest)
  {
   lnxp->lnx_dlc_maxbtu =  (1 << qllc->lci->lci_pout);
  }

 sdlc_set_clear_busy(qllc->sdlc_idb, sdb->sdlc_address, FALSE);

}
 
/*
 * qllc_open_upstream_lnx()
 *
 * Upstream QLLC support.           
 * 3174 -TOK- <> Router <> -QLLC- Host         
 *
 * Opens up first an x.25 connection and then a QLLC connection with
 * the x.25 SNA Host.
 * vmacaddr   - virtual mac address of the x.25 sna device.
 * idb        - idb of the x.25 link
 *              blocking state.
 * blocking   - TRUE if blocking open, FALSE if non-blocking open.
 * pw         - password.
 */
qllctype *
qllc_open_upstream_lnx (uchar *vmacaddr, idbtype *idb, boolean blocking, uchar *pw)
{
 qllctype *qllc;
 lcitype  *lci = NULL;

  addrtype proto_addr;

/* set the cud to null since x25 will interogate the link */
/* type and insert the proper cud for qllc*/
 uint     cudlen = 0;
 uchar    *cud = NULL;

 
  /* check if the qllc control block is currently active */
  if ( (qllc = qllc_by_dte(vmacaddr) ) != NULL )
   {
    /* if we are already active, either other side is doing cleaning or
     * we are trying after connection is established, ignore(safe gaurd).
     */
    return (NULL);       
    if (qllc->qllc_state != QST_ADM)  
     {
      return (NULL);         /* already in use */
     }
   }
  else
   {
    if ( (qllc = qllc_findfree() ) == NULL ) 
     {
     return (NULL);         /* no memory */
     }
   }
 
  /* open the x.25 virtual circuit with the x.25 SNA Host */
  /* look up the idb and the x.121 addr in the address map */
  memset(&proto_addr, 0, sizeof(proto_addr));
  ieee_copy(vmacaddr, &proto_addr.qllc_addr[0]);
  proto_addr.type   = ADDR_QLLC;
  proto_addr.length = ADDRLEN_QLLC;

 lci = reg_invoke_media_x25_place_call(idb, &proto_addr, NULL,
		X25_UNASSIGNED_PREC, LINK_QLLC, cud, cudlen); 

  /*
   * If x.25 connection fails to open free the qllc control block.
   */
  if (!lci)
   {
    errmsg(&msgsym(NULLPTR, QLLC),"lci"); 
    qllc_freeup(qllc);
    return(NULL);
   }
 
  qllc->lci        = lci;
  lci->qllc        = qllc;

  qllc->blocking   = blocking;

  qllc_setstate ( qllc, QST_ADM);
  /*
   * If non-blocking, return immediately, else wait for something to
   * happen.
   */
  if (!blocking)
   return(qllc);
	 
  qllc->open_block = QLS_WAITING;
	 
  /* await arrival of QUA rsp OR session timeout */
  /* await QSM from the host */
  edisms( (blockproc *)qllc_openBLOCK, (ulong)qllc);
	 
  if (qllc->open_block == QLS_SUCCESS)
   return(qllc);

  /* free the qllc control block */
  qllc_close(qllc);
  return(NULL);
}
	 
char *qllc_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
   return("N.H");
}

boolean match_qllc_addr (parseinfo *csb, char *buf, int *pi, void *addr_pointer,
                       int in_help, uint flag, idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    ushort hexval[3];

    if (match_hexadecimal(buf, pi, &hexval[0]) &&
        match_char(buf, pi, '.') &&
        match_hexadecimal(buf, pi, &hexval[1]) &&
        match_char(buf, pi, '.') &&
        match_hexadecimal(buf, pi, &hexval[2])) {

        addrp->type = ADDR_QLLC;
        addrp->length = ADDRLEN_QLLC;
	ieee_copy((uchar *)&hexval[0], addrp->qllc_addr);
	return(TRUE);
    }
    return(FALSE);
}

/* QllcPrPacket 
 * 
 * display an input or output packet 
 */

static void QllcPrPacket (qllctype *qllc, paktype *pak, char direction)
{
    uchar ctl;

    if (pak == NULL) {
	return;
    }
    if (qllc == NULL) {
	buginf("\nQLLC: no context ");
    } else {
	buginf("\nQLLC: %s %c: ", qllc->idb->namestring, direction);
    }
    
    ctl = qllc_extract_ctl(pak);
    switch (ctl) {

    case QXID:
	buginf("QXID-%s ",
	       (((qllc_header *)(pak->datagramstart))->addr == QLLC_CMD )?
	       "CMD":"RSP");
	pr_xid(pak, QLLC_HDRSIZE);

	break;
    case QSM:
	buginf("QSM");
	break;	  
    case QDISC:
	buginf((((qllc_header *)(pak->datagramstart))->addr == QLLC_CMD ) ?
	       "QDISC" : "QRD");
	break;
    case QUA:
	buginf("QUA");
	break;
    case QDM:
	buginf("QDM");
	break;
    case QFRMR:
        {
	    qllc_frmr_t *qfrmr;
	    
	    buginf("QFRMR");
	    qfrmr = (qllc_frmr_t *)(&pak->datagramstart[QLLC_HDRSIZE]);
	    buginf(" %02x %02x %02x",
		   qfrmr->q_ctl, qfrmr->q_cmdrsp, qfrmr->q_WXYZ);
    	}
	break;
    case QTEST:
	buginf("QTEST");
	break;
    case QRR:
	buginf("QRR");
	break;
    case DATA_PACKET:
	buginf("Data Packet. %d bytes", pak->datagramsize);
	break;
    case TOO_SHORT:
	buginf("Too Short");
	break;
    default:
	buginf("Unknown Packet Type %02x", ctl);
	break;
    }
}

/************************************************************************
 *									*
 *			      QLLC CLSI SUPPORT				*
 *									*
 * The implementation is object-oriented				*
 *									*
 * The classes we define are						*
 *									*
 * Qllc Sap Registry			QSapRegistryT 	aka QSR		*
 * Qllc Sap				QSapT				*
 * Pvc Range				QSapPVCRangeT			*
 * Virtual Mac address range		QSapVmacRangeT	aka qvmr	*
 * Qllc Cep				qllctype	aka QCep	*
 * Qllc Cep list			QCepListT			*
 * Qllc Timers and Retries		QTimersT			*
 *									*
 ************************************************************************
 */


/************************************************************************
 *									*
 *			QLLC Sap Registry Methods			*
 *									*
 * Aggregation methods for managing SAPs.				*
 *									*
 * The registry consists of two AVL trees; one for SVC SAPs,		*
 * and one for PVC SAPs							*
 *									*
 ************************************************************************
 */


/* QSapRegistryCreate
 *
 * Constructor
 */
 
QSapRegistryT *QSapRegistryCreate (idbtype *idb)
{
    QSapRegistryT	*qsr;

    qsr = malloc_named(sizeof(QSapRegistryT), "QSap Registry");
    idb->qsap_registry = (sap_registry_t *)qsr;
    
    /* Lazily, we assume that malloc returns NULLs */
    
    return(qsr);
}

/* QSapRegistryDestroy
 *
 * Destructor
 *
 * It needs to close all the CEPs associated with the SAP.
 */

void QSapRegistryDestroy (idbtype *swidb)
{
    buginf("Qllc SapRegistry Destructor not implemented");
}

/* comparison routines to run the AVL trees */

/* QSVCSapCompare
 *
 * Compare for SVCs, using X>121 subaddress
 */
 
enum avl_compare QSVCSapCompare (avl_node_type *node1, avl_node_type *node2)
{
    QPSapT *qpsap1, *qpsap2;
    int compare;
    
    qpsap1 =(QPSapT *)node1;
    qpsap2 =(QPSapT *)node2;
    compare = strcmp(QPSapGetSubaddress(qpsap1),
		     QPSapGetSubaddress(qpsap2));
			
    if (compare < 0)
	return (AVL_LT);
    else if (compare == 0)
	return (AVL_EQ);
    else
	return (AVL_GT);
}

/* QSVCSapCompare
 *
 * Compare for PVCs, using lcn ranges
 */
 
enum avl_compare QPVCSapCompare (avl_node_type *node1, avl_node_type *node2)
{
    QPSapT *qpsap1, *qpsap2;
    uint  pvc1Low, pvc1High, pvc2Low, pvc2High;

    qpsap1 =(QPSapT *)node1;
    qpsap2 =(QPSapT *)node2;

    pvc1Low = QPSapGetPvcLow(qpsap1);
    pvc2Low = QPSapGetPvcLow(qpsap2);
    pvc1High = QPSapGetPvcHigh(qpsap1);
    pvc2High = QPSapGetPvcHigh(qpsap2);
	
    if (pvc1High < pvc2Low)
	return (AVL_LT);
    if (pvc2High < pvc1Low)
	return (AVL_GT);
    else
	return (AVL_EQ);
}

/* Qllc Sap Registry operations to look up, add, or remove
 * Sap entries.
 */

/* QSRUSapIdMatch
 *
 * action for walker
 */

static boolean QSRUSapIdMatch (QPSapT *qpsap, va_list argptr)
{
    QSapRegistryT	*qsr;
    word		uSapId;
	
    qsr = va_arg(argptr, QSapRegistryT *);
    uSapId = va_arg(argptr, word);
	
    if (qpsap->fUSapId == uSapId) {
	qsr->fQSapHit = qpsap;
	return (FALSE);			/* terminates the walk */
    } else {
	return (TRUE);
    }
}

/* QSRSearchByUSap
 *
 * Search both the SVC list and the PVC list for our qsap
 */
 
QPSapT *QSRSearchByUSap (QSapRegistryT *qsr, ClsUSapIdT uSapId)
{
    qsr->fQSapHit = NULL;
	
    /* Try SVC's */
    if (avl_walk( (avl_node_type *)qsr->fQSRSvcList, 
		  (avl_walker_type)QSRUSapIdMatch, qsr, uSapId)) {
	/* didn't terminate - no hit */
	
	/* So try PVCs */
	avl_walk( (avl_node_type *)qsr->fQSRPvcList, 
		  (avl_walker_type)QSRUSapIdMatch, qsr, uSapId);
    }
    return (qsr->fQSapHit);
}

#if 0
/* QSRSearchByPSapId
 *
 * look up by PSapId - for validation only
 *
 * Maybe we should use this to be sure that a qsap exists
 * before deleting it.
 */

/* QSRPSapIdMatch
 *
 * action for walker
 */

static avl_walker_type QSRPSapIdMatch (QSapT *qsap, va_list ap)
{
    QSapRegistryT *qsr;
    ClsUSapIdT qsap;
    
    qsr = va_arg(argptr, QSapRegistryT *);
    qsap = va_arg(argptr, ClsUSapIdT);
    
    if (qsap->fQUSapId == qsap) {
	qsr->fQSapHit = qsap;
	return (FALSE);				/* terminates the walk */
    } else {
	return (TRUE);
    }
}

/* QSRSearchByPSapId
 *
 * Search both the SVC list and the PVC list for our qsap
 */

QSapT *QSRSearchByPSapId (QSapRegistryT *qsr, QSapT qsap)
{
    qsr->fQSapHit = NULL;
    
    /* Try SVC's */
    if (avl_walk(qsr->fSVCList, QSRPSapMatch, qsr, uSapId)) {
	/* didn't terminate - no hit */
	
	/* So try PVCs */
	avl_walk(qsr->fPVCList, QSRPSapMatch, qsr, uSapId);
    }
    return (qsr->fQSapHit);
}
#endif

/* QSRSearchBySubaddress
 *
 * Get the sap entry from a subaddress string
 */

static QPSapT *QSRSearchSubaddress (QSapRegistryT *qsr, uchar *subaddress)
{
    QPSapT qpsap;	/* temp for comparison */
    
    CLSASSERT(strlen(subaddress) < ADDRLEN_X121);
    strcpy(QPSapGetSubaddress(&qpsap), subaddress);
    
    return (QSVCSapSearch (qsr, &qpsap));
}

/* QSRSearchByPvcRange
 *
 * Get the sap entry from a pvc range
 */

static QPSapT *QSRSearchByPvcRange (QSapRegistryT *qsr, QSapPVCRangeT *range)
{
    QPSapT qpsap;	/* temp for comparison */
	
    CLSASSERT(range->fPVCLow <= range->fPVCHigh);
    QPSapGetPvcRange(&qpsap)->fPVCLow = range->fPVCLow;
    QPSapGetPvcRange(&qpsap)->fPVCHigh = range->fPVCHigh;
	
    return (QPVCSapSearch (qsr, &qpsap));
}

/* QSRSearchByPvc
 *
 * degenerate case of QSRSearchByPvcRange
 *
 * Create a dummy range that's one lcn wide and go looking for the
 * range
 */

static QPSapT *QSRSearchByPvc (idbtype *idb, int lcn)
{
    QSapRegistryT	*qsr;
    QSapPVCRangeT	range;

    qsr = QllcPortGetSapRegistry(idb);
    if (qsr == NULL) {
	return(NULL);
    }
    range.fPVCLow = range.fPVCHigh = lcn;
    return(QSRSearchByPvcRange(qsr, &range));
}

/* QSRSearchBySap
 *
 * Look for an entry in the Sap Registry that matches the calling sap
 * by subaddress or by PVC range
 */

QPSapT *QSRSearchBySap (QSapRegistryT *qsr, QPSapT *qpsap)
{	
    if (IsSVCPSap(qpsap)) {
	return (QSRSearchSubaddress(qsr, QPSapGetSubaddress(qpsap)));
    } else {	/* PVCs */
	return (QSRSearchByPvcRange(qsr, QPSapGetPvcRange(qpsap)));
    }
}

/* QSRAddQSap
 *
 * Add a QSapEntry to the registry
 */

boolean QSRAddQSap (QSapRegistryT *qsr, QPSapT *new)
{
    QPSapT	*existing;
	
    /* make sure there's no conflict on subaddress or pvc range */
    existing = QSRSearchBySap(qsr, new);
    if (existing != NULL) {
	printf("\n%%QLLC: Sap conflicts with existing Sap");
	return (FALSE);
    }
    
    if (IsSVCPSap(new)) {
	QSVCSapInsert (qsr, new);
    } else {
	QPVCSapInsert (qsr, new);
    }
    return (TRUE);
}

/* QSRRemoveQSap
 *
 * Remove the sap from the registry
 */
 
boolean QSRRemoveQSap (QSapRegistryT *qsr, QPSapT *toast)
{
    if (IsSVCPSap(toast)) {
	return( (boolean)QSVCSapDelete (qsr, toast));
    } else {
	return( (boolean)QPVCSapDelete (qsr, toast));
    }
}	
/* QllcSearchByUcepid
 * 
 * look through all the idb's with qllc sap registries
 * look through all the Sap's; look through all the CEPS
 * Stop if there a match on uCepId
 *
 * FOR NOW THIS IS A STUB ROUTINE
 * [phil]
 */

qllctype *QCepSearchByUCepId (ClsUCepIdT uCepId)
{
    return(NULL);
}

/************************************************************************
 *										*
 *				QLLC SAP Methods				*
 *										*
 * Note that there are different ways to aggregate SVCs and PVCs		*
 *										*
 ************************************************************************
 */
 
/* QSapCreate
 *
 * Constructor - clone from fields in the act.sap parameter block
 * and add in the fields that are really needed
 */
 
QPSapT *QSapCreate (ClsUSapIdT uSapId, QSapT *qActSap, idbtype *idb)
{
    QPSapT	*qpsap;
	
    qpsap = malloc(sizeof (QPSapT));
    if (qpsap == NULL)
	return(NULL);
	
    /* structure assignment */
    qpsap->fQUSap = *qActSap;

    qpsap->fUSapId = (word)uSapId;	/* things we don't get from qActSap */
    qpsap->idb = idb;
    QCepListCreate(qpsap);	
    return (qpsap);
}

/* QSapDestroy
 *
 * Destructor
 *
 * For Sanity's sake we want qllc the CEP's to be gone
 */
 
void QSapDestroy (QPSapT *qpsap)	
{
    QCepListDestroy(qpsap);
    free(qpsap);
}

/************************************************************************
 *									*
 *		Virtual Mac Address Range Methods			*
 *									*
 * Methods to create/Destroy a range,					*
 *									*
 * Methods to add to, delete from, look up, and get next in the 	*
 * QSap Registry (I know, these ought to be QSapRegistry Methods, but	*
 * I don't understand why, since what I'm looking at is QSapVmacRangeT	*
 * objects.  These are static inline wrappers for AVL methods)		*
 *									*
 * Methods for searching for, occupying or release individual virtual	*
 * mac addresses							*
 ************************************************************************
 */
 
/* QVmacRCompare
 *
 * Compare two virtual mac address ranges
 * We use <= because 
 *	the high point on a range of n numbers is one less than
 * 	the low point + n
 */
 
enum avl_compare QVmacRCompare (avl_node_type *node1, avl_node_type *node2)
{
    QSapVmacRangeT *qvmr1, *qvmr2;	
	
    uchar mac1High[IEEEBYTES];
    uchar mac2High[IEEEBYTES];
    uchar *vmac1Low, *vmac2Low;
	
    qvmr1 = (QSapVmacRangeT *)node1;
    qvmr2 = (QSapVmacRangeT *)node2;

    vmac1Low = qvmr1->fVmacStart;
    vmac2Low = qvmr2->fVmacStart;
    ieee_add(vmac1Low, qvmr1->fNumVmacs, mac1High);
    ieee_add(vmac2Low, qvmr2->fNumVmacs, mac2High);

    if ( ieee_compare(mac1High , vmac2Low) <= 0)
	return (AVL_LT);
    if ( ieee_compare(mac2High , vmac1Low) <= 0)
	return (AVL_GT);
    else
	return (AVL_EQ);
}


/* QVmacRangeCreate
 *
 * Constructor
 */
 
QSapVmacRangeT *
QVmacRangeCreate (QPSapT *qpsap, uchar *vmacStart, int numVmacs)
{
    QSapVmacRangeT	*qvmr;
	
    qvmr = malloc(sizeof(QSapVmacRangeT) + numVmacs);
    CLSASSERT(qvmr);
    qpsap->fQVmacRange = qvmr;
    if (qvmr != NULL) {
	ieee_copy(vmacStart, qvmr->fVmacStart);
	qvmr->fNumVmacs = numVmacs;
	qvmr->fQPsap     = qpsap;
    }
    return (qvmr);
}
/* QVmacRangeDestroy
 *
 * Destructor
 */
 
void QVmacRangeDestroy (QPSapT *qpsap)
{
    QSapVmacRangeT	*qvmr;
	
    qvmr = qpsap->fQVmacRange;
	
    if (qvmr != NULL) {
	free(qpsap->fQVmacRange);
	qpsap->fQVmacRange = NULL;
    }
}

static int QSapFindFreeVmac (QPSapT *qpsap, uchar *vmacaddr)
{
    QSapVmacRangeT	*qvmr;
    int			ix;

    qvmr = qpsap->fQVmacRange;
    CLSASSERT(qpsap->fQVmacRange);
	
    if (qvmr == NULL)
	return (NO_VMAC);
    
    for (ix = 0; ix < qvmr->fNumVmacs; ix++) {
	if (!QSapVmacMapInUse(qpsap, ix)) {
	    QSapVmacMapAllocate(qpsap, ix);
	    ieee_add(&qvmr->fVmacStart[0], ix, vmacaddr); 
	    return (ix);
	}
    }
    return (NO_VMAC);
}


boolean QSapVmacMapInUse (QPSapT *qpsap, int ix)
{
    CLSASSERT(qpsap->fQVmacRange);
    return(qpsap->fQVmacRange->fVmacMap[ix] != VMAC_FREE);
}

void QSapVmacMapAllocate (QPSapT *qpsap, int ix)
{
    CLSASSERT(qpsap->fQVmacRange);
    qpsap->fQVmacRange->fVmacMap[ix] = VMAC_IN_USE;
}

void QSapVmacMapFree (QPSapT *qpsap, int ix)
{
    if (qpsap != NULL) {
	CLSASSERT(qpsap->fQVmacRange);
	qpsap->fQVmacRange->fVmacMap[ix] = VMAC_FREE;
    }
}

/************************************************************************
 *									*
 *			QLLC PVC Sap Methods				*
 *									*
 * Attach/Release PVCs to/from their SAP				*
 *									*
 ************************************************************************
 */

/* QSapClaimPvcs
 *
 * When Qllc actsap or act ring is issued and identifies a range of PVCS
 * this code will pick up all the lci's configured for this range
 * and attach them to the SAP.
 *
 * It's considered a config error a pvc within the range to be owned
 * by another x.25 application, or by a qllc srb. 
 */
 
boolean QSapClaimPvcs (QPSapT *qpsap)
{
    idbtype     *idb;
    lcitype     *lci;
    qllctype    *qllc;
    uint        lcn;
    x25map      *x25map;
    uchar       *vmac;

    idb = qpsap->idb;
    for (lcn = QPSapGetPvcLow(qpsap); lcn <= QPSapGetPvcHigh(qpsap); lcn++) {
        lci = x25_lci_lookup(hwidb_or_null(idb), lcn, NULL);
        if (lci == NULL) {
            continue;
        }
        qllc = lci->qllc;
        if (qllc == NULL) {
            return (FALSE);
        }
        if (QPvcLookupUnclaimed(qllc) == NULL) {
            return(FALSE);
        } else {
            QPvcRemoveUnclaimed(qllc);
            /* 
             * Find a vmac if configured.
             */
            x25map = lci->lci_map;
            if (x25map->map_addr_count > X25_MAP_SECONDARY_ADDR_INX) {
                vmac = (uchar *)
                    &x25map->map_address[X25_MAP_SECONDARY_ADDR_INX].addr.qllc_addr;
            } else {
                vmac = NULL;
            }
            /*
             * Possible to call QCepCreate() multiple times for a qllc 
             * control block for PVCs.  Make sure event queues are cleared
             * before calling QCepCreate().
             */
            QEventCleanQueues(qllc);
            QCepCreate(qpsap, qllc, NULL, vmac, NULL);
            CepInit(&qllc->fCep, &lci->lci_holdQ, lci->lci_holdQ.maximum, 
                    QLLC_TXQ_LOW_WATER, QLLC_TXQ_HIGH_WATER, QLLC_CLS_FLOW_IND_DAMP);
        }
    }
    return(TRUE);
}


/************************************************************************
 *									*
 *			QLLC Cep List Methods				*
 *									*
 * Aggregation methods for managing CEPs.				*
 * PVCs are managed in an array; SVCs are managed in an AVL tree	*
 *									*
 ************************************************************************
 */

/* QCepListCreate
 *
 * Constuctor
 *
 * Note that a PVC range includes both endpoints
 * There's nothing to do for SVCs (except make sure that 
 * the tree is empty)
 */

QCepListT *QCepListCreate (QPSapT *qpsap)
{
    int		numPvcs;
	
    if (IsSVCPSap(qpsap)) {
	return ( (QCepListT *)(&qpsap->fQCepList));	/* just the root of the AVL tree */
    } else { 	/* PVCs */
	numPvcs = (QPSapGetPvcHigh(qpsap) - QPSapGetPvcLow(qpsap)) +1;
	qpsap->fQCepList = malloc(numPvcs * sizeof (qllctype *)); 
	return (qpsap->fQCepList);
    }
}
 
/* QCepDetachSap
 *
 * Sap is going down. Detach the cep from it, and close it.
 */
 
static boolean QCepDetachSap (qllctype *qllc, va_list ap)
{
    QPSapT *qpsap;    

    /*
     * Dummy up a CloseStnReq to Clear/Reset
     * the VC as necessary.
     */
    qllc->fUCepId = (qllctype *) CLS_NO_CEP;
    QXFsmExecute(qllc, QX25CloseStnReq);

    /*
     * Cleanup and go away.
     */
    qpsap = qllc->fQSap;
    qllc->fUCepId = NO_CLS;
    qllc->fQSap = NO_CLS;
    if (qllc->qllc_ispvc) {
        QCepReset(qllc);
    } else {
        QCepSvcListDelete(qpsap, qllc);  /* Remove Cep from sap's CepList */
        QX25EventInput(qllc, X25_DELETE_EVENT);        
    }
    return (TRUE);
}

/* QCepListDestroy
 *
 * Destructor
 *
 * In the normal way of things there shouldn't be any Ceps left on
 * the Sap.  If there are, mark the Sap as invalid.  Leave the pointer
 * to the UCep active.
 */
 
void QCepListDestroy (QPSapT *qpsap)
{
    qllctype	*qllc;
    qllctype	**pvcList;
    int		numPvcs;
    int		ix;    

    if (IsSVCPSap(qpsap)) {
	avl_walk( (avl_node_type *)qpsap->fQCepList, 
		 (avl_walker_type)QCepDetachSap);
    } else { 	/* PVCs */
	numPvcs = (QPSapGetPvcHigh(qpsap) - QPSapGetPvcLow(qpsap)) +1;
	pvcList = (qllctype **)qpsap->fQCepList; 
    
	for (ix = 0; ix < numPvcs; ix++) {
	    qllc = pvcList[ix];
            if (qllc) {
                QCepDetachSap(qllc, NULL);
                QPvcAddUnclaimed(qllc);
            }
	}
        free(qpsap->fQCepList);        
    }
}

/* QCepListInsert
 *
 * Add to its list
 */
 
qllctype *QCepListInsert (QPSapT* qpsap, qllctype *qllc)
{
    if (IsSVCPSap(qpsap)) {
	return (QCepSvcInsert(qpsap, qllc));
    } else {
	return (QCepPvcInsert(qpsap, qllc));
    }
}

/* QCepListGetFirst
 *
 * Get the first element
 */
 
qllctype *QCepListGetFirst (QPSapT* qpsap)
{
    if (IsSVCPSap(qpsap)) {
	return (QCepSvcGetFirst(qpsap));
    } else {
	return (QCepPvcGetFirst(qpsap));
    }
}

/* QCepGetNext
 *
 * Get the next element
 */ 

qllctype *QCepGetNext (QPSapT* qpsap, qllctype *qllc)
{
    if (IsSVCPSap(qpsap)) {
	return (QCepSvcGetNext(qpsap, qllc));
    } else {
	return (QCepPvcGetNext(qpsap, qllc));
    }
}

static boolean QCepListWalk (QPSapT *qpsap, QCepWalkT proc, ...)
{
    va_list argptr;

    va_start(argptr, proc);

    if (IsSVCPSap(qpsap)) {
	return((boolean) avl_walk( 
				  (avl_node_type *)qpsap->fQCepList, 
				  (avl_walker_type)QCepSvcProc, 
				  proc, 
				  argptr));
    } else {				/* PVC Walker */
	return(QCepPvcWalk(qpsap, proc, argptr));
    }
    va_end(argptr);
}


/* Cep List SVC Methods - except for the inlines */


enum avl_compare 
QCepSvcCompare  (avl_node_type *node1, avl_node_type *node2)
{
    int compare;
    qllctype *qcep1, *qcep2;

    char *remX121Addr1, *remX121Addr2;
    uchar *partner1, *partner2;
    
    qcep1 = (qllctype *)node1;
    qcep2 = (qllctype *)node2;

    /*
     * First comparison: Remote X.121 Address.
     */
    remX121Addr1 = QCepGetRemX121Addr(qcep1);
    remX121Addr2 = QCepGetRemX121Addr(qcep2);
    compare = strcmp(remX121Addr1, remX121Addr2);
    if (compare < 0) {
	return (AVL_LT);
    } else if (compare > 0) {
	return (AVL_GT);
    }
    

    /* 
     * If both CEPs have partner MAC Addresses, then
     * compare on the partner Mac Address; otherwise,
     * we must stop.
     */ 
    if ( !(QCepTestPartner(qcep1) && QCepTestPartner(qcep2)) ) {
	return (AVL_EQ);
    }
    partner1 = QCepGetPartner(qcep1);
    partner2 = QCepGetPartner(qcep2);
    compare = ieee_compare(partner1, partner2);   
    if (compare < 0) {
	return (AVL_LT);
    } else if (compare > 0) {
	return (AVL_GT);
    } 

    /*
     * Test the Virtual Mac Addresses associated with the SVCs.
     */
    if ( ! (QCepTestVmac(qcep1) && QCepTestVmac(qcep2)) ) {
         return (AVL_EQ);
    }
    compare = ieee_compare(qcep1->fVmac, qcep2->fVmac);
    if (compare < 0) {
	return (AVL_LT);
    } else if (compare > 0) {
	return (AVL_GT);
    } else {
	return (AVL_EQ);
    }
}

/************************************************************************
 *									*
 *			QLLC Cep List PVC Methods			*
 *									*
 ************************************************************************
 */

/* QCepPvcFindSapEntry
 *
 * Find the right place in the range for the qllctype
 */
 
static qllctype **QCepPvcFindSapEntry (QPSapT* qpsap, qllctype *qllc)
{
    ushort	lcn;
    qllctype	**pvcList;
    uint	ix;  
    uint	numPvcs;
	
    numPvcs = (QPSapGetPvcHigh(qpsap) - QPSapGetPvcLow(qpsap)) +1;
    pvcList = (qllctype  **)(qpsap->fQCepList);
	
    lcn = QCepGetLcn(qllc);
    if (lcn > QPSapGetPvcHigh(qpsap)  || lcn < QPSapGetPvcLow(qpsap)) {
		
	CLSASSERT(!"PVC out for range for Sap ");
	return (NULL);
    }
		
    ix = lcn - QPSapGetPvcLow(qpsap);
    return (&pvcList[ix]);
    
}

/* 
 * QCepInPvcList
 */
static boolean QCepInPvcList (QPSapT* qpsap, qllctype *qllc)
{   
    qllctype **pvcEntry;
   
    pvcEntry = QCepPvcFindSapEntry(qpsap, qllc);	
    if (pvcEntry && *pvcEntry) {
        return(TRUE);
    }
    return (FALSE);
}

/* QCepPvcInsert
 *
 * Add a qllc to the PVC list
 */
 
qllctype *QCepPvcInsert (QPSapT* qpsap, qllctype *qllc)
{    
    qllctype	**pvcEntry;
	
    pvcEntry = QCepPvcFindSapEntry(qpsap, qllc);	
	
    if (pvcEntry == NULL) {
       return (NULL);
   }
   
    if (*pvcEntry != NULL) {
	CLSASSERT(!"PVC duplicate for Sap");
	return (NULL);
	}
    *pvcEntry = qllc;
    return (qllc);
}

/* QCepPvcListDelete
 *
 * Remove a qllc from the PVC list
 */
 
qllctype *QCepPvcListDelete (QPSapT* qpsap, qllctype *qllc)
{
    qllctype	**pvcEntry;
	
    pvcEntry = QCepPvcFindSapEntry(qpsap, qllc);	
	
    if (pvcEntry == NULL) {
   	return (NULL);
    }
   
    if (*pvcEntry != qllc) {
	CLSASSERT(!"Trying to remove non-existent pvc from sap ");
	return (NULL);
    }
    *pvcEntry = NULL;
    return(qllc);    
}

/* QCepPvcGetFirst
 *
 * Find the first qllc in the PVC list
 */
 
qllctype *QCepPvcGetFirst (QPSapT* qpsap)
{
    qllctype	**pvcList;
    qllctype	*qllc;
    uint	ix;    
    uint	numPvcs;
	
    numPvcs = (QPSapGetPvcHigh(qpsap) - QPSapGetPvcLow(qpsap)) +1;
    pvcList = (qllctype **)(qpsap->fQCepList); 
	
    for (ix = 0; ix < numPvcs; ix++) {
  	qllc = pvcList[ix];
	if (qllc != NULL) {
	    return (qllc);
	}
    }
    return (NULL);
}

/* QCepPvcGetNext
 *
 * Find the next qllc in the PVC list
 */
 
qllctype *QCepPvcGetNext (QPSapT* qpsap, qllctype *qllc)
{
    qllctype	**pvcList;
    ushort	lcn;
    uint	ix;    
    uint	numPvcs;

    numPvcs = (QPSapGetPvcHigh(qpsap) - QPSapGetPvcLow(qpsap)) +1;
    pvcList = (qllctype **)(qpsap->fQCepList);
	
    lcn = QCepGetLcn(qllc);
    if (lcn > QPSapGetPvcHigh(qpsap)  || lcn < QPSapGetPvcLow(qpsap)) {
	CLSASSERT(!"PVC out for range for Sap");
	return (NULL);
    }

    for (ix = lcn - QPSapGetPvcLow(qpsap); ix < numPvcs; ix++) {
  	qllc = pvcList[ix];
	if (qllc != NULL) {
	    return (qllc);
	}
    }
    return (NULL);
}

boolean QCepPvcWalk (QPSapT *qpsap, QCepWalkT proc, va_list argptr)
{
    boolean 	retval;
    qllctype	*qllc;
    qllctype	**pvcList;
    uint	ix;    
    uint	numPvcs;
	
    numPvcs = (QPSapGetPvcHigh(qpsap) - QPSapGetPvcLow(qpsap)) +1;
    pvcList = (qllctype **)(qpsap->fQCepList);

    for (ix = 0; ix < numPvcs; ix++) {
  	qllc = pvcList[ix];
	if (qllc != NULL) {
	    mem_lock(qllc);
	    retval = (*proc)(qllc, argptr);
	    if (mem_refcount(qllc) == 1) {
		/*
		 * Obscure condition - if the (proc) was a display
		 * routine, it may have blocked. If so, the contents
		 * of the list may have changed (while we were
		 * waiting).  Just ignore this and continue
		 */
		free(qllc);
		return(TRUE);
	    }
	    free(qllc);
	    if (!retval) {
		return(FALSE);
	    }
	} /* qllc != NULL */
    }
    return(TRUE);
}

boolean QCepSvcProc (qllctype *qllc, va_list argptr)
{
    boolean	retval;
    QCepWalkT	*proc;
    va_list	argptr2;

    proc = va_arg(argptr, QCepWalkT *);
    argptr2   = va_arg(argptr, va_list);

    mem_lock(qllc);
    retval = (*proc) (qllc, argptr2);
    if (mem_refcount(qllc) == 1) {
	/*
	 * Obscure condition - if the (proc) was a display routine,
	 * it may have blocked. If so, the contents of the tree may have
	 * changed (while we were waiting).  If we got here, we're the last
	 * one hanging onto this node in the tree, and it's probably toast.
	 * Abort the walk as we're screwed.
	 */
	free(qllc);
	va_end(argptr);
	return(FALSE);
    }
    free(qllc);				/* to go with mem_lock */
    return(retval);
}

/************************************************************************
 *									*
 *			QLLC Cep Methods				*
 *									*
 * Methods for a single CEP.						*
 *									*
 ************************************************************************
 */

/* QCepCreate
 *
 * constructor
 *
 * well, not quite - qllc_findfree is the legacy function
 * that calls malloc.   PHIL
 *
 * If we want any of these parameters, we'll pass them in, if
 * not we'll pass in NULL.  It's up to the Sap to call this
 * routine, both for incoming and outgoing calls
 *
 * If the X121 address is NULL, we had better be dealing with a
 * PVC!
 */


static qllctype *QCepCreate (QPSapT *qpsap, qllctype *qllc, 
			     QLLCReqOpenStnT *qllcReqOpenStn,
                             uchar *vmac, char *x121address)
{
    QSapT	*qsap;
    int		index;
    int		ix;

    if (qllc == NULL) {
        qllc = qllc_findfree();
        if (qllc == NULL) {
            errmsg(&msgsym(NO_RESOURCE, QLLC));
            return(NULL);
        }
    }
    
    qllc->fUCepId = (void *)CLS_NO_CEP;
    qllc->fQSap = qpsap;
    qllc->idb = qpsap->idb;
    qllc->fVmacIndex = NO_VMAC;

    /* Set up for FSMs */
    qllc->fX25State = QX25_P1;
    qllc->fLState = QLClosed;

    memcpy(&qllc->fTimers, &QDefaultTimers, sizeof(QDefaultTimers));

    for (ix = 0; ix < MAX_QLLC_EVENTS; ix++) {
	p_enqueue(&qllc->fFreeEventQ, &qllc->fEvents[ix]);
    }
    /* since findfree calls malloc all the fields are zeroed, 
     * and flags are cleared 
     */

    if (x121address) {			/* used for SVC */
	memcpy(&qllc->fRemote, x121address, sizeof (qllc->fRemote));
	QCepSetRemote(qllc);
    }
    qsap = &qpsap->fQUSap;


    if (qllcReqOpenStn != NULL) {	
	/* 
	 * Outgoing call - DLU provides Vmac if needed.
         * Put the partner in the CEP.
	 */
	if ((qllcReqOpenStn->fParms.fDestAddr.type == STATION_IEEE48) &&
            (QSapTestVmac(qsap)) ) {
            /*
             * If a partner is configured, then make sure 
             * that the partner matches.
             */
            if (QSapTestPartner(qsap)) {
                if (ieee_compare(qllcReqOpenStn->fParms.fSrcAddr.addr,
                    QSapGetPartner(qsap)) != 0) {                    
                    errmsg(&msgsym(PARTNER_MISMATCH, QLLC), 
                           qllcReqOpenStn->fParms.fSrcAddr.addr,
                           QSapGetPartner(qsap));
                    QCepDestroy(qllc);
                    return(NULL);
                }
            }
	    ieee_copy(qllcReqOpenStn->fParms.fDestAddr.addr, qllc->fVmac);
	    QSetVmac(&qllc->fQCepFlags);
            ieee_copy(qllcReqOpenStn->fParms.fSrcAddr.addr, qllc->fPartner);
            QSetPartner(&qllc->fQCepFlags);
        }
    } else {
    	/*
	 * Incoming Call
	 * if the caller's X.121 address maps to a vmac, use it,
	 * else if the sap has a vmac pool use that,
	 * else limited to native addressing
	 */
	if (vmac != NULL) {
	    ieee_copy(vmac, qllc->fVmac);
	    QSetVmac(&qllc->fQCepFlags);
	} else if (QPSapTestVmac(qpsap)) {
	    index = QSapFindFreeVmac(qpsap, qllc->fVmac);
	    if (index == NO_VMAC) {
                errmsg(&msgsym(NO_RESOURCE, QLLC));
		QCepDestroy(qllc);
		return(NULL);
	    } else {
		qllc->fVmacIndex = index;
		QSetVmac(&qllc->fQCepFlags);
	    }
	}
        if (QTestPartner(&qsap->fQSapFlags)) {
            snpa_copy_to_buffer(&qsap->fPartner, qllc->fPartner);
            QSetPartner(&qllc->fQCepFlags);
        }
    }


    if (QSapTestTRSaps(qsap)) {
	qllc->fDsap = qsap->fDsap;
	qllc->fSsap = qsap->fSsap;
	QSetTRSaps(&qllc->fQCepFlags);
    } else {
	qllc->fDsap = SAP_IBM_SNA;
	qllc->fSsap = SAP_IBM_SNA;
    }
	
    if (QTestTimers(&qsap->fQSapFlags)) {
	memcpy(&qllc->fTimers, &qsap->fQTimers, sizeof(QTimersT));
	QSetTimers(&qllc->fQCepFlags);
    }

    if (QPSapTestProxyXid(qpsap)) {
	qllc->fProxyXid = qpsap->fQUSap.fQSapXid;
	QCepSetProxyXid(qllc);
    }

    if (QTestNpsiPoll(&qsap->fQSapFlags)) {
        QSetNpsiPoll(&qllc->fQCepFlags);
    }

    /*
     * Insert into the Cep List.
     */
    if (!QCepListInsert(qpsap, qllc)) {	
        /* This shouldn't happen */
        CLSASSERT(!"QCepListInsertFailed");
        if (!qllc->qllc_ispvc) {
            QCepDestroy(qllc);
        }
        return(NULL);
    }

    mgd_timer_set_type(&qllc->fTimer, QX25_TIMER);
    if (qllc->qllc_ispvc) {
	mgd_timer_start(&qllc->fTimer, QT1_DEFAULT);
    }
    return(qllc);	
}

/* QCepDestroy
 *
 * Destructor
 *
 * Some associated packets and and the memory are freed in qllc_freeup
 * PHIL
 */
 
void QCepDestroy (qllctype *qllc)
{
    paktype *pak;
	
    QCepUnbindLci(qllc);

    /* purge any outstanding packets */
    pak = qllc->fPacket;

    qllc->fPacket = NULL;
    if (pak) {
	datagram_done(pak);
    }

    /* remove all the packets associated with the 
     * Logical Link Event Queue
     */
	 
    while ( QLFsmEventDequeue(qllc) != FSM_NO_EVENT) {
	pak = qllc->fPacket;
	qllc->fPacket = NULL;
	if (pak) {
	    datagram_done(pak);
	}
    }
	
    /* return the virtual mac to the pool */
    if (qllc->fVmacIndex != NO_VMAC) {
	QSapVmacMapFree(qllc->fQSap, qllc->fVmacIndex);
    }
	
    qllc_freeup(qllc);
    return;
}

/* PakDrainQ
 * pull all the packets of the queue,
 * and deallocate them
 */

static void PakDrainQ (queuetype *queue)
{
    paktype *pak;

    while ((pak = p_dequeue(queue)) != NULL) {
	datagram_done(pak);
    }
}
/* QCepReset
 *
 * Clean up data
 */

void QCepReset (qllctype *qllc)
{
    PakDrainQ(&qllc->qllc_holdQ);
    PakDrainQ(&qllc->qllc_lnxQ);
    qllc->qllc_net_busy = FALSE;
    qllc->qllc_x25_busy = FALSE;
    
    PakDrainQ(&qllc->qllc_reassmQ);
 
    qllc->qllc_xid_rsp_pending = FALSE;
    qllc->adm_reason = 0;

    qllc->blocking = FALSE;
    qllc->status_flags = 0;

    qllc_clean_qllc_inputQ(qllc->lci);
    if (qllc->inbuf) {
	datagram_done(qllc->inbuf);
	qllc->inbuf = NULL;
    }

    qllc->retry_count = 0;

    qllc->lnx_running = FALSE;

	/* CLSI Fields */
    qllc->fRetries = 0;
    QEventCleanFsmQueues(qllc);
    datagram_done(qllc_get_stored_xid3(qllc));

    qllc->fClsRetries = 0;
    qllc->fQLFsmRunning = FALSE;
    qllc->fQXFsmRunning = FALSE;
    qllc->fX25State = QX25_P1;
    qllc->fLState = QLClosed;
    if (qllc->fQSap && !QSapTestPartner(&qllc->fQSap->fQUSap)) {
        ieee_zero(qllc->fPartner);
        QClearPartner(&qllc->fQCepFlags);
    }
    return;
}

/* badsubaddress
 *
 * prepare and output error message for an invalid subaddress
 */

static void badsubaddress (addrtype *dst_addr, addrtype *idb_x25_addr)
{
    errmsg(&msgsym(BAD_SUBADDRESS, QLLC), dst_addr->addr.x121_address, 
	   idb_x25_addr->addr.x121_address);
    return;
}

/* QCepIncomingCall
 *
 * boolean function called when an Incoming Call packet is received
 *
 * Returns TRUE is the connection is acceptable
 * FALSE, if the call needs to be Cleared.
 *
 * Get x.25 address for the interface,
 * Match up till they diverge,  everything else is subaddress (can be null)
 * Get the sap,
 *
 * If there's no match on the SAP, then see if we can find the sap
 * through a Vmac Range Search.  This only works if the vmac is in the
 * qllc Vmac Range map.
 * 
 * Get a new qllc type
 * Pass the message on to the FSM
 *
 * Address strings are stored internally as (length, bytes[length])
 * without a terminating 0
 */

static boolean QCepIncomingCall (paktype *pak, lcitype *lci, 
	uchar *cause, uchar *diagnostic, uchar **reason)
{
    addrtype		*dstAddr;
    addrtype		*srcAddr;
    addrtype 		*idbAddr;
    uchar		subaddress[ADDRLEN_X121];
    int			subaddrLength;
    
    x25map		*x25map;
    uchar		*vmac;		/* vmac mapped to X.121 */
    uchar		*remoteX121;	/* char string: who's calling */

    idbtype		*idb;
    QSapRegistryT 	*qsr;
    QPSapT		*qpsap;
    qllctype		*qllc;
    
    vmac = NULL;			/* for the compiler */
    idb = pak->if_input;

    x25map = lci->lci_map;
    if (x25map->map_addr_count > X25_MAP_SECONDARY_ADDR_INX) {
	/* a x25 map qllcstatement was configured */
	vmac = (uchar *)
	    &x25map->map_address[X25_MAP_SECONDARY_ADDR_INX].addr.qllc_addr;
    }

    remoteX121 = (uchar *)
	&x25map->map_address[X25_MAP_PRIMARY_ADDR_INX].addr.x121_addr;

    qsr = QllcPortGetSapRegistry(idb);
    if (qsr == NULL) {
	errmsg(&msgsym(NO_QSR, QLLC));
	*reason = "\nQllc SAP registry missing";
	return(FALSE);
    }

/* Extract the subaddress */

    dstAddr = x25_get_dst_address(pak, lci);
    srcAddr = x25_get_src_address(pak, lci);
     
    idbAddr = x25_get_idb_x25_address(idb);
     
    CLSASSERT(dstAddr->type == ADDR_X121);
    CLSASSERT(idbAddr->type == ADDR_X121);
    
    /* sanity - called address must be at least as long as interface's
     * X.121 address, and called must start with the same digits.
     */
    subaddrLength = dstAddr->length - idbAddr->length;
    if (subaddrLength < 0) {
	badsubaddress(dstAddr, idbAddr);
	return(FALSE);
     }
    if (memcmp(dstAddr->addr.x121_address, 
	       idbAddr->addr.x121_address, 
	       idbAddr->length) != 0) {
	badsubaddress(dstAddr, idbAddr);
	return(FALSE);
	
    }
    
    memcpy(subaddress, &(dstAddr->addr.x121_address[idbAddr->length]),
    	subaddrLength);
    subaddress[subaddrLength] = '\0';	/* make it a C string */
    

    /* 
     * Find the SAP - it has to exist.
     */
    qpsap = QSRSearchSubaddress(qsr, subaddress);
    if (qpsap == NULL) {
        errmsg(&msgsym(INCALL_CFG, QLLC), subaddress);
        return(FALSE);
    }

    /*
     * For Incoming Calls, if we are using a virtual Mac Address (DLSw+),
     * then we better have a partner configured as well.
     */
    if ((vmac || QPSapTestVmac(qpsap)) && !QPSapTestPartner(qpsap)) {
        errmsg(&msgsym(INCALL_NO_PARTNER, QLLC), subaddress);
        return(FALSE);
    }

    qllc = QCepCreate(qpsap, NULL, NULL, vmac, srcAddr->addr.x121_address);
    if (qllc == NULL) {
	return(FALSE);
    }

    QCepBindLci(qllc, lci);
    CepInit(&qllc->fCep, &lci->lci_holdQ, lci->lci_holdQ.maximum, 
	    QLLC_TXQ_LOW_WATER, QLLC_TXQ_HIGH_WATER, QLLC_CLS_FLOW_IND_DAMP);
    
    QXFsmExecute(qllc, QX25IncomingCall);
    return (TRUE);
}

/* QCepMatchReq
 *
 * match a qllc to a request open station request
 */

static boolean QCepMatchReq (qllctype *qllc, va_list argptr)
{
    QLLCReqOpenStnT 		*qllcReqOpenStn;
    ReqOpnStnQLLCParms_t	*parms;

    qllcReqOpenStn = va_arg(argptr, QLLCReqOpenStnT *);
    parms = &qllcReqOpenStn->fParms;

    if (snpa_get_type(&parms->fDestAddr)==  STATION_IEEE48) {
        if (ieee_equal(parms->fDestAddr.addr, qllc->fVmac) &&
            (parms->fSrcSap == qllc->fDsap)) {
            if (QTestPartner(&qllc->fQCepFlags)) {
                if (ieee_equal(parms->fSrcAddr.addr, qllc->fPartner)) {
                    qllc->fQSap->fQCepHit = qllc;
                    return(FALSE);      /* terminate the walk */
                }
            } else if (qllc->qllc_ispvc) {
                /*
                 * It's a PVC but no partner was specified
                 * in the "qllc dlsw pvc..." command.  
                 * Allow any to connect to this Mac Address.
                 */
                ieee_copy(parms->fSrcAddr.addr, qllc->fPartner);
                QSetPartner(&qllc->fQCepFlags);
                qllc->fQSap->fQCepHit = qllc;
                return(FALSE);          /* terminate the walk */
            }
        }
    }

    else if (snpa_get_type(&parms->fDestAddr) == STATION_X121) {
	if (strcmp(parms->fDestAddr.addr, qllc->fRemote) == 0) {
	    qllc->fQSap->fQCepHit = qllc;
	    return(FALSE);		/* terminate the walk */
	}
    }
	
    else if (snpa_get_type(&parms->fDestAddr)==   STATION_X25PVC) {
	if (GETSHORT(&parms->fDestAddr.addr) == QCepGetLcn(qllc)) {
	    qllc->fQSap->fQCepHit = qllc;
	    return(FALSE);		/* terminate the walk */
	}
    }
	
    else {
	CLSASSERT("!Invalid station type");
    }
    return(TRUE);
}

/* QCepFindOpened
 *
 * Find a qllctype that matches the Request Open Station Request
 *
 * This is used on an Incoming Call, so that it can be correlated
 * to a existing qllc
 */

qllctype *QCepFindOpened (QPSapT *qpsap, QLLCReqOpenStnT *qllcReqOpenStn)
{
    /* If dest addr and Src addr are ieee48 then we have to look at
     * mapped virtual vmac addresses, and partner's sap.
     * Local vmac is fDestAddr
     * Partner is fSrcAddr, fSrcSap.
     * 
     * Else dest addr can be an x121 address or a pvc
     */
    qpsap->fQCepHit = NULL;
    QCepListWalk(qpsap, (QCepWalkT *)QCepMatchReq, qllcReqOpenStn);

    return (qpsap->fQCepHit);
}

/* QFindX121Address
 *
 * Look at an address type, and see if it's either a direct X.121 address
 * or look it up in the X.25 address map
 *
 *  THIS IS GROSS, but I don't want to ask gil for the service just yet
 */

static boolean 
QFindX121Address (uchar *x121Address, hwaddrtype *hwaddr, hwidbtype *hwidb)
{
    x25map *x25map;
    addrtype addr;
    addrtype *addrP;

    memset(&addr, 0, sizeof(addr));
    addr.type = snpa_get_type(hwaddr);	/* ??? won't go so well for native mode PHIL */
    addr.length = snpa_get_length(hwaddr);
    snpa_copy_to_buffer(hwaddr, (uchar *)(&addr.x121_addr));

    if (snpa_get_type(hwaddr) == STATION_X121) {
	addrP = &addr;
    } else if (snpa_get_type(hwaddr) == STATION_IEEE48) {
	addr.type = ADDR_QLLC;
	x25map = x25_map_find(hwidb, &addr, 0,  X25_MAPS_QLLC);
	if (x25map == NULL) {
	    return (FALSE);
	} 
	addrP = &x25map->map_address[X25_MAP_PRIMARY_ADDR_INX].addr;
    } else {
	printf("%%Incompatible type for X.25 address map %d", 
	       snpa_get_type(hwaddr));
	return(FALSE);
    }
    memcpy(x121Address, &addrP->addr.x121_address, ADDRLEN_X121);
    return(TRUE);
}

static boolean 
QX121AddressLookup (QPSapT *qpsap, QLLCReqOpenStnT *qllcReqOpenStn,
		    uchar *x121Address)
{
    idbtype 		*idb;
    hwidbtype		*hwidb;

    idb = qpsap->idb;
    hwidb = hwidb_or_null(idb);

    CLSASSERT(hwidb);

    if (QFindX121Address(x121Address, &qllcReqOpenStn->fParms.fDestAddr,hwidb))
	return (TRUE);


/* no luck with Request Open Stn Req, try the SAP */

    if (QPSapTestRemote(qpsap)) {
	return (QFindX121Address(x121Address, &qpsap->fQUSap.fRemote, hwidb));
    }
    return(FALSE);
}

/* QCepReqOpenStn
 *
 * Handle Request Open Station Request
 *
 * If it's in response to an id stn.Ind, then the qllc should already exist
 * make sure that's so
 *
 * Else the call originates with DLU, and we populate the cep from
 * the SAP parameters.
 *
 * In either case we perform the override with parameters in Request Open Stn
 *
 * constructor - part two
 * update the cep with live parameters from 
 * Request Open Station Request
 *
 */

qllctype *QCepReqOpenStn (QPSapT *qpsap, ClsUCepIdT uCepId, 
			  QLLCReqOpenStnT *qllcReqOpenStn, qllctype *qllc)
{
    if (qllc == NULL) {			/* nope: it's an outgoing call */
	char x121Address[ADDRLEN_X121];/* we want our own copy */

	/*
	 * look in the Req Open Stn, then the sap.  
	 *
	 * If it's PVC we should have already found it.  
	 *
	 * If it's a virtual mac address, we have to look in the X.25
	 * address map
	 */

        if (QTestPvc(&qpsap->fQUSap.fQSapFlags)) {
	    return((qllctype *)QllcNoRemoteAddress);
        } 

	if (!QX121AddressLookup(qpsap, qllcReqOpenStn, x121Address)) {
            if (lnx_error_debug) {
                buginf("\nQLLC: error - no remote address for outgoing call!");
            }
	    return((qllctype *)QllcNoRemoteAddress);
	}
	qllc = QCepCreate(qpsap, NULL, qllcReqOpenStn, NULL, x121Address);
	if (qllc == NULL) {
	    return ( (qllctype *)QllcNoMem);
	    /* could also be busy */
	}
    }
    qllc->fUCepId = uCepId;

    if (qpsap->fQUSap.fMaxIfield != 0) {
	qllc->fQMaxBtu = qpsap->fQUSap.fMaxIfield;
    } else {
	qllc->fQMaxBtu = QLLC_DEFAULT_MAX_IFIELD;
    }

    return (qllc);
}

/* QCepBindLci
 *
 * Build the linkage between qllctype and lci
 */
 
void QCepBindLci (qllctype *qllc, lcitype *lci)
{
    CLSASSERT(qllc);
    if (qllc == NULL)
	return;
		
    qllc->lci = lci;
    if (lci != NULL) {
	lci->qllc = qllc;
    }
}

/* QCepUnbindLci
 *
 * Remove the linkage between qllctype and lci
 */
 
void QCepUnbindLci (qllctype *qllc)
{
    lcitype 	*lci;
    leveltype	level;
	
    if (qllc == NULL)
	return;
    
    level = raise_interrupt_level(NETS_DISABLE);
    lci = qllc->lci;
    qllc->lci = NULL;
		
    if (lci != NULL)
	lci->qllc = NULL;
    reset_interrupt_level(level);
}


/************************************************************************
 *									*
 *			QLLC Cep Inputs from X.25			*
 *									*
 ************************************************************************
 */


/* QX25EventInput
 *
 * Present the X.25 event to the appropriate FSM.
 * No packets needed here; the only packet that was interesting
 * was the Incoming Call Packet, and that is passed through the
 * QCepIncomingCall path.
 *
 * We need special handling for DELETE_EVENT.  Packets must be removed
 * from qllc_inputQ.
 *
 * We need special handling for UNQUENCH_EVENT.  It;s time to start
 * pumping out the packets again.
 */
 
static void 
QX25EventInput (qllctype *qllc, uchar event)
{
    int 		qEvent;
    boolean		useLFsm;
	
    useLFsm = FALSE;
	
    switch(event) {
	
    default:
    case X25_BOGUS_EVENT:
	qEvent = (QXInputT)FSM_NO_EVENT;
	break;
    case X25_INCALL_EVENT:
	qEvent = (QXInputT)FSM_NO_EVENT;
	break;
    case X25_ACCEPT_EVENT:
	qEvent = QX25CallConfirm;
	break;
    case X25_CLEAR_IND_EVENT:
	qEvent = QX25ClearIndication;
	break;
    case X25_CLEAR_CONF_EVENT:
	qEvent = (QXInputT)QX25ClearConfirm;
	break;
    case X25_RESET_IND_EVENT:
        if (qllc->out_of_order_pvc) {
            qEvent = QX25ResetOutOfOrder;
	} else if ((qllc->qllc_ispvc) &&
	    (qllc->fX25State == QX25_P4_D1) && 
	    (qllc->fLState == QLClosed)) {
	    /*
	     *	For PVCs (with CLS), Reset Indication is ambiguous, because
	     *	it really has two meanings:
	     *  1) PVC Partner wants to establish a connection.
	     *	2) PVC Partner wants to close a connection.
	     *	This can result in a synchronization problem.
	     *	One way to "resynchronize" is to stay in P4/D1 when
	     *	we get a RESET, if the QLLC link is not yet established.
	     * 
	     *	We could drive the RESET thru the FSM and have an action
	     *	routine look at the link state and drive some new event 
	     *	through the FSM... but (IMHO) that is just as ugly as what
	     *	we are doing here...
	     */
	    if (qllc_state_debug) {
		buginf("\nReset Ind, PVC UP and Link Closed - (ResetCfm, X, X)");
	    }
	    qEvent = (QXInputT)FSM_NO_EVENT;
	} else {
	    qEvent = QX25ResetIndication;
        }
	break;
    case X25_RESET_CONF_EVENT:
	qEvent = QX25ResetConfirm;
	break;
    case X25_RR_EVENT:
	qEvent = QX25QuenchOff;
	useLFsm = TRUE;
	break;
    case X25_RNR_EVENT:
	qEvent = QX25QuenchOn;
	useLFsm = TRUE;
	break;
    case X25_UNQUENCH_EVENT:
	if (qllc != NULL) {
	    qllc_psend(qllc, (paktype *)NULL);	
	    /* restart pending x.25 traffic */
	}
	qEvent = (QXInputT)FSM_NO_EVENT;
	break;
    case X25_DELETE_EVENT:
        qllc_clean_qllc_inputQ(qllc->lci);
        QCepUnbindLci(qllc);
        qEvent = QX25Delete;
	break;
    }

    if (qEvent == FSM_NO_EVENT)
	return;
    
    if (useLFsm) {
	QLFsmExecute (qllc, (QLInputT)qEvent, NULL);
    } else {
	QXFsmExecute (qllc, (QXInputT)qEvent);
    }
    return;
}

/* QX25PacketInput
 *
 * Come here with a packet once we've recognized the
 * qllctype for a CSLI supported one.  Get it ready for 
 * the Logical Link Layer FSM.
 */
 
static void QX25PacketInput (qllctype *qllc, int newevent)
{
    QLInputT	qEvent;
    paktype     *pak;
	
    qEvent = QMapToCsliEvent(newevent);
	
    if (qEvent != (QLInputT)FSM_NO_EVENT) {
        /*
         * We only need to forward the packet for
         * XIDs and I-frames.
         */
        if ((qEvent == QLXID) || (qEvent == QLData)) {
            pak = qllc->inbuf;
        } else {
            pak = NULL;
            datagram_done(qllc->inbuf);
        }
	QLFsmExecute(qllc, qEvent, pak);
    } else if (qllc->inbuf) {
	datagram_done(qllc->inbuf); 
	/* If this ever executes at interrupt level 
	 * call protocol_discard instead 
	 */
    }
    qllc->inbuf = NULL;
}

/* QMapToCsliEvent
 *
 * Map input to qllc Logical Link Layer event
 * Assume that the raw event types are ordered,
 * and start at 1
 */
 
static QLInputT QMapToCsliEvent (int event)
{
    static const QLInputT  QEventMap[] = {
	(QLInputT)FSM_NO_EVENT,		/* QRAW_TOO_LOW		*/
	QLXID,				/* REC_QXID_CMD		*/	
	QLTest,				/* REC_QTEST_CMD	*/
	QLXID,				/* REC_QXID_RSP		*/
	QLTest,				/* REC_QTEST_RSP	*/
	QLSM,				/* REC_QSM_CMD		*/
	QLDISC,				/* REC_QDISC_CMD	*/
	QLRD,				/* REC_QRD_RSP		*/
	QLUA,				/* REC_QUA_RSP		*/
	QLDM,				/* REC_QDM_RSP		*/
	QLFRMR,				/* REC_QFRMR_RSP	*/
	QLRR,				/* REC_QRR_CMD		*/
	QLData,				/* REC_QIFRAME		*/
	QLFRMR,				/* REC_FRMR_EVENT	*/
	(QLInputT)FSM_NO_EVENT,		/* REC_TOO_QSMALL	*/
    };

    if (event <= QRAW_TOO_LOW || event >= QRAW_TOO_HIGH) {
	/* it's invalid */
	CLSASSERT(!"QMapToCsliEvent input event out of range");
	return ( (QLInputT)FSM_NO_EVENT);
    } else {
	return (QEventMap[event]);
    }
}


/************************************************************************
 *									*
 *			QLLC MAP METHODS				*
 *									*
 ************************************************************************
 */

/* qllc_hash_vmac
 *
 * build a hash key in the usual way
 */

static inline int qllc_hash_vmac (uchar *vmac)
{	
    return(xnethash(vmac, IEEEBYTES) & (QLLC_HASH_SIZE -1));
}

/* qllcmap_add
 *
 * add it to the hash table 
 * fail if it's already there, and has a an x25 map entry
 *	returning NULL
 *
 * else return the map entry
 */

qllcmap_t *qllcmap_add (uchar *vmac, x25map *map)
{
    qllcmap_t	*qllcmap;
    int		n;
    leveltype	level;

    qllcmap = qllcmap_lookup(vmac);
    if (qllcmap != NULL) {
	if (qllcmap->map != NULL) {
	    printf("\n%%Duplicate virtual MAC addr %e.  Ignored", vmac);
	    return (NULL);
	} else {
	    qllcmap->map = map;
	    return(qllcmap);
	}
    } else {
	qllcmap = malloc(sizeof(qllcmap_t));
	if (qllcmap == NULL) {
	    printf(nomemory);
	    return (NULL);
	} else {
	    qllcmap->map = map;
	    ieee_copy(vmac, qllcmap->vmac);

	    n = qllc_hash_vmac(vmac);
	    level = raise_interrupt_level(NETS_DISABLE);
	    qllcmap->next = qllcmap_table[n];
	    qllcmap_table[n] = qllcmap;
	    reset_interrupt_level(level);
	    return(qllcmap);
	}
    }
}


/* qllcmap_remove
 *
 * remove from the hash table, fail if it's not there
 */

boolean qllcmap_remove (uchar *vmac)
{
    int 	n;
    qllcmap_t 	*qllcmap, **prev;
    leveltype	level;

    n = qllc_hash_vmac(vmac);

    level = raise_interrupt_level(NETS_DISABLE);
    prev = &qllcmap_table[n];
    qllcmap = *prev;

    while (qllcmap) {
	if (ieee_equal(vmac, qllcmap->vmac)) {
	    *prev  = qllcmap->next;
	    qllcmap->next = NULL;
	    reset_interrupt_level(level);
	    free(qllcmap);
	    return(TRUE);
	}
	prev = &qllcmap->next;
	qllcmap = qllcmap->next;
    }

    reset_interrupt_level(level);
    if (qllc_event_debug)
	buginf("\nQLLC: qllcmap_remove: map entry %e not found", vmac);

    return(TRUE);
}

/* qllcmap_lookup
 *
 * returns lnx map entry,
 * or NULL, if there isn't one
 */

qllcmap_t *qllcmap_lookup (uchar *vmac)
{
    qllcmap_t	*qllcmap;
    int		n;
    leveltype 	level;
    
    n = qllc_hash_vmac(vmac);
    
    level = raise_interrupt_level(NETS_DISABLE);
    for (qllcmap = qllcmap_table[n]; qllcmap != NULL;
	 qllcmap = qllcmap->next) {
	if (ieee_equal(qllcmap->vmac, vmac)) {
	    reset_interrupt_level(level);
	    return (qllcmap);
	}
    }
    reset_interrupt_level(level);
    return (NULL);
}

/************************************************************************
 *									*
 *			QLLC Access to idb fields			*
 *									*
 ************************************************************************
 */

/* qllc_hwidb_get_data
 *
 * If there isn't any qllc data attached, make it up
 * if we are forced to.
 */

QDataT *qllc_hwidb_get_data (hwidbtype *hwidb, boolean force)
{
    QDataT	*qData;

    if (hwidb == NULL) {
	return(NULL);
    }
    if (hwidb->qllc_data != NULL) {
	return(hwidb->qllc_data);
    }
    if (!force || onintstack()) {	/* don't/can't return anything */
	return(NULL);
    }
    qData = malloc(sizeof(QDataT));
    queue_init(&qData->fMsgQ, QMSG_MAX);
    hwidb->qllc_data = qData;
    return(qData);
}

/************************************************************************
 *									*
 *			QLLC DLSw+ SUPPORT				*
 *									*
 * DLSw+ requires a few things from QLLC:				*
 *									*
 * 1) Handle the configuration command, and build CLSI messages		*
 * 2) Pass prepared CLSI messages back to DLSw for Activate Ring	*
 * 3) Pull Rings from the QLLC SAP registry, and do the nv_write 	*
 * 4) Support for csb->sense is FALSE					*
 *									*
 ************************************************************************
 */

/* forward declaration */

static TCLSIMsg *QllcGetActRingMsg (idbtype *swidb);
static void  QllcUnconfigDlsw(parseinfo *csb, idbtype *idb);

/*  QllcConfigDlsw
 *
 * Parse the configuration options,
 * build a CLSI message, and put it on the Act Ring Pending
 */

void QllcConfigDlsw (parseinfo *csb, idbtype *idb)
{
    TCLSIMsg		*theMsg;
    QDataT		*qData;
    ActSapQLLCParms_t	*qActSap;
    QSapT		*qsap;
    ulong 		flags;
    ulong		xid;		/* ASSUME XID string is FOUR bytes */

    qData = qllc_idb_get_data(idb, TRUE);
    if (qData == NULL) {
	printf("\n%%Insufficient Memory");
	return;
    }

    flags = (ulong)GETOBJ(int,1);

    /* validate parameters */
    if (*GETOBJ(string,1) && QTestPvc(&flags) ) {
	printf("\n%%Configured for both SVC and PVC");
	return;
    }

    if (QTestTRSaps(&flags) &&
	( (GETOBJ(int,5) | GETOBJ(int,6)) & 1) )  {
	printf("\n%%Saps must be even");
	return;
    }

    if (QTestProxyXid(&flags)
	&& (!valid_XID(GETOBJ(string,1), (uchar *)&xid, XID_SIZE)) ) {
	return;
    }

    if (!csb->sense) {
	QllcUnconfigDlsw(csb, idb);
	return;
    }

    /* build CLSI Message */

    theMsg =  CLSIMsgCreateHeader(CLS_ACTIVATE_RING | CLS_REQUEST,
  					   offsetof(ActRngReq_t,fParms) -
					   CLS_ACT_RING_REQ_HDR_SZ +
					   sizeof(ActRngQLLCParms_t),
					   CLS_LLC_LAYER,
					   CLS_ENTITY_MGR,
					   0);
    if (theMsg == NULL) {
	printf(nomemory);
	return;
    }

    qActSap = (ActRngQLLCParms_t *)(((ActRngReq_t*)theMsg)->fParms);
    qsap = &qActSap->fQActSap;
    memset(qsap, 0, sizeof(QSapT));

    qsap->fQSapFlags = flags;
    
    if (QTestPvc(&flags)) {
	qsap->fDispatch.fQSapPvcRange.fPVCLow = GETOBJ(int,2);
	qsap->fDispatch.fQSapPvcRange.fPVCHigh = ISOBJ(int,3) ?
	    GETOBJ(int,3) : GETOBJ(int,2);
    } else if (ISOBJ(paddr,1)) {
	strcpy(qsap->fDispatch.fQSapSubaddress, GETOBJ(paddr,1)->x121_addr);
    }

    if (QTestVmac(&flags)) {
	snpa_copy_to_buffer( GETOBJ(hwaddr,1), qActSap->fVmacStart);
	if (ISOBJ(int,4)) {
	    qActSap->fNumVmacs = GETOBJ(int,4);
	} else {
	    qActSap->fNumVmacs = 1;
	}
    }

    if (QTestPartner(&flags)) {
	snpa_copy(GETOBJ(hwaddr,2), &qsap->fPartner);
    }

    if (QTestTRSaps(&flags)) {
	qsap->fDsap = GETOBJ(int,5);
	qsap->fSsap = GETOBJ(int,6);
    }

    if( QTestProxyXid(&flags)) {
	qsap->fQSapXid = xid;
    }

    /* put the message on the 'Pending' queue */
    CLSIMsgSetLink(theMsg, NULL);
    p_enqueue( &(qData->fMsgQ), theMsg);

    reg_add_dlsw_get_ring_req(CLS_QLLC, 
			      QllcGetActRingMsg, "QllcGetActRingMsg");
    reg_invoke_srt_set_srb_params(hwidb_or_null(idb), csb->sense);
}

/* QllcUnconfigDlsw
 *
 * Unconfigure DLSw for a given QLLC connection
 *
 * This is done by:
 *	finding the QPSap from the configuration paramters
 *	extracting the USapId (lives in the CLS core)
 *	using USapId to build a Decativate Ring CLSMessage
 *	passing the message (via registry) to DLSw+
 */
 
 
static void QllcUnconfigDlsw (parseinfo *csb, idbtype *idb)
{
    QSapRegistryT 	*qsr;
    QPSapT		theQpsap;
    QPSapT		*qpsap;
    QSapPVCRangeT	*qpvcr;
    TCLSIMsg		*theMsg;
	
    qsr = QllcPortGetSapRegistry(idb);
    if (qsr == NULL) {
	printf("%%\nInterface not configured for QLLC");
	return;
    }
	
    /* build up a data structure for a search template */

    qpsap = &theQpsap;

    memset(qpsap, 0, sizeof(QPSapT));
    qpsap->fQUSap.fQSapFlags = GETOBJ(int,1);
	
    if (IsSVCPSap(qpsap)) {
	strcpy(QPSapGetSubaddress(qpsap), GETOBJ(paddr,1)->x121_addr);
    } else {
	qpvcr = QPSapGetPvcRange(qpsap);
	qpvcr->fPVCLow  = GETOBJ(int,2);
	qpvcr->fPVCHigh = ISOBJ(int,3) ?
            GETOBJ(int,3) :GETOBJ(int,2);
    }
	
    qpsap = QSRSearchBySap(qsr, qpsap);
    if(qpsap == NULL) {
	printf("\n%%QLLC subaddress or PVC range parameters not recognized");
	return;
    }
    theMsg =  CLSIMsgCreateHeader(CLS_DEACTIVATE_RING | CLS_REQUEST,
				  0, 
				  CLS_LLC_LAYER,
				  CLS_P_SAP_ID,
				  qpsap->fUSapId);
    if (theMsg == NULL) {
	printf(nomemory);
	return;
    }
    reg_invoke_dlsw_queue_cls_message(theMsg);
}

/* QllcNvWriteDlswSap
 *
 * Single action routine to spit out the confguration command:
 *
 * qllc dlsw 
 *	[{subaddress subaddress}|{pvc pvc-low [pvc-high]}] 
 *	[vmac <virtual mac address> [<poolsize>]] 
 *	[partner <partner mac address>] 
 *	[sap dsap ssap] 
 *	[xid xidstring]
 *	[npsi-poll]
 */

static boolean QllcNvWriteDlswSap (QPSapT *qpsap)
{
    QSapVmacRangeT	*qvmr;
    QSapT		*qsap;
    
    qsap = &qpsap->fQUSap;

    if (! (QTestDlsw(&qsap->fQSapFlags)) ) {
	return(TRUE);
    }
    nv_write(TRUE, "qllc dlsw ");
    
    if (IsSVCSap(qsap)) {
    	nv_add(*(QPSapGetSubaddress(qpsap)), 
	       "subaddress %s ", QPSapGetSubaddress(qpsap) );
    } else {
    	nv_add(TRUE, "pvc %d ",  QPSapGetPvcLow (qpsap));
	nv_add(QPSapGetPvcLow (qpsap) != QPSapGetPvcHigh (qpsap), 
	       "%d ",  QPSapGetPvcHigh (qpsap));
    }
    if (QPSapTestVmac(qpsap)) {
	qvmr = qpsap->fQVmacRange;
	nv_add(TRUE, "vmacaddr %e ", qvmr->fVmacStart);
	nv_add(qvmr->fNumVmacs > 1, "%d ", qvmr->fNumVmacs);
    }
    nv_add(QSapTestPartner (qsap), "partner %e ",
	   QSapGetPartner(qsap));

    nv_add(QSapTestTRSaps(qsap), "sap %02x %02x ", qsap->fDsap, qsap->fSsap);

    nv_add(QSapTestProxyXid(qsap), "xid %08x ", qsap->fQSapXid);

    nv_add(QSapTestNpsiPoll(qsap), "npsi-poll ");

    return(TRUE);
}

/* QllcNvWriteDlsw
 *
 * Look at all the SAPs in the SAP registry, and re-generate
 * the corresponding "qllc dslw" command if they are DSLW SAPs
 */

void QllcNvWriteDlsw (parseinfo *csb)
{
    QSapRegistryT       *qsr;
   
    if (!is_x25(csb->interface->hwptr)) {
	return;
    }
    qsr = QllcPortGetSapRegistry(csb->interface);
    if (qsr == NULL) {
	return;
    }
    /* walk the svc SAPs */
     avl_walk( (avl_node_type *)qsr->fQSRSvcList,
	      (avl_walker_type)QllcNvWriteDlswSap);

    /* ... and the PVCs */
    avl_walk( (avl_node_type *)qsr->fQSRPvcList,
	     (avl_walker_type)QllcNvWriteDlswSap);
    return;
}

/* QllcGetActRingMsg
 *   Pull ActRing/DeactRing messages off the queue.
 *
 */

static TCLSIMsg *QllcGetActRingMsg (idbtype *swidb)
{
    QDataT	*qData;

    qData = qllc_idb_get_data(swidb, FALSE);
    if (qData == NULL) {
	return(NULL);
    }
    
    return(p_dequeue(&(qData->fMsgQ)) );
}

/* QllcShowOneCep
 *
 * Display the info for a single Cep
 */

static void QllcShowOneCep (qllctype *qllc)
{
    lcitype *lci;
    
    lci = qllc->lci;
    
    if (lci != NULL) {
	printf("\n lci %d", lci->lci_lci);
	if (strcmp(qllc->fRemote, "")) {
	    printf(" %s", qllc->fRemote);
	}
	printf("\n Circuit State %s, Logical Link State %s", 
	       prQXState(qllc->fX25State), prQLState(qllc->fLState) );
		
	if (!is_ieee_zero(qllc->fVmac))
	    printf(" %e(%02x)->", qllc->fVmac, qllc->fSsap);
	if (!is_ieee_zero(qllc->fPartner))
	    printf("%e(%02x)", qllc->fPartner, qllc->fDsap);
    }
}

/* QllcShowSapsCeps
 *
 * Walk a CepList displaying QLLC Cep Info
 * for all the Ceps belonging to the QLLC SAP.
 */
 
static void QllcShowSapsCeps (QPSapT *qpsap)
{
    QCepListWalk(qpsap, (QCepWalkT *)QllcShowOneCep);
    return;
}

/* QllcShowCeps
 *
 * Display all the Ceps
 */

void QllcShowCeps (void)
{
    QSapRegistryT       *qsr;
    idbtype		*idb;
    
    
    FOR_ALL_SWIDBS(idb) {
	if (!(is_x25(idb->hwptr)) )
	    continue;
	qsr = QllcPortGetSapRegistry(idb);
	if(qsr == NULL)
	    continue;
			
	printf("\nInterface %s", idb->namestring);

	/* walk the svc SAPs */
	avl_walk( (avl_node_type *)qsr->fQSRSvcList,
		 (avl_walker_type)QllcShowSapsCeps);


	/* ... and the PVCs */
	avl_walk( (avl_node_type *)qsr->fQSRPvcList,
		     (avl_walker_type)QllcShowSapsCeps);
    }
}
