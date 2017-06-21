/* $Id: modemcap.c,v 3.6.28.8 1996/08/26 17:54:52 bjthomas Exp $
 * $Source: /release/112/cvs/Xsys/ts/modemcap.c,v $
 *------------------------------------------------------------------
 * modemcap.c  -- routines for dealing with modem capabilities database
 *
 * June 1995, Tim Kolar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modemcap.c,v $
 * Revision 3.6.28.8  1996/08/26  17:54:52  bjthomas
 * CSCdi54592:  change in modemcap does not do nvgen
 * Branch: California_branch
 *
 * Revision 3.6.28.7  1996/08/08  17:56:36  sjackson
 * CSCdi65288:  Modem autoconfig should always redetect and attempt to
 * lock speed
 * Branch: California_branch
 * Modem locking speed is now configureable which prevents changing of the
 * desired line speed. Configuration is also attempted after every hangup.
 *
 * Revision 3.6.28.6  1996/08/07  09:03:18  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.28.5  1996/07/03  18:18:02  sjackson
 * CSCdi61825:  Modemcap edit should allow a no configuration
 * Branch: California_branch
 *
 * Revision 3.6.28.4  1996/06/28  23:31:03  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.28.3  1996/05/21  10:07:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.28.2  1996/03/28  18:38:36  sjackson
 * CSCdi50364:  Modem autoconfig database should contain Microcom commands
 * Branch: California_branch
 *
 * Revision 3.6.28.1  1996/03/18  22:19:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.1  1996/03/07  10:58:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6  1996/01/22  07:29:59  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/12  20:03:22  sjackson
 * CSCdi44704:  modem busyout command is needed
 * Command added as an extension of the line modem autoconfigure
 *
 * Revision 3.4  1995/12/17  18:39:54  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/18  22:18:00  tkolar
 * CSCdi44151:  Modem autoconfig gets line speeds confused
 * Change the way that the modemcap SPD attribute is used by modem
 * configuration.
 *
 * Revision 3.2  1995/11/17  18:55:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:33:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/26  01:07:58  tkolar
 * *** empty log message ***
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 *  Modemcap
 *
 *    Modemcap is loosely based on Unix style termcaps.  There are a series
 *  of records (one per modem) each containing a list of attributes.  
 *  Below is a list of attributes, with their text token in parens.
 *
 *   Factory defaults (FD)
 *	Return modem to factory default configuration
 *   Autoanswer (AA)
 *      Set modem to answer the phone if DTR is high, preferably
 *      on the first ring.
 *   Carrier detect (CD)
 *      Have the modem raise CD when a carrier is detected.  Oddly this is
 *      not the default for most modems, which just raise CD and leave it 
 *      high forever.
 *   Drop with DTR (DTR)
 *      Have the modem drop the connection if we drop DTR.   There is
 *      frequently an option to reset the configuration while doing this,
 *      and this option should *NOT* be used.  The connection should drop
 *      and that's all.
 *   Set hardware flow control (HFL)
 *      Have the modem use RTS/CTS (out-of-band) flowcontrol.
 *   Set software flow control (SFL)
 *      Have the modem use XON/XOFF (in-band) flowcontrol
 *   Lock to maximum DTE speed (SPD)
 *      Tell modem to lock the speed at which it talks to the commserver
 *      to a single rate, preferably the highest possible.
 *   Desired DTE speed (DTE)
 *      Lock the speed at which the modem talks to the commserver.
 *   Best error control (BER)
 *      Tell modem to negotiate its best error control with remote modems.
 *      For ARAP users this optimally will be MNP5/LAPB but not MNP4
 *   Best compression (BCP)
 *      Tell modem to negotiate its best compression with remote modems.
 *   No error control (NER)
 *      Tell modem to negotiate no error control with remote modems.
 *   No compression (NCP)
 *      Tell modem to negotiate no compression with remote modems.
 *   No echo (NEC)
 *      Ask modem to not echo characters.  Currently not used.
 *   No response codes (NRS)
 *      Ask modem to not send a response when we issue a command.  
 *	Currently not used.
 *   Set Caller ID (CID)
 *      Request that Caller ID information be returned when dialin occurs.
 *      Current not used.
 *   On-hook (ONH)
 *      Tell the modem to go on-hook to accept calls.
 *   Off-hook (OFH)
 *      Tell the modem to go off-hook to stop accepting calls.
 *   Miscellaneous strings (MSC)
 *      Send any extra commands necessary for the modem to work (possibly 
 *      with specific platforms)
 *   Template entry (TPL)
 *      This is the name of another modem type, which will be referenced for
 *      the value of any of the above attributes if it is not set on the 
 *      the current modem type.
 *   Default Modemap (DEF)
 *      This value should only appear in the default modemcap entries (listed
 *      in this file.)  It is used to make sure that these entries are not
 *      nvgenned.
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#include <string.h>
#include <ciscolib.h>
#include "interface.h"
#include "packet.h"
#include "../h/config.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_actions.h"
#include "../h/sys_registry.h"
#include "../ts/modemcap_public.h"
#include "modemcap.h"

dt_type *modemcap;

static char *const default_modems[] = {
    "default:FD=&F:AA=S0=1:CD=&C1:DTR=&D2:NEC=E0:NRS=Q1:ONH=H0:OFH=H1:DEF=T",
    "codex_3260:HFL=*FL3:SPD=*SC1:BER=*SM3:BCP=*DC1:NER=*SM1:NCP=*DC0:CID=&S1:DEF=T:TPL=default",
    "usr_courier:HFL=&H1&R2:SPD=&B1:BER=&M4:BCP=&K1:NER=&M0:NCP=&K0:DEF=T:TPL=default",
    "usr_sportster:TPL=usr_courier:DEF=T",
    "hayes_optima:HFL=&K3:BER=&Q5:BCP=&Q9:NER=&Q0:NCP=&Q0:SPD=&Q6:DEF=T:TPL=default",
    "global_village:HFL=&K3\\X1:BCP=%C1:NCP=%C0:SPD=\\J0:DEF=T:TPL=default",
    "viva:HFL=&K3:BER=&Q5:BCP=%C1:NER=&Q6:NCP=%C0:SPD=\\N3:DEF=T:TPL=default",
    "telebit_t3000:HFL=S58=2:BER=S180=3:BCP=S190=1:NER=S180=0:NCP=S190=0:SPD=S51=7:DEF=T:TPL=default",
    "microcom_hdms:HFL=\\Q3:SPD=$B115200:DTE=115200:BER=\\N5:BCP=\\J0%C3:NER=\\N0:NCP=%C0:DEF=T:TPL=default",
    ""
};

void modemcap_init (void)
{
    char *const *ptr;

    /* Initialize data train */
    modemcap = dt_init();

    /* read the default modems into the modemcap database */
    for (ptr = default_modems; *(ptr[0]) != 0; ptr++) {
    	parse_modemcap_entry(*ptr);
    }
}

static void print_modemcap_attr (char *nameptr, char *attr, char *description) 
{
    char *attrptr;

    attrptr = modemcap_get_string(nameptr, attr, 0);
    if (!attrptr)
	attrptr = "[not set]";
    printf("%s (%s):  %s\n", description, attr, attrptr);
}

void show_modemcap (parseinfo *csb)
{
    char *current, *nameptr;
    
    nameptr = GETOBJ(string,1);

    /*  if GETOBJ(string,1) has no string, then display a list of
		modems */
    if (nameptr[0] == '\0') {
	for (current = dt_get_first_record(modemcap);
		current != NULL;
		current = dt_get_next_record(modemcap, current)) {
	    printf("%s\n", current);
    	}
	return;
    }

    if (!dt_record_exists(modemcap, nameptr)) {
	printf("There is no record of modem %s\n", nameptr);
	return;
    }

    printf("Modemcap values for %s\n", nameptr);

    print_modemcap_attr(nameptr, "FD", "Factory Defaults");
    print_modemcap_attr(nameptr, "AA", "Autoanswer");
    print_modemcap_attr(nameptr, "CD", "Carrier detect");
    print_modemcap_attr(nameptr, "DTR", "Drop with DTR");
    print_modemcap_attr(nameptr, "HFL", "Hardware Flowcontrol");
    print_modemcap_attr(nameptr, "SPD", "Lock DTE speed");
    print_modemcap_attr(nameptr, "DTE", "DTE locking speed");
    print_modemcap_attr(nameptr, "BER", "Best Error Control");
    print_modemcap_attr(nameptr, "BCP", "Best Compression");
    print_modemcap_attr(nameptr, "NER", "No Error Control");
    print_modemcap_attr(nameptr, "NCP", "No Compression");
    print_modemcap_attr(nameptr, "NEC", "No Echo");
    print_modemcap_attr(nameptr, "NRS", "No Result Codes");
    print_modemcap_attr(nameptr, "SFL", "Software Flowcontrol");
    print_modemcap_attr(nameptr, "CID", "Caller ID");
    print_modemcap_attr(nameptr, "ONH", "On-hook");
    print_modemcap_attr(nameptr, "OFH", "Off-hook");
    print_modemcap_attr(nameptr, "MSC", "Miscellaneous");
    print_modemcap_attr(nameptr, "TPL", "Template entry");

    if (dt_find_attr(modemcap, nameptr, MODEMCAP_DEFAULT)) {
	printf("Modem entry is built-in.\n");
    }

}


static int modemcap_type_to_token (char *type)
{
    if (!strncmp("FD", type, strlen("FD"))) {
        return(MODEMCAP_FACDEFAULTS);
    }
    if (!strncmp("AA", type, strlen("AA"))) {
        return(MODEMCAP_AUTOANSWER);
    }
    if (!strncmp("CD", type, strlen("CD"))) {
        return(MODEMCAP_CARDETECT);
    }
    if (!strncmp("DTR", type, strlen("DTR"))) {
        return(MODEMCAP_DTR);
    }
    if (!strncmp("HFL", type, strlen("HFL"))) {
        return(MODEMCAP_HARDFLOW);
    }
    if (!strncmp("SPD", type, strlen("SPD"))) {
        return(MODEMCAP_SPEED);
    }
    if (!strncmp("DTE", type, strlen("DTE"))) {
        return(MODEMCAP_DTESPEED);
    }
    if (!strncmp("BER", type, strlen("BER"))) {
        return(MODEMCAP_BESTERR);
    }
    if (!strncmp("BCP", type, strlen("BCP"))) {
        return(MODEMCAP_BESTCOMP);
    }
    if (!strncmp("NER", type, strlen("NER"))) {
        return(MODEMCAP_NOERR);
    }
    if (!strncmp("NCP", type, strlen("NCP"))) {
        return(MODEMCAP_NOCOMP);
    }
    if (!strncmp("NEC", type, strlen("NEC"))) {
        return(MODEMCAP_NOECHO);
    }
    if (!strncmp("NRS", type, strlen("NRS"))) {
        return(MODEMCAP_NORESULT);
    }
    if (!strncmp("SFL", type, strlen("SFL"))) {
        return(MODEMCAP_SOFTFLOW);
    }
    if (!strncmp("CID", type, strlen("CID"))) {
        return(MODEMCAP_CALLID);
    }
    if (!strncmp("ONH", type, strlen("ONH"))) {
        return(MODEMCAP_ONHOOK);
    }
    if (!strncmp("OFH", type, strlen("OFH"))) {
        return(MODEMCAP_OFFHOOK);
    }
    if (!strncmp("MSC", type, strlen("MSC"))) {
        return(MODEMCAP_MISC);
    }
    if (!strncmp("TPL", type, strlen("TPL"))) {
        return(MODEMCAP_TEMPLATE);
    }
    if (!strncmp("DEF", type, strlen("DEF"))) {
        return(MODEMCAP_DEFAULT);
    }
    return(MODEMCAP_ERR);
}

char *modemcap_get_string (char *modemname, char *attrtype, int recurse_level)
{
    int type;
    char *ptr, *template;

    if ((modemname == NULL) || (attrtype == NULL)) {
	return(NULL);
    }

    if (recurse_level > MODEMCAP_MAX_RECURSE) {
	return(NULL);
    }

    type = modemcap_type_to_token(attrtype);

    ptr = dt_find_attr(modemcap, modemname, type);

    if (ptr == NULL) {
	template = dt_find_attr(modemcap, modemname, MODEMCAP_TEMPLATE);
	ptr = modemcap_get_string(template, attrtype, recurse_level+1);
    }
    return(ptr);
}

boolean modemcap_record_exists (char *modemname)
{
    return(dt_record_exists(modemcap, modemname));
}

boolean parse_modemcap_entry (char *string)
{
    int count, state = MCAP_ATTR, attr_type = MODEMCAP_ERR;
    char *ptr, buf[MODEMCAP_MAXLEN];
    char modemname[MODEMCAP_MAXLEN], *nameptr;

    nameptr = modemname;

    ptr = string;
    while (*ptr) {
	/*
	 * A colon is the separator but it can be escaped with a preceeding
	 * backslash, skip over it in that case.
	 */
	if (*ptr == ':') {
	    break;
	} else if (*ptr == '\\' && *(ptr + 1) == ':') {
	    ptr++;
	}
	*nameptr++ = *ptr++;
	if (nameptr - modemname >= MODEMCAP_MAXLEN)
	     break;
    }

    *nameptr = 0;

    if (!dt_record_exists(modemcap, modemname)) {
        dt_add_record(modemcap, modemname);
    }

    count = 0;

    while (*++ptr != '\0') {
	if (count >= MODEMCAP_MAXLEN) {
	    break;
	}
	switch(state) {
	  case MCAP_ATTR:
	    if (*ptr == ':') {
		/* end of set */
	    } else if (*ptr == '=') {
		state = MCAP_VALUE;
		buf[count] = 0;
		count = 0;
		attr_type = modemcap_type_to_token(buf);
	    } else {
		buf[count++] = *ptr;
	    }
	    break;
	  case MCAP_VALUE:
	    if (*ptr == ':') {
		/* end of set */
		buf[count] = 0;
		dt_change_attr(modemcap, modemname, attr_type, buf);
		state = MCAP_ATTR;
		count = 0;
	    } else if (*ptr == 0) {
	       /* do nothing */
	    } else {
		/*
		 * Check for an escaped colon, skip the escape character
		 */
		if (*ptr == '\\' && *(ptr + 1) == ':') {
		    ptr++;
		}
		buf[count++] = *ptr;
	    }
	    break;
	  default:
	    break;
	}
    }
    if (state == MCAP_VALUE) {
	buf[count] = 0;
	dt_change_attr(modemcap, modemname, attr_type, buf);
    }
    return(TRUE);
}

/*
 * modemcap_find_attr
 *
 * Wrapper to extract the attribute and insert any escape characters. The
 * attribute is copied into the previously allocated buffer.
 */
static void modemcap_find_attr(char *buf, dt_type *modemcap, char *current,
				int attr)
{
    char *ptr;

    ptr = dt_find_attr(modemcap, current, attr);
    if (ptr) {
	while (*ptr) {
	    if (*ptr == ':') {
		*buf++ = '\\';
	    }
	    *buf++ = *ptr++;
	}
    }
    *buf = '\0';
}

void modemcap_command (parseinfo *csb)
{
    const char *no_modemcap_entry = "%% Modemcap entry '%s' does not exist";
    const char *default_modemcap  = "%% Default modemcap entries cannot be %s";
    char *current, *attr;

    if (csb->nvgen) {
        if (csb->which == PARSER_MODEMCAP_ENTRY) {
	    /*
	     * Allocate a buffer which can be used for any escape characters,
	     * size allows for the maximum number of characters each escaped.
	     */
	    attr = malloc(MODEMCAP_MAXLEN*2);
	    if (attr == NULL) {
		return;
	    }
	    for (current = dt_get_first_record(modemcap);
		    current != NULL;
		    current = dt_get_next_record(modemcap, current)) {

		/* if this is a default entry, skip it */
		if (NULL != dt_find_attr(modemcap, current, MODEMCAP_DEFAULT))
		    continue;

		nv_write(current != NULL, "modemcap entry %s",
				current);
		modemcap_find_attr(attr, modemcap, current,
		    MODEMCAP_FACDEFAULTS);
		nv_add(*attr != '\0', ":FD=%s", attr);
		modemcap_find_attr(attr, modemcap, current,
		    MODEMCAP_AUTOANSWER);
		nv_add(*attr != '\0', ":AA=%s", attr);
		modemcap_find_attr(attr, modemcap, current,
		    MODEMCAP_CARDETECT);
		nv_add(*attr != '\0', ":CD=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_DTR);
		nv_add(*attr != '\0', ":DTR=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_HARDFLOW);
		nv_add(*attr != '\0', ":HFL=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_SPEED);
		nv_add(*attr != '\0', ":SPD=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_DTESPEED);
		nv_add(*attr != '\0', ":DTE=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_BESTERR);
		nv_add(*attr != '\0', ":BER=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_BESTCOMP);
		nv_add(*attr != '\0', ":BCP=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_NOERR);
		nv_add(*attr != '\0', ":NER=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_NOCOMP);
		nv_add(*attr != '\0', ":NCP=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_NOECHO);
		nv_add(*attr != '\0', ":NEC=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_NORESULT);
		nv_add(*attr != '\0', ":NRS=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_SOFTFLOW);
		nv_add(*attr != '\0', ":SFL=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_CALLID);
		nv_add(*attr != '\0', ":CID=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_ONHOOK);
		nv_add(*attr != '\0', ":ONH=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_OFFHOOK);
		nv_add(*attr != '\0', ":OFH=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_MISC);
		nv_add(*attr != '\0', ":MSC=%s", attr);
		modemcap_find_attr(attr, modemcap, current, MODEMCAP_TEMPLATE);
		nv_add(*attr != '\0', ":TPL=%s", attr);
            }
	    free(attr);
            return;
        } else if (csb->which == PARSER_MODEMCAP_EDIT) {
            /* print nothing for this */
            return;
        } else {
            buginf("\n%% Bogus nvgen in modemcap_command, %d", csb->which);
        }
        return;
    }

    switch(csb->which) {
      case PARSER_MODEMCAP_EDIT:
	if (csb->sense) {
	    if (!dt_record_exists(modemcap, GETOBJ(string,1))) {
		dt_add_record(modemcap, GETOBJ(string,1));
	    }
	    if (dt_find_attr(modemcap, GETOBJ(string,1), MODEMCAP_DEFAULT)) {
		printf(default_modemcap, "edited");
		break;
	    }
	    dt_change_attr(modemcap, GETOBJ(string,1), GETOBJ(int,1),
			    GETOBJ(string,2));
	    if (GETOBJ(int,1) == MODEMCAP_SPEED) {
		char dte_speed[12]; /* Enough space for a ulong */

		sprintf(dte_speed, "%u", GETOBJ(int,2));
		dt_change_attr(modemcap, GETOBJ(string,1), MODEMCAP_DTESPEED,
				dte_speed);
	    }
	} else {
	    if (dt_record_exists(modemcap, GETOBJ(string,1))) {
		if (dt_find_attr(modemcap, GETOBJ(string,1),
				    MODEMCAP_DEFAULT)) {
		    printf(default_modemcap, "edited");
		} else if (dt_find_attr(modemcap, GETOBJ(string,1),
					GETOBJ(int,1)) == NULL) {
		    printf("%% Modemcap value cannot be edited due to "
			    "possible template inheritance");
		} else {
		    dt_change_attr(modemcap, GETOBJ(string,1),
				    GETOBJ(int,1), NULL);
		    if (GETOBJ(int,1) == MODEMCAP_SPEED) {
			dt_change_attr(modemcap, GETOBJ(string,1),
					MODEMCAP_DTESPEED, NULL);
		    }
		}
	    } else {
		printf(no_modemcap_entry, GETOBJ(string,1));
	    }
	}
	break;
      case PARSER_MODEMCAP_ENTRY:
	if (csb->sense) {
	    parse_modemcap_entry(GETOBJ(string,2));
	} else {
	    if (dt_record_exists(modemcap, GETOBJ(string,2))) {
		if (dt_find_attr(modemcap, GETOBJ(string,2),
		    MODEMCAP_DEFAULT)) {
		    printf(default_modemcap, "removed");
		} else {
		    dt_delete_record(modemcap, GETOBJ(string,2));
		}
	    } else {
		printf(no_modemcap_entry, GETOBJ(string,2));
	    }
	}
	break;
      default:
	buginf("\n%% bogus case %d in modemcap_command", csb->which);	
	break;
    }


}


/* ------------------ extras ---------------- */


/*  
 *  Data Train virtual data type
 *
 *    A very inefficient but compact method for storing groups of
 *      ASCII strings.
 *
 *    Warning -- this database is only useful for strings with
 *      all characters greater than ASCII 32.
 *
 *  Overview:
 *
 *    The users receives a pointer to a train.  Users can add an arbitrary
 *      number of records to the train (named by strings) and up to 
 *      250 attributes per record (named by predefined integers).
 *    
 *    Typical usage, modemcap:
 *
 *    dt_type *train;
 *
 *    train = dt_init();
 *    dt_add_record(train, "Telebit_3000")
 *    dt_add_attr(train, "Telebit_3000", FAC_DEFAULTS, "&F");
 *
 *    printf("Factory defaults string for Telebit_3000 is %s.\n",
 *	dt_find_attr(train, "Telebit_3000", FAC_DEFAULTS);
 *
 *  Data type:
 *
 *    dt_type;
 *
 *  Calls:
 *
 *    ---External---
 *    dt_init(void)
 *    dt_free(dt_type *)
 *    dt_add_record(dt_type *, char *recordname)
 *    dt_change_attr(dt_type *, char *recordname, int attribute, char *value)
 *    dt_find_attr(dt_type *, char *recordname, int attribute)
 *    dt_get_first_record(dt_type *)
 *    dt_get_next_record(dt_type *, char *recordname)
 *
 *    ---Internal---
 *    dt_add_attr(dt_type *, char *recordptr, int attribute, char *value)
 *    dt_delete_attr(dt_type *, char *recordptr, int attribute)
 *    dt_delete_record(dt_type *, char *recordname)
 *
 *  Internals:
 *
 *    Internally a train is one long array of records, which is reallocated 
 *      when its bounds are exceeded.
 *
 *    A record in the database looks like so.   Notice that all tokens
 *     are preceded by a NULL -- this null also serves as the end of 
 *     string for preceding strings.
 *
 *  	<NULL><NAME_TOKEN><STRING><NULL><ATTR_TOKEN><STRING>
 *					<NULL><ATTR_TOKEN><STRING>
 *						 . . . . . .
 *					<STRING><NULL><END_TOKEN>
 *
 *    The end of a record is signaled by the appearance of the NAME_TOKEN
 *      for the next record.
 *
 *    The very last record in the database is followed by an END token.
 *   
 */

/*
 *  dt_bcopy -- a bcopy that handles overlapping areas correctly
 */
static void dt_bcopy (char *first, char *second, int len)
{
    int count;

    if (second > first) {
	for (count = len-1; count >= 0; count--) {
	    second[count] = first[count];
        }
    } else {
	for (count = 0; count < len; count++) {
	    second[count] = first[count];
	}
    }
}


/*  
 *  Allocate an initialize a data train.  Returns a pointer to an 
 *    initialized data train.
 */
dt_type *dt_init (void)
{
    dt_type *train;

    train = malloc_named(sizeof(dt_type), "Data Train");
    if (!train) {
	return(NULL);
    }
    train->data_train = malloc_named(DATA_TRAIN_BASESIZE, "Data Train");
    memset(train->data_train, 0, DATA_TRAIN_BASESIZE);
    if (!train->data_train) {
	return(NULL);
    }
    train->dt_size = DATA_TRAIN_BASESIZE;
    train->data_train[0] = 0;
    train->data_train[1] = DATA_TRAIN_END;
    train->dt_curlen = 2;
    return(train);
}

/*
 *  Clean up after a data train.  Free all of the space we're using .
 */
void dt_free (dt_type *train)
{
    free(train->data_train);
    free(train);
}

/*
 *  Add a record to a data train 
 */
void dt_add_record (dt_type *train, char *string) 
{
    char *ptr;
    boolean next_is_token = FALSE;
    int count;

    if ((train == NULL) || (string == NULL))
	return;

    /* if we're out of space, realloc twice as much */
    while ((train->dt_curlen + strlen(string) +2) > train->dt_size) {
	train->data_train = realloc(train->data_train, train->dt_size *2);
        memset(train->data_train+train->dt_size, 0, train->dt_size);
	train->dt_size *= 2;
	if (train->data_train == NULL)  {
	   return;
	}
    }

    ptr = train->data_train;

    /*  find the end of the train, and add the record */
    for (count = 0; count < train->dt_size; count++) {
	if (next_is_token) {
	    if (*ptr == DATA_TRAIN_END) {
		*ptr++ = DATA_TRAIN_NAME;
		dt_bcopy(string, ptr, strlen(string));
		ptr += strlen(string);
		*ptr++ = '\0';
		*ptr = DATA_TRAIN_END;
		train->dt_curlen += strlen(string) +2;
		return;
	    }
	    next_is_token = FALSE;
	} else {
	    if (*ptr == '\0') {
		next_is_token = TRUE;
	    }
	}
	ptr++;
    } 
    /* shouldn't have gotten here */
    return;
}

/*
 *  Given the name of a record in a data train, return a pointer to 
 *    the front of the record 
 */
static char *dt_find_record (dt_type *train, char *name) {
    char *ptr;
    boolean armed = FALSE;
    int count;

    if ((train == NULL) || (name == NULL))
	return(NULL);

    ptr = train->data_train;

    for (count = 0; count < train->dt_size; count++) {
	if (armed) {
	    if (*ptr == DATA_TRAIN_END) {
		return(NULL);
	    }
	    if (*ptr == DATA_TRAIN_NAME) {
		if (!strcmp(ptr+1, name)) {
		    return(ptr-1);
		}
	    }
	    armed = FALSE;
	} else {
	    if (*ptr == 0) {
		armed = TRUE;
	    }
	}
	ptr++;
    }
    /* should never get this far */
    return(NULL);

}

void dt_delete_record (dt_type *train, char *string) 
{
    char *recordptr, *next;


    if ((train == NULL) || (string == NULL))
	return;

    recordptr = dt_find_record(train, string);
    if (recordptr == NULL)
	return;

    next = dt_get_next_record(train, string);


    if (next == NULL) {
	/* this is the last record */
	recordptr[1] = DATA_TRAIN_END;
	train->dt_curlen = &recordptr[1] - train->data_train + 1;
    } else {
        next -= 2;
	dt_bcopy(next, recordptr, (train->data_train+train->dt_curlen)-next);
	train->dt_curlen -= next-recordptr;
    }

}

/*
 *  Given a train, a recordname, and an attribute, return a pointer to
 *   the actual string.  This is the major way of retrieving data from
 *   a data train.
 */
char *dt_find_attr (dt_type *train, char *recordname, int attr)
{
    char *recordptr, *ptr;
    int count;
    boolean armed = FALSE;

    if ((train == NULL) || (recordname == NULL))
	return(NULL);

    recordptr = dt_find_record(train, recordname);

    /* skip the record name */
    ptr = recordptr + strlen(recordptr+2) +2;

    for (count = 0; count < train->dt_size; count++) {
	if (armed) {
	    if ((*ptr == DATA_TRAIN_END) || (*ptr == DATA_TRAIN_NAME)) {
		return(NULL);
	    }
	    if (*ptr == attr) {
		return(ptr+1);
	    }
	    armed = FALSE;
	} else {
	    if (*ptr == 0) {
		armed = TRUE;
	    }
	}
	ptr++;
    }
    /* Should never get this far */
    return(NULL);
}

/*
 *  Delete an attribute from a record
 */
static void dt_delete_attr (dt_type *train, char *recordptr, int attr) 
{
    char *ptr;
    boolean armed = FALSE;
    int attrlen, count;

    if ((train == NULL) || (recordptr == NULL))
	return;

    /* skip the record name */
    ptr = recordptr + strlen(recordptr+2) +2;

    for (count = 0; count < train->dt_size; count++) {
	if (armed) {
	    if ((*ptr == DATA_TRAIN_END) || (*ptr == DATA_TRAIN_NAME)) {
		return;
	    }
	    if (*ptr == attr) {
		 attrlen = strlen(ptr+1) +2;
		 train->dt_curlen -= attrlen;
		 dt_bcopy(ptr+attrlen, ptr, train->dt_curlen  
				- (ptr-train->data_train));
	    }
	    armed = FALSE;
	} else {
	    if (*ptr == 0) {
		armed = TRUE;
	    }
	}
	ptr++;
    }
    /* should never get this far */
}


/*
 *  Add an attribute to a record 
 *
 *  This should only be called from dt_change_attr()
 */
static void dt_add_attr (dt_type *train, char *recordptr, int attr, char *value) 
{
    char *ptr;

    if ((train == NULL) || (recordptr == NULL) || (value == NULL))
	return;

    ptr = recordptr + strlen(recordptr+2) + 2;

    /* move the array over to make room */
    dt_bcopy(ptr, ptr+strlen(value)+2, 
		train->dt_curlen - (ptr-train->data_train));
    train->dt_curlen += strlen(value) +2;

    ptr[0] = 0;
    ptr[1] = attr;
    /*  copy the value into the array */
    dt_bcopy(value, ptr+2, strlen(value));


}

/*
 * change an attribute (or delete it) given the train and the recordname
 *   this is the major way to alter data in a train 
 */
void dt_change_attr (dt_type *train, char *recordname, int attr, char *value)
{
    char *recordptr;

    if ((train == NULL) || (recordname == NULL))
	return;

    if (value) {
	while ((train->dt_curlen + strlen(value) +2) >= train->dt_size) {
	    train->data_train = realloc(train->data_train, train->dt_size *2);
	    memset(train->data_train+train->dt_size, 0, train->dt_size);
	    train->dt_size *= 2;
	    if (train->data_train == NULL)  {
	       return;
	    }
	}
    }

    recordptr = dt_find_record(train, recordname);

    dt_delete_attr(train, recordptr, attr);

    if (value)
        dt_add_attr(train, recordptr, attr, value);


}

char *dt_get_first_record (dt_type *train)
{
   if (train == NULL) 
      return(NULL);
   if (train->data_train[1] == DATA_TRAIN_END)
      return(NULL);
    else
      return(&train->data_train[2]);
}

/*
 *  dt_get_next_record
 *  	Return the name of the next record in the database 
 */
char *dt_get_next_record (dt_type *train, char *recordname) 
{
    char *recordptr, *ptr;
    boolean armed = FALSE;
    int count;

    if ((train == NULL) || (recordname == NULL))
	return(NULL);

    recordptr = dt_find_record(train, recordname);

    if (!recordptr)
       return(NULL);

    /* 
     * Start stepping from this record, if we hit an END, then return NULL,
     *	otherwise, return the name of the next record 
     *
     * Specifically, we step from 2 bytes into this record so we skip this
     *   record's DATA_TRAIN_NAME.
     */

    ptr = recordptr+2;
    for (count = 0; count < train->dt_size; count++) {
	if (armed) {
	    if (*ptr == DATA_TRAIN_END) 
		return(NULL);
	    if (*ptr == DATA_TRAIN_NAME)
		return(ptr+1);
	    armed = FALSE;
	} else {
	    if (*ptr == 0) {
		armed = TRUE;
	    }
	}
	ptr++;
    }
    /* should never get this far */
    return(NULL);
}

boolean dt_record_exists (dt_type *train, char *recordname) 
{
    return(NULL != dt_find_record(train, recordname));
}

void dt_dump_train (dt_type *train)
{
    int count;

    for (count = 0; count < train->dt_curlen; count++) {
	printf(" %d", train->data_train[count]);
    }
    printf("\n");
}
