
fr_vcq.h,1161
enum fr_vcq_type fr_vcq_type33,975
    FR_VCQ_CUSTOM,34,994
    FR_VCQ_PRIORITY,35,1013
    FR_VCQ_FCFS36,1034
#define FR_VCQ_HOLDQ 39,1054
#define VCQ_DEFAULT_QUEUE(VCQ_DEFAULT_QUEUE41,1079
#define VCQ_PRIORITY_QUEUE_INC(VCQ_PRIORITY_QUEUE_INC42,1161
#define FR_PVC_INC(FR_PVC_INC44,1241
#define FR_PVC_DEC(FR_PVC_DEC45,1307
#define FOR_ALL_IDB_VC(FOR_ALL_IDB_VC48,1375
#define FOR_ALL_VCQ_VC(FOR_ALL_VCQ_VC53,1522
typedef boolean (*fr_oqueue_t)fr_oqueue_t61,1769
typedef boolean (*fr_oqueue_t)(fr_pvc_info_type *, paktype *, enum HEADTAIL)HEADTAIL61,1769
typedef paktype * (*fr_oqueue_dequeue_t)fr_oqueue_dequeue_t62,1847
typedef paktype * (*fr_oqueue_dequeue_t)(fr_pvc_info_type fr_pvc_info_type62,1847
struct VCQ VCQ64,1914
struct fr_idb_vcq fr_idb_vcq80,2765
#define idb_vcq 86,2867
VCQ_PRIORITY_ARRAY 94,3078
static inline void vcq_delete 106,3301
static inline struct VCQ *pvc_find_vcq 114,3451
static inline struct VCQ *mapclass_find_vcq 127,3774
static inline struct fr_vcq_params *get_vcq_params 143,4069
static inline struct fr_vcq_params *vc_vcq_params 152,4267
static inline boolean is_vcq_configured 169,4700
