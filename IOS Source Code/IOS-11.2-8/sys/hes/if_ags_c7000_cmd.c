/* $Id: if_ags_c7000_cmd.c,v 3.6.8.4 1996/08/29 02:23:25 xtang Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_ags_c7000_cmd.c,v $
 *------------------------------------------------------------------
 * if_ags_c7000_cmd.c:  IP command functions for the AGS and RP1 (C/7000)
 *
 * February 1994, Jack Jenney
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_ags_c7000_cmd.c,v $
 * Revision 3.6.8.4  1996/08/29  02:23:25  xtang
 * CSCdi60447:  FSIP max MTU=14816 not 18000
 * Branch: California_branch
 *              Implemented new FSIP command 93 to pass the available
 *              buffer size in the controller to the system.
 *              The corresponding microcode change are in
 *              cvs -d /micro fsip_q (fsip187-0)
 *              and cvs -d /micro fsip_i (rsp_fsip204-0).
 *
 * Revision 3.6.8.3  1996/08/20  00:34:44  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.6.8.2  1996/08/08  23:55:15  whp4
 * CSCdi52884:  cip ignores mtu size on datagrams to the host
 * Branch: California_branch
 * SP needs to know CIP mtu is different from AIP mtu for shared path.
 *
 * Revision 3.6.8.1  1996/07/08  22:25:16  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.6  1996/02/28  22:42:17  motto
 * CSCdi49312:  CIP IPC not working after router reload
 *
 * Revision 3.5  1996/02/08  20:11:01  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.4  1996/01/23  08:47:47  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.3  1995/11/17  09:17:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:00:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/08/31  22:38:35  izhu
 * CSCdi32813:  SDLC Multidrops need router to ignore DCD for High-End
 * Platform
 * and
 * CSCdi38317  STUN: cannot IPL a 3725 using FSIP
 *
 * Convert to midrange signal model for line state (RTS/CTS state not
 * used),
 * permit DCD to be ignored.
 *
 * Revision 2.6  1995/08/16  18:24:01  kmitchel
 * CSCdi38045:  Resync counter wraps resulting in ineffective timeout
 *              recovery
 *              - Also convert ctrlr_resync() from inline to function
 *
 * Revision 2.5  1995/08/09  00:19:35  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.4  1995/07/13  00:13:46  dlobete
 * CSCdi34151:  UDP transport non-operational
 * Comment fixup.
 *
 * Revision 2.3  1995/07/12  22:26:31  dlobete
 * CSCdi34149:  Cleanup slave applications
 *
 * Revision 2.2  1995/07/06  02:48:14  rharagan
 * CSCdi36490:  pinging on the AIP induced a crash in the SP microcode
 * Send the version command to the IP not the SP if the tx queue appears
 * to hang.
 *
 * Revision 2.1  1995/06/07  20:40:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_cmd.h"
#include "../hes/if_cip.h"
#include "../if/tring.h"
#include "../if/ether.h"
#include "../hes/if_trip.h"
#include "../hes/if_ags_c7000_trip.h"
#include "../hes/if_hes_common.h"
#include "../hes/ctrlr_cmds.h"

#include "../ui/debug.h"
#include "../tbridge/tbridge_debug.h"

#include "../vlan/vlan.h"
#include "../vlan/vlan_debug.h"


/*****************************************************************************
 *
 *****************************************************************************/

/*
 * global variables
 */

 const char Controller_d_s_cmd_d_required_d_attempts[] = 
            "\nController %d %s cmd (%d) required %d attempts.";
 const char Controller_d_s_cmd_d_x_x_required_d_attempts[] =
            "\nController %d %s cmd (%d 0x%-08x) required %d attempts.";
 const char Controller_d_interface_selection_d_required_retry[] =
            "\nController %d interface selection (%d) required retry.";
 const char Controller_d_interface_selection_d_failed_no_retry[] =
            "\nController %d interface selection (%d) failed; no retry.";
 const char ctrlr_msg[] = "crtlr";
 const char cbus_msg[] = "cbus";

/*****************************************************************************
 * Name: select_if
 *
 * Description: Selects an interface for subsequent commands.
 * N.B. The drivers now use this routine before issuing interface
 *      specific commands, *and* before issuing controller specific
 *      commands.  Therefore we select both the controller, and then
 *      the interface.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort select_if (hwidbtype *hwidb)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    ushort if_type;
    
    regptr->argreg = hwidb->cbus_ctrlr_index;
    regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    status = regptr->cmdreg;
    if (status != CMD_RSP_OKAY) {
	return(status);
    }

    fci_grab_if(DCI_FROM_HWIDB(hwidb), regptr,
		&status, hwidb->mci_index,
		&if_type);
    return(status);
}


/*****************************************************************************
 * Name: send_errors_cmd
 *
 * Description: Send a FCI_CMD_ERRORS command to the interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   arg0 -- argument for the errors command 
 *
 * Return Value: 
 *   error_count -- the value of the error counter indicated, or 0 if there
 *     was an error.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
long send_errors_cmd (hwidbtype *hwidb, ushort error_number)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    ulong error_count;

    error_count = 0;
    /*
     * Clear out high bits in case only 16 are returned 
     */
    regptr->largreg = 0; 
     
    fci_cbus_lqry_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CMD_ERRORS, error_number, &error_count,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return (long)error_count;
}


/*****************************************************************************
 * Name: send_applique_cmd
 *
 * Description: Sends a CCB_CMD_APPLIQUE to the interface indicated
 *              This wrapper will work the MCI serial card or the
 *              cbus serial card.  The former takes an argument, while
 *              the latter ignores the argument.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   app_cmd -- applique command to the MCI serial card, the cbus
 *              card ignores what we pass here.
 *   applique_ptr -- pointer to the variable in which to put the applique type
 *     returned from the IP.  This is only written if the status is
 *     CMD_RSP_OKAY. Interpretation of this value is hardware
 *     specific, i.e. the cbus card returns a different code-type from
 *     what the MCI serial card returns.
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_applique_cmd (hwidbtype *hwidb, ushort app_cmd, 
			  ushort *applique_ptr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = app_cmd;
    regptr->cmdreg = MCI_CMD_APPLIQUE;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*applique_ptr = regptr->argreg;
    }
    return(status);
}


/*****************************************************************************
 * Name: send_rx_offset_cmd
 *
 * Description: Sends a FCI_RX_OFFSET command to the interface 
 * indicated.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   rx_offset -- the required rx_offset for the interface
 *                N.B.  Unlike the RSP platform, this parameter
 *                is specified in WORDS (not bytes).
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_rx_offset_cmd (hwidbtype *hwidb, ushort rx_offset)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = rx_offset;
    regptr->cmdreg = FCI_CMD_BUFFERSIZE;
    status = regptr->cmdreg;
    if (status != CMD_RSP_OKAY) {
	return(status);
    }
    fci_cbus_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr, 
			  &status,
			  FCI_CMD_EXECUTE, FCI_RX_OFFSET, 
			  CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}


/*****************************************************************************
 * Name: send_fip_hold_reg_cmd
 *
 * Description: Sends a FCI_FDDI_HOLD_REG command to the interface indicated
 *
 * Note:
 *   This command is only valid for FDDI interfaces
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   hold_reg -- the value for the hold register
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_hold_reg_cmd (hwidbtype *hwidb, ushort hold_reg)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = hold_reg;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_HOLD_REG;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fip_burst_mode_cmd
 *
 * Description: Sends a FCI_FDDI_BURST_MODE command to the interface indicated
 *
 * Note:
 *   This command is only valid for FDDI interfaces
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   burst_mode -- the value for the burst mode register
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_burst_mode_cmd (hwidbtype *hwidb, ushort burst_mode)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = burst_mode;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_BURST_MODE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fip_phy_a_lem_cmd
 *
 * Description: Sends a FCI_FDDI_PHY_A_LEM command to the interface indicated
 *
 * Note:
 *   This command is only valid for FDDI interfaces
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   lem_cmd -- the command to write to the phy A LEM
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_phy_a_lem_cmd (hwidbtype *hwidb, ushort lem_cmd)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = lem_cmd;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_PHY_A_LEM;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fip_phy_b_lem_cmd
 *
 * Description: Sends a FCI_FDDI_PHY_B_LEM command to the interface indicated
 *
 * Note:
 *   This command is only valid for FDDI interfaces
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   lem_cmd -- the command to write to the phy B LEM
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_phy_b_lem_cmd (hwidbtype *hwidb, ushort lem_cmd)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = lem_cmd;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_PHY_B_LEM;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fddi_cam_rd_arg_cmd
 *
 * Description: Send a FCI_FDDI_CAM_RD_ARG command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   read_value -- returns the value read from the CAM argument register
 *   on the FIP, but only if status is CMD_RSP_OKAY. If status indicates
 *   an error, read_value is left untouched.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 * 
 *****************************************************************************/
ushort send_fddi_cam_rd_arg_cmd (hwidbtype *hwidb, ushort *read_value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_FDDI_CAM_RD_ARG;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*read_value = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_fip_cam_wr_arg_cmd
 *
 * Description: Sends a FCI_FDDI_CAM_WR_ARG command to the interface indicated
 *
 * Note:
 *   This command is only valid for FDDI interfaces
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   write_value -- the value to write to the FIP CAM argument register
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fddi_cam_wr_arg_cmd (hwidbtype *hwidb, ushort write_value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = write_value;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_CAM_WR_ARG;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fip_cam_cmd
 *
 * Description: Sends a FCI_FDDI_CAM_CMD command to the interface indicated
 *
 * Note:
 *   This command is only valid for FDDI interfaces
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   cam_op -- the CAM operation code to send with the FCI_FDDI_CAM_CMD
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fddi_cam_cmd (hwidbtype *hwidb, ushort cam_op)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = cam_op;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_CAM_CMD;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*********************************************************************
 * Name: send_fip_cam_cmd_3arg
 *
 * Description: This is RSP specific and hence it is a stub here.
 *
 * Note:
 **********************************************************************/
ushort send_fddi_cam_cmd_3arg(hwidbtype *hwidb, ushort cam_op,
                              ushort arg0,
                              ushort arg1,
                              ushort arg2)
{
    return(TRUE);
}


/*****************************************************************************
 * Name: send_fip_get_state_cmd
 *
 * Description: Send a FCI_FDDI_GET_STATE command to the
 *              interface indicated to read the phy state
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   which_phy -- set to either FCI_PHYA_STATE or FCI_PHYB_STATE
 *
 * Output:
 *   state -- returns the phy state code read from the FIP, but only if
 *   status is CMD_RSP_OKAY. If status indicates an error, state is left
 *   untouched.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 * 
 *****************************************************************************/
ushort send_fip_get_state_cmd (hwidbtype *hwidb,
			       ushort	which_phy,
			       ushort	*state)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = which_phy;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_GET_STATE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY)
	*state = regptr->argreg;

    return(status);
}

/*****************************************************************************
 * Name: send_cbus_buffer_park_cmd
 *
 * Description: Sends a FCI_CBUS_BUFFER_PARK command to the interface indicated
 *
 * The FCI_CBUS_BUFFER_PARK command parks buffers on an interface.
 * Parking buffers on an interface is called stoking the interface.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   slots -- number of buffers to park (i.e. reserve)
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_cbus_buffer_park_cmd (hwidbtype *hwidb, ushort slots)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_ctrlr_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CBUS_BUFFER_PARK, slots,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}


/*****************************************************************************
 * Name: send_rxbytes_cmd
 *
 * Description: Sends a FCI_MAX_BUFFERSIZE command to the interface indicated
 *   Specifies the maximum number of bytes that an interface will be allowed
 *   to received. *size* is in BYTES.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   max_rx_bytes -- the max buffersize for the interface in BYTES
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_rxbytes_cmd (hwidbtype *hwidb, ushort max_rx_bytes)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = max_rx_bytes;
    regptr->cmdreg = FCI_CMD_BUFFERSIZE;
    status = regptr->cmdreg;
    if (status != CMD_RSP_OKAY) {
	return(status);
    }
    fci_cbus_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr, 
			  &status,
			  FCI_CMD_EXECUTE, FCI_MAX_BUFFERSIZE, 
			  CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}


/*****************************************************************************
 * Name: send_rx_buffer_limit_cmd
 *
 * Description: Sends a FCI_RX_BUFFER_LIMIT command to the interface indicated
 *   Fix the maximun number of buffers that may be consumed
 *   by an interface for receive data.
 *              This commands is only used on the AGS+/c7000
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   buffer_limit -- the buffer limit for the interface
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_rx_buffer_limit_cmd (hwidbtype *hwidb, ushort buffer_limit)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = buffer_limit;
    regptr->cmdreg = FCI_CMD_BUFFERSIZE;
    status = regptr->cmdreg;
    if (status != CMD_RSP_OKAY) {
	return(status);
    }

    regptr->argreg = FCI_RX_BUFFER_LIMIT;
    regptr->cmdreg = FCI_CMD_EXECUTE;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_rx_error_cmd
 *
 * Description: Sends a FCI_RX_ERROR command to the interface indicated.
 *   Sets the pool from which the IP gets buffers for love notes.
 *
 * Note:
 *   This command is only used on the C/7000 and AGS+
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   error_pool -- pool number for the interface to use to get love notes
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_rx_error_cmd (hwidbtype *hwidb, ushort error_pool)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = error_pool;
    regptr->cmdreg = FCI_CMD_BUFFERSIZE;
    status = regptr->cmdreg;
    if (status != CMD_RSP_OKAY) {
	return(status);
    }
    fci_cbus_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr, 
			  &status,
			  FCI_CMD_EXECUTE, FCI_RX_ERROR, 
			  CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}


/*****************************************************************************
 * Name: send_assign_poolindex_cmd
 *
 * Description: Sends a FCI_ASSIGN_POOLINDEX command to the interface indicated.
 *   Assign an interface to a particular buffer pool -- for both
 *   receive and transmit buffers.
 *
 * Note:
 *   This command is only used on the C/7000 and AGS+
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   pool -- pool number for the interface to use to for receive and transmit
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_assign_poolindex_cmd (hwidbtype *hwidb, ushort pool)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = pool;
    regptr->cmdreg = FCI_CMD_BUFFERSIZE;
    status = regptr->cmdreg;
    if (status != CMD_RSP_OKAY) {
	return(status);
    }
    fci_cbus_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr, 
			  &status,
			  FCI_CMD_EXECUTE, FCI_ASSIGN_POOLINDEX, 
			  CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}


/*****************************************************************************
 * Name: send_tx_allocate_cmd
 *
 * Description: Sends a FCI_CMD_TX_ALLOCATE command to the interface indicated.
 *
 * Note:
 *   This command is only used on the C/7000 and AGS+
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   pool -- pool number for the interface to use to for receive and transmit
 *   size -- the size of the buffer to allocate
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_tx_allocate_cmd (hwidbtype *hwidb, ushort pool, ushort size)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = size;
    regptr->cmdreg = FCI_CMD_BUFFERSIZE;
    status = regptr->cmdreg;
    if (status != CMD_RSP_OKAY) {
	return(status);
    }

    regptr->argreg = pool;
    regptr->cmdreg = FCI_CMD_TX_ALLOCATE;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_status_cmd
 *
 * Description: Send a FCI_CMD_STATUS command to the interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   arg0 -- argument for the status command
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_status_cmd (hwidbtype *hwidb, ushort arg0)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_cbus_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			  &status,
			  FCI_CMD_STATUS, arg0,
			  CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}


/*****************************************************************************
 * Name: send_if_enable_cmd
 *
 * Description: Send a FCI_CMD_STATUS command with arg MCI_IF_ENABLE
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   hwidb->hw_enabled gets set TRUE.  Drivers can use this field to
 *                     determine whether or not the system thinks the
 *                     interface is enabled.
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_if_enable_cmd (hwidbtype *hwidb)
{
    ushort status;

    hwidb->hw_enabled = TRUE;
    status = send_status_cmd(hwidb, MCI_IF_ENABLE);
    return(status);
}


/*****************************************************************************
 * Name: send_if_disable_cmd
 *
 * Description: Send a FCI_CMD_STATUS command with arg MCI_IF_DISABLE
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   hwidb->hw_enabled gets set FALSE.  Drivers can use this field to
 *                     determine whether or not the system thinks the
 *                     interface is enabled.
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_if_disable_cmd (hwidbtype *hwidb)
{
    ushort status;

    hwidb->hw_enabled = FALSE;
    status = send_status_cmd(hwidb, MCI_IF_DISABLE);
    return(status);
}


/*****************************************************************************
 * Name: send_if_loopback_cmd
 *
 * Description: Send a FCI_CMD_STATUS command with arg MCI_IF_LOOPBACK
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_if_loopback_cmd (hwidbtype *hwidb)
{
    ushort status;

    status = send_status_cmd(hwidb, MCI_IF_LOOPBACK);
    return(status);
}


/*****************************************************************************
 * Name: send_if_noloopback_cmd
 *
 * Description: Send a FCI_CMD_STATUS command with arg MCI_IF_NOLOOPBACK
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_if_noloopback_cmd (hwidbtype *hwidb)
{
    ushort status;

    status = send_status_cmd(hwidb, MCI_IF_NOLOOPBACK);
    return(status);
}


/*****************************************************************************
 * Name: send_if_throttle_cmd
 *
 * Description: Send a FCI_CMD_STATUS command with arg FCI_IF_THROTTLE
 * N.B.  This command is processed by the SP, it is not passed on to
 *       the IPs.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   throttle_state_ptr -- a pointer to a ushort in which to put the present
 *     throttle state of the interface (according to the IP).
 *     *throttle_state_ptr == 0 -- interface is throttled
 *     *throttle_state_ptr != 0 -- interface is *NOT* throttled
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_if_throttle_cmd (hwidbtype *hwidb, ushort *throttle_state_ptr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_IF_THROTTLE;
    regptr->cmdreg = FCI_CMD_STATUS;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*throttle_state_ptr = regptr->argreg;
    }
    return(status);
}


/*****************************************************************************
 * Name: send_ctrlr_reset_cmd
 *
 * Description: Send a FCI_CMD_CTRLR_RESET to an MCI controller.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_ctrlr_reset_cmd (hwidbtype *hwidb)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->cmdreg = FCI_CMD_CTRLR_RESET;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_ctrlr_load_cmd
 *
 * Description: Send a FCI_CMD_CTRLR_LOAD to an MCI controller.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   arg0 -- argument for the controller load command
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_ctrlr_load_cmd (hwidbtype *hwidb, ushort arg0)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = arg0;
    regptr->cmdreg = FCI_CMD_CTRLR_LOAD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_clockrate_cmd
 *
 * Description: Send a MCI_CMD_CLOCKRATE command
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   cookie -- argument for the clockrate command
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_clockrate_cmd (hwidbtype *hwidb, ushort cookie)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = cookie;
    regptr->cmdreg = MCI_CMD_CLOCKRATE;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_clocktype_cmd
 *
 * Description: Send a MCI_CMD_CLOCKTYPE command
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   prescalar -- argument for the clocktype
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_clocktype_cmd (hwidbtype *hwidb, ushort prescalar)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = prescalar;
    regptr->cmdreg = MCI_CMD_CLOCKTYPE;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_alt_idle_cmd
 *
 * Description: 
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enable -- TRUE or FALSE
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_alt_idle_cmd (hwidbtype *hwidb, ushort enable)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = enable;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CMD_ALT_IDLE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_delay_cmd
 *
 * Description: Send a FCI_CMD_DELAY command
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   delay -- delay value to set
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_delay_cmd (hwidbtype *hwidb, ushort delay)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = delay;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CMD_DELAY;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_hip_ta_cmd
 *
 * Description: 
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   ta -- 
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_hip_ta_cmd (hwidbtype *hwidb, ushort ta)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = ta;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_HSCI_TA;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_hip_iclk_cmd
 *
 * Description: 
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   iclk -- 
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_hip_iclk_cmd (hwidbtype *hwidb, ushort iclk)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = iclk;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_HSCI_ICLK;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_hip_lc_inhibit_cmd
 *
 * Description: 
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   lc -- 
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_hip_lc_inhibit_cmd (hwidbtype *hwidb, ushort lc)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = lc;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_HSCI_LC_INHIBIT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_hip_loopback_cmd
 *
 * Description: 
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   loopback -- 
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_hip_loopback_cmd (hwidbtype *hwidb, ushort loopback)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = loopback;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_HSCI_LOOPBACK;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_carrier_cmd
 *
 * Description: Send a MCI_CMD_CARRIER command and gets the response
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   carrier_state_ptr -- a pointer to a ushort in which to put the present
 *     carrier state of the interface (according to the IP).
 *     *carrier_state_ptr == 0 -- no carrier
 *     *carrier_state_ptr != 0 -- carrier
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 * 
 *****************************************************************************/
ushort send_carrier_cmd (hwidbtype *hwidb, ushort *carrier_state_ptr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    *carrier_state_ptr = 0;
    fci_cbus_qry_rslt(DCI_FROM_HWIDB(hwidb), regptr, 
		      &status,
		      FCI_CMD_CARRIER, carrier_state_ptr,
		      CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}


/*****************************************************************************
 * Name: send_lights_cmd
 *
 * Description: Send a FCI_CMD_LIGHTS command 
 *              This commands is only used on MCI interfaces.
 *
 * Input: 
 *   hwidb -- pointer to the mci structure for the controller
 *   lights -- value for the lights command
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_lights_cmd (hwidbtype *hwidb, ushort lights)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = lights;
    regptr->cmdreg = MCI_CMD_LIGHTS;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_station_cmd
 *
 * Description: Send a MCI_CMD_STATION command and get a response value
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   byte_number -- which byte of the station address (ethernet address) to
 *     get from the IP.
 *   addr_ptr -- a pointer to the byte in which to put the address
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_station_cmd (hwidbtype *hwidb, ushort byte_number, char *addr_ptr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    ushort short_reply;

    regptr->argreg = byte_number;
    regptr->cmdreg = MCI_CMD_STATION;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	/*
	 * Ensure we do a word read from the MCI, not bytes,
	 * which are illegal.
	 */
	short_reply = regptr->argreg;
	*addr_ptr = short_reply & 0xFF;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_cbus_cip_mema_select_cmd
 *
 * Description: Send a FCI_CTRLR_ARGUMENT and FCI_CMD_CIP_ADDR_SELECT
 *
 * Assumptions:
 *   1. the interface has been selected
 *
 * Input: 
 *   hwidb -- pointer to the mci structure for the controller
 *   mema_addr -- address in MEMA to select
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_cbus_cip_mema_select_cmd (hwidbtype *hwidb, ulong mema_addr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    
    fci_ctrlr_cmd_larg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			    &status,
			    FCI_CMD_CIP_ADDR_SELECT, mema_addr,
			    CTRLR_CMD_ATTEMPTS, FALSE);
    return(status);
}


/*****************************************************************************
 * Name: send_mci_mema_select_cmd
 *
 * Description: Send a MCI_CMD_MEMA_SELECT
 *
 * Assumptions:
 *   1. the interface has been selected
 *
 * Input: 
 *   hwidb -- pointer to the mci structure for the controller
 *   mema_addr -- address in MEMA to select
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_mci_mema_select_cmd (hwidbtype *hwidb, ushort mema_addr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = mema_addr;
    regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    status = regptr->cmdreg;
    return(status);

}


/*****************************************************************************
 * Name: send_cbus_cip_mema_write_cmd
 *
 * Description: Send a FCI_CMD_CIP_MEM_WRITE controller command
 *
 * Assumptions:
 *   1. the interface has been selected
 *   2. the MEMA address has been selected
 *
 * Input: 
 *   hwidb -- pointer to the mci structure for the controller
 *   mema_value -- value to write to selected address
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_cbus_cip_mema_write_cmd (hwidbtype *hwidb, ushort mema_value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_ctrlr_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CMD_CIP_MEM_WRITE, mema_value,
			   CTRLR_CMD_ATTEMPT_ONCE, FALSE);
    return(status);
}


/*****************************************************************************
 * Name: send_mci_mema_write_cmd
 *
 * Description: Send a MCI_CMD_MEMA_WRITE
 *
 * Assumptions:
 *   1. the interface has been selected
 *   2.  the MEMA address has been selected
 *
 * Input: 
 *   hwidb -- pointer to the mci structure for the controller
 *   mema_addr -- address in MEMA to select
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_mci_mema_write_cmd (hwidbtype *hwidb, ushort mema_value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = mema_value;
    regptr->cmdreg = MCI_CMD_MEMA_WRITE;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_af_select_cmd
 *
 * Description: Send a MCI_CMD_AF_SELECT
 *
 * Assumptions:
 *   1. the interface has been selected
 *
 * Input: 
 *   hwidb -- pointer to the mci structure for the controller
 *   af_addr -- address in the address filter to select
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_af_select_cmd (hwidbtype *hwidb, ushort af_addr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = af_addr;
    regptr->cmdreg = MCI_CMD_AF_SELECT;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_af_write_cmd
 *
 * Description: Send a MCI_CMD_AF_WRITE
 *
 * Assumptions:
 *   1. the interface has been selected
 *   2. the address filter address has been selected
 *
 * Input: 
 *   hwidb -- pointer to the hwidb for the interface
 *   af_value -- value to write to the selected address in the address filter
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_af_write_cmd (hwidbtype *hwidb, ushort af_value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = af_value;
    regptr->cmdreg = MCI_CMD_AF_WRITE;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_get_icb_cmd
 *
 * Description: given a hwidb, it returns the address of the ICB in the cBus
 *              This commands is only used on the AGS+/c7000
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   The address in MEMD of the ICB for the interface
 *****************************************************************************/
ushort send_get_icb_cmd (hwidbtype *hwidb)
{
    return(0);
}

/*****************************************************************************
 * Name: send_set_mtu_cmd
 *
 * Description: Inform the SP/SSP of the protocol MTU of a given interface.
 *              This command is only used on the c7000.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *   mtu -- protocol mtu value
 *   prot_code -- FCI_SET_{IP,IPX,CLNS}_MTU value
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *****************************************************************************/
ushort send_set_mtu_cmd (hwidbtype *hwidb, ushort mtu, ushort prot_code)
{
    mciregtype *regptr = hwidb->mci_regptr;

    regptr->largreg = (mtu << 16) + prot_code;
    regptr->cmdreg = FCI_CMD_SET_MTU;
    return(regptr->cmdreg);
}


/*****************************************************************************
 * Name: send_hip_read_loop_cmd
 *
 * Description: Send a FCI_HSCI_READ_LOOP command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   mode -- returns the loop mode, but only if status is CMD_RSP_OKAY
 *           if status indicates an error, mode is left untouched.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 * 
 *****************************************************************************/
ushort send_hip_read_loop_cmd (hwidbtype *hwidb, ushort *mode)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_HSCI_READ_LOOP;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*mode = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_serial_crc_32_cmd
 *
 * Description: Send a FCI_SERIAL_CRC_32 command to the
 *              interface indicated
 *              This switches between 32-bit and 16-bit CRCs.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean, TRUE indicates crc32 should be enabled,
 *                       FALSE indicates crc16 should be used.
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_serial_crc_32_cmd (hwidbtype *hwidb, ushort enabled)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = enabled;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_SERIAL_CRC_32;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fsip_crc4_cmd
 *
 * Description: Send a FCI_SERIAL_CRC4 command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean, TRUE indicates crc4 should be enabled
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fsip_crc4_cmd (hwidbtype *hwidb, ushort enabled)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = enabled;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_SERIAL_CRC4;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fsip_ts16_cmd
 *
 * Description: Send a FCI_SERIAL_TS16 command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean, TRUE indicates ts16 should be enabled
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fsip_ts16_cmd (hwidbtype *hwidb, ushort enabled)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = enabled;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_SERIAL_TS16;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fsip_clocksource_cmd
 *
 * Description: Send a FCI_SERIAL_CLOCKSOURCE command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fsip_clocksource_cmd (hwidbtype *hwidb, ushort enabled)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = enabled;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_SERIAL_CLOCKSOURCE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fsip_start_slot_g703_cmd
 *
 * Description: Send a FCI_SERIAL_START_SLOT_G703 command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   slot -- requested timeslot number
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fsip_start_slot_g703_cmd (hwidbtype *hwidb, ushort slot)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = slot;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_SERIAL_START_SLOT_G703;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fsip_stop_slot_g703_cmd
 *
 * Description: Send a FCI_SERIAL_STOP_SLOT_G703 command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   slot -- requested timeslot number
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fsip_stop_slot_g703_cmd (hwidbtype *hwidb, ushort slot)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = slot;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_SERIAL_STOP_SLOT_G703;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fsip_signal_sts_cmd
 *
 * Description: Send a FCI_FSIP_SIGNAL_STS command to the
 *              interface indicated.
 *              This command queries the FSIP for the current modem
 *              control signal status.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   signal_status -- if status == CMD_RSP_OKAY,
 *                      then returns the current signal status
 *                      else, is left untouched
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fsip_signal_sts_cmd (hwidbtype *hwidb, ushort *signal_status)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_FSIP_SIGNAL_STS;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY)
      *signal_status = regptr->argreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fsip_nrzi_cmd
 *
 * Description: Send a FCI_FSIP_NRZI command to the
 *              interface indicated
 *              This switches nrzi mode off and on
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean, TRUE indicates nrzi should be enabled,
 *                       FALSE indicates nrzi should be disabled.
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fsip_nrzi_cmd (hwidbtype *hwidb, ushort enabled)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = enabled;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FSIP_NRZI;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fsip_tx_invert_clk_cmd
 *
 * Description: Send a FCI_FSIP_TX_INVERT_CLK command to the
 *              interface indicated
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean, TRUE indicates the tx clock should be inverted
 *                       FALSE indicates it shouldn't
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fsip_tx_invert_clk_cmd (hwidbtype *hwidb, ushort enabled)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = enabled;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FSIP_TX_INVERT_CLK;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fsip_halfduplex_cmd
 *
 * Description: Send a FCI_FSIP_HALFDUPLEX command to the
 *              interface indicated.
 *              This command switches between half and full duplex
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean, TRUE indicates the halfduplex should be enabled
 *                       FALSE indicates fullduplex should be used
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fsip_halfduplex_cmd (hwidbtype *hwidb, ushort enabled)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = enabled;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FSIP_HALFDUPLEX;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_fsip_ignore_dcd_cmd
 *
 * Description: Send a FCI_SERIAL_IGNORE_DCD command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean, TRUE indicates ignore_dcd should be enabled
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fsip_ignore_dcd_cmd (hwidbtype *hwidb, ushort enabled)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = enabled;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_SERIAL_IGNORE_DCD;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/***************************************************************************
 * Name: send_fsip_max_mtu_supported_cmd
 *
 * Description: Send a MAX_MTU_SUPPORTED command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output: 
 *    -- maximum mtu size supported by the local buffering
 *       in the controller.
 *
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 **************************************************************************/
ushort send_fsip_max_mtu_supported_cmd (hwidbtype *hwidb, 
                                      ushort *max_mtu_supported)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = MAX_MTU_SUPPORTED;
    regptr->cmdreg = FCI_CTRLR_CMD;

    status = regptr->cmdreg;
    if (status == FCI_RSP_OKAY) {
      *max_mtu_supported = regptr->argreg;
    }

    return (status);
}


/*****************************************************************************
 * Name: send_aip_sonet_cmd
 *
 * Description: issues an FCI_CMD_AIP_SONET command to the AIP indicated.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *   sonetmode -- the sonet mode to use in the command
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_aip_sonet_cmd (hwidbtype *hwidb, ulong sonetmode)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->largreg = sonetmode;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CMD_AIP_SONET;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_aip_framing_cmd
 *
 * Description: issues an FCI_CMD_AIP_FRAMING command to the AIP indicated.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *   framing -- the framing type to use in the command
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_aip_framing_cmd (hwidbtype *hwidb, ulong framing)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->largreg = framing;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CMD_AIP_FRAMING;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_aip_addr_select_cmd
 *
 * Description: issues an FCI_CMD_AIP_ADDR_SELECT command to the AIP indicated.
 *   It selects an address for subsequent MEM_READ or MEM_WRITE commands.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *   addr -- address in AIP memory to select
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_aip_addr_select_cmd (hwidbtype *hwidb, ulong addr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->largreg = addr;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CMD_AIP_ADDR_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_aip_mem_read_cmd
 *
 * Description: issues an FCI_CMD_AIP_MEM_READ command to the AIP indicated.
 *   It reads a 16-bit (ushort) value from the address selected.  The read
 *   command automatically increments the address so that successive read
 *   commands read through AIP memory.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *   data_ptr -- a pointer to the place the data read is supposed to go.  If
 *     the status code is not CMD_RSP_OKAY, nothing is written to this address.
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_aip_mem_read_cmd (hwidbtype *hwidb, ushort *data_ptr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CMD_AIP_MEM_READ;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*data_ptr = regptr->argreg;
    }
    return(status);
}


/*****************************************************************************
 * Name: send_aip_mem_write_cmd
 *
 * Description: issues an FCI_CMD_AIP_MEM_WRITE command to the AIP indicated.
 *   It writes a 16-bit (ushort) value from the address selected.  The write
 *   command automatically increments the address so that successive write
 *   commands write AIP memory in order.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *   data -- the data to write to the AIP
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_aip_mem_write_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CMD_AIP_MEM_WRITE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_aip_cmdblock_cmd
 *
 * Description: issues an FCI_CMD_AIP_CMDBLOCK command to the AIP indicated.
 *   This command returns the address of the command block buffer in the AIP.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *   cmdblock_ptr -- a pointer to the pointer in which to put the return value
 *     from the command.  If the status is CMD_RSP_OKAY, this value is
 *     written, otherwise it is left unchanged.
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_aip_cmdblock_cmd (hwidbtype *hwidb, ulong *cmdblock_ptr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CMD_AIP_CMDBLOCK;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*cmdblock_ptr = regptr->largreg;
    }
    return(status);
}


/*****************************************************************************
 * Name: send_aip_cmdparse_cmd
 *
 * Description: issues an FCI_CMD_AIP_CMDPARSE command to the AIP indicated.
 *   This command causes the block command in the command block in AIP memory
 *   to be executed. 
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_aip_cmdparse_cmd (hwidbtype *hwidb)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CMD_AIP_CMDPARSE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_aip_ds3scramble_cmd
 *
 * Description: issues an FCI_CMD_AIP_DS3SCRAMBLE command to the AIP indicated.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *   framing -- the framing type to use in the command
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_aip_ds3scramble_cmd (hwidbtype *hwidb, boolean enable)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->largreg = enable;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CMD_AIP_DS3SCRAMBLE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_ip_ctrlr_load_cmd
 *
 * Description: issues an FCI_CTRLR_LOAD command to the IP indicated.
 *              This commands is only used on the AGS+/c7000
 *
 * Input: 
 *   dci -- a pointer to the dci for the ip
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *****************************************************************************/
ushort send_ip_ctrlr_load_cmd (dcitype *dci)
{
    return(CMD_RSP_OKAY);
}


/*****************************************************************************
 * Name: send_ip_page_select_cmd
 *
 * Description: issues an FCI_CTRLR_PAGE_SELECT command to the IP indicated.
 *              This commands is only used on the AGS+/c7000
 *
 * Input: 
 *   dci -- a pointer to the dci for the ip
 *   page -- the page number to be selected
 *
 * Note: the IP must be selected using the select_if() function.
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *****************************************************************************/
ushort send_ip_page_select_cmd (dcitype *dci, ushort page)
{
    return(CMD_RSP_OKAY);
}

/*****************************************************************************
 * Name: send_bridge_cmd
 *
 * Description: Send a FCI_CMD_BRIDGE command to the i'face
 *              indicated.  This commands loads the mask provided
 *              into the SPs bridgemask variable.
 *              This commands is only used on the AGS+/c7000
 *              
 *
 * Input: 
 *   hwidb -- pointer to a hardware idb for the interface
 *   mask -- the value to be written to the bridging mask
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 * 
 *****************************************************************************/
ushort send_bridge_cmd (hwidbtype *hwidb, ushort mask)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = mask;
    regptr->cmdreg = FCI_CMD_BRIDGE;
    status = regptr->cmdreg;

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s FCI_CMD_BRIDGE mask %04x status %04x",
	       hwidb->hw_namestring, mask, status);
#endif  /* TBRIDGE_DEBUG */

    return(status);
}

/*****************************************************************************
 * Name: send_af_read_cmd
 *
 * Description: Send a FCI_CMD_AF_READ command to the 
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   data -- returns the address filter data, if status == CMD_RSP_OKAY
 *           if status indicates an error, data is left untouched.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 * 
 *****************************************************************************/
ushort send_af_read_cmd (hwidbtype *hwidb, ushort *data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->cmdreg = FCI_CMD_AF_READ;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*data = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_if_disable_cmd_delay
 *
 * Description: Send a FCI_CMD_STATUS command with arg MCI_IF_DISABLE,
 *              Identical to send_if_disable_cmd() except that it
 *              delays the specified time before returning.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   ms -- the delay in ms.  This routine will wait this long
 *         before returning
 *
 *
 * Output:
 *   hwidb->hw_enabled gets set FALSE.  Drivers can use this field to
 *                     determine whether or not the system thinks the
 *                     interface is enabled.
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *****************************************************************************/
ushort send_if_disable_cmd_delay (hwidbtype *hwidb, ulong ms)
{
    ushort status;

    hwidb->hw_enabled = FALSE;

    status = send_status_cmd_delay(hwidb, MCI_IF_DISABLE, ms);
    return(status);
}

/*****************************************************************************
 * Name: send_if_enable_cmd_delay
 *
 * Description: Send a FCI_CMD_STATUS command with arg MCI_IF_ENABLE,
 *              Identical to send_if_enable_cmd() except that it
 *              delays the specified time before returning.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   ms -- the delay in ms.  This routine will wait this long
 *         before returning.
 *
 * Output:
 *   hwidb->hw_enabled gets set TRUE.  Drivers can use this field to
 *                     determine whether or not the system thinks the
 *                     interface is enabled.
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *****************************************************************************/
ushort send_if_enable_cmd_delay (hwidbtype *hwidb, ulong ms)
{
    ushort status;

    hwidb->hw_enabled = TRUE;

    status = send_status_cmd_delay(hwidb, MCI_IF_ENABLE, ms);
    return(status);
}

/*****************************************************************************
 * Name: send_status_cmd_delay
 *
 * Description: Send a FCI_CMD_STATUS command to the interface
 *              indicated with a delay.  Identical to send_status_cmd
 *              except it waits a specified time before returning.
 *              This allows the interface time to complete the
 *              command before other commands can be sent to it.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   arg0 -- argument for the status command
 *   ms -- the delay to wait
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *****************************************************************************/
ushort send_status_cmd_delay (hwidbtype *hwidb, ushort arg0, ulong ms)
{
    ushort status;

    status = send_status_cmd(hwidb, arg0);
    /*
     * Delay before returning 
     */
    DELAY(ms);
    return(status);
}

/*****************************************************************************
 * Name: send_fip_en_read_cmd
 *
 * Description: Send a FDDI_CMD_EN_READ command to the
 *              interface indicated.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   data -- the value read from the FIP. Only written if status ==
 *      CMD_RSP_OKAY, otherwise untouched.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_en_read_cmd (hwidbtype *hwidb, ushort *data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->cmdreg = FDDI_CMD_EN_READ;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*data = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_fip_en_write_cmd
 *
 * Description: Send a FDDI_CMD_EN_WRITE command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   data -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_en_write_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FDDI_CMD_EN_WRITE;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_fip_hack_cmd
 *
 * Description: Send the FDDI hack command.  What follows is some
 *              some mail from Tony Li that attempts to explain
 *              what this command does:
 *
 * The folks who did the FIP were, ummm, inelegant in some aspects and
 * stole some bits out of the top of the rxlength value to pass some CAM
 * information.  Now, we go to do SSE switching, the ALU needs to easily
 * and quickly identify these interfaces so that it can perform its
 * magic.  This was done by storing the FDDI hack value in the icb.  The
 * ucode checks the inbound icb when doing its work.
 * We're not very happy with this solution, and thus it has this name.
 *
 * The good news is that it is only relevant to the SP, it does not
 * get passed on to the FIP, and hence is a stub on the RSP, but is
 * implemented here "as is" for the RP1/AGS.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   none
 *
 *****************************************************************************/
void send_fip_hack_cmd (hwidbtype *hwidb)
{
    mciregtype *regptr = hwidb->mci_regptr;

    regptr->argreg = 1;
    regptr->cmdreg = FCI_CMD_ARGUMENT;
    regptr->argreg = FCI_FDDI_HACK;
    regptr->cmdreg = FCI_CMD_EXECUTE;
}

/*****************************************************************************
 * Name: send_fip_fm_select_cmd
 *
 * Description: Send a FDDI_CMD_FM_SELECT command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   data --
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_fm_select_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_cts_delay_cmd
 *
 * Description: Send a MCI_CMD_CTS_DELAY command to the
 *              interface indicated.
 *              NOTE: This command is used for MCI serial IPs only.
 *              Since such an IP does not exist on RSP, our version
 *              of this routine does nothing.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   delay --
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_cts_delay_cmd (hwidbtype *hwidb, ushort cts)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = cts;
    regptr->cmdreg = MCI_CMD_CTS_DELAY;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_cts_watchdog_cmd
 *
 * Description: Send a MCI_CMD_CTS_WATCHDOG command to the
 *              interface indicated.
 *              NOTE: This command is used for MCI serial IPs only.
 *              Since such an IP does not exist on RSP, our version
 *              of this routine does nothing.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   timeout --
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_cts_watchdog_cmd (hwidbtype *hwidb, ushort timeout)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = timeout;
    regptr->cmdreg = MCI_CMD_CTS_WATCHDOG;
    status = regptr->cmdreg;
    return(status);
}


/*****************************************************************************
 * Name: send_fip_en_select_cmd
 *
 * Description: Send a FDDI_CMD_EN_SELECT command to the
 *              interface indicated.  This selects a register.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   reg -- the register to be selected
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_en_select_cmd (hwidbtype *hwidb, ushort reg)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = reg;
    regptr->cmdreg = FDDI_CMD_EN_SELECT;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fip_fm_write_cmd
 *
 * Description: Send a FDDI_CMD_FM_WRITE command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   data -- the data to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_fm_write_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FDDI_CMD_FM_WRITE;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_reg_select_cmd
 *
 * Description: Send a FCI_CTRLR_REG_SELECT command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   reg -- the register to select
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_reg_select_cmd (hwidbtype *hwidb, ushort reg)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = reg;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTRLR_REG_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_reg_read_cmd
 *
 * Description: Send a FCI_CTRLR_REG_READ command to the 
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   data -- returns the register data, if status == CMD_RSP_OKAY
 *           if status indicates an error, data is left untouched.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_reg_read_cmd (hwidbtype *hwidb, ushort *data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CTRLR_REG_READ;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*data = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_reg_write_cmd
 *
 * Description: Send a FCI_CTRLR_REG_WRITE command to the 
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   data -- the register data to write
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_reg_write_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTRLR_REG_WRITE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fip_applique_select_cmd
 *
 * Description: Send a FDDI_CMD_APPLIQUE_SELECT command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   data -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_applique_select_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FDDI_CMD_APPLIQUE_SELECT;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fip_applique_read_cmd
 *
 * Description: Send a FDDI_CMD_APPLIQUE_READ command to the 
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   data -- returns the read data, if status == CMD_RSP_OKAY
 *           if status indicates an error, data is left untouched.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_applique_read_cmd (hwidbtype *hwidb, ushort *data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->cmdreg = FDDI_CMD_APPLIQUE_READ;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*data = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_fip_applique_write_cmd
 *
 * Description: Send a FDDI_CMD_APPLIQUE_WRITE command to the
 *              interface indicated.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   data -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_applique_write_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FDDI_CMD_APPLIQUE_WRITE;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fip_get_rval_cmd
 *
 * Description: Send an FCI_FDDI_GET_RVAL command to the 
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   index -- indicates PHY_A or PHY_B
 *
 * Output:
 *   data -- returns the read data, if status == CMD_RSP_OKAY
 *           if status indicates an error, data is left untouched.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_get_rval_cmd (hwidbtype *hwidb, ushort index, ushort *data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = index;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_GET_RVAL;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*data = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_fip_get_py_ls_cmd
 *
 * Description: Send a FCI_FDDI_PHY_GET_LS command to the 
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   index -- indicates GET_PHYA_LS or GET_PHYB_LS
 *
 * Output:
 *   data -- returns the read data, if status == CMD_RSP_OKAY
 *           if status indicates an error, data is left untouched.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_get_py_ls_cmd (hwidbtype *hwidb, ushort index, ushort *data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = index;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_PHY_GET_LS;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*data = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_fip_phy_a_int_mask_cmd
 *
 * Description: Send a FCI_FDDI_PHY_A_INT_MASK command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   data -- the data to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_phy_a_int_mask_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_PHY_A_INT_MASK;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fip_phy_b_int_mask_cmd
 *
 * Description: Send a FCI_FDDI_PHY_B_INT_MASK command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   data -- the data to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_phy_b_int_mask_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_PHY_B_INT_MASK;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_fip_fm_read_cmd
 *
 * Description: Send a FDDI_CMD_FM_READ command to the 
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   data -- returns the register data, if status == CMD_RSP_OKAY
 *           if status indicates an error, data is left untouched.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fip_fm_read_cmd (hwidbtype *hwidb, ushort *data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->cmdreg = FDDI_CMD_FM_READ;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*data = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_trip_af_or_cmd
 *
 * Description: Send a FCI_CTR_AF_OR command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   data -- the data to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_af_or_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_AF_OR;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);

}

/*****************************************************************************
 * Name: send_trip_spyg_mem_select_cmd
 *
 * Description: Send a FCI_CTR_SPYG_MEM_SELECT command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   addr -- the spyglass memory address to be  selected
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_spyg_mem_select_cmd (hwidbtype *hwidb, ushort addr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = addr;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_SPYG_MEM_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_spyg_mem_read_cmd
 *
 * Description: Send a FCI_CTR_SPYG_MEM_READ command to the
 *              interface indicated.  The user should have
 *              previously selected the SPYG address with a
 *              call to send_trip_spyg_mem_select_cmd().  This
 *              routine merely posts the read request.  The
 *              caller should then delay some period, and finally
 *              issue a send_trip_spyg_mem_read_rslt_cmd() to 
 *              actually fetch the read data.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_spyg_mem_read_cmd (hwidbtype *hwidb)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CTR_SPYG_MEM_READ;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_spyg_mem_read_rslt_cmd
 *
 * Description: Send a FCI_CTR_SPYG_MEM_READ_RSLT command to the
 *              interface indicated.  The user should have
 *              previously issued a send_trip_spyg_mem_read_cmd().
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   data  -- the read data
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_spyg_mem_read_rslt_cmd (hwidbtype *hwidb, ushort *data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CTR_SPYG_MEM_READ_RSLT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*data = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_trip_spyg_mem_write_cmd
 *
 * Description: Send a FCI_CTR_SPYG_MEM_WRITE command to the
 *              interface indicated.  The user should have
 *              previously issued a send_trip_spyg_mem_read_cmd().
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   data  -- the data to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_spyg_mem_write_cmd (hwidbtype *hwidb, ushort data)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = data;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_SPYG_MEM_WRITE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_memb_select_cmd
 *
 * Description: Send a FCI_CTR_MEMB_SELECT command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   addr -- the MEMB address to be  selected
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_memb_select_cmd (hwidbtype *hwidb, ushort addr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = addr;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_memb_cont_write_cmd
 *
 * Description: Send a FCI_CTR_MEMB_C_WRITE command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value  -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_memb_cont_write_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = value;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_C_WRITE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_memb_cont_read_cmd
 *
 * Description: Send an FCI_CTR_MEMB_C_READ command to the
 *              IP indicated.
 *              
 *
 * Input: 
 *   idb -- pointer to the hwidb indicating the i'face to be sent the command
 *
 * Output:
 *   value --  the data read from MEMB
 *             N.B. This field is only written to if the return status
 *             is CMD_RSP_OKAY.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_memb_cont_read_cmd (hwidbtype *hwidb, ushort *value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CTR_MEMB_C_READ;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*value = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
 * Name: send_trip_set_mac_hi_cmd
 *
 * Description: Send a FCI_CTR_MAC_HIGH command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value  -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_set_mac_hi_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = value;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MAC_HIGH;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_set_mac_mid_cmd
 *
 * Description: Send a FCI_CTR_MAC_MIDDLE command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value  -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_set_mac_mid_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = value;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MAC_MIDDLE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_set_mac_high_lo_cmd
 *
 * Description: Send a FCI_CTR_MAC_LOW command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value  -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_set_mac_lo_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = value;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MAC_LOW;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_poke_cuetrc_cmd
 *
 * Description: Send a FCI_CTR_POKE_CUETRC_CMD command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value  -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_poke_cuetrc_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = value;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_POKE_CUETRC_CMD;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_reset_ph_bit_cmd
 *
 * Description: Send a FCI_CTR_RESET_PH_BIT command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value  -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_reset_ph_bit_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = value;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_RESET_PH_BIT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_set_ph_bit_cmd
 *
 * Description: Send a FCI_CTR_SET_PH_BIT command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value  -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_set_ph_bit_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = value;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_SET_PH_BIT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_read_ring_stat_cmd
 *
 * Description: Send an FCI_CTR_READ_RING_STAT command to the
 *              i'face indicated.
 *              
 *
 * Input: 
 *   idb -- pointer to the hwidb indicating the i'face to be sent the command
 *
 * Output:
 *   ring_status --  
 *              the ring status returned from the i'face.
 *              N.B. This field is only written to if the return status
 *              is CMD_RSP_OKAY.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_read_ring_stat_cmd (hwidbtype *hwidb, ushort *ring_status)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CTR_READ_RING_STAT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*ring_status = regptr->argreg;
    }
    return (status);
}

/*****************************************************************************
 * Name: send_trip_get_check_ptr_cmd
 *
 * Description: Send an FCI_CTR_GET_CHECK_PTR command to the
 *              i'face indicated.
 *              
 *
 * Input: 
 *   idb -- pointer to the hwidb indicating the i'face to be sent the command
 *
 * Output:
 *   check_ptr --  
 *              the check_ptr returned from the i'face.
 *              N.B. This field is only written to if the return status
 *              is CMD_RSP_OKAY.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_get_check_ptr_cmd (hwidbtype *hwidb, ushort *check_ptr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CTR_GET_CHECK_PTR;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*check_ptr = regptr->argreg;
    }
    return (status);
}

/*****************************************************************************
 * Name: send_trip_accept_mac_cmd
 *
 * Description: Send a FCI_CTR_ACCEPT_MAC command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value  -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_accept_mac_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = value;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_ACCEPT_MAC;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_af_size_cmd
 *
 * Description: Send an FCI_CMD_AF_SIZE command to the
 *              i'face indicated.
 *              
 *
 * Input: 
 *   idb -- pointer to the hwidb indicating the i'face to be sent the command
 *
 * Output:
 *   size --    the size of the AF RAM returned from the i'face.
 *              N.B. This field is only written to if the return status
 *              is CMD_RSP_OKAY.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_af_size_cmd (hwidbtype *hwidb, ushort *size)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->cmdreg = FCI_CMD_AF_SIZE;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*size = regptr->argreg;
    }
    return (status);
}

/*****************************************************************************
 * Name: send_trip_init_cmd
 *
 * Description: Send a FCI_CTR_INIT command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value  -- the value to be written
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_init_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = value;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_INIT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_trip_init_complete_cmd
 *
 * Description: Send a FCI_CTR_INIT_COMPLETE command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   error -- the error code returned from the interface
 *            only written to if status is CMD_RSP_OKAY
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_trip_init_complete_cmd (hwidbtype *hwidb, ushort *error)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CTR_INIT_COMPLETE;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*error = regptr->argreg;
    }
    return(status);
}
 

/*****************************************************************************
 * Name: send_txqlength_cmd
 *
 * Description: Send a FCI_CMD_TXQLENGTH command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   count -- the count returned from the interface
 *            only written to if status is CMD_RSP_OKAY
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_txqlength_cmd (hwidbtype *hwidb, ushort *count)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    regptr->argreg = FCI_CMD_TXQLENGTH;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == CMD_RSP_OKAY) {
	*count = regptr->argreg;
    }
    return(status);
}

/*****************************************************************************
* Name: send_short_txq_localbuf_cmd
*
* Description: Send a FCI_SHORT_TXQ_LOCALBUF command to the
*              interface indicated
*
* Input: 
*   hwidb -- pointer to the hardware IDB for the interface
* 
* Output:
*   none
*
* Return Value:
*   status -- the status of the command: CMD_RSP_OKAY if successful,
*     otherwise an error code.
*
*****************************************************************************/
ushort send_short_txq_localbuf_cmd (hwidbtype *hwidb, ushort enabled)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status; 
    regptr->argreg = enabled ? TRUE : FALSE;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CMD_SHORT_TXQ_LOCALBUF;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: send_daughter_card_type_cmd
 *
 * Description: Send a FCI_CMD_CIP_DAUGHTER_TYPE command to the
 *              interface indicated.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   type  -- the daughter type returned from the interface
 *            only written to if status is CMD_RSP_OKAY
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_cip_daughter_card_type_cmd (hwidbtype *hwidb, ushort *type)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    
    fci_ctrlr_qry_rslt(DCI_FROM_HWIDB(hwidb), regptr,
		       &status,
		       FCI_CMD_CIP_DAUGHTER_TYPE, type,
		       CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: get_cip_cfg_block_cmd
 *
 * Description: Send a FCI_CMD_CIP_CFGBLOCK command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   mem_addr -- a CIP memory address; used for writing to CIP memory
 *            only written to if status is CMD_RSP_OKAY
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort get_cip_cfg_block_cmd (hwidbtype *hwidb, ulong *mem_addr)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    
    fci_ctrlr_lqry_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			&status,
			FCI_CMD_CIP_CFGBLOCK, mem_addr,
			CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_cip_parse_blk_cmd
 *
 * Description: Send a FCI_CMD_CIP_CFGPARSE command to the
 *              interface indicated.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_cip_parse_blk_cmd (hwidbtype *hwidb)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    
    fci_ctrlr_cmd_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			&status,
			FCI_CMD_CIP_CFGPARSE,
			CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_cip_data_rate_cmd
 *
 * Description: Send a FCI_CMD_CIP_SET_PCA_BW command to the
 *              interface indicated.  Only valid for PCA port adapters
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value -- data rate 
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_cip_data_rate_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_ctrlr_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CMD_CIP_SET_PCA_BW, value,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_cip_stats_int_cmd
 *
 * Description: Send a FCI_CMD_CIP_STATS_INTERVAL command to the 
 *              interface indicated.  This tells the CIP how frequently
 *              to send up love letters.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value -- interval value, specified in seconds.
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_cip_stats_int_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_ctrlr_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CMD_CIP_STATS_INTERVAL, value,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_pld_version_cmd
 *
 * Description: Send a FCI_CTRLR_VERSION, FCI_PLD_VERSION command to the 
 *              interface indicated. 
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   vpld_ver  -- the vpld version returned by the IP.
 *                only written to if status is CMD_RSP_OKAY
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_pld_version_cmd (hwidbtype *hwidb, ushort *vpld_ver)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    
    fci_ctrlr_qry_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CTRLR_VERSION, FCI_PLD_VERSION, vpld_ver,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_rom_version_cmd
 *
 * Description: Send a FCI_CTRLR_VERSION, FCI_ROM_VERSION command to the 
 *              interface indicated. 
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   rom_ver  -- the EPROM version returned by the IP.
 *               only written to if status is CMD_RSP_OKAY
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   Uses errmsg() to report command failure.
 *
 *****************************************************************************/
ushort send_rom_version_cmd (hwidbtype *hwidb, ushort *rom_ver)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    
    fci_ctrlr_qry_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CTRLR_VERSION, FCI_ROM_VERSION, rom_ver,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_sw_version_cmd
 *
 * Description: Send a FCI_CTRLR_VERSION, FCI_SW_VERSION command to the 
 *              interface indicated. 
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   sw_ver  -- the SW version returned by the IP.
 *               only written to if status is CMD_RSP_OKAY
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 * Side effects:
 *   None. Used specifically to see if i/f is alive.
 *
 *****************************************************************************/
ushort send_sw_version_cmd (hwidbtype *hwidb, ushort *sw_ver)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    
    fci_ctrlr_qry_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CTRLR_VERSION, FCI_SW_VERSION, sw_ver,
			   CTRLR_CMD_ATTEMPTS, FALSE);
    return(status);
}


/*
 * send_feip_isl_vlan_colour ()
 *
 * This encodes an ISL vLAN Identifier in an FEIP command such that
 * the FEIP ucode can autonomously switch ISL packets corresponding
 * to an SSE cache entry. (Currently only done for IP).
 */

ushort send_feip_isl_vlan_colour (idbtype *vlan_subinterface, boolean on_or_off)
{

mciregtype *regptr = vlan_subinterface->hwptr->mci_regptr;
ulong       colour_and_cmd;
ushort      status;

    /*
     * Setting the top bit (i.e. "on") indicates that the FEIP should
     * pass the interior packet encapsulated with given vLAN colour to
     * the SSP, to let it Silicon Switch it.
     */

    if (on_or_off == TRUE) {

        colour_and_cmd = (((vlan_subinterface->isl_vlan->colour |
                       SET_vLAN_SSE_SWITCHING) << 16) | FEIP_ISL_COLOUR_ENTRY);

        if (vlan_packet_debug) 

            buginf ("\n\nvLAN: FEIP/SSE downloading ISL vLAN ID %d.\n",
                     vlan_subinterface->isl_vlan->colour);

    } else {

        colour_and_cmd = (((vlan_subinterface->isl_vlan->colour |
                     CLEAR_vLAN_SSE_SWITCHING) << 16) | FEIP_ISL_COLOUR_ENTRY);

        if (vlan_packet_debug) 

            buginf ("\n\nvLAN: FEIP/SSE clearing ISL vLAN ID %d.\n",
                     vlan_subinterface->isl_vlan->colour);
    }

    fci_ctrlr_cmd_larg_rslt(DCI_FROM_HWIDB(vlan_subinterface->hwptr), regptr,
			    &status, FCI_FEIP_SPECIFIC, colour_and_cmd, 
			    CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_specific_cmd_1arg
 *
 * Description: Send a specific controller command with 1 argument
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   cmd -- the command to be issued
 *   cmd_arg -- the argument for that command
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_specific_cmd_1arg (hwidbtype *hwidb, ushort cmd, ushort cmd_arg)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_ctrlr_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   cmd, cmd_arg,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_specific_cmd_1arg_lret
 *
 * Description: Send a specific controller command with 1 argument
 * and return the long value resulting from that command
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   cmd -- the command to be issued
 *   cmd_arg -- the argument for that command
 *
 * Output:
 *   retdata
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_specific_cmd_1arg_lret (hwidbtype *hwidb, ushort cmd, ushort cmd_arg, ulong *retdata)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    
    fci_ctrlr_lqry_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			    &status,
			    cmd, cmd_arg, retdata,
			    CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_ipc_slave_cmd
 *
 * Description:
 *
 * Note:
 *   This command is only valid for IP's capable as IPC slaves.
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_ipc_slave_cmd (hwidbtype *hwidb, ulong seat_id, ulong zpid )
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    leveltype   level;
 
    level = raise_interrupt_level(NETS_DISABLE);
 
    status = select_if(hwidb);
    if (status != CMD_RSP_OKAY) {
	reset_interrupt_level(level);
        return(status);
    }

    fci_ctrlr_cmd_2larg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			     &status,
			     FCI_SEND_IPC_SLAVE,
                             seat_id, zpid,
			     CTRLR_CMD_ATTEMPTS, TRUE);

    reset_interrupt_level(level);
    return(status);
}

/*****************************************************************************
 * Name: ctrlr_resync
 *
 * Description: Recover from CMD_RSP_ERR_TMOUT errors by sending the
 *              the Interface Processor a resync command.
 *
 * Input: 
 *   dci    - pointer to structure containing the resynchronization info
 *   regptr - pointer to the MCI registers
 *   msg    - boolean indicator of whether to report resynchronization failure 
 *
 * Output:
 *   resync counters are updated.
 *
 * Return Value:
 *   boolean -- TRUE if the resynchronization was successful, 
 *              FALSE if unsuccessful, or if the IP doesn't support it.
 *
 *****************************************************************************/
boolean ctrlr_resync (
    dcitype *dci,
    mciregtype *regptr,
    boolean msg)
{
    ushort sresult;
    ushort response;
    int count = 0;

    if (!dci)
	return FALSE;
    
    dci->ctrlr_sync_calls++;
    /*
     * Return if the controller doesn't support resync
     */
    if (!dci->resyncable)
	return FALSE;

    regptr->argreg = dci->ctrlr_unit;
    regptr->cmdreg = FCI_CMD_CTRLR_SELECT;

    dci->ctrlr_sync_times++;
    do {
	regptr->argreg = ++dci->ctrlr_sync_count;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_CTRLR_CMDSYNC;
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	if (sresult == CMD_RSP_OKAY) {
	    response = regptr->argreg;
	    if (response == (ushort) dci->ctrlr_sync_count) {
		if (count > dci->ctrlr_sync_phase)
		    dci->ctrlr_sync_phase = count;
		return TRUE;	
	    }
	}
	if (count%2)
	    DELAY(CTRLR_RESYNC_PAUSE_MS);
    }
    while (++count < CTRLR_RESYNC_ATTEMPTS);
    /*
     * Should this indicate that the dci->resyncable should be reset to FALSE ?
     * It most likely means the controller is not participating in the resync 
     * Need a worser message here ?
     */
    if (msg)
	errmsg(&msgsym(CTRLRCMDFAIL1, CBUS), 
	       dci->ctrlr_unit, FCI_CTRLR_CMDSYNC, 
	       dci->ctrlr_sync_count, sresult, count);

    /* Reflect badness */
    dci->ctrlr_sync_fails++;    
    dci->ctrlr_sync_phase = count;
    return FALSE;
}

/*****************************************************************************
 * Name: send_vip_4r_start_block_cmd
 *
 * Description: Send FCI_CMD_VIP_4R_START_BLOCK command
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_vip_4r_start_block_cmd (hwidbtype *hwidb)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_ctrlr_cmd_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CMD_VIP_4R_START_BLOCK,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_vip_4r_write_block_cmd
 *
 * Description: Send FCI_CMD_VIP_4R_WRITE_BLOCK command
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value -- data to write
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_vip_4r_write_block_cmd (hwidbtype *hwidb, ushort value)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_ctrlr_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CMD_VIP_4R_WRITE_BLOCK, value,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_vip_4r_end_block_cmd
 *
 * Description: Send FCI_CMD_VIP_4R_END_BLOCK command
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_vip_4r_end_block_cmd (hwidbtype *hwidb)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_ctrlr_cmd_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CMD_VIP_4R_END_BLOCK,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_vip_4r_stat_request_cmd
 *
 * Description: Send FCI_CMD_VIP_4R_REQUEST_STATS command
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   type  -- type of stats 
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_vip_4r_stat_request_cmd (hwidbtype *hwidb, ushort type)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_ctrlr_cmd_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CMD_VIP_4R_REQUEST_STATS, type,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

/*****************************************************************************
 * Name: send_vip_request_config_cmd
 *
 * Description: Send FCI_CMD_VIP_REQUEST_CONFIG command
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_vip_request_config_cmd (hwidbtype *hwidb)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;

    fci_ctrlr_cmd_rslt(DCI_FROM_HWIDB(hwidb), regptr,
			   &status,
			   FCI_CMD_VIP_REQUEST_CONFIG,
			   CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}
/*****************************************************************************+  * Name: send_specific_cmd_1arg_sret
 *
 * Description: Send a specific controller command with 1 argument
 * and return the short value resulting from that command
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   cmd -- the command to be issued
 *   cmd_arg -- the argument for that command
 *
 * Output:
 *   retdata
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_specific_cmd_1arg_sret (hwidbtype *hwidb, ushort cmd, ushort cmd_arg, ushort *retdata)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    
    fci_ctrlr_qry_arg_rslt(DCI_FROM_HWIDB(hwidb), regptr,
                          &status,
                          cmd, cmd_arg, retdata,
                          CTRLR_CMD_ATTEMPTS, TRUE);
    return(status);
}

