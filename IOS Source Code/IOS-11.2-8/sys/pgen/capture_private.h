/* $Id: capture_private.h,v 3.2.60.2 1996/05/17 11:40:23 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/pgen/capture_private.h,v $
 *------------------------------------------------------------------
 *
 * Declarations for things that are shared among packet capture
 * modules, but not visible outside the packet generator
 *
 * Jan 1995, skoh
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: capture_private.h,v $
 * Revision 3.2.60.2  1996/05/17  11:40:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.46.2  1996/05/05  23:43:57  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.2.46.1  1996/05/02  22:06:58  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.60.1  1996/04/27  06:41:35  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.18.1  1996/04/26  01:04:11  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.2  1995/11/17  18:55:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/09  00:21:43  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.1  1995/06/07  22:33:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CAPTURE_FLAG	ppp_bridge
#define CAPTURE_Q	lapb_holdq


#define CAPTURE_SAVED_IQUEUE vpn_save_iqueue /* copy of orig. idb vector */

#define CAPTURE_DEFAULT_PKT 10	/* default # of pkt's to be captured */
#define CAPTURE_DISPLAY_COLUMNS 16	/* display 16 colums a line */

#define CAPTURE_MOST_SIGN_WORD 0xffff0000		/* max. int */
#define CLEAR_CAPTURE 2       /* Used for csb->which in the parser */
/*
 * function prototypes
 */
extern void capture_parser_init (void);

extern void capture_enqueue (hwidbtype *, paktype *);
extern void capture_command (parseinfo *);
extern void show_capturedinfo (parseinfo *);
extern void capture_dump_commands (parseinfo *);
extern void capture_clear_command (parseinfo *);


#define CAPTURE_SUPPORTED_IDBTYPE(hwidb)                \
                 ((hwidb)->type == IDBTYPE_MCIETHER ||  \
                  (hwidb)->type == IDBTYPE_MCISERIAL ||  \
		  (hwidb)->type == IDBTYPE_CBUSETHER || \
		  (hwidb)->type == IDBTYPE_CBUSSERIAL || \
		  (hwidb)->type == IDBTYPE_HSSI || \
		  (hwidb)->type == IDBTYPE_FDDI ||      \
		  (hwidb)->type == IDBTYPE_FDDIT ||     \
		  (hwidb)->type == IDBTYPE_T1 ||     \
		  (hwidb)->type == IDBTYPE_E1 ||     \
		  (hwidb)->type == IDBTYPE_ASYNC ||     \
		  (hwidb)->type == IDBTYPE_CTR ||       \
		  (hwidb)->type == IDBTYPE_AIP ||       \
		  (hwidb)->type == IDBTYPE_FEIP)
		  
#define IFTYPE_CAPTURE   (IFTYPE_IEEE | IFTYPE_SERIAL | IFTYPE_HSSI |  \
			  IFTYPE_ASYNC | IFTYPE_ATM | IFTYPE_T1)

#define CAPTURE_IS_LONG_VALUE(var) (((ulong)(var) & CAPTURE_MOST_SIGN_WORD) \
				     ? 1 : 0)








