/* $Id: latgroup.c,v 3.5.18.2 1996/08/15 23:30:05 asb Exp $
 * $Source: /release/112/cvs/Xsys/lat/latgroup.c,v $
 *------------------------------------------------------------------
 * latgroup.c - LAT group code handling
 *  
 * May 8, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latgroup.c,v $
 * Revision 3.5.18.2  1996/08/15  23:30:05  asb
 * CSCdi57523:  configuring input-lat-service-permit crashes router
 * Branch: California_branch
 *
 * Revision 3.5.18.1  1996/03/18  20:39:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/07  10:04:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  01:07:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/07  16:13:46  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/01/16  07:42:23  billw
 * CSCdi46868:  latgroup.o should not be in core subsystem
 *
 * Revision 3.3  1995/12/17  18:32:03  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:33:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "mtcsys.h"
#include "config.h"
#include "latgroup.h"
#include "lat_public.h"
#include "parser.h"
#include "../parser/parser_defs_lat.h"


/*
 * This code creates, compares, and otherwise manipulates "Lat Group code
 *  lists", both named and unnamed.  It is used by bridging (for "lat group
 *  code filtering") and by lat (for the obvious purposes.)  Therefore, this
 *  module is normally a part of a library loaded if needed by the image.
 */

/*
 * Global storage
 */

queuetype latgrpQ;

latgroup *lat_find_group (name)
    char *name;
{
    latgroup *latg;

    for (latg = (latgroup *) latgrpQ.qhead; latg; latg = latg->next)
	if (!strcasecmp(latg->name, name))
	    return(latg);
    return(NULL);
}

boolean lat_parse_grouplist (parseinfo *csb,
			     uchar *setmask,		/* Mask to set */
			     uchar *clearmask)		/* Mask to clear */
{
    uchar mask[256/8];
    int i;

    if (GETOBJ(int,3) == LAT_GROUP_ALL) {
	for (i = 0; i < 32; i++) {
	    mask[i] = 0xff;
	}
    } else {
	bcopy(GETOBJ(string,2), mask, 256/8);
    }
    /*
     * If this is a no command with no arguments, then create a clear
     * mask with all bits, and a set mask with only group one specified.
     * This restores the default condition.
     */

    if ((!csb->sense) && (GETOBJ(int,3) == 0)) {
	memset(clearmask, 0xff, 32);
	memset(setmask, 0, 32);
	setmask[0] = 1;
	return(TRUE);
    }

    /*
     * Handle the "no" modifier when a list was present; this simply
     * inverts the meaning.
     */

    if (!csb->sense) {
	switch (GETOBJ(int,4)) {
	  case LAT_GROUP_SET:
	  case LAT_GROUP_ENABLED:
	    GETOBJ(int,4) = LAT_GROUP_DISABLED;
	    break;
	  case LAT_GROUP_DISABLED:
	    GETOBJ(int,4) = LAT_GROUP_ENABLED;
	    break;
	}
    }

    /*
     * Perform the desired action.
     */

    switch (GETOBJ(int,4)) {
      case LAT_GROUP_SET:
	memset(clearmask, 0xff, 32);			/* Clear all bits */
	bcopy(mask, setmask, 32);		/* Set the specified ones */
	break;
      case LAT_GROUP_ENABLED:
	memset(clearmask, 0, 32);		/* Clear nothing */
	bcopy(mask, setmask, 32);		/* Set the specified bits */
	break;
      case LAT_GROUP_DISABLED:
	bcopy(mask, clearmask, 32);		/* Clear specified bits */
	memset(setmask, 0, 32);			/* Set nothing */
	break;
      default:
	bad_parser_subcommand(csb, GETOBJ(int,4));
	return(FALSE);
    }
    return(TRUE);
}

/*
 * lat_apply_groupmask - Apply group code masks to a mask
 */

void lat_apply_groupmask (targetmask, setmask, clearmask)
     byte *targetmask;
     byte *setmask;
     byte *clearmask;
{
    int i;

    /*
     * Apply the masks to the target group code list
     */

    for (i = 0; i < 32; i++) {
	targetmask[i + 1] &= ~clearmask[i]; /* Apply the clear mask */
	targetmask[i + 1] |= setmask[i];    /* Apply the set mask */
    }

    /*
     * Calculate the length of the group code list
     */

    for (i = 32; i > 0; i--)
      if (targetmask[i])
	break;

    targetmask[0] = i;
}

/*
 * Find a group code list matching mask
 */

latgroup *lat_mask_group (mask)
     uchar *mask;
{
    latgroup *latg;
    int i;
    int matchlen = mask[0] + 1;

    for (latg = (latgroup *) latgrpQ.qhead; latg; latg = latg->next) {
	for (i = 0; i < matchlen; i++)
	  if (latg->groups[i] != mask[i])
	    break;
	if (i == matchlen)
	  return(latg);
    }
    return(NULL);
}

/*
 * nv_lat_groupmask - Write out a group code mask to NVRAM
 *
 * This command is used to write out a group code list to non-volatile
 * memory.
 *
 * If the named_group option is TRUE, we will search for any named groups
 * that match this mask, and use that as the group specification.
 *
 * If a name can not be found, we will write out the group code list using
 * the hyphenated notation whenever possible.
 *
 * If the default option is TRUE, we will not write anything if this
 * is the default group code mask of just group 0.
 */

void nv_lat_groupmask (cmd, mask, named_group, def)
     char *cmd;
     byte *mask;
     boolean named_group;
     boolean def;
{
    latgroup *group;
    boolean in_range = FALSE;
    int i;
    int start_range = -1; /* Keep compiler happy */
    
    /*
     * If this list contains just group code zero, don't write anything
     */

    if (def && mask[0] == 1 && mask[1] == 1)
      return;

    /*
     * Write out the prefix
     */

    if (cmd)
      nv_write(TRUE, "lat %s", cmd);

    /*
     * If we can find a match for this mask, write it out using the
     * mask name.
     */

    if (named_group) {
	group = lat_mask_group(mask);		/* Look for a mask */
	if (group) {
	    nv_add(TRUE, " %s", group->name);
	    return;
	}
    }

    /*
     * If this mask is empty, write out a command to disable all
     * groups.
     */

    if (!mask[0]) {
	nv_add(TRUE, " all disabled");
	return;
    }

    /*
     * Not a simple case - write out a command line to set all of the
     * specified groups.
     */

    for (i = 0; i <= (mask[0] << 3); i++) {
	if (mask[(i >> 3) + 1] & (1 << (i & 7))) { /* Group code set */
	    if (!in_range) {		/* If not in a range */
		nv_add(TRUE, " %d", i); /* Specify the range start */
		in_range = TRUE;	/* We are in a range now */
		start_range = i;	/* Rememeber where we started */
	    }
	} else {			/* Not set */
	    if (in_range) {		/* Were we in a range? */
		nv_add(start_range != (i - 1), "-%d", i - 1);
		in_range = FALSE;
	    }
	}
    }
}

/*
 * lat_compare_groups
 * Check for an overlap between two group code masks
 */

boolean lat_compare_groups (mask1, mask2)
     byte *mask1;
     byte *mask2;
{
    int end;

    end = mask1[0] < mask2[0] ? mask1[0] : mask2[0];

    for (; end != 0; end--) {
	if (mask1[end] & mask2[end])
	  break;
    }
    return(end != 0);
}

/*
 * print_lat_groups
 * pretty-print a variable length bit-list of lat groups.
 */

int print_lat_groups (grp, mar, c, printflag)
    uchar *grp;
    int mar,c;
    boolean printflag;
{
    int i,bits,k, width;
    int linecount = 1;

    width = 0;
    for (i=0; i < grp[0]; i++) {
	bits = grp[i+1];
	for (k=0; k < 8; k++) {
	    if (bits & 1) {
		if (printflag)
		  printf("%-4d", i*8+k);
		if ((++width % c) == 0) {
		    if (printflag)
		      printf("\n%*c", mar, ' ');
		    linecount++;
		}
	    } /* if bits */
	    bits >>= 1;
	} /* for k */
    } /* for i */
    return(linecount);
}


/*
 * match_lat_group_name 
 * Match a given lat group name with defined latgroup names.
 */
boolean match_lat_group_name (char *buf, int *pi,
				     char *num, char *name)
{
    latgroup *group;
    int j;
    int i = *pi;

    /* Accept ALL strings for now ! */
    j = 0;
    while (isalnum(buf[i])) {
	name[j++] = buf[i++];
    }
    name[j] = '\0';

    /* don't do anything here if these match */
    if (j == 0) {
	return(FALSE);
    }
    if ((i > *pi) && (group = lat_find_group(name))) {
	*pi = i;
	for (j=0; j < group->groups[0]; j++) {
	    num[j] |= group->groups[j+1];
	}
	return(TRUE);
    } else {
	return(FALSE);
    }
}




