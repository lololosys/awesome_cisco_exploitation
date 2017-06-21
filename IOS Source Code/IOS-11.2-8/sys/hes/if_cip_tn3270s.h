/* $Id: if_cip_tn3270s.h,v 3.1.4.2 1996/08/07 08:59:04 snyder Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_cip_tn3270s.h,v $
 *------------------------------------------------------------------
 * if_cip_tn3270s.h Channel Interface Processor TN3270 Server support
 *
 * September 1995, Karen Mitchell
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_cip_tn3270s.h,v $
 * Revision 3.1.4.2  1996/08/07  08:59:04  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  11:18:04  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/21  03:21:38  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:01:48  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CIP_TN3270S_H__
#define __CIP_TN3270S_H__


/* Config prompts for TN3270S subcommand modes */
extern char *const pTn3270sSvrPrompt;
extern char *const pTn3270sPuPrompt;
extern char *const pTn3270sDlurPrompt;
extern char *const pTn3270sDlurPuPrompt;
extern char *const pTn3270sDlurSapPrompt;

/*
 * Prototypes
 */
/* Interface from general cip support code */
extern void cip_tn3270s_create(idbtype *, cip_idb_infotype *);
extern void cip_tn3270s_send_cfg(idbtype *);
extern void cip_tn3270s_cfg_acknowledge(hwidbtype *, ushort, ulong, uchar *, ulong);
extern void cip_tn3270s_reset_cfg(hwidbtype *, cip_idb_infotype *);
extern void cip_tn3270s_display_vc_list(idbtype *);
extern void cip_tn3270s_cfg_str(ushort, uchar *, uchar *, ushort);

/* Sub command support */

/* tn3270s subcommand mode */
extern void  tn3270s_svr_subcommand(parseinfo *, transition *, void *);
extern void *tn3270s_svr_set_vars(parseinfo *);
extern void  tn3270s_svr_reset_vars(parseinfo *, void *);

extern void  tn3270s_pu_subcommand(parseinfo *, transition *, void *);
extern void *tn3270s_pu_set_vars(parseinfo *);
extern void  tn3270s_pu_reset_vars(parseinfo *, void *);

extern void  tn3270s_dlur_subcommand(parseinfo *, transition *, void *);
extern void *tn3270s_dlur_set_vars(parseinfo *);
extern void  tn3270s_dlur_reset_vars(parseinfo *, void *);

extern void  tn3270s_dlur_pu_subcommand(parseinfo *, transition *, void *);
extern void *tn3270s_dlur_pu_set_vars(parseinfo *);
extern void  tn3270s_dlur_pu_reset_vars(parseinfo *, void *);

extern void  tn3270s_dlur_sap_subcommand(parseinfo *, transition *, void *);
extern void *tn3270s_dlur_sap_set_vars(parseinfo *);
extern void  tn3270s_dlur_sap_reset_vars(parseinfo *, void *);

/* Configuration commands - General */
extern void tn3270s_cfg_command(parseinfo *);

extern void tn3270s_exit_command(parseinfo *);

#endif /* __CIP_TN3270S_H__ */


