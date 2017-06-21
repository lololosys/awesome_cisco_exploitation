/* $Id: dsx1_chain.c,v 3.5.20.5 1996/09/02 10:31:49 styang Exp $
 * $Source: /release/112/cvs/Xsys/if/dsx1_chain.c,v $
 *--------------------------------------------------------------------
 * DSX1 parse chains (Channelized T1/E1)
 *
 * July 1993, Ronnie Kon (moved from t1_chain.c 10/94, Bill May)
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *--------------------------------------------------------------------
 * $Log: dsx1_chain.c,v $
 * Revision 3.5.20.5  1996/09/02  10:31:49  styang
 * CSCdi61183:  Cannot tell hardware revision of cards from command line
 * Branch: California_branch
 * To show hardware version info for the modem card and T1/E1 card.
 *
 * Revision 3.5.20.4  1996/07/15  20:43:08  towu
 * CSCdi61208:  show cont t1 for cas channel does not display transmit RBS
 * status
 * Branch: California_branch
 *         o Added feature to show rbs xmit status when serial debug turn
 *         on.
 *         o Added new field to collect rbs xmit status in dsx1info
 *         structure.
 *
 * Revision 3.5.20.3  1996/06/16  21:12:52  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.5.20.2  1996/05/21  09:49:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.20.1  1996/03/18  20:13:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/09  05:06:01  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.14.1  1996/02/20  00:56:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/01  06:03:56  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/17  18:28:44  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:26:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:48:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/02  10:53:16  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  21:51:02  wmay
 * CSCdi36002:  CE1 layer 1 fails Australia Homologation - add Australia
 * option to framing controller E1 command.
 *
 * Revision 2.1  1995/06/07  20:51:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *--------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "if_controller.h"
#include "dsx1.h"
#include "parser_defs_t1.h"
#include "parser_defs_e1.h"
#include "dsx1_registry.h"
#include "sys_registry.h"
#include "../os/techsupport.h"

#define ALTERNATE	NONE
#include "cfg_ctrlr_t1.h"
LINK_POINT(t1_interface_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "exec_show_controllers_t1.h"
LINK_POINT(t1_show_ctrllr_cmd, ALTERNATE);
#undef	ALTERNATE

#define ALTERNATE	NONE
#include "cfg_ctrlr_e1.h"
LINK_POINT(e1_interface_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE	NONE
#include "exec_show_controllers_e1.h"
LINK_POINT(e1_show_ctrllr_cmd, ALTERNATE);
#undef ALTERNATE

static void show_controller_dsx1(cdbtype *, boolean);
static void show_t1_techsupport(parseinfo *csb);
static void show_e1_techsupport(parseinfo *csb);

/* called by the driver to add the T1 commands to the parser */
void add_t1_commands (void)
{
    static boolean done_it = FALSE;

    if(done_it)
	return;
    done_it = TRUE;
    
    parser_add_commands(PARSE_ADD_CFG_CONTROLLER_CMD,
			&pname(t1_interface_commands), "T1");
    parser_add_commands(PARSE_ADD_SHOW_CONTROLLER_CMD,
			&pname(t1_show_ctrllr_cmd), "T1");
    reg_add_show_techsupport(show_t1_techsupport, "show_t1_techsupport");
}

/* Also called by the driver to add the E1 commands to the parser */
void add_e1_commands (void)
{
    static boolean done_it = FALSE;

    if (done_it)
	return;
    parser_add_commands(PARSE_ADD_CFG_CONTROLLER_CMD,
			&pname(e1_interface_commands), "E1");
    parser_add_commands(PARSE_ADD_SHOW_CONTROLLER_CMD,
			&pname(e1_show_ctrllr_cmd), "E1");
    reg_add_show_techsupport(show_e1_techsupport, "show_e1_techsupport");
}

/*
 * show_controller_chain
 * Walk the cdb chain, showing all controllers of a specific type, or
 * a specified controller.
 * This is an interactive routine intended for "show" commands.
 * We walk each cdb.
 */
static void show_controller_chain (cdbtype *target,
				   enum pif_types_ cont_type,
				   void (*rtn)(cdbtype *, boolean),
				   boolean Verbose)
{
    boolean onceflag;
    cdbtype *contr;
    
    onceflag = FALSE;
    
    FOR_ALL_CDBS(contr) {
	if (contr->pif_type == cont_type) {
	    if ( target && ( target != contr ))
		continue;
	    if ( onceflag ) {
		if ( target != NULL )
		    return;
		automore_conditional( 0 );
	    } else onceflag = TRUE;
	    (*rtn)(contr, Verbose);
	}
    }
    
} /* end show_controller_chain() */

void show_t1_controller (parseinfo *csb)
{
    cdbtype *cdb;
    boolean verbose;
    boolean unit_based_platform = FALSE;
    uint context1, context2;   

    if ((IS_FAMILY(FAMILY_XX)) || (IS_FAMILY(FAMILY_SIERRA)) || 
                                     (IS_FAMILY(FAMILY_BRASIL)))
	unit_based_platform = TRUE;
    
    if (GETOBJ(int,1) != -1) {
	context1 = GETOBJ(int,1);
	context2 = GETOBJ(int,2);

	if (unit_based_platform)
	    cdb = controller_find(0 /* we dont care for slot*/ , context1);
	else
	    cdb = controller_find(context1, context2);

	if (cdb == NULL) {
	    if (unit_based_platform)
		printf("\n%% No such controller %d", GETOBJ(int,1));
            else
		printf("\n%% No such controller %d/%d", GETOBJ(int,1),
		       GETOBJ(int,2));
	    return;
	}
	if (cdb->pif_type != PIFT_T1) {
	    if (unit_based_platform)
		printf("\n%% Controller at %d is not a T1",
		       GETOBJ(int,1));
	    else
		printf("\n%% Controller at %d/%d is not a T1",
		       GETOBJ(int,1), GETOBJ(int,2));
	    return;
	}
	verbose = TRUE;
    } else {
	cdb = NULL;
	verbose = FALSE;
    }
    automore_enable(NULL);
    show_controller_chain(cdb, PIFT_T1, show_controller_dsx1, verbose);
    automore_disable();
}

void show_e1_controller (parseinfo *csb)
{
    cdbtype *cdb;
    boolean verbose;
    boolean unit_based_platform = FALSE;
    uint context1, context2;   

    if ((IS_FAMILY(FAMILY_XX)) || (IS_FAMILY(FAMILY_SIERRA)))
	unit_based_platform = TRUE;
    
    if (GETOBJ(int,1) != -1) {
	context1 = GETOBJ(int,1);
	context2 = GETOBJ(int,2);

	if (unit_based_platform)
	    cdb = controller_find(0 /* we dont care for slot*/ , context1);
	else
	    cdb = controller_find(context1, context2);

	if (cdb == NULL) {
            if (unit_based_platform)
		printf("\n%% No such controller %d", GETOBJ(int,1));
            else
		printf("\n%% No such controller %d/%d", GETOBJ(int,1),
		       GETOBJ(int,2));
	    return;
	}
	if (cdb->pif_type != PIFT_E1) {
	    if (unit_based_platform)
		printf("\n%% Controller at %d is not an E1",
		       GETOBJ(int,1));
	    else
		printf("\n%% Controller at %d/%d is not an E1",
		       GETOBJ(int,1), GETOBJ(int,2));

	    return;
	}
	verbose = TRUE;
    } else {
	cdb = NULL;
	verbose = FALSE;
    }
    automore_enable(NULL);
    show_controller_chain(cdb, PIFT_E1, show_controller_dsx1, verbose);
    automore_disable();
}

/*
 * show_MIB_table will show the raw data for a dsx1 table
 */
static void show_dsx1_MIB_table (dsx1TableType *tbl)
{
    printf("\n     %d Line Code Violations, %d Path Code Violations",
	   tbl->dsx1LCVs, tbl->dsx1PCVs );
    printf("\n     %d Slip Secs, %d Fr Loss Secs, "
	   "%d Line Err Secs, %d Degraded Mins",
	   tbl->dsx1CSSs, tbl->dsx1SEFSs, tbl->dsx1LESs, tbl->dsx1DMs );
    printf("\n     %d Errored Secs, %d Bursty Err Secs, "
	   "%d Severely Err Secs, %d Unavail Secs",
	   tbl->dsx1ESs, tbl->dsx1BESs, tbl->dsx1SESs, tbl->dsx1UASs );
}

const char *get_dsx1_framing (enum dsx1_framing_type type)
{
    switch (type) {
    case DSX1_FRM_ESF:		return("ESF");
    case DSX1_FRM_SF:		return("SF");
    case DSX1_FRM_E1:		return("NO-CRC4");
    case DSX1_FRM_E1_CRC:	return("CRC4");
    default:			return("Unknown");
    }
}

const char *get_dsx1_homologation_type (enum dsx1_homologation_type type)
{
    switch (type) {
    default: return("");
    case DSX1_HOMOL_AUSTRALIA: return("Australia");
    }
}

static const char *get_dsx1_line_code (enum dsx1_linecode_type type)
{
    switch (type) {
    case DSX1_LC_AMI:	return("AMI");
    case DSX1_LC_B8ZS:	return("B8ZS");
    case DSX1_LC_HDB3:	return("HDB3");
    default:		return("Unknown");
    }
}

/*
 * show_controller_dsx1
 * This routine will display all sorts of data about the E1 and T1's that
 */
static void show_controller_dsx1 (cdbtype *cdb, boolean verbose)
{
    dsx1info *dsx1;
    int line_status;
    int loop_status;

    dsx1 = dsx1_find_by_unit(cdb->unit);
    printf("\n%s is %s.", cdb->cdb_namestring,
	   print_cdbstate(cdb->cdb_state));

    if (cdb->cdb_state == IDBS_ADMINDOWN)
	return;
    
    line_status = dsx1->MIB.C.dsx1LineStatus;
    loop_status = dsx1->MIB.C.dsx1LoopbackConfig;

    switch (dsx1->loop) {
    case DSX1_LOOP_LOCAL:
	printf(" (Locally Looped)");
	break;
    case DSX1_LOOP_REMOTE:
	printf(" (Remotely Looped)");
	break;
    default:
	break;
    }

    /* Indicate if we are being looped by remote station */
    switch (loop_status) {
    case DSX1_PAYLOAD_LOOP:
        printf(" (Payload Loop by Remote)");
        break;
    case DSX1_LINE_LOOP:
        printf(" (Line Loop by Remote)");
        break;
    default:
	break;
    }

    if (dsx1->pif_type == PIFT_E1 ) {
	printf("\n  Applique type is %s",
	       dsx1_applique_name(cdb->unit));
    }
    if (cdb->cdb_description != NULL)
	printf("\n  Description: %s", cdb->cdb_description);

    if ( line_status == DSX1_NO_ALARM ) {
	printf("\n  No alarms detected.");
    } else {
	if ( line_status & DSX1_ALL_TX ) {
	    if ( line_status & DSX1_TX_REM_ALARM )
		printf("\n  Transmitter is sending remote alarm.");
	    else if ( line_status & DSX1_TX_AIS )
		printf("\n  Transmitter is sending AIS.");
	    else printf("\n  Transmitter ERROR %d", line_status);
	}
	if ( line_status & DSX1_RX_LOS )
	    printf("\n  Receiver has loss of signal.");
	else if ( line_status & DSX1_RX_AIS )
	    printf("\n  Receiver is getting AIS.");
	else if ( line_status & DSX1_RX_LOF )
	    printf("\n  Receiver has loss of frame.");
	else if ( line_status & DSX1_RX_REM_ALARM )
	    printf("\n  Receiver has remote alarm.");
	else if (( line_status & DSX1_ALL_RX ) == 0 )
	    printf("\n  Receiver has no alarms.");
	else
	    printf("\n  Receiver has ? - value %d\n", line_status );
    }
    reg_invoke_show_nim_hw_ver(cdb);

    printf("\n  Framing is %s",
	   get_dsx1_framing(dsx1->framing));
    if (dsx1->homologation_type != DSX1_HOMOL_NORMAL) {
	printf("(%s)", get_dsx1_homologation_type(dsx1->homologation_type));
    }
    printf(", Line Code is %s", get_dsx1_line_code(dsx1->linecode));

    if (dsx1->clocksource == 0x0) {
		printf(", Clock Source is ");
		if (dsx1->clocking == DSX1_CLK_INTERNAL)
    		printf("Internal.");
    	else if (dsx1->clocking == DSX1_CLK_FREE)
    		printf("Free Running.");
    	else
    		printf("Line.");
    } else {
    	printf(", Clock Source is");
    	printf(" %s", "Line");
    	printf(" %s.", 
    	dsx1->clocksource == DSX1_PRIMARY ? "Primary" : "Secondary");
    }

    printf("\n  Data in current interval (%d seconds elapsed):",
	   dsx1->MIB.C.dsx1TimeElapsed );

    show_dsx1_MIB_table(&dsx1->MIB.CurrentData);

    if (verbose && dsx1->MIB.C.dsx1ValidIntervals > 0) {
	int ix, startix;

	/*
	 * Display all the verbose data
	 * Start by finding the first table in the list
	 */
	for (startix = 0;
	     startix < QUARTER_HOURS_IN_DAY &&
	     dsx1->MIB.Intervals[startix].dsx1Interval != 1;
	     startix++);

	for (ix = 0; ix < dsx1->MIB.C.dsx1ValidIntervals; ix++ ) {
	    printf("\n  Data in Interval %d:", ix + 1);
	    show_dsx1_MIB_table( &dsx1->MIB.Intervals[startix]);
	    if ( startix == 0 )
		startix = QUARTER_HOURS_IN_DAY - 1;
	    else
		startix--;
	}
    }
    if ( dsx1->MIB.C.dsx1ValidIntervals >= QUARTER_HOURS_IN_DAY )
	printf("\n  Total Data (last 24 hours)");
    else {
	if ( dsx1->MIB.C.dsx1ValidIntervals != 0 )
		printf("\n  Total Data (last %d 15 minute intervals):",
	        	dsx1->MIB.C.dsx1ValidIntervals );
    }
    
    if ( dsx1->MIB.C.dsx1ValidIntervals != 0 ) {
	    printf("\n     %d Line Code Violations, %d Path Code Violations,",
		   dsx1->MIB.TotalData.dsx1LCVs,
		   dsx1->MIB.TotalData.dsx1PCVs );
	    printf("\n     %d Slip Secs, %d Fr Loss Secs, "
		   "%d Line Err Secs, %d Degraded Mins,",
		   dsx1->MIB.TotalData.dsx1CSSs,
		   dsx1->MIB.TotalData.dsx1SEFSs,
		   dsx1->MIB.TotalData.dsx1LESs,
		   dsx1->MIB.TotalData.dsx1DMs );
	    printf("\n     %d Errored Secs, %d Bursty Err Secs, "
		   "%d Severely Err Secs, %d Unavail Secs",
		   dsx1->MIB.TotalData.dsx1ESs,
		   dsx1->MIB.TotalData.dsx1BESs,
		   dsx1->MIB.TotalData.dsx1SESs,
		   dsx1->MIB.TotalData.dsx1UASs );
    }
    if (serial_debug && (dsx1_cas_supp == TRUE)) {
        /* show A B signal bits */
        boolean first_time = TRUE;
        uchar *rx_sig, *tx_sig, rx_signals, tx_signals;
        uchar r_bita, r_bitb, r_bitc, r_bitd;
        uchar t_bita, t_bitb, t_bitc, t_bitd;
        uint  ix, index;
 
        rx_sig = dsx1->rx_rbs;
        tx_sig = dsx1->tx_rbs;
        if (dsx1->framing == DSX1_FRM_ESF) {
                for ( ix = 1; ix <= dsx1->max_timeslots; ix++ ) {
                    if ( is_t1_cas(dsx1, dsx1->timeslots[ix])) {
                        rx_signals = rx_sig[(ix-1)/2];
                        tx_signals = tx_sig[(ix-1)/2];
                        if (ix & 1) {
                           rx_signals >>= 4;
                           tx_signals >>= 4;
                        }
                        r_bita = (rx_signals & DSX1_RBS_ESF_BITA) >> 3;
                        r_bitb = (rx_signals & DSX1_RBS_ESF_BITB) >> 2;
                        r_bitc = (rx_signals & DSX1_RBS_ESF_BITC) >> 1;
                        r_bitd = (rx_signals & DSX1_RBS_ESF_BITD);
 
                        t_bita = (tx_signals & DSX1_RBS_ESF_BITA) >> 3;
                        t_bitb = (tx_signals & DSX1_RBS_ESF_BITB) >> 2;
                        t_bitc = (tx_signals & DSX1_RBS_ESF_BITC) >> 1;
                        t_bitd = (tx_signals & DSX1_RBS_ESF_BITD);
 
                        if ( first_time == TRUE) {
                          /* print header */
                                printf("\n Robbed bit signals state:\n");
                                printf("\ttimeslots\trxA rxB rxC rxD \t"
                                        "txA txB txC txD\n\n");
                                first_time = FALSE;
                        }
                        printf("\t%9d\t%3d %3d %3d %3d  \t%3d %3d %3d %3d\n",
                                ix, r_bita, r_bitb, r_bitc,r_bitd, t_bita,
                                t_bitb, t_bitc, t_bitd);
                    }
 
                }
        }
        else if (dsx1->framing == DSX1_FRM_SF) {        /* SF mode */
                for ( ix = 1; ix <= dsx1->max_timeslots; ix++ ) {
                    if ( is_t1_cas(dsx1, dsx1->timeslots[ix])) {
                        rx_signals = rx_sig[(ix-1)/4];
                        tx_signals = tx_sig[(ix-1)/4];
                        index = ((ix-1) & 3);
                        if (index == 0) {
                           rx_signals >>= 6;
                           tx_signals >>= 6;
                        }
                        else if (index == 1) {
                           rx_signals >>= 4;
                           tx_signals >>= 4;
                        }
                        else if (index == 2) {
                           rx_signals >>= 2;
                           tx_signals >>= 2;
                        }
                        r_bita = (rx_signals & DSX1_RBS_SF_BITA) >> 1;
                        r_bitb = (rx_signals & DSX1_RBS_SF_BITB);
                        t_bita = (tx_signals & DSX1_RBS_SF_BITA) >> 1;
                        t_bitb = (tx_signals & DSX1_RBS_SF_BITB);
                        if ( first_time == TRUE) {
                          /* print header */
                            printf("\t Robbed bit signals state:\n");
                            printf("\ttimeslots\t\trxA\trxB\t\ttxA\ttxB\n\n");
                            first_time = FALSE;
                        }
                        printf("\t%9d\t\t%1d\t%1d\t\t%1d\t%1d\n", ix, r_bita,
                                r_bitb, t_bita, t_bitb);
                    }
                }
        }
    }
}

static void show_t1_techsupport (parseinfo *csb)
{
    show_techsupport_command(csb, "show controllers t1");
}

static void show_e1_techsupport (parseinfo *csb)
{
    show_techsupport_command(csb, "show controllers e1");
}
