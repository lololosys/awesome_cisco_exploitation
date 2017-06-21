/* $Id: luapi_test.c,v 3.1.4.3 1996/08/29 02:43:40 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/luapi_test.c,v $
 *------------------------------------------------------------------
 * LU API - LU built-in testcode.
 *
 * Sept 1995, Lesley Cox
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: luapi_test.c,v $
 * Revision 3.1.4.3  1996/08/29  02:43:40  lcox
 * CSCdi67526:  remove old edism logic from luapi and style cleanup
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/06/27  16:34:53  hampton
 * Keep your mitts off the scheduler private data structures.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:48:49  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.3  1996/04/24  00:50:14  lcox
 * Branch: IbuMod_Calif_branch
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.1.2.1  1996/03/22  18:50:49  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:39  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"

#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"

#include "luapi_test.h"
#include "dspu.h"
#include "snapiu.h"
#include "snautil.h"

#include "dspu_ls.h"
static void UpStreamFlowCtrl (int test)
{
    LinkStn *lsP;
    if ((lsP =  (LinkStn*)GetFirstChild(GetUplsRoot())) != NULL) {
        LsFlowControlFromPu(lsP, test == 1 ?  FlwCtl_Stop : FlwCtl_Resume);
    }
}
 



#define RCVBUFFLEN 5000
#define SNDBUFFLEN 5000


/* meanings for applNo values */
enum ApplNo {
    APPLNAME_NSPECHO = 0,
    APPLNAME_ITPECHOS = 1
};


/* names of applications corresponding to APPLNAME_* values */
static const char* const applName[] = {
  "NSPECHO",
  "ITPECHOS"
};


typedef struct luApi_param_
{
    const char *testName;
    pid_t testPid;
    char  hostName[18];
    byte  locaddr;
    byte  applNo;
    byte  applName[18];
    uint  applNameLen;
    byte  modeName[18];

    boolean* traceP;

    boolean testPassed;

    LuApiReturnCode expectedOpenCode;

    byte rcv [RCVBUFFLEN];
    byte expectedRcv [RCVBUFFLEN];
    uint expectedRcvLength;     /* note: may be beyond expectedRcv size */
    uint rcvdLength;             /* a counter for multiple segment rcvs */
    byte send [SNDBUFFLEN];

    void * luHandle;  /* place to save luapi Handle */

    boolean wakeupByRcv;  /* FALSE=>wakeup by kill */

    watched_boolean * wb;

} luApi_param;



static void CreateAndWatchBoolean (watched_boolean ** wb)
{
    *wb = create_watched_boolean("testluapiWb", 1);
    if (*wb == NULL)
        buginf("\ntestLuapi: failed to create watched boolean\n");
    else {
        process_set_boolean( *wb, FALSE);
        process_watch_boolean( *wb, ENABLE, RECURRING);
    }
}

static void UnwatchAndDeleteBoolean (watched_boolean ** wb)
{
    process_watch_boolean( *wb, DISABLE, FALSE);
    delete_watched_boolean( wb);
}


static void BeginTest (luApi_param *  params)
{
    buginf("\n%s-%u: BEGIN TEST %s LU %d APPLNO %d",
           params->testName, params->testPid,
           params->hostName, params->locaddr, params->applNo);
}


static void EndTest (luApi_param *  params)
{
    if (params->testPassed)
        buginf("\n%s-%u: TEST PASSED\n",
               params->testName, params->testPid);
    else
        buginf("\n%s-%u: TEST FAILED\n",
               params->testName, params->testPid);
}


static void KillTest (luApi_param *  params)
{
    EndTest(params);

    if (params->wb)
        UnwatchAndDeleteBoolean(&(params->wb));

    free(params);

    process_kill(THIS_PROCESS);
}


static void closeIndication (
             void * applicationHandle,
             byte * bufferPtr)
{
    luApi_param *   params;
    params = (luApi_param*) applicationHandle;

    if (*(params->traceP)) {
        buginf("\n%s-%u: closeIndication Called\n",
               params->testName, params->testPid);
        buginf("\t applicationHandle = %u\n", applicationHandle);
        buginf("\t bufferPtr = %u\n", bufferPtr);
    }

    params->wakeupByRcv = FALSE;

    process_set_boolean( params->wb, TRUE);
}


static void printbytes (byte *  bufferPtr,
                        uint    bufferLen,
                        boolean indent)
{
    uint j;
  
    j = 0;
    for (; bufferLen > 0; bufferLen--) {
        if (indent) {
            if (j++ == 0)
                buginf(" \n\t               ");
            else if (j == 9)
                buginf("  ");
            else if (j == 16)
                j = 0;
        }
        if (*bufferPtr < 16)
            buginf("0");
        buginf("%x", *(bufferPtr++));
    }
    if (indent)
        buginf(" \n");
}


static const char* const luApiReturnCodes[] = {
    "Success",
    "InsufficientMemory",
    "InvalidHandle",
    "HostConnInactive",
    "NoLuAvailable",
    "HostApplInvalid",
    "BindFailure",
    "FlowClosed",
    "IRMViolation"
};

static void printrc (LuApiReturnCode rc)
{
    if (rc <= 8)
        buginf("%u (%s)", rc, luApiReturnCodes[rc]);
    else
        buginf("%u", rc);
}


static void luReceiveIndication (
             void     * applicationHandle,
             byte     * bufferPtr,
             uint       bufferLen,
             boolean    lastFrame,
             hword      systemSense,
             hword      userSense)
{
    uint  i;
    luApi_param *   params;
    params = (luApi_param*) applicationHandle;

    if (*(params->traceP)) {
        buginf("\n%s-%u: luReceiveIndication called\n",
               params->testName, params->testPid);
        buginf("\t applicationHandle = %u\n", applicationHandle);
        buginf("\t buffer  Len = %u\n", bufferLen);
        buginf("\t         Data:");
        if (bufferLen < RCVBUFFLEN)
            i = bufferLen;
        else
            i = RCVBUFFLEN;
        printbytes(bufferPtr, i, TRUE);
        if (bufferLen > RCVBUFFLEN)
            buginf("\t               truncated!\n");
        buginf("\t lastFrame = %s\n", (lastFrame? "TRUE": "FALSE"));
        buginf("\t systemSense = %u\n", systemSense);
        buginf("\t userSense = %u\n", userSense);
    }
  
    if (systemSense || userSense) {
        buginf("\n%s-%u: Unexpected sense of rcv Ind: 0x%x 0x%x\n",
               params->testName, params->testPid, systemSense, userSense);
      
        params->wakeupByRcv = TRUE;
        process_set_boolean( params->wb, TRUE);

    } else {
        boolean ok = TRUE;
    
        if (params->expectedRcvLength) {
            /* confirm rcvd data is correct when expected value is setup */
        
            if (lastFrame) {
                if (params->rcvdLength + bufferLen != params->expectedRcvLength)
                    ok = FALSE;
            } else {
                if (params->rcvdLength + bufferLen >= params->expectedRcvLength)
                ok = FALSE;
            }
            if (!ok)
                buginf(
          "\n%s-%u: Length of rcvd data (%d) differs from expected (%d)\n",
                 params->testName, params->testPid,
                 params->rcvdLength + bufferLen,
                 params->expectedRcvLength); 
            else {
                int a = 0;
                int b = bufferLen;
                if (b >= RCVBUFFLEN)
                    b = RCVBUFFLEN - 1;
                while (ok && (a < b))
                if (bufferPtr[a++] !=
                      params->expectedRcv[params->rcvdLength++]) {
                    ok = FALSE;
                    buginf(
          "\n%s-%u: Received data mismatch: want 0x%x, got 0x%x at byte %u\n",
                       params->testName, params->testPid,
                       params->expectedRcv[--params->rcvdLength],
                       bufferPtr[--a], params->rcvdLength);
                }
            }
            if (!ok)
                params->testPassed = FALSE;
        }

        if (lastFrame) {
            params->wakeupByRcv = TRUE;
            process_set_boolean( params->wb, TRUE);

        } else {
            LuApiReturnCode rc;

            if (*(params->traceP))
                buginf("\n%s-%u: Calling luReceiveContinue ... \n",
                       params->testName, params->testPid);

            rc = luReceiveContinue( params->luHandle, RCVBUFFLEN,
                                    params->rcv );

            if (*(params->traceP)) {
                buginf("\n%s-%u: Called luReceiveContinue ... rc: ",
                  params->testName, params->testPid);
                printrc(rc);
                buginf(" \n");
            }
        }
    }
}


static void DoLuReceive (luApi_param *  params)
{
    LuApiReturnCode rc;

    if (*(params->traceP))
        buginf("\n%s-%u: Calling luReceive ... \n",
               params->testName, params->testPid);

    rc = luReceive( params->luHandle, RCVBUFFLEN, params->rcv,
              &luReceiveIndication );

    if (*(params->traceP)) {
        buginf("\n%s-%u: Return from luReceive ... rc: ",
               params->testName, params->testPid);
        printrc(rc);
        buginf(" \n");
    }
}

static void sscpLuReceiveIndication (
             void     * applicationHandle,
             byte     * bufferPtr,
             uint       bufferLen,
             boolean    lastFrame,
             hword      systemSense,
             hword      userSense)
{
    uint  i;
    luApi_param *   params;
    params = (luApi_param*) applicationHandle;

    if (*(params->traceP)) {
        buginf("\n%s-%u: sscpluReceiveIndication called\n",
               params->testName, params->testPid);
        buginf("\t applicationHandle = %u\n", applicationHandle);
        buginf("\t buffer  Len = %u\n", bufferLen);
        buginf("\t         Data:");
        if (bufferLen < RCVBUFFLEN)
           i = bufferLen;
        else
           i = RCVBUFFLEN;
        printbytes(bufferPtr, i, TRUE);
        if (bufferLen > RCVBUFFLEN)
           buginf("\t               truncated!\n");
        buginf("\t lastFrame = %s\n", (lastFrame? "TRUE": "FALSE"));
        buginf("\t systemSense = %u\n", systemSense);
        buginf("\t userSense = %u\n", userSense);
    }
  
    if (systemSense || userSense) {
        buginf("\n%s-%u: Unexpected sense of rcv Ind: %u %u\n",
               params->testName, params->testPid,
               systemSense, userSense);
        params->testPassed = FALSE;
        
        params->wakeupByRcv = TRUE;
        process_set_boolean( params->wb, TRUE);

    } else {
        if (lastFrame) {
            params->wakeupByRcv = TRUE;
            process_set_boolean( params->wb, TRUE);
            params->rcvdLength = bufferLen;

        } else {
  
            LuApiReturnCode rc;

            if (*(params->traceP))
                buginf("\n%s-%u: Calling sscpLuReceiveContinue ... \n",
                       params->testName, params->testPid);

            rc = sscpLuReceiveContinue( params->luHandle,
                                        RCVBUFFLEN, params->rcv );

            if (*(params->traceP)) {
                buginf("\n%s-%u: Called sscpLuReceiveContinue ... rc: ",
                       params->testName, params->testPid);
                printrc(rc);
                buginf(" \n");
            }
        }
    }
}

static void DoSscpLuReceive (luApi_param *  params)
{
    LuApiReturnCode rc;

    if (*(params->traceP))
        buginf("\n%s-%u: Calling sscpLuReceive ... handle: %u\n",
               params->testName, params->testPid);

    rc = sscpLuReceive( params->luHandle, RCVBUFFLEN,
                        params->rcv, &sscpLuReceiveIndication );

    if (*(params->traceP)) {
        buginf("\n%s-%u: Return from sscpLuReceive ... rc: ",
               params->testName, params->testPid);
        printrc(rc);
        buginf(" \n");
    }
}


static boolean DoLuSend (luApi_param *  params,
                         uint frameLen
                        )
{
    LuApiReturnCode rc;

    if (*(params->traceP))
        buginf("\n%s-%u: Calling luSend ... \n",
               params->testName, params->testPid);

    rc = luSend( params->luHandle, frameLen, params->send );

    if (*(params->traceP)) {
        buginf("\n%s-%u: Return from luSend ... ",
               params->testName, params->testPid);
        printrc(rc);
        buginf(" \n");
    }

    return (rc == Success);
}

static void DoSscpLuSend (luApi_param *  params,
                          uint  bufferLen
                         )
{
    LuApiReturnCode rc;

    if (*(params->traceP))
        buginf("\n%s-%u: Calling sscpLuSend ... \n",
               params->testName, params->testPid);

    rc = sscpLuSend( params->luHandle, bufferLen, params->send );

    if (*(params->traceP)) {
        buginf("\n%s-%u: Return from sscpLuSend ... ",
               params->testName, params->testPid);
        printrc(rc);
        buginf(" \n");
    }
}

static boolean DoWait (luApi_param *  params,
                       void (*doReceive)(luApi_param *) )
{
    ulong major;
    ulong minor;
    boolean done = FALSE;

    while (!done) {
        if (*(params->traceP))
            buginf("\n%s-%u: DoWait asweep!\n",
                   params->testName, params->testPid);

        process_wait_for_event();

        if (*(params->traceP))
            buginf("\n%s-%u: DoWait wake!\n",
                   params->testName, params->testPid);

        while (process_get_wakeup(&major, &minor)) { 
            if (major == BOOLEAN_EVENT) {

                /* reset boolean */
                process_set_boolean( params->wb, FALSE);

                if (params->wakeupByRcv) {
                    /* reissue receive */
                    (*doReceive)(params);
                    done = TRUE;
                } else
                    return (FALSE);  /* closing */

            } else {
                buginf("\n%s-%u: unexpected wakeup: major %u, minor %u\n",
                       params->testName, params->testPid, major, minor);
            }
        }
    }
    return (TRUE);  /* to continue */
}

static void DoLuClose (luApi_param *params)
{
    LuApiReturnCode rc;
    byte *          bfrReturn;

    /* close lu-lu api */
    rc = luClose( params->luHandle, &bfrReturn);
    if (*(params->traceP)) {
        buginf("\n%s-%u: LuApiReturnCode for luClose is ",
               params->testName, params->testPid);
        printrc(rc);
        buginf(" \n\t luHandle = %u", params->luHandle);
        buginf(" \n\t bfrReturn = %u\n", bfrReturn);
    }
  
    if (rc != Success) {
         buginf("\n%s-%u: Unexpected return to luClose (",
                params->testName, params->testPid);
         printrc(rc);
         buginf(")\n");
    }

    if (bfrReturn != params->rcv)
         buginf("\n%s-%u: Unexpected buffer return from luClose\n",
                params->testName, params->testPid);
}


static void FillABCD (byte *buffer, int fillLength)
{
    uint i = 0;
    while (fillLength >= 26) {
        ConvertAsciiToEbcdic("ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                             &buffer[i],
                             26);
        fillLength -= 26;
        i += 26;
    }
    if (fillLength > 0)
        ConvertAsciiToEbcdic("ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                             &buffer[i],
                             fillLength );
}


static void BuildNspEchoReqAndRsp (luApi_param *  params,
                                   hword          seqNo,
                                   word           echoLength,
                                   int            agentDataLength
                                  )
{
    ConvertAsciiToEbcdic("WxYz", &(params->send[0]), 4);
    params->send[4] = 0x01; /* version */
    params->send[5] = 0x02; /* Echo Request */
    *((hword *)(&(params->send[6]))) = seqNo;
    *((word *)(&(params->send[8]))) = echoLength;
    FillABCD(&(params->send[12]), agentDataLength);

    ConvertAsciiToEbcdic("WxYz", &(params->expectedRcv[0]), 4);
    params->expectedRcv[4] = 0x01; /* version */
    params->expectedRcv[5] = 0x03; /* Echo Response */
    *((hword *)(&(params->expectedRcv[6]))) = seqNo;
    *((word *)(&(params->expectedRcv[8]))) = echoLength;

    FillABCD(&(params->expectedRcv[12]), echoLength);

    params->expectedRcvLength = 12 + echoLength;
    params->rcvdLength = 0;
}  


static void NspEchoLoop (luApi_param *  params,
                         uint startAgentLength,
                         uint startSeqNo,
                         uint startLength,
                         uint stopLength,
                         uint increment)
{
    uint agentDataLength = startAgentLength;
    uint echoLength = startLength;
    uint seqNo = startSeqNo;
    boolean terminate = FALSE;

    while (echoLength <= stopLength) {
        BuildNspEchoReqAndRsp( params, seqNo++, echoLength, agentDataLength);

        if (!DoLuSend( params, 12 + agentDataLength)) {
            terminate = TRUE;
            DoLuClose(params);
        } else if (!DoWait( params, DoLuReceive))
            terminate = TRUE;

        if (terminate) {
            buginf(
       "\n%s-%u Echoloop terminated. SeqNo: %u. ReqLen: %u. EchoLen: %u",
                   params->testName, params->testPid,
                   seqNo - 1, agentDataLength, echoLength);
            KillTest(params); /* no return */
        }

        echoLength += increment;
        agentDataLength += increment;
    }
}

static const byte itpechosReqHeader[] = {
    0x7d, 0xd7, 0xe8, 0x11, 0xd1, 0x60
};

static const byte itpechosRspHeader[] = {
    0xF1, 0xC3, 0x11, 0xD1, 0x60, 0x13, 0x11, 0x40,
    0x40, 0x3C, 0x4E, 0x7F, 0x00, 0x11, 0x40, 0x40, 
    0x12, 0x40, 0x40
};

static void BuildItpEchosReqAndRsp (luApi_param * params,
                                    int           length)
{
    memcpy( &(params->send[0]), itpechosReqHeader,
            sizeof(itpechosReqHeader)); 
    FillABCD( &(params->send[sizeof(itpechosReqHeader)]),
              length);

    memcpy( &(params->expectedRcv[0]), itpechosRspHeader,
            sizeof(itpechosRspHeader)); 
    FillABCD( &(params->expectedRcv[sizeof(itpechosRspHeader)]),
              length);

    params->expectedRcvLength = sizeof(itpechosRspHeader) + length;
    params->rcvdLength = 0;
}


static void ItpEchosLoop (luApi_param *  params,
                         uint startLength,
                         uint stopLength,
                         uint increment)
{
    uint echoLength = startLength;
    boolean terminate = FALSE;

    while (echoLength <= stopLength) {
        BuildItpEchosReqAndRsp( params, echoLength);

        if (!DoLuSend( params, sizeof(itpechosReqHeader) + echoLength)) {
            terminate = TRUE;
            DoLuClose(params);
        } else if (!DoWait( params, DoLuReceive))
            terminate = TRUE;

        if (terminate) {
            buginf("\n%s-%u Echoloop terminated. EchoLen: %u",
                   params->testName, params->testPid, echoLength);
            KillTest(params); /* no return */
        }

        echoLength += increment;
    }
}


static void DoLuT0orT2Open (luApi_param *  params)
{
    LuApiReturnCode rc;
    void *          luHandle = NULL;
    byte            locaddr;
    hword           sysSense;
    hword           userSense;
    byte            initSelfData[30];
    byte            bindDataBuffer[20];
    uint            bindDataCnt;
    uint            i;

    params->testPassed = FALSE;

    ConvertAsciiToEbcdic(INIT_SELF_SIGNATURE, initSelfData, 7);
    initSelfData[7] = 0x01;
    ConvertAsciiToEbcdic(MYRTNAME, &(initSelfData[8]), strlen(MYRTNAME));
    locaddr = params->locaddr;

    rc = luT0orT2QueryPu(params->hostName);
    if (rc != params->expectedOpenCode) {
        if ((params->expectedOpenCode == Success) ||
            (params->expectedOpenCode == InvalidHandle) ||
            (params->expectedOpenCode == HostConnInactive)) {
            buginf("\n%s-%u: Unexpected return for luT0orT2QueryPu (%s) is ",
              params->testName, params->testPid, params->hostName);
            printrc(rc);
            buginf(" \n");
        }
    }
  
    rc = luT0orT2Open(
            params,
            &luHandle,
            params->hostName,
            &locaddr,
            params->applNameLen,
            params->applName,
            params->modeName,
            strlen(MYRTNAME) + 8,
            initSelfData,
            &closeIndication,
            &sysSense,
            &userSense,
            sizeof(bindDataBuffer),
            bindDataBuffer,
            &bindDataCnt
                );

    if (*(params->traceP)) {
        buginf("\n%s-%u: LuApiReturnCode for luT0orT2Open is ",
               params->testName, params->testPid);
        printrc(rc);
        buginf(" \n\t applicationHandle = %u", params);
        buginf(" \n\t luHandle = %u", luHandle);
        buginf(" \n\t hostName = %s", params->hostName);
        buginf(" \n\t locaddr = %u", locaddr);
        buginf(" \n\t applNameLen = %u", params->applNameLen);
        buginf(" \n\t applName = ");
        printbytes((byte*)(params->applName), params->applNameLen, FALSE);
        buginf(" \n\t modeName = ");
        printbytes((byte*)(params->modeName), 8, FALSE);
        buginf(" \n\t userDataLen = %u", strlen(MYRTNAME) + 8);
        buginf(" \n\t userData = ");
        printbytes((byte*)(initSelfData), strlen(MYRTNAME) + 8, FALSE);
        buginf(" \n\t sysSense = %x\t userSense = %x", sysSense, userSense);
        buginf(" \n\t bindDataBufferLength = %u", sizeof(bindDataBuffer));
        buginf(" \n\t bindDataRcvdLength = %u", bindDataCnt);
        buginf(" \n\t bindData = ");
        if (bindDataCnt < sizeof(bindDataBuffer))
            i = bindDataCnt;
        else
            i = sizeof(bindDataBuffer);
        printbytes((byte*)(bindDataBuffer), i, TRUE);
    }
  
    if (rc != params->expectedOpenCode) {
         buginf("\n%s-%u: Unexpected return to luT0orT2Open (",
                params->testName, params->testPid);
         printrc(rc);
         buginf(")\n");
    }

    if (rc) {
         if (rc == params->expectedOpenCode)
            params->testPassed = TRUE;
         KillTest(params);
    }

    buginf("\n%s-%u: OPENED: PU %s LU %d APPL %s",
        params->testName, params->testPid,
        params->hostName, locaddr,
        applName[params->applNo]
        );

    params->luHandle = luHandle;
}


static void DoLogoff (luApi_param *params, byte *sendBuffer)
{
    if (params->applNo == APPLNAME_ITPECHOS) {
        memcpy( &sendBuffer[0], itpechosReqHeader,
                sizeof(itpechosReqHeader)); 
        ConvertAsciiToEbcdic("logoff",
                &sendBuffer[sizeof(itpechosReqHeader)], 6);
        DoLuSend(params, sizeof(itpechosReqHeader) + 6);
    } else {
        /* send logoff */
        ConvertAsciiToEbcdic("WxYz", &sendBuffer[0], 4);
        sendBuffer[4] = 0x01; /* version */
        sendBuffer[5] = 0x01; /* Logoff Request */

        params->expectedRcvLength = 0;  /* no response expected */
        params->rcvdLength = 0;
        DoLuSend(params, 6);
    }

    while (DoWait(params, DoLuReceive))
         ;
}


static void OpenEchoAppl (luApi_param *params)
{
    DoLuT0orT2Open(params);  /* doesn't return if open fails */
    DoLuReceive(params);   /* kick off receive cycle */

    if (params->applNo == APPLNAME_ITPECHOS) {
        /* expect one frame to set up screen */
        params->expectedRcvLength = 0;  /* don't check answer */
        if (!DoWait(params, DoLuReceive))
            KillTest(params); /* no return */
    }
}


static void EchoLoop (luApi_param *  params,
                      uint startAgentLength,
                      uint startSeqNo,
                      uint startLength,
                      uint stopLength,
                      uint increment)
{
    if (params->applNo == APPLNAME_ITPECHOS)
        ItpEchosLoop( params, startLength, stopLength, increment);
    else
        NspEchoLoop( params, startAgentLength, startSeqNo, startLength,
                     stopLength, increment);
}


static void TestAppl1 (luApi_param *params)
{
    buginf("\n%s-%u: BEGIN TEST",
        params->testName, params->testPid);

    /* open, one echo exchange, logoff to close */
    OpenEchoAppl(params);  /* doesn't return if open fails */
    EchoLoop(params, 6, 1, 8, 8, 1);
    DoLogoff(params, &(params->send[0])); /* returns when done */

    /* open, one echo exchange, close directly */
    OpenEchoAppl(params);  /* doesn't return if open fails */
    EchoLoop(params, 6, 1, 8, 8, 1);
    DoLuClose(params);

    params->testPassed = TRUE;
}


static void TestAppl3 (luApi_param * params)
{
    buginf("\n%s-%u: BEGIN TEST",
           params->testName, params->testPid);

    /* open, one echo exchange, logoff to close */
    DoLuT0orT2Open(params);  /* doesn't return if open fails */
    DoLuReceive(params);   /* kick off receive cycle */
    EchoLoop(params, 0, 1, 1, 2000, 1); 
    DoLogoff(params, &(params->send[0])); /* returns when done */

    params->testPassed = TRUE;
}

 
#define MAXECHOSIZE 50

static void TestAppl4 (luApi_param * params)
{
    BeginTest(params);

    /* open, one echo exchange, logoff to close */
    OpenEchoAppl(params);  /* doesn't return if open fails */
    EchoLoop(params, 0, 1, 1, MAXECHOSIZE, 1); 
    DoLogoff(params, &(params->send[0])); /* returns when done */

    params->testPassed = TRUE;
}

 
static void TestAppl5 (luApi_param * params)
{
    buginf("\n%s-%u: BEGIN TEST",
           params->testName, params->testPid);

    /* open, one echo exchange, logoff to close */
    OpenEchoAppl(params);  /* doesn't return if open fails */
    while (1) {
        EchoLoop(params, 0, 1, 1, MAXECHOSIZE, 1); 
        buginf("\n%s-%u: %d messages exchanged",
               params->testName, params->testPid, MAXECHOSIZE);
    }
    /* never get here ... */
    params->testPassed = FALSE;
}

 
typedef struct {
    byte        TH0;       /* Transmission Header       */
    byte        TH1;
    byte        DAF;       /* Destination Address Field */
    byte        OAF;       /* Origin Address Field      */
    byte        SNF[2];    /* Sequence Number Field     */
    byte        RH0;       /* Request Header            */
    byte        RH1;
    byte        RH2;
    byte        ruData[1]; /* Data array                */
} SNA; 

#define TH_RH_LEN 9
#define swab(a,b) { byte c = b; b = a; a = c; }

static boolean ChangeSscpLuReqToPosRsp (SNA* snaP, uint* length)
{

     /* make sure length is enough */
     if (*length < TH_RH_LEN+1)
         return (FALSE);
     
     /* Make sure it is a first or only segment. */
     if ((snaP->TH0 & TH0_BBIU) == 0)
         return (FALSE);
   
     /* Determine if the MU is already a response, in which case this is 
      * an error, and FALSE is returned.                                 
      */
     if ((snaP->RH0 & RH0B0_RRI) == RH0B0_RSP)
         return (FALSE);

     /* Make sure that the request is for definite response. */
     if ( ((snaP->RH1 & RH1B3_ERI) == RH1B3_ER) ||
          (((snaP->RH1 & (RH1B0_DR1I|RH1B2_DR2I)) == 0)) )
         return (FALSE);
   
     if ( ((snaP->RH0 & RH0B1B2_Category) == RH0B1B2_FMD) &&
          ((snaP->RH0 & RH0B4_FI) == RH0B4_NSH) &&
              ( (snaP->ruData[0] == 0x01) ||
               (snaP->ruData[0] == 0x41) ||
               (snaP->ruData[0] == 0x81) ) ) {
         /* Response Category is FMD */
         if (*length < TH_RH_LEN+3)
             return (FALSE);
         *length = TH_RH_LEN + 3; /* 3 bytes for the request code */
     } else
         *length = TH_RH_LEN + 1; /* 1 byte for the request code */
     
     /* Set the RH and TH */
     snaP->RH0 = (snaP->RH0 | ( RH0B0_RSP /* Response                */
                               + RH0B6_BC /* First in chain          */
                               + RH0B7_EC /* Last in chain           */
                          )) & ~RH0B5_SD; /* Sense Data Not Included */

     snaP->RH1 = snaP->RH1 | RH1B3_P; /* Positive Response       */

     snaP->RH1 &= ~RH1B3_ER; 

     snaP->TH0 = snaP->TH0 | ( TH0_BBIU /* Whole BIU               */
                             + TH0_EBIU
                             );

     snaP->RH2 = 0;                                 
 
     /* Interchange OAF and DAF */  
     swab(snaP->OAF,snaP->DAF);

     return (TRUE);
}                       

static void DoSscpEchoReqRsp (luApi_param * params,
                              boolean       def,
                              int           length)
{
    byte *          sendBuffer;
    uint            rspLength;

    sendBuffer = &(params->send[0]);

    /* TH */
    sendBuffer[0] = 0x2C;
    sendBuffer[1] = 0x00;
    sendBuffer[2] = 0x00;
    sendBuffer[3] = 0x00;
    sendBuffer[4] = 0x00;
    sendBuffer[5] = 0x00;
    /* RH */
    sendBuffer[6] = 0x0B;
    if (def)
        sendBuffer[7] = 0x80; /* definite response requested */
    else
        sendBuffer[7] = 0x90; /* exception response requested */
    sendBuffer[8] = 0x00;

    /* RU */
    sendBuffer[9] = 0x81;
    sendBuffer[10] = 0x03;
    sendBuffer[11] = 0x87;
    sendBuffer[12] = 0x01;
    sendBuffer[13] = length + 2;
    sendBuffer[14] = 0x00;
    sendBuffer[15] = 0x01;

    FillABCD(&(sendBuffer[16]), length);

    params->expectedRcvLength = 0;  /* don't check answer */

    DoSscpLuSend(params, 16 + length);

    /* expect one ECHOTEST request and, if def, one REQECHO response
     * but note that these can come in either order
     */
    if (!DoWait(params, DoSscpLuReceive))
        KillTest(params); /* no return */
    rspLength = params->rcvdLength;
    if (ChangeSscpLuReqToPosRsp((SNA*)(&(params->rcv[0])), &rspLength)) {
        /* need to send response to ECHOTEST request */
        memcpy(sendBuffer, params->rcv, rspLength);
        DoSscpLuSend(params, rspLength);
    }
    if (def) {
        if (!DoWait(params, DoSscpLuReceive))
            KillTest(params); /* no return */
        rspLength = params->rcvdLength;
        if (ChangeSscpLuReqToPosRsp((SNA*)(&(params->rcv[0])), &rspLength)) {
            /* need to send response to ECHOTEST request */
            memcpy(sendBuffer, params->rcv, rspLength);
            DoSscpLuSend(params, rspLength);
        }
    }
}


/*
 * process entry point for SSCP-LU API test
 */
static void TestAppl2 (luApi_param * params)
{
    LuApiReturnCode rc;
    void *          luHandle = NULL;
    byte            locaddr;
    byte *          bfrReturn;

    params->testPassed = TRUE;

    locaddr = params->locaddr;

    buginf("\n%s-%u: BEGIN TEST",
           params->testName, params->testPid);

    rc = sscpLuQueryPu (params->hostName);
    if( rc != Success) {
        buginf("\n%s-%u: LuApiReturnCode for sscpLuQueryPu (%s) is ",
               params->testName, params->testPid, params->hostName);
        printrc(rc);
        buginf(" \n");
    }
  
    rc = sscpLuOpen(
              params,
              &luHandle,
              params->hostName,
              &locaddr,
              FALSE,                /* deny LU session */
              &closeIndication
                  );

    if (*(params->traceP)) {
        buginf("\n%s-%u: LuApiReturnCode for sscpLuOpen is ",
               params->testName, params->testPid);
        printrc(rc);
        buginf(" \n\t luHandle = %u", luHandle);
        buginf(" \n\t hostName = %s", params->hostName);
        buginf(" \n\t locaddr = %u\n", locaddr);
    }
  
    if (rc) {
        buginf("\n%s-%u: terminating - sscpLuOpen returned failure code ",
               params->testName, params->testPid);
        printrc(rc);
        buginf(" \n");
        KillTest(params); /* no return */
    }

    buginf("\n%s-%u: OPENED: PU %s LU %d",
           params->testName, params->testPid,
           params->hostName, locaddr );

    params->luHandle = luHandle;

    DoSscpLuReceive(params);

    DoSscpEchoReqRsp(params, TRUE, 6);
    DoSscpEchoReqRsp(params, TRUE, 4);
    DoSscpEchoReqRsp(params, FALSE, 6);
    DoSscpEchoReqRsp(params, FALSE, 0);
    DoSscpEchoReqRsp(params, FALSE, 252);
    DoSscpEchoReqRsp(params, FALSE, 253);
  
    /* close api */
    rc = sscpLuClose( params->luHandle, &bfrReturn);
    if (*(params->traceP)) {
        buginf("\n%s-%u: LuApiReturnCode for sscpLuClose is ",
               params->testName, params->testPid);
        printrc(rc);
        buginf(" \n\t luHandle = %u", params->luHandle);
        buginf(" \n\t bfrReturn = %u\n", bfrReturn);
    }
  
}


static void luApi_param_reinit (luApi_param*   param)
{
    (param)->testPassed = FALSE;
    (param)->wakeupByRcv = TRUE;
}


static void luApi_param_init (luApi_param**   param,
                              boolean*        traceP,
                              const char*     hostName,
                              char*           modeName,
                              byte            locaddr,
                              byte            applNo,
                              LuApiReturnCode expectedOpenCode
                             )
{
    *param = malloc(sizeof(luApi_param));
    if (*param) {
        process_get_name(THIS_PROCESS, &((*param)->testName));
        process_get_pid(&((*param)->testPid));
        memcpy(&((*param)->hostName[0]), hostName, strlen(hostName));
        (*param)->hostName[strlen(hostName)] = 0;
        ConvertAsciiToEbcdic(applName[applNo],
                             (*param)->applName,
                             strlen(applName[applNo]));
        (*param)->applNameLen = strlen(applName[applNo]);
        ConvertAsciiToEbcdic(modeName, (*param)->modeName, strlen(modeName));
        (*param)->locaddr = locaddr;
        (*param)->applNo = applNo;
        (*param)->expectedOpenCode = expectedOpenCode;

        (*param)->traceP = traceP;

        (*param)->testPassed = FALSE;
        (*param)->wakeupByRcv = TRUE;

        (*param)->wb = 0;
        CreateAndWatchBoolean(&((*param)->wb));
        if ((*param)->wb == 0) {
            free(*param);
            *param = 0;
        }

    } else {
        const char *testName;
        pid_t testPid;
        process_get_name(THIS_PROCESS, &(testName));
        process_get_pid(&(testPid));
        buginf("\n%s-%u: allocation of param buffer failed",
               testName, testPid);
        process_kill(THIS_PROCESS);
    }
}


#define LUAPI_TESTMAX 8
#define LUAPI_SUBTESTMAX 4
static boolean traceTest[LUAPI_TESTMAX][LUAPI_SUBTESTMAX] = {
    {FALSE, FALSE, FALSE, FALSE},
    {FALSE, FALSE, FALSE, FALSE},
    {FALSE, FALSE, FALSE, FALSE},
    {FALSE, FALSE, FALSE, FALSE},
    {FALSE, FALSE, FALSE, FALSE},
    {FALSE, FALSE, FALSE, FALSE},
    {FALSE, FALSE, FALSE, FALSE},
    {FALSE, FALSE, FALSE, FALSE}
};

static void RunTestAppl1 (boolean*        traceP,
                          const char*     hostName,
                          byte            locaddr,
                          byte            applNo)
{
    luApi_param *param;
    luApi_param_init(&param, traceP,
        hostName, "        ", locaddr, applNo, Success);
    TestAppl1(param);
    KillTest(param);
}

#define DUMMYNAME " "
static void RunTestAppl2 (boolean*        traceP,
                          const char*     hostName,
                          byte            locaddr,
                          byte            applNo)
{
    luApi_param *param;
    luApi_param_init(&param, traceP,
        hostName, DUMMYNAME, locaddr, applNo, Success);
    TestAppl2(param);
    KillTest(param);
}


static void RunTestAppl3 (boolean*        traceP,
                          const char*     hostName,
                          byte            locaddr,
                          byte            applNo)
{
    luApi_param *param;
    luApi_param_init(&param, traceP,
        hostName, "        ", locaddr, applNo, Success);
    TestAppl3(param);
    KillTest(param);
}

static void RunTestAppl4 (boolean*        traceP,
                          const char*     hostName,
                          byte            locaddr,
                          byte            applNo)
{
    luApi_param *param;
    luApi_param_init(&param, traceP,
        hostName, "        ", locaddr, applNo, Success);
    TestAppl4(param);
    KillTest(param);
}


static void RunTestAppl4InvH (boolean*        traceP,
                              const char*     hostName,
                              byte            locaddr,
                              byte            applNo)
/* a test expecting an invalid host name for error code test   */
{
    luApi_param *param;
    luApi_param_init(&param, traceP,
        hostName, "        ", locaddr, applNo, InvalidHandle);
    TestAppl4(param);
    KillTest(param);
}

static void RunTestAppl5 (boolean*        traceP,
                          const char*     hostName,
                          byte            locaddr,
                          byte            applNo)
{
    luApi_param *param;
    luApi_param_init(&param, traceP,
        hostName, "        ", locaddr, applNo, Success);
    TestAppl5(param);
    KillTest(param);
}


static void LoopTestAppl1 (boolean*        traceP,
                           const char*     hostName,
                           byte            locaddr,
                           byte            applNo)
{
    luApi_param *param;
    luApi_param_init(&param, traceP,
        hostName, "        ", locaddr, applNo, Success);
    do {
        luApi_param_reinit(param);
        TestAppl1(param);
        EndTest(param);
    } while (param->testPassed);
    KillTest(param);
}


static void LoopTestAppl2 (boolean*        traceP,
                           const char*     hostName,
                           byte            locaddr,
                           byte            applNo)
{
    luApi_param *param;
    luApi_param_init(&param, traceP,
        hostName, "        ", locaddr, applNo, Success);
    do {
        luApi_param_reinit(param);
        TestAppl2(param);
        EndTest(param);
    } while (param && param->testPassed);
    KillTest(param);
}

static void FlowOn (boolean*        traceP,
                    const char*     hostName,
                    byte            locaddr,
                    byte            applNo)
{
    buginf("\nFlow ON\n");
    UpStreamFlowCtrl(0);
    process_kill(THIS_PROCESS);
}

static void FlowOff (boolean*        traceP,
                     const char*     hostName,
                     byte            locaddr,
                     byte            applNo)
{
    buginf("\nFlow OFF\n");
    UpStreamFlowCtrl(1);
    process_kill(THIS_PROCESS);
}


typedef void (*testFunction) (boolean*, const char*, byte, byte);

static const testFunction procTest[LUAPI_TESTMAX][LUAPI_SUBTESTMAX] = {
    {RunTestAppl1, RunTestAppl1, 0, 0},
    {RunTestAppl2, 0, FlowOff, FlowOn},
    {RunTestAppl1, RunTestAppl2, RunTestAppl2, RunTestAppl1},
    {RunTestAppl3, 0, 0, 0},
    {RunTestAppl4, RunTestAppl4, 0, 0},
    {RunTestAppl4, RunTestAppl4InvH, 0,0},
    {RunTestAppl4, 0, 0, 0},
    {RunTestAppl5, LoopTestAppl1, LoopTestAppl2, 0}
};

/* we need to keep static copies of the test names around
 * because they are accessed by the process we create to
 * write buginf messages and for the process name
 */
static char * const testName[LUAPI_TESTMAX][LUAPI_SUBTESTMAX] = {
    {"Test001A", "Test001B", "Test001C", "Test001D"},
    {"Test002A", "Test002B", "Test002C", "Test002D"},
    {"Test003A", "Test003B", "Test003C", "Test003D"},
    {"Test004A", "Test004B", "Test004C", "Test004D"},
    {"Test005A", "Test005B", "Test005C", "Test005D"},
    {"Test006A", "Test006B", "Test006C", "Test006D"},
    {"Test007A", "Test007B", "Test007C", "Test007D"},
    {"Test008A", "Test008B", "Test008C", "Test008D"},
};


typedef struct luApi_arg_
{
    int   testIndex;
    int   subTestIndex;
    char  hostName[18];
    byte  locaddr;
    byte  applNo;
} luApi_arg;


static void TestProcessMain (void)
{
    void * argGetPtr;
    luApi_arg * arg;
   
    if (!process_get_arg_ptr(&argGetPtr)) {
        buginf("\ntestluapi: terminating - Failed to get arg ptr\n");
        process_kill(THIS_PROCESS);
    }
    arg = (luApi_arg *) argGetPtr;
    procTest[arg->testIndex][arg->subTestIndex](
        &(traceTest[arg->testIndex][arg->subTestIndex]),
        arg->hostName,
        arg->locaddr,
        arg->applNo
        );
}

static void StartTestProcess (int         testIndex,
                              int         subTestIndex,
                              const char* hostName,
                              byte        locaddr,
                              byte        applNo
                             )
{
    static luApi_arg arg;
    pid_t testLuapiPid;

    arg.testIndex = testIndex;
    arg.subTestIndex = subTestIndex;
    memcpy(&(arg.hostName[0]), hostName, strlen(hostName));
    arg.hostName[strlen(hostName)] = 0;
    arg.locaddr = locaddr;
    arg.applNo = applNo;
    testLuapiPid = process_create(TestProcessMain,
                          testName[testIndex][subTestIndex],
                          LARGE_STACK,
                          PRIO_NORMAL);
    if (testLuapiPid != NO_PROCESS)
        process_set_arg_ptr(testLuapiPid, &arg); 
}


/* 
 *   CODE FOR PARSING TEST COMMANDS.
 *
 */

#define DEBUG_LUAPITEST_EXECUTE 1
#define DEBUG_LUAPITEST_TRACE   2

void luapitest_debug_command (parseinfo *csb)
{
    int testNo;
    int subNo;
    const char* hostName;
    byte locaddr;
    int applNo;

    testNo = GETOBJ(int, 1);
    subNo = GETOBJ(int, 2);
    hostName = GETOBJ(string, 1);
    
    switch (csb->which) {
    case DEBUG_LUAPITEST_EXECUTE:
        locaddr = GETOBJ(int, 3);
        applNo = GETOBJ(int, 4);
        if (procTest[testNo-1][subNo-1])
            StartTestProcess(testNo-1, subNo-1, hostName, locaddr, applNo);
        break;

    case DEBUG_LUAPITEST_TRACE:
        if (testNo)
            traceTest[testNo-1][subNo-1] = csb->sense;
        else {
            for (testNo = 0; testNo < LUAPI_TESTMAX; testNo++)
                for (subNo = 0; subNo < LUAPI_SUBTESTMAX; subNo++)
                    traceTest[testNo][subNo] = csb->sense;
        }
        break;

    default:             /* If not a defined "debug luapitest" command */
        debug_command(csb);   /*   use generic debug handler           */
        break;
    }
}


/*
 * Parse chains for debug commands
 */
/*
 * debug luapitest execute <test_no> <subtest_no>
 *            <pu_name> [<locaddr>] [<hostappl>]
 *
 */
EOLS	(debug_luapitest_exec_eol, luapitest_debug_command,
         DEBUG_LUAPITEST_EXECUTE);
NUMBER	(debug_luapitest_exec_hostappl, debug_luapitest_exec_eol,
         no_alt, OBJ(int, 4), 0, 1, "Host Application No");
NUMBER	(debug_luapitest_exec_locaddr, debug_luapitest_exec_hostappl,
         no_alt, OBJ(int, 3), 0, 255, "LU address");
NOP	(debug_luapitest_exec_nop, debug_luapitest_exec_locaddr,
         debug_luapitest_exec_eol);
SET	(debug_luapitest_exec_hostappl_set, debug_luapitest_exec_nop,
         OBJ(int,4), 0);
SET	(debug_luapitest_exec_locaddr_set, debug_luapitest_exec_hostappl_set,
         OBJ(int,3), 0);
STRING	(debug_luapitest_exec_puname, debug_luapitest_exec_locaddr_set,
         no_alt, OBJ(string, 1), "Host/Pu name");
NUMBER	(debug_luapitest_exec_subtestno, debug_luapitest_exec_puname,
         no_alt, OBJ(int, 2), 1, LUAPI_SUBTESTMAX, "subtest number");
NUMBER	(debug_luapitest_exec_testno, debug_luapitest_exec_subtestno,
         no_alt, OBJ(int, 1), 1, LUAPI_TESTMAX, "test number");
KEYWORD	(debug_luapitest_execute, debug_luapitest_exec_testno,
         no_alt, "execute", "LUAPI execute test", PRIV_OPR);


/*
 * debug luapitest trace [<test_no> <subtest_no>]
 *
 */
EOLS	(debug_luapitest_trace_eol, luapitest_debug_command,
         DEBUG_LUAPITEST_TRACE);
NUMBER	(debug_luapitest_trace_subtestno, debug_luapitest_trace_eol,
         no_alt, OBJ(int, 2), 1, LUAPI_SUBTESTMAX, "subtest number");
NUMBER	(debug_luapitest_trace_testno, debug_luapitest_trace_subtestno,
         no_alt, OBJ(int, 1), 1, LUAPI_TESTMAX, "test number");
NOP	(debug_luapitest_trace_nop, debug_luapitest_trace_testno,
         debug_luapitest_trace_eol);
SET	(debug_luapitest_trace_subtestno_set, debug_luapitest_trace_nop,
         OBJ(int,2), 0);
SET	(debug_luapitest_trace_testno_set, debug_luapitest_trace_subtestno_set,
         OBJ(int,1), 0);
KEYWORD (debug_luapitest_trace, debug_luapitest_trace_testno_set,
         debug_luapitest_execute, "trace", "LUAPI TEST trace", PRIV_OPR);


KEYWORD (debug_luapitest, debug_luapitest_trace,
         NONE, "luapitest", "LUAPI TEST ", PRIV_OPR);

LINK_POINT(luapitest_debug_command, debug_luapitest);


static parser_extension_request luapitest_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(luapitest_debug_command) },
    { PARSE_LIST_END, NULL }
};

static void luapitest_subsys_init (subsystype *subsys)
{
    parser_add_command_list(luapitest_chain_init_table, "luapitest");
}


/*
 * LUAPITEST subsystem header
 */

#define LUAPITEST_MAJVERSION 1
#define LUAPITEST_MINVERSION 0
#define LUAPITEST_EDITVERSION  1

SUBSYS_HEADER(luapitest,
              LUAPITEST_MAJVERSION, LUAPITEST_MINVERSION,
              LUAPITEST_EDITVERSION,
	      luapitest_subsys_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: sna_pu",
	      "req: sna_pu");
