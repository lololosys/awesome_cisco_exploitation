/* $Id: sysmgt_snanaumib.h,v 3.1.4.1 1996/05/17 11:19:25 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/sysmgt_snanaumib.h,v $
 *------------------------------------------------------------------
 * NAU mib rp-cip communication structure
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysmgt_snanaumib.h,v $
 * Revision 3.1.4.1  1996/05/17  11:19:25  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/21  03:21:48  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:01:05  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _SYSMGT_RFC1666_H_
#define _SYSMGT_RFC1666_H_



#define NameLen 12
#define DescriptionLen 128
#define LuNameLen      48
#define LuSnaNameLen   17
#define LuModelLen     8

/*
 * The snaNode family. 
 */


typedef
struct _snaNode {
        unsigned long   snaNodeAdminTableLastChange;
        unsigned long   snaNodeOperTableLastChange;
        unsigned long   snaNodeLinkAdminTableLastChange;
        unsigned long   snaNodeLinkOperTableLastChange;
} SnaNode_t;

/*
 * The snaNodeAdminEntry family. 
 */


typedef
struct _snaNodeAdminEntry {
        long            snaNodeAdminIndex;
        char            snaNodeAdminName[NameLen];
        long            snaNodeAdminType;
        long            snaNodeAdminXidFormat;
        long            snaNodeAdminXid;
        long            snaNodeAdminEnablingMethod;
        long            snaNodeAdminLuTermDefault;
        long            snaNodeAdminMaxLu;
        char            snaNodeAdminHostDescription[DescriptionLen];
        long            snaNodeAdminStopMethod;
        long            snaNodeAdminState;
        long            snaNodeAdminRowStatus;
} SnaNodeAdminEntry_t;


/*
 * The snaNodeOperEntry family. 
 */


typedef
struct _snaNodeOperEntry {
        long            snaNodeAdminIndex;
        char            snaNodeOperName[NameLen];
        long            snaNodeOperType;
        long            snaNodeOperXidFormat;
        long            snaNodeOperXid;
        long            snaNodeOperEnablingMethod;
        long            snaNodeOperLuTermDefault;
        long            snaNodeOperMaxLu;
        char            snaNodeOperHostDescription[DescriptionLen];
        long            snaNodeOperStopMethod;
        long            snaNodeOperState;
        char            snaNodeOperHostSscpId[NameLen];
        unsigned long   snaNodeOperStartTime;
        unsigned long   snaNodeOperLastStateChange;
        unsigned long   snaNodeOperActFailures;
        long            snaNodeOperActFailureReason;
}SnaNodeOperEntry_t;


typedef
struct _snaPu20StatsEntry {
        long            snaNodeAdminIndex;
        unsigned long   snaPu20StatsSentBytes;
        unsigned long   snaPu20StatsReceivedBytes;
        unsigned long   snaPu20StatsSentPius;
        unsigned long   snaPu20StatsReceivedPius;
        unsigned long   snaPu20StatsSentNegativeResps;
        unsigned long   snaPu20StatsReceivedNegativeResps;
        unsigned long   snaPu20StatsActLus;
        unsigned long   snaPu20StatsInActLus;
        unsigned long   snaPu20StatsBindLus;
} SnaPu20StatsEntry_t;

/*
 * The snaNodeLinkAdminEntry family. 
 */


typedef
struct _snaNodeLinkAdminEntry {
        long            snaNodeAdminIndex;
        long            snaNodeLinkAdminIndex;
        long            snaNodeLinkAdminMaxPiu;
        long            snaNodeLinkAdminRowStatus;
} SnaNodeLinkAdminEntry_t;


/*
 * The snaNodeLinkOperEntry family. 
 */


typedef
struct _snaNodeLinkOperEntry {
        long            snaNodeAdminIndex;
        long            snaNodeLinkAdminIndex;
        long            snaNodeLinkOperMaxPiu;
} SnaNodeLinkOperEntry_t;


/*
 * The snaLuAdminEntry family. 
 */


typedef
struct _snaLuAdminEntry {
        long            snaNodeAdminIndex;
        long            snaLuAdminLuIndex;
        char            snaLuAdminName[LuNameLen];
        char            snaLuAdminSnaName[LuSnaNameLen];
        unsigned char   snaLuAdminLocalAddress;
        char            aligned[2];          /* LuSnaNameLen = 17, aligned to 20 */
        long            snaLuAdminType;
        long            snaLuAdminDepType;
        char            snaLuAdminDisplayModel[LuModelLen];
        long            snaLuAdminTerm;
        long            snaLuAdminRowStatus;
} SnaLuAdminEntry_t;


/*
 * The snaLuOperEntry family. 
 */


typedef
struct _snaLuOperEntry {
        long            snaNodeAdminIndex;
        long            snaLuAdminLuIndex;
        char            snaLuOperName[LuNameLen];
        char            snaLuOperSnaName[LuSnaNameLen];
        unsigned char   snaLuOperLocalAddress;
        char            aligned[2];         /* LuSnaNameLen = 17, aligned to 20 */
        long            snaLuOperType;
        long            snaLuOperDepType;
        char            snaLuOperDisplayModel[LuModelLen];
        long            snaLuOperTerm;
        long            snaLuOperState;
        unsigned long   snaLuOperSessnCount;
} SnaLuOperEntry_t;

/*
 * The snaLuSessnEntry family. 
 */


typedef
struct _snaLuSessnEntry {
        long            snaNodeAdminIndex;
        long            snaLuAdminLuIndex;
        long            snaLuSessnIndex;
        char            snaLuSessnLocalApplName[LuNameLen];
        char            snaLuSessnRemoteLuName[LuSnaNameLen];
        char            aligned[3];     /* LuSnaNameLen = 17 */
        long            snaLuSessnMaxSndRuSize;
        long            snaLuSessnMaxRcvRuSize;
        long            snaLuSessnSndPacingSize;
        long            snaLuSessnRcvPacingSize;
        unsigned long   snaLuSessnActiveTime;
        long            snaLuSessnAdminState;
        long            snaLuSessnOperState;
        long            snaLuSessnSenseData;
        long            snaLuSessnTerminationRu;
        long            snaLuSessnUnbindType;
} SnaLuSessnEntry_t;



/*
 * The snaLuSessnStatsEntry family. 
 */

typedef
struct _snaLuSessnStatsEntry {
        long            snaNodeAdminIndex;
        long            snaLuAdminLuIndex;
        unsigned long   snaLuSessnStatsSentBytes;
        unsigned long   snaLuSessnStatsReceivedBytes;
        unsigned long   snaLuSessnStatsSentRus;
        unsigned long   snaLuSessnStatsReceivedRus;
        unsigned long   snaLuSessnStatsSentNegativeResps;
        unsigned long   snaLuSessnStatsReceivedNegativeResps;
} SnaLuSessnStatsEntry_t;


/*
 * The snaLuRtmEntry family. 
 */


typedef
struct _snaLuRtmEntry {
        long            snaLuRtmPuIndex;
        long            snaLuRtmLuIndex;
        long            snaLuRtmState;
        unsigned long   snaLuRtmStateTime;
        long            snaLuRtmDef;
        long            snaLuRtmBoundary1;
        long            snaLuRtmBoundary2;
        long            snaLuRtmBoundary3;
        long            snaLuRtmBoundary4;
        unsigned long   snaLuRtmCounter1;
        unsigned long   snaLuRtmCounter2;
        unsigned long   snaLuRtmCounter3;
        unsigned long   snaLuRtmCounter4;
        unsigned long   snaLuRtmOverFlows;
        long            snaLuRtmObjPercent;
        long            snaLuRtmObjRange;
        long            snaLuRtmNumTrans;
        long            snaLuRtmLastRspTime;
        long            snaLuRtmAvgRspTime;
} SnaLuRtmEntry_t;


#endif
