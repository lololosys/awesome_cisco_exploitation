/* $Id: clnsfast_mci.c,v 3.2.60.1 1996/03/18 19:03:22 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/clns/clnsfast_mci.c,v $
 *------------------------------------------------------------------
 * clnsfast_mci.c -- CLNS Fast Switching module providing MCI/CBUS specific
 *                   Support.
 *
 * 7-Aug-1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clnsfast_mci.c,v $
 * Revision 3.2.60.1  1996/03/18  19:03:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:13:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  13:40:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:52:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:08:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/10  07:58:32  tli
 * CSCdi40977:  CLNS: fast switching AIP->FDDI fails
 * Off by one error.  Also some general housecleaning.
 *
 * Revision 2.3  1995/08/12  07:36:03  fred
 * CSCdi38525:  Correct pointer to hwidbtype when checking for
 * idb->priority_list
 *
 * Revision 2.2  1995/07/10  05:32:14  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.1  1995/06/07  20:18:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines Exported from this module
 * clns_fastswitch:	Fast switches clns packests(called from if_mci.c)
 *
 * The above routines is called through a switch table "fast_switch"
 * using the RXTYPE code as the index. ( The RXTYPE contains packet type
 * code when RX_SETUP is issued to MCI )
 */

#include "master.h"
#include "interface_private.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "packet.h"
#include "access.h"
#include "address.h"
#include "../if/ether.h"
#include "config.h"
#include "../wan/serial.h"
#include "../hes/if_mci.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/iso_igrp.h"
#include "../clns/clns_externs.h"
#include "../if/fddi.h"
#include "../hes/if_fci.h"
#include "../hes/if_hes_common.h"
#include "../clns/clns_inlines.h"
#include "../if/network.h"
#include "../wan/smds.h"
#include "../if/atm.h"

/*
 * The sniff buffer actually contains two more bytes than is officially
 * in the packet payload, in order to word-align the destination NSAP
 * address.  Thus the "fudge" byte is actually at offset 2 in the buffer.
 *
 * See the definition of "clns_fast_header" in clns.h.
 */


/*
 * Working buffer for header sniffing.  The size is three bytes of fudge,
 * the fixed header, two full-size addresses, the segmentation part,
 * and room for one option (in case the global QoS option is present).
 */
#define CLNS_BUFFER_SIZE (3 + CLNS_FIXEDHDR_SIZE + (2 * CLNS_MAX_NSAP_LENGTH) \
			  + sizeof(clns_segpart_t) + CLNS_MIN_OPTION_SIZE)

ulong clns_header_long[(CLNS_BUFFER_SIZE + 3) / 4];  /* Stay aligned! */

#define clns_head ((uchar *)clns_header_long)
#define fast_head (*((clns_fast_header *) clns_header_long))

/* 
 * Offset definitions for the MCI controller.  Note that we subtract 2 bytes
 * to get our three bytes of fudge (to word-align the destination address).
 */
#define CLNS_COPY_OFFSET  CLNS_FIXEDHDR_SIZE + 1
#define	ETHER_OFFSET      MCI_OFFSETSIZE-1+((ETHER_ISO3_ENCAPBYTES - \
					     ETHER_ARPA_ENCAPBYTES)>>1)
#define SERIAL_OFFSET	  (MCI_OFFSETSIZE-1)
#define FDDI_OFFSET MCI_OFFSETSIZE-1+((FDDI_ISO3_ENCAPBYTES - \
				       ETHER_ARPA_ENCAPBYTES)>>1)
#define	ATM_OFFSET(a,b)	((a)->rxoffset + btow(b) - 1)
extern ulong clns_traffic[CLNS_MAXTRAFFIC];	/* statistics */

enum if_type { IF_ETHER, IF_FDDI, IF_SERIAL, IF_ATM };

#define	CLNS_TTL_OFFSET		3	/* Words to TTL in CLNS PDU */

/*
 * Indicate CLNS as both the DSAP and SSAP.
 */
#define CLNS_SAPSAP ((SAP_CLNS << 8) | SAP_CLNS)

/*
 * Second word of a SAP encapsulated packet, with control and CLNS NLPID.
 */
#define CLNS_CTLNLPID ((LLC1_UI << 8) | NLP_ISO8473)

/*
 * Inlines
 */

/*
 * Write ethernet MAC header
 */
static inline void write_emac (mciregtype *in_mci_reg_ptr,
			       volatile ulong *in_mciwrite,
			       uchar *snpa,
			       hwidbtype *idb_ptr)
{
    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;
    /*   
     * Write the ethernet MAC header. 
     */  
    *in_mciwrite = ((ulong *)snpa)[0];	   
    *((volatile ushort *)in_mciwrite) = ((ushort *)snpa)[2];	   
    *in_mciwrite = ((ulong *)idb_ptr->hardware)[0];	   
    *((volatile ushort *)in_mciwrite) = ((ushort *)idb_ptr->hardware)[2];
}

/*
 * Write the FDDI MAC header.
 */
static inline void write_fmac_and_start (mciregtype *in_mci_reg_ptr,
					 ushort temp,
					 volatile ulong *in_mciwrite,
					 uchar *snpa,
					 hwidbtype *idb_ptr)
{
    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;    
    temp |= MCI_TX_ODDALIGN;   		   

    /*
     * FDDI frame control
     */
    *((volatile ushort *)in_mciwrite) = FDDI_LLC_FC_BYTE;  
    *in_mciwrite = ((ulong *)snpa)[0];	   
    *((volatile ushort *)in_mciwrite) = ((ushort *)snpa)[2];	   
    if (idb_ptr->type == IDBTYPE_FDDIT) {				   
	*in_mciwrite = ((ulong *)idb_ptr->hardware)[0]; 
	*((volatile ushort *)in_mciwrite) = ((ushort *)idb_ptr->hardware)[2];
    } else {							   
	*in_mciwrite = ((ulong *)idb_ptr->		   
			bit_swapped_hardware)[0];	   
	*((volatile ushort *)in_mciwrite) = ((ushort *)idb_ptr->
					     bit_swapped_hardware)[2];	   
    }								   
    *((volatile ushort *)in_mciwrite) = CLNS_SAPSAP; /* DSAP/SSAP */	   
    *((volatile ushort *)in_mciwrite) = CLNS_CTLNLPID; /* CTL/NLPID */	   
    in_mci_reg_ptr->argreg = temp;					   
    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;			   
}

/*
 * Write the hdlc header.  Include the board encapsulation, if required
 */
static inline void clns_write_hdlc_header(volatile ulong *in_mciwrite,
					  clns_cache_adjtype *adj)
{
    if (adj->length == HDLC_ENCAPBYTES)
	*((volatile ushort *)in_mciwrite) = *(ushort *)&adj->rewrite[0];
    else
	*in_mciwrite = *(ulong *)&adj->rewrite[0];
}
/*
 * Write the ATM header.  Include the board encapsulation, if required
 */
static inline void clns_write_atm_header(volatile ulong *in_mciwrite,
					  clns_cache_adjtype *adj)
{
    uint length = btow(adj->length) - 1;
    uint indx;

    for(indx = 0; length ; indx += sizeof(ushort), length--) {
	*((volatile ushort *)in_mciwrite) = *(ushort *)&adj->rewrite[indx];
    }
    *((volatile ushort *)in_mciwrite) = CLNS_CTLNLPID; /* CTL/NLPID */	   
}

/*
 * clns_mci2mci
 * Fast switch CLNS packets between two different MCI controllers.
 * Called from clns_fastswitch().
 */

static boolean clns_mci2mci (
    hwidbtype *in_idb_ptr,			/* packet input interface */
    clns_adjtype *adj,
    uchar *snpa)
{	

    hwidbtype *out_idb_ptr;			/* output interface */
    mciregtype *in_mci_reg_ptr, *out_mci_reg_ptr; /* mci register addresses */
    ushort temp, bytes;
    volatile ulong *out_mciwrite;
    
    /*
     * Set up some common variables
     */
    out_idb_ptr = adj->idb->hwptr;
    in_mci_reg_ptr = in_idb_ptr->mci_regptr;
    out_mci_reg_ptr = out_idb_ptr->mci_regptr;
    out_mciwrite = &(out_mci_reg_ptr->write1long);
    bytes = in_idb_ptr->rxlength;
    /* 
     * Select the destination interface 
     */
    out_mci_reg_ptr->argreg = out_idb_ptr->mci_index;
    out_mci_reg_ptr->cmdreg = MCI_CMD_SELECT;
    /*
     * Reserve a buffer in window 1 of the destination MCI
     *
     * The MCI doesn't care what the argreg value is -- the FCI requires that
     * it be the buffer pool number i.e. which pool to get buffer from.
     */
    out_mci_reg_ptr->argreg = out_idb_ptr->buffer_pool;
    out_mci_reg_ptr->cmdreg = MCI_CMD_TX1_RESERVE;
    if ((temp = out_mci_reg_ptr->cmdreg) == MCI_RSP_OKAY) {
	/*
	 * Set up write pointer to base of destination transmit window.
	 * Do some input accounting.  Figure out how to write new MAC header.
	 * Then adjust the read pointer of the receiver window to point after
	 * Packet Type in XNS data. Compute number of bytes to be copied
         * (in temp) and to be sent (in bytes).
	 */
	out_mci_reg_ptr->argreg = 0;
	out_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;
	in_idb_ptr->counters.inputs_fast[ACCT_PROTO_CLNS]++;
	in_idb_ptr->counters.rx_cumbytes_fast[ACCT_PROTO_CLNS] += bytes;
	out_idb_ptr->counters.outputs_fast[ACCT_PROTO_CLNS]++;
	out_idb_ptr->counters.tx_cumbytes_fast[ACCT_PROTO_CLNS] += bytes;
	clns_traffic[CLNS_FORWARD]++;
	clns_traffic[CLNS_INPUT]++;
	temp = bytes;
	if (out_idb_ptr->status & IDB_ETHER) {
	    /*
	     * Output is Ethernet.
	     */
	    if (in_idb_ptr->status & IDB_SERIAL) {
		/*
		 * Output ethernet -- input serial, adjust receive offset,
		 * number of bytes to copy (temp) and size (bytes) of output
		 * datagram.
		 */
		in_mci_reg_ptr->argreg = MCI_SERIAL_OFFSET + 
		    ((HDLC_ENCAPBYTES + CLNS_COPY_OFFSET) >>1);
		temp -= (HDLC_ENCAPBYTES + CLNS_COPY_OFFSET);
		bytes += (ETHER_ISO3_ENCAPBYTES - HDLC_ENCAPBYTES);
		if (bytes < MINETHERSIZE)
		    bytes = MINETHERSIZE;
	    } else if (in_idb_ptr->status & IDB_FDDI) {
		/*
		 * Output ethernet -- input fddi, check for max datagram 
		 * size on ethernet, set receive offset, adjust number of
		 * of bytes to copy (temp) and size (bytes) of output datagram.
		 */
		in_mci_reg_ptr->argreg = (FDDI_RECEIVE_OFFSET + 
		    (((FDDI_ISO3_ENCAPBYTES + CLNS_COPY_OFFSET)>>1) + 1));
		temp -= (FDDI_ISO3_ENCAPBYTES + CLNS_COPY_OFFSET);
		bytes -= (FDDI_ISO3_ENCAPBYTES - ETHER_ISO3_ENCAPBYTES);
		if (bytes < MINETHERSIZE)
		    bytes = MINETHERSIZE;
	    } else {
		/*
		 * Output ethernet -- input ethernet, adjust receive offset 
		 * and number of bytes to copy (temp).
		 */
		in_mci_reg_ptr->argreg = MCI_ETHER_OFFSET + 
		                     ((ETHER_ISO3_ENCAPBYTES +
				       CLNS_COPY_OFFSET) >>1);
		temp -= (ETHER_ISO3_ENCAPBYTES + CLNS_COPY_OFFSET);
	    }
	    in_mci_reg_ptr->cmdreg = MCI_CMD_RX_SELECT;	
	    *out_mciwrite = ((ulong *)snpa)[0];
	    *((volatile ushort *) out_mciwrite) = ((ushort *)snpa)[2];
	    *out_mciwrite = ((ulong *)out_idb_ptr->hardware)[0];
	    *((volatile ushort *) out_mciwrite) =
		((ushort *)out_idb_ptr->hardware)[2];
	    *((volatile ushort *) out_mciwrite) = bytes -
		ETHER_ARPA_ENCAPBYTES;                          
	    /* DSAP/SSAP */   
	    *((volatile ushort *) out_mciwrite) = CLNS_SAPSAP; 
	    /* CTL/NLPID */
	    *((volatile ushort *) out_mciwrite) = CLNS_CTLNLPID; 
	    *out_mciwrite = *((ulong *)&fast_head.hlen);
	    *out_mciwrite = *((ulong *)&fast_head.clns_seglen);
	} else if (out_idb_ptr->status & IDB_FDDI) {
	    /*
	     * Output is fddi.
	     */
	    if (in_idb_ptr->status & IDB_ETHER) {
		/* 
		 * Input ethernet -- output fddi, adjust receive offset,
		 *  number of bytes to copy (temp), and size (bytes)
		 * of output datagram.
		 */
		in_mci_reg_ptr->argreg = MCI_ETHER_OFFSET + 
		                     ((ETHER_ISO3_ENCAPBYTES +
				       CLNS_COPY_OFFSET)>>1);
		temp -= (ETHER_ISO3_ENCAPBYTES + CLNS_COPY_OFFSET);
		bytes += (FDDI_ISO3_ENCAPBYTES - ETHER_ISO3_ENCAPBYTES);
	    } else if (in_idb_ptr->status & IDB_FDDI) {
		/*
		 * Input fddi -- output fddi, adjust receive offset
		 * and the number of bytes to copy (temp).
		 */
		in_mci_reg_ptr->argreg = FDDI_RECEIVE_OFFSET + 
		    (((FDDI_ISO3_ENCAPBYTES + CLNS_COPY_OFFSET)>>1) + 1);
		temp -= (FDDI_ISO3_ENCAPBYTES + CLNS_COPY_OFFSET);
	    } else {
		/*
		 * Input serial -- output fddi, adjust recieve offset,
		 * number of bytes to copy (temp) and size (bytes) of
		 * output datgram.
		 */
		in_mci_reg_ptr->argreg = MCI_SERIAL_OFFSET +
		                    ((HDLC_ENCAPBYTES + CLNS_COPY_OFFSET)>>1);
		temp -= (HDLC_ENCAPBYTES + CLNS_COPY_OFFSET);
		bytes += (FDDI_ISO3_ENCAPBYTES - HDLC_ENCAPBYTES);
	    }
	    bytes |=  MCI_TX_ODDALIGN;
	    in_mci_reg_ptr->cmdreg = MCI_CMD_RX_SELECT;
	    /*
	     * Write the FDDI SNAP encapsulation header.
	     */
	    *((volatile ushort *) out_mciwrite) = FDDI_LLC_FC_BYTE; /*       ?? fc */
	    *out_mciwrite = ((ulong *)snpa)[0];
	    *((volatile ushort *) out_mciwrite) = ((ushort *)snpa)[2];
	    if (out_idb_ptr->type == IDBTYPE_FDDIT) {
		*out_mciwrite = ((ulong *)out_idb_ptr->hardware)[0];
		*((volatile ushort *) out_mciwrite) =
		    ((ushort *)out_idb_ptr->hardware)[2];
	    } else {
		*out_mciwrite =
		    ((ulong *)out_idb_ptr->bit_swapped_hardware)[0];
		*((volatile ushort *) out_mciwrite) =
		    ((ushort *)out_idb_ptr->bit_swapped_hardware)[2];
	    }
	    /* DSAP/SSAP */
	    *((volatile ushort *) out_mciwrite) = CLNS_SAPSAP; 
	    /* CTL/NLPID */
	    *((volatile ushort *) out_mciwrite) = CLNS_CTLNLPID; 
	    *out_mciwrite = *((ulong *)&fast_head.hlen);
	    *out_mciwrite = *((ulong *)&fast_head.clns_seglen);
	} else {
 	    /*
	     * Output is HDLC.
	     */
	    if (in_idb_ptr->status & IDB_ETHER) {
		/* 
		 * Input ethernet -- output serial.
		 */
		in_mci_reg_ptr->argreg = MCI_ETHER_OFFSET 
		                    + ((ETHER_ISO3_ENCAPBYTES + CLNS_COPY_OFFSET)>>1);
		temp -= (ETHER_ISO3_ENCAPBYTES + CLNS_COPY_OFFSET);
		bytes -= (ETHER_ISO3_ENCAPBYTES - HDLC_ENCAPBYTES);
	    } else if (in_idb_ptr->status & IDB_FDDI) {
		/*
		 * Input fddi
		 */
		in_mci_reg_ptr->argreg = FDDI_RECEIVE_OFFSET + 
		    (((FDDI_ISO3_ENCAPBYTES + CLNS_COPY_OFFSET)>>1) + 1);
		temp -= (FDDI_ISO3_ENCAPBYTES + CLNS_COPY_OFFSET);
		bytes -= (FDDI_ISO3_ENCAPBYTES - HDLC_ENCAPBYTES);
		
	    } else {
		/*
		 * Input serial -- output serial, no adjustment needed.
		 */
		in_mci_reg_ptr->argreg = MCI_SERIAL_OFFSET 
		             + ((HDLC_ENCAPBYTES + CLNS_COPY_OFFSET)>>1);
		temp -= (HDLC_ENCAPBYTES + CLNS_COPY_OFFSET);
	    }
	    in_mci_reg_ptr->cmdreg = MCI_CMD_RX_SELECT;
	    *((volatile ushort *) out_mciwrite) = HDLC_STATION;
	    *((volatile ushort *) out_mciwrite) = CLNS_SAPSAP; /* DSAP/SSAP */
	    /* CTL/NLPID */
	    *((volatile ushort *) out_mciwrite) = CLNS_CTLNLPID;
	    *out_mciwrite = *((ulong *)&fast_head.hlen);
	    *out_mciwrite = *((ulong *)&fast_head.clns_seglen);
	}
	/*
	 * Copy bulk of datagram from one MCI to another.
	 * Transmit modified copy and flush original in receive buffer.
	 */
	mci2mci(&in_mci_reg_ptr->readlong,out_mciwrite,temp);
	out_mci_reg_ptr->argreg = bytes;
	out_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;
    } else  if (out_idb_ptr->priority_list) {
	/* if sorting traffic and interface is congested, process switch */
	return FALSE;
    } else {
	/*
	 * Reserve failed on output -- flush the packet.
	 */	
	out_idb_ptr->counters.output_total_drops++;
    }
    in_mci_reg_ptr->cmdreg = MCI_CMD_RX_FLUSH;
    return(TRUE);

}


/*
 * Do the bulk of fast-switching.
 */
static inline boolean fast_path (hwidbtype *output, ushort bytes,
				 mciregtype *in_mci_reg_ptr,
				 volatile ulong *in_mciwrite,
				 enum if_type type,
				 clns_cache_adjtype *adj, hwidbtype *input,
				 int input_encsize, int input_offset,
				 vc_info_t *invc)
{
    output->counters.outputs_fast[ACCT_PROTO_CLNS]++;                      
    output->counters.tx_cumbytes_fast[ACCT_PROTO_CLNS] += bytes;            
    if (output->status & IDB_ETHER) {                             
	switch (type) {                                            
	case IF_ETHER:                                             
	    /* in eth,-- out eth */                                
	    in_mci_reg_ptr->argreg = ETHER_OFFSET + CLNS_TTL_OFFSET;             
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;           
	    /* Rewrite word with TTL. */                           
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                      
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);         
	    in_mci_reg_ptr->argreg = MCI_ETHER_OFFSET;             
	    write_emac(in_mci_reg_ptr, in_mciwrite, adj->dst_snpa, output);
	    in_mci_reg_ptr->argreg = bytes;                         
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;            
	    return(TRUE);                                          
	case IF_FDDI:                                              
	    /* in fddi -- out eth */                               
	    bytes -=(FDDI_ISO3_ENCAPBYTES - ETHER_ISO3_ENCAPBYTES);         
	    if (bytes < MINETHERSIZE)                               
		bytes = MINETHERSIZE;                               
	    in_mci_reg_ptr->argreg = FDDI_OFFSET + CLNS_TTL_OFFSET;              
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;           
	    /* Rewrite word with TTL. */                           
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);          
	    in_mci_reg_ptr->argreg = MCI_ETHER_OFFSET - 1;          
	    write_emac(in_mci_reg_ptr, in_mciwrite, adj->dst_snpa, output);
	    *((volatile ushort *)in_mciwrite) = bytes - ETHER_ARPA_ENCAPBYTES;
	    in_mci_reg_ptr->argreg = bytes;                          
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;             
	    return(TRUE);                                           
	case IF_SERIAL:                                             
	    /* Input serial -- output ethernet. */                  
	    bytes += (ETHER_ISO3_ENCAPBYTES - input_encsize);              
	    if (bytes < MINETHERSIZE)                                
		bytes = MINETHERSIZE;                                
	    in_mci_reg_ptr->argreg = SERIAL_OFFSET + CLNS_TTL_OFFSET;             
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;            
	    /* Rewrite word with TTL. */                            
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);          
	    in_mci_reg_ptr->argreg = MCI_ETHER_OFFSET - 1;          
	    write_emac(in_mci_reg_ptr, in_mciwrite, adj->dst_snpa, output);
	    *((volatile ushort *)in_mciwrite) = bytes - ETHER_ARPA_ENCAPBYTES;
	    in_mci_reg_ptr->argreg = bytes;                          
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;             
	    return(TRUE);                                           
	case IF_ATM:
	    /* Input ATM -- output ethernet. */                  
	    ATM_INFAST(input);
	    invc->infast++;
	    bytes -= input_encsize;
	    bytes += ETHER_ISO3_ENCAPBYTES;
	    if (bytes < MINETHERSIZE)                                
		bytes = MINETHERSIZE;                                
	    in_mci_reg_ptr->argreg = input_offset + CLNS_TTL_OFFSET;
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;            
	    /* Rewrite word with TTL. */                            
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);          
	    in_mci_reg_ptr->argreg = (input_offset + 1)
	      - btow(ETHER_ISO3_ENCAPBYTES);
	    write_emac(in_mci_reg_ptr, in_mciwrite, adj->dst_snpa, output);
	    *((volatile ushort *)in_mciwrite) = bytes - ETHER_ARPA_ENCAPBYTES;
	    *((volatile ushort *)in_mciwrite) = CLNS_SAPSAP; /* For NLPID */
	    in_mci_reg_ptr->argreg = bytes;                          
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;             
	    return(TRUE);
	} /* end switch */                                          
    } else if (output->status & IDB_FDDI) {                        
	/*                                                          
	 * Output fddi.                                             
	 */                                                         
	switch (type) {                                             
	case IF_ETHER:                                              
	    /* Input ethernet -- output fddi.*/                     
	    bytes += (FDDI_ISO3_ENCAPBYTES - ETHER_ISO3_ENCAPBYTES);         
	    in_mci_reg_ptr->argreg = ETHER_OFFSET + CLNS_TTL_OFFSET;              
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;            
	    /* Rewrite word with TTL. */                            
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);          
	    in_mci_reg_ptr->argreg = FDDI_RECEIVE_OFFSET + 1;       
	    write_fmac_and_start(in_mci_reg_ptr, bytes, in_mciwrite,
				 adj->dst_snpa, output);
	    return(TRUE);
	case IF_FDDI:                                               
	    /* Input fddi -- output fddi. */                        
	    in_mci_reg_ptr->argreg = FDDI_OFFSET + CLNS_TTL_OFFSET;               
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;            
	    /* Rewrite word with TTL. */                            
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);          
	    in_mci_reg_ptr->argreg = FDDI_RECEIVE_OFFSET;           
	    write_fmac_and_start(in_mci_reg_ptr, bytes, in_mciwrite,
				 adj->dst_snpa, output);
	    return(TRUE);
	case IF_SERIAL:                                             
	    /* Input serial -- output fddi.*/                       
	    bytes += (FDDI_ISO3_ENCAPBYTES                            
		     - input_encsize);          
	    if (bytes < MINETHERSIZE)                                
		bytes = MINETHERSIZE;                                 
	    in_mci_reg_ptr->argreg = SERIAL_OFFSET + CLNS_TTL_OFFSET;             
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;            
	    /* Rewrite word with TTL. */                            
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);          
	    in_mci_reg_ptr->argreg = FDDI_RECEIVE_OFFSET;           
	    write_fmac_and_start(in_mci_reg_ptr, bytes, in_mciwrite,
				 adj->dst_snpa, output);
	    return(TRUE);
	case IF_ATM:
	    /* Input ATM -- output fddi. */                  
	    ATM_INFAST(input);
	    invc->infast++;
	    bytes -= input_encsize;
	    bytes += FDDI_ISO3_ENCAPBYTES;
	    in_mci_reg_ptr->argreg = input_offset + CLNS_TTL_OFFSET;
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;            
	    /* Rewrite word with TTL. */                            
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);          
	    in_mci_reg_ptr->argreg = input_offset - btow(FDDI_ENCAPBYTES);
	    write_fmac_and_start(in_mci_reg_ptr, bytes, in_mciwrite,
				 adj->dst_snpa, output);
	    return(TRUE);
	}                                                           
    } else if (output->status & IDB_SERIAL) {                      
	/*                                                          
	 * Output is Serial.                                        
	 */                                                         
	switch (type) {                                             
	case IF_ETHER:                                              
	    /* Input ethernet -- output serial. */                  
	    in_mci_reg_ptr->argreg = ETHER_OFFSET + CLNS_TTL_OFFSET;              
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;            
	    /* Rewrite word with TTL. */                           
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);         
	    in_mci_reg_ptr->argreg = MCI_OFFSETSIZE + 1 -
		                     btow(output->encsize);         
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;
	    clns_write_hdlc_header(in_mciwrite, adj);
	    bytes -= (ETHER_ISO3_ENCAPBYTES - output->encsize);               
	    in_mci_reg_ptr->argreg = bytes;                           
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;              
	    return(TRUE);              			         
	case IF_FDDI:                                                
	    /* Input fddi -- output serial. */                       
	    bytes -= (FDDI_ISO3_ENCAPBYTES - output->encsize);          
	    in_mci_reg_ptr->argreg = FDDI_OFFSET + CLNS_TTL_OFFSET;                
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;             
	    /* Rewrite word with TTL. */                           
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);         
	    in_mci_reg_ptr->argreg = MCI_OFFSETSIZE - btow(output->encsize);
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;
	    clns_write_hdlc_header(in_mciwrite,adj);
	    in_mci_reg_ptr->argreg = bytes;                           
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;              
	    return(TRUE);                                            
	case IF_SERIAL:                                              
	    /* Input serial -- output serial. */
	    bytes -= (input_encsize - output->encsize);
	    in_mci_reg_ptr->argreg = SERIAL_OFFSET + CLNS_TTL_OFFSET;              
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;             
	    /* Rewrite word with TTL. */                           
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);         
	    in_mci_reg_ptr->argreg = MCI_OFFSETSIZE - btow(output->encsize);
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;
	    clns_write_hdlc_header(in_mciwrite,adj);
	    in_mci_reg_ptr->argreg = bytes;                           
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;              
	    return(TRUE);                                            
	case IF_ATM:
	    /* Input ATM -- output serial. */                  
	    ATM_INFAST(input);
	    invc->infast++;
	    bytes -= input_encsize;
	    bytes += output->encsize;
	    in_mci_reg_ptr->argreg = input_offset + CLNS_TTL_OFFSET;
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;            
	    /* Rewrite word with TTL. */                            
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);          
	    in_mci_reg_ptr->argreg = (input_offset + 1) -
		btow(output->encsize);
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;
	    clns_write_hdlc_header(in_mciwrite,adj);
	    in_mci_reg_ptr->argreg = bytes;                          
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;             
	    return(TRUE);
	}                                                            
    } else if (is_atm(output)) {
	/*                                                          
	 * Output is ATM.                                        
	 *
	 * The macro btow() returns the number of words spanned by
	 * the number of bytes. That is to say it rounds up when the 
	 * length is odd. The result is the argreg will be one off when
	 * the length is odd. Subtracting one fixes that.
	 */
	ATM_OUTFAST(output);
	ATM_VCOUTFAST(output, *(vcd_t *)adj->rewrite);
	switch (type) {                                             
	case IF_ETHER:                                              
	    /* Input ethernet -- output ATM. */                  
	    in_mci_reg_ptr->argreg = ETHER_OFFSET + CLNS_TTL_OFFSET;              
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;            
	    /* Rewrite word with TTL. */                           
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);         
	    in_mci_reg_ptr->argreg = MCI_OFFSETSIZE + 1 -
		                     btow(adj->length - 1);         
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;
	    clns_write_atm_header(in_mciwrite, adj);
	    bytes -= (ETHER_ISO3_ENCAPBYTES - (adj->length - 1));               
	    in_mci_reg_ptr->argreg = bytes;                           
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;              
	    return(TRUE);              			         
	case IF_FDDI:                                                
	    /* Input fddi -- output ATM. */                       
	    bytes -= (FDDI_ISO3_ENCAPBYTES - (adj->length - 1));          
	    in_mci_reg_ptr->argreg = FDDI_OFFSET + CLNS_TTL_OFFSET;                
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;             
	    /* Rewrite word with TTL. */                           
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);         
	    in_mci_reg_ptr->argreg = MCI_OFFSETSIZE - btow(adj->length - 1);
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;
	    clns_write_atm_header(in_mciwrite,adj);
	    in_mci_reg_ptr->argreg = bytes;                           
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;              
	    return(TRUE);                                            
	case IF_SERIAL:                                              
	    /* Input serial -- output ATM. */
	    bytes -= (input_encsize - (adj->length - 1));
	    in_mci_reg_ptr->argreg = SERIAL_OFFSET + CLNS_TTL_OFFSET;              
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;             
	    /* Rewrite word with TTL. */                           
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);         
	    in_mci_reg_ptr->argreg = MCI_OFFSETSIZE - btow(adj->length - 1);
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;
	    clns_write_atm_header(in_mciwrite,adj);
	    in_mci_reg_ptr->argreg = bytes;                           
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;              
	    return(TRUE);                                            
	case IF_ATM:
	    /* Input ATM -- Output ATM */
	    ATM_INFAST(input);
	    invc->infast++;
	    bytes -= input_encsize;
	    bytes += adj->length;
	    in_mci_reg_ptr->argreg = input_offset + CLNS_TTL_OFFSET;
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;             
	    /* Rewrite word with TTL. */                           
	    *((volatile ushort *)in_mciwrite) =
		*((ushort *)&fast_head.lifetime);
	    /* Rewrite long with checksum. */                       
	    *in_mciwrite = *((ulong *) &fast_head.clns_seglen);         
	    in_mci_reg_ptr->argreg = input_offset + 1 - btow(adj->length - 1);
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_SELECT;
	    clns_write_atm_header(in_mciwrite,adj);
	    in_mci_reg_ptr->argreg = bytes;                           
	    in_mci_reg_ptr->cmdreg = MCI_CMD_TX1_START;              
	    return(TRUE);
	}                                                            
    }
    return(FALSE);
}

/*
 * clns_fastswitch
 * Very high speed switching of Ether,HDLC or FDDI encapsulated CLNS datagrams.
 * This routine has been very heavily optimized for speed, since it is in
 * the inner loop of the code that handles back-to-back datagrams.  Do not
 * add permanent debugging code, subroutines, or much of anything else.
 * The GNU CC compiler will generate the best code.
 * Speedups *must* be debugged with an oscilloscope.
 */

#ifdef mc68000
#define CLEARWORD(word) {\
    asm volatile ( "\
        clrl %0" \
                 : "=r" ((word)));\
}
#else
#define CLEARWORD(word) {\
    word = 0;\
    }
#endif
	
#ifdef mc68000
#define CKSUMWORD(ptr, mask, c0, c1, temp) {\
    asm volatile ( "\
        movew %1@+,%0" \
		  : "=r" ((temp))\
		  : "a" ((ptr)));\
    c1 += c0;\
    c0 += temp;\
    temp &= mask;\
    c1 += c0;\
    c1 += temp;\
}
#else
#define CKSUMWORD(ptr, mask, c0, c1, temp) {\
    ulong temp = *ptr++;\
    c1 += c0;\
    c0 += temp;\
    temp &= mask;\
    c1 += c0;\
    c1 += temp;\
}
#endif

boolean clns_fastswitch (hwidbtype *idb_ptr)
{

    mciregtype *in_mci_reg_ptr;
    clns_cache_adjtype *cache_adj;
    ushort temp; 
    ulong src_addr_len, dest_addr_len, min_hdr_len, header_len; 
    ulong *h_ptr = clns_header_long;
    enum if_type type;
    volatile ulong *in_mciread, *in_mciwrite;
    ulong word;
    clns_fast_header *headptr = &fast_head;
    uint encsize, input_offset;
    vc_info_t *vc = NULL;

    /*
     * encsize == idb_ptr->encsize for all but ATM interfaces.
     *	ATM has different encapsulation required per VC
     */
    encsize = idb_ptr->encsize;	
    /*
     * SETUP CLNS data read pointer
     */
    in_mci_reg_ptr = idb_ptr->mci_regptr;
    in_mciread = &(in_mci_reg_ptr->readlong);
    if (idb_ptr->status & IDB_ETHER) {
	input_offset = ETHER_OFFSET;
	type = IF_ETHER;
    } else if (is_atm(idb_ptr)) {
        /*
         * In the case of ATM, CBUS_AIP_RECEIVE in cbus_interrupt leaves
         * first 2 words (vcd & enctype) in sniff buffer and vc_info
         * pointer in atm_db.  Check for NULL vc done in CBUS_AIP_RECEIVE.
         * The EncTYPE can only be AAL5SNAP, AAL5NLPID or AAL34SMDS!
         */
	vc = idb_ptr->atm_db->fast_vc_info;
        /* switch on EncTYPE (2nd 16-bit word) */
	switch(idb_ptr->sniff[SNIFFBASE+0].d.sword[1]) {
	case ATM_ENCTYPE_SNAP:
	    encsize = ATM_SNAP_ENCAPBYTES - ATM_SNAP_SAP_ADJBYTES;
	    input_offset = MCI_OFFSETSIZE;
	    break;
	case ATM_ENCTYPE_NLPID:
	    encsize = ATM_NLPID_ENCAPBYTES - 1;
	    /*
	     * Be back up 1 word to position over the alleged '0xFEFE'
	     * portion of the frame.  0xFEFE may or may not be in this
	     * location, but the remaining SNIFF code expects the initial
	     * offset to be pointing at that location.  The 0xFEFE
	     * is actually ignored by all of this FS code.
	     */
	    input_offset = MCI_OFFSETSIZE - 1;
	    break;
	case ATM_ENCTYPE_SMDS:
	    encsize = ATM_SMDS_ENCAPBYTES - ATM_SNAP_SAP_ADJBYTES;
	    input_offset = MCI_OFFSETSIZE + btow(sizeof(smdstype));
	    break;
	default:
	    return(FALSE);
	}
	type = IF_ATM;
    } else	
	if (idb_ptr->status & IDB_SERIAL) {
	    input_offset = SERIAL_OFFSET;
	    type = IF_SERIAL;
	} else { 
	    input_offset = FDDI_OFFSET;
	    type = IF_FDDI;
	}
    in_mci_reg_ptr->argreg = input_offset;
    /*
     * point at CLNS data 
     */
    in_mci_reg_ptr->cmdreg = MCI_CMD_RX_SELECT;
    /* 
     * Start reading CLNS data and
     * make sure that it is ISO8473
     * packet.(read: fudge, pid)
     *
     * Note that we let the compiler generate the first reference to get
     * the registers set up.
     */
    *h_ptr++ = *in_mciread;	/* Word 0 */
    if (headptr->pid != NLP_ISO8473)
        return FALSE;

    *h_ptr++ = *in_mciread; /* Word 1 */

    if (headptr->version != CLNS_VERSION)
        return(FALSE);

    if (headptr->lifetime <= 1)
        return(FALSE);
    /*
     * Read in the rest of a full-sized header.  It's cheaper just to
     * do the whole thing now in an unrolled loop.
     */
    *h_ptr++ = *in_mciread; /* Word 2 */
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread; /* Word 10 */
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread;
    *h_ptr++ = *in_mciread; /* Word 15 */
    
    /*
     * start reading address part
     */
    
    dest_addr_len = headptr->dest[0];
    if (dest_addr_len >= CLNS_MAX_NSAP_LENGTH ||
	dest_addr_len < MIN_CACHE_SEARCH_LEN) return (FALSE);
    src_addr_len = *(((uchar *) (&headptr->dest)) + dest_addr_len + 1);
    min_hdr_len = CLNS_FIXEDHDR_SIZE + 1 + dest_addr_len + 1 + src_addr_len + 
	(headptr->sp ? sizeof(clns_segpart_t) : 0);
    header_len = headptr->hlen;

    if (header_len > min_hdr_len) {
        uchar *ptr = (uchar *) &headptr->pid;
	/*
	 * options are present
	 * in the packet. We handle only
 	 * the global QoS option and the padding 
	 * option for fast switching.
	 */
	if ((GETLONG(&ptr[min_hdr_len]) & CLNS_QOS_OPT_MASK) == 
	      CLNS_QOS_OPT_VALUE) {
	    min_hdr_len += CLNS_MIN_OPTION_SIZE;
	    if (header_len != min_hdr_len) { 
	        if (header_len < min_hdr_len)
	            return(FALSE);
	        if (ptr[min_hdr_len] != CLNS_OPT_PADDING)
	            return(FALSE);
	        if (header_len != min_hdr_len + ptr[min_hdr_len+1] + 2)
	            return(FALSE);	
	    }
	} else if (ptr[min_hdr_len] == CLNS_OPT_PADDING) {
	    min_hdr_len += ptr[min_hdr_len+1] + 2;
	    if (header_len != min_hdr_len) {
	        if (header_len < min_hdr_len)
	            return(FALSE);
	        if ((GETLONG(&ptr[min_hdr_len]) & CLNS_QOS_OPT_MASK) != 
		     CLNS_QOS_OPT_VALUE)
	            return(FALSE);
                if (header_len != min_hdr_len + CLNS_MIN_OPTION_SIZE)
	            return(FALSE);
	    }
	} else
	    return(FALSE);
    } else if (header_len != min_hdr_len) {
	return(FALSE);
    }

    /*
     * If there's a checksum present, verify it.
     *
     * We use an algorithm that is essentially taken from sklower's paper
     * in SIGCOMM.  However, we observe that (a) the packet is odd-aligned,
     * and (b) the first octet is invariant, so we only actually run the
     * algorithm over the 2nd and latter octets after initializing the
     * variables as appropriate.
     *
     * We observe that the likely minimum header size is 37 (with 9 octet 
     * NSAPs), and the probable size (with 20 octet NSAPs) is 57 or 60
     * (without or with the global QoS option, respectively).  We
     * optimize for those cases.
     * 
     */
    if (GETSHORT(headptr->checksum) != 0) {
	ushort *cksumptr;
	ulong c0, c1;
	ushort z;
	ulong bytecount = header_len - 1;
	ulong mask = 0xff00;
	/*
	 * verify the checksum 
	 */
	c0 = c1 = NLP_ISO8473;
	cksumptr = (ushort *) (&headptr->pid + 1);
	CLEARWORD(word);
	if (bytecount >= 36) {
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    bytecount -= 36;
	}
	if (bytecount >= 20) {
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    bytecount -= 20;
	}
	if (bytecount == 3) {
	    CKSUMWORD(cksumptr, mask, c0, c1, word);
	    c0 += *((uchar *)cksumptr);
	    c1 += c0;
	} else if (bytecount) {
	    while (bytecount >= 8) {
		CKSUMWORD(cksumptr, mask, c0, c1, word);
		CKSUMWORD(cksumptr, mask, c0, c1, word);
		CKSUMWORD(cksumptr, mask, c0, c1, word);
		CKSUMWORD(cksumptr, mask, c0, c1, word);
		bytecount -= 8;
	    }
	    while (bytecount >= 2) {
		CKSUMWORD(cksumptr, mask, c0, c1, word);
		bytecount -= 2;
	    }
	    if (bytecount) {
		c0 += *((uchar *)cksumptr);
		c1 += c0;
	    }
	}

	/* adjust accumulated sums to mod 255 */
	z = 0;
	MOD255(c0, z);
	MOD255(c1, z);
	if ((c0 | c1) != 0) {
	    return FALSE;
	} else {
	    /*
	     * update checksum
	     */
	    uchar z = 0;
	    uchar *ptr = headptr->checksum;
	    ADD_MOD255(*ptr, 5, z);
	    ptr++;
	    SUB_MOD255(*ptr, 4, z);
	}
    }
    --(headptr->lifetime);

    /* Filter the packet if appropriate. */
    
    if (idb_ptr->fast_rcvidb->clns_filter_in) {
	uchar *srcad_ptr = ((uchar *)(headptr->dest)) + 
	    dest_addr_len + 1;
	if (!clns_filter_addr(idb_ptr->fast_rcvidb->clns_filter_in,
			      (nsap_addr *) headptr->dest,
			      (nsap_addr *) srcad_ptr))
	    return FALSE;
    }

    if ((cache_adj = fast_nsap_cache_search(headptr, dest_addr_len))) {
	clns_adjtype *adj = cache_adj->adj;
	idbtype *adjidb = adj->idb;

	/* Slow switch if we need to generate a MAC rewrite. */

	if (!cache_adj->length)
	    return(FALSE);

	/* Slow switch if we need to generate a redirect, or if CLNS is off. */

	if (adjidb == idb_ptr->fast_rcvidb || !(adjidb->hwptr->clns_fastokay)) 
	    return(FALSE);

	/* Slow switch if we must fragment. */

	if (adjidb->clns_mtu < *((ushort *) fast_head.clns_seglen))
	    return(FALSE);

	/* Filter the packet if appropriate. */
	
	if (adjidb->clns_filter_out) {
	    uchar *srcad_ptr = ((uchar *)(headptr->dest)) + 
		fast_head.dest[0] + 1;
	    if (!clns_filter_addr(adjidb->clns_filter_out,
				  (nsap_addr *) headptr->dest,
				  (nsap_addr *) srcad_ptr))
		return FALSE;
	}

	/* 
	 * if sorting traffic and interface is congested, process switch 
	 */
	if (adjidb->hwptr->output_qcount && adjidb->hwptr->priority_list) {
	    return FALSE;
	}

	if (adjidb->hwptr->devctl == idb_ptr->devctl) {  

	    /*
	     * Output interface is on the same MCI. Move
	     * RX buffer from receive window to transmit
	     * window 1. This may fail if interface output
	     * queue limit has been exceeded.
	     */     
	    in_mci_reg_ptr->argreg = adjidb->hwptr->mci_index;
	    in_mci_reg_ptr->cmdreg = MCI_CMD_RX_MOVE;    
	    idb_ptr->counters.inputs_fast[ACCT_PROTO_CLNS]++; 
	    if (in_mci_reg_ptr->cmdreg == MCI_RSP_OKAY) {
		/* 
		 * Do some accounting of packets and bytes
		 * in and out.
		 */ 
		temp  = idb_ptr->rxlength;
		clns_traffic[CLNS_INPUT]++;
		clns_traffic[CLNS_FORWARD]++;
		idb_ptr->counters.rx_cumbytes_fast[ACCT_PROTO_CLNS] += temp;
		in_mciwrite = &(in_mci_reg_ptr->write1long);
		return (fast_path( adjidb->hwptr, temp,
				  in_mci_reg_ptr, in_mciwrite,
				  type, cache_adj, 
				  idb_ptr, encsize, input_offset, vc));
	    } else if (adjidb->hwptr->priority_list) {

		/* 
		 * if sorting traffic and interface is congested, process switch 
		 */
		return FALSE;
	    } else { 
		/*
		 * Move failed -- flush the packet.
		 */	
		in_mci_reg_ptr->cmdreg = MCI_CMD_RX_FLUSH;
		adjidb->hwptr->counters.output_total_drops++;
		return(TRUE);
	    }
	} else {		/* they are on different MCIs */
	    /*
	     * Output is on another MCI, if TX1 bug, drop
	     * the packet.
	     */
	    if (((mcitype *)adjidb->hwptr->devctl)->tx1bug == FALSE) { 
		if (clns_mci2mci(idb_ptr, adj, cache_adj->dst_snpa))
		    return(TRUE);
	    }	
	    return(FALSE);
	}
    } else {
	return(FALSE);
    }
    return(FALSE);             
}				/* end of clns_fastswitch */

/*
 * clns_fast_fragmentation
 * This isn't as fast as it's IP cousin (from which it is derived).  This
 * relies on slow switching doing the real work.
 */

static boolean clns_fast_fragmentation (hwidbtype *inidb, hwidbtype *outidb)
{
    mciregtype *regptr;
    ushort utmp1, i;
    paktype *pak;

    regptr = inidb->mci_regptr;

    /*
     * Get the input and output mac lengths.
     */
    utmp1 = regptr->rxsrbinfo;
    utmp1 >>= 8;

    /*
     * Move to the start input MAC header (that's no longer there).
     * utmp1 = input mac length
     */
    regptr->rx_select = inidb->rxoffset;
    i = inidb->rxlength - utmp1;
    pak = input_getbuffer(i, inidb);
    if (!pak) {
	cbus_mci_idb_throttle(inidb);
	return(FALSE);
    }
    pak->datagramsize = i;
    pak->datagramstart = pak->data_area + ENCAPBYTES;

    /*
     * +1 thanks to the FFB.
     */
    pak->mac_start = pak->datagramstart - utmp1 + 1;
    pak->addr_start = pak->datagramstart;
    pak->rif_start = NULL;
    pak->riflen = 0;
    pak->info_start = pak->mac_start;
    pak->network_start = pak->mac_start;
    mci2buffer(&regptr->readlong, pak->mac_start, i);		
    inidb->counters.inputs++;
    clns_enqueue(pak);
    return(FALSE);
}


static void clns_mci_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring CLNS
     */
    if (!(flags & FSF_CLNS))
	return;

    if (idb->fast_switch_flag & FSF_CLNS) {
	/*
	 * This is a kludge and should not be necessary, but the microcode
	 * keeps changing the rxtype without warning.  The cost of the
	 * extra registration is trivial.
	 */
	reg_add_ether_fs(RXTYPE_CLNS,
			 (service_ether_fs_type)clns_fastswitch,
			 "clns_fastswitch");
	reg_add_ether_fs(RXTYPE_SAP_CLNS,
			 (service_ether_fs_type)clns_fastswitch,
			 "clns_fastswitch");
	reg_add_fddi_fs(RXTYPE_CLNS,
			(service_fddi_fs_type)clns_fastswitch,
			"clns_fastswitch");
	reg_add_hdlc_fs(RXTYPE_CLNS,
			(service_hdlc_fs_type)clns_fastswitch,
			"clns_fastswitch");
	reg_add_aip_fs(RXTYPE_CLNS,
			(service_aip_fs_type)clns_fastswitch,
			"clns_fastswitch");
	reg_add_fast_fragmentation(RXTYPE_JUMBO_CLNP,
				   (service_fast_fragmentation_type)
				   clns_fast_fragmentation,
				   "clns_fast_fragmentation");
    } else {
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_CLNS);
	reg_add_fast_fragmentation(RXTYPE_JUMBO_CLNP,
				   (service_fast_fragmentation_type)
				   return_false, "return_false");
    }
}

static void clns_mci_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(clns_mci_fs_register_drivers,
			     "clns_mci_fs_register_drivers");
}

/*
 * MCI FS subsystem header
 */

#define CLNSMCIFS_MAJVERSION 1
#define CLNSMCIFS_MINVERSION 0
#define CLNSMCIFS_EDITVERSION  1

SUBSYS_HEADER(clns_mci_fs, CLNSMCIFS_MAJVERSION, CLNSMCIFS_MINVERSION, CLNSMCIFS_EDITVERSION,
	      clns_mci_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: clns", 
	      "req: fastswitch, clns");


/* end of clnsfast_mci.c */
