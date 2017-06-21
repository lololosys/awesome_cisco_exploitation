/* $Id: atmcore_rmctt.h,v 3.1 1995/11/09 11:02:28 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm-core/atmcore_rmctt.h,v $
 *------------------------------------------------------------------
 * atmcore_rmctt.h-ATM Switch Resource Manager Connection Traffic Table
 *
 *  Resource manager Connection Traffic Table (CTT) management interface
 *     header file
 *
 *  This file describes the interface provided by Resource Management to
 *  managing/accessing the Connection Traffic Table.
 *
 *   RM CTT EXTERNAL FUNCTIONS
 *     --------------------------
 *  To accomodate the above requirements, RM provides a number of primative
 *  functions to use the CTT.
 *
 *   CTT Row primative          Function
 *   ------------------------------------
 *   Allocate                   Allocate a new Stable row, or find an active
 *                               duplicate.
 *   Release                    Delete a row.
 *   Modify                     Modify the contents of a row.
 *   Get                        Return the traffic parameters of a row.
 *   Activate                   Make row available for use with VxLs.
 *   Deactivate                 Make row not-available for use with VxLs (to
 *                              modify row).
 *   Activate Transient         Create&activate Transient row for SVC.
 *   Attach VxL                 Used when an existing row is used by a VxL.
 *   Detach VxL                 Used when a VxL relinquishes use of a row.
 *
 *  The user can allocate CTT rows for use in PVCs, either explicitly or
 *  implicitly. When an SVC is established, we just grab a free row in
 *  the CTT, point to SVC for traffic data. The rows are stored in an AVL tree
 *  indexed by row number.
 *
 *  Transient rows are released once their use-counts go to zero.
 *  Stable rows have row numbers from (1..1073741823)
 *  Transient rows have row numbers from (1073741823..2147483647)
 *
 *  April, 1995         Michael Grobelch
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmcore_rmctt.h,v $
 * Revision 3.1  1995/11/09  11:02:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/10  18:09:42  rbhide
 * add a placeholder for rhino branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
