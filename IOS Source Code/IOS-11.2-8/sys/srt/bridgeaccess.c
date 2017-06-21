/* $Id: bridgeaccess.c,v 3.3.62.6 1996/08/28 13:15:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/srt/bridgeaccess.c,v $
 *------------------------------------------------------------------
 * bridgeaccess.c -- Extended ("Slow") Bridging Access Lists
 *
 * 12-May-92, Paul Traina
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bridgeaccess.c,v $
 * Revision 3.3.62.6  1996/08/28  13:15:42  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.5  1996/08/14  02:36:34  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.3.62.4  1996/07/26  00:54:04  hampton
 * Fix the automore code in show access-list.  [CSCdi61084]
 * Branch: California_branch
 *
 * Revision 3.3.62.3  1996/06/28  23:26:12  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/05/30  23:45:25  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/03/18  22:10:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:50:25  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  18:45:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:52:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:41:44  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:30:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:00:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../srt/srt_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "access.h"
#include "logger.h"
#include "../tbridge/msg_tbridge.c"	/* Not a typo, see logger.h */
#include "config.h"
#include "../if/ether.h"
#include "parser.h"
#include "../parser/parser_defs_access.h"
#include "../srt/bridgeaccess.h"

typedef struct xmacacctype_ {
        struct xmacacctype_ *next;	/* link to next entry */
	mac_address_type srcaddr;	/* source MAC address */
	mac_address_type srcmask;	/* mask of wildcarded address bits */
	mac_address_type dstaddr;	/* destination MAC address */
	mac_address_type dstmask;	/* mask of wildcarded address bits */
	ulong operand;			/* value to compare against */
	boolean grant;			/* TRUE if permit, FALSE if deny */
	ushort offset;			/* offset into packet of read value */
	ushort size;			/* size of read value */
	ushort opr;			/* operator name */
} xmacacctype;

static inline xmacacctype *mac_get_ext_acl (int number)
{
    return((xmacacctype *)accessarray[number]);
}

static inline void mac_set_ext_acl (int number, xmacacctype *acl)
{
    accessarray[number] = (accessptr *)acl;
}

/*
 * Table used to map extended access list operators to their names.
 */

static id_string bridge_access_operators[] = {
    {OPC_NONE, OPC_NONE_STR},
    {OPC_LT, OPC_LT_STR},
    {OPC_GT, OPC_GT_STR},
    {OPC_EQ, OPC_EQ_STR},
    {OPC_NEQ, OPC_NEQ_STR},
    {OPC_AND, OPC_AND_STR},
    {OPC_XOR, OPC_XOR_STR},
    {0, NULL}
};

static char *bridge_access_opstr (int opr)
{
    char *opstr;

    opstr = lookup_id_string(bridge_access_operators, opr);
    if (opstr) {
	return(opstr);
    } else {
	return("???");
    }
}

typedef ulong (*xmac_get_data_routine_t)(void*, int, short);
/*
 * xmacaccesscheck_inline
 * Perform an access check on a packet.
 * Returns TRUE to send packet, FALSE otherwise.
 *
 *  int list                        xmac access list
 *
 *  mac_address_type *dest,         destination MAC address
 *
 *  mac_address_type *source,       source MAC address
 *
 *  uchar *data,                    pointer to  address in the packet.
 *
 *  int data_offset,                offset of data counting from 
 *                                  the start of dest MAC address
 *
 *  int  size,                      size of data i.e from data to the
 *                                  end of the packet. Optional parameter.
 *                                  If not set no saniny check is done
 *                                     against item->offset
 * 
 *  xmac_get_data_routine_t *routine The other way to get data if data
 *                                      parameter is not set
 *                               
 *  void *first_parameter,         first parameter of rotine
 *                                 oher two parameters are  :
 *                                 item->offset and  item->size
 */

static boolean xmacaccesscheck_inline (
    int list,
    mac_address_type *dest,
    mac_address_type *source,
    uchar *data,
    int data_offset,
    int data_size,
    xmac_get_data_routine_t routine,
    void *first_parameter)
{
    xmacacctype *item;
    ushort us_mask;
    ulong  ul_mask;
    ulong  value;

    /*
     * Find the access list.  Return TRUE (pass the packet)
     * if the list doesn't exist or is illegal.
     */

    if (list < MINXMACACC || list > MAXXMACACC) {
	errmsg(&msgsym(BADXMAC, TBRIDGE), list);
	return(TRUE);
    }
    item = mac_get_ext_acl(list);
    if (item == NULL) {
	errmsg(&msgsym(BADXMAC, TBRIDGE), list);
	return(TRUE);
    }

    /*
     * Linear scan through list, look for a match.
     */
    for ( ; item; item = item->next) {
	/*
	 * this is in weird order for a reason... it's best to check the
	 * low bits first, in hopes of an early match failure... and we
	 * hope that the DA was on a word boundary so we can tune the memory
	 * access to be as fast as possible (or not break a RISC)
	 */
	us_mask = ~item->dstmask.u.sword[2];	/* last two bytes of DA */
	if (!((GETSHORT(&dest->u.sword[2]) & us_mask) == 
	      item->dstaddr.u.sword[2]))
	  continue;
	us_mask = ~item->srcmask.u.sword[2];	/* last two bytes of SA */
	if (!((GETSHORT(&source->u.sword[2]) & us_mask) == 
	      item->srcaddr.u.sword[2]))
	  continue;
	ul_mask = ~item->dstmask.u.lword[0];	/* first four bytes of DA */
	if (!((GETLONG(&dest->u.lword[0]) & ul_mask) == 
	      item->dstaddr.u.lword[0]))
	  continue;
	ul_mask = ~item->srcmask.u.lword[0];	/* first four bytes of SA */
	if (!((GETLONG(&source->u.lword[0]) & ul_mask) == 
	      item->srcaddr.u.lword[0]))
	  continue;

	if (item->opr == OPC_NONE || (!data && !routine))
	   return(item->grant);
	if (data) {
	    /* this isn't as quite as slow as it looks...
	       but it is a piece of crap and should be improved on somehow */
	    ushort size = item->size;
	    uchar *p = data + item->offset - data_offset;
	    if ((data_size != 0) &&
		(size + item->offset > data_size + data_offset)){
		return(item->grant);
            }
	    if ((data_offset != 0) &&
		(item->offset < data_offset)) {
		return(item->grant);
	    }
	    value = *p++;
	    while (--size)
	      value = value<<8 | *p++;
	} else {
	    value = (routine)(first_parameter, item->offset,
							  item->size);
	}
		
	switch (item->opr) {
	  case OPC_LT:
	    if (value < item->operand)
	      return(item->grant);
	    break;
	  case OPC_GT:
	    if (value > item->operand)
	      return(item->grant);
	    break;
	  case OPC_EQ:
	    if (value == item->operand)
	      return(item->grant);
	    break;
	  case OPC_NEQ:
	    if (value != item->operand)
	      return(item->grant);
	    break;
	  case OPC_AND:
	    if (value & item->operand)
	      return(item->grant);
	    break;
	  case OPC_XOR:
	    if (value ^ item->operand)
	      return(item->grant);
	    break;
	  default:
	    return(FALSE);
	}
	continue;
    }
    return(FALSE);
}

boolean xmacaccesscheck_gen (
    int list,
    mac_address_type *dest,
    mac_address_type *source,
    uchar *data,
    int data_offset,
    int data_size)
{
    return (xmacaccesscheck_inline(list,dest, source,  data, data_offset,
				   data_size, 0, 0));
}


/*
 * xmacaccesscheck
 * Perform an access check on a bridged incoming or outgoing packet.
 * Returns TRUE to send packet, FALSE otherwise.
 * "datagram" parameter has to be valid. It is a pointer of a packet
 * 	buffer with either MAC header only or the whole content.
 * If "registry_read" parameter is TRUE, we have to read extra data by  
 * 	invoking a registry since we have MAC header only.
 */

boolean xmacaccesscheck (
    hwidbtype *idb,
    int list,
    uchar *datagram,
    boolean registry_read)
{
    xmacacctype *item;
    ushort us_mask;
    ulong  ul_mask;
    mac_address_type *source, *dest;
    ulong  value;

    if (!datagram)
	return(TRUE);

    /*
     * Find the access list.  Return TRUE (pass the packet)
     * if the list doesn't exist or is illegal.
     */

    if (list < MINXMACACC || list > MAXXMACACC)
	return(TRUE);

    dest   = (mac_address_type *) datagram;
    source = (mac_address_type *) (datagram + IEEEBYTES);

    /*
     * Linear scan through list, look for a match.
     */
    for (item = mac_get_ext_acl(list); item; item = item->next) {
	/*
	 * this is in weird order for a reason... it's best to check the
	 * low bits first, in hopes of an early match failure... and we
	 * hope that the DA was on a word boundary so we can tune the memory
	 * access to be as fast as possible (or not break a RISC)
	 */
	us_mask = ~item->dstmask.u.sword[2];	/* last two bytes of DA */
	if (!((GETSHORT(&dest->u.sword[2]) & us_mask) == 
	      item->dstaddr.u.sword[2]))
	  continue;
	us_mask = ~item->srcmask.u.sword[2];	/* last two bytes of SA */
	if (!((GETSHORT(&source->u.sword[2]) & us_mask) == 
	      item->srcaddr.u.sword[2]))
	  continue;
	ul_mask = ~item->dstmask.u.lword[0];	/* first four bytes of DA */
	if (!((GETLONG(&dest->u.lword[0]) & ul_mask) == 
	      item->dstaddr.u.lword[0]))
	  continue;
	ul_mask = ~item->srcmask.u.lword[0];	/* first four bytes of SA */
	if (!((GETLONG(&source->u.lword[0]) & ul_mask) == 
	      item->srcaddr.u.lword[0]))
	  continue;

	if (item->opr == OPC_NONE)
	   return(item->grant);

	if (!registry_read) {
	    /* this isn't as quite as slow as it looks...
	       but it is a piece of crap and should be improved on somehow */
	    ushort size = item->size;
	    uchar *p = datagram + item->offset;

	    value = *p++;
	    while (--size)
	      value = value<<8 | *p++;
	} else {
	    /*
	     * If the packet is coming from MultiBus Token Ring, bypass the
	     * check of data after the MAC addresses and send the packet 
	     * since for now we don't have a clean way to read extra data 
	     * from MultiBus.
	     */
	    if ((idb->status & IDB_MCI) || (idb->status & IDB_CBUS))
		value = reg_invoke_tbridge_read_bytes(idb, datagram,
		    item->offset, item->size);
	    else
		return(TRUE);
	}
	
	switch (item->opr) {
	  case OPC_LT:
	    if (value < item->operand)
	      return(item->grant);
	    break;
	  case OPC_GT:
	    if (value > item->operand)
	      return(item->grant);
	    break;
	  case OPC_EQ:
	    if (value == item->operand)
	      return(item->grant);
	    break;
	  case OPC_NEQ:
	    if (value != item->operand)
	      return(item->grant);
	    break;
	  case OPC_AND:
	    if (value & item->operand)
	      return(item->grant);
	    break;
	  case OPC_XOR:
	    if (value ^ item->operand)
	      return(item->grant);
	    break;
	  default:
	    return(FALSE);
	}
    }
    return(FALSE);
}

/*
 * xmacaccess_command
 * Parse an extended bridgeing access-list command line.
 *
 *  "access-list <list> permit|deny <src> <srcmask> <dst> <dstmask>
 *				    <offset> <size> <operator> <operand>
 */

void xmacaccess_command (parseinfo *csb)
{
    int number, i;
    boolean grant;
    xmacacctype local_item, *new;
    xmacacctype *item, *wrk_item;

    if (csb->nvgen) {
	for (i = MINXMACACC; i <= MAXXMACACC; i++) {
	    item = mac_get_ext_acl(i);
	    while (item) {
		nv_write(TRUE,"%s %d %s %e %e %e %e", csb->nv_command, i,
			 item->grant ? "permit" : "deny  ",
			 &item->srcaddr.u.sword[0],
			 &item->srcmask.u.sword[0],
			 &item->dstaddr.u.sword[0],
			 &item->dstmask.u.sword[0]);
		nv_add(item->opr != OPC_NONE, " 0x%x %d %s 0x%lx", 
			 item->offset, item->size,
			 bridge_access_opstr(item->opr),
			 item->operand);
		item = item->next;
	    }
	}
	return;
    }

    /*
     * Get list number
     */
    number = GETOBJ(int,1);

    /*
     * If parsing "no access-list <list>", stop now to delete list.
     */
    if (csb->sense == FALSE) {
	item = mac_get_ext_acl(number);
	mac_set_ext_acl(number, NULL);
	while (item) {
	    wrk_item = item->next;
	    free(item);
	    item = wrk_item;
	}
	return;
    }

    grant = (boolean) GETOBJ(int,2);

    new = &local_item;

    new->grant = grant;
    
    /*
     * Get source address and wildcard mask
     */
    ieee_copy(GETOBJ(hwaddr,1)->addr, &new->srcaddr.u.byte[0]);
    ieee_copy(GETOBJ(hwaddr,2)->addr, &new->srcmask.u.byte[0]);

    /*
     * Get dest address and wildcard mask
     */
    ieee_copy(GETOBJ(hwaddr,3)->addr, &new->dstaddr.u.byte[0]);
    ieee_copy(GETOBJ(hwaddr,4)->addr, &new->dstmask.u.byte[0]);

    /*
     * Clear any bits in the etheraddress that are set in the mask
     * so that comparisons work as expected:
     */
    new->srcaddr.u.lword[0] &= ~new->srcmask.u.lword[0];
    new->srcaddr.u.sword[2] &= ~new->srcmask.u.sword[2];
    new->dstaddr.u.lword[0] &= ~new->dstmask.u.lword[0];
    new->dstaddr.u.sword[2] &= ~new->dstmask.u.sword[2];

    /*
     * Get the offset into the packet
     */
    new->offset = GETOBJ(int,3);
    if (new->offset < IEEEBYTES * 2  && (GETOBJ(int,5) != OPC_NONE))
	printf("\nWarning: extended access lists may be unnecessary");

    if (new->offset > MINETHERSIZE)
        printf("\nWarning: unpredictable results with offset larger than"
	       "\nminimum ethernet packet size.");

    /*
     * Get the size of the region we're going to compare to
     */
    new->size = GETOBJ(int,4);

    /*
     * Get operator
     */
    new->opr = GETOBJ(int,5);

    /*
     * Get operand
     */
    new->operand = GETOBJ(int,6);
     
    /*
     * Check to ensure that an entry does not exist that exactly matchs what
     * we are attempting to add. If it does, exit stage left.
     */
    item = mac_get_ext_acl(number);
    wrk_item = mac_get_ext_acl(number);
    while (item) {
	if (!bcmp(((uchar *)item)+sizeof(xmacacctype *),
		  ((uchar *)new)+sizeof(xmacacctype *),
		  sizeof(xmacacctype)-sizeof(xmacacctype *)))
	  return;
	wrk_item = item;
	item = item->next;
    }	
    
    /*
     * Construct the item descriptor
     */
    item = malloc(sizeof(xmacacctype));
    if (!item) {
	printf(nomemory);
	return;
    }
    bcopy((uchar *) &local_item, (uchar *)item, sizeof(xmacacctype));
    item->next = NULL;
    if (!wrk_item)
	mac_set_ext_acl(number, item);
    else
	wrk_item->next = item;
}

/*
 * show_xmacaccess
 * Show the extended bridging access lists
 */

void show_xmacaccess (int listnum)
{
    int i;
    xmacacctype *item;
    
    for (i = MINXMACACC; i <= MAXXMACACC; i++) {
	if ((listnum > 0) && (i != listnum))
	    continue;
	item = mac_get_ext_acl(i);
	if (!item)
	    continue;
	if (automore_quit())
	    return;
	printf("\nExtended Bridging access list %d", i);
	for (; item; item = item->next) {
	    if (!mac_get_ext_acl(i) || !validlink(item->next))
		break;
	    printf("\n    %s %e %e %e %e",
		   item->grant ? "permit" : "deny  ",
		   &item->srcaddr.u.sword[0],
		   &item->srcmask.u.sword[0],
		   &item->dstaddr.u.sword[0],
		   &item->dstmask.u.sword[0]);
	    if (item->opr != OPC_NONE)
		printf("\n           0x%x %d %s 0x%lx",
		       item->offset, item->size,
		       bridge_access_opstr(item->opr),
		       item->operand);
	}
    }
}

/*
 * show_xmacaccess_shell
 *
 * Wrap an automore shell around show_xmacaccess so that show_access()
 * doesn't get all ruffled by our use of automore().
 */
void
show_xmacaccess_shell (int list)
{
    automore_enable(NULL);
    show_xmacaccess(list);
    automore_disable();
}    
