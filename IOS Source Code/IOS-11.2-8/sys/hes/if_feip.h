/* $Id: if_feip.h,v 3.5.12.1 1996/03/18 19:41:44 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/if_feip.h,v $
 *------------------------------------------------------------------
 * if_feip.h:  FEIP specific constants & definitions
 *
 * November 1994, Gary Kramling
 * Adapted from if_cip.h by Stig Thormodsrud
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: if_feip.h,v $
 * Revision 3.5.12.1  1996/03/18  19:41:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.2  1996/02/20  00:51:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  21:26:43  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.4.20.1  1996/01/24  21:46:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/02  06:55:23  tgrennan
 * CSCdi42449:  4/8e address filter improperly set by RP
 *
 * 4/8e address filter works like FE and FEIP instead of EIP/MCI.
 * Also, split large switch in cbus_mci_idb_init() into sub-functions.
 *
 * Revision 3.3  1995/11/17  09:17:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:52  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:00:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/09  00:19:49  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.1  1995/06/07  20:41:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef	__IF_FEIP_H__
#define __IF_FEIP_H__

/*
 * Time for system software to wait before issuing new commands
 * after a disable
 */
#define FEIP_DISABLE_TIME     5     

/*
 * Prototypes
 */
void pas_ethernet_interface_command(parseinfo *);
void pas_ethernet_rate_command (parseinfo *);
void show_extended_pas_ethernet_commands(parseinfo *);
void feip_platform_init(subsystype *);
void fast_ethernet_parser_init(void);

void feip_full_duplex_command(parseinfo *);
void feip_show_util(hwidbtype *);

void fast_ethernet_debug_init(void);
void feip_show_cbus(ushort);
void pas_ethernet_af_init(hwidbtype *);
void feip_media_type_command(parseinfo *);
void convert_raw_eeprom_to_pa_info(hwidbtype *hwidb, void *pa_info_slots);

#endif
