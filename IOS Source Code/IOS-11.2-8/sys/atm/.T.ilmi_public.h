
ilmi_public.h,1850
#define _ILMI_PUBLIC_H_ 65,2254
#define ILMI_ERROR 71,2311
#define ILMI_OK 72,2338
#define ILMI_NETPREFIX_LEN 78,2400
#define ILMI_ATMADDRESS_LEN 79,2435
#define CLIENTNAMESIZE 80,2470
typedef void (* ilmiNotifyCB)ilmiNotifyCB83,2507
typedef int ilmiClient;ilmiClient84,2546
typedef int ilmiErrcode;ilmiErrcode85,2570
typedef uint ilmiIntf;ilmiIntf86,2595
typedef ushort ilmiOper;ilmiOper87,2618
typedef ushort ilmiReqID;ilmiReqID88,2643
typedef short ilmiEvContext;ilmiEvContext89,2669
typedef char * ilmiLabel;ilmiLabel90,2698
typedef enum ilmiClientNotifyType_ ilmiClientNotifyType_93,2726
}ilmiClientNotifyType;ilmiClientNotifyType101,2900
typedef enum ilmiFunctionType_ ilmiFunctionType_103,2924
}ilmiFunctionType;ilmiFunctionType110,3063
typedef enum ilmiOperType_ ilmiOperType_112,3083
}ilmiOperType;ilmiOperType118,3265
typedef enum ilmiTrapType_ ilmiTrapType_120,3281
}ilmiTrapType;ilmiTrapType123,3341
typedef enum ilmiVCCChangeTrapType_ ilmiVCCChangeTrapType_125,3357
}ilmiVCCChangeTrapType;ilmiVCCChangeTrapType132,3504
typedef enum ilmiAddrState_ ilmiAddrState_134,3529
}ilmiAddrState;ilmiAddrState137,3606
 typedef enum ilmiAddrType_{ilmiAddrType_143,3658
 }ilmiAddrType;ilmiAddrType146,3726
typedef enum ilmiAddressTestType_ ilmiAddressTestType_148,3743
}ilmiAddressTestType;ilmiAddressTestType152,3863
typedef enum ilmiErrorType_ ilmiErrorType_158,3918
} ilmiErrorType;ilmiErrorType173,4212
typedef enum ilmiResponseType_ ilmiResponseType_178,4287
}ilmiResponseType;ilmiResponseType185,4440
typedef enum ilmiIntfState_ ilmiIntfState_190,4531
 }ilmiIntfState;ilmiIntfState195,4648
 typedef enum ilmiEventType_ ilmiEventType_197,4666
 }ilmiEventType;ilmiEventType202,4794
typedef struct ilmiClientReq_ ilmiClientReq_215,5088
}ilmiClientReq;ilmiClientReq237,6391
