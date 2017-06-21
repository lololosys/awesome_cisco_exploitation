/* $Id: subsys.h,v 3.3.20.2 1996/07/06 05:52:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/h/subsys.h,v $
 *------------------------------------------------------------------
 * subsys.h -- Definitions for Subsystem Data Structure
 *
 * 1-June-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: subsys.h,v $
 * Revision 3.3.20.2  1996/07/06  05:52:52  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.3.20.1  1996/03/18  19:37:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/09  05:05:10  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.24.1  1996/02/20  00:49:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/01/25  11:17:28  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.2  1995/11/17  09:13:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:23:54  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:36:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SUBSYS_H__
#define __SUBSYS_H__

/*
 * Subsystem header
 * One instance of this data structure for every subsystem.
 * This allows us to discover and initialize a subsystem after the
 * linking phase is complete.  The information in the subsystem header
 * may change when dynamic loading and linking is added to the system.
 */

#define SUBSYS_MAGIC_NUMBER     0xC15C0515     /* CISCO SIS, sort of */
#define SUBSYS_MAGIC_NUMBER_PT1 ((SUBSYS_MAGIC_NUMBER>>16)&0xFFFF)
#define SUBSYS_HEADER_VERSION   1L

#define SUBSYS_CLASS_MIN        0x0001
#define SUBSYS_CLASS_KERNEL     0x0001
#define SUBSYS_CLASS_LIBRARY    0x0002
#define SUBSYS_CLASS_DRIVER     0x0003
#define SUBSYS_CLASS_PROTOCOL   0x0004
#define SUBSYS_CLASS_MANAGEMENT 0x0005
#define SUBSYS_CLASS_REGISTRY   0x0006
#define SUBSYS_CLASS_SYSINIT    0x0007
#define SUBSYS_CLASS_MAX        0x0008

#define SUBSYS_STATUS_INACTIVE  0x0000
#define SUBSYS_STATUS_ACTIVE    0x0001

#define SUBSYS_PROPERTIES_MAX        2

typedef void (*subsys_entry)(subsystype *);
typedef ushort subsys_class;
typedef ushort subsys_type;

struct subsystype_ {
    ulong   magic1;
    ulong   magic2;
    ulong   header_version;
    ulong   kernel_majversion;
    ulong   kernel_minversion;
    char   *namestring;
    ulong   subsys_majversion;
    ulong   subsys_minversion;
    ulong   subsys_editversion;
    subsys_entry  entry;
    subsys_class  class;
    subsys_type   status;
    ulong   ID;
    const char   *const properties[SUBSYS_PROPERTIES_MAX];
};

/*
 * SUBSYS_HEADER
 * Use this macro to set up a subsystem data structure
 */

#define SUBSYS_HEADER(name,major,minor,edit,init_addr,\
		      class, property0, property1) \
    subsystype sub_ ## name ## _header = { \
        SUBSYS_MAGIC_NUMBER, \
        SUBSYS_MAGIC_NUMBER, \
        SUBSYS_HEADER_VERSION, \
        SYS_MAJORVERSION, \
        SYS_MINORVERSION, \
        #name, \
        major, \
        minor, \
        edit,  \
        init_addr, \
	class, \
	SUBSYS_STATUS_INACTIVE, \
	0, \
	{ property0, property1 } \
    };

/*
 * Prototypes
 */
extern subsystype *subsys_find_by_name(char *name);
extern void subsys_init(void);
extern void subsys_init_class(subsys_class class);

#endif /* __SUBSYS_H__ */
