/* $Id: rtt_alert.c,v 3.1.4.3 1996/06/10 17:18:21 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/dspu/rtt_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management RTT alert/resolution handler
 *
 * Jan 96, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rtt_alert.c,v $
 * Revision 3.1.4.3  1996/06/10  17:18:21  lmetzger
 * CSCdi59985:  Memory leak when sending alerts/resolves
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/06/07  22:24:29  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:49:09  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.4  1996/05/14  06:18:29  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.1.2.3  1996/04/26  07:35:25  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.1.2.2  1996/04/19  02:39:08  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.1.2.1  1996/03/22  18:50:51  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:41  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "string.h"
#include "interface_generic.h"

#define RttMaxAlertSize 200
#define RttMaxResolveSize 200
#include "../snanm/rtt_conn_change_alert.alth"
#include "../snanm/rtt_threshold_alert.alth"
#include "../snanm/rtt_timeout_alert.alth"
#include "rtt_alert_int.h"
#include "mv_sr.h"
#include "snautil.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "../rtt/rtt_lib.h"
#include "../os/clock.h"
#include "dspu_fp.h"

static void RttUpdateCpName (byte* cpVector)
{
    const char* cpName = 0;
    if (strcmp(hostname, "Router") == 0)
        cpName = GetFocalPuName();
    if (cpName == 0)
	cpName = hostname;
    ConvertToEbcdicName(cpName,cpVector); /* cp name */
}

static void rttSetHRname (byte* sv)
{
    byte* sf;
    short sfLen;
    ForAllSfInSV(sv,sf,sfLen) {
	if (sf[1] == 0x10) {		/* hierachy name list */
	    byte* ssf = &sf[3];		/* length field of hierachy name list entry*/
	    byte lenLeft = sf[0] - 3;
	    while (lenLeft > 4){	/* each name list > 4 */
		switch (ssf[ssf[0] + 1]) {
		case 0xf4:		/* control point name */
		    RttUpdateCpName(&ssf[1]);
		    break;

		case 0x40:		/* Application name */
		    ConvertToEbcdicName("NSPECHO", &ssf[1]);
		    break;

		}
		lenLeft -= (ssf[0] + 2);
		ssf += ssf[0] + 2;	/* next name list */
	    }
	}
    }
}

static void rttSetIncidentId (byte *sv,
			      clock_epoch *uniqueToken)
{
    /*
     *  We only support encoding scheme #1
     */
    ConvertToEbcdicName("        ", &sv[5]); /* NetId */
    RttUpdateCpName(&sv[13]);		/* CpName */
    ConvertToEbcdicName("NSPECHO ", &sv[21]); /* App Name */
    memcpy(&sv[29], uniqueToken, sizeof(clock_epoch));

    return;
}

static void rttSetResolvParams (byte* buf,
				clock_epoch *uniqueToken)
{
    byte* sv;
    short svLen;

    ForAllSvInMV(((MV*)buf), sv, svLen){
	switch (sv[1]) {

	case 0x42:			/* Relative Time */
	    /* This value is set in the MV_VectorSent 
	     *  which is called from the MV_SendAlert 
	     *  function. 
	     */
	    break;

	case 0x05:			/* Resource Hierarchy Name List */
	    rttSetHRname(sv);
	    break;

	case 0x4a:			/* Incident Identifier */
	    rttSetIncidentId(sv, uniqueToken);
	    break;

	case 0x10:			/* Product ID */
	    /* This value is set in the MV_SendAlert */
	    break;

	} /* switch */
    } /* ForAllSvInMV */

    return;
}

static void rttSetAlertParam (byte* buf, 
			      long rttMonCtrlAdminIndex,
			      clock_epoch *uniqueToken)
{ 
    byte* sv;
    short svLen;

    ForAllSvInMV(((MV*)buf), sv, svLen){
	switch (sv[1]) {

	case 0x42:			/* Relative Time */
	    /* This value is set in the MV_VectorSent 
	     *  which is called from the MV_SendAlert 
	     *  function. 
	     */
	    break;

	case 0x05:			/* Resource Hierarchy Name List */
	    rttSetHRname(sv);
	    break;

	case 0x4a:			/* Incident Identifier */
	    clock_get_time(uniqueToken);
	    rttSetIncidentId(sv, uniqueToken);
	    break;

	case 0x10:			/* Product ID */
	    /* This value is set in the MV_SendAlert */
	    break;

	} /* switch */
    } /* ForAllSvInMV */

    return;

}

static void fixupTotalLength (byte *buf)
{
    /* all resolve structures are the same length */
    SetMvLen(((MV*)buf), sizeof(struct
				rttMonThresholdNotification_Resolve));
}


void rtt_Resolution (Rtt_ResolveType resolveType,
		     long rttMonCtrlAdminIndex,
		     void *uniqueToken)
{
    byte buf[RttMaxResolveSize];   
    boolean createResolve = FALSE;

    switch(resolveType){
    
    case rttMonConnectionChangeNotification_Resolve: 
#ifdef RTTMONCONNCHNGRESOLVE_H
	memcpy(buf, (char*)&RTTMONTHRESHOLDRESOLVE,
	       sizeof(struct rttMonThresholdNotification_Resolve));
	SetMvLen(((MV*)buf), 
		 sizeof(struct rttMonThresholdNotification_Resolve));
	createResolve = TRUE;
#endif
	break;
    
    case rttMonThresholdNotification_Resolve:
#ifdef RTTMONTHRESHOLDRESOLVE_H
	memcpy(buf, (char*)&RTTMONTHRESHOLDRESOLVE,
	       sizeof(struct rttMonThresholdNotification_Resolve));
	SetMvLen(((MV*)buf),
		 sizeof(struct rttMonThresholdNotification_Resolve));
	createResolve = TRUE;
#endif
	break;
    
    case rttMonTimeoutNotification_Resolve:
#ifdef RTTMONTIMEOUTRESOLVE_H
	memcpy(buf, (char*)&RTTMONTIMEOUTRESOLVE,
	       sizeof(struct rttMonTimeoutNotification_Resolve));
	SetMvLen(((MV*)buf),
		 sizeof(struct rttMonTimeoutNotification_Resolve));
	createResolve = TRUE;
#endif
	break;
    
    }
  
    if (createResolve) {
	fixupTotalLength(buf);
	/* set specific values */
	rttSetResolvParams(buf, (clock_epoch *)uniqueToken);
	MV_SendAlert((MV*)buf);
    } else {
	reg_invoke_dumpRttMonProcessHeader(rttMonCtrlAdminIndex, 
					   RttResolutionNotSupported);
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t Type is %d\n",
		    resolveType + 1);
    }
}


void rtt_Alert (Rtt_AlertType alertType, 
		long rttMonCtrlAdminIndex,
		void *uniqueToken)
{
    byte buf[RttMaxAlertSize];   
    boolean createAlert = FALSE;
  
    switch(alertType){
    
    case rttMonConnectionChangeNotification_Alert: 
#ifdef RTTMONCONNCHNGALERT_H
	memcpy(buf, (char*)&RTTMONCONNCHNGALERT, 
	       sizeof(struct rttMonConnectionChangeNotification_Alert));
	createAlert = TRUE;
#endif
	break;
    
    case rttMonThresholdNotification_Alert:
#ifdef RTTMONTHRESHOLDALERT_H
	memcpy(buf, (char*)&RTTMONTHRESHOLDALERT, 
	       sizeof(struct rttMonThresholdNotification_Alert));
	createAlert = TRUE;
#endif
	break;
    
    case rttMonTimeoutNotification_Alert:
#ifdef RTTMONTIMEOUTALERT_H
	memcpy(buf, (char*)&RTTMONTIMEOUTALERT, 
	       sizeof(struct rttMonTimeoutNotification_Alert));
	createAlert = TRUE;
#endif
	break;
    
    }
  
    if (createAlert) {
	/* set specific values */
	rttSetAlertParam(buf, rttMonCtrlAdminIndex, (clock_epoch *)uniqueToken);
	MV_SendAlert((MV*)buf);
    } else {
	reg_invoke_dumpRttMonProcessHeader(rttMonCtrlAdminIndex, 
					   RttAlertNotSupported);
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t Type is %d\n",
		    alertType + 1);
    }
}









