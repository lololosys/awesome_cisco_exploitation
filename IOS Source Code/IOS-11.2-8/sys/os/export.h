/* $Id: export.h,v 3.2 1995/11/17 18:47:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/export.h,v $
 *------------------------------------------------------------------
 * export.h - Definitions for symbol export/import facility
 *
 * December 22, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This module contains the definitions for the export/import facility.
 *------------------------------------------------------------------
 * $Log: export.h,v $
 * Revision 3.2  1995/11/17  18:47:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:54:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __EXPORT_H__
#define __EXPORT_H__

/*
 * Macros
 */

#define EXPORT_INIT(facility) { extern void EXPORT_ ## facility ## _init(void); EXPORT_ ## facility ## _init(); }

/*
 * Data structures
 */

typedef struct export_list_ {
    struct export_list_ *next;	/* Link to next export list */
    char *name;			/* Name of this export list */
    int nitems;			/* Number of items in vector */
    void *vector;		/* Address of transfer vector */
} export_list;

/*
 * Function prototypes
 */

extern void export_list_init(export_list *);
extern export_list *export_find_list(char *);
extern void *export_import(char *, int);

#endif __EXPORT_H__
