/* $Id: atmcore_rm.c,v 3.1 1995/11/09 11:02:16 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm-core/atmcore_rm.c,v $
 *------------------------------------------------------------------
 * atmcore_rm.c-ATM Switch Resource Manager Initialization and Utility Functions
 *
 *  June, 1995         Michael Grobelch
 *  For now, assume the following structure:
 *
 *  The hardware idb points to atmdb, which points to the RM structure for
 *  the physical port, which then points to the RM structure for all
 *  logical ports. This will then be changed when softidb scheme is finalized.
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmcore_rm.c,v $
 * Revision 3.1  1995/11/09  11:02:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/10  18:09:39  rbhide
 * add a placeholder for rhino branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
