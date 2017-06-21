/* $Id: unix_fast.c,v 3.2 1995/11/17 17:55:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/unix/unix_fast.c,v $
 *------------------------------------------------------------------
 * unix_fast.c -- cisco running under UNIX - fast switching support
 *
 * 6-January-1992, Kirk Lougheed
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_fast.c,v $
 * Revision 3.2  1995/11/17  17:55:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:33:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/28  09:32:17  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:12:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#define PAN
#include "../ip/ipfast.h"
#undef PAN


boolean ether_ip_fastswitch(paktype *ptr) { return(FALSE); }
boolean fddi_ip_fastswitch(paktype *ptr) { return(FALSE); }
boolean token_ip_fastswitch(paktype *ptr) { return(FALSE); }
boolean hdlc_ip_fastswitch(paktype *ptr) { return(FALSE); }
boolean hdlc_unknown_fastswitch(paktype *ptr) { return(FALSE); }
boolean smds_ip_fastswitch(paktype *ptr) { return(FALSE); }
boolean smds_unknown_fastswitch(paktype *ptr) { return(FALSE); }

boolean xns_fastswitch(void) { return(FALSE); }
boolean token_nov_fastswitch1(void) { return(FALSE); }
boolean token_nov_fastswitch2(void) { return(FALSE); }

boolean dn_fastswitch(void) { return(FALSE); }
boolean clns_fastswitch(void) { return(FALSE); }
boolean ether_nov_fastswitch(void) { return(FALSE); }
boolean ether_nov_fastswitch0(void) { return(FALSE); }
boolean ether_nov_fastswitch1(void) { return(FALSE); }
boolean ether_nov_fastswitch2(void) { return(FALSE); }
boolean hdlc_nov_fastswitch(void) { return(FALSE); }
boolean fddi_nov_fastswitch1(void) { return(FALSE); }
boolean fddi_nov_fastswitch2(void) { return(FALSE); }
void set_vring_fast_entry (void *ptr, idbtype *idb) { return; }
void set_vring_fast_peer (void *ptr, idbtype *idb) {  return; }
void fs_srb_setup(void)  { return;  }

void vines_setup_fs_tables(void) { return; }
void vines_clear_fs_tables(void) { return; }


void unix_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * This table is not currently complete.  If someone has occasion to
     * do some fast switching on a unix platform, please feel free to 
     * flesh things out as necessary.  See <proto>fast_les.c and 
     * <proto>fast_mci.c for the general idea.
     */
	reg_add_ether_fs(RXTYPE_DODIP,
			 (service_ether_fs_type)ether_ip_fastswitch,
			 "ether_ip_fastswitch");
	reg_add_token_fs(RXTYPE_DODIP,
			 (service_token_fs_type)token_ip_fastswitch,
			 "token_ip_fastswitch");
	reg_add_fddi_fs(RXTYPE_DODIP,
			(service_fddi_fs_type)fddi_ip_fastswitch,
			"fddi_ip_fastswitch");
	reg_add_hdlc_fs(RXTYPE_DODIP,
			(service_hdlc_fs_type)hdlc_ip_fastswitch,
			"hdlc_ip_fastswitch");
	reg_add_hdlc_fs(RXTYPE_UNKNOWN,
			(service_hdlc_fs_type)hdlc_unknown_fastswitch,
			"hdlc_unknown_fastswitch");
	reg_add_ppp_fs(RXTYPE_DODIP,
		       (service_ppp_fs_type)hdlc_ip_fastswitch,
		       "hdlc_ip_fastswitch");
	reg_add_smds_fs(RXTYPE_DODIP,
			(service_smds_fs_type)smds_ip_fastswitch,
			"smds_ip_fastswitch");
	reg_add_smds_fs(RXTYPE_UNKNOWN,
			(service_smds_fs_type)smds_unknown_fastswitch,
			"smds_unknown_fastswitch");
}

void unix_fs_init (subsystype *subsys)
{
    reg_add_register_drivers(unix_fs_register_drivers,
			     "unix_fs_register_drivers");
}

/*
 * UNIX FS subsystem header
 */

#define UNIXFS_MAJVERSION 1
#define UNIXFS_MINVERSION 0
#define UNIXFS_EDITVERSION  1

SUBSYS_HEADER(unix_fs, UNIXFS_MAJVERSION, UNIXFS_MINVERSION, UNIXFS_EDITVERSION,
	      unix_fs_init, SUBSYS_CLASS_DRIVER,
	      "seq: fastswitch",
	      "req: fastswitch");

