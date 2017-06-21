/* $Id: cfg_int_lapb.h,v 3.4.20.1 1996/03/18 22:49:31 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/x25/cfg_int_lapb.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ L A P B . H
 *
 * X.25 Level 2 support
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_lapb.h,v $
 * Revision 3.4.20.1  1996/03/18  22:49:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.4.2  1996/03/13  02:13:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.4.1  1996/02/20  21:54:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/02  17:07:07  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Incorporate code review comment.
 *
 * Revision 3.3  1996/01/15  04:47:57  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Dynamically update N1 value whenever there is configuration change on
 * hardware MTU, LAPB/X25 modulo, or lapb compression.
 *
 * Revision 3.2  1995/11/17  18:05:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:06:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * lapb hold-queue <number>
 * no lapb hold-queue
 */

PARAMS_KEYONLY(lapb_holdq, no_alt, "hold-queue",
	       OBJ(int,1), 0, 200,
	       lapb_encaps, LAPB_CMD_HOLDQ,
	       "Size of LAPB packet holding queue",
	       "Number of packets",
	       PRIV_CONF | PRIV_HIDDEN);
ASSERT	(lapb_holdqa, lapb_holdq, no_alt, !is_x25(csb->interface->hwptr));

/************************************************************************
 * (OBSOLETE, kept for backwards compatability)
 *	lapb protocol { apollo | appletalk | clns | decnet
 *			ip | novell | vines | xns }
 *	no lapb protocol
 *
 * OBJ(int,1) = LINK_...
 */

EOLS	(lapb_proto_eol, lapb_encaps, LAPB_CMD_PROTOCOL);
PROTONAME(lapb_proto_name, lapb_proto_eol, no_alt, OBJ(int,1),
	PMATCH_DGRAM PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP);
NOPREFIX(lapb_proto_noprefix, lapb_proto_name, lapb_proto_eol);
NVGENS	(lapb_proto_nvgen, lapb_proto_noprefix, lapb_encaps, LAPB_CMD_PROTOCOL);
KEYWORD(lapb_proto_kwd, lapb_proto_nvgen, NONE,
	"protocol", "Specify protocol to run over LAPB",
	PRIV_CONF | PRIV_HIDDEN |  PRIV_NONVGEN);
ASSERT	(lapb_proto, lapb_proto_kwd, lapb_holdqa,
	 (csb->interface->hwptr->enctype == ET_LAPB));

/************************************************************************
 * lapb K <number>
 * no lapb K
 */

PARAMS_KEYONLY(lapb_k, lapb_proto, "k",
	       OBJ(int,1), 1, 127,
	       lapb_encaps, LAPB_CMD_K,
	       "Maximum number of outstanding frames (window size)",
	       "LAPB k parameter (frames)",
	       PRIV_CONF);

/************************************************************************
 * lapb N2 <number>
 * no lapb N2
 */

PARAMS_KEYONLY(lapb_n2, lapb_k, "N2",
	       OBJ(int,1), 1, 255,
	       lapb_encaps, LAPB_CMD_N2,
	       "Maximum number of attempts to transmit a frame",
	       "LAPB N2 parameter (transmit count)",
	       PRIV_CONF);

/************************************************************************
 * lapb N1 <number>
 * no lapb N1
 */

EOLS	(lapb_n1_eol, lapb_encaps, LAPB_CMD_N1);
ASSERT	(lapb_n1_assert, lapb_n1_eol, no_alt, GETOBJ(int,1) % 8 == 0);

static void lapb_n1_range (parseinfo *csb, uint *lower, uint *upper)
{
    if (csb && csb->interface && csb->interface->hwptr) {
	*lower = lapb_min_n1(csb->interface->hwptr);
	*upper = LAPB_DEFAULT_N1(csb->interface->hwptr);
    }
}

NUMBER_FUNC(lapb_n1_num, lapb_n1_assert, no_alt, OBJ(int,1),
	    lapb_n1_range, "LAPB N1 parameter (bits; multiple of 8)");
NOPREFIX(lapb_n1_noprefix, lapb_n1_num, lapb_n1_eol);
NVGENS	(lapb_n1_nvgen, lapb_n1_noprefix, lapb_encaps, LAPB_CMD_N1);
KEYWORD	(lapb_n1, lapb_n1_nvgen, lapb_n2, "N1",
	 "Maximum number of bits per I-frame", PRIV_CONF);

/************************************************************************
 * lapb T4 <number>
 * no lapb T4
 */

PARAMS_KEYONLY(lapb_t4, lapb_n1, "T4",
	       OBJ(int,1), 0, 255, lapb_encaps, LAPB_CMD_T4,
	       "Keepalive timer", "LAPB T4 parameter (seconds)", PRIV_CONF);

/************************************************************************
 * lapb interface-outage <number>
 * no lapb interface-outage
 */

PARAMS_KEYONLY(lapb_int_outage, lapb_t4, "interface-outage",
	       OBJ(int,1), 0, 64000, lapb_encaps, LAPB_CMD_T3,
	       "Interface outage deadband (partial T3)",
	       "Allowable interface outage (milliseconds)", PRIV_CONF);

/************************************************************************
 * lapb T1 <number>
 * no lapb T1
 */

PARAMS_KEYONLY(lapb_t1, lapb_int_outage, "T1",
	       OBJ(int,1), 1, 64000, lapb_encaps, LAPB_CMD_T1,
	       "Retransmission timer", 
	       "LAPB T1 parameter (milliseconds)", PRIV_CONF);

/************************************************************************
 * lapb modulo {8 | 128}
 * no lapb modulo
 */

EOLS    (lapb_modulo_eol, lapb_encaps, LAPB_CMD_MODULO);
KEYWORD_ID(lapb_modulo_128, lapb_modulo_eol, no_alt,
	   OBJ(int,1), LAPB_MODULO_128,
	   "128", "Extended frame numbering", PRIV_CONF);
KEYWORD_ID(lapb_modulo_8, lapb_modulo_eol, lapb_modulo_128,
	   OBJ(int,1), LAPB_MODULO_8,
	   "8", "Standard frame numbering", PRIV_CONF);
NOPREFIX(lapb_modulo_noprefix, lapb_modulo_8, lapb_modulo_eol);
NVGENS  (lapb_modulo_nvgen, lapb_modulo_noprefix, lapb_encaps, LAPB_CMD_MODULO);
KEYWORD (lapb_modulo_kwd, lapb_modulo_nvgen, lapb_t1,
	 "modulo", "Set frame numbering modulus", PRIV_CONF);

/************************************************************************
 * lapb ...
 */

KEYWORD	(lapb_kwd, lapb_modulo_kwd, NONE,
	 "lapb", "X.25 Level 2 parameters (Link Access Procedure, Balanced)",
	 PRIV_CONF);

TEST_IDBSTATUS(lapb_test, lapb_kwd, NONE, ALTERNATE, IDB_SERIAL);


#undef	ALTERNATE
#define	ALTERNATE	lapb_test
