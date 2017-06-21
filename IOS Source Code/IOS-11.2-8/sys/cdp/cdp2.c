/* $Id: cdp2.c,v 3.2.58.4 1996/07/15 16:37:19 tylin Exp $:
 * $Source: /release/112/cvs/Xsys/cdp/cdp2.c,v $
 *------------------------------------------------------------------
 * Support for CDP (Cisco Discovery Protocol)
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cdp2.c,v $
 * Revision 3.2.58.4  1996/07/15  16:37:19  tylin
 * CSCdi62802:  cdp crashes from invalid packets
 *    Only record the first 19 characters port_id and platform string.
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/06/28  07:20:45  tylin
 * CSCdi60805:  7513 restarted by error - a SegV exception, PC 0x600EBE98
 *    Avoid sign extension problem for cdp_hash().
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/04/15  21:13:04  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/18  19:01:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  08:39:25  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  13:39:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:49:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  23:16:02  hampton
 * Miscellaneous timer cleanups for CDP.  [CSCdi37215]
 *
 * Revision 2.2  1995/06/18 19:39:10  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:14:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "ieee.h"
#include "address.h"
#include "../ui/debug.h"
#include "cdp_externs.h"
#include "cdp_debug.h"
#include "../clns/clns.h"


/*
 * cdp_hash 
 * 
 * Hash a device ID entry, and return hash index. Hash function is standard
 * IP hash function.
 */
cdp_hash_type cdp_hash (char *entry, int length)
{
    cdp_hash_type hash;
    
    if (length <= 0)
	return (0);

    hash = 0;
    while (length--) {
	hash ^= *entry++;
    }
    return(hash);
}

/*
 * cdp_walk_hash
 * 
 * Return a pointer to the next entry in the table. If entry passed in is NULL,
 * start from the first entry in the table.
 */
cdp_cache_type *cdp_walk_hash (int *hash_index, cdp_cache_type *entry) 
{
    if (entry) {
	entry = entry->next;
	if (!entry)
	    *hash_index += 1;
    }

    while (TRUE){
	if (entry)
	    return(entry);
	while ((entry = cdp_table[*hash_index]) == 0){
	    *hash_index += 1;
	    if (*hash_index >= NETHASHLEN)
		return(NULL);
	}
    }
}

/*
 * cdp_hash_insert
 * 
 * Insert entry into hash table.
 */
void cdp_hash_insert (cdp_cache_type *entry)
{
    cdp_hash_type h;
    ulong device_id;

    /*
     * Return if entry already exists.
     */
    if (entry && cdp_hash_lookup(entry->entry_name,
				 entry->entry_name_len,
				 entry->idb, &device_id))
	return;
    /*
     * Return if entry name is NULL
     */
    if (!entry->entry_name)
	return;

    h = cdp_hash(entry->entry_name, entry->entry_name_len);
    entry->next = cdp_table[h];
    cdp_table[h] = entry;
}

/*
 * cdp_hash_lookup
 * 
 * Return entry from hash table, if found. Else return NULL. If we find a
 * hash table entry with the same name, we set value of 'device_num' to the
 * device number found in the hash entry. This way, we will have the same
 * device number value for a given device identifier.
 */
cdp_cache_type *cdp_hash_lookup (char *entry, uchar name_len, idbtype *idb,
				 ulong *device_num)
{
    cdp_hash_type h;
    cdp_cache_type *hash_entry;

    *device_num = 0;
    if (!entry)
	return (NULL);

    h = cdp_hash(entry, name_len);
    for (hash_entry = cdp_table[h]; hash_entry;
	                                      hash_entry = hash_entry->next) {
	if ((hash_entry->entry_name_len != name_len) ||
	    (strncmp(hash_entry->entry_name, entry, name_len) != 0)) {
	    continue;
        }
	/*
	 * Names match. Set value of 'device_num' to the device number
	 * present in the hash entry. Then see if idb's match.
	 */
	*device_num = hash_entry->device_number;
	if (hash_entry->idb == idb)
	    break;
    }
    return(hash_entry);
}

/*
 * Define all the CDP TLV decode functions, indexed by TLV type code
 */

cdp_decode_func_t cdp_decode_func[] = {
    NULL,
    cdp_handle_device_info,
    cdp_handle_address_info,
    cdp_handle_port_id_info,
    cdp_handle_capabilities_info,
    cdp_handle_version_info,
    cdp_handle_platform_info,
    NULL
};

/*
 * cdp_insert_packet_info
 * 
 * Insert item information into cache entry. If the flag 'new_entry' is
 * TRUE, insert into hash table also. Further, this routine assumes that all
 * necessary validity checks have been made on the packet, so none will be made
 * here.
 */
void cdp_insert_packet_info (cdp_hdrtype *cdphdr,
			     uint pak_info_len,
			     idbtype *idb,
			     cdp_cache_type *cache_entry,
			     boolean new_entry)
{
    int bytes_left;
    cdp_info_type *item;
    ushort item_length, tlv_code;

    /*
     * More sanity checks
     */
    if (!cache_entry)
	return;
    /*
     * Set input IF before doing TLV processing.
     */
    if (new_entry)
	cache_entry->idb = idb;

    bytes_left = pak_info_len - CDP_HEADER_OVERHEAD;
    item = (cdp_info_type *)cdphdr->item;
    while (bytes_left > 0) {
	tlv_code = GETSHORT(&item->code);
	if (CDP_TYPE_KNOWN(tlv_code) && cdp_decode_func[tlv_code])
	    (*cdp_decode_func[tlv_code])(item, cache_entry, new_entry);
	item_length = GETSHORT(&item->length);
	bytes_left -= item_length;
        item = (cdp_info_type *)((char *)item + item_length);
    }
    
    /*
     * Update TTL
     */
    TIMER_START(cache_entry->expiration_time, cdphdr->ttl*ONESEC);

    /*
     * If new entry, enter into hash table. 
     */
    if (new_entry) {
	cdp_hash_insert(cache_entry);
    }
    return;
}

/*
 * cdp_handle_device_info
 *
 * Add device ID info to cache entry. If the flag 'new_entry' is TRUE, it
 * means we have a new entry.
 */
void cdp_handle_device_info (cdp_info_type *item, cdp_cache_type *cache_entry,
	 		        boolean new_entry)
{

    ushort data_length;

    data_length = GETSHORT(&item->length) - CDP_TLV_OVERHEAD;
    /*
     * Entering a device ID makes sense only for new entries, or if the name
     * is NULL.
     */
     if (new_entry || (!cache_entry->entry_name)) {
	 /*
	  * Allocate extra byte for NULL terminator
	  */
         cache_entry->entry_name = malloc(data_length + 1);
	 if (!cache_entry->entry_name) {
	    cdp_traffic[CDP_NOMEM]++;
	    return;
	 }
	 cache_entry->entry_name_len = data_length;
         bcopy(item->value, cache_entry->entry_name, data_length);
	 /*
	  * Insert NULL terminator for string
	  */
	cache_entry->entry_name[data_length] = '\0'; 
     }

     return;
}

/*
 * cdp_handle_address_info
 *
 * Add address info to cache entry. 
 */
void cdp_handle_address_info (cdp_info_type *item,
			      cdp_cache_type *cache_entry, boolean dummy)
{
    uchar *temp, *cache_buffer;
    cdp_addr_info *cdp_addr;
    int num_addr, value_len, addr_len;
    snap_hdr *snap;

    num_addr = GETLONG(item->value);
  
    /* 
     * Free existing buffer, if it exists.
     */
    if (cache_entry->addr) {
	free (cache_entry->addr);
    }
  
    /*
     * allocate space to hold new address info
     */
    cache_entry->addr = malloc(CDP_ADDRESS_OVERHEAD +
			       (num_addr * sizeof(struct addrtype_)));
    if (!cache_entry->addr) {
        cdp_traffic[CDP_NOMEM]++;
	return;	    
    }
    cdp_addr = (cdp_addr_info *)cache_entry->addr;
    cdp_addr->num_addr = num_addr;

    cache_buffer = cdp_addr->data;

    /* skip over 'num addresses' field */
    temp = (char *)item->value + CDP_ADDRESS_OVERHEAD;
    value_len = GETSHORT(&item->length) - CDP_TLV_OVERHEAD -
	                                               CDP_ADDRESS_OVERHEAD;

    while (num_addr > 0) {
	if (value_len <= 0)
	    break;

	switch(*temp) {
	    case CDP_NLPID_FORMAT:
		switch(*(temp+2)) {
		    case CDP_IP_NLPID:
		        cdp_handle_ip_address(&value_len, &cache_buffer,
					      &temp, &num_addr);
		        break;
		    case CDP_CLNS_NLPID:
		        cdp_handle_clns_address(&value_len, &cache_buffer,
						&temp, &num_addr);
		        break;
		    default:
			/* skip the info */
			addr_len = GETSHORT(temp + CDP_NLPID_INFO_OVERHEAD);
			temp += CDP_NLPID_INFO_OVERHEAD +
			                  CDP_ADDR_LEN_FIELD_LEN + addr_len;
			value_len -= (CDP_NLPID_INFO_OVERHEAD +
				          CDP_ADDR_LEN_FIELD_LEN + addr_len);
			num_addr--;
		        cdp_evbuginf("\nCDP-EV: Invalid NLPID (%d)", *temp);
		        cdp_traffic[CDP_BADPACKET]++;
		}
		break;
	    case CDP_8022_FORMAT:
		/*
		 * 802.2 format addresses are of the form AAAA03 000000 xxxx,
		 * where 'xxxx' identifies the protocol. We will de-multiplex
		 * based on these last two bytes, which correspond to the
		 * ethernet type code.
		 */
		    snap = (snap_hdr *)(temp + 2);
		    switch (GETSHORT(&snap->type)) {
		        case TYPE_DECNET:
		            cdp_handle_decnet_address(&value_len,
						      &cache_buffer, &temp,
						      &num_addr);
			    break;
		        case TYPE_NOVELL1:
		            cdp_handle_novell_address(&value_len,
						      &cache_buffer, &temp,
						      &num_addr);
		            break;
		        case TYPE_ETHERTALK:
		            cdp_handle_apple_address(&value_len,
						     &cache_buffer, &temp,
						     &num_addr);
		            break;
			default:
			    /* skip the info */
			    addr_len = GETSHORT(temp + CDP_8022_INFO_OVERHEAD);
			    temp += CDP_8022_INFO_OVERHEAD +
				           CDP_ADDR_LEN_FIELD_LEN + addr_len;
			    value_len -= (CDP_8022_INFO_OVERHEAD +
					   CDP_ADDR_LEN_FIELD_LEN + addr_len);
			    num_addr--;
			    cdp_evbuginf("\nCDP-EV: Invalid protocol type (%d)"
					 , *temp);
			    cdp_traffic[CDP_BADPACKET]++;

			}
		break;
	    default:
		/*
		 * We need to return because we don't know how many bytes
		 * to skip.
		 */
		return;
	}
    }

    return;
}

/*
 * cdp_handle_ip_address
 *
 * Handle IP address information. The argument 'info' will point at the
 * beginning of the address encoding information, i.e. at 'PT'.
 * 'protocol info' fields. The function will also advance the pointer past
 * the end of the address. 'Value_len' is the length of the entire 'value'
 * part, starting from the 'PT'. Input info is copied to the buffer
 * 'cache_addr', which is then advanced accordingly. NO length check is done
 * for this buffer, since we have made sure that we have allocated enough
 * space.
 *
 * If we see something that is not valid, we advance the 'info' pointer to
 * the end of the information i.e. we 'step over'.
 */
void cdp_handle_ip_address (int *value_len, uchar **cache_buffer,
	 	            uchar **info, int *num_addr)
{
    uchar *temp, *output_buf;

    temp = *info;

    /*
     * Scan the IP info fields to make sure it's all valid. Also, determine
     * length of buffer we need to allocate.
     */
	    
    /* Protocol type must correspond to NLPID format */
    if (*temp != CDP_NLPID_FORMAT) {
        cdp_evbuginf("\nCDP-EV: Invalid NLPID format (%d)", *temp);
	cdp_traffic[CDP_BADPACKET]++;

	/* 'step over' if we don't like this information */
	*info += CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + ADDRLEN_IP;

	return;
    }

    temp++;

    /* Protocol type length must be 1 */
    if (*temp != CDP_NLPID_LEN) {
        cdp_evbuginf("\nCDP-EV: Invalid NLPID length (%d)", *temp);
	cdp_traffic[CDP_BADPACKET]++;

	/* 'step over' if we don't like this information */
	*info += CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + ADDRLEN_IP;

	return;
    }

    temp++;    

    /* We're interested in IP here */
    if (*temp != CDP_IP_NLPID) {
        cdp_evbuginf("\nCDP-EV: Expecting IP NLPID instead of (%d)", *temp);
	cdp_traffic[CDP_BADPACKET]++;

	/* 'step over' if we don't like this information */
	*info += CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + ADDRLEN_IP;

	return;
    }

    temp++;

    if (GETSHORT(temp) != ADDRLEN_IP) {
        cdp_evbuginf("\nCDP-EV: Invalid IP address length (%d)",
		     GETSHORT(temp));
	cdp_traffic[CDP_BADPACKET]++;

	/* 'step over' if we don't like this information */
	*info += CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + ADDRLEN_IP;

	return;
    }
    
    /*
     * the 'address length' field is 2 bytes. Advancing 'temp' will make it
     * point to the IP address
     */
    temp += CDP_ADDR_LEN_FIELD_LEN;

    output_buf = *cache_buffer;
  
    /* 
     * fill in address type, length and then value. See 'struct addrtype_'
     * in 'address.h' for format of this info.
     */
    *output_buf++ = ADDR_IP;
    *output_buf++ = ADDRLEN_IP;  
    PUTLONG(output_buf, GETLONG(temp));

    /* Advance 'info' */
    *info += CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + ADDRLEN_IP;
	
    /* Advance output buffer */
    *cache_buffer += (2 + ADDRLEN_IP);

    /* Decrement number of addresses */
    (*num_addr)--;

    /* 
     * Decrement 'value_len' by the length of this information, to
     * reflect the remaining size of the input buffer.
     */

    (*value_len) -= CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                           ADDRLEN_IP;

    return;
}

/*
 * cdp_handle_clns_address
 *
 * Handle CLNS address information. The argument 'info' will point at the
 * beginning of the address encoding information, i.e. at 'PT'.
 * 'protocol info' fields. The function will also advance the pointer past
 * the end of the address, and after it has parsed the CLNS information,
 * will decrement 'value_len' accordingly. Input info is copied to the buffer
 * 'cache_addr', which is then advanced accordingly. NO length check is done
 * for this buffer, since we have made sure that we have allocated enough
 * space.
 *
 * If we see something that is not valid, we advance the 'info' pointer to
 * the end of the information i.e. we 'step over'.
 */
void cdp_handle_clns_address (int *value_len, uchar **cache_buffer,
			      uchar **info, int *num_addr)
{
    uchar *temp, *output_buf;
    ushort nsap_len;

    temp = *info;

    /*
     * We need this here, so that we can step over the information, in case
     * there is invalid information.
     */
    nsap_len = GETSHORT(temp + CDP_NLPID_INFO_OVERHEAD);

    /*
     * Scan the CLNS info fields to make sure it's all valid. 
     */
	    
    /* Protocol type must correspond to NLPID format */
    if (*temp != CDP_NLPID_FORMAT) {
        cdp_evbuginf("\nCDP-EV: Invalid NLPID format (%d)", *temp);

	/* 'step over' if we don't like this information */
	*info += CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + nsap_len;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    temp++;

    /* Protocol type length must be 1 */
    if (*temp != CDP_NLPID_LEN) {
        cdp_evbuginf("\nCDP-EV: Invalid NLPID length (%d)", *temp);

	/* 'step over' if we don't like this information */
	*info += CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + nsap_len;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    temp++;    

    /* We're interested in CLNS here */
    if (*temp != CDP_CLNS_NLPID) {
        cdp_evbuginf("\nCDP-EV: Expecting CLNS NLPID instead of (%d)", *temp);

	/* 'step over' if we don't like this information */
	*info += CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + nsap_len;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    temp++;

    if ((nsap_len < CLNS_MIN_NSAP_LENGTH) ||
	(nsap_len > CLNS_MAX_NSAP_LENGTH)) {
	    cdp_evbuginf("\nCDP-EV: Invalid CLNS address length (%d)",
			 nsap_len);

	    /* 'step over' if we don't like this information */
	    *info += CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
		                                                    nsap_len;

	    cdp_traffic[CDP_BADPACKET]++;
	    return;
    }

    /*
     * the 'address length' field is 2 bytes. Advancing 'temp' will make it
     * point to the CLNS address
     */
    temp += CDP_ADDR_LEN_FIELD_LEN;

    output_buf = *cache_buffer;
  
    /* 
     * fill in address type, length and then value. See 'struct addrtype_'
     * in 'address.h' for format of this info.
     */
    *output_buf++ = ADDR_CLNS;
    *output_buf++ = nsap_len;  
    bcopy(temp, output_buf, nsap_len);

    /* Advance 'info' */
    *info += CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + nsap_len;

    /* Advance output buffer */
    *cache_buffer += (2 + nsap_len);

    /* Decrement number of addresses */
    (*num_addr)--;

    /* 
     * Decrement 'value_len' by the length of this information, to
     * reflect the remaining size of the input buffer.
     */

    (*value_len) -= CDP_NLPID_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                           nsap_len;

    return;
}			 

/*
 * cdp_handle_novell_address
 *
 * Handle Novell address information. The argument 'info' will point at the
 * beginning of the address encoding information, i.e. at 'PT'.
 * 'protocol info' fields. The function will also advance the pointer past
 * the end of the address. 'Value_len' is the length of the entire 'value'
 * part, starting from the 'PT'.
 */
void cdp_handle_novell_address (int *value_len, uchar **cache_buffer,
		                uchar **info, int *num_addr)
{
    uchar *temp, *output_buf;

    temp = *info;

    /*
     * Scan the Novell info fields to make sure it's all valid. 
     */
	    
    /* Protocol type must correspond to 802.2 format */
    if (*temp != CDP_8022_FORMAT) {
        cdp_evbuginf("\nCDP-EV: Invalid 802.2 format (%d)", *temp);

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_NOVELL;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    temp++;

    /* Protocol type length must be 8 */
    if (*temp != CDP_8022_FORMAT_LEN) {
        cdp_evbuginf("\nCDP-EV: Invalid 802.2 length (%d)", *temp);

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_NOVELL;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    temp++;

    /*
     * 'temp' now points to the 'protocol info' field, which consists of
     * 8 bytes - 3 SAP header bytes (AAAA03) followed by 3 OUI bytes,
     * followed by the type. Skip the 1st 6 bytes, to get to the type
     * field. (See "struct snap_hdr_" in ieee.h if you need to know
     * more.
     */
    temp += (SAP_HDRBYTES + SNAP_OUIBYTES);

    /* We're interested in Novell here */
    if (GETSHORT(temp) != TYPE_NOVELL1) {
        cdp_evbuginf("\nCDP-EV: Expecting Novell type instead of (%d)",
		     GETSHORT(temp));

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_NOVELL;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    /* skip ethernet type to get to 'address length' field */
    temp += CDP_ADDR_LEN_FIELD_LEN;

    /*
     * Check Novell address length field to make sure it's valid. 
     */
	    
    if (GETSHORT(temp) != ADDRLEN_NOVELL) {
        cdp_evbuginf("\nCDP-EV: Invalid Novell address length (%d)",
		     GETSHORT(temp));

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_NOVELL;

	cdp_traffic[CDP_BADPACKET]++;
        return;
    }

    /*
     * the 'address length' field is 2 bytes. Advancing 'temp' will make it
     * point to the Novell address
     */
    temp += CDP_ADDR_LEN_FIELD_LEN;

    output_buf = *cache_buffer;

    /* 
     * fill in address type, length and then value. See 'struct addrtype_'
     * in 'address.h' for format of this info. A Novell address has a
     * 4 byte net, followed by a 6 byte node number.
     */
    *output_buf++ = ADDR_NOVELL;
    *output_buf++ = ADDRLEN_NOVELL;

    /* Get net number, then node number */
    bcopy(temp, output_buf, ADDRLEN_NOVELL);

    /* Advance 'info' */
    *info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + ADDRLEN_NOVELL;

    /* Advance output buffer */
    *cache_buffer += (2 + ADDRLEN_NOVELL);

    /* Decrement as we parse addresses */
    (*num_addr)--;
    
    /*
     * Decrement 'value_len', to reflect the remaining size of the input
     * buffer. The amount deducted is the total length of Novell information.
     */

    (*value_len) -= (CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
		                                             ADDRLEN_NOVELL);

    return;
}

/*
 * cdp_handle_apple_address
 *
 * Handle Appletalk address information. The argument 'info' will point at the
 * beginning of the address encoding information, i.e. at 'PT'.
 * 'protocol info' fields. The function will also advance the pointer past
 * the end of the address. 'Value_len' is the length of the entire 'value'
 * part, starting from the 'PT'.
 */
void cdp_handle_apple_address (int *value_len, uchar **cache_buffer,
		               uchar **info, int *num_addr)
{
    uchar *temp, *output_buf;

    temp = *info;

    /*
     * Scan the Appletalk info fields to make sure it's all valid. 
     */
	    
    /* Protocol type must correspond to 802.2 format */
    if (*temp != CDP_8022_FORMAT) {
        cdp_evbuginf("\nCDP-EV: Invalid 802.2 format (%d)", *temp);

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_APPLE;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    temp++;

    /* Protocol type length must be 8 */
    if (*temp != CDP_8022_FORMAT_LEN) {
        cdp_evbuginf("\nCDP-EV: Invalid 802.2 length (%d)", *temp);

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_APPLE;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    temp++;

    /*
     * 'temp' now points to the 'protocol info' field, which consists of
     * 8 bytes - 3 SAP header bytes (AAAA03) followed by 3 OUI bytes,
     * followed by the type. Skip the 1st 6 bytes, to get to the type
     * field. (See "struct snap_hdr_" in ieee.h if you need to know
     * more.
     */
    temp += (SAP_HDRBYTES + SNAP_OUIBYTES);

    /* We're interested in Appletalk here */
    if (GETSHORT(temp) != TYPE_ETHERTALK) {
        cdp_evbuginf("\nCDP-EV: Expecting Appletalk type instead of (%d)",
		     GETSHORT(temp));

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_APPLE;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    /* skip ethernet type to get to 'address length' field */
    temp += CDP_ADDR_LEN_FIELD_LEN;

    /*
     * Check Appletalk address length field to make sure it's valid. 
     */
	    
    if (GETSHORT(temp) != ADDRLEN_APPLE) {
        cdp_evbuginf("\nCDP-EV: Invalid Appletalk address length (%d)",
		     GETSHORT(temp));

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_APPLE;

	cdp_traffic[CDP_BADPACKET]++;
        return;
    }

    /*
     * the 'address length' field is 2 bytes. Advancing 'temp' will make it
     * point to the Appletalk address
     */
    temp += CDP_ADDR_LEN_FIELD_LEN;

    output_buf = *cache_buffer;

    /* 
     * fill in address type, length and then value. See 'struct addrtype_'
     * in 'address.h' for format of this info. A Novell address has a
     * 4 byte net, followed by a 6 byte node number.
     */
    *output_buf++ = ADDR_APPLE;
    *output_buf++ = ADDRLEN_APPLE;

    /* Copy Appletalk info */
    bcopy(temp, output_buf, ADDRLEN_APPLE);

    /* Advance 'info' */
    *info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + ADDRLEN_APPLE;

    /* Advance output buffer */
    *cache_buffer += (2 + ADDRLEN_APPLE);

    /* Decrement as we parse addresses */
    (*num_addr)--;
    
    /*
     * Decrement 'value_len', to reflect the remaining size of the input
     * buffer. The amount deducted is the total length of Appletalk information.
     */

    (*value_len) -= (CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
		                                                ADDRLEN_APPLE);

    return;
}

/*
 * cdp_handle_decnet_address
 *
 * Handle DECnet address information. The argument 'info' will point at the
 * beginning of the address encoding information, i.e. at 'PT'.
 * 'protocol info' fields. The function will also advance the pointer past
 * the end of the address. 'Value_len' is the length of the entire 'value'
 * part, starting from the 'PT'.
 */
void cdp_handle_decnet_address (int *value_len, uchar **cache_buffer,
			           uchar **info, int *num_addr)
{
    uchar *temp, *output_buf;

    temp = *info;

    /*
     * Scan the DECnet info fields to make sure it's all valid. 
     */
	    
    /* Protocol type must correspond to 802.2 format */
    if (*temp != CDP_8022_FORMAT) {
        cdp_evbuginf("\nCDP-EV: Invalid 802.2 format (%d)", *temp);

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_DECNET;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    temp++;

    /* Protocol type length must be 8 */
    if (*temp != CDP_8022_FORMAT_LEN) {
        cdp_evbuginf("\nCDP-EV: Invalid 802.2 length (%d)", *temp);

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                      ADDRLEN_DECNET;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    temp++;

    /*
     * 'temp' now points to the 'protocol info' field, which consists of
     * 8 bytes - 3 SAP header bytes (AAAA03) followed by 3 OUI bytes,
     * followed by the type. Skip the 1st 6 bytes, to get to the type
     * field. (See "struct snap_hdr_" in ieee.h if you need to know
     * more.
     */
    temp += (SAP_HDRBYTES + SNAP_OUIBYTES);

    /* We're interested in DECnet here */
    if (GETSHORT(temp) != TYPE_DECNET) {
        cdp_evbuginf("\nCDP-EV: Expecting DECnet type instead of (%d)",
		     GETSHORT(temp));

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_DECNET;

	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    /* skip ethernet type to get to 'address length' field */
    temp += CDP_ADDR_LEN_FIELD_LEN;

    /*
     * Check DECnet address length field to make sure it's valid. 
     */
	    
    if (GETSHORT(temp) != ADDRLEN_DECNET) {
        cdp_evbuginf("\nCDP-EV: Invalid DECnet address length (%d)",
		     GETSHORT(temp));

	/* 'step over' if we don't like this information */
	*info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
	                                                       ADDRLEN_DECNET;

	cdp_traffic[CDP_BADPACKET]++;
        return;
    }

    /*
     * the 'address length' field is 2 bytes. Advancing 'temp' will make it
     * point to the DECnet address
     */
    temp += CDP_ADDR_LEN_FIELD_LEN;

    output_buf = *cache_buffer;
  
    /* 
     * fill in address type, length and then value. See 'struct addrtype_'
     * in 'address.h' for format of this info.
     */
    *output_buf++ = ADDR_DECNET;
    *output_buf++ = ADDRLEN_DECNET;  

    PUTSHORT(output_buf, GETSHORT(temp));
    /* Advance 'info' */
    *info += CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN + ADDRLEN_DECNET;

    /* Advance output buffer */
    *cache_buffer += (2 + ADDRLEN_DECNET);

    /* Decrement as we parse addresses */
    (*num_addr)--;
    
    /*
     * Decrement 'value_len', to reflect the remaining size of the input
     * buffer. The amount deducted is the total length of DECnet information.
     */

    (*value_len) -= (CDP_8022_INFO_OVERHEAD + CDP_ADDR_LEN_FIELD_LEN +
		                                               ADDRLEN_DECNET);

    return;
}

/*
 * cdp_handle_port_id_info
 *
 * Add port ID info to cache entry. 
 */
void cdp_handle_port_id_info (cdp_info_type *item, cdp_cache_type *cache_entry,
			      boolean new_entry)
{
    ushort data_len, i;
    uchar *temp;

    /*
     * NOTE: data_len does not include NULL terminator in string
     */
    data_len = GETSHORT(&item->length) - CDP_TLV_OVERHEAD;
    data_len = min(data_len, (CDP_MAX_PORT_ID_STR_LEN - 1));

    temp = item->value;

    for (i = 0; i < data_len; i++) 
	cache_entry->port_id[i] = *temp++;

    /*
     * Now insert NULL terminator
     */
    cache_entry->port_id[data_len] = '\0';
    return;    

}

/*
 * cdp_handle_capabilities_info
 *
 * Add capabilities info to cache entry. 
 */
void cdp_handle_capabilities_info (cdp_info_type *item,
				  cdp_cache_type *cache_entry,
				  boolean new_entry)
{
    if (new_entry) {
	PUTLONG(&cache_entry->capabilities, GETLONG(item->value));
    }
    else {
	/*
	 * Check if information has changed
	 */
	if (GETLONG(item->value) != cache_entry->capabilities)
	    cache_entry->capabilities = GETLONG(item->value);
    }
    return;
}

/*
 * cdp_handle_version_info
 *
 * Add version info to cache entry. 
 */
void cdp_handle_version_info (cdp_info_type *item, cdp_cache_type *cache_entry,
			      boolean new_entry)
{
    ushort data_length;
    
    /*
     * NOTE: data_length does not include NULL terminator in string
     */
    data_length = GETSHORT(&item->length) - CDP_TLV_OVERHEAD;
    if (new_entry) {
	/*
	 * Allocate extra byte for NULL terminator
	 */
        cache_entry->version = malloc(data_length + 1);
        if (!cache_entry->version) {
           cdp_traffic[CDP_NOMEM]++;
           return;
	}
        bcopy(item->value, cache_entry->version, data_length);
	cache_entry->version[data_length] = '\0';
    }
    else {
	/*
	 * Check if information has changed.
	 */
	
	if ((strlen(cache_entry->version) != data_length) || 
	    (strncmp(item->value, cache_entry->version, data_length))) {
	    /*
	     * If lengths not equal, free buffer, and get a new
	     * buffer, allowing an extra byte for NULL terminator.
	     */
	    if (strlen(cache_entry->version) != data_length) {
	        free(cache_entry->version);
	        cache_entry->version = malloc(data_length + 1);
                if (!cache_entry->version) {
                   cdp_traffic[CDP_NOMEM]++;
                   return;
	       }
	    }
            bcopy(item->value, cache_entry->version, data_length);
	    cache_entry->version[data_length] = '\0';
	}
    }    

    return;
}

/*
 * cdp_handle_platform_info
 *
 * Add platform info to cache entry.
 */
void cdp_handle_platform_info (cdp_info_type *item,
			       cdp_cache_type *cache_entry, boolean new_entry)
{
    ushort data_len, i;
    uchar *temp;

    /*
     * NOTE: data_len does not include NULL terminator in string
     */
    data_len = GETSHORT(&item->length) - CDP_TLV_OVERHEAD;
    data_len = min(data_len, (CDP_MAX_PLATFORM_STR_LEN - 1));

    temp = item->value;

    for (i = 0; i < data_len; i++) 
	cache_entry->platform[i] = *temp++;

    cache_entry->platform[data_len] = '\0';
    return;    
}

/*
 * cdp_cache_ager
 * 
 * Age expired cache entries
 */
void cdp_cache_ager (void)
{
    int bucket;
    cdp_cache_type *entry, *next, *prev;

    for (bucket = 0; bucket < NETHASHLEN; bucket++) {
	prev = NULL;
	for (entry = cdp_table[bucket]; entry; prev = entry, entry = next) {
	    next = entry->next;
	    if (TIMER_RUNNING_AND_AWAKE(entry->expiration_time)) {
		cdp_adjbuginf("\nCDP-AD: Aging entry for %s, "
			      "on interface %s",
 			      entry->entry_name, entry->idb->namestring);

		cdp_delete_cache_entry(entry);

		/*
		 * Entry being deleted is first one in chain
		 */
		if (!prev)
		    cdp_table[bucket] = next;
		else
		    prev->next = next;

		entry = prev;
	    }
	}
    }
}

/*
 * cdp_delete_cache_entry
 *
 * Delete given cache entry.
 */
void cdp_delete_cache_entry (cdp_cache_type *entry)
{

    free(entry->entry_name);  
    
    /*
     * We need to check for 'version' or 'addr' being non-NULL
     * because these could be NULL if we received too small a fragment that 
     * did not have 'version' or 'addr' information.
     */
    if (entry->version)
	free(entry->version);

    if (entry->addr)
	free(entry->addr);

    entry->next = NULL;
    free(entry);
    return;
}
