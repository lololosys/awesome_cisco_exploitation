/* $Id: at_access.c,v 3.6.6.5 1996/09/07 15:43:19 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_access.c,v $
 *------------------------------------------------------------------
 * at_access.c -- handle AppleTalk access lists
 * 
 * August 1989, David Edwards
 * September 1991, Major rewrite, David Edwards
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_access.c,v $
 * Revision 3.6.6.5  1996/09/07  15:43:19  dwong
 * CSCdi68416:  Need to update the nbp filter counter correctly
 * Branch: California_branch
 *
 * Revision 3.6.6.4  1996/08/13  00:26:31  dwong
 * CSCdi65598:  NBP filters outbound LkUp when received as FwdReq
 * Branch: California_branch
 *
 * Revision 3.6.6.3  1996/06/28  23:04:46  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.6.2  1996/03/23  01:25:36  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.6.6.1  1996/03/18  18:51:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  06:26:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/07  08:28:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  00:16:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/03/05  22:13:51  kleung
 * CSCdi49904:  atalk_upper vs atalk_cvu are equivalent
 *
 * Revision 3.5  1996/02/07  16:09:31  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  05:58:49  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/08  04:55:06  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.2  1995/11/17  08:41:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:49:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/29  22:32:35  dwong
 * CSCdi40454:  Wants NBP filters to take effect on outbound traffic
 * Make nbp packets (un)interesting packets when nbp filters are used
 * with dialer lists.
 *
 * Revision 2.1  1995/06/07  20:07:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include <interface.h>
#include "packet.h"
#include "atalk_private.h"
#include "at_access.h"
#include "at_nbp.h"
#include "access.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_atalk.h"

static char const *
atacc_nomem = "\n%%No memory available for AppleTalk access list %d";


static id_string const at_cmd_str[] = {
    { ATACC_ANYZONES, ATACCKWD_ADDITIONAL_ZONES},
    { ATACC_CABLE, ATACCKWD_CABLE_RANGE},
    { ATACC_INCLUDES, ATACCKWD_INCLUDES},
    { ATACC_NETWORK, ATACCKWD_NETWORK},
    { ATACC_OTHERS, ATACCKWD_OTHER_ACCESS},
    { ATACC_WITHIN, ATACCKWD_WITHIN},
    { ATACC_ZONE, ATACCKWD_ZONE},
    { ATACC_DELETE, ATACCKWD_NO_APPLETALK},
    { ATACC_NBP_OBJ, ATACCKWD_NBP_OBJ},
    { ATACC_NBP_TYPE, ATACCKWD_NBP_TYPE},
    { ATACC_NBP_ZONE, ATACCKWD_NBP_ZONE},
    {0, NULL}
};

/*
 * Lookup an appletalk command string
 */
static char const *at_cmd_lookup_str (ushort id)
{
    return (lookup_id_string(at_cmd_str, id));
}

/*
 * atacc_GetTable - Return an access table (optionally creating a new table)
 */
static atacctable*
atacc_GetTable (int number, createOps op)
{
    atacctable* table;
    if ((number < MINATALKACC) || (number > MAXATALKACC))
	return (NULL);
    table = (atacctable*) accessarray[number];
    if (table == NULL) {
	if (op != createIt)
	    return (NULL);
	table = malloc(sizeof(atacctable));
	if (table == NULL) {
	    printf(atacc_nomem,number);
	    return (NULL);
	}
	table->networks = RBTreeCreate("Appletalk", "AT", "AccessTable",
				       at_print_key_interval,
				       &appletalk_debug->flag);
	if (table->networks == NULL) {
	    free(table);
	    printf(atacc_nomem,number);
	    return (NULL);
	}
	table->defaccess = FALSE;
	table->defbroadcastaccess = FALSE;
	table->defzoneaccess = FALSE;
	table->defzonebroadcastaccess = FALSE;
	table->number = number;
	table->inactive = FALSE;
	table->zones = NULL;
	table->zonecount = 0;
	table->nbpacclist = NULL;
	accessarray[number] = (accessptr*)table;
	return (table);
    }
    if ((table->inactive) && (op != createIt))
	return (NULL);
    table->inactive = FALSE;
    return (table);
}


/*
 * Find the existing entry or create a new entry if not found
 */
static nbpaccentry 
*atacc_FindNBPEntry(int nbpentry, int sequence, nbpaccentry *nbpacclist)
{
    nbpaccentry *next=nbpacclist;
    nbpaccentry *prev=next;

    while (next != NULL) {
    
      switch (nbpentry){

        case ATACC_NBP_OBJ:
        case ATACC_NBP_TYPE:
        case ATACC_NBP_ZONE:
	  if (sequence == next->sequence)
	    return (next);
	  break;

	default:
	  if (next->nbpentry == nbpentry)
	    return (next);
	  break;

	}
      
      prev = next;
      next = next->next;

    }

    /*
     * Entry not fount, so create a new one
    */
    next = malloc(sizeof(nbpaccentry));
    if (next == NULL)
      return NULL;
    
    /*
     * Link into the nbp access list
     */
    if (prev)
      prev->next = next;

    /* 
     * Do mininum initialization
     */
    next->nbpentry = 0;
    next->sequence = -1;
    next->object_len = 0;
    next->object = NULL;
    next->type_len = 0;
    next->type = NULL;
    next->zone_len = 0;
    next->zone = NULL;
    next->p_object = NULL;
    next->p_type = NULL;
    next->p_zone = NULL;
    next->next = NULL;

    return (next);
}

static void
atacc_AddNBPEntry(int number,
		  boolean grant, 
		  int nbpentry, 
		  int sequence, 
		  char *nbpname)
{
    atacctable *acctab = atacc_GetTable(number, createIt);
    nbpaccentry *nbp = NULL;
    char temp[100];
    uchar *d, *s;
    int len;

    if (acctab == NULL)
      return;

    if ((len = atalk_hex_filter(nbpname, temp, 100)) > NBP_namelen){
      printf("NBP string can not exceed 32 characters.\n");
      return;
    }

    nbp = atacc_FindNBPEntry(nbpentry, sequence, acctab->nbpacclist);

    if (nbp) {

      /*
       * The access list is nil initially, so set it here
       */
      if (acctab->nbpacclist == NULL)
	  acctab->nbpacclist = nbp;

      nbp->sequence = sequence; 
      nbp->grant = grant;
      nbp->nbpentry = nbpentry;
      d = (uchar *)temp;

      switch (nbpentry) {

        case ATACC_NBP_OBJ:
	   if (nbp->p_object)
	     free(nbp->p_object);
	   nbp->p_object = malloc(strlen(nbpname)+1);
	   if (nbp->p_object)
	     strcpy(nbp->p_object, nbpname);
	   else {
	     printf(atacc_nomem,number);
	     break;
	   }
	   /* 
	    * store hex filtered upper case string here
	    */
	   if (nbp->object)
	     free(nbp->object);
	   nbp->object_len = len;
	   nbp->object = malloc(len);
	   if (nbp->object) {
	     s = (uchar *)nbp->object;
	     while (len--)
	       (*s++) = atalk_upper(*d++);
	     atalk_NBPFilters++;
	   }
	   else {
	     free(nbp->p_object);
	     printf(atacc_nomem,number);
	   }
	   break;

	case ATACC_NBP_TYPE:
	   if (nbp->p_type)
	     free(nbp->p_type);
	   nbp->p_type = malloc(strlen(nbpname)+1);
	   if (nbp->p_type)
	     strcpy(nbp->p_type, nbpname);
	   else {
	     printf(atacc_nomem,number);
	     break;
	   }
	   /* 
	    * store hex filtered upper case string here
	    */
	   if (nbp->type)
	     free(nbp->type);
	   nbp->type_len = len;
	   nbp->type = malloc(len);
	   if (nbp->type) {
	     s = (uchar *)nbp->type;
	     while (len--)
	       (*s++) = atalk_upper(*d++);
	     atalk_NBPFilters++;
	   }
	   else {
	     free(nbp->p_type);
	     printf(atacc_nomem,number);
	   }
	   break;

	case ATACC_NBP_ZONE:
	   if (nbp->p_zone)
	     free(nbp->p_zone);
	   nbp->p_zone = malloc(strlen(nbpname)+1);
	   if (nbp->p_zone)
	     strcpy(nbp->p_zone, nbpname);
	   else {
	     printf(atacc_nomem,number);
	     break;
	   }
	   /* 
	    * store hex filtered upper case string here
	    */
	   if (nbp->zone)
	     free(nbp->zone);
	   nbp->zone_len = len;
	   nbp->zone = malloc(len);
	   if (nbp->zone) {
	     s = (uchar *)nbp->zone;
	     while (len--)
	       (*s++) = atalk_upper(*d++);
	     atalk_NBPFilters++;
	   }
	   else {
	     free(nbp->p_zone);
	     printf(atacc_nomem,number);
	   }
	   break;

	case ATACC_NBP_OTHERS:
	   atalk_NBPFilters++;
	   break;
	}
    }
    else
      printf(atacc_nomem,number);
}

static void
atacc_DelNBPEntry(int number, int nbpentry, int sequence)
{
    atacctable *acctab=atacc_GetTable(number, mustExist);
    nbpaccentry *next, *prev;

    if (acctab == NULL)
      return;

    prev = next = acctab->nbpacclist;

    while (next) {

      switch (nbpentry) {

        case ATACC_NBP_OBJ:
  	  if (next->sequence == sequence) {
	    if (next->object) {
	      free(next->object);
	      free(next->p_object);
	      next->object = NULL;
	      next->p_object = NULL;
	      if (next->type == NULL && next->zone == NULL) {
		prev->next = next->next;
	        /*
	         * if removing the last one, nil out the list;
	         * if removing the first one, link to the next one
	         */
	        if (next == acctab->nbpacclist)
	          acctab->nbpacclist = next->next;
	        free(next);
	      }
	      atalk_NBPFilters--;
	    }
	    return;
	  }
	  break;

        case ATACC_NBP_TYPE:
  	  if (next->sequence == sequence) {
	    if (next->type) {
	      free(next->type);
	      free(next->p_type);
	      next->type = NULL;
	      next->p_type = NULL;
	      if (next->object == NULL && next->zone == NULL) {
		prev->next = next->next;
	        /*
	         * if removing the last one, nil out the list;
	         * if removing the first one, link to the next one
	         */
	        if (next == acctab->nbpacclist)
	          acctab->nbpacclist = next->next;
	        free(next);
	      }
	      atalk_NBPFilters--;
	    }
	    return;
	  }
	  break;

        case ATACC_NBP_ZONE:
  	  if (next->sequence == sequence) {
	    if (next->zone) {
	      free(next->zone);
	      free(next->p_zone);
	      next->zone = NULL;
	      next->p_zone = NULL;
	      if (next->object == NULL && next->type == NULL) {
		prev->next = next->next;
	        /*
	         * if removing the last one, nil out the list;
	         * if removing the first one, link to the next one
	         */
	        if (next == acctab->nbpacclist)
	          acctab->nbpacclist = next->next;
	        free(next);
	      }
	      atalk_NBPFilters--;
	    }
	    return;
	  }
	  break;

	case ATACC_NBP_OTHERS:
	  if (next->nbpentry == nbpentry) {
	    prev->next = next->next;
	    /*
	     * if removing the last one, nil out the list;
	     * if removing the first one, link to the next one
	     */
	    if (next == acctab->nbpacclist)
	        acctab->nbpacclist = next->next;
	    free(next);
	    atalk_NBPFilters--;
	    return;
	  }
	  break;

	}
      
      prev = next;
      next = next->next;
    } 
} 

static void
atacc_DeleteNBPList(nbpaccentry *nbpacclist)
{
    nbpaccentry *next, *prev;

    next = nbpacclist;

    while (next) {
      if (next->nbpentry == ATACC_NBP_OTHERS) {
	  atalk_NBPFilters--;
      } else {
          if (next->object) {
	      free(next->object);
	      free(next->p_object);
	      atalk_NBPFilters--;
	  }
	  if (next->type) {
	      free(next->type);
	      free(next->p_type);
	      atalk_NBPFilters--;
	  }
	  if (next->zone) {
	      free(next->zone);
	      free(next->p_zone);
	      atalk_NBPFilters--;
	  }
      }
      prev = next;
      next = next->next;
      free(prev);
    }
}

static void 
atacc_WriteNBPList(parseinfo *csb, int number, nbpaccentry *nbpacclist)
{
    nbpaccentry *next;
    char *cmd_str;
    boolean defaccess=FALSE;

    next = nbpacclist;

    while (next) {

      switch (next->nbpentry) {	

        case ATACC_NBP_OBJ:
        case ATACC_NBP_TYPE:
        case ATACC_NBP_ZONE:
	   if (next->p_object) {
	     cmd_str = (char *)at_cmd_lookup_str(ATACC_NBP_OBJ);
	     nv_write(TRUE, "%s %d %s %s %d %s %s", 
		      csb->nv_command,
		      number,
		      next->grant ? "permit" : "deny",
		      "nbp",
		      next->sequence,
		      cmd_str,
		      next->p_object);
	   }
	   if (next->p_type) {
	     cmd_str = (char *)at_cmd_lookup_str(ATACC_NBP_TYPE);
	     nv_write(TRUE, "%s %d %s %s %d %s %s", 
		      csb->nv_command,
		      number,
		      next->grant ? "permit" : "deny",
		      "nbp",
		      next->sequence,
		      cmd_str,
		      next->p_type);
	   }
	   if (next->p_zone) {
	     cmd_str = (char *)at_cmd_lookup_str(ATACC_NBP_ZONE);
	     nv_write(TRUE, "%s %d %s %s %d %s %s", 
		      csb->nv_command,
		      number,
		      next->grant ? "permit" : "deny",
		      "nbp",
		      next->sequence,
		      cmd_str,
		      next->p_zone);
	   }	   
           break;

	case ATACC_NBP_OTHERS:
	   defaccess = next->grant;

	 }

      next = next->next;

    }

    /*
     * Default access
     */
    nv_write(TRUE, "%s %d %s %s", 
	     csb->nv_command,
	     number,
	     defaccess ? "permit" : "deny",
	     "other-nbps");    
}

static void 
atacc_ShowNBPList(nbpaccentry *nbpacclist)
{
    nbpaccentry *next;
    char *cmd_str;
    boolean defaccess=FALSE;

    next = nbpacclist;

    while (next) {
      
      switch (next->nbpentry) {	

        case ATACC_NBP_OBJ:
        case ATACC_NBP_TYPE:
        case ATACC_NBP_ZONE:
	   if (next->p_object) {
	     cmd_str = (char *)at_cmd_lookup_str(ATACC_NBP_OBJ);
	     printf("\n  %s %s %d %s %s", 
		      next->grant ? "permit" : "deny",
		      "nbp",
		      next->sequence,
		      cmd_str,
		      next->p_object);
	   }
	   if (next->p_type) {
	     cmd_str = (char *)at_cmd_lookup_str(ATACC_NBP_TYPE);
	     printf("\n  %s %s %d %s %s", 
		      next->grant ? "permit" : "deny",
		      "nbp",
		      next->sequence,
		      cmd_str,
		      next->p_type);
	   }
	   if (next->p_zone) {
	     cmd_str = (char *)at_cmd_lookup_str(ATACC_NBP_ZONE);
	     printf("\n  %s %s %d %s %s",
		      next->grant ? "permit" : "deny",
		      "nbp",
		      next->sequence,
		      cmd_str,
		      next->p_zone);
	   }	   
           break;

	case ATACC_NBP_OTHERS:
	   defaccess = next->grant;

	 }

      next = next->next;

    }

    /*
     * Default access
     */
    printf("\n  %s %s",
	   defaccess ? "permit" : "deny",
	   "other-nbps");   
}

static boolean
atacc_match_nbp_name(nbpOps *nbp, nbpaccentry *filter)
{
    uchar *s, *d;
    int len;

    /* 
     * match nbp object
     */
    if (filter->object) {
	if (filter->object_len != nbp->name.len)
	    return (FALSE);
	s = nbp->name.p;
	d = (uchar *)filter->object;
	len = filter->object_len;
	while (len--) {
	    if (atalk_upper(*s++) != (*d++))
	      return (FALSE);
	}
     }

    /* 
     * match nbp type
     */
    if (filter->type) {
	if (filter->type_len != nbp->type.len)
	    return (FALSE);
	s = nbp->type.p;
	d = (uchar *)filter->type;
	len = filter->type_len;
	while (len--) {
	    if (atalk_upper(*s++) != (*d++))
	      return (FALSE);
	}
     }

    /* 
     * match nbp zone
     */
    if (filter->zone) {
	if (filter->zone_len != nbp->zone.len)
	    return (FALSE);
	s = nbp->zone.p;
	d = (uchar *)filter->zone;
	len = filter->zone_len;
	while (len--) {
	    if (atalk_upper(*s++) != (*d++))
	      return (FALSE);
	}
    }

    return (TRUE);  /* they are matched */
}

static boolean
atacc_filter_nbp(nbpOps *nbp, nbpaccentry *nbpacclist)
{
    nbpaccentry *next;
    boolean othernbps;
    
    othernbps = FALSE;
    next = nbpacclist;
    while (next) {
        if (next->sequence == -1) {  /* other-nbps */
	    othernbps = next->grant;
	    next = next->next;
	    continue;
	}
	if (atacc_match_nbp_name(nbp, next))
	    return (next->grant);
	next = next->next;
    }
    return (othernbps);
}

/*
 * Handle reply packet with multiple tuples.  Return NULL if
 * all tuples are pitched.
 */
static paktype*
atacc_filter_nbpreply (paktype *reply, nbpaccentry *nbpacclist, int prioritylist)
{
    paktype *dup;
    int tupcount, ntuples, nbytes, newsize;
    uchar  *tp, *dup_tp;
    nbpOps nbp;

    dup = atalk_pak_duplicate(reply);
    if (dup == NULL)
        return (reply);
    
    /* point at tuple */
    tp = (uchar  *)(reply->transport_start + NBP_OVERHEADBYTES);
    dup_tp = (uchar *)(dup->transport_start + NBP_OVERHEADBYTES);

    ntuples = (int)nbp_get_count((nbptype *)reply->transport_start);
    newsize = tupcount = 0;

    while (ntuples--) {
        if (atnbp_DecodeTuple(tp, &nbp)) {
	    if (atacc_filter_nbp(&nbp, nbpacclist) == TRUE) {
	        /* 
		 * 3 = length bytes for name, type, and zone
		 */
	        nbytes = NBPTUPLESIZE + nbp.name.len + nbp.type.len + nbp.zone.len + 3;
		bcopy(tp, dup_tp, nbytes);
		dup_tp += nbytes;
		newsize += nbytes;
		tupcount++;
       	    }
	    
	    /* move to the next tuple */
	    tp += (NBPTUPLESIZE + nbp.name.len + nbp.type.len + nbp.zone.len + 3);
        }
	else {
	    /* it should never get here */
	    datagram_done((paktype *)dup);
	    return (reply);
	}
    }

    /* pitch the original packet */
    datagram_done((paktype *)reply);

    if (newsize) {
        /*
	 * set the new packet size and do checksum if necessary
	 */
        nbp_set_count((nbptype *)dup->transport_start, (uchar)tupcount);
        dup->atalk_datalen = newsize + NBP_OVERHEADBYTES;
	atalk_pak_setlength(dup, dup->atalk_datalen);
	if (atalk_pak_cksum(dup))
	    atalk_pak_setcksum(dup, atalk_compute_crc(dup));
        return (dup);
    }

    if (prioritylist)
        datagram_done(dup);
    else
        atalk_dispose(ATALK_NOACCESS, dup);
    return (NULL);
}

/*
 * atalk_nbpcheck
 *
 * Perform nbp packet check for atalk_input().  Check for NBP filters.
 */
paktype*
atalk_nbpcheck (paktype *pak, int prioritylist)
{
    atalkidbtype *idb;
    atacctable *accentry;
    nbpaccentry *nbpacclist;
    nbpOps nbp;
    uchar opcode;
    int acclist;

    if ((atalk_NBPFilters == 0) || (atalk_pak_getpt(pak) != DDP_TYPE_NBP) ||
	(pak->flags & PAK_SENDSELF))
        return (pak);
 
    if (prioritylist)
        acclist = prioritylist;
    else {
        idb = atalk_getidb(pak->if_input);
	if (idb == NULL)
	    return (pak);
	acclist = idb->atalk_accesslist;
    }

    accentry = atacc_GetTable(acclist, mustExist);
    if (accentry == NULL)
      return (pak);

    nbpacclist = accentry->nbpacclist;
    if (nbpacclist == NULL)
        return (pak);

    opcode = nbp_get_type((nbptype *)pak->transport_start);

    switch (opcode) {
        case NBP_LkUpReply:
            if (nbp_get_count((nbptype *)pak->transport_start) > 1)
	       return (atacc_filter_nbpreply(pak, nbpacclist, prioritylist));

        case NBP_BrRq:
        case NBP_LkUp:
        case NBP_FwdReq:
	    if (!atnbp_DecodeOps(pak,&nbp)) {
	        if (prioritylist)
		    datagram_done(pak);
		else
	            atalk_dispose(ATALK_NBP_BAD, pak);
		return (NULL);
	    }
	    if (atacc_filter_nbp(&nbp, nbpacclist) == FALSE) {
	        if (prioritylist)
		    datagram_done(pak);
		else
	            atalk_dispose(ATALK_NOACCESS, pak);
		return (NULL);
	    }

        default:
	    return (pak);
    }
}

static zipaccentry*
atacc_GetFirstZone (atacctable* acctab)
{
    zipaccentry* z = (zipaccentry*) acctab->zones;
    if (z == NULL)
	return (NULL);
    return (z);
}

static zipaccentry*
atacc_GetNextZone (zipaccentry* z)
{
    if (z != NULL)
	return (z->next);
    else
	return (NULL);
}

/*
 * atacc_GetZone
 * Find a zone for this access list
 */
static zipaccentry*
atacc_GetZone (atacctable* acctab,ziptabletype* zone)
{
    zipaccentry* z = atacc_GetFirstZone(acctab);
    if (zone == NULL)
	return (NULL);
    while (z != NULL) {
	if (z->zone == zone)
	    return (z);
	z = atacc_GetNextZone(z);
    }
    return (NULL);
}

/*
 * atacc_AddZone
 * Add a zone to an access list list
 */
static boolean
atacc_AddZone (atacctable* acctab, ziptabletype* zone, boolean grant)
{
    zipaccentry* zfirst;
    zipaccentry *zprev;
    zipaccentry* z = atacc_GetZone(acctab,zone);
    if ((z != NULL) || (zone == NULL))
	return (z != NULL);
    z = malloc(sizeof(struct zipaccentry_));
    if (z == NULL)
	return (FALSE);
    z->zone = zone;
    z->next = NULL;
    z->grant = grant;
    zfirst = atacc_GetFirstZone(acctab);
    if (zfirst == NULL) {
	acctab->zones = z;
    } else {
	/*
	 * Put zone at END of list so 'write term' shows zones in order
	 * created.
	 *
	 */
 zprev = zfirst;
	while (zprev->next != NULL)
	    zprev = zprev->next;
	zprev->next = z;
    }
    zone->acccount++;
    acctab->zonecount++;
    return (TRUE);
}

/*
 * atacc_DeleteZone
 * Delete a zone from an access list list
 */
static boolean
atacc_DeleteZone (atacctable* acctab, ziptabletype* zone)
{
    zipaccentry* zprev = NULL;
    zipaccentry* z = atacc_GetFirstZone(acctab);
    if (zone == NULL)
	return (FALSE);
    while ((z != NULL) && (z->zone != zone)) {
	zprev = z;
	z = atacc_GetNextZone(z);
    }
    if (z == NULL)
	return (FALSE);
    if (zprev == NULL) {
	acctab->zones = z->next;
    } else {
	zprev->next = z->next;
    }
    z->next = NULL;
    acctab->zonecount--;
    zone->acccount--;
    free(z);
    return (TRUE);
}

/*
 * atacc_ACLUsesZone
 * Check if a zone is defined for a list.
 */
boolean
atacc_ACLUsesZone (ziptabletype* zone, int list)
{
    atacctable* table = atacc_GetTable(list,mustExist);
    if (table==NULL)
	return (FALSE);
    return (atacc_GetZone(table,zone) != NULL);
}

/*
 * atacc_GetFirstNetwork - Return the first network entry or NULL
 */
static atacctype*
atacc_GetFirstNetwork (atacctable* table)
{
    if (table == NULL)
	return (NULL);
    return (atacctype*) RBTreeFirstNode(table->networks);
}

/*
 * atacc_GetNextNetwork - Return the logical next network entry or NULL
 */
static atacctype*
atacc_GetNextNetwork (atacctype* entry)
{
    if (entry == NULL)
	return (NULL);
    return (atacctype*) RBTreeNextNode(&entry->link);
}

/*
 * atacc_GetNetworkEntry - Return a matching network entry or NULL
 */
static atacctype*
atacc_GetNetworkEntry (atacctable* table,treeKey key)
{
    if (table == NULL)
	return (NULL);
    return (atacctype*) RBTreeIntSearch(table->networks,key);
}

/*
 * atacc_GetFreeNetworkEntry - Return a free entry or possibly create one.
 */
static atacctype*
atacc_GetFreeNetworkEntry (atacctable* table,createOps op)
{
    atacctype* entry;

    if (table == NULL)
	return (NULL);
    entry = (atacctype*)RBTreeGetFreeNode(table->networks);
    if ((entry != NULL) || (op != createIt))
	return entry;
    entry = malloc(sizeof(atacctype));
    if (entry == NULL) 
	printf(atacc_nomem,table->number);
    return (entry);
}
#if DEAD
/*
 * atacc_ProtectNetwork - Protect/unprotect a network entry from deletion
 */
static void
atacc_ProtectNetwork (atacctype* entry, protectOps op)
{
    if (entry == NULL)
	return;
    (void) RBTreeNodeProtect(&entry->link,op == protectIt);
}
#endif
/*
 * atacc_PrintNetworkDescr - Print a network descriptor
 */
static void
atacc_PrintNetworkDescr (atacctype* entry)
{
    if (entry == NULL)
	return;
    if (entry->high == 0)
	printf("network %d",entry->low);
    else
	printf("cable-range %d-%d",entry->low,entry->high);
}

/*
 * atalk_zonecheck
 * Perform an access check for zone lists
 * Returns TRUE if permitted, FALSE otherwise.
 */
boolean
atalk_zonecheck (ziptabletype* zone,int list)
{
    zipaccentry* z;
    atacctable *table;
  
    table = atacc_GetTable(list,mustExist);
    if (table==NULL)
	return (TRUE);
    z = atacc_GetZone(table,zone);
    if (z != NULL) {
	return (z->grant);
    }
    return (table->defzoneaccess);
}

/*
 * atalk_rtmpzonecheck
 * Perform an access check for zone lists with a klist of networks.
 * Returns TRUE if permitted, FALSE otherwise.
 */
boolean
atalk_rtmpzonecheck (ziptabletype* z, int list)
{
    zipnetentry *zn;
    atacctable *table;
  
    table = atacc_GetTable(list,mustExist);
    if ((table == NULL) || (z == NULL))
	return (TRUE);
    if (table->zonecount != 0) { /* If there's any zone access control */
	if (!atalk_zonecheck(z,list))
	    return (FALSE);	 /* This zone is access controlled */
	if (table->networks->activeNodes == 0)
	    return (TRUE);	 /* Since zones passed and no nets for */
				 /* list, we're good */
    }
    if (atalk_permitPartialZones)
	return (TRUE);		/* We assume the net whose zones we're */
				/* checking is valid */
    /* 
     * Since the zone is not explicitly controlled, we now check if it's
     * partially obscured due to access control in the list.  If any are
     * found, we deny access.  
     */
    zn = atalk_firstNet(z);
    if (zn == NULL)
	return (TRUE);		/* Weird, why are we here if zone wasn't */
				/* in use? */
    while (zn != NULL) {
	if (!atalk_routecheck(zn->net->dstrngstr, zn->net->dstrngend, list))
	    return (FALSE);	/* Some nets in this zone are not valid */
	zn = atalkzn_nextNet(zn);
    }
    return (TRUE);
}

/* atalk_gzlzonecheck
 * Perform an access check for GetZoneList replies.  Returns TRUE if zone
 * is permitted, FALSE otherwise.
 */
boolean
atalk_gzlzonecheck (ziptabletype *z, int list)
{
    zipnetentry *zn;
    boolean net_permit;

    if (list == ATFreeTradeZone)
	return (FALSE);

    zn = atalk_firstNet(z);
    while (zn != NULL) {
	net_permit = atalk_routecheck(zn->net->dstrngstr, zn->net->dstrngend,
				     list);
	if (!net_permit && !atalk_permitPartialZones)
	    return (FALSE);
	if (net_permit && atalk_permitPartialZones)
	    return (TRUE);
	zn = atalkzn_nextNet(zn);
    }
    /*
     * We've completed the loop across all nets in the zone name.
     * If we didn't return from the middle of the loop, then either:
     * 1. We were not permitting partial zones and all nets were
     *    permitted.  Permit the zone. (return true)
     * 2. We were permitting partial zones and all nets were denied.
     *    In this case, deny the zone. (return false)
     */
    if (!atalk_permitPartialZones)
	return (TRUE);
    else
	return (FALSE);
}

/*
 * atalk_routecheck
 * Perform an access check for routing updates
 * Returns TRUE if permitted, FALSE otherwise.
 */
boolean 
atalk_routecheck (ushort low, ushort high, int list)
{
    atacctype *entry;
    atacctable *table;
  
    table = atacc_GetTable(list,mustExist);
    if (table == NULL)
	return (TRUE);
    if ((table->zonecount > 0) && (table->networks->activeNodes == 0))
	return (TRUE);		/* Hold off until zone check is called */
    entry = atacc_GetNetworkEntry(table,IntervalKey(low,high));
    if (entry != NULL) {
	if (entry->exactmatch) {
	    if ((entry->low != low)||(entry->high != high))
		return (table->defaccess);
	} else if (entry->within) {
	    if ((entry->low > low)||(entry->high < high))
		return (table->defaccess);
	}
	return (entry->grant);
    }
    return (table->defaccess);
}

/*
 * atalk_accesscheck
 * Perform an access check for appletalk datagrams
 * Returns TRUE if permitted, FALSE otherwise.
 * isbroadcast is TRUE if the destination node address of the packet is a
 * broadcast.
 */
boolean 
atalk_accesscheck (ushort net, int list, boolean isbroadcast)
{
    atacctype *entry;
    atacctable *table;
  
    table = atacc_GetTable(list,mustExist);
    if (table == NULL)
	return (TRUE);
    entry = atacc_GetNetworkEntry(table,IntervalKey(net,0));
    if (entry != NULL) {
	return (isbroadcast ? entry->grant_broadcasts : entry->grant);
    }
    return (isbroadcast ? table->defbroadcastaccess : table->defaccess);
}

/*
 * atalk_ftzPortCheck
 *
 * Check if port is connected to a FTZ.  If so, we provide
 * maximum security (eg: usually deny all)
 */
boolean
atalk_ftzPortCheck (atalkidbtype* idb)
{
    if (idb && idb->atalk_gzlfilter == ATFreeTradeZone)
	return TRUE;
    return FALSE;
}

/*
 * atalk_priorityaccesscheck
 *
 * Perform access check for priority queuing
 */
boolean
atalk_priorityaccesscheck (paktype *pak, int list)
{
    atacctable *table;
    paktype *dup;

    table = atacc_GetTable(list,mustExist);
    if (table == NULL)
        return (TRUE);

    /* nbp filter */
    if (table->nbpacclist && atalk_pak_getpt(pak) == DDP_TYPE_NBP) {
        dup = atalk_pak_duplicate(pak);
	if (dup == NULL)
	    return (TRUE);
	dup = atalk_nbpcheck(dup, list);
	if (dup == NULL)
	    return (FALSE);
	datagram_done(dup);
    }
    /* zone list filter and network filter */
    if (atalk_sendcheck(atalk_pak_dnet(pak), list) &&
        atalk_accesscheck(atalk_pak_dnet(pak),  list,
            atutil_NodeIsBroadcast(atalk_pak_dnode(pak))))
        return (TRUE);
    return(FALSE);

}

/*
 * atalk_sendcheck
 *
 * Perform access check for atalk_send().  Check for zone filtering.
 */
boolean
atalk_sendcheck (ushort net, int list)
{
    atalk_rdb_t *entry;
    zipnetentry *zn;
    atacctable *table;
    rbTree	*tree;

    table = atacc_GetTable(list, mustExist);
    if (table == NULL)
	return (TRUE);
    tree = atroute_GetTree();
    entry = atroute_FindRoute(tree, net, FALSE);
    if (!entry)
	return TRUE;
    zn = atalk_firstZone(entry);
    if (zn == NULL) {
	ReleaseRoute(entry);
	return (TRUE);
    }

    /* This is broken and should not be done here... */
    if (table->zonecount != 0) /* If there's any zone access control */
	while (zn != NULL) {
	    if (   !atalk_zonecheck(zn->zone, list)  /* Explicit control? */
		|| !atalk_gzlzonecheck(zn->zone, list)) { /* Partial zone? */
		ReleaseRoute(entry);
		return (FALSE);
	    }
	    zn = atalkzn_nextZone(zn);
	}
    ReleaseRoute(entry);
    return (TRUE);			/* All zones passed muster. */
}
    
/*
 * atacc_DeleteTable - Delete or disable an access table.
 */
static void
atacc_DeleteTable (int number)
{
    atacctype* entry = NULL;
    zipaccentry* z = NULL;
    atacctable* table = atacc_GetTable(number,mustExist);
    if (table == NULL) 
	return;
    if (table->nbpacclist) {
        atacc_DeleteNBPList(table->nbpacclist);
	table->nbpacclist = NULL;
    }
    while ((z = atacc_GetFirstZone(table)) != NULL)
	atacc_DeleteZone(table,z->zone);
    while ((entry = atacc_GetFirstNetwork(table)) != NULL)
	RBTreeDelete(table->networks,&entry->link);
    while ((entry = atacc_GetFreeNetworkEntry(table,mustExist)) != NULL)
	free(entry);
    if (RBTreeFreeNodeCount(table->networks) == 0) {
	free(table->networks);
	free(table);
	accessarray[number] = NULL;
    } else {
	table->inactive = TRUE;
    }
}

static void
atacc_AddNetworkEntry (int number,atacctype* proto)
{
    atacctable* table = atacc_GetTable(number,createIt);
    atacctype* entry = NULL;
    if (table == NULL) 
	return;
    entry = atacc_GetNetworkEntry(table,IntervalKey(proto->low,proto->high));
    if (entry != NULL) {
	if ((entry->low != proto->low)||(entry->high != proto->high)) {
	    printf("\n%%Appletalk access list %d: ",number);
	    atacc_PrintNetworkDescr(proto);
	    printf(" overlaps ");
	    atacc_PrintNetworkDescr(entry);
	    return;
	}
    } else {
	entry = atacc_GetFreeNetworkEntry(table, createIt);
	if (entry == NULL) 
	    return;
	RBTreeIntInsert(proto->low,proto->high,table->networks,&entry->link);
    }
    entry->low = proto->low;
    entry->high = proto->high;
    entry->exactmatch = proto->exactmatch;
    entry->within = proto->within;
    entry->grant = proto->grant;
    entry->keyword = proto->keyword;
    entry->grant_broadcasts = proto->grant_broadcasts;
}

static void
atacc_DelNetworkEntry (int number,atacctype* proto)
{
    atacctable* table = atacc_GetTable(number,mustExist);
    atacctype* entry = NULL;
    if (table == NULL) 
	return;
    entry = atacc_GetNetworkEntry(table,IntervalKey(proto->low,proto->high));
    if (entry == NULL)
	return;
    if ((entry->low != proto->low)||(entry->high != proto->high))
	return;
    RBTreeDelete(table->networks,&entry->link);
}

/*
 * atacc_SetTableDefault - Set the default for other access
 */
static void
atacc_SetTableDefault (int number, boolean grant, boolean grantbroadcast)
{
    atacctable* table = atacc_GetTable(number,createIt);
    if (table == NULL) {
	printf("\nNo memory available for AppleTalk access list");
	return;
    }
    table->defaccess = grant;
    table->defbroadcastaccess = grantbroadcast;
}

/*
 * atacc_SetZoneDefault - Set the default for other zones 
 */
static void
atacc_SetZoneDefault (int number, boolean grant)
{
    atacctable* table = atacc_GetTable(number,createIt);
    if (table == NULL) {
	printf("\nNo memory available for AppleTalk access list");
	return;
    }
    table->defzoneaccess = grant;
}

/*
 * acc_HandleZone
 * Handle a parsed zone.
 */
static void
acc_HandleZone (boolean sense,
		int number,
		boolean grant,
		ushort zlen,
		uchar* zbuf)
{
    ziptabletype* zone = atzip_FindZone(zbuf,zlen);
    atacctable *acctab = atacc_GetTable(number,mustExist);
    zipaccentry* z = NULL;
    if (!sense) {
	if ((zone == NULL) || (acctab == NULL))
	    return;
	else
	    atacc_DeleteZone(acctab,zone);
	return;
    }
    if (zone == NULL)
	zone = atzip_CreateZone(zbuf,zlen);
    if (acctab == NULL)
	acctab = atacc_GetTable(number,createIt);
    if ((zone == NULL) || (acctab == NULL))
	return;
    z = atacc_GetZone(acctab,zone);
    if (z != NULL) {
	z->grant = grant;
	return;
    }
    (void) atacc_AddZone(acctab,zone,grant);
}

/*
 * atalkaccess_command
 * Parse an AppleTalk access list.
 *
 * Many of the below network lists accept the keyword 'broadcast-permit' or
 * 'broadcast-deny' at the end of the string.
 *
 *	"[no] access-list <list> permit|deny <net>"
 *	"[no] access-list <list> permit|deny additional-zones"
 *	"[no] access-list <list> permit|deny network <net>"
 *	"[no] access-list <list> permit|deny cable-range <low>-<high>"
 *	"[no] access-list <list> permit|deny includes <low>-<high>"
 *	"[no] access-list <list> permit|deny within <low>-<high>"
 *	"[no] access-list <list> permit|deny other-access"
 *	"[no] access-list <list> permit|deny zone <zone>"
 *  <zone> == * means all zones
 *
 *      "[no] access-list <list> permit|deny nbp other-nbps"
 *      "[no] access-list <list> permit|deny nbp <sequence> object|type|zone <string>"
 *
 */

void atalkaccess_command (parseinfo *csb)
{
    int zlen;
    int number;
    boolean grant;
    boolean grant_broadcasts;
    uchar zbuf[ZONELEN+1];
    atacctype proto;

    if (csb->nvgen) {
	atacctype *entry;
	atacctable *table;
	zipaccentry* z;
	int i;
	
	for (i = MINATALKACC; i <= MAXATALKACC; i++) {
	    table = atacc_GetTable(i,mustExist);
	    if (table != NULL) {
	        if (table->nbpacclist)
		    atacc_WriteNBPList(csb, i, table->nbpacclist);
		z = atacc_GetFirstZone(table);
		if (z != NULL) {
		    while (z != NULL) {
			nv_write(TRUE,"%s %d %s zone ", csb->nv_command, i,
				 z->grant ? "permit" : "deny");
			atalk_nv_printzone(z->zone->pname);
			z = atacc_GetNextZone(z);
		    }
		    nv_write(table->defzoneaccess||(table->zonecount > 0),
			     "%s %d %s additional-zones", csb->nv_command, i,
			     table->defzoneaccess ? "permit" : "deny");
		}
		entry = atacc_GetFirstNetwork(table);
		while (entry != NULL) {
		    nv_write(TRUE,"%s %d %s %s %d", 
			     csb->nv_command, i,
			     entry->grant ? "permit" : "deny",
			     at_cmd_lookup_str(entry->keyword),
			     entry->low);
		    nv_add((boolean)entry->high, "-%d", entry->high);
		    nv_add(entry->grant_broadcasts != entry->grant,
			   " %s", entry->grant_broadcasts ?
			   "broadcast-permit" : "broadcast-deny");
		    entry = atacc_GetNextNetwork(entry);
		}
		nv_write(TRUE,"%s %d %s other-access", 
			 csb->nv_command, i,
			 table->defaccess ? "permit" : "deny");
		nv_add(table->defaccess != table->defbroadcastaccess,
		       " %s", table->defbroadcastaccess ?
		       "broadcast-permit" : "broadcast-deny");
	    }
	}
	return;
    }

    /* Get access list number and permit/deny flag */
    number = GETOBJ(int,1);
    grant = GETOBJ(int,2);
    grant_broadcasts = GETOBJ(int,6) ? GETOBJ(int,7) : grant;

    proto.within = FALSE;
    proto.exactmatch = FALSE;
    proto.grant = grant;
    proto.grant_broadcasts = grant_broadcasts;
    proto.low = 0;
    proto.high = 0;
    proto.keyword = GETOBJ(int,3);
    zlen = 0;

    switch (GETOBJ(int,3)) {

      case ATACC_NBP_OBJ:
      case ATACC_NBP_TYPE:
      case ATACC_NBP_ZONE:
        if (csb->sense)
          atacc_AddNBPEntry(number, grant, GETOBJ(int,3),GETOBJ(int,4),GETOBJ(string,1));
	else
	  atacc_DelNBPEntry(number, GETOBJ(int,3), GETOBJ(int,4));
        return;

      case ATACC_NBP_OTHERS:
        if (csb->sense)
          atacc_AddNBPEntry(number, grant, GETOBJ(int,3), -1, NULL);
        else
          atacc_DelNBPEntry(number, GETOBJ(int,3), 0);
        return;

      case ATACC_DELETE:
	atacc_DeleteTable(number);
	return;

      case ATACC_NETWORK:
	proto.exactmatch = TRUE;
	proto.low = (ushort)GETOBJ(int,4);
	break;

      case ATACC_ANYZONES:
	atacc_SetZoneDefault(number,grant);
	return;

      case ATACC_CABLE:
	proto.low = (ushort)GETOBJ(int,4);
	proto.high = (ushort)GETOBJ(int,5);
	proto.exactmatch = TRUE;
	break;

      case ATACC_INCLUDES:
	proto.low = (ushort)GETOBJ(int,4);
	proto.high = (ushort)GETOBJ(int,5);
	break;

      case ATACC_OTHERS:
	atacc_SetTableDefault(number,grant,grant_broadcasts);
	return;

      case ATACC_WITHIN:
	proto.low = (ushort)GETOBJ(int,4);
	proto.high = (ushort)GETOBJ(int,5);
	proto.within = TRUE;
	break;

      case ATACC_ZONE:
	zlen = atalk_hex_filter(GETOBJ(string,1), (char *) zbuf, ZONELEN);
	if (zlen > 0)
	    acc_HandleZone(csb->sense,number,grant,zlen,zbuf);
	else
	    printf("\nAppleTalk zone required for access list");
	return;

      default:
	bad_parser_subcommand(csb, GETOBJ(int,3));
	return;		   

    }

    if ((proto.low == 0) || (proto.low >= ATALK_STARTUP_START) ||
	((proto.high != 0) && (proto.high < proto.low)) ||
	(proto.high >= ATALK_STARTUP_START)) {
	if (proto.high != 0)
	    printf("\nInvalid Appletalk access range (%d-%d)",proto.low,
		   proto.high);
	else
	    printf("\nInvalid Appletalk access network (%d)",proto.low);
	return;
    }
    if (csb->sense)
	atacc_AddNetworkEntry(number,&proto);
    else
	atacc_DelNetworkEntry(number,&proto);
}

/*
 * atacc_ShowTable - Display access list
 */
static void
atacc_ShowTable (int number)
{
    atacctype *entry;
    atacctable *table = atacc_GetTable(number, mustExist);
    zipaccentry *z;

    if (!table)
	return;

    printf("\nAppleTalk access list %d:", number);
    if (table->nbpacclist)
        atacc_ShowNBPList(table->nbpacclist);
    z = atacc_GetFirstZone(table);
    while (z != NULL) {
	printf("\n  %s zone %s", z->grant ? "permit" : "deny",
	       z->zone->pname);
	z = atacc_GetNextZone(z);
    }
    if ((table->zonecount > 0) || table->defzoneaccess)
	printf("\n  %s additional-zones",
	       table->defzoneaccess ? "permit" : "deny");
    entry = atacc_GetFirstNetwork(table);
    while (entry != NULL) {
	if (automore_quit())
	    return;
	printf("\n  %s %s %d", entry->grant ? "permit" : "deny",
	       at_cmd_lookup_str(entry->keyword), entry->low);
	if (entry->high != 0)
	    printf("-%d", entry->high);
	if (entry->grant_broadcasts != entry->grant)
	    printf(" %s", entry->grant_broadcasts ? "broadcast-permit" :
		   "broadcast-deny");
	entry = atacc_GetNextNetwork(entry);
    }
    if (automore_quit())
	return;
    printf("\n  %s other-access", table->defaccess ? "permit" : "deny");
    if (table->defbroadcastaccess != table->defaccess)
	    printf(" %s", table->defbroadcastaccess ? "broadcast-permit" :
		   "broadcast-deny");
    return;
}

/*
 * show_atalkaccess_shell
 *
 * Wrap an automore shell around show_atalkaccess so that show_access()
 * doesn't get all ruffled by our use of automore().
 */
void
show_atalkaccess_shell (int list)
{
    automore_enable(NULL);
    show_atalkaccess(list);
    automore_disable();
}    

/*
 * show_atalkaccess
 * Show AppleTalk access lists
 */
void
show_atalkaccess (int list)
{
    int lst;
    atacctable *table;

    if (list > 0) {
	if ((list < MINATALKACC) || (list > MAXATALKACC))
	    return;

	table = atacc_GetTable(list, mustExist);
	if (table == NULL) {
	    printf("\n%%No AppleTalk access list %d", list);
	    return;
	}
	atacc_ShowTable(list);
	return;
    }
    for (lst = MINATALKACC; lst <= MAXATALKACC; lst++) {
	if (!automore_quit())
	    atacc_ShowTable(lst);
	else
	    return;
    }
}


