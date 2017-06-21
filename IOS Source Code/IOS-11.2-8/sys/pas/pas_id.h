/* $Id: pas_id.h,v 3.4.6.3 1996/07/04 03:02:27 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/pas_id.h,v $
 *------------------------------------------------------------------
 * pas_id.h : PCI Port Adaptor ID fields
 *
 * January 1995, Michael Beesley 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: pas_id.h,v $
 * Revision 3.4.6.3  1996/07/04  03:02:27  mbeesley
 * CSCdi62104:  Update PA/PM id enumerations
 * Branch: California_branch
 *
 * Revision 3.4.6.2  1996/04/29  17:57:33  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.4.6.1  1996/04/19  15:26:22  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.5  1996/03/19  08:30:19  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.4  1996/01/08  19:24:00  ptran
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.3  1995/12/17  02:14:55  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.2  1995/12/06  21:49:09  rmontino
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.1  1995/11/17  18:04:32  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.4  1996/03/06  08:42:06  feisal
 * CSCdi50820:  Support for 5E port adapters on VIP1
 *
 * Revision 3.3  1995/12/06  23:55:06  mbeesley
 * CSCdi45076:  Need to reserve Port Adaptor ID types
 * Update the software to be consistent with the port adaptor
 * eeprom specification, which has many PA ID values reserved
 * for port adaptor's in development.
 *
 * Revision 3.2  1995/11/17  18:55:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 ************************* PLEASE READ **********************************
 * Notes:
 * 1. This file contains a list of ID values that get programmed into
 *    hardware. So, as is with cpu.h and emt.h, any changes to this file
 *    in a release must also be made to older releases of software so ID
 *    values do not get overloaded. Also, any changes here should be
 *    reflected in the Port Adaptor EEPROM Specification document.
 * 2. Any addition made to this list of Port Adapter IDs must be
 *    accompanied by an addition to the array of descriptive strings for
 *    Port Adapters in pas/pas_controller.c.
 *
 * Q. HOW DOES ONE GET A NEW PORT ADAPTER/MODULE ID ?
 * A. Send email to get-paid@cisco.com with the following info :
 *
 *       - the enumeration (P?_HARDWARE_<whatever>) that you would 
 *         like to use, where ? is A or M for Adapter or Module
 *       - a descriptive string (as in comments below)
 *
 * To look at the latest assignment of ID's, see 
 *            /nfs/csc/Docs/PAID_assignment
 */
 
typedef enum pa_hardware_type_ {
    PA_HARDWARE_UNKNOWN      = 0xffff,
    PA_HARDWARE_1FE          = 0,     /* 1 port Fast Ethernet */
    PA_HARDWARE_8E           = 1,     /* 8 port Ethernet */
    PA_HARDWARE_4E           = 2,     /* 4 port Ethernet */
    PA_HARDWARE_5E           = 3,     /* 5 port Ethernet */
    PA_HARDWARE_4T           = 4,     /* 4 port Serial */
    PA_HARDWARE_4R           = 5,     /* 4 port Token Ring */
    PA_HARDWARE_CE1	     = 6,     /* 2 port Channelized E1 */
    PA_HARDWARE_CT1	     = 7,     /* 2 port Channelized T1 */
    PA_HARDWARE_EEC	     = 8,     /* Encryption Engine */
    PA_HARDWARE_CEC	     = 9,     /* Compression Engine, 768K RAM */
    PA_HARDWARE_FDDI_DAS_MM  = 10,    /* Dual Attached MM Station */
    PA_HARDWARE_FDDI_DAS_SS  = 11,    /* Dual Attached SS Station */
    PA_HARDWARE_M4T	     = 12,    /* 4T Mueslix Serial */
    PA_HARDWARE_M8T_V35      = 13,    /* 8T Mueslix Serial-V35 */
    PA_HARDWARE_M8T_X21      = 14,    /* 8T Mueslix Serial-X21 */
    PA_HARDWARE_M8T_RS232    = 15,    /* 8T Mueslix Serial-RS232 */
    PA_HARDWARE_DUAL_HSSI    = 16,    /* 2 port HSSI */
    PA_HARDWARE_1FE_TX_ISL   = 17,    /* 1 port Fast Ethernet, TX-ISL */
    PA_HARDWARE_1FE_FX_ISL   = 18,    /* 1 port Fast Ethernet, FX-ISL */
    PA_HARDWARE_1FE_TX_NISL  = 19,    /* 1 port Fast Ethernet, Tx-nISL */
    PA_HARDWARE_C7100_IO1FE  = 20,    /* 1 port Fast Ethernet, C7100 IO card */
    PA_HARDWARE_C7100_CPU    = 21,    /* C7100 CPU card */
    PA_HARDWARE_AMDAHL       = 22,    /* Amdahl Port Adaptor */
    PA_HARDWARE_ATM_OC3_MM   = 23,    /* OC3 MM ATM (1 port) */
    PA_HARDWARE_CEC_3M       = 24,    /* Compression engine, 3M RAM */
    PA_HARDWARE_POSIP        = 25,    /* POSIP */
    PA_HARDWARE_CT3          = 26,    /* CT3, 1 port CT3, 4 port DSX-1 */
    PA_HARDWARE_100VG        = 27,    /* 100VG, 1 port */
    PA_HARDWARE_1FE_FX_NISL  = 28,    /* 1 port Fast Ethernet, FX-nISL */
    PA_HARDWARE_SINGLE_HSSI  = 29,    /* 1 port HSSI */
    PM_HARDWARE_2E2W         = 30,    /* Combo 2E2W */
    PM_HARDWARE_1E1R2W       = 31,    /* Combo 1E1R2W */
    PM_HARDWARE_MBRI_4ST     = 32,    /* 4 port S/T interface Multi-BRI */
    PM_HARDWARE_MBRI_8ST     = 33,    /* 8 port S/T interface Multi-BRI */
    PM_HARDWARE_MBRI_4U      = 34,    /* 4 port U interface Multi-BRI */
    PM_HARDWARE_MBRI_8U      = 35,    /* 8 port U interface Multi-BRI */
    PM_HARDWARE_CD2430_4AS   = 36,    /* 4 port Async/Sync */
    PM_HARDWARE_CD2430_8AS   = 37,    /* 8 port Async/Sync */
    PA_HARDWARE_T1_1PORT_CSU = 38,    /* 1 port Channelized T1 w/CSU */
    PA_HARDWARE_T1_2PORT_CSU = 39,    /* 2 port Channelized T1 w/CSU */
    PA_HARDWARE_T1_1PORT     = 40,    /* 1 port Channelized T1 w/o CSU */
    PA_HARDWARE_T1_2PORT     = 41,    /* 2 port Channelized T1 w/o CSU */
    PA_HARDWARE_E1_1PORT     = 42,    /* 1 port Channelized E1 */
    PA_HARDWARE_E1_2PORT     = 43,    /* 2 port Channelized E1 */
    PA_HARDWARE_ATM_OC3_SM   = 44,    /* 1port OC3 SM ATM (155mb) */
    PA_HARDWARE_ATM_OC3_UTP5 = 45,    /* 1port OC3 UTP5 ATM (155mb) */
    PA_HARDWARE_BUS_AND_TAG  = 46,    /* 1port IBM Bus and Tag */
    PA_HARDWARE_ESCON        = 47,    /* 1port IBM ESCON channel */
    PA_HARDWARE_16E          = 48,    /* 16 port ethernet */
    PA_HARDWARE_FMM_FD       = 49,    /* Dual attached full duplex MM */
    PA_HARDWARE_FSM_FD       = 50,    /* Dual attached full duplex SM */
    PA_HARDWARE_UNKNOWN_51   = 51,    /* Unknown */
    PA_HARDWARE_UNKNOWN_52   = 52,    /* Unknown */
    PA_HARDWARE_C7100_JUNIOR = 53,    /* Low performance C7100 CPU card */
    PA_HARDWARE_C7100_IO     = 54,    /* C7100 IO card (no media) */
    PA_NUM_HARDWARE_TYPES,            /* Not an actual type */

} pa_hardware_type_t;

/* end of file */
