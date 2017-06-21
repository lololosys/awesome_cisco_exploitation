/* $Id: atm_dxi.h,v 3.5.12.1 1996/03/18 22:46:51 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/wan/atm_dxi.h,v $
 *------------------------------------------------------------------
 * Support for ATM-DXI encapsulation and RFC1483 support
 *
 * October 1994, Shankar Natarajan
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_dxi.h,v $
 * Revision 3.5.12.1  1996/03/18  22:46:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.2  1996/03/07  11:06:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.22.1  1996/02/20  21:49:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  04:22:00  shankar
 * CSCdi48025:  DXI ATM - DXI MAP DISSAPEARS AFTER RELOAD
 *
 * Revision 3.4  1995/11/29  18:44:06  shankar
 * CSCdi41439:  ags crash during atm-dxi test. Part II.
 *
 * Revision 3.3  1995/11/17  18:00:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:47:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/15  02:14:35  dcoli
 * CSCdi39433:  Bad Encapulation Error
 * Break frame_relay and atm_dxi into separate object modules.
 * Make link2nlpid and nlpid2link inline functions so that they can be
 * shared without the overhead of a registry call.
 *
 * Revision 2.1  1995/06/07  23:16:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __ATM_DXI_H__
#define __ATM_DXI_H__
#include "address.h"
#include <string.h>
#include <ciscolib.h>

extern uchar const apple_snap[];
extern uchar const tb1490_snap[];

# define ATM_DXI_SNAPENCAPBYTES 	10
# define ATM_DXI_BRIDGE_ENCAPBYTES 	12
# define ATM_DXI_MUXENCAPBYTES 		2
# define ATM_DXI_ENCAPBYTES 		4
# define DXI_SNAP_NLPID             	0x0300
# define DXI_SNAP_CONTROL           	0x80
# define DXI_NON_ISO_LLC_SNAP		0xAAAA
# define DXI_ISO_LLC			0xFEFE
# define ATM_DXI_ISOPDU_HDRBYTES        5
# define ADDRLEN_DXI			2
# define ATM_DXI_SNAP1PAD    		0x0300
# define ATM_DXI_SNAP0PAD    		0x0380
# define ATM_DXI_EA_BITS 		0x0001
# define DFA_DELETED 			4
# define DFA_ACTIVE 			2
# define IS_DFA_ACTIVE(x)		(x & 2)
# define IS_DFA_DELETED(x) 		(x & 4)
# define RELAYHASH 			64
# define MIN_DFA 			0
# define MAX_DFA 			1023
# define MAXDFA_VALUES	    		1024
# define DXI_PVC_LOCAL			0
# define DXI_PVC_STATIC			0
# define DXI_1483_ETHER_BRIDGE          0x0007
# define DXI_IEEE_DEC_BPDU              0x000E

# define NLPID_DEF(type) 		((type == LINK_IP) || \
					(type == LINK_CLNS))

typedef struct atm_dxi_hdr_ {
    ushort 	dfa;			/* DXI Frame address */
    uchar  	snap_start[0];		/* provides offset only */
    ushort 	type;			/* protocol type */
    uchar  	data[0];		/* Data starts here... */
} atm_dxi_hdr;

typedef struct atm_dxi_llc_header_ {
    ushort 	dfa;                    /* DXI Frame address */
    uchar  	llc_start[0];		/* provides offset only */
    uchar  	snap_start[0];		/* provides offset only */
    ushort 	llc;                    /* Start of LLC header */
    ulong 	snap_ctrl_oui[0];    	/* offset to read ctrl and oui */
    uchar  	cntrl;                  /* Control byte, 0x03 */
    uchar  	iso_pdu_start[0];     	/* provides offset */
    uchar  	oui[3];                 /* 0x000000 */
    ushort 	etype;                  /* Ethertype */
    uchar  	snap_data_start[0];	/* Start of SNAP data */
} atm_dxi_llc_hdr;

typedef struct atm_dxi_bridge_hdr_ {
    struct 	atm_dxi_llc_header_ atm_dxi_bridge;
					/* Bridge header resembles LLC snap */
    ushort 	pad_byte;               /* Padbyte */
    uchar  	bridge_data_start[0];   /* Start of bridge frame*/
} atm_dxi_bridge_hdr;

enum DXI_MAP_TYPE {
    DXI_MAP_SNAP = 1,
    DXI_MAP_NLPID,
    DXI_MAP_MUX 
}; 

typedef struct dxi_pvc_stats_type_ {
    ulong num_input_pkts;      	/* pkts received on serial */
    ulong num_output_pkts;     	/* pkts sent out over serial */
    ulong num_bytes_rcv;       	/* no of bytes received on serial */
    ulong num_bytes_send;      	/* total no of bytes sent over serial  */
    ulong num_fast_input_pkts;  /* pkts received on serial */
    ulong num_fast_output_pkts; /* pkts sent out over serial */
    ulong num_fast_bytes_rcv;   /* total no of bytes received on serial */
    ulong num_fast_bytes_send;  /* total no of bytes sent over serial */
    ulong num_dropped_pkts;    	/* no of dropped packets */
} dxi_pvc_stats_type;

typedef struct atm_dxi_map_ {
    struct atm_dxi_map_ *next; 		/* Pointer to next map */
    ushort 		dfa;		/* DXI Frame address */
    ushort 		dfa_num;	/* equivalent of DLCI */
    idbtype 		*idb;		/* Pointer to swidb */
    uint 		link;		/* linktype, LINK_IP etc. */
    boolean 		broadcast;	/* carry broadcast traffic ? */
    addrtype 		PACKED(proto_addr);	
					/* Protocol address */
} dximaptype;

struct dxi_pvc_info_type_ {
    ushort 		dfa;
    ushort 		map_flag;	/* map exists or not */
    dxi_pvc_stats_type 	pvc_stats; 	/* statistics info for the PVC */
    short 		use_count;	/* number of refs by maps to this PVC */
    ushort 		pvc_type;	/* dynamic or static */
    idbtype 		*idb;		/* software idb pointer */
    boolean 		configured;	/* explicitly configured */
    boolean 		dedicated_pvc;	/* PVC dedicated to a protocol */
    uint 		linktype;	/* carried protocol */
    enum DXI_MAP_TYPE 	encaps_type; 	/* encapsulation type*/
    enum DXI_MAP_TYPE 	rxtype;      	/* for fast switching */
    uchar 		pvc_status;	/* status of this PVC active/inactive */
};

struct dxi_idb_struct_type_ {
    dxi_pvc_info_type 	*dxi_dfa_tab[MAXDFA_VALUES];	/* table of PVCs */
    ulong 		fast_input_encsize; 	/* high end fast switching */
    ulong 		fast_input_dfa;		/* high end fast switching */
    ushort 		PACKED(dxi_err_dfa);  		
						/* DFA of last error pkt */
};

/*
 * DFA map structure
 * Fallout from the frame relay code. Ugly but works!
 * In order to understand the mumbo-jumbo, please look at a 
 * Frame Relay code or a ATM-DXI spec.
 */
#define NUM_TO_DFA(x) (((x & 0x03F0) << 6 ) | ((x & 0x0F) << 4 ))
#define DFA_TO_NUM(x) (((x & 0xfc00) >> 6) | ((x & 0xf0) >> 4))
#define BR_NUM_TO_DFA(x,y) (((x & 0x03F0) << 6 ) | ((x & 0x0F) << 4 ))
#define BR_DFA_TO_NUM(x,y) (((x & 0xFC00 ) >> 6) | ((x & 0xF0) >> 4))

#define HWIDB_TO_DXI_PVC(hwidb,num) hwidb->atm_dxi_stuff->dxi_dfa_tab[num]
static inline ulong dxi_pvc_input_pkts (dxi_pvc_stats_type *stats)
{
    return(stats->num_input_pkts + stats->num_fast_input_pkts);
}

static inline ulong dxi_pvc_output_pkts (dxi_pvc_stats_type *stats)
{
    return(stats->num_output_pkts + stats->num_fast_output_pkts);
}

static inline ulong dxi_pvc_input_bytes (dxi_pvc_stats_type *stats)
{
    return(stats->num_bytes_rcv + stats->num_fast_bytes_rcv);
}

static inline ulong dxi_pvc_output_bytes (dxi_pvc_stats_type *stats)
{
    return(stats->num_bytes_send + stats->num_fast_bytes_send);
}

static inline void clear_dxi_pvc_stats (dxi_pvc_stats_type *pvc)
{
    memset(pvc, 0,sizeof(dxi_pvc_stats_type));
}

static inline void INC_DXI_PVC_FAST_INPUT (dxi_pvc_info_type *pvc,
					   ushort bytes)
{
    pvc->pvc_stats.num_fast_input_pkts++;
    pvc->pvc_stats.num_fast_bytes_rcv += bytes;
}

static inline void INC_DXI_PVC_FAST_OUTPUT (dxi_pvc_info_type *pvc,
					    ushort bytes)
{
    if (pvc) {
	pvc->pvc_stats.num_fast_output_pkts++;
	pvc->pvc_stats.num_fast_bytes_send += bytes;
    }
}
/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * atm_dxi.c
 */
extern boolean dxi_vencapsulate(paktype *, long);
extern boolean dxi_encaps(paktype *, long, dximaptype *);
extern boolean dxi_doencaps (idbtype *, paktype *, dximaptype *);
extern dximaptype *dxi_addr_enter(idbtype *, addrtype *, ushort, boolean, uint);
extern dximaptype *dxi_broadcast(idbtype *, paktype *, uint);
extern dximaptype *dxi_addr_lookup(idbtype *, addrtype *, uint);
extern long dxi_getlink(paktype *);
extern dxi_pvc_info_type *dxi_pvc_init(idbtype *, ushort, boolean);
extern void dxi_pvc_clear(hwidbtype *, ushort);
extern void dxi_clear_map(idbtype *);
extern void dxi_display_map(dximaptype *);
extern void dxi_display_int_dfa (dximaptype *);
extern void dxi_hash_init(void);
extern void dxi_input(hwidbtype *, paktype *);
extern void dxi_map_delete(dximaptype *);
extern void dxi_parse_map(parseinfo *, idbtype *);
extern void show_atm_dxi_map(void);
extern void dxi_cstate(hwidbtype *);
extern void dxi_parse_int_dfa(idbtype *, ushort, ushort, boolean, ushort, boolean);
extern void atm_dxi_parser_init(void);
extern void dxi_encaps_nvram_write(parseinfo *); 
extern void dxi_encaps_show_protocol_attr(idbtype *); 
extern void dxi_encaps_set_protocol_attr(idbtype *); 
extern void dxi_debug_pak(paktype *, char *);
extern dxi_pvc_info_type * create_dxi_pvc(void);
extern void dxi_snap_encaps(paktype *, ushort);
extern void dxi_nlpid_encaps(paktype *, ushort);
extern void show_dxi_idb_stats (hwidbtype *, ushort );
extern void display_dxi_pvc_stats(dxi_pvc_info_type *, ushort, boolean);
extern void show_dxi_pvc_stats (parseinfo *);
extern ushort vpivci_to_dfa_conversion(ushort, ushort);
extern void dxi_parse_pvc_command(parseinfo *, idbtype *, ushort, 
						ushort, ushort, boolean);
extern void dxi_set_rxtype(dxi_pvc_info_type *); 
#endif __ATM_DXI_H__
