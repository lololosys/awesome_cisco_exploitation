/* $Id: sr_config.h,v 3.2.62.1 1996/04/15 03:05:41 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_config.h,v $
 *------------------------------------------------------------------
 * SNMPv1/SNMPv2 Bilingual Agent Configuration
 *
 * April 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_config.h,v $
 * Revision 3.2.62.1  1996/04/15  03:05:41  jjohnson
 * CSCdi53343:  Remove support for the SNMPv2 Simplified Security
 * Conventions
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:59:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/03  22:26:26  jjohnson
 * CSCdi35401:  Parser help for snmp-server packetsize is incorrect
 * Differentiate between the minimum, maximum, and default packetsize
 *
 * Revision 2.2  1995/07/01  00:38:20  jjohnson
 * CSCdi35423:  snmp-server packetsize command no longer works
 *
 * Revision 2.1  1995/06/07  22:48:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __sr_config_h__
#define __sr_config_h__

#define SR_DEF_PARTY_MESSAGE_SIZE     SNMP_MAX_MSG_SIZE_DEF
#define SR_DEF_PARTY_LIFETIME         300

extern  boolean snmp_add_view SR_PROTOTYPE((char    *name,
					    char    *subtree,
					    long     viewtype,
					    long     storagetype));
extern  boolean snmp_add_context SR_PROTOTYPE((char    *contextname,
					       char    *contextoid,
					       char    *viewname,
					       long     storagetype));
extern  boolean snmp_add_party SR_PROTOTYPE((char           *partyname,
					     char           *partyoid,
					     int             partydomain,
					     unsigned long   partyaddress,
					     unsigned short  partyport,
					     int	     partyaccesslist,
					     long	     partymessagesize,
					     int	     partyauthtype,
					     OctetString    *partyauthkey,
					     long	     partyauthclock,
					     long	     partyauthlifetime,
					     int	     partyprivtype,
					     OctetString    *partyprivkey,
					     long            local,
					     long            storagetype));
extern  boolean snmp_add_acl SR_PROTOTYPE((char          *targetname,
					   char          *subjectname,
					   char          *resourcename,
					   long           privileges,
					   long           storagetype));
extern  void snmp_admin_init SR_PROTOTYPE((void));
extern  boolean snmp_add_community_proxy SR_PROTOTYPE((char    *community,
						       char    *viewname,
						       int      accesslist,
						       long     privileges));
extern  void snmp_delete_community_proxy SR_PROTOTYPE((char    *community,
						       char    *viewname));
extern	boolean snmp_add_trap_proxy SR_PROTOTYPE((ipaddrtype	address,
						  char	       *community,
						  ulong		flags));
extern	void snmp_delete_trap_proxy SR_PROTOTYPE((ipaddrtype	address,
						  char	       *community,
						  ulong		flags));

extern  void snmp_write_views SR_PROTOTYPE((char *cfgcmd));
extern  void snmp_write_contexts SR_PROTOTYPE((char *cfgcmd));
extern  void snmp_write_parties SR_PROTOTYPE((char *cfgcmd,
					      boolean hide_passwords));
extern  void snmp_write_acls SR_PROTOTYPE((char *cfgcmd));

extern  void snmp_show_views SR_PROTOTYPE((void));
extern  void snmp_show_contexts SR_PROTOTYPE((void));
extern  void snmp_show_parties SR_PROTOTYPE((void));
extern  void snmp_show_acls SR_PROTOTYPE((void));

extern  void snmp_configure_view SR_PROTOTYPE((parseinfo *csb));
extern  void snmp_configure_context SR_PROTOTYPE((parseinfo *csb));
extern  void snmp_configure_party SR_PROTOTYPE((parseinfo *csb));
extern  void snmp_configure_acl SR_PROTOTYPE((parseinfo *csb));


#endif /* __sr_config_h__ */
