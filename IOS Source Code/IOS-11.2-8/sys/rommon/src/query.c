/* $Id: query.c,v 3.2.62.1 1996/03/18 21:46:25 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/query.c,v $
 *------------------------------------------------------------------
 * query.c
 *
 * February 1993, Rob Clevenger
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Mechanism to query the user for test parameters.
 *------------------------------------------------------------------
 * $Log: query.c,v $
 * Revision 3.2.62.1  1996/03/18  21:46:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:41:46  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:04:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:42:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Mechanism to query the user for test parameters.
** Query the user for the test parameters dictated by the queryflag bits.
** There must be an argument for each bit set in queryflag.  The current
** value of the argument will be displayed as the default and will be used
** if the user simply enters a <ret>.
**
** Note the case of QU_R_WR (read or write; case 4).  If this case is
** specified 'r' (for read) and the QU_VALUE (specify the value to be
** written) bit is set the routine will automatically skip over the
** query and the pointer arg supplied for it.  Additionally, if the user
** chooses "write" and the QU_OPSIZ bit is set, the value to be written
** is tested to see if it "fits" into the specified operation size.
*/

#include <stdarg.h>
#include "queryflags.h"

static
char *querystrings[] = {  /* strings to match the queryflag bits */
    "source address",
    "destination address",
    "start address",
    "test size or length in bytes",
    "read or write the location (r/w)",
    "pattern to be written",
    "number of passes",
    "operation size 'l'ong, 'w'ord, or 'b'yte",
    "incrementing pattern",
    "scope trigger address",
};
static char opsizstr[] = "bwll";  /* must be in this order with 2 l's */

void    
query_user(QUERYFLAG queryflag, ...)
{
    int i;
    va_list argptr;
    unsigned temp, mask, wrpat, *temp_ptr;
    char rd_wr;
    char buffer[80];

    wrpat = mask = rd_wr = 0;
    va_start(argptr, queryflag);
    for(i=0; i < (sizeof(QUERYFLAG)*8); i++) {
	if((queryflag >> i) & 1) {                      /* bit is set */
	    temp_ptr = va_arg(argptr, unsigned *);      /* get next argument */
	    temp = *temp_ptr;
	    switch(i) {
	    case QU_VALUE_BIT:
		/*
		** If user wishes to read, there is no
		** need to query for write value.  Also
		** protect against operation size checking
		** on "wrpat" by turning off the QU_VALUE
		** bit.
		*/
		if(rd_wr == 'r') {
		    queryflag &= ~QU_VALUE;
		    break;
		}
		/* fall through */
	    case QU_SOURCE_BIT: case QU_DEST_BIT:
	    case QU_START_BIT: case QU_SIZE_BIT:
	    case QU_TRIGGER_BIT: case QU_PASSES_BIT:
		sprintf(buffer, "Enter in hex the %s", querystrings[i]);
		temp = gethex_answer(buffer, temp, 0, 0xFFFFFFFF);
		if( i == QU_VALUE_BIT ) wrpat = temp;
		break;
	    case QU_R_WR_BIT:
		sprintf(buffer, "Do you wish to %s?",
			querystrings[QU_R_WR_BIT]);
		rd_wr = temp = getc_answer(buffer, "rw", temp);
		break;
	    case QU_OPSIZ_BIT:
		temp = opsizstr[temp-1];  /* convert to ascii char */
		if(!strchr(opsizstr, temp)) temp = 'b';  /* validate it */
      		sprintf(buffer, "Enter the %s",
			querystrings[QU_OPSIZ_BIT]);
		temp = getc_answer(buffer, opsizstr, temp);
		switch(temp) {  /* convert back to a number */
		case 'l':
		    temp = 4;
		    mask = 0;
		    break;
		case 'w':
		    temp = 2;
		    mask = 0xffff0000;
		    break;
		case 'b':
		    temp = 1;
		    mask = 0xffffff00;
		    break;
		}
		if((queryflag & QU_VALUE) && (wrpat & mask)) {
		    sprintf(buffer,
 "Value (0x%x) is larger than the op. size - respecify? y/n",
			    wrpat);
		    if(getc_answer(buffer, "yn", 'y') == 'y') {
			i--;  /* do this (QU_OPSIZ) over again */
			va_rollback(argptr, unsigned *);
			continue;
		    }
		}
		break;
	    default:
		continue;  /* in case of unsupported bit */
	    }
	    *temp_ptr = temp;  /* set new value */
	}
    }
    va_end(argptr);
}

int
getc_answer(char *msg, char *cmpstr, char curval)
{
    char buffer[4];

    while(1) {
	printf("%s  [%c]:  ", msg, curval);
	getline(buffer,sizeof(buffer));
	if(buffer[0] == '\0')  /* null line returns current value */
	    return(curval);
	if(strchr(cmpstr, buffer[0])) return(buffer[0]);
    }
}

unsigned
gethex_answer(char *msgstr, unsigned currentval, unsigned min, unsigned max)
{
  char buffer[32];
  unsigned newval;

  while(1) {
    printf("%s [0x%x]:  ", msgstr, currentval);
    getline(buffer,sizeof(buffer));
    if(buffer[0] == '\0')  /* null line returns current value */
      return(currentval);
    if((getnum(buffer,16,&newval)) <= 0 || (newval < min) || (newval > max)) {
      printf("valid entry 0x%x to 0x%x...try again\n", min, max);
      continue;
    } else return(newval);
  }
}





