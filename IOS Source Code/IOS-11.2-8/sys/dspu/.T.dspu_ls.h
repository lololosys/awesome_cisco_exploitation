
dspu_ls.h,924
#define __LS_H42,1360
#define WORK_PU 54,1570
#define WORK_XIDNULL_REQ 55,1625
#define WORK_XIDNULL_RSP 56,1691
#define WORK_ROSR_COUT 57,1757
#define WORK_ROSR_CIN 58,1831
#define WORK_XID3_REQ 59,1904
#define WORK_XID3_RSP 60,1935
#define WORK_XID3_REQ2 62,2025
typedef hword LsWorkType;LsWorkType65,2057
typedef enum LsReqState LsReqState67,2084
        LsRS_Connected, LsRS_Closed} LsReqState;LsReqState68,2110
typedef enum LsState LsState70,2160
        } LsState;LsState74,2408
typedef enum LsInput LsInput76,2428
        } LsInput;LsInput84,2933
  struct LinkStn LinkStn88,3005
#define GetPuChildren(GetPuChildren217,8005
#define LsChild(LsChild220,8138
#define LsGetSap(LsGetSap224,8259
#define LsGetAddrP(LsGetAddrP227,8337
#define LsGetRemoteSapAddr(LsGetRemoteSapAddr230,8424
#define LsGetLocalSapAddr(LsGetLocalSapAddr233,8513
#define LsQueryFlowOff(LsQueryFlowOff236,8616
