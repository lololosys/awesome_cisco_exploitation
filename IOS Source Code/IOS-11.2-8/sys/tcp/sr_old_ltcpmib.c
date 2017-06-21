/* $Id: sr_old_ltcpmib.c,v 3.5 1996/02/21 17:47:10 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/tcp/sr_old_ltcpmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Robert Snyder (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_ltcpmib.c,v $
 * Revision 3.5  1996/02/21  17:47:10  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1995/12/14  08:28:18  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.3  1995/11/24  16:42:01  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  18:48:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/12  23:45:33  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.1  1995/06/07 23:05:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

#include "master.h"
#include "../snmp/snmp_api.h"
#include "sr_old_ltcpmib.h"
#include "sr_old_ltcpmib-mib.h"
#include "sr_tcpmib2.h"
#include "../snmp/sr_old_ltsmib.h"
#include "../snmp/sr_lsmi.h"

#include "interface_private.h"
#include "packet.h"
#include "tcp.h"
#include "connect.h"
#include "ttysrv.h"
#include "../ip/ip_registry.h"
#include "../parser/parser_defs_snmp.h"

static const char tty_trap_oid[] = "cisco.0.1";

static void invoke_tty_trap (conntype *conn);


void
init_old_ltcpmib(void)
{
    load_mib(old_ltcpmib_OidList, old_ltcpmib_OidListNum);
    load_oid(old_ltcpmib_oid_table);
    register_snmp_trap(TRAP_TTY, tty_trap_oid);
    reg_add_tcp_tty_close(invoke_tty_trap, "invoke_tty_trap");
}








/*---------------------------------------------------------------------
 * Retrieve data from the ltcpConnEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ltcpConnEntry_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ltcpConnEntry_t *data = NULL;
    unsigned long   buffer[10];
    OID             inst;
    int             carry;
    unsigned long   tcpConnLocalAddress;
    long            tcpConnLocalPort;
    unsigned long   tcpConnRemAddress;
    long            tcpConnRemPort;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 10
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 10) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 9 + object->oid.length, &tcpConnRemPort, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 5 + object->oid.length, &tcpConnRemAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 4 + object->oid.length, &tcpConnLocalPort, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &tcpConnLocalAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ltcpConnEntry_get(serialNum, contextInfo, arg, searchType, tcpConnLocalAddress, tcpConnLocalPort, tcpConnRemAddress, tcpConnRemPort)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 10;
	inst.oid_ptr[0] = (data->tcpConnLocalAddress >> 24) & 0xff;
	inst.oid_ptr[1] = (data->tcpConnLocalAddress >> 16) & 0xff;
	inst.oid_ptr[2] = (data->tcpConnLocalAddress >> 8) & 0xff;
	inst.oid_ptr[3] = (data->tcpConnLocalAddress >> 0) & 0xff;
	inst.oid_ptr[4] = data->tcpConnLocalPort;
	inst.oid_ptr[5] = (data->tcpConnRemAddress >> 24) & 0xff;
	inst.oid_ptr[6] = (data->tcpConnRemAddress >> 16) & 0xff;
	inst.oid_ptr[7] = (data->tcpConnRemAddress >> 8) & 0xff;
	inst.oid_ptr[8] = (data->tcpConnRemAddress >> 0) & 0xff;
	inst.oid_ptr[9] = data->tcpConnRemPort;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_loctcpConnInBytes
      case I_loctcpConnInBytes:
	dp = &data->loctcpConnInBytes;
	break;
#endif				       /* I_loctcpConnInBytes */

#ifdef I_loctcpConnOutBytes
      case I_loctcpConnOutBytes:
	dp = &data->loctcpConnOutBytes;
	break;
#endif				       /* I_loctcpConnOutBytes */

#ifdef I_loctcpConnInPkts
      case I_loctcpConnInPkts:
	dp = &data->loctcpConnInPkts;
	break;
#endif				       /* I_loctcpConnInPkts */

#ifdef I_loctcpConnOutPkts
      case I_loctcpConnOutPkts:
	dp = &data->loctcpConnOutPkts;
	break;
#endif				       /* I_loctcpConnOutPkts */

#ifdef I_loctcpConnElapsed
      case I_loctcpConnElapsed:
	dp = &data->loctcpConnElapsed;
	break;
#endif				       /* I_loctcpConnElapsed */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}


ltcpConnEntry_t *
k_ltcpConnEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   tcpConnLocalAddress,
    long            tcpConnLocalPort,
    unsigned long   tcpConnRemAddress,
    long            tcpConnRemPort)
{
    static ltcpConnEntry_t ltcpConnEntryData;
    tcbtype *tcbptr;

    tcbptr = find_tcp_connection(serialNum,
                              searchType,
                              tcpConnLocalAddress,
                              tcpConnLocalPort,
                              tcpConnRemAddress,
                              tcpConnRemPort);

    if (tcbptr == NULL)
        return(NULL);

    ltcpConnEntryData.tcpConnLocalAddress = tcbptr->localhost.ip_addr;
    ltcpConnEntryData.tcpConnLocalPort =  tcbptr->localport;
    ltcpConnEntryData.tcpConnRemAddress = tcbptr->foreignhost.ip_addr;
    ltcpConnEntryData.tcpConnRemPort = tcbptr->foreignport;

    ltcpConnEntryData.loctcpConnInBytes = tcbptr->bytesincount;
    ltcpConnEntryData.loctcpConnOutBytes = tcbptr->bytesoutcount;
    ltcpConnEntryData.loctcpConnInPkts = tcbptr->pakindata;
    ltcpConnEntryData.loctcpConnOutPkts = tcbptr->pakoutdata;
    ltcpConnEntryData.loctcpConnElapsed = 
        ELAPSED_TIME(tcbptr->starttime) / (ONESEC / 100);
    return (&ltcpConnEntryData);
}

/*
          tcpConnectionClose TRAP-TYPE
              ENTERPRISE  cisco
              VARIABLES   { tslineSesType, tcpConnState,
                            loctcpConnElapsed, loctcpConnInBytes,
                            loctcpConnOutBytes, tsLineUser }
              DESCRIPTION
                          "A tty trap signifies that a TCP connection,
                          previously established with the sending
                          protocol entity for the purposes of a tty
                          session, has been terminated."
              ::= 1
*/
#define TTY_TRAP_NUMBER 1
#define SNMP_TTY_TRAP_VBCOUNT 6

static const OID tcpConnState = {LNtcpConnState, (ulong *)IDtcpConnState};
static const OID loctcpConnElapsed = {LNloctcpConnElapsed, (ulong *)IDloctcpConnElapsed};
static const OID loctcpConnInBytes = {LNloctcpConnInBytes, (ulong *)IDloctcpConnInBytes};
static const OID loctcpConnOutBytes = {LNloctcpConnOutBytes, (ulong *)IDloctcpConnOutBytes};

/*
 * the following are *really* nasty.  The OID array (ID*) for the following
 * objects are located in a separate subsystem, so we are forced to
 * replicate them here.  Fortunately they are not subject to change
 */
static const ulong LIDtslineSesType[] = { 1, 3, 6, 1, 4, 1, 9, 2, 9, 3, 1, 1 };
static const ulong LIDtsLineUser[] = { 1, 3, 6, 1, 4, 1, 9, 2, 9, 2, 1, 18 };

static const OID tslineSesType = {LNtslineSesType, (ulong *)LIDtslineSesType};
static const OID tsLineUser = {LNtsLineUser, (ulong *)LIDtsLineUser};

static void
invoke_tty_trap (conntype *conn)
{
    OID        *vbList[SNMP_TTY_TRAP_VBCOUNT + 1];
    OID		tcpinstanceOID;
    ulong	tcpinstance[10];
    OID		ttyinstanceOID;
    ulong	ttyinstance[2];
    int		vbIndex;
    tcbtype    *tcb;
    ulong	locaddr;
    ulong	remaddr;

    /*
     * The varbind objects tcpConnState, loctcpConnElapsed, loctcpConnInBytes,
     * and loctcpConnOutBytes have an instance described by:
     * INDEX { tcpConnLocalAddress, tcpConnLocalPort,
     *         tcpConnRemAddress, tcpConnRemPort }
     */
    tcb = (tcbtype *)conn->tcb;
    locaddr = tcb->localhost.ip_addr;
    remaddr = tcb->foreignhost.ip_addr;
    tcpinstance[0] = (locaddr >> 24) & 0xff;
    tcpinstance[1] = (locaddr >> 16) & 0xff;
    tcpinstance[2] = (locaddr >> 8) & 0xff;
    tcpinstance[3] = (locaddr >> 0) & 0xff;
    tcpinstance[4] = tcb->localport;
    tcpinstance[5] = (remaddr >> 24) & 0xff;
    tcpinstance[6] = (remaddr >> 16) & 0xff;
    tcpinstance[7] = (remaddr >> 8) & 0xff;
    tcpinstance[8] = (remaddr >> 0) & 0xff;
    tcpinstance[9] = tcb->foreignport;
    tcpinstanceOID.oid_ptr = tcpinstance;
    tcpinstanceOID.length = 10;

    /*
     * The varbind object tslineSesType has an instance described by:
     * INDEX { tslineSesLine, tslineSesSession }
     * And the varbind object tsLineUser has an instance described by:
     * INDEX { tsLineNumber }
     * both tsLineSesLine and tsLineNumber are conn->tty->ttynum
     */
    if (conn->tty) {
	ttyinstance[0] = conn->tty->ttynum;
	ttyinstance[1] = conn->number;
    } else {
	ttyinstance[0] = 0;
	ttyinstance[1] = 0;
    }
    ttyinstanceOID.oid_ptr = ttyinstance;

    /*
     * Build the varbind list
     */

    ttyinstanceOID.length = 2;	/* tslineSesType uses both tty indices */
    vbList[0] = CatOID((OID *) &tslineSesType, &ttyinstanceOID);
    vbList[1] = CatOID((OID *) &tcpConnState, &tcpinstanceOID);
    vbList[2] = CatOID((OID *) &loctcpConnElapsed, &tcpinstanceOID);
    vbList[3] = CatOID((OID *) &loctcpConnInBytes, &tcpinstanceOID);
    vbList[4] = CatOID((OID *) &loctcpConnOutBytes, &tcpinstanceOID);
    ttyinstanceOID.length = 1;	/* tslineUser uses only the line index */
    vbList[5] = CatOID((OID *) &tsLineUser, &ttyinstanceOID);
    vbList[6] = NULL;

    /*
     * initiate the trap
     */
    snmp_trap(ENTERPRISE_TRAP, TTY_TRAP_NUMBER, vbList,
	      (OID *)&ciscoOID, (char *)tty_trap_oid);

    /*
     * free any allocated data
     */
    for (vbIndex = 0; vbIndex < SNMP_TTY_TRAP_VBCOUNT; vbIndex++) {
	FreeOID(vbList[vbIndex]);
    }
}
      
