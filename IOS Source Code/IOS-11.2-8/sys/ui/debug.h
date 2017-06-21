/* $Id: debug.h,v 3.2.62.3 1996/07/25 09:56:13 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ui/debug.h,v $
 *------------------------------------------------------------------
 * debug.h -- External declarations for debug flag management
 *
 *  May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: debug.h,v $
 * Revision 3.2.62.3  1996/07/25  09:56:13  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/07/23  18:32:51  snyder
 * CSCdi63791:  debugging arrays should be const
 *              4112 out of data space, 556 out of image
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:29:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/14  01:36:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.2.26.1  1996/02/20  21:25:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:47:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:11:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DEBUG_H__
#define __DEBUG_H__

/*
 * debug_item_type defines a standard way of describing a debugging
 * flag. Most protocols keep their debugging flags in special
 * cells, which are pointed to by the members of arrays of entries
 * of this type. The generic debugging support provides a few routines
 * for dealing with such arrays. By convention, the end of an array
 * is flagged by a null flag pointer.
 */
typedef struct debug_item_type_ {
    boolean *var;		/* Address of on/off flag */
    char *txt;			/* Descriptive text about what to debug */
} debug_item_type;

/* 
 * debug_list_t and debug_listq_find()
 * define a general way for a subsystem to get idb
 * and access list used for filtering debug output
 */
 
typedef struct debug_list_ {
    struct debug_list_ *next;
    boolean *pflag;
    idbtype *idb;
    int access_list;
} debug_list_t;

debug_list_t *debug_listq_find (boolean *pflag);

/* Initialize the debugging system */
extern void debug_init(void);

/*
 * Determine whether "debug all" is in effect. When a new subsystem
 * is initialized, it uses this routine to determine whether it should
 * immediately turn on its debugging flags or not.
 */
extern boolean debug_all_p(void);

/*
 * Generic routine for "debug" parse chains to call. Only useful if you're
 * using totally stock flags and arrays.
 */
extern void debug_command(parseinfo *csb);

/* Routine to handle "debug all" and "undebug all" */
extern void debug_all_cmd(parseinfo *csb);

/* Routine that implements the "show debug" command */
extern void show_debug(void);

/*
 * Display the value of a debugging flag in the common format used for
 * all such flags. The "always" argument is used to force the flag
 * to be displayed even if it's turned off; the normal "show" display
 * only shows flags whose sense is opposite of the sense of "debug all".
 */
extern void debug_show_flag(boolean flag, boolean always, char *text);

/*
 * Show a whole array of flags. The end of the array is marked by an
 * entry with a null flag pointer.
 */
extern void debug_show_flags(const debug_item_type *array, char *heading);

/*
 * generic routine to call when anybody issues
 * a "debug all" or "undebug all" command or
 * whenever you want to set the state of all
 * debug flags at once.
 */

extern void generic_debug_all (const debug_item_type *, boolean );


#endif __DEBUG_H__
