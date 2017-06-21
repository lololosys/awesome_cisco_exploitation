/* $Id: std_dci.h,v 3.1 1995/11/09 09:06:51 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/std_dci.h,v $
 *------------------------------------------------------------------
 * std_dci.h -- Standard header for Fast switching diagnostics.
 *
 * 22-February-1991, Bruce Sherry
 *
 * Copyright (c) 1991-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: std_dci.h,v $
 * Revision 3.1  1995/11/09  09:06:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:52  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../defs.h"
#include "../../types.h"
#include "../h/ether.h"
#include "../h/if_mci.h"
#include "../h/if_fci.h"
#include "../../ctype.h"
#include "../../test.h"
#include "../../cpu.h"
#include "../../reason.h"
#include "../../extern.h"
#include "../../stdio.h"
#include "../h/mci_err.h"
#include "../h/dci_diag_defs.h"

/*
 *  Function declarations
 */

extern boolean  store_error(), quit_display(), mci_xnvt();
extern short    touch_device();
extern void     init_error_struct(), print_errors(), show_err_mask(),
		show_dci_auto_results(), get_number(), type_mema(), type_memd();
extern int      mci_xnvs(), applique_type();

/*
 *  Things in dci_test.c
 */

extern boolean	dci_select(), dci_mema_test(), dci_memd_test(),
		dci_afmem_test(), dci_af_readwrite(), mec_aftest(),
		dci_check_one(), dci_setmask(), dci_ping_test(), dci_setfilter();
extern mcitype	*cBus_findcontroller();
extern void	dci_alloc_diag_mem(), mec_command(), mec_basic_debug(),
		mec_debug_ports(), dci_execute_2commands(),
		dci_config(), dci_soft_reset(), dci_revision(),
		mec_search(), select_mec_mema(), write_mec_mema(),
		dci_mema_load(), dci_mema_write(), dci_mema_read(),
		dci_mema_readwrite(), dci_type_mema(), select_mec_memd(),
		write_mec_memd(), dci_memd_load(), dci_memd_write(),
		dci_memd_read(), dci_memd_readwrite(),
		select_mec_af(), write_mec_af(), dci_type_afmem(),
		dci_allocate_buffers(), display_buffers(),
		cbus_type_mema(), write_cbus_mema(), select_cbus_mema(),
		test_parameters(), dci_setup_unit();
extern ushort	dci_command(), read_mec_mema(), read_mec_memd(),
		read_mec_af(), read_cbus_mema(), ex_cbus_command();
extern short	dci_af_sndrcv(), dci_ping();

/*
 *  Global variables
 */

extern MCI_TEST *diag_results[];
extern char	baddr[];

/*
 *  Things in dci_test.c
 */

extern mcitype	*mcidata[MAXCTRLLERS];
extern dcitype	*dcidata[FCI_MAXCTRLRS];
extern boolean	reporting, verbose, looping, ping_loopback, pkt_recvd,
                standard_packet;
extern int	errors, default_interface, board_to_test, fci_loopback_mode,
		serial_prescale, abort_count, total_wait, total_pkts;
extern uchar	*packet1, *packet2, *packet3, *custom_packet;
extern ushort	packet_length, before_reserve, debug_level, longest,
		icb_loc_0, memcheck, mema_start, mema_end;
extern ushort	pkt_offset, pkt_start, pkt_end, pkt_skip, ping_wait,
		park_buffers;
