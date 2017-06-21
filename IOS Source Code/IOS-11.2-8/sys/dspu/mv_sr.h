/* $Id: mv_sr.h,v 3.2 1995/11/17 09:05:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/mv_sr.h,v $
 *------------------------------------------------------------------
 * SNA Network Management Product Major Vector API
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mv_sr.h,v $
 * Revision 3.2  1995/11/17  09:05:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:29:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef mv_sr_h
#define mv_sr_h

#include "mv_parse.h"
#include "nmtype.h"

#ifndef MVReceiverGeneric
#define MVReceiverGeneric MVReceiver
#endif

#define MaxAlertSize 200

typedef struct AlertQ_ {
    struct AlertQ_* next;
    char   but[MaxAlertSize];
 } AlertQ;
    
/* definition of MV receive proc */
typedef word MVReceive(MVReceiverGeneric*, MV * vector, hword vecLen, 
                        char* correlator, hword correlatorLength);

#define MVReceiverBaseClass \
   MVReceiver* next;  /* filled in when registered */ \
   hword  mvId;         /* major vector id to intercept */ \
   hword applIdLen;    /* if len 0, intercept any unclaimed */ \
   char  applId [8];   /* optional appl Id to intercept */ \
   MVReceive* vecRcv; /* addr of rcv proc */ \
   void (*fpInactive)(MVReceiverGeneric*); /* virtual function */


struct MVReceiver {
  MVReceiverBaseClass
};


/* usage of MVReceiver base class example:
* typedef struct MyMVReceiver MyMVReceiver;
* struct MyMVReceiver{
*   MVReceiverBaseClass
*   void*  mypointer;
* };
*
* #undef MVReceiverGeneric
* #define MVReceiverGeneric MyMVReceiver
*
*/



MV_OpRcv_RC               /* OK, Duplicate, Invalid */
MV_OpenReceive (       /* register object to receive vectors */
        MVReceiver *);      /* address of object to handle receive */


hword MV_CloseReceive (      /* deregister receive */
        MVReceiverGeneric *);

        /* returns: */
        /* 0 = OK */

MV_SV_RC       /* (OK, NoFp, Overflow) */
MV_SendVector (DataHeader* headerP,
                    char* correlator, hword correlatorLength);


MV_SV_RC       /* (OK, NoFp, Overflow) */
MV_SendAlert (MV* alert);

MV_SV_RC       /* (OK, NoFp, Overflow) */
MV_PuRegSend(DataHeader* head);   /* registration sent */

/* initializer - calls registration and returns result thereof */
MVReceiverGeneric*
MVR_MVReceiver (hword mvId,               /* major vector */
                char * applId,            /* null or id   */
                MVReceive* rcvProc,       /* proc to received */
                hword sizeofMVReceiverGeneric,
                MV_OpRcv_RC* rc);

/* destructor */
void MVR_Delete (MVReceiver* this);   /* calls deregistration */

void MVSRinit(void);


#define NoClients   0x080c0005
#define IllegalAppl 0x086b0650
#define InsufficientResource 0x08120000

#define MTO_MV_KEY 0x6F
#define MTO_NL_KEY 0x06
#define MTO_AN_KEY 0x01
#define MTO_AP_KEY 0x50
#define MTO_TX_KEY 0x13
#define MTO_SDT_KEY 0x31
#define MTO_CODESET_KEY 0x02
#define MTO_SDTSUB_KEY 0x30

#define MTO_BUFFSIZE 512
#define MTO_NAMELEN 8
#define MTO_LINELEN 80

#define IDX_MVLENHI 0
#define IDX_MVLENLO 1
#define IDX_GENLEN  4
#define IDX_NAMESVLEN 6
#define IDX_ASSOCNAMEKEY 7

struct mto_selfDefText_subvect {
  unsigned char selfDefinedTextLen, selfDefinedTextKey;
  unsigned char codeSetLen, codeSet, charSetHo, charSetLo, codePageHi, codePageLo;
  unsigned char textLen, textKey;
  unsigned char textBuff[MTO_LINELEN];
  struct mto_selfDefText_subvect * next;  /* Not data sent in "Msg To Operator" */
};

struct mto_name_subvect {
  unsigned char name[MTO_NAMELEN];
  unsigned char nameLen;
  struct mto_name_subvect * next;      /* Not data sent in "Msg To Operator" */
};

struct mto_major_vector { 
  unsigned char mvLenHi, mvLenLo, mvKeyHi, mvKeyLo;
  unsigned char nameListLen, nameListKey;
  unsigned char associatedNameLen, associatedNameKey;
};

struct mto_applName_subvect {
  unsigned char applNameLen, applNameKey, applName[8];
};

struct mto_text_subvect {
  unsigned char textLenHi, textLenLo, textKeyHi, textKeyLo;
};

void mto_fill_nameVector (struct mto_name_subvect *, uchar *);

boolean mto_fill_selfDefTextVector (struct mto_selfDefText_subvect *,
                                    unsigned char * );

MV_SV_RC MV_MessageToOperator (uchar*,
                               struct mto_name_subvect *,
                               struct mto_selfDefText_subvect *);
#endif

