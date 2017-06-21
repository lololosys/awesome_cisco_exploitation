/* $Id: if_hes_common.h,v 3.4.46.1 1996/07/08 22:25:27 mwu Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_hes_common.h,v $
 *------------------------------------------------------------------
 * if_hes_common.h -- Prototypes for the common high end functions 
 *   found in if_hes_common.c, and other useful defines.
 *
 * August, 1994  Jack Jenney
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_hes_common.h,v $
 * Revision 3.4.46.1  1996/07/08  22:25:27  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.4  1995/12/10  23:27:34  rpratt
 * CSCdi45343:  Chassis MIB enhancements for VIP & port adapters
 *
 * Revision 3.3  1995/11/17  09:18:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:00:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:41:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__IF_HES_COMMON_H__)
#define __IF_HES_COMMON_H__

/*
 * Defines for setting hwidb->status
 */
#define MCI_IDBFLAGS	(IDB_MCI | IDB_BCAST | IDB_HEARSELF | IDB_BRIDGE)
#define FCI_IDBFLAGS	(IDB_CBUS | IDB_BCAST | IDB_HEARSELF | IDB_BRIDGE)


/*
 * Externs for functions defined in if_hes_common.c
 */
extern int cbus_max_ctrlr_interfaces;
extern int cbus_restarts_lineup;
extern int cbus_restarts_output;
extern int cbus_restarts_error;


extern void hes_idb_init_before(hwidbtype *hwidb);
extern void hes_idb_init_after(hwidbtype *hwidb);
extern void shutdown_pulled_interfaces(void);
extern void init_new_hwidb(hwidbtype* idb);
extern boolean cbus_mci_set_maxdgram(hwidbtype *hwidb, int bufsize,
				     int maxdgram);
extern void cbus_mci_carrier(hwidbtype *hwidb);
extern void get_cbus_ctrlr_errors(hwidbtype *hwidb);
extern char* cbus_mci_idb_typestring(hwidbtype* hwidb);
extern void cbus_mci_rxtypes_update(hwidbtype *);
extern void cbus_mci_idb_throttle(hwidbtype *);
extern boolean cbus_mci_idb_init(mcitype *, int, int, hwidbtype *,
				 hwidbtype *);
extern ushort cbus_af_read(hwidbtype *, ushort);
extern void cbus_af_and(hwidbtype *, ushort, ushort);
extern void cbus_af_or(hwidbtype *, ushort, ushort);
extern void cbus_af_write(hwidbtype *, ushort, ushort);
extern void cbus_mci_input_error(hwidbtype*);
extern void cbus_set_transmit_delay(hwidbtype *idb, int value);
extern void get_cbus_errors(hwidbtype *idb);
extern void interface_panic(hwidbtype *hwidb, const char *namestring);
extern void card_type_and_descr(int fci_type, int if_count, int* card_type,
				char** card_descr);
extern void sub_card_type_and_descr(int fci_sub_type, int if_count, int* card_type, 
				    char** card_descr);

/*
 * Externs for functions in if_ags_c7000.c
 */
extern char *cbus_mci_memtest(mcitype *);
extern void cbus_mci_output(hwidbtype *);
extern void cbus_mci_throttle(hwidbtype *);
extern void mci_add_interface(mcitype *, int, hwidbtype *);
extern void dci_add_interface (mcitype *, int, hwidbtype *);

/*
 * Other externs
 */
extern int mec_eth;
#ifndef	RSP
extern mcitype *cbus;
#endif

extern void cbus_mci_txlimit(hwidbtype *);
extern void platform_req_interface_restart(hwidbtype *hwidb);
extern void platform_req_card_restart(hwidbtype *hwidb);
extern void platform_req_complex_restart(hwidbtype *hwidb);
extern void platform_demand_complex_restart(hwidbtype *hwidb);
extern boolean ip_on_cbus2(hwidbtype *hwidb);
extern void platform_light_off(hwidbtype *hwidb);
extern void platform_light_on(hwidbtype *hwidb);
extern void platform_update_lights(hwidbtype *hwidb);
extern boolean platform_cbus_reset_in_progress(hwidbtype *hwidb);
extern void platform_show_switch_counts(hwidbtype *hwidb);

extern boolean cbus_unit2slotunit(int, int *, int *);  
extern int cbus_slotunit2unit(int, int);
extern int cbus_get_next_if_unit(int, int *);

extern boolean	ctrlr_7500_compat(uchar ctrlr, ushort hwver);

extern void	mark_hwidbs_nohardware(hwidbtype *first_hwidb);
extern int	mark_hwidbs_hardware_present(hwidbtype *first_hwidb);

#endif /* !defined(__IF_HES_COMMON_H__) */
