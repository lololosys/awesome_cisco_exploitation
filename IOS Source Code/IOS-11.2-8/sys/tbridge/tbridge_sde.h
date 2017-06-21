/*-------------------------------------------------------------------------
 *
 * $Id: tbridge_sde.h,v 3.1.62.2 1996/09/10 02:38:29 hou Exp $
 *
 * tbridge/tbridge_sde.h
 *
 * August 1994, Martin McNealis.
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Declarations for 802.10 Transparently Bridged vLANs/Secure Data Exchange.
 *
 *-------------------------------------------------------------------------
 * 
$Endlog$
 */



/*
 * Exported functions and external declarations.
 */

extern  void set_802_10_sde_encap (parseinfo *csb);
extern  void deencap_802_10_sde (idbtype *swidb, boolean sense);
extern  void dot10_sde_vlan_encaps_nvram_write (parseinfo *csb);
extern  boolean check_802_10_subif_bridging (spantype *span, boolean delete);
extern 	void sde_enqueue(paktype *pak);
extern  void tbridge_construct_hdlc_sde_bpdu_header(paktype *pak);
extern  boolean dot10_capable_subinterface (idbtype *input_swidb,
					    spantype *span, int index);

extern said_descriptor *said_table[];


/*
 *  Dimension of hashed array of Security Associations.
 */

#define  MAX_SAIDS       256

/*
 *  The SAID is encoded in 4 Bytes.
 */

#define  MAX_SAID_VALUE  0xFFFFFFFF

/*
 * IEEE 802.10 SDE (Secure Data Exchange) encapsulation.
 */

/* Clear header bytes */

# define SDE_CLR_HDR_BYTES    	7   	/* Clear header bytes */

/* Protected header bytes */

# define SDE_PT_SID_BYTES	8	/* Station ID bytes */
# define SDE_SAID_BYTES         4       /* Security Association ID */
# define SDE_PT_FLAG_BYTES	1	/* Flag byte */
# define SDE_PT_FID_BYTES	4	/* Fragmentation ID bytes */
# define SDE_PT_HDR_BYTES     	(SDE_PT_SID_BYTES + SDE_PT_FLAG_BYTES \
				 + SDE_PT_FID_BYTES)

/* 
 * Total SDE encapsulation bytes. Make the size an even number by adding 
 * some protected header fields. No Fragmentation ID since for now we don't 
 * support fragmentation.
 */
 
# define SDE_ENCAPBYTES       	(SDE_CLR_HDR_BYTES + SDE_PT_SID_BYTES \
				 + SDE_PT_FLAG_BYTES)
				 
# define SDE_ENCAPWORDS     	(SDE_ENCAPBYTES / 2)

/* returned from tbridge_parse_sde_header_inline */
enum SDE_RETVAL { 
    SDE_SUBIF_BRIDGE,  /* sde packet from sde sub interface, will be bridged */
    SDE_SUBIF_ROUTE,   /* sde packet from sde sub interface, will be routed */
    SDE_MAINIF_BRIDGE, /* sde packet from main interface, will be bridged,
                        * if the main i/f is configured as bridging 
                        */
    SDE_DISCARD        /* sde packet from main interface, will be discarded,
                        * if the main i/f isn't configured as bridging
                        */
};

/*
 * 802.10 SDE encapsulation header data stucture.
 */
typedef struct sdehdrtype_ {
    uchar sde_dsap;
    uchar sde_lsap;
    uchar sde_control;
    uchar sde_said[4];			/* Security Association ID */
    uchar sde_sid[8];                 	/* Station ID */
    uchar sde_flags;                	/* Flags */
    uchar sde_fid[4];               	/* Fragment ID */
} sdehdrtype;


/*
 * This is the 802.10 Security Association data structure.
 *
 * It holds the SAID `colour' field of the security association
 * and the sub-interfaces configured with that SAID.
 *
 * The SAID values are expected to be unique throughout the network.
 *
 * We maintain an array, indexed by hardware interface number, of the
 * software IDBs associated with this SAID value, so that when we match a
 * received 802.10 packet's SAID with those configured on the received
 * interface, we can then index the corresponding subinterface IDB - which
 * ultimately let's us get the packet to the correct bridge group !
 *
 * Note that there can only be one sub-interface per physical interface with a
 * given SAID; but multiple sub-interfaces (of different hardware idbs) can be
 * configured with the same SAID - for redundancy.
 *
 * Let's keep this long word aligned ...
 */

struct said_descriptor_ {

    ulong    sdb_said;                           /* Security Assocation ID. */
    uchar    sdb_fragmentation;                  /* Do we exceed the MTU ? */
    uchar    sdb_encryption;                     /* Packet data encrypted ? */
    uchar    sdb_integrity_check;                /* Has packet been modified */
    uchar    sdb_number_subif_in_bgroup;         /* Statistical count */
    idbtype  *sdb_sub_interface[MAX_INTERFACES]; /* Software IDBs associated
                                                    with this SDE entity */
    said_descriptor *next_said_db;               /* List of descriptors */

    ulong    ingressing_dot10_packets[VLAN_PROTOCOLS_MAX];
    ulong    egressing_dot10_packets[VLAN_PROTOCOLS_MAX];
};


