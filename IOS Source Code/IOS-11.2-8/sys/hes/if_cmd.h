/* $Id: if_cmd.h,v 3.4.22.5 1996/08/29 02:23:40 xtang Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_cmd.h,v $
 *------------------------------------------------------------------
 * if_cmd.h -- Prototypes for command wrapper functions.
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_cmd.h,v $
 * Revision 3.4.22.5  1996/08/29  02:23:40  xtang
 * CSCdi60447:  FSIP max MTU=14816 not 18000
 * Branch: California_branch
 *              Implemented new FSIP command 93 to pass the available
 *              buffer size in the controller to the system.
 *              The corresponding microcode change are in
 *              cvs -d /micro fsip_q (fsip187-0)
 *              and cvs -d /micro fsip_i (rsp_fsip204-0).
 *
 * Revision 3.4.22.4  1996/08/20  00:34:50  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.4.22.3  1996/08/08  23:55:19  whp4
 * CSCdi52884:  cip ignores mtu size on datagrams to the host
 * Branch: California_branch
 * SP needs to know CIP mtu is different from AIP mtu for shared path.
 *
 * Revision 3.4.22.2  1996/07/08  22:25:20  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.4.22.1  1996/05/22  23:25:53  rramacha
 * CSCdi58249:  Enable IP DFS on 4T
 * Branch: California_branch
 * Pass serial encap to serial driver in VIP
 *
 * Revision 3.4  1996/01/23  08:47:50  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.3  1995/11/17  09:17:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:44  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:00:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/31  22:38:39  izhu
 * CSCdi32813:  SDLC Multidrops need router to ignore DCD for High-End
 * Platform
 * and
 * CSCdi38317  STUN: cannot IPL a 3725 using FSIP
 *
 * Convert to midrange signal model for line state (RTS/CTS state not
 * used),
 * permit DCD to be ignored.
 *
 * Revision 2.3  1995/08/09  00:19:43  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.2  1995/07/06  02:48:18  rharagan
 * CSCdi36490:  pinging on the AIP induced a crash in the SP microcode
 * Send the version command to the IP not the SP if the tx queue appears
 * to hang.
 *
 * Revision 2.1  1995/06/07  20:41:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__IF_CMD_H__)
#define __IF_CMD_H__

/*
 * You may find it useful to include the following include files
 * before including this file.
 * "interface.h"
 * "slot.h"
 */

/*
 * Possible response code to the command wrapper functions
 * (used to be in if_fci.h)
 */
#define CMD_RSP_OKAY	       0	/* successful operation */
#define CMD_RSP_ERROR	  0x8000	/* this bit indicates an error */
#define CMD_RSP_ERR_UNK   0x8000	/* unknown error */
#define CMD_RSP_ERR_ILR   0x8001	/* illegal register read */
#define CMD_RSP_ERR_ILW   0x8002	/* illegal register write */
#define CMD_RSP_ERR_UNIT  0x8003	/* impossible unit number */
#define CMD_RSP_ERR_NPRS  0x8004	/* unit not present */
#define CMD_RSP_ERR_FNV   0x8005	/* function not valid */
#define CMD_RSP_ERR_IVO   0x8006	/* invalid offset */
#define CMD_RSP_ERR_NBF   0x8007	/* no available buffers */
#define CMD_RSP_ERR_CMD   0x8008	/* illegal command code */
#define CMD_RSP_ERR_MEM   0x8009	/* no more memory */
#define CMD_RSP_ERR_VLD   0x800A	/* RX_CHECK failed */
#define CMD_RSP_ERR_CHK   0x800B	/* RX_CHECK failed, bad checksum */
#define CMD_RSP_ERR_TTL   0x800C	/* RX_CHECK failed, TTL exceeded */
#define CMD_RSP_ERR_BFR   0x800D	/* buffer already in window */
#define CMD_RSP_ERR_QFL   0x800E	/* output queue full */
#define CMD_RSP_ERR_NCP   0x800F	/* no capability present */
#define CMD_RSP_ERR_TMOUT 0x8010	/* timed out */
#define CMD_RSP_ERR_HSA	  0x8012 	/* hot stall active */
#define CMD_RSP_ERR_SSE   0x8015	/* SSE hung */
#define CMD_RSP_ERR_FRAG  0x8016	/* needs fragmenting */
#define CMD_RSP_ERR_CMDQFL 0x8017	/* cmd queue full, cmd was dropped */

/*
 * Constants for use with send_fip_phy_[ab]_lem_cmd() functions:
 */
#define FDDI_LEM_ON                           1
#define FDDI_LEM_OFF                          0

/*
 * FDDI CAM interface
 *
 * The FDDI-T CAM interface is used to read, write, and manage the
 * CAM on the FDDI-T.  This is accomplished through a simple command/data
 * command port pair that talks to the FDDI-T uCode.  There is a 5 word
 * (16 bit) argument area that is written via the CCB_CMD_FIP_CAM_WR_ARG command.
 * Which 16 bit word is being written is automatically incremented after
 * any read or write of the CCB_CMD_FIP_CAM_WR_ARG.  The pointer is reset back
 * to the beginning following the execution of any command.  The data is 
 * organized as follows:
 *
 *     camarg   NOP/CLR/LOCK/   INS     RMV     CHK     CHK     SET
 *              UNLOCK                                  rd_arg
 *
 *       0      no data         maddr   maddr   maddr   msw     mod_arg
 *       1                      maddr   maddr   maddr   lsw
 *       2                      maddr   maddr   maddr
 *       3                      ramdata
 *       4
 *
 *     camarg   RD      RD      WR      RD_CTL  RD_CTL  WR_CTL
 *                      rd_arg                  rd_arg
 *
 *       0      camaddr maddr   camaddr ctl_reg value   ctl_reg
 *       1              maddr   maddr                   value
 *       2              maddr   maddr
 *       3              ramdata maddr
 *       4                      ramdata
 *
 *     camarg   RD_DATA RD_DATA WR_DATA LOCK    UNLOCK  LEARN
 *                      rd_arg          CAM     CAM     MA
 *
 *       0              value   value                   maddr
 *       1                                              maddr
 *       2                                              maddr
 *       3
 *       4
 *
 * Commands are issued via the command CCB_CMD_FIP_CAM_CMD.  The following commands
 * are defined:
 *
 *      0       NOP.  Do nothing.  Used to reset the arg pointer.
 *      1       CLR CAM.  This removes all entries in the CAM.
 *      2       INS ENTRY.  Insert an entry into the CAM.
 *      3       RMV ENTRY.  Remove an entry from the CAM.
 *      4       CHK ENTRY.   Run an associative cycle on an address
 *      5       SET MODIFIER.  Set up the modifier for direct read/write access.
 *      6       RD.  Read CAM directly
 *      7       WR.  Write CAM directly
 *      8       RD_CTL.  Read Control Register
 *      9       WR_CTL.  Write Control Register
 *      10      RD_DATA.  Read CAM Data port
 *      11      WR_DATA.  Write CAM Data port
 *      12      LOCK_CAM.  Lock out the cam for an extended period.
 *      13      UNLOCK_CAM.  Unlock the extended lockout.
 *      14      LEARN_MA.  Learn the mac address of this node.
 *      15      INSERT_MC. Just like insert entry but allows Multicast
 *                      insertions.
 */

#define FDDI_CAM_NOP                    0
#define FDDI_CAM_CLR                    1
#define FDDI_CAM_INSERT                 2
#define FDDI_CAM_REMOVE                 3
#define FDDI_CAM_CHECK                  4
#define FDDI_CAM_SET_MODIFIER           5

#  define FCSM_DISABLE                          0x00
#  define FCSM_VALID                            0x04
#  define FCSM_EMPTY                            0x05
#  define FCSM_SKIP                             0x06
#  define FCSM_RANDOM                           0x07

#  define FCSM_AR                               0x20
#  define FCSM_HM                               0x28
#  define FCSM_NFA                              0x30

#define FDDI_CAM_RD_CAM                 6
#define FDDI_CAM_WR_CAM                 7
#define FDDI_CAM_RD_CTL                 8
#define FDDI_CAM_WR_CTL                 9
#  define FCRWC_CTL                             0x00
#  define FCRWC_PA                              0x08
#  define FCRWC_SEG                             0x10
#  define FCRWC_NFA                             0x18
#  define FCRWC_AR                              0x20
#  define FCRWC_DS                              0x28
#  define FCRWC_PS                              0x30
#  define FCRWC_PD                              0x38
#  define FCRWC_STATUS                          0xFFFF
#  define FCRWC_INSTR                           0xFFFF
#define FDDI_CAM_RD_DATA                10
#define FDDI_CAM_WR_DATA                11
#define FDDI_CAM_LOCK_CAM               12
#define FDDI_CAM_UNLOCK_CAM             13
#define FDDI_CAM_LEARN_MA               14
#define FDDI_CAM_INSERT_MC              15

/*
 * constants for the send_fsip_signal_sts_cmd() function
 */
#define FSIP_MASK_CTS                         0x1
#define FSIP_MASK_RTS                         0x2
#define FSIP_MASK_DTR                         0x4
#define FSIP_MASK_DSR                         0x8
#define FSIP_MASK_DCD                         0x10

/*
 * constants for the send_hip_iclk_cmd() function
 */
#define HSSI_CLK_EXTERNAL                     0
#define HSSI_CLK_INTERNAL                     1

/*
 * constants for the send_hip_loopback_cmd() function
 */
#define HSSI_LOOP_OFF                         0
#define HSSI_LOOP_LINE                        1
#define HSSI_LOOP_REMOTE                      2
#define HSSI_LOOP_DTE                         3

/*
 * constants for the send_hip_lc_inhibit_cmd() function
 */
#define HSSI_LCI_ON                           1
#define HSSI_LCI_OFF                          0

/*
 * constants for the send_errors_cmd() function.  These
 * flags are used by the caller to inidicate which error
 * count they're interested in.
 */
#define CMD_ERRORS_GIANTS	   0
#define CMD_ERRORS_NOBUFFERS	   1
#define CMD_ERRORS_ACQUIRE	   2
#define CMD_ERRORS_COLLISIONS	   3
#define CMD_ERRORS_RX_OVERRUN	   4
#define CMD_ERRORS_CRC		   5
#define CMD_ERRORS_FRAME	   6
#define CMD_ERRORS_RUNT		   7
#define CMD_ERRORS_TX_UNDERFLOW	   8
#define CMD_ERRORS_FDDI_LEM_A	   9
#define CMD_ERRORS_FDDI_LEM_B	  10
#define CMD_ERRORS_APPLIQUE	  11
#define CMD_ERRORS_PARITY	  12
#define CMD_ERRORS_RX_THROTTLE	  13
#define CMD_ERRORS_FRAMES_SEEN	  14
#define CMD_ERRORS_CLAIMS	  15
#define CMD_ERRORS_BEACONS	  16
#define CMD_ERRORS_MAC_GIANTS	  17
#define CMD_ERRORS_MAC_IGNORED	  18
#define CMD_ERRORS_RX_RFED	  19
#define CMD_ERRORS_RX_REDI	  20
#define CMD_ERRORS_RX_NULL_RCP	  21
#define CMD_ERRORS_RX_EXPLICIT_ABORT 22
#define CMD_ERRORS_RX_IMPLICIT_ABORT 23
#define CMD_ERRORS_TX_SFED	  24
#define CMD_ERRORS_TX_SEDI	  25
#define CMD_ERRORS_TX_FALSE_TOKEN 26
#define CMD_ERRORS_TX_DMA_PARITY  27
#define CMD_ERRORS_TX_NEXT_BUFFER 28
#define CMD_ERRORS_TX_STRIP_ABORT 29
#define CMD_ERRORS_TX_PTT_EXPIRED 30
#define CMD_ERRORS_RX_RECOVER_RCP 31

#ifndef ASMINCLUDE

/*
 * External references to the command wrapper functions
 */
extern ushort select_if(hwidbtype *hwidb);
extern long send_errors_cmd(hwidbtype *hwidb, ushort error_number);
extern ushort send_applique_cmd(hwidbtype *hwidb, ushort app_cmd,
				ushort *applique_ptr);
extern ushort send_rx_offset_cmd(hwidbtype *hwidb, ushort rx_offset);
extern ushort send_fip_hold_reg_cmd(hwidbtype *hwidb, ushort hold_reg);
extern ushort send_fip_burst_mode_cmd(hwidbtype *hwidb, ushort burst_mode);
extern ushort send_fip_phy_a_lem_cmd(hwidbtype *hwidb, ushort lem_cmd);
extern ushort send_fip_phy_b_lem_cmd(hwidbtype *hwidb, ushort lem_cmd);
extern ushort send_fddi_cam_rd_arg_cmd(hwidbtype *hwidb, ushort *read_value);
extern ushort send_fddi_cam_wr_arg_cmd(hwidbtype *hwidb, ushort write_value);
extern ushort send_fddi_cam_cmd(hwidbtype *hwidb, ushort cam_op);
extern ushort send_fddi_cam_cmd_3arg(hwidbtype *hwidb, ushort cam_op,
                            ushort arg0, ushort arg1, ushort arg2);
extern ushort send_fip_get_state_cmd(hwidbtype *hwidb, ushort which_phy,
				     ushort *state);
extern ushort send_cbus_buffer_park_cmd(hwidbtype *hwidb, ushort slots);
extern ushort send_rxbytes_cmd(hwidbtype *hwidb, ushort max_rx_bytes);
extern ushort send_rx_buffer_limit_cmd(hwidbtype *hwidb, ushort buffer_limit);
extern ushort send_rx_error_cmd(hwidbtype *hwidb, ushort error_pool);
extern ushort send_assign_poolindex_cmd(hwidbtype *hwidb, ushort pool);
extern ushort send_tx_allocate_cmd(hwidbtype *hwidb, ushort pool, ushort size);
extern ushort send_status_cmd(hwidbtype *hwidb, ushort arg0);
extern ushort send_if_enable_cmd(hwidbtype *hwidb);
extern ushort send_if_disable_cmd(hwidbtype *hwidb);
extern ushort send_if_loopback_cmd(hwidbtype *hwidb);
extern ushort send_if_noloopback_cmd(hwidbtype *hwidb);
extern ushort send_if_throttle_cmd(hwidbtype *hwidb,
				   ushort *throttle_state_ptr);
extern ushort send_ctrlr_reset_cmd(hwidbtype *hwidb);
extern ushort send_ctrlr_load_cmd(hwidbtype *hwidb, ushort arg0);
extern ushort send_clockrate_cmd(hwidbtype *hwidb, ushort cookie);
extern ushort send_clocktype_cmd(hwidbtype *hwidb, ushort prescalar);
extern ushort send_alt_idle_cmd (hwidbtype *hwidb, ushort enable);
extern ushort send_delay_cmd(hwidbtype *hwidb, ushort delay);
extern ushort send_hip_ta_cmd(hwidbtype *hwidb, ushort ta);
extern ushort send_hip_iclk_cmd(hwidbtype *hwidb, ushort iclk);
extern ushort send_hip_lc_inhibit_cmd(hwidbtype *hwidb, ushort lc);
extern ushort send_hip_loopback_cmd(hwidbtype *hwidb, ushort loopback);
extern ushort send_carrier_cmd(hwidbtype *hwidb, ushort *carrier_state_ptr);
extern ushort send_lights_cmd(hwidbtype *idb, ushort lights);
extern ushort send_station_cmd(hwidbtype *hwidb,
			       ushort byte_number, char *addr_ptr);
extern ushort send_mci_mema_select_cmd(hwidbtype *hwidb, ushort mema_addr);
extern ushort send_mci_mema_write_cmd(hwidbtype *hwidb, ushort mema_value);

extern ushort send_af_select_cmd(hwidbtype *hwidb, ushort af_addr);
extern ushort send_af_write_cmd(hwidbtype *hwidb, ushort af_value);
extern ushort send_get_icb_cmd(hwidbtype *hwidb);
extern ushort send_set_mtu_cmd(hwidbtype *hwidb, ushort mtu, ushort prot_code);
extern ushort send_hip_read_loop_cmd(hwidbtype *hwidb, ushort *mode);
extern ushort send_serial_crc_32_cmd(hwidbtype *hwidb, ushort enabled);
extern ushort send_fsip_crc4_cmd(hwidbtype *hwidb, ushort enabled);
extern ushort send_fsip_ts16_cmd(hwidbtype *hwidb, ushort enabled);
extern ushort send_fsip_clocksource_cmd(hwidbtype *hwidb, ushort enabled);
extern ushort send_fsip_start_slot_g703_cmd(hwidbtype *hwidb, ushort slot);
extern ushort send_fsip_stop_slot_g703_cmd(hwidbtype *hwidb, ushort slot);
extern ushort send_fsip_signal_sts_cmd(hwidbtype *hwidb,
				       ushort *signal_status);
extern ushort send_fsip_nrzi_cmd(hwidbtype *hwidb, ushort enabled);
extern ushort send_fsip_tx_invert_clk_cmd(hwidbtype *hwidb, ushort enabled);
extern ushort send_fsip_halfduplex_cmd(hwidbtype *hwidb, ushort enabled);
extern ushort send_fsip_ignore_dcd_cmd(hwidbtype *hwidb, ushort enabled);
extern ushort send_fsip_max_mtu_supported_cmd(hwidbtype *hwidb, 
					      ushort *max_mtu_supported);
extern ushort send_aip_sonet_cmd(hwidbtype *hwidb, ulong sonetmode);
extern ushort send_aip_framing_cmd(hwidbtype *hwidb, ulong framing);
extern ushort send_aip_addr_select_cmd(hwidbtype *hwidb, ulong addr);
extern ushort send_aip_mem_read_cmd(hwidbtype *hwidb, ushort *data_ptr);
extern ushort send_aip_mem_write_cmd(hwidbtype *hwidb, ushort data);
extern ushort send_aip_cmdblock_cmd(hwidbtype *hwidb, ulong *cmdblock_ptr);
extern ushort send_aip_cmdparse_cmd(hwidbtype *hwidb);
extern ushort send_aip_setver_cmd(hwidbtype *hwidb);
extern ushort send_aip_ds3scramble_cmd(hwidbtype *hwidb, boolean enable);
extern ushort send_ip_ctrlr_load_cmd(dcitype *dci);
extern ushort send_ip_page_select_cmd(dcitype *dci, ushort page);
extern ushort send_bridge_cmd(hwidbtype *hwidb, ushort mask);
extern ushort send_af_read_cmd(hwidbtype *hwidb, ushort *data);
extern ushort send_if_disable_cmd_delay(hwidbtype *hwidb, ulong ms);
extern ushort send_if_enable_cmd_delay(hwidbtype *hwidb, ulong ms);
extern ushort send_status_cmd_delay(hwidbtype *hwidb, ushort arg0, ulong ms);
extern ushort send_fip_en_read_cmd(hwidbtype *hwidb, ushort *data);
extern ushort send_fip_en_write_cmd(hwidbtype *hwidb, ushort data);
extern void send_fip_hack_cmd(hwidbtype *hwidb);
extern ushort send_fip_fm_select_cmd(hwidbtype *hwidb, ushort data);
extern ushort send_cts_delay_cmd(hwidbtype *hwidb, ushort cts);
extern ushort send_cts_watchdog_cmd(hwidbtype *hwidb, ushort timeout);
extern ushort send_fip_applique_select_cmd(hwidbtype *hwidb, ushort data);
extern ushort send_fip_en_select_cmd(hwidbtype *hwidb, ushort reg);
extern ushort send_fip_fm_write_cmd(hwidbtype *hwidb, ushort data);
extern ushort send_reg_select_cmd(hwidbtype *hwidb, ushort reg);
extern ushort send_reg_read_cmd(hwidbtype *hwidb, ushort *data);
extern ushort send_reg_write_cmd(hwidbtype *hwidb, ushort data);
extern ushort send_fip_applique_read_cmd(hwidbtype *hwidb, ushort *data);
extern ushort send_fip_applique_write_cmd(hwidbtype *hwidb, ushort data);
extern ushort send_fip_get_rval_cmd(hwidbtype *hwidb, ushort index, 
				    ushort *data);
extern ushort send_fip_get_py_ls_cmd(hwidbtype *hwidb,
				     ushort index, ushort *data);
extern ushort send_fip_phy_a_int_mask_cmd(hwidbtype *hwidb, ushort data);
extern ushort send_fip_phy_b_int_mask_cmd(hwidbtype *hwidb, ushort data);
extern ushort send_fip_fm_read_cmd(hwidbtype *hwidb, ushort *data);
extern ushort send_version_cmd(slottype*, ushort subcmd, ushort* version);
extern ushort send_global_ptrs_cmd(slottype *slot,
				   ushort event_q_ptr,
				   ushort love_note_free_q_ptr,
				   ushort love_letter_free_q_ptr,
				   ushort *max_love_letter_size);
extern ushort send_raw_queue_ptr_cmd(hwidbtype *hwidb, ushort raw_q_ptr);
extern ushort send_free_queue_ptrs_cmd(hwidbtype *hwidb,
				       ushort local_free_q_ptr,
				       ushort global_free_q_ptr,
				       ushort global_free_q_size);
extern ushort send_rx_limits_cmd(hwidbtype *hwidb,
				 ushort rx_limit_lo, ushort rx_limit_hi);
extern ushort send_tx_ptrs_cmd(hwidbtype *hwidb,
			       ushort tx_q_ptr, ushort tx_acc_ptr);
extern ushort send_return_buffer_cmd(hwidbtype *idb, ushort *rx_count);

extern ushort send_trip_af_or_cmd(hwidbtype *hwidb, ushort data);
extern ushort send_trip_spyg_mem_select_cmd(hwidbtype *hwidb, ushort addr);
extern ushort send_trip_spyg_mem_read_cmd(hwidbtype *hwidb);
extern ushort send_trip_spyg_mem_read_rslt_cmd(hwidbtype *hwidb, ushort *data);
extern ushort send_trip_spyg_mem_write_cmd(hwidbtype *hwidb, ushort data);
extern ushort send_trip_memb_select_cmd(hwidbtype *hwidb, ushort addr);
extern ushort send_trip_memb_cont_write_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_trip_memb_cont_read_cmd(hwidbtype *idb, ushort *value);
extern ushort send_trip_set_mac_hi_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_trip_set_mac_mid_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_trip_set_mac_lo_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_select_cmd(slottype *slotp,
			      ushort iface_num, ushort *if_type);
extern ushort send_trip_poke_cuetrc_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_trip_reset_ph_bit_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_trip_set_ph_bit_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_trip_read_ring_stat_cmd(hwidbtype *idb,
					   ushort *ring_status);
extern ushort send_trip_get_check_ptr_cmd(hwidbtype *idb, ushort *check_ptr);
extern ushort send_trip_accept_mac_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_af_size_cmd(hwidbtype *idb, ushort *size);
extern ushort send_trip_init_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_trip_init_complete_cmd(hwidbtype *hwidb, ushort *error);
extern ushort send_txqlength_cmd(hwidbtype *hwidb, ushort *count);
extern ushort send_short_txq_localbuf_cmd(hwidbtype *hwidb, ushort enable);
extern ushort send_specific_cmd_1arg(hwidbtype *hwidb, ushort cmd, ushort cmd_arg);
extern ushort send_specific_cmd_1arg_lret(hwidbtype *hwidb, ushort cmd, ushort cmd_arg, ulong *retdata);
extern ushort send_specific_cmd_1arg_sret(hwidbtype *hwidb, ushort cmd, ushort cmd_arg, ushort *retdata);
extern ushort send_feip_isl_vlan_colour (idbtype *vlan_subinterface,
					 boolean on_or_off);
extern ushort send_cip_daughter_card_type_cmd(hwidbtype *hwidb, ushort *type);
extern ushort get_cip_cfg_block_cmd(hwidbtype *hwidb, ulong *mem_addr);
extern ushort send_cip_parse_blk_cmd(hwidbtype *hwidb);
extern ushort send_cip_data_rate_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_cip_stats_int_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_pld_version_cmd(hwidbtype *hwidb, ushort *vpld_ver);
extern ushort send_rom_version_cmd(hwidbtype *hwidb, ushort *rom_ver);
extern ushort send_sw_version_cmd(hwidbtype *hwidb, ushort *sw_ver);
extern ushort send_cbus_cip_mema_select_cmd(hwidbtype *hwidb, ulong mema_addr);
extern ushort send_cbus_cip_mema_write_cmd(hwidbtype *hwidb,
					   ushort mema_value);

extern ushort send_ipc_slave_cmd(hwidbtype *hwidb, ulong seat_id, ulong zpid);

extern ushort send_vip_4r_start_block_cmd(hwidbtype *hwidb);
extern ushort send_vip_4r_write_block_cmd(hwidbtype *hwidb, ushort value);
extern ushort send_vip_4r_end_block_cmd(hwidbtype *hwidb);
extern ushort send_vip_4r_stat_request_cmd(hwidbtype *hwidb, ushort type);
extern ushort send_vip_request_config_cmd(hwidbtype *hwidb);
extern ushort send_serial_encap_cmd(hwidbtype *hwidb, ulong encap);

#endif /* !defined(ASMINCLUDE) */

#endif /* !defined(__IF_CMD_H__) */

