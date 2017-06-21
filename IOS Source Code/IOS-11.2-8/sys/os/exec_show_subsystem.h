/* $Id: exec_show_subsystem.h,v 3.3.20.1 1996/08/12 16:05:18 widmer Exp $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S U B S Y S T E M . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_subsystem.h,v $
 * Revision 3.3.20.1  1996/08/12  16:05:18  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/25  11:20:21  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.2  1995/11/17  18:47:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:13  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/28  09:30:03  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  22:26:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show subsys
 *
 */
EOLNS	(show_subsys_all_eol, subsys_show_name);
SET     (show_subsys_all_set, show_subsys_all_eol,
	 OBJ(string,1), NULL);

/******************************************************************
 * show subsys class <class>
 *
 */
EOLNS	(show_subsys_class_eol, subsys_show_class);

KEYWORD_ID (show_subsys_class_registry, show_subsys_class_eol,
	    no_alt, OBJ(int,1), SUBSYS_CLASS_REGISTRY,
	    "registry", "Registry subsystems", PRIV_USER);
KEYWORD_ID (show_subsys_class_management, show_subsys_class_eol,
	    show_subsys_class_registry, OBJ(int,1), SUBSYS_CLASS_MANAGEMENT,
	    "management", "Management subsystems", PRIV_USER);
KEYWORD_ID (show_subsys_class_protocol, show_subsys_class_eol,
	    show_subsys_class_management, OBJ(int,1), SUBSYS_CLASS_PROTOCOL,
	    "protocol", "Protocol subsystems", PRIV_USER);
KEYWORD_ID (show_subsys_class_driver, show_subsys_class_eol,
	    show_subsys_class_protocol, OBJ(int,1), SUBSYS_CLASS_DRIVER,
	    "driver", "Driver subsystems", PRIV_USER);
KEYWORD_ID (show_subsys_class_library, show_subsys_class_eol,
	    show_subsys_class_driver, OBJ(int,1), SUBSYS_CLASS_LIBRARY,
	    "library", "Library subsystems", PRIV_USER);
KEYWORD_ID (show_subsys_class_kernel, show_subsys_class_eol,
	    show_subsys_class_library, OBJ(int,1), SUBSYS_CLASS_KERNEL,
	    "kernel", "Kernel subsystems", PRIV_USER);

KEYWORD (show_subsys_class, show_subsys_class_kernel, show_subsys_all_set,
	 "class", "Show subsystems by class", PRIV_USER|PRIV_USER_HIDDEN);

/******************************************************************
 * show subsys name <name>
 *
 */
EOLNS	(show_subsys_name_eol, subsys_show_name);

STRING (show_subsys_namestring, show_subsys_name_eol, no_alt,
	OBJ(string,1), "Subsystem to list; trailing * is a wildcard");

KEYWORD (show_subsys_name, show_subsys_namestring, show_subsys_class,
	 "name", "Show subsystems by name", PRIV_USER|PRIV_USER_HIDDEN);

/******************************************************************
 * show subsys [name | class ]
 *
 */
KEYWORD (show_subsys, show_subsys_name, ALTERNATE,
	 "subsys", "Show subsystem information", PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_subsys
