/* $Id: cookie.c,v 3.2.58.1 1996/03/21 23:26:51 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/cookie.c,v $
 *------------------------------------------------------------------
 * cookie.c
 *
 * March 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Cookie utility program.  Derived from DiagMon for Cristal by
 * Dave Turner (April 1993)
 *------------------------------------------------------------------
 * $Log: cookie.c,v $
 * Revision 3.2.58.1  1996/03/21  23:26:51  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:01  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/20  00:54:25  ljiang
 * CSCdi42394:  Add vendor codes for cisco partners
 *
 * Revision 2.2  1995/09/22  04:36:14  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.1  1995/06/07  22:36:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  Function: x2444_util -- basic menu utility for displaying and
 *                          altering contents of X2444 serial cookie.
 *
 *  93/04/02  Dave Turner
 *
 *  First checks that serial cookie really exists on board.  If so,
 *  reads and saves contents.  Then displays contents field by field,
 *  allowing the user to alter the byte(s) in any field.  After inter-
 *  acting with the user and storing input, if any, in the copied array,
 *  this function compares the possibly modified content with the original,
 *  and, if any difference is found, stores the new content into the 
 *  X2444 EEPROM.
 *
 *  Returns: 0 for no errors; 1 for readback failure; 2 for no X2444.
 */
#include "monitor.h"
#include "cookie.h"
#include "proto.h"

struct cookie_table {
    char      *field_desc;   /* Name of the field */
    uchar     num_bytes;     /* Length in bytes */
    uchar     num_vals;      /* Number of possible enumerated values in field */
    char *    *vals_ptr;     /* Ptr to an array of strings, one per possible value */
};

static char *if_vals[] =
{"unknown","1E1T","2E","1R1T","1R1E","2R","1E1B",
     "1E2T","1R2T","1E1T1B","1R1T1B","1E2T1B","1R2T1B","1R1B","1E2T8A",
     "1R2T8A", "1E1T1I", "1R1T1I", "1R2T1E", "2R2T", "2E2T", "1E2Thub",
     "", "1E2T1Bhub", "1E1N"};

static char *vendor_codes[]={"unknown","cisco","Synoptics","Chipcom",
                        "Cabletron","DEC","NCR","USRobotics","Alcatel","NEC",
                        "DSC","Microcom","OKI","UB Network","Hewlett Packard"};

static char *proc_codes[]={"PAN","MERLOT","XX","BRUT","BASS","CRISTAL",
			       "CANCUN","SIERRA","C1000"};

static struct cookie_table cookie_info[] = {
    "interfaces",                  1,  sizeof(if_vals)/sizeof(char *),       if_vals,
    "vendor",                      1,  sizeof(vendor_codes)/sizeof(char *),  vendor_codes,
    "ethernet Hw address",         6,   0,                                   (char **)0,
    "processor",                   1,  sizeof(proc_codes)/sizeof(char *),    proc_codes,
    "unused 1",                    7,   0,                                   (char **)0,
    "BCD-packed 8-digit serial #", 4,   0,                                   (char **)0,
    "unused 2",                    9,   0,                                   (char **)0,
    "capabilities (future)",       2,   0,                                   (char **)0,
    "cookie version #",            1,   0,                                   (char **)0,
};

int 
toss_cookie(int argc, char *argv[])
{
    struct cookie *cptr;
    uchar contents[32];
    uchar modcon[32];    /* Copy of original contents modified by user */
    uchar *fld_ptr, *byte_ptr;   /* Pointers in array modcon */
    uchar *tok_ptr;   /* Ptr to user's input buffer */
    uchar inbuf[30];  /* for line input by user */
    uchar i, j, k, numchar;
    long  val;

    /*  First read contents of cookie */
    cptr = get_cookie();
    movbyte((char *)cptr, contents, sizeof(contents));

    /*
    ** If the user is not privileged simply dump the cookie in hex and exit
    */
    if(!(hkeepflags & H_PRIVILEGED)) {
	byte_ptr = contents;
	printf("\ncookie:\n");
	for(j=0; j<2; j++) {
	    for (i = 0; i < sizeof(contents)/2;  i++)
		printf("%2.2x ", *byte_ptr++);
	    putchar('\n');
	}
	return(0);
    }

    /* Now make copy of current contents into "user's" copy */
    movbyte (contents, modcon, sizeof(contents));

    /* User now sees contents field by field with option of altering */
    printf("\nView/alter bytes of serial cookie by field --%s",
	   "\nInput hex byte(s) or: CR -> skip field; ? -> list values");
    fld_ptr = modcon;  /* Point to first field */
    for (i = 0; i < sizeof(cookie_info)/sizeof(struct cookie_table);) {
	printf("\n%s:", cookie_info[i].field_desc);
	/* show bytes in field */
	byte_ptr = fld_ptr;
	for (j = 0; j < cookie_info[i].num_bytes; j++) {
	    printf(" %2.2x",*byte_ptr++);
        }
	if (cookie_info[i].vals_ptr)
	    /* Show string corresponding to this value */
	    if (*fld_ptr < cookie_info[i].num_vals)
		printf("    (%s)",*(cookie_info[i].vals_ptr + *fld_ptr));
	    else
		printf("    (unknown)");
	/* Now get input for this field */
	byte_ptr = fld_ptr;
	/* Space prompt out by length of field title */
        printf("\n%*.s",strlen(cookie_info[i].field_desc) + 2,"> ");
        getline(inbuf, sizeof(inbuf));
        if (*inbuf == 0) {
            fld_ptr += cookie_info[i].num_bytes;
            i++; /* user input <CR> */
            continue;
        }
        tok_ptr = inbuf; 
	for (j = 0; j < cookie_info[i].num_bytes; j++) {
            /* Bypass blanks */
            while (*tok_ptr == ' ')
                tok_ptr++;
            numchar = getnnum(tok_ptr, 16, &val, 2);
#if DEBUG
	    printf("\ntok_ptr=%x *tok_ptr=%2.2x%2.2x numchar=%x val=%x byte_ptr=%x",
		   tok_ptr, *tok_ptr, *(tok_ptr+1), numchar, val, byte_ptr);
#endif
            if ((numchar == 0) || (*tok_ptr == '?')) {
                /* user has not completed line */
                if (cookie_info[i].vals_ptr) {
		    /* Print possible values in this field */
		    printf("\n");
		    for (k = 0; k < cookie_info[i].num_vals; k++)
			printf("%x=%s ",k,*(cookie_info[i].vals_ptr + k));
		}
                else
                    printf("\n<CR> to skip field; otherwise, enter %d byte(s)",
                           cookie_info[i].num_bytes);
                break;
            }
            /* Ok, store val in modcon[] */
            *byte_ptr++ = (uchar)val;
            tok_ptr += numchar;
        }
	if (j == cookie_info[i].num_bytes) {
	    /* Got all the bytes in this field from the user */
	    fld_ptr += cookie_info[i].num_bytes;
	    i++;
	} else {
	    /* Ensure that modcon field still same as original contents */
	    byte_ptr = fld_ptr;  /* Back to start of modcon field */
	    for (j = 0; j < cookie_info[i].num_bytes; j++) {
		*byte_ptr++ = contents[byte_ptr - modcon];
	    }
	}
        
    } /* END for i = ... (all fields in cookie)	*/

    /* Now compare the possibly modified copy with the original */
    if (bcmp(contents, modcon, sizeof(contents))) {
	put_cookie((struct cookie *)modcon);
    }

    return(0);

}

/* end of module */
