/* $Id: mv_parse.h,v 3.2 1995/11/17 09:05:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/mv_parse.h,v $
 *------------------------------------------------------------------
 * SNA Network Management Major vector parser.
 *  Provides the utilitities to find sub vector from major vector and
 * iteration macro to scan through the major and sub vector.
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mv_parse.h,v $
 * Revision 3.2  1995/11/17  09:05:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef mv_parse_h
#define mv_parse_h

#include "nmtype.h"

#define MV_ReplyRuncmd  0x61
#define MV_Runcmd       0x8061       


struct SV {
  /* Subvector/subfield layout */
  byte len;
  byte key;
} ;

 struct MV {
  /* Major vector layout */
  byte lenHi;
  byte lenLo;
  byte keyHi;
  byte keyLo;
  SV   sv;
} ;


/* Primitives for parsing vectors in contiguous memory. */

/* hword MV_Key(MV* mv);             returns Key value */
#define MV_Key(mv)  ((mv->keyHi << 8) + mv->keyLo)

#define SetMvKey(mv, mvKey) mv->keyHi = (mvKey) >> 8; \
                            mv->keyLo = (mvKey) & 0xff;

/* hword MV_Len(MV* mv);               returns Length */
#define MV_Len(mv) ((mv->lenHi << 8) + mv->lenLo)

#define SetMvLen(mv, mvLen)  mv->lenHi = (mvLen) >> 8;\
                             mv->lenLo = (mvLen) & 0xff;

/* find a major vector */
MV* MV_FindMV(MV* msVector, hword mvId, hword len);
MV* MV_NextMV(MV* msVector, hword len);

SV* MV_FindSV(MV* mv, byte svId);  /* finds spec'd subvector */

/* hword SV_Key(SV* sv);                returns Key value */
#define SV_Key(sv)  (sv->key)

/* hword SV_Len(SV* sv);                returns Length */
#define SV_Len(sv)  (sv->len)

SV * SV_FindSV(SV* sv, byte svId);  /*find embedded SV/SF */

/* the followings is used for sv iteration */
#define ForAllSvInMV(mv, sv, svLen) \
  for (svLen = MV_Len(mv) - 4, sv = (byte*)mv + 4; svLen > 0 && sv[0] <= svLen; svLen -= sv[0], sv += sv[0])

/* the following is for mv iteration, it assume vecLen and mv defined */
#define FOR_ALL_MV  \
   for(; vecLen > 4; vecLen -= MV_Len(mv), mv += MV_Len(mv))

#define ForAllSfInSV(sv, sf, sfLen) \
  for (sfLen = sv[0] - 2, sf = sv + 2; sfLen > 0 && sf[0] <= sfLen; sfLen -= sf[0], sf += sf[0])

#endif
