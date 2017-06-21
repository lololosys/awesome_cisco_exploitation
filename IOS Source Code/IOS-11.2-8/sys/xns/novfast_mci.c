/* $Id: novfast_mci.c,v 3.13.4.9 1996/08/17 02:19:36 hou Exp $
 * $Source: /release/112/cvs/Xsys/xns/novfast_mci.c,v $
 *------------------------------------------------------------------
 * novfast_mci.c -- XNS Fast Switching module providing MCI/CBUS specific
 *                  Support.
 *
 * William H. Palmer, October 1993
 * 29-April-1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novfast_mci.c,v $
 * Revision 3.13.4.9  1996/08/17  02:19:36  hou
 * CSCdi65648:  7k send out erroneous isl vlan packets
 * Branch: California_branch
 * - add ISL vLAN code to handle IPX SAP input packet in fastswitching path.
 *
 * Revision 3.13.4.8  1996/08/14  14:12:18  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * add multilink half-fastswitching support
 *
 * Revision 3.13.4.7  1996/08/14  02:37:44  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.13.4.6  1996/08/13  02:27:46  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.13.4.5  1996/06/23  15:05:54  boyle
 * CSCdi61041:  Input acl not logged with the action permit
 * Branch: California_branch
 *
 * Revision 3.13.4.4  1996/05/17  12:18:12  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.4  1996/05/06  00:12:40  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.2.3  1996/05/02  22:08:53  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.2  1996/04/03  22:21:51  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.2.1  1996/03/17  18:56:46  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.13.4.3  1996/05/03  02:18:50  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.13.4.2  1996/04/27  06:47:53  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.6.2.3  1996/04/26  00:11:53  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.2.2  1996/04/17  00:09:35  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.2.1  1996/02/23  21:33:39  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.13.4.1  1996/03/18  22:53:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/16  08:00:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.2  1996/03/13  02:14:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.1  1996/02/20  22:00:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/03/09  18:13:37  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.12  1996/02/27  19:28:34  achopra
 * CSCdi45189:  "show atm traffic" shows different (wrong) packet counts
 *
 * Revision 3.11  1996/02/25  03:53:30  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.10  1996/02/24  00:23:22  rbadri
 * CSCdi48703:  IPX SAP encaps fails to fastswitch on LANE
 * Use nov_hes_oa_fastswitch() to fastswitch SAP encapsulated packets.
 *
 * Revision 3.9  1996/01/28  00:47:49  bbenson
 * CSCdi47385:  ipx fails to fast-switch over aip (7000)
 * Fixes a bug introduced by 45952, where fastswitching was broken for
 * SNAP, and MUX 1483 VCs.
 *
 * Revision 3.8  1996/01/23  08:57:34  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.7  1996/01/20  06:53:39  lbustini
 * CSCdi39980:  BOD not working on PRI when fastswitching is enabled
 * Use correct MIP vc number when selecting output interface.
 *
 * Revision 3.6  1996/01/09  23:43:24  bbenson
 * CSCdi45952:  IPX w/ SAP encaps on LANE doesnt fastswitch to other
 * interfaces
 * Register aip_nov_fastswitch, and clean up the FAST_PATH and
 * FAST_PATH_OA macros
 *
 * Revision 3.5  1996/01/01  01:54:57  john
 * CSCdi44647:  ipx ping fails with fast-switching enabled on the aip
 *
 * Revision 3.4  1995/12/01  06:26:07  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.3  1995/11/20  17:22:27  rbadri
 * CSCdi42852:  IPX fastswitching over ATM fails in setup with vlan switch
 * For LANE, choose the offset based on encaps. Write the SAP encapsulation
 * with correct values.
 *
 * Revision 3.2  1995/11/17  19:23:44  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:39:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/08/01  01:59:51  bbenson
 * CSCdi37991:  AIP fastpath for IPX can be improved.
 * - collapse aip/lane specific into just aip cases.
 *
 * Revision 2.7  1995/07/31  21:03:45  john
 * CSCdi36543:  Rconsole is time-out with 2 netware clients (SPX-Spoof)
 *
 * Revision 2.6  1995/07/27  06:23:15  bbenson
 * CSCdi37378:  IPX on multiple subinterfaces, some configs, wont
 * fastswitch
 * - Remove the xxx_ok boolean checks from LANE fastswitch section of
 *   aip_nov_fastswitch.  They're useful for _real_ ethernet interfaces to
 *   prevent dup'ed encaps on the same hwidb
 *
 * Revision 2.5  1995/07/10  05:33:12  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.4  1995/07/07  04:30:35  bbenson
 * CSCdi35019:  Fast/SSE switching of LANE IPX SAP encaps not working.
 * - Split out writing of lane encaps from nov_write_hes_aip_header.
 * - Allow same enctype on subinterfaces of non-broadcast media (AIP).
 * - Set uninitialized pak->encsize in novell_et_novell_ether.
 *
 * Revision 2.3  1995/06/29  21:09:41  mschaef
 * CSCdi36392:  Novell Fast Switching fails on ppp
 *
 * Revision 2.2  1995/06/28  09:33:52  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:29:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines Exported from this module
 * ????_nov_fastswitch:	Fast switches novell packets( called from if_mci.c)
 *
 * Both the above routines are called through a switch table "fast_switch"
 * using the RXTYPE code as the index. ( The RXTYPE contains packet type
 * code when RX_SETUP is issued to MCI )
 */

#include "master.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "access.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "config.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"
#include "../wan/atm_dxi.h"
#include "../wan/serial.h"
#include "../wan/dialer.h"
#include "../wan/dialer_private.h"
#include "../hes/if_mci.h"
#include "../if/fddi.h"
#include "../hes/if_fci.h"
#include "../if/network.h"
#include "../if/atm.h"

#include "xns.h"
#include "novell.h"
#include "novfast.h"
#include "novfast_hes.h"
#include "../lane/lane.h"
#include "../lex/lex_fs_mci.h"


/*
 * local variables needed for smds.
 * sm_pad_offset is the MCI offset for the long before the pad
 * sm_pad is the number of pad bytes
 * sm_lastbuffer will contain the last long.  In the odd aligned
 * case, it will contain the whole trailer from the byte before
 * the last_long, the pad bytes, and the trailer
 */

static ushort sm_pad_offset, sm_pad;
static uchar sm_lastbuffer[12];


/*
 * Layout of datagram sniff buffer
 */

/*
 *	     ARPA	   ISO1		  ISO2		  HDLC
 *
 *	  D  D  D  D	D  D  D  D	D  D  D  D
 *   	  D  D  S  S	D  D  S  S	D  D  S  S
 *	  S  S  S  S	S  S  S  S	S  S  S  S
 *   0	  P  P  x  x	L  L  DS SS	L  L  DS SS	B  B  P  P
 *   1	  x  x  x  x	CT x  x  x	CT SN SN SN	x  x  x  x
 *   2	  x  x  x  x	x  x  x  x	SN SN P  P	x  x  x  x
 *   3	  x  x  x  x	x  x  x  x	x  x  x  x	x  x  x  x
 *   4    x  x  x  x	x  x  x  x	x  x  x  x	x  x  x  x
 *   5    x  x 		x  x  x  x 	x  x  x  x	x  x  x  x
 *   6    		x		x  x  x  x
 *   7    				x  x  x  x
 */


/* 
 * Definitions.
 */
#define XNS_OFFSETB_DNET 	6	/* 6 bytes */


/*
 * Minimal debug support
 */
#undef DEBUG
#ifdef DEBUG
static void display_media (hwidbtype *idb)
{
    if (!novell_debug) return;

    if (idb->status & IDB_ETHER)
	buginf("e");
    else if (idb->status & IDB_FDDI) {
	buginf("f");
	if (idb->type == IDBTYPE_FDDIT)
	    buginf("(t)");
    }
    else if (idb->status & IDB_TR)
	buginf("t");
    else if (idb->status & IDB_LEX)
	buginf("l");
    else if (idb->status & IDB_SERIAL) {
	if (idb->status == IDB_HSSI)
	    buginf("h");
	else
	    buginf("s");
    }
    else
	buginf("?");
}
static void display_enctype (int enctype)
{
    if (!novell_debug) return;
    buginf("%s", encstring(enctype));
}
static void display_rxtype (hwidbtype *idb)
{
    if (!novell_debug) return;

    switch (idb->rxtype) {
      case    RXTYPE_NOVELL1: buginf("nov1"); break;
      case    RXTYPE_NOVELL2: buginf("nov2"); break;
      case  RXTYPE_SNAP_NOV1: buginf("snap"); break;
      case RXTYPE_SAP_NOVELL: buginf(" sap"); break;
                     default: buginf("?%-02x?", idb->rxtype); break;
    }
}
static void
display_trace (hwidbtype *input,
	       hwidbtype *output,
	       novpathtype *path,
	       char *string)
{
    if (!novell_debug) return;

    buginf("%s", string);

    buginf(" (");
    if (input) {
	display_media(input);
    }
    if (output) {
	buginf("->");
	display_media(output);
    }
    buginf(") (");
    if (input) {
	display_rxtype(input);
    }
    if (path->novp_nidb_ptr) {
	buginf("->");
	display_enctype(path->novp_nidb_ptr->enctype);
	if (path->enctype != path->novp_nidb_ptr->enctype) {
	    buginf("*** bad cache ***");
	}
    } else {
	buginf("*** NULL nidb ***");
    }
    buginf(")");
}
#endif

/*
 * ipx_fast_supported
 *
 * Determine whether fastswitch support present for given hw/config
 *
 */
static boolean ipx_fast_supported (int linktype, hwidbtype *hwidb)
{

    if (!hwidb || (linktype != LINK_NOVELL))
	return(FALSE);

    /*
     * Old MCI with "tx1bug" is never supported:
     */
    if (hwidb->status & IDB_MCI)
	if (!mci_supports_fastswitching(hwidb))
	    return(FALSE);

    /*
     * MCI/CBUS:
     */
    if (hwidb->status & (IDB_MCI | IDB_CBUS)) {

	/*
	 * Ethernet: all flavors
	 */
	if (hwidb->status & IDB_ETHER)
	    return(TRUE);
	/*
	 * Fddi: all flavors
	 */
	if (hwidb->status & IDB_FDDI)
	    return(TRUE);
	/*
	 * Tokenring: CTR only
	 */
	if ((hwidb->status & IDB_TR) && (hwidb->type == IDBTYPE_CTR))
	    return(TRUE);
	/*
	 * Serial: all flavors if encap is HDLC/PPP.  FR only if DDR is not
	 *         configured
	 */
	if (hwidb->status & IDB_SERIAL)
	    if ((hwidb->enctype == ET_HDLC) ||
		 (is_ppp(hwidb) && !is_multilink(hwidb)) ||
		 (is_frame_relay(hwidb) && !is_ddr(hwidb)) ||
		 is_atm_dxi(hwidb) || is_smds(hwidb))
		return(TRUE);
	/*
	 * ATM: all flavors
	 */
	if (is_atm(hwidb)) {
	    return(TRUE);
	}
	/*
	 * Lex: dependant on the serial line bound to.
	 */
	if (hwidb->status & IDB_LEX)
	    return(TRUE);
    }
    /*
     * Dialer interfaces:
     */
    if (hwidb->status & IDB_DIALER)
	if ((hwidb->enctype == ET_HDLC) ||
	    (is_ppp(hwidb) && !is_multilink(hwidb)))
	    return(TRUE);

    /*
     *  That's all folks!
     */
    return(FALSE);
}

/* nov_mci2mci
 * Fast switch NOVELL packets between two different MCI controllers.
 * Called from nov_hes_fastswitch().
 */

static boolean nov_mci2mci (hwidbtype *input,
			    novpathtype *path,
			    long len_hops,
			    ushort csum)
{	
    hwidbtype *output;
    mciregtype *inreg, *outreg;
    ushort temp, bytes;
    ushort ipxlen;
    int enctype;

    /*
     * Set up some common variables
     */
    output = path->novp_idb_ptr->hwptr;
    outreg = output->mci_regptr;
    
#ifdef DEBUG
    display_trace(input, output, path, " mci ");
#endif

    /* 
     * Select destination MCI interface 
     */	
    outreg->argreg = output->mci_index;
    outreg->cmdreg = MCI_CMD_SELECT;
    
    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    outreg->argreg = output->buffer_pool;
    outreg->cmdreg = MCI_CMD_TX1_RESERVE;
    inreg = input->mci_regptr;
    input->counters.inputs_fast[ACCT_PROTO_NOVELL]++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_NOVELL] += input->rxlength;
    if ((temp = outreg->cmdreg) == MCI_RSP_OKAY) {
	
	/*
	 * Set up write pointer to base of destination transmit window.
	 * Do some input accounting.  Figure out how to write new MAC header.
	 * Then adjust the read pointer of the receiver window to point after
	 * old MAC header.  Compute number of bytes to be copied (in temp) and
	 * to be sent (in bytes).
	 */
	outreg->argreg = 0;
	outreg->cmdreg = MCI_CMD_TX1_SELECT;
	novell_stat[NOVELL_INPUT]++;
	novell_stat[NOVELL_FORWARDED]++;
	bytes = (len_hops >> 16);
	temp = (bytes - XNS_OFFSETB_DNET);
	len_hops = update_msb_tc(len_hops, path);
	if (input->status & IDB_ETHER) {
	    /* 
	     * Input ethernet
	     */
	    if (input->rxtype == RXTYPE_SNAP_NOV1) {
		inreg->argreg = ISO2_IPX_OFFSET + XNS_DNET_OFFSET;
	    } else {		/* ET_NOVELL_ETHER or ET_ARPA */
		inreg->argreg = ARPA_IPX_OFFSET + XNS_DNET_OFFSET;
	    }
	} else if (input->status & IDB_FDDI) {
	    /*
	     * Input fddi
	     */
	    if (input->rxtype == RXTYPE_SNAP_NOV1) {
		inreg->argreg = (FDDI_ISO2_IPX_OFFSET + 
				 input->rxrifwordlen +
				 XNS_DNET_OFFSET);
	    } else { /* RXTYPE_NOVELL2 (FDDI RAW) */
		inreg->argreg = (FDDI_RAW_IPX_OFFSET + 
				 input->rxrifwordlen +
				 XNS_DNET_OFFSET);
	    }
	} else if (input->status & IDB_TR) {
	    /*
	     * Input token ring
	     */
	    inreg->argreg = (TR_ISO2_IPX_OFFSET +
			     input->rxrifwordlen +
			     XNS_DNET_OFFSET);
	} else if (input->status & IDB_SERIAL) {
	    /*
	     * Input serial
	     */
	    if (is_atm_dxi(input)) {
		inreg->argreg = input->rxoffset +
		    XNS_DNET_OFFSET + 
			btow(input->atm_dxi_stuff->fast_input_encsize);
	    } else if (input->rxtype == RXTYPE_NOVELL1)
		inreg->argreg = HDLC_IPX_OFFSET + XNS_DNET_OFFSET;
	    else
		inreg->argreg = FR_IETF_IPX_OFFSET + XNS_DNET_OFFSET;
	} else if (input->status & IDB_LEX) {
	    /* 
	     * Input Lex -- output fddi, adjust receive offset,
	     * number of bytes to copy (temp), and size (bytes)
	     * of output datagram.
	     */
	    if (input->rxtype == RXTYPE_SNAP_NOV1) {
		inreg->argreg = LEX_ISO2_OFFSET + XNS_DNET_OFFSET;
	    } else {		/* ET_NOVELL_ETHER or ET_ARPA */
		inreg->argreg = LEX_ARPA_OFFSET + XNS_DNET_OFFSET;
	    }
	}

	inreg->cmdreg = MCI_CMD_RX_SELECT;

	if (output->status & IDB_ETHER) {
	    /*
	     * Output is Ethernet.
	     */
	    enctype = path->enctype;
	    ipxlen = bytes;
	    if (enctype == ET_SAP) {
		bytes += ETHER_SAP_ENCAPBYTES;
	    } else if (enctype == ET_SNAP) {
		bytes += ETHER_SNAP_ENCAPBYTES;
	    } else {
		bytes += ETHER_ARPA_ENCAPBYTES;
	    }
	    bytes = nov_write_hes_ether_header(enctype, bytes, outreg,
					       path, ipxlen, csum,
					       len_hops);
	} else if (output->status & IDB_FDDI) {
	    /*
	     * Output is fddi.
	     */
	    bytes = nov_write_hes_fddi_header(path->enctype, outreg,
					      path, bytes, csum,
					      len_hops);
	} else if (output->status & IDB_TR) {
	    /*
	     * Output is token ring.
	     */
	    bytes = nov_write_hes_token_header(path->enctype, outreg,
					       path, bytes, csum,
					       len_hops);
	} else if (output->status & IDB_SERIAL) {
	    /*
	     * output is serial
	     */
	    bytes = nov_write_hes_serial_header(output, outreg, path,
						bytes, csum, len_hops);
	} else if (output->status & IDB_LEX) {
	    /*
	     * Output is Lex.
	     */
	    bytes = nov_write_hes_lex_header(path->enctype, outreg,
					     path, bytes, csum,
					     len_hops);
	}
	
	/*
	 * Copy bulk of datagram from one MCI to another.
	 * Transmit modified copy and flush original in receive buffer.
	 */
	mci2mci(&inreg->readlong, &outreg->write1long, temp);
	inreg->cmdreg = MCI_CMD_RX_FLUSH;
	outreg->argreg = bytes;
	outreg->cmdreg = MCI_CMD_TX1_START;
	output->counters.outputs_fast[ACCT_PROTO_NOVELL]++;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_NOVELL] +=
	    (bytes & ~MCI_TX_ODDALIGN);
	return(TRUE);
    } else if (output->priority_list) { 
        /*
         * if output is congested and we are sorting, defer to process level
         */
         return(FALSE);
    } else {
	
	/*
	 * Reserve failed on output -- flush the packet.
	 */	
	output->counters.output_total_drops++;
	inreg->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }
}

/* nov_mci2mci1
 * Fast switch NOVELL packets between two different MCI controllers.
 * Called from nov_hes_oa_fastswitch().
 */

static boolean nov_mci2mci1 (hwidbtype *input,
			     novpathtype *path,
			     long len_hops,
			     ushort csum)
{	
    idbtype *swoutput;
    hwidbtype *output;
    mciregtype *inreg, *outreg;
    ushort temp, bytes;
    ushort ipxlen;
    int enctype;

    /*
     * Set up some common variables
     */
    swoutput = path->novp_idb_ptr;
    output = swoutput->hwptr;

    outreg = output->mci_regptr;
    
#ifdef DEBUG
    display_trace(input, output, path, " mci1");
#endif

    /* 
     * Select destination MCI interface 
     */	
    outreg->argreg = output->mci_index;
    outreg->cmdreg = MCI_CMD_SELECT;

    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    outreg->argreg = output->buffer_pool;
    outreg->cmdreg = MCI_CMD_TX1_RESERVE;
    inreg = input->mci_regptr;
    input->counters.inputs_fast[ACCT_PROTO_NOVELL]++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_NOVELL] += input->rxlength;
    if ((temp = outreg->cmdreg) == MCI_RSP_OKAY) {

	/*
	 * Set up write pointer to base of destination transmit window.
	 * Do some input accounting.  Figure out how to write new MAC header.
	 * Then adjust the read pointer of the receiver window to point after
	 * old MAC header.  Compute number of bytes to be copied (in temp) and
	 * to be sent (in bytes).
	 */
	outreg->argreg = 0;
	outreg->cmdreg = MCI_CMD_TX1_SELECT;
	novell_stat[NOVELL_INPUT]++;
	novell_stat[NOVELL_FORWARDED]++;
	bytes = (len_hops >> 8);
	temp = (bytes - XNS_OFFSETB_DNET);
	len_hops = update_lsb_tc(len_hops, path);


	if (input->status & IDB_ETHER) {
	    /*
	     * input ethernet
	     */
	    inreg->argreg = ISO1_IPX_OFFSET + XNS_DNET_OFFSET;
	} else if (input->status & IDB_FDDI) {
	    /*
	     * input fddi
	     */
	    inreg->argreg = (FDDI_ISO1_IPX_OFFSET + XNS_DNET_OFFSET +
			     input->rxrifwordlen);
	} else if (input->status & IDB_TR) {		
	    /*
	     *  -- input token ring
	     */
	    inreg->argreg = (TR_ISO1_IPX_OFFSET + XNS_DNET_OFFSET +
			     input->rxrifwordlen);
	} else if (input->status & IDB_SERIAL) {
	    /*
	     *  -- input serial
	     */
#ifdef DEBUG
	    if (novell_debug) buginf("*** NO OA serial");
#endif
	    output->counters.output_total_drops++;
	    inreg->cmdreg = MCI_CMD_RX_FLUSH;
	    return(TRUE);
	} else if (input->status & IDB_LEX) {
	    /*
	     *  -- input Lex
	     */
	    inreg->argreg = LEX_ISO1_OFFSET + XNS_DNET_OFFSET;
	}

	inreg->cmdreg = MCI_CMD_RX_SELECT;


	if (output->status & IDB_ETHER) {
	    /*
	     * Output is Ethernet.
	     */
	    enctype = path->enctype;
	    ipxlen = bytes;
	    if (enctype == ET_SAP) {
		bytes += ETHER_SAP_ENCAPBYTES;
	    } else if (enctype == ET_SNAP) {
		bytes += ETHER_SNAP_ENCAPBYTES;
	    } else {
		bytes += ETHER_ARPA_ENCAPBYTES;
	    }
	    bytes = nov_write_hes_ether_oa_header(enctype, bytes,
						  outreg, path,
						  ipxlen, csum,
						  len_hops);
	} else if (output->status & IDB_FDDI) {
	    /*
	     * Output is fddi.
	     */
	    bytes = nov_write_hes_fddi_oa_header(path->enctype,
						 outreg, path,
						 bytes, csum, len_hops);
	} else if (output->status & IDB_TR) {
	    /*
	     * Output is token ring.
	     */
	    bytes = nov_write_hes_token_oa_header(path->enctype,
						  outreg, path,
						  bytes, csum,len_hops);
	} else if (output->status & IDB_SERIAL) {
 	    /*
	     * Output is HDLC.
	     */
	    bytes = nov_write_hes_serial_oa_header(output, outreg, path,
						   bytes, csum,
						   len_hops);
	    bytes |= MCI_TX_ODDALIGN;
	} else if (output->status & IDB_LEX) {
	    /*
	     * Output is Lex.
	     */
	    bytes = nov_write_hes_lex_oa_header(path->enctype, outreg,
						path, bytes, csum,
						len_hops);
	}
	
	/*
	 * Copy bulk of datagram from one MCI to another.
	 * Transmit modified copy and flush original in receive buffer.
	 */
	mci2mci(&inreg->readlong, &outreg->write1long, temp + 1);
	inreg->cmdreg = MCI_CMD_RX_FLUSH;
	outreg->argreg = bytes;
	outreg->cmdreg = MCI_CMD_TX1_START;
	output->counters.outputs_fast[ACCT_PROTO_NOVELL]++;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_NOVELL] +=
	    (bytes & ~MCI_TX_ODDALIGN);
	return(TRUE);
    } else if (output->priority_list) { 
        /*
         * if output is congested and we are sorting, defer to process level
         */
         return(FALSE);
    } else {
	
	/*
	 * Reserve failed on output -- flush the packet.
	 */	
	output->counters.output_total_drops++;
	inreg->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }
}

inline static boolean inline_item_denied (xnseacctype *item,
					  int list,
					  uchar proto,
					  ulong dstnet,
					  ulong dsthost_high,
					  ushort dsthost_low,
					  ushort dsock,
					  ulong srcnet,
					  ulong srchost_high,
					  ushort srchost_low,
					  ushort ssock)

{
    ulong flags;

    if (item == NULL) {
	return(FALSE);
    }
    
    do {
	flags = item->flags;
       	/*
	 * A network of -1 indicates all XNS networks.
	 * A socket of 0 indicates all XNS sockets.
	 */
	if (flags & XNS_SRCNET) { 
	    if (item->srcnet != NOVELL_TYPE_ANY_NET) {
	      if (flags & XNS_SRCNETMASK) {
		if (item->srcnet != (srcnet & item->srcnetmask))
		  continue;
	      } else {
		if (item->srcnet != srcnet)
		  continue;
	      }
	    }
	}
	if (flags & XNS_DSTNET) { 
	    if (item->dstnet != NOVELL_TYPE_ANY_NET) {
	      if (flags & XNS_DSTNETMASK) {
		if (item->dstnet != (dstnet & item->dstnetmask))
		  continue;
	      } else {
		if (item->dstnet != dstnet)
		  continue;
	      }
	    }
	}
	if (flags & XNS_SRCHOST) {
	    /*
	     * Check for match on source host address
	     */
	  if (flags & XNS_SRCHOSTMASK) {
	    if ((srchost_low & item->srchostmask.u.sword[2]) != 
		item->srchost.u.sword[2])
	      continue;
	    if ((srchost_high & item->srchostmask.u.lword[0]) != 
		(item->srchost.u.lword[0]))
	      continue;
	  } else {
	    if (srchost_low != 	item->srchost.u.sword[2])
	      continue;
	    if (srchost_high != item->srchost.u.lword[0])
	      continue;
	  }
	}
	
	if (flags & XNS_DSTHOST) {
	    /*
	     * Check for match on dest host address
	     */
	  if (flags & XNS_DSTHOSTMASK) {
	    if ((dsthost_low & item->dsthostmask.u.sword[2]) != 
		item->dsthost.u.sword[2])
	      continue;
	    if ((dsthost_high & item->dsthostmask.u.lword[0]) != 
		(item->dsthost.u.lword[0]))
	      continue;
	  } else {
	    if (dsthost_low != item->dsthost.u.sword[2])
	      continue;
	    if (dsthost_high != item->dsthost.u.lword[0])
	      continue;
	  }
	}
	
	/* check if it is extended access list */
	if (list > MAXNOVACC ) {
	    if (flags & XNS_PROTOCOL) {
		if ((item->protocol != (ushort) -1) &&
		    (item->protocol != proto))
		    continue;
	    }
	    if (flags & XNS_SRCSOCKET) {
		if (item->srcsocket && (item->srcsocket != ssock))
		    continue;
	    }
	    if (flags & XNS_DSTSOCKET) {
		if(item->dstsocket && (item->dstsocket != dsock))
		    continue;
	    }
	    if (item->log)
		return (TRUE);
	}

#ifdef DEBUG
if ((!item->grant) && novell_debug) buginf("*deny*");
#endif

	return(!item->grant);

    } while ((item = item->next));

#ifdef DEBUG
if (novell_debug) buginf("**deny**");
#endif

    /* Got here -- access list says implied deny */
    return(TRUE); /* Return "item_denied" */
}

static boolean item_denied (xnseacctype *item,
			    int list,
			    uchar proto,
			    ulong dstnet,
			    ulong dsthost_high,
			    ushort dsthost_low,
			    ushort dsock,
			    ulong srcnet,
			    ulong srchost_high,
			    ushort srchost_low,
			    ushort ssock)
{
    return(inline_item_denied(item,
			      list,
			      proto,
			      dstnet,
			      dsthost_high,
			      dsthost_low,
			      dsock,
			      srcnet,
			      srchost_high,
			      srchost_low,
			      ssock));
}

/*
 * nov_acct_and_access_check
 * Determine if we are allowed to forward the passed datagram, based on the
 * NOVELL access pointer. Return FALSE if output access is denied.
 *
 * Also, perform IPX accounting on a packet.
 */
static boolean nov_acct_and_access_check_mci (hwidbtype *input,
					      idbtype *swoutput,
					      uchar proto,
					      ushort ipxlen,
					      boolean bcast)
{
    mciregtype *regptr;
    xnseacctype *item = NULL;
    int list = 0;
    register ulong srcnet, dstnet;
    register ushort srchost_low, dsthost_low, ssock, dsock;
    ulong srchost_high, dsthost_high;

    regptr = input->mci_regptr;

    if (input->status & IDB_ETHER) {
	if (input->rxtype == RXTYPE_SAP_NOVELL) {
	    /*
	     * Note that we have already sniffed 5 longwords, beginning with
	     * the packet type byte. Sniff two more longwords.
	     */
	    input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
	    input->sniff[SNIFFBASE+7].d.lword = regptr->readlong;
	    proto        = *(uchar  *)&input->sniff[SNIFFBASE+1].d.byte[0];
	    dstnet       = *(ulong  *)&input->sniff[SNIFFBASE+1].d.byte[1];
	    dsthost_high = *(ulong  *)&input->sniff[SNIFFBASE+2].d.byte[1];
	    dsthost_low  = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[1];
	    dsock        = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[3];
	    srcnet       = *(ulong  *)&input->sniff[SNIFFBASE+4].d.byte[1];
	    srchost_high = *(ulong  *)&input->sniff[SNIFFBASE+5].d.byte[1];
	    srchost_low  = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[1];
	    ssock        = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[3];
	} else if (input->rxtype == RXTYPE_SNAP_NOV1) {
	    regptr->argreg = ISO2_IPX_OFFSET + XNS_DNET_OFFSET;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    dstnet       = regptr->readlong;  
	    dsthost_high = regptr->readlong;  
	    dsthost_low  = regptr->readshort;
	    dsock        = regptr->readshort;
	    srcnet       = regptr->readlong;  
	    srchost_high = regptr->readlong;  
	    srchost_low  = regptr->readshort;
	    ssock        = regptr->readshort;
	} else { /* RXTYPE_NOVELL1 or RXTYPE_NOVELL2 */
	    regptr->argreg = ARPA_IPX_OFFSET + XNS_DNET_OFFSET;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    dstnet       = regptr->readlong;  
	    dsthost_high = regptr->readlong;  
	    dsthost_low  = regptr->readshort;
	    dsock        = regptr->readshort;
	    srcnet       = regptr->readlong;  
	    srchost_high = regptr->readlong;  
	    srchost_low  = regptr->readshort;
	    ssock        = regptr->readshort;
	}
    } else if (input->status & IDB_FDDI) {
	if (input->rxtype == RXTYPE_SNAP_NOV1) {
	    regptr->argreg = (FDDI_ISO2_IPX_OFFSET + 
			      input->rxrifwordlen +
			      XNS_DNET_OFFSET);
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    dstnet       = regptr->readlong;  
	    dsthost_high = regptr->readlong;  
	    dsthost_low  = regptr->readshort;
	    dsock        = regptr->readshort;
	    srcnet       = regptr->readlong;  
	    srchost_high = regptr->readlong;  
	    srchost_low  = regptr->readshort;
	    ssock        = regptr->readshort;
	} else { /* RXTYPE_SAP_NOVELL */
	/*
	 * Note that we have already sniffed 5 longwords, beginning with
	 * the packet type byte. Sniff two more longwords.
	 */
	    input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
	    input->sniff[SNIFFBASE+7].d.lword = regptr->readlong;
	    proto        = *(uchar  *)&input->sniff[SNIFFBASE+1].d.byte[0];
	    dstnet       = *(ulong  *)&input->sniff[SNIFFBASE+1].d.byte[1];
	    dsthost_high = *(ulong  *)&input->sniff[SNIFFBASE+2].d.byte[1];
	    dsthost_low  = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[1];
	    dsock        = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[3];
	    srcnet       = *(ulong  *)&input->sniff[SNIFFBASE+4].d.byte[1];
	    srchost_high = *(ulong  *)&input->sniff[SNIFFBASE+5].d.byte[1];
	    srchost_low  = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[1];
	    ssock        = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[3];
	}
    } else if (input->status & IDB_TR) {
	/*
	 * Note that we have already sniffed 5 longwords, beginning with
	 * the packet type byte. Sniff two more longwords.
	 */
	if (input->rxtype == RXTYPE_SAP_NOVELL) {
	    input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
	    input->sniff[SNIFFBASE+7].d.lword = regptr->readlong;
	    proto        = *(uchar  *)&input->sniff[SNIFFBASE+1].d.byte[0];
	    dstnet       = *(ulong  *)&input->sniff[SNIFFBASE+1].d.byte[1];
	    dsthost_high = *(ulong  *)&input->sniff[SNIFFBASE+2].d.byte[1];
	    dsthost_low  = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[1];
	    dsock        = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[3];
	    srcnet       = *(ulong  *)&input->sniff[SNIFFBASE+4].d.byte[1];
	    srchost_high = *(ulong  *)&input->sniff[SNIFFBASE+5].d.byte[1];
	    srchost_low  = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[1];
	    ssock        = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[3];
	} else { /* RXTYPE_SNAP_NOV1 */
	    regptr->argreg =
	      TR_ISO2_IPX_OFFSET + input->rxrifwordlen + XNS_DNET_OFFSET;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    dstnet       = regptr->readlong;  
	    dsthost_high = regptr->readlong;  
	    dsthost_low  = regptr->readshort;
	    dsock        = regptr->readshort;
	    srcnet       = regptr->readlong;  
	    srchost_high = regptr->readlong;  
	    srchost_low  = regptr->readshort;
	    ssock        = regptr->readshort;
	}
    } else { /* IDB_SERIAL */
	regptr->argreg = HDLC_IPX_OFFSET + XNS_DNET_OFFSET;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	dstnet       = regptr->readlong;  
	dsthost_high = regptr->readlong;  
	dsthost_low  = regptr->readshort;
	dsock        = regptr->readshort;
	srcnet       = regptr->readlong;  
	srchost_high = regptr->readlong;  
	srchost_low  = regptr->readshort;
	ssock        = regptr->readshort;
    }

    /*
     * Validate ALL packets against input filter:
     */
    list = input->fast_rcvidb->novell_inaccesslist; /* %%% */
    item = ipx_get_ext_acl(list);
    
#ifdef DEBUG
    if (novell_debug) if (item) buginf("(in)");
#endif
    
    if (item &&
 	item_denied(item, list, proto, dstnet, dsthost_high, dsthost_low,
 		    dsock,srcnet, srchost_high, srchost_low, ssock)) {
 	return(FALSE);
    }
    
#ifdef DEBUG
    if (novell_debug) if (item) buginf("(ok)");
#endif
    
    /*
     * Special handling for helpered, directed broadcasts:
     */
    if (bcast) {
 	
#ifdef DEBUG
  	if (novell_debug) buginf(" (bc)");
#endif
  	/*
  	 * Screen out ineligible entries:
  	 */
  	if ((dstnet == 0) ||
  	    (dstnet == NOVELL_TYPE_ANY_NET) ||
  	    ((proto == NOVELL_TYPE_NETBIOS) && !novell_type20_helpered) ||
  	    (dsock == NOVELL_SOCK_RIP) ||
  	    (dsock == NOVELL_SOCK_SAP) ||
  	    (dsock == NOVELL_SOCK_NLSP) ||
  	    (dsock == NOVELL_SOCK_IGRP)) {
#ifdef DEBUG
  	    if (novell_debug) buginf("*bc deny*");
#endif
  	    return(FALSE);
  	}
    }
    
    list = swoutput->novell_accesslist;
    item = ipx_get_ext_acl(list);

#ifdef DEBUG
    if (novell_debug) if (item) buginf("(out)");
#endif

    if (item &&
	item_denied(item, list, proto, dstnet,
		    dsthost_high, dsthost_low, dsock,srcnet,
		    srchost_high, srchost_low, ssock)) {
	return(FALSE);
    }

#ifdef DEBUG
	if (novell_debug) if (item) buginf("(ok)");
#endif
    
    if (swoutput->ipx_accounting)
      return(nov_fast_acct_do_work(srcnet, dstnet, srchost_high, srchost_low,
				   dsthost_high, dsthost_low, ipxlen));
    return(TRUE);
}

/*
 * nov_access_check
 */
static boolean nov_access_check_mci (hwidbtype *input,
				     int list,
				     uchar proto)
{
    mciregtype *regptr;
    xnseacctype *item;
    register ulong srcnet, dstnet;
    register ushort srchost_low, dsthost_low, ssock, dsock;
    ulong srchost_high, dsthost_high;

    regptr = input->mci_regptr;

    if (list == 0) {
	return(FALSE);
    }

    item = ipx_get_ext_acl(list);
    if (item == NULL) {
	return(FALSE);
    }

    if (input->status & IDB_ETHER) {
	if (input->rxtype == RXTYPE_SAP_NOVELL) {
	    /*
	     * Note that we have already sniffed 5 longwords, beginning with
	     * the packet type byte. Sniff two more longwords.
	     */
	    input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
	    input->sniff[SNIFFBASE+7].d.lword = regptr->readlong;
	    proto        = *(uchar  *)&input->sniff[SNIFFBASE+1].d.byte[0];
	    dstnet       = *(ulong  *)&input->sniff[SNIFFBASE+1].d.byte[1];
	    dsthost_high = *(ulong  *)&input->sniff[SNIFFBASE+2].d.byte[1];
	    dsthost_low  = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[1];
	    dsock        = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[3];
	    srcnet       = *(ulong  *)&input->sniff[SNIFFBASE+4].d.byte[1];
	    srchost_high = *(ulong  *)&input->sniff[SNIFFBASE+5].d.byte[1];
	    srchost_low  = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[1];
	    ssock        = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[3];
	} else if (input->rxtype == RXTYPE_SNAP_NOV1) {
	    regptr->argreg = ISO2_IPX_OFFSET + XNS_DNET_OFFSET;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    dstnet       = regptr->readlong;  
	    dsthost_high = regptr->readlong;  
	    dsthost_low  = regptr->readshort;
	    dsock        = regptr->readshort;
	    srcnet       = regptr->readlong;  
	    srchost_high = regptr->readlong;  
	    srchost_low  = regptr->readshort;
	    ssock        = regptr->readshort;
	} else { /* RXTYPE_NOVELL1 or RXTYPE_NOVELL2 */
	    regptr->argreg = ARPA_IPX_OFFSET + XNS_DNET_OFFSET;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    dstnet       = regptr->readlong;  
	    dsthost_high = regptr->readlong;  
	    dsthost_low  = regptr->readshort;
	    dsock        = regptr->readshort;
	    srcnet       = regptr->readlong;  
	    srchost_high = regptr->readlong;  
	    srchost_low  = regptr->readshort;
	    ssock        = regptr->readshort;
	}
    } else if (input->status & IDB_FDDI) {
	if (input->rxtype == RXTYPE_SNAP_NOV1) {
	    regptr->argreg = (FDDI_ISO2_IPX_OFFSET + 
			      input->rxrifwordlen +
			      XNS_DNET_OFFSET);
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    dstnet       = regptr->readlong;  
	    dsthost_high = regptr->readlong;  
	    dsthost_low  = regptr->readshort;
	    dsock        = regptr->readshort;
	    srcnet       = regptr->readlong;  
	    srchost_high = regptr->readlong;  
	    srchost_low  = regptr->readshort;
	    ssock        = regptr->readshort;
	} else { /* RXTYPE_SAP_NOVELL */
	/*
	 * Note that we have already sniffed 5 longwords, beginning with
	 * the packet type byte. Sniff two more longwords.
	 */
	    input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
	    input->sniff[SNIFFBASE+7].d.lword = regptr->readlong;
	    proto        = *(uchar  *)&input->sniff[SNIFFBASE+1].d.byte[0];
	    dstnet       = *(ulong  *)&input->sniff[SNIFFBASE+1].d.byte[1];
	    dsthost_high = *(ulong  *)&input->sniff[SNIFFBASE+2].d.byte[1];
	    dsthost_low  = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[1];
	    dsock        = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[3];
	    srcnet       = *(ulong  *)&input->sniff[SNIFFBASE+4].d.byte[1];
	    srchost_high = *(ulong  *)&input->sniff[SNIFFBASE+5].d.byte[1];
	    srchost_low  = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[1];
	    ssock        = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[3];
	}
    } else if (input->status & IDB_TR) {
	/*
	 * Note that we have already sniffed 5 longwords, beginning with
	 * the packet type byte. Sniff two more longwords.
	 */
	if (input->rxtype == RXTYPE_SAP_NOVELL) {
	    input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
	    input->sniff[SNIFFBASE+7].d.lword = regptr->readlong;
	    proto        = *(uchar  *)&input->sniff[SNIFFBASE+1].d.byte[0];
	    dstnet       = *(ulong  *)&input->sniff[SNIFFBASE+1].d.byte[1];
	    dsthost_high = *(ulong  *)&input->sniff[SNIFFBASE+2].d.byte[1];
	    dsthost_low  = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[1];
	    dsock        = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[3];
	    srcnet       = *(ulong  *)&input->sniff[SNIFFBASE+4].d.byte[1];
	    srchost_high = *(ulong  *)&input->sniff[SNIFFBASE+5].d.byte[1];
	    srchost_low  = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[1];
	    ssock        = *(ushort *)&input->sniff[SNIFFBASE+6].d.byte[3];
	} else { /* RXTYPE_SNAP_NOV1 */
	    regptr->argreg =
	      TR_ISO2_IPX_OFFSET + input->rxrifwordlen + XNS_DNET_OFFSET;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    dstnet       = regptr->readlong;  
	    dsthost_high = regptr->readlong;  
	    dsthost_low  = regptr->readshort;
	    dsock        = regptr->readshort;
	    srcnet       = regptr->readlong;  
	    srchost_high = regptr->readlong;  
	    srchost_low  = regptr->readshort;
	    ssock        = regptr->readshort;
	}
    } else { /* IDB_SERIAL */
	regptr->argreg = HDLC_IPX_OFFSET + XNS_DNET_OFFSET;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	dstnet       = regptr->readlong;  
	dsthost_high = regptr->readlong;  
	dsthost_low  = regptr->readshort;
	dsock        = regptr->readshort;
	srcnet       = regptr->readlong;  
	srchost_high = regptr->readlong;  
	srchost_low  = regptr->readshort;
	ssock        = regptr->readshort;
    }

#ifdef DEBUG
    if (novell_debug) if (item) buginf("(acl-%d)", list);
#endif
    
    if (item_denied(item, list, proto, dstnet, dsthost_high, dsthost_low,
 		    dsock,srcnet, srchost_high, srchost_low, ssock)) {
 	return(FALSE);
    }
    
#ifdef DEBUG
    if (novell_debug) if (item) buginf("(ok)");
#endif
    
    return(TRUE);
}


#define DDR_SETNEXT(OUT, DDB)						    \
    output_mci_regptr = OUT->mci_regptr;                                    \
    if (output->cache_board_encap != NULL) {				    \
	/*								    \
	 * Set proper MIP channel.					    \
	 */								    \
	(*output->cache_board_encap)(output, path->novp_mh.mac_bytes,	    \
				     sizeof(ushort));			    \
    }

#define DDR_SETNEXT_OA(OUT, DDB)					    \
    output_mci_regptr = OUT->mci_regptr;				    \
    if (output->cache_board_encap != NULL) {				    \
	/*								    \
	 * Set proper MIP channel.					    \
	 */								    \
	(*output->cache_board_encap)(output, &path->novp_mh_oa.mac_bytes[1],\
				     sizeof(ushort));			    \
    }

#define DDR_CLASSIFY(OFFSET, RESUME)					    \
    if (!nov_access_check_mci(input, dialerlist, (ushort)len_hops))	    \
	goto RESUME;


/*
 * Run input swidb match only if input-access-group defined:
 */
#define SET_FAST_RCVIDB(ENCAP)                                              \
    FOR_ALL_SWIDBS_ON_HW(input, swinput) {                                  \
        if (swinput->novellnets &&                                          \
	    (swinput->primary_nidb->enctype == (ENCAP))) {                  \
	    input->fast_rcvidb = swinput;                                   \
	    break;                                                          \
	}                                                                   \
    }

#define NOVELL_CONNECTED_NET                                                \
    /*									    \
     * The destination is on directly connected net. Read the destnation    \
     * address from received packet and check if it is targeted at us or a  \
     * a broadcast packet and return FALSE if that is the case.	            \
     */									    \
    if ((output->status & IDB_FDDI) && (output->type !=IDBTYPE_FDDIT))     \
        return(FALSE);


/*
 * NOV_FAST_PATH is a generic output for even aligned input packets.
 * nov_fast_smds_setup_inline
 * This will set up the pad and trailer if we are even aligned
 */
static inline void nov_fast_smds_setup_inline (hwidbtype *input,
					       hwidbtype *output,
					       mciregtype *outptr,
					       novpathtype *path,
					       ushort ipxlen,
					       ushort THELINE)
{
    smds_pdb_t *spd = output->smds_pdb;
    smdstype *sm;

    sm_pad_offset = THELINE + ((ipxlen & ~0x03) >> 1);
    sm = (smdstype *)&path->novp_mh.mac_bytes[spd->fast_output_modelen];
    /*
     * find out the pad size and adjust the per packet smds stuff
     */
    ipxlen += path->total_encap_bytes - spd->fast_output_modelen;
    sm_pad = 3 - ((ipxlen + 3) % 4);
    sm->smds_basize = ipxlen - 4 + sm_pad;
    sm->smds_xhlpi &= ~3;
    sm->smds_xhlpi |= sm_pad;
    sm->smds_betag = output->smds_begin_tag++;

    /*
     * if we have a pad, read the last-long into sm_lastbuffer.
     * pad the bytes and move the trailer in
     */
    if (sm_pad) {
	outptr->argreg = sm_pad_offset;
	outptr->cmdreg = MCI_CMD_RX_SELECT;
	*(ulong *)&sm_lastbuffer[0] = outptr->readlong;
	switch (sm_pad) {
	case 3:
	    PUTLONG(&sm_lastbuffer[1], 0);
	    PUTLONG(&sm_lastbuffer[4], GETLONG(&sm->smds_rsvd));
	    break;
	case 2:
	    PUTSHORT(&sm_lastbuffer[2], 0);
	    PUTLONG(&sm_lastbuffer[4], GETLONG(&sm->smds_rsvd));
	    break;
	case 1:
	    sm_lastbuffer[3] = 0;
	    PUTLONG(&sm_lastbuffer[4], GETLONG(&sm->smds_rsvd));
	    break;
	default: /* Not needed */
	    PUTLONG(sm_lastbuffer, GETLONG(&sm->smds_rsvd));
	    break;
	}
    } else {
	/* No pad - just transfer the smds trailer */
	PUTLONG(sm_lastbuffer, GETLONG(&sm->smds_rsvd));
    }
}
/*
 * NOV_FAST_PATH is a generic output for even aligned input packets.
 * THE_LINE points to the start of the packet, with the input encapsulation
 * deleted.
 */

#define NOV_FAST_PATH(THE_LINE)						    \
                                 					    \
    output_status = output->status;					    \
                                 					    \
    if (output_status & IDB_ETHER) {                                	    \
	/*								    \
         * -- output ethernet.						    \
         */                                                                 \
	enctype = path->enctype;                                            \
	temp_length = ipxlen;                                               \
                                 					    \
	if (enctype == ET_SAP) {                                            \
	    temp_length += ETHER_SAP_ENCAPBYTES;                            \
	    inreg->argreg = THE_LINE - btow(ETHER_SAP_ENCAPBYTES);	    \
	} else if (enctype == ET_SNAP) {                                    \
	    temp_length += ETHER_SNAP_ENCAPBYTES;                           \
	    inreg->argreg = THE_LINE - btow(ETHER_SNAP_ENCAPBYTES);	    \
                                                                            \
	} else if (enctype == ET_ISL) {                                     \
                                                                            \
	    temp_length += (ETHER_ARPA_ENCAPBYTES + ISL_HEADER_LENGTH);     \
                                                                            \
	    inreg->argreg = THE_LINE - btow(ETHER_ARPA_ENCAPBYTES +         \
				            ISL_HEADER_LENGTH);             \
       				                                            \
	} else if (enctype == ET_SDE) {                                     \
                                                                            \
	    temp_length += (ETHER_ARPA_ENCAPBYTES + SDE_ENCAPBYTES);        \
                                                                            \
	    inreg->argreg = THE_LINE - btow(ETHER_ARPA_ENCAPBYTES +	    \
					    SDE_ENCAPBYTES);                \
                                                                            \
	} else {  /* ET_ARPA || ET_NOVELL_ETHER */                          \
                                                                            \
	    temp_length += ETHER_ARPA_ENCAPBYTES;                           \
	    inreg->argreg = THE_LINE - btow(ETHER_ARPA_ENCAPBYTES);	    \
	}                                                                   \
                                                                            \
	inreg->cmdreg = MCI_CMD_TX1_SELECT;                  	            \
	temp_length = nov_write_hes_ether_header(enctype, temp_length,	    \
						 inreg, path, ipxlen,	    \
						 csum, len_hops);	    \
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;                                  \
                                                                            \
    } else if (output_status & IDB_FDDI) {                          	    \
	/*                                                                  \
	 *  -- output fddi.						    \
	 */								    \
	temp_length = ipxlen + (path->total_encap_bytes);                   \
	 if ((enctype = path->enctype) == ET_SNAP) {                        \
             inreg->argreg = THE_LINE - btow(FDDI_SNAP_ENCAPBYTES) -	    \
		    path->rifwords;					    \
	 } else if ((enctype = path->enctype) == ET_NOVELL_FDDI) {          \
             inreg->argreg = THE_LINE - btow(FDDI_ENCAPBYTES) -             \
		    path->rifwords;					    \
	 } else { /* ET_SAP */                                              \
	     inreg->argreg = THE_LINE - (FDDI_SAP_ENCAPBYTES >> 1) -	    \
		    path->rifwords;					    \
	 }                                                                  \
	 inreg->cmdreg = MCI_CMD_TX1_SELECT;				    \
	 temp_length = nov_write_hes_fddi_header(enctype, inreg, path,	    \
						 ipxlen, csum, len_hops);   \
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
    } else if (output_status & IDB_TR) {                          	    \
	/*                                                                  \
	 * -- output token ring.					    \
	 */								    \
	if ((enctype = path->enctype) == ET_SAP) {                          \
	    inreg->argreg = THE_LINE - btow(TRING_SAP_ENCAPBYTES)  - 	    \
		    path->rifwords;					    \
	} else { /* ET_SNAP */                                              \
	    inreg->argreg = THE_LINE - (TRING_SNAP_ENCAPBYTES >> 1) -	    \
		    path->rifwords;             			    \
	}                                                                   \
	inreg->cmdreg = MCI_CMD_TX1_SELECT;                                 \
	temp_length = nov_write_hes_token_header(enctype, inreg, path,	    \
						 ipxlen, csum, len_hops);   \
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
     } else if (output_status & IDB_ATM) {                                  \
        /*                                                                  \
         * -- output ATM.						    \
         */                                                                 \
        inreg->argreg = THE_LINE - path->total_encap_words;                 \
        inreg->cmdreg = MCI_CMD_TX1_SELECT;                                 \
        if (is_atm_lane(path->novp_idb_ptr)) {                              \
            temp_length = path->total_encap_bytes + ipxlen;                 \
            temp_length = nov_write_hes_lane_header(path->enctype,          \
                                                    temp_length,            \
                                                    inreg,                  \
                                                    path,                   \
                                                    ipxlen,                 \
                                                    csum,                   \
                                                    len_hops);              \
        } else {                                                            \
            temp_length = nov_write_hes_atm_header(inreg,                   \
                                                   path,                    \
                                                   ipxlen,                  \
                                                   csum,                    \
                                                   len_hops);               \
	}                                                                   \
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
	ATM_VCOUTFAST(output, path->novp_mh.mac_shorts[0]);		    \
	ATM_OUTFAST(output);						    \
     } else if (output_status & IDB_SERIAL) {				    \
	/*                                                                  \
	 * -- output serial.						    \
	 */                                                                 \
	if (output->fast_switch_type == FS_SMDS) {			    \
	    inreg->argreg = sm_pad_offset;				    \
	    inreg->cmdreg = MCI_CMD_TX1_SELECT;			    	    \
	    inreg->write1long = GETLONG(&sm_lastbuffer[0]);    	    	    \
	    if (sm_pad) {						    \
	       inreg->write1long = GETLONG(&sm_lastbuffer[4]);    	    \
	       ipxlen += sm_pad;				    	    \
	    }								    \
	    ipxlen += sizeof(ulong);				    	    \
	}								    \
	inreg->argreg = THE_LINE - path->total_encap_words;		    \
	inreg->cmdreg = MCI_CMD_TX1_SELECT;                  	            \
	temp_length = nov_write_hes_serial_header(output, inreg, path,	    \
						  ipxlen, csum, len_hops);  \
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
     } else if (output_status & IDB_LEX) {				    \
	/*								    \
         * -- output lex.						    \
         */                                                                 \
	enctype = path->enctype;                                            \
	temp_length = ipxlen;                                               \
	if (enctype == ET_SAP) {                                            \
	    inreg->argreg = THE_LINE - WORDS(ROUND(path->length));	    \
	} else if (enctype == ET_SNAP) {                                    \
	    inreg->argreg = THE_LINE - WORDS(path->length);		    \
	} else { /* ET_ARPA || ET_NOVELL_ETHER */                           \
	    inreg->argreg = THE_LINE - WORDS(path->length);		    \
	}                                                                   \
	inreg->cmdreg = MCI_CMD_TX1_SELECT;                  	            \
	temp_length = nov_write_hes_lex_header(enctype, inreg, path, ipxlen,\
					       csum, len_hops);		    \
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
    }								            \
    novell_stat[NOVELL_INPUT]++;					    \
    novell_stat[NOVELL_FORWARDED]++;					    \
    output->counters.tx_cumbytes_fast[ACCT_PROTO_NOVELL] +=                 \
                       (temp_length & ~MCI_TX_ODDALIGN);		    \
    output->counters.outputs_fast[ACCT_PROTO_NOVELL]++;			    \
    return(TRUE);



/*
 * nov_hes_fastswitch
 * fast switching routine for even aligned novell input packets.
 * THE_LINE variable should contain the offset (in words) to the
 * start of the IPX packet. This is an inline - if THE_LINE is a constant,
 * the code generated will be very efficient
 */
static boolean nov_hes_fastswitch (hwidbtype *input, ushort THE_LINE)
{
    mciregtype *inreg, *output_mci_regptr;
    novcachetype *cache;
    novpathtype *path;
    hwidbtype *output;
    idbtype *swoutput;
    ushort temp_length, addrlow, dsock;
    charlong net, addrhigh;
    ulong indx, len_hops, output_status;
    ushort ipxlen, csum;
    int enctype;

    /* If spx spoofing is enable, force processing in the slow path */
    if (is_ddr(input)) {
      /* get the dialer hw interface */
      hwidbtype *netidb;

      netidb = get_netidb(input);
      if (netidb->novell_spxspoof)
	return(FALSE);
    } else {
      if (input->novell_spxspoof)
	return(FALSE);
    }

    /*
     * Setup novell data read pointer to check the hop count.
     */
    temp_length = 0;
    inreg = input->mci_regptr;
    inreg->argreg = THE_LINE + XNS_CSUM_OFFSET;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    csum = inreg->readshort;
    len_hops = inreg->readlong;
    ipxlen = (len_hops >> 16);

    /* check if it has valid hop count (including reserved tc bits) */
    if (((ushort)(len_hops) < (novell_max_hops << 8))) {
      hop_count_ok:
	net.d.lword = inreg->readlong;
	addrhigh.d.lword = inreg->readlong;
	addrlow = inreg->readshort;

	indx = net.d.lword;
	indx ^= addrhigh.d.lword;
	indx ^= addrlow;
	indx ^= (indx >> 16);
	indx ^= (indx >> 8);
	indx &= 0xFF;

	for (cache = novcache[indx]; cache; cache = cache->novc_next_ptr) {
	    if ((*(ushort *)&cache->novc_dest_address[4] == addrlow) &&
		(*(ulong *)cache->novc_dest_address == addrhigh.d.lword) &&
		(cache->novc_dest_net == net.d.lword)) { 
		if (cache->novc_version == novcache_version) {
  		    if (cache->novc_active) {
  		      n_resume_active:
			path = cache->novc_path_next_ptr;
			swoutput = path->novp_idb_ptr;
			output = swoutput->hwptr;
			
			/* Check total size; dump to slow switch if too big */
			if ((ulong) (ipxlen + path->total_encap_bytes) >
			    output->max_pak_size)
			    return(FALSE);
			
			/*
			 * if output is congested and we are sorting traffic,
			 * defer to process level
			 */
			if (output->output_qcount && output->priority_list)
			    return(FALSE);
			
			/* 
			 * Check if this cache entry is for directly connected net.
			 */
			if (!cache->novc_connected) {
			  n_resume_flow:
			    /*
			     * if the source net in the packet is zero,
			     * send the packet to process level.
			     */
			    dsock = inreg->readshort; /* skip */
			    if (inreg->readlong == 0) {
				return(FALSE);
			    }
			    cache->novc_path_next_ptr = path->novp_next_ptr;
			    /*
			     * mci_testptr is always equal to mci_regptr, except 
			     * when the interface is configured for DDR, in which 
			     * case it is NULL so that we fall through the 
			     * following tests directly to is_ddr().
			     */ 
			    output_mci_regptr = output->mci_testptr;
			    
			    /* 
			     * Check if access list is enabled on *any* interface:
			     */
			    if (!novell_global_acl_checks && !cache->novc_bcast) {
			      resume_dialer:
			      n_yes_doit:
				if (inreg == output_mci_regptr) {
				    /*                                            
				     * Output interface is on the same MCI. Move
				     * RX buffer from receive window to transmit
				     * window 1. This may fail if interface output
				     * queue limit has been exceeded.
				     */
				    if (output->fast_switch_type == FS_SMDS) {
					nov_fast_smds_setup_inline(input,
								   output,
								   output_mci_regptr,
								   path,
								   ipxlen,
								   THE_LINE);
				    }
				    inreg->argreg = output->mci_index;
				    inreg->cmdreg = MCI_CMD_RX_MOVE;
				    len_hops = update_msb_tc(len_hops, path);
				    input->counters.inputs_fast[ACCT_PROTO_NOVELL]++;
				    input->counters.rx_cumbytes_fast[ACCT_PROTO_NOVELL] +=
					input->rxlength;
				    if (inreg->cmdreg == MCI_RSP_OKAY) {
#ifdef DEBUG
					display_trace(input, output,
						      path, "nfpE ");
#endif
					NOV_FAST_PATH(THE_LINE);
					return(TRUE);
				    } else if (output->priority_list) { 
					/*
					 * if output is congested and we are
					 * sorting, defer to process level
					 */
					return(FALSE);
				    } else { 
					/*
					 * Move failed -- flush the packet.
					 */
#ifdef DEBUG
					display_trace(input, output, path,
						      "*** move failed");
#endif
					inreg->cmdreg = MCI_CMD_RX_FLUSH;
					output->counters.output_total_drops++;
					return(TRUE);
				    }
				} else if (output_mci_regptr) {
				    /*
				     * Another MCI
				     */
				    /*
				     * if lex output encaps is sap - punt to
				     * process level because if output interface
				     * is lex with encaps sap the packet
				     * gets Odd Aligned.
				     */
				    if ((output->status & IDB_LEX) &&
					(path->enctype == ET_SAP)) 
					return (FALSE);
				    return(nov_mci2mci(input, path,
						       len_hops, csum));
				} else if (is_ddr(output)) {
				    DDR_FASTSWITCH(input, output, cache,DDR_SETNEXT,
						   ipx_dialerlist, DDR_CLASSIFY, 0,
						   path->dialergroup,
						   resume_dialer);
				} else {
				    /*
				     * Oops.  Shouldn't be here.
				     */
				    return(FALSE);
				}
			    } else {
				/*
				 * Check if access list enabled on *this* itf:
				 */
				if (swoutput->novell_output_acl_checks ||
				    input->novell_input_acl_checks ||
				    cache->novc_bcast) {
				    if (!(nov_acct_and_access_check_mci
					  (input, swoutput, (ushort)len_hops,
					   ipxlen, cache->novc_bcast)))
					goto n_deny_access;
				}
				goto n_yes_doit;
			    }
			} else {
			    NOVELL_CONNECTED_NET;
			    goto n_resume_flow;		   
			}
   		    } else {
  			cache->novc_active++;
  			goto n_resume_active;
   		    }
		} else {
		    goto n_deny_access;
		}
	    }
	}
    } else {
	/*
	 * The max hop count may have been reached. However, now do a
	 * comprehensive check to determine if the hop count is excessive.
	 */
	if (NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED) {
	    if ((((ushort)(len_hops) >= (XNS_IGRP_MIN_HOPS << 8)) &&
		 ((ushort)(len_hops) <  (XNS_IGRP_MAX_HOPS << 8))) ||
		((ushort)(len_hops)  < ((XNS_MAX_HOPS+1) << 8))) {
		goto hop_count_ok;
	    }
	}
    }
n_deny_access:
    return(FALSE);             
}

/***************************************************************************
 *             NOVELL FAST SWITCHING ODD ALIGNED ROUTINES
 ***************************************************************************/
/*
 * nov_fast_smds_oa_setup_inline
 * sets up the pad and trailer information that we need when we are
 * odd aligned.
 *
 * Note that this routine needs to be called before the RXMOVE command
 * as we can't read back after that point.
 */
static inline void nov_fast_smds_oa_setup_inline (hwidbtype *input,
						  hwidbtype *output,
						  mciregtype *outptr,
						  novpathtype *path,
						  ushort ipxlen,
						  ushort THELINE)
{
    smds_pdb_t *spd = output->smds_pdb;
    smdstype *sm;
    
    sm_pad_offset = THELINE + ((ipxlen & ~0x03) >> 1);
    outptr->argreg = sm_pad_offset;
    outptr->cmdreg = MCI_CMD_RX_SELECT;
    PUTLONG(sm_lastbuffer, outptr->readlong);

    sm = (smdstype *)&path->novp_mh_oa.mac_bytes[spd->fast_output_modelen+1];
    ipxlen += path->total_encap_bytes - spd->fast_output_modelen;
    sm_pad = 3 - ((ipxlen + 3) % 4);
    sm->smds_basize = ipxlen - 4 + sm_pad;
    sm->smds_xhlpi &= ~3;
    sm->smds_xhlpi |= sm_pad;
    sm->smds_betag = output->smds_begin_tag++;

    /*
     * Pad out with zeroes, then move the trailer into the buffer.
     * 1st byte of sm_lastbuffer will always have data - we're odd
     * aligned, remember.  This is done here so we don't have to
     * remember sm.
     */
    switch (sm_pad) {
    case 3:
	PUTLONG(&sm_lastbuffer[2], 0);
	PUTLONG(&sm_lastbuffer[5], GETLONG(&sm->smds_rsvd));
	break;
    case 2:
	PUTSHORT(&sm_lastbuffer[3], 0);
	PUTLONG(&sm_lastbuffer[5], GETLONG(&sm->smds_rsvd));
	break;
    case 1:
	sm_lastbuffer[4] = 0;
	PUTLONG(&sm_lastbuffer[5], GETLONG(&sm->smds_rsvd));
	break;
    default:
	PUTLONG(&sm_lastbuffer[1], GETLONG(&sm->smds_rsvd));
    }
}
/*
 * NOV_FAST_PATH_OA is the generic output routine for input packets
 * that were odd aligned.  THE_LINE points to the input packet after
 * the input encapsulation
 */
#define NOV_FAST_PATH_OA(THE_LINE)					    \
    output_status = output->status;					    \
    if (output_status & IDB_ETHER) {                                	    \
        /*								    \
         * -- output ethernet.						    \
         */                                                                 \
	enctype = path->enctype;                                            \
	temp_length = ipxlen;                                               \
	if (enctype == ET_SAP) {                                            \
	    temp_length += ETHER_SAP_ENCAPBYTES;                            \
	    inreg->argreg = THE_LINE - (ETHER_SAP_ENCAPBYTES >> 1);	    \
	} else if (enctype == ET_SNAP) {                                    \
	    temp_length += ETHER_SNAP_ENCAPBYTES;                           \
	    inreg->argreg = THE_LINE - (ETHER_SNAP_ENCAPBYTES >> 1);        \
	} else if (enctype == ET_ISL) {                                     \
	    temp_length += (ISL_HEADER_LENGTH +	ETHER_ARPA_ENCAPBYTES);  \
	    inreg->argreg = THE_LINE - ((ISL_HEADER_LENGTH +                \
					 ETHER_ARPA_ENCAPBYTES) >> 1);  \
	} else { /* ET_ARPA || ET_NOVELL_ETHER */                           \
	    temp_length += ETHER_ARPA_ENCAPBYTES;                           \
	    inreg->argreg = THE_LINE - (ETHER_ARPA_ENCAPBYTES >> 1);	    \
	}                                                                   \
	inreg->cmdreg = MCI_CMD_TX1_SELECT;                  	            \
	temp_length = nov_write_hes_ether_oa_header(enctype, temp_length,   \
						    inreg, path, ipxlen,    \
						    csum, len_hops);	    \
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
    } else if (output_status & IDB_FDDI) {                          	    \
	/*                                                                  \
	 * -- output fddi.		                                    \
	 */								    \
	temp_length = ipxlen + (path->total_encap_bytes);                   \
	if ((enctype = path->enctype) == ET_SNAP) {                         \
	    inreg->argreg = (THE_LINE - (FDDI_SNAP_ENCAPBYTES >> 1) -	    \
			     path->rifwords);             		    \
	} else if (enctype == ET_NOVELL_FDDI) {                             \
	    inreg->argreg = (THE_LINE - (FDDI_ENCAPBYTES >> 1) -	    \
			     path->rifwords);             		    \
        } else {                                                            \
	    inreg->argreg = (THE_LINE - (FDDI_SAP_ENCAPBYTES >> 1) -	    \
			     path->rifwords);             		    \
	}                                                                   \
	inreg->cmdreg = MCI_CMD_TX1_SELECT;                  	            \
	temp_length = nov_write_hes_fddi_oa_header(enctype, inreg, path,    \
						   ipxlen, csum, len_hops); \
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
    } else if (output_status & IDB_TR) {                          	    \
        /*                                                                  \
	 * -- output token ring.					    \
	 */								    \
	if ((enctype = path->enctype) == ET_SAP) {                          \
	    inreg->argreg = (THE_LINE - (TRING_SAP_ENCAPBYTES >> 1) -	    \
			     path->rifwords);             		    \
	} else { /* ET_SNAP */                                              \
	    inreg->argreg = (THE_LINE - (TRING_SNAP_ENCAPBYTES >> 1) -	    \
			     path->rifwords);             		    \
	}                                                                   \
	inreg->cmdreg = MCI_CMD_TX1_SELECT;                                 \
	temp_length = nov_write_hes_token_oa_header(enctype, inreg, path,   \
						    ipxlen, csum, len_hops);\
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
     } else if (output_status & IDB_ATM) {                                  \
        /*                                                                  \
         * -- output ATM.		                                    \
         *                                                                  \
         * Since total_encap_words is calc'd with btow, we can't use it     \
         * for nov_write_hes_lane_oa_header.                                \
         */                                                                 \
        if (is_atm_lane(path->novp_idb_ptr)) {                              \
            inreg->argreg = THE_LINE - (path->total_encap_bytes >> 1);      \
            inreg->cmdreg = MCI_CMD_TX1_SELECT;                             \
            temp_length = path->total_encap_bytes + ipxlen;                 \
            temp_length = nov_write_hes_lane_oa_header(path->enctype,       \
                                                       temp_length,         \
                                                       inreg,               \
                                                       path,                \
                                                       ipxlen,              \
                                                       csum,                \
                                                       len_hops);           \
        } else {                                                            \
            inreg->argreg = THE_LINE - path->total_encap_words;             \
            inreg->cmdreg = MCI_CMD_TX1_SELECT;                             \
            temp_length = nov_write_hes_atm_oa_header(inreg,                \
                                                      path,                 \
                                                      ipxlen,               \
                                                      csum,                 \
                                                      len_hops);            \
	}                                                                   \
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
	ATM_VCOUTFAST(output, path->novp_mh.mac_shorts[0]);                 \
	ATM_OUTFAST(output);						    \
    } else if (output_status & IDB_SERIAL) {				    \
	/*                                                                  \
	 * -- output serial.		                                    \
	 *                                                                  \
	 * Note: Odd-aligned serial output is supported on HSSI/FSIP only.  \
	 *                                                                  \
	 */                                                                 \
	if (output->fast_switch_type == FS_SMDS) {			    \
	    inreg->argreg = sm_pad_offset;				    \
	    inreg->cmdreg = MCI_CMD_TX1_SELECT;			    	    \
	    /* 								    \
	     * Note that we are writing an extra byte here		    \
	     */                                                             \
	    inreg->write1long = GETLONG(&sm_lastbuffer[0]);	    	    \
	    if (sm_pad) {						    \
	       inreg->write1long = GETLONG(&sm_lastbuffer[4]);    	    \
	       inreg->write1short = GETSHORT(&sm_lastbuffer[8]);    	    \
	       ipxlen += sm_pad;				    	    \
	    } else {							    \
	       inreg->write1short = GETSHORT(&sm_lastbuffer[4]);    	    \
	    }								    \
	    ipxlen += sizeof(ulong);				    	    \
	}								    \
	inreg->argreg = THE_LINE - path->total_encap_words;	    	    \
	inreg->cmdreg = MCI_CMD_TX1_SELECT;                  	            \
	temp_length = nov_write_hes_serial_oa_header(output, inreg, path,   \
						     ipxlen, csum, len_hops);\
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
    } else if (output_status & IDB_LEX) {                                   \
        /*								    \
         * -- output Lex.		                                    \
         */                                                                 \
	enctype = path->enctype;                                            \
	temp_length = ipxlen;                                               \
	if (enctype == ET_SAP) {                                            \
	    inreg->argreg = THE_LINE - WORDS(path->length);		    \
	} else if (enctype == ET_SNAP) {                                    \
	    inreg->argreg = THE_LINE - WORDS(path->length);		    \
	} else { /* ET_ARPA || ET_NOVELL_ETHER */                           \
	    inreg->argreg = THE_LINE - WORDS(path->length);		    \
	}                                                                   \
	inreg->cmdreg = MCI_CMD_TX1_SELECT;                  	            \
	temp_length = nov_write_hes_lex_oa_header(enctype, inreg, path,	    \
						  ipxlen, csum, len_hops);  \
	inreg->argreg = temp_length;					    \
        inreg->cmdreg = MCI_CMD_TX1_START;				    \
    }									    \
    novell_stat[NOVELL_INPUT]++;					    \
    novell_stat[NOVELL_FORWARDED]++;					    \
    output->counters.tx_cumbytes_fast[ACCT_PROTO_NOVELL] +=                 \
                       (temp_length & ~MCI_TX_ODDALIGN);		    \
    output->counters.outputs_fast[ACCT_PROTO_NOVELL]++;			    \
    return(TRUE);

/*
 * nov_hes_oa_fastswitch
 * fast switching routine for odd aligned novell input packets.
 * THE_LINE variable should contain the offset (in words) to the
 * odd aligned IPX packet. This is an inline - if THE_LINE is a constant,
 * the code generated will be very efficient
 */
static inline boolean nov_hes_oa_fastswitch (hwidbtype *input, ushort THE_LINE)
{
    mciregtype *inreg, *output_mci_regptr;
    novcachetype *cache;
    novpathtype *path;
    hwidbtype *output;
    idbtype *swoutput;
    ushort temp_length, addrlow;
    charlong net, addrhigh;
    ulong indx, len_hops, output_status;
    ushort ipxlen, csum;
    int enctype;

    /*
     * Setup novell data read pointer to check the hop count.
     */
    temp_length = 0;
    inreg = input->mci_regptr;
    inreg->argreg = THE_LINE + XNS_CSUM_OFFSET;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    csum = inreg->readshort;
    len_hops = inreg->readlong;
    ipxlen   = (ushort)(len_hops >> 8);

    /* check if it has valid hop count (including reserved tc bits) */
    if (((uchar)(len_hops) < novell_max_hops)) {
    oa_hop_count_ok:

	input->sniff[SNIFFBASE+1].d.lword = inreg->readlong;
	input->sniff[SNIFFBASE+2].d.lword = inreg->readlong;
	input->sniff[SNIFFBASE+3].d.lword = inreg->readlong;
	input->sniff[SNIFFBASE+4].d.lword = inreg->readlong;
	input->sniff[SNIFFBASE+5].d.lword = inreg->readlong;
	net.d.lword = *(ulong *)&input->sniff[SNIFFBASE+1].d.byte[1];
	addrhigh.d.lword = *(ulong *)&input->sniff[SNIFFBASE+2].d.byte[1];
	addrlow = *(ushort *)&input->sniff[SNIFFBASE+3].d.byte[1];

	indx = net.d.lword;
	indx ^= addrhigh.d.lword;
	indx ^= addrlow;
	indx ^= (indx >> 16);
	indx ^= (indx >> 8);
	indx &= 0xFF;
	for (cache = novcache[indx]; cache; cache = cache->novc_next_ptr) {
	    if ((*(ushort *)&cache->novc_dest_address[4] == addrlow) &&
		(*(ulong *)cache->novc_dest_address == addrhigh.d.lword) &&
		(cache->novc_dest_net == net.d.lword)) { 
		if (cache->novc_version == novcache_version) {
  		    if (cache->novc_active) {
  		      oa_n_resume_active:
			path = cache->novc_path_next_ptr;
			swoutput = path->novp_idb_ptr;
			output = swoutput->hwptr;
			
			/* Check total size; dump to slow switch if too big */
			if ((ulong) (ipxlen + path->total_encap_bytes) >
			    output->max_pak_size)
			    return(FALSE);
			
			/*
			 * if output is congested and we are sorting traffic,
			 * defer to process level
			 */
			if (output->output_qcount && output->priority_list)
			    return(FALSE);
			
			/* 
			 * Check if this cache entry is for directly connected net.
			 */
			if (!cache->novc_connected) {
			  oa_n_resume_flow:
			    /*
			     * if the source net in the packet is zero,
			     * send the packet to process level.
			     */
			    if (*(ulong *)&input->sniff[SNIFFBASE+4].d.byte[1]
				== 0) {
				return(FALSE);
			    }
			    cache->novc_path_next_ptr = path->novp_next_ptr;
			    /*
			     * mci_testptr is always equal to mci_regptr, except
			     * when the interface is configured for DDR, in which
			     * case it is NULL so that we fall through the
			     * following tests directly to is_ddr().
			     */
			    output_mci_regptr = output->mci_testptr;
			    /* 
			     * Check if access list is enabled on *any* interface:
			     */
			    if (!novell_global_acl_checks && !cache->novc_bcast) {
			      resume_dialer:
			      oa_n_yes_doit:
				if (inreg == output_mci_regptr) {
				    /*                                            
				     * Output interface is on the same MCI. Move
				     * RX buffer from receive window to transmit
				     * window 1. This may fail if interface output
				     * queue limit has been exceeded.
				     *
				     * MCI serial ports cannot support odd byte
				     * start, so toss the frame to process level.
				     */
				    if ((output->status & IDB_SERIAL) ||
					(output->status & IDB_LEX))
					if (output->status & IDB_MCI)
					    return(FALSE);
				    if (output->fast_switch_type == FS_SMDS) {
					nov_fast_smds_oa_setup_inline(input,
								      output,
								      output_mci_regptr,
								      path,
								      ipxlen,
								      THE_LINE);
				    }
				    inreg->argreg = output->mci_index;
				    inreg->cmdreg = MCI_CMD_RX_MOVE;
				    len_hops = update_lsb_tc(len_hops, path);
				    input->counters.inputs_fast[ACCT_PROTO_NOVELL]++;
				    input->counters.rx_cumbytes_fast[ACCT_PROTO_NOVELL] +=
					input->rxlength;
				    if (inreg->cmdreg == MCI_RSP_OKAY) {
#ifdef DEBUG
					display_trace(input, output,
						      path, "nfpE1");
#endif
					NOV_FAST_PATH_OA(THE_LINE);
					return(TRUE);
				    } else if (output->priority_list) { 
					/*
					 * if output is congested and we are
					 * sorting, defer to process level
					 */
					return(FALSE);
				    } else { 
					
					/*
					 * Move failed -- flush the packet.
					 */
#ifdef DEBUG
					display_trace(input, output, path,
						      "*** move failed");
#endif
					inreg->cmdreg = MCI_CMD_RX_FLUSH;
					output->counters.output_total_drops++;
					return(TRUE);
				    }
				} else if (output_mci_regptr) {
				    /*
				     * Another MCI:
				     * MCI serial ports cannot support odd byte
				     * start, so toss the frame to process level.
				     */
				    if ((output->status & IDB_SERIAL) ||
					(output->status & IDB_LEX))
					if (output->status & IDB_MCI)
					    return(FALSE);
				    return(nov_mci2mci1(input, path,
							len_hops, csum));
				} else if (is_ddr(output)) {
				    DDR_FASTSWITCH(input, output, cache,
						   DDR_SETNEXT_OA, ipx_dialerlist, 
						   DDR_CLASSIFY, 0,
						   path->dialergroup,
						   resume_dialer);
				} else {
				    /*
				     * Oops.  Shouldn't be here.
				     */
				    return(FALSE);
				}
			    } else {
				/*
				 * Check if access list enabled on *this* itf:
				 */
				if (swoutput->novell_output_acl_checks ||
				    input->novell_input_acl_checks ||
				    cache->novc_bcast) {
				    if (!(nov_acct_and_access_check_mci
					  (input, swoutput, (ushort)len_hops,
					   ipxlen, cache->novc_bcast)))
					goto oa_n_deny_access;
				}
				goto oa_n_yes_doit;
			    }
			} else {
			    NOVELL_CONNECTED_NET;
			    goto oa_n_resume_flow;		   
			}
		    } else {
			cache->novc_active++;
			goto oa_n_resume_active;
		    }
		} else {
		    goto oa_n_deny_access;
		}
	    }
	}
    } else {
	/*
	 * The max hop count may have been reached. However, now do a
	 * comprehensive check to determine if the hop count is excessive.
	 */
	if (NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED) {
	    if ((((uchar)(len_hops) >= XNS_IGRP_MIN_HOPS) &&
		 ((uchar)(len_hops) <  XNS_IGRP_MAX_HOPS)) ||
		((uchar)(len_hops)  < (XNS_MAX_HOPS+1))) {
		goto oa_hop_count_ok;
	    }
	}
    }
oa_n_deny_access:
    return(FALSE);
}

/*
 * ether_nov_fastswitch0
 * Fast switching of NOVELL-ETHER encapsulated packets.
 * NOTE: This could be combined with ether_nov_fastswitch for a
 * reduction in code, at the expense of the debugging output.
 */
static boolean ether_nov_fastswitch0 (hwidbtype *input)
{
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*enf0:");
#endif

    input->rxtype = RXTYPE_NOVELL2;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_ether_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_NOVELL_ETHER);

    return(nov_hes_fastswitch(input, ARPA_IPX_OFFSET));
}

/*
 * ether_nov_fastswitch
 * Fast switching of ARPA encapsulated packets.
 */
static boolean ether_nov_fastswitch (hwidbtype *input)
{
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*enfA:");
#endif

    input->rxtype = RXTYPE_NOVELL1;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_arpa_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_ARPA);

    return(nov_hes_fastswitch(input, ARPA_IPX_OFFSET));
}

/*
 * ether_nov_fastswitch1
 * Fast switching of ISO encapsulated packets. 
 */
static boolean ether_nov_fastswitch1 (hwidbtype *input)
{
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*enf1:");
#endif

    input->rxtype = RXTYPE_SAP_NOVELL;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_iso1_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_SAP);

    return(nov_hes_oa_fastswitch(input, ISO1_IPX_OFFSET));
}

/*
 * ether_nov_fastswitch2
 * Fast switching of ISO2 encapsulated packets. 
 */
static boolean ether_nov_fastswitch2 (hwidbtype *input)
{
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*enf2:");
#endif

    input->rxtype = RXTYPE_SNAP_NOV1;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_iso2_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_SNAP);

    return(nov_hes_fastswitch(input, ISO2_IPX_OFFSET));
}


/*
 * fddi_nov_fastswitch1
 * Fast switching of ISO1 fddi encapsulated packets.
 */
static boolean fddi_nov_fastswitch1 (hwidbtype *input)
{
    mciregtype *inreg;
    idbtype *swinput;
    
#ifdef DEBUG
    if (novell_debug) buginf("\n*fnf1:");
#endif
    inreg = input->mci_regptr;
    /*
     * This is the entry point for both ISO and SNAP encap packets when
     * running some versions of the old uCode. In order to handle
     * both encaps gracefully, we have to sniff the SAP field. If packet
     * is really SNAP, invoke the correct routine. To minimize the
     * performance impact, use new (V11) uCode.
     */
    inreg->argreg = MCI_OFFSETSIZE - 1 + input->rxrifwordlen;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    if (inreg->readshort == SNAPSNAP) {
#ifdef DEBUG
	if (novell_debug) buginf("->fnf2");
#endif
	/*
	 * Note: this could probably be called directly -
	 * would have to remove it below the other
	 */
        return(reg_invoke_fddi_fs(RXTYPE_SNAP_NOV1, input));
    }
    input->rxtype = RXTYPE_SAP_NOVELL;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_iso1_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_SAP);

    return(nov_hes_oa_fastswitch(input, 
				 (FDDI_ISO1_IPX_OFFSET + input->rxrifwordlen)));
}

/*
 * fddi_nov_fastswitch2
 * Fast switching of SNAP fddi encapsulated packets. 
 */
static boolean fddi_nov_fastswitch2 (hwidbtype *input)
{
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*fnf2:");
#endif

    input->rxtype = RXTYPE_SNAP_NOV1;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_iso2_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_SNAP);

    return(nov_hes_fastswitch(input, 
			      (FDDI_ISO2_IPX_OFFSET + input->rxrifwordlen)));
}

/*
 * fddi_nov_fastswitch_raw
 * Fast switching of fddi raw (unencapsulated) packets. 
 */
static boolean fddi_nov_fastswitch_raw (hwidbtype *input)
{
#ifdef DEBUG
    if (novell_debug) buginf("\n*fnfr:");
#endif
    input->rxtype = RXTYPE_NOVELL2;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_ether_ok)
	return(FALSE);
    return(nov_hes_fastswitch(input, 
			      (FDDI_RAW_IPX_OFFSET + input->rxrifwordlen)));
}

#ifdef DEBUG
/*
 * fddi_nov_fastswitch_unk
 * Sniff out possible unclassified fddi raw (unencapsulated) packets. 
 *
 * NOTE: FOR DEBUGGING ONLY. RAW FDDI FS CANNOT BE SUPPORTED OFFICIALLY
 *       ON HES UNTIL UCODE CAN CLASSIFY RAW FDDI PACKETS AS RXTYPE_NOVELL2.
 *       AT THAT POINT THIS ROUTINE WILL BE DELETED (SINCE THE PREVIOUS
 *       ROUTINE WILL BE INVOKED).
 */
static boolean fddi_nov_fastswitch_unk (hwidbtype *input)
{
    mciregtype *inreg;

/*#ifdef DEBUG*/
    if (novell_debug) buginf("\n*fnfu:");
/*#endif*/

    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_ether_ok)
	return(FALSE);

    inreg = input->mci_regptr;
    inreg->argreg = MCI_OFFSETSIZE - 1 + input->rxrifwordlen;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    if (inreg->readshort == 0xFFFF) {
/*#ifdef DEBUG*/
	if (novell_debug) buginf("->fnfr");
/*#endif*/
    }
    input->rxtype = RXTYPE_NOVELL2;
    return(nov_hes_fastswitch(input, 
			      (FDDI_RAW_IPX_OFFSET + input->rxrifwordlen)));
}
#endif

/*
 * token_nov_fastswitch2
 * Fast switching of SNAP token-ring encapsulated packets. 
 */
static boolean token_nov_fastswitch2 (hwidbtype *input)
{
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*tnf2:");
#endif

    input->rxtype = RXTYPE_SNAP_NOV1;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_iso2_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_SNAP);

    return(nov_hes_fastswitch(input,
			      (TR_ISO2_IPX_OFFSET + input->rxrifwordlen)));
}

/*
 * token_nov_fastswitch1
 * Fast switching of ISO1 token-ring encapsulated packets.
 */
static boolean token_nov_fastswitch1 (hwidbtype *input)
{
    mciregtype *inreg;
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*tnf1:");
#endif

    inreg = input->mci_regptr;
    /*
     * This is the entry point for both ISO and SNAP encap packets when
     * running some versions of the old uCode. In order to handle
     * both encaps gracefully, we have to sniff the SAP field. If packet
     * is really SNAP, invoke the correct routine. To minimize the
     * performance impact, use new (V11) uCode.
     */
    inreg->argreg = MCI_OFFSETSIZE - 1 + input->rxrifwordlen;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    if (inreg->readshort == SNAPSNAP) {
#ifdef DEBUG
	if (novell_debug) buginf("->tnf2");
#endif
	/*
	 * Note: this could also be a direct call
	 */
        return(reg_invoke_token_fs(RXTYPE_SNAP_NOV1, input));
    }
    input->rxtype = RXTYPE_SAP_NOVELL;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_iso1_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_SAP);

    return(nov_hes_oa_fastswitch(input,
				 (TR_ISO1_IPX_OFFSET + input->rxrifwordlen)));
}

/*
 * serial_nov_fastswitch
 * Fast switching of hdlc/ppp/cisco fr serial encapsulated packets. 
 */
static boolean serial_nov_fastswitch (hwidbtype *input)
{    
#ifdef DEBUG
    if (novell_debug) buginf("\n* snf:");
#endif

    input->rxtype = RXTYPE_NOVELL1;
    return(nov_hes_fastswitch(input, HDLC_IPX_OFFSET));
}

/*
 * frame_nov_fastswitch
 * Fast switching of IETF frame relay serial encapsulated packets. 
 * encapsulation length is FR_IETF_SNAP_ENCAPBYTES
 */
static boolean frame_nov_fastswitch (hwidbtype *input)
{
#ifdef DEBUG
    if (novell_debug) buginf("\n*frnf:");
#endif

    /*
     * This is the entry point for both variants of IETF encapsulation:
     *
     * - FR_SNAP0PAD ( 9 bytes) dlci+03+80+00+00+00+8137
     * - FR_SNAP1PAD (10 bytes) dlci+03+00+80+00+00+00+8137
     *
     * The fastswitch path does not currently handle the zero pad encap
     * so dump it to process switching.
     */
    return(nov_hes_fastswitch(input, FR_IETF_IPX_OFFSET));
}

/*
 * atm_dxi_nov_fastswitch
 * Fast switching of atm-dxi serial encapsulated packets.
 *
 * input encapsulation length is variable, and is located in
 * input->atm_dxi_stuff->fast_input_encapbytes;
 */
static boolean atm_dxi_nov_fastswitch (hwidbtype *input)
{
    ushort atm_dxi_the_line;
#ifdef DEBUG
    if (novell_debug) buginf("\n*atmd:");
#endif

    atm_dxi_the_line = input->rxoffset +
	btow(input->atm_dxi_stuff->fast_input_encsize);
    return(nov_hes_fastswitch(input, atm_dxi_the_line));
}

/*
 * aip_nov_fastswitch
 * Fast switching of atm encapsulated packets. 
 */
static boolean aip_nov_fastswitch (hwidbtype *input)
{
    mciregtype *inreg;
    uint lineoff;
    vc_info_t *vc;
    
#ifdef DEBUG
    if (novell_debug) buginf("\n* hnf:");
#endif

    inreg = input->mci_regptr;
    /*
     * Get the incoming VC info.  Lets us know what kind of encapsulation
     * mania we're dealing with.  Info is left in hwidb by CBUS_AIP_RECEIVE
     * in cbus_interrupt.
     *
     * Check for NULL vc done in CBUS_AIP_RECEIVE
     */
    vc = input->atm_db->fast_vc_info;   
    vc->infast++;			/* Count it. */

    if (input->novell_input_acl_checks)
	input->fast_rcvidb = vc->swidb;

    ATM_INFAST(input);
    lineoff = AIP_IPX_OFFSET(input, vc);

    switch (vc->flags & VC_FLAG_AAL_MASK) {
    case ATM_VC_AAL5NLPID:
	lineoff += btow(SNAP_HDRBYTES) - 1; /* Don't count SSAP & DSAP here */
	break;
    case ATM_VC_AAL5LANE:
	switch (inreg->rxtype) {
	case RXTYPE_NOVELL1:	/* arpa */
	case RXTYPE_NOVELL2:	/* novell raw */
	    break;
	case RXTYPE_SAP_NOVELL:
	    lineoff += SAP_HDRBYTES >> 1;
            return(nov_hes_oa_fastswitch(input, lineoff));
	    break;
	case RXTYPE_SNAP_NOV1:
	    lineoff += SNAP_HDRBYTES >> 1;
	    break;
	default:
	    return(FALSE);
	}
    }

    return(nov_hes_fastswitch(input, lineoff));
}


/*
 * lex_nov_fastswitch1
 * Fast switching of lex encapsulated packets.
 * input encapsulation is ARPA
 */
static boolean lex_nov_fastswitch1 (hwidbtype *input)
{
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*lnfARPA:");
#endif

    input->rxtype = RXTYPE_NOVELL1;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_arpa_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_ARPA);

    return(nov_hes_fastswitch(input, LEX_ARPA_OFFSET));
}

/*
 * lex_nov_fastswitch2
 * Fast switching of lex encapsulated packets. 
 */
static boolean lex_nov_fastswitch2 (hwidbtype *input)
{
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*lnfNE:");
#endif

    input->rxtype = RXTYPE_NOVELL2;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_ether_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_NOVELL_ETHER);

    return(nov_hes_fastswitch(input, LEX_ARPA_OFFSET));
}

/*
 * Same interface. Lay new encapsulation over old in receive buffer.
 * Input is ISO1. Note that the data packet is odd-aligned.
 */
static boolean lex_nov_fastswitch_sap(hwidbtype *input)
{
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*lnfSAP:");
#endif

    input->rxtype = RXTYPE_SAP_NOVELL;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_iso1_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_SAP);

    return(nov_hes_oa_fastswitch(input, LEX_ISO1_OFFSET));
}

/*
 * Same interface. Lay new encapsulation over old in receive buffer.
 * Input encap is ISO2 (SNAP).
 */

static boolean lex_nov_fastswitch_snap(hwidbtype *input)
{
    idbtype *swinput;

#ifdef DEBUG
    if (novell_debug) buginf("\n*lnfSNAP:");
#endif

    input->rxtype = RXTYPE_SNAP_NOV1;
    /*
     * Bail if input encap not allowed on this interface:
     */
    if (!input->novell_iso2_ok)
	return(FALSE);

    if (input->novell_input_acl_checks)
	SET_FAST_RCVIDB(ET_SNAP);

    return(nov_hes_fastswitch(input, LEX_ISO2_OFFSET));
}


static void nov_mci_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring IPX
     */
    if (!(flags & (FSF_NOVELL1 | FSF_NOVELL2)))
	return;

    if (idb->novell_fastokay) {
	reg_add_ether_fs(RXTYPE_NOVELL1,
			 (service_ether_fs_type)ether_nov_fastswitch,
			 "ether_nov_fastswitch");
	reg_add_ether_fs(RXTYPE_NOVELL2,
			 (service_ether_fs_type)ether_nov_fastswitch0,
			 "ether_nov_fastswitch0");
        reg_add_ether_fs(RXTYPE_SNAP_NOV1,
			 (service_ether_fs_type)ether_nov_fastswitch2,
			 "ether_nov_fastswitch2");
        reg_add_ether_fs(RXTYPE_SAP_NOVELL,
			 (service_ether_fs_type)ether_nov_fastswitch1,
			 "ether_nov_fastswitch1");
        reg_add_hdlc_fs(RXTYPE_NOVELL1,
			(service_hdlc_fs_type)serial_nov_fastswitch,
			"serial_nov_fastswitch");
        reg_add_hdlc_fs(RXTYPE_NOVELL2,
			(service_hdlc_fs_type)serial_nov_fastswitch,
			"serial_nov_fastswitch");
	reg_add_ppp_fs(RXTYPE_NOVELL1,
		       (service_hdlc_fs_type)serial_nov_fastswitch,
		       "serial_nov_fastswitch");
	reg_add_smds_fs(RXTYPE_SNAP_NOV1,
			(service_smds_fs_type)serial_nov_fastswitch,
			"serial_nov_fastswitch");
	reg_add_smds_fs(RXTYPE_NOVELL1,
			(service_smds_fs_type)serial_nov_fastswitch,
			"serial_nov_fastswitch");
	reg_add_smds_fs(RXTYPE_NOVELL2,
			(service_smds_fs_type)serial_nov_fastswitch,
			"serial_nov_fastswitch");
        reg_add_framerelay_fs(RXTYPE_NOVELL1,
			      (service_framerelay_fs_type)
			      serial_nov_fastswitch, "serial_nov_fastswitch");
        reg_add_framerelay_fs(RXTYPE_NOVELL2,
			      (service_framerelay_fs_type)
			      serial_nov_fastswitch, "serial_nov_fastswitch");
        reg_add_framerelay_fs(RXTYPE_SNAP_NOV1,
			      (service_framerelay_fs_type)frame_nov_fastswitch,
			      "atm_dxi_nov_fastswitch");
        reg_add_atm_dxi_fs(RXTYPE_NOVELL1,
			   (service_atm_dxi_fs_type)atm_dxi_nov_fastswitch,
			   "serial_nov_fastswitch");
        reg_add_atm_dxi_fs(RXTYPE_NOVELL2,
			   (service_atm_dxi_fs_type)atm_dxi_nov_fastswitch,
			   "atm_dxi_nov_fastswitch");
        reg_add_atm_dxi_fs(RXTYPE_SNAP_NOV1,
			   (service_atm_dxi_fs_type)atm_dxi_nov_fastswitch,
			   "atm_dxi_nov_fastswitch");
	reg_add_fddi_fs(RXTYPE_NOVELL1,
			(service_fddi_fs_type)fddi_nov_fastswitch1,
			"fddi_nov_fastswitch1");
	reg_add_fddi_fs(RXTYPE_SAP_NOVELL,
			(service_fddi_fs_type)fddi_nov_fastswitch1,
			"fddi_nov_fastswitch1");
        reg_add_fddi_fs(RXTYPE_SNAP_NOV1,
			(service_fddi_fs_type)fddi_nov_fastswitch2,
			"fddi_nov_fastswitch2");
        reg_add_fddi_fs(RXTYPE_NOVELL2,
			(service_fddi_fs_type)fddi_nov_fastswitch_raw,
			"fddi_nov_fastswitch_raw");
#ifdef DEBUG
        reg_add_fddi_fs(RXTYPE_UNKNOWN,
			(service_fddi_fs_type)fddi_nov_fastswitch_unk,
			"fddi_nov_fastswitch_unk");
#endif
        reg_add_token_fs(RXTYPE_SNAP_NOV1,
			 (service_token_fs_type)token_nov_fastswitch2,
			 "token_nov_fastswitch2");
        reg_add_token_fs(RXTYPE_NOVELL1,
			 (service_token_fs_type)token_nov_fastswitch1,
			 "token_nov_fastswitch1");
        reg_add_aip_fs(RXTYPE_NOVELL1,
		       (service_aip_fs_type)aip_nov_fastswitch,
		       "aip_nov_fastswitch");
        reg_add_aip_fs(RXTYPE_NOVELL2,
		       (service_aip_fs_type)aip_nov_fastswitch,
		       "aip_nov_fastswitch");
        reg_add_aip_fs(RXTYPE_SNAP_NOV1,
			(service_aip_fs_type)aip_nov_fastswitch,
			"aip_nov_fastswitch");
        reg_add_aip_fs(RXTYPE_SAP_NOVELL,
		       (service_aip_fs_type)aip_nov_fastswitch,
		       "aip_nov_fastswitch_sap");
	reg_add_lex_fs(RXTYPE_NOVELL1,
		       (service_lex_fs_type)lex_nov_fastswitch1,
		       "lex_nov_fastswitch1");
        reg_add_lex_fs(RXTYPE_NOVELL2,
		       (service_lex_fs_type)lex_nov_fastswitch2,
		       "lex_nov_fastswitch2");
	reg_add_lex_fs(RXTYPE_SAP_NOVELL,
		       (service_lex_fs_type)lex_nov_fastswitch_sap,
		       "lex_nov_fastswitch_sap");
        reg_add_lex_fs(RXTYPE_SNAP_NOV1,
		       (service_lex_fs_type)lex_nov_fastswitch_snap,
		       "lex_nov_fastswitch_snap");
    } else {
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_NOVELL1);
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_NOVELL2);
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_SNAP_NOV1);
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_SAP_NOVELL);
    }
}

static void nov_mci_fs_init (subsystype *subsys)
{
    reg_add_register_drivers(nov_mci_fs_register_drivers,
			     "nov_mci_fs_register_drivers");
    reg_add_fast_supported(ipx_fast_supported, "ipx_fast_supported");
}

/*
 * Novell MCI FS subsystem header
 */

#define NOVMCIFS_MAJVERSION 1
#define NOVMCIFS_MINVERSION 0
#define NOVMCIFS_EDITVERSION  1

SUBSYS_HEADER(nov_mci_fs, NOVMCIFS_MAJVERSION, NOVMCIFS_MINVERSION, NOVMCIFS_EDITVERSION,
	      nov_mci_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: ipx",
	      "req: fastswitch, ipx");

