/* $Id: lan_alert.h,v 3.2 1995/11/17 09:05:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/lan_alert.h,v $
 *------------------------------------------------------------------
 * SNA Network Management LAN alert prototype declaration
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lan_alert.h,v $
 * Revision 3.2  1995/11/17  09:05:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef lan_alert_h
#define lan_alert_h



void SetHRname (hwidbtype* idb,  byte* sv, const char* adaptorName);
void SetFailCause(hwidbtype* idb, byte* sv, const char* name);
void SetX25Address (byte* ebcdic, addrtype* x25Addr);
void SetCpName(byte *sv);
void SendAlertToHost (byte* buf, hwidbtype*   idb);

void SetLanParam(byte* buf, hwidbtype* idb, const char* name);
void Ether_Alert(Ether_AlertType alertType, hwidbtype* idb);
void Qllc_Alert(Qllc_AlertType alertType, qllctype* qllc);
void Sdlc_Alert(Sdlc_AlertType alertType, sdlc_data_block *sdb);
void Lanllc_Alert(Lanllc_AlertType alertType,llctype *llc);
void Cipllc_Alert(Cipllc_AlertType alertType,cip_llctype *cipllc);
void TR_Alert(TR_AlertType alertType, hwidbtype* idb);
void Lapb_Alert(Lapb_AlertType alertType, hwidbtype* idb);
void Dlc_Alert(Dlc_AlertType alertType, hwidbtype *idb);
void Tcp_Alert(conntype* connptr);
void X25_Alert(X25_AlertType alertType, lcitype* lci, hwidbtype *idb, 
    uchar* eventDataX25);
void PuRegIdInit(void);


#endif
