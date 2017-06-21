
svip_dma.h,1139
#define SVIP_DMA_BUFFERS	49,1546
#define SVIP_POLL_SLOP 50,1578
#define SVIP_MIN_DRQ_CLEAR 51,1620
typedef struct drq_entry_t_ drq_entry_t_55,1684
} drq_entry_t;drq_entry_t58,1780
#define DRQ_DMA_OWN	60,1796
#define DRQ_DMA_EOP	61,1859
#define DRQ_DMA_TRANSFER 62,1913
#define DRQ_STATUS_MASK 63,1979
#define DRQ_QUEUE_SHIFT 64,2043
#define DRQ_SIZE 66,2124
#define DRQ_ADVANCE_RING_INDEX(DRQ_ADVANCE_RING_INDEX68,2198
typedef struct dtq_struct0_ dtq_struct0_70,2260
} dtq_struct0_t;dtq_struct0_t74,2462
typedef struct dtq_entry_t_ dtq_entry_t_76,2480
} dtq_entry_t;dtq_entry_t82,2591
#define dtq_word0 83,2606
#define dtq_struct0 84,2637
#define dtq_word1 85,2665
#define DTQ_DMA_OWN	87,2694
#define DTQ_DMA_EOP	88,2757
#define DTQ_INTFC_MASK 89,2811
#define DTQ_INTFC_SHIFT 90,2878
#define DTQ_INTFC_STW_SHIFT 91,2952
#define DTQ_LENGTH_MASK 92,3031
#define DTQ_SIZE 94,3096
#define DTQ_ADVANCE_RING_INDEX(DTQ_ADVANCE_RING_INDEX95,3169
#define VIPER2QE_PCIADDR(VIPER2QE_PCIADDR97,3231
static inline boolean vip_acquire_global_lock 115,3751
static inline void vip_clear_global_lock_request 124,3895
