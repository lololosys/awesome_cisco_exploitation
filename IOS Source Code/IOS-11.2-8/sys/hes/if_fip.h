/* $Id: if_fip.h,v 3.4.12.2 1996/08/16 00:49:58 ogrady Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_fip.h,v $
 *------------------------------------------------------------------
 * if_fip.h -- fddi routines
 *
 * May 1993, David Hampton
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_fip.h,v $
 * Revision 3.4.12.2  1996/08/16  00:49:58  ogrady
 * CSCdi54594:  SRB on FDDI propogates broadcast storms
 *              Branch: California_branch
 *              Very small bridge packets not stripped from ring.  Make
 *              routine that inits FIP hold register common instead of
 *              platform specific.
 *
 * Revision 3.4.12.1  1996/06/07  00:33:49  kao
 * CSCdi59481:  4R does not work when hot swapped
 * CSCdi59000:  FDDI PA line status is always down after an eoir event
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/13  20:25:18  hampton
 * Move platform specific defines from common include files to platform
 * specific include files.  [CSCdi48844]
 *
 * Revision 3.3  1995/11/17  09:17:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:41:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FIP_H__
#define __FIP_H__


/*
 * Default values for the FDDI (formac) controller.
 */
#define FDDI_FORMAC_BASE_MASK	0x1600
#define FDDIT_FORMAC_BASE_MASK	0x0000
#define FDDI_DUP_ADDR_MASK	0x0038  
#define FDDIT_DUP_ADDR_MASK     0x001e

# define FM_MODE_ON_LINE		0x6000
# define FM_MODE_OFF_LINE		0x0000
# define FM_MODE_DELADET		0x0800
# define FM_MODE_NORECV			0x0400
# define FM_MODE_DA_EQ_MA		0x0000
# define FM_MODE_DA_AND_SA_EQ_MA	0x0100  
# define FM_MODE_PROMISCUOUS		0x0200
# define FM_MODE_RB_BUS			0x0000    
# define FM_MODE_RA_BUS        		0x0020
 
#define FDDI_MODE		(FM_MODE_ON_LINE | FM_MODE_DA_EQ_MA)
#define FDDIT_MODE		(FM_MODE_ON_LINE | FM_MODE_PROMISCUOUS | \
				 FM_MODE_DELADET)

/*
 * FDDIT constant that defines what extra bits show up in the
 * byte cell of the Buffer header.  This is visible through
 * reg->rxlength.
 *
 */

#define FDDIT_SRB		0x8000
#define FDDIT_CAM_VALID		0x4000
#define FDDIT_SA_FND		0x2000
#define FDDIT_RIF_CAM		(FDDIT_SRB | FDDIT_CAM_VALID | FDDIT_SA_FND)






/*
 * FDDI Frame Status from Formac on FCI/FCIT/FIP:
 */
#define FDDI_E_INDICATOR	0x80
#define FDDI_A_INDICATOR	0x40
#define FDDI_C_INDICATOR	0x20
#define FDDI_ERROR_THIS_NODE	0x10
#define FDDI_ADDRESS_RECOGNIZED	0x08
#define FDDI_MAC_FRAME		0x04
#define FDDI_SMT_FRAME		0x02
#define FDDI_IMPLEMENTER_FRAME	0x01

#define FDDI_SX_MASK		(FDDI_E_INDICATOR | FDDI_ERROR_THIS_NODE)

extern uchar dbptr_fcit[6];
extern uchar dbptr_fddi[6];

extern int smt_process_id;
extern queuetype smtinputQ;
extern queuetype smtbufQ;
extern boolean	fddi_smt_packet_debug;

extern void	hes_fddi_idb_init(hwidbtype *idb);
extern void	fddi_carrier(hwidbtype *);
extern void	fddi_reset(hwidbtype *);
extern void	fddi_system_configured(hwidbtype *);
extern void	fip_platform_init(subsystype *subsys);
extern void	fip_leds(hwidbtype *);

extern void	fddi_print_versions(hwidbtype *hwidb);
extern void	fddit_print_regs(ushort regs[]);
extern void	fddi_print_ip_regs(hwidbtype *hwidb, ushort regs[]);
extern void	fddi_set_options(hwidbtype *hwidb);
extern boolean	fddi_smt_enabled_any_intfc(hwidbtype *hwidb);
extern void	fip_restart_init(hwidbtype *hwidb);
extern ushort	build_claim_beacon_buffer(hwidbtype *hwidb, int trt);
extern void	fddi_check_for_old_cam_bug(hwidbtype *hwidb);

extern void	hes_fddi_set_hold_reg(hwidbtype *hwidb, int byte_rxoffset);
extern int      get_fip_byte_rxoffset(hwidbtype *hwidb);
extern void	fddi_smt_debug(paktype *pak, boolean output);
extern phytype *hes_get_phy(hwidbtype *hwidb, int index);
extern void cbus_vip_1f_reset (hwidbtype *hwidb);
extern void cbus_vip_1f_periodic(hwidbtype *hwidb);


#endif  /* __FIP_H__ */
