/* $Id: if_cip.h,v 3.6.8.5 1996/08/26 18:24:52 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_cip.h,v $
 *------------------------------------------------------------------
 * if_cip.h:  CIP specific constants & definitions
 *
 * February 1994, Stig Thormodsrud
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: if_cip.h,v $
 * Revision 3.6.8.5  1996/08/26  18:24:52  kmitchel
 * CSCdi66467:  CIP love letter stats not displayed after 49 days of uptime
 * Branch: California_branch
 *
 * Revision 3.6.8.4  1996/08/07  08:58:49  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.8.3  1996/06/02  15:31:49  motto
 * CSCdi50208:  Issuing csna, llc or tcp CIP show cmd immed after CIP
 * reset fails
 * Branch: California_branch
 *
 * Revision 3.6.8.2  1996/05/17  11:17:48  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.34.6  1996/04/09  06:39:07  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.34.5  1996/04/03  14:29:28  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.34.4  1996/03/28  11:28:37  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.34.3  1996/03/21  06:07:54  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.5.34.2  1996/03/17  17:49:17  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.5.34.1  1996/03/15  05:55:36  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comments
 *
 * Revision 3.6.8.1  1996/03/18  19:41:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.2  1996/03/16  06:50:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.16.1  1996/01/24  21:46:18  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/02/28  19:47:53  motto
 * CSCdi45011:  Get hung output on CIP virtual interface
 *
 * Revision 3.5  1995/12/08  19:26:13  smackie
 * Move memory size defines into a central place. (CSCdi45214)
 *
 * Revision 3.4  1995/12/05  15:12:47  motto
 * CSCdi30997:  snmp sub-agent for IBM channel claw cfg returns too many
 * instances
 *
 * Revision 3.3  1995/11/17  09:17:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:32  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:00:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/09/15  05:57:04  ppearce
 * CSCdi40438:  Cannot config LLC parms for RSRB/LAK on CSNA virtual i/f
 *
 * Revision 2.7  1995/09/11  10:23:47  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.6  1995/08/27  17:16:44  motto
 * CSCdi39079:  LF field in RIF for CIP ILANs is never greater than 2052
 *
 * Revision 2.5  1995/08/22  17:18:37  motto
 * CSCdi37937:  CIP sysmgt RPCs timeout when all interfaces shutdown
 *
 * Revision 2.4  1995/08/09  21:33:23  ppearce
 * CSCdi38300:  Add CSNA packet switching to 75xx (RSP)
 *
 * Revision 2.3  1995/08/08  16:46:57  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.2  1995/06/16 04:27:38  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:40:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef	__IF_CIP_H__
#define	__IF_CIP_H__

#define CIP_DEFAULT_MTU     4096
#define CIP_VIRTUAL_MTU     4472            /* For the CIP virtual interface */

/*
 * Maximum number of adapters supported by CIP
 */
#define CIP_MAX_DAUGHTERS   2

/*
 * Virtual port on CIP card (index base of 0)
 */
#define CIP_VIRTUAL_PORT   2

/*
 * Adapter types returned from FCI_CMD_CIP_DAUGHTER_TYPE
 *                          or FCI_CMD_CIP_ADPTR_INFO (FCI_ADPTR_TYPE)
 */
#define CIP_NONE                0
#define CIP_PCA_DAUGHTER        1
#define CIP_ECA_DAUGHTER        2
#define CIP_VIRTUAL_INTERFACE   3

#define CIP_NUM_INTERFACE_TYPE  3

/* 
 * Bandwidth defaults for both IGRP and the 'show int' display. 
 */
#define VISIBLE_CIP_PCA_BANDWIDTH_3      24576 /* 24.576 Mbps */
#define VISIBLE_CIP_PCA_BANDWIDTH_4_5    36864 /* 36.864 Mbps */
#define VISIBLE_CIP_ECA_BANDWIDTH        98304 /* 98.304 Mbps*/

/*
 * IGRP delay metrics
 */
#define CIP_PCA_DELAY_3		41	/* 410 usec */
#define CIP_PCA_DELAY_4_5	27	/* 270 usec */
#define CIP_ECA_DELAY		10	/* 100 usec */

/*
 * The PCA daughter board supports several different data rates.  The
 * ones we support are defined in the pca_rates[].  We default to the
 * slowest rate.  The rate code is the value sent to the CIP to select
 * a rate.
 *
 * CHANNEL_PROTCL     MBytes/Sec     Rate code
 *      S                 3              0
 *      S4                4.5            1
 */
#define CIP_NUM_PCA_RATES        2
#define CIP_PCA_RATE_3           0        /* rate codes sent to CIP */
#define CIP_PCA_RATE_4_5         1
#define CIP_PCA_DEFAULT_RATE     0        /* index into pca_rates[] */

/* 
 * In cip_reset() the stat interval gets set to
 * CIP_DEFAULT_STATS_INTERVAL, but it can be changed by using the
 * 'test cbus' command. 
 */
#define CIP_DEFAULT_STATS_INTERVAL   10   /* seconds between stats update */
#define CIP_MIN_STATS_INTERVAL        5   /* don't do it more often */

/* 
 * CIP specific cbus commands 
 */
#define FCI_CMD_CIP_DAUGHTER_TYPE    66   /* Is it PCA or ECA */
#define FCI_CMD_CIP_SET_PCA_BW       67   /* Set PCA data rate */
#define FCI_CMD_CIP_ADDR_SELECT      70   /* Select an address */
#define FCI_CMD_CIP_MEM_WRITE        71   /* Write data */
#define FCI_CMD_CIP_MEM_READ         72   /* Read data */
#define FCI_CMD_CIP_CFGBLOCK         73   /* get address of cfg block */
#define FCI_CMD_CIP_CFGPARSE         74   /* tell CIP to parse cfg block */
#define FCI_CMD_CIP_DIAGNOSTIC       75   /* run diagnostics */
#define FCI_CMD_CIP_STATS_INTERVAL   76   /* set stats interval */

#define FCI_CMD_CIP_ADPTR_INFO       77   /* query adapter information    */
#  define FCI_ADPTR_TYPE              0   /*    Adapter type              */
#  define FCI_ADPTR_HWVER             1   /*    Adapter hardware version  */
#  define FCI_ADPTR_UCODE             2   /*    Adapter microcode version */
#define FCI_CMD_CIP_CARD_INFO        78   /* query card information       */
#  define FCI_SRAM_AVAIL              0   /*    SRAM available            */
#  define FCI_SRAM_TOTAL              1   /*    SRAM total                */
#  define FCI_DRAM_AVAIL              2   /*    DRAM available            */
#  define FCI_DRAM_TOTAL              3   /*    DRAM total                */

#define CIP_NAME_LEN        9
#define CIP_MAX_PATH        0xffff
#define CIP_MAX_DEVICE      0xff

/*
 * FLAGS used in cip_vc flags field
 */
#define CIP_VC_INUSE  0x0001
#define CIP_ADJ_DEV   0x0002      /* CLAW and OFFLOAD use the adjacent odd device */
#define CIP_VC_BRDCST 0x0004	/* do broadcast flooding on this vc */
#define CIP_VC_SROUTE 0x0008    /* static route added */

/*
 * Configuration acknowledgment bits (ack_flags)
 */
#define CIP_CFG_PNDING 0x0001    /* configuration acknowledgment pending */
#define CIP_CFG_CREATE 0x0002    /* CREATE pending */
#define CIP_CFG_UPDATE 0x0004    /* UPDATE pending */
#define CIP_CFG_DELETE 0x0008    /* DELETE pending */

/*
 * Asychronous state change flags (state_flags)
 */
#define CIP_CHG_PNDING 0x0001    /* state change pending */
#define CIP_CHG_UP     0x0002    /* state transition UP */
#define CIP_CHG_DOWN   0x0004    /* state transition DOWN */

/*
 * FLAGS used in ack_flags parameter for configuration acknowledgment
 */
#define CIP_CFG_ACK_CIP      0x0001  /* CIP   acknowledgement */
#define CIP_CFG_ACK_LOCAL    0x0002  /* local acknowledgement */

/*
 * FLAGS used in path_flags field
 */
#define PATH_ESTABLISHED 0x0080

#define CIP_PER_INT_MAX_VC  255
#define CIP_TEARDOWN_VC     0x80	/* tear down this vc */
#define CIP_NEW_VC          -1		/* create a new vc # */
#define CIP_INVALID_VC      -2		/* make vc invalid */

#define CIP_NODEID_SIZE     24		/* size of nodeid block  */
#define CIP_CFG_SIZE       (136)	/* size of largest command block, 
					 * including correlator
					 */

#define CIP_DISABLE_TIME 5		/* CHANNELTODO - find good value  */

/*
 * Search types used by find_next_path() and find_next_path_device()
 */
#define SEARCH_NEXT   1
#define SEARCH_MATCH  2
#define NEXT_BEST     3

/* 
 * These lengths are the string lengths used in both structures of
 * node descriptors.
 */
#define ND_TYPE_LEN      6
#define ND_MODEL_LEN     3
#define ND_MANUF_LEN     3
#define ND_PLANT_LEN     2
#define ND_SERIAL_LEN   12

/*
 * silly info need for IBM nodeid 
 *
 * This structure is the one used by the RP to send its info to the
 * CIP.  It does not have all the fields as the full node descriptor.
 * The full node descriptor structure (defined as struct nd) is used
 * in the love-letter sent from the CIP.
 */
struct cip_nodeid {
    uchar dev_type;                   /* so that we can use cip_write_mem to send */
    uchar type_number[ND_TYPE_LEN];   /* Router type */
    uchar model_number[ND_MODEL_LEN]; /* Router model */
    uchar plant[ND_PLANT_LEN];        /* Plant of manufacture */
    uchar sequence[ND_SERIAL_LEN];    /* Serial number for router */
};
typedef struct cip_nodeid cip_nodeidtype;


/*
 * Link Incident Record types (defined in cip/slc/lir.h)
 */
#define LIR_IMPL_INCIDENT   0  /* Implicit Incident */
#define LIR_BER_EXCEEDED    1  /* Bit-Error-Rate Exceeded */
#define LIR_LINK_FAILED     2  /* Link Failed */
#define LIR_NOS             3  /* Not Operational Sequence Recognized */
#define LIR_SEQ_TO          4  /* Link Failure - Sequence Timeout */
#define LIR_INVALID_SEQ     5  /* Link Failure - Invalid Sequence */
#define MAX_LIR_TYPES       6  /* Total number of lir types */
#define LIR_ND_SIZE         8  /* 8 words */


/* 
 * dbcb values taken from cip/slc/dbcb.h  
 */
#define DB_VALID         0x8000  /* There is a daughter board connected */
#define DB_ESCON         0x4000  /* This daughter card is an escon adapter */
#define DB_LOADED        0x2000  /* Daughter board processor is running */
#define DB_END_LOAD      0x1000  /* Terminate the load that is running */
#define DB_OFFLINE        0x800  /* Channel has been taken offline */
#define DB_GO_OFFLINE     0x400  /* Request has been made to take channel offline */ 
#define DB_RQC_PENDING    0x200  /* Request connection is pending */
#define DB_RES1_1         0x100  /* Reserved */
#define DB_RES1_2          0x80  /* Reserved */
#define DB_RES1_3          0x40  /* Reserved */
#define DB_LOAD_PENDING    0x20  /* Flag to indicate load has been requested */
#define DB_FORCD_LOGOUT    0x10  /* Remember forced logout has occurred */
#define DB_RES2_1           0x8  /* Reserved */
#define DB_RES2_2           0x4  /* Reserved */ 
#define DB_RES2_3           0x2  /* Reserved */
#define DB_RES2_4           0x1  /* Reserved */

/* 
 * pacb flags values taken from cip/dma/pacb.h  
 */
#define PA_VALID         0x8000  /* There is a daughter board connected */
#define PA_ENABLED       0x4000  /* Flag to indicate MEMD accesses are allowed */
#define PA_SIGNAL        0x2000  /* Signal detected */
#define PA_SEND_LOVE     0x1000  /* Time to send a love note */
#define PA_PASS_DISABLE   0x800  /* Passive disable issued */
#define PA_PASS_ENA_PEND  0x400  /* Passive enable issued but not done */
                                 /* Remaining bits reserved */ 
/*
 * Data structure used for storing PCA data transfer rates
 */
typedef struct {
    char *name;
    char *value;
    int rate_code;
} rate_type;

/*
 * Data transfer statistics (defined in cip/slc/stats.h)
 */
typedef struct data_stats_ {
    ulong blocks_read;          /* Number of read data transfer CCWs processed */
    ulong blocks_written;       /* Number of write data transfer CCWs */
    ulong bytes_read_high;      /* Number of bytes read */
    ulong bytes_read_low;       /* Number of bytes read */
    ulong bytes_written_high;   /* Number of bytes written */
    ulong bytes_written_low;    /* Number of bytes written */
    ulong read_blocks_dropped;  /* Number of read blocks dropped */
    ulong write_blocks_dropped; /* Number of write blocks dropped */
    ulong failed_memd_buf;      /* Failed attempts to get a memd buffer */
    boolean connected;          /* Status of logical link */
    uchar reserved1[3];         /* Align on 8 byte field */
    ulong reserved2;            /* &*@#$ make it match the cip structure */
} data_stats;

/*
 * Statistics kept for each subchannel  (cip/slc/stats.h)
 */
#define CLAW_MAX_LINKS    32
typedef struct statistics_ {
    ulong connections;          /* Number of times device was connected -
                                   for some devices this correlates with
				   number of start subchannels */
    ulong cmd_retries;          /* When used with above counter will
				   accurately count start subchannels */
    ulong cancels;              /* Number of halt subchannels */
    ulong sel_resets;           /* Number of selective resets */
    ulong system_resets;        /* Number of system resets */
    ulong dev_errors;           /* Number of device level errors */
    ulong cu_busy;              /* Number of short busy sents */
    ulong reserved;             /* Reserved */
    data_stats data[CLAW_MAX_LINKS]; /* Area for data counters */
} statistics;

/*
 * CIP load metric fields
 */
typedef struct cip_metrics_ {
    ulong one_min;
    ulong five_min;
    ulong sixty_min;
} cip_metrics;
typedef cip_metrics *cip_metrics_p;


/*
 * Process Level event support
 */
/*
 * Values for the first field of a CIP generated CFG_ACK msg
 */
enum CFGACK_CODE {
     CIP_CFG_NACK   =  0,     /* Negative response to a cfg command */
     CIP_CFG_ACK    =  1,     /* Positive response to a cfg command */
     CIP_CFG_SR_ADD =  2,     /* Add Static route                   */
     CIP_CFG_SR_DEL =  3      /* Del Static route                   */
};

struct deed_t_ {              /* Format of encapsulation DEED message */
    ulong  deed_code;               /* code */
    uchar  deed_data[CIP_CFG_SIZE]; /* A copy of the configuration command */
};
typedef struct deed_t_ deed_t;

enum dispose_t_ {
    DISPOSE_TO_CHUNK,  /* Return element to the chunk */
    DISPOSE_TO_PREQ    /* Return element to the preQ */
};
typedef enum dispose_t_ dispose_t;

enum plevent_loc_t_ {
    PLEVEL_CHNK,   /* the chunk */
    PLEVEL_PREQ,   /* the preQ */    
    PLEVEL_RDYQ    /* the rdyQ */
};
typedef enum plevent_loc_t_ plevent_loc_t;

/* Process level work element */
enum PLEVENT {
     PLEVENT_CFGACK,           /* Configuration acknowledgement */
     PLEVENT_ASYREQ,           /* Asynchronous cfg state change */
     PLEVENT_ENABLE            /* Process a deferred enable */
};
/* Size of the data field of an encapsulated DEED message 
 * This size must be large enough for largest deed response,
 * and must be a multiple of 4 bytes since this
 * is filled in by the mci2buffer() routine
 * which always moves 4 bytes at a time.
 */
#define PLEVENT_DSIZE ((sizeof(deed_t)+3) & 0xFFFC)

struct plevel_t_ {
    struct plevel_t_ *next;
    enum PLEVENT pl_event;         /* Event code */
    ushort pl_size;                /* logical size of element */ 
    uchar  pl_data[PLEVENT_DSIZE]; /* data - size must be multiple of 4 */
};
typedef struct plevel_t_ plevel_t;

#define CIP_PLEVENT_ELEMENT_SIZE (sizeof(plevel_t))
#define CIP_PLEVENT_COUNT      16            /* Number of elements in each chunk */ 
#define CIP_PLEVENT_CHUNK_NAME "CIP Events"  /* Chunk name */

#define CIP_PLEVEL_PREQ_MIN     2   /* Minimum number of elements available on preQ */

/* vector for processing CIP encapsulated msgs */
typedef void (*cip_encap_t)(hwidbtype *, void *, ushort);

/* 
 * info allocated per interface on the CIP
 */
struct cip_idb_info {
    queuetype cip_vcQ;
                                  /* Fields for Process Level Evebt support */
    ushort      plevel_preQ_min;  /* Minimum number of elements available for use */    
    queuetype   plevel_preQ;      /* Queue of preallocated plevel elements */
    queuetype   plevel_rdyQ;      /* Ready Queue; elements to be processed at proclevel */
    ushort      reset_correlator; /* Reset correlator; flush event with old value */
    ushort      count_delay_ack;  /* Test hack- delay processing of cfg acks */
    cip_encap_t cip_encaps;       /* vector for processing CIP encapsulated msgs */

    void *pVlanData;
    void *pTn3270sData;
    int daughter_type;
    int data_rate;              /* PCA only, index into pca_rates[] */
    ushort vc_base;  
    ushort next_vc;
    int subchannels;
    ulong cfg_block;            /* address of where to write CIP cfg blocks */
    sys_timestamp last_love_letter;
    ulonglong cip_timestamp;
    /* stuff from CARD love letter */
    ulong sram_size;
    ulong sram_free;
    ulong dram_size;
    ulong dram_free;
    ulong cpu_util;
    cip_metrics cip_cpu_load;
    cip_metrics cip_dma_load;
    
    /* stuff from DB love letter */
    ulong db_flags;
    ulong pacb_flags;
    ulong link_addr;
    ulong node_id[LIR_ND_SIZE];
    ulong devices;
    ulong no_buffers;
    ulong crc_errors;
    ulong load;
    ulong link_incident[MAX_LIR_TYPES];
    cip_metrics cip_channel_load;

    sys_timestamp timeOfReset;
    /* SNA config stuff here */
    queuetype cip_cta_ttQ;
    /* SYSMGT info here */
    void *sysmgt_ipc_info;
};
typedef struct cip_idb_info cip_idb_infotype;

/*
 * info allocated per 'subchannel' of an interface
 */
struct cip_vc {
    struct cip_vc *next;
    struct cip_vc *claw_pair;
    short vc_num;
    ushort flags;
    uchar dev_type;
    uint path;
    uint device;
    ipaddrtype  ipaddr;      
    ipaddrtype  ipaddr2;           /* only used by data-pusher */
    uchar hostname[CIP_NAME_LEN];  /* Host name for the CLAW device */
    uchar cipname[CIP_NAME_LEN];   /* Workstation name for the CLAW device */
    uchar hostappl[CIP_NAME_LEN];  /* Host name of the CLAW IP link */
    uchar cipappl[CIP_NAME_LEN];   /* Workstation name of the CLAW IP link */
    uchar hostappl2[CIP_NAME_LEN]; /* Host name of the CLAW API link */
    uchar cipappl2[CIP_NAME_LEN];  /* Workstation name of the CLAW API link */
    ushort maxpiu;                  /* CSNA max block size     */
    ushort blocking_time_delay;     /* CSNA block time delay   */
    ushort blocking_length_delay;   /* CSNA block delay length */
    ushort ack_flags;               /* cfg acknowledgment flags */
    ushort state_flags;             /* state change flags */
    ulong path_flags;
    ulong dev_flags;
    ulong last_sense;
    ulonglong last_sense_time;
    statistics stats;
    statistics si_stats;
};
typedef struct cip_vc cip_vctype;

#define CIP_NUM_CLAW_NAMES    4
#define CIP_NUM_OFFLOAD_NAMES 6

/*
 * CIP love letter structures   (defined by cip/dma/mib_data.h)
 */

#define LL_TIMESTAMP_SIZE   2     /* size of timestamp field (words) */
#define LL_LEN_SIZE         1     /* size of length field (words)    */
#define LL_TYPE_SIZE        1     /* size of type field   (words)    */
#define LL_HEADER_SIZE      (LL_LEN_SIZE + LL_TYPE_SIZE)

#define CIP_MAX_MIB_DATA    336   /* size of the largest ll type */

typedef struct {    /* love-letter template */
    ulong len;
    ulong type;
    ulong data[CIP_MAX_MIB_DATA]; /* one of the next 3 structures based on type */
} cip_ll;

#define CIP_MIB_CARD        1     /* cip_ll_card */
#define CIP_MIB_DB          2     /* cip_ll_db   */
#define CIP_MIB_DEV         3     /* cip_ll_dev  */
#define CIP_MIB_PA          4     /* cip_ll_pa   */

typedef struct {    /* CIP_MIB_CARD */
    ulong sram_size;
    ulong sram_free;
    ulong dram_size;
    ulong dram_free;
    ulong cpu_util;
    ulong cpu_load_one_min;
    ulong cpu_load_five_min;
    ulong cpu_load_sixty_min;
    ulong dma_load_one_min;
    ulong dma_load_five_min;
    ulong dma_load_sixty_min;
} cip_ll_card;

typedef struct {    /* CIP_MIB_DB */
    ulong flags;
    ulong link_addr;
    ulong node_id[LIR_ND_SIZE];
    ulong devices;
    ulong no_buffers;           /* Times MEMD buffer was not available */
    ulong crc_errors;           /* Count of bit error bursts */
    ulong load;
    ulong link_incident[MAX_LIR_TYPES];
    ulong pacb_flags;           /* port adapter flags */
    ulong channel_load_one_min;
    ulong channel_load_five_min;
    ulong channel_load_sixty_min;
} cip_ll_db;

typedef struct {    /* CIP_MIB_DEV */
    ulong vc;
    ulong path;
    ulong path_flags;
    ulong dev_address;
    ulong dev_type;
    ulong dev_flags;
    ulong last_sense;
    ulong last_sense_time[2];  /* 64 bit timestamp, units 20ns*/
    statistics stats;
} cip_ll_dev;

typedef struct {    /* CIP_MIB_PA */
    ulong no_buffers;           /* Times MEMD buffer was not available */
    ulong crc_errors;           /* Count of bit error bursts */
    ulong pacb_flags;           /* port adapter flags */
} cip_ll_pa;

/*
 * Node descriptor (stolen from CIP's slc/lir.h)
 */
typedef struct {
    struct {
	uchar validity     :3;        /* Node-ID validity */
	uchar type         :1;        /* Node type */
	uchar reserved1    :4;        /* Reserved */
    } flags;                          /* flags */
    uchar reserved2;                  /* Reserved */
    uchar class;                      /* Class - we use 9 CTCA */
    uchar reserved3;                  /* Zero for us */
    uchar type_number[ND_TYPE_LEN];   /* Device type */
    uchar model_number[ND_MODEL_LEN]; /* Model number */
    uchar manufacturer[ND_MANUF_LEN]; /* Manufacturer */
    uchar plant[ND_PLANT_LEN];        /* Plant of manufacture */
    uchar sequence[ND_SERIAL_LEN];    /* Sequence number */
    ushort tag;                       /* Tag */
} nd;
#define ND_VALID            0  /* ND is valid */
#define ND_VAL_NOT_CURRENT  1  /* ND is valid but not current */
#define ND_NOT_VALID        2  /* ND is not valid */
#define CLASS_DIRECT        1  
#define CLASS_CTCA          9  /* Class for CTCA */
#define CLASS_SWITCH        10
/* 
 * structure used to tally daughter, path, and device totals
 */
typedef struct {
    ulong blk_read;
    ulong blk_write;
    ulonglong byte_read;
    ulonglong byte_write;
    ulong drop_read;
    ulong drop_write;
    ulong failed_memd;
} cip_totals;
#define TOT_DEV    0        /* Device totals */
#define TOT_PATH   1        /* Path totals   */
#define TOT_DB     2        /* Daughter board totals */
#define NUM_TOTALS 3


/*
 * info allocated per 'cta-test-tool command' of an interface
 */
struct cta_tt_cmd {
    struct cta_tt_cmd *next;
    uint   command;
    uint   offset;
    uint   length;
    uchar  value[8];
    ushort ack_flags;               /* cfg acknowledgment flags */

};
typedef struct cta_tt_cmd cta_tt_cmdtype;


/*
 * macro for walking all the cta-test-tool commands on idb
 */
#define FOR_ALL_CTA_TT_CMDS_ON_CIP_IDB_INFO(info, cta_tt_p) \
    for ((cta_tt_p) = (cta_tt_cmdtype *)(info)->cip_cta_ttQ.qhead; \
         (cta_tt_p); (cta_tt_p) = (cta_tt_p)->next)

/*
 * macro for walking all the vc's on idb
 */
#define FOR_ALL_VCS_ON_CIP_IDB_INFO(info, vc) \
    for ((vc) = (cip_vctype *)(info)->cip_vcQ.qhead; (vc); (vc) = (vc)->next)

/*
 * interval at which we get love-letters 
 */
extern ushort cip_stats_interval;   

#define CHANNEL_ENCAPS_LOVE   0xdead  /* Encapsulation type for love letter */
#define CHANNEL_ENCAPS_CFGACK 0xdeed  /* Encapsulation type for config response */
#define CHANNEL_ENCAPS_IPC    0xbead  /* Encapsulation type for CIP IPC */
#define CHANNEL_ENCAPS_VLAN   0x2001 /* Encapsulation type for vlan traffic */

 
 
#define SEGMENT_NAME_MAX_LENGTH 16
#define COMPILE_INFO_MAX_LENGTH 44

/*
 * The cipapplversion
 */
typedef struct cip_appl_version_ {
    char name[SEGMENT_NAME_MAX_LENGTH];
    char compile_info[COMPILE_INFO_MAX_LENGTH];
    int revision;
}cip_appl_version_t;

#define CIP_APPL_VERSION_SIZE             sizeof(cip_appl_version_t)

/*
 * The cipversion
 */

typedef struct cip_version_ {
    int hw_version_major;
    int hw_version_minor;
    int sw_version_major;
    int sw_version_minor;
} cip_version_t;


/*
 *  if_shutdown string for show commands that require IPC when
 *  the interface is shutdown.
 */
extern const char if_shutdown[];

extern const char cip_show_util_indentation[];

/*
 * Prototypes
 */
void channel_interface_csna_command(parseinfo *);
void channel_interface_command(parseinfo *);
void channel_rate_command(parseinfo *csb);
void cip_plevel_enqueue(hwidbtype *, cip_idb_infotype *, plevel_t *, ushort);
plevel_t *cip_plevel_element_get(cip_idb_infotype *, plevent_loc_t);
void cip_process_queued_events(hwidbtype *, cip_idb_infotype *);
char *cip_cfg_name_str(ushort);
cip_idb_infotype *cip_cardInfoGet(long *slot, int searchType);
cip_idb_infotype *cip_interfaceInfoGet(long *slot, long *port, int searchType);
cip_vctype * cip_getSubchannel(long *slot, long *port, long *path,
			       long *device, int searchType);
cip_vctype * cip_getSubchannelClaw(long *slot, long *port, long *path,
				   long *device, long *claw_index,
				   int searchType);
extern cip_vctype * cip_getSubchannelClawConfig(long *slotP, long *portP, 
                                                long *pathP, long *deviceP, 
                                                long *clawP, int search);
cip_vctype *cip_getSubchannelOffloadConfig(long *slotP, long *portP, 
					   long *pathP, long *deviceP,
					   int search);
cip_vctype *cip_getSubchannelCsnaConfig(long *slotP, long *portP, 
					long *pathP, long *deviceP,
					hwidbtype **hwidb, int search);
data_stats *cip_getDataStats(cip_vctype *vc, long claw_link);
ulong cip_timeSinceLastReset(cip_idb_infotype * cip);
boolean cip_daughterBoardSignal(cip_idb_infotype * cip);
boolean cip_daughterBoardOffline(cip_idb_infotype * cip);
boolean cip_daughterBoardStatus(cip_idb_infotype *cip);
ulong cip_subchannelWriteBlocksDropped(cip_vctype *vc);
void channel_idb_init(hwidbtype *);
void channel_periodic(hwidbtype *);
void cip_reset(hwidbtype *);
boolean cip_send_or_ack(hwidbtype *, cip_idb_infotype *, ushort, uchar *,
			ulong);
boolean cip_cmd_valid_on_interface(hwidbtype *, ushort, boolean);
boolean cip_interface_is_virtual(hwidbtype *);
void cip_copy_idb_id_info(hwidbtype *src_idb, hwidbtype *dst_idb);
void cip_copy_idb_info(hwidbtype *src_idb, hwidbtype *dst_idb);

ulonglong cip_get_love_letter_timestamp(cip_idb_infotype *);
void cip_set_love_letter_timestamp(cip_idb_infotype *, ulonglong);

extern void cip_restart_init(hwidbtype *);
extern void cip_platform_init(subsystype *);
extern void cip_encaps_deed(hwidbtype *, void *, ushort);   
extern void cip_chassis_serial_init(cip_nodeidtype *cip_nodeid);
extern void cip_show_cbus(ushort indx);
extern void cip_fs_setup(hwidbtype *);

boolean ipc_avail_when_interfaces_shut(void);

boolean get_cip_appl_version(int, long *, cip_appl_version_t *);
void get_cip_version(int, cip_version_t *);

hwidbtype *hwidb_lookup(ushort, ushort);
boolean cip_version_info(ushort, ushort *, ushort *);

boolean cip_len_exceeded(uchar *, uint);
boolean cip_validate_ipaddr(idbtype *, ushort, ipaddrtype);

boolean channel_vencap(paktype *pak, long address);

#endif
