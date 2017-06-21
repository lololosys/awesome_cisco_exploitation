/* $Id: rtt_alert_int.h,v 3.1.4.2 1996/06/07 22:24:33 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/dspu/rtt_alert_int.h,v $
 *------------------------------------------------------------------
 * SNA Network Management resolutions
 *
 * Jan 96, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rtt_alert_int.h,v $
 * Revision 3.1.4.2  1996/06/07  22:24:33  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:49:13  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.2  1996/05/14  06:18:30  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.1.2.1  1996/03/22  18:50:53  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:44  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Do not reformat!!  READ READ READ READ
 *  The following is a tweeked version generated from the 
 *  SNA Alert compiler.  The changes allow them to be used
 *  as SNA Resolutions.
 */

static const struct
  rttMonConnectionChangeNotification_Resolve
 { unsigned char A_altLenHi, A_altLenLo, A_altKeyHi, A_altKeyLo;
  unsigned char A_genLen, A_genKey;
  unsigned char A_type, A_descrHi, A_descrLo;
  unsigned char A_relLen, A_relKey;
  unsigned char A_reltime[5];
  unsigned char A_cpLen, A_cpKey;
  unsigned char A_cpc0Hi, A_cpc0Lo;
  unsigned char A_d0Len;
  unsigned char A_f0[1];
  unsigned char A_d1Len;
  unsigned char A_f1[3];
  unsigned char cpRhName [0x8];
  unsigned char A_f2[3];
  unsigned char appRhName [0x7];
  unsigned char A_f3[2];
  unsigned char A_d2Len;
  unsigned char A_f4[1];
  unsigned char A_d3Len;
  unsigned char A_f5[2];
  unsigned char netID [0x8];
  unsigned char cpIIdName [0x8];
  unsigned char appIIdName [0x8];
  unsigned char epochTime [0x8];
  unsigned char A_d4Len;
  unsigned char A_f6[1];
  unsigned char A_d5Len;
  unsigned char A_f7[3];

} RTTMONCONNCHNGRESOLVE={0x00,0x00,0,2
,0x05,0x92,0x07,0x33,0x05
,0x07,0x42,{0,0,0,0,0}
,0x04,0x93,0x20,0x03
,0x1A,{0x05}
,0x18,{0x10,0x80,0x09},{0,0,0,0,0,0,0,0},{0x20,0xF4,0x08},{0,0,0,0,0,0,0},{0x40,0x40}
,0x25,{0x4A}
,0x23,{0x01,0x01},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}
,0x06,{0x97}
,0x04,{0x86,0x40,0x00}
};

#define RTTMONCONNCHNGRESOLVE_H

static const struct
  rttMonThresholdNotification_Resolve
 { unsigned char A_altLenHi, A_altLenLo, A_altKeyHi, A_altKeyLo;
  unsigned char A_genLen, A_genKey;
  unsigned char A_type, A_descrHi, A_descrLo;
  unsigned char A_relLen, A_relKey;
  unsigned char A_reltime[5];
  unsigned char A_cpLen, A_cpKey;
  unsigned char A_cpc0Hi, A_cpc0Lo;
  unsigned char A_d0Len;
  unsigned char A_f0[1];
  unsigned char A_d1Len;
  unsigned char A_f1[3];
  unsigned char cpRhName [0x8];
  unsigned char A_f2[3];
  unsigned char appRhName [0x7];
  unsigned char A_f3[2];
  unsigned char A_d2Len;
  unsigned char A_f4[1];
  unsigned char A_d3Len;
  unsigned char A_f5[2];
  unsigned char netID [0x8];
  unsigned char cpIIdName [0x8];
  unsigned char appIIdName [0x8];
  unsigned char epochTime [0x8];
  unsigned char A_d4Len;
  unsigned char A_f6[1];
  unsigned char A_d5Len;
  unsigned char A_f7[3];

} RTTMONTHRESHOLDRESOLVE={0x00,0x00,0,2
,0x05,0x92,0x07,0x40,0x11
,0x07,0x42,{0,0,0,0,0}
,0x04,0x93,0x40,0x03
,0x1A,{0x05}
,0x18,{0x10,0x80,0x09},{0,0,0,0,0,0,0,0},{0x20,0xF4,0x08},{0,0,0,0,0,0,0},{0x40,0x40}
,0x25,{0x4A}
,0x23,{0x01,0x01},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}
,0x06,{0x97}
,0x04,{0x86,0x40,0x00}
};

#define RTTMONTHRESHOLDRESOLVE_H

static const struct
  rttMonTimeoutNotification_Resolve 
 { unsigned char A_altLenHi, A_altLenLo, A_altKeyHi, A_altKeyLo;
  unsigned char A_genLen, A_genKey;
  unsigned char A_type, A_descrHi, A_descrLo;
  unsigned char A_relLen, A_relKey;
  unsigned char A_reltime[5];
  unsigned char A_cpLen, A_cpKey;
  unsigned char A_cpc0Hi, A_cpc0Lo;
  unsigned char A_d0Len;
  unsigned char A_f0[1];
  unsigned char A_d1Len;
  unsigned char A_f1[3];
  unsigned char cpRhName [0x8];
  unsigned char A_f2[3];
  unsigned char appRhName [0x7];
  unsigned char A_f3[2];
  unsigned char A_d2Len;
  unsigned char A_f4[1];
  unsigned char A_d3Len;
  unsigned char A_f5[2];
  unsigned char netID [0x8];
  unsigned char cpIIdName [0x8];
  unsigned char appIIdName [0x8];
  unsigned char epochTime [0x8];
  unsigned char A_d4Len;
  unsigned char A_f6[1];
  unsigned char A_d5Len;
  unsigned char A_f7[3];

} RTTMONTIMEOUTRESOLVE={0x00,0x6A,0,2
,0x05,0x92,0x07,0x40,0x02
,0x07,0x42,{0,0,0,0,0}
,0x04,0x93,0x40,0x03
,0x1A,{0x05}
,0x18,{0x10,0x80,0x09},{0,0,0,0,0,0,0,0},{0x20,0xF4,0x08},{0,0,0,0,0,0,0},{0x40,0x40}
,0x25,{0x4A}
,0x23,{0x01,0x01},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}
,0x06,{0x97}
,0x04,{0x86,0x40,0x00}
};

#define RTTMONTIMEOUTRESOLVE_H
