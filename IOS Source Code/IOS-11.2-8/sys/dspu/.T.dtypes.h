
dtypes.h,1932
#define __DTYPES_H79,2903
#define NULL 89,3084
            }boolean;boolean95,3170
typedef struct Dspu Dspu;Dspu100,3270
typedef struct Cps Cps;Cps101,3346
typedef struct Sap Sap;Sap102,3414
typedef struct Adaptor Adaptor;Adaptor103,3482
typedef struct LinkStn LinkStn;LinkStn104,3556
typedef struct Pu Pu;Pu105,3616
typedef struct Lu Lu;Lu106,3677
typedef struct Itimer Itimer;Itimer107,3737
typedef struct WrkBBlk WrkBBlk;WrkBBlk108,3767
typedef struct PoolCB  PoolCB;PoolCB109,3799
typedef struct AdaptorId AdaptorId;AdaptorId110,3864
typedef struct PoolRange PoolRange;PoolRange111,3900
typedef struct ConnParmsT  ConnParmsT;ConnParmsT112,3936
typedef struct RetryInfoT  RetryInfoT;RetryInfoT113,4011
        } LuPooledE;LuPooledE117,4141
#define offsetof(offsetof120,4180
typedef union Xid Xid123,4255
        } Xid;Xid126,4383
typedef union Xid XidU;XidU127,4398
typedef struct  MacAddrT{MacAddrT129,4477
        } MacAddrT;MacAddrT131,4635
typedef struct  PuNameT PuNameT134,4657
        } PuNameT;PuNameT136,4723
typedef enum XMS_SearchModeE XMS_SearchModeE138,4743
      } XMS_SearchModeE;XMS_SearchModeE141,4890
typedef struct XidAddrSapT XidAddrSapT143,4916
        } XidAddrSapT;XidAddrSapT149,5065
             FlwCtl_None, FlwCtl_Stop, FlwCtl_Resume} FlowControlE;FlowControlE152,5104
typedef enum ConnectStatus ConnectStatus163,5485
}ConnectStatus;ConnectStatus167,5696
typedef enum LinkType LinkType171,5755
        } LinkType;LinkType174,5898
        } DspuDLCType_e;DspuDLCType_e191,6418
typedef enum WorkResult WorkResult193,6444
        } WorkResult;WorkResult197,6546
typedef enum CallDirection CallDirection199,6569
             CallDir_In, CallDir_Out} CallDirection;CallDirection200,6598
            }BlockingType;BlockingType205,6716
#define UnitIndex 208,6745
#define PortIndex 209,6767
struct AdaptorId AdaptorId211,6790
