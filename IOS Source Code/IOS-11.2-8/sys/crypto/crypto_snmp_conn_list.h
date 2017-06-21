/* $Id: crypto_snmp_conn_list.h,v 3.1.4.1 1996/04/19 14:56:04 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_snmp_conn_list.h,v $
 *------------------------------------------------------------------
 * Cisco IP encryption mib.
 * A header that defines structure for snmp connection list.
 * 
 * April 1996, Subodh Nijsure
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_snmp_conn_list.h,v $
 * Revision 3.1.4.1  1996/04/19  14:56:04  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.1  1996/04/10  17:40:33  snijsure
 * Branch: IosSec_branch
 * Checking in implementation of Cisco IP encryption MIB.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef CRYPTO_SNMP_CONN_LIST_H_
#define CRYPTO_SNMP_CONN_LIST_H_
extern short pconn_used;

/* A structure that encapsulate connection information, to be returned in
 * cieConnTable object.
 */

typedef struct crypto_snmp_connection_ {
   uint engineID;		/* Unique ID of the engine, CPU #, EPA  ID # */
   long connectionIndex ;	/* A monotonically increasing integer, a 
			           SNMP counter */
   ushort connectionStatus ;	/* Status of connection */ 
   long protectedEntity ;	/* IP address of PE */
   long unprotectedEntity ;	/* IP address of UPE */
   long noPktsEncrypted ;	
   long noPktsDecrypted ;
   long noPktsDropped ;
   long localTimeEstablished ;   /* sysUptime at which connection was 
				    establised */

   ushort alg_type; 		/* Algorithm used to encrypt data */
   struct crypto_snmp_connection_ *next;
} crypto_snmp_connection;

typedef struct   snmp_crypto_mykey_  {
   crypto_mykey *key;
   struct snmp_crypto_mykey_ *next;
} snmp_crypto_mykey; 

#endif
