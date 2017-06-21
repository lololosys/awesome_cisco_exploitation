/* $Id: c4000_fddi.c,v 3.9.18.7 1996/08/28 13:12:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_fddi.c,v $
 *------------------------------------------------------------------
 * C4000 platform specific FDDI code.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c4000_fddi.c,v $
 * Revision 3.9.18.7  1996/08/28  13:12:40  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.18.6  1996/08/26  15:11:29  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.9.18.5  1996/07/23  13:28:29  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.18.4  1996/07/16  20:55:19  nrvankay
 * CSCdi62177:  smt-que-ue-threshold cannot be removed
 * Branch: California_branch
 * Move the csb->sense check up. Don't reclaim the smt buffers because
 * get_smt_bufs will take care of this.
 *
 * Revision 3.9.18.3  1996/07/09  06:04:36  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.9.18.2  1996/05/17  11:54:05  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.4.2  1996/04/26  15:24:12  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.4.1  1996/04/03  21:41:59  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.18.1  1996/03/18  22:00:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/13  02:00:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/02/20  17:58:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/07  16:15:29  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/02/01  06:09:23  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7.2.1  1996/01/24  23:02:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1996/01/22  07:15:35  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/05  10:18:16  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.5  1996/01/05  04:56:56  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.4  1995/12/28  12:56:05  iraznjev
 * CSCdi37664:  crash in xboot when fddi valid-transmission-time configured
 *
 * Revision 3.3  1995/11/17  18:45:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/08  16:48:57  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.3  1995/07/18 06:48:45  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.2  1995/06/21 03:15:22  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  22:56:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***********************************************************************
				Includes
***********************************************************************/
/* cisco includes */
#include "master.h"
#include "logger.h"
#include "../les/msg_fddi.c"		/* Not a typo, see logger.h */
#include "../xdi/cschdrsif.h"
#include "../if/fddi.h"
#include "config.h"
#include "parser.h"
#include "../if/parser_defs_fddi.h"
#include "../os/old_timer_callbacks.h"
#include "../os/old_timer_callbacks_private.h"
#include "stacks.h"

/* xdi includes */
#include "../xdi/xdihdrsif.h"
#include "../xdi/fddihdr.h"
#include "../xdi/csphdr.h"
#include "../xdi/cspglbl.h"
#include "../xdi/smtmsg.h"
#include "../xdi/fbmframe.h"
#include "../xdi/fbmhdr.h"
#include "../xdi/fbmproto.h"
#include "../xdi/mibglbl.h"
#include "../xdi/mibproto.h"
#include "../if/if_fddi.h"
#include "../util/itemlist.h"

#include "../if/if_fddi_public.h"
#include "c4000_fddi_public.h"
#include "c4000_xdi_public.h"

/***********************************************************************
				Variables
***********************************************************************/
/* --- internal --- */
/* times corresponding to tvx settings, in units of 10s of nanoseconds */
/* 2-188 in natl book */
ulong tvx_table[17] = { 4096, 8192, 16384, 32768, 65536, 131070, 
			 262140, 524290, 1048600, 2097200, 4194300,
			 8388600, 16777000, 33554000, 67109000, 
			 134220000, (ulong)-1 };

static char *linestate[] = { "ALS", "ILS", "no signal", "<future>", 
		      "MLS", "HLS", "QLS", "NLS" ,
		      "unknown, last was ALS",
		      "unknown, last was ILS",
		      "unknown, last was no signal",
		      "unknown, last was <future>",
		      "unknown, last was MLS",
		      "unknown, last was HLS",
		      "unknown, last was QLS",
		      "unknown, last was NLS" };

int fddi_smtqsize;
int      smt_dropped[MAX_STATION_COUNT],    /* # of dropped smt frames */
         smt_passed[MAX_STATION_COUNT];     /* # of processed smt frames */
int      smt_process_id = 0;                /* Process ID for SMT process */
static timer_t  rt_timers[MAX_RTTIMER_COUNT];
volatile ushort num_running_rt_timers = 0;


void fddi_idb_init_les (hwidbtype *idb)
/*********************************************************************
Function:	Initialize IDB with default values for FDDI media.
Parameters:	idb	= interface hardware idb pointer.
Return:		void
*********************************************************************/
{
    fddisb   *fsb;

    /* We have at least one FDDI interface, so we need FDDI debug flags */
    fddi_idb_init(idb);

    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);

    TIMER_STOP(fsb->fddi_operational);
    idb->buffersize = 5000;  /* guess, CJS ALERT */

    /*  
     * Default paramters.
     */
    fsb->fddi_trt = DEFAULT_TRT_TIME;
    fsb->fddi_tvx = DEFAULT_T_TVX_TIME;   
    fddi_smtqsize = DEFAULT_SMTBUFCOUNT;
    idb->fddi_options = FDDI_SMT_FRAMES;

    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
}


void fddi_interface (hwidbtype *idb)
/*********************************************************************
Function:	Print FDDI interface status. This is requested
		    through the "show interface" command.
Parameters:	idb	= interface hardware idb pointer.
Return:		void
*********************************************************************/
{
    uchar crsr;
    char c = 'S';
    xx_fddi_regs_t *f = (xx_fddi_regs_t *)GET_SLOT_BASE(idb->slot);
    uchar una[6],dna[6];  /* bit swapped upstream & downstream nayba */
    char *pcstate, *pcnbr;
    fddisb *fsb;

    fsb = idb_get_hwsb_inline(idb, HWIDB_SB_FDDI);
    

    /* LSM line state mask, LSU line state unknown bit */
    if (f->id_prom[0] & XX_FDDI_DUAL) c = 'A';
    crsr = f->player0.crsr & (PLAYER_CRSR_LS_MSK | PLAYER_CRSR_LSU);
    pcstate = get_pcm_state_string (CSPDATA (idb->fddi_stn_id,
					     portData[0].pcState));
    pcnbr = get_phy_type_string (CSPDATA (idb->fddi_stn_id,
					  portData[0].PC_Neighbor));
    printf("\n  Phy-%c state is %s, neighbor is %s, status %s",
	   c, pcstate, pcnbr, linestate[crsr]);
    
    if ( XX_FDDI_DUAL & f->id_prom[0] ) {
        crsr = f->player1.crsr & (PLAYER_CRSR_LS_MSK | PLAYER_CRSR_LSU);
        pcstate = get_pcm_state_string (CSPDATA (idb->fddi_stn_id,
						 portData[1].pcState));
        pcnbr = get_phy_type_string (CSPDATA (idb->fddi_stn_id,
					      portData[1].PC_Neighbor));
        printf("\n  Phy-B state is %s, neighbor is %s, status %s",
	       pcstate, pcnbr, linestate[crsr]);
    }

    printf("\n  ECM is %s, CFM is %s, RMT is %s",
           get_smt_string(&SMTMsg.ec_state,
			  MIBDATA (idb->fddi_stn_id, mib->smtData.ECMState)),
           get_smt_string(&SMTMsg.cf_state,
			  MIBDATA (idb->fddi_stn_id, mib->smtData.CF_State)),
           get_smt_string(&SMTMsg.rm_state,
			  MIBDATA (idb->fddi_stn_id,
				   mib->macBase[0].hwData.RMTState)));

    printf("\n  Requested token rotation %u usec, "
	   "negotiated %u usec",
	   fsb->fddi_trt, fsb->fddi_trt_neg);
    /* the printfs below will output wrong info if XX_TVXSCALE changes */
    printf("\n  Configured tvx is %u usec, using %u",
	   fsb->fddi_tvx,tvx_table[f->bmac.tvx]/XX_TVXSCALE);
    if ( tvx_table[f->bmac.tvx] % XX_TVXSCALE ) 
      printf(".%02d",tvx_table[f->bmac.tvx] % XX_TVXSCALE);
    printf(" usec, ");

    if (!TIMER_RUNNING(fsb->fddi_operational))
	printf("ring not operational");
    else {
	printf("ring operational ");
	print_dhms(fsb->fddi_operational);
    }

    printf("\n  %d SMT frames processed, %d dropped, %d SMT buffers",
	   smt_passed[idb->fddi_stn_id], smt_dropped[idb->fddi_stn_id],
	   fddi_smtqsize);

    ieee_swap(MIBDATA (idb->fddi_stn_id, mib->macBase[0].hwData.UpstreamNbr),
	      una);
    ieee_swap(MIBDATA (idb->fddi_stn_id, mib->macBase[0].hwData.DownstreamNbr),
	      dna);

    printf("\n  Upstream neighbor %e, downstream neighbor %e", 
	   una,dna);
}


void fddi_config_commands (parseinfo *csb)
/*********************************************************************
Function:	Parse FDDI interface specific commands.
		    Current supported interface specific commands:
			fddi token-rotation-time <microsecond value>
			fddi valid-transmission-time <microsecond value>
			fddi tl-min-time <microsecond value>
			fddi cmt-max-rate <event count in events/sec>
Parameters:	csb	= 
Return:		void
*********************************************************************/
{
    hwidbtype *idb;
    xx_fddi_regs_t *f;
    leveltype level;
    boolean cmt_config_changed = FALSE;
    ulong i,j,oldtrt,oldtvx;
    fddisb *fsb;
    ONE_ITEM_LIST(tst, FDDI_HW_TRT, sizeof(int), 0);
    /* Initialize local variables */
    idb = hwidb_or_null(csb->interface);

    /* Process only if interface exists */
    if (!idb) 
	return;

    /* Check if FDDI interface */
    if (!(idb->status & IDB_FDDI)) {
	if (!csb->nvgen)
	    printf("\n%%Only allowed on Fddi interfaces");
	return;
    }

    f = (xx_fddi_regs_t *)GET_SLOT_BASE(idb->slot);
    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);
    (*idb->state_config)(idb, IDB_CONTROL_READ_CONFIG, &tst_list);
    if ( csb->nvgen ) {
	switch (csb->which) {	
	  case FDDI_INTFC_TRT:
	    nv_write((fsb->fddi_trt != XX_DEFAULT_TRT),
		     "%s %u", csb->nv_command,
		     fsb->fddi_trt);
	    break;
	  case FDDI_INTFC_TVX:
	    nv_write((fsb->fddi_tvx != XX_DEFAULT_TVX),
		     "%s %u", csb->nv_command,
		     fsb->fddi_tvx);
	    break;
	  case FDDI_INTFC_TL_MIN:
	    nv_write((STADATA (idb->fddi_stn_id, TLMin) != TL_MIN),
		     "%s %u", csb->nv_command,
		     STADATA (idb->fddi_stn_id, TLMin));
	    break;
	  case FDDI_INTFC_ENCAP:
	    if (idb->fddi_options & FDDI_ENCAPSULATE)
	      nv_write(TRUE, csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
	return;
    }

    switch (csb->which) {
        case FDDI_INTFC_ENCAP:
	/*
	 * Succeeding in changing the encapsulation mode flushes
	 * this ports bridging entries from the bridge cache if
	 * any.  We also reset after all of this to clear the CAM
	 * and reset the AF to be appropriate for the current
	 * bridging state.
	 */
	level = raise_interrupt_level(NETS_DISABLE);
	if (!csb->sense) {
	    if ((idb->type != IDBTYPE_BSI)) {
		reset_interrupt_level(level);
		return;
	    }
	    idb->fddi_options &= ~FDDI_ENCAPSULATE;
	    idb->span_encapsize = FDDI_FC;             /* Just FC byte */
	    idb->bridge_transit = FALSE;
	    idb->firstsw->tbridge_media = TBR_FDDI;
	} else {
	    idb->fddi_options |= FDDI_ENCAPSULATE;
	    idb->span_encapsize = FDDI_SNAP_ENCAPBYTES;	/* transit */
	    if (idb->tbridge_on_hwidb)
		idb->bridge_transit = TRUE;
	    idb->firstsw->tbridge_media = TBR_FDDI_TRANSIT;
	}
	/*
	 * If in a bridging group, clear the bridge cache of any entries.
	 */
	reg_invoke_bridge_clear_interface(idb->firstsw);
	(*idb->reset)(idb);
	if (idb->counters.resets > 0)
	    idb->counters.resets--;
	reset_interrupt_level(level);
	idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
	return;

    case FDDI_INTFC_TL_MIN:
	if (!csb->sense || !STADATA (idb->fddi_stn_id, TLMin))
	    STADATA (idb->fddi_stn_id, TLMin) = TL_MIN;
	else 
	    STADATA (idb->fddi_stn_id, TLMin) = GETOBJ(int,1);
	idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
	return;
	
     case FDDI_INTFC_TRT:
        oldtrt = fsb->fddi_trt;

	if (csb->sense) {
	    i = GETOBJ(int,1);
	    if ( (i < (DEFAULT_T_MIN_TIME)) || 
		( i > (DEFAULT_T_MAX_TIME))) {
		printf("\nTRT must be between %d us and %d us",
		       DEFAULT_T_MIN_TIME, DEFAULT_T_MAX_TIME);
		idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
		return;
	    }
	    fsb->fddi_trt = i;
        } else {
	    fsb->fddi_trt = XX_DEFAULT_TRT;
	}

	if (oldtrt != fsb->fddi_trt) 
	    cmt_config_changed = TRUE;
	break;

    case FDDI_INTFC_TVX:
	oldtvx  = fsb->fddi_tvx;
        i = GETOBJ(int, 1);

	if (csb->sense) {
	    /*	
	     * Ensure that the TVX is in range for Natl chipset
	     */
	    if ( ( i < XX_MIN_TVX ) ||
		 ( i > tvx_table[15]/XX_TVXSCALE) )  {
		printf("Valid transmission timer (TVX) must be >= %d usecs"
		       " and < %d usecs", 
		       XX_MIN_TVX,tvx_table[15]/XX_TVXSCALE);
		idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
		return;
	    }
	    fsb->fddi_tvx = i;
	} else {
	    fsb->fddi_tvx = XX_DEFAULT_TVX;
	}

	if (oldtvx != fsb->fddi_tvx) cmt_config_changed = TRUE;
	
    for (j = 0; (j < 16) & (tvx_table[i]/XX_TVXSCALE > i); j++);

    /*
    *   If smt is initialized, MIB is also initialized so we can
    *   update it, otherwise don't do nothing with it. MIB values
    *   will be copied later automaticaly, after smt is initialized.
    */
    if (STADATA (idb->fddi_stn_id, smtUp) == TRUE) {

        MIBDATA (idb->fddi_stn_id, mib->macBase[0].hwData.TvxValue) =
            -MUsecs2Fddi(tvx_table[i]/XX_TVXSCALE);
    }

	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
	return;
    }
    if ( system_configured && cmt_config_changed ) {
	fddi_disconnect (idb->fddi_stn_id);
	fddi_connect (idb->fddi_stn_id);
    }
    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
}


void smt_threshold_command (parseinfo *csb)
/*********************************************************************
Function:	Process command to set number of SMT buffers.
Parameters:	csb	= 
Return:		void
*********************************************************************/
{
    int n, i;
    ulong oldqsize;

    if (!nfddinets)
	return;

    /*
     * Get the required command parameter value (the threshold)
     */
    if (csb->nvgen) {
	if (nfddinets && (fddi_smtqsize != DEFAULT_SMTBUFCOUNT)) {
	    nv_write(TRUE, "smt-queue-threshold %u",
		     fddi_smtqsize);
	}
	return;
    }

    if (!csb->sense) {
	n = DEFAULT_SMTBUFCOUNT;
    } else {
        n = GETOBJ(int,1);
    }
    if (n) {
	if (n > MAX_SMTBUFCOUNT)
	    n = MAX_SMTBUFCOUNT;
	oldqsize = fddi_smtqsize;
	fddi_smtqsize = n;
	if (system_configured)
	    for (i=0; i<nfddinets; i++)
		fddi_disconnect (i);
	
	n = get_smt_bufs(fddi_smtqsize);
	if (n) {
	    printf("only %d smt buffers have been allocated\n",n);
	    fddi_smtqsize = n;
	}
	
	if (system_configured)
	    for (i=0; i<MAX_STATION_COUNT; i++)
		if (STADATA(i,hwPresent) &&
		    (HWIDBDATA(i,0,state) != IDBS_ADMINDOWN))
		    fddi_connect (i);
    }
}


/*********************************************************************
	Environment Interface Routines
*********************************************************************/

/* 
 * clear xxfddi specific counters (they aren't in the idb)
 */
void xxfddi_clearcounters (hwidbtype *idb)
{
    bsi_instance_t *ds = idb->devctl;  /* INSTANCE */
    ds->cmt_lsupi[0] = ds->cmt_lsupv[0] = ds->cmt_ebou[0] = 0;
    ds->cmt_lsupi[1] = ds->cmt_lsupv[1] = ds->cmt_ebou[1] = 0;
    smt_dropped[0] = smt_passed[0] = 0;
}

/*********************************************************************
	Interrupt Interface Routines
*********************************************************************/
 

/*
 * start smt for idb
 * returns nonzero if err
 */
static int init_smt (hwidbtype *idb)
{
    /* set up for CMT interrupts */
    nim_register_hi_irq_handler (idb->slot, CSPInterruptHandler,
				 IDBTYPE_BSI, idb->fddi_stn_id);
    
    if (MIBMainMulti(idb->fddi_stn_id)) {
	errmsg(&msgsym(NOMEMORY, FDDI), idb->fddi_stn_id, "MIBMainMulti");
        return(-1);
    }
    if (FBMMainMulti(idb->fddi_stn_id)) {
	errmsg(&msgsym(NOMEMORY, FDDI), idb->fddi_stn_id, "FBMMainMulti");
	return(-1);
    }

    /* Create Beacon frame */
    BMACDATA (idb->fddi_stn_id, 0, bcn_ptr) = getbuffer (128);
/*
 * BOGOSITY ALERT!!!  The below code mungs the pointer without ever
 * having checked that getbuffer returned a valid packet...
 */
    (BMACDATA (idb->fddi_stn_id, 0, bcn_ptr))->refcount = 2;

    /* Indicate that SMT was initialized successfuly */
    STADATA (idb->fddi_stn_id, smtUp) = TRUE;

    return(0);
}

/*
 * start fddi on xx, called *after* nvram is parsed
 */
void fddi_system_configured (hwidbtype *idb)
{
    bsi_instance_t *ds;

    if (!idb)
	return;

    ds = idb->devctl; 

    /* initialize smt.  init bsi & cmt only if not shutdown. */ 
    if (STADATA (idb->fddi_stn_id, smtUp) != TRUE) {
	if (!init_smt(idb) && (idb->state != IDBS_ADMINDOWN)) {
	    bsi_initialize(idb);
	    if (!ds->board_reset && STADATA (idb->fddi_stn_id, smtUp)) {
		init_cmt(idb);
	    } else
		bprintf(" cmt not reset, bsi_init just done ");
	}
    }

    /*
     * If we haven't fired up the SMT input process yet, then
     * do so.
     */
    if (smt_process_id == 0) {
	smt_process_id = cfork(fddi_smt_input, 0, STACK_SIZE_DEFAULT,
			       "SMT input", 0);
    }
}


timer_t *create_rt_timer (void *func, void *arg1, void *arg2, void *arg3,
			  char *name, timer_types type)
/*********************************************************************
Function:	Creates a real time timer and sets callback function
		and arguments provided.

		A real time (RT) timer differs from a normal system
		timer in that it runs at interrupt level, providing
		much more accurate timing services for applications
		that depend on it. The callback function should
		minimize its processing time, postponing less-critical
		tasks to be performed at process level.

		The RT timers are organized in a fixed size indexed
		table (the use of a linked list was avoided to allow the
		RT timer tick interrupt handler to be run at NMI level
		which would cause problems when pointers are to be
		updated). Using a fixed size table is not a problem
		since RT timers are rarely used. For speed reasons,
		a count of active RT timers is maintained which helps
		to avoid searching through inactive timer entries.

		Creating a RT timer just involves finding a free table
		entry and marking it as used.
Parameters:	func:		callback function address.
		arg1:		pointer to 1st arg to callback function.
		arg2:		ptr to 2nd arg to callback function.
		arg3:		ptr to 3rd arg to callback function.
		name:		ptr to RT timer name.
		type:		RT timer type (CONTINUOUS, ONCE).
Return:		pointer to timer structure allocated to timer just
			created, or NULL if creation process failed.
*********************************************************************/
{
    timer_t     *timer;
    ushort	tmr = 0;

    /* Look for a free timer entry */
    while (tmr<MAX_RTTIMER_COUNT)
	if (rt_timers[tmr].tm_state == TMS_UNUSED) break;
	else tmr++;
    if (tmr == MAX_RTTIMER_COUNT) {
        buginf("\nTIMER: max number of real time timers exceeded ");
        return(NULL);
    } else
	timer = &rt_timers[tmr];

    /* Entry found - fill in attributes */
    timer->tm_fun_ptr = func;
    timer->tm_arg1 = arg1;
    timer->tm_arg2 = arg2;
    timer->tm_arg3 = arg3;
    timer->tm_state = TMS_IDLE;
    timer->tm_interval = 0;
    TIMER_STOP(timer->tm_expiration);
    timer->tm_invoked = 0;
    timer->tm_name = name;
    timer->tm_type = type;

    return(timer);
}


void set_rt_timer (timer_t *timer, ulong value)
/*********************************************************************
Function:	Program the timer to expire in a given number of
		miliseconds.
Parameters:	timer:		RT timer entry pointer.
		value:		Timer expiration in miliseconds.
Return:		void
*********************************************************************/
{
    /* Check arguments and timer */
    if (timer == NULL || value == 0 || timer->tm_state == TMS_UNUSED)
        return;

    /*
     * If the timer is currently running and it is a continuous timer,
     * simply update the interval.  This will preserve the interval on
     * the current run.
     */
    if ((timer->tm_type == TMT_CONTINUOUS) &&
        (timer->tm_state == TMS_RUNNING)) {
	/* Running Countinuous timer - Just update the interval */
        timer->tm_interval = value;
    } else {
        /* Start timer with given values */
	timer->tm_interval = value;
	TIMER_START(timer->tm_expiration, value);
	if (timer->tm_state != TMS_RUNNING) {
	    num_running_rt_timers++;
	    timer->tm_state = TMS_RUNNING;
	}
    }
}


void stop_rt_timer (timer_t *timer)
/*********************************************************************
Function:	Stop the given RT timer.
Parameters:	timer_ptr:	RT timer entry pointer.
Return:		void
*********************************************************************/
{
    /* Validate timer pointer */
    if (timer == NULL)
        return;

    /* Make sure timer is running before stopping it */
    if (timer && timer->tm_state == TMS_RUNNING) {
        timer->tm_state = TMS_IDLE;
	num_running_rt_timers--;
    }
}


void rt_timer_int_hand (void)
/*********************************************************************
Function:	RT timer interrupt handler.

		This function is to be called from the platform timer
		tick interrupt handler, which can even be at the NMI
		level. The RT timers implementation scheme was
		designed to NOT have any critical section which would
		need disabling interrupts and therefore prevent this
		interrupt handler from being called at NMI level.

		Note that the platform timer tick interrupt handler
		is responsible for saving the machine context and
		calling this function as a simple C code routine.
Parameters:	None.
Return:		void
*********************************************************************/
{
    timer_t *timer;
    ushort num_run;

    /*
     * Don't touch timer structures if timer structures
     * are being used - if it was time to take an action
     * it will be postponed for 4ms until the next NMI
     * tick happens.
     */
    if (CSPInterruptsDisabled)
	return;

    /*
     * Check all active timers
     */
    for (num_run=0, timer=rt_timers;
	 num_run<num_running_rt_timers && timer<&rt_timers[MAX_RTTIMER_COUNT];
	 timer++) {

	/* Skip timer if not running */
	if (timer->tm_state != TMS_RUNNING)
	    continue;

	/* Running timer - check for time-out */
	num_run++;
        if (AWAKE(timer->tm_expiration)) {
#ifdef DEBUG_TIMERS
            buginf("\nRT_Timer %x expired, exp = %#Ta",
                   timer, timer->tm_expiration);
#endif
            /*
             * For continuous timers, bump up the expiration time by the
             * interval (so we don't slip).  If the expiration time has
             * already passed, effectively skip a number of intervals
             * but continue to avoid slippage.
             *
             * For one-shots, idle the timer.
             */
            if (timer->tm_type == TMT_CONTINUOUS) {
                TIMER_UPDATE(timer->tm_expiration, timer->tm_interval);
#ifdef DEBUG_TIMERS
                buginf("\nRestarted, new exp = %#Ta", timer->tm_expiration);
#endif
            } else {
		num_running_rt_timers--;
                timer->tm_state = TMS_IDLE;
#ifdef DEBUG_TIMERS
                buginf("\nNot restarted");
#endif
            }
            timer->tm_invoked++;

            /* Call the callback routine. */

#ifdef DEBUG_TIMERS
            buginf("\nInvoking %x", timer);
#endif
            (*(timer->tm_fun_ptr))(timer->tm_arg1,
                                   timer->tm_arg2,
                                   timer->tm_arg3);
        }
    }
}


void rt_timer_init (void)
/*********************************************************************
Function:	Initialize the RT timer data structure and enable the
		platform timer tick interrupt.
Parameters:	None.
Return:		void
*********************************************************************/
{
    ushort i;

    /* Initialize real time timer structure */
    for (i=0; i < MAX_RTTIMER_COUNT; i++)
	rt_timers[i].tm_state = TMS_UNUSED;

    /* Start RT timer tick (platform specific) */
    rt_timer_start ();
}


#ifdef DEBUG_TIMERS
static void dump_rt_timers (void)
/*********************************************************************
Function:	Print RT timer status.
Parameters:	None.
Return:		void
*********************************************************************/
{
    ushort	i;

    printf("\n\nRTTmr   S   Expire   Interval  T Invoke Name");
    for (i=0; i<MAX_RTTIMER_COUNT; i++) {
	if (rt_timers[i]->tm_state == TMS_UNUSED)
	    break;
	timer = &rt_timers[i];
        printf("\n%-6d %c %-#10Ta %-10d %c %-6d %s",
               i, (timer->tm_state == TMS_RUNNING) ? 'A':'I',
               timer->tm_expiration, timer->tm_interval,
               (timer->tm_type == TMT_ONCE) ? 'O':'C',
               timer->tm_invoked, timer->tm_name);
    }
}
#endif
