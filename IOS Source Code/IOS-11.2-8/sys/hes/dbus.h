/* $Id: dbus.h,v 3.4.10.4 1996/09/04 18:12:31 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/hes/dbus.h,v $
 *------------------------------------------------------------------
 * Diagnostic bus prototypes for "public" use.
 *
 * October 1992, William H. Palmer
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dbus.h,v $
 * Revision 3.4.10.4  1996/09/04  18:12:31  kmitchel
 * CSCdi66418:  IOS overreacts to DBUS io port-whine
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/06/21  18:19:43  kmitchel
 * CSCdi47923:  DBUS-3-NORESP DBUS-3-BADCTRLRTYPE msgs when booting on RSP
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/04/23  14:02:27  banderse
 * CSCdi50739:  CIP and other swdbus cards do not need extra reset
 * Branch: California_branch
 * add dbus_ctrlr_start() routine.
 *
 * Revision 3.4.10.1  1996/03/27  07:44:28  dbath
 * CSCdi51848:  Pulled slave remains in chassis MIB.
 * Branch: California_branch
 * Add access routine to the slots[] array.
 *
 * Revision 3.4  1996/02/23  10:15:23  feisal
 * CSCdi45132:  Crash when remove VIP when on IF-CON to VIP
 *
 * Revision 3.3  1995/11/17  09:15:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:38:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:59:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/11  07:08:02  mcieslak
 * Reload the microcode for a slot that has failed and allow
 * for IPs that are disabled to be removed cleanly.
 * CSCdi41907:  complex restart / EOIR of disabled cards broken
 *
 * Revision 2.5  1995/09/12  18:22:10  kramling
 * CSCdi37548:  FEIP Parity error requires OIR or Power cycle to clear it
 *
 * Revision 2.4  1995/08/10  19:11:43  schaefer
 * CSCdi38464: need branch patch in 11.0
 * don't crash if two RSPs are installed
 *
 * Revision 2.3  1995/07/27  04:45:15  hampton
 * The 7000 RP serial number should be accessible via the platform_get_xxx
 * routines.  [CSCdi37744]
 *
 * Revision 2.2  1995/07/17 08:21:12  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.1  1995/06/07  20:38:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern boolean system_mac_addr_avail;
extern uchar system_mac_addr[];

extern char* card_type_to_str(ushort, ushort);
extern void set_eoir_capable(ushort);
extern void cxbus_eoir_reset(mcitype*);	/* XXX - move to C7000-specific file */
extern void cxbus_soft_reset(mcitype*);	/* XXX - move to C7000-specific file */
extern void dbus_xilinx_reload(dgtype*, boolean);
extern boolean swbus_present(dgtype*);
extern boolean dbus_parallel_inline(ushort);
extern boolean dbus_wait_for_sw_ready(dgtype*, char*);
extern void dbus_restore_io_status_bits(dgtype *);
extern boolean dbus_slot_enabled(dgtype*);
extern void dgbus_analyze(boolean);
extern boolean dbus_validate(uint slot);
extern void mark_removed_cards (void);
extern ushort dbus_controller_type(dgtype*);
extern boolean srswitch_pslot(int pslot);
extern boolean ssip_smip_pslot(int pslot);
extern boolean is_cip2_hw(int);
extern ushort dbus_hw_version(dgtype*);
extern ushort dbus_hw_revision(dgtype*);
extern void dbus_mci_internal_clock_disable(dgtype*, uint);
extern void dbus_mci_internal_clock_enable(dgtype*, uint);
extern void dbus_mci_invert_txc_disable(dgtype* dbus, uint channel);
extern void dbus_mci_invert_txc_enable(dgtype* dbus, uint channel);
extern void dbus_mci_nrzi_disable(dgtype*, uint);
extern void dbus_mci_nrzi_enable(dgtype*, uint);
extern uchar dbus_slot_enable(dgtype*);
extern void dbus_slot_enable_finish(dgtype*, uchar);
extern void dbus_write_fip_leds(uint,uchar);
extern boolean download_installed_microcode(dgtype*, boolean);
extern boolean download_microcode_from_ram(uint, uchar *, uint, boolean);
extern boolean download_microcode_chunk_from_ram(uchar **, long *, long,
						 void *);
extern void hotswap_determine_system_mac_addr(void);
extern void slot_enabled_led_on(uint);
extern boolean switchproc_present(dgtype*);
extern void disable_slot(uint);
extern boolean slot_enabled(uint);
extern slottype* get_slotp(int);
extern dgtype *pslot_to_dbus(int);
extern int dbus_to_pslot(dgtype *);
extern boolean cxbus_card_present(uint);
extern boolean rsp_present(uint);
extern ulong dbus_serial_number(dgtype*);
extern dgtype* dbus_rpslot_to_dbus(void);
extern int dbus_system_rp_slot(void);
extern boolean dbus_cpu_slot(uint slot);
extern void dbus_ctrlr_reset(dgtype*); /* should be private */
extern uchar dbus_ctrlr_start(dgtype*, uchar);
extern uchar dbus_ctrlr_start_nowaited(dgtype*, uchar);
extern void dbus_ctrlr_start_nowaited_finish(dgtype*, uchar);
extern void dbus_hard_ctrlr_reset(dgtype*);
extern void dbus_parser_init(void);
extern void dbus_slot_reset(uint);
extern void dbus_logger_drain(uint);
extern void sip_nrzi_control(hwidbtype* idb, boolean enable);
extern void sip_invert_txc_control(hwidbtype* idb, boolean enable);
extern void sip_internal_txc_control(hwidbtype* idb, boolean enable);
extern boolean dbus_check_msg(dgtype *dbus, int slot);

extern boolean dbus_present(dgtype *dbus);
extern boolean slot_present(uint slot);
extern void dbus_slot_error(uint slot);
extern void dbus_error_scan(void);
extern boolean board_has_instr_error(uint slot);
extern void dbus_hard_ctrlr_reset_reset (uchar card_type, dgtype* dbus);
extern void dbus_hard_ctrlr_reset_delay (uchar card_type, dgtype* dbus);
extern void dbus_hard_ctrlr_reset_wait (uchar card_type, dgtype* dbus);
extern void dbus_soft_reset_reset (uchar card_type, dgtype* dbus);
extern void dbus_soft_reset_delay (uchar card_type, dgtype* dbus);
extern void dbus_soft_reset_wait (uchar card_type, dgtype* dbus);
extern void dbus_soft_reset_all_specific_controllers (boolean[]);
extern void dbus_reset_recovery_counters(uint);

extern void dbus_reset_IP_slots(void);
extern void dbus_startup(void);

extern uchar dbus_io_reg_read(uint, uint);
extern void  dbus_io_reg_write(uint, uint, uchar);

extern uchar dbus_io_status_reg_read(uint);
extern void  dbus_io_status_reg_write(uint, uchar);

#define DBUS_IO_BIT_ON TRUE
#define DBUS_IO_BIT_OFF FALSE
extern void  dbus_io_status_bit(uint, uchar, boolean);
