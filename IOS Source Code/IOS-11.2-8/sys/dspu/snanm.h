/* network management nmvt vector */

typedef struct MV MV;
typedef struct Fp_ Fp;                      /* focal point */
typedef struct X25_EventData_ X25_EventData;

typedef enum  {
   MV_SV_OK = 0,
   MV_SV_NoFP = 0x8000,
   MV_SV_Overflow = 0x8001,
   MV_SV_NoMem
} MV_SV_RC;
 
typedef enum {
   MV_OpRcv_OK = 0,
   MV_OpRcv_Duplicate = 0x8000,
   MV_OpRcv_Invalid   = 0x8001,
   MV_OpRcv_NoMem     = 0x8002
} MV_OpRcv_RC;
 
