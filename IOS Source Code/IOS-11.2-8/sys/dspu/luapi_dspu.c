/* $Id: luapi_dspu.c,v 3.1.4.4 1996/08/29 02:43:34 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/luapi_dspu.c,v $
 *------------------------------------------------------------------
 * LU API DSPU control block structures and methods.
 *
 * Sept 1995, Lesley Cox
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the finite state machines, structures, and
 * procedures for the LU API DSPU control block.
 *------------------------------------------------------------------
 * $Log: luapi_dspu.c,v $
 * Revision 3.1.4.4  1996/08/29  02:43:34  lcox
 * CSCdi67526:  remove old edism logic from luapi and style cleanup
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/07/29  21:47:02  snyder
 * CSCdi64412:  declare some arrays const that should be sub_sna_pu
 *              moves 2376 out of data, saves 116 as well
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/06/19  12:31:30  lcox
 * CSCdi60760:  luapi rcvContinue fails for segmented frames
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:48:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.3  1996/04/24  00:50:15  lcox
 * Branch: IbuMod_Calif_branch
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.1.2.2  1996/04/09  22:36:39  lmetzger
 * Update of Initial Version of Response Time Reporter
 *
 * Revision 3.1.2.1  1996/03/22  18:50:45  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:34  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "luapi.h"
#include "luapi_handle.h"
#include "luapi_dspu.h"
#include "snaprim.h"
#include "snapiu.h"
#include "dspu_fsmstr.h"
#include "dspu_lu.h"
#include "dspu_pu.h"
#include "dspu_puutil.h"
#include "dspu_error.h"
#include "snautil.h"



/*
 * ApiState
 *   API state enumeration.
 */
typedef enum ApiState {
    ApiOpening,  /* API open in progress                             */
    ApiOpen,     /* open completed, send/receive open to application */
    ApiClosing   /* closing due to Close call from application       */
} ApiState;


/*
 * SSCP-LU enumerations.
 * ---------------------
 */

/*
 * SscpLuInput
 *   SSCP-LU FSM input enumeration.
 */
typedef enum SscpLuInput {
    StartDynamicLu,  /* start LU: was inactive, DDDLU to request act*/
    StartStaticLu,   /* start LU: already active, but not available */
    uRplyPsdRspP,    /* +ve response to NMVT Reply PSID received    */
    uRplyPsdRspN,    /* -ve response to NMVT Reply PSID received    */
    uActlu,          /* ACTLU reqeust received                      */
    uDactlu,         /* DACTLU request received                     */
    Available,       /* signal when desiredState==ActAv             */
    NotAvailable,    /* signal when desiredState==ActNotAv          */
    uNtfyRspP,       /* +ve response to Notify received             */
    uNtfyRspN,       /* -ve response to Notify received             */
    uInitselfRspP,   /* +ve response to Initself received           */
    uInitselfRspN,   /* -ve response to Initself received           */
    uNspe,           /* NSPE request received                       */
    uOtherSscpLuReq, /* other SSCP-LU sna request received          */
    uOtherSscpLuRsp, /* other SSCP-LU sna response received         */
    uLuLu,           /* LU-LU session traffic received              */
    StopLu,          /* Signal that PU is inactive                  */
    CloseLuLu,       /* Signal that LU-LU API sesion is closing     */
    ApiSendDefReq,   /* a definite response request from sscpLuSend */
    ApiSendOther,    /* other request or response from sscpLuSend   */
    ApiReceive,      /* sscpLuReceive call processing               */
    ApiClose,        /* sscpLuClose call processing                 */
    sscpLuMaxInput
} SscpLuInput;
  
/*
 * LuApiSscpLuState
 *   SSCP-LU FSM state enumeration.
 */
typedef enum LuApiSscpLuState {
    LuApi_Reset,     /* initial transient state                         */
    PendActlu,       /* waiting for ACTLU or processing an ACTLU        */
    PendNotifyNARsp, /* waiting for rsp to Notify (Not Available)       */
    ActNotAv,        /* SSCP-LU session active, SSCP-LU API in use      */
    PendNotifyARsp,  /* waiting for rsp to Notify (Available)           */
    ActAv,           /* LU-LU API in use, SSCP-LU API may be in use     */
    DefRspReqdNotAv, /* ActNotAv but waiting for definite rsp from host */
    DefRspReqdAv,    /* ActAv but waiting for definite rsp from host    */
    sscpLuMaxState
} LuApiSscpLuState;
                  
/*
 * sscpLuFsmStates
 *   SSCP-LU FSM state name strings for "show" and trace output.
 */
static const char* const sscpLuFsmStates[] = {
    "Reset",
    "PendActlu",
    "PendNotifyNARsp",
    "ActNotAv",
    "PendNotifyARsp",
    "ActAv",
    "DefRspReqdNotAv",
    "DefRspReqdAv"
};


/*
 * sscpLuFsmInputs
 *   SSCP-LU FSM input name strings for trace output.
 */
static const char* const sscpLuFsmInputs[] = {
    "StartDynamicLu",  /* start LU: was inactive, DDDLU to request act*/
    "StartStaticLu",   /* start LU: already active, but not available */
    "uRplyPsdRspP",    /* +ve response to NMVT Reply PSID received    */
    "uRplyPsdRspN",    /* -ve response to NMVT Reply PSID received    */
    "uActlu",          /* ACTLU reqeust received                      */
    "uDactlu",         /* DACTLU request received                     */
    "Available",       /* signal when desiredState==ActAv             */
    "NotAvailable",    /* signal when desiredState==ActNotAv          */
    "uNtfyRspP",       /* +ve response to Notify received             */
    "uNtfyRspN",       /* -ve response to Notify received             */
    "uInitselfRspP",   /* +ve response to Initself received           */
    "uInitselfRspN",   /* -ve response to Initself received           */
    "uNspe",           /* NSPE request received                       */
    "uOtherSscpLuReq", /* other SSCP-LU sna request received          */
    "uOtherSscpLuRsp", /* other SSCP-LU sna response received         */
    "uLuLu",           /* LU-LU session traffic received              */
    "StopLu",          /* Signal that PU is inactive                  */
    "CloseLuLu",       /* Signal that LU-LU API sesion is closing     */
    "ApiSendDefReq",   /* a definite response request from sscpLuSend */
    "ApiSendOther",    /* other request or response from sscpLuSend   */
    "ApiReceive",      /* sscpLuReceive call processing               */
    "ApiClose",        /* sscpLuClose call processing                 */
};



/*
 * LU-LU FSM enumerations.
 * -----------------------
 */

/*
 * LuLuInput
 *   LU-LU FSM input enumeration.
 */
typedef enum LuLuInput {
   Bind,            /* BIND request received from upstream           */
   Unbind,          /* UNBIND request received from upstream         */
   Sdt,             /* SDT request received from upstream            */
   Clear,           /* CLEAR request received from upstream          */
   FmdReq,          /* FMD request received from upstream            */
   OtherReq,        /* other type of request received from upstream  */
   FmdNegRsp,       /* negative response to FMD request received     */
   OtherRsp,        /* other type of response received from upstream */
   PacingRsp,       /* pacing response from upstream                 */
   SendBindPosRspNoSdt, /* send +rsp(BIND), no sdt in profile        */
   SendBindPosRspSdt,   /* send +rsp(BIND), sdt in profile           */
   luLuApiSend,     /* a request or response from sscpLuSend         */
   luLuApiReceive,  /* sscpLuReceive call processing                 */
   luLuApiClose,    /* sscpLuClose call processing                   */
   luLuMaxInput
} LuLuInput;
  
/*
 * LuLuState
 *   LU-LU FSM state enumeration.
 */
typedef enum LuLuState {
    LuLuReset,          /* initial state             */
    Bound,              /* bound but waiting for SDT */
    FlowOpen,           /* bound and SDT received    */
    luLuMaxState
} LuLuState;


/*
 * luLuFsmStates
 *   LU-LU FSM state name strings for "show" output.
 */
static const char* const luLuFsmStates[] = {
    "LuLuReset",
    "Bound",
    "FlowOpen"
};


/*
 * luLuFsmInputs
 *   LU-LU FSM input name strings for "show" output.
 */
static const char* const luLuFsmInputs[] = {
    "Bind",
    "Unbind",
    "Sdt",
    "Clear",
    "FmdReq",
    "OtherReq",
    "FmdNegRsp",
    "OtherRsp",
    "PacingRsp",
    "SendBindPosRspNoSdt",
    "SendBindPosRspSdt",
    "luLuApiSend",
    "luLuApiReceive",
    "luLuApiClose",
    "luLuMaxInput"
};
  

/*
 * ReqRspInd
 *   Indicator to identify SNA requests and responses.
 */
typedef enum ReqRspInd {
    ReceiveRequest,
    ReceiveResponse
} ReqRspInd;



/***********************************************************************
* LU API STRUCTURE                                                      
************************************************************************
*
* The following structures form the LU API CB.
*   
**/


/*
 * LuApiCommon: fields that the LU-LU and SSCP-LU APIs both need.
 */
typedef struct LuApiCommon {
    ApiState           apiState;  /* opening, open or closing   */
    void*              applH;     /* application LU Handle      */
    TCloseIndication   closeInd;  /* close indication entry pt  */

    LuApiReturnCode    openResult;  /* retcode from fsm         */
    watched_boolean *  openComplete;

    TReceiveIndication rcvInd;   /* receive indication entry pt */
    byte*              rcvBuf;   /* application rcv buffer      */
    uint               rcvBufLen; /* rcvBuf length              */

    LuApiReturnCode    sendResult;     /* retcode from fsm      */
    watched_boolean *  closeComplete;
} LuApiCommon;


/*
 * SscpLuApi: SSCP-LU API specific data
*/
typedef struct SscpLuApi {
    LuApiCommon        c;

    boolean            denyLuSession;  /* sscp-lu api exclusive use flag */

    /* sscpLuReceive and sscpLuReceive Indication specific stuff */
    boolean            reqFirst; /* give req to appl before rsp */
    DataReq_t*         qdReqP;   /* req rcvd from host          */
    DataReq_t*         qdRspP;   /* rsp rcvd from host          */

    /* sscpLuSend data */
    DataReq_t*         sendQdReqP; /* to hold pending ntfy rsp */
} SscpLuApi;


/*
 * LuLuApiOpenParm: appl open data -valid during luOpen only.
 */
typedef struct LuLuApiOpenParm {
    uint             applNameLen;
    byte*            applName;
    byte*            modeName;
    uint             userDataLen;
    byte*            userData;
    hword*           sysSense;
    hword*           userSense;
    uint             bindDataLen;
    byte*            bindData;
    uint*            bindDataCnt;
} LuLuApiOpenParm;


/*
 * LuLuApi: LU-LU API specific data
 */
typedef struct LuLuApi {
    LuApiCommon        c;

    /* handle for method vectors */
    LuApiHandle *      luHandleP;

    /* data in openParmP is valid during luOpen only */
    LuLuApiOpenParm*   openParmP;

    /* lu type - determines behaviour at certain points */
    byte               luType;

    /* save area for Show command data */
    char               hostApplName[9]; /* host application name     */

    /* useful data from BIND */
    byte               plu;           /* primary LU for DAF field    */
    byte               tsProfile;     /* to remember about SDT etc   */
    hword              snf;           /* seq no for LU-LU requests   */
    byte               rh1;           /* for building LU-LU requests */
    byte               rh2;           /* for building LU-LU requests */
    uint               sendPacingWindow; /* for pacing of req to host*/

    /* luSend data */
    uint               pacingCount;   /* frames sent since last p rsp*/

    /* luReceive and luReceive Indication data */
    DataReq_t*         qdBfrP;          /* bfr rcvd from host        */
    ReqRspInd          qdRri;           /* type of qdBfrP            */
    boolean            lastFrameWasEC;  /* remember chaining state   */
} LuLuApi;


/* LuApiWorkBits
 *   used as a way to record which message needs to be generated
 *   when a buffer is available later
 */
enum LuApiWorkBits {
    LUAPIWORK_ACTLURSP      =  0x01,
    LUAPIWORK_INITSELF      =  0x02,
    LUAPIWORK_NOTIFYAVAIL   =  0x04,
    LUAPIWORK_NOTIFYUNAVAIL =  0x08
};


/*
 * LuApi: the main structure holding api data per LU
 */
struct LuApi {
    byte          sscpLuState;  /* sscp-lu fsm state                 */
    byte          desiredState; /* desired sscp-lu fsm state         */
    byte          luLuState;    /* lu-lu fsm state                   */
    SscpLuApi*    sscpLuP;      /* SSCP-LU API stuff 0=>closed       */
    LuLuApi*      luLuP;        /* LU-LU API stuff   0=>closed       */
    byte          workBits;     /* flags to remember work to do      */
    byte          workProgress; /* remember work in progress to clean*/
    DataReq_t*    actluReqP;    /* place holder for actlu pending rsp*/
    hword         lastReqType;  /* for received segmented frames     */
};


/*
 * LUAPI CONSTRUCTORS                                                     
 * ------------------
 *
 * The following routines are the LUAPI constructors.
 *   
 */


/*
 * LuApiHandle_nw
 *
 *
 * Description:
 *
 *   This function creates the LUAPI HANDLE control block.
 *   On failure, a null pointer is returned.
 *   Note: There is no corresponding LuApiHandle_dl because this
 *   structure is never deleted - it is kept around so that illegal
 *   calls to the API post "closing" are detected.
 *   
 */
static LuApiHandle* LuApiHandle_nw (const LuApiMethod * methodP)
{
    LuApiHandle* luHandleP = DspuMalloc(sizeof(LuApiHandle));
    if (luHandleP) {
        luHandleP->method = methodP;
    }
    return (luHandleP);
}


/*
 * LuApi_nw
 *
 * This function creates the LUAPI control block and connects it to
 * an LU control block.
 * On failure, a null pointer is returned.
 *   
 */
static Lu* LuApi_nw (Pu*     puP,       /* upstream PU pointer */
                     hword   luNumber   /* lu number           */
                    )
{
    Lu* luP = 0;
    LuApi* luApiP = DspuMalloc(sizeof(LuApi));
    if (luApiP) {

        luP = Lu_nw(puP, luNumber, 0); /* Construct lu and connect to 
                                           upstream pu */

        if (luP) {
            luP->usage = luInUseByLuApi;
            luP->luApiP = luApiP;

            luApiP->sscpLuState = LuApi_Reset;
            luApiP->desiredState = ActNotAv;
            luApiP->luLuState = LuLuReset;
            luApiP->sscpLuP = 0;
            luApiP->luLuP = 0;
            luApiP->actluReqP = 0;
            luApiP->workBits = 0;
        } else {
            DspuFree(luApiP);
            luApiP = 0;
        }
    }
    return (luP);
}


/*
 * LuApiCommon_init
 *
 * This function initialises a LuApiCommon CB.
 *   
 */
static void LuApiCommon_init (LuApiCommon*       luCP,
                              void*              applH,
                              TCloseIndication   closeInd
                             )
{
    luCP->apiState = ApiOpening;
    luCP->applH = applH;
    luCP->closeInd = closeInd;

    luCP->openResult = Success;

    luCP->openComplete = NULL;
    luCP->closeComplete = NULL;

    luCP->rcvInd = 0;
    luCP->rcvBuf = 0;
    luCP->rcvBufLen = 0;

    luCP->sendResult = Success;
}


/*
 * SscpLuApi_nw
 *
 * This function creates a new SSCP-LU API control block and connects it
 * to a given LUAPI LU.
 * On failure, a null pointer is returned.
 *   
 */
static SscpLuApi* SscpLuApi_nw  (Lu* luP,
                                 void*              applH,
                                 TCloseIndication   closeInd
                                )
{
    SscpLuApi* this = DspuMalloc(sizeof(SscpLuApi));
    if (this) {
        LuApiCommon_init( &(this->c), applH, closeInd);
        this->denyLuSession = FALSE;
        this->reqFirst = FALSE;
        this->qdReqP = 0;
        this->qdRspP = 0;
        this->sendQdReqP = 0;
        luP->luApiP->sscpLuP = this;
    }
    return (this);
}


/*
 * LuLuApiOpenParm_nw
 *
 * This function creates a new LU-LU API OPEN PARM control block.
 * On failure, a null pointer is returned.
 *   
 */
static LuLuApiOpenParm* LuLuApiOpenParm_nw (void)
{
    LuLuApiOpenParm* this = DspuMalloc( sizeof(LuLuApiOpenParm));
    return (this);
}


/*
 * LuLuApi_nw
 *
 * This function creates a new LU-LU API control block and connects it
 * to a given LUAPI LU.
 * On failure, a null pointer is returned.
 *   
 */
static LuLuApi* LuLuApi_nw (Lu*                luP,
                            void*              applH,
                            TCloseIndication   closeInd,
                            LuApiHandle *      luHandleP
                           )
{
    LuLuApi* this = DspuMalloc(sizeof(LuLuApi));
    if (this) {

        this->openParmP = LuLuApiOpenParm_nw();

        if (this->openParmP) {
            LuApiCommon_init( &(this->c), applH, closeInd);
            this->luHandleP = luHandleP;
            luHandleP->specificHandle = luP;
            this->luType = 0;
            this->qdBfrP = 0;
            this->qdRri = ReceiveRequest;
            this->lastFrameWasEC = TRUE;
            this->snf = 1;
            luP->luApiP->luLuP = this;
        } else {
            DspuFree(this);
            this = 0;
        }  
    }
    return (this);
}


/*
 * LuLuApi_dl
 *   Detach an LU-LU API CB from an LUAPI control block and free it.
 */
static void LuLuApi_dl (Lu* luP)
{
    LuLuApi* luLuP = luP->luApiP->luLuP;
    luP->luApiP->luLuP = NULL;
    if (luLuP) {
        if (luLuP->luHandleP)
            luLuP->luHandleP->specificHandle = 0;
        if (luLuP->openParmP)
            DspuFree(luLuP->openParmP);
        if (luLuP->qdBfrP)
            DspuCLSIMsgDestroy( luLuP->qdBfrP);
        DspuFree(luLuP);
    }
}


/*
 * SscpLuApi_dl
 *   Detach an SSCP-LU API CB from an LUAPI control block and free it.
 */
static void SscpLuApi_dl (Lu* luP)
{
    SscpLuApi* sscpLuP = luP->luApiP->sscpLuP;
    luP->luApiP->sscpLuP = NULL;
    if (sscpLuP) {
        if (sscpLuP->qdReqP)
            DspuCLSIMsgDestroy( sscpLuP->qdReqP);
        if (sscpLuP->qdRspP)
            DspuCLSIMsgDestroy( sscpLuP->qdRspP);
        if (sscpLuP->sendQdReqP)
            DspuCLSIMsgDestroy( sscpLuP->sendQdReqP);
        DspuFree(sscpLuP);
    }
}


/*
 * LuApi_dl
 *  Detach LU from PU and delete control blocks 
 *  associated with this LUAPI LU.
 */
static void LuApi_dl (Lu** luP)
{
    if ((*luP)->luApiP)
        DspuFree((*luP)->luApiP);
    LuDelete(*luP);
    luP = 0;
}


/*
 * SSCPLU AND LULU FSM UTILITIES
 * -----------------------------
 *   Functions called by the SSCP-LU and LU-LU FSM actions.
 */

/* forward declaration of fsm function */
static boolean SscpLuFsm (Lu         * this, 
                          SscpLuInput  fsmInput, 
                          DataReq_t  * clsiBfrP);

/*
 * SendPosRsp
 *    Change a request into a positive response and send it to the PU.
 *    If the Change fails then it is because the request did not
 *    request a definite response, so then we free the buffer.
 */
static void SendPosRsp (Lu*        this,
                        DataReq_t* clsiBfrP
                       )
{
    if ( ChangeMuToPosRsp(clsiBfrP) )
        SendPu(this->upPuP, clsiBfrP);
    else
        DspuCLSIMsgDestroy(clsiBfrP);
}

/*
 * SendPosRspWithPacing
 *   Change a request into a positive response and send it to the PU.
 *   If the Change fails then it is because the request did not
 *   request a definite response, so then we change it into an IPR
 *   if we needed to send a pacing response.  If that Change fails
 *   then it is because we did not need to send a pacing response,
 *   so then we free the buffer.
 */
static void SendPosRspWithPacing (Lu*        this,
                                  DataReq_t* clsiBfrP
                                 )
{
    if ( ChangeMuToPosRsp(clsiBfrP) )
        SendPu( this->upPuP, clsiBfrP);
    else if ( ChangeMuToIPR( clsiBfrP) )
        SendPu( this->upPuP, clsiBfrP);
    else
        DspuCLSIMsgDestroy(clsiBfrP);
}

/*
 * SendNegRsp
 *   Change a request into a negative response and send it to the PU.
 *   If the Change fails then it is because the request was sent
 *   "no response", so then the buffer is freed.
 */
static void SendNegRsp (Lu*        this,
                        DataReq_t* clsiBfrP,
                        hword      systemSense,
                        hword      userSense
                       )
{
    if ( ChangeMuToNegRsp(clsiBfrP, systemSense, userSense) ) {
        SendPu(this->upPuP, clsiBfrP);
        IncrPuNegativeRspSents(this->upPuP);
    } else {
        DspuCLSIMsgDestroy(clsiBfrP);
    }
}


/*
 * SendNegRspWithPacing
 *   Change a request into a negative response and send it to the PU.
 *   If the Change fails then it is because the request was sent
 *   "no response", so then we change it into an Isolated Pacing Rsp
 *   if we needed to send a pacing response.  If that Change fails
 *   then it is because we did not need to send a pacing response,
 *   so then the buffer is freed.
 */
static void SendNegRspWithPacing (Lu*        this,
                                  DataReq_t* clsiBfrP,
                                  hword      systemSense,
                                  hword      userSense
                                 )
{
    if ( ChangeMuToNegRsp(clsiBfrP, systemSense, userSense) ) {
        SendPu(this->upPuP, clsiBfrP);
        IncrPuNegativeRspSents(this->upPuP);

    } else if ( ChangeMuToIPR( clsiBfrP) )
        SendPu( this->upPuP, clsiBfrP);

    else
        DspuCLSIMsgDestroy(clsiBfrP);
}


/*
 * SendActluPosRsp
 *   Change an ACTLU request into a positive response and send it to the PU.
 *   If the Change fails then it is because we couldn't get a buffer just
 *   now, so then we return FALSE to allow us to retry later.
 */
static boolean SendActluPosRsp (Lu*        this,
                                DataReq_t* clsiBfrP,
                                SNANotifyType notifyType)
{
    DataReq_t* actluRspP = ChangeActluToPosRsp(clsiBfrP, notifyType);
    if (actluRspP == 0)
        return (FALSE);

    /* cleanup original buffer if copy was made */
    if (actluRspP != clsiBfrP)
        DspuCLSIMsgDestroy(clsiBfrP);
    SendPu(this->upPuP, actluRspP);
    return (TRUE);
}


/*
 * SendInitself
 *    Build and send an INITSELF request.
 */
static boolean SendInitself (Lu* this)
{

    LuLuApiOpenParm* openParmP = this->luApiP->luLuP->openParmP;
    DataReq_t*  initselfP;
   
    initselfP = BuildInitself ( this->upLuNumber,    /* OAF */
                                1,                   /* SNF */
                                openParmP->modeName,
                                openParmP->applNameLen,
                                openParmP->applName,
                                openParmP->userDataLen,
                                openParmP->userData);
    if (initselfP == 0)
        return (FALSE);

    SendPu( this->upPuP, initselfP);
    return (TRUE);
}


/*
 * LogForLuApiWork
 *   This procedure is called whenever the LUAPI needs to ask
 *   permission of the global buffer management to get a buffer.
 *   It turns on the appropriate bit in the LU work bits, and calls 
 *   LogForLuWork in the LU.
 */
static void LogForLuApiWork (Lu  *this, byte work)
{
    this->luApiP->workBits |= work;
    LogForLuWork( this, WORK_LUAPI);
}


/*
 * LogActluPosRsp
 *    Log to send a pos rsp actlu when a buffer is available.
 */
static void LogActluPosRsp (Lu*        this,
                            DataReq_t* clsiBfrP)
{
    /* be clean and check save ptr first,
     * but should never find non null value
     * unless remote device violates SNA
     */
    if (!this->luApiP->actluReqP)
        DspuCLSIMsgDestroy(clsiBfrP);
        
    this->luApiP->actluReqP = clsiBfrP;
    LogForLuApiWork( this, LUAPIWORK_ACTLURSP);
}


/*
 * IfOpenLogForInitself
 *    If the LU-LU API is in opening state then
 *    log to send an initself when a buffer is available.
 */
static void IfOpenLogForInitself (Lu* this)
{
    if (this->luApiP->luLuP)
        if (this->luApiP->luLuP->c.apiState == ApiOpening)
            LogForLuApiWork( this, LUAPIWORK_INITSELF);
}


/*
 * LuApiDoWork
 *   This checks the work bits in order.
 *
 *   If the work bit is on and buffers are available it sends the
 *   appropriate command and turns the bit off.
 *   TRUE is returned if the work was done or no buffers were available.
 *   FALSE is returned if no LU work bit was set.
 *   The enum workResult is set to indicate if 
 *   work was done, work was not done or no buffers were available.
 * 
 *   Note: This function returns a boolean since it is called by
 *         DoLuWorkUp within QueryAllChildrenParm.
 */
boolean LuApiDoWork (Lu* this ,WorkResult* workResult )
{
    DataReq_t* clsiBfrP;
    boolean ret = FALSE;
    *workResult = WORK_NOT_DONE;

    if (this->luApiP->workBits & LUAPIWORK_ACTLURSP) {
        if (this->luApiP->desiredState == ActAv){
            ret = SendActluPosRsp( this, this->luApiP->actluReqP,
                                   LU_AVAILABLE);
            if (ret) {
                PuLuMapSetLuActive( this->upPuP, this->upLuNumber);
                SscpLuFsm( this, Available, NULL);
	    }

        } else{
            ret = SendActluPosRsp( this, this->luApiP->actluReqP,
                                   LU_NOT_AVAILABLE);
            if (ret) {
                PuLuMapSetLuActive( this->upPuP, this->upLuNumber);
                SscpLuFsm( this, NotAvailable, NULL);
	    }
        }
        if (ret) {
            this->luApiP->workBits &= (~LUAPIWORK_ACTLURSP);
            this->luApiP->workProgress |= LUAPIWORK_ACTLURSP;
            *workResult = WORK_DONE;
        }

    } else if (this->luApiP->workBits & LUAPIWORK_INITSELF) {
        if (SendInitself( this)) {
            this->luApiP->workBits &= (~LUAPIWORK_INITSELF);
            this->luApiP->workProgress |= LUAPIWORK_INITSELF;
            *workResult = WORK_DONE;
            ret = TRUE;
        }

    } else if (this->luApiP->workBits & LUAPIWORK_NOTIFYUNAVAIL) {
        clsiBfrP = BuildNotify(this->upLuNumber, LU_NOT_AVAILABLE);
        if (clsiBfrP) {
            SendPu(this->upPuP, clsiBfrP);
            this->luApiP->workBits &= (~LUAPIWORK_NOTIFYUNAVAIL);
            this->luApiP->workProgress |= LUAPIWORK_NOTIFYUNAVAIL;
            *workResult = WORK_DONE;
            ret = TRUE;
        }

    } else if (this->luApiP->workBits & LUAPIWORK_NOTIFYAVAIL) {
        clsiBfrP = BuildNotify(this->upLuNumber, LU_AVAILABLE);
        if (clsiBfrP) {
            SendPu(this->upPuP, clsiBfrP);
            this->luApiP->workBits &= (~LUAPIWORK_NOTIFYAVAIL);
            this->luApiP->workProgress |= LUAPIWORK_NOTIFYAVAIL;
            *workResult = WORK_DONE;
            ret = TRUE;
        }
    }

    /* relog if work is still to be done */
    if (this->luApiP->workBits)
        LogForLuWork( this, WORK_LUAPI);

    return (ret);
}


/*
 * AckWorkComplete
 *   Acknowledge the buffer by calling PuFreeBuffer.
 */
static void AckWorkComplete (Lu* this, byte work)
{
    if (this->luApiP->workProgress & work) {
        this->luApiP->workProgress &= (~work);
        PuFreeBuffer( this->upPuP);
    }
}

      
/*
 * CompleteLuReceive
 *   We have both an application buffer and a received frame for it.
 *   This function does the following:
 *      copy data into the users buffer,
 *      send +rsp for a definite response request or
 *        free the receive buffer,
 *      and then drive luReceive Indication.
 */
static void CompleteLuReceive (Lu* this)
{
    LuLuApi* luLuP = this->luApiP->luLuP;
    uint  dataLen = 0;
    boolean endData = TRUE;
    hword sysSense = 0;
    hword userSense = 0;
    byte * bufPtr;

    if (luLuP->qdRri == ReceiveRequest) {
        DecodeLuLuFmd( luLuP->qdBfrP,
                       luLuP->c.rcvBufLen, luLuP->c.rcvBuf, &dataLen,
                       &luLuP->lastFrameWasEC, &endData);
        SendPosRspWithPacing(this, luLuP->qdBfrP);

    } else { /* ReceiveResponse - for luReceive must be negative*/

        SNAMU  snaMu;
        /* we decode the MU again to get the response code -
         * we did it once before, but since we don't expect to get
         * -ve rsps much, it doesn't matter that it's inefficient
         * to do it again
         */
        DecodeMu(luLuP->qdBfrP, &snaMu);
        sysSense = snaMu.systemSense;
        userSense = snaMu.userSense;
        DspuCLSIMsgDestroy(luLuP->qdBfrP);
    }
    luLuP->qdBfrP = 0;

    /* get rcvBuf ptr before rcvInd call because luReceiveContinue
     * may be called from rcvInd
     */
    bufPtr = luLuP->c.rcvBuf;
    luLuP->c.rcvBuf = NULL;
    luLuP->c.rcvBufLen = 0;

    luLuP->c.rcvInd( luLuP->c.applH, bufPtr, dataLen, endData,
                     sysSense, userSense );
}


/*
 * CompleteSscpLuReceive
 *   We have both an application buffer and a received frame for it.
 *   This function does the following:
 *      copy data into the users buffer,
 *      free the receive buffer,
 *      and then drive sscpLuReceive Indication.
 */
static void CompleteSscpLuReceive (Lu* this)
{
    SscpLuApi* sscpLuP = this->luApiP->sscpLuP;
    uint  dataLen;
    uint  dataCopyLen;
    hword sysSense = 0;
    hword userSense = 0;
    DataReq_t* bufferP;
    byte * bufPtr;

    if (sscpLuP->reqFirst) {
        bufferP = sscpLuP->qdReqP;
        sscpLuP->qdReqP = 0;
        sscpLuP->reqFirst = FALSE;

    } else { /* response */

        SNAMU  snaMu;
        /* we decode the MU again to get the response code -
         * we did it once before, but since we don't expect to get
         * -ve rsps much, it doesn't matter that it's inefficient
         * to do it again
         */
        DecodeMu(sscpLuP->qdRspP, &snaMu);
        sysSense = snaMu.systemSense;
        userSense = snaMu.userSense;

        bufferP = sscpLuP->qdRspP;
        sscpLuP->qdRspP = 0;
        sscpLuP->reqFirst = TRUE;
    }

    dataLen = GetDataLength(bufferP);
    dataCopyLen = sscpLuP->c.rcvBufLen;
    if (dataLen < dataCopyLen)
        dataCopyLen = dataLen;
    memcpy( sscpLuP->c.rcvBuf, ReturnDataHandle(bufferP), dataCopyLen);
    DspuCLSIMsgDestroy( bufferP);

    /* get rcvBuf ptr before rcvInd call because sscpLuReceiveContinue
     * may be called from rcvInd
     */
    bufPtr = sscpLuP->c.rcvBuf;
    sscpLuP->c.rcvBuf = NULL;
    sscpLuP->c.rcvBufLen = 0;

    sscpLuP->c.rcvInd( sscpLuP->c.applH, bufPtr, dataLen, TRUE,
                       sysSense, userSense );
}


/*
 * CloseSscpLu
 *   A utility of the FSMs to do close actions appropriate to the
 *   state of API calls.
 **/
static void CloseSscpLu (Lu** thisP)
{
    SscpLuApi* sscpLuP = (*thisP)->luApiP->sscpLuP;

    if (sscpLuP)
        switch (sscpLuP->c.apiState) {

        case ApiOpen:
            sscpLuP->c.closeInd( sscpLuP->c.applH, sscpLuP->c.rcvBuf);
            SscpLuApi_dl( *thisP);
            if ( ! (*thisP)->luApiP->luLuP )
                LuApi_dl( thisP);
            break;

        case ApiClosing:
            /* signal sscpLuClose to complete the clean up */
            if (sscpLuP->c.closeComplete)
                process_set_boolean(sscpLuP->c.closeComplete, TRUE);
            break;

        case ApiOpening:
            /* signal sscpLuOpen to complete the clean up */
            if (sscpLuP->c.openComplete)
                process_set_boolean(sscpLuP->c.openComplete, TRUE);
            break;
        }
}


/*
 * SaveOpenResult
 *   Save the result (success or failure reason) for return on
 *   the api interface later.
 */
static void SaveOpenResult (LuApiCommon* cP,
                            LuApiReturnCode result)
{
    if (cP)
        if (cP->apiState == ApiOpening)
            cP->openResult = result;
}


/*
 * SaveOpenResultWithSenseCode
 *   Save the result (success or failure reason) and an SNA
 *   SENSE code for return on the api interface later.
 */
static void SaveOpenResultWithSenseCode (LuLuApi* luLuP,
                                         LuApiReturnCode result,
                                         DataReq_t* rspP)
{
    SNAMU  snaMu;
    if (luLuP) {
        if (rspP) {
            DecodeMu(rspP, &snaMu);
            *(luLuP->openParmP->sysSense) = snaMu.systemSense;
            *(luLuP->openParmP->userSense) = snaMu.userSense;
        }
        luLuP->c.openResult = result;
    }
}


/*
 * CloseLu
 *   A utility of the FSMs to do close actions appropriate to the
 *   state of API calls.
 */
static void CloseLu (Lu** thisP)
{
    LuLuApi* luLuP = (*thisP)->luApiP->luLuP;
   
    if (luLuP)
        switch (luLuP->c.apiState) {

        case ApiOpen:
            luLuP->c.closeInd(luLuP->c.applH, luLuP->c.rcvBuf);
            LuLuApi_dl( *thisP);
            if ( ! (*thisP)->luApiP->sscpLuP )
                LuApi_dl( thisP);
            break;

        case ApiClosing:
            /* signal luClose to complete the clean up */
            if (luLuP->c.closeComplete)
                process_set_boolean(luLuP->c.closeComplete, TRUE);
            break;

        case ApiOpening:
            /* signal luOpen to complete the clean up */
            if (luLuP->c.openComplete)
                process_set_boolean(luLuP->c.openComplete, TRUE);
            break;
        }
}


/*
 * CloseLuApis
 *   A utility of the FSMs to do both close actions.
 */
static void CloseLuApis (Lu** thisP, LuApiReturnCode status)
{
    /* Note: CloseSscpLu will destroy the control blocks if
     * the lu-lu api is closed!
     */
    SaveOpenResult( &((*thisP)->luApiP->sscpLuP->c), status );
    CloseSscpLu( thisP);
    if (*thisP) {
        SaveOpenResult( &((*thisP)->luApiP->luLuP->c), status );
        CloseLu( thisP);
    }
}


/*
 * CompleteOpen
 *   A utility of the FSMs to record the open status and
 *   kick the luOpen or sscpLuOpen to complete.
 */
static void CompleteOpen (LuApiCommon* apiCP, LuApiReturnCode status)
{
    if (apiCP)
        if (apiCP->apiState == ApiOpening) {
            apiCP->openResult = status;
            if (status == Success)
                apiCP->apiState = ApiOpen;

            /* signal luOpen or sscpLuOpen to complete */
            if (apiCP->openComplete)
                process_set_boolean(apiCP->openComplete, TRUE);
        }
}

/*
 * CompleteBind
 *   A utility of the LU-LU FSM to run checks on the bind
 *   and respond to it appropriately.
 *   The return is TRUE when a positive bind rsp is sent.
 */
static boolean CompleteBind (Lu* this, DataReq_t* clsiBfrP)
{
    BindData decodedBind;
    hword errorbyte;
    boolean passBind;
    LuLuApi* luLuP = this->luApiP->luLuP;
   
    if (!luLuP) {
        SendNegRsp( this, clsiBfrP, ResourceNotAvail0801, rnaNotReady);
        return (FALSE);
    }

    passBind = DecodeBind( clsiBfrP, &decodedBind,
                           luLuP->openParmP->bindDataLen,
                           luLuP->openParmP->bindData,
                           luLuP->openParmP->bindDataCnt);
    if (passBind) {

        errorbyte = 0; /* to satisfy the compiler */
        if ( (decodedBind.luType != 0) && (decodedBind.luType != 2) ) {
            passBind = FALSE;
            errorbyte = 14;
        }
        if (passBind) {
           /* save the BIND info we need to build PIUs */
           luLuP->plu = decodedBind.plu;
           luLuP->tsProfile = decodedBind.tsProfile;
           luLuP->sendPacingWindow = decodedBind.sSendPacingWindow;
           luLuP->luType = decodedBind.luType;

           switch (decodedBind.sluRspProtocol) {
           case NoResponse:
               luLuP->rh1 = 0;
               break;
           case ExceptionResponse:
           case DefiniteOrExceptionResponse:
               luLuP->rh1 = 0x90;
               break;
           case DefiniteResponse:
               luLuP->rh1 = 0x80;
               break;
           }

           if (decodedBind.bracketSupport) {
               if (decodedBind.bsSecMaySendEB)
                   luLuP->rh2 = 0xC0; /* send everything BB, EB */
               else
                   luLuP->rh2 = 0xA0; /* send everything BB, CD */
           }

           SendPosRsp( this, clsiBfrP);

       } else
           SendNegRsp( this, clsiBfrP, InvalidParameter, errorbyte);

    } else
        SendNegRsp( this, clsiBfrP, RULengthError, 0);

    if (!passBind)
        SaveOpenResult( &(luLuP->c), BindFailure);

    return (passBind);
}


/*
 * PacingQSend
 *   Send request to PU via pacing queue.
 */
static void PacingQSend (Lu* this, DataReq_t* clsiBfrP)
{
    LuLuApi* luLuP = this->luApiP->luLuP;

    if (luLuP->sendPacingWindow) {
        if (luLuP->pacingCount == 0)
            SetPacingBit( clsiBfrP);
        if (luLuP->pacingCount == luLuP->sendPacingWindow) {
            /* should queue request here if we were really
             * implementing pacing - but the RTTM appl doesn't
             * require this because it always waits for echo
             * responses after sending just one request
             */
            SendPu( this->upPuP, clsiBfrP);

        } else {
            SendPu( this->upPuP, clsiBfrP);
            luLuP->pacingCount++;
        }

    } else
        SendPu( this->upPuP, clsiBfrP);  /* no pacing */
}


/*
 * PacingQRelease
 *   Pacing response received - send stuff from pacing queue.
 *   See PacingQSend
 */
static void PacingQRelease (Lu* this)
{
    this->luApiP->luLuP->pacingCount = 0;
}




/*
 * sscpLuFsm
 * ---------
 *    SSCP-LU FSM Structure
 *
 */

static const FsmStruct sscpLuFsm[sscpLuMaxInput][sscpLuMaxState] = {

/*                   0         1        2        3        4        5        6        7   */
/*                   Reset    Pend     Pend     Act      Pend     Act      Def      Def  */
/*                            Actlu    Notify   NotAv    Notify   Av       Rsp      Rsp  */
/*                                     NotAv             Avail             Reqd     Reqd */
/*                                     Rsp               Rsp               NotAv    Av   */
/*StartDynamicLu*/ {{1,a1},  {1,a0},  {2,a5},  {3,a2},  {4,a5},  {5,a5},  {6,a0},  {7,a0}},
/*StartStaticLu*/  {{3,a2},  {1,a0},  {2,a5},  {3,a2},  {4,a5},  {5,a5},  {6,a0},  {7,a0}},
/*uRplyPsdRspP*/   {{0,a9},  {1,a9},  {2,a9},  {3,a9},  {4,a9},  {5,a9},  {6,a9},  {7,a9}},
/*uRplyPsdRspN*/   {{0,a9},  {0,a10}, {2,a9},  {3,a9},  {4,a9},  {5,a9},  {6,a9},  {7,a9}},
/*uActlu*/         {{0,a9},  {1,a3},  {1,a3},  {1,a3},  {1,a3},  {1,a3},  {1,a3},  {1,a3}},
/*uDactlu*/        {{0,a9},  {0,a35}, {0,a35}, {0,a35}, {0,a35}, {0,a35}, {0,a35}, {0,a35}},
/*Available*/      {{0,a0},  {5,a4},  {2,a0},  {4,a6},  {4,a0},  {5,a0},  {4,a6},  {5,a0}},
/*NotAvailable*/   {{0,a0},  {3,a5},  {2,a0},  {3,a0},  {4,a0},  {2,a7},  {3,a0},  {2,a7}},
/*uNtfyRspP*/      {{0,a9},  {1,a9},  {3,a11}, {3,a9},  {5,a12}, {5,a9},  {6,a9},  {7,a9}},
/*uNtfyRspN*/      {{0,a9},  {1,a9},  {2,a15}, {3,a9},  {2,a14}, {5,a9},  {6,a9},  {7,a9}},
/*uInitselfRspP*/  {{0,a9},  {1,a9},  {2,a9},  {3,a9},  {4,a9},  {5,a8},  {6,a9},  {7,a8}},
/*uInitselfRspN*/  {{0,a9},  {1,a9},  {2,a9},  {3,a9},  {4,a9},  {2,a13}, {6,a9},  {7,a25}},
/*uNspe*/          {{0,a9},  {1,a16}, {2,a17}, {3,a17}, {4,a17}, {2,a18}, {6,a17}, {7,a26}},
/*uOtherSscpLuReq*/{{0,a9},  {1,a16}, {2,a19}, {3,a19}, {4,a19}, {5,a19}, {6,a19}, {7,a19}},
/*uOtherSscpLuRsp*/{{0,a9},  {1,a9},  {2,a20}, {3,a20}, {4,a20}, {5,a20}, {6,a21}, {7,a21}},
/*uLuLu*/          {{0,a0},  {1,a16}, {2,a16}, {3,a16}, {4,a16}, {5,a27}, {6,a16}, {7,a27}},
/*StopLu*/         {{0,a0},  {0,a36}, {0,a36}, {0,a36}, {0,a36}, {0,a36}, {0,a36}, {0,a36}},
/*CloseLuLu*/      {{0,a0},  {1,a33}, {2,a34}, {3,a33}, {4,a34}, {2,a7},  {6,a33}, {7,a34}},
/*ApiSendDefReq*/  {{0,a0},  {1,a28}, {2,a22}, {6,a23}, {4,a22}, {7,a23}, {6,a24}, {7,a24}},
/*ApiSendOther*/   {{0,a0},  {1,a28}, {2,a23}, {3,a23}, {4,a23}, {5,a23}, {6,a23}, {7,a23}},
/*ApiReceive*/     {{0,a0},  {1,a28}, {2,a29}, {3,a29}, {4,a29}, {5,a29}, {6,a29}, {7,a29}},
/*ApiClose*/       {{0,a0},  {1,a28}, {2,a30}, {0,a30}, {4,a30}, {5,a30}, {6,a0},  {7,a0}}
};


/*
 * SscpLuFsm
 *   SSCP LU FSM handling
 *
 */
static boolean SscpLuFsm (Lu         * this, 
                          SscpLuInput  fsmInput, 
                          DataReq_t  * clsiBfrP)
{
    LuApi* luApiP = this->luApiP;

    boolean passToLuLuFsm = FALSE;
    const FsmStruct * luFsm = &sscpLuFsm[fsmInput][luApiP->sscpLuState];

    DspuFsmTrace2(this->debugLevel, LuFsmTrace, this->upPuP, this->upLuNumber,
                  fsmInput, luApiP->sscpLuState, luFsm->state, luFsm->action);

    /* Check if LuState is changing between active<-->inactive
     *   LuStateChangeTrap will be generated from within Check() routine
     */
    if (luApiP->sscpLuState != luFsm->state){
        DspuFsmChgTrace2(this->debugLevel, LuFsmChgTrace, 
                         this->upPuP, this->upLuNumber,
                         sscpLuFsmInputs[fsmInput], 
                         sscpLuFsmStates[luApiP->sscpLuState],
                         sscpLuFsmStates[luFsm->state]);
    }
    luApiP->sscpLuState = luFsm->state;
   
    switch(luFsm->action) {

    /*---- Null action. */
    case a0:     
        break;

    /*---- Send NMVT Reply PSID */
    case a1:
        LogDddlu(this);
        break;

    /*---- Start on LU that is active but not available. */
    case a2:
        CompleteOpen( &(luApiP->sscpLuP->c), Success);
        if (luApiP->desiredState == ActAv)
            SscpLuFsm(this, Available, NULL);
        break;

    /*---- ACTLU received - send +rsp and drive appropriate fsm action */
    case a3:
        AckWorkComplete( this, LUAPIWORK_NOTIFYUNAVAIL);        
        AckWorkComplete( this, LUAPIWORK_NOTIFYAVAIL);        
        AckWorkComplete( this, LUAPIWORK_INITSELF);        
        if (luApiP->desiredState == ActAv) {
            if (SendActluPosRsp( this, clsiBfrP, LU_AVAILABLE)) {
                PuLuMapSetLuActive( this->upPuP, this->upLuNumber);
                SscpLuFsm( this, Available, NULL);
	    } else
                LogActluPosRsp( this, clsiBfrP);
        } else {
            if (SendActluPosRsp( this, clsiBfrP, LU_NOT_AVAILABLE)) {
                PuLuMapSetLuActive( this->upPuP, this->upLuNumber);
                SscpLuFsm( this, NotAvailable, NULL);
	    } else
                LogActluPosRsp( this, clsiBfrP);
        }
        break;

    /*---- +RSP ACTLU(Available) has been sent. */
    case a4:
        CompleteOpen( &(luApiP->sscpLuP->c), Success);
        IfOpenLogForInitself( this);
        break;

    /*---- +RSP ACTLU(Not Available) has been sent. */
    case a5:
        CompleteOpen( &(luApiP->sscpLuP->c), Success);
        break;

    /*---- currently active - now want to also be available */
    case a6:
        luApiP->desiredState = ActAv;
        LogForLuApiWork(this, LUAPIWORK_NOTIFYAVAIL);
        break;

    /*---- currently act and avail - want to be available no longer */
    case a7:
        luApiP->desiredState = ActNotAv;
        LogForLuApiWork(this, LUAPIWORK_NOTIFYUNAVAIL);
        /* forget waiting for any possible pending initself rsp */
        AckWorkComplete( this, LUAPIWORK_INITSELF);
        break;

    /*---- Initself +ve response received. */
    case a8:
        AckWorkComplete( this, LUAPIWORK_INITSELF);
        DspuCLSIMsgDestroy(clsiBfrP);
        break;

    /*---- Discard a response. */
    case a9:
        if (clsiBfrP)
            DspuCLSIMsgDestroy(clsiBfrP);
        break;

    /*---- -RSP to Reply PSID received from host. */
    case a10:
        SaveOpenResultWithSenseCode( luApiP->luLuP, NoLuAvailable, clsiBfrP);
        CompleteOpen( &(luApiP->sscpLuP->c), NoLuAvailable);
        CompleteOpen( &(luApiP->luLuP->c), NoLuAvailable);
        /* discard response buffer */
        if (clsiBfrP)
            DspuCLSIMsgDestroy(clsiBfrP);
        break;

    /*---- +RSP to Notify(Not Avail) received from host. */
    case a11:
        AckWorkComplete( this, LUAPIWORK_NOTIFYUNAVAIL);
        /* discard response buffer */
        DspuCLSIMsgDestroy(clsiBfrP);

        /* If we need to notify that we are now available, do that
         * in preference to sending a queued request from the sscp-lu api.
         * If there is a queued sscp-lu api request it will get forwarded
         * after the notify available response.
         */
        if (luApiP->desiredState == ActAv)
            SscpLuFsm( this, Available, NULL);
        else {
            if (luApiP->sscpLuP) {
                clsiBfrP = luApiP->sscpLuP->sendQdReqP;
                if (clsiBfrP) {
                    luApiP->sscpLuP->sendQdReqP = 0;
                    SscpLuFsm( this, ApiSendDefReq, clsiBfrP);
                }
            }

            /* now complete close of lu-lu api if close was in progress */
            CloseLu( &this);       /* note: may destroy luApi CB */
	}
        break;

    /*---- +RSP to Notify(Avail) received from host. */
    case a12:
        AckWorkComplete( this, LUAPIWORK_NOTIFYAVAIL);
        /* discard response buffer */
        DspuCLSIMsgDestroy(clsiBfrP);
        if (this->luApiP->desiredState != ActAv)
            SscpLuFsm( this, NotAvailable, NULL);
        else
            IfOpenLogForInitself( this);
        break;

    /*---- -RSP to Initself received from host. */
    case a13:
        AckWorkComplete( this, LUAPIWORK_INITSELF);
        luApiP->desiredState = ActNotAv;
        LogForLuApiWork(this, LUAPIWORK_NOTIFYUNAVAIL);
        SaveOpenResultWithSenseCode( luApiP->luLuP, HostApplInvalid, clsiBfrP);
        /* discard response buffer */
        DspuCLSIMsgDestroy(clsiBfrP);
        break;

    /*---- -RSP to Notify(Avail) received from host.
     *     This is certainly not expected to happen and I am
     *     not sure what really should be done about it if it does.
     *     For now, the best reaction seems to be to give up on
     *     the session establishment and send notify unavailable
     *     (that's similar to what dspu does).
     */
    case a14:
        AckWorkComplete( this, LUAPIWORK_NOTIFYAVAIL);
        SaveOpenResultWithSenseCode( luApiP->luLuP, NoLuAvailable, clsiBfrP);
        /* discard response buffer */
        DspuCLSIMsgDestroy(clsiBfrP);
        LogForLuApiWork(this, LUAPIWORK_NOTIFYUNAVAIL);
        break;

    /*---- -RSP to Notify(Not Avail) received from host.
     *     This is also not expected to ever happen.
     *     It's even harder to know what to really do if
     *     it does.  Send it again (again following dspu's example)
     */
    case a15:
        AckWorkComplete( this, LUAPIWORK_NOTIFYUNAVAIL);
        /* discard response buffer */
        if (clsiBfrP)
            DspuCLSIMsgDestroy(clsiBfrP);
        LogForLuApiWork(this, LUAPIWORK_NOTIFYUNAVAIL);
        break;

    /*---- request received when we didn't think we were active */
    case a16:
        /* Send neg rsp 8005. */
        SendNegRsp( this, clsiBfrP, NoSession, 0);
        break;

    /*---- Received totally unexpected NSPE. */
    case a17:
        SendPosRsp( this, clsiBfrP);
        break;

    /*---- Received NSPE. So conn to host appl is failing - must clean up.*/
    case a18:
        SaveOpenResult( &(luApiP->luLuP->c), HostApplInvalid);
        SendPosRsp( this, clsiBfrP);
        luApiP->desiredState = ActNotAv;
        LogForLuApiWork(this, LUAPIWORK_NOTIFYUNAVAIL);
        break;

    /*---- process "other" SSCP-LU request */
    case a19:
        if (luApiP->sscpLuP->c.apiState == ApiOpen)
            if (luApiP->sscpLuP->qdReqP == 0){
                luApiP->sscpLuP->qdReqP = clsiBfrP;
                if (luApiP->sscpLuP->qdRspP == 0)
                    luApiP->sscpLuP->reqFirst = TRUE;
                if (luApiP->sscpLuP->c.rcvBuf)
                    CompleteSscpLuReceive(this);
            } else
                SendNegRsp( this, clsiBfrP, LinkFailure, 0);
        else
            /* no application - just say "ok" to host */
            SendPosRsp( this, clsiBfrP);
        break;

    /*---- process "other" SSCP-LU response */
    case a20:
        if (luApiP->sscpLuP->c.apiState == ApiOpen) {
            if (luApiP->sscpLuP->qdRspP == 0) {
                luApiP->sscpLuP->qdRspP = clsiBfrP;
                if (luApiP->sscpLuP->qdReqP == 0)
                    luApiP->sscpLuP->reqFirst = FALSE;
                if (luApiP->sscpLuP->c.rcvBuf)
                    CompleteSscpLuReceive(this);
            } else {
                /* discard response buffer */
                if (clsiBfrP)
                    DspuCLSIMsgDestroy(clsiBfrP);
            }
        } else {
            /* discard response buffer */
            if (clsiBfrP)
                DspuCLSIMsgDestroy(clsiBfrP);
        }
        break;

    /*---- process "other" SSCP-LU response when def rsp pending */
    case a21:
        if (luApiP->sscpLuP->c.apiState == ApiOpen) {
            if (luApiP->sscpLuP->qdRspP == 0) {
                luApiP->sscpLuP->qdRspP = clsiBfrP;
                clsiBfrP = 0;
                if (luApiP->sscpLuP->qdReqP == 0)
                    luApiP->sscpLuP->reqFirst = FALSE;
                if (luApiP->sscpLuP->c.rcvBuf)
                    CompleteSscpLuReceive(this);
            }

        } else
            if (luApiP->sscpLuP->c.apiState == ApiClosing)
                CloseSscpLu( &this);

        /* discard response buffer if not given to appl */
        if (clsiBfrP)
            DspuCLSIMsgDestroy(clsiBfrP);

        /* need to check 'this' pointer because 
         * CloseSscpLu may destroy the CBs.
         */
        if (this) {
            if (luApiP->desiredState == ActAv)
                SscpLuFsm( this, Available, NULL);
            else
                SscpLuFsm( this, NotAvailable, NULL);
        }
        break;

    /*---- process api send def rsp request when Notify rsp pending */
    case a22:
        if (luApiP->sscpLuP->sendQdReqP) {
            /* if fsm is working to intended design then the only way
             * we'll get here is if the application has violated IRM
             */
            DspuCLSIMsgDestroy(clsiBfrP);
            luApiP->sscpLuP->c.sendResult = IRMViolation;
        } else {
            luApiP->sscpLuP->sendQdReqP = clsiBfrP;
            clsiBfrP = 0;
        }
        break;

    /*---- process api send (no rsp pending or sending a nonDR req) */
    case a23:
        SendPu(this->upPuP, clsiBfrP);
        break;

    /*---- process api send - appl has violated IRM */
    case a24:
        DspuCLSIMsgDestroy(clsiBfrP);
        luApiP->sscpLuP->c.sendResult = IRMViolation;
        break;

    /*---- process -rsp(initself) when def rsp pending */
    case a25:
        AckWorkComplete( this, LUAPIWORK_INITSELF);
        SaveOpenResultWithSenseCode( luApiP->luLuP, HostApplInvalid, clsiBfrP);
        /* discard response buffer */
        DspuCLSIMsgDestroy(clsiBfrP);
        luApiP->desiredState = ActNotAv;
        break;

    /*---- process NSPE req when def rsp pending */
    case a26:
        SaveOpenResult( &(luApiP->luLuP->c), HostApplInvalid);
        SendPosRsp( this, clsiBfrP);
        luApiP->desiredState = ActNotAv;
        break;

    /*---- forward to LU-LU FSM */
    case a27:
        passToLuLuFsm = TRUE;
        break;

    /*---- api call when we're not active - should never happen */
    case a28:
        break;

    /*---- process sscp-lu api receive */
    case a29:
        if (luApiP->sscpLuP->qdReqP || luApiP->sscpLuP->qdRspP)
            CompleteSscpLuReceive(this);
        break;

    /*---- process sscp-lu api close - not waiting for a def rsp */
    case a30:
        CloseSscpLu( &this);
        break;

    /*---- lu-lu api closing and no need to wait for anything to complete */
    case a33:
        luApiP->desiredState = ActNotAv;
        /* forget waiting for any possible pending initself rsp */
        AckWorkComplete( this, LUAPIWORK_INITSELF);
        CloseLu( &this);
        break;

    /*---- lu-lu api closing but we need to wait for session clean up */
    case a34:
        luApiP->desiredState = ActNotAv;
        /* forget waiting for any possible pending initself rsp */
        AckWorkComplete( this, LUAPIWORK_INITSELF);
        break;

    /*---- DACTLU handling: send +rsp, close apis and clean up */
    case a35:
        PuLuMapSetLuInactive( this->upPuP, this->upLuNumber);
        SendPosRsp( this, clsiBfrP);
        AckWorkComplete( this, LUAPIWORK_NOTIFYUNAVAIL);        
        AckWorkComplete( this, LUAPIWORK_NOTIFYAVAIL);        
        AckWorkComplete( this, LUAPIWORK_INITSELF);        
        CloseLuApis( &this, NoLuAvailable);
        break;

    /*---- Stop due to DACTPU. */
    case a36:
        CloseLuApis( &this, HostConnInactive);
        break;
    }
  
    return (passToLuLuFsm);
}



/*
 * luLuFsm
 * -------
 *   LU-LU FSM Structure
 *
 */
static const FsmStruct luLuFsm[luLuMaxInput][luLuMaxState] = {

/*                         0         1        2        */
/*                       LuLuReset  Bound    Flow Open */
/*Bind               */ {{0,a1},    {1,a2},   {2,a2}},
/*Unbind             */ {{0,a3},    {0,a3},   {0,a3}},
/*Sdt                */ {{0,a4},    {2,a6},   {2,a5}},
/*Clear              */ {{0,a4},    {1,a5},   {1,a5}},
/*FmdReq             */ {{0,a4},    {1,a11},  {2,a8}},
/*OtherReq           */ {{0,a4},    {1,a5},   {2,a5}},
/*FmdNegRsp          */ {{0,a9},    {1,a10},  {2,a10}},
/*OtherRsp           */ {{0,a9},    {1,a9},   {2,a9}},
/*PacingRsp          */ {{0,a0},    {1,a0},   {2,a14}},
/*SendBindPosRspNoSdt*/ {{2,a7},    {1,a0},   {2,a0}},
/*SendBindPosRspSdt  */ {{1,a0},    {1,a0},   {2,a0}},
/*luLuApiSend        */ {{0,a12},   {1,a12},  {2,a13}},
/*luLuApiReceive     */ {{0,a15},   {1,a15},  {2,a15}},
/*luLuApiClose       */ {{0,a17},   {0,a16},  {0,a16}}
                 };

/*
 * LuLuFsm
 *   LU-LU FSM handling
 */

static boolean                   /* forward frame if TRUE */
LuLuFsm (Lu* this, LuLuInput fsmInput, DataReq_t* clsiBfrP)
{
    boolean forward = FALSE;
    LuApi* luApiP = this->luApiP;
    const FsmStruct* luFsm = &luLuFsm[fsmInput][luApiP->luLuState];

    if (luApiP->luLuState != luFsm->state){
        DspuFsmChgTrace2(this->debugLevel, LuFsmChgTrace, 
                         this->upPuP, this->upLuNumber,
                         luLuFsmInputs[fsmInput], 
                         luLuFsmStates[luApiP->luLuState],
                         luLuFsmStates[luFsm->state]);
    }
    luApiP->luLuState = luFsm->state;
    switch(luFsm->action){

    /*---- Null action. */
    case a0:     
        break;
     
    /*---- Handle Bind
     *     Run checks - if fail, send -rsp and clean up luOpen.
     *     If success, send +rsp and drive LuFsm with SendBindPosRspNoSdt
     *     or SendBindPosRspSdt as appropriate.
     */
    case a1:
        if (CompleteBind(this, clsiBfrP)) {
           if (luApiP->luLuP->tsProfile == 3)
               LuLuFsm(this, SendBindPosRspSdt, NULL);
           else
               LuLuFsm(this, SendBindPosRspNoSdt, NULL);
        } else
           SscpLuFsm( this, CloseLuLu, NULL);
        break;
     
    /*---- Bind received when we were already bound. */
    case a2:
        SendNegRsp(this, clsiBfrP, SessionLimitExceeded, sleDependentLu);
        break;
     
    /*---- Unbind received.
     *     Send +rsp to host and CloseLuLu to SscpLuFsm.
     *     The luOpen may or may not be still in progress, so we record
     *     the result as failing to establish a bound process.
     */
    case a3:
        SaveOpenResult( &(luApiP->luLuP->c), BindFailure);
        SendPosRsp( this, clsiBfrP);
        SscpLuFsm( this, CloseLuLu, NULL);
        break;
     
    /*---- LU-LU session traffic received before BIND.
     *     Send -rsp(8005).
     */
    case a4:
        SendNegRsp( this, clsiBfrP, NoSession, 0);
        break;
     
    /*---- Send positive response. */
    case a5:
        SendPosRspWithPacing( this, clsiBfrP);
        break;
     
    /*---- BIND succeeded, SDT now received.
     *     Send positive response and return success to luOpen
     */
    case a6:
        SendPosRsp( this, clsiBfrP);
        /* drop through - to common code with a7 */
        
    /*---- BIND succeeded, SDT not expected.
     *     Return success to luOpen
     */
    case a7:
        CompleteOpen( &(luApiP->luLuP->c), Success);
        break;
     
    /*---- Handle FMD request from host. */
    case a8:
        if (luApiP->luLuP->qdBfrP == 0) {
            luApiP->luLuP->qdBfrP = clsiBfrP;
            luApiP->luLuP->qdRri = ReceiveRequest;
            if (luApiP->luLuP->c.rcvBuf)
                CompleteLuReceive(this);
        } else
            SendNegRspWithPacing( this, clsiBfrP, LinkFailure, 0);
        break;
     
    /*---- Discard a response. */
    case a9:
        if (clsiBfrP)
            DspuCLSIMsgDestroy(clsiBfrP);
        break;

    /*---- Handle -ve response to application request. */
    case a10:
        if (luApiP->luLuP->qdBfrP == 0) {
            luApiP->luLuP->qdBfrP = clsiBfrP;
            luApiP->luLuP->qdRri = ReceiveResponse;
            if (luApiP->luLuP->c.rcvBuf)
                CompleteLuReceive(this);
        } else
            SendNegRsp( this, clsiBfrP, LinkFailure, 0);
        break;

    /*---- Handle FMD request with flow closed.
     *     Send neg rsp 2005.
     */
    case a11:
        SendNegRspWithPacing( this, clsiBfrP, DataTrafficReset, 0);
        break;
     
    /*---- API luSend when data traffic flow closed.
     *     Return error return code to user.
     */
    case a12:
        luApiP->luLuP->c.sendResult = FlowClosed;
        break;
     
    /*---- API luSend. */
    case a13:
        /* real work of this action is done after we return from FSM */
        forward = TRUE;
        break;
     
    /*---- Pacing response received.
     *     Dequeue and send buffers from the pacing queue
     *     until the queue is exhausted or the pacing 
     *     window is reached.
     */
    case a14:
        PacingQRelease(this);
        break;
     
    /*---- API luReceive.
     *     If there is a pending receive buffer, then we can do
     *     the processing on this immediately.
     *     If there is no pending buffer then the user buffer
     *     details and luReceiveIndication address have already
     *     been placed in the luApi object.
     */
    case a15:
        if (luApiP->luLuP->qdBfrP)
            CompleteLuReceive(this);
        break;
     
    /*---- API luClose
     *     Send UNBIND Type X'0E', send CloseLuLu to SSCP-LU FSM.
     */
    case a16:
        clsiBfrP = BuildUnbind(this->upLuNumber, 0x0E, BLOCKING);
        SendPu(this->upPuP, clsiBfrP);
        SscpLuFsm(this, CloseLuLu, NULL);
        break;
     
    /*---- Send CloseLuLu to SSCP-LU FSM. */
     case a17:
        SscpLuFsm(this, CloseLuLu, NULL);
        break;
    }

    return (forward);
}
    

/*
 * LuApiReceiveSscpLu
 *   This is called to a pass data packet destined for an LU API LU
 *   on the SSCP-LU session.
 *
 */
void LuApiReceiveSscpLu (
       Lu*             this,
       DataReq_t*      clsiBfrP,       /* clsi buffer for data          */
       SNAMU*          snaMu
       )
{
    hword          fsmInput;

    if (snaMu->reqRsp == SNA_REQUEST) {
     
        if (snaMu->ruType == SNA_ACTLU)
            fsmInput = uActlu;
        else if (snaMu->ruType == SNA_DACTLU)
            fsmInput = uDactlu;
        else if (snaMu->ruType == SNA_NSPE)
            fsmInput = uNspe;
        else
            fsmInput = uOtherSscpLuReq;

    } else if (snaMu->reqRsp == SNA_RESPONSE) {

        if (snaMu->ruType == SNA_NOTIFY)
            fsmInput = (snaMu->rspType == SNA_POSITIVE)?
                         uNtfyRspP: uNtfyRspN;

        else if (snaMu->ruType == SNA_INITSELF)
            fsmInput = (snaMu->rspType == SNA_POSITIVE)?
                         uInitselfRspP: uInitselfRspN;

        else 
            fsmInput = uOtherSscpLuRsp;
    }

    else /* only (snaLu->reqRsp == SNA_NO_RH) expected here */
        fsmInput = uOtherSscpLuReq;  /* must be a middle or last segment */
    
    SscpLuFsm(this, fsmInput, clsiBfrP);
}


/*
 * LuApiReceiveLuLu
 *   This is called to a pass data packet destined for an LU API LU
 *   on the LU-LU session.
 *
 */
void LuApiReceiveLuLu (
       Lu*             this,
       DataReq_t*      clsiBfrP,       /* clsi buffer for data      */
       SNAMU*          snaMu
       )
{
    hword           fsmInput;

    if (SscpLuFsm(this, uLuLu, clsiBfrP)) {
              
       /* LU_LU data is accepted by SscpLuFsm, now pass to
        * the lu-lu fsm.
        */
       if (snaMu->reqRsp == SNA_REQUEST) {

           if (snaMu->ruType == SNA_BIND)
               fsmInput = Bind;

           else if (snaMu->ruType == SNA_UNBIND)
               fsmInput = Unbind;

           else if (snaMu->ruType == SNA_SDT)
               fsmInput = Sdt;

           else if (snaMu->ruType == SNA_CLEAR)
               fsmInput = Clear;

           else if (snaMu->fmd == TRUE)
               fsmInput = FmdReq;

           else
               fsmInput = OtherReq;

           /* record frame type for segmenting case */
           this->luApiP->lastReqType = fsmInput;

       } else if (snaMu->reqRsp == SNA_RESPONSE) {

           /* additional FSM call for pacing response */
           if (snaMu->pacing != 0)
               LuLuFsm(this, PacingRsp, clsiBfrP);

           if (snaMu->fmd == TRUE && (snaMu->rspType == SNA_NEGATIVE))
               fsmInput = FmdNegRsp;

           else
               fsmInput = OtherRsp;

        } else {
            /* only (snaLu->reqRsp == SNA_NO_RH) expected here
             * i.e. must be middle or last segment
             */
            fsmInput = this->luApiP->lastReqType;
        }

        LuLuFsm(this, fsmInput, clsiBfrP);

    }
}


/*
 * LuApiReceiveLuNmvtPsidRsp
 *   This is called to a pass a response to an NMVT PSID
 *   request sent by the LU.  The response is passed so that the LU
 *   may detect +ve vs -ve response and sense code, but it is expected
 *   to be finished with the buffer on return and the PU will free it.
 */
void LuApiReceiveLuNmvtPsidRsp (
       Lu*             this,
       DataReq_t*      clsiBfrP
       )
{
    hword  fsmInput;
    SNAMU  snaMu;

    if (clsiBfrP) {
        DecodeMu(clsiBfrP, &snaMu);

        fsmInput = (snaMu.rspType == SNA_POSITIVE)?
                     uRplyPsdRspP: uRplyPsdRspN;

        SscpLuFsm(this, fsmInput, clsiBfrP);
    }
}


/*
 * LuApiStartLu
 *   Drives appropriate Start function on SSCP-LU FSM.
 */
static void LuApiStartLu (Lu* this)
{
    if (PuLuMapIsLuActive(this->upPuP, this->upLuNumber))
        SscpLuFsm(this, StartStaticLu, NULL);
    else
        SscpLuFsm(this, StartDynamicLu, NULL);
}


/*
 * LuApiResetLu
 *   Drives the SSCP-LU FSM for hierachical reset
 */
void LuApiResetLu (Lu*  this)
{
   SscpLuFsm(this, StopLu, NULL);
}


/*
 * LuApiLuDelete
 *   This is the Lu destructor called when the upstream pu is destructed
 *   (from Pu_dl through LuPuChildDelete)
 *   The action required is equivalent to DACTPU handling because the
 *   application interface must be cleaned up.  The control blocks are
 *   always cleaned up when the interfaces are closed.
 */
void LuApiLuDelete (Lu* this)
{
    SscpLuFsm(this, StopLu, NULL);
}


/*
 * FindPuForLuApiUse
 *   Utility function for luOpen and sscpLuOpen.
 *   Look up pu by name and ensure that it really is active.
 */
static LuApiReturnCode FindPuForLuApiUse (char* hostName, Pu** puP)
{
    *puP = FindActiveUpPu(hostName);
    if (!(*puP)) {
        if (FindInactiveUpPu(hostName))
            return (HostConnInactive);
        else
            return (InvalidHandle);

    } else if ( !PuQueryUpPuActive((*puP)) ) {
       /* in active list, but probably caught it in
        * process of deactivation
        */
       return (HostConnInactive);
    }
    return (Success);
}


/*
 * FindLocaddr
 *   Search for an LU locaddr on a PU - common part of search for
 *   implementation of the "locaddr 0" option the luT0orT2Open and
 *   sscpLuOpen commands.
 *   The search is in the following order:
 *     - an active LU not allocated (to DSPU or LUAPI)
 *     - if dddlu supported on the PU, an unallocated inactive LU
 */
static byte FindLocaddr (Pu* puP)
{
    byte findLoc;

    /* search for an active LU not allocated (to DSPU or LUAPI) */
    findLoc = 0;
    do {
        findLoc++;
        if (PuLuMapIsLuActive(puP, findLoc))
            if (!QueryLuConflict( puP, findLoc, 1))
                return (findLoc);
    } while (findLoc < 255);

    /* if dddlu supported on the PU, find an unallocated inactive LU */
    if (PuIsDddluSupported(puP)) {
        findLoc = 0;
        do {
            findLoc++;
            if (!PuLuMapIsLuActive(puP, findLoc))
                if (!QueryLuConflict( puP, findLoc, 1))
                    return (findLoc);
        } while (findLoc < 255);
    }

    return (0);   /* failed to find any suitable LU */
}
    

/*
 * luT0orT2QueryPu request
 *   Check PU definition for LU type 0 or type 2 session.
 *   
 */
LuApiReturnCode luT0orT2QueryPu (
     char*            hostName    /* host name on the "sna host" or
                                     "dspu host" command             */
    )
{
    Pu* puP;
    return (FindPuForLuApiUse(hostName, &puP));
}


/*
 * luT0orT2Send request
 *   Send request on session.
 */
static LuApiReturnCode luT0orT2Send (
     void*    luApiLuH,  /* luapi LU Handle     */
     uint     ruDataLen, /* Length of RU data   */
     byte*    ruData     /* RU data             */
    )
{
    Lu* luP = luApiLuH;
    LuLuApi* luLuP = luP->luApiP->luLuP;
    if (!luLuP)
        return (InvalidHandle);

    if ( LuLuFsm(luP, luLuApiSend, NULL) ) {

        /*
         * Allocate a clsiBuffer, copy data into buffer,
         * send buffer to PU via pacing queue.
         * If buffer could not be allocated return failure to luSend.
         */
        DataReq_t *clsiBfrP;

        clsiBfrP = BuildLuLuDataReq( luP->upLuNumber,    /* OAF */
                                     luLuP->plu,         /* DAF */
                                     luLuP->snf,       /* SNF */
                                     luLuP->rh1,         /* DR1I, ERI */
                                     luLuP->rh2,     /* BBI, EBI, CDI */
                                     ruDataLen, ruData);
        if (clsiBfrP) {
            PacingQSend(luP, clsiBfrP);
            luLuP->c.sendResult = Success;
            if (luLuP->snf == 0xFFFF)
                luLuP->snf = 0;
            else
                luLuP->snf++;

         } else
            luLuP->c.sendResult = InsufficientMemory;
    }

    return (luLuP->c.sendResult);
}


/*
 * luT0orT2Receive request
 *   application ready to receive request on session.
 */
static LuApiReturnCode luT0orT2Receive (
     void*              luApiLuH,  /* luapi LU Handle            */
     uint               rcvBufLen, /* Length of receive buffer   */
     byte*              rcvBuf,    /* receive buffer             */
     TReceiveIndication luRcvInd   /* rcv indication entry point */
    )
{
    Lu* luP = luApiLuH;
    LuLuApi* luLuP = luP->luApiP->luLuP;
    if (!luLuP)
        return (InvalidHandle);

    luLuP->c.rcvBufLen = rcvBufLen;
    luLuP->c.rcvBuf = rcvBuf;
    luLuP->c.rcvInd = luRcvInd;

    LuLuFsm(luP, luLuApiReceive, NULL);

    return (Success);
}


/*
 * luT0orT2ReceiveContinue request
 *   application ready to continue to receive on session.
 */
static LuApiReturnCode luT0orT2ReceiveContinue (
     void*              luApiLuH,  /* luapi LU Handle            */
     uint               rcvBufLen, /* Length of receive buffer   */
     byte*              rcvBuf     /* receive buffer             */
    )
{
    Lu* luP = luApiLuH;
    LuLuApi* luLuP = luP->luApiP->luLuP;
    if (!luLuP)
        return (InvalidHandle);

    luLuP->c.rcvBufLen = rcvBufLen;
    luLuP->c.rcvBuf = rcvBuf;

    return (Success);
}


/*
 * luT0orT2Close request
 *   Request session close down.
 */
static LuApiReturnCode luT0orT2Close (
     void*  luApiLuH,   /* luapi LU Handle                          */
     byte** bufferP     /* outstanding rcv buffer, if any (returned)*/
    )
{
    LuApiReturnCode ret = Success;
    Lu* luP = luApiLuH;
    LuLuApi* luLuP = luP->luApiP->luLuP;
    if (!luLuP)
        return (InvalidHandle);

    if (!process_push_event_list(NULL))
        return (InsufficientMemory);

    luLuP->c.closeComplete = create_watched_boolean("DSPU LUAPI Close", 2);
    if (luLuP->c.closeComplete == NULL)
        ret = InsufficientMemory;
    else {

        luP->luApiP->desiredState = ActNotAv;
        luLuP->c.apiState = ApiClosing;
        /* init the boolean we watch to know when closing is done */
        process_set_boolean(luLuP->c.closeComplete, FALSE);

        process_watch_boolean(luLuP->c.closeComplete, ENABLE, RECURRING);

        /* take receive buffer away from luapi object now so that we don't
         * call luReceive Indication while the close is progressing
         */
        *bufferP = luLuP->c.rcvBuf;
        luLuP->c.rcvBuf = NULL;
        luLuP->c.rcvBufLen = 0;

        /* if we are holding a request pending a luReceive call then
         * send +ve rsp or discard it as appropriate
         */
        if (luLuP->qdBfrP) {
            SendPosRsp( luP, luLuP->qdBfrP);
            luLuP->qdBfrP = 0;
        }

        LuLuFsm( luP, luLuApiClose, NULL);

        /* we need to block this process to wait until the session is clean */
        process_wait_for_event();

        /* assume that we've been woken by the event we want - nothing else
         * is expected and we may as well continue clean up anyhow
         */

        process_watch_boolean( luLuP->c.closeComplete, DISABLE, FALSE);
        delete_watched_boolean( &(luLuP->c.closeComplete) );
    }
    process_pop_event_list(NULL);

    if (ret == Success) {
        /* we can now go ahead and delete the luLuP object and
         * then clean up all the control blocks if the sscpLuApi
         * is not in use
         */
        LuLuApi_dl( luP);
        if (!luP->luApiP->sscpLuP)
            LuApi_dl( &luP);
    }
    return (ret);
}


/*
 * luT0orT2Method structure
 */
static const LuApiMethod luT0orT2Method = {
   luT0orT2Send,
   luT0orT2Receive,
   luT0orT2ReceiveContinue,
   luT0orT2Close
};


/*
 * luT0orT2Open request
 *   Open function for LU type 0 or type 2 session.
 *   The BIND determines the LU type - either is accepted.
 */
LuApiReturnCode luT0orT2Open (
     void*            applLuH,    /* application LU Handle           */
     void**           luApiLuH,   /* luapi LU Handle (returned)      */
     char*            hostName,   /* host name on the "sna host" or
                                     "dspu host" command             */
     byte*            locaddr,    /* LU address on host name PU
                                     if passed 0, value returned     */
     uint             applNameLen,/* Length of Host application name */
     byte*            applName,   /* Host application name (EBCDIC)  */
     byte*            modeName,   /* Mode name (EBCDIC, 8 chars)     */
     uint             userDataLen,/* Length of user data             */
     byte*            userData,   /* User data for INITSELF RU       */
     TCloseIndication luCloseInd, /* close indication entry point    */
     hword*           sysSense,   /* system sense returned on failure*/
     hword*           userSense,  /* user sense returned on failure  */
     uint             bindDataLen,/* length of bindData buffer       */
     byte*            bindData,   /* buffer for return of BIND User
                                     data on successful open         */
     uint*            bindDataCnt /* length of BIND User data        */
    )
{
    Lu* luP;
    LuLuApi* luLuP;
    Pu* puP;
    boolean newLu = FALSE;
    LuApiReturnCode ret;
    byte findLoc;
    LuApiHandle * luHandleP;

    /* put defaults into return parameters */
    *sysSense = 0;
    *userSense = 0;
    *luApiLuH = NULL;
    *bindDataCnt = 0;

    /* find PU */
    ret = FindPuForLuApiUse(hostName, &puP);
    if (ret != Success)
        return (ret);
 
    luHandleP = LuApiHandle_nw(&luT0orT2Method);
    if (!luHandleP)
        return (InsufficientMemory);

    /* look for LU */
    luP = 0;
    if (*locaddr) {
        if (QueryLuConflict( puP, *locaddr, 1)) {
            /* case where lu cb already exists */
            luP = GetChild( GetLuChildren(puP), *locaddr);
            if ((luP->usage != luInUseByLuApi)  ||    /* not owned by LUAPI */
                (luP->luApiP->luLuP) ||           /* in use by another appl */
                (luP->luApiP->sscpLuP && luP->luApiP->sscpLuP->denyLuSession) )
                                           /* deny selected on sscp open */
                return (NoLuAvailable);
        } else {
            /* case where no lu cb exists */
            if (!PuLuMapIsLuActive(puP, *locaddr))
                if (!PuIsDddluSupported(puP))
                    return (NoLuAvailable);
        }
    } else {
        /* locaddr 0 means that lu api is supposed to figure out a locaddr */
        *locaddr = FindLocaddr(puP);
        if (*locaddr == 0) {
            /* search for an LUAPI LU, but not LU-LU session and not denied */
            findLoc = 0;
            do {
                findLoc++;
                luP = GetChild( GetLuChildren(puP), findLoc);
                if (luP)
                    if ( (luP->usage == luInUseByLuApi) &&
                         !(luP->luApiP->luLuP) &&
                         !(luP->luApiP->sscpLuP->denyLuSession) )
                        *locaddr = findLoc;
            } while ((*locaddr == 0) && (findLoc < 255));
            /* note: if *locaddr is non-zero, luP is now set up too */

           if (*locaddr == 0)
               return (NoLuAvailable);  /* none available on this PU */
        }
    }

    /* if we need to, create a new LU & LUAPI combined object */
    if (luP == 0) {
        luP = LuApi_nw(puP, *locaddr);
        if (!luP)
            return (InsufficientMemory);
        newLu = TRUE;
    }

    /* create LU-LU API object and add to LU object */
    luLuP = LuLuApi_nw( luP, applLuH, luCloseInd, luHandleP);
    if (!luLuP) {
        if (newLu)
            LuApi_dl( &luP);
        return (InsufficientMemory);
    }
   
    luP->luApiP->desiredState = ActAv;
    luLuP->openParmP->applNameLen = applNameLen;
    luLuP->openParmP->applName = applName;
    luLuP->openParmP->modeName = modeName;
    luLuP->openParmP->userDataLen = userDataLen;
    luLuP->openParmP->userData = userData;
    luLuP->openParmP->sysSense = sysSense;
    luLuP->openParmP->userSense = userSense;
    luLuP->openParmP->bindDataLen = bindDataLen;
    luLuP->openParmP->bindData = bindData;
    luLuP->openParmP->bindDataCnt = bindDataCnt;
    /* save host application names for show commands */
    ConvertEbcdicToAscii( applName, luLuP->hostApplName, 8);
    luLuP->hostApplName[8] = 0;

    if (!process_push_event_list(NULL))
        luLuP->c.openResult = InsufficientMemory;
     else {
        luLuP->c.openComplete = create_watched_boolean("DSPU LUAPI Open", 1);
        if (luLuP->c.openComplete == NULL)
            luLuP->c.openResult = InsufficientMemory;
        else {

            process_set_boolean(luLuP->c.openComplete, FALSE);
            process_watch_boolean( luLuP->c.openComplete, ENABLE, RECURRING);

            LuApiStartLu(luP);

            /* we need to block this process to wait until the session is up */
            process_wait_for_event();

            /* assume that we've been woken by the event we want 
             * - nothing else is expected
             */

            process_watch_boolean( luLuP->c.openComplete, DISABLE, FALSE);
            delete_watched_boolean( &(luLuP->c.openComplete) );
        }
        process_pop_event_list(NULL);
    }

    /* now we're either open or we failed for some reason */
    ret = luLuP->c.openResult;  /* save now - we may delete objects */
    if (ret == Success) {
        *luApiLuH = luHandleP;
    } else {
        LuLuApi_dl( luP);
        if (!luP->luApiP->sscpLuP)
            LuApi_dl( &luP);  
    }

    return (ret);
}



/*
 * sscpLuOpen request
 *   Open function for application to access SSCP-LU session.
 */
LuApiReturnCode sscpLuOpen (
     void*            applLuH,    /* application LU Handle           */
     void**           sscpLuApiH, /* sscp-lu api LU Handle (returned)*/
     char*            hostName,   /* host name on the "sna host" or
                                     "dspu host" command             */
     byte*            locaddr,    /* LU address on host name PU
                                     if passed 0, value returned     */
     boolean          denyLuSession, /* disallow luOpen while
                                        sscpLu api is open           */
     TCloseIndication sscpLuCloseInd /* close indication entry point */
    )
{
    Lu* luP;
    SscpLuApi* sscpLuP;
    boolean newLu = FALSE;
    LuApiReturnCode ret;
    Pu* puP;
    byte findLoc;

    /* find PU */
    ret = FindPuForLuApiUse(hostName, &puP);
    if (ret != Success)
        return (ret);
 
    /* look for LU */
    luP = 0;
    if (*locaddr) {
        if (QueryLuConflict( puP, *locaddr, 1)) {
            /* case where lu cb already exists */
            if (denyLuSession)
                return (NoLuAvailable);   /* deny if existing session */
            luP = GetChild( GetLuChildren(puP), *locaddr);
            if ( (luP->usage != luInUseByLuApi) ||  /* not owned by LUAPI */
                (luP->luApiP->sscpLuP) )        /* in use by another appl */
                return (NoLuAvailable);
        } else {
            /* case where no lu cb exists */
            if (!PuLuMapIsLuActive(puP, *locaddr))
                if (!PuIsDddluSupported(puP))
                    return (HostConnInactive);   /* inactive and can't act it */
        }

    } else {
        /* locaddr 0 means that lu api is supposed to figure out a locaddr */
        *locaddr = FindLocaddr(puP);
        if (*locaddr == 0) {
            if (denyLuSession)
                return (NoLuAvailable);  /* none available on this PU */

            /* if not denyLuSession, find an LUAPI LU, but not SSCP-LU sess */
            findLoc = 0;
            do {
                findLoc++;
                luP = GetChild( GetLuChildren(puP), findLoc);
                if (luP)
                    if ( (luP->usage == luInUseByLuApi) &&
                        !(luP->luApiP->sscpLuP) )
                        *locaddr = findLoc;
            } while ((*locaddr == 0) && (findLoc < 255));
            /* note: if *locaddr is non-zero, luP is now set up too */

            if (*locaddr == 0)
                return (NoLuAvailable);  /* none available on this PU */
        }
    }

    /* if we need to, create a new LU & LUAPI combined object */
    if (luP == 0) {
        luP = LuApi_nw(puP, *locaddr);
        if (!luP)
            return (InsufficientMemory);
        newLu = TRUE;
    }

    /* create SSCP-LU object and add to LU object */
    sscpLuP = SscpLuApi_nw( luP, applLuH, sscpLuCloseInd);
    if (!sscpLuP) {
        if (newLu)
            LuApi_dl( &luP);
        return (InsufficientMemory);
    }

    sscpLuP->denyLuSession = denyLuSession;

    if (!process_push_event_list(NULL))
        sscpLuP->c.openResult = InsufficientMemory;
    else {
        sscpLuP->c.openComplete =
                     create_watched_boolean("DSPU LUAPIs Open", 3);
        if (sscpLuP->c.openComplete == NULL)
            sscpLuP->c.openResult = InsufficientMemory;
        else {
            process_set_boolean(sscpLuP->c.openComplete, FALSE);
            process_watch_boolean(sscpLuP->c.openComplete, ENABLE, RECURRING);

            LuApiStartLu(luP);

            /* we need to block this process to wait until the session is up */
            process_wait_for_event();

            /* assume that we've been woken by the event we want 
             * - nothing else is expected
             */

            process_watch_boolean( sscpLuP->c.openComplete, DISABLE, FALSE);
            delete_watched_boolean( &(sscpLuP->c.openComplete) );
        }
        process_pop_event_list(NULL);
    }

    /* now we're either open or we failed for some reason */
    ret = sscpLuP->c.openResult;  /* save now - we may delete objects */
    if (sscpLuP->c.openResult == Success)
        *sscpLuApiH = luP;
    else {
        SscpLuApi_dl( luP);
        if (!luP->luApiP->luLuP)
            LuApi_dl( &luP);
    }

    return (ret);
}
                          
                          
/*
 * sscpLuSend request
 *   Send request on SSCP-LU session.
 */
LuApiReturnCode sscpLuSend (
     void*    sscpLuApiH,  /* sscp-lu api LU Handle    */
     uint     piuDataLen,  /* Length of PIU (TH,RH&RU) */
     byte*    piuData      /* PIU data                 */
    )
{
    boolean defreq;
    DataReq_t *clsiBfrP;
    Lu* luP = sscpLuApiH;
    SscpLuApi* sscpLuP = luP->luApiP->sscpLuP;
    if (!sscpLuP)
        return (InvalidHandle);

    /* Allocate a clsiBuffer, copy data into buffer. */
    clsiBfrP = BuildSscpLuDataReq( luP->upLuNumber,
                                   piuDataLen, piuData,
                                   &defreq);

    if (clsiBfrP) {
        if (defreq)
            SscpLuFsm( luP, ApiSendDefReq, clsiBfrP);
        else
            SscpLuFsm( luP, ApiSendOther, clsiBfrP);
    } else
        sscpLuP->c.sendResult = InsufficientMemory;

    return (sscpLuP->c.sendResult);
}
   
   
/*
 * sscpLuReceive request
 *   application request to receive data from SSCP-LU session.
 */
LuApiReturnCode sscpLuReceive (
     void*              sscpLuApiH,  /* sscp-lu api LU Handle       */
     uint               rcvBufLen,   /* Length of receive buffer    */
     byte*              rcvBuf,      /* receive buffer              */
     TReceiveIndication sscpLuRcvInd /* receive indication entry pt */
    )
{
    Lu* luP = sscpLuApiH;
    SscpLuApi* sscpLuP = luP->luApiP->sscpLuP;
    if (!sscpLuP)
        return (InvalidHandle);

    sscpLuP->c.rcvBufLen = rcvBufLen;
    sscpLuP->c.rcvBuf = rcvBuf;
    sscpLuP->c.rcvInd = sscpLuRcvInd;

    SscpLuFsm(luP, ApiReceive, NULL);

    return (Success);
}


/*
 * sscpLuReceiveContinue request
 *   receive request on SSCP-LU session.
 */
LuApiReturnCode sscpLuReceiveContinue (
     void*              sscpLuApiH,  /* sscp-lu api LU Handle       */
     uint               rcvBufLen,   /* Length of receive buffer    */
     byte*              rcvBuf       /* receive buffer              */
    )
{
    Lu* luP = sscpLuApiH;
    SscpLuApi* sscpLuP = luP->luApiP->sscpLuP;
    if (!sscpLuP)
        return (InvalidHandle);

    sscpLuP->c.rcvBufLen = rcvBufLen;
    sscpLuP->c.rcvBuf = rcvBuf;

    return (Success);
}


/*
 * sscpLuClose request
 *   Request close down of application access to SSCP-LU session.
 */
LuApiReturnCode sscpLuClose (
     void*  sscpLuApiH,  /* sscp-lu api LU Handle                    */
     byte** bufferP      /* outstanding rcv buffer, if any (returned)*/
    )
{
    LuApiReturnCode ret = Success;
    Lu* luP = sscpLuApiH;
    SscpLuApi* sscpLuP = luP->luApiP->sscpLuP;
    if (!sscpLuP)
        return (InvalidHandle);

    if (!process_push_event_list(NULL))
        return (InsufficientMemory);

    sscpLuP->c.closeComplete = create_watched_boolean("DSPU LUAPIs Close", 4);
    if (sscpLuP->c.closeComplete == NULL)
        ret = InsufficientMemory;
    else {

        sscpLuP->c.apiState = ApiClosing;
        process_set_boolean(sscpLuP->c.closeComplete, FALSE);
        process_watch_boolean( sscpLuP->c.closeComplete, ENABLE, RECURRING);

        /* take receive buffer away from luapi object now so that we don't
         * call sscpLuReceive Indication while the close is progressing
         */
        *bufferP = sscpLuP->c.rcvBuf;
        sscpLuP->c.rcvBuf = NULL;
        sscpLuP->c.rcvBufLen = 0;

        /* if we are holding a request pending a sscpLuReceive call then
         * send +ve rsp or discard it as appropriate
         */
        if (sscpLuP->qdReqP) {
            SendPosRsp( luP, sscpLuP->qdReqP);
            sscpLuP->qdReqP = 0;
        }

        SscpLuFsm( luP, ApiClose, NULL);
   
        /* we need to block this process to wait until the session is clean */
        process_wait_for_event();

        /* assume that we've been woken by the event we want - nothing else
         * is expected and we may as well continue clean up anyhow
         */

        process_watch_boolean( sscpLuP->c.closeComplete, DISABLE, FALSE);
        delete_watched_boolean( &(sscpLuP->c.closeComplete) );
    }

    process_pop_event_list(NULL);

    if (ret == Success) {
        /* we can now go ahead and delete the sscpLuP object and
         * then clean up all the control blocks if the luLuApi
         * is not in use
         */
        SscpLuApi_dl( luP);
        if (!luP->luApiP->luLuP)
            LuApi_dl( &luP);
    }
    return (ret);
}


/*
 * LuApiShowLu
 *   This displays the LU details.
 */
void LuApiShowLu (Lu* this)
{
    if (this->luApiP->luLuP) {
        printf(   "     LU %03d API LU, HOST APPLICATION %s\n",
               this->upLuNumber, this->luApiP->luLuP->hostApplName);
        if (this->luApiP->sscpLuP) {
            printf("            API LU, SSCP-LU SESSION API\n");
        }
    } else if (this->luApiP->sscpLuP) {
        printf(   "     LU %03d API LU, SSCP-LU SESSION API\n",
             this->upLuNumber);
    }

    printf(        "            SSCP-LU STATUS %s LU-LU STATUS %s\n",
      sscpLuFsmStates[this->luApiP->sscpLuState],
      luLuFsmStates[this->luApiP->luLuState]);

}


/*
 * LuApiQueryLuActive
 *   Returns TRUE if the LU is active.
 */
boolean LuApiQueryLuActive (Lu* this)
{
    return ((this->luApiP->sscpLuState != LuApi_Reset) &&
            (this->luApiP->sscpLuState != PendActlu) );
}


/*
 * LuApiQueryLuBound
 *   Returns TRUE if the LU is bound.
 */
boolean LuApiQueryLuBound (Lu* this)
{
    return (this->luApiP->luLuState != LuLuReset);
}
