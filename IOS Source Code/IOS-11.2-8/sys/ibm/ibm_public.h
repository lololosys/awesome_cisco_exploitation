/* $Id: ibm_public.h,v 3.2.60.1 1996/05/17 11:20:02 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibm_public.h,v $
 *------------------------------------------------------------------
 * Prototypes for public routines
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ibm_public.h,v $
 * Revision 3.2.60.1  1996/05/17  11:20:02  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/11  14:37:44  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  14:33:32  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  09:22:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:25:25  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:46:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void ibmnm_shutdown(hwidbtype *);
extern void ibmnm_enqueue(paktype *);
extern void stun_periodic(void);
extern void stun_sprintf_addr(uchar *, boolean, uchar, char *);
extern void stif_input(paktype *);
extern void sdlc_parser_init(void);
extern void stun_input(hwidbtype *, paktype *);
extern void stun_shutinterface(hwidbtype *);
extern void slack_burn_and_die(hwidbtype *, sdlc_data_block *);
extern boolean prilist_protocol_stun(parseinfo *, plisttype *);
extern void priority_protocol_stun_nv_add(plisttype *);
extern boolean priority_protocol_stun_compare(plisttype *, plisttype *);
extern boolean stun_prioritization(plisttype *, paktype *, hwidbtype *);
