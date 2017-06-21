
rvip_dma.h,1799
#define RVIP_MIN_DMA_LENGTH 43,1272
#define RVIP_MAX_DMA_LENGTH	44,1304
#define RVIP_MAX_DMA_LEN_SHIFT	45,1338
#define RVIP_MAX_LOCK_SPIN	46,1373
#define RVIP_DMA_ENTRIES	47,1406
#define RVIP_DMA_USEC_TIME	48,1437
#define RVIP_DMA_FORCE_THRESH	49,1469
#define RVIP_DMA_OUTPUTQ_LIMIT	50,1503
#define RVIP_DMA_POLL_POOL	52,1539
#define RVIP_DMA_BUFFERS	54,1591
#define RVIP_DMA_FROM_MEMD	59,1681
#define RVIP_DMA_EOP	60,1737
#define RVIP_FREE_DMA_ENTRY	61,1782
#define RVIP_REQUEST_DMA_ENTRY	62,1834
#define RVIP_DONE_DMA_ENTRY	63,1892
#define RVIP_MANUAL_DMA_ENTRY	64,1944
#define RVIP_SCATTERED_DMA	65,1997
#define RVIP_DMA_DFS	66,2046
#define RVIP_DMA_COMPLETE	68,2092
#define RVIP_DMA_DIRECTION_TX 73,2197
#define RVIP_DMA_DIRECTION_RX 74,2260
#define RVIP_DMA_DISABLE	75,2321
#define RVIP_DMA_INT_ENABLE 76,2376
#define RVIP_DMA_LENGTH_SHIFT 78,2438
#define DMA_ENTRY_OFS_NEXT	83,2535
#define DMA_ENTRY_OFS_LENGTH	84,2565
#define DMA_ENTRY_OFS_STATUS	85,2597
#define DMA_ENTRY_OFS_TOT_LENGTH	86,2629
#define DMA_ENTRY_OFS_ENTRY_DATA	87,2664
#define DMA_ENTRY_OFS_IFCNTL	88,2700
#define DMA_ENTRY_OFS_BHDR	89,2733
#define DMA_ENTRY_OFS_MEMD_ADDR	90,2764
#define DMA_ENTRY_OFS_PCI_ADDR	91,2800
} dma_entry_t;dma_entry_t117,3541
typedef struct rvip_dma_poll_t_ rvip_dma_poll_t;rvip_dma_poll_t122,3633
struct rvip_dma_poll_t_ rvip_dma_poll_t_124,3683
typedef struct dma_cntl_t_ dma_cntl_t_146,4081
} dma_cntl_t;dma_cntl_t186,4857
static inline boolean vip_acquire_global_lock 211,5476
static inline void vip_clear_global_lock_request 227,5760
static inline boolean dma_incr_request_count 236,5972
static inline dma_entry_t *dma_get_done_blocks 245,6175
static inline void dma_mark_free 264,6545
static inline dma_entry_t *dma_get_free_blocks 276,6800
