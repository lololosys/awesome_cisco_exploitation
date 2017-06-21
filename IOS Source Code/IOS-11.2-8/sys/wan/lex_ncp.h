/* $Id: lex_ncp.h,v 3.2 1995/11/17 18:03:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/lex_ncp.h,v $
 *------------------------------------------------------------------
 * lex_ncp.h - LEX Control Protocol definitions.
 *
 * 01-March-1994, David Coli
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lex_ncp.h,v $
 * Revision 3.2  1995/11/17  18:03:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:18:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LEX_NCP_H__
#define __LEX_NCP_H__

/*
 * LEX Options.
 */
#define CI_MAC_SUPPORT    1	/* 802.3/4/5 canonical/non-canonical addr */
#define CI_TINYGRAM       2	/* Tinygram compression-pad byte removal */
#define CI_MAC_ADDRESS    3	/* MAC address in 802.1 canonical format */
#define CI_SPANNING_TREE  4	/* Spanning Tree protocol */
#define CI_MAC_RELAY      5	/* MAC relay mode */


/*
 *  Option Lengths
 */
#define PPP_TYPE_LEN_SIZE   2   /* Size in bytes of PPP TYPE + LENGTH fields */
#define LENCI_MAC_SUPPORT   3 	/* times number of MAC types supported */
#define LENCI_TINYGRAM      3
#define LENCI_MAC_ADDRESS   8
#define LENCI_SPANNING_TREE 2	/* + 1 for each protocol supported */
#define LENCI_MAC_RELAY     3

/*
 * Protocol header field size defines.
 * Don't want to use sizeof() which is not portable.
 */
#define BCP_TYP_LEN_FLD_SIZE 2  /* Size of BCP TYPE + LENGTH fields */
#define TINYGRAM_VAL_SIZE    (LENCI_TINYGRAM - BCP_TYP_LEN_FLD_SIZE)
#define MAC_SUPPORT_VAL_SIZE (LENCI_MAC_SUPPORT - BCP_TYP_LEN_FLD_SIZE)
#define MAX_SPANNING_TREE_VAL_SIZE 4
#define MAC_RELAY_VAL_SIZE   (LENCI_MAC_RELAY - BCP_TYP_LEN_FLD_SIZE)


/*
 * MAC types supported
 */
#define CI_IEEE_8023_CAN       1
#define CI_IEEE_8024_NCAN      2
#define CI_IEEE_8025_NCAN      3
#define CI_FDDI_NCAN           4
#define CI_IEEE_8024_CAN       10
#define CI_IEEE_8025_CAN       11
#define CI_FDDI_CAN            12

#define CI_MAX_MACTYPE         7

/*
 * Tinygram Compression states
 */
#define CI_TINYGRAM_ENABLED   1
#define CI_TINYGRAM_DISABLED  2

/* 
 * Spanning Tree Protocols supported
 */
#define CI_SPAN_NONE    0   /* No spanning tree protocol supported */
#define CI_SPAN_8021D   1   /* IEEE 802.1D spanning tree */
#define CI_SPAN_8021G   2   /* IEEE 802.1G extended spanning tree */
#define CI_SPAN_IBM_SR  3   /* IBM Source Route spanning tree */
#define CI_SPAN_DEC     4   /* DEC LANbridge 100 spanning tree */

#define CI_MAX_ST CI_SPAN_DEC

#define CI_MACRELAY_PROTO_SUP 1 /* MAC relay protocol version supported */

typedef struct lex_options {
    boolean neg_mac_support;	/* Negotiate MAC support? */
    boolean neg_tinygram_comp;	/* Negotiate tinygram compression? */
    boolean neg_mac_address;	/* Negotiate MAC address? */
    boolean neg_spanning_tree;	/* Negotiate spanning tree protocol? */
    boolean neg_mac_relay;      /* Negotiate MAC relay option? */
    short mac_support_count;	/* Number of MAC types supported */
    ushort mac_types[CI_MAX_MACTYPE];/* Traffic types which you can receive */
    uchar tinygram_state;	/* Enabled(1)/Disabled(2) */
    uchar mac_address[IEEEBYTES];
    uchar span_protos[CI_MAX_ST];/* Each uchar can specify a supported proto */
    short span_proto_count;	/* Number of spanning tree protos supported */
    uchar mac_relay_version;	/* protocol version supported */
} lex_options;

void close_a_lex_ncp (hwidbtype *);     /* Used in delete lex idb*/
#endif
