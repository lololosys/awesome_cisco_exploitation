/* $Id: atmcore_rmcac.h,v 3.1 1995/11/09 11:02:26 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm-core/atmcore_rmcac.h,v $
 *------------------------------------------------------------------
 * atmcore_rmcac.h-ATM Switch Resource Mgr Connection Admission Control
 *
 * Resource manager Connection Admission Control (RCAC) interface header file
 *
 *  This file describes the interface provided by Resource Management to:
 *      - determine whether a particular connection request can be satisfied
 *        from a traffic/QOS standpoint (e.g. whether bandwidth is available,
 *        with particular loss/delay characteristics). If the request can be
 *        satisfied, allocate resource for the connection.
 *      - create and delete logical interfaces (terminated VPs).
 *
 *  Separate RCAC allocation request/responses exist to:
 *  - attempt to allocate connection resources (traffic) on a interface or
 *    choose a interface that can satisfy the request from a list of interface.
 *  - release connection resources (VPI/VCI, traffic) on a interface.
 *  - compare traffic parms of two VPLs/VCLs to see that they are
 *    compatible.
 *  - collect PNNI metrics.
 *
 *  Sequences of these requests can be combined to provide for the
 *  needs of PVC and SVC connection resource allocation and release.
 *  They do not provide the neccesary interface for installation of
 *  connections.
 *
 *
 *  April, 1995         Michael Grobelch
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmcore_rmcac.h,v $
 * Revision 3.1  1995/11/09  11:02:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/10  18:09:41  rbhide
 * add a placeholder for rhino branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

