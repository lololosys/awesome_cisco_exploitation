/* $Id: mib.c,v 1.2 1996/07/10 06:44:22 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/mib.c,v $
 *------------------------------------------------------------------
 * mib.c  NEAT MIB file
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mib.c,v $
 * Revision 1.2  1996/07/10 06:44:22  jliu
 *     	o Created another timer(250ms) for framing process and one second
 *           DSX1 performance data statistics in order to pass ACCUNET test
 * 	o Code changed for both AT&T and ANSI fdl test
 *         o Removed frame_mode in T1InfoType by using FrameType in frm_info
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include 	"t1_headers.h"


void SetMIBLineStatus ( LineStatus *stptr, ushort mask )
{
    *stptr |= mask;
    *stptr &= ~DSX1_NO_ALARM;
}

void ClearMIBLineStatus ( LineStatus *stptr, ushort mask )
{
    *stptr &= ~mask;
    
    if (( *stptr & DSX1_HAVE_ERRORS ) == 0 )
	*stptr |= DSX1_NO_ALARM;
}

void Cleardsx1Table ( dsx1TableType *p )
{
    p->dsx1ESs = 0;
    p->dsx1UASs = 0;
    p->dsx1BESs = 0;
    p->dsx1SESs = 0;
    p->dsx1CSSs = 0;
    p->dsx1LOFCs = 0;
}

void ClearTotalTable ( dsx1TotalTableType *p )
{
    p->dsx1ESs = 0;
    p->dsx1UASs = 0;
    p->dsx1BESs = 0;
    p->dsx1SESs = 0;
    p->dsx1CSSs = 0;
    p->dsx1LOFCs = 0;
}

void ClearMIBData( T1InfoType *t1 )
{
    dsx1MIBDataType *mib = t1->Mib;

    mib->CurrentData.dsx1Interval = 0;
    mib->Config.dsx1TimeElapsed = 0;
    Cleardsx1Table( &mib->CurrentData );
    ClearTotalTable( &mib->TotalData );
}

