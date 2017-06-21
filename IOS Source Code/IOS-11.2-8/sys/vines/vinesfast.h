/* $Id: vinesfast.h,v 3.2.62.3 1996/08/13 14:34:54 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vines/vinesfast.h,v $
 *------------------------------------------------------------------
 * vinesfast.h -- VINES Fast Switching definitions.
 *
 * July 1992, David R. Hampton
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vinesfast.h,v $
 * Revision 3.2.62.3  1996/08/13  14:34:54  rbadri
 * CSCdi64089:  Multiprotocol support fixes required
 * Branch: California_branch
 * Vines support for LANE
 *
 * Revision 3.2.62.2  1996/07/17  00:02:57  ehk
 * CSCdi59059:  Out of order packets when using vines route-cache
 * Branch: California_branch
 * invalidate a fast cache entry before passing to process switching
 * a packet with metric notification bit set and relearn a new cache
 * to maintain original packet ordering.
 *
 * Revision 3.2.62.1  1996/05/17  12:15:46  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/05/02  22:08:20  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:59:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:34:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:15:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * VINES routing cache data structure and definitions.
 */
#define VINES_HASHLEN	256			/* number of hash entries */
#define MAXMAC_BYTES	60			/* max size of mac header */
#define MAXMAC_LONGS	(btol(MAXMAC_BYTES))
#define MAXMAC_SHORTS	(btow(MAXMAC_BYTES))
#define RCACHE_TIMEOUT	(15*ONEMIN)		/* cache entry timeout */
#define RCACHE_DEPTH	3 

#define V_FC_VALID	0x00000001

#define FOR_ALL_CACHE_BUCKETS(bucket) \
    for ((bucket) = 0; (bucket) < VINES_HASHLEN; (bucket)++)
#define FOR_ALL_BUCKET_ENTRIES(bucket, entry) \
    for ((entry) = vinescache[(bucket)]; (entry); (entry) = (entry)->next_entry) 
#define FOR_ALL_BUCKET_ENTRIES_W_PREV(bucket, entry, prev) \
    (prev) = (vinescachetype *)&vinescache[(bucket)]; \
    for ((entry) = vinescache[(bucket)]; (entry); (prev) = (entry), (entry) = (entry)->next_entry) 

/*
 * Output encapsulations for vines
 */
typedef enum VFAST_TYPE_ {
 V_FS_ETHER_SNAP  =  0,
 V_FS_ETHER_ARPA  =  1,
 V_FS_TOKEN_SNAP  =  2,
 V_FS_TOKEN_VINES =  3,
 V_FS_FDDI        =  4,
 V_FS_HDLC        =  5,
 V_FS_FRAME       =  6,
 V_FS_FRAME_IETF  =  7,
 V_FS_SMDS_CISCO  =  8,
 V_FS_SMDS_DXI    =  9,
 V_FS_PPP         = 10,
 V_FS_ATM         = 11,
 V_FS_ATM_DXI	  = 12,
 V_FS_TUNNEL	  = 13,
 V_FS_MAX         = 14,
 V_FS_UNKNOWN     = 15
} VFAST_TYPE;

typedef enum vfast_entry_type_ {
    VF_CLIENT = 0,
    VF_SERVER,
    VF_ROUTER,
    VF_LAST_TYPE
} vfast_entry_type;

struct vinesfastpathtype_ {
    struct vinesfastpathtype_ *next_path;	/* pointer to next path */
    idbtype *idb;				/* interface to be used */
    union {
	uchar  mac_bytes[MAXMAC_BYTES];
	ulong  mac_longs[MAXMAC_LONGS];
	ushort mac_shorts[MAXMAC_SHORTS];
    } vinesp_mh;				/* mac header itself */
    VFAST_TYPE encaptype;			/* encaps used in this entry */
    ulong      length;				/* stored length */
    ulong      reallength;			/* including AC, FC, etc. */
    ushort     wordlength;
    ushort     rifwordlength;
    ushort     subint_reference_num;		/* for pvc accounting */
    ushort     dummy;
    vinesneighbortype *neighbor;		/* for finding by next hop */
};

typedef struct vinescachetype_ {
    struct vinescachetype_ *next_entry;		/* pointer to next entry */
    ulong  flags;				/* valid flag */
    vinesfastpathtype *all_paths;		/* all paths */
    vinesfastpathtype *next_path;		/* next path to use */
    ushort path_count;				/* number of paths */
    ulong  net;					/* destination net */
    ushort host;				/* destination host */
    sys_timestamp lastupdate;			/* last updated time in ms */ 
    vfast_entry_type entry_type;
} vinescachetype;



typedef struct shortlong_ {
    union {
	ulong lword;
	ushort sword[2];
    } d;
} shortlong;

/*
 * Layout of datagram sniff buffer
 *
 *
 *	    ETHERNET
 *	    ARPA	      ISO2		 HDLC
 *
 *	    D  D  D  D	      D	 D  D  D
 *	    D  D  S  S	      D	 D  S  S
 *	    S  S  S  S	      S	 S  S  S	 x  x  x  x 
 *	    P  P  x  x	      L	 L  x  x	 x  x  x  x 
 *	    x  x  x  x	      DS SS CT SN	 x  x  x  x 
 *	    x  x  x  x	      SN SN P  P	 B  B  P  P 
 *   1	    -- -- -- --	      -- -- -- --	 -- -- -- --
 *   2	    CK CK LN LN	      CK CK LN LN	 CK CK LN LN
 *   3	    TC PT x  x	      TC PT x  x	 TC PT x  x 
 *   4	    dn dn dn dn	      dn dn dn dn	 dn dn dn dn
 *   5	    dh dh x  x	      dh dh x  x	 dh dh x  x 
 *   6	    x  x  x  x	      x	 x  x  x	 x  x  x  x 
 *   7	    x  x  x  x	      x	 x  x  x
 *   8	    x  x  x  x	      x	 x  x  x
 *
 *	    TOKEN RING
 *	    ISO1	      ISO2
 *
 *	    D  D  D  D	      D	 D  D  D 
 *	    D  D  S  S	      D	 D  S  S 
 *	    S  S  S  S	      S	 S  S  S 
 *	    DS SS x  x	      DS SS x  x 
 *	    x  x  x  x	      DS SS CT SN
 *	    x  x  CT DX	      SN SN P  P 
 *   1	    -- -- -- --	      -- -- -- --
 *   2	    CK CK LN LN	      CK CK LN LN
 *   3	    TC PT x  x	      TC PT x  x 
 *   4	    dn dn dn dn	      dn dn dn dn
 *   5	    dh dh x  x	      dh dh x  x 
 *   6	    x  x  x  x	      x	 x  x  x 
 *   7	    x  x  x  x	      x	 x  x  x 
 *   8	    x  x  x  x	      x	 x  x  x 
 *
 */
/*
 * Definitions for the fields of the hwidb sniff buffer that will be
 * used.  This makes it much easier to find data once it has been read in
 * to the buffer.
 */
#define fast_net_start	  sniff[SNIFFBASE+2].d.lword

#define checksum_length	  sniff[SNIFFBASE+3].d.lword
#define checksum_only	  sniff[SNIFFBASE+3].d.sword[0]
#define length_only	  sniff[SNIFFBASE+3].d.sword[1]
#define hops_ptype	  sniff[SNIFFBASE+4].d.sword[0]
#define destination_net	  sniff[SNIFFBASE+5].d.lword
#define destination_net_h sniff[SNIFFBASE+5].d.sword[0]
#define destination_net_l sniff[SNIFFBASE+5].d.sword[1]
#define destination_host  sniff[SNIFFBASE+6].d.sword[0]

#define smds_lastlong	  sniff[SNIFFBASE+9].d.lword

/* For low end.  Found on the pak structure */
#define vinesipstart	  dataptr

#define VINES_OFFSETB_SNET	12	/* 12 bytes */
#define VINES_OFFSETW_SNET	6	/* 6 words */
#define ISO2_OVERHEAD		3	/* 3 words */

#define E_SNAP_HDR_BYTES_IN	(ETHER_SNAP_ENCAPBYTES)
#define E_ARPA_HDR_BYTES_IN	(ETHER_ARPA_ENCAPBYTES)
#define F_SNAP_HDR_BYTES_IN	(FDDI_SNAP_ENCAPBYTES + input->rxrifwordlen*2)
#define T_SNAP_HDR_BYTES_IN	(TRING_SNAP_ENCAPBYTES + input->rxrifwordlen*2)
#define T_VINES_HDR_BYTES_IN	(VINES_TRING_SAP_ENCAPBYTES + input->rxrifwordlen*2)
#define HDLC_HDR_BYTES_IN	(HDLC_ENCAPBYTES)
#define FRAMEC_HDR_BYTES_IN	(FRAME_RELAY_ENCAPBYTES)
#define FRAMEI_HDR_BYTES_IN	(FRAME_RELAY_SNAPENCAPBYTES)
#define SMDS_HDR_BYTES_IN	(sizeof(smdstype) + sizeof(snap_hdr))
#define	ATM_HDR_BYTES_IN	(vc->encapsize)

#define E_SNAP_HDR_WORDS_IN	(ETHER_SNAP_ENCAPBYTES >> 1)
#define E_ARPA_HDR_WORDS_IN	(ETHER_ARPA_ENCAPBYTES >> 1)
#define F_SNAP_HDR_WORDS_IN     (FDDI_SNAP_ENCAPBYTES/2 + input->rxrifwordlen + 1)
#define T_SNAP_HDR_WORDS_IN	(TRING_SNAP_ENCAPBYTES/2 + input->rxrifwordlen)
#define T_VINES_HDR_WORDS_IN	(VINES_TRING_SAP_ENCAPBYTES/2 + input->rxrifwordlen)
#define HDLC_HDR_WORDS_IN	(HDLC_ENCAPBYTES >> 1)
#define FRAMEC_HDR_WORDS_IN	(FRAME_RELAY_ENCAPBYTES >> 1)
#define FRAMEI_HDR_WORDS_IN	(FRAME_RELAY_SNAPENCAPBYTES >> 1)
#define SMDS_HDR_WORDS_IN	btow(SMDS_HDR_BYTES_IN)
#define	ATM_HDR_WORDS_IN	(vc->encapsize >> 1)

#define E_SNAP_OFFSETW_HOPS	(E_SNAP_HDR_WORDS_IN + VINES_OFFSETW_HOPS)
#define E_ARPA_OFFSETW_HOPS	(E_ARPA_HDR_WORDS_IN + VINES_OFFSETW_HOPS)
#define F_SNAP_OFFSETW_HOPS	(F_SNAP_HDR_WORDS_IN + VINES_OFFSETW_HOPS)
#define T_SNAP_OFFSETW_HOPS	(T_SNAP_HDR_WORDS_IN + VINES_OFFSETW_HOPS)
#define T_VINES_OFFSETW_HOPS	(T_VINES_HDR_WORDS_IN + VINES_OFFSETW_HOPS)
#define HDLC_OFFSETW_HOPS	(HDLC_HDR_WORDS_IN + VINES_OFFSETW_HOPS)
#define SMDS_OFFSETW_HOPS	(SMDS_HDR_WORDS_IN + VINES_OFFSETW_HOPS)
#define ATM_OFFSETW_HOPS	(ATM_HDR_WORDS_IN + VINES_OFFSETW_HOPS)

#define E_SNAP_OFFSETB_DNET	(E_SNAP_HDR_BYTES_IN + VINES_OFFSETB_DNET)
#define E_ARPA_OFFSETB_DNET	(E_ARPA_HDR_BYTES_IN + VINES_OFFSETB_DNET)
#define F_SNAP_OFFSETB_DNET	(F_SNAP_HDR_BYTES_IN + VINES_OFFSETB_DNET)
#define T_SNAP_OFFSETB_DNET	(T_SNAP_HDR_BYTES_IN + VINES_OFFSETB_DNET)
#define T_VINES_OFFSETB_DNET	(T_VINES_HDR_BYTES_IN + VINES_OFFSETB_DNET)
#define HDLC_OFFSETB_DNET	(HDLC_HDR_BYTES_IN + VINES_OFFSETB_DNET)
#define SMDS_OFFSETB_DNET	(SMDS_HDR_BYTES_IN + VINES_OFFSETB_DNET)
#define ATM_OFFSETB_DNET	(ATM_HDR_BYTES_IN + VINES_OFFSETB_DNET)

#define E_SNAP_OFFSETW_DNET	(E_SNAP_HDR_WORDS_IN + VINES_OFFSETW_DNET)
#define E_ARPA_OFFSETW_DNET	(E_ARPA_HDR_WORDS_IN + VINES_OFFSETW_DNET)
#define F_SNAP_OFFSETW_DNET	(F_SNAP_HDR_WORDS_IN + VINES_OFFSETW_DNET)
#define T_SNAP_OFFSETW_DNET	(T_SNAP_HDR_WORDS_IN + VINES_OFFSETW_DNET)
#define T_VINES_OFFSETW_DNET	(T_VINES_HDR_WORDS_IN + VINES_OFFSETW_DNET)
#define HDLC_OFFSETW_DNET	(HDLC_HDR_WORDS_IN + VINES_OFFSETW_DNET)
#define SMDS_OFFSETW_DNET	(SMDS_HDR_WORDS_IN + VINES_OFFSETW_DNET)
#define ATM_OFFSETW_DNET	(ATM_HDR_WORDS_IN + VINES_OFFSETW_DNET)

#define E_SNAP_OFFSETB_SNET	(E_SNAP_HDR_BYTES_IN + VINES_OFFSETB_SNET)
#define E_ARPA_OFFSETB_SNET	(E_ARPA_HDR_BYTES_IN + VINES_OFFSETB_SNET)
#define F_SNAP_OFFSETB_SNET	(F_SNAP_HDR_BYTES_IN + VINES_OFFSETB_SNET)
#define T_SNAP_OFFSETB_SNET	(T_SNAP_HDR_BYTES_IN + VINES_OFFSETB_SNET)
#define T_VINES_OFFSETB_SNET	(T_VINES_HDR_BYTES_IN + VINES_OFFSETB_SNET)
#define HDLC_OFFSETB_SNET	(HDLC_HDR_BYTES_IN + VINES_OFFSETB_SNET)
#define SMDS_OFFSETB_SNET	(SMDS_HDR_BYTES_IN + VINES_OFFSETB_SNET)
#define ATM_OFFSETB_SNET	(ATM_HDR_BYTES_IN + VINES_OFFSETB_SNET)

#define E_SNAP_OFFSETW_SNET	(E_SNAP_HDR_WORDS_IN + VINES_OFFSETW_SNET)
#define E_ARPA_OFFSETW_SNET	(E_ARPA_HDR_WORDS_IN + VINES_OFFSETW_SNET)
#define F_SNAP_OFFSETW_SNET	(F_SNAP_HDR_WORDS_IN + VINES_OFFSETW_SNET)
#define T_SNAP_OFFSETW_SNET	(T_SNAP_HDR_WORDS_IN + VINES_OFFSETW_SNET)
#define T_VINES_OFFSETW_SNET	(T_VINES_HDR_WORDS_IN + VINES_OFFSETW_SNET)
#define HDLC_OFFSETW_SNET	(HDLC_HDR_WORDS_IN + VINES_OFFSETW_SNET)
#define SMDS_OFFSETW_SNET	(SMDS_HDR_WORDS_IN + VINES_OFFSETW_SNET)
#define ATM_OFFSETW_SNET	(ATM_HDR_WORDS_IN + VINES_OFFSETW_SNET)

#define E_SNAP_HDR_BYTES_OUT	(ETHER_SNAP_ENCAPBYTES)
#define E_ARPA_HDR_BYTES_OUT	(ETHER_ARPA_ENCAPBYTES)
#define F_SNAP_HDR_BYTES_OUT	(path->reallength)
#define T_SNAP_HDR_BYTES_OUT	(path->reallength)
#define T_VINES_HDR_BYTES_OUT	(path->reallength)
#define HDLC_HDR_BYTES_OUT	(HDLC_ENCAPBYTES)
#define PPP_HDR_BYTES_OUT	(PPP_ENCAPBYTES)
#define SMDS_HDR_BYTES_OUT	(path->reallength)
#define ATM_HDR_BYTES_OUT	(path->reallength)

#define E_SNAP_HDR_WORDS_OUT	(ETHER_SNAP_ENCAPBYTES >> 1)
#define E_ARPA_HDR_WORDS_OUT	(ETHER_ARPA_ENCAPBYTES >> 1)
#define F_SNAP_HDR_WORDS_OUT    (path->wordlength)
#define T_SNAP_HDR_WORDS_OUT	(path->wordlength)
#define T_VINES_HDR_WORDS_OUT	(path->wordlength)
#define HDLC_HDR_WORDS_OUT	(HDLC_ENCAPBYTES >> 1)
#define SMDS_HDR_WORDS_OUT	(path->wordlength)
#define ATM_HDR_WORDS_OUT	(path->wordlength)


#define V_SMDS_SETUP                                                    \
    pad = 3 - ((input->length_only + 3) % 4);	/* 0-3 bytes of pad */	\
    basize = input->length_only + SMDS_BASIZEBYTES + SNAP_HDRBYTES + pad; \
    size_to_xmit += pad + SMDS_TRAILER;					\
									\
    {									\
        ulong offset = path->length - SNAP_HDRBYTES - SMDS_SMDSBYTES;	\
        sm = (smdstype *)&path->vinesp_mh.mac_bytes[offset];		\
    }									\
    sm->smds_xhlpi &= ~3;						\
    sm->smds_xhlpi |= pad;						\
    sm->smds_betag = output->smds_begin_tag++;				\
    sm->smds_basize = basize;						\


/***********************************************************************
 *
 *			Externs and Prototypes
 *
 ***********************************************************************/

/*
 * VINES Cache table
 */
extern ulong vcache_locked;
extern vinescachetype *vinescache[VINES_HASHLEN];

/*
 * VINES Function Declarations
 *     vines/vinesfast.c
 */

/* vinesfast.c */
extern void vines_cache_init(void);
extern void vines_free_entry(vinescachetype *, vinescachetype *);
extern void vines_flush_cache_by_idb(hwidbtype *);
extern void vines_flush_cache_entry(ulong, ushort);
extern void vines_invalidate_cache_entry(ulong, ushort);
extern void vines_cache_update(paktype *);
extern void vinescache_ager(boolean);
extern void show_vines_cache(ulong,ushort,vinesneighbortype *, vinesidbtype *);
extern void vines_flush_cache_by_server(vinesnettype *);
extern void vines_flush_cache_by_neighbor(vinesneighbortype *);
extern void vines_bump_cache(hwidbtype *, idbtype *, boolean);
extern void vcache_clear_counters(void);

/* vinesfast_xxx.c */
extern void vines_clear_fs_tables(void);
extern void vines_setup_fs_tables(void);
