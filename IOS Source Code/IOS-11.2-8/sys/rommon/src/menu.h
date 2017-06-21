/* $Id: menu.h,v 3.2 1995/11/17 18:42:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/menu.h,v $
 *------------------------------------------------------------------
 * menu.h
 *
 * April 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the diagnostic menues.
 *------------------------------------------------------------------
 * $Log: menu.h,v $
 * Revision 3.2  1995/11/17  18:42:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:37:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __MENU_H__
#define __MENU_H__

/*
** Structure defines for the handling of menus.
*/

struct menuinfo {
  char *mtitle;          /* the menu title */
  int mtparam;           /* optional title parameter */
  int (*mtfunc)();       /* optional menu title function (may be NULL) */
  char *mprompt;         /* optional menu prompt (may be NULL for default) */
  int msize;             /* the number of items in the menu */
  struct mitem *miptr;   /* pointer to array of menu item structures */
};

struct mitem {
  char *mline;           /* the menu item display line OR */
  int (*mlfunc)();       /* the menu item display line function */
  int mlparam;           /* optional menu line parameter */
  int (*mfunc)();        /* the function invoked for this menu item */
  int *mfparam;          /* optional function parameter */
  int mflag;             /* function flag (see below) */
  int (*mixfunc) ();     /* the menu item exists function */
  int mixparam;          /* parameter for above function */
};

/* defines for mflag above */
#define MF_CONTINUOUS    0x01    /* menu item may be run continuously */
#define MF_DOALL         0x02    /* menu item can be included in doall */
#define MF_NOTNET        0x04    /* menu item not allowed when net boot */

extern void menu(struct menuinfo *menuptr, char menucmd);
extern void displaymenu(struct menuinfo *menuptr);

#endif  /* __MENU_H__ */
