/* $Id: name.c,v 3.3.28.3 1996/09/11 17:39:13 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/name.c,v $
 *------------------------------------------------------------------
 * name.c -- protocol independent host name and address lookup
 *
 * 8-December-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: name.c,v $
 * Revision 3.3.28.3  1996/09/11  17:39:13  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.3.28.2  1996/06/28  23:24:57  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/03/18  21:30:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  10:19:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/22  06:56:15  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:49:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:57:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "config.h"
#include "name.h"

#define BUBBLEDEPTH 10		/* used in optimizing linear searches */
#define PRUNETIMEOUT 50		/* prune cache every 50 lookups */
#define IDLEHOURS 72		/* let an entry age for three days */

/*
 * Local Storage.
 */
queuetype nameQ;		/* pointer to name cache queue */
queuetype delnameQ;		/* list of names to delete */
ulong prunetime;		/* for timing out cache entries */

/*
 * name_verify
 * Screen out malformed hostname strings.
 * Rejects null strings and strings with illegal chars.  Note that we don't
 * do any length checking since domain style names may be arbitrarily long.
 */

boolean name_verify (char *name)
{
    boolean good;
    char c;
    boolean non_numeric_seen = FALSE;

    if (null(name))
	return(BADCHARACTERS);		/* screen out null strings */
    while ((c = *name++) != '\0') {
	if (((c >= 'A') && (c <= 'Z')) ||
	    ((c >= 'a') && (c <= 'z')) ||
	    (c == '-') || (c == '_')) {
	    good = TRUE;
	    non_numeric_seen = TRUE;
	} else
	    good = ((c >= '0') && (c <= '9')) || (c == '.');
	if (!good)
	    return(BADCHARACTERS);	/* screen out bad characters */
    };

    if (!non_numeric_seen)	/* Check for strings with only numbers */
	return(BADCHARACTERS);	/* with possibles dots.  Not allowed! */

    return(OK);			/* string appears well formed */
}

/*
 * name_free
 * Flush an unlinked cache entry
 */

void name_free (nametype *ptr)
{
    if (ptr->sharecount > 0) {
	enqueue(&delnameQ, ptr);
	return;
    }
    free(ptr->loginstring);		/* release auto login string */
    free(ptr->busystring);		/* release busy message string */
    if (ptr->flags & NAM_PAD) {
	free(ptr->v.x121.facilities);    /* release possible facilities */
	free(ptr->v.x121.userdata);      /* ands user data */
    }
    free(ptr);				/* free main storage */
}

/*
 * name_update_entry
 * Update new entry with certain old entry information, discard old entry.
 */

void name_update_entry (nametype *old, nametype *newp)
{
    if (old)  {
	if (old->flags & NAM_IP)
	    newp->v.ip.port = old->v.ip.port;   /* remember port */
	newp->busystring = old->busystring;	/* remember busy string */
	newp->loginstring = old->loginstring;	/* remember login string */
        newp->resumestring = old->resumestring; /* remember resume string */
	old->busystring = NULL;
	old->loginstring = NULL;
        old->resumestring = NULL;
	newp->flags |= old->flags & NAM_RLOGIN; /* preserve old rloginness */
	name_free(old);
    } else
	newp->flags |= NAM_RLOGIN;	/* assume rlogin is possible */

}

/*
 * name_lookup
 * Given a host name, return pointer to a cache entry, or NULL if name
 * could not be looked up.  Possible numeric host specifications must
 * be handled elsewhere.
 *
 * The addition of the parameter *name_kind* is kind of a *kludge* --
 * when other (aside from IP) address types support a name lookup
 * function, the parameter can be removed. The caller will then have
 * handle the different styles of addresses in the name cache.
 */

nametype *name_lookup (char *name, int *errcode, int name_kind)
{
    nametype *ptr, *revalid;
    int status;

    revalid = NULL;			/* not revalidating at this point */
    name = deblank(name);		/* strip leading white space */
    status = name_verify(name);		/* verify correctness of form */
    *errcode = status;			/* give error code to user */
    if (status != OK)			/* if bad status, return NULL now */
	return(NULL);
    ptr = (nametype *) name_cache_lookup(name, NULL, (name_kind|NAM_UNKNOWN));
    if (ptr != NULL) {

	/*
	 * If this is a cached unknown entry, and the caller did not
	 * ask for cached unknown entries, then indicate failure to
	 * the caller.
	 */

	if ((ptr->flags & NAM_UNKNOWN) && !(name_kind & NAM_UNKNOWN))
	    return(NULL);

	if (!(ptr->flags & NAM_REVALIDATE)) {
	    return(ptr);		/* addresses thought to be okay */
	} else if (ptr->flags & NAM_USES_DOMAIN) { /* can we revalidate? */
	    revalid = ptr;		/* need revalidation. save ptr */
	    unqueue(&nameQ, ptr);	/* pretend we don't know it */
	}
    }

    if (!(name_kind & NAM_USES_DOMAIN))
	return(ptr);			/* exit if domain name not used */

    if (!(name_kind & NAM_ANY))		/* if no protos left, punt */
	return(ptr);

    if (reg_invoke_name_lookup_string(name, name_kind,
		      revalid, &ptr, errcode)) {
	return(ptr);
    }

    /*
     * If we are not looking for unknown entries, then cache the
     * the failure here.  Timeout after 2 minutes.
     */
    if (!(name_kind & NAM_UNKNOWN)) {
	ptr = malloc(sizeof(nametype) + strlen(name));
	if (ptr) {
	    GET_TIMESTAMP(ptr->lasttime);
	    TIMER_START(ptr->expiretime, NAM_UNK_CACHETIMEOUT);
	    ptr->flags = NAM_UNKNOWN | (name_kind & NAM_ANY);
	    strcpy(ptr->name, name);
	    ptr->namecount = 1;
	    enqueue(&nameQ, ptr);
	}
    }
    return(NULL);
}

/*
 * name_age_cache
 * Scan the host name/address cache, flushing non-permanent, unused
 * entries.  We also use this time to check for domain name entries that
 * have expired.
 */

static void name_age_cache (void)
{
    nametype *ptr, *next;

    ptr = (nametype *) nameQ.qhead;
    while (ptr != NULL) {
	next = ptr->next;
	if (!(ptr->flags & NAM_PERMANENT)) {
	    if ((ELAPSED_TIME(ptr->lasttime) > IDLEHOURS*ONEHOUR) ||
		(TIMER_RUNNING_AND_AWAKE(ptr->expiretime))) {

		if (ptr->loginstring || ptr->busystring) {
		    ptr->flags &= ~NAM_ADDRVALID;
		} else {
		    unqueue(&nameQ, ptr);
		    name_free(ptr);
		}
	    }
	}
    	ptr = next;
    }
}

/*
 * name_cache_lookup
 * Scan name cache data structure for the given host name or address.
 * Return pointer or NULL.
 */

nametype *name_cache_lookup (char *name, addrtype *address, int name_kind)
{
    nametype *ptr;
    int depth;
    int i;
    char *nameptr;
    
    /*
     * Every PRUNETIME invocations of this routine, we scan the cache list
     * and flush temporary entries that are not being used.  WE scan here
     * at the beginning of the routine to prevent finding an expired
     * timer, freeing the entry in the name_age_cache() routine, and then
     * returning a  pointer to the now non-existent entry.
     */
    if (--prunetime <= 0) {
	name_age_cache();
	prunetime = PRUNETIMEOUT;
    }
    
    depth = 0;
    for (ptr = (nametype *)nameQ.qhead; ptr; ptr = ptr->next) {
	depth++;

	/*
	 * If this entry is not for the selected type, skip it
	 */
	if (!(ptr->flags & (name_kind & NAM_ANY)))
	    continue;
	
	/*
	 * We were passed a name string.
	 * First apply the default, simple-minded string comparison.
	 * Then see if more sophisticated matches are called for. 
	 */
	if (name != NULL) {
	    nameptr = ptr->name;
            /*
             * namecount is a field of nametype to resolve CSCdi66910.
             * To not break the stability of 11.0 and 11.1,
             * the following intelligence is added.
             * The namecount field must always be greater than or
             * equal to one, if it is 0, it must be uninitialized,
             * so initialize it to one here".
             */
            if (ptr->namecount == 0)
                ptr->namecount = 1;

	    for (i = 0; i < ptr->namecount; i++) {
		if (!strcasecmp(nameptr, name))
		    break;
		if (reg_invoke_name_match_string(nameptr, name))
		    break;
		nameptr += strlen(nameptr) + 1;
	    }
	    if (i != ptr->namecount)
		break;
	    continue;
	}

	/*
	 * We were passed an address.
	 * Skip unknown cached names, then call the service point
	 * to do actual compare numeric address comparisons.
	 */
	if (address != NULL) {
	    if (ptr->flags & NAM_UNKNOWN)
		continue;
	    if (reg_invoke_name_match_number(address->type, ptr, address))
		break;
	}
    }
    
    /*
     * Done scanning list.  Found a match?
     */
    if (ptr) {
	GET_TIMESTAMP(ptr->lasttime);	/* update last reference */

	/*
	 * If we found an entry and if it was far down the list, move it
	 * to the head of the list to speed future lookups.
	 */

	if (depth > BUBBLEDEPTH) {
	    unqueue(&nameQ, ptr);	/* unlink entry */
	    requeue(&nameQ, ptr);	/* move to head of list */
	}

	/*
	 * If this is a cached failure, and the caller is not looking
	 * for cached failures, or if this entry has expired, then we
	 * should remove this entry from the list.
	 */

	if (ptr->flags & NAM_UNKNOWN) {
	    if (AWAKE(ptr->expiretime) || !(name_kind & NAM_UNKNOWN)) {
		unqueue(&nameQ, ptr); /* Remove from list */
		name_free(ptr);	/* Clean up data structures */
		return(NULL);	/* Return failure */
	    }
	    return(ptr);	/* Return the cached unknown failure */
	}

	/*
	 * Check the expiration time for this particular name to see if it
	 * has expired.  If so, set the revalidate flag to indicate that this
	 * name is now suspect.  Also set the revalidate flag if there
	 * are no valid addresses.  This may happen if an entry has a
	 * login string associated wih it, but the addresses have timed
	 * out.
	 */
	if ((TIMER_RUNNING_AND_AWAKE(ptr->expiretime)) ||
	    ((ptr->flags & NAM_ADDRVALID) == 0))
	    if (!(ptr->flags & NAM_PERMANENT))
		ptr->flags |= NAM_REVALIDATE;

    }

    return(ptr);
}

/*
 * name_periodic
 * Do periodic (every minute) name functions.  This includes
 * freeing any names that have been deleted, but not freed yet.
 */

static void name_periodic (void)
{
    nametype *ptr, *next;

    for (ptr = (nametype *) delnameQ.qhead; ptr; ptr = next) {
	next = ptr->next;
	if (ptr->sharecount <= 0) {
	    unqueue(&delnameQ, ptr);
	    name_free(ptr);
	}
    }
    
    /*
     * Flush the non-permanent, unused entries from the name/address cache.
     * Set the prunetime to PRUNETIMEOUT, so that cache could be aged either
     * in one minute or after PRUNETIMEOUT namelookup tries.
     */
    name_age_cache();
    prunetime = PRUNETIMEOUT;
}

/*
 * name_valid
 * Routine to set/clear the revalidation flag
 */

void name_valid (nametype *ptr, int sense)
{
    if (ptr->flags & NAM_PERMANENT)	/* always believe permanent entries */
	return;
    if (sense == TRUE)
	ptr->flags &= ~NAM_REVALIDATE;	/* it worked.  Entry is valid */
    else
	ptr->flags |= NAM_REVALIDATE;	/* failure.  Entry is suspect */
}

/*
 * name_show
 * Print out domain information and current hostname cache.
 */

#define HOSTLEN 25

/* CLNS entries need 2 lines for display */
#define CLNS_HOST_DISPLAY_LEN 2		

static const char hosthead[] =
    "Host                     Flags      Age Type   Address(es)";
static char *continue_text =
  "\n                                        ";

void name_show (parseinfo *csb)
{
    char namebuffer[HOSTLEN+1], *status;
    ulong time;
    nametype *ptr;
    boolean onehost;
    char *host;
    long flags;
    int i;
    char *nameptr;

    host = GETOBJ(string,1);
    onehost = !null(host);
    if (!onehost)
	reg_invoke_name_show_servers();

    /*
     * We are done displaying the initial header lines. Now disable and 
     * then re-enable window size recognition, but specifying that the
     * host info header be printed at the top of every page.
     */
    automore_disable();
    automore_enable(hosthead);

    /* 
     * Kludge the line count, so that the display of the host info
     * continues on the same page as the initial header lines (provided
     * there is enough space to do so).  This set of magic crocks makes
     * unsafe assumptions about the SERVICE_NAME_SHOW_SERVERS output.
     */
    if ((stdio->tty_length == 1) ||(stdio->tty_length == 2))  
        stdio->tty_length = 3;
    else if ((stdio->tty_length == 3) || (stdio->tty_length == 4))
        stdio->automore_count = 5 - stdio->tty_length;
    else if (stdio->tty_length > 4) 
	stdio->automore_count = 5;    

    ptr = (nametype *) nameQ.qhead;
    while (ptr != NULL && validmem(ptr)) {
	nameptr = ptr->name;
	if (onehost) {
            /*
             * namecount is a field of nametype to resolve CSCdi66910.
             * To not break the stability of 11.0 and 11.1,
             * the following intelligence is added.
             * The namecount field must always be greater than or
             * equal to one, if it is 0, it must be uninitialized,
             * so initialize it to one here".
             */
            if (ptr->namecount == 0)
                ptr->namecount = 1;

	    for (i = 0; i < ptr->namecount; i++) {
		if (!strcasecmp(nameptr, host) ||
		    reg_invoke_name_match_string(nameptr, host)) {
		    break;
		}
		nameptr += strlen(nameptr) + 1;
	    }
	    if (i == ptr->namecount) {
		ptr = ptr->next;
		continue;
	    }
	}
	ptr->sharecount++;

	/* 
	 * CLNS host info requires 2 lines to display; so make sure
	 * that both lines are displayed on the same page, if the terminal
	 * window size is large enough.
	 */
	if ((ptr->flags & NAM_CLNS) && 
	    (stdio->tty_length > CLNS_HOST_DISPLAY_LEN))
 	    automore_conditional(CLNS_HOST_DISPLAY_LEN + 1);
	
	sstrncpy(namebuffer, nameptr, HOSTLEN+1);
	time = ELAPSED_TIME(ptr->lasttime)/ONEHOUR;
	if (ptr->flags & NAM_UNKNOWN)
	    status = "UN";
	else if ((TIMER_RUNNING_AND_AWAKE(ptr->expiretime)) ||
	    !(ptr->flags & NAM_ADDRVALID))
	    status = "EX";
	else
	    status = (ptr->flags & NAM_REVALIDATE) ? "??" : "OK";
	printf("\n%*s(%s, %s) ", HOSTLEN, namebuffer,
	       (ptr->flags & NAM_PERMANENT) ? "perm" : "temp", status);
	if (time > 99)
	    printf("**");
	else
	    printf("%-2d", time);

	flags = (ptr->flags & NAM_ANY);
	reg_invoke_name_show_hosts(flags, ptr, continue_text);

	if (onehost) {
	    if (ptr->loginstring)
		printf("\n\nLogin-string=\"%s\"", ptr->loginstring);
	    if (ptr->busystring)
		printf("\n\nBusy-message=\"%s\"", ptr->busystring);
	} else if (ptr->namecount > 1) {
	    printf("\n");
	    for (i = 1; i < ptr->namecount; i++) {
		nameptr += strlen(nameptr) + 1;
		sstrncpy(namebuffer, nameptr, HOSTLEN+1);
		printf("  %*s", HOSTLEN, namebuffer);
	    }
	}
	ptr->sharecount--;
	ptr = ptr->next;
    }
    automore_disable();
}

/*
 * name_clear_host
 * The EXEC command to clear a host from the cache
 */

void name_clear_host (parseinfo *csb)
{
    name_delete(GETOBJ(string,1), NAM_ANY);
}

/*
 * name_delete
 * Remove a given hostname from the cache.
 * If the hostname is '*', flush the entire cache, but not permanent entries
 */

void name_delete (char *name, int name_kind)
{
    nametype *ptr, *next;
    int i;
    char *nameptr;
    
    if (!name) {
	printf("%% No hostname specified.  Use \"*\" to delete all names.");
	return;
    }
    name = deblank(name);
    ptr = (nametype *) nameQ.qhead; 
    if (*name == '*') {
	while (ptr != NULL) {
	    next = ptr->next;
	    if ((ptr->flags & NAM_PERMANENT) == 0) {
		unqueue(&nameQ, ptr);
		name_free(ptr);
	    }
	    ptr = next;
	}
    } else {
	while (ptr != NULL) {
	    next = ptr->next;
	    if (ptr->flags & name_kind) {
		nameptr = ptr->name;
                /*
                 * namecount is a field of nametype to resolve CSCdi66910.
                 * To not break the stability of 11.0 and 11.1,
                 * the following intelligence is added.
                 * The namecount field must always be greater than or
                 * equal to one, if it is 0, it must be uninitialized,
                 * so initialize it to one here".
                 */
                if (ptr->namecount == 0)
                    ptr->namecount = 1;

		for (i = 0; i < ptr->namecount; i++) {
		    if (!strcasecmp(nameptr, name) ||
			reg_invoke_name_match_string(nameptr, name)) {
                        unqueue(&nameQ, ptr);
                        name_free(ptr);
                        break;
		    }
		    nameptr += strlen(nameptr) + 1;
		}
	    }
	    ptr = next;
 	}
    }
}

/*
 * name_addr2numeric
 * Given an address type, return a numeric string.
 * Call this if you always want an IP address or a CLNS NSAP instead
 * of a user friendly name string.
 */

char *name_addr2numeric (addrtype *address)
{
    char *buffer = string_getnext();

    buffer[0] = '\0';
    reg_invoke_sprintf_address_addr(address->type, buffer, address);
    return(buffer);
}

/*
 * name_addr2string
 * Convert an addrtype into a hostname or numeric string.
 */

char *name_addr2string (addrtype *address)
{
    nametype *ptr;

    /*
     * Go do a name lookup.  We may use the cache or we may use
     * a name service of some sort (such as DNS)
     */
    ptr = (nametype *) reg_invoke_name_lookup_number(address->type, address);
    /*
     * If we have a user friendly name string, use it.
     * Otherwise return a numeric string
     */
    if (ptr)
	return(ptr->name);
    else
	return(name_addr2numeric(address));
}

/*
 * numeric_lookup
 * Given a string, return IP or NSAP host address if it was a numeric string.
 * Return TRUE, if a valid address found, otherwise return FALSE.
 */

boolean numeric_lookup (uchar addr_type, char *str, addrtype *address)
{
    return(reg_invoke_parse_address(addr_type, str, address));
}

/*
 * name_init
 * Initialize the system name cache data structures.
 */

void name_init (void)
{
    queue_init(&nameQ, 0);
    queue_init(&delnameQ, 0);
    prunetime = PRUNETIMEOUT;		/* prune cache this often */

    /*
     * Register some functions
     */
    reg_add_onemin(name_periodic, "name_periodic");
    reg_add_default_name_lookup_number(
	     (service_name_lookup_number_type )return_false, "return_false");
    reg_add_default_name_match_number(
	     (service_name_match_number_type ) return_false, "return_false");
}
