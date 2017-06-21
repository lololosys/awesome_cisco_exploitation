/* $Id: pas_controller.h,v 3.2.60.2 1996/05/31 23:44:14 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/pas_controller.h,v $ 
 *------------------------------------------------------------------
 * pas_controller.h - PCI port-adaptor based controller support
 *
 * June 1995 - Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pas_controller.h,v $
 * Revision 3.2.60.2  1996/05/31  23:44:14  mbeesley
 * CSCdi59046:  Implement config guidelines
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/21  23:24:23  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1996/02/21  23:06:03  mbeesley
 * CSCdi49494:  Predator needs to support cdbs for channelized interfaces
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:55:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/19  21:54:41  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Hardware handle
 */
typedef slottype platform_hware_handle_type;

/*
 * Prototypes
 */
extern cdbtype 	*pas_controller_create(int, enum pif_types_, int, int,
				       platform_hware_handle_type *);
extern ushort	pas_bandwidth_points(int pa_type);

/* end of file */
