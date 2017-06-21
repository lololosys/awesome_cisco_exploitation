
if_controller.h,760
#define __IF_CONTROLLER_H__61,2139
#define SLOT_BASED_CDB_PLATFORM 69,2291
enum pif_types_ pif_types_82,2687
    PIFT_UNKNOWN,83,2739
    PIFT_ETHERNET,84,2757
    PIFT_TOKENRING,85,2776
    PIFT_SERIAL,86,2796
    PIFT_HSSI,87,2814
    PIFT_FDDI,88,2829
    PIFT_ATM,89,2844
    PIFT_T1,90,2858
    PIFT_CHANNEL,91,2871
    PIFT_E1,92,2889
    PIFT_FASTETHER,93,2902
#define CDB_CONFIGURE	99,2965
#define	CDB_NOHARDWARE	100,3028
#define	CDB_CRASHED	101,3095
#define CDB_EOIR_RESET	102,3150
struct cdbtype_ cdbtype_104,3219
#define FOR_ALL_CDBS(FOR_ALL_CDBS165,5068
static inline uint controller_index_inline 189,5941
static inline hwidbtype* controller_find_from_cdb 199,6151
static inline cdbtype* controller_find_from_idb 212,6478
