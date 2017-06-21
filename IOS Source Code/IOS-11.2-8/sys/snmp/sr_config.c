/* $Id: sr_config.c,v 3.2.60.5 1996/08/16 23:13:47 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_config.c,v $
 *------------------------------------------------------------------
 * sr_config.c: SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1994, Jeffrey T. Johnson
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * sr_config.c is a replacement for the SNMP Research startup
 * configuration files (readv2.c k_fileio.c).  A replacement is
 * needed since the SNMP Research code is designed for a one-time
 * configuration at startup (with subsequent configuration only
 * available via SNMP) whereas the Cisco agent is configurable at
 * any time via the CLI.  The SNMP Research agent also assumes that
 * some type of filesystem is available, but all we have is nvram.
 *
 *------------------------------------------------------------------
 * $Log: sr_config.c,v $
 * Revision 3.2.60.5  1996/08/16  23:13:47  jjohnson
 * CSCdi63694:  router crashes while configuring SNMP party
 * Make sure that configured context and party ids are unique
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/05/23  07:38:20  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.2.60.3  1996/05/22  13:52:35  jjohnson
 * CSCdi57672:  More than 512 interfaces: Maximum number of parties reached
 * Don't create default parties for secondary or unnumbered ip addresses
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/04/15  03:05:40  jjohnson
 * CSCdi53343:  Remove support for the SNMPv2 Simplified Security
 * Conventions
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  21:48:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  12:33:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  17:14:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:58:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/04  06:44:07  hampton
 * sched_private.h included too many places  [CSCdi38246]
 *
 * Revision 2.4  1995/07/18 02:08:28  anke
 * CSCdi37147:  partyMaxMessageSize returns out of range
 * changed SR_PROXY_PARTY_MESSAGE_SIZE to be the max possible value
 * according to partyMaxMessageSize object definition
 *
 * Revision 2.3  1995/07/01  00:38:18  jjohnson
 * CSCdi35423:  snmp-server packetsize command no longer works
 *
 * Revision 2.2  1995/06/20  07:14:02  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:48:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../util/md5.h"
#include "sr_snmpd.h"
#include "sr_config.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_defs_snmp.h"
#include "login.h"
#include "address.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "snmp_registry.h"
#include "bitlogic.h"
#include "../os/techsupport.h"

/*
 * this file only used for SNMPv2
 */
#ifdef  SR_SNMPv2


static char    *nullString = "";
static char    *restricted = "restricted";
static char    *everything = "everything";
#ifdef LIGHTOID
static char    *system_str = "1.3.6.1.2.1.1";
static char    *snmpStats = "1.3.6.1.6.3.1.1.1";
static char    *snmpParties = "1.3.6.1.6.3.3.2.1";
static char    *internet = "1.3.6.1";
static char    *initialPartyId = "1.3.6.1.6.3.3.1.3";
static char    *initialContextId = "1.3.6.1.6.3.3.1.4";
#else
static char    *system_str = "system";
static char    *snmpStats = "snmpStats";
static char    *snmpParties = "snmpParties";
static char    *internet = "internet";
static char    *initialPartyId = "initialPartyId";
static char    *initialContextId = "initialContextId";
#endif

/*
 * ciscoAdmin ::= { cisco 11 }
 * ciscoProxy ::= { ciscoAdmin 1 }
 * ciscoPartyProxy ::= { ciscoProxy 1 }
 * ciscoContextProxy ::= { ciscoProxy 2 }
 */
static char    *ciscoPartyProxy = "1.3.6.1.4.1.9.11.1.1";
static char    *ciscoContextProxy = "1.3.6.1.4.1.9.11.1.2";

static char     workbuffer[256];	/* buffer for string conversion */
static int	proxy_party_count;

#define MIN_PASSWORD_LEN  8

/*
 * the party max message size for v1 proxy parties should be larger
 * than the maximum allowable value for the snmp-server packetsize
 * command so that the snmp-server packetsize value will be used
 * According to the mib object partyMaxMessageSize the largest allowed
 * value is 65507
 */
#define SR_PROXY_PARTY_MESSAGE_SIZE	0xffe3

/* 
 * forward declaration of functions
 */
static char *community_contextname (char *viewname);

/*
 * MakeHexFromOctetString:
 * converts an octet string into a hex-ascii representation
 */

static void
MakeHexFromOctetString (OctetString *oct_ptr,
			char        *buf)
{
    long            idx;
    char            buffer[8];

    strcpy(buf, "");
    for (idx = 0; idx < oct_ptr->length; idx++) {
	sprintf(buffer, "%02x", oct_ptr->octet_ptr[idx]);
	strcat(buf, buffer);
    }
}


/*
 * MakeOctetStringFromHex
 * converts a text string containing hex-ascii into an octet string.  The
 * string may be prefixed with a "0x" but need not be.  Since it takes two
 * ascii characters to form one octet, the input string must be an even
 * length.
 */
OctetString *
MakeOctetStringFromHex (char *hex)
{
    int		oslen;
    int		cvt;
    char       *cptr;
    uchar      *osptr;

    /*
     * move the string into our work buffer, then convert it to upper case
     */
    strcpy(workbuffer, hex);
    uppercase(workbuffer);

    /*
     * ignore a "0x" prefix
     */
    if ((workbuffer[0] == '0') && (workbuffer[1] == 'X')) {
	cptr = &workbuffer[2];
    } else {
	cptr = workbuffer;
    }

    /*
     * since 2 characters form one octet, we can convert in place
     * also, since atohex will fail on a odd-length string, we don't
     * need to explicitly check for it.
     */
    osptr = (uchar *)workbuffer;
    oslen = 0;
    while (*cptr) {
	cvt = atohex(cptr);
	if (cvt == -1)
	    return(NULL);
	*osptr++ = (uchar)cvt;
	oslen++;
	cptr += 2;
    }

    /*
     * the characters have been converted into an octet string, so now
     * it's a simple task to create an OctetString structure
     */
    return(MakeOctetString(workbuffer, oslen));
}

void
InitParties (void)
{
    max_party_index = 1;
    v2party_head_ptr = NULL;
    v2party_tail_ptr = NULL;

    /*
     * since the SNMP Research code uses realloc to expand the
     * party index table, we must initially allocate an empty
     * record so that subsequent reallocs will succeed.  This
     * same logic applies to the acl, view, and context tables.
     */
    parties = malloc(sizeof(PARTY_TABLE));
    num_parties = 1;
}

void
InitAclEntries (void)
{
    v2acl_head_ptr = NULL;
    v2acl_tail_ptr = NULL;
    access_control = malloc(sizeof(V2_ACL));
    num_acls = 1;
}

void
InitViewEntries (void)
{
    v2view_head_ptr = NULL;
    v2view_tail_ptr = NULL;
    party_views = malloc(sizeof(V2_VIEW));
    num_views = 1;
}

void
InitContextEntries (void)
{
    max_context_index = 1;
    v2context_head_ptr = NULL;
    v2context_tail_ptr = NULL;
    contexts = malloc(sizeof(V2_CONTEXT_TABLE));
    num_contexts = 1;
}

int
WriteParties (void)
{
    /* return a good status */
    return (0);
}

int
WriteAclFile (void)
{
    /* return a good status */
    return (0);
}

int
WriteViewFile (void)
{
    /* return a good status */
    return (0);
}

int
WriteContextFile (void)
{
    /* return a good status */
    return (0);
}

static long
snmp_find_named_view (char *handle)
{
    long	idx;

    for (idx = 0; idx < num_views; idx++) {
	if (party_views[idx].mib_view == NULL) {
	    continue;
	}
	if (party_views[idx].mib_view->viewHandle == NULL) {
	    continue;
	}
	if (strcmp(handle, party_views[idx].mib_view->viewHandle) == 0) {
	    return(idx);
	}
    }
    return(-1);
}


static long
snmp_find_named_context (char *handle)
{
    long	idx;

    for (idx = 0; idx < num_contexts; idx++) {
	if (contexts[idx].context == NULL) {
	    continue;
	}
	if (contexts[idx].context->contextHandle == NULL) {
	    continue;
	}
	if (strcmp(handle, contexts[idx].context->contextHandle) == 0) {
	    return(idx);
	}
    }
    return(-1);
}


static long
snmp_find_named_party (char *handle)
{
    long	idx;

    for (idx = 0; idx < num_parties; idx++) {
	if (parties[idx].party == NULL) {
	    continue;
	}
	if (parties[idx].party->partyHandle == NULL) {
	    continue;
	}
	if (strcmp(handle, parties[idx].party->partyHandle) == 0) {
	    return(idx);
	}
    }
    return(-1);
}

/*
 * Check that there is no dangling contextViewIndex (in a context entry)
 * referencing a view that was deleted.
 */
static boolean 
snmp_view_referenced_by_context_entry (long viewidx)
{
    int idx;

    for (idx = 0; idx < num_contexts; idx++) {
        if (contexts[idx].context == NULL) {
            continue;
        }
        if (contexts[idx].context->contextViewIndex == viewidx) {
            return(TRUE);
        }
    }
    return(FALSE);
}

boolean
snmp_add_view (char    *name,
	       char    *subtree,
	       long     viewtype,
	       long	storagetype)
{
    MIB_VIEW    *viewptr;
    long	 viewidx;
    long	 idx;
    long	 contextidx;
    int		 oidstrlen;
    V2_VIEW     *newarray;
    MIB_VIEW    *newview;
    char	*stree;
    char	*contextname;
    int 	 bytes, bit;
    uchar	*mask;

    /*
     * assume we really will be creating a new view element
     */

    newview = malloc(sizeof(MIB_VIEW));
    if (newview == NULL) {
	printf(nomemory);
	return(FALSE);
    }

    /*
     * convert the subtree from text to OID
     */
    oidstrlen = strlen(subtree);
    stree = malloc(oidstrlen+1);
    if (stree == NULL) {
	printf(nomemory);
	return(FALSE);
    }
    for (idx = 0; idx <= oidstrlen; idx++)
	stree[idx] = (subtree[idx] == '*') ? '0' : subtree[idx];

    newview->viewSubtree = MakeOIDFromDot(stree);
    free(stree);

    if (newview->viewSubtree == NULL) {
	FreeViewElement(newview);
	printf("\n%% Illegal subtree oid: %s", subtree);
	return(FALSE);
    }

    bytes = ((newview->viewSubtree->length - 1)  >> 3) + 1;

    mask = malloc(bytes);
    if (mask == NULL) {
	FreeViewElement(newview);
	printf(nomemory);
	return(FALSE);
    }

    for (idx = 0; idx < bytes; idx++)
	mask[idx] = 0xff;

    bit = newview->viewSubtree->length - 1;
    for (idx = oidstrlen - 1; idx && bit; idx--) {
	if (subtree[idx] == '*')
	    CLEAR_LEFT_BIT(bit, mask); 
	if (subtree[idx] == '.')
	    bit--;
    }
    /*
     * eliminate any 0xff octets from the end of the mask
     */
    for (idx = bytes - 1; idx >= 0; idx--) {
	if (mask[idx] == 0xff)
	    bytes--;
	else
	    break;
    }
	    
    /*
     * convert the mask from text to OctetString
     */
    newview->viewMask = MakeOctetString(mask, bytes);
    if (newview->viewMask == NULL) {
	FreeViewElement(newview);
	printf("\n%% Illegal subtree mask: %s", mask);
	free(mask);
	return(FALSE);
    }
    free(mask);

    /*
     * see if a view with the given handle already exists
     */

    idx = snmp_find_named_view(name);
    if (idx == -1) {
	/*
	 * name not found
	 * need to determine the next available view number
	 * then add the name to the viewname Q
	 */
	viewidx = 1;
	for (idx = 0; idx < num_views; idx++) {
	    if (party_views[idx].mib_view != NULL) {
		if (viewidx <= party_views[idx].mib_view->viewIndex) {
		    viewidx = party_views[idx].mib_view->viewIndex + 1;
		}
	    }
	}

        /* 
         * ok, we've got the highest viewIndex that is free. 
         * However, a "free" viewidx may still be referenced by a
         * context entry (dangling viewIndex). Avoid dangling viewIndices
         */
        while (snmp_view_referenced_by_context_entry(viewidx)) {
            ++viewidx;
        }

	if (viewidx > SR_MAX_INDEX) {
	    /*
	     * yuck, there is an entry with an index of SR_MAX_INDEX.
	     * chances are the view table is sparse, so enter a tedious
	     * loop to find an unused index.  this may take a while :-(
	     */
	    for (viewidx = 1; viewidx <= SR_MAX_INDEX; viewidx++) {
		for (idx = 0; idx < num_views; idx++)
		    if ((party_views[idx].mib_view != NULL) &&
			(viewidx == party_views[idx].mib_view->viewIndex)) {
			break;
		    }
		if (idx == num_views) {
                    /* 
                     * make sure selected viewidx  is not referenced 
                     */
                    if (!snmp_view_referenced_by_context_entry(viewidx)) {
                        /* found an unused index.  yeah */
                        break;
                    }
		}
	    }
	}

	if (viewidx > SR_MAX_INDEX) {
	    /*
	     * there are no available view indexes.  This probably isn't
	     * possible, but who am I to argue?
	     */
	    printf("\n%% View table is full");
	    FreeViewElement(newview);
	    return(FALSE);
	}

    } else {

	/*
	 * check for a duplicate view table entry
	 * it is conceivable that a given view has multiple handles
	 * since each view entry has a handle.  thus it is possible
	 * that viewptr was already moved past a duplicate record
	 * in snmp_find_named_view, hence we'll restart the view
	 * match at the start of hte view list
	 */
	viewidx = party_views[idx].mib_view->viewIndex;
        for (idx = 0; idx < num_views; idx++) {
	    viewptr = party_views[idx].mib_view;
            if (viewptr == NULL) {
                continue;
            }
	    if ((viewptr->viewIndex == viewidx) &&
		(CmpOID(viewptr->viewSubtree, newview->viewSubtree) == 0)) {
		/*
		 * Duplicate view -- just replace it
		 */
		DeleteViewElement(viewptr);
		party_views[idx].mib_view = NULL;
		break;
	    }
	    if (viewptr->viewIndex > viewidx) {
		/* gone too far in sorted list, no duplicate */
		break;
	    }
	}
    }

    /*
     * fill the new view table entry
     */
    newview->viewIndex = viewidx;
    newview->viewStatus = ACTIVE;
    newview->viewType = viewtype;
    newview->viewStorageType = storagetype;
    setstring (&newview->viewHandle, name);

    /*
     * find a slot in the view array
     */
    for (idx = 0; idx < num_views; idx++) {
	if (party_views[idx].mib_view == NULL) {
	    break;
	}
    }

    if (idx == num_views) {
	/* array is full, so reallocate it */
	num_views++;

	newarray = realloc(party_views, (num_views * sizeof(V2_VIEW)));
	if (newarray == NULL) {
	    num_views--;
	    FreeViewElement(newview);
	    printf("\n%% Memory allocation failure");
	    return(FALSE);
	}

	party_views = newarray;
    }

    party_views[idx].mib_view = newview;
    party_views[idx].last_active = GetTimeNow() / 100;

    /*
     * insert the new view element into the lexigraphically sorted list
     */

    if (ViewInsertSort(newview) == -1) {
	printf("\n%% Unable to insert view element into linked list");
	FreeViewElement(newview);
	party_views[idx].mib_view = NULL;
	return(FALSE);
    }

    /*
     * check if a context exists for this view. An example of this could
     * be if a view with the same name was previously created
     * and then deleted, but an snmp community/context with this view
     * was not deleted.
     */
    contextname = community_contextname(name);
    if (contextname == NULL) {
        FreeViewElement(newview);
        party_views[idx].mib_view = NULL;
        printf("\n%% Memory allocation failure");
        return(FALSE);
    }

    contextidx = snmp_find_named_context(contextname);
    if (contextidx != -1) {
        /* context exists. Update its viewIndex */
        contexts[contextidx].context->contextViewIndex = viewidx;
    }

    free(contextname);

    return(TRUE);
}

/*
 * snmp_delete_view:
 * this function is complicated by the fact that a view may consist of
 * multiple view entries.  Furthermore, each entry in a given view may
 * have a different handle.  Hence, the first step is to locate a view
 * with the given handle in order to detemine the viewIndex.  Then we
 * must walk the list of views and delete the one with the correct
 * viewIndex that matches the subtree (or delete all the view entries
 * with the correct viewIndex if the subtree is null)
 */
static void
snmp_delete_view (char    *name,
		  char    *subtree)
{
    MIB_VIEW  *view;
    long       idx;
    long       viewIndex;
    OID       *oid = NULL;
    boolean    found = FALSE;

    idx = snmp_find_named_view(name);
    if (idx == -1) {
	printf("\n%% Unknown view: %s", name);
	return;
    }

    viewIndex = party_views[idx].mib_view->viewIndex;

    if ((subtree != NULL) && (strlen(subtree) > 0)) {
	oid = MakeOIDFromDot(subtree);
	if (oid == NULL) {
	    printf("\n%% Bad OID subtree: %s", subtree);
	    return;
	}
    }

    for (idx = 0; idx < num_views; idx++) {
	view = party_views[idx].mib_view;
	if (view == NULL) {
	    continue;
	}
	if ((view->viewIndex == viewIndex) &&
	    ((oid == NULL) ||
	     (CmpOID(view->viewSubtree, oid) == 0))) {
	    /*
	     * found one
	     */
	    found = TRUE;
	    DeleteViewElement(view);
	    party_views[idx].mib_view = NULL;
	    if (oid != NULL) {
		/*
		 * there can only be one exact match
		 */
		break;
	    }
	}
    }
    if (!found) {
	printf("\n%% No view %s with subtree %s", name, subtree);
    }

    return;
}

boolean
snmp_add_context (char    *contextname,
		  char    *contextoid,
		  char    *viewname,
		  long	   storagetype)
{

    CONTEXT          *cptr;
    long	      idx;
    long              contextidx = 0;
    long              viewidx;
    V2_CONTEXT_TABLE *newarray;
    CONTEXT          *newcontext;
    boolean	      newindex = TRUE;


    /*
     * make sure the view exists
     */
    idx = snmp_find_named_view(viewname);
    if (idx == -1) {
	printf("\n%% View %s does not exist", viewname);
	return(FALSE);
    }
    viewidx = party_views[idx].mib_view->viewIndex;

    /*
     * create a new context record
     */
    newcontext = malloc(sizeof(CONTEXT));
    if (newcontext == NULL) {
	printf(nomemory);
	return(FALSE);
    }

    /*
     * convert the context id from text to oid
     */
    newcontext->contextIdentity = MakeOIDFromDot(contextoid);
    if (newcontext->contextIdentity == NULL) {
	printf("\n%% Illegal context id: %s", contextoid);
	FreeContextElement(newcontext);
	return(FALSE);
    }

    /*
     * check for a pre-existing context name
     */
    idx = snmp_find_named_context(contextname);
    if (idx != -1) {
	cptr = contexts[idx].context;
	if (CmpOID(cptr->contextIdentity, newcontext->contextIdentity)) {
	    printf("\n%% A context named %s with a different OID already exists", contextname);
	    FreeContextElement(newcontext);
	    return(FALSE);
	}
	/*
	 * we'll be replacing the old context with the new one
	 */
	newindex = FALSE;
	contextidx = cptr->contextIndex;
	DeleteContextElement(cptr);
	contexts[idx].context = NULL;
    } else {
	/*
	 * see if the contextId already exists under a different name
	 */
	for (contextidx = 0; contextidx < num_contexts; contextidx++) {
	    if (contexts[contextidx].context == NULL) {
		continue;
	    }
	    if ((CmpOID(contexts[contextidx].context->contextIdentity,
			newcontext->contextIdentity) == 0)) {
		printf("\n%% The context %s already exists", contextoid);
		FreeContextElement(newcontext);
		return(FALSE);
	    }
	}

    }

    if (newindex) {
	/*
	 * pick an unused contextIndex.
	 */
	if (max_context_index <= SR_MAX_INDEX) {
	    contextidx = max_context_index++;
	} else {
	    /*
	     * yuck, there is an entry with an index of SR_MAX_INDEX.
	     * chances are the context table is sparse, so enter a tedious
	     * loop to find an unused index.  this may take a while :-(
	     */
	    for (contextidx = 1; contextidx <= SR_MAX_INDEX; contextidx++) {
		for (idx = 0; idx < num_contexts; idx++) {
		    if ((contexts[idx].context != NULL) &&
			(contextidx == contexts[idx].context->contextIndex)) {
			break;
		    }
		}
		if (idx == num_contexts) {
		    /* found an unused index.  yeah */
		    break;
		}
	    }
	}

	if (contextidx > SR_MAX_INDEX) {
	    /*
	     * there are no available context indexes.  This probably isn't
	     * possible, but who am I to argue?
	     */
	    printf("\n%% Context table is full");
	    FreeContextElement(newcontext);
	    return(FALSE);
	}
    }

    newcontext->contextIndex = contextidx;

    /*
     * fill up the rest of the context record
     */
    newcontext->contextProxyDstParty = CloneOID(&nullOID);
    newcontext->contextProxySrcParty = CloneOID(&nullOID);
    newcontext->contextProxyContext = CloneOID(&nullOID);
    newcontext->contextLocalEntity = MakeOctetStringFromText(nullString);
    newcontext->contextLocal = SR_LOCAL;
    newcontext->contextViewIndex = viewidx;
    newcontext->contextLocalTime = C_CURRENT_TIME;
    newcontext->contextStorageType = storagetype;
    newcontext->contextStatus = ACTIVE;
    setstring(&newcontext->contextHandle, contextname);

    /*
     * make sure all the fields were allocated
     */
    if ((newcontext->contextIdentity == NULL) ||
	(newcontext->contextProxyDstParty == NULL) ||
	(newcontext->contextProxySrcParty == NULL) ||
	(newcontext->contextProxyContext == NULL) ||
	(newcontext->contextLocalEntity == NULL)) {
	FreeContextElement(newcontext);
	printf("\n%% Memory allocation failure");
	return(FALSE);
    }

    /*
     * find a slot in the context array
     */
    for (idx = 0; idx < num_contexts; idx++) {
	if (contexts[idx].context == NULL) {
	    break;
	}
    }

    if (idx == num_contexts) {
	/* array is full, so reallocate it */
	num_contexts++;

	newarray = realloc(contexts, (num_contexts * sizeof(V2_CONTEXT_TABLE)));
	if (newarray == NULL) {
	    num_contexts--;
	    FreeContextElement(newcontext);
	    printf("\n%% Memory allocation failure");
	    return(FALSE);
	}

	contexts = newarray;
    }

    contexts[idx].context = newcontext;
    contexts[idx].last_active = GetTimeNow() / 100;

    /*
     * insert the new context into the lexigraphically sorted list
     */

    ContextInsertSort(newcontext);

    return(TRUE);
}

static void
snmp_delete_context (char *contextname)
{
    long	idx;

    idx = snmp_find_named_context(contextname);
    if (idx != -1) {
	DeleteContextElement(contexts[idx].context);
	contexts[idx].context = NULL;
    } else {
	printf("\n%% Unknown context: %s", contextname);
    }
    return;
}

boolean
snmp_add_party (char           *partyname,
		char           *partyoid,
		int             partydomain,
		unsigned long   partyaddress,
		unsigned short  partyport,
		int		partyaccesslist,
		long		partymessagesize,
		int		partyauthtype,
		OctetString    *partyauthkey,
		long		partyauthclock,
		long		partyauthlifetime,
		int	        partyprivtype,
		OctetString    *partyprivkey,
		long            local,
		long            storagetype)
{

    PARTY            *pptr;
    unsigned char    *optr;
    long	      idx;
    long              partyidx = 0;
    boolean           newindex = TRUE;
    PARTY_TABLE      *newarray;
    PARTY            *newparty;

    /*
     * create a new party record
     */
    newparty = malloc(sizeof(PARTY));
    if (newparty == NULL) {
	printf(nomemory);
	return(FALSE);
    }

    /*
     * convert the party id from text to oid
     */
    newparty->partyIdentity = MakeOIDFromDot(partyoid);
    if (newparty->partyIdentity == NULL) {
	printf("\n%% Illegal party id: %s", partyoid);
	FreePartyElement(newparty);
	return(FALSE);
    }

    /*
     * check for a pre-existing party name
     */
    idx = snmp_find_named_party(partyname);
    if (idx != -1) {
	pptr = parties[idx].party;
	if (CmpOID(pptr->partyIdentity, newparty->partyIdentity)) {
	    printf("\n%% A party named %s with a different OID already exists",
		   partyname);
	    FreePartyElement(newparty);
	    return(FALSE);
	}
	/*
	 * we'll replace the existing party with a new one
	 */
	newindex = FALSE;
	partyidx = pptr->partyIndex;
	DeletePartyElement(pptr);
	parties[idx].party = NULL;
    } else {
	/*
	 * see if the partyId already exists under a different name
	 */
	for (partyidx = 0; partyidx < num_parties; partyidx++) {
	    if (parties[partyidx].party == NULL) {
		continue;
	    }
	    if ((CmpOID(parties[partyidx].party->partyIdentity,
			newparty->partyIdentity) == 0)) {
	    printf("\n%% The party %s already exists",
		   partyoid);
	    FreePartyElement(newparty);
	    return(FALSE);
	    }
	}
    }

    if (newindex) {
	/*
	 * pick an unused partyIndex.
	 */
	if (max_party_index <= SR_MAX_INDEX) {
	    partyidx = max_party_index++;
	} else {
	    /*
	     * yuck, there is an entry with an index of SR_MAX_INDEX.
	     * chances are the party table is sparse, so enter a tedious
	     * loop to find an unused index.  this may take a while :-(
	     */
	    for (partyidx = 1; partyidx <= SR_MAX_INDEX; partyidx++) {
		for (idx = 0; idx < num_parties; idx++) {
		    if ((parties[idx].party != NULL) &&
			(partyidx == parties[idx].party->partyIndex)) {
			break;
		    }
		}
		if (idx == num_parties) {
		    /* found an unused index.  yeah */
		    break;
		}
	    }
	}

	if (partyidx > SR_MAX_INDEX) {
	    /*
	     * there are no available party indexes.  This probably isn't
	     * possible, but who am I to argue?
	     */
	    printf("\n%% Party table is full");
	    FreePartyElement(newparty);
	    return(FALSE);
	}
    }

    newparty->partyIndex = partyidx;

    /*
     * fill up the rest of the party record
     */
    if (partydomain == RFC_1157_DOMAIN) {
	newparty->partyAuthProtocol = TRIVIAL_AUTHENTICATION;
	newparty->partyPrivProtocol = ABSENT;
    } else {
#ifndef SR_UNSECURABLE
	newparty->partyAuthProtocol = partyauthtype;
	if ((partyauthtype == MD5_SNMPv2_AUTHENTICATION) &&
	    (partyauthkey->length != KEY_LENGTH)) {
	    printf("\n%% Invalid authentication key");
	    FreePartyElement(newparty);
	    return(FALSE);
	}

#else                           /* SR_UNSECURABLE */
	newparty->partyAuthProtocol = NOAUTH_AUTHENTICATION;
#endif                          /* SR_UNSECURABLE */

#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
	newparty->partyPrivProtocol = partyprivtype;
	if ((partyprivtype == DESPRIV) &&
	    (partyprivkey->length != KEY_LENGTH)) {
	    printf("\n%% Invalid privacy key");
	    FreePartyElement(newparty);
	    return(FALSE);
	}
#else                           /* SR_NO_PRIVACY */
	newparty->partyPrivProtocol = NOPRIV;
#endif                          /* SR_NO_PRIVACY */
#else                           /* SR_UNSECURABLE */
	newparty->partyPrivProtocol = NOPRIV;
#endif                          /* SR_UNSECURABLE */

    }

    newparty->privs = 0;	/* obsolete field */
    newparty->domain = SNMPv2_DOMAIN;
    newparty->partyTDomain = partydomain;
    newparty->partyTPort = partyport;
    newparty->access_list = partyaccesslist;
    newparty->partyMaxMessageSize = partymessagesize;
    newparty->partyAuthClock = partyauthclock;
    newparty->partyAuthPublic = MakeOctetString(nullString, 0);
    newparty->partyAuthPrivate = CloneOctetString(partyauthkey);
    newparty->partyAuthLifetime = partyauthlifetime;
    newparty->partyPrivPublic = MakeOctetString(nullString, 0);
    newparty->partyPrivPrivate = CloneOctetString(partyprivkey);
    newparty->partyDiscriminator = 0;
    newparty->snmpTrapNumbers = 0;
    newparty->partyCloneFrom = -1;

    newparty->partyLocal = local;
    newparty->partyStorageType = storagetype;
    newparty->partyStatus = ACTIVE;
    setstring(&newparty->partyHandle, partyname);

#ifndef SR_SNMPv1
    if (newparty->partyTDomain == RFC_1157_DOMAIN) {
	/*
	 * Do not want parties that represent community strings
	 * being included in the party table.  Setting these
	 * entries to invalid allows one configuration file to be
	 * used.
	 */
	newparty->partyStatus = DESTROY;
    }
#endif                          /* SR_SNMPv1 */

    newparty->partyTAddress = malloc(sizeof(OctetString));
    if (newparty->partyTAddress) {
	newparty->partyTAddress->octet_ptr = malloc(TADDR_LEN);
	if (newparty->partyTAddress->octet_ptr) {
	    newparty->partyTAddress->length = TADDR_LEN;
	    optr = &newparty->partyTAddress->octet_ptr[TADDR_LEN-1];
	    *optr-- = (partyport >> 0) & 0xff;
	    *optr-- = (partyport >> 8) & 0xff;
	    *optr-- = (partyaddress >>  0) & 0xff;
	    *optr-- = (partyaddress >>  8) & 0xff;
	    *optr-- = (partyaddress >> 16) & 0xff;
	    *optr-- = (partyaddress >> 24) & 0xff;
	}
    }

    /*
     * make sure all the fields were allocated
     */
    if ((newparty->partyIdentity == NULL) ||
	(newparty->partyTAddress == NULL) ||
	(newparty->partyAuthPublic == NULL) ||
	(newparty->partyAuthPrivate == NULL) ||
	(newparty->partyPrivPublic == NULL) ||
	(newparty->partyPrivPrivate == NULL)) {
	FreePartyElement(newparty);
	printf("\n%% Memory allocation failure");
	return(FALSE);
    }

    /*
     * find a slot in the party array
     */
    for (idx = 0; idx < num_parties; idx++) {
	if (parties[idx].party == NULL) {
	    break;
	}
    }

    if (idx == num_parties) {
	/* array is full, reallocate unless we've reached the max number*/
	num_parties++;
	newarray = realloc(parties, (num_parties * sizeof(PARTY_TABLE)));
	if (newarray == NULL) {
	    num_parties--;
	    FreePartyElement(newparty);
	    printf("\n%% Memory allocation failure");
	    return(FALSE);
	}

	parties = newarray;
    }

    parties[idx].party = newparty;
    parties[idx].last_active = GetTimeNow() / 100;

    /*
     * insert the new party into the lexigraphically sorted list
     */

    PartyInsertSort(newparty);

    return(TRUE);

}

static void
snmp_delete_party (char *partyname)
{
    long	idx;

    idx = snmp_find_named_party(partyname);
    if (idx != -1) {
	DeletePartyElement(parties[idx].party);
	parties[idx].party = NULL;
    } else {
	printf("\n%% Unknown party: %s", partyname);
    }
    return;
}

boolean
snmp_add_acl (char          *targetname,
	      char          *subjectname,
	      char          *resourcename,
	      long           privileges,
	      long           storagetype)
{
    PARTY      *tpptr;
    PARTY      *spptr;
    CONTEXT    *rcptr;
    ACCESS     *aptr;
    long	idx;
    V2_ACL     *newarray;
    long        targetidx;
    long        subjectidx;
    long        resourceidx;
    boolean     newentry;

    /*
     * translate the 2 party names
     */
    idx = snmp_find_named_party(targetname);
    if (idx == -1) {
	printf("\n%% Unknown target party name: %s", targetname);
	return(FALSE);
    }
    tpptr = parties[idx].party;
    targetidx = tpptr->partyIndex;

    idx = snmp_find_named_party(subjectname);
    if (idx == -1) {
	printf("\n%% Unknown subject party name: %s", subjectname);
	return(FALSE);
    }
    spptr = parties[idx].party;
    subjectidx = spptr->partyIndex;

    /*
     * translate the context name
     */
    idx = snmp_find_named_context(resourcename);
    if (idx == -1) {
	printf("\n%% Unknown resource context name: %s", resourcename);
	return(FALSE);
    }
    rcptr = contexts[idx].context;
    resourceidx = rcptr->contextIndex;

    /*
     * check for a duplicate acl
     */
    newentry = TRUE;
    for (aptr = v2acl_head_ptr; aptr; aptr = aptr->next_ptr) {
	if ((aptr->aclTarget == targetidx) &&
	    (aptr->aclSubject == subjectidx) &&
	    (aptr->aclResources == resourceidx)) {
	    newentry = FALSE;
	    break;
	}
    }

    if (newentry) {
	/*
	 * create a new acl record
	 */
	aptr = malloc(sizeof(ACCESS));
	if (aptr == NULL) {
	    printf(nomemory);
	    return(FALSE);
	}

	/*
	 * find a slot in the acl array
	 */
	for (idx = 0; idx < num_acls; idx++) {
	    if (access_control[idx].acl == NULL) {
		break;
	    }
	}

	if (idx == num_acls) {
	    /* array is full, reallocate */
	    num_acls++;
	    newarray = realloc(access_control, (num_acls * sizeof(V2_ACL)));
	    if (newarray == NULL) {
		num_acls--;
		FreeAclElement(aptr);
		return(FALSE);
	    }

	    access_control = newarray;
	}

	aptr->aclTarget = targetidx;
	aptr->aclSubject = subjectidx;
	aptr->aclResources = resourceidx;

    } else {
	
	/*
	 * find the slot in the acl array
	 */
	for (idx = 0; idx < num_acls; idx++) {
	    if (access_control[idx].acl == aptr) {
		break;
	    }
	}

	if (idx == num_acls) {
	    /*
	     * !!! we have an acl that isn't in the index table !!!
	     * this is an "impossible" situation
	     */
	    printf("\n%% Internal snmp error");
	    return(FALSE);
	}
    }

    aptr->aclPrivileges = privileges;
    aptr->aclStorageType = storagetype;
    aptr->aclStatus = ACTIVE;

    if ((spptr->partyStatus == DESTROY) ||
	(tpptr->partyStatus == DESTROY) ||
	(rcptr->contextStatus == DESTROY)) {
	aptr->aclStatus = DESTROY;
    }

    access_control[idx].acl = aptr;
    access_control[idx].last_active = GetTimeNow() / 100;

    /*
     * insert the new acl into the lexigraphically sorted list
     */
    if (newentry) {
	AclInsertSort(aptr);
    }

    return(TRUE);

}

static void
snmp_delete_acl (char          *targetname,
		 char          *subjectname,
		 char          *resourcename)
{
    ACCESS     *aptr;
    long	idx;
    long        targetidx;
    long        subjectidx;
    long        resourceidx;

    /*
     * translate the 2 party names
     */
    idx = snmp_find_named_party(targetname);
    if (idx == -1) {
	printf("\n%% Unknown target party name: %s", targetname);
	return;
    }
    targetidx = parties[idx].party->partyIndex;

    idx = snmp_find_named_party(subjectname);
    if (idx == -1) {
	printf("\n%% Unknown subject party name: %s", subjectname);
	return;
    }
    subjectidx = parties[idx].party->partyIndex;

    /*
     * translate the context name
     */
    idx = snmp_find_named_context(resourcename);
    if (idx == -1) {
	printf("\n%% Unknown resource context name: %s", resourcename);
	return;
    }
    resourceidx = contexts[idx].context->contextIndex;

    /*
     * find the acl
     */
    for (idx = 0; idx < num_acls; idx++) {
	aptr = access_control[idx].acl;
	if (aptr == NULL) {
	    continue;
	}
	if ((aptr->aclTarget == targetidx) &&
	    (aptr->aclSubject == subjectidx) &&
	    (aptr->aclResources == resourceidx)) {
	    DeleteAclElement(aptr);
	    access_control[idx].acl = NULL;
	    break;
	}
    }
    return;
}

static char *
proxy_partyoid (void)
{
    char	buffer[255];
    char       *oid = NULL;

    sprintf(buffer, "%s.%u", ciscoPartyProxy, proxy_party_count++);
    setstring(&oid, buffer);
    return (oid);
}

static char *
community_partyname (char *community)
{
    char	buffer[255];
    char       *handle = NULL;

    sprintf(buffer, "*cp.%s", community);
    setstring(&handle, buffer);
    return (handle);
}

static char *
proxy_contextoid (char *viewname)
{
    char	buffer[255];
    char       *handle = NULL;
    int		len;
    int		idx;

    sprintf(buffer, "%s.0", ciscoContextProxy);
    len = strlen(viewname);
    for (idx = 0; idx < len; idx++)
	sprintf(&buffer[strlen(buffer)], ".%d", (ulong)viewname[idx]);
    setstring(&handle, buffer);
    return (handle);
}

static char *
community_contextname (char *viewname)
{
    char	buffer[255];
    char       *handle = NULL;

    sprintf(buffer, "*cc.%s", viewname);
    setstring(&handle, buffer);
    return (handle);
}

static char *
traphost_partyname (ipaddrtype	address,
		    char       *community)
{
    char	buffer[255];
    char       *handle = NULL;

    /*
     * do not change the traphost party handle prefix "*tp."
     * without also changing the traphost party check hack
     * in sr_mgmt.c: findContextInfo()
     */
    sprintf(buffer, "*tp.%i.%s", address, community);
    setstring(&handle, buffer);
    return (handle);
}

static char *
traphost_contextname (ulong flags)
{
    char	buffer[255];
    char       *handle = NULL;

    sprintf(buffer, "*tc.%08x", flags);
    setstring(&handle, buffer);
    return (handle);
}

boolean
snmp_add_community_proxy (char    *community,
			  char	  *viewname,
			  int      accesslist,
			  long     privileges)
{
    char	*partyname;
    char	*partyoid;
    char	*contextname;
    char	*contextoid;
    OctetString *authkey;
    OctetString *privkey;
    boolean      result = 0;

    /*
     * verify that the view exists
     */
    if (snmp_find_named_view(viewname) == -1) {
	printf("\n%% Unknown view name: %s", viewname);
	return FALSE;
    }

    /*
     * create a handle and oid for the party
     */
    partyname = community_partyname(community);
    partyoid = proxy_partyoid();

    /*
     * create a handle and oid for the context
     */
    contextname = community_contextname(viewname);
    contextoid = proxy_contextoid(viewname);

    authkey = MakeOctetStringFromText(community);
    privkey = MakeOctetString(nullString, 0);

    result = snmp_add_party(partyname, partyoid,
			    RFC_1157_DOMAIN, 0xffffffff, 161, accesslist,
			    SR_PROXY_PARTY_MESSAGE_SIZE,
			    TRIVIAL_AUTHENTICATION, authkey,
			    0, SR_DEF_PARTY_LIFETIME,
			    ABSENT, privkey, SR_LOCAL, NONVOLATILE);

    if (result) {
	/*
	 * see if a context has already been created for the view
	 * if not, create it
	 */
	if (snmp_find_named_context(contextname) == -1) {
	    result = snmp_add_context(contextname, contextoid, viewname,
				      NONVOLATILE);
	}
    }
	
    if (result) {
	result = snmp_add_acl(partyname, partyname, contextname, privileges,
			      NONVOLATILE);
    }

    FreeOctetString(authkey);
    FreeOctetString(privkey);
    free(partyname);
    free(partyoid);
    free(contextname);
    free(contextoid);

    return(result);
}

void
snmp_delete_community_proxy (char    *community,
			     char    *viewname)
{
    char *partyname;
    char *contextname;

    /*
     * create a handle for the party
     */
    partyname = community_partyname(community);
    contextname = community_contextname(viewname);
    snmp_delete_acl(partyname, partyname, contextname);
    snmp_delete_party(partyname);
    free(partyname);
    free(contextname);
}

boolean
snmp_add_trap_proxy (ipaddrtype	address,
		     char      *community,
		     ulong      flags)
{
    char	*partyhandle = NULL;
    char	*partyoid = NULL;
    char	*ctxhandle = NULL;
    OctetString *authkey;
    OctetString *privkey;
    boolean      result = 0;
    traptype	*tp;

    /*
     * create a party handle
     */
    partyhandle = traphost_partyname(address, community);

    /*
     * create a view/context handle
     */
    ctxhandle = traphost_contextname(flags);

    /*
     * create a party id
     */
    partyoid = proxy_partyoid();

    authkey = MakeOctetStringFromText(community);
    privkey = MakeOctetString(nullString, 0);

    result = snmp_add_party(partyhandle, partyoid,
			    RFC_1157_DOMAIN, address, 162, 0, snmp_size,
			    TRIVIAL_AUTHENTICATION, authkey,
			    0, SR_DEF_PARTY_LIFETIME,
			    ABSENT, privkey, SR_LOCAL, NONVOLATILE);

    if (result) {
	/*
	 * see if a view matching the flags already exists
	 * (the associated context and view use the same handle)
	 */
	if (snmp_find_named_view(ctxhandle) == -1) {
	    /*
	     * create the view.  start by including everything, and then
	     * eliminate all traps that are not allowed per the flagword
	     */
	    snmp_add_view(ctxhandle, internet, INCLUDED, NONVOLATILE);
	    for (tp = (traptype *)trap_registrationQ.qhead; tp; tp = tp->next) {
		if (((1 << tp->trap_keyword_id) & flags) == 0)
		    snmp_add_view(ctxhandle, tp->trap_oid_str, EXCLUDED, NONVOLATILE);
	    }
	}

	/*
	 * see if a context matching the flags already exists
	 */
	if (snmp_find_named_context(ctxhandle) == -1) {
	    /*
	     * create a context id by appending the flagword to
	     * the ciscoContextProxy.
	     */
	    sprintf(workbuffer, "%s.%lu", ciscoContextProxy, flags);

	    /*
	     * create the context
	     */
	    snmp_add_context(ctxhandle, workbuffer, ctxhandle, NONVOLATILE);
	}
	result = snmp_add_acl(partyhandle, partyhandle, ctxhandle, TRAP_MASK,
			      NONVOLATILE);
    }

    FreeOctetString(authkey);
    FreeOctetString(privkey);
    free(ctxhandle);
    free(partyhandle);
    free(partyoid);

    return(result);
}

void
snmp_delete_trap_proxy (ipaddrtype	address,
			char	       *community,
			ulong		flags)
{
    char	*partyhandle = NULL;
    char	*ctxhandle = NULL;

    /*
     * create party and context handles
     */
    partyhandle = traphost_partyname(address, community);
    ctxhandle = traphost_contextname(flags);

    snmp_delete_acl(partyhandle, partyhandle, ctxhandle);
    snmp_delete_party(partyhandle);

    free(ctxhandle);
    free(partyhandle);
}

static void
snmp_name_view (MIB_VIEW *view)
{
    char workbuffer[20];

    if (view->viewHandle == NULL) {
	sprintf(workbuffer, "View%04x", view->viewIndex);
	setstring(&view->viewHandle, workbuffer);
    }
}

static void
snmp_name_context (CONTEXT *context)
{
    char workbuffer[20];

    if (context->contextHandle == NULL) {
	sprintf(workbuffer, "Context%04x", context->contextIndex);
	setstring(&context->contextHandle, workbuffer);
    }
}

static void
snmp_name_party (PARTY *party)
{
    char workbuffer[20];

    if (party->partyHandle == NULL) {
	sprintf(workbuffer, "Party%04x", party->partyIndex);
	setstring(&party->partyHandle, workbuffer);
    }
}

void
snmp_write_views (char *cfgcmd)
{
    MIB_VIEW   *view;
    int		viewbits;
    int		limit;
    int		subid;
    int		stridx;

    for (view = v2view_head_ptr; view; view = view->next_ptr) {
	if ((view->viewStatus == ACTIVE) &&
	    (view->viewStorageType == NONVOLATILE)) {

	    /* make sure any masked subidentifier is set to 0 */
	    viewbits = view->viewMask->length * 8;
	    limit = MIN(viewbits, view->viewSubtree->length);
	    for (subid = 0; subid < limit; subid++) {
		if (!CHECK_LEFT_BIT(subid, view->viewMask->octet_ptr)) {
		    view->viewSubtree->oid_ptr[subid] = 0;
		}
	    }

	    /* convert the OID to a string */
	    MakeDotFromOID(view->viewSubtree, workbuffer);

	    /* insert any required mask characters into the OID string */
	    subid = view->viewSubtree->length - 1;
	    for (stridx = strlen(workbuffer) - 2; stridx; stridx--) {
		if (workbuffer[stridx] == '.') {
		    if ((subid < viewbits) &&
			(!CHECK_LEFT_BIT(subid, view->viewMask->octet_ptr))) {
			workbuffer[stridx+1] = '*';
		    }
		    subid--;
		}
	    }

	    if (view->viewHandle == NULL) {
		snmp_name_view(view);
	    }

	    /*
	     * don't write derived views
	     */
	    if (view->viewHandle[0] == '*') {
		continue;
	    }

	    nv_write(TRUE, "%s %s %s", cfgcmd, view->viewHandle, workbuffer);
	    if (view->viewType == INCLUDED) {
		nv_add(TRUE, " included");
	    } else {
		nv_add(TRUE, " excluded");
	    }
	}
    }

}

static void
snmp_show_storage(long storagetype)
{
    switch (storagetype) {
    case OTHER:
	printf(" other");
	break;
    case VOLATILE:
	printf(" volatile");
	break;
    case NONVOLATILE:
	printf(" nonvolatile");
	break;
    case PERMANENT:
	printf(" permanent");
	break;
    default:
	printf(" invalid");
	break;
    }
}

static void
snmp_show_status(long status)
{
    switch(status) {
    case ACTIVE:
	break;
    case NOT_IN_SERVICE:
	printf(" not-in-service");
	break;
    case NOT_READY:
	printf(" not-ready");
	break;
    case CREATE_AND_GO:
	printf(" create-go");
	break;
    case CREATE_AND_WAIT:
	printf(" create-wait");
	break;
    case DESTROY:
	printf(" destroy");
	break;
    default:
	printf(" invalid");
	break;
    }
}

void
snmp_show_views (void)
{
    MIB_VIEW *view;

    for (view = v2view_head_ptr; view; view = view->next_ptr) {

	printf("\n%u", view->viewIndex);
	if (view->viewHandle != NULL) {
	    printf("(%s)", view->viewHandle);
	}

	workbuffer[0] = ' ';
	MakeDotFromOID(view->viewSubtree, &workbuffer[1]);
	printf(workbuffer);
	
	if (view->viewMask->length) {
	    MakeHexFromOctetString(view->viewMask, workbuffer);
	} else {
	    workbuffer[0] = '\0';
	}
	printf(" '%s'", workbuffer);

	if (view->viewType == INCLUDED) {
	    printf(" included");
	} else {
	    printf(" excluded");
	}

	snmp_show_storage(view->viewStorageType);

	snmp_show_status(view->viewStatus);
    }

}

void
snmp_write_parties (char *cfgcmd, boolean hide_pw)
{
    PARTY      *party;
    ulong	partyaddr;

    for (party = v2party_head_ptr; party; party = party->next_ptr) {
	if ((party->partyStatus == ACTIVE) &&
	    (party->partyStorageType == NONVOLATILE)) {
	    MakeDotFromOID(party->partyIdentity, workbuffer);
	    if (party->partyHandle == NULL) {
		snmp_name_party(party);
	    }

	    /*
	     * don't write derived parties
	     */
	    if (party->partyHandle[0] == '*') {
		continue;
	    }

	    partyaddr = OctetStringToIP(party->partyTAddress);
	    nv_write(TRUE, "%s %s %s udp %i %u", cfgcmd, party->partyHandle,
		     workbuffer, partyaddr, party->partyTPort);
	    nv_add(party->partyMaxMessageSize != SR_DEF_PARTY_MESSAGE_SIZE,
		   " packetsize %d", party->partyMaxMessageSize);
	    nv_add(party->partyLocal == SR_REMOTE, " remote");

	    switch (party->partyAuthProtocol) {
	    case TRIVIAL_AUTHENTICATION:
		if (!hide_pw) {
		    bcopy(party->partyAuthPrivate->octet_ptr, workbuffer,
			  party->partyAuthPrivate->length);
		    workbuffer[party->partyAuthPrivate->length] = '\0';
		}
		nv_add(TRUE, " authentication snmpv1 %s", 
		       (hide_pw) ? techsupport_blank_string : workbuffer);
		break;
#ifndef SR_UNSECURABLE
	    case MD5_SNMPv2_AUTHENTICATION:
		if (!hide_pw) {
		    MakeHexFromOctetString(party->partyAuthPrivate,
					   workbuffer);
		}
		nv_add(TRUE, " authentication md5 %s",
		       (hide_pw) ? techsupport_blank_string : workbuffer);
		nv_add(party->partyAuthLifetime != SR_DEF_PARTY_LIFETIME,
		       " lifetime %u", party->partyAuthLifetime);

#ifndef SR_NO_PRIVACY

		if (party->partyPrivProtocol == DESPRIV) {
		    if (!hide_pw) {
			MakeHexFromOctetString(party->partyPrivPrivate,
					       workbuffer);
		    }
		    nv_add(TRUE, " privacy des %s",
			   (hide_pw) ? techsupport_blank_string : workbuffer);
		}
#endif                          /* SR_NO_PRIVACY */

		break;
#endif                          /* SR_UNSECURABLE */
	    }
	}
    }
}

void
snmp_show_parties (void)
{
    PARTY      *party;
    ulong	partyaddr;

    for (party = v2party_head_ptr; party; party = party->next_ptr) {

	printf("\n%u", party->partyIndex);
	if (party->partyHandle != NULL) {
	    printf("(%s)", party->partyHandle);
	}

	workbuffer[0] = ' ';
	MakeDotFromOID(party->partyIdentity, &workbuffer[1]);
	printf(workbuffer);

	partyaddr = OctetStringToIP(party->partyTAddress);
	printf(" %i %u %u", partyaddr, party->partyTPort,
	       party->partyMaxMessageSize);

	switch (party->partyLocal) {
	case SR_LOCAL:
	    printf(" local");
	    break;
	case SR_REMOTE:
	    printf(" remote");
	    break;
	default:
	    printf(" unknown");
	    break;
	}

	printf("\n  auth ");
	switch (party->partyAuthProtocol) {
	default:
	    printf("unkAuth");
	    break;
	case TRIVIAL_AUTHENTICATION:
	    bcopy(party->partyAuthPrivate->octet_ptr, workbuffer,
		  party->partyAuthPrivate->length);
	    workbuffer[party->partyAuthPrivate->length] = '\0';
	    printf("snmpv1Auth %s", workbuffer);
	    break;
	case NOAUTH_AUTHENTICATION:
	    printf("noAuth");
	    break;
#ifndef SR_UNSECURABLE
	case MD5_SNMPv2_AUTHENTICATION:
	    MakeHexFromOctetString(party->partyAuthPrivate, workbuffer);
	    printf("md5 %s life %u\n ", workbuffer, party->partyAuthLifetime);
	    break;
#endif                          /* SR_UNSECURABLE */
	}

	printf(" priv ");
	switch (party->partyPrivProtocol) {
	default:
	    printf("unkPriv");
	    break;
	case ABSENT:
	    printf("none");
	    break;
	case NOPRIV:
	    printf("noPriv");
	    break;
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
	case DESPRIV:
	    MakeHexFromOctetString(party->partyPrivPrivate, workbuffer);
	    printf("des %s", workbuffer);
	    break;
#endif                          /* SR_NO_PRIVACY */
#endif                          /* SR_UNSECURABLE */
	}

	snmp_show_storage(party->partyStorageType);

	snmp_show_status(party->partyStatus);
    }
}

void
snmp_write_contexts (char *cfgcmd)
{
    CONTEXT  *context;
    long      viewidx;

    for (context = v2context_head_ptr; context; context = context->next_ptr) {
	if ((context->contextStatus == ACTIVE) &&
	    (context->contextStorageType == NONVOLATILE)) {

	    MakeDotFromOID(context->contextIdentity, workbuffer);
	    viewidx = GetViewEntry(context->contextViewIndex);
	    if (viewidx == -1) {
		/*
		 * the associated view is invalid
		 */
		continue;
	    }
	    if (context->contextHandle == NULL) {
		snmp_name_context(context);
	    }
	    if (party_views[viewidx].mib_view->viewHandle == NULL) {
		snmp_name_view (party_views[viewidx].mib_view);
	    }

	    /*
	     * don't write derived contexts or
	     * contexts which reference derived views
	     */
	    if ((context->contextHandle[0] == '*') ||
		(party_views[viewidx].mib_view->viewHandle[0] == '*')) {
		continue;
	    }

	    nv_write(TRUE, "%s %s %s %s", cfgcmd, context->contextHandle,
		     workbuffer, party_views[viewidx].mib_view->viewHandle);
	}
    }

}

void
snmp_show_contexts (void)
{

    CONTEXT  *context;
    long      viewidx;

    for (context = v2context_head_ptr; context; context = context->next_ptr) {

	printf("\n%u", context->contextIndex);
	if (context->contextHandle != NULL) {
	    printf("(%s)", context->contextHandle);
	}

	workbuffer[0] = ' ';
	MakeDotFromOID(context->contextIdentity, &workbuffer[1]);
	printf(workbuffer);

	printf(" %u", context->contextViewIndex);
	viewidx = GetViewEntry(context->contextViewIndex);
	if ((viewidx != -1) &&
	    (party_views[viewidx].mib_view->viewHandle != NULL)) {
	    printf("(%s)", party_views[viewidx].mib_view->viewHandle);
	}

	snmp_show_storage(context->contextStorageType);

	snmp_show_status(context->contextStatus);
    }

}

void
snmp_write_acls (char *cfgcmd)
{
    ACCESS   *acl;
    long      targetidx;
    long      subjectidx;
    long      resourceidx;

    for (acl = v2acl_head_ptr; acl; acl = acl->next_ptr) {
	if ((acl->aclStatus == ACTIVE) &&
	    (acl->aclStorageType == NONVOLATILE)) {

	    targetidx = GetPartyLocation(acl->aclTarget);
	    subjectidx = GetPartyLocation(acl->aclSubject);
	    resourceidx = GetContextEntry(acl->aclResources);
	    if (targetidx == -1 || subjectidx == -1 || resourceidx == -1) {
		/*
		 * a party or context is no longer valid
		 */
		continue;
	    }
	    /*
	     * make sure everybody has a name
	     */
	    if (parties[targetidx].party->partyHandle == NULL) {
		snmp_name_party(parties[targetidx].party);
	    }
	    if (parties[subjectidx].party->partyHandle == NULL) {
		snmp_name_party(parties[subjectidx].party);
	    }
	    if (contexts[resourceidx].context->contextHandle == NULL) {
		snmp_name_context(contexts[resourceidx].context);
	    }

	    /*
	     * don't write acls which reference derived parties or contexts
	     */
	    if ((parties[targetidx].party->partyHandle[0] == '*') ||
		(parties[subjectidx].party->partyHandle[0] == '*') ||
		(contexts[resourceidx].context->contextHandle[0] == '*')) {
		continue;
	    }

	    nv_write(TRUE, "%s %s %s %s %#x", cfgcmd,
		     parties[targetidx].party->partyHandle,
		     parties[subjectidx].party->partyHandle,
		     contexts[resourceidx].context->contextHandle,
		     acl->aclPrivileges);
	}
    }

}

void
snmp_show_acls (void)
{
    ACCESS   *acl;
    long      targetidx;
    long      subjectidx;
    long      resourceidx;

    for (acl = v2acl_head_ptr; acl; acl = acl->next_ptr) {

	printf("\n%u", acl->aclTarget);
	targetidx = GetPartyLocation(acl->aclTarget);
	if ((targetidx != -1) &&
	    (parties[targetidx].party->partyHandle != NULL)) {
	    printf("(%s)", parties[targetidx].party->partyHandle);
	}

	printf(" %u", acl->aclSubject);
	subjectidx = GetPartyLocation(acl->aclSubject);
	if ((subjectidx != -1) &&
	    (parties[subjectidx].party->partyHandle != NULL)) {
	    printf("(%s)", parties[subjectidx].party->partyHandle);
	}

	printf(" %u", acl->aclResources);
	resourceidx = GetContextEntry(acl->aclResources);
	if ((resourceidx != -1) &&
	    (contexts[resourceidx].context->contextHandle != NULL)) {
	    printf("(%s)", contexts[resourceidx].context->contextHandle);
	}

	printf(" %#x", acl->aclPrivileges);

	snmp_show_storage(acl->aclStorageType);

	snmp_show_status(acl->aclStatus);
    }
}

/*
 * snmp_configure_view:
 * OBJ(string,1) = <viewname>
 * OBJ(string,2) = <oidtree>
 * OBJ(int,1) =	TRUE == included
 *		FALSE == excluded
 */
void
snmp_configure_view (parseinfo *csb)
{
    if (csb->sense) {
	snmp_add_view(GETOBJ(string, 1),
		      GETOBJ(string, 2),
		      GETOBJ(int, 1) ? INCLUDED : EXCLUDED,
		      NONVOLATILE);
    } else {
	snmp_delete_view(GETOBJ(string, 1),
			 GETOBJ(string, 2));
    }
}

/*
 * snmp_configure_context:
 * OBJ(string,1) = <contextname>
 * OBJ(string,2) = <contextoid>
 * OBJ(string,3) = <viewname>
 */
void
snmp_configure_context (parseinfo *csb)
{
    if (csb->sense) {
	snmp_add_context(GETOBJ(string, 1),
			 GETOBJ(string, 2),
			 GETOBJ(string, 3),
			 NONVOLATILE);
    } else {
	snmp_delete_context(GETOBJ(string, 1));
    }
}

/*
 * snmp_configure_party:
 * OBJ(string,1) = <partyname>
 * OBJ(string,2) = <partyoid>
 * OBJ(string,3) = <md5 key> or <community>
 * OBJ(string,4) = <des key>
 * OBJ(paddr,1) = <ipaddress>
 * OBJ(int,1) = <protocol>	SNMP_PARTY_PROTO_UDP == udp
 *				SNMP_PARTY_PROTO_IPX == ipx
 *				SNMP_PARTY_PROTO_ATALK == atalk
 * OBJ(int,2) = <port>
 * OBJ(int,3) = <packetsize>
 * OBJ(int,4) = SNMP_PARTY_LOCAL == local
 *		SNMP_PARTY_REMOTE == remote
 * OBJ(int,5) = <authentication>	SNMP_PARTY_AUTH_MD5 == md5
 *					SNMP_PARTY_AUTH_SNMPV1 == snmpv1
 * OBJ(int,6) = TRUE == clock is set
 * OBJ(int,7) = <clock>
 * OBJ(int,8) = TRUE == lifetime is set
 * OBJ(int,9) = <lifetime>
 * OBJ(int,10) = <privacy>		SNMP_PARTY_PRIV_DES == des
 */
void
snmp_configure_party (parseinfo *csb)
{
    int		 domain;
    int		 packetsize;
    int		 authtype;
    int		 privtype;
    OctetString *authkey;
    OctetString *privkey;

    if (csb->sense) {
	switch (GETOBJ(int, 5)) {
	case SNMP_PARTY_AUTH_SNMPV1:
	    domain = RFC_1157_DOMAIN;
	    authtype = TRIVIAL_AUTHENTICATION;
	    authkey = MakeOctetStringFromText(GETOBJ(string, 3));
	    privtype = ABSENT;
	    privkey = MakeOctetString(nullString, 0);
	    break;
#ifndef SR_UNSECURABLE
	case SNMP_PARTY_AUTH_MD5:
	    domain = SNMP_UDP_DOMAIN;
	    authtype = MD5_SNMPv2_AUTHENTICATION;
	    authkey = MakeOctetStringFromHex(GETOBJ(string, 3));
	    switch (GETOBJ(int,10)) {
#ifndef SR_NO_PRIVACY
	    case SNMP_PARTY_PRIV_DES:
		privtype = DESPRIV;
		privkey = MakeOctetStringFromHex(GETOBJ(string,4));
		break;
#endif                          /* SR_NO_PRIVACY */
	    default:
		privtype = NOPRIV;
		privkey = MakeOctetString(nullString, 0);
		break;
	    }
	    break;
#endif                          /* SR_UNSECURABLE */
	default:
	    domain = SNMP_UDP_DOMAIN;
	    authtype = NOAUTH_AUTHENTICATION;
	    authkey = MakeOctetString(nullString, 0);
	    privtype = NOPRIV;
	    privkey = MakeOctetString(nullString, 0);
	    break;
	}

	packetsize = GETOBJ(int, 3);
	if (packetsize == 0)
	    packetsize = SR_DEF_PARTY_MESSAGE_SIZE;

	snmp_add_party(GETOBJ(string, 1),
		       GETOBJ(string, 2),
		       domain,
		       GETOBJ(paddr, 1)->ip_addr,
		       GETOBJ(int, 2),
		       0,
		       packetsize,
		       authtype,
		       authkey,
		       ISOBJ(int, 6) ? GETOBJ(int, 7) : 0,
		       ISOBJ(int, 8) ? GETOBJ(int, 9) : SR_DEF_PARTY_LIFETIME,
		       privtype,
		       privkey,
		       GETOBJ(int, 4) == SNMP_PARTY_REMOTE ? SR_REMOTE :
		                                             SR_LOCAL,
		       NONVOLATILE);

	FreeOctetString(authkey);
	FreeOctetString(privkey);

    } else {
	snmp_delete_party(GETOBJ(string, 1));
    }
}

/*
 * snmp_configure_acl:
 * OBJ(string,1) = <dst party name>
 * OBJ(string,2) = <src party name>
 * OBJ(string,3) = <context name>
 * OBJ(int,1) = <privilege>
 */
void
snmp_configure_acl (parseinfo *csb)
{
    if (csb->sense) {
	snmp_add_acl(GETOBJ(string, 1),
		     GETOBJ(string, 2),
		     GETOBJ(string, 3),
		     GETOBJ(int, 1),
		     NONVOLATILE);
    } else {
	snmp_delete_acl(GETOBJ(string, 1),
			GETOBJ(string, 2),
			GETOBJ(string, 3));
    }
}

/*
 * snmp_ip_address_change:
 * called via the ip_address_change registry when an ip address is added
 * or deleted.  when called, all parties, contexts and acls associated with
 * the address will be created or destroyed as appropriate
 */
static void
snmp_ip_address_change (idbtype *idb,
			ipaddrtype address,
			ipaddrtype mask,
			boolean secondary,
			boolean adding)
{
    char	       *oidstring = NULL;
    char	       *agtpartyname = NULL;
    char	       *mgrpartyname = NULL;
    char	       *contextname = NULL;
    OctetString	       *nullkey;

    if (adding) {
	/*
	 * reject secondary addresses and unnumbered interfaces
	 */

	if ((idb->ip_address == 0) ||
	    (idb->ip_address != address))
	    return;

	/*
	 * create the default noAuth/noPriv parties for this address,
	 * as well as the default context and acl
	 */
	nullkey = MakeOctetStringFromText(nullString);

	sprintf(workbuffer, "%s.%i.1", initialPartyId, address);
	setstring(&oidstring, workbuffer);

	sprintf(workbuffer, "*iap.%0x", address);
	setstring(&agtpartyname, workbuffer);

	snmp_add_party(agtpartyname, oidstring,
		       SNMP_UDP_DOMAIN, address, 161, 0,
		       SR_DEF_PARTY_MESSAGE_SIZE,
		       NOAUTH_AUTHENTICATION, nullkey, 0, 0,
		       NOPRIV, nullkey, SR_LOCAL, NONVOLATILE);
	
	sprintf(workbuffer, "%s.%i.2", initialPartyId, address);
	setstring(&oidstring, workbuffer);

	sprintf(workbuffer, "*imp.%0x", address);
	setstring(&mgrpartyname, workbuffer);

	snmp_add_party(mgrpartyname, oidstring,
		       SNMP_UDP_DOMAIN, 0, 0, 0,
		       SR_DEF_PARTY_MESSAGE_SIZE,
		       NOAUTH_AUTHENTICATION, nullkey, 0, 0,
		       NOPRIV, nullkey, SR_REMOTE, NONVOLATILE);

	sprintf(workbuffer, "%s.%i.1", initialContextId, address);
	setstring(&oidstring, workbuffer);

	sprintf(workbuffer, "*ic.%0x", address);
	setstring(&contextname, workbuffer);

	snmp_add_context(contextname, oidstring, restricted, NONVOLATILE);

	snmp_add_acl(agtpartyname,
		     mgrpartyname,
		     contextname,
		     GET_MASK | GET_NEXT_MASK | BULK_MASK,
		     NONVOLATILE);

	snmp_add_acl(mgrpartyname,
		     agtpartyname,
		     contextname,
		     GET_RESPONSE_MASK,
		     NONVOLATILE);

	free(oidstring);
	free(agtpartyname);
	free(mgrpartyname);
	free(contextname);
	FreeOctetString(nullkey);

    } else {
	/*
	 * ignore deletes for now
	 */
    }
}


void
snmp_admin_init (void)
{
    /*
     * init counter used to seed proxy party oids
     */
    proxy_party_count = 0;

    /*
     * create the initial mib views per RFC 1447
     */

    snmp_add_view(restricted, system_str, INCLUDED, PERMANENT);
    snmp_add_view(restricted, snmpStats, INCLUDED, PERMANENT);
    snmp_add_view(restricted, snmpParties, INCLUDED, PERMANENT);
    snmp_add_view(everything, internet, INCLUDED, PERMANENT);

    /*
     * the initial mib parties and contexts will be created later as
     * each address is configured.  Since the initial acls require that
     * the associated parties and context be in place, the initial acl
     * creation will also take place later
     */

    /*
     * register services which allow ILMI to add to the admin model
     */
    reg_add_add_snmp_view(snmp_add_view, "snmp_add_view");
    reg_add_add_snmp_community(snmp_add_community_proxy,
			       "snmp_add_community_proxy");
    /*
     * register a function to process ip address changes
     */
    reg_add_ip_address_change(snmp_ip_address_change,
			      "snmp_ip_address_change");

}


#endif                          /* SR_SNMPv2 */
