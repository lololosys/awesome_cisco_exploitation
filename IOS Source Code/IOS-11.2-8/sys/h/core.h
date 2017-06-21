/* $Id: core.h,v 3.2 1995/11/17 09:11:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/core.h,v $
 *------------------------------------------------------------------
 * core.h - Core system definitions
 *
 * December 28, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module contains the definitions for the core system/module
 * interface.
 *------------------------------------------------------------------
 * $Log: core.h,v $
 * Revision 3.2  1995/11/17  09:11:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:34:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This module contains the definitions for the interface between the core
 * module and load modules. Be careful in what you change here, because
 * incompatible changes cause the need to re-release all of the load
 * modules.
 */

#ifndef __CORE_H__
#define __CORE_H__


/*
 * Define the version numbers
 *
 * The major version number, CORE_MAJVER, is changed whenever an incompatible
 * change is made between the interface between the core system and load
 * modules. Each load module contains an expected major version number -
 * if this is different from the actual version number, the module will
 * not be loaded.
 *
 * The minor version number, CORE_MINVER, is changed whenever an upwards
 * compatible change has been made to the core/module interface. A module
 * will be loaded as long as its expected minor version number is less than or
 * equal to the current core version number.
 */

#define CORE_MAJVER	0	/* Core major version number */
#define CORE_MINVER	0	/* Core minor version number */

/*
 * Define the layout of the core vector
 *
 * When a module is loaded, it is passed the address of the core vector.
 * The core vector contains the initial information that a module needs
 * in order to initialize itself.
 *
 * The core vector should only contain things which are not easily
 * obtainable via the export mechanism, such as addresses and data related
 * to the export mechanism. This is because the core vector mechanism
 * contains only limited type checking, and should therefore be used with
 * extreme prejudice.
 */

typedef struct core_vector_ {
    ulong majver;		/* Major version number */
    ulong minver;		/* Minor version number */
    ulong len;			/* Length of core vector */
    void *(*import)(char *, int); /* Address of import handler */
} core_vector;

/*
 * Define the function codes for the core/module interface
 *
 * The core module calls load modules at the defined entry point, and
 * passes a function code and a list of parameters specific to the
 * function code.
 *
 * The function codes are defined here.
 */

enum MODULE_FUNCTION {
    MODULE_INIT			/* Initialize a load module */
  };

#endif __CORE_H__
