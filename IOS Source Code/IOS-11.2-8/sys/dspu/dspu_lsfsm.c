/* $Id: dspu_lsfsm.c,v 3.1.62.2 1996/08/28 03:58:24 bchan Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_lsfsm.c,v $
 *------------------------------------------------------------------
 * DSPU Link Station FSM Function.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This file contains the finite state machine table for link station
 * handling.
 *------------------------------------------------------------------
 * $Log: dspu_lsfsm.c,v $
 * Revision 3.1.62.2  1996/08/28  03:58:24  bchan
 * CSCdi65090:  NSP stops sending xid when switched major nodes inactivated
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/07/19  22:45:50  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  11:28:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:59  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LSFSM_C
#define __LSFSM_C

#include "master.h"
#include "dspu.h"
#include "dspu_ls.h"
#include "dspu_fsmstr.h"

const FsmStruct LsFsmTable[LsS_MaxState][LsI_MaxInput] = {

/* INPUT. LsS_Reset.        NEW STATE.       ACTION. */

/* LsI_StartLs. */         {{LsS_PendOut,    a1},
/* LsI_StopLs. */           {LsS_Reset,      a0},
/* LsI_ResetLs. */          {LsS_Reset,      a0},
/* LsI_IdStnInd. */         {LsS_PendIn,     a23},
/* LsI_IdStnIndPU. */       {LsS_PendInPu,   a23},
/* LsI_ReqOpnCnf. */        {LsS_Reset,      a0},
/* LsI_ReqOpnCnfX. */       {LsS_Reset,      a0},
/* LsI_IdIndCnfNull. */     {LsS_Reset,      a0},
/* LsI_IdIndCnf0. */        {LsS_Reset,      a0},
/* LsI_IdIndCnf3. */        {LsS_Reset,      a0},
/* LsI_ConnectLs. */        {LsS_Reset,      a0},
/* LsI_ConnectCnf. */       {LsS_Reset,      a0},
/* LsI_ConnectInd. */       {LsS_Reset,      a0},
/* LsI_ConnectedInd. */     {LsS_Reset,      a0},
/* LsI_SigStnCnf. */        {LsS_Reset,      a0},
/* LsI_DiscCnf. */          {LsS_Reset,      a0},
/* LsI_DiscInd. */          {LsS_Reset,      a0},
/* LsI_CloseCnf. */         {LsS_Reset,      a31},
/* LsI_DataInd. */          {LsS_Reset,      a0},
/* LsI_DataReq. */          {LsS_Reset,      a0},
/* LsI_FlowReqStop. */      {LsS_Reset,      a0},
/* LsI_FlowReqResume. */    {LsS_Reset,      a0},
/* LsI_Timeout. */          {LsS_Reset,      a0},
/* LsI_PuStopped. */        {LsS_Reset,      a0},
/* LsiXid3Err.  */          {LsS_Reset,      a0}},

/* INPUT. LsS_PendOut.      NEW STATE        ACTION. */

/* LsI_StartLs. */         {{LsS_PendOut,    a1},
/* LsI_StopLs. */           {LsS_PendOut,    a2},
/* LsI_ResetLs. */          {LsS_Reset,      a2},
/* LsI_IdStnInd. */         {LsS_PendClose,  a5},
/* LsI_IdStnIndPU. */       {LsS_PendOut,    a0},
/* LsI_ReqOpnCnf. */        {LsS_Xid,        a6},
/* LsI_ReqOpnCnfX. */       {LsS_PendOut,    a31},
/* LsI_IdIndCnfNull. */     {LsS_PendOut,    a0}, 
/* LsI_IdIndCnf0. */        {LsS_PendOut,    a0},
/* LsI_IdIndCnf3. */        {LsS_PendOut,    a0},
/* LsI_ConnectLs. */        {LsS_PendOut,    a0},
/* LsI_ConnectCnf. */       {LsS_PendOut,    a0},
/* LsI_ConnectInd. */       {LsS_PendOut,    a0},
/* LsI_ConnectedInd. */     {LsS_PendOut,    a0},
/* LsI_SigStnCnf. */        {LsS_PendOut,    a0},
/* LsI_DiscCnf. */          {LsS_PendOut,    a0},
/* LsI_DiscInd. */          {LsS_PendClose,  a16},
/* LsI_CloseCnf. */         {LsS_Reset,      a21},
/* LsI_DataInd. */          {LsS_PendOut,    a0},
/* LsI_DataReq. */          {LsS_PendOut,    a0},
/* LsI_FlowReqStop. */      {LsS_PendOut,    a0},
/* LsI_FlowReqResume. */    {LsS_PendOut,    a0},
/* LsI_Timeout. */          {LsS_PendOut,    a0},
/* LsI_PuStopped. */        {LsS_Reset,      a26},
/* LsiXid3Err.  */          {LsS_Reset,      a2}},

/* INPUT. LsS_PendIn.       NEW STATE        ACTION. */

/* LsI_StartLs. */         {{LsS_PendIn,     a0},
/* LsI_StopLs. */           {LsS_PendClose,  a3},
/* LsI_ResetLs. */          {LsS_Reset,      a15},
/* LsI_IdStnInd. */         {LsS_PendIn,     a0},
/* LsI_IdStnIndPU. */       {LsS_PendIn,     a0},
/* LsI_ReqOpnCnf. */        {LsS_PendXid,    a7},
/* LsI_ReqOpnCnfX. */       {LsS_PendIn,     a31},
/* LsI_IdIndCnfNull. */     {LsS_PendIn,     a0},
/* LsI_IdIndCnf0. */        {LsS_PendIn,     a0},
/* LsI_IdIndCnf3. */        {LsS_PendIn,     a0},
/* LsI_ConnectLs. */        {LsS_PendIn,     a0},
/* LsI_ConnectCnf. */       {LsS_PendIn,     a0},
/* LsI_ConnectInd. */       {LsS_PendIn,     a0},
/* LsI_ConnectedInd. */     {LsS_PendIn,     a0},
/* LsI_SigStnCnf. */        {LsS_PendIn,     a0},
/* LsI_DiscCnf. */          {LsS_PendIn,     a0},
/* LsI_DiscInd. */          {LsS_PendClose,  a16},
/* LsI_CloseCnf. */         {LsS_Reset,      a21},
/* LsI_DataInd. */          {LsS_PendIn,     a0},
/* LsI_DataReq. */          {LsS_PendIn,     a0},
/* LsI_FlowReqStop. */      {LsS_PendIn,     a0},
/* LsI_FlowReqResume. */    {LsS_PendIn,     a0},
/* LsI_Timeout. */          {LsS_PendIn,     a0},
/* LsI_PuStopped. */        {LsS_Reset,      a0},
/* LsiXid3Err.  */          {LsS_Reset,      a15}},

/* INPUT. LsS_PendInPu.     NEW STATE        ACTION. */

/* LsI_StartLs. */         {{LsS_PendInPu,   a0},
/* LsI_StopLs. */           {LsS_PendClose,  a3},
/* LsI_ResetLs. */          {LsS_Reset,      a32},
/* LsI_IdStnInd. */         {LsS_PendInPu,   a0},
/* LsI_IdStnIndPU. */       {LsS_PendInPu,   a0},
/* LsI_ReqOpnCnf. */        {LsS_Xid,        a7},
/* LsI_ReqOpnCnfX. */       {LsS_PendInPu,   a31},
/* LsI_IdIndCnfNull. */     {LsS_PendInPu,   a0},
/* LsI_IdIndCnf0. */        {LsS_PendInPu,   a0},
/* LsI_IdIndCnf3. */        {LsS_PendInPu,   a0},
/* LsI_ConnectLs. */        {LsS_PendInPu,   a0},
/* LsI_ConnectCnf. */       {LsS_PendInPu,   a0},
/* LsI_ConnectInd. */       {LsS_PendInPu,   a0},
/* LsI_ConnectedInd. */     {LsS_PendInPu,   a0},
/* LsI_SigStnCnf. */        {LsS_PendInPu,   a0},
/* LsI_DiscCnf. */          {LsS_PendInPu,   a0},
/* LsI_DiscInd. */          {LsS_PendClose,  a16},
/* LsI_CloseCnf. */         {LsS_Reset,      a21},
/* LsI_DataInd. */          {LsS_PendInPu,   a0},
/* LsI_DataReq. */          {LsS_PendInPu,   a0},
/* LsI_FlowReqStop. */      {LsS_PendInPu,   a0},
/* LsI_FlowReqResume. */    {LsS_PendInPu,   a0},
/* LsI_Timeout. */          {LsS_PendInPu,   a0},
/* LsI_PuStopped. */        {LsS_Reset,      a26},
/* LsiXid3Err.  */          {LsS_Reset,      a32}},


/* INPUT. LsS_PendXid.      NEW STATE        ACTION. */

/* LsI_StartLs. */         {{LsS_PendXid,    a0},
/* LsI_StopLs. */           {LsS_PendClose,  a3},
/* LsI_ResetLs. */          {LsS_PendClose,  a3},
/* LsI_IdStnInd. */         {LsS_PendXid,    a19},
/* LsI_IdStnIndPU. */       {LsS_PendXid,    a0},
/* LsI_ReqOpnCnf. */        {LsS_PendXid,    a31},
/* LsI_ReqOpnCnfX. */       {LsS_PendXid,    a31},
/* LsI_IdIndCnfNull. */     {LsS_PendXid,    a8},
/* LsI_IdIndCnf0. */        {LsS_Xid,        a10},
/* LsI_IdIndCnf3. */        {LsS_Xid,        a27},
/* LsI_ConnectLs. */        {LsS_PendXid,    a0},
/* LsI_ConnectCnf. */       {LsS_PendXid,    a0},
/* LsI_ConnectInd. */       {LsS_PendXid,    a0},
/* LsI_ConnectedInd. */     {LsS_PendXid,    a0},
/* LsI_SigStnCnf. */        {LsS_PendXid,    a0},
/* LsI_DiscCnf. */          {LsS_PendXid,    a0},
/* LsI_DiscInd. */          {LsS_PendClose,  a16},
/* LsI_CloseCnf. */         {LsS_Reset,      a21},
/* LsI_DataInd. */          {LsS_PendXid,    a0},
/* LsI_DataReq. */          {LsS_PendXid,    a0},
/* LsI_FlowReqStop. */      {LsS_PendXid,    a0},
/* LsI_FlowReqResume. */    {LsS_PendXid,    a0},
/* LsI_Timeout. */          {LsS_PendXid,    a0},
/* LsI_PuStopped. */        {LsS_PendClose,  a3},
/* LsiXid3Err.  */          {LsS_PendClose,  a3}},

/* INPUT. LsS_Xid.          NEW STATE        ACTION. */

/* LsI_StartLs. */         {{LsS_Xid,        a0},
/* LsI_StopLs. */           {LsS_PendClose,  a3},
/* LsI_ResetLs. */          {LsS_PendClose,  a3},
/* LsI_IdStnInd. */         {LsS_Xid,        a19},
/* LsI_IdStnIndPU. */       {LsS_Xid,        a0},
/* LsI_ReqOpnCnf. */        {LsS_Xid,        a31},
/* LsI_ReqOpnCnfX. */       {LsS_Xid,        a31},
/* LsI_IdIndCnfNull. */     {LsS_Xid,        a9},
/* LsI_IdIndCnf0. */        {LsS_Xid,        a11},
/* LsI_IdIndCnf3. */        {LsS_Xid,        a28},
/* LsI_ConnectLs. */        {LsS_ConnOut,    a12},
/* LsI_ConnectCnf. */       {LsS_Xid,        a0},
/* LsI_ConnectInd. */       {LsS_ConnIn,     a14},
/* LsI_ConnectedInd. */     {LsS_Xid,        a0},
/* LsI_SigStnCnf. */        {LsS_Xid,        a0},
/* LsI_DiscCnf. */          {LsS_Xid,        a0},
/* LsI_DiscInd. */          {LsS_PendClose,  a16},
/* LsI_CloseCnf. */         {LsS_Reset,      a21},
/* LsI_DataInd. */          {LsS_Xid,        a0},
/* LsI_DataReq. */          {LsS_Xid,        a0},
/* LsI_FlowReqStop. */      {LsS_Xid,        a0},
/* LsI_FlowReqResume. */    {LsS_Xid,        a0},
/* LsI_Timeout. */          {LsS_Xid,        a34},
/* LsI_PuStopped. */        {LsS_PendClose,  a3},
/* LsiXid3Err.  */          {LsS_Xid,        a33}},

/* INPUT. LsS_ConnOut.      NEW STATE        ACTION. */

/* LsI_StartLs. */         {{LsS_ConnOut,    a0},
/* LsI_StopLs. */           {LsS_PendDisc,   a4},
/* LsI_ResetLs. */          {LsS_PendClose,  a3},
/* LsI_IdStnInd. */         {LsS_ConnOut,    a19},
/* LsI_IdStnIndPU. */       {LsS_ConnOut,    a0},
/* LsI_ReqOpnCnf. */        {LsS_ConnOut,    a31},
/* LsI_ReqOpnCnfX. */       {LsS_ConnOut,    a31},
/* LsI_IdIndCnfNull. */     {LsS_ConnOut,    a0},
/* LsI_IdIndCnf0. */        {LsS_ConnOut,    a0},
/* LsI_IdIndCnf3. */        {LsS_ConnOut,    a0},
/* LsI_ConnectLs. */        {LsS_ConnOut,    a0},
/* LsI_ConnectCnf. */       {LsS_Conn,       a13}, 
/* LsI_ConnectInd. */       {LsS_ConnOut,    a14},
/* LsI_ConnectedInd. */     {LsS_ConnOut,    a0},
/* LsI_SigStnCnf. */        {LsS_ConnOut,    a0},
/* LsI_DiscCnf. */          {LsS_ConnOut,    a0},
/* LsI_DiscInd. */          {LsS_PendClose,  a16},
/* LsI_CloseCnf. */         {LsS_Reset,      a21},
/* LsI_DataInd. */          {LsS_ConnOut,    a0},
/* LsI_DataReq. */          {LsS_ConnOut,    a0},
/* LsI_FlowReqStop. */      {LsS_ConnOut,    a0},
/* LsI_FlowReqResume. */    {LsS_ConnOut,    a0},
/* LsI_Timeout. */          {LsS_ConnOut,    a0},
/* LsI_PuStopped. */        {LsS_PendClose,  a3},
/* LsiXid3Err.  */          {LsS_PendClose,  a3}},

/* INPUT. LsS_ConnIn.       NEW STATE        ACTION. */

/* LsI_StartLs. */         {{LsS_ConnIn,     a0},
/* LsI_StopLs. */           {LsS_PendDisc,   a4},
/* LsI_ResetLs. */          {LsS_PendClose,  a3},
/* LsI_IdStnInd. */         {LsS_ConnIn,     a19},
/* LsI_IdStnIndPU. */       {LsS_ConnIn,     a0},
/* LsI_ReqOpnCnf. */        {LsS_ConnIn,     a31},
/* LsI_ReqOpnCnfX. */       {LsS_ConnIn,     a31},
/* LsI_IdIndCnfNull. */     {LsS_ConnIn,     a0},
/* LsI_IdIndCnf0. */        {LsS_ConnIn,     a0},
/* LsI_IdIndCnf3. */        {LsS_ConnIn,     a0},
/* LsI_ConnectLs. */        {LsS_ConnIn,     a0},
/* LsI_ConnectCnf. */       {LsS_ConnIn,     a0},
/* LsI_ConnectInd. */       {LsS_ConnIn,     a0},
/* LsI_ConnectedInd. */     {LsS_Conn,       a22},
/* LsI_SigStnCnf. */        {LsS_ConnIn,     a0},
/* LsI_DiscCnf. */          {LsS_ConnIn,     a0},
/* LsI_DiscInd. */          {LsS_PendClose,  a16},
/* LsI_CloseCnf. */         {LsS_Reset,      a21},
/* LsI_DataInd. */          {LsS_ConnIn,     a0},
/* LsI_DataReq. */          {LsS_ConnIn,     a0},
/* LsI_FlowReqStop. */      {LsS_ConnIn,     a0},
/* LsI_FlowReqResume. */    {LsS_ConnIn,     a0},
/* LsI_Timeout. */          {LsS_ConnIn,     a0},
/* LsI_PuStopped. */        {LsS_PendClose,  a3},
/* LsiXid3Err.  */          {LsS_PendClose,  a3}},

/* INPUT. LsS_Conn.         NEW STATE        ACTION. */

/* LsI_StartLs. */         {{LsS_Conn,       a0},
/* LsI_StopLs. */           {LsS_PendDisc,   a4},
/* LsI_ResetLs. */          {LsS_PendClose,  a3},
/* LsI_IdStnInd. */         {LsS_Conn,       a19},
/* LsI_IdStnIndPU. */       {LsS_Conn,       a0},
/* LsI_ReqOpnCnf. */        {LsS_Conn,       a31},
/* LsI_ReqOpnCnfX. */       {LsS_Conn,       a31},
/* LsI_IdIndCnfNull. */     {LsS_Conn,       a0},
/* LsI_IdIndCnf0. */        {LsS_Conn,       a0},
/* LsI_IdIndCnf3. */        {LsS_Conn,       a0},
/* LsI_ConnectLs. */        {LsS_Conn,       a0},
/* LsI_ConnectCnf. */       {LsS_Conn,       a0},
/* LsI_ConnectInd. */       {LsS_Conn,       a0},
/* LsI_ConnectedInd. */     {LsS_Conn,       a0},
/* LsI_SigStnCnf. */        {LsS_Conn,       a0},
/* LsI_DiscCnf. */          {LsS_Conn,       a0},
/* LsI_DiscInd. */          {LsS_PendClose,  a16},
/* LsI_CloseCnf. */         {LsS_Reset,      a21},
/* LsI_DataInd. */          {LsS_Conn,       a17},
/* LsI_DataReq. */          {LsS_Conn,       a20},
/* LsI_FlowReqStop. */      {LsS_Conn,       a29},
/* LsI_FlowReqResume. */    {LsS_Conn,       a30},
/* LsI_Timeout. */          {LsS_Conn,       a0},
/* LsI_PuStopped. */        {LsS_PendDisc,   a24},
/* LsiXid3Err.  */          {LsS_PendClose,  a3}},

/* INPUT. LsS_PendDisc.     NEW STATE        ACTION. */

/* LsI_StartLs. */         {{LsS_PendDisc,   a0},
/* LsI_StopLs. */           {LsS_PendDisc,   a2},
/* LsI_ResetLs. */          {LsS_PendClose,  a3},
/* LsI_IdStnInd. */         {LsS_PendDisc,   a19},
/* LsI_IdStnIndPU. */       {LsS_PendDisc,   a0},
/* LsI_ReqOpnCnf. */        {LsS_PendDisc,   a31},
/* LsI_ReqOpnCnfX. */       {LsS_PendDisc,   a31},
/* LsI_IdIndCnfNull. */     {LsS_PendDisc,   a0},
/* LsI_IdIndCnf0. */        {LsS_PendDisc,   a0},
/* LsI_IdIndCnf3. */        {LsS_PendDisc,   a0},
/* LsI_ConnectLs. */        {LsS_PendDisc,   a0},
/* LsI_ConnectCnf. */       {LsS_PendDisc,   a0},
/* LsI_ConnectInd. */       {LsS_PendDisc,   a0},
/* LsI_ConnectedInd. */     {LsS_PendDisc,   a0},
/* LsI_SigStnCnf. */        {LsS_PendDisc,   a0},
/* LsI_DiscCnf. */          {LsS_PendClose,  a3},
/* LsI_DiscInd. */          {LsS_PendClose,  a3},
/* LsI_CloseCnf. */         {LsS_Reset,      a21},
/* LsI_DataInd. */          {LsS_PendDisc,   a0},
/* LsI_DataReq. */          {LsS_PendDisc,   a0},
/* LsI_FlowReqStop. */      {LsS_PendDisc,   a0},
/* LsI_FlowReqResume. */    {LsS_PendDisc,   a0},
/* LsI_Timeout. */          {LsS_PendDisc,   a0},
/* LsI_PuStopped. */        {LsS_PendClose,  a3},
/* LsiXid3Err.  */          {LsS_PendClose,  a3}},

/* INPUT. LsS_PendClose.    NEW STATE        ACTION. */

/* LsI_StartLs. */         {{LsS_PendClose,  a0},
/* LsI_StopLs. */           {LsS_PendClose,  a2},
/* LsI_ResetLs. */          {LsS_PendClose,  a2},
/* LsI_IdStnInd. */         {LsS_PendClose,  a19},
/* LsI_IdStnIndPU. */       {LsS_PendClose,  a0},
/* LsI_ReqOpnCnf. */        {LsS_Xid,        a7},
/* LsI_ReqOpnCnfX. */       {LsS_PendClose,  a25},
/* LsI_IdIndCnfNull. */     {LsS_PendClose,  a0},
/* LsI_IdIndCnf0. */        {LsS_PendClose,  a0},
/* LsI_IdIndCnf3. */        {LsS_PendClose,  a0},
/* LsI_ConnectLs. */        {LsS_PendClose,  a0},
/* LsI_ConnectCnf. */       {LsS_PendClose,  a0},
/* LsI_ConnectInd. */       {LsS_PendClose,  a0},
/* LsI_ConnectedInd. */     {LsS_PendClose,  a0},
/* LsI_SigStnCnf. */        {LsS_PendClose,  a0},
/* LsI_DiscCnf. */          {LsS_PendClose,  a0},
/* LsI_DiscInd. */          {LsS_PendClose,  a0},
/* LsI_CloseCnf. */         {LsS_Reset,      a21},
/* LsI_DataInd. */          {LsS_PendClose,  a0},
/* LsI_DataReq. */          {LsS_PendClose,  a0},
/* LsI_FlowReqStop. */      {LsS_PendClose,  a0},
/* LsI_FlowReqResume. */    {LsS_PendClose,  a0},
/* LsI_Timeout. */          {LsS_Reset,      a21},
/* LsI_PuStopped. */        {LsS_PendClose,  a0},
/* LsiXid3Err.  */          {LsS_PendClose,  a2}}

};

#endif  /* __LSFSM_C. */
/*---- End include file: lsfsm.c. */
