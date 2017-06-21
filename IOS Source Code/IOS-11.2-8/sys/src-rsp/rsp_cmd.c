/* $Id: rsp_cmd.c,v 3.3.20.5 1996/08/29 02:25:08 xtang Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_cmd.c,v $
 *------------------------------------------------------------------
 * rsp_cmd -- Command wrapper functions for the RSP
 *
 * June 1994 Jack Jenney
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_cmd.c,v $
 * Revision 3.3.20.5  1996/08/29  02:25:08  xtang
 * CSCdi60447:  FSIP max MTU=14816 not 18000
 * Branch: California_branch
 *              Implemented new FSIP command 93 to pass the available
 *              buffer size in the controller to the system.
 *              The corresponding microcode change are in
 *              cvs -d /micro fsip_q (fsip187-0)
 *              and cvs -d /micro fsip_i (rsp_fsip204-0).
 *
 * Revision 3.3.20.4  1996/08/28  13:13:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.20.3  1996/07/08  22:52:32  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/05/22  23:26:23  rramacha
 * CSCdi58249:  Enable IP DFS on 4T
 * Branch: California_branch
 * Pass serial encap to serial driver in VIP
 *
 * Revision 3.3.20.1  1996/03/18  22:01:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/07  10:44:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.2  1996/02/20  18:00:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/01  06:09:42  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2.26.1  1996/01/24  23:03:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  18:47:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:28:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/31  22:38:26  izhu
 * CSCdi32813:  SDLC Multidrops need router to ignore DCD for High-End
 * Platform
 * and
 * CSCdi38317  STUN: cannot IPL a 3725 using FSIP
 *
 * Convert to midrange signal model for line state (RTS/CTS state not
 * used),
 * permit DCD to be ignored.
 *
 * Revision 2.4  1995/08/15  22:49:19  kramling
 * CSCdi38825:  MII activation fails in RSP under 11.0(1.0.2)
 * send_feip_specific_cmd() didn't get filled in.
 *
 * Revision 2.3  1995/08/09  00:21:55  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.2  1995/07/16  22:30:53  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.1  1995/06/07  22:57:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "logger.h"
#include "interface_private.h"
#include "rsp_commands.h"
#include "../hes/if_fci.h"
#include "../hes/ccb_cmd.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/if_cmd.h"
#include "rsp_bufhdr.h"
#include "rsp_if.h"
#include "rsp_qasic.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */




/*****************************************************************************
 * Reporting CCB command failures 
 *
 * The status_msg and status_msg_1arg functions provide a convenient way for
 * command wrappers to report a failure using errmsg() 
 *
 *****************************************************************************/

/*****************************************************************************
 * Name: status_msg
 *
 * Description: Reports a command failure via errmsg 
 *              if status is not CMD_RSP_OKAY
 *
 * Input: 
 *   status -- the error code to check
 *   hwidb  -- pointer to the hardware IDB for the interface
 *   cmd    -- the CCB command that returned the status
 *
 * Return Value: 
 *   void
 *
 *****************************************************************************/
void status_msg (ushort status, hwidbtype *hwidb, ushort cmd)
{
    if (status)
	errmsg(&msgsym(CCBCMDFAIL0, CBUS), 
	       hwidb->slot, cmd, status);
}


/*****************************************************************************
 * Name: status_msg_1arg
 *
 * Description: Reports a command failure via errmsg 
 *              if status is not CMD_RSP_OKAY
 *
 * Input: 
 *   status -- the error code to check
 *   hwidb  -- pointer to the hardware IDB for the interface
 *   cmd    -- the CCB command that returned the status
 *   arg    -- argument that was issued with cmd 
 *
 * Return Value: 
 *   void
 *
 *****************************************************************************/
void status_msg_1arg (ushort status, hwidbtype *hwidb, ushort cmd, ulong arg)
{
    if (status)
	errmsg(&msgsym(CCBCMDFAIL1, CBUS), 
	       hwidb->slot, cmd, arg, status);
}


/*****************************************************************************
 * Name: select_if
 *
 * Description: Selects an interface for subsequent commands.
 *              This is one of the commands where we need to mimic
 *              the smarts done by the SP.  In particular, we need to:
 *                . use a CCB command rather than a FCI/MCI command
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_SELECT, hwidb->slotunit);
    status = get_if_status(hwidb);

    return(status);
}


/*****************************************************************************
 * Name: send_errors_cmd
 *
 * Description: Send a CCB_CMD_ERRORS command to the interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   arg0 -- argument for the errors command 
 *           (see CMD_ERRORS_xxx in if_cmd.h for possible values)
 *
 * Return Value: 
 *   error_count -- the value of the error counter indicated, or 0 if there
 *     was an error.
 *
 *****************************************************************************/
long send_errors_cmd (hwidbtype *hwidb, ushort error_number)
{
    ushort status;
    ushort error_count;

    send_if_cmd_1arg(hwidb,CCB_CMD_ERRORS,error_number);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	error_count = get_if_val(hwidb);
    } else {
	error_count = 0;
    }
    return(error_count);
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
 *     CMD_RSP_OKAY.  Interpretation of this value is hardware
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_APPLIQUE, app_cmd);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*applique_ptr = get_if_val(hwidb);
    }
    return(status);
}


/*****************************************************************************
 * Name: send_rx_offset_cmd
 *
 * Description: Sends a CCB_CMD_RX_OFFSET command to the interface 
 * indicated.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   rx_offset -- the required rx_offset for the interface
 *                N.B.  Unlike previous platforms, this parameter
 *                is specified in BYTES (not words).
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_rx_offset_cmd (hwidbtype *hwidb, ushort rx_offset)
{
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_RX_OFFSET, rx_offset);
    status = get_if_status(hwidb);
    return(status);
}


/*****************************************************************************
 * Name: send_fip_hold_reg_cmd
 *
 * Description: Sends a CCB_CMD_FIP_HOLD_REG command to the interface indicated
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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_FIP_HOLD_REG,hold_reg);
    status = get_if_status(hwidb);
    return(status);
}


/*****************************************************************************
 * Name: send_fip_burst_mode_cmd
 *
 * Description: Sends a CCB_CMD_FIP_BURST_MODE command to the interface
 * indicated
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_BURST_MODE, burst_mode);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_fip_phy_a_lem_cmd
 *
 * Description: Sends a CCB_CMD_FIP_PHY_A_LEM command to the interface indicated
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_PHY_A_LEM, lem_cmd);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_fip_phy_b_lem_cmd
 *
 * Description: Sends a CCB_CMD_FIP_PHY_B_LEM command to the interface indicated
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_PHY_B_LEM, lem_cmd);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_fddi_cam_rd_arg_cmd
 *
 * Description: Send a CCB_CMD_FIP_CAM_RD_ARG command to the
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_FIP_CAM_RD_ARG);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*read_value = get_if_val(hwidb);
    }

    return(status);
}

/*****************************************************************************
 * Name: send_fip_cam_wr_arg_cmd
 *
 * Description: Sends a CCB_CMD_FIP_CAM_WR_ARG command to the interface
 * indicated
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
    ushort status;
    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_CAM_WR_ARG, write_value);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_fip_cam_cmd
 *
 * Description: Sends a CCB_CMD_FIP_CAM_CMD command to the interface indicated
 *
 * Note:
 *   This command is only valid for FDDI interfaces
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   cam_op -- the CAM operation code to send with the CCB_CMD_FIP_CAM_CMD
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fddi_cam_cmd (hwidbtype *hwidb, ushort cam_op)
{
    ushort status;
    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_CAM_CMD, cam_op);
    status = get_if_status(hwidb);
    return(status);
}
/*****************************************************************************
 * Name: send_fip_cam_cmd_3arg
 *
 * Description: Sends a CCB_CMD_VIP_CAM_POPULATE or
 *              CCB_CMD_VIP_CAM_DEPOPULATE command to the interface.
 *
 * Note:
 *   This command is only valid for FDDI interfaces
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface
 *   cam_op -- the CAM operation code to send (POPULATE or DEPOPULATE)
 *   arg0, arg1, arg2 : ushort values with the MAC addr to (de)populate.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_fddi_cam_cmd_3arg (hwidbtype *hwidb, ushort cam_op,
                              ushort arg0,
                              ushort arg1,
                              ushort arg2)
{
    ushort status;
    select_if(hwidb);
    send_if_cmd_3arg(hwidb, cam_op, arg0, arg1, arg2);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_fip_get_state_cmd
 *
 * Description: Send a CCB_CMD_FIP_GET_STATE command to the
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_GET_STATE, which_phy);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*state = get_if_val(hwidb);
    }

    return(status);
}

/*****************************************************************************
 * Name: send_cbus_buffer_park_cmd
 *
 * Description: Sends a FCI_CBUS_BUFFER_PARK command to the interface indicated
 *
 *		We need to figure out what this routine will do on the RSP!
 *		%%% XXX
 *
 *              The CCB_CMD_BUFFER_PARK command parks buffers on an interface.
 *		Parking buffers on an interface is called stoking the
 *		interface.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   slots -- number of buffers to park (i.e. reserve)
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_cbus_buffer_park_cmd (hwidbtype *hwidb, ushort slots)
{
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_BUFFER_PARK, slots);
    status = get_if_status(hwidb);

    return(status);
}


/*****************************************************************************
 * Name: send_rxbytes_cmd
 *
 * Description: Sends a CCB_CMD_RXBYTES command to the interface indicated
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
 *****************************************************************************/
ushort send_rxbytes_cmd (hwidbtype *hwidb, ushort max_rx_bytes)
{
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_RXBYTES, max_rx_bytes);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_status_cmd
 *
 * Description: Send a CCB_CMD_STATUS command to the interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   arg0 -- argument for the status command
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_status_cmd (hwidbtype *hwidb, ushort arg0)
{
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_STATUS, arg0);
    status = get_if_status(hwidb);
    return(status);
}


/*****************************************************************************
 * Name: send_if_enable_cmd
 *
 * Description: Send a CCB_CMD_STATUS command with arg CCB_STATUS_IF_ENABLE
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

    status = send_status_cmd(hwidb,CCB_STATUS_IF_ENABLE);
    return(status);
}


/*****************************************************************************
 * Name: send_if_disable_cmd
 *
 * Description: Send a CCB_CMD_STATUS command with arg CCB_STATUS_IF_DISABLE
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

    status = send_status_cmd(hwidb,CCB_STATUS_IF_DISABLE);
    return(status);
}


/*****************************************************************************
 * Name: send_if_loopback_cmd
 *
 * Description: Send a CCB_CMD_STATUS command with arg CCB_STATUS_IF_LOOPLOCAL
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

    status = send_status_cmd(hwidb,CCB_STATUS_IF_LOOPLOCAL);
    return(status);
}


/*****************************************************************************
 * Name: send_if_noloopback_cmd
 *
 * Description: Send a CCB_CMD_STATUS command with arg CCB_STATUS_IF_NOLOOP
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

    status = send_status_cmd(hwidb,CCB_STATUS_IF_NOLOOP);
    return(status);
}


/*****************************************************************************
 * Name: send_ctrlr_reset_cmd
 *
 * Description: Send a FCI_CMD_CTRLR_RESET to an MCI controller.
 *              Since we don't have MCI controllers on the RSP, this
 *              routine is just a stub.
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
    return(CMD_RSP_OKAY);
}


/*****************************************************************************
 * Name: send_ctrlr_load_cmd
 *
 * Description: Send a FCI_CMD_CTRLR_LOAD to an MCI controller.
 *              Since we don't have MCI controllers on the RSP, this
 *              routine is just a stub.
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
    return(CMD_RSP_OKAY);
}


/*****************************************************************************
 * Name: send_clockrate_cmd
 *
 * Description: 
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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_CLOCKRATE,cookie);
    status = get_if_status(hwidb);

    return(status);
}


/*****************************************************************************
 * Name: send_clocktype_cmd
 *
 * Description: 
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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_CLOCKTYPE,prescalar);
    status = get_if_status(hwidb);

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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_ALT_IDLE,enable);
    status = get_if_status(hwidb);

    return(status);
}


/*****************************************************************************
 * Name: send_delay_cmd
 *
 * Description: 
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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_DELAY,delay);
    status = get_if_status(hwidb);

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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_HIP_TA,ta);
    status = get_if_status(hwidb);

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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_HIP_ICLK,iclk);
    status = get_if_status(hwidb);

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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_HIP_LC_INHIBIT,lc);
    status = get_if_status(hwidb);

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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_HIP_LOOPBACK,loopback);
    status = get_if_status(hwidb);

    return(status);
}


/*****************************************************************************
 * Name: send_carrier_cmd
 *
 * Description: Send a CCB_CMD_CARRIER command and gets the response
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
 *****************************************************************************/
ushort send_carrier_cmd (hwidbtype *hwidb, ushort *carrier_state_ptr)
{
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_CARRIER);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*carrier_state_ptr = get_if_val(hwidb);
    }
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
ushort send_lights_cmd (hwidbtype *idb, ushort lights)
{
    return(CMD_RSP_OKAY);
}


/*****************************************************************************
 * Name: send_station_cmd
 *
 * Description: Send a CCB_CMD_STATION command and get a response value
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   byte_number -- which byte of the station address (ethernet address) to
 *     get from the IP.
 *   addr_ptr -- a pointer to the byte inwhich to put the address
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.
 *
 *****************************************************************************/
ushort send_station_cmd (hwidbtype *hwidb, ushort byte_number, char *addr_ptr)
{
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_STATION,byte_number);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*addr_ptr = (char)get_if_val(hwidb); 
    }
    return(status);
}


/*****************************************************************************
 * Name: send_cbus_mema_select_cmd
 *
 * Description: Send a CCB_CMD_MEMA_SELECT
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
ushort send_cbus_mema_select_cmd (hwidbtype *hwidb, ushort mema_addr)
{
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_MEMA_SELECT,mema_addr);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_mci_mema_select_cmd
 *
 * Description: Send a CCB_CMD_MEMA_SELECT
 *
 * Assumptions:
 *   1. this should never happen on the rsp.
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
    return(TRUE);
}

/*****************************************************************************
 * Name: send_cbus_mema_write_cmd
 *
 * Description: Send a CCB_CMD_MEMA_SELECT
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
ushort send_cbus_mema_write_cmd (hwidbtype *hwidb, ushort mema_value)
{
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_MEMA_WRITE,mema_value);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_mci_mema_write_cmd
 *
 * Description: Send a CCB_CMD_MEMA_SELECT
 *
 * Assumptions:
 *   1. this should never happen on the rsp
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
    return(TRUE);
}

/*****************************************************************************
 * Name: send_af_select_cmd
 *
 * Description: Send a CCB_CMD_AF_SELECT
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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_AF_SELECT,af_addr);
    status = get_if_status(hwidb);
    return(status);
}


/*****************************************************************************
 * Name: send_af_write_cmd
 *
 * Description: Send a CCB_CMD_AF_WRITE
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
    int retry;
    ushort status;

    for (retry=0;;retry++) {
	send_if_cmd_1arg(hwidb,CCB_CMD_AF_WRITE,af_value);
	status = get_if_status(hwidb);

	if ((status != CMD_RSP_ERR_CMDQFL) || (retry >= 3))
	    break;

	/*
	 * CSCdi83940 - VIP CCB command queue is full.  Some filters
	 * take up to 4.4ms to install.  Wait a while and try again.
	 */
	usecdelay(4400);
    }

    status_msg_1arg(status, hwidb, CCB_CMD_AF_WRITE, af_value);
    return(status);
}


/*****************************************************************************
 * Name: send_hip_read_loop_cmd
 *
 * Description: Send a CCB_CMD_HIP_READ_LOOP command to the
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_HIP_READ_LOOP);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*mode = get_if_val(hwidb);
    }

    return(status);
}

/*****************************************************************************
 * Name: send_serial_crc_32_cmd
 *
 * Description: Send a CCB_CMD_SERIAL_CRC_32 command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_SERIAL_CRC_32, enabled);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fsip_crc4_cmd
 *
 * Description: Send a CCB_CMD_FSIP_CRC4 command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean, TRUE indicates crc4 should be enabled,
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FSIP_CRC4, enabled);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fsip_ts16_cmd
 *
 * Description: Send a CCB_CMD_FSIP_TS16 command to the
 *              interface indicated
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean, TRUE indicates ts16 should be enabled,
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FSIP_TS16, enabled);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fsip_clocksource_cmd
 *
 * Description: Send a CCB_CMD_FSIP_CLOCKSOURCE command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FSIP_CLOCKSOURCE, enabled);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fsip_start_slot_g703_cmd
 *
 * Description: Send a CCB_CMD_FSIP_START_SLOT_G703 command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FSIP_START_SLOT_G703, slot);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fsip_stop_slot_g703_cmd
 *
 * Description: Send a CCB_CMD_FSIP_STOP_SLOT_G703 command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FSIP_STOP_SLOT_G703, slot);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fsip_signal_sts_cmd
 *
 * Description: Send a CCB_CMD_FSIP_SIGNAL_STS command to the
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_FSIP_SIGNAL_STS);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*signal_status = get_if_val(hwidb);
    }

    return(status);
}

/*****************************************************************************
 * Name: send_fsip_nrzi_cmd
 *
 * Description: Send a CCB_CMD_FSIP_NRZI command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FSIP_NRZI, enabled);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fsip_tx_invert_clk_cmd
 *
 * Description: Send a CCB_CMD_FSIP_TX_INVERT_CLK command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FSIP_TX_INVERT_CLK, enabled);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fsip_halfduplex_cmd
 *
 * Description: Send a CCB_CMD_FSIP_HALFDUPLEX command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FSIP_HALFDUPLEX, enabled);
    return(get_if_status(hwidb));
}


/*****************************************************************************
 * Name: send_fsip_ignore_dcd_cmd
 *
 * Description: Send a CCB_CMD_FSIP_IGNORE_DCD command to the
 *              interface indicated
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface
 *   enabled -- boolean, TRUE indicates ignore_dcd should be enabled,
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FSIP_IGNORE_DCD, enabled);
    return(get_if_status(hwidb));
}

/*****************************************************************
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
 ********************************************************************/
ushort send_fsip_max_mtu_supported_cmd (hwidbtype *hwidb, 
                                      ushort *max_mtu_supported)
{
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_MAX_MTU_SUPPORTED);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
      *max_mtu_supported = get_if_val(hwidb);
    }
    
    return(status);
}
    
/*****************************************************************************
 * Name: send_aip_sonet_cmd
 *
 * Description: issues an CCB_CMD_AIP_SONET command to the AIP indicated.
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
    ushort status;

    send_if_cmd_larg(hwidb,CCB_CMD_AIP_SONET,sonetmode);
    status = get_if_status(hwidb);
    return(status);
}


/*****************************************************************************
 * Name: send_aip_framing_cmd
 *
 * Description: issues an CCB_CMD_AIP_FRAMING command to the AIP indicated.
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
    ushort status;

    send_if_cmd_larg(hwidb,CCB_CMD_AIP_FRAMING,framing);
    status = get_if_status(hwidb);
    return(status);
}


/*****************************************************************************
 * Name: send_aip_addr_select_cmd
 *
 * Description: issues an CCB_CMD_AIP_ADDR_SELECT command to the AIP indicated.
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
    ushort status;

    send_if_cmd_larg(hwidb,CCB_CMD_AIP_ADDR_SELECT,addr);
    status = get_if_status(hwidb);
    return(status);
}


/*****************************************************************************
 * Name: send_aip_mem_read_cmd
 *
 * Description: issues an CCB_CMD_AIP_MEM_READ command to the AIP indicated.
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
    ushort status;

    send_if_cmd(hwidb,CCB_CMD_AIP_MEM_READ);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*data_ptr = get_if_val(hwidb);
    }
    return(status);
}


/*****************************************************************************
 * Name: send_aip_mem_write_cmd
 *
 * Description: issues an CCB_CMD_AIP_MEM_WRITE command to the AIP indicated.
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
    ushort status;

    send_if_cmd_1arg(hwidb,CCB_CMD_AIP_MEM_WRITE,data);
    status = get_if_status(hwidb);
    return(status);
}


/*****************************************************************************
 * Name: send_aip_cmdblock_cmd
 *
 * Description: issues an CCB_CMD_AIP_CMDBLOCK command to the AIP indicated.
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
    ushort status;

    send_if_cmd(hwidb,CCB_CMD_AIP_CMDBLOCK);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*cmdblock_ptr = get_if_lval(hwidb);
    }
    return(status);
}


/*****************************************************************************
 * Name: send_aip_cmdparse_cmd
 *
 * Description: issues an CCB_CMD_AIP_CMDPARSE command to the AIP indicated.
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
    ushort status;

    send_if_cmd(hwidb,CCB_CMD_AIP_CMDPARSE);
    status = get_if_status(hwidb);

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
    ushort status;

    send_if_cmd_larg(hwidb,CCB_CMD_AIP_DS3SCRAMBLE,enable);
    status = get_if_status(hwidb);

    return(status);
}

/*****************************************************************************
 * Name: send_bridge_cmd
 *
 * Description: Send a CCB_CMD_BRIDGE command to the i'face
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
    return(CMD_RSP_OKAY);
}

/*****************************************************************************
 * Name: send_af_read_cmd
 *
 * Description: Send a CCB_CMD_AF_READ command to the 
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_AF_READ);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*data = get_if_val(hwidb);
    }

    return(status);
}

/*****************************************************************************
 * Name: send_if_disable_cmd_delay
 *
 * Description: Send a CCB_CMD_STATUS command with arg CCB_STATUS_IF_DISABLE,
 *              Identical to send_if_disable_cmd() except that it
 *              delays the specified time before checking for a
 *              result (with a cpu loop).
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   ms -- the delay in ms.  This routine will wait this long
 *         before checking the result code.
 *
 * Output:
 *   hwidb->hw_enabled gets set FALSE.  Drivers can use this field to
 *                     determine whether or not the system thinks the
 *                     interface is enabled.
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_if_disable_cmd_delay (hwidbtype *hwidb, ulong ms)
{
    ushort status;

    hwidb->hw_enabled = FALSE;

    status = send_status_cmd_delay(hwidb,CCB_STATUS_IF_DISABLE,ms);
    return(status);
}

/*****************************************************************************
 * Name: send_if_enable_cmd_delay
 *
 * Description: Send a CCB_CMD_STATUS command with arg CCB_STATUS_IF_ENABLE,
 *              Identical to send_if_enable_cmd() except that it
 *              delays the specified time before checking for a
 *              result (with a cpu loop).
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   ms -- the delay in ms.  This routine will wait this long
 *         before checking the result code.
 *
 * Output:
 *   hwidb->hw_enabled gets set TRUE.  Drivers can use this field to
 *                     determine whether or not the system thinks the
 *                     interface is enabled.
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_if_enable_cmd_delay (hwidbtype *hwidb, ulong ms)
{
    ushort status;

    hwidb->hw_enabled = TRUE;

    status = send_status_cmd_delay(hwidb,CCB_STATUS_IF_ENABLE,ms);
    return(status);
}

/*****************************************************************************
 * Name: send_status_cmd_delay
 *
 * Description: Send a CCB_CMD_STATUS command to the interface
 *              indicated with a delay.  Identical to send_status_cmd
 *              except it waits a specified time before returning.
 *              This allows the interface time to complete the
 *              command before other commands can be sent to it.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   arg0 -- argument for the status command
 *   ms -- the delay to wait before returning
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
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
 * Description: Send a CCB_CMD_FIP_EN_READ command to the
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_FIP_EN_READ);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*data = get_if_val(hwidb);
    }
    return(status);
}

/*****************************************************************************
 * Name: send_fip_en_write_cmd
 *
 * Description: Send a FIP_CMD_EN_WRITE command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_EN_WRITE, data);
    return(get_if_status(hwidb));
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
 * get passed on to the FIP, and hence is a stub on the RSP.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   none (we daren't even read the cmdreg, lest it cause problems)
 *
 *****************************************************************************/
void send_fip_hack_cmd (hwidbtype *hwidb)
{
}

/*****************************************************************************
 * Name: send_fip_fm_select_cmd
 *
 * Description: Send a CCB_CMD_FIP_FM_SELECT command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_FM_SELECT, data);
    return(get_if_status(hwidb));
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
    return (CMD_RSP_OKAY);
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
    return (CMD_RSP_OKAY);
}


/*****************************************************************************
 * Name: send_fip_en_select_cmd
 *
 * Description: Send a CCB_CMD_FIP_EN_SELECT command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_EN_SELECT, reg);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fip_fm_write_cmd
 *
 * Description: Send a CCB_CMD_FIP_FM_WRITE command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_FM_WRITE, data);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_reg_select_cmd
 *
 * Description: Send a CCB_CMD_REG_SELECT command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_REG_SELECT, reg);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_reg_read_cmd
 *
 * Description: Send a CCB_CMD_REG_READ command to the 
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_REG_READ);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*data = get_if_val(hwidb);
    }
    return(status);
}

/*****************************************************************************
 * Name: send_reg_write_cmd
 *
 * Description: Send a CCB_CMD_REG_WRITE command to the 
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_REG_WRITE, data);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_fip_applique_select_cmd
 *
 * Description: Send a CCB_CMD_FIP_APPLIQUE_SELECT command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_APPLIQUE_SELECT, data);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fip_applique_read_cmd
 *
 * Description: Send a CCB_CMD_FIP_APPLIQUE_READ command to the 
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_FIP_APPLIQUE_READ);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*data = get_if_val(hwidb);
    }

    return(status);
}

/*****************************************************************************
 * Name: send_fip_applique_write_cmd
 *
 * Description: Send a CCB_CMD_FIP_APPLIQUE_WRITE command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_APPLIQUE_WRITE, data);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fip_get_rval_cmd
 *
 * Description: Send an CCB_CMD_FIP_GET_RVAL command to the 
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_GET_RVAL, index);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*data = get_if_val(hwidb);
    }

    return(status);
}

/*****************************************************************************
 * Name: send_fip_get_py_ls_cmd
 *
 * Description: Send a CCB_CMD_FIP_GET_PY_LS command to the 
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_GET_PY_LS, index);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*data = get_if_val(hwidb);
    }

    return(status);
}

/*****************************************************************************
 * Name: send_fip_phy_a_int_mask_cmd
 *
 * Description: Send a CCB_CMD_FIP_PHY_A_INT_MASK command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_PHY_A_INT_MASK, data);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fip_phy_b_int_mask_cmd
 *
 * Description: Send a CCB_CMD_FIP_PHY_B_INT_MASK command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_FIP_PHY_B_INT_MASK, data);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_fip_fm_read_cmd
 *
 * Description: Send a CCB_CMD_FIP_FM_READ command to the 
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_FIP_FM_READ);

    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*data = get_if_val(hwidb);
    }

    return(status);
}

/*****************************************************************************
 * Name: send_trip_af_or_cmd
 *
 * Description: Send a CCB_CMD_TRIP_AF_OR command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_AF_OR, data);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_global_ptrs_cmd
 *
 * Description: Send a CCB_CMD_GLOBAL_PTRS command to the
 *              IP indicated.
 *              
 *
 * Input: 
 *   slotp -- pointer to the slot indicating the IP to be sent the command
 *   event_q_ptr -- the 16-bit offset into memd indicating which
 *                  hardware queue we wish this IP to use for event posting
 *   love_note_free_q_ptr -- 
 *                  the 16-bit offset into memd indicating which
 *                  hardware queue we wish this IP to use for fetching
 *                  free buffers for love note generation
 *   love_letter_free_q_ptr --
 *                  the 16-bit offset into memd indicating which
 *                  hardware queue we wish this IP to use for fetching
 *                  free buffers for love letter generation
 *
 * Output:
 *   max_love_letter_size --
 *                  the IP returns the size of its maximum love
 *                  letter.  The RSP can use this to sanity check
 *                  compatibility between the IP and the RSP's love
 *                  letter buffer pool.
 *                  This field is only written to if the return status
 *                  is CMD_RSP_OKAY.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_global_ptrs_cmd (slottype *slotp, 
			     ushort event_q_ptr,
			     ushort love_note_free_q_ptr,
			     ushort love_letter_free_q_ptr,
			     ushort *max_love_letter_size)
{
    ushort status;

    send_ip_cmd_3arg(slotp, CCB_CMD_GLOBAL_PTRS,
		     event_q_ptr, love_note_free_q_ptr,
		     love_letter_free_q_ptr);
    status = get_ip_status(slotp);
    if (status == CMD_RSP_OKAY) {
	*max_love_letter_size = get_ip_val(slotp);
    }
    return (status);
}

/*****************************************************************************
 * Name: send_version_cmd
 *
 * Description: Send a CCB_CMD_VERSION command to the IP indicated.
 *
 * Input: 
 *   slotp -- pointer to the slot indicating the IP to be sent the command
 *   subcmd -- version subcommand, CCB_VERSION_SW or CCB_VERSION_HW
 *
 * Output:
 *   16bit version, major version in high byte, minor version in low byte
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_version_cmd (slottype *slotp, ushort subcmd, ushort* version)
{
    ushort status;

    send_ip_cmd_1arg(slotp, CCB_CMD_VERSION, subcmd);
    status = get_ip_status(slotp);
    if (status == CMD_RSP_OKAY) {
	*version = get_ip_val(slotp);
    }
    return (status);
}

/*****************************************************************************
 * Name: send_raw_queue_ptr_cmd
 *
 * Description: Send a CCB_CMD_RAW_QUEUE_PTR command to the
 *              IP indicated.
 *              
 *
 * Input: 
 *   idb -- pointer to the hwidb indicating the i'face to be sent the command
 *   raw_q_ptr -- the 16-bit offset into memd indicating which
 *                hardware queue we wish this i'face to use for the
 *                raw queue, i.e. where it should put incoming data
 *
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_raw_queue_ptr_cmd (hwidbtype *idb, ushort raw_q_ptr)
{
    send_if_cmd_1arg(idb, CCB_CMD_RAW_QUEUE_PTR, raw_q_ptr);
    return (get_if_status(idb));
}

/*****************************************************************************
 * Name: send_free_queue_ptrs_cmd
 *
 * Description: Send a CCB_CMD_FREE_QUEUE_PTRS command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   idb -- pointer to the hwidb indicating the i'face to be sent the command
 *   local_free_q_ptr -- 
 *                  the 16-bit offset into memd indicating which
 *                  hardware queue we wish this i'face to use for its
 *                  local stash of free data buffers
 *   global_free_q_ptr -- 
 *                  the 16-bit offset into memd indicating which
 *                  hardware queue we wish this i'face to use when it
 *                  needs to allocate more global buffers
 *   global_free_q_size --
 *                  the size, in bytes, of the buffers in the above
 *                  global free queue.
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_free_queue_ptrs_cmd (hwidbtype *idb, 
				 ushort local_free_q_ptr,
				 ushort global_free_q_ptr,
				 ushort global_free_q_size)
{
    send_if_cmd_3arg(idb, CCB_CMD_FREE_QUEUE_PTRS,
		     local_free_q_ptr, global_free_q_ptr,
		     global_free_q_size);
    return (get_if_status(idb));
}

/*****************************************************************************
 * Name: send_rx_limits_cmd
 *
 * Description: Send a CCB_CMD_RX_LIMITS command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   idb -- pointer to the hwidb indicating the i'face to be sent the command
 *   rx_limit_lo -- informs the i'face the minimum number of rx buffers
 *                  it should hang on to during idler moments.
 *                  An idle interface will return buffers
 *                  from its local_free_q to the global_free_q
 *                  until it has rx_limit_lo buffers outstanding
 *   rx_limit_hi -- informs the i'face the maximum number of rx
 *                  buffers it may have outstanding at any time.
 *                  Once this value is reached, the i'face may not
 *                  take any more buffers from the global_free_q.
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_rx_limits_cmd (hwidbtype *idb, 
			   ushort rx_limit_lo,
			   ushort rx_limit_hi)
{
    send_if_cmd_2arg(idb, CCB_CMD_RX_LIMITS, rx_limit_lo, rx_limit_hi);
    return (get_if_status(idb));
}

/*****************************************************************************
 * Name: send_tx_ptrs_cmd
 *
 * Description: Send a CCB_CMD_TX_PTRS command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   idb -- pointer to the hwidb indicating the i'face to be sent the command
 *   tx_q_ptr --    the 16-bit offset into memd indicating which
 *                  hardware queue we wish this i'face to feed from
 *                  for its outgoing data packets.
 *   tx_acc_ptr --  the 16-bit offset into memd indicating which
 *                  hardware accumulator we wish this i'face to increment
 *                  when it transmits a packet.
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_tx_ptrs_cmd (hwidbtype *idb, 
			 ushort tx_q_ptr,
			 ushort tx_acc_ptr)
{
    send_if_cmd_2arg(idb, CCB_CMD_TX_PTRS, tx_q_ptr, tx_acc_ptr);
    return (get_if_status(idb));
}

/*****************************************************************************
 * Name: send_return_buffer_cmd
 *
 * Description: Send a CCB_CMD_RETURN_BUFFER command to the
 *              IP indicated.
 *              
 *
 * Input: 
 *   idb -- pointer to the hwidb indicating the i'face to be sent the command
 *
 * Output:
 *   rx_count --    the current value of the i'faces rx count.
 *                  This field is only written to if the return status
 *                  is CMD_RSP_OKAY.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_return_buffer_cmd (hwidbtype *idb, ushort *rx_count)
{
    ushort status;

    send_if_cmd(idb, CCB_CMD_RETURN_BUFFER);
    status = get_if_status(idb);
    if (status == CMD_RSP_OKAY) {
	*rx_count = get_if_val(idb);
    }
    return (status);
}

/*****************************************************************************
 * Name: send_trip_spyg_mem_select_cmd
 *
 * Description: Send a CCB_CMD_TRIP_SPYGLASS_SELECT command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_SPYGLASS_SELECT, addr);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_spyg_mem_read_cmd
 *
 * Description: Send a CCB_CMD_TRIP_SPYGLASS_READ command to the
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
    send_if_cmd(hwidb, CCB_CMD_TRIP_SPYGLASS_READ);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_spyg_mem_read_rslt_cmd
 *
 * Description: Send a CCB_CMD_TRIP_SPYGLASS_READ_VAL command to the
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_TRIP_SPYGLASS_READ_VAL);
    status = get_if_status(hwidb);

    if (status == CMD_RSP_OKAY) {
	*data = get_if_val(hwidb);
    }
    return(status);
}

/*****************************************************************************
 * Name: send_trip_spyg_mem_write_cmd
 *
 * Description: Send a CCB_CMD_TRIP_SPYGLASS_WRITE command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_SPYGLASS_WRITE, data);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_memb_select_cmd
 *
 * Description: Send a CCB_CMD_TRIP_MEMB_SELECT command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_MEMB_SELECT, addr);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_memb_cont_write_cmd
 *
 * Description: Send a CCB_CMD_TRIP_MEMB_CONT_WRITE command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_MEMB_CONT_WRITE, value);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_memb_cont_read_cmd
 *
 * Description: Send an CCB_CMD_TRIP_MEMB_CONT_READ command to the
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
ushort send_trip_memb_cont_read_cmd (hwidbtype *idb, ushort *value)
{
    ushort status;

    send_if_cmd(idb, CCB_CMD_TRIP_MEMB_CONT_READ);
    status = get_if_status(idb);
    if (status == CMD_RSP_OKAY) {
	*value = get_if_val(idb);
    }
    return (status);
}

/*****************************************************************************
 * Name: send_trip_set_mac_hi_cmd
 *
 * Description: Send a CCB_CMD_TRIP_SET_MAC_HI command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_SET_MAC_HI, value);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_set_mac_mid_cmd
 *
 * Description: Send a CCB_CMD_TRIP_SET_MAC_MID command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_SET_MAC_MID, value);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_set_mac_high_lo_cmd
 *
 * Description: Send a CCB_CMD_TRIP_SET_MAC_LO command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_SET_MAC_LO, value);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_select_cmd
 *
 * Description: Send an CCB_CMD_SELECT command to the
 *              IP indicated.  Note that this results in the
 *              same command being sent to the IP as the select_if()
 *              wrapper.  However, the two wrappers provide different
 *              functionality.   Use
 *
 *              select_if(idb) when you want to select an interface
 *                             prior to issuing further commands.
 *
 *              send_select_cmd(slotp) when you want to probe the
 *                                     IP to determine what sort
 *                                     of interfaces it has.
 *              
 *
 * Input: 
 *   slotp -- pointer to the slot indicating the IP to be sent the
 *            command
 *   iface_num -- the interface number who's type we want to determine
 *
 * Output:
 *   if_type --  returns the interface type (see FCI_TYPE_xxx
 *               in if_fci.h).
 *               N.B. This field is only written to if the return status
 *               is CMD_RSP_OKAY.
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_select_cmd (slottype *slotp, ushort iface_num, ushort *if_type)
{
    ushort status;

    send_ip_cmd_1arg(slotp, CCB_CMD_SELECT, iface_num);

    status = get_ip_status(slotp);

    /*
     * This code may need to be revisited depending on whether or not
     * the ucoders decide it's an "error" to select an illegal
     * interface.  This is currently under discussion by the
     * the ucoders, but the current feeling is that it won't
     * be an error, i.e. we will get CMD_RSP_OKAY back as a status.
     */
 
    if (status == CMD_RSP_OKAY) {
	*if_type = get_ip_val(slotp);
    }

    return (status);
}

/*****************************************************************************
 * Name: send_trip_poke_cuetrc_cmd
 *
 * Description: Send a CCB_CMD_TRIP_POKE_CUETRC command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_POKE_CUETRC, value);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_reset_ph_bit_cmd
 *
 * Description: Send a CCB_CMD_TRIP_RESET_PH_BIT command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_RESET_PH_BIT, value);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_set_ph_bit_cmd
 *
 * Description: Send a CCB_CMD_TRIP_SET_PH_BIT command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_SET_PH_BIT, value);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_read_ring_stat_cmd
 *
 * Description: Send an CCB_CMD_TRIP_READ_RING_STAT command to the
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
ushort send_trip_read_ring_stat_cmd (hwidbtype *idb, ushort *ring_status)
{
    ushort status;

    send_if_cmd(idb, CCB_CMD_TRIP_READ_RING_STAT);
    status = get_if_status(idb);
    if (status == CMD_RSP_OKAY) {
	*ring_status = get_if_val(idb);
    }
    return (status);
}

/*****************************************************************************
 * Name: send_trip_get_check_ptr_cmd
 *
 * Description: Send an CCB_CMD_TRIP_GET_CHECK_PTR command to the
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
ushort send_trip_get_check_ptr_cmd (hwidbtype *idb, ushort *check_ptr)
{
    ushort status;

    send_if_cmd(idb, CCB_CMD_TRIP_GET_CHECK_PTR);
    status = get_if_status(idb);
    if (status == CMD_RSP_OKAY) {
	*check_ptr = get_if_val(idb);
    }
    return (status);
}

/*****************************************************************************
 * Name: send_trip_accept_mac_cmd
 *
 * Description: Send a CCB_CMD_TRIP_ACCEPT_MAC command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_ACCEPT_MAC, value);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_af_size_cmd
 *
 * Description: Send an CCB_CMD_AF_SIZE command to the
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
ushort send_af_size_cmd (hwidbtype *idb, ushort *size)
{
    ushort status;

    send_if_cmd(idb, CCB_CMD_AF_SIZE);
    status = get_if_status(idb);
    if (status == CMD_RSP_OKAY) {
	*size = get_if_val(idb);
    }
    return (status);
}

/*****************************************************************************
 * Name: send_trip_init_cmd
 *
 * Description: Send a CCB_CMD_TRIP_INIT command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_TRIP_INIT, value);
    return(get_if_status(hwidb));
}

/*****************************************************************************
 * Name: send_trip_init_complete_cmd
 *
 * Description: Send a CCB_CMD_TRIP_SPYGLASS_RESULT command to the
 *              interface indicated (the drivers know this as
 *              init-complete since it's a response from the spyglass
 *              the completes the initalisation.
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_TRIP_SPYGLASS_RESULT);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*error = get_if_val(hwidb);
    }
    return (status);
}


/*****************************************************************************
 * Name: send_txqlength_cmd
 *
 * Description: Send a CCB_CMD_TXQLENGTH command to the
 *              interface indicated.
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_TXQLENGTH);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) {
	*count = get_if_val(hwidb);
    }
    return (status);
}


/*****************************************************************************
 * Name: send_short_txq_localbuf_cmd
 *
 * Description: Send a CCB_CMD_SHORT_TXQ_LOCALBUF command to the
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
    send_if_cmd_1arg(hwidb, CCB_CMD_SHORT_TXQ_LOCALBUF, 
		     enabled ? TRUE : FALSE);
    return(get_if_status(hwidb));
}


/*****************************************************************************
 * Name: send_daughter_card_type_cmd
 *
 * Description: Send a FCI_CMD_CIP_DAUGHTER_TYPE command to the
 *              interface indicated.
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   type -- the daugher card type
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_CIP_DAUGHTER_TYPE);
    status = get_if_status(hwidb);

    if (status == CMD_RSP_OKAY) 
	*type = get_if_val(hwidb);

    status_msg(status, hwidb, CCB_CMD_CIP_DAUGHTER_TYPE);
    return(status);
}

/*****************************************************************************
 * Name: get_cip_cfg_block_cmd
 *
 * Description: get the config block from the cip 
 *              
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   mem_addr -- pointer to config block
 *   error -- the error code returned from the interface
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_CIP_CFGBLOCK);
    status    = get_if_status(hwidb);

    if (status == CMD_RSP_OKAY) 
	*mem_addr = get_if_lval(hwidb);

    status_msg(status, hwidb, CCB_CMD_CIP_CFGBLOCK);
    return(status);
}

/*****************************************************************************
 * Name: send_cip_parse_blk_cmd
 *
 * Description: Send a CCB_CMD_CIP_CFGPARSE command to the
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_CIP_CFGPARSE );
    status    = get_if_status(hwidb);

    status_msg(status, hwidb, CCB_CMD_CIP_CFGPARSE);
    return(status);
}

/*****************************************************************************
 * Name: send_cip_data_rate_cmd
 *
 * Description: Send a CCB_CMD_CIP_SET_PCA_BW command to the
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
    ushort status;
    
    send_if_cmd_1arg(hwidb, CCB_CMD_CIP_SET_PCA_BW, value);
    status    = get_if_status(hwidb);

    status_msg_1arg(status, hwidb, CCB_CMD_CIP_SET_PCA_BW,
		    value);
    return(status);
}

/*****************************************************************************
 * Name: send_cip_stats_int_cmd
 *
 * Description: Send a CCB_CMD_CIP_STATS_INTERVAL command to the 
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_CIP_STATS_INTERVAL, value);
    status    = get_if_status(hwidb);

    status_msg_1arg(status, hwidb, CCB_CMD_CIP_STATS_INTERVAL,
		    value);
    return(status);
}

/*****************************************************************************
 * Name: send_cbus_cip_mema_write_cmd
 *
 * Description: Send a CCB_CMD_CIP_MEM_WRITE controller command
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
ushort send_cbus_cip_mema_write_cmd (hwidbtype *hwidb, ushort value)
{
    ushort status;
    
    send_if_cmd_1arg(hwidb, CCB_CMD_CIP_MEM_WRITE, value);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_cbus_cip_mema_select_cmd
 *
 * Description: Send a CCB_CMD_CIP_ADDR_SELECT command 
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
    ushort status;

    send_if_cmd_larg(hwidb, CCB_CMD_CIP_ADDR_SELECT, mema_addr);
    status = get_if_status(hwidb);
    return(status);
}

/*****************************************************************************
 * Name: send_pld_version_cmd
 *
 * Description: Send a CCB_CMD_CTRLR_VERSION, CCB_VERSION_PLD
 *              command to the interface indicated. 
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_CTRLR_VERSION, CCB_VERSION_PLD);
    status = get_if_status(hwidb);

    if (status == CMD_RSP_OKAY) 
	*vpld_ver = get_if_val(hwidb);

    status_msg_1arg(status, hwidb, CCB_CMD_CTRLR_VERSION, CCB_VERSION_PLD);
    return(status);
}

/*****************************************************************************
 * Name: send_rom_version_cmd
 *
 * Description: Send a CCB_CMD_CTRLR_VERSION, CCB_VERSION_ROM
 *              command to the interface indicated. 
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Output:
 *   rom_ver -- the EPROM version returned by the IP.
 *              only written to if status is CMD_RSP_OKAY
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_CTRLR_VERSION, CCB_VERSION_ROM);
    status = get_if_status(hwidb);

    if (status == CMD_RSP_OKAY) 
	*rom_ver = get_if_val(hwidb);

    status_msg_1arg(status, hwidb, CCB_CMD_CTRLR_VERSION, CCB_VERSION_ROM);
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
    ushort status;

    send_if_cmd_1arg(hwidb, cmd, cmd_arg);
    status = get_if_status(hwidb);

    status_msg_1arg(status, hwidb, cmd, cmd_arg);
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
    ushort status;
    send_if_cmd_1arg(hwidb, cmd, cmd_arg);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) 
	*retdata = get_if_lval(hwidb);

    status_msg_1arg(status, hwidb, cmd, cmd_arg);
    return(status);
}
/*****************************************************************************
 * Name: send_specific_cmd_1arg_sret
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
    ushort status;
    send_if_cmd_1arg(hwidb, cmd, cmd_arg);
    status = get_if_status(hwidb);
    if (status == CMD_RSP_OKAY) 
	*retdata = get_if_val(hwidb);

    status_msg_1arg(status, hwidb, cmd, cmd_arg);
    return(status);
}

/*****************************************************************************
 * Name: send_vip_4r_start_block_cmd
 *
 * Description: Send CCB_CMD_VIP_4R_START_BLOCK command
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
    ushort status;

    send_if_cmd(hwidb, CCB_CMD_VIP_4R_START_BLOCK);
    status = get_if_status(hwidb);

    return(status);
}

/*****************************************************************************
 * Name: send_vip_4r_write_block_cmd
 *
 * Description: Send CCB_CMD_VIP_4R_WRITE_BLOCK command
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   value -- data to be written
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_VIP_4R_WRITE_BLOCK, value);
    status = get_if_status(hwidb);

    return(status);
}

/*****************************************************************************
 * Name: send_vip_4r_end_block_cmd
 *
 * Description: Send CCB_CMD_VIP_4R_END_BLOCK command
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
    ushort status;

    send_if_cmd(hwidb,  CCB_CMD_VIP_4R_END_BLOCK);
    status = get_if_status(hwidb);

    return(status);
}

/*****************************************************************************
 * Name: send_vip_4r_stat_request_cmd
 *
 * Description: Send CCB_CMD_VIP_4R_REQUEST_STATS command
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
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_VIP_4R_REQUEST_STATS, type);
    status = get_if_status(hwidb);

    return(status);
}


/*****************************************************************************
 * Name: send_serial_encap_cmd
 *
 * Description: Send a CCB_CMD_SERIAL_ENCAP command to the
 *              interface indicated
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface
 *   encap -- serial encapsulation type viz., ET_HDLC, ET_SMDS,
 *
 * Output:
 *   none
 *
 * Return Value:
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise an error code.
 *
 *****************************************************************************/
ushort send_serial_encap_cmd (hwidbtype *hwidb, ulong encap)
{
    send_if_cmd_larg(hwidb, CCB_CMD_SERIAL_ENCAP, encap);
    return(get_if_status(hwidb));
}
