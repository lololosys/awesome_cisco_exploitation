/* $Id: sr_snmpcfg.h,v 3.2 1995/11/17 19:01:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_snmpcfg.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snmpcfg.h,v $
 * Revision 3.2  1995/11/17  19:01:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:18:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__SNMPCFG_H)
#define __SNMPCFG_H

/*
 * we are compiling SNMP Research agent software
 */
#define SR_AGENT

/*
The description of the various #defines used by the SNMP Research SNMPv2
code was taken from e-mail received from SNMP Research Technical Support
<support@snmp.com> in response to a query issued by Cisco.
*/

/*
The protocol used by the agent will be either SNMP version 1 (SNMPv1), SNMP
version 2 (SNMPv2), or bilingual (both SNMPv1 and SNMPv2, sometimes referred
to as a "BOTH" compile).  The default makefiles will build the bilingual
agent by passing the keyword "BOTH" and the protocol definitions for both v1
and v2 using 
        
        SNMP_VER = BOTH
        VER_DEFS = "-DSR_SNMPv1 -DSR_SNMPv2"
*/

/* bi-lingual agent */
#define BOTH

/*
DEBUG is used to turn the debugging on and off.  Set DEBUG to an empty string
( DEBUG = ) to turn off debugging statements.
*/

/* disable debugging */
#undef DEBUG

/*
SNMPINFO is used to instruct the agent and utilities to get their OID
translations from the snmpinfo.dat file instead of the snmp-mib.h file.  See
the manual page on snmpinfo.dat.
*/

/* use snmp-mib.h */
#undef SNMPINFO

/*
LIGHT and SETS defines are for tailoring "lean" agents.  Certain fuctions are
eliminated when LIGHT is defined: specifically, the text-to-OID translation
table is not available.  SETS are only enabled if SETS is defined at compile
time; if it's not there, the theory is that all the test and set routines
will be eliminated.  I say "theory" because I have not personally tested that
feature, though I know that the engineers use it occasionally to get small
agents for specific testing purposes.

[note: it turns out that the LIGHT switch in the SNMP Research code is
overloaded.  Not only does it disable the text-to-oid translation, but
it also prevents object textual names from being included in the mib
oid table.  Since we probably want text-to-oid translation, but don't
care about having names in the mib, the LIGHT switch has been divided
into two switches.
LIGHT is used to eliminate textual names from the mib
LIGHTOID is used to eliminate text-to-oid translation]
*/

/* enable text-to-oid translation */
#undef LIGHTOID

/* disable textual names in the oid table */
#define LIGHT

/* enable set processing */
#define SETS


/*
SR_UNSECURABLE and SR_NO_PRIVACY are used to disable different authentication
and privacy protocols.  The default for V2 (either bilingual or V2 only) is
to use both the privacy and authentication protocols.  DES and MD5 are the
tools of choice for these tasks, but there is a problem with exporting DES
and MD5, so these two defines exist for stripping out the un-exportable
sections of code.  

Defining SR_NO_PRIVACY disables DES; using unifdef, you can actually remove
all references from the code, if necessary: 

        unifdef -DSR_NO_PRIVACY fname > newfname ; mv newfname fname

Defining SR_UNSECURABLE disables MD5 and DES (without authentication there
can be no privacy).   If SR_UNSECURABLE is used, the resulting binaries have
no-auth access only, which is essentially the same as v1 using communities
only the configuration files are more complex.  unifdef can be used to strip
both DES and MD5 from the code:

        unifdef -DSR_UNSECURABLE fname > newfname ; mv newfname fname

It actually may be possible to ship MD5 in the future, so the two protocols
are treated separately.  Also, different rules apply to binaries, and it may
be sufficient to simply compile with -DSR_NO_PRIVACY.  (In most cases, no
encrypted messages may be sent without causing lots of people in dark suits
to get very agitated.)  Check with competent legal counsel before
export-shipping ANY MD5 or DES in either source or binary form.  Most will
probably advise against it.  There are lengthy gov't forms to fill out and
all.
*/

/* enable MD5 authentication */
#undef SR_UNSECURABLE

/* disable DES privacy */
#define SR_NO_PRIVACY


#endif /* !defined(__SNMPCFG_H) */
