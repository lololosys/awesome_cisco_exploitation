/* $Id: xnsfast_mci.c,v 3.4.12.6 1996/08/14 14:12:24 dblair Exp $
 * $Source: /release/112/cvs/Xsys/xns/xnsfast_mci.c,v $
 *------------------------------------------------------------------
 * xnsfast_mci.c -- XNS Fast Switching module providing MCI/CBUS specific
 *                  Support.
 *
 * 29-April-1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xnsfast_mci.c,v $
 * Revision 3.4.12.6  1996/08/14  14:12:24  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * add multilink half-fastswitching support
 *
 * Revision 3.4.12.5  1996/08/14  02:38:11  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.4.12.4  1996/08/13  14:41:23  rbadri
 * CSCdi64089:  Multiprotocol support fixes required
 * Branch: California_branch
 * XNS support for LANE
 *
 * Revision 3.4.12.3  1996/05/17  12:18:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.3  1996/05/06  00:13:28  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.4.2.2  1996/05/02  22:09:04  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.1  1996/04/03  22:23:04  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.12.2  1996/04/27  06:48:13  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.20.3  1996/04/26  00:13:48  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.2.20.2  1996/04/17  00:11:24  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.2.20.1  1996/02/23  21:33:48  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.4.12.1  1996/03/18  22:54:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  02:15:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  22:02:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  19:44:44  hampton
 * Reorder routines to remove platform specific extern statements in
 * fastswitching code.  [CSCdi48844]
 *
 * Revision 3.3  1996/02/13  08:18:10  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.2  1995/11/17  19:25:52  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:40:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/10  05:33:15  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.1  1995/06/07  23:31:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines Exported from this module
 * xns_fastswitch:	Fast switches xns packests(called from if_mci.c)
 *
 * The above routines are called through a switch table "fast_switch"
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
#include "../wan/serial.h"
#include "../hes/if_mci.h"
#include "../if/fddi.h"
#include "../hes/if_fci.h"
#include "../if/network.h"
#include "xns.h"
#include "xnsfast.h"
#include "../wan/dialer.h"
#include "../if/atm.h"
#include "../lane/lane.h"


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
#define SOURCE_NET_OFFSET 	9	/* 9 words */
#define ISO2_OVERHEAD   	3	/* 3 words */
#define XNS_DNET_OFFSET         3       /* 3 words */
#define XNS_OFFSETW_DHOST	5	/* 5 words */

typedef struct shortlong_ {
    union {
	ulong lword;
	ushort sword[2];
    } d;
} shortlong;

extern ushort xns_fqc(int, int);


/*
 * Small inline functions -- to get code where we want it.
 *
 * NOTE: Do not confuse the order/complexity of C language
 * constructs with the underlying efficiency/inefficiency of
 * generated code. !!!!!
 */

inline static void xns_write_ether_header (
    mciregtype *inreg,
    xnspathtype *path,
    long temp_length)
{
    
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    inreg->write1long = path->xnsp_mh.mac_longs[0];
    inreg->write1long = path->xnsp_mh.mac_longs[1];
    inreg->write1long = path->xnsp_mh.mac_longs[2];
    inreg->write1short = path->xnsp_mh.mac_shorts[6];
    inreg->argreg = temp_length;
    inreg->cmdreg = MCI_CMD_TX1_START;
}


/*
 * Writes FDDI Mac header.
 */

inline static void xns_write_fddi_header (
    mciregtype *inreg,
    xnspathtype *path,
    long temp_length)
{
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    inreg->write1short = FDDI_LLC_FC_BYTE;
    inreg->write1long = path->xnsp_mh.mac_longs[0];
    inreg->write1long = path->xnsp_mh.mac_longs[1];
    inreg->write1long = path->xnsp_mh.mac_longs[2];
    inreg->write1long = 0xaaaa0300;
    inreg->write1long = TYPE_XNS;
    inreg->argreg = temp_length | MCI_TX_ODDALIGN;
    inreg->cmdreg = MCI_CMD_TX1_START;
}


/*
 * Writes Token Ring Mac header.  Assumes even alignment of the
 * original frame.
 */
inline static void xns_write_token_header (
    mciregtype *inreg,
    xnspathtype *path,
    long temp_length)
{
    int i;

    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    inreg->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;
    for (i = 0; i < path->length/2; i++)
	inreg->write1short = path->xnsp_mh.mac_shorts[i];
    inreg->argreg = temp_length;
    inreg->cmdreg = MCI_CMD_TX1_START;
}

/*
 * Inline function to write the ATM-LANE header
 * VCD+Encap+LEC-ID (3 words) + ETHER_ARPA_ENCAPS
 */
inline static void xns_write_atm_lane_header (
    mciregtype *inreg,
    xnspathtype *path,
    long temp_length)
{
    /* Account for the additional encaps that you are using */
    temp_length +=  ATM_AAL5_LANE_ENCAPBYTES;		    

    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    inreg->write1long = path->xnsp_mh.mac_longs[0];
    inreg->write1long = path->xnsp_mh.mac_longs[1];
    inreg->write1long = path->xnsp_mh.mac_longs[2];
    inreg->write1long = path->xnsp_mh.mac_longs[3];
    inreg->write1long = path->xnsp_mh.mac_longs[4];
    inreg->argreg = temp_length;
    inreg->cmdreg = MCI_CMD_TX1_START;
}

/*
 * Writes serial header - could use HDLC_XNSCODE, but might want
 * to fast switch ppp, frame relay in the future
 */
inline static void xns_write_serial_header (
    mciregtype *inreg,
    xnspathtype *path,
    long temp_length,
    hwidbtype *output)
{
			      
    if (output->encsize == HDLC_ENCAPBYTES)	
	inreg->write1long = path->xnsp_mh.mac_longs[0];
    else {
	inreg->write1long = path->xnsp_mh.mac_longs[0];
	inreg->write1short = path->xnsp_mh.mac_shorts[2];
    }
    inreg->argreg = temp_length;
    inreg->cmdreg = MCI_CMD_TX1_START;
}


/*
 * xns_fast_supported
 *
 * Determine whether fastswitch support present for given hw/config
 *
 */
static boolean xns_fast_supported (int linktype, hwidbtype *hwidb)
{
    if (!hwidb || (linktype != LINK_XNS))
	return(FALSE);
    /*
     * Old MCI with "tx1bug" is never supported:
     */
    if (hwidb->status & IDB_MCI)
	if (!mci_supports_fastswitching(hwidb))
	    return(FALSE);

    if (hwidb->status & (IDB_MCI | IDB_CBUS)) {

	/*
	 * Ethernet:
	 */
	if ((hwidb->status & IDB_ETHER) && (hwidb->enctype == ET_ARPA)) {
	    return(TRUE);
	}
	/*
	 * Fddi:
	 */
	if ((hwidb->status & IDB_FDDI) && (hwidb->enctype == ET_SNAP)) {
	    return(TRUE);
	}
	/*
	 * Tokenring: not currently supported
	 */
	if (hwidb->status & IDB_TR) {
	    return(FALSE);
	}
	/*
	 * Serial: if encap is HDLC and DDR is not configured
	 */
	if (hwidb->status & IDB_SERIAL) {
	    if ((hwidb->enctype == ET_HDLC) && !is_ddr(hwidb) &&
		!is_multilink(hwidb)) {
		return(TRUE);
	    }
	}
	/*
	 * ATM
	 */
	if (hwidb->status & IDB_ATM) {
	    return(TRUE);
	}


    }

    return(FALSE);
}

/*
 * xns_mci2mci
 * Fast switch XNS packets between two different MCI controllers.
 * Called from xns_fastswitch().
 */

static boolean xns_mci2mci (
    hwidbtype *input,
    xnspathtype *path,
    long length,
    ushort checksum)
{	
    hwidbtype *output;
    mciregtype *inreg, *outreg;
    ushort temp, bytes;
    int i;

    /*
     * Set up some common variables
     */
    output = path->xnsp_idb_ptr->hwptr;
    outreg = output->mci_regptr;
    inreg = input->mci_regptr;
    bytes = input->rxlength;
    
    /* 
     * Select the destination interface 
     */
    outreg->argreg = output->mci_index;
    outreg->cmdreg = MCI_CMD_SELECT;
    
    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    outreg->argreg = output->buffer_pool;
    outreg->cmdreg = MCI_CMD_TX1_RESERVE;
    input->counters.inputs_fast[ACCT_PROTO_XNS]++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_XNS] += bytes;
    if ((temp = outreg->cmdreg) == MCI_RSP_OKAY) {
	
	/*
	 * Set up write pointer to base of destination transmit window.
	 * Do some input accounting.  Figure out how to write new MAC header.
	 * Then adjust the read pointer of the receiver window to point after
	 * Packet Type in XNS data. Compute number of bytes to be copied
         * (in temp) and to be sent (in bytes).
	 */
	outreg->argreg = 0;
	outreg->cmdreg = MCI_CMD_TX1_SELECT;
	output->counters.outputs_fast[ACCT_PROTO_XNS]++;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_XNS] += bytes;
	xns_stat[XNS_INPUT]++;
	xns_stat[XNS_FORWARDED]++;
	temp = bytes;
  	length += 256;   /* adding 1 to hop count */
	if (output->status & IDB_ETHER) {
	    
	    /*
	     * Output is Ethernet.
	     */
	    if (input->status & IDB_SERIAL) {
		
		/*
		 * Output ethernet -- input serial, adjust receive offset,
		 * number of bytes to copy (temp) and size (bytes) of output
		 * datagram.
		 */
		inreg->argreg = MCI_SERIAL_OFFSET + ((HDLC_ENCAPBYTES + 
						      XNS_OFFSETB_DNET) >> 1);
		temp -= (HDLC_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes += (ETHER_ARPA_ENCAPBYTES - HDLC_ENCAPBYTES);
		if (bytes < MINETHERSIZE)
		    bytes = MINETHERSIZE;
	    } else if (input->status & IDB_FDDI) {
		
		/*
		 * Output ethernet -- input fddi, set receive offset, 
		 * adjust number of bytes to copy (temp) and size (bytes) 
		 * of output datagram.
		 */
		inreg->argreg = (FDDI_RECEIVE_OFFSET +  
				 (((FDDI_SNAP_ENCAPBYTES + 
				    XNS_OFFSETB_DNET) >> 1) + 1));
		temp -= (FDDI_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes -= (FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);
		if (bytes < MINETHERSIZE)
		    bytes = MINETHERSIZE;
	    } else if (input->status & IDB_TR) {
		
		/*
		 * Output ethernet -- input token ring, set receive offset, 
		 * adjust number of bytes to copy (temp) and size (bytes) 
		 * of output datagram.
		 */
		inreg->argreg = (CTR_RECEIVE_OFFSET +  
				 (TRING_SNAP_ENCAPBYTES >> 1) +
				 input->rxrifwordlen +
				 (XNS_OFFSETB_DNET >> 1));
		temp -= (TRING_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes += (ETHER_ARPA_ENCAPBYTES - TRING_SNAP_ENCAPBYTES - 
			  input->rxrifwordlen*2 + path->extra_bytes);
		if (bytes < MINETHERSIZE)
		    bytes = MINETHERSIZE;
	    } else {
		
		/*
		 * Output ethernet -- input ethernet, adjust receive offset 
		 * and number of bytes to copy (temp).
		 */
		inreg->argreg = MCI_ETHER_OFFSET + ((ETHER_ARPA_ENCAPBYTES + 
						     XNS_OFFSETB_DNET) >> 1);
		temp -= (ETHER_ARPA_ENCAPBYTES + XNS_OFFSETB_DNET);
	    }
	    inreg->cmdreg = MCI_CMD_RX_SELECT;	
	    outreg->write1long = path->xnsp_mh.mac_longs[0];
	    outreg->write1long = path->xnsp_mh.mac_longs[1];
	    outreg->write1long = path->xnsp_mh.mac_longs[2];
	    outreg->write1short = path->xnsp_mh.mac_shorts[6];
	    outreg->write1short = checksum;
	    outreg->write1long =  length;	  /* len, hop count */
	} else if (output->status & IDB_FDDI) {
	    
	    /*
	     * Output is fddi.
	     */
	    if (input->status & IDB_ETHER) {
		
		/* 
		 * Input ethernet -- output fddi, adjust receive offset,
		 *  number of bytes to copy (temp), and size (bytes)
		 * of output datagram.
		 */
		inreg->argreg = MCI_ETHER_OFFSET + ((ETHER_ARPA_ENCAPBYTES + 
						     XNS_OFFSETB_DNET) >> 1);
		temp -= (ETHER_ARPA_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes += (FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);
	    } else if (input->status & IDB_FDDI) {
		
		/*
		 * Input fddi -- output fddi, adjust receive offset
		 * and the number of bytes to copy (temp).
		 */
		inreg->argreg = FDDI_RECEIVE_OFFSET + 
		    (((FDDI_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET) >> 1)  + 1);
		temp -= (FDDI_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET);
	    } else if (input->status & IDB_TR) {
		
		/*
		 * Output fddi -- input token ring, set receive offset, 
		 * adjust number of bytes to copy (temp) and size (bytes) 
		 * of output datagram.
		 */
		inreg->argreg = (CTR_RECEIVE_OFFSET +  
				 (TRING_SNAP_ENCAPBYTES >> 1) +
				 input->rxrifwordlen +
				 (XNS_OFFSETB_DNET >> 1));
		temp -= (TRING_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes += (FDDI_SNAP_ENCAPBYTES - TRING_SNAP_ENCAPBYTES - 
			  input->rxrifwordlen*2);
	    } else {
		
		/*
		 * Input serial -- output fddi, adjust recieve offset,
		 * number of bytes to copy (temp) and size (bytes) of
		 * output datgram.
		 */
		inreg->argreg = MCI_SERIAL_OFFSET +
		    ((HDLC_ENCAPBYTES + XNS_OFFSETB_DNET) >> 1);
		temp -= (HDLC_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes += (FDDI_SNAP_ENCAPBYTES - HDLC_ENCAPBYTES);
	    }
	    bytes |= MCI_TX_ODDALIGN;
	    inreg->cmdreg = MCI_CMD_RX_SELECT;
	    
	    /*
	     * Write the FDDI SNAP encapsulation header.
	     */
	    outreg->write1short = FDDI_LLC_FC_BYTE;
	    outreg->write1long = path->xnsp_mh.mac_longs[0];
	    outreg->write1long = path->xnsp_mh.mac_longs[1];
	    outreg->write1long = path->xnsp_mh.mac_longs[2];
	    outreg->write1long = path->xnsp_mh.mac_longs[3];
	    outreg->write1long = path->xnsp_mh.mac_longs[4];
	    outreg->write1short = checksum;
	    outreg->write1long =  length;
	    
	} else if (output->status & IDB_TR) {
	    
	    /*
	     * Output is token ring.
	     */
	    if (input->status & IDB_ETHER) {
		
		/*
		 * Input ethernet -- output token ring, adjust receive
		 * offset, number of bytes to copy (temp), and size
		 * (bytes) of output datagram.
		 */
		inreg->argreg = MCI_ETHER_OFFSET + ((ETHER_ARPA_ENCAPBYTES + 
						     XNS_OFFSETB_DNET) >> 1);
		temp -= (ETHER_ARPA_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes += (path->length+2 - ETHER_ARPA_ENCAPBYTES);
	    } else if (input->status & IDB_FDDI) {
		
		/*
		 * Input fddi -- output token ring, adjust receive
		 * offset, number of bytes to copy (temp), and size
		 * (bytes) of output datagram.
		 */
		inreg->argreg = FDDI_RECEIVE_OFFSET + 
		    (((FDDI_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET) >> 1)  + 1);
		temp -= (FDDI_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes += (path->length+2 - FDDI_SNAP_ENCAPBYTES);
	    } else if (input->status & IDB_TR) {
		
		/*
		 * Input token ring -- output token ring, set receive
		 * offset, adjust number of bytes to copy (temp), and
		 * size (bytes) of output datagram.
		 */
		inreg->argreg = (CTR_RECEIVE_OFFSET +  
				 (TRING_SNAP_ENCAPBYTES >> 1) +
				 input->rxrifwordlen +
				 (XNS_OFFSETB_DNET >> 1));
		temp -= (TRING_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes += (path->length+2 - TRING_SNAP_ENCAPBYTES - 
			  input->rxrifwordlen*2);
	    } else {
		
		/*
		 * Input serial -- output token ring, adjust recieve
		 * offset, number of bytes to copy (temp), and size
		 * (bytes) of output datgram.
		 */
		inreg->argreg = MCI_SERIAL_OFFSET +
		    ((HDLC_ENCAPBYTES + XNS_OFFSETB_DNET) >> 1);
		temp -= (HDLC_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes += (path->length+2 - HDLC_ENCAPBYTES);
	    }
	    inreg->cmdreg = MCI_CMD_RX_SELECT;
	    
	    /*
	     * Write the Token Ring SNAP encapsulation header.
	     */
	    outreg->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;
	    outreg->write1long = path->xnsp_mh.mac_longs[0];
	    outreg->write1long = path->xnsp_mh.mac_longs[1];
	    outreg->write1long = path->xnsp_mh.mac_longs[2];
	    for (i = 6; i < path->length/2; i++)
		outreg->write1short = path->xnsp_mh.mac_shorts[i];
	    outreg->write1short = checksum;
	    outreg->write1long =  length;
	    
	} else {
	    
 	    /*
	     * Output is HDLC.  Go ahead and write the serial
	     * encapsulation word (0F000600).
	     */
	    outreg->write1long = HDLC_XNSCODE;
	    outreg->write1short = checksum;
	    outreg->write1long =  length;

	    if (input->status & IDB_ETHER) {
		/*
		 * Input ethernet -- output serial.  Select receive
		 * offset, adjust number of bytes to copy (temp) and size
		 * (bytes) of output datgram.
		 */
		inreg->argreg = MCI_ETHER_OFFSET + ((ETHER_ARPA_ENCAPBYTES + 
						     XNS_OFFSETB_DNET) >> 1);
		temp -= (ETHER_ARPA_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes -= (ETHER_ARPA_ENCAPBYTES - HDLC_ENCAPBYTES);
	    } else if (input->status & IDB_FDDI) {
		/*
		 * Input fddi -- output serial.  Select receive offset,
		 * adjust number of bytes to copy (temp) and size (bytes)
		 * of output datgram.
		 */
		inreg->argreg = FDDI_RECEIVE_OFFSET + 
		    (((FDDI_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET) >> 1) + 1);
		temp -= (FDDI_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes -= (FDDI_SNAP_ENCAPBYTES - HDLC_ENCAPBYTES);
	    } else if (input->status & IDB_TR) {
		/*
		 * Output serial -- input token ring, set receive offset, 
		 * adjust number of bytes to copy (temp) and size (bytes) 
		 * of output datagram.
		 */
		inreg->argreg = (CTR_RECEIVE_OFFSET +  
				 (TRING_SNAP_ENCAPBYTES >> 1) +
				 input->rxrifwordlen +
				 (XNS_OFFSETB_DNET >> 1));
		temp -= (TRING_SNAP_ENCAPBYTES + XNS_OFFSETB_DNET);
		bytes += (HDLC_ENCAPBYTES - TRING_SNAP_ENCAPBYTES - 
			  input->rxrifwordlen*2);
	    } else {
		/*
		 * Input serial -- output serial, no adjustment needed.
		 */
		inreg->argreg = MCI_SERIAL_OFFSET + ((HDLC_ENCAPBYTES + 
						      XNS_OFFSETB_DNET) >> 1);
		temp -= (HDLC_ENCAPBYTES + XNS_OFFSETB_DNET);
	    }
	    inreg->cmdreg = MCI_CMD_RX_SELECT;
	}
	
	/*
	 * Copy bulk of datagram from one MCI to another.
	 * Transmit modified copy and flush original in receive buffer.
	 */
	mci2mci(&inreg->readlong, &outreg->write1long, temp);
	inreg->cmdreg = MCI_CMD_RX_FLUSH;
	outreg->argreg = bytes;
	outreg->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);
    } else if (output->priority_list) {
	/*
	 * if output is congested and we are sorting traffic, 
	 * defer to process path
	 */
	return(FALSE);
    } else {
	
	/*
	 * Reserve failed on output -- flush the packet.
	 */	
	inreg->cmdreg = MCI_CMD_RX_FLUSH;	
	output->counters.output_total_drops++;
	return(TRUE);
    }
}

/*
 * xns_access_check_mci
 * Determine if we are allowed to forward the passed datagram, based on the
 * XNS access pointer. Return FALSE if output access is denied.
 */
static boolean xns_access_check_mci (
    hwidbtype *input,
    ulong list,
    ushort proto)
{
    mciregtype	*regptr;
    ulong	flags;
    shortlong	tm_acc[2];
    xnseacctype *item;
    ulong	srcnet, dstnet, dsthost, srchost, pep_transaction_id;
    charlong 	misaligned[7];
    ushort	pep_client_type;
    xnssb	*xsb;

    regptr = input->mci_regptr;
    item = xns_get_ext_acl(list);
    if (input->status & IDB_FDDI) {
	regptr->argreg = MCI_OFFSETSIZE + XNS_DNET_OFFSET +
	    ((FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES) >> 1); 
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	dstnet = regptr->readlong;
	dsthost = regptr->readlong; 
	tm_acc[0].d.lword = regptr->readlong;  
	srcnet = regptr->readlong;  
	srchost = regptr->readlong;  
	tm_acc[1].d.lword = regptr->readlong;
	pep_transaction_id = regptr->readlong;
	pep_client_type = regptr->readshort;
    } else if (input->status & IDB_TR) {
	xsb = idb_get_swsb_inline(input->fast_rcvidb, SWIDB_SB_XNS);
	if (xsb->xns_enctype == ET_3COM_TR) {
	    regptr->argreg = CTR_RECEIVE_OFFSET + (TRING_SAP_ENCAPBYTES >> 1) +
		input->rxrifwordlen + XNS_DNET_OFFSET;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    misaligned[0].d.lword = regptr->readlong;
	    misaligned[1].d.lword = regptr->readlong;
	    misaligned[2].d.lword = regptr->readlong;
	    misaligned[3].d.lword = regptr->readlong;
	    misaligned[4].d.lword = regptr->readlong;
	    misaligned[5].d.lword = regptr->readlong;
	    misaligned[6].d.lword = regptr->readlong;
	    dstnet            = *(ulong *)&misaligned[0].d.byte[1];
	    dsthost           = *(ulong *)&misaligned[1].d.byte[1];
	    tm_acc[0].d.lword = *(ulong *)&misaligned[2].d.byte[1];
	    srcnet            = *(ulong *)&misaligned[3].d.byte[1];
	    srchost           = *(ulong *)&misaligned[4].d.byte[1];
	    tm_acc[1].d.lword = *(ulong *)&misaligned[5].d.byte[1];
	    pep_transaction_id = *(ulong *)&misaligned[6].d.byte[1];
	    pep_client_type = *(ushort *)&misaligned[7].d.byte[1];
	} else {
	    regptr->argreg = CTR_RECEIVE_OFFSET + (TRING_SNAP_ENCAPBYTES >> 1) +
		input->rxrifwordlen + XNS_DNET_OFFSET;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    dstnet = regptr->readlong;
	    dsthost = regptr->readlong; 
	    tm_acc[0].d.lword = regptr->readlong;  
	    srcnet = regptr->readlong;  
	    srchost = regptr->readlong;  
	    tm_acc[1].d.lword = regptr->readlong;
	    pep_transaction_id = regptr->readlong;
	    pep_client_type = regptr->readshort;
	}
    } else {
	regptr->argreg = MCI_OFFSETSIZE + XNS_DNET_OFFSET;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	dstnet = regptr->readlong;  
	dsthost = regptr->readlong; 
	tm_acc[0].d.lword = regptr->readlong;  
	srcnet = regptr->readlong;  
	srchost = regptr->readlong;  
	tm_acc[1].d.lword = regptr->readlong;
	pep_transaction_id = regptr->readlong;
	pep_client_type = regptr->readshort;
    }
    do {
	flags = item->flags;
	
       	/*
	 * A network of -1 indicates all XNS networks.
	 * A socket of 0 indicates all XNS sockets.
	 */
	if (flags & XNS_SRCNET) { 
	    if ((item->srcnet != -1L) &&
		(item->srcnet != (srcnet & ~item->srcnetmask)))
		continue;
	}

	if (flags & XNS_DSTNET) { 
	    if ((item->dstnet != -1L) &&
		(item->dstnet != (dstnet & ~item->dstnetmask)))
		continue;
	}

	if (flags & XNS_SRCHOST) {
	    
	    /*
	     * Check for match on source host address
	     */
	    if ((tm_acc[1].d.sword[0] & ~item->srchostmask.u.sword[2]) != 
		item->srchost.u.sword[2])
		continue;
	    if ((srchost & ~item->srchostmask.u.lword[0]) != 
		(item->srchost.u.lword[0]))
		continue;
	}

	if (flags & XNS_DSTHOST) {
	    
	    /*
	     * Check for match on destination host address
	     */
	    if ((tm_acc[0].d.sword[0] & ~item->dsthostmask.u.sword[2]) != 
		item->dsthost.u.sword[2])
		continue;
	    if ((dsthost & ~item->dsthostmask.u.lword[0]) != 
		(item->dsthost.u.lword[0]))
		continue;
	}
	
	/* check if it is extended access list */
	if (list > MAXXNSACC) {
	    if (flags & XNS_PROTOCOL) {
		if (item->protocol != (proto & 0xFF))
		    continue;
		if (flags & XNS_PEPCLIENT) {
		    if ((proto & 0xFF) == XNS_TYPE_PEP) {
			if (item->pep_clienttype != pep_client_type)
			  continue;
		    }
		}
	    }
	    if (flags & XNS_SRCSOCKET) {
		if (item->srcsocket && (item->srcsocket != 
					tm_acc[1].d.sword[1]))
		    continue;
	    }
	    if (flags & XNS_DSTSOCKET) {
		if(item->dstsocket && (item->dstsocket != 
				       tm_acc[0].d.sword[1]))
		    continue;
	    }
	}
	return(item->grant);
    } while ((item = item->next));
    return(FALSE);
}

/* fast_path macro */
#define	     XNS_FAST_PATH		                                    \
                                                                            \
        output->counters.outputs_fast[ACCT_PROTO_XNS]++;		    \
	output->counters.tx_cumbytes_fast[ACCT_PROTO_XNS] += temp_length;   \
  	if (output->status & IDB_ETHER) {                                   \
	    /*                                                              \
	     * Output is ethernet.                                          \
	     */                                                             \
	    if (input_status & IDB_ETHER) {                                 \
  		if (checksum == NULL_CHECKSUM) {                            \
		    inreg->argreg = MCI_ETHER_OFFSET +                      \
			(ETHER_ARPA_ENCAPBYTES >> 1) + 2;  	            \
        	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
               } else {                                                     \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = MCI_ETHER_OFFSET + 			    \
			(ETHER_ARPA_ENCAPBYTES >> 1);       		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		inreg->argreg = MCI_ETHER_OFFSET;                           \
		xns_write_ether_header(inreg, path, temp_length);           \
		return(TRUE);						    \
	    } else if (input_status & IDB_FDDI) {                           \
	        /* Input fddi -- output ethernet.*/                         \
		temp_length -= (FDDI_SNAP_ENCAPBYTES 			    \
				- ETHER_ARPA_ENCAPBYTES);		    \
		if (temp_length < MINETHERSIZE)                             \
		    temp_length = MINETHERSIZE;                             \
                /* UPDATE_CHECKSUM */                                       \
  		if (checksum == NULL_CHECKSUM) {                            \
		    inreg->argreg = FDDI_RECEIVE_OFFSET +                   \
			(FDDI_SNAP_ENCAPBYTES >> 1) + 1 + 2;  	            \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	        } else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = FDDI_RECEIVE_OFFSET +                   \
			(FDDI_SNAP_ENCAPBYTES >> 1) + 1;		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
   		}                                                           \
                inreg->argreg = (MCI_ETHER_OFFSET +                         \
				 ((FDDI_SNAP_ENCAPBYTES -	            \
				   ETHER_ARPA_ENCAPBYTES) >> 1));      	    \
                xns_write_ether_header(inreg, path, temp_length);           \
                return(TRUE);				                    \
	    } else if (input_status & IDB_TR) {                             \
                /* Input token ring -- output ethernet.*/                   \
                temp_length += (ETHER_ARPA_ENCAPBYTES 			    \
				- TRING_SNAP_ENCAPBYTES -    		    \
                                input->rxrifwordlen*2);                     \
                if (temp_length < MINETHERSIZE)                             \
                    temp_length = MINETHERSIZE;                             \
                /* UPDATE_CHECKSUM */                                       \
                if (checksum == NULL_CHECKSUM) {                            \
                    inreg->argreg = (CTR_RECEIVE_OFFSET +                   \
                                     (TRING_SNAP_ENCAPBYTES >> 1) +         \
                                     input->rxrifwordlen + 2);              \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = (ushort)len_hops;                  \
                } else {                                                    \
                    checksum = xns_fqc(len_hops, checksum);                 \
                    inreg->argreg = (CTR_RECEIVE_OFFSET +                   \
                                     (TRING_SNAP_ENCAPBYTES >> 1) +         \
                                     input->rxrifwordlen);                  \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = checksum;                          \
                    inreg->write1long = len_hops;                           \
                }                                                           \
                inreg->argreg = (CTR_RECEIVE_OFFSET +                       \
                                 (TRING_SNAP_ENCAPBYTES >> 1) +             \
                                 input->rxrifwordlen -                      \
                                 (ETHER_ARPA_ENCAPBYTES >> 1));             \
                xns_write_ether_header(inreg, path, temp_length);           \
                return(TRUE);                                               \
            } else if (input_status & IDB_ATM) {                            \
	        /* Input ATM-LANE -- output ethernet.*/                     \
		temp_length -= ATM_AAL5_LANE_ENCAPBYTES;		    \
		if (temp_length < MINETHERSIZE)                             \
		    temp_length = MINETHERSIZE;                             \
                /* UPDATE_CHECKSUM */                                       \
  		if (checksum == NULL_CHECKSUM) {                            \
                /* in line with the rest of code: add 1 word for LEC-ID */  \
		    inreg->argreg = (MCI_OFFSETSIZE +                       \
				     (ETHER_ARPA_ENCAPBYTES >> 1) + 1 + 2); \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	        } else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = (MCI_OFFSETSIZE +                       \
                                     (ETHER_ARPA_ENCAPBYTES >> 1) + 1 );    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
   		}                                                           \
                inreg->argreg = (MCI_OFFSETSIZE + 1);                       \
                xns_write_ether_header(inreg, path, temp_length);           \
                return(TRUE);				                    \
             } else {				                            \
		/* Input serial -- output ethernet. */                      \
		temp_length += ETHER_ARPA_ENCAPBYTES - input->encsize;      \
		if (temp_length < MINETHERSIZE)                             \
		    temp_length = MINETHERSIZE;                             \
		/* UPDATE_CHECKSUM */                                       \
  		if (checksum == NULL_CHECKSUM) {                            \
		    inreg->argreg = MCI_ETHER_OFFSET + 2 +                  \
			(ETHER_ARPA_ENCAPBYTES >> 1); 			    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	       	} else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = MCI_ETHER_OFFSET + 			    \
			(ETHER_ARPA_ENCAPBYTES >> 1);     		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		inreg->argreg = MCI_ETHER_OFFSET;                           \
		xns_write_ether_header(inreg, path, temp_length);           \
                return(TRUE);						    \
            } /* end of output being ETHER */                               \
	} else if (output->status & IDB_FDDI) {                             \
	    /*                                                              \
	     * Output fddi.                                                 \
	     */                                                             \
	    if (input_status & IDB_ETHER) {                                 \
		/*                                                          \
		 * Input ethernet -- output fddi.                           \
		 */                                                         \
		temp_length += (FDDI_SNAP_ENCAPBYTES			    \
				- ETHER_ARPA_ENCAPBYTES);      		    \
        	/* UPDATE_CHECKSUM */                                       \
  		if (checksum  == NULL_CHECKSUM) {                           \
		    inreg->argreg = MCI_ETHER_OFFSET + 2 +                  \
			(ETHER_ARPA_ENCAPBYTES >> 1);  			    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	       	} else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = MCI_ETHER_OFFSET +                      \
			(ETHER_ARPA_ENCAPBYTES >> 1);  			    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		inreg->argreg = (MCI_ETHER_OFFSET -                         \
				 (((FDDI_SNAP_ENCAPBYTES -            	    \
				    ETHER_ARPA_ENCAPBYTES) >> 1) + 1));     \
		xns_write_fddi_header(inreg, path, temp_length);            \
                return(TRUE);						    \
	    } else if (input_status & IDB_FDDI) {	                    \
		/*                                                          \
		 * Input fddi -- output fddi.                               \
		 */                                                         \
		/* UPDATE_CHECKSUM */                                       \
  		if (checksum  == NULL_CHECKSUM) {                           \
		    inreg->argreg = FDDI_RECEIVE_OFFSET + 2 +               \
			(FDDI_SNAP_ENCAPBYTES >> 1) + 1;		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	       	} else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = FDDI_RECEIVE_OFFSET +                   \
			(FDDI_SNAP_ENCAPBYTES >> 1) + 1; 		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		inreg->argreg = FDDI_RECEIVE_OFFSET;                        \
		xns_write_fddi_header(inreg, path, temp_length);            \
                return(TRUE);						    \
            } else if (input_status & IDB_TR) {                             \
                /*                                                          \
                 * Input token ring -- output fddi.                         \
                 */                                                         \
                temp_length += (FDDI_SNAP_ENCAPBYTES 			    \
				- TRING_SNAP_ENCAPBYTES - 		    \
                                input->rxrifwordlen*2);                     \
                /* UPDATE_CHECKSUM */                                       \
                if (checksum  == NULL_CHECKSUM) {                           \
                    inreg->argreg = (CTR_RECEIVE_OFFSET +                   \
                                     (TRING_SNAP_ENCAPBYTES >> 1) +         \
                                     input->rxrifwordlen + 2);              \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = (ushort)len_hops;                  \
                } else {                                                    \
                    checksum = xns_fqc(len_hops, checksum);                 \
                    inreg->argreg = (CTR_RECEIVE_OFFSET +                   \
                                     (TRING_SNAP_ENCAPBYTES >> 1) +         \
                                     input->rxrifwordlen);                  \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = checksum;                          \
                    inreg->write1long = len_hops;                           \
                }                                                           \
                inreg->argreg = (CTR_RECEIVE_OFFSET +                       \
                                 (TRING_SNAP_ENCAPBYTES >> 1) +             \
                                 input->rxrifwordlen -                      \
                                 FDDI_SNAP_ENCAPWORDS);                     \
                xns_write_fddi_header(inreg, path, temp_length);            \
                return(TRUE);                                               \
	    } else if (input_status & IDB_ATM) {                            \
		/*                                                          \
		 * Input ATM-LANE -- output fddi.                           \
		 */                                                         \
		temp_length += (FDDI_SNAP_ENCAPBYTES -			    \
				ETHER_ARPA_ENCAPBYTES -                     \
				ATM_AAL5_LANE_ENCAPBYTES);     		    \
        	/* UPDATE_CHECKSUM */                                       \
  		if (checksum  == NULL_CHECKSUM) {                           \
		    inreg->argreg = (MCI_OFFSETSIZE + 2 + 1 +               \
				     (ETHER_ARPA_ENCAPBYTES >> 1)); 	    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	       	} else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = (MCI_OFFSETSIZE + 1 +    	            \
				     (ETHER_ARPA_ENCAPBYTES >> 1)); 	    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
                /* 1 word  for LEC-ID and the other for FC */     	    \
		inreg->argreg = (MCI_OFFSETSIZE + 1 -                       \
				 (((FDDI_SNAP_ENCAPBYTES -            	    \
				    ETHER_ARPA_ENCAPBYTES) >> 1) + 1));     \
		xns_write_fddi_header(inreg, path, temp_length);            \
                return(TRUE);						    \
            } else  {					                    \
		/*                                                          \
		 * Input serial -- output fddi.                             \
		 */                                                         \
		temp_length += (FDDI_SNAP_ENCAPBYTES - input->encsize);     \
		/* UPDATE_CHECKSUM */                                       \
  		if (checksum == NULL_CHECKSUM) {                            \
		    inreg->argreg = MCI_SERIAL_OFFSET + 2 +                 \
			(HDLC_ENCAPBYTES >> 1);			            \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	       	} else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = MCI_SERIAL_OFFSET +                     \
			(HDLC_ENCAPBYTES >> 1);    			    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		inreg->argreg = (MCI_SERIAL_OFFSET -                        \
				 (((FDDI_SNAP_ENCAPBYTES -	            \
				    HDLC_ENCAPBYTES) >> 1) + 1)); 	    \
		xns_write_fddi_header(inreg, path, temp_length);            \
                return(TRUE);						    \
	    }                                                               \
        } else if (output->status & IDB_TR) {                               \
            /*                                                              \
             * Output token ring.                                           \
             */                                                             \
            if (input_status & IDB_ETHER) {                                 \
                /*                                                          \
                 * Input ethernet -- output token ring.                     \
                 */                                                         \
                temp_length += (path->length + 2 - ETHER_ARPA_ENCAPBYTES);  \
                /* UPDATE_CHECKSUM */                                       \
                if (checksum  == NULL_CHECKSUM) {                           \
                    inreg->argreg = MCI_ETHER_OFFSET + 2 +                  \
                        (ETHER_ARPA_ENCAPBYTES >> 1);                       \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = (ushort)len_hops;                  \
                } else {                                                    \
                    checksum = xns_fqc(len_hops, checksum);                 \
                    inreg->argreg = MCI_ETHER_OFFSET +                      \
                        (ETHER_ARPA_ENCAPBYTES >> 1);                       \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = checksum;                          \
                    inreg->write1long = len_hops;                           \
                }                                                           \
                inreg->argreg = (MCI_ETHER_OFFSET -                         \
                                 ((path->length+2 -                         \
                                   ETHER_ARPA_ENCAPBYTES) >> 1));           \
                xns_write_token_header(inreg, path, temp_length);           \
                return(TRUE);                                               \
            } else if (input_status & IDB_FDDI) {                           \
                /*                                                          \
                 * Input fddi -- output token ring.                         \
                 */                                                         \
                temp_length += (path->length + 2 - FDDI_SNAP_ENCAPBYTES);   \
                /* UPDATE_CHECKSUM */                                       \
                if (checksum  == NULL_CHECKSUM) {                           \
                    inreg->argreg = FDDI_RECEIVE_OFFSET + 2 +               \
                        (FDDI_SNAP_ENCAPBYTES >> 1) + 1;                    \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = (ushort)len_hops;                  \
                } else {                                                    \
                    checksum = xns_fqc(len_hops, checksum);                 \
                    inreg->argreg = FDDI_RECEIVE_OFFSET +                   \
                        (FDDI_SNAP_ENCAPBYTES >> 1) + 1;                    \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = checksum;                          \
                    inreg->write1long = len_hops;                           \
                }                                                           \
                inreg->argreg = (FDDI_RECEIVE_OFFSET -                      \
                                 ((path->length+2 -                         \
                                   FDDI_SNAP_ENCAPBYTES) >> 1));            \
                xns_write_token_header(inreg, path, temp_length);           \
                return(TRUE);                                               \
            } else if (input_status & IDB_TR) {                             \
                /*                                                          \
                 * Input token ring -- output token ring.                   \
                 */                                                         \
                temp_length += ((2 + path->length) - TRING_SNAP_ENCAPBYTES -\
                                input->rxrifwordlen*2);                     \
                /* UPDATE_CHECKSUM */                                       \
                if (checksum  == NULL_CHECKSUM) {                           \
                    inreg->argreg = (CTR_RECEIVE_OFFSET +                   \
                                     (TRING_SNAP_ENCAPBYTES >> 1) +         \
                                     input->rxrifwordlen + 2);              \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = (ushort)len_hops;                  \
                } else {                                                    \
                    checksum = xns_fqc(len_hops, checksum);                 \
                    inreg->argreg = (CTR_RECEIVE_OFFSET +                   \
                                     (TRING_SNAP_ENCAPBYTES >> 1) +         \
                                     input->rxrifwordlen);                  \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = checksum;                          \
                    inreg->write1long = len_hops;                           \
                }                                                           \
                inreg->argreg = (CTR_RECEIVE_OFFSET +                       \
                                 (TRING_SNAP_ENCAPBYTES >> 1) +             \
                                 input->rxrifwordlen -                      \
                                 ((2+path->length) >> 1));                  \
                xns_write_token_header(inreg, path, temp_length);           \
                return(TRUE);                                               \
            } else if (input_status & IDB_ATM) {                            \
                /*                                                          \
                 * Input ATM-LANE -- output token ring.                     \
                 */                                                         \
                temp_length += (path->length + 2 - ETHER_ARPA_ENCAPBYTES -  \
				ATM_AAL5_LANE_ENCAPBYTES);                  \
                /* UPDATE_CHECKSUM */                                       \
                if (checksum  == NULL_CHECKSUM) {                           \
                    inreg->argreg = (MCI_OFFSETSIZE + 2 + 1 +               \
				     (ETHER_ARPA_ENCAPBYTES >> 1));         \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = (ushort)len_hops;                  \
                } else {                                                    \
                    checksum = xns_fqc(len_hops, checksum);                 \
                    inreg->argreg = (MCI_OFFSETSIZE + 1 +		    \
				     (ETHER_ARPA_ENCAPBYTES >> 1));           \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = checksum;                          \
                    inreg->write1long = len_hops;                           \
                }                                                           \
                inreg->argreg = (MCI_OFFSETSIZE + 1 -                      \
                                 ((path->length+2 -                         \
                                   ETHER_ARPA_ENCAPBYTES) >> 1));           \
                xns_write_token_header(inreg, path, temp_length);           \
                return(TRUE);                                               \
            } else {                                                        \
                /*                                                          \
                 * Input serial -- output token ring.                       \
                 */                                                         \
                temp_length += (path->length + 2 - input->encsize);         \
                /* UPDATE_CHECKSUM */                                       \
                if (checksum == NULL_CHECKSUM) {                            \
                    inreg->argreg = MCI_SERIAL_OFFSET + 2 +                 \
                        (HDLC_ENCAPBYTES >> 1);                             \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = (ushort)len_hops;                  \
                } else {                                                    \
                    checksum = xns_fqc(len_hops, checksum);                 \
                    inreg->argreg = MCI_SERIAL_OFFSET +                     \
                        (HDLC_ENCAPBYTES >> 1);                             \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = checksum;                          \
                    inreg->write1long = len_hops;                           \
                }                                                           \
                inreg->argreg = (MCI_SERIAL_OFFSET -                        \
                                 ((path->length+2 -                         \
                                   HDLC_ENCAPBYTES) >> 1));                 \
                xns_write_token_header(inreg, path, temp_length);           \
                return(TRUE);                                               \
            }                                                               \
	    } else if (output->status & IDB_ATM) {                          \
	    /*                                                              \
	     * Output is ATM-LANE.                                          \
	     */                                                             \
	    if (input_status & IDB_ETHER) {                                 \
  		if (checksum == NULL_CHECKSUM) {                            \
		    inreg->argreg = MCI_ETHER_OFFSET +                      \
			(ETHER_ARPA_ENCAPBYTES >> 1) + 2;  	            \
        	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
               } else {                                                     \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = MCI_ETHER_OFFSET + 			    \
			(ETHER_ARPA_ENCAPBYTES >> 1);       		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		if (temp_length < MINETHERSIZE)                             \
		    temp_length = MINETHERSIZE;                             \
		inreg->argreg = MCI_ETHER_OFFSET - ATM_AAL5_LANE_ENCAPWORDS;\
		xns_write_atm_lane_header(inreg, path, temp_length);        \
		return(TRUE);						    \
	    } else if (input_status & IDB_FDDI) {                           \
	        /* Input fddi -- output ATM-LANE */                         \
		temp_length -= (FDDI_SNAP_ENCAPBYTES - 			    \
				ETHER_ARPA_ENCAPBYTES);		    	    \
		if (temp_length < MINETHERSIZE)                             \
		    temp_length = MINETHERSIZE;                             \
                /* UPDATE_CHECKSUM */                                       \
  		if (checksum == NULL_CHECKSUM) {                            \
		    inreg->argreg = FDDI_RECEIVE_OFFSET +                   \
			(FDDI_SNAP_ENCAPBYTES >> 1) + 1 + 2;  	            \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	        } else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = FDDI_RECEIVE_OFFSET +                   \
			(FDDI_SNAP_ENCAPBYTES >> 1) + 1;		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
   		}                                                           \
                inreg->argreg = (MCI_ETHER_OFFSET +                         \
				 ((FDDI_SNAP_ENCAPBYTES -	            \
				   ETHER_ARPA_ENCAPBYTES) >> 1) -           \
				 ATM_AAL5_LANE_ENCAPWORDS);      	    \
		xns_write_atm_lane_header(inreg, path, temp_length);        \
                return (TRUE);						    \
	    } else if (input_status & IDB_TR) {                             \
                /* Input token ring -- output ATM-LANE.*/                   \
                temp_length += (ETHER_ARPA_ENCAPBYTES 			    \
				- TRING_SNAP_ENCAPBYTES -    		    \
                                input->rxrifwordlen*2);                     \
                if (temp_length < MINETHERSIZE)                             \
                    temp_length = MINETHERSIZE;                             \
                /* UPDATE_CHECKSUM */                                       \
                if (checksum == NULL_CHECKSUM) {                            \
                    inreg->argreg = (CTR_RECEIVE_OFFSET +                   \
                                     (TRING_SNAP_ENCAPBYTES >> 1) +         \
                                     input->rxrifwordlen + 2);              \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = (ushort)len_hops;                  \
                } else {                                                    \
                    checksum = xns_fqc(len_hops, checksum);                 \
                    inreg->argreg = (CTR_RECEIVE_OFFSET +                   \
                                     (TRING_SNAP_ENCAPBYTES >> 1) +         \
                                     input->rxrifwordlen);                  \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = checksum;                          \
                    inreg->write1long = len_hops;                           \
                }                                                           \
                inreg->argreg = (CTR_RECEIVE_OFFSET +                       \
                                 (TRING_SNAP_ENCAPBYTES >> 1) +             \
                                 input->rxrifwordlen -                      \
				 ATM_AAL5_LANE_ENCAPWORDS -                 \
                                 (ETHER_ARPA_ENCAPBYTES >> 1));             \
		xns_write_atm_lane_header(inreg, path, temp_length);        \
                return(TRUE);                                               \
            } else if (input_status & IDB_ATM) {                            \
	        /* Input ATM-LANE -- output ATM-LANE.*/                     \
                temp_length -=  ATM_AAL5_LANE_ENCAPBYTES;		    \
		if (temp_length < MINETHERSIZE)                             \
		    temp_length = MINETHERSIZE;                             \
                /* UPDATE_CHECKSUM */                                       \
  		if (checksum == NULL_CHECKSUM) {                            \
                /* in line with the rest of code: add 1 word for LEC-ID */  \
		    inreg->argreg = (MCI_OFFSETSIZE +                       \
				     (ETHER_ARPA_ENCAPBYTES >> 1) + 1 + 2); \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	        } else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = (MCI_OFFSETSIZE +                       \
                                     (ETHER_ARPA_ENCAPBYTES >> 1) + 1 );    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
   		}                                                           \
                inreg->argreg = MCI_OFFSETSIZE - (ATM_ENCAP_HDRBYTES >> 1); \
		xns_write_atm_lane_header(inreg, path, temp_length);        \
                return(TRUE);				                    \
             } else {				                            \
		/* Input serial -- output ATM-LANE. */                      \
		temp_length += ETHER_ARPA_ENCAPBYTES - input->encsize;      \
		if (temp_length < MINETHERSIZE)                             \
		    temp_length = MINETHERSIZE;                             \
		/* UPDATE_CHECKSUM */                                       \
  		if (checksum == NULL_CHECKSUM) {                            \
		    inreg->argreg = MCI_ETHER_OFFSET + 2 +                  \
			(ETHER_ARPA_ENCAPBYTES >> 1); 			    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	       	} else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = MCI_ETHER_OFFSET + 			    \
			(ETHER_ARPA_ENCAPBYTES >> 1);     		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		inreg->argreg = (MCI_ETHER_OFFSET -                         \
				 ATM_AAL5_LANE_ENCAPWORDS);                 \
		xns_write_atm_lane_header(inreg, path, temp_length);        \
                return(TRUE);						    \
            } /* end of output being ATM-LANE */                            \
 	} else if (output->status & IDB_SERIAL) {                           \
	    /*                                                              \
	     * Output is Serial.                                            \
	     */                                                             \
	    if (input_status & IDB_ETHER) {                                 \
		/*                                                          \
		 * Input ethernet -- output serial.                         \
		 */                                                         \
		/* UPDATE_CHECKSUM */                                       \
  		if (checksum == NULL_CHECKSUM) {                            \
		    inreg->argreg = MCI_ETHER_OFFSET + 2 +                  \
			(ETHER_ARPA_ENCAPBYTES >> 1);	    		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	       	} else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = MCI_ETHER_OFFSET +                      \
			(ETHER_ARPA_ENCAPBYTES >> 1);      		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		inreg->argreg = MCI_OFFSETSIZE - btow(output->encsize);     \
		inreg->cmdreg = MCI_CMD_TX1_SELECT;                         \
		temp_length -= ETHER_ARPA_ENCAPBYTES;                       \
		temp_length += output->encsize;				    \
	        xns_write_serial_header(inreg, path, temp_length, output);  \
		return(TRUE);              			            \
	    } else if (input_status & IDB_FDDI) {                           \
		/*                                                          \
		 * Input fddi -- output serial.                             \
		 */                                                         \
		temp_length -= (FDDI_SNAP_ENCAPBYTES - output->encsize);    \
		/* UPDATE_CHECKSUM */                                       \
  		if (checksum == NULL_CHECKSUM) {                            \
		    inreg->argreg = FDDI_RECEIVE_OFFSET + 2 +               \
			(FDDI_SNAP_ENCAPBYTES >> 1) + 1;                    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	       	} else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = FDDI_RECEIVE_OFFSET +                   \
			(FDDI_SNAP_ENCAPBYTES >> 1) + 1;     		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		inreg->argreg = (FDDI_RECEIVE_OFFSET +                      \
				 (((FDDI_SNAP_ENCAPBYTES -                  \
				    output->encsize) >> 1) + 1));           \
		inreg->cmdreg = MCI_CMD_TX1_SELECT;                         \
	        xns_write_serial_header(inreg, path, temp_length, output);  \
		return(TRUE);                                               \
	    } else if (input_status & IDB_TR) {                             \
                /*                                                          \
                 * Input token ring -- output serial.                       \
                 */                                                         \
                temp_length += (output->encsize - TRING_SNAP_ENCAPBYTES -   \
                                input->rxrifwordlen*2);                     \
                /* UPDATE_CHECKSUM */                                       \
                if (checksum == NULL_CHECKSUM) {                            \
                    inreg->argreg = (CTR_RECEIVE_OFFSET +                   \
                                     (TRING_SNAP_ENCAPBYTES >> 1) +         \
                                     input->rxrifwordlen + 2);              \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = (ushort)len_hops;                  \
                } else {                                                    \
                    checksum = xns_fqc(len_hops, checksum);                 \
                    inreg->argreg = (CTR_RECEIVE_OFFSET +                   \
                                     (TRING_SNAP_ENCAPBYTES >> 1) +         \
                                     input->rxrifwordlen);                  \
                    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
                    inreg->write1short = checksum;                          \
                    inreg->write1long = len_hops;                           \
                }                                                           \
                inreg->argreg = CTR_RECEIVE_OFFSET +                        \
                                (TRING_SNAP_ENCAPBYTES >> 1) +              \
                                input->rxrifwordlen -                       \
                                btow(output->encsize);                      \
                inreg->cmdreg = MCI_CMD_TX1_SELECT;                         \
	        xns_write_serial_header(inreg, path, temp_length, output);  \
                return(TRUE);                                               \
	    } else if (input_status & IDB_ATM) {                            \
		/*                                                          \
		 * Input ATM-LANE -- output serial.                         \
		 */                                                         \
		/* UPDATE_CHECKSUM */                                       \
  		if (checksum == NULL_CHECKSUM) {                            \
		    inreg->argreg = (MCI_OFFSETSIZE + 1 + 2 +               \
				     (ETHER_ARPA_ENCAPBYTES >> 1));	    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	       	} else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
		    inreg->argreg = (MCI_OFFSETSIZE + 1 +                   \
				     (ETHER_ARPA_ENCAPBYTES >> 1)); 	    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		inreg->argreg = (MCI_OFFSETSIZE -  btow(output->encsize) +  \
				 (ETHER_ARPA_ENCAPBYTES >> 1) +1);	    \
		inreg->cmdreg = MCI_CMD_TX1_SELECT;                         \
		temp_length -= (ETHER_ARPA_ENCAPBYTES +			    \
				ATM_AAL5_LANE_ENCAPBYTES - output->encsize);\
	        xns_write_serial_header(inreg, path, temp_length, output);  \
		return(TRUE);              			            \
            } else {					                    \
		/*                                                          \
		 * Input serial -- output serial.                           \
		 */                                                         \
		temp_length -= input->encsize;				    \
		temp_length += output->encsize;				    \
		if (checksum == NULL_CHECKSUM) {                            \
	 	    inreg->argreg = MCI_SERIAL_OFFSET + 2 +                 \
			(HDLC_ENCAPBYTES >> 1); 	      		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
	 	    inreg->write1short = (ushort)len_hops;                  \
  	        } else {                                                    \
	 	    checksum = xns_fqc(len_hops, checksum);                 \
	 	    inreg->argreg = MCI_SERIAL_OFFSET +                     \
			(HDLC_ENCAPBYTES >> 1); 	    		    \
         	    inreg->cmdreg = MCI_CMD_TX1_SELECT;                     \
         	    inreg->write1short = checksum;                          \
	 	    inreg->write1long = len_hops;                           \
  		}                                                           \
		inreg->argreg = MCI_OFFSETSIZE - btow(output->encsize);     \
		inreg->cmdreg = MCI_CMD_TX1_SELECT;                         \
	        xns_write_serial_header(inreg, path, temp_length, output);  \
		return(TRUE);                                               \
	    }                                                               \
        } 

/*
 * xns_fastswitch
 * Very high speed switching of Ether, HDLC or FDDI encapsulated XNS datagrams.
 * This routine has been very heavily optimized for speed, since it is in
 * the inner loop of the code that handles back-to-back datagrams.  Do not
 * add permanent debugging code, subroutines, or much of anything else.
 * The GNU CC compiler will generate the best code.
 * Speedups *must* be debugged with an oscilloscope.
 */
boolean xns_fastswitch (hwidbtype *input)
{
    mciregtype	*inreg;
    xnscachetype *cache;
    xnspathtype	*path;
    hwidbtype	*output;
    idbtype	*swoutput;
    xnssb	*xsb;
    ushort	checksum;
    ushort	temp_length, addrlow;
    shortlong	net, addrhigh;
    vc_info_t *vc;
    ulong	indx, len_hops, input_status;
    
    /*
     * SETUP XNS data read pointer
     */
    inreg = input->mci_regptr;
    if (input->status & IDB_FDDI)
	inreg->argreg = FDDI_RECEIVE_OFFSET +
	    (FDDI_SNAP_ENCAPBYTES >> 1) + 1;
    else if (input->status & IDB_TR)
	inreg->argreg = (CTR_RECEIVE_OFFSET +
	    (TRING_SNAP_ENCAPBYTES >> 1) + input->rxrifwordlen);
    else if (input->status & IDB_ATM) {
	/*
	 * We want to fastswitch XNS only for LANE. Select the network
	 * header by moving past the LEC-ID and ARPA encapsulation.
	 */
	vc = input->atm_db->fast_vc_info; 
	if (is_atm_lane(vc->swidb)) {
	    inreg->argreg = (MCI_OFFSETSIZE + 
			     ((ATM_LANE_LECID_ENCAPBYTES +
			     ETHER_ARPA_ENCAPBYTES) >> 1));
	} else {
	    return (FALSE);
	}
    } else {
	inreg->argreg = MCI_OFFSETSIZE;
    }
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    
    /* 
     * Start reading XNS data.
     */
    checksum = inreg->readshort;
    len_hops = inreg->readlong;  
    /* check if it has valid hop count (including reserved tc bits) */
    if (((ushort)(len_hops) < (XNS_MAX_HOPS << 8))) {
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
	for (cache = xnscache[indx]; cache; cache = cache->xnsc_next_ptr) {
	    if ((*(ushort *)&cache->xnsc_dest_address[4] == addrlow) &&
		(*(ulong *)cache->xnsc_dest_address == addrhigh.d.lword) &&
		(cache->xnsc_dest_net == net.d.lword)) { 
		if (cache->xnsc_version == xnscache_version) {
		    path = cache->xnsc_path_next_ptr;
		    swoutput = path->xnsp_idb_ptr;
		    if (swoutput == input->fast_rcvidb)
			return(FALSE);
		    output = swoutput->hwptr;

		    /*
		     * if output is congested and we are sorting traffic,
		     * defer to process level
		     */
		    if (output->output_qcount && output->priority_list)
			return(FALSE);
		    
		    /* 
		     * Check if this cache entry is for directly
		     * connected net.
		     */
		    if (!cache->xnsc_connected) {
		      resume_flow:
			/*
			 * if the source net in the packet is zero,
			 * send the packet to process level.
			 */
			if (input->status & IDB_FDDI)
			    inreg->argreg = MCI_OFFSETSIZE +
				((FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES) >> 1) +
				(SOURCE_NET_OFFSET);
			else if (input->status & IDB_TR)
			    inreg->argreg = MCI_OFFSETSIZE +
				((TRING_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES) >> 1) +
				(SOURCE_NET_OFFSET);
			else if (input->status & IDB_ATM) {
			    /* 
			     * For sure this is LANE as it has already been
			     * checked.
			     */
			    inreg->argreg = (MCI_OFFSETSIZE +
					     ((ATM_LANE_LECID_ENCAPBYTES +
					      ETHER_ARPA_ENCAPBYTES) >> 1) +
					     SOURCE_NET_OFFSET);
			} else {
			    inreg->argreg = (MCI_OFFSETSIZE +
					     SOURCE_NET_OFFSET);
			}
			inreg->cmdreg = MCI_CMD_RX_SELECT;
			if (inreg->readlong == 0) {
			    return(FALSE);
			}
			cache->xnsc_path_next_ptr = path->xnsp_next_ptr;
			/* 
			 * Check if access list is enabled on this interface.
			 */
			xsb = idb_get_swsb_inline(path->xnsp_idb_ptr,
						  SWIDB_SB_XNS);
			if (!(ushort)xsb->xns_accesslist) {
			yes_doit:
			    if (inreg == output->mci_regptr) {
				
				/*       
				 * Output interface is on the same MCI. Move
				 * RX buffer from receive window to transmit
				 * window 1. This may fail if interface output
				 * queue limit has been exceeded.
				 */     
				temp_length = input->rxlength;
				inreg->argreg = (ushort)output->mci_index;
				inreg->cmdreg = MCI_CMD_RX_MOVE;    
				len_hops += 256; /* adding 1 to hop count */
				input->counters.inputs_fast[ACCT_PROTO_XNS]++;
				input->counters.rx_cumbytes_fast[ACCT_PROTO_XNS] += temp_length;
				if (inreg->cmdreg == MCI_RSP_OKAY) {
				    /* 
				     * Do some accounting of packets and bytes
				     * in and out.
				     */ 
				    xns_stat[XNS_INPUT]++;
				    xns_stat[XNS_FORWARDED]++; 
				    input_status = input->status;
				    XNS_FAST_PATH;
		    		} else if (output->priority_list) {
		    		    /*
		     		     * if output is congested and we are
				     * sorting traffic, defer to process level
		     		     */
				    return(FALSE);
				} else { 
				    /*
				     * Move failed -- flush the packet.
				     */	
				    inreg->cmdreg = MCI_CMD_RX_FLUSH;
				    output->counters.outputs_fast[ACCT_PROTO_XNS]--;
				    output->counters.output_total_drops++;
				    return(TRUE);
				}
			    } else if (output->mci_regptr) {
				/*
				 * Another MCI
				 */
				if (checksum != NULL_CHECKSUM)
				    checksum = xns_fqc(len_hops, checksum);
				return(xns_mci2mci(input, path, len_hops, 
						   checksum));
			    } else {
				/*
				 * Oops.  Shouldn't be here.
				 */
				return(FALSE);
			    }
			} else {
			    if (xns_get_ext_acl(xsb->xns_accesslist)) {
				if (!(xns_access_check_mci(input, 
							   xsb->xns_accesslist,
							   (ushort)len_hops)))
				    goto deny_access;
			    }
			    goto yes_doit;
			}
		    } else {
			idbtype *swinput = input->fast_rcvidb;
			xsb = idb_get_swsb_inline(swinput, SWIDB_SB_XNS);
			if (((addrlow == 
			      *((ushort *) &xsb->xns_node[4])) &&
			     (addrhigh.d.lword  ==  *((ulong *)xsb->xns_node)))
			    || ((addrlow == 
			      *((ushort *) &xsb->xns_node[4])) &&
				(addrhigh.d.lword  == *((ulong *)xsb->xns_node)))
			    || ((output->status & IDB_FDDI) &&
				(output->type != IDBTYPE_FDDIT)))
			    return(FALSE);
			goto resume_flow;
		    }
		} else {
		    goto deny_access;
		}
	    }
	}
    /*
     * The max hop count has apparently been exceeded. However, now do the
     * "correct" check for XNS hop count (ignoring reserved bits):
     */
    } else if (((ushort)(len_hops) & (XNS_TC_HOP_MASK << 8)) <
		(XNS_MAX_HOPS << 8)) {
      goto hop_count_ok;
    } else {
      return(FALSE);
    }
  deny_access:
    return(FALSE);             
}

static void xns_mci_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring XNS
     */
    if (!(flags & FSF_XNS))
	return;

    if (idb->fast_switch_flag & FSF_XNS) {
        fs_table_fill((fastswitch_type)xns_fastswitch, "xns_fastswitch",
		      RXTYPE_XNS);
	reg_add_smds_fs(RXTYPE_XNS, (service_smds_fs_type)return_false,
			"return_false");
	reg_add_aip_fs(RXTYPE_XNS, (service_aip_fs_type)xns_fastswitch,
			"xns_fastswitch");

    } else {
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_XNS);
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_SNAP_XNS);
    }
}

static void xns_mci_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(xns_mci_fs_register_drivers,
			     "xns_mci_fs_register_drivers");
    reg_add_fast_supported(xns_fast_supported, "xns_fast_supported");
}

/*
 * XNS MCI FS subsystem header
 */

#define XNSMCIFS_MAJVERSION 1
#define XNSMCIFS_MINVERSION 0
#define XNSMCIFS_EDITVERSION  1

SUBSYS_HEADER(xns_mci_fs, XNSMCIFS_MAJVERSION, XNSMCIFS_MINVERSION, XNSMCIFS_EDITVERSION,
	      xns_mci_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: xns",
	      "req: fastswitch, xns");
