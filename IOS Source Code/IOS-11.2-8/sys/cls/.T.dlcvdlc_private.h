
dlcvdlc_private.h,3023
#define __DLCVDLC_PRIVATE_H__118,4521
#define VDLCBGLIST 128,4783
typedef enum vdlc_events_ vdlc_events_132,4856
} vdlc_events;vdlc_events134,4905
} VdlcStateT;VdlcStateT166,5850
} VCepEventT;VCepEventT198,6552
    VDLC_TIMER205,6603
#define VDLC_DEFAULT_OPEN_TIMER 212,6650
#define VDLC_BUFFER_WAIT	213,6697
#define VDLC_DEFAULT_MAX_RETRIES	214,6732
#define VDLC_DEFAULT_MAXMTU	216,6788
} VdlcPriorityT;VdlcPriorityT220,6873
typedef boolean VPortWalkT(VPortWalkT222,6891
typedef boolean VSapWalkT(VSapWalkT223,6949
typedef boolean VCepWalkT(VCepWalkT224,7004
struct VrgT_ VrgT_255,8062
#define MAX_VDLCNAME	288,9427
struct VPortT_ VPortT_289,9486
struct VSapT_ VSapT_327,11084
struct VCepT_ VCepT_374,13059
static INLINE VPortT *VrgGetDefaultPort 468,17343
static inline void VrgRemovePort 473,17431
static INLINE VPortT *VrgLookupPortByVmac 479,17546
static INLINE void VPortSetVmac(496,17987
static INLINE uchar *VPortGetVmac 501,18085
static INLINE boolean VPortAddSap 511,18215
static INLINE void VPortRemoveSap 517,18367
static INLINE VSapT *VPortLookupSapByValue 522,18483
static inline VSapT *VportLookupSapByUSapId 530,18707
static inline VPortT *VPortGetNext 538,18932
static INLINE idbtype *VPortGetIdb 544,19060
static INLINE void VPortSetVrg 549,19154
static INLINE VrgT *VPortGetVrg 555,19239
static inline void VSapModify 567,19538
static INLINE void VSapSetPort 573,19647
static INLINE VPortT *VSapGetPort 578,19733
static INLINE boolean VSapAddCep 583,19821
static INLINE boolean VSapRemoveCep 591,20004
static INLINE VCepT *VSapLookupCep 599,20190
static inline VSapT *VSapGetNext 608,20412
static INLINE void VSapSetSapValue 613,20530
static INLINE uchar VSapGetSapValue 618,20624
static INLINE VCepT *VSapGetFirstCep 623,20719
static INLINE ClsUSapIdT VSapGetUSapId 629,20845
static INLINE void VSapSetUSapId 634,20941
static INLINE void VSapSetPriority 639,21038
static inline void  VCepModify 651,21355
static INLINE VdlcStateT VCepGetState 656,21464
static INLINE VSapT *VCepGetSap 661,21546
static INLINE void VCepSetSap 666,21620
static INLINE VCepT *VCepGetPartner 671,21701
static INLINE void VCepSetPartner 676,21783
static INLINE llctype *VCepGetLlc 682,21920
static inline boolean VCepSetLlc 687,21996
static inline void VCepClearLlc 694,22135
static INLINE void VCepBgEnqueue 702,22255
static INLINE void VCepBgRemove 713,22527
static INLINE paktype *VCepGetRxPacket 723,22729
static INLINE CepT *VCepGetDlcCep 728,22825
static INLINE void VCepSetDteString 733,22902
static INLINE uchar *VCepGetDteString 739,23096
static INLINE ClsUCepIdT VCepGetUCepId 744,23182
static INLINE list_header *VCepPriorityToList 755,23418
static INLINE boolean VCepRxQueueEmpty 760,23507
static INLINE void VCepSetPriority 765,23604
static INLINE void VCepSetUCepId(771,23710
#define VDLC_DEFAULT_HOLDQ	781,23894
#define VDLC_TXQ_LOW_WATER	782,23969
#define VDLC_TXQ_HIGH_WATER	783,24016
#define VDLC_CLS_FLOW_IND_DAMP	784,24064
