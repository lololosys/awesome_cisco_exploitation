/* $Id: dspu_fp.h,v 3.1 1995/11/09 11:28:16 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspu_fp.h,v $
 *------------------------------------------------------------------
 * Focal point operation.
 * This is part of dspu sub-system and it is the interface to the SNA Network
 * management. The dspu-subsystem can compile and link without sna network
 * management syb-system.
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dspu_fp.h,v $
 * Revision 3.1  1995/11/09  11:28:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:51  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef fp_h
#define fp_h

#include "dtypes.h"
#include "../cls/clsi_msg.h"
#include "nmtype.h"

typedef void RspRoutine(Pu* objP, DataReq_t* clsiBfrP);
typedef MV_SV_RC VectorSent(DataHeader* vector, char* correlator, hword corrleatorLen);
typedef word MVSRreceive(MV * vector, hword vecLen, 
                        char* correlator, hword correlatorLength);
typedef void FpStatusChanged (void);


/* fpVector, this is the interface to the FP API
 */
extern MVSRreceive*  fpVectorReceived;
extern FpStatusChanged* fpUp;
extern FpStatusChanged* fpDown;
extern FpStatusChanged* fpConfigured;

Fp* Fp_new(Pu* focalPu);

static inline void RegisterFpUp (FpStatusChanged* routine) {fpUp = routine;}

static inline void RegisterFpDown (FpStatusChanged* routine) {fpDown = routine;}

static inline void RegisterReceive (MVSRreceive* routine) {fpVectorReceived = routine;}

static inline void RegisterFpConfigured (FpStatusChanged* routine) {fpConfigured = routine;}

Pu*  GetFocalPu(void);

const char* GetFocalPuName(void);


MV_SV_RC Fp_VectorSent(hword totalBuf, DataReq_t** buffer);



/* A pu had been deleted, delete focal point if the pu is a focal point */
void Fp_PuDeleted(Pu* puP);

/* A SSCP-PU reqeust from host */
word             /* SNA sense code, 0 is OK */
Fp_ReqFromHost(Pu* hostPu, DataReq_t* buf);

/* A SSCP-PU response from host */
void Fp_ResponseReceived(DataReq_t* buf);

/* Send SSCP-PU data to focalPoint */
boolean    
Fp_Send(DataReq_t* buf, Pu* objP);

void
Fp_HostPuUp(Pu* puP);                   /* FocalPoint PU is coming up. */

void
Fp_GoingDown (Pu* this);                /* FocalPoint PU is going down. */

/* The focalpoint pu is ready to send data */
void SscpClearToSend(Pu* puP); 

void Fp_ReqVectorReceive(DataReq_t* buf);

void Fp_RspVectorReceive(DataReq_t* buf);

boolean IsFocalPointExisted(void);

void ProcessFpData(void);

#endif
