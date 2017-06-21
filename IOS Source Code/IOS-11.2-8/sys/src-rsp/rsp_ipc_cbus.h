/* $Id: rsp_ipc_cbus.h,v 3.1.64.2 1996/06/06 21:42:56 kao Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_ipc_cbus.h,v $
 *------------------------------------------------------------------
 * IPC RSP Common CBus driver constants and structures
 *
 * June 1995, Yan Ke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_ipc_cbus.h,v $
 * Revision 3.1.64.2  1996/06/06  21:42:56  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * Fix populate_txq_txacc_to_ips not defined in boot image
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/06/04  05:04:34  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * move all DFS/IPC related variables and routines to rsp_ipc_cbus.c
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  13:28:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/26  21:33:02  motto
 * Decouple IPC from RSP subsystem.
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.2  1995/09/17  20:55:36  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.1  1995/08/04  01:41:11  yanke
 * Initial commit of placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define IPC_CARD_LIST_SIZE (MAX_SLOTS + 1)

/*
 * Function Prototypes.
 */
extern int ipc_set_hw_loopback(boolean);


extern ipc_card_t ipc_card_list[IPC_CARD_LIST_SIZE];

extern void populate_txq_txacc_to_ips_body(void);
