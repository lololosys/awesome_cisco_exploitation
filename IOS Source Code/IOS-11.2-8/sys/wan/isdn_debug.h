/* $Id: isdn_debug.h,v 3.2.62.2 1996/05/19 03:10:52 hrobison Exp $
 * $Source: /release/112/cvs/Xsys/wan/isdn_debug.h,v $
 *------------------------------------------------------------------
 * isdn_debug.h -- Debugging declarations for ISDN
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isdn_debug.h,v $
 * Revision 3.2.62.2  1996/05/19  03:10:52  hrobison
 * CSCdi56813:  CSCdi46413 breaks platform independent file modularity
 * Branch: California_branch
 * With the excuse of changes required by the new compiler, take the
 * opportunity to remove platform dependancies, improve code
 * modularity and do some general code clean-up.
 *
 * Revision 3.2.62.1  1996/05/01  14:52:51  hrobison
 * CSCdi46413:  MIP when used as PRI must be in slot 0-4
 * Branch: California_branch
 * Change PRI initialization: for the 7xxx platform the
 * dsl id is now assigned independently of the MIP card
 * slot/subunit location.  The number of PRI interfaces
 * per router is still limited to 10.  The dsl assignment
 * for the 4xxx platform is based on the slot location
 * and corrected to allow only 1 dsl id per slot.
 *
 * Read bug release notes for details on how the ISDN
 * debug output and ISDN show commands have changed.
 * The debug output now uses the hw_short_namestring
 * rather than PRI<dsl #> or BRI<dsl #>.  The show commands
 * now take the interface name or the dsl id as input.
 *
 * Revision 3.2  1995/11/17  18:03:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:18:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up ISDN debugging */
extern void isdn_debug_init(void);
extern int isdn_debug_dsl;		/* DSL for selective debugging */

/* Functions to assign the DSL */
extern inline char *DSL_TO_NAME(int);   /* get interface namestring from Dsl id */
extern void isdn_init_dsl_list(void);
extern int isdn_verify_dsl(void);
extern void isdn_assign_dsl(hwidbtype *, int);
extern void isdn_deassign_dsl(int);
#define ISDN_DEBUG_NO_DSL -1		/* DSL number for being unselective */

/* Functions to handle the number of available B-channels */
extern inline void isdn_increment_bchan (isdn_bri_t *);
extern inline void isdn_decrement_bchan (isdn_bri_t *);

/* Inline to test for debugging */
static inline boolean isdn_debug_ok (boolean flag, int dsl)
{
    if (!flag)
	return(FALSE);			/* Debugging off */
    if (isdn_debug_dsl == ISDN_DEBUG_NO_DSL)
	return(TRUE);			/* Debugging all interfaces */
    if (isdn_debug_dsl == dsl)
	return(TRUE);			/* Debugging this interface */
    return(FALSE);			/* Never mind... */
}

/*
 * The actual debugging flags are defined in isdn_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../wan/isdn_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../wan/isdn_debug_flags.h"
