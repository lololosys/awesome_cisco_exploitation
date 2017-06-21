/* $Id: cipllc_types.h,v 3.3 1996/01/29 22:00:52 motto Exp $
 * $Source: /release/111/cvs/Xsys/dspu/cipllc_types.h,v $
 *------------------------------------------------------------------
 * SNA Network Management CIP LLC alert handler definitions
 *
 * May 95, Mike Otto
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cipllc_types.h,v $
 * Revision 3.3  1996/01/29  22:00:52  motto
 * CSCdi46976:  CIP NetView alerts are not formatted correctly
 *
 * Revision 3.2  1995/11/17  09:04:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/09  19:06:58  rbatz
 * CSCdi38250:  CIP CSNA SNMP traps do not work
 *
 * Revision 2.1  1995/06/07  20:28:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CIP_LLC_TYPE_H__
#define __CIP_LLC_TYPE_H__

#include "../hes/sysmgt_snallc.h"

/*
 * This structure provides a decoupling
 * of the CIP subsystem from the SNANM
 * subsystem.
 */
struct cip_llctype_ {
    hwidbtype   *idb;                         /* pointer to CIP virtual idb */
    ushort      csna_port;                    /* CSNA port number */
    ushort      csna_path;                    /* CSNA path number */
    ushort      csna_device;                  /* CSNA device number */
    long        virtual_index;                /* Virtual idx of CIP LLC port */
    uint        adapno;                       /* Virtual Adaptor number */
    char        vadapt_name[9];               /* Name of vadapter */
    ushort      lan_id;                       /* virtual LAN id */
    ushort      local_ring;                   /* Local SRB ring for vadapt */
    uchar       local_macaddr[IEEEBYTES];     /* vadapter mac address */
    uchar       local_sap;                    /* local SAP opened by VTAM */
    uchar       remote_macaddr[IEEEBYTES];    /* remote mac address */
    uchar       remote_sap;                   /* remote SAP */
    uchar       v_r;                          /* Nr count */
    uchar       v_s;                          /* Ns count */
    uchar       last_n_r_rxd;                 /* last Nr rxd */
    uchar       last_ctrl_rxd_msb;            /* last control rxd hi-order */
    uchar       last_ctrl_rxd_lsb;            /* last control rxd lo-order */
    uchar       last_ctrl_txd_msb;            /* last control txd hi-order */
    uchar       last_ctrl_txd_lsb;            /* last control txd lo-order */
    uchar       lstn_role;                    /* lstn role (local/rem busy) */
    uchar       frmr[FRMR_INFO_LEN];          /* frmr bytes */
    sysmgt_snadlcllc_states_t cc_state;       /* llc connection state */
};

#endif
