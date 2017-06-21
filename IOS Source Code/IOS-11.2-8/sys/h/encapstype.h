/* $Id: encapstype.h,v 3.5.12.5 1996/07/28 07:32:41 irfan Exp $
 * $Source: /release/112/cvs/Xsys/h/encapstype.h,v $
 *------------------------------------------------------------------
 * encapstype.h -- Definitions of encapsulations
 *
 * March 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: encapstype.h,v $
 * Revision 3.5.12.5  1996/07/28  07:32:41  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.5.12.4  1996/05/17  11:15:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.1  1996/04/05  06:06:22  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 * Revision 3.5.12.3  1996/05/10  01:37:22  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/03/24  00:28:05  ppearce
 * 7000 platform build failure
 * Branch: California_branch
 *   Move definition of "encapstype" back to interface_private.h
 *     to fix 7000 build failure of assembly generated file
 *
 * Revision 3.5.12.1  1996/03/23  01:29:56  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.5  1996/02/13  08:11:36  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.4  1996/01/31  21:15:00  motto
 * CSCdi47239:  DLSW+ doesnt forward test frames to CIP internal lan
 *
 * Revision 3.3  1995/11/28  23:15:37  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.2  1995/11/17  09:12:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:58:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/13  20:26:34  shj
 * CSCdi41281:  memory leak associated with loopback interface crashes
 * router
 * Define ET_LOOP to be used in hwidb->enctype on loopback interfaces.
 * Prevents set_default_queueing from changing hwidb->oqueue on loopback
 * interfaces, which led to packets being queued forever.
 *
 * Revision 2.2  1995/09/30  05:01:43  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.1  1995/06/07  20:34:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__ENCAPSTYPE_H__
#define	__ENCAPSTYPE_H__

/*
 * Encapsulation methods
 * When adding a new code, be sure to update interface_up() function.
 */

#define ET_NULL			0	/* undefined -- error */
#define ET_ARPA			1	/* Ethernet - DDN style */
#define ET_SAP			2	/* Ethernet - HP blew it */ 
#define ET_SNAP			3	/* 802.2 SNAP types */
#define ET_1822			4	/* DDN - 1822 (obsolete) */
#define ET_HDLC			5	/* Serial - raw HDLC */
#define ET_UNUSED2		6	/* not used was ET_3MB */
#define ET_UNUSED1		7	/* not used was ET_HDH */
#define ET_LAPB			8	/* Serial - LAPB */
#define ET_X25			9	/* Serial - X.25 */
#define ET_HUB			10	/* HUB fiber optic */
#define ET_NOVELL_ETHER		11	/* Novell style XNS on Ethernet */
#define ET_UNSUPPORTED		12	/* Unsupported protocols */
#define ET_3COM_TR		13	/* 3Com XNS over TR 802.2 0x80 */
#define ET_UB_TR		14	/* Ungermann-Bass XNS over TR SNAP */
#define ET_APOLLO		15	/* Apollo domain packets */
#define ET_PPP			16	/* Serial PPP */
#define ET_UNUSED4  		17	/* Unused. Was ET_ISO3 */
#define ET_VINES_TR		18	/* ISO1 with Vines demux byte */
#define ET_ETHERTALK		19	/* AppleTalk phase 1 on ethernet */
#define ET_FRAME_RELAY		20	/* Frame Relay */
#define ET_SMDS			21	/* SMDS */
#define ET_MAC			22	/* MAC level packets */
#define ET_ULTRA_ISO2		23	/* unused */
#define ET_ULTRA_ISO1		24	/* unused */
#define ET_STUN			25	/* Serial - serial tunnelling */
#define ET_BRIDGE		26	/* Packet in bridge encapsulation */
#define ET_LLC2			27	/* LLC 2 */
#define ET_SDLCP		28	/* Serial - SDLC (primary) */
#define ET_SDLCS		29	/* Serial - SDLC (secondary) */
#define ET_SLIP			30	/* Async SLIP encapsulation */
#define ET_TUNNEL		31	/* Standard Tunnel Interface */
#define ET_BRIDGE_ENCAPS	32	/* Bridge encap on local gen packs */
#define ET_ATM			33	/* ATM interface encaps */
#define ET_ATM_DXI      	34	/* ATM DXI implementation */
#define ET_FR_IETF      	35	/* Frame Relay with IETF encaps */
#define ET_SMDS_DXI             36	/* SMDS DXI implementation */
#define ET_ATM_DXI_IETF		37	/* atm-dxi with IETF encaps */
#define ET_CHANNEL		38	/* IBM Channel */
#define ET_SDLC			39	/* CLSI compliant SDLC */
#define ET_SDE          	40	/* 802.10 Secure Data Exchange */
#define ET_BSTUN                41      /* block serial tunnel */
#define ET_DIALER               42      /* Dialer encapsulation */
#define ET_NOVELL_FDDI		43	/* Novell style XNS on FDDI */
#define ET_V120			44      /* V120 ISDN->ASYNC encaps */
#define ET_ISL                  45      /* Inter Switch Link - vLANs on FEIP */
#define ET_LOOP                 46      /* loopback interface */
#define ET_CHANNEL_ILAN         47      /* IBM Channel internal LAN */
#define ET_SERIAL_AUTODETECT    48      /* Autodetected for Serials */
#define ET_CPP			49	/* Combinet proprietary protocol */
#define ET_NDLC                 50      /* NCIA DLC */
#define ET_MAX			51	/* one larger then largest ET */

#define ET_SNAP_LEN 8
#define ET_SAP_LEN 3
#define ET_8022_NOVELL_LEN 3		/* ISO1 */
#define ET_8022_3COM_LEN 3		/* ISO1 */
#define ET_SNAP_UB_LEN 8		/* ISO2 */
#define ET_APOLLO_LEN 30		/* Apollo Domain (on ethernet) */
#define ET_VINES_LEN 4			/* ISO1 plus a demux byte */
#define ET_8022_VINES_LEN 3		/* ISO2 */

#endif	__ENCAPSTYPE_H__
