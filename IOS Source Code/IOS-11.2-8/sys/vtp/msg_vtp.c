/* $Id: msg_vtp.c,v 1.1.2.2 1996/09/14 00:10:01 hampton Exp $
 * $Source: /release/112/cvs/Xsys/vtp/msg_vtp.c,v $
 *------------------------------------------------------------------
 * M S G  _ V T P . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_vtp.c,v $
 * Revision 1.1.2.2  1996/09/14  00:10:01  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 1.1.2.1  1996/08/28  13:20:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1  1996/08/25  01:18:18  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(VTP);
msgdef(ERROR, VTP, LOG_ERR, 0, "%s");
msgdef_section("");

msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

facdef(PRUNING);
msgdef(NOTRUNK,  PRUNING, LOG_ERR, 0, \
       "trunk %x not found(%s)"); 
msgdef(NODOMAIN, PRUNING, LOG_ERR, 0, \
       "domain %s(%d) not found");
msgdef(INVLNKST, PRUNING, LOG_ERR, 0, \
       "invalid link state: %x"); 
msgdef(NOVLAN,   PRUNING, LOG_ERR, 0, \
       "vlan %d not found (%s)");
msgdef(INVSPTST, PRUNING, LOG_ERR, 0, \
       "invalid SPT state: %x");
msgdef(INVASSOC, PRUNING, LOG_ERR, 0, \
       "invalid vlan local assoc: %x (%s)");
msgdef(ISDEFAULT, PRUNING, LOG_ERR, 0, \
       "cannot modify default VLAN id %d");
msgdef(JOINNONAME, PRUNING, LOG_NOTICE, 0, \
       "no domain name in rx Join (trunk %x)");
msgdef(JOINDIFFDOMAIN, PRUNING, LOG_NOTICE, 0, \
       "domain %s not found in rx Join (trunk %x)");
msgdef(JOINNOTRUNK, PRUNING, LOG_NOTICE, 0, \
       "trunk %x not found for rx Join");
msgdef(JOINDISCARD, PRUNING, LOG_NOTICE, 0, \
       "rx Join on trunk %x when pruning disabled");
msgdef(JOININVFSTV, PRUNING, LOG_ALERT, 0, \
       "Join rx on trunk %s-invalid first vlan: %d");  
msgdef(JOININVLSTV, PRUNING, LOG_ALERT, 0, \
       "Join rx on trunk %s-invalid last vlan: %d");
msgdef(JOININVLEN, PRUNING, LOG_ALERT, 0, \
       "Join rx on trunk %s-invalid len: %d (%d)");
msgdef(INVTLV, PRUNING, LOG_ALERT, 0, \
       "rx summary in domain %s with invalid TLV value:%x");
msgdef(FSMSTCHG, PRUNING, LOG_INFO, 0, \
       "T%s,V%d:st=%s,event=%s,new st=%s");
msgdef(LEARNDOMAIN, PRUNING, LOG_NOTICE, 0, \
       "learn domain %s from network");
msgdef(INVPMODE, PRUNING, LOG_ERR, 0, \
       "invalid pruning mode: %x");
msgdef(NOBUF, PRUNING, LOG_WARNING, 0, \
       "no mbuf to build join");
