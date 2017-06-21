/* $Id: subsys_private.h,v 3.1.64.2 1996/09/06 01:11:51 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/subsys_private.h,v $
 *------------------------------------------------------------------
 * subsys_private.h - Private declarations for subsystem code
 *
 * June 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: subsys_private.h,v $
 * Revision 3.1.64.2  1996/09/06  01:11:51  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/07/06  05:55:48  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.1  1995/11/09  12:48:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/11  20:12:27  smackie
 * Increase the recursive depth maximum to deal with the RSP. (CSCdi36893)
 *
 * Revision 2.2  1995/07/11  02:50:48  smackie
 * Add some paranoia to the subsystem sequence recursion and prevent
 * bogusness from causing stack problems. Also throw in support for
 * subsystems in multiple data segments. (CSCdi36893)
 *
 * Revision 2.1  1995/06/28  09:18:51  smackie
 * Finally make subsystem sequencing robust with multiple hierarchy
 * sequences. Tidy up subsystem code into the bargain and split the
 * parser code into a new file. Make the subsystem code far noisier
 * about bogusness. (CSCdi23568)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Defines
 */
#define SUBSYS_PROPERTIES_LIST_SIZE  80
#define SUBSYS_PROPERTIES_CHUNK_SIZE 10
#define SUBSYS_SEQUENCE_MAX_DEPTH    8

/*
 * Externs
 */
extern list_header subsyslist[];
extern const char  subsys_property_seq[];
extern const char  subsys_property_req[];

/*
 * Prototypes
 */
extern char *subsys_get_class_string(subsys_class class);
extern boolean subsys_get_property_list(subsystype *subsys,
					const char *const property,
					char       *property_list);
extern boolean subsys_verify_required_list(subsystype *subsys,
					   boolean verbose);

