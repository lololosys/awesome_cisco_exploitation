/* $Id: ncia.h,v 3.1.2.6 1996/07/11 21:13:41 jolee Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia.h,v $
 *------------------------------------------------------------------
 * ncia.h - Definitions for Native Client Interface Architecture
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia.h,v $
 * Revision 3.1.2.6  1996/07/11  21:13:41  jolee
 * CSCdi62553:  Move externs from .c to .h that was lost in CSCdi61974
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/07/03  06:32:04  jolee
 * CSCdi61974:  Fix NCIA crash if unconfig a client in active open
 * processing
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/18  19:01:19  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/12  06:51:43  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/28  22:33:47  schiang
 * CSCdi58852:  router crashes when ncia server is un-configured
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.6  1996/05/14  18:42:11  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.5  1996/05/14  04:31:49  jolee
 * CSCdi57570:  Added clear client support, removed backup mac suppport
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.4  1996/05/03  22:06:18  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.3  1996/04/26  15:16:42  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/04/24  06:28:25  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.1.4.1  1996/04/05  06:05:40  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:13:58  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#define NCIA_PROTO_VERSION          1
#define NCIA_HASH_SIZE              32     /* must be power of 2 */
#define NCIA_HASH_MASK              (NCIA_HASH_SIZE - 1)
#define NCIA_TCP_WINDOW_SIZE        100
#define NCIA_TCP_MAX_QUEUE_DEFAULT  100
#define NCIA_LOCAL_RING             10
#define NCIA_BRIDGE                 1
#define NCIA_TARGET_RING            20
#define NCIA_SAP_IN_USE_BY_BRIDGE   0xff

#define NCIA_GENERAL_TIMEOUT        (15 * ONESEC)

/* Flagsd */
#define NCIA_SERVER_CONFIGURED      0x00000001  /* ncia is configured         */
#define NCIA_SERVER_ACTIVE          0x00000002  /* enable.req is received     */
#define NCIA_SERVER_STOP            0x00000004  /* ncia is stopped            */
#define NCIA_INBOUND_ONLY           0x00000008  /* server doesn't connect out */


#define MAX_TIMEOUT        10
#define NCIA_MAX_SAP        3
#define NCIA_DLSW_SAP       0

#define NCIA_DEFAULT_NDLC_KEEPALIVE 30        /* SEC */
#define NCIA_DEFAULT_TCP_KEEPALIVE  20        /* MIN */

#define  NCIA_SNA_ONLY      1
#define  NCIA_SNA_NETBIOS   2


STRUCTURE_REF(NciaSapT);
STRUCTURE_REF(NciaCepT);



struct NciaSapT_ {
    word             fUSapId;
    word             fPSapId;
    NciaCepT        *fNciaCep;
    ncia_servertype *fNciaGroup;
    byte             fSapValue;
} ;



struct NciaCepT_ {
    NciaCepT     *fnext;                    /* in peer control block */
    NciaCepT     *fNextCep;                 /* in CEP list */
    NciaSapT     *fPSap;

    nciapeertype *fNciaClient;
    word          fUCepId;
    word          fPCepId;

    ulong         fClientCktId;
    uchar         fTargetAddr[IEEEBYTES];
    uchar         fTargetSap;               /* Partner Sap */
    uchar         fClientSap;               /* Client Sap */

    long          fState;

    ulong         fNciaCepFlags;

    paktype      *fPacket;
    paktype      *fXid;
    mgd_timer     fTimer;
    queuetype     fInputQ;                  /* store packets from client */
    queuetype     fOutputQ;                 /* store packets from DLU */

    ushort        fInitWindowSize;          /* for sender */
    short         fGrantPacket;
    ushort        fCurrentWindow;

    ushort        fClientInitWindowSize;    /* for receiver */
    short         fClientGrantPacket;       /* # of packets a client can send */
    ushort        fClientCurrentWindow;
    uchar         fFCACKOwed;
    uchar         fFCFlag;

    uchar         fLFSize;
} ;



/* for NCIA Server */
struct ncia_servertype_ {
    ncia_servertype *fnext;
    ipaddrtype       ipaddr;                /* tcpdriver and udp */
    uchar            server_mac[IEEEBYTES];
    uchar            vmac_start[IEEEBYTES]; /* bottom virtual Mac Address */
    ushort           vmac_range;
    idbtype         *ncia_swidb;
    ulong            flags;
    ulong            tcp_keepalive_int;
    ulong            ndlc_keepalive_int;
    ushort           index;
    ushort           ncia_free_client_index;
    ushort          *vmac_list;
    ushort           hash_tbl_element[NCIA_HASH_SIZE];
    queuetype        hash_tbl[NCIA_HASH_SIZE];
    NciaSapT         sap_registry[NCIA_MAX_SAP];
    uchar            enable_count;
    uchar            name[6];
    ushort           num_netbios_stn;
};


#define NCIA_RSRB_UP        0x01

/* for RSRB */
typedef struct ncia_rsrb_ {
    struct ncia_rsrb_ *fnext;
    ushort            rsrb_target_ring;
    uchar             rsrb_bridge;
    ushort            rsrb_local_ring;
    uchar             dlsw_bridge;
    ushort            dlsw_ring;
    uchar             rsrb_mac[IEEEBYTES];
    ushort            flag;
} ncia_rsrb_t;



typedef enum nciaOpenFailureCode_
{
    NciaInUse            = -1,
    NciaNoMem            = -2,
    NciaNoRemoteAddress  = -3,
} nciaOpenFailureCode;


enum NciaTimerType {
    NCIA_CIRCUIT_TIMER,
    NCIA_CLIENT_TIMER
};



/* Circuit related flag */

#define NciaCircuitDebugEVENT   0x00000002
#define NciaCircuitDebugSTATE   0x00000004
#define NciaCircuitDebugFLOW    0x00000008
#define NciaCircuitDebugERROR   0x00000010

#define NciaCircuitDebugALL     (NciaCircuitDebugEVENT | NciaCircuitDebugSTATE \
                                 | NciaCircuitDebugFLOW | NciaCircuitDebugERROR)

#define NciaUsedBySAP           0x00000020
#define NciaTestRspPending      0x00000040

#define NciaXidRspPending       0x00000100
#define NciaClientBusy          0x00000200      /* can't send packets to client
                                                   due to flow control */
#define NciaClsBusy             0x00000400      /* can't send packets to DLU */
#define NciaTcpBusy             0x00000800      /* can't send packets to client
                                                   due to tcp congetion */
#define NciaSendFCA             0x00001000      /* need to send FCA to client */
#define NciaFreeCkt             0x00002000      /* need to free ncia ckt ctl 
                                                   blk once ReqOpnStn.Cnf (fail)
                                                   is sent to DLU */


extern void *ncia_next_item;
extern int   ncia_hash_index;

#define NEXT_SERVER  ((ncia_servertype *) ncia_next_item)
#define NEXT_CLIENT  ((nciapeertype *) ncia_next_item)  
#define NEXT_CIRCUIT ((NciaCepT *) ncia_next_item)


#define ForEachNciaServer(group, server) \
    for ((server) = (group).qhead, \
        NEXT_SERVER = (server) ? (server)->fnext : NULL; \
        server; \
        server = NEXT_SERVER, \
        NEXT_SERVER = (server) ? (server)->fnext : NULL)

#define ForEachNciaClientHashQueue \
 for (ncia_hash_index = 0; ncia_hash_index < NCIA_HASH_SIZE; \
                          ncia_hash_index++)

#define ForEachNciaClient(server, client) \
 ForEachNciaClientHashQueue \
     for ((client) = (server)->hash_tbl[ncia_hash_index].qhead, \
         NEXT_CLIENT = (client) ? (client)->fnext : NULL;\
         client; \
         client = NEXT_CLIENT, \
         NEXT_CLIENT = (client) ? (client)->fnext : NULL)

#define ForEachNciaCircuit(client, circuit) \
    for ((circuit) = (client)->NciaCircuit.qhead, \
        NEXT_CIRCUIT = (circuit) ? (circuit)->fnext : NULL;  \
        circuit; \
        circuit = NEXT_CIRCUIT, \
        NEXT_CIRCUIT = (circuit) ? (circuit)->fnext : NULL)

#define NCIA_FCI      0x80
#define NCIA_FCA      0x40
#define NCIA_FCO_MASK 0x07


#define NciaRepeatWindowOp    0x00
#define NciaIncrementWindowOp 0x01
#define NciaDecrementWindowOp 0x02
#define NciaResetWindowOp     0x03
#define NciaHalveWindowOp     0x04

/* we give each client at least 20 granted packet */
#define NCIA_MAX_GRANTED_PACKET_SIZE 20
#define NCIA_INIT_WINDOW_SIZE        1
#define NCIA_MAX_WINDOW              20


#define IEEE_LFMASK       0x70
#define IEEE_LF516        0x00
#define IEEE_LF1470       0x10
#define IEEE_LF2052       0x20
#define IEEE_LF4399       0x30
#define IEEE_LF8130       0x40
#define IEEE_LF11407      0x50
#define IEEE_LF17749      0x60
#define IEEE_LFINITIAL    0x70
 
#define IEEE_LF516_VAL    516
#define IEEE_LF1470_VAL   1470
#define IEEE_LF2052_VAL   2052
#define IEEE_LF4399_VAL   4399
#define IEEE_LF8130_VAL   8130
#define IEEE_LF11407_VAL  11407
#define IEEE_LF17749_VAL  17749
#define IEEE_LF_INFINITY  65535


/* # define MAXETHERSIZE 1500               largest 10MB packet, w/o encaps */

#define NCIA_MTU        1500


/* NCIA command commands */

#define NCIA_RSRB               1
#define CONFIG_NCIA_SERVER      2
#define CONFIG_NCIA_CLIENT      3

/* NCIA debug commands */

#define DEBUG_NCIA              1


#define EXEC_NCIA_STOP          1
#define EXEC_NCIA_START         2


#define NDLC_DATA ndlc_packet->ndlc_data

#define NCIA_DONT_CARE          0


#define NCIA_FREE_PAK(clsBlock)        if (clsBlock->fPakPtr) {          \
                                datagram_done(clsBlock->fPakPtr);        \
                                clsBlock->fPakPtr = NULL; }

/*
 * check if ncia server is stopped or not
 */
#define NCIA_CHECK_SERVER_IS_UP(ncia_server, clsBlock)  \
        if (ncia_server && (ncia_server->flags & NCIA_SERVER_STOP || \
          !(ncia_server->flags & NCIA_SERVER_ACTIVE))) { \
            NciaServerDebugAll("\nNCIA: server is not up!"); \
            NCIA_FREE_PAK(clsBlock); \
            return; }


static
inline int ncia_print_frame_size (int code)
{
    switch(code) {
    case IEEE_LF516:        return(IEEE_LF516_VAL);
    case IEEE_LF1470:       return(IEEE_LF1470_VAL);
    case IEEE_LF2052:       return(IEEE_LF2052_VAL);
    case IEEE_LF4399:       return(IEEE_LF4399_VAL);
    case IEEE_LF8130:       return(IEEE_LF8130_VAL);
    case IEEE_LF11407:      return(IEEE_LF11407_VAL);
    case IEEE_LF17749:      return(IEEE_LF17749_VAL);
    case IEEE_LFINITIAL:    return(IEEE_LF_INFINITY);
    default:                return(0);
    }
}



/* ncia.c */
extern queuetype ncia_group;
extern mgd_timer ncia_master_timer;
extern boolean   ncia_running;

extern NciaCepT *ncia_cep_search_by_ucepid(ClsUCepIdT uCepId);
extern void ncia_send_ndlc_no_ckt(nciapeertype *ncia_client, 
                           ClsDlcParamBlockT *clsBlock, ClsOpcodeT opCode);
extern NciaCepT *ncia_get_cep_from_ndlc(nciapeertype *ncia_client, 
                                        paktype *pak);
extern void ncia_drain_client_outputq(nciapeertype *ncia_client);
extern void ncia_drain_circuit_outQ(NciaCepT *ncia_ptr);
extern boolean ncia_verify_circuit(ulong circuit);
extern nciapeertype *ncia_verify_client(ipaddrtype client_ipaddr);
extern void ncia_insert_cep(NciaCepT *ncia_circuit);
extern void ncia_remove_cep(NciaCepT *ncia_circuit);
extern void ncia_command(parseinfo *csb);
extern ncia_servertype *ncia_get_server_by_index(int server_num);
extern nciapeertype *ncia_find_client_by_mac(idbtype *swidb, uchar *addr);
extern NciaCepT *ncia_find_circuit(nciapeertype *ncia_client, uchar *SrcAddr, 
                            uchar SrcSap, uchar DestSap);
extern NciaCepT *ncia_circuit_create(nciapeertype *ncia_client, uchar *SrcAddr, 
                              uchar SrcSap, uchar DestSap, NciaSapT *NciaSap);
extern boolean ncia_check_client_sap(nciapeertype *NciaClient, uchar Sap);
extern boolean ncia_cep_find_opened(NciaSapT *NciaSap, ClsUCepIdT uCepId);
extern ncia_servertype *ncia_idb_to_server(idbtype *swidb);




/* dlcncia.c */
extern void ncia_wput(DlcPCepIdT pCepId, DlcOpcodeT opcode, 
               ClsDlcParamBlockT *clsBlock);
extern void NciaSendUControlStnInd(ClsDlcParamBlockT *clsBlock, paktype *pak, 
                                   ClsUSapIdT uSap);
extern void NciaSetupUControlStnInd(uchar dsap, uchar ssap, uchar *dmac,
                                    uchar *smac, uchar control, paktype *pak);
extern void ncia_cls_notify(NciaCepT *pCepId, ClsOpcodeT opcode, 
                               ClsErrorCodeT error, paktype *pak);

extern void nciaClearCircuit(nciapeertype *ncia_client, NciaCepT *pCepId);
extern void nciaClearCircuitOnly(nciapeertype *ncia_client, NciaCepT *pCepId);

