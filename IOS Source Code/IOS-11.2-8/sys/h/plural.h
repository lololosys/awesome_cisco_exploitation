/* $Id: plural.h,v 3.1 1996/02/08 22:51:13 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/h/plural.h,v $
 *------------------------------------------------------------------
 * plural.h:  macros to support output of plurality strings
 *
 * February 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: plural.h,v $
 * Revision 3.1  1996/02/08  22:51:13  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PLURAL_H__
#define __PLURAL_H__

/*
 * Plural(argument, single, multiple)
 * Given an argument, returns single if it is one, else returns multiple
 */

#define Plural(arg,sing,mult) (((arg) == 1)?sing:mult)


/*
 * ArgAndPlural(argument, single, multiple)
 * Similiar to Plural but outputs a comma-separated list consisting
 * of the argument followed by the output of Plural
 */

#define ArgAndPlural(arg,sing,mult) (arg),(((arg) == 1)?sing:mult)

#endif
