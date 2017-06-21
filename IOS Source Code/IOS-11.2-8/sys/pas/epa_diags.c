
/* $Id: epa_diags.c,v 
 *------------------------------------------------------------------
 * epa_diags.c - Diagnostic test for Encryption PA.
 *
 * June 1996, Paulina D. Tran
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: epa_diags.c,v $
 * Revision 3.1.2.7  1996/09/11  20:13:21  ptran
 * CSCdi68839:  $Log is missing from esa files
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "stdlib.h"
#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "subsys.h"
#include "sched.h"
#include "parser.h"
 
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
 
#include "../if/pci.h"
#include "../hes/if_fci.h"
#include "../if/network.h"
 
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_priority.h"

#include "if_pas.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "pas_id.h"

#include "../ip/ip.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_public.h"

#include "msg_epad.c"			/* Not a typo, see logger.h */
#include "if_pas_epad.h"
#if defined(RVIP) || defined(SVIP)
#include "if_vip_epad.h"
#endif
#if defined(PREDATOR)
#include "if_c7100_epad.h"
#endif

#include "epa_diags.h"
#include "pas_diags.h"

#define ALTERNATE     NONE
#include "epad_test_chain.h"
LINK_POINT (test_epad_commands, ALTERNATE);
#undef ALTERNATE
 
/*
 * Parser extension data structure
 */
parser_extension_request epadtest_chain_init_table[] = {
    { PARSE_ADD_TEST_PAS_CMD, &pname(test_epad_commands) },
    { PARSE_LIST_END, NULL }
};

static epad_stat_t stat;
static paktype *inpak = NULL;
static paktype *inpak2 = NULL;

#define START_SEQUENCE 0 /* #define EPA_FIX_PATTERN */
static ulong send_id[2]={START_SEQUENCE,0};
static ulong recv_id[2]={START_SEQUENCE,0};

static hwidbtype *outidb = NULL;
static hwidbtype *outidb2 = NULL;

/*** Use for encryption test ***/
static ulong decrypt_id = 0;
static int num_traffic = 0;

/*** Use for key management test ***/
static short ghandle1 = 0; /* size = 64, read only */
static short ghandle2 = 0; /* size = 64, write */
static short ghandle3 = 0; /* size = 20, write */
static short ghandle4 = 0; /* size = 20, write */
static short ghandle5 = 0; /* size = 1024, write */
static char gdh_public[PUBLIC_SIZE]; /* global dh public array */
static uchar gdss_public[PUBLIC_SIZE]; /* global dss public array */
static short gsession = 0; /* global session number */
static uchar ghash[SHA_HASH_SIZE]; /* global hash array */
static uchar gR[R_SIZE]; /* global R array */
static uchar gS[S_SIZE]; /* globl S array */
static uchar passwd[6] = "CYCORE";
static epa_ktest_t ktest_table[NUM_KEY_CMD][MAX_KEY_TEST];
static void init_ktest_result(void);
static ushort handle_array[MAX_ALLOC];
static char giv[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static uint verbose = 0;

static watched_boolean *next_test = NULL;
static boolean ABORT_TEST = FALSE;
static boolean EPA_TIMEOUT = FALSE;
extern ulong epad_interrupt_handler(hwidbtype *);
 
#if defined(RVIP) || defined(SVIP)
#define PUBLIC_PARTICLE_BYTES VIP_PARTICLE_BYTES
#endif
#if defined(PREDATOR)
#define PUBLIC_PARTICLE_BYTES C7100_PARTICLE_BYTES
#endif

/**** TEST ULTILITY FUNCTION ****/
/* need to define EPAD_TEST and EPAD_ACCOUNTING in epa driver for these
 * tests to work
 */
/****************************************************************************
 * put_check_byte - Put a maker at the beginning and at the end of data to
 * detect data overrun.
 ****************************************************************************/
static inline void put_check_byte (uchar *start, ulong bytes)
{
    *(start - 1) = CHECK_BYTE;
    if (bytes % 4) {
	bytes += (4 - bytes%4);
    }
    *(start + bytes) = CHECK_BYTE;
}

/****************************************************************************
 * verify_check_byte
 ****************************************************************************/
static inline boolean verify_check_byte (uchar *p, ulong size)
{
    if (*(p - 1) != CHECK_BYTE) {
        pas_diag_errmsg(MSG_ERROR, "Buffer modified past beginning:\n");
        return FALSE;
    }
    if (size % 4) {
	size += (4 - size%4);
    }
    
    if (*(p + size) != CHECK_BYTE) {
	pas_diag_errmsg(MSG_ERROR, "Buffer modified past end:\n");
	return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
 * process_recv_data - Check the receive data with the expected value.
 * If the length is not a multiple of 4, the last couple bytes will not
 * be check.
 *****************************************************************************/
void process_recv_data (uint ring, paktype *pak, particletype *mp)
{
    uint i;
    uchar *p;
    ulong *lp;
    ulong size;

    p = (pak) ? pak->datagramstart : mp->data_start;
    lp = (pak) ? (ulong *)pak->datagramstart : (ulong *)mp->data_start;
    size = (pak) ? pak->datagramsize : mp->data_bytes;
       
    for (i=0;i<size/4;i++) {
        if (lp[i] != ~recv_id[ring]++) {
            pas_diag_errmsg(MSG_ERROR, "rcv data error expect:0x%08x, got:0x%08x\n",
		~(recv_id[ring]-1),lp[i]);
	    pas_diag_errmsg(MSG_ERROR, "%s ring, %s type, buf:0x%08x, size:%d, addr:0x%08x, offset:%d\n",
		    ring ? "Key" : "Frame", pak ? "Pak" : "Particle", 
		    lp, size, &lp[i], i);
	    recv_id[ring] = ~(lp[i])+1;
	    ABORT_TEST = TRUE;
	    process_set_boolean(next_test, TRUE);
	    break;
	}
    }
    if (size % 4) recv_id[ring]++;

    if (!verify_check_byte(p, size)) {
	pas_diag_errmsg(MSG_ERROR, "%s ring, %s type, buf:0x%08x, size:%d\n",
           	ring ? "Key" : "Frame", pak ? "Pak" : "Particle", p, size);
	ABORT_TEST = TRUE;
	process_set_boolean(next_test, TRUE);
    }
}

/*****************************************************************************
 * service_bitflip_traffic - Check packet that has been bitfliped for 
 * correct pattern.
 *****************************************************************************/
void service_bitflip_traffic (hwidbtype *idb, paktype *pak)
{
    particletype *mp;
 
    stat.num_encrypt_rcv++;
    if (!pak_has_particles(pak)) {
	process_recv_data(EPAD_FRAME_DEVICE,pak,NULL);
	if (!ABORT_TEST) {
	    datagram_done(pak);
	}
    } else {
        while ((mp = dequeue_inline(&pak->particlequeue))) {
	    process_recv_data(EPAD_FRAME_DEVICE,NULL,mp);
	    if (!ABORT_TEST) {
	        retparticle(mp);
	    }    
        }
    }
    if (stat.num_encrypt_rcv == num_traffic) {
	process_set_boolean(next_test, TRUE);
    }
}
/*****************************************************************************
 * test_callback
 *****************************************************************************/
void test_callback (ushort flags, paktype *pak)
{
    particletype *mp;
 
    if (!pak_has_particles(pak)) {
	process_recv_data(EPAD_KEY_DEVICE,pak,NULL);	
	if (!ABORT_TEST) {
	    datagram_done(pak);
	}
    } else {
        while ((mp = dequeue_inline(&pak->particlequeue))) {
	    process_recv_data(EPAD_KEY_DEVICE,NULL,mp);
	    if (!ABORT_TEST) {
		retparticle(mp);
	    }
        }
    }
}

/*****************************************************************************
 * epad_create_encryption_blk
 *****************************************************************************/
char ses_cntl_blk[sizeof(encrypt_cntl_blk_t)];
particletype *epad_create_encryption_blk (hwidbtype *idb, char id,
					  short session, char *iv_gen,
					  ushort offset, paktype *pak)
{
    encrypt_cntl_blk_t *blk;
    particletype *mp;

    if (id == ENCRYPT) {
	stat.num_encrypt_snt++;
    } else {
	stat.num_decrypt_snt++;
    } 
    mp = epad_get_particle_inline(epad_gidb[0] , TRUE);
    if (!mp) {
        pas_diag_errmsg(MSG_ERROR,
                "No particles avail idb->pool=0x%x for encryption blk\n",
                idb->pool);
        ABORT_TEST = TRUE;
        process_set_boolean(next_test, TRUE);
        return NULL;
    }
 
    blk = (encrypt_cntl_blk_t *)mp->data_start;
    blk->call_id = id;
    blk->status = 0;
    if (iv_gen) {
	bcopy(iv_gen, blk->iv_gen, 8);
    } else {
	bzero(blk->iv_gen, 8);
    }
    blk->session = session;
    blk->offset = offset;
    blk->total_length = pak->datagramsize;

    return (mp);
}

/*****************************************************************************
 * gen_data_pattern - 
 * Generate a data pattern of 4 bytes.  Always write multiple of 4.
 *****************************************************************************/
void gen_data_pattern (uchar *buf, int length, int offset, 
		       ulong *start)
{

    int i;
    int num;

    if (offset) {
	/*** need more work on this offset ***/
	pas_diag_errmsg(MSG_STATUS, "offset = %d, buf = 0x%08x\n", 
			offset, buf);
	for (i= 0; i<offset; i++) {
	    *buf = 0x1;
	}
	buf += offset;
    }

    num = (length % 4) ? (length/4) + 1 : (length/4);

    for (i=0; i < num; i++) {
	*((ulong *)buf + i) = (*start)++;
    } 
}

/*****************************************************************************
 * epad_gen_packet - Will try to get the pak until it succeed.
 * For random size of packet, reduce the size until a contiguous pak is
 * optained from the buffer.
 *****************************************************************************/
paktype *epad_getpak (int *size, boolean rndsize)
{	
    paktype *pak = NULL;
    int retry = 0;

    if (*size > 16384) pas_diag_errmsg(MSG_WARNING, " size > 16384\n", *size);
    do {
	
	pas_diag_errmsg(MSG_WARNING, "Trying to get free pak of size = %d\n", 
			*size);
	if (rndsize) 
	    *size = (*size >> 1) & 0xFFFFFFFC;
	else
	    if (retry++ > 1000) break;
    } while (*size && !(pak = getbuffer(*size)));
	
    if (rndsize && !(*size)) {
	*size = 100;
	pas_diag_errmsg(MSG_WARNING, "Waiting for free pak of size %d\n", 
			*size);
	retry = 0;
	while ((!(pak = getbuffer(*size))) && retry++ < 1000) {
	    process_suspend();
	}
    }
    return pak;
}
   

/*****************************************************************************
 * epad_genpacket - Create a packet with either contiguous or 
 * segmented packet with given data size.
 *****************************************************************************/
paktype *epad_genpack (uint ring, uint cont, uint size, uint offset,
		       boolean rndsize)
{
    paktype *pak = NULL;
    particletype *mp = NULL;
    int i;
    hwidbtype *gidb = epad_gidb[ring];
    epad_instance_t *ds = INSTANCE(gidb);
    int particle_size = PUBLIC_PARTICLE_BYTES;

    if (cont == 1) {
	/*
	 * Send contiguous packet
	 */	
	if (!(pak = getbuffer(size+offset))) {
	    pak = epad_getpak(&size, rndsize);
	    if (!pak) return NULL;
	}
	/*** Fill in data ***/
	gen_data_pattern(pak->datagramstart, size, 
			 offset, &send_id[ring]);
	pak->datagramsize=size+offset;
	put_check_byte(pak->datagramstart, size + offset);
	pak->if_output = outidb->firstsw;
    } else {
        /*
         * Send particled packet
         */
	int numpar=0, parsize=0;
        pak = inpak;

	numpar = (size+offset) / particle_size;
	numpar = ((size+offset) % particle_size) ? 
	         (numpar + 1) : numpar;
        for (i=0;i<numpar;i++) {
	    int retry = 0;
	    while ((!(mp = epad_get_particle_inline(epad_gidb[0], FALSE))) && retry++ < 5) {
/*
		pas_diag_errmsg(MSG_WARNING, 
				"%d: Waiting for free particle.\n",retry);
*/
		process_suspend();
	    }
	    if (retry >= 5) {
		pas_diag_errmsg(MSG_ERROR,"Waiting for free particle timeout, packet size = %d\n", size);
		ABORT_TEST = TRUE;
                EPA_TIMEOUT = TRUE;
		process_set_boolean(next_test, TRUE);
		return NULL;
	    }
	    parsize = (i == numpar-1) ? (size-i*particle_size) : 
	                                particle_size;
	    /* offset the data of the first particle */
	    offset = (i==0) ? offset : 0;

	    gen_data_pattern(mp->data_start, parsize, 
				 offset, &send_id[ring]);
	    mp->data_bytes = parsize;
	    if (parsize == 0) {
		pas_diag_errmsg(MSG_WARNING, "genpack: parsize = %d size=%d\n",
				parsize, size);
	    }
	    put_check_byte(mp->data_start, parsize);

	    particle_enqueue_inline(pak,mp);
        }
        pak->datagramsize = size;
    }
    if (verbose == 2) {
	pas_diag_errmsg(MSG_PROGRAM, 
	    "Sending %s packet of %d bytes to %s ring start id %d, count=%d\n",
	    cont==1 ? "contiguous" : "segmented", size,
	    ring==0 ? "frame" : "key", send_id[ring],ds->count);
    }
   
    return pak;
}

/*****************************************************************************
 * rand16 - Generate a random number of 2 bytes.
 *****************************************************************************/
static inline ushort rand16 (void)
{
    ulong current;
    ushort i,rnd;
 
    GET_TIMESTAMP32(current);
    rnd = (ushort) (current >> 2);
 
    for (i=0;i<3;i++)
        rnd = rnd ^ (rnd << 4);
 
    return(rnd);
}

/*****************************************************************************
 * get_rnd - Calculate random ring, continuous and size for a packet
 * Calculate the ring, cont and size if random was selected
 * bit 1: 0=high priority, 1=low priority ring
 * bit 2: 0=particles, 1=continuous packet
 * size (512 x 31 bytes maximum allow for frame device,
 *		   512 x 8 bytes maximum allow for key device) 
 *****************************************************************************/
void get_rnd_param (boolean rndring, boolean rndcont, boolean rndsize,
			int *ring, int *cont, int *size)
{
    ushort rnd = rand16();
    epad_instance_t *ds;
    int maxsize = 0;
    int particle_size = PUBLIC_PARTICLE_BYTES;


    if (rndring) *ring = rnd & 1; 
    if (rndcont) *cont = (rnd>>1) & 1;
    ds = INSTANCE(epad_gidb[*ring]);

    if (rndsize) {
	int num;
	/* For frame ring, save one buffer descriptor to store 
	 * the epa header 
	 */
	num = (*ring == EPAD_FRAME_DEVICE) ? ds->num_ring_entries -1 :
	                                    ds->num_ring_entries;
	maxsize = ((num * particle_size) > MAX_FRAME_SIZE) ? MAX_FRAME_SIZE :
	           num * particle_size;
	*size = (rnd>>2) % maxsize;
	/* just to get away from the zero byte problem */
	if (*size == 0) *size = particle_size;
    } 
}

static pooltype *diag_pool;
/*****************************************************************************
 * Create 2 idbs, 1 to use when we send encrypted packet out to a virtual
 * output interface and 1 to use when we send the decrypted packet out.
 * Create inpak to store that packet that we generated and send down to
 * the EPA.
 *****************************************************************************/
boolean test_setup (int num)
{
    hwidbtype * idb = epad_gidb[0];
    int pool_size = EPAD_FRAME_RING_ENTRIES + (2*EPAD_FRAME_HOLDQ_SIZE);
    mempool *buffer_mempool;

    next_test = create_watched_boolean("EPAD next test", 0);
    process_set_boolean(next_test, FALSE);
    ABORT_TEST = FALSE;
    EPA_TIMEOUT = FALSE;
    /* 
     * Adjust global pool to get more particle for testing
     */
    if (!diag_pool) {
        buffer_mempool = pas_buffer_mempool(idb);
        if (idb->pool_group == POOL_GROUP_INVALID) {
            idb->pool_group = pool_create_group();
        }

        diag_pool = particle_pool_create("EPA DIAG POOL",
                                    idb->pool_group,
                                    VIP_PARTICLE_BYTES,
                                    POOL_SANITY,
                                    32,
                                    buffer_mempool);
        if (!diag_pool) {
	    pas_diag_errmsg(MSG_ERROR, "Cannot create diag pool\n");
            return(FALSE);
        }
 

        pool_adjust(diag_pool, 0, pool_size, pool_size, TRUE);
        idb->pool->fallback = diag_pool;
    }

    /*
     * Create output idb in place of the real output interface idb.
     * Create input pak struct to store incoming particles
     */
    if (!outidb) {
	if (!(outidb = epad_idb_create(0, 0))) {
	    pas_diag_errmsg(MSG_ERROR, "Can't create outidb\n");
	    return FALSE;
	}
	outidb->crypto_vectors = malloc(sizeof(crypto_vector_t));
    }

    /*
     * Set up incoming paktype for incomming packet
     */
    if (!(inpak = pool_getbuffer(epad_gidb[EPAD_FRAME_DEVICE]->headerpool))) {
	pas_diag_errmsg(MSG_ERROR, 
			"Can't create in comming pak type, inpak\n");
	return FALSE;
    }
    inpak->if_output = outidb->firstsw;

    if (num == 2) {
	/*
	 * Create output idb in place of the real output interface idb.
	 * Create input pak struct to store incoming particles
	 */
	if (!outidb2) {
	    if (!(outidb2 = epad_idb_create(0, 0))) {
		pas_diag_errmsg(MSG_ERROR, "Can't create outidb2\n");
		return FALSE;
	    }
	    outidb2->crypto_vectors = malloc(sizeof(crypto_vector_t));
	}


	if (!(inpak2 = pool_getbuffer(epad_gidb[EPAD_FRAME_DEVICE]->headerpool))) {
	    pas_diag_errmsg(MSG_ERROR, "Can't create in comming pak type\n");
	    return FALSE;
	}
	inpak2->if_output = outidb2->firstsw;
    }
    return TRUE;
}

/*****************************************************************************
 * test_cleanup - Clean up pak structure that we use for testing
 *****************************************************************************/
static void test_cleanup(int num)
{
    datagram_done(inpak);
    if (num == 2) {
	datagram_done(inpak2);
    }
    delete_watched_boolean(&next_test);
}
void hexdump (uchar *p, int off, int len)
{
    int i;
    char buf[20];
   
    buf[16] = '\0';
    for (i=0; i<len; i++) {
        buf[i % 16] = (p[i]<' '||p[i]>126) ? '.' : p[i];
        if ((i % 16) == 0)
            ttyprintf(CONTTY, "%08x: %02x", off+i, p[i]);
        else if ((i % 8) == 0)
            ttyprintf(CONTTY, "  %02x", p[i]);
        else if ((i % 16) == 15)
            ttyprintf(CONTTY," %02x   %s\n", p[i], buf);
        else
            ttyprintf(CONTTY," %02x", p[i]);
    }
    if ((i = (i % 16)) != 0) {
        buf[i] = 0;
        if (i<9)
            ttyprintf(CONTTY," ");
        while (i++ < 16)
            ttyprintf(CONTTY,"   ");
	ttyprintf(CONTTY,"   %s\n", buf);

     }
}


/*****************************************************************************
 * send_to_epad
 *****************************************************************************/
static inline void send_to_epad(hwidbtype *idb, paktype *pak)
{
    leveltype level;
    level = raise_interrupt_level(NETS_DISABLE);
   
    /*hexdump(pak->datagramstart, 0, pak->datagramsize);*/
    epad_fastsend(idb,pak);
    reset_interrupt_level(level);
}

/*****************************************************************************
 * epad_throttle - Wait for the EPA card to keep up before sending more
 * packetes.
 * Return TRUE  - the hold queue is now empty, we can put more packet on.
 * Return FALSE - Timeout on waiting for the holdqueue to be empty.
 *****************************************************************************/
boolean epad_throttle (hwidbtype *idb1, hwidbtype *idb2)
{
    int retry;
    retry = 0;
    if (idb2) {
	/* Waiting for either queue to empty */
	while (idb1->output_qcount && idb2->output_qcount) {
	    process_suspend();		/* let other processes run */
	    if (retry++ > 2000000) {
		pas_diag_errmsg(MSG_ERROR, "Waiting for EPA timeout\n");
		ABORT_TEST = TRUE;
	 	process_set_boolean(next_test, TRUE);
                EPA_TIMEOUT = TRUE;
		return FALSE;
	    } 
	}
    } else {
	while (idb1->output_qcount) {
            process_suspend();
	    if (retry++ > 2000000) {
		pas_diag_errmsg(MSG_ERROR, "Waiting for EPA timeout\n");
		ABORT_TEST = TRUE;
		process_set_boolean(next_test, TRUE);
                EPA_TIMEOUT = TRUE;
		return FALSE;
	    }
	}
    }
    return TRUE;
}

/*****************************************************************************
 * process_encrypt_data - Will check the encrypted data to make sure
 * that no 4 consencutive bytes are the same as the clear text.
 *****************************************************************************/
static void process_encrypt_data (uint ring, paktype *pak, particletype *mp)
{
    /* Check the encrypted data */
    uint i;
    uchar *p;
    ulong *lp;
    ulong size;
    boolean match = FALSE;

    p = (pak) ? pak->datagramstart : mp->data_start;
    lp = (pak) ? (ulong *)pak->datagramstart : (ulong *)mp->data_start;
    size = (pak) ? pak->datagramsize : mp->data_bytes;
    
    for (i=0;i<size/4;i++) {
        if (lp[i] == recv_id[ring]++) {
	    if (match == TRUE) {
		pas_diag_errmsg(MSG_ERROR, 
				"encrypt data error: not expect:0x%08x\n",
				lp[i]);
		pas_diag_errmsg(MSG_ERROR, "%s ring, %s type, buf:0x%08x, size:%d, addr:0x%08x, %d\n",
				ring ? "Key" : "Frame", pak ? "Pak" : "Particle", 
				lp, size, &lp[i], i);
		ABORT_TEST = TRUE;
		process_set_boolean(next_test, TRUE);
		break;
	    } else {
		match = TRUE;
	    }
	} else {
	    match = FALSE;
	}
    }
    if (size % 4) recv_id[ring]++;
    if (!verify_check_byte(p, size)) {
	pas_diag_errmsg(MSG_ERROR, "%s ring, %s type, buf:0x%08x, size:%d\n",
           	ring ? "Key" : "Frame", pak ? "Pak" : "Particle", p, size);
	ABORT_TEST = TRUE;
	process_set_boolean(next_test, TRUE);
    }
#if EPA_FIX_PATTERN
    recv_id[ring] = START_SEQUENCE;
#endif
}

/*****************************************************************************
 * process_decrypt_data - The decrypted data has to be the same as clear text.
 *****************************************************************************/
static void process_decrypt_data (uint ring, paktype *pak, particletype *mp)
{
    /* Check the decrypted data */
    uint i;
    uchar *p;
    ulong *lp;
    ulong size;

    p = (pak) ? pak->datagramstart : mp->data_start;
    lp = (pak) ? (ulong *)pak->datagramstart : (ulong *)mp->data_start;
    size = (pak) ? pak->datagramsize : mp->data_bytes;
  
    for (i=0;i<size/4;i++) {
        if ((lp[i]) != decrypt_id++) {
            pas_diag_errmsg(MSG_ERROR, "decrypt data error: expect:0x%08x, got:0x%08x\n",
		decrypt_id-1,lp[i]);
	    pas_diag_errmsg(MSG_ERROR, "%s ring, %s type, buf:0x%08x, size:%d, addr:0x%08x, offset:%d\n",
		    ring ? "Key" : "Frame", pak ? "Pak" : "Particle", 
		    lp, size, &lp[i], i);
	    /*decrypt_id = lp[i]+1;*/
	    ABORT_TEST = TRUE;
	    process_set_boolean(next_test, TRUE);
	    break;
	}
    }
    if (size % 4) decrypt_id++;
    if (!verify_check_byte(p, size)) {
	pas_diag_errmsg(MSG_ERROR, "%s ring, %s type, buf:0x%08x, size:%d\n",
           	ring ? "Key" : "Frame", pak ? "Pak" : "Particle", p, size);
	ABORT_TEST = TRUE;
	process_set_boolean(next_test, TRUE);
    }

}

/*****************************************************************************
 * service_decrypt_traffic - Operate as the output interface fastsend vector
 * that receive the decrypted packet.
 *****************************************************************************/
void service_decrypt_traffic (hwidbtype *idb, paktype *pak)
{
    particletype *mp;
    int bytes = 0;

#if EPA_FIX_PATTERN
    decrypt_id = START_SEQUENCE;
#endif
    stat.num_decrypt_rcv++;
    if (!pak_has_particles(pak)) {
	process_decrypt_data(EPAD_FRAME_DEVICE,pak,NULL);
	bytes = pak->datagramsize;
	if (!ABORT_TEST) {
	    datagram_done(pak);
	}
    } else {
        while ((mp = dequeue_inline(&pak->particlequeue))) {
	    process_decrypt_data(EPAD_FRAME_DEVICE,NULL,mp);
	    bytes += mp->data_bytes;
	    if (!ABORT_TEST) {
		retparticle(mp);
	    }
        }
        datagram_done(pak);
    }
    if (ABORT_TEST) {
	pas_diag_errmsg(MSG_ERROR, "ERROR: found on packet size = %d\n", bytes);
    }
    if (stat.num_decrypt_rcv == num_traffic) {
	process_set_boolean(next_test, TRUE);
    }
}

/*****************************************************************************
 * service_decrypt_traffic - Operate as the output interface fastsend vector
 * that receive a packet. No checking is done here to avoid overhead when
 * measuring through put.
 *****************************************************************************/
void service_encrypt_throughput (hwidbtype *idb, paktype *pak)
{
    particletype *mp;
    hwidbtype *idb2 = epad_gidb[EPAD_FRAME_DEVICE];
 
    stat.num_encrypt_rcv++;
    if (!pak_has_particles(pak)) {
	datagram_done(pak);
    } else {
        while ((mp = dequeue_inline(&pak->particlequeue))) { 
	    retparticle(mp);  
        }
    }
    if (stat.num_encrypt_rcv == num_traffic ||
	((stat.num_encrypt_rcv + idb2->counters.output_total_drops) == num_traffic)) {
	process_set_boolean(next_test, TRUE);
    }
}

/*****************************************************************************
 * service_encrypt_traffic -
 * This routine expected to receive an encrypted packet.  The content of 
 * this packet will be verified against the clear text.  If the encryted
 * data seems ok, this packet will be send down to the EPA again for
 * decryption.
 *****************************************************************************/
void service_encrypt_traffic (hwidbtype *idb, paktype *pak)
{
    particletype *mp;
    particletype *hdr;
    paktype *newpak;
    epad_instance_t *ds = INSTANCE(epad_gidb[0]);
    int bytes = 0;
    int cont=0;
    int count = 0;
  
    stat.num_encrypt_rcv++;
    count = pak_has_particles(pak);
    if ( !pak_has_particles(pak)) {
	process_encrypt_data(EPAD_FRAME_DEVICE,pak,NULL);
	bytes = pak->datagramsize;
	cont = 1;
	pak->if_output = outidb2->firstsw;
	newpak = pak;	
    } else {
        newpak = inpak2;
	while ((mp = dequeue_inline(&pak->particlequeue))) {
	    process_encrypt_data(EPAD_FRAME_DEVICE,NULL,mp);
	    bytes += mp->data_bytes;
	    if (!ABORT_TEST) {		
		particle_enqueue_inline(newpak, mp);
	    }
	}
    }
    if (ABORT_TEST) {
	pas_diag_errmsg(MSG_ERROR, ".... ABORT TEST .....");
	pas_diag_errmsg(MSG_ERROR, "ERROR: found on packet size = %d\n", bytes);
	return;
    }
    /* change the crypto flag to decrypt */
    newpak->cryptoflags = CRYPTO_DECRYPT;

    hdr = epad_create_encryption_blk(epad_gidb[0], DECRYPT, 
				     gsession, giv, 0, newpak);
    if (!hdr) {
	epad_drop_packet(idb, newpak);
	return;
    }
    newpak->unspecified[0]=(ulong)hdr;
    
    if (verbose == 2) {
	pas_diag_errmsg(MSG_PROGRAM, "Sending encrypted %s packet of %d bytes  to frame ring, count = %d\n",
		cont==1? "contiguous" : "segmented", bytes, ds->count);
    }
    /* send the packet to be decrypted */
    send_to_epad(epad_gidb[EPAD_FRAME_DEVICE], newpak);
}

/*****************************************************************************
 * encryption_test_summary - Print out test summary
 *****************************************************************************/
static void encryption_test_summary (int type)
{	
    pas_diag_errmsg(type, 
		    "Sent %d packet(s) to be encrypted, receive %d encrypted packet(s)\n", 
		    stat.num_encrypt_snt, stat.num_encrypt_rcv);
    pas_diag_errmsg(type, 
		    "Sent %d packet(s) to be decrypted, receive %d decrypted packet(s)\n", 
		     stat.num_decrypt_snt, stat.num_decrypt_rcv);
}

/*****************************************************************************
 * wait_test_done
 * Wait for last test to finish before starting the next one
 *****************************************************************************/
static void wait_test_done (void)
{
   
    process_watch_boolean(next_test, ENABLE, ONE_SHOT);
    process_wait_for_event();
    process_set_boolean(next_test, FALSE);
}

/*****************************************************************************
 * ktest_status - Read the status field in the epa command
 *****************************************************************************/
static char ktest_status (paktype *pak, particletype *mp)
{
    epacmd_t *cmdp;
    
    if (pak) {
	cmdp = (epacmd_t *)pak->datagramstart;
    } else {
	cmdp = (epacmd_t *)mp->data_start;
    }
    return (cmdp->status); 
}
/*****************************************************************************
 * dump_buffer - 
 *****************************************************************************/
static void dump_buffer (uchar *buffer, int buffersize)
{
    int i;

    ttyprintf(CONTTY, "\n");
    for (i=0; i < buffersize; i++) {
	if (((i+1)%4==0)) {
	    ttyprintf(CONTTY, "%02x ", *(buffer+i));
	} else if (((i+1)%16==0)) {
	    ttyprintf(CONTTY, "%02x\n", *(buffer+i));
	} else {
	    ttyprintf(CONTTY, "%02x", *(buffer+i));
	}
    }
    ttyprintf(CONTTY, "\n");	
      
}

/*****************************************************************************
 * dump_key_cmd - Will printout the entire content of the epa command 
 * received.  
 *****************************************************************************/
void dump_key_cmd (ushort flags, paktype *pak)
{
    particletype *mp;


    if (!pak_has_particles(pak)) {
	dump_buffer(pak->datagramstart, pak->datagramsize);
	datagram_done(pak);
    } else {
	while ((mp = dequeue_inline(&pak->particlequeue))) {
	    dump_buffer(mp->data_start, mp->data_bytes);
	    retparticle(mp);
	}
    }
    process_set_boolean(next_test, TRUE);
}

/*****************************************************************************
 * create_key_cmd - Create a specified key command.  Key command can
 * either be in paktype or particletype if pak is passed in as NULL.
 *****************************************************************************/
paktype* create_key_cmd (paktype *pak, ushort comm_id, void *input, 
			 uchar *data)
{
    int size=0;
    int input_dsize = 0;
    int output_dsize = 0;
    int totalsize = 0;
    int particle_size = PUBLIC_PARTICLE_BYTES;

    switch (comm_id) {
      case CRYPTO_HMA_ALLOC:
	size = sizeof(hMemAlloc_t);
	break;
 
      case CRYPTO_HMA_FREE:
	size = sizeof(hMemFree_t);
	break;
 
      case CRYPTO_HMA_WRITE:
	size = sizeof(hMemWrite_t);
	input_dsize = ((hMemWrite_t *)input)->size;
	break;
 
      case CRYPTO_HMA_READ:
	size = sizeof(hMemRead_t);
	output_dsize = ((hMemRead_t *)input)->size;
	break;
 
      case CRYPTO_MEM_SYNDROME:
	size = sizeof(hMemSyndrome_t);
	output_dsize = ((hMemSyndrome_t *)input)->len;
	break;

      case CRYPTO_HMA_COPY:
	size = sizeof(hMemCopy_t);
	break;
 
      case CRYPTO_READ_RANDOM:
	size = sizeof(hReadRandom_t);
	output_dsize = ((hReadRandom_t *)input)->size;
	break;
 
      case CRYPTO_GEN_RANDOM:
	size = sizeof(hGenRandom_t);
	break;
 
      case CRYPTO_SHA:
	size = sizeof(hSHA_t);
	input_dsize = ((hSHA_t *)input)->size;
	output_dsize = SHA_HASH_SIZE;
	break;

      case CRYPTO_SHA_INIT:
	size = sizeof(hSHAinit_t);
	/* need to define ctxt */
	/* output_dsize = sizeof(ctxt); */
	break;

      case CRYPTO_SHA_UPDATE:
	size = sizeof(hSHAupdate_t);
	/* need to add sizeof(ctxt) */
	input_dsize = ((hSHAupdate_t *)input)->size; 
	break;

      case CRYPTO_SHA_FINAL:
	size = sizeof(hSHAfinal_t);
	/* need to add sizeof(ctxt) */
	output_dsize = SHA_HASH_SIZE;
	break;

      case CRYPTO_GEN_DSS_PARAMETERS:
	size = sizeof(hGenDSSparameters_t);
	output_dsize = ((hGenDSSparameters_t *)input)->size * 2 + Q_SIZE;
	break;

      case CRYPTO_GET_DSS_PQG:
	size = sizeof(hGetDSSpqg_t);
	output_dsize = ((hGetDSSpqg_t *)input)->size * 2 + Q_SIZE;
	break;

      case CRYPTO_GEN_DSS_NUMBER:
	size = sizeof(hGenDSSnumber_t);
	break;

      case CRYPTO_GEN_DSS_KEY:
	size = sizeof(hGenDSSkey_t);
	output_dsize = ((hGenDSSkey_t *)input)->size;
	break;
 
      case CRYPTO_GEN_DSS_SIGNATURE:
	size = sizeof(hGenDSSsignature_t);
	output_dsize = R_SIZE + S_SIZE;
	break;
 
      case CRYPTO_VER_DSS_SIGNATURE:
	size = sizeof(hVerDSSsignature_t);
	input_dsize = ((hVerDSSsignature_t *)input)->size;
	break;
 
      case CRYPTO_GEN_DH_PAIR:
	size = sizeof(hGenDHpair_t);
	output_dsize = ((hGenDHpair_t *)input)->size;
	break;
 
      case CRYPTO_GET_DH_SHARED:
	size = sizeof(hGetDHsharedNumber_t);
	input_dsize = ((hGetDHsharedNumber_t *)input)->size;
	break;
 
      case CRYPTO_SET_CIPHER_KEY:
	size = sizeof(hSetCipherkey_t);
	break;
 
      case CRYPTO_DH_ONE_TIME_PAD:
	size = sizeof(hDHoneTimePad_t);
	input_dsize = ((hDHoneTimePad_t *)input)->size;
	output_dsize = ((hDHoneTimePad_t *)input)->size;
	break;

      case CRYPTO_GET_DH_KEY:
	size = sizeof(hGetDHkey_t);
	input_dsize = ((hGetDHkey_t *)input)->size; /* modulus & public */
	break;

      case CRYPTO_GET_EPA_STATUS:
	size = sizeof(epaGetStatus_t);
	break;
 
      case CRYPTO_SET_EPA_STATUS:
	size = sizeof(epaSetStatus_t);
	input_dsize = ((epaSetStatus_t *)input)->size;
	break;
 
      case CRYPTO_SET_EPA_PASSWD:
	size = sizeof(epaSetPassword_t);
	input_dsize = ((epaSetPassword_t *)input)->size;
	break;
 
      case CRYPTO_ZEROIZE_EPA:
	size = sizeof(epaZeroize_t);
	break;
 
      case CRYPTO_READ_FLASH:
	size = sizeof(epaReadFlash_t);
	output_dsize = ((epaReadFlash_t *)input)->size;
	break;
 
      case CRYPTO_WRITE_FLASH:
	size = sizeof(epaWriteFlash_t);
	input_dsize = ((epaWriteFlash_t *)input)->size;
	break;
 
      case CRYPTO_SET_RNG_SEED:
	size = sizeof(setRNGseed_t);
	input_dsize = ((setRNGseed_t *)input)->size;
	break;

      case CRYPTO_NEW_SESSION:
	size = sizeof(epaNewSession_t);
	break;
 
      case CRYPTO_FREE_SESSION:
	size = sizeof(epaFreeSession_t);
	break;
 
      case CRYPTO_GET_TIME:
	size = sizeof(epaGetTime_t);
	break;
 
      case CRYPTO_GET_METHODS:
	size = sizeof(epaGetMethods_t);
	break;
	
      case CRYPTO_EPA_DIAG:
	size = sizeof(cyl_epa_diag_t);
	output_dsize = ((cyl_epa_diag_t *)input)->request_length;
	break;
      default:
	pas_diag_errmsg(MSG_ERROR, "command id = 0x%08x is not supported\n", comm_id);
	return NULL;
    }

    totalsize = size+input_dsize+output_dsize;
    if (!pak) { /* use pak type */
	if (!(pak = getbuffer(totalsize))) {
	    return NULL;
	}
	bcopy(input, pak->datagramstart, size);
	if (input_dsize && data) {
	    bcopy(data, pak->datagramstart+size, input_dsize);
	}
	pak->datagramsize = size + input_dsize + output_dsize;
	pak->if_output = outidb->firstsw;
    } else { /* use particles */
	particletype *mp = NULL;
	int i;
	int bytescopy = 0;

	/* copy command */
	if (size > particle_size) {
	    /* 
	     * Assume that the key command without input and output data
	     * always smaller than particle size
	     */
	    pas_diag_errmsg(MSG_ERROR, 
		  "key command (%d) is larger than 1 particle size (%d)\n",
			    size, particle_size);
	    return NULL;
	}
        if (!(mp = epad_get_particle_inline(epad_gidb[1], TRUE))) {
            pas_diag_errmsg(MSG_ERROR, "Out of particles\n");
            return NULL;
        }
	bzero(mp->data_start, size);
	bcopy(input, mp->data_start, size);
	mp->data_bytes  = size;
	/* enqueue the first mp */
	if ((!input_dsize && !output_dsize) ||
	    (size == particle_size)) {
	    particle_enqueue_inline(pak, mp);
	}

	/* copy input */
	if (input_dsize) {
	    int numpar = 0;
	    if (size+input_dsize < particle_size) {
	       bytescopy = input_dsize;
	       numpar = 0;
	    } else {
		bytescopy = particle_size - size;
		input_dsize -= bytescopy;
		numpar = (input_dsize) / particle_size;
		numpar = ((input_dsize) % particle_size) ? 
	             (numpar + 1) : numpar;
	    }
	    if (data) {
		bcopy(data, mp->data_start+size, bytescopy);
		data += bytescopy;
	    } else {
		bzero(mp->data_start+size, bytescopy);
	    }
	    mp->data_bytes += bytescopy;
	    if ((!output_dsize) || (bytescopy == particle_size)) {
		particle_enqueue_inline(pak, mp);
	    }

	    for (i=0; i < numpar; i++) {
		if (!(mp = epad_get_particle_inline(epad_gidb[1], TRUE)) ) {
		    pas_diag_errmsg(MSG_ERROR, "Out of particles\n");
		    return NULL;
		}   
		bytescopy = (i == (numpar-1)) ? 
		    (input_dsize - (i*particle_size)): particle_size;
		if (data) {
		    bcopy(data, mp->data_start, bytescopy);
		    data += bytescopy;
		} else {
		    bzero(mp->data_start, bytescopy);
		}
		mp->data_bytes = bytescopy;
		if (bytescopy == particle_size || !output_dsize) {
		    particle_enqueue_inline(pak, mp);
		}
	    }
	}

	/* reserve space for output */
	if (output_dsize) {
	    if (totalsize <= particle_size) {
		mp->data_bytes += output_dsize;
		particle_enqueue_inline(pak, mp);
	    } else {
		int outlen;
		int numpar = 0;

		outlen = particle_size - bytescopy;
		mp->data_bytes += outlen;
		particle_enqueue_inline(pak, mp);

		/* allocate the rest of particle for the output */
		output_dsize -= outlen;
		numpar = output_dsize / particle_size;
		numpar = (output_dsize % particle_size) ? 
		(numpar + 1) : numpar;
		for (i=0; i < numpar; i++) {
		    if (!(mp = epad_get_particle_inline(epad_gidb[1], TRUE)) ) {
			pas_diag_errmsg(MSG_ERROR, "Out of particles\n");
			return NULL;
		    } 
		    if (i == numpar-1) {
			mp->data_bytes = output_dsize - (i*particle_size);
		    } else {
			mp->data_bytes = particle_size;
		    }
		    particle_enqueue_inline(pak, mp);
		}	
	    }
	}
    }
    return pak;
}

/*****************************************************************************
 * init_ktest_result - Initialize a table with expected test result.
 *****************************************************************************/
static void init_ktest_result (void)
{
    ktest_table[CRYPTO_HMA_COPY][0].exp_status = SUCCESS; /* succeed */
    ktest_table[CRYPTO_HMA_COPY][1].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_HMA_COPY][2].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_HMA_COPY][3].exp_status = ERR_HMA_SIZE;

    ktest_table[CRYPTO_SET_RNG_SEED][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_SET_EPA_PASSWD][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_ZEROIZE_EPA][0].exp_status = SUCCESS;

    ktest_table[CRYPTO_MEM_SYNDROME][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_MEM_SYNDROME][1].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_MEM_SYNDROME][2].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_MEM_SYNDROME][3].exp_status = ERR_INPUT_LEN;
    ktest_table[CRYPTO_MEM_SYNDROME][4].exp_status = ERR_INPUT_VAL;

    ktest_table[CRYPTO_READ_RANDOM][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_READ_RANDOM][1].exp_status = ERR_HMA_SIZE;
    ktest_table[CRYPTO_READ_RANDOM][2].exp_status = SUCCESS;

    ktest_table[CRYPTO_GEN_RANDOM][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GEN_RANDOM][1].exp_status = ERR_HMA_SIZE;
    ktest_table[CRYPTO_GEN_RANDOM][2].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_GEN_RANDOM][3].exp_status = SUCCESS;
    ktest_table[CRYPTO_GEN_RANDOM][4].exp_status = ERR_HMA_SIZE;

    ktest_table[CRYPTO_GEN_DH_PAIR][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GEN_DH_PAIR][1].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_GEN_DH_PAIR][2].exp_status = ERR_DH_LEN;

    ktest_table[CRYPTO_GET_DH_SHARED][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GET_DH_SHARED][1].exp_status = ERR_DH_LEN;
    ktest_table[CRYPTO_GET_DH_SHARED][2].exp_status = ERR_HMA_HANDLE;

    ktest_table[CRYPTO_SET_CIPHER_KEY][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_SET_CIPHER_KEY][1].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_SET_CIPHER_KEY][2].exp_status = ERR_HMA_HANDLE;

    ktest_table[CRYPTO_DH_ONE_TIME_PAD][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_DH_ONE_TIME_PAD][1].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_DH_ONE_TIME_PAD][2].exp_status = ERR_DH_LEN;

    ktest_table[CRYPTO_GET_DH_KEY][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GET_DH_KEY][1].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_GET_DH_KEY][2].exp_status = ERR_DH_LEN;
    ktest_table[CRYPTO_GET_DH_KEY][3].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_GET_DH_KEY][4].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_GET_DH_KEY][5].exp_status = ERR_HMA_SIZE;
    ktest_table[CRYPTO_GET_DH_KEY][6].exp_status = ERR_HMA_SIZE;

    ktest_table[CRYPTO_FREE_SESSION][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_FREE_SESSION][1].exp_status = ERR_HMA_HANDLE;

    ktest_table[CRYPTO_NEW_SESSION][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_NEW_SESSION][1].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_NEW_SESSION][2].exp_status = ERR_METHOD;
    ktest_table[CRYPTO_NEW_SESSION][3].exp_status = SUCCESS;

    ktest_table[CRYPTO_SHA][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_SHA][1].exp_status = ERR_INPUT_LEN;
    ktest_table[CRYPTO_SHA_INIT][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_SHA_UPDATE][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_SHA_FINAL][0].exp_status = SUCCESS;

    ktest_table[CRYPTO_GEN_DSS_PARAMETERS][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GEN_DSS_PARAMETERS][1].exp_status = ERR_ERR_DSS_LEN;
    ktest_table[CRYPTO_GEN_DSS_PARAMETERS][2].exp_status = ERR_ERR_DSS_LEN;

    ktest_table[CRYPTO_GET_DSS_PQG][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GET_DSS_PQG][1].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_GET_DSS_PQG][2].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_GET_DSS_PQG][3].exp_status = ERR_INPUT_VAL;

    ktest_table[CRYPTO_GEN_DSS_NUMBER][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GEN_DSS_NUMBER][1].exp_status = ERR_HMA_HANDLE;

    ktest_table[CRYPTO_GEN_DSS_KEY][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GEN_DSS_KEY][1].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_GEN_DSS_KEY][2].exp_status = ERR_ERR_DSS_LEN;

    ktest_table[CRYPTO_GEN_DSS_SIGNATURE][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GEN_DSS_SIGNATURE][1].exp_status = ERR_HMA_HANDLE;

    ktest_table[CRYPTO_VER_DSS_SIGNATURE][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_VER_DSS_SIGNATURE][1].exp_status = ERR_DSS_SIGNATURE;
    ktest_table[CRYPTO_VER_DSS_SIGNATURE][2].exp_status = ERR_DSS_SIGNATURE;
    ktest_table[CRYPTO_VER_DSS_SIGNATURE][3].exp_status = ERR_DSS_SIGNATURE;
    ktest_table[CRYPTO_VER_DSS_SIGNATURE][4].exp_status = ERR_ERR_DSS_LEN;

    ktest_table[CRYPTO_READ_FLASH][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_WRITE_FLASH][0].exp_status = SUCCESS;

    ktest_table[CRYPTO_GET_TIME][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GET_TIME][1].exp_status = SUCCESS;
    ktest_table[CRYPTO_GET_TIME][2].exp_status = SUCCESS;

    ktest_table[CRYPTO_GET_METHODS][0].exp_status = SUCCESS;

    ktest_table[CRYPTO_HMA_FREE][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_HMA_FREE][1].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_HMA_FREE][2].exp_status = SUCCESS;

    ktest_table[CRYPTO_SET_EPA_STATUS][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_SET_EPA_STATUS][1].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][2].exp_status = ERR_INPUT_LEN;
    ktest_table[CRYPTO_SET_EPA_STATUS][3].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][4].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][5].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][6].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][7].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][8].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][9].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][10].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][11].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][12].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_SET_EPA_STATUS][13].exp_status = SUCCESS;
    ktest_table[CRYPTO_SET_EPA_STATUS][14].exp_status = SUCCESS;
    ktest_table[CRYPTO_SET_EPA_STATUS][15].exp_status = SUCCESS;
    ktest_table[CRYPTO_SET_EPA_STATUS][16].exp_status = SUCCESS;

    ktest_table[CRYPTO_HMA_READ][0].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_HMA_READ][1].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_HMA_READ][2].exp_status = SUCCESS;

    ktest_table[CRYPTO_HMA_WRITE][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_HMA_WRITE][1].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_HMA_WRITE][2].exp_status = ERR_HMA_SIZE;
    ktest_table[CRYPTO_HMA_WRITE][3].exp_status = ERR_ACCESS;
    ktest_table[CRYPTO_HMA_WRITE][4].exp_status = ERR_HMA_HANDLE;
    ktest_table[CRYPTO_HMA_WRITE][5].exp_status = SUCCESS;

    ktest_table[CRYPTO_GET_EPA_STATUS][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_GET_EPA_STATUS][1].exp_status = SUCCESS;
    ktest_table[CRYPTO_GET_EPA_STATUS][2].exp_status = SUCCESS;
    ktest_table[CRYPTO_HMA_ALLOC][0].exp_status = SUCCESS;
    ktest_table[CRYPTO_HMA_ALLOC][1].exp_status = ERR_HMA_ALLOC;
    ktest_table[CRYPTO_HMA_ALLOC][2].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_HMA_ALLOC][3].exp_status = ERR_INPUT_VAL;
    ktest_table[CRYPTO_HMA_ALLOC][4].exp_status = SUCCESS;
    ktest_table[CRYPTO_HMA_ALLOC][5].exp_status = SUCCESS;
    ktest_table[CRYPTO_HMA_ALLOC][6].exp_status = SUCCESS;
    ktest_table[CRYPTO_EPA_DIAG][0].exp_status = SUCCESS;
    /*ktest_table[][] = ;*/

}
uint write_pattern[5] = {0x00112233, 0x44556677, 
                                    0x8899AABB, 0xFFDDEEFF , 0x00112233};
/*****************************************************************************
 * print_pass - Print out test description of the test that has passed.
 *****************************************************************************/
static void print_pass (char cmd_exp, ushort state)
{
    if (verbose) {
	pas_diag_errmsg(MSG_SYSTEM, "%s", 
			ktest_table[(int)cmd_exp][(int)state].desc);
	pas_diag_errmsg(MSG_SYSTEM, "Result: Passed\n\n");
    } else {
	ttyprintf(CONTTY, "..");
    }
    ktest_table[(int)cmd_exp][(int)state].result = 1;
}

/*****************************************************************************
 * print_fail - Print out test description of the test that has failed.
 *****************************************************************************/
static void print_fail (char cmd_exp, ushort state, char cmd_rcv, char status)
{
    pas_diag_errmsg(MSG_ERROR, "%s", ktest_table[(int)cmd_exp][(int)state].desc);
    pas_diag_errmsg(MSG_ERROR, "Result: FAILED\n");
    pas_diag_errmsg(MSG_ERROR, "[Expected cmd = %d, status = %d]\n", cmd_exp, 
	    ktest_table[(int)cmd_exp][(int)state].exp_status); 
    pas_diag_errmsg(MSG_ERROR, "[Received cmd: %d, status = %d]\n\n", 
	    cmd_rcv, status);
    ktest_table[(int)cmd_exp][(int)state].result = 2;
}
/*****************************************************************************
 * verify_ktest_result - Check the key test result with the expected 
 * result.
 *****************************************************************************/
static void verify_ktest_result (char cmd_exp, ushort state, paktype *pak)
{
    char cmd_rcv;
    char status;
    boolean done = TRUE;
    particletype *mp = NULL;
    uchar *buffer;
    int buffersize;
    static int num_free = 0;
    static int num_alloc = 0;

    if (pak_has_particles(pak)) {
	mp = dequeue_inline(&pak->particlequeue);
	status = ktest_status(NULL, mp);
	buffer = mp->data_start;
	buffersize = mp->data_bytes;
    } else {
	status = ktest_status(pak, NULL);
	buffer = pak->datagramstart;
	buffersize = pak->datagramsize;
    }
    cmd_rcv = (char)(*buffer);
    if (ktest_table[(int)cmd_exp][(int)state].exp_status == status && 
	cmd_exp == cmd_rcv) {
	switch(cmd_exp) {
	  case CRYPTO_GEN_DH_PAIR: {
	      if (state == 0) {
		  hGenDHpair_t *output = (hGenDHpair_t *)buffer;
		  char *public;

		  public = (char *) buffer + sizeof(hGenDHpair_t);
		  bcopy(public, gdh_public, output->size);
	      }
	      break;
	  }
	  case CRYPTO_NEW_SESSION: {
	      if (state == 0) {
		  epaNewSession_t *output;
		  output = (epaNewSession_t *)buffer;
		  gsession = output->handle;
	      }
	      break;
	  }
	  case CRYPTO_SHA: {	
	      if (state == 0) {
		  char *hashp;
		  hSHA_t *ptr;
		  ptr = (hSHA_t *)buffer;
		  hashp = (char *)(buffer + sizeof(hSHA_t)+ptr->size);
		  bcopy(hashp, ghash, sizeof(ghash));
	      }
	      break;
	  }
	  case CRYPTO_GEN_DSS_KEY: {
	      if (state == 0) {
		  char *dss_public;
		  hGenDSSkey_t *output;
		  if (mp) {
		      output = (hGenDSSkey_t *)mp->data_start;
		      dss_public = (char *)mp->data_start + sizeof(hGenDSSkey_t);
		  } else {
		      output = (hGenDSSkey_t *)buffer;
		      dss_public = (char *)buffer + sizeof(hGenDSSkey_t);
		  }
		  bcopy(dss_public, gdss_public, output->size);
	      }
	      break;
	  }
	  case CRYPTO_GEN_DSS_SIGNATURE: {
	      if (state == 0) {
		  char *Rptr, *Sptr;
		  Rptr = buffer + sizeof(hGenDSSsignature_t);
		  Sptr = Rptr + sizeof(gR);
		  bcopy(Rptr, gR, sizeof(gR));
		  bcopy(Sptr, gS, sizeof(gS));
	      }
	      break;
	  }
	  case CRYPTO_GET_TIME: {
	      static ulong rtc_time1=0, rtc_time2=0;
	      epaGetTime_t *output;

	      output = (epaGetTime_t *)(buffer);
	      if (state == 0) {
		  rtc_time1 = output->rtc_time;
		  done = FALSE;
		  DELAY(2000);
	      } else if (state == 1) {
		  rtc_time2 = output->rtc_time;
		  if (rtc_time2 <= rtc_time1) {
		      done = FALSE;
		      print_fail(cmd_exp, state, cmd_rcv, status);
		      pas_diag_errmsg(MSG_ERROR, "[rtc_time1 = 0x%x, rtc_time2 = 0x%x\n",
			      rtc_time1, rtc_time2);
		  }
	      } else {
		  ttyprintf(CONTTY, "Real time clock = 0x%x, boot time = 0x%x\n",
			  output->rtc_time, output->boot_time);
		  done = FALSE;
	      }
	      break;
	  }
	    
	  case CRYPTO_GET_METHODS: {
	      int i;
	      epaGetMethods_t *output;
	      char method;

	      output = (epaGetMethods_t *)(buffer);
	      for (i=0; i < 32; i++) {
		  method = output->methods[i];
		  if (method != 0x1 && method != 0x2 && method != 0x3 &&
		      method != 0x4 && method != 0x7 && method != 0xA &&
		      method != 0xb && method != 0xc && method != 0xd &&
		      method != 0x14 && method != 0x15 && method !=0x20 &&
		      method != -1) {	
		      pas_diag_errmsg(MSG_ERROR, "%d: Unsupport Method = %x\n", i, method);
		      break;
		  }
	      }
	      if (i < 32) {
		  print_fail(cmd_exp, state, cmd_rcv, status);
		  dump_buffer(buffer, buffersize);
		  done = FALSE;
	      } 
	      break;
	  }
	  case CRYPTO_SET_EPA_STATUS: {
	      if (state == 0) {
		  epaGetStatus_t iget;
		  paktype *newpak;
		  hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];

		  done = FALSE;
		  /* Now send the command to read the status, make sure that
		   * the EPA_TESTING bit is set
		   */
		  iget.call_id = CRYPTO_GET_EPA_STATUS;
		  newpak = create_key_cmd(inpak2, CRYPTO_GET_EPA_STATUS, &iget, NULL);
		  newpak->cryptoflags = 13;
		  send_to_epad(idb, newpak);
	      }
	      break;
	  }
	  case CRYPTO_HMA_WRITE: {
	      if (state == 0) {
		  hMemRead_t input;
		  paktype *newpak;
		  hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];

		  done = FALSE;
		  input.call_id = CRYPTO_HMA_READ;
		  input.handle = ghandle3;
		  input.size = sizeof(write_pattern);
		  newpak = create_key_cmd(inpak2, CRYPTO_HMA_READ, &input, NULL);
		  newpak->cryptoflags = 5; /* goto read command */
		  sprintf(ktest_table[CRYPTO_HMA_WRITE][newpak->cryptoflags].desc, 
			  ktest_table[CRYPTO_HMA_WRITE][state].desc);
		  send_to_epad(idb, newpak);
	      }
	      break;
	  }
	  case CRYPTO_GET_EPA_STATUS: {
	      if (state == 1) {
		  epaGetStatus_t *get;

		  get = (epaGetStatus_t *)(buffer);
		  ttyprintf(CONTTY, "EPA status:\n");
		  ttyprintf(CONTTY, "  Flags: 0x%08x, Version: 0x%08x\n", 
			  get->flags, get->version);
		  ttyprintf(CONTTY, "  Reset_ctr: 0x%08x, SysError: 0x%08x, error_time: 0x%08x\n", 
			  get->reset_ctr, get->sys_error, get->error_time);
	      } else if (state == 2) {
		  epaGetStatus_t *get;
		  get = (epaGetStatus_t *)(buffer);
		  ttyprintf(CONTTY, "LED: \n");
		  if (get->flags & 0x00000080) {
		      ttyprintf(CONTTY, "  Boot   ON\n");
		  } else {
		      ttyprintf(CONTTY, "  Boot   OFF\n");
		  } 
		  if (get->flags & 0x00000040) {
		      ttyprintf(CONTTY, "  Active ON\n");
		  } else {
		      ttyprintf(CONTTY, "  Active OFF\n");
		  }
		  if (get->flags & 0x00000020) {
		      ttyprintf(CONTTY, "  Error  ON\n");
		  } else {
		      ttyprintf(CONTTY, "  Error  OFF\n");
		  } 
		  if (get->flags & 0x00000010) {
		      ttyprintf(CONTTY, "  Tamper ON\n");
		  } else {
		      ttyprintf(CONTTY, "  Tamper OFF\n");
		  }
	      }
	      break;
	  }
	  case CRYPTO_HMA_ALLOC: {
	      hMemAlloc_t *oalloc;
	      if (state == 0) { /* sucessfully allocate now free it */
		  paktype *newpak;
		  hMemFree_t input;

		  done = FALSE;
		  oalloc = (hMemAlloc_t *)buffer;
		  input.call_id = CRYPTO_HMA_FREE;
		  input.handle = oalloc->handle;
		  newpak = create_key_cmd(inpak2, CRYPTO_HMA_FREE, &input, NULL);
		  newpak->cryptoflags = 6; 
		  sprintf(ktest_table[CRYPTO_HMA_ALLOC][6].desc, "%s",
			  ktest_table[CRYPTO_HMA_ALLOC][state].desc);
		  send_to_epad(epad_gidb[EPAD_KEY_DEVICE], newpak); 
	      } else if (state == 4) { 
		  /* special case to allocate all memory before free */
		  oalloc = (hMemAlloc_t *)buffer;
		  handle_array[num_alloc++] = oalloc->handle;
		  done = FALSE;
	      }
	     
	      break;
	  }
	  default:
	    
	}
	if (done) {
	    print_pass(cmd_exp, state);
	} 
	process_set_boolean(next_test, TRUE); 
	if (mp) {
	    retparticle(mp);
	    epad_cleanup_particles(pak);
	} else {
	    datagram_done(pak);
	}
    } else if (cmd_exp != cmd_rcv) {
	if (cmd_rcv == CRYPTO_HMA_ALLOC && cmd_exp == CRYPTO_HMA_FREE &&
	    state == 0) {
	    hMemFree_t input;
	    hMemAlloc_t *oalloc;
	    paktype *newpak;

	    oalloc = (hMemAlloc_t *)buffer;
	    
	    /* Send command to free the HMA block just allocated*/
	    input.call_id = CRYPTO_HMA_FREE;
	    input.status = 0;
	    input.handle = oalloc->handle;
	    newpak = create_key_cmd(inpak2, CRYPTO_HMA_FREE, &input, NULL);
	    newpak->cryptoflags = 2; 
	    sprintf(ktest_table[CRYPTO_HMA_FREE][2].desc, 
	    "Test 2: Request to free an allocated HMA memory\n");
	    send_to_epad(epad_gidb[EPAD_KEY_DEVICE], newpak);
	} else if (cmd_rcv == CRYPTO_GET_EPA_STATUS && 
		   cmd_exp == CRYPTO_SET_EPA_STATUS && state == 13) {
	    epaGetStatus_t *get;
	    get = (epaGetStatus_t *)buffer;	   
	    /* Check to see if testing flag is off => 1 */
	    if (get->flags & 0x20000000) {
		print_pass(cmd_exp, state);
	    } else {
		print_fail(cmd_exp, state, cmd_rcv, status);
		dump_buffer(buffer, buffersize);
	    }
	    process_set_boolean(next_test, TRUE);
	} else if (cmd_rcv == CRYPTO_HMA_READ && cmd_exp == CRYPTO_HMA_WRITE &&
		   state == 5) {
	    int size = sizeof(hMemRead_t);
	    if (!bcmp(buffer+size, write_pattern, 
		      buffersize - size)) {
		print_pass(cmd_exp, state);
	    } else {
		print_fail(cmd_exp, state, cmd_rcv, status);
		dump_buffer(buffer, buffersize);
	    }
	    process_set_boolean(next_test, TRUE);
	} else if (cmd_rcv == CRYPTO_HMA_FREE && cmd_exp == CRYPTO_HMA_ALLOC
		   && state == 5) {
	    if (status == 0) {
		num_free++;
	    } else {
		print_fail(cmd_exp, state, cmd_rcv, status);
		process_set_boolean(next_test, TRUE);
		num_free = 0;
		num_alloc = 0;
	    }
	    if (num_alloc == num_free) {
		print_pass(cmd_exp, state);
		process_set_boolean(next_test, TRUE);
		num_free = 0;
		num_alloc = 0;
	    }
	} else if (cmd_rcv == CRYPTO_HMA_FREE && cmd_exp == CRYPTO_HMA_ALLOC
		   && state == 6) {
	    if (status == 0) {
		print_pass(cmd_exp, state);
	    } else {
		print_fail(cmd_exp, state, cmd_rcv, status);
	    }	 
	} else {
	    print_fail(cmd_exp, state, cmd_rcv, status);
	    dump_buffer(buffer, buffersize);
	    process_set_boolean(next_test, TRUE);
	}
	if (mp) {
	    retparticle(mp);
	    epad_cleanup_particles(pak);
	} else {
	    datagram_done(pak);
	}
    } else {
	if (cmd_rcv == CRYPTO_HMA_ALLOC && state == 4 ) {
	    hMemAlloc_t *oalloc;
	    oalloc = (hMemAlloc_t *)pak->datagramstart;
	    if (oalloc->size == MAX_ALLOC) {
		num_alloc++;
		num_free++;
		done = FALSE;
		process_set_boolean(next_test, TRUE);
		/* start deallocate all hma, that were sucessfully allocated */
	    }
	    done = FALSE;
	}
	
	if (done) {
	    print_fail(cmd_exp, state, cmd_rcv, status);
	    dump_buffer(buffer, buffersize);
	    process_set_boolean(next_test, TRUE);
	}
	if (mp) {
	    retparticle(mp);
	    epad_cleanup_particles(pak);
	} else {
	    datagram_done(pak);
	}	
	if (cmd_rcv == CRYPTO_HMA_WRITE && state == 0) {
	    /* in case test 0 fail it can go on to next test */
	    process_set_boolean(next_test, TRUE);
	}
    } 
}

/*****************************************************************************
 * service_global_alloc - Expect to receive good status on create global
 * hma alloc.  These hma will be used through out the key test.
 *****************************************************************************/
static void service_global_alloc (ushort state, paktype *pak)
{
    char cmd_id;
    hMemAlloc_t *oalloc;
    uchar *buffer = NULL;
    int buffersize;
    char status;
    particletype *mp = NULL;

    if (verbose == 2) {
	pas_diag_errmsg(MSG_PROGRAM, "service_global_alloc: state = %d\n", 
			state);
    }
    if (pak_has_particles(pak)) {
	mp = dequeue_inline(&pak->particlequeue);
	buffer = mp->data_start;
	buffersize = mp->data_bytes;
	status = ktest_status(NULL, mp);
    } else {
	buffer = pak->datagramstart;
	buffersize = pak->datagramsize;
	status = ktest_status(pak, NULL);
    }
    cmd_id = (char)(*buffer);
    switch(state) {
      case 1:
      case 2: /* alloc 2 global handle use later for testing */
      case 3:
      case 4:
      case 5:
	if (cmd_id != CRYPTO_HMA_ALLOC) {
	    pas_diag_errmsg(MSG_PROGRAM,"service_global_alloc %d: unexpected cmd_id %d\n",
		    state, cmd_id);
	    ABORT_TEST = TRUE;
	    process_set_boolean(next_test, TRUE);
	} else if (status == 0) {
	    oalloc = (hMemAlloc_t *)buffer;
	    switch (state) {
	      case 1:	    
	        ghandle1 = oalloc->handle;
		break;
	      case 2:
	        ghandle2 = oalloc->handle;
		break;
	      case 3:
		ghandle3 = oalloc->handle;
		break;
	      case 4:
		ghandle4 = oalloc->handle;
		break;
	      case 5:
		ghandle5 = oalloc->handle;
		process_set_boolean(next_test, TRUE);
		break;
	    }
	    if (verbose == 2) {
		pas_diag_errmsg(MSG_PROGRAM, "alloc global handle %d: size = %d, attribute = 0x%x, handle = %d\n", 
		    state, oalloc->size, oalloc->attribute, oalloc->handle);
	    }
	}
	break;
      default:
	pas_diag_errmsg(MSG_ERROR, 
		     "service global_alloc: state %d not supported\n", state);
	ABORT_TEST = TRUE;
	process_set_boolean(next_test, TRUE);
    }
    if (mp) {
	retparticle(mp);
    } else {
	datagram_done(pak);
    }
}

/*****************************************************************************
 * alloc_global_handle - Allocate some hma memory to be use in all of
 * the key test.  We done this to eliminate having to allocate and reallocate
 * hma memory for each test.
 *****************************************************************************/
static void alloc_global_handle(void)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    hMemAlloc_t ialloc;
    paktype *pak;

    if (ABORT_TEST) return;
    ds->callback = service_global_alloc;
    /**** Allocate 2 hma memory to use through out this test ****/
    /**** ghandle1 ****/
    ialloc.call_id = CRYPTO_HMA_ALLOC;
    ialloc.size = GHANDLE1_SIZE;
    ialloc.attribute = 0x10; /* read access allow */
    pak = create_key_cmd(inpak, CRYPTO_HMA_ALLOC, &ialloc, NULL);
    pak->cryptoflags = 1; /* ghandle */
    send_to_epad(idb, pak);

    /**** ghandle2 ****/
    ialloc.size = GHANDLE2_SIZE; 
    ialloc.attribute = 0x20; /* write access allow */
    pak = create_key_cmd(inpak, CRYPTO_HMA_ALLOC, &ialloc, NULL);
    pak->cryptoflags = 2; /* ghandle 2 */
    send_to_epad(idb, pak);

    /**** ghandle 3 ****/
    ialloc.size = GHANDLE3_SIZE; 
    ialloc.attribute = 0x20|0x10; /* write and read access allow */
    pak = create_key_cmd(inpak, CRYPTO_HMA_ALLOC, &ialloc, NULL);
    pak->cryptoflags = 3; /* ghandle 3 */
    send_to_epad(idb, pak);

    /**** ghandle 4 ****/
    ialloc.size = GHANDLE4_SIZE; 
    ialloc.attribute = 0x20|0x10; /* write and read access allow */
    pak = create_key_cmd(inpak, CRYPTO_HMA_ALLOC, &ialloc, NULL);
    pak->cryptoflags = 4; /* ghandle 4*/
    send_to_epad(idb, pak);

    /**** ghandle 5 ****/
    ialloc.size = GHANDLE5_SIZE; 
    ialloc.attribute = 0x20|0x10; /* write and read access allow */
    pak = create_key_cmd(inpak, CRYPTO_HMA_ALLOC, &ialloc, NULL);
    pak->cryptoflags = 5; /* ghandle 5*/
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_global_free - service a response to a request to free global
 * hma memory.
 *****************************************************************************/
static void service_global_free (ushort state, paktype *pak)
{
    char cmd_id;
    uchar *buffer = NULL;
    int buffersize;
    char status;
    particletype *mp = NULL;

    if (verbose == 2) {
	pas_diag_errmsg(MSG_PROGRAM, "service_global free: state = %d\n", state);
    }
    if (pak_has_particles(pak)) {
	mp = dequeue_inline(&pak->particlequeue);
	buffer = mp->data_start;
	buffersize = mp->data_bytes;
	status = ktest_status(NULL, mp);
    } else {
	buffer = pak->datagramstart;
	buffersize = pak->datagramsize;
	status = ktest_status(pak, NULL);
    }
    cmd_id = (char)(*buffer);
    switch(state) {
      case 1:
      case 2: /* free global handle */
      case 3:
      case 4:
      case 5:
	if (cmd_id != CRYPTO_HMA_FREE) {
	    pas_diag_errmsg(MSG_ERROR,
			    "service_global free%d: unexpected cmd_id %d\n",
			    state, cmd_id);
	    ABORT_TEST = TRUE;
	    process_set_boolean(next_test, TRUE);
	} else if (status == 0) {
	    switch (state) {
	      case 1:
		if (verbose == 2)
		    pas_diag_errmsg(MSG_PROGRAM, "free global handle%d = %d\n", state, 
			    ghandle1);
		ghandle1 = 0;
		break;
	      case 2:
		if (verbose == 2) 
		    pas_diag_errmsg(MSG_PROGRAM, "free global handle%d = %d\n", state, 
			    ghandle2);
		ghandle2 = 0;
		break;
	      case 3:
		if (verbose == 2)
		    pas_diag_errmsg(MSG_PROGRAM, "free global handle%d = %d\n", state, ghandle3);
		ghandle3 = 0;
		break;
	      case 4:
		if (verbose == 2)
		    pas_diag_errmsg(MSG_PROGRAM, "free global handle%d = %d\n", state, ghandle4);
		ghandle4 = 0;
		break;
	      case 5:
		if (verbose == 2)
		    pas_diag_errmsg(MSG_PROGRAM, "free global handle%d = %d\n", state, ghandle5);
		ghandle5 = 0;
		process_set_boolean(next_test, TRUE);
		break;
	    }
	
	}
	break;
      default:
	pas_diag_errmsg(MSG_ERROR, "service global_free: state %d not supported\n", state);
	ABORT_TEST = TRUE;
	process_set_boolean(next_test, TRUE);
    }
    if (mp) {
	retparticle(mp);
    } else {	
	datagram_done(pak);
    }
}

/*****************************************************************************
 * free_global_handle - Request to free globle hma memory
 *****************************************************************************/
static void free_global_handle(void)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    hMemFree_t ifree;
    paktype *pak;

    if (ABORT_TEST) return;
    ds->callback = service_global_free;
    /**** Allocate 2 hma memory to use through out this test ****/
    /**** ghandle1 ****/
    ifree.call_id = CRYPTO_HMA_FREE;
    ifree.handle = ghandle1;
    pak = create_key_cmd(inpak, CRYPTO_HMA_FREE, &ifree, NULL);
    pak->cryptoflags = 1; /* ghandle */
    send_to_epad(idb, pak);

    /**** ghandle2 ****/
    ifree.handle = ghandle2;
    pak = create_key_cmd(inpak, CRYPTO_HMA_FREE, &ifree, NULL);
    pak->cryptoflags = 2; /* ghandle 2 */
    send_to_epad(idb, pak);

    /**** ghandle3 ****/
    ifree.handle = ghandle3;
    pak = create_key_cmd(inpak, CRYPTO_HMA_FREE, &ifree, NULL);
    pak->cryptoflags = 3; /* ghandle 3 */
    send_to_epad(idb, pak);

    /**** ghandle4 ****/
    ifree.handle = ghandle4;
    pak = create_key_cmd(inpak, CRYPTO_HMA_FREE, &ifree, NULL);
    pak->cryptoflags = 4; /* ghandle 4 */
    send_to_epad(idb, pak);

    /**** ghandle5 ****/
    ifree.handle = ghandle5;
    pak = create_key_cmd(inpak, CRYPTO_HMA_FREE, &ifree, NULL);
    pak->cryptoflags = 5; /* ghandle 5 */
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_epaSetStatus - 
 * Service a response to a request to set epa status.
 *****************************************************************************/
static void service_ktest_epaSetStatus (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_SET_EPA_STATUS, state, pak);
}

/*****************************************************************************
 * service_ktest_epaSetPassword -
 * Service a response from a request to set epa password
 *****************************************************************************/
static void service_ktest_epaSetPassword (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_SET_EPA_PASSWD, state, pak);
}

/*****************************************************************************
 * set passwd -
 * Set epa passwd to a predefined password "CYCORE" for testing
 *****************************************************************************/
static void set_passwd (void)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    epaSetPassword_t isetpasswd;
    paktype *pak;

    ds->callback = service_ktest_epaSetPassword;
    sprintf(ktest_table[CRYPTO_SET_EPA_PASSWD][0].desc, 
	    "\nSet passwd\n");
    isetpasswd.call_id = CRYPTO_SET_EPA_PASSWD;
    isetpasswd.size = sizeof(passwd);
    pak = create_key_cmd(inpak, CRYPTO_SET_EPA_PASSWD, &isetpasswd, 
			 (uchar *)passwd);
    pak->cryptoflags = 0;
    send_to_epad(idb, pak);
    wait_test_done();

}
/*****************************************************************************
 * epaSetStatus - 
 * Send a request to set epa status
 *****************************************************************************/
static void epaSetStatus (ushort state, ushort size, ulong mask, ulong flags,
			  uchar *pword)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epaSetStatus_t isetstatus;
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;

    ds->callback = service_ktest_epaSetStatus;
    isetstatus.call_id = CRYPTO_SET_EPA_STATUS;
    isetstatus.size = size;
    isetstatus.mask = mask;
    isetstatus.flags = flags;
    pak = create_key_cmd(inpak, CRYPTO_SET_EPA_STATUS, &isetstatus, 
			 pword);    
    pak->cryptoflags = state; 
    send_to_epad(idb, pak);
    wait_test_done();
}
/*****************************************************************************
 * clear_tamper
 * Request to clear epa tamper bit.
 *****************************************************************************/
static void clear_tamper (void)
{
   
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][14].desc, "Clear tamper\n");
    epaSetStatus(14, sizeof(passwd), 0x80000000, 0, (uchar *)passwd);
}

/*****************************************************************************
 * clear_xtract
 * Request to clear extract bit.
 *****************************************************************************/
static void clear_xtract (void)
{
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][15].desc, "\nClear xtract\n");
    epaSetStatus(15, sizeof(passwd), 0x40000000, 0, (uchar *)passwd); 
}

/*****************************************************************************
 * set priviledge
 * Request to change epa to privilegde mode.
 *****************************************************************************/
static void set_priviledge (void)
{
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][16].desc, "\nSet priviledge\n");
    epaSetStatus(16, sizeof(passwd), 0x10000000, 0x10000000, (uchar *)passwd);
}

/*****************************************************************************
 * service_ktest_epaZeroize
 * Service a response to request clear epa memory
 *****************************************************************************/
static void service_ktest_epaZeroize (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_ZEROIZE_EPA, state, pak);
}

/*****************************************************************************
 * service_ktest_setRNGseed
 * Service a response from epa to set random seed.
 *****************************************************************************/
static void service_ktest_setRNGseed(ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_SET_RNG_SEED, state, pak);
}

/*****************************************************************************
 * set_randomseed
 * Send a request to set random seed.
 *****************************************************************************/
static void set_randomseed (void)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    setRNGseed_t iseed;
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    ushort rnd = rand16();

    /* set random seed */
    ds->callback = service_ktest_setRNGseed;
    iseed.call_id = CRYPTO_SET_RNG_SEED;
    iseed.size = sizeof(ushort); 
    pak = create_key_cmd(inpak, CRYPTO_SET_RNG_SEED, &iseed, (uchar *)(&rnd));
    pak->cryptoflags = 0; 
    sprintf(ktest_table[CRYPTO_SET_RNG_SEED][pak->cryptoflags].desc,
	    "Set random seed\n");
    send_to_epad(idb, pak); 
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_epaGetStatus
 * Service reply in response of a request to get epa status.
 *****************************************************************************/
static void service_ktest_epaGetStatus (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GET_EPA_STATUS, state, pak);
}

/*****************************************************************************
 * service_ktest_epaGetTime
 * Service a reply in response of a request to get time from epa.
 *****************************************************************************/
static void service_ktest_epaGetTime (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GET_TIME, state, pak);
}

/*****************************************************************************
 * service_ktest_epaGetMethods
 * Service a reply in response of a request to get all the methods that
 * support by the epa.
 *****************************************************************************/
static void service_ktest_epaGetMethods (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GET_METHODS, state, pak);
}

/*****************************************************************************
 * service_ktest_hMemAlloc
 * Service a reply in response of a request to allocate hma memory.
 *****************************************************************************/
void service_ktest_hMemAlloc(ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_HMA_ALLOC, state, pak);
}

/*****************************************************************************
 * hMemAlloc -
 * Send a  command to allocate hma memory
 *****************************************************************************/
static void hMemAlloc (ushort flags, ushort size, char attribute)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    hMemAlloc_t input;
    paktype *pak;

    ds->callback = service_ktest_hMemAlloc;
    input.call_id = CRYPTO_HMA_ALLOC;
    input.size = size;
    input.attribute = attribute;
    pak = create_key_cmd(inpak, CRYPTO_HMA_ALLOC, &input, NULL);
    pak->cryptoflags = flags;
    send_to_epad(idb,pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hMemFree
 * Service a reply in response of a request to free hma memory.
 *****************************************************************************/
void service_ktest_hMemFree(ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_HMA_FREE, state, pak);
}

/*****************************************************************************
 * hMemFree -
 * Send a command to free hma memory.
 *****************************************************************************/
static void hMemFree (ushort flags, short handle)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hMemFree_t input;

    ds->callback = service_ktest_hMemFree;
    input.call_id = CRYPTO_HMA_FREE;
    input.handle = handle;
    pak = create_key_cmd(inpak, CRYPTO_HMA_FREE, &input, NULL);
    pak->cryptoflags = flags;
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hMemWrite-
 * Service a reply in response to a request to write to hma memory.
 *****************************************************************************/
static void service_ktest_hMemWrite (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_HMA_WRITE, state, pak);
}

/*****************************************************************************
 * hMemWrite -
 * Send a command to write hma memory
 *****************************************************************************/
static void hMemWrite (ushort flags, short handle, ushort size, uchar *buffer)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hMemWrite_t input;

    ds->callback = service_ktest_hMemWrite;
    input.call_id = CRYPTO_HMA_WRITE;
    input.handle = handle;
    input.size = size;
    pak = create_key_cmd(inpak, CRYPTO_HMA_WRITE, &input, buffer);    
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hMemRead
 * Service a reply in response of a request to read hma epa memory.
 *****************************************************************************/
static void service_ktest_hMemRead (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_HMA_READ, state, pak);
}

/*****************************************************************************
 * hMemRead -
 * Send a command to read hma memory
 *****************************************************************************/
static void hMemRead (ushort flags, short handle, ushort size)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hMemRead_t input;

    ds->callback = service_ktest_hMemRead;
    input.call_id = CRYPTO_HMA_READ;
    input.size = size;
    input.handle = handle;
    pak = create_key_cmd(inpak, CRYPTO_HMA_READ, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hMemCopy
 * Service a reply to a response of a request to copy hma epa memory.
 *****************************************************************************/
static void service_ktest_hMemCopy (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_HMA_COPY, state, pak);
}

/*****************************************************************************
 * hMemCopy -
 * Send a request to copy hma memory.
 *****************************************************************************/
static void hMemCopy (ushort flags, ushort size, short src_handle, 
		      short dst_handle)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hMemCopy_t input;

    ds->callback = service_ktest_hMemCopy;

    input.call_id = CRYPTO_HMA_COPY;
    input.size = size;
    input.src_handle = src_handle;
    input.dst_handle = dst_handle;
    pak = create_key_cmd(inpak, CRYPTO_HMA_COPY, &input, NULL);
    pak->cryptoflags = flags;
    send_to_epad(idb, pak);
    
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hMemSyndrome
 * Service a reply in response of a memory syndrome command.
 *****************************************************************************/
static void service_ktest_hMemSyndrome (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_MEM_SYNDROME, state, pak);
}

/*****************************************************************************
 * hMemSyndrome
 * Send a syndrome command to epa.
 *****************************************************************************/
static void hMemSyndrome (ushort flags, short handle, ushort offset,
			  ushort size, ushort len)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hMemSyndrome_t input;

    ds->callback = service_ktest_hMemSyndrome;

    input.call_id = CRYPTO_MEM_SYNDROME;	
    input.handle = handle;
    input.offset = offset;
    input.size = size;	
    input.len = len;
    pak = create_key_cmd(inpak, CRYPTO_MEM_SYNDROME, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hReadRandom -
 * Service a reply in response of a request to read random number
 *****************************************************************************/
static void service_ktest_hReadRandom (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_READ_RANDOM, state, pak);
}

/*****************************************************************************
 * hReadRandom-
 * Send a request to read random number to epa.  And wait to receive a
 * response from the EPA before exit this function.
 *****************************************************************************/
static void hReadRandom (ushort flags, ushort size)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hReadRandom_t input;

    ds->callback = service_ktest_hReadRandom;
    input.call_id = CRYPTO_READ_RANDOM;
    input.size = size;
    pak = create_key_cmd(inpak, CRYPTO_READ_RANDOM, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak); 
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hGenRandom -
 * Service a reply in response to a request to generate random number.
 *****************************************************************************/
static void service_ktest_hGenRandom (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GEN_RANDOM, state, pak);
}
/*****************************************************************************
 * generate_random -
 * Send a request to generate random number to the EPA.  And wait to receive a
 * response from the EPA before exit this function.
 *****************************************************************************/
static void hGenRandom (ushort flags, ushort size, short handle)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hGenRandom_t input;

    ds->callback = service_ktest_hGenRandom;
    input.call_id = CRYPTO_GEN_RANDOM;
    input.size = size;
    input.handle = handle;
    pak = create_key_cmd(inpak, CRYPTO_GEN_RANDOM, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak); 
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_gendhpair
 * Service a reply in response to a request to genenrate diffie Hellman pair
 *****************************************************************************/
static void service_ktest_gendhpair (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GEN_DH_PAIR, state, pak);
}

/*****************************************************************************
 * gendhpair -
 * Send a request to generate Diffie Hellman pair to the EPA.  
 * And wait to receive a response from the EPA before exit this function.
 *****************************************************************************/
static void gendhpair(ushort flags, short secret, ushort size)
{
   hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
   epad_instance_t *ds = INSTANCE(idb);
   hGenDHpair_t input;
   paktype *pak;

   ds->callback = service_ktest_gendhpair;
   input.call_id = CRYPTO_GEN_DH_PAIR;
   input.secret = secret;
   input.size = size;
   pak = create_key_cmd(inpak, CRYPTO_GEN_DH_PAIR, &input, NULL);
   pak->cryptoflags = flags;
   send_to_epad(idb, pak);
   wait_test_done();
}

/*****************************************************************************
 * service_ktest_gendhshared
 * Service a reply in response of a request to get Diffie Hellman Shared
 * number.
 *****************************************************************************/
static void service_ktest_getdhshared (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GET_DH_SHARED, state, pak);
}

/*****************************************************************************
 * getdhshared -
 * Send a request to get Diffie Hellman Shared Number to the EPA. 
 * And wait to receive a response from the EPA before exit this function.
 *****************************************************************************/
static void getdhshared (ushort flags, short secret, short shared, 
			 ushort size, char *public)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    hGetDHsharedNumber_t input;
    paktype *pak;

    ds->callback = service_ktest_getdhshared;
    input.call_id = CRYPTO_GET_DH_SHARED;
    input.secret = secret;
    input.shared = shared;
    input.size = size;
    pak = create_key_cmd(inpak, CRYPTO_GET_DH_SHARED, &input, public);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hSetCipherkey -
 * Service a reply in response of a request to set Cipher key to the EPA.
 *****************************************************************************/
static void service_ktest_hSetCipherkey (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_SET_CIPHER_KEY, state, pak);
}

/*****************************************************************************
 * setCipherkey -
 * Send a request to set Cipher key to the EPA.
 * And wait to receive a response from the EPA before exit this function.
 *****************************************************************************/
static void setCipherkey (ushort flags, ushort size, short shared, short key,
			  short offset, short method)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    hSetCipherkey_t input;
    paktype *pak;

    ds->callback = service_ktest_hSetCipherkey;
    input.call_id = CRYPTO_SET_CIPHER_KEY;
    input.size = size;
    input.shared = shared;
    input.key = key;
    input.offset = offset;
    input.method = method;
    pak = create_key_cmd(inpak, CRYPTO_SET_CIPHER_KEY, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hDHoneTimePad
 * Service a reply in response of a command Diffie Hellman One time PAD.
 *****************************************************************************/
static void service_ktest_hDHoneTimePad (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_DH_ONE_TIME_PAD, state, pak);
}

/*****************************************************************************
 * hDHoneTimePad -
 * Send a command DH One Time Pad to the epa.
 * And wait to receive a response from the EPA before exit this function.
 *****************************************************************************/
static void hDHoneTimePad (ushort flags, ushort size, ushort shared)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hDHoneTimePad_t input;
    
    ds->callback = service_ktest_hDHoneTimePad;
    input.call_id = CRYPTO_DH_ONE_TIME_PAD;    
    input.shared = shared;
    input.size = size;
    pak = create_key_cmd(inpak, CRYPTO_DH_ONE_TIME_PAD, &input, NULL);
    pak->cryptoflags = flags;
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hGetDHkey - 
 * Service a reply in response to a request to get Diffie Hellman key.
 *****************************************************************************/
static void service_ktest_hGetDHkey (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GET_DH_KEY, state, pak);
}

/*****************************************************************************
 * getDHkey -
 * Send a request to get Diffie Hellman key to the epa.
 *****************************************************************************/
static void getDHkey (ushort flags, ushort size, short secret, 
		      ushort key1size, short key1,
		      ushort key2size, short key2, char *public)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    hGetDHkey_t input;
    paktype *pak;

    ds->callback = service_ktest_hGetDHkey;
    input.call_id = CRYPTO_GET_DH_KEY;

    input.secret = secret;
    input.size = size;
    input.key1size = key1size;
    input.key1 = key1;
    input.key2size = key2size;
    input.key2 = key2;
    pak = create_key_cmd(inpak, CRYPTO_GET_DH_KEY, &input, gdh_public);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_freesession -
 * Service a reply in response to a request to free a session.
 *****************************************************************************/
static void service_ktest_freesession (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_FREE_SESSION, state, pak);
}

/*****************************************************************************
 * freeSession -
 * Send a free session command to the epa
 *****************************************************************************/
static void freeSession (ushort flags, short handle)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    epaFreeSession_t input;

    ds->callback = service_ktest_freesession;
    input.call_id = CRYPTO_FREE_SESSION;
    input.handle = handle;
    pak = create_key_cmd(inpak, CRYPTO_FREE_SESSION, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_newsession -
 * Service a reply in response to a request to create new session.
 *****************************************************************************/
static void service_ktest_newsession (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_NEW_SESSION, state, pak);
}

/*****************************************************************************
 * newSession -
 * Send a request to create new session to the epa.
 *****************************************************************************/
static void newSession (ushort flags, short iv, short key, ushort method)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    epaNewSession_t input;

    ds->callback = service_ktest_newsession;
    input.call_id = CRYPTO_NEW_SESSION;

    input.iv = iv;
    input.key = key;
    input.method = method;
    pak = create_key_cmd(inpak, CRYPTO_NEW_SESSION, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hSHA -
 * Service a reply in response to a request to create a secure hash.
 *****************************************************************************/
static void service_ktest_hSHA (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_SHA, state, pak);
}

/*****************************************************************************
 * hSHA -
 * Send a request to create a secure hash to the epa.
 *****************************************************************************/
static void hSHA (ushort flags, ushort size, char *message)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hSHA_t input;
 
    ds->callback = service_ktest_hSHA;
    input.call_id = CRYPTO_SHA;
    input.size = size;
    pak = create_key_cmd(inpak, CRYPTO_SHA, &input, message);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hSHAinit
 * Service a reply in response of a request to initialize secure hash.
 *****************************************************************************/
static void service_ktest_hSHAinit (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_SHA_INIT, state, pak);
}

/*****************************************************************************
 * service_ktest_hSHAupdate
 * Service a reply in response of a request to update secure hash.
 *****************************************************************************/
static void service_ktest_hSHAupdate (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_SHA_UPDATE, state, pak);
}

/*****************************************************************************
 * service_ktest_hSHAfinal
 * Service a reply in response of a request to finalize the secure hash.
 *****************************************************************************/
static void service_ktest_hSHAfinal (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_SHA_FINAL, state, pak);
}

/*****************************************************************************
 * service_ktest_hGenDSSparameters
 * Service a reply in response of a request to generate Digital Signature  
 * Standard.
 *****************************************************************************/
static void service_ktest_hGenDSSparameters (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GEN_DSS_PARAMETERS, state, pak);
}

/*****************************************************************************
 * hGenDSSparameters -
 * Send a request to generate Digital Signature Standard.
 *****************************************************************************/
static void hGenDSSparameters (ushort flags, ushort size)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hGenDSSparameters_t input;

    ds->callback = service_ktest_hGenDSSparameters;
    input.call_id = CRYPTO_GEN_DSS_PARAMETERS;
    input.size = size;
    pak = create_key_cmd(inpak, CRYPTO_GEN_DSS_PARAMETERS, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hGetDSSpqg
 * Service a reply in response of a request to get Digital Signature Standard
 * p, q, g parameters.
 *****************************************************************************/
static void service_ktest_hGetDSSpqg (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GET_DSS_PQG, state, pak);
}

/*****************************************************************************
 * hGetDSSpqg -
 * Send a request to get Digital Signature Standard p, q, g parameters.
 *****************************************************************************/
static void hGetDSSpqg (ushort flags, ushort size)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hGetDSSpqg_t input;

    ds->callback = service_ktest_hGetDSSpqg;
    input.call_id = CRYPTO_GET_DSS_PQG;
    input.size = size;

/*
    if (flags == 3) {
	pak = create_key_cmd(NULL, CRYPTO_GET_DSS_PQG, &input, NULL);
    } else {
*/
	pak = create_key_cmd(inpak, CRYPTO_GET_DSS_PQG, &input, NULL);
/*
    }
*/
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hGenDSSnumber -
 * Service a reply in response to a request to generate Digital Signature
 * Standard number.
 *****************************************************************************/
static void service_ktest_hGenDSSnumber (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GEN_DSS_NUMBER, state, pak);
}

/*****************************************************************************
 * hGenDSSnumber -
 * Send a request to generate Digital Signature Standard number to the EPA.
 *****************************************************************************/
static void hGenDSSnumber (ushort flags, short handle)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hGenDSSnumber_t input;

    ds->callback = service_ktest_hGenDSSnumber;
    input.call_id = CRYPTO_GEN_DSS_NUMBER;
    input.handle = handle;
    pak = create_key_cmd(inpak, CRYPTO_GEN_DSS_NUMBER, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hGenDSSkey
 * Service a reply in response of a request to generate DSS key.
 *****************************************************************************/
static void service_ktest_hGenDSSkey (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GEN_DSS_KEY, state, pak);
}

/*****************************************************************************
 * hGenDSSkey -
 * Send a request to generate DSS key to the EPA.
 *****************************************************************************/
static void hGenDSSkey (ushort flags, short secret, ushort size)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hGenDSSkey_t input;

    ds->callback = service_ktest_hGenDSSkey;
    input.call_id = CRYPTO_GEN_DSS_KEY;
    input.secret = secret;
    input.size = size;
    pak = create_key_cmd(inpak, CRYPTO_GEN_DSS_KEY, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hGenDSSsignature -
 * Service a reply in response of a request to generate DSS signature.
 *****************************************************************************/
static void service_ktest_hGenDSSsignature (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_GEN_DSS_SIGNATURE, state, pak);
}

/*****************************************************************************
 * hGenDSSsignature
 * Send a request to generate DSS signature to the EPA.
 *****************************************************************************/
static void hGenDSSsignature (ushort flags, short secret, ushort size,
				   uchar *hash) 
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hGenDSSsignature_t input;

    ds->callback = service_ktest_hGenDSSsignature;
    input.call_id = CRYPTO_GEN_DSS_SIGNATURE;
    input.secret = secret;
    input.size = size;
    bcopy(hash, input.hash, SHA_HASH_SIZE);
    pak = create_key_cmd(inpak, CRYPTO_GEN_DSS_SIGNATURE, &input, NULL);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_hVerDSSsignature
 * Service a reply in response to a request to verify DSS signature.
 *****************************************************************************/
static void service_ktest_hVerDSSsignature (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_VER_DSS_SIGNATURE, state, pak);
}

/*****************************************************************************
 * hverDSSsignature
 *****************************************************************************/
static void hverDSSsignature (ushort flags, ushort size, uchar *hash,
			      uchar *r, uchar *s, uchar *public)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hVerDSSsignature_t input;

    ds->callback = service_ktest_hVerDSSsignature;
    input.call_id = CRYPTO_VER_DSS_SIGNATURE;
    if (hash) {
	bcopy(hash, input.hash, SHA_HASH_SIZE);
    } else {
	bzero(input.hash, SHA_HASH_SIZE);
    }
    if (r) {
	bcopy(r, input.R, R_SIZE);
    } else {
	bzero(input.R, R_SIZE);
    }
    if (s) {
	bcopy(s, input.S, S_SIZE);
    } else {
	bzero(input.S, S_SIZE);
    }
    input.size = size;
    pak = create_key_cmd(inpak, CRYPTO_VER_DSS_SIGNATURE, &input, public);
    pak->cryptoflags = flags; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * ktest_epaZeroize
 * Send a request to clear the EPA memory.
 *****************************************************************************/
static void ktest_epaZeroize (void)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    epaZeroize_t input;

    if (ABORT_TEST) return;
    ds->callback = service_ktest_epaZeroize;
    process_set_boolean(next_test, FALSE);
    sprintf(ktest_table[CRYPTO_ZEROIZE_EPA][0].desc, "Zeroize\n");
    input.call_id = CRYPTO_ZEROIZE_EPA;
    pak = create_key_cmd(inpak, CRYPTO_ZEROIZE_EPA, &input, NULL);
    pak->cryptoflags = 0; 
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * service_ktest_epaReadFlash
 * Service a reply in response to a request to read flash
 *****************************************************************************/
static void service_ktest_epaReadFlash (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_READ_FLASH, state, pak);
}

/*****************************************************************************
 * service_ktest_epaWriteFlash
 * Service a reply in response to a request to write epa flash.
 *****************************************************************************/
static void service_ktest_epaWriteFlash (ushort state, paktype *pak)
{
    verify_ktest_result(CRYPTO_WRITE_FLASH, state, pak);
}

/**** BRING UP TEST ****/
/*****************************************************************************
 * epad_sendinit_cmd - Send init block
 *****************************************************************************/
void epad_sendinit_cmd (parseinfo *csb)
{
    /*** Get pointer to amcc registers ***/
    if (!epad_gidb[0]) {
        ttyprintf(CONTTY,"Error: IDB not found: Driver not initialized\n");
        return;
    }

    epad_send_init_block(epad_gidb[0]);

    if (!epad_gidb[1]) {
        ttyprintf(CONTTY,"Error: IDB not found: Driver not initialized\n");
        return;
    }

    epad_send_init_block(epad_gidb[1]);
}

/*****************************************************************************
 * epad_download_cmd - Start download test
 *****************************************************************************/
void epad_download_cmd (parseinfo *csb)
{
    epad_instance_t *ds;
    amcc_op_regs_t *op_regs;

    ttyprintf(CONTTY,"Starting download test\n");

    /*** Get pointer to amcc registers ***/
    if (!epad_gidb[0]) {
        ttyprintf(CONTTY,"Error: IDB not found: Driver not initialized\n");
        return;
    }
    ds = INSTANCE(epad_gidb[0]);
    op_regs = ds->op_regs;

    if (!epad_download(op_regs)) {
        ttyprintf(CONTTY,"Download test failed\n");
        return;
    }
    ttyprintf(CONTTY,"Download test succeeded\n");
}

/*****************************************************************************
 * epad_mailbox_test - Mailbox test:
 *	- EPA driver writes a long word into mailbox
 *	- EPA firmware reads the content of the mailbox, and write the 
 *	inverse into mailbox.
 *	- EPA driver reads mailbox and compares the result
 *****************************************************************************/
static void epad_mailbox_test (amcc_op_regs_t *op_regs, uint numtries)
{
    volatile ulong *omb, *imb;
    ulong ombFull, imbFull;
    boolean err = FALSE;
    int i, mbox, t;
    ulong data;
    uint mailbox_test_pattern[4] = {0x00112233, 0x44556677, 
                                    0x8899AABB, 0xFFDDEEFF };
                              /* mailbox 4 always returns 0x00 in high byte */

    ttyprintf(CONTTY,"\nStarting MAILBOX test\n");
    for (i=0;i<numtries;i++) {
	ttyprintf(CONTTY,"Pass %d\n",i);
	for (mbox=0; mbox<4; mbox++) {
	    omb = &op_regs->omb1 + mbox;
	    imb = &op_regs->imb1 + mbox;
	    ombFull = OMB1_FULL << (4 * mbox);
	    imbFull = IMB1_FULL << (4 * mbox);
	    err = FALSE;
	    data = *imb;
                    
	    /*** Write mailbox ***/
	    t=0;
	    while ((op_regs->mbef & ombFull) && (!err)) {
		ttyprintf(CONTTY,"Trying to write, OMB%d is full, mbef:0x%08x\n", mbox+1,
			op_regs->mbef);
		DELAY(1);
		if (t++ > 20)
		err = TRUE;
	    }
	    if (!err) {
		*omb = mailbox_test_pattern[mbox];
	    }
 
	    /*** Read mailbox ***/
	    t=0;
	    while ((!(op_regs->mbef & imbFull)) && (!err)) {
		ttyprintf(CONTTY,"Trying to read, IMB%d is empt, mbef:0x%08x\n", mbox+1,
			op_regs->mbef);
		DELAY(1);
		if (t++ > 20) {
		    err = TRUE;
		    break;
		}
	    }
	    if (!err) {
		data = *imb;
		if (data != ~mailbox_test_pattern[mbox])
		ttyprintf(CONTTY,"BOX%d: test failed, in:0x%08x out:0x%08x\n\n",
			mbox+1,mailbox_test_pattern[mbox],data);
		else
		ttyprintf(CONTTY,"BOX%d: test passed, in:0x%08x out:0x%08x\n\n",
			mbox+1,mailbox_test_pattern[mbox],data);
	    } else {
		ttyprintf(CONTTY, "BOX%d: test failed, communication timeout\n\n", mbox+1);
	    }
	    mailbox_test_pattern[mbox] = mailbox_test_pattern[mbox] + 1;
		
	}
    }
    ttyprintf(CONTTY,"Ending MAILBOX test\n");
}

/*****************************************************************************
 * epad_interrupt_test
 *	- EPA driver write 0x1 to mailbox 4, interrupting the EPA firmware
 *	- Upon receipt of the interrupt, 
 *	the EPA firmware writes 0x02 to mailbox 4 and asserts INTR#0
 *	- Upon receipt of the interrupt, the EPA driver increments a counter,
 *	and deasserts INTR#0
 * This test will not work, needs to modify epad_interrupt to increment
 * stat.intr_rcvd.
 *****************************************************************************/
static void epad_interrupt_test (amcc_op_regs_t *op_regs, uint numtries)
{
    int i;

    ttyprintf(CONTTY,"\nStarting INTERRUPT test\n");
    stat.intr_rcvd = 0;

    for (i=0;i<numtries;i++) {
	/*** Read status ***/
	if (op_regs->mbef & OMB4_FULL) {
	    ttyprintf(CONTTY,"OMB4 is full, mbef:0x%08x\n", op_regs->mbef);
	    continue;
	}
	    
	/*** Write into mailbox ***/
	op_regs->omb4 = 0x1;
	DELAY(5);
    }
    ttyprintf(CONTTY,"Summary: %d interrupts sent, %d interrupts received\n",
	    numtries, stat.intr_rcvd);
    ttyprintf(CONTTY,"Ending interrupt test\n");
}

/*****************************************************************************
 * epad_buffer_transfer - 
 * 	- EPA driver allocates a chunk of memory and fills it with a 
 *	known pattern.
 *	- The EPA driver then writes an address of the buffer to mailbox 2,
 *	and size of the buffer into mailbox 1.
 * 	- The EPA firmware compliments the given buffer in shared PCI memory
 *	and posts a message to mailbox 1.
 *	- Upon receipt of the message in mailbox 1, the EPA driver will 
 *	verify the contents of the buffer.
 *****************************************************************************/
static void epad_buffer_transfer_test (amcc_op_regs_t *op_regs, uint numtries,
				uint buffer_size, ulong X)
{
    unsigned long status;
    ulong *buf;
    uint epa2vipfail=0,vip2epafail=0;
    int i, t, r;

    ttyprintf(CONTTY,"Starting BUFFER TRANSFER test\n");
    ttyprintf(CONTTY,"Clearing incoming mailboxes\n");
    r = op_regs->imb1;
    r = op_regs->imb2;
    r = op_regs->imb3;
    r = op_regs->imb4;

    /*** Get new buffer ***/
    buf = (ulong *)malloc_pcimem(buffer_size*4);
    if (!buf) {
	ttyprintf(CONTTY,"Error: malloc_pcimem failed\n");
	return;
    }
    ttyprintf(CONTTY,"Using buffer 0x%x  size %d\n",buf,buffer_size);
	
    for (i=0;i<numtries;i++) {
	/*
	 * VIP to EPA
	 */
	/*** Init buffer ***/
	for (r=0;r<buffer_size;r++) {
	    *(buf+r) = X;
	    X = ( (X << 3 ) ^ ( X >> 3 ) ) + 0xAA5555AA;
	}
 
	/*** Write size into mailbox 2 ***/
	op_regs->omb2 = buffer_size;

	/*** Write buffer address into mailbox 1 ***/
	op_regs->omb1 = (ulong)EPAD_BUF_ADDR(buf);
 
	/*** Wait for EPA to put somethine in mailbox 1 ***/
	r = 0;
	for (t=0;t<2000;t++) {
	    status = op_regs->mbef;
	    if ( (status & IMB1_FULL) && (status & IMB2_FULL) )
	        break;
	    DELAY(1);
	}
	if (t==2000) {
	    ttyprintf(CONTTY,"2 sec Timeout\n");
	    vip2epafail++;
	    epa2vipfail++;
	    X++;
	    continue;
	}
	r = op_regs->imb1;
	r = op_regs->imb2;
	if (r != -1) {
	    ttyprintf(CONTTY,"EPA reports err in long#%d\n",r);
	    vip2epafail++;
	}

	/*
	 * EPA to VIP
	 */
	X = *buf;

	/*** Check contents of buffer ***/
	for (r=1;r<buffer_size;r++) {
	    X = ( (X << 3 ) ^ ( X >> 3 ) ) + 0xAA5555AA;
	    if (*(buf+r) != X) {
		ttyprintf(CONTTY,
			"VIP reports err in long#%d expected 0x%x got 0x%x\n",r,X,*(buf+r));
		epa2vipfail++;
		break;
	    }
	}
	X++;
    }
    ttyprintf(CONTTY,
	    "VIP->EPA Summary: %d passes, %d successes, %d failures\n",
	    i, i-vip2epafail, vip2epafail);
    ttyprintf(CONTTY,
	    "EPA->VIP Summary: %d passes, %d successes, %d failures\n",
	    i, i-epa2vipfail, epa2vipfail);
 
    /*free(buf);*/
    ttyprintf(CONTTY,"Used buffer 0x%x  size %d. Examine with 'show mem'.\n",
	    buf,buffer_size);
    ttyprintf(CONTTY,"Ending buffer transfer test\n");
}

/*****************************************************************************
 * epad_bringup_test_cmd - Start bringup tests
 *****************************************************************************/
void epad_bringup_test_cmd (parseinfo *csb)
{
    uint testnum = GETOBJ(int, 1);
    uint numtries = GETOBJ(int, 2);
    uint buffer_size = GETOBJ(int, 3); /* for buffer transfer test only */
    ulong X = GETOBJ(int, 4); /* for buffer transfer test only */
    epad_instance_t *ds;

    /*** Get pointer to amcc registers ***/
    if (!epad_gidb[0]) {
        ttyprintf(CONTTY,"Error: IDB not found: Driver not initialized\n");
        return;
    }
    ds = INSTANCE(epad_gidb[0]);

    switch (testnum) {
      case 1: /*** Mailbox test ***/
	epad_mailbox_test(ds->op_regs, numtries);
        break;
	
      case 2: /*** Interrupt test VIP->EPA ***/
	epad_interrupt_test(ds->op_regs, numtries);
        break;
	
      case 3: /*** Buffer transfer test ***/
	epad_buffer_transfer_test(ds->op_regs, numtries, buffer_size, X);
        break;
    }
}

/*****************************************************************************
 * dump_pci_opregs
 *****************************************************************************/
static void dump_pci_opregs(uint bay)
{
    amcc_op_regs_t *op_regs;

    op_regs = get_pci_device_base(bay, 0);
    ttyprintf(CONTTY,"\n EPA Operation Register:\n");
    ttyprintf(CONTTY,"Outgoing Mailbox 1:          0x%08x\n", op_regs->omb1);
    ttyprintf(CONTTY,"Outgoing Mailbox 2:          0x%08x\n", op_regs->omb2);
    ttyprintf(CONTTY,"Outgoing Mailbox 3:          0x%08x\n", op_regs->omb3);
    ttyprintf(CONTTY,"Outgoing Mailbox 4:          0x%08x\n", op_regs->omb4);
    ttyprintf(CONTTY,"Incoming Mailbox 1:          0x%08x\n", op_regs->imb1);
    ttyprintf(CONTTY,"Incoming Mailbox 2:          0x%08x\n", op_regs->imb2);
    ttyprintf(CONTTY,"Incoming Mailbox 3:          0x%08x\n", op_regs->imb3);
    ttyprintf(CONTTY,"Incoming Mailbox 4:          0x%08x\n", op_regs->imb4);
    ttyprintf(CONTTY,"FIFO:                        0xxxxxxxxx\n");
    ttyprintf(CONTTY,"Master Write Address:        0x%08x\n", op_regs->mwar);
    ttyprintf(CONTTY,"Master Write Transfer Count: 0x%08x\n", op_regs->mwtc);
    ttyprintf(CONTTY,"Master Read Address:         0x%08x\n", op_regs->mrar);
    ttyprintf(CONTTY,"Master Read Transfer:        0x%08x\n", op_regs->mrtc);
    ttyprintf(CONTTY,"Mailbox Empty/Full Status:   0x%08x\n", op_regs->mbef);
    ttyprintf(CONTTY,"Interrupt Control/Status:    0x%08x\n", op_regs->intcsr);
    ttyprintf(CONTTY,"Bus Master Control/Status::  0x%08x\n", op_regs->mcsr);
}

/*****************************************************************************
 * dump_pci_config
 *****************************************************************************/
static void dump_pci_config (uint pa_bay)
{
    uchar bus_no;
    ulong r;
    int i;

    bus_no = get_pci_bus_no(pa_bay);

    ttyprintf(CONTTY,"\nEPA PCI configuration registers:\n");
    r = pci_config_read(bus_no, 0, 0, PCI_VENDOR_ID_OFFSET);
    ttyprintf(CONTTY,"Vendor/Device id:            0x%08x\n",r);

    r= pci_config_read(bus_no, 0, 0, PCI_COMMAND_REG_OFFSET);
    ttyprintf(CONTTY,"Command/Status:              0x%08x\n",r);

    r = pci_config_read(bus_no, 0, 0, PCI_REVISION_ID_OFFSET);
    ttyprintf(CONTTY,"Revision/Class:              0x%08x\n",r);

    r = pci_config_read(bus_no, 0, 0, PCI_LATENCY_OFFSET);
    ttyprintf(CONTTY,"Cache/Lat/Hdr/Bist:          0x%08x\n",r);

    for (i=0;i<6;i++) {
        r = pci_config_read(bus_no, 0, 0, PCI_IO_BASE_OFFSET+(4*i));
        ttyprintf(CONTTY,"Base Addr Reg %d:             0x%08x\n",i,r);
    }

    r = pci_config_read(bus_no, 0, 0, 0x3C);
    ttyprintf(CONTTY,"IntrLn/IntrPn/MinGr/MaxLat:  0x%08x\n",r);
}

/*****************************************************************************
 * show_amcc_menu
 *****************************************************************************/
static void show_amcc_menu(void)
{
    printf("\n");
    printf("1) Initialize bridge chip\n");
    printf("2) Initialize AMCC chip\n");
    printf("3) Dump PCI Config regs\n");
    printf("4) Dump AMCC Oper regs\n");
    printf("5) Read mbox status (mbef)\n");
    printf("6) Read from mbox\n");
    printf("7) Write to mbox\n");
    printf("q) quit\n");
    printf("Select option: ");
}

/*****************************************************************************
 * epad_test_amcc_cmd -
 * This command allow the user the ability to 
 * 	- initialize the bridge chip
 *	- initialize the AMCC chip
 *	- dump pci config register
 *	- dump AMCC operation register
 *	- read mail box status 
 *	- read from a specific mailbox
 *	- write to a specific mailbox
 *****************************************************************************/
void epad_test_amcc_cmd(parseinfo *csb)
{
    char buf[100];
    boolean quit = FALSE;
    uint pa_bay;
    long mbx;
    ulong value;
    char *op_regs,*op_regs_ptr;
 
    printf("\nFor which bay? ");
    rdtty(buf,sizeof(buf));
    printf("\n");
 
    if (buf[0] == 0)
        return;
 
    pa_bay = atoi(buf);
    if (pa_bay!=0 && pa_bay!=1)
        return;
 
    while (!quit) {
        show_amcc_menu();
        rdtty(buf,sizeof(buf));
 
         switch (buf[0]) {
           case '1': 
	      /* Initialize pci bridge device */
	      if (!pas_bridge_config(pa_bay)) {
		  pas_deactivate_bay(pa_bay);
		  printf("bridge initialization FAILED\n");
	      } else {
		  printf("bridge initialized\n");
	      }
              break;
           case '2':
              epad_pci_init(pa_bay,0);
              printf("pci bus initialized\n");
              break;
           case '3':
	      dump_pci_config(pa_bay);
              break;
           case '4':
	      dump_pci_opregs(pa_bay);
              break;
           case '5':
              op_regs = get_pci_device_base(pa_bay,0);
              op_regs_ptr = op_regs + 0x34;
              value = *((ulong *)op_regs_ptr);
 
              printf("     In  Out\n");
              printf("box# 43214321\n");
              printf("     %08x\n",value);
              break;
           case '6':
              printf("From which mbx? ");
              rdtty(buf,sizeof(buf));
              printf("\n");
 
              if (buf[0] == 0) {
                  break;
              }
 
              mbx = atoi(buf) - 1;
              if (mbx<0 && mbx>3) {
                  break;
              } 

              op_regs = get_pci_device_base(pa_bay,0);
              
              op_regs_ptr = op_regs + 0x34;
              value = *((ulong *)op_regs_ptr);
              printf("Old status:      0x%08x\n",value);
              op_regs_ptr = ((char *)op_regs) + ((4*mbx)+16);
              value = *((ulong *)op_regs_ptr);
              printf("Read mbx %d:     0x%08x\n",mbx+1,value);

              op_regs_ptr = op_regs + 0x34;
              value = *((ulong *)op_regs_ptr);
              printf("New status:      0x%08x\n",value);
              break;
           case '7':
              printf("To which mbx? ");
              rdtty(buf,sizeof(buf));
              printf("\n");
 
              if (buf[0] == 0) {
                  break;
              } 
              mbx = atoi(buf) - 1;
              if (mbx<0 && mbx>3) {
                  break;
              }
              printf("What value? ");
              rdtty(buf,sizeof(buf)); 
              printf("\n");

              if (buf[0] == 0) {
                  break;
              }
              op_regs = get_pci_device_base(pa_bay,0); 

              op_regs_ptr = op_regs + 0x34;
              value = *((ulong *)op_regs_ptr);
              printf("Old status:      0x%08x\n",value);

              op_regs_ptr = ((char *)op_regs) + (4*mbx);
              value = atoi(buf);
              *((ulong *)op_regs_ptr) = value;
              printf("Wrote to mbx %d: 0x%08x\n",mbx+1,value);

              op_regs_ptr = op_regs + 0x34;
              value = *((ulong *)op_regs_ptr);
              printf("New status:      0x%08x\n",value);
              break;
           case 'q': 
              quit = TRUE;
              break;
        }
     }
}
/*****************************************************************************
 * epad_process_rings -
 * Will go through the descriptor ring and process all the packet that
 * are currently on the ring.
 *****************************************************************************/
static void epad_process_rings (uint ring)
{
    epad_instance_t *ds;
    leveltype level;
    static int epa_head[2]={0,0};
    void (*oldcallback)(ushort flags, paktype *pak); 
    uint count=0;
   
    if (ring != EPAD_KEY_DEVICE)
        ring = EPAD_FRAME_DEVICE;

    /*** Get pointer to ds ***/
    if (!epad_gidb[ring]) {
        ttyprintf(CONTTY,"Error: IDB not found: Driver not initialized\n");
        return;
    }
    ds = INSTANCE(epad_gidb[ring]);

    oldcallback = ds->callback;
    ds->callback = test_callback;

    while (1) {
        if (ds->beg[epa_head[ring]].control == (EPAD_FW_OWN|EPAD_FIRST)) {
            count = ds->beg[epa_head[ring]].numbuf - 1;
            ds->beg[epa_head[ring]].control &= (uchar)0x7F;
        } else if (ds->beg[epa_head[ring]].control == (EPAD_FW_OWN|EPAD_ONLY)) {
            count = ds->beg[epa_head[ring]].numbuf - 1;
            ds->beg[epa_head[ring]].control &= (uchar)0x7F;
        } else if (ds->beg[epa_head[ring]].control == EPAD_MID || 
                         ds->beg[epa_head[ring]].control == EPAD_LAST) {
            if (count <= 0) 
                break;
            count--;
        } else {
            break;
        }
        EPAD_ADVANCE_INDEX(epa_head[ring],ds->adv_idx_mask);
    }

    level = raise_interrupt_level(NETS_DISABLE);
    epad_interrupt_handler(epad_gidb[ring]);
    reset_interrupt_level(level);

    ds->callback = oldcallback;
}

/*****************************************************************************
 * epad_process_rings -
 *****************************************************************************/
void epad_process_rings_cmd (parseinfo *csb)
{
    uint ring = GETOBJ(int,1);

    if (ring != EPAD_KEY_DEVICE)
        ring = EPAD_FRAME_DEVICE;

    epad_process_rings(ring);
}

boolean create_session (void)
{
    ktest_epaZeroize();
    if (ktest_table[CRYPTO_ZEROIZE_EPA][0].result != 1) {
	ABORT_TEST = TRUE;
    }

    set_passwd();
    if (ktest_table[CRYPTO_SET_EPA_PASSWD][0].result != 1) {
	ABORT_TEST = TRUE;
    }

    clear_tamper();
    if (ktest_table[CRYPTO_SET_EPA_STATUS][14].result != 1) {
	ABORT_TEST = TRUE;
    }

    clear_xtract();
    if (ktest_table[CRYPTO_SET_EPA_STATUS][15].result != 1) {
	ABORT_TEST = TRUE;
    }
    alloc_global_handle();

    sprintf(ktest_table[CRYPTO_GEN_DH_PAIR][0].desc, 
	    "gendhpair %d\n", ghandle1);
    gendhpair(SUCCESS, ghandle1, 64);
    if (ktest_table[CRYPTO_GEN_DH_PAIR][0].result != 1) {
	ABORT_TEST = TRUE;
	return FALSE;
    }
	
    sprintf(ktest_table[CRYPTO_GET_DH_SHARED][0].desc,
	    "gendhshared secret = %d, shared = %d\n", 
	    ghandle1, ghandle2);
    getdhshared(SUCCESS, ghandle1, ghandle2, 64, gdh_public);
    if (ktest_table[CRYPTO_GET_DH_SHARED][0].result != 1) {
	ABORT_TEST = TRUE;
	return FALSE;
    }

    sprintf(ktest_table[CRYPTO_SET_CIPHER_KEY][0].desc,
	    "setCipherkey shared = %d, key = %d\n", 
	    ghandle2, ghandle3);
    setCipherkey(SUCCESS, GHANDLE2_SIZE, ghandle2, ghandle3, 0, 0xB);
    if (ktest_table[CRYPTO_SET_CIPHER_KEY][0].result != 1) {
	ABORT_TEST = TRUE;
	return FALSE;
    }

    sprintf(ktest_table[CRYPTO_SET_CIPHER_KEY][0].desc,
	    "setCipherkey shared = %d, key = %d\n", 
	    ghandle2, ghandle4);
    setCipherkey(SUCCESS, GHANDLE2_SIZE, ghandle2, ghandle4, 0, 0xB);
    if (ktest_table[CRYPTO_SET_CIPHER_KEY][0].result != 1) {
	ABORT_TEST = TRUE;
	return FALSE;
    }

    sprintf(ktest_table[CRYPTO_NEW_SESSION][0].desc,
	    "newSessiion iv = %d, key = %d\n", ghandle3, ghandle4);
    newSession(SUCCESS, ghandle3, ghandle4, 0xB);
    if (ktest_table[CRYPTO_NEW_SESSION][0].result != 1) {
	ABORT_TEST = TRUE;
	return FALSE;
    }
    return TRUE;
}
void destroy_session (void)
{
    sprintf(ktest_table[CRYPTO_FREE_SESSION][0].desc,
	    "freeSession handle = %d\n", gsession);
    freeSession(SUCCESS, gsession);
    if (ktest_table[CRYPTO_FREE_SESSION][0].result != 1) {
	ABORT_TEST = TRUE;
	return;
    }
    free_global_handle();
}


/*****************************************************************************
 * epad_traffic_cmd - 
 * Allow the user to generate a packet and send to the EPA to encrypt/decrypt
 * or bitflip.
 * Required input are
 *	type - packet send in scattered or contiguous
 *	number of packet to send
 *	size of packet (0 = random size)
 *	offset (from the beginning of the buffer to where you want the
 *		EPA to encrypt/decrypt, bitflips)
 *	mode - encrypt/decrypt, bitflip or throughput
 *	verbosity level
 *****************************************************************************/
void epad_traffic_cmd (parseinfo *csb)
{
    int cont = GETOBJ(int, 1);
    int numpacket = GETOBJ(int, 2);
    int size = GETOBJ(int, 3);
    int offset = GETOBJ(int, 4);
    char mode = GETOBJ(int, 5);
    boolean rndtype, rndsize, rndoffset;
    hwidbtype *idb = epad_gidb[EPAD_FRAME_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    int i, ring = EPAD_FRAME_DEVICE;
    particletype *hdr = NULL;
    boolean result;
    ulong time;
    char flags = 0;
    sys_timestamp traffic_timer;
    int num_err = 0;

    verbose = GETOBJ(int, 6);
    if (!test_setup(2)) {
	return;
    }
    if (create_session() == FALSE) {
	pas_diag_errmsg(MSG_ERROR, "Failed to create session\n");
	return;
    }
    if (mode == 0) {		/* do encryption then decryption */
	((crypto_vector_t *)outidb->crypto_vectors)->media_fastsend = 
	service_encrypt_traffic;
	((crypto_vector_t *)outidb2->crypto_vectors)->media_fastsend = 
	service_decrypt_traffic;
	flags = ENCRYPT;
    } else if (mode == 1) {	/* bit flips */
	((crypto_vector_t *)outidb->crypto_vectors)->media_fastsend =
	service_bitflip_traffic;
	flags = BITFLIP|TESTMODE;
    } else { /* measure encrypt through put */
	((crypto_vector_t *)outidb->crypto_vectors)->media_fastsend =
	service_encrypt_throughput;
	flags = ENCRYPT;
    }
#if 0
    /* we are using the hardcode connection */
    gsession = 0;
    bzero(giv, sizeof(giv));
#endif

    /*** Reset statistic counter ***/
    stat.num_encrypt_snt = 0;
    stat.num_encrypt_rcv = 0;
    stat.num_decrypt_snt = 0;
    stat.num_decrypt_rcv = 0;
    ds->num_snt = ds->num_rcv = ds->bytes_snt = ds->bytes_rcv = 0;
    recv_id[0] = decrypt_id = send_id[0] = START_SEQUENCE;

    rndtype = (cont == 2) ? TRUE : FALSE;
    rndsize = (size == 0) ? TRUE : FALSE;
    rndoffset = (offset == MAX_OFFSET+1) ? TRUE : FALSE;
 
    num_traffic = numpacket;
    GET_TIMESTAMP(traffic_timer);
    for (i=0; i < numpacket; i++) {
	if (ABORT_TEST) {
	    pas_diag_errmsg(MSG_ERROR, "... ABORT TEST ...\n");
	    break;
	}

	/*** generate random packet type, size, and offset ***/
	if (rndtype || rndsize) {
	    get_rnd_param(FALSE, rndtype, rndsize,&ring, &cont, &size);
	}

	/*** If the descriptor ring is full,  wait for the EPA ***/
	if (mode <= 2) {
	    if ( idb->output_qcount ) {
		if (epad_throttle(idb, NULL)) {
		    i--;
		    continue;
		} else {
		    break;
		}
	    }
	} else {
	   if (mode == 3) { /* stop when holdq is full */
		if (idb->output_qcount == ds->holdq_size) {
			i--;
			continue;
		}
	    } else if (mode == 5) {
		/* only put one packet on a ring at a time */
		if (ds->count != 0) {
			i--;
			continue;
		}
	    } else if (mode == 6) {
		/* stop when holdq has something */
		if ( idb->output_qcount ) {
		    i--;
		    continue;
		}
	    }
	}
	if (!(pak = epad_genpack(EPAD_FRAME_DEVICE,
				 cont, size, 0, rndsize))) {
	    break;
	}

	pak->cryptoflags = CRYPTO_ENCRYPT;

	/*** Attach Encryption header ***/	
	hdr = epad_create_encryption_blk(idb, flags, gsession, giv, 0, pak);
	if (!hdr) {
	    epad_drop_packet(idb, pak);
	    break;
	}
	pak->unspecified[0] = (ulong)hdr;
	send_to_epad(idb, pak);
	if ((verbose==1) && (!(i % 256))) {	/* display every 256 loops */
	    encryption_test_summary(MSG_STATUS);
	} else if ((verbose == 0) && (!(i % 256))) {
	    ttyprintf(CONTTY, ".");
	}
    }
    wait_test_done();
    time = ELAPSED_TIME(traffic_timer);
    if (mode >= 2) {
	if (time > 0) {
	    ttyprintf(CONTTY, "***Performance:\n");
	    ttyprintf(CONTTY, "%d pps, ", (stat.num_encrypt_rcv*1000)/time);
	    ttyprintf(CONTTY, "%d bps\n", (ds->bytes_snt /time) * 8000);
	}
    } 
    pas_diag_errmsg(MSG_SYSTEM, "\n***Summary:\n");
    pas_diag_errmsg(MSG_SYSTEM, "***Test ran for %d mili sec, bytes sent = %d, bytes receive = %d\n", 
	    time, ds->bytes_snt, ds->bytes_rcv);
    encryption_test_summary(MSG_SYSTEM);
    num_err = ds->num_too_big + ds->num_nohdr + ds->num_desc_err + 
              ds->num_status_err;
    if (mode != 1) {
        result = ((!ABORT_TEST) && (!num_err) && 
	      (stat.num_encrypt_snt == stat.num_encrypt_rcv) &&
	      (stat.num_decrypt_snt == stat.num_decrypt_rcv));
    } else {
        result = ((!ABORT_TEST) && (!num_err) &&
              (stat.num_decrypt_snt == stat.num_encrypt_rcv));
    }
    pas_diag_errmsg(MSG_SYSTEM, "***Result:  %s\n\n", result ? "Passed" : "Failed");

    if (!EPA_TIMEOUT || !ABORT_TEST) {
        destroy_session();
    }
    test_cleanup(2);
    ds->callback = crypto_engine_callback;
}

/*****************************************************************************
 * epad_key_wf_cmd -
 * Allow the user to write to epa flash
 * Required input are:
 *	Number of bytes to write to flash
 *	Offset from beginning of flash to begin writing
 *	Data to write to flash memory.
 *****************************************************************************/
void epad_key_wf_cmd (parseinfo *csb)
{
    int offset = GETOBJ(int, 1);
    int size = GETOBJ(int, 2);
    epaWriteFlash_t input;
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    char data[MAX_FRAME_SIZE];
    int i;

    verbose = GETOBJ(int, 3);
    if (!test_setup(1)) {
	return;
    }
    
    set_priviledge();
    ds->callback = service_ktest_epaWriteFlash;
    ttyprintf(CONTTY, "Write Flash\n");
    input.call_id = CRYPTO_WRITE_FLASH;
    input.size = size;
    input.offset = offset;
    for (i=0; i < size/4; i++) {
	*((ulong *)data + i) = i;
    } 
    
    pak = create_key_cmd(inpak, CRYPTO_WRITE_FLASH, &input, data);
    pak->cryptoflags = 0; 
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_memRead_cmd
 * Allow the user to read hma memory.
 * Required input are:
 *	Handle of HMA block to read
 *	number of bytes to read
 *****************************************************************************/
void epa_memRead_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hMemRead_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_HMA_READ;
    keycmd.handle = GETOBJ(int, 1);
    keycmd.size = GETOBJ(int, 2);
    pak = create_key_cmd(inpak, CRYPTO_HMA_READ, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;

}

/*****************************************************************************
 * epa_memAlloc_cmd
 * Allow the user to allocate hma memory.
 * Required input are:
 *	Number of bytes to allocate
 *	Attribute bits for the HMA block
 *****************************************************************************/
void epa_memAlloc_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hMemAlloc_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_HMA_ALLOC;
    keycmd.size = GETOBJ(int, 1);
    keycmd.attribute = GETOBJ(int, 2);
    pak = create_key_cmd(inpak, CRYPTO_HMA_ALLOC, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_memFree_cmd
 * Allow user to free hma memory
 * Required input are:
 *	Handle of HMA block to deallocate
 *****************************************************************************/
void epa_memFree_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hMemFree_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_HMA_FREE;
    keycmd.handle = GETOBJ(int, 1);
    pak = create_key_cmd(inpak, CRYPTO_HMA_FREE, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_genRandom_cmd
 * Allow user to generate random number
 * Required input are:
 *	Size of random number to generate
 *****************************************************************************/
void epa_genRandom_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hGenRandom_t keycmd;
    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_GEN_RANDOM;
    keycmd.size = GETOBJ(int, 1);
    keycmd.handle = GETOBJ(int, 2);
    pak = create_key_cmd(inpak, CRYPTO_GEN_RANDOM, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_genDSSkey_cmd
 * Allow user to generate DSS key
 * Required input are:
 * 	HMA handle for generated secret number
 *	Size of modulus P and G
 *****************************************************************************/
void epa_genDSSkey_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hGenDSSkey_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_GEN_DSS_KEY;
    keycmd.secret = GETOBJ(int, 1);
    keycmd.size = GETOBJ(int, 2);
    pak = create_key_cmd(inpak, CRYPTO_GEN_DSS_KEY, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_genDSSsig_cmd
 * Allow user to generate Digital Signature Standard.
 * Required input are:
 *	size of modulus P and G
 *	secure hash result
 *****************************************************************************/
void epa_genDSSsig_cmd(parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hGenDSSsignature_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_GEN_DSS_SIGNATURE;
    keycmd.secret = GETOBJ(int, 1);
    keycmd.size = GETOBJ(int, 2);
    bcopy(GETOBJ(string, 1), keycmd.hash, SHA_HASH_SIZE);
    pak = create_key_cmd(inpak, CRYPTO_GEN_DSS_SIGNATURE, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_genDHpair_cmd
 * Allow the user to generate Diffie Hellman pair
 * Required input are:
 *	Size of base and modulus numbers
 *****************************************************************************/
void epa_genDHpair_cmd(parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hGenDHpair_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_GEN_DH_PAIR;
    keycmd.secret = GETOBJ(int, 1);
    keycmd.size = GETOBJ(int, 2);
    pak = create_key_cmd(inpak, CRYPTO_GEN_DH_PAIR, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_getDHshared_cmd -
 * Allow user to get Diffie Hellman shared number.
 * Required input are:
 *	HMA handle of secret number memory
 *	HMA handle of shared number memory
 *	size of base and modulus long number
 *	DH public number
 *****************************************************************************/
void epa_getDHshared_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hGetDHsharedNumber_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_GET_DH_SHARED;
    keycmd.secret = GETOBJ(int, 1);
    keycmd.shared = GETOBJ(int, 2);
    keycmd.size = GETOBJ(int, 3);
    pak = create_key_cmd(inpak, CRYPTO_GET_DH_SHARED, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_set -
 * Allow user to set DES Cipher key.
 * Required input are: 
 *	HMA handle of shared number
 *	HMA handle of DES and Cipherkey.
 *****************************************************************************/
void epa_setCipherkey_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hSetCipherkey_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_SET_CIPHER_KEY;
    keycmd.size = GETOBJ(int, 1);
    keycmd.shared = GETOBJ(int, 2);
    keycmd.key = GETOBJ(int, 3);
    pak = create_key_cmd(inpak, CRYPTO_SET_CIPHER_KEY, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_newSession_cmd
 * Allow user to create a new session.
 * Required input are:
 *	HMA handle to 8-byte IV value.	
 *	HMA handle to 8-byte session key
 *	Encryption Method
 *****************************************************************************/
void epa_newSession_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    epaNewSession_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_NEW_SESSION;
    keycmd.iv = GETOBJ(int, 1);
    keycmd.key = GETOBJ(int, 2);
    keycmd.method = GETOBJ(int, 3);
    keycmd.handle = GETOBJ(int, 4);
    pak = create_key_cmd(inpak, CRYPTO_NEW_SESSION, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_freeSession_cmd
 * Allow user to free session 
 * Required input are:
 *	HMA handle to Session control block
 *****************************************************************************/
void epa_freeSession_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    epaFreeSession_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_FREE_SESSION;
    keycmd.handle = GETOBJ(int, 1);
    pak = create_key_cmd(inpak, CRYPTO_FREE_SESSION, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_setPasswd_cmd
 * Allow user to set password.
 * Required input are:
 *	Size of the password
 *	Password
 *****************************************************************************/
void epa_setPasswd_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    epaSetPassword_t keycmd;
    char passwd[20];
    
    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_SET_EPA_PASSWD;
    keycmd.size = GETOBJ(int, 1);
    bcopy(GETOBJ(string, 1), passwd, keycmd.size);
    pak = create_key_cmd(inpak, CRYPTO_SET_EPA_PASSWD, &keycmd, passwd);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_sha_cmd
 * Allow user to set secure hash
 * Required input are
 *	Size of the message
 *	Message from which to compute secure hash
 *****************************************************************************/
void epa_sha_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hSHA_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_SHA;
    keycmd.size = GETOBJ(int, 1);
    pak = create_key_cmd(inpak, CRYPTO_SHA, &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_verDSSsig_cmd
 * Allow use to verify DSS signature
 * Required input are:
 *	size of modulus P and G
 *	secure hash result
 *	R
 *	S
 *	public number
 *****************************************************************************/
void epa_verDSSsig_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    hVerDSSsignature_t keycmd;

    if (!test_setup(1)) {
	return;
    }   
    keycmd.call_id = CRYPTO_VER_DSS_SIGNATURE;
    keycmd.size = GETOBJ(int, 1);
    bcopy(GETOBJ(string, 1), keycmd.hash, SHA_HASH_SIZE);
    bcopy(GETOBJ(string, 2), keycmd.R, R_SIZE);
    bcopy(GETOBJ(string, 2), keycmd.S, S_SIZE);
    pak = create_key_cmd(inpak, CRYPTO_VER_DSS_SIGNATURE , &keycmd, NULL);
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_setStatus_cmd -
 * Allow user to set EPA status
 * Required input are:
 *	size of password buffer
 *	mask of flags to modify
 *	new status flag values
 *	password for clearing privileged status flags
 *****************************************************************************/
void epa_setStatus_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    epaSetStatus_t keycmd;

    if (!test_setup(1)) {
	return;
    }
    keycmd.call_id = CRYPTO_SET_EPA_STATUS;
    keycmd.size = GETOBJ(int, 1);
    switch(GETOBJ(int, 2)) {
      case 0:
	keycmd.mask = 0x80000000;
	break;
      case 1:
	keycmd.mask = 0x40000000;
	break;
      case 2:
	keycmd.mask = 0x20000000;
	break;
      case 3:
	keycmd.mask = 0x10000000;
	break;
    }
    if (GETOBJ(int, 3)) {
	keycmd.flags = keycmd.mask;
    } else {
	keycmd.flags = 0;
    }
    pak = create_key_cmd(inpak, CRYPTO_SET_EPA_STATUS, &keycmd, 
			 (uchar *)(GETOBJ(string, 1)));    
    ds->callback = dump_key_cmd;
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epad_key_zeroize_cmd
 * Allow user to clears all of memory (including HMA non-volatile parameters).
 * All current data including keys, sessions, ppassword, and certificates
 * will be lost.
 *****************************************************************************/
void epad_key_zeroize_cmd (parseinfo *csb)
{
    epad_instance_t *ds = INSTANCE(epad_gidb[EPAD_KEY_DEVICE]);
    verbose = GETOBJ(int, 1);
    if (!test_setup(1)) {
	return;
    }
    ktest_epaZeroize();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epad_key_rf_cmd -
 * Allow user to read EPA flash.
 * Required input are:
 * 	Number of bytes to read from flash
 *	Offset from beginning of flash to begin reading
 *****************************************************************************/
void epad_key_rf_cmd (parseinfo *csb)
{
    int offset = GETOBJ(int, 1);
    int size = GETOBJ(int, 2);
    epaReadFlash_t input;
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;

    verbose = GETOBJ(int, 3);
    if (!test_setup(1)) {
	return;
    }
    ds->callback = service_ktest_epaReadFlash;
    ttyprintf(CONTTY, "Read Flash\n");
    input.call_id = CRYPTO_READ_FLASH;
    input.size = size;
    input.offset = offset;
    pak = create_key_cmd(inpak, CRYPTO_READ_FLASH, &input, NULL);
    pak->cryptoflags = 0; 
    send_to_epad(idb, pak);
    wait_test_done();  
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epad_statustime_cmd
 * Allow user to get status and time of the EPA
 *****************************************************************************/
void epad_statustime_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    epaGetTime_t input;
    epaGetStatus_t istatus;

    if (!test_setup(1)) {
	return;
    }

    ds->callback = service_ktest_epaGetStatus;

    istatus.call_id =  CRYPTO_GET_EPA_STATUS;
    pak = create_key_cmd(inpak, CRYPTO_GET_EPA_STATUS, &istatus, NULL);
    pak->cryptoflags = 1;
    send_to_epad(idb, pak);
    wait_test_done();

    ds->callback = service_ktest_epaGetTime;
    process_set_boolean(next_test, FALSE);
    input.call_id = CRYPTO_GET_TIME;
    pak = create_key_cmd(inpak, CRYPTO_GET_TIME, &input, NULL);
    pak->cryptoflags = 2; 
    send_to_epad(idb, pak);
    wait_test_done();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epad_show_statistic_cmd
 *****************************************************************************/
void epad_show_statistic_cmd (parseinfo *csb)
{
    ttyprintf(CONTTY, "\nEPAD STATISTIC:\n");
    ttyprintf(CONTTY, "intr_rcvd = %d\n", stat.intr_rcvd);
    ttyprintf(CONTTY, 
	    "num_encrypt_snt           = %d, num_encrypt_rcv           = %d\n", 
	    stat.num_encrypt_snt, stat.num_encrypt_rcv);
    ttyprintf(CONTTY, 
	    "num_decrypt_snt           = %d, num_decrypt_rcv           = %d\n\n", 
	    stat.num_decrypt_snt, stat.num_decrypt_rcv);
    ttyprintf(CONTTY, 
	    "undef_cmd_snt             = %d, undef_cmd_rcv             = %d\n", 
	    stat.undef_cmd_snt, stat.undef_cmd_rcv);
}

/*****************************************************************************
 * epad_led_cmd
 * Allow a remote user to find which LED is on or off.
 *****************************************************************************/
void epad_led_cmd (parseinfo *csb)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    epaGetStatus_t istatus;

    if (!test_setup(1)) {
	return;
    }

    ds->callback = service_ktest_epaGetStatus;

    istatus.call_id =  CRYPTO_GET_EPA_STATUS;
    istatus.status = 0;
    pak = create_key_cmd(inpak, CRYPTO_GET_EPA_STATUS, &istatus, NULL);
    pak->cryptoflags = 2;
    send_to_epad(idb, pak);
    wait_test_done();
}

/**** DIAG ****/
#define LRG_FRAME_SIZE 4096
/*****************************************************************************
 * epad_frame_cmd
 * Allow the user to encrypt/decrypt or bitflip a packet from size 1 
 * to maximum size.
 * Always send segmented packet since SVIP doesn't support contiguous packet.
 * Required input are
 *	mode - encrypt/decrypt or bitflip
 *	verbosity level
 *****************************************************************************/
void epad_frame_cmd(parseinfo *csb)
{
    int num;
    int ring=EPAD_FRAME_DEVICE, cont=0, size=0;
    paktype *pak;
    particletype *hdr;
    hwidbtype *idb = epad_gidb[EPAD_FRAME_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    int err;
    boolean result;
    char flag=0;
    int num_err = 0;
    int num_test = 0;

    if (csb) {
	flag = GETOBJ(int, 1);
	flag = (flag) ? ENCRYPT|BITFLIP|TESTMODE : ENCRYPT;
	verbose = GETOBJ(int, 2);
        num_test = GETOBJ(int, 3);
    } else {
	flag = ENCRYPT;
	verbose = 2;
        num_test = 1;
    }
    if (!test_setup(2)) {
	return;
    }
    if (create_session() == FALSE) {
	pas_diag_errmsg(MSG_ERROR, "Failed to create session\n");
	return;
    }
    if (flag == ENCRYPT) {
	((crypto_vector_t *)outidb->crypto_vectors)->media_fastsend = 
        service_encrypt_traffic;
	((crypto_vector_t *)outidb2->crypto_vectors)->media_fastsend = 
        service_decrypt_traffic;
    } else {
	((crypto_vector_t *)outidb->crypto_vectors)->media_fastsend = 
	service_bitflip_traffic;
    }
    recv_id[0] = decrypt_id = send_id[0] = START_SEQUENCE;


    pas_diag_errmsg(MSG_STATUS, "Cisco Encryption/Decryption Test\n\n");
    
    pas_diag_errmsg(MSG_STATUS, "Request to encrypt/decrypt packet of size 1 to %d\n", LRG_FRAME_SIZE);
    /*** Reset statistic counter and service function ***/
    stat.num_encrypt_snt=0;
    stat.num_encrypt_rcv=0;
    stat.num_decrypt_snt=0;
    stat.num_decrypt_rcv=0;
    err = ds->num_too_big + ds->num_nohdr + ds->num_desc_err + ds->num_status_err;

    num_traffic = LRG_FRAME_SIZE * num_test;
    for (num=0; num < num_test; num++) {
	if (ABORT_TEST) {
	    pas_diag_errmsg(MSG_ERROR, "... ABORT TEST ...\n");
	    break;
	}
	for (size =1; size <= LRG_FRAME_SIZE; size++) {	
	    if (ABORT_TEST) {
		pas_diag_errmsg(MSG_ERROR, "\n... ABORT TEST ...\n");
		break;
	    }
		
	    /*** if descriptor ring is full, wait for EPA ***/
	    if (idb->output_qcount) {
		if (epad_throttle(idb, NULL)) {
		    size--;
		    continue;
		} else {
		    break;
		}
	    }
	    pak = epad_genpack(ring, cont, size, 0, FALSE); 
	    if (!pak) break;
	    pak->cryptoflags = CRYPTO_ENCRYPT;
	    hdr = epad_create_encryption_blk(idb, 
					     flag, gsession, giv, 0, pak);
	    if (!hdr) {
		epad_drop_packet(idb, pak);
		break;
	    }
	    pak->unspecified[0] = (ulong)hdr;	
	    send_to_epad(idb, pak);
	    if (verbose && (!(size % 256))) { 
		/* display summary every 256 loops */
		encryption_test_summary(MSG_STATUS);
	    } else if (verbose == 0 && (!(size % 256))) {
		ttyprintf(CONTTY, ".");
	    }
	}
    }
    
    wait_test_done();
    pas_diag_errmsg(MSG_SYSTEM, "***Summary:\n");
    encryption_test_summary(MSG_SYSTEM);
    num_err = ds->num_too_big + ds->num_nohdr + ds->num_desc_err + 
              ds->num_status_err;
    result = ((!ABORT_TEST) && (err == num_err) && 
	      (stat.num_encrypt_snt == stat.num_encrypt_rcv) &&
	      (stat.num_decrypt_snt == stat.num_decrypt_rcv));
    pas_diag_errmsg(MSG_SYSTEM, "***Result:  %s\n\n", 
		    result ? "Passed" : "Failed");
    if (!result) {
        pas_diag_errmsg(MSG_ERROR, "Test ran: %d, last packet size: %d\n", num, size-1);
    }

    if (!EPA_TIMEOUT || !ABORT_TEST) {
        destroy_session();
    }
    test_cleanup(2);
}

/*****************************************************************************
 * ktest_hMemAlloc
 * Various test to test the functionality of command hMemAlloc
 * Test 0: Allocate HMA memory of maximum size allow
 * Test 1: Allocate HMA memory of size over the maximum allow
 * Test 2: Allocate HMA memory with size 0 byte
 * Test 3: Allocate HMA memory with an invalid attribute 
 * Test 4: Request to allocate HMA memory from size 1 until epa ran out of
 *	memory
 *****************************************************************************/
static void ktest_hMemAlloc (void)
{
    ushort size=0;
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    hMemAlloc_t input;
    hMemFree_t ifree;
    paktype *pak;
    char attribute = 0;
    int i;
    int testnum = 0;

    if (ABORT_TEST) return;
    ds->callback = service_ktest_hMemAlloc;
    process_set_boolean(next_test, FALSE);

    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING HMA ALLOC ****\n\n");
    /*** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_HMA_ALLOC][testnum].desc, 
	    "Test %d: Allocate HMA memory of size = %d bytes (maximum size allow)\n", 
	    testnum++, MAX_HMA_SIZE);
    hMemAlloc(0, MAX_HMA_SIZE,0x20); 

   /*** TEST 1 ***/
    sprintf(ktest_table[CRYPTO_HMA_ALLOC][testnum].desc, 
	    "Test %d: Allocate HMA memory of size = %d bytes (over the maximum size allow)\n",
	    testnum++, MAX_HMA_SIZE+1);
    hMemAlloc(1, MAX_HMA_SIZE+1,0x20); 

    /*** TEST 2 ***/
    sprintf(ktest_table[CRYPTO_HMA_ALLOC][testnum].desc, 
	    "Test %d: Allocate HMA memory with size 0 byte\n", testnum++);
    hMemAlloc(2, 0, 0x20);

    /****TEST 3 *****/
    attribute = 3;
    sprintf(ktest_table[CRYPTO_HMA_ALLOC][testnum].desc, 
	    "Test %d: Allocate HMA memory with an invalid attribute = 0x%x\n", testnum++, attribute);
    hMemAlloc(3, 512, attribute);

    /****TEST 4 *****/
    sprintf(ktest_table[CRYPTO_HMA_ALLOC][testnum].desc,
	    "Test %d: Request to allocate HMA memory from size 1 to %d\n",
	    testnum++, MAX_ALLOC);
    for (size=1; size < MAX_ALLOC+1; size++) {
	if (idb->output_qcount) {
	    if (epad_throttle(idb, NULL)) {
		size--;
		continue;
	    } else {
		break;
	    }
	}
	input.call_id = CRYPTO_HMA_ALLOC;
	input.size = size;
	input.attribute = 0x10;
	pak = create_key_cmd(inpak, CRYPTO_HMA_ALLOC, &input, NULL);
	pak->cryptoflags = 4;  /* 3 */
	send_to_epad(idb,pak);
	wait_test_done();
    }
   
    sprintf(ktest_table[CRYPTO_HMA_ALLOC][5].desc,
	    "Test %d: Request to allocate HMA memory from size 1 to %d\n",
	    4, MAX_ALLOC);
    /* Now free all the allocate memory */
    /* deallocate all hma, that were sucessfully allocated */
    for (i=0; i < MAX_ALLOC-1; i++) {
	if (idb->output_qcount) {
	    if (epad_throttle(idb, NULL)) {
		i--;
		continue;
	    } else {
		break;
	    }
	}
	ifree.call_id = CRYPTO_HMA_FREE;
	ifree.handle = handle_array[i];
	pak = create_key_cmd(inpak, CRYPTO_HMA_FREE, &ifree, NULL);
	pak->cryptoflags = 5; /* 4 */
	send_to_epad(idb, pak); 
    }
    wait_test_done();
}

/*****************************************************************************
 * ktest_hMemFree
 * Various test to test the functionality of hMemFree command
 * Test 0: Request to free an allocated HMA memory
 * Test 1: Request to free a unallocated HMA memory
 *****************************************************************************/
static void ktest_hMemFree (void)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    hMemAlloc_t input;
    paktype *pak;
    int testnum = 0;

    if (ABORT_TEST) return;
    ds->callback = service_ktest_hMemFree;
    
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING HMA FREE ****\n\n");
    /**** TEST 0 ****/
    input.call_id = CRYPTO_HMA_ALLOC;
    input.size = 512;
    input.attribute = 0x20;
    pak = create_key_cmd(inpak, CRYPTO_HMA_ALLOC, &input, NULL);
    pak->cryptoflags = 0; 
    sprintf(ktest_table[CRYPTO_HMA_FREE][testnum].desc, 
	    "Test %d: Request to free an allocated HMA memory\n", testnum++);
    send_to_epad(idb, pak);
    wait_test_done();

    /**** TEST 1 ****/
    sprintf(ktest_table[CRYPTO_HMA_FREE][testnum].desc, 
	    "Test %d: Request to free a unallocated HMA memory\n", testnum++);
    hMemFree(1, 0);
}
/*****************************************************************************
 * ktest_hMemWrite
 * Various test to test the functionality of hMemWrite command
 * Test 0: 
 *	- Allocate HMA memory
 *	- Write to the allocated HMA memory
 *	- Read the allocated HMA memory and check the result.
 * Test 1: Request a HMA memory write with size = 0;
 * Test 2: Request a HMA memory write that went beyond the memory boundary.
 * Test 3: Request to write to a block with no write access
 * Test 4: Request to write to a block that has not been allocated
 *****************************************************************************/
static void ktest_hMemWrite (void)
{
    int testnum = 0;    
    char blk_64[64];

    if (ABORT_TEST) return;
   
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING HMA WRITE ****\n\n");
    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_HMA_WRITE][testnum].desc, 
	    "Test %d: Request a HMA memory write, \n      then read the memory to verify the content\n", 
	    testnum++);
    hMemWrite(0, ghandle3, sizeof(write_pattern), (uchar *)write_pattern);
    wait_test_done();

    /**** TEST 1 ****/
    sprintf(ktest_table[CRYPTO_HMA_WRITE][testnum].desc, 
	    "Test %d: Request to write zero byte\n", testnum++);
    hMemWrite(1, ghandle2, 0, (uchar *)write_pattern);
    
    /**** TEST 2 ****/
    sprintf(ktest_table[CRYPTO_HMA_WRITE][testnum].desc, 
	    "Test %d: Request a HMA memory write beyond the memory boundary\n",
	    testnum++);
    hMemWrite(2, ghandle3, sizeof(write_pattern) +1, (uchar *)write_pattern);

    /**** TEST 3 ****/
    sprintf(ktest_table[CRYPTO_HMA_WRITE][testnum].desc, 
	    "Test %d: Request to write to a block with no write access\n", 
	    testnum++);
    hMemWrite(3, ghandle1, GHANDLE1_SIZE, blk_64);

    /**** TEST 4 ****/ 
    sprintf(ktest_table[CRYPTO_HMA_WRITE][testnum].desc, 
	    "Test %d: Request to write to a block that has not been allocated\n", 
	    testnum++);
    hMemWrite(4, 0, sizeof(write_pattern), (uchar *)write_pattern);
}

/*****************************************************************************
 * ktest_hMemRead
 * Test 0: Request to read size 0
 * Test 1: Request to read from an invalid handle
 *****************************************************************************/
static void ktest_hMemRead (void)
{
    int testnum = 0;    

    if (ABORT_TEST) return;

    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING HMA READ ****\n\n");
    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_HMA_READ][testnum].desc, 
	    "Test %d: Request to read size 0 \n", testnum++);
    hMemRead(0, ghandle1, 0);

    /**** TEST 1 ****/
    sprintf(ktest_table[CRYPTO_HMA_READ][testnum].desc, 
	    "Test %d: Request to read from an invalid handle\n", testnum++);
    hMemRead(1, 0, GHANDLE1_SIZE);
}

/*****************************************************************************
 * ktest_hMemCopy
 * Allocate 2 HMA memory:
 * Test 0: Normal memory copy
 * Test 1: copy from a non-existing source HMA block
 * Test 2: copy to a non-existing destination HMA block
 * Test 3: copy from large block to small block 
 *****************************************************************************/
static void ktest_hMemCopy (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING HMA COPY ****\n\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_HMA_COPY][testnum].desc, 
	    "Test %d: Request to copy size = %d\n", testnum++, 
	    sizeof(write_pattern));
    hMemCopy(0, sizeof(write_pattern), ghandle3, ghandle4);
    
    /**** TEST 1 ****/
    sprintf(ktest_table[CRYPTO_HMA_COPY][testnum].desc,
	    "Test %d: Request to copy from a non-existing source HMA block\n", 
	    testnum++);
    hMemCopy(1, sizeof(write_pattern), 1, ghandle4);

    /**** TEST 2 ****/
    sprintf(ktest_table[CRYPTO_HMA_COPY][testnum].desc, 
	    "Test %d: Request to copy to a non-existing destination HMA block\n",
	    testnum++);
    hMemCopy(2, sizeof(write_pattern), ghandle3, 0);

    /**** TEST 3 ****/
    sprintf(ktest_table[CRYPTO_HMA_COPY][testnum].desc, 
	    "Test %d: Request to copy a large block to small block\n", 
	    testnum++);
    hMemCopy(3, sizeof(write_pattern), ghandle2, ghandle3);
}
/*****************************************************************************
 * ktest_hMemSyndrome
 * Test 0: Request to generate HMA syndrome
 * Test 1: Request to generate a HMA syndrome with invalid handle
 * Test 2: Request to generate a HMA syndrome with offset went beyond
 *	the memory size
 * Test 3: Request HMA syndrome with len > buffer size 
 * Test 4: Request to generate a HMA syndrome with size 0
 *****************************************************************************/
static void ktest_hMemSyndrome (void)
{

    int testnum = 0;

    if (ABORT_TEST) return;
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING HMA SYNDROME ****\n\n");

    /**** TEST 0 ****/	
    sprintf(ktest_table[CRYPTO_MEM_SYNDROME][testnum].desc, 
	    "Test %d: Request to generate HMA syndrome of %d size\n",
	    testnum++, GHANDLE2_SIZE);
    hMemSyndrome(0, ghandle2, 0, GHANDLE2_SIZE, 10);

    /**** TEST 1 ****/
    sprintf(ktest_table[CRYPTO_MEM_SYNDROME][testnum].desc,
	    "Test %d: Request HMA syndrome with invalid handle\n", testnum++);
    hMemSyndrome(1, 0, 0, GHANDLE2_SIZE, 10);

    /**** TEST 2 ****/
    sprintf(ktest_table[CRYPTO_MEM_SYNDROME][testnum].desc,
	    "Test %d: Request HMA syndrome with offset %d, buffer size=%d\n", 
	    testnum++, GHANDLE2_SIZE+5, GHANDLE2_SIZE);
    hMemSyndrome(2, ghandle2, GHANDLE2_SIZE + 5, GHANDLE2_SIZE, 10);

    /**** TEST 3 ****/
    sprintf(ktest_table[CRYPTO_MEM_SYNDROME][testnum].desc,
	    "Test %d: Request HMA syndrome with len (%d) > buffer size (%d)\n",
	    testnum++, GHANDLE2_SIZE + 1, GHANDLE2_SIZE);
    hMemSyndrome(3, ghandle2, 0, GHANDLE2_SIZE, GHANDLE2_SIZE+1);

    /**** TEST 4 ****/
    sprintf(ktest_table[CRYPTO_MEM_SYNDROME][testnum].desc,
	    "Test %d: Request HMA syndrome with len 0\n", testnum++);
    hMemSyndrome(4, ghandle2, 0, GHANDLE2_SIZE, 0);  
}
/*****************************************************************************
 * ktest_hReadRandom
 * Various test to test the functionality of hReadRandom command
 * Test 0: Request to read a random number
 * Test 1: Request to read a random number of size 0
 * Test 2: Request to read a random number of maximum size = 1024
 *****************************************************************************/
static void ktest_hReadRandom (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);

    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING READ RANDOM ****\n\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_READ_RANDOM][testnum].desc,
	    "Test %d: Request to read a random number\n", testnum++);
    hReadRandom(0, sizeof(ushort));

    /**** TEST 1 ****/    
    sprintf(ktest_table[CRYPTO_READ_RANDOM][testnum].desc,
	    "Test %d: Request to read a random number of size 0\n", 
	    testnum++);
    hReadRandom(1, 0);

   /**** TEST 2 ****/
    sprintf(ktest_table[CRYPTO_READ_RANDOM][testnum].desc,
	    "Test %d: Request to read a random number of maximum size = %d\n",
	    testnum++, SIZE_1K - sizeof(hReadRandom_t));
    hReadRandom(2, SIZE_1K - sizeof(hReadRandom_t));
}
/*****************************************************************************
 * ktest_hGenRandom
 * Test 0: Request to generate random number
 * Test 1: Request to generate a random number with size 0
 * Test 2: Request to generate a random number with invalid handle
 * Test 3: Request to generate a random number with maximum size
 * Test 4: Request to generate a random number over the maximum size
 *****************************************************************************/
static void ktest_hGenRandom (void)
{
    int testnum = 0;
 
    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);

    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING GENERATE RANDOM ****\n\n");
    set_priviledge();
    set_randomseed();
    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_GEN_RANDOM][testnum].desc,
	    "Test %d: Request to generate random number\n", testnum++);
    hGenRandom(0, 5, ghandle3);


    /**** TEST 1 ****/
    sprintf(ktest_table[CRYPTO_GEN_RANDOM][testnum].desc,
	    "Test %d: Request to generate a random number with size 0\n", 
	    testnum++);
    hGenRandom(1, 0, ghandle1);

    /**** TEST 2 ****/
    sprintf(ktest_table[CRYPTO_GEN_RANDOM][testnum].desc,
	    "Test %d: Request to generate a random number with invalid handle\n", 
	    testnum++);
    hGenRandom(2, 5, 12345);

    /**** TEST 3 ****/
    sprintf(ktest_table[CRYPTO_GEN_RANDOM][testnum].desc,
	    "Test %d: Request to generate a random number with maximum size %d\n", 
	    testnum++, SIZE_1K );
    hGenRandom(3, SIZE_1K, ghandle5);

#if 0
    /**** TEST 4 ****/
    sprintf(ktest_table[CRYPTO_GEN_RANDOM][testnum].desc,
	    "Test %d: Request to generate a random number over the maximum size %d\n", 
	    testnum++, SIZE_1K +1);
    hGenRandom(4, SIZE_1K +1, ghandle5); 
#endif
}

/*****************************************************************************
 * ktest_gendhpair
 * Test 0: Request to generate DH pair
 * Test 1: Request to generate DH pair with an invalid HMA handle
 * Test 2: Request to generate DH pair with size = 0
 *****************************************************************************/
static void ktest_gendhpair (void)
{
   int testnum = 0;

   if (ABORT_TEST) return;
   process_set_boolean(next_test, FALSE);
   if (verbose)
       pas_diag_errmsg(MSG_STATUS, "**** TESTING GENERATE DH PAIR ****\n\n");

   /**** TEST 0 ****/
   sprintf(ktest_table[CRYPTO_GEN_DH_PAIR][0].desc,
	   "Test %d: Generate DH pair\n", testnum++);
   gendhpair(0,ghandle1, 64); 

   /**** TEST 1 ****/
   sprintf(ktest_table[CRYPTO_GEN_DH_PAIR][1].desc,
	   "Test %d: Generate DH pair command with invalid HMA handle\n", 
	   testnum++);
   gendhpair(1, 1, 64);

   /**** TEST 2 ****/
   sprintf(ktest_table[CRYPTO_GEN_DH_PAIR][2].desc,
	   "Test %d: Generate DH pair with size = 0\n", testnum++);
   gendhpair(2, ghandle1, 0);
}

/*****************************************************************************
 * ktest_getdhshared
 * Various test to test the functionality of get DH shared number command.
 * Test 0: Request to get DH shared number
 * Test 1: Request to get DH shared number with size 0
 * Test 2: Request to get DH shared number with an invalid secret handle
 *****************************************************************************/
static void ktest_getdhshared (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING GEN DH SHARE NUMBER ****\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_GET_DH_SHARED][0].desc,
	    "Test %d: Request to get DH shared number\n", testnum++);
    getdhshared(0, ghandle1, ghandle2, 64, gdh_public);

    /**** TEST 1 ****/ 
    sprintf(ktest_table[CRYPTO_GET_DH_SHARED][1].desc,
	    "Test %d: Request to get DH shared number with size = 0\n", 
	    testnum++);
    getdhshared(1, ghandle1, ghandle2, 0, gdh_public);

    /**** TEST 3 ****/
    sprintf(ktest_table[CRYPTO_GET_DH_SHARED][2].desc,
	    "Test %d: Request to get DH shared number with invalid secret handle\n", 
	    testnum++);
    getdhshared(2, 0, ghandle2, 64, gdh_public);
}

/*****************************************************************************
 * Test 0: Request to set the key.
 * Test 1: Request to set the Cipher with invalid shared HMA handle.
 * Test 2: Request to set the Cipher with invalid key HMA handle.
 *****************************************************************************/
static void ktest_hSetCipherkey (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING SET CIPHER KEY ****\n\n");
 
    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_SET_CIPHER_KEY][0].desc,
	    "Test %d: Request to set the CYPHER key.\n", testnum++);	
    setCipherkey(0, GHANDLE2_SIZE, ghandle2, ghandle3, 0, 0xA);

    /**** TEST 1 ****/    
    sprintf(ktest_table[CRYPTO_SET_CIPHER_KEY][1].desc,
	    "Test %d: Request to set the CIPHER key with invalid shared HMA handle.\n", 
	    testnum++);
    setCipherkey(1, GHANDLE2_SIZE, 0, ghandle3, 0, 0xA);

    /**** TEST 2 ****/    
    sprintf(ktest_table[CRYPTO_SET_CIPHER_KEY][2].desc,
	    "Test %d: Request to set the Cipher key with invalid key HMA handle.\n", 
	    testnum++);
    setCipherkey(2, GHANDLE2_SIZE, ghandle2, 0, 0, 0xA);
}
/*****************************************************************************
 * ktest_hDHoneTimePad
 * Test 0: Request a DH one time pad
 * Test 1: Request a DH one time pad with an invalid shared handle
 * Test 2: Request a DH one time pad with size 0
 *****************************************************************************/
static void ktest_hDHoneTimePad (void)
{

    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING DH ONE TIME PAD ****\n\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_DH_ONE_TIME_PAD][testnum].desc,
	    "Test %d: Request a DH one time pad\n", testnum++);
    hDHoneTimePad(0, 64, ghandle1);

    /**** TEST 1 ****/    
    sprintf(ktest_table[CRYPTO_DH_ONE_TIME_PAD][testnum].desc,
	    "Test %d: Request a DH one time pad with an invalid shared handle\n", 
	    testnum++);
    hDHoneTimePad(1, 64, 0);

    /**** TEST 2 ****/    
    sprintf(ktest_table[CRYPTO_DH_ONE_TIME_PAD][testnum].desc,
	    "Test %d: Request a DH one time pad with size 0\n", testnum++);
    hDHoneTimePad(2, 0, ghandle1);
}

/*****************************************************************************
 * ktest_hGetDHkey
 * Test 0: Request to get a DH key
 * Test 1: Request to get a DH key with an invalid secret handle
 * Test 2: Request to get a DH key with an invalid DH modulus handle
 * Test 3: Request to get a DH key with an invalid key 1 handle
 * Test 4: Request to get a DH key with an invalid key 2 handle
 * Test 5: Request to get a DH key with an invalid key 1 size
 * Test 6: Request to get a DH key with an invalid key 2 size
 *****************************************************************************/
static void ktest_hGetDHkey (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING GET DH KEY ****\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_GET_DH_KEY][0].desc,
	    "Test %d: Request to get a DH key\n", testnum++);
    getDHkey(0, PUBLIC_SIZE, ghandle1, GHANDLE3_SIZE, ghandle3, GHANDLE4_SIZE,
	     ghandle4, gdh_public);

    /**** TEST 1 ****/
    sprintf(ktest_table[CRYPTO_GET_DH_KEY][1].desc,
	    "Test %d: Request to get a DH key given an invalid secret handle\n", 
	    testnum++);
    getDHkey(1, PUBLIC_SIZE, 0, GHANDLE3_SIZE, ghandle3, GHANDLE4_SIZE,
	     ghandle4, gdh_public);

    /**** TEST 2 ****/    
    sprintf(ktest_table[CRYPTO_GET_DH_KEY][2].desc,
	    "Test %d: Request to get a DH key with size  = 0\n", testnum++);
    getDHkey(2, 0, ghandle1, GHANDLE3_SIZE, ghandle3, GHANDLE4_SIZE,
	     ghandle4, gdh_public);

    /**** TEST 3 ****/    
    sprintf(ktest_table[CRYPTO_GET_DH_KEY][3].desc,
	    "Test %d: Request to get a DH key with an invalid key 1 handle\n",
	    testnum++);
    getDHkey(3, PUBLIC_SIZE, ghandle1, GHANDLE3_SIZE, 0, GHANDLE4_SIZE,
	     ghandle4, gdh_public);

    /**** TEST 4 ****/    
    sprintf(ktest_table[CRYPTO_GET_DH_KEY][4].desc,
	    "Test %d: Request to get a DH key with an invalid key 2 handle\n",
	    testnum++);
    getDHkey(4, PUBLIC_SIZE, ghandle1, GHANDLE3_SIZE, ghandle3, GHANDLE4_SIZE,
	     0, gdh_public);

     /**** TEST 5 ****/    
    sprintf(ktest_table[CRYPTO_GET_DH_KEY][5].desc,
	    "Test %d: Request to get a DH key with an invalid key 1 size = %d\n", 
	    testnum++, GHANDLE3_SIZE-10);
    getDHkey(5, PUBLIC_SIZE, ghandle1, GHANDLE3_SIZE-10, ghandle3, 
	     GHANDLE4_SIZE, ghandle4, gdh_public);

     /**** TEST 6 ****/    
    sprintf(ktest_table[CRYPTO_GET_DH_KEY][6].desc,
	    "Test %d: Request to get a DH key with an invalid key 2 size = %d\n", 
	    testnum++, GHANDLE4_SIZE+1);
    getDHkey(6, PUBLIC_SIZE, ghandle1, GHANDLE3_SIZE, ghandle3, 
	     GHANDLE4_SIZE+1, ghandle4, gdh_public);  
}

/*****************************************************************************
 * ktest_freesession
 * Test 0: Request to release a session control block
 * Test 1: Request to release a session control block that's already freed.
 *****************************************************************************/
static void ktest_freesession (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
         pas_diag_errmsg(MSG_STATUS, "**** TESTING SET FREE SESSION ****\n\n");

    /**** TEST 0 ****/ 
    /* assume that session already created by new session test */
    sprintf(ktest_table[CRYPTO_FREE_SESSION][0].desc,
	    "Test %d: Request to release a session control block\n", 
	    testnum++);
    freeSession(0, gsession);

    /**** TEST 1 ****/    
    sprintf(ktest_table[CRYPTO_FREE_SESSION][1].desc,
	    "Test %d: Request to release a already freed session\n", 
	    testnum++);
    freeSession(1, gsession);
}

/*****************************************************************************
 * ktest_newsession
 * Test 0: Request to create a new session control block
 * Test 1: Request to create a new session control block with invalid key
 * Test 2: Request to create a new session control block with undefine method
 *****************************************************************************/
static void ktest_newsession (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
         pas_diag_errmsg(MSG_STATUS, "**** TESTING SET NEW SESSION ****\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_NEW_SESSION][0].desc,
	    "Test %d:  Request to create a new session control block\n", 
	    testnum++);
    newSession(0, ghandle1, ghandle2, 0xA);

    /**** TEST 1 ****/    
    sprintf(ktest_table[CRYPTO_NEW_SESSION][1].desc,
	    "Test %d: Request to create a new session control block with invalid key\n", 
	    testnum++);
    newSession(1, ghandle1, 0, 0xA);

    /**** TEST 2 ****/
    sprintf(ktest_table[CRYPTO_NEW_SESSION][2].desc,
	    "Test %d: Request to create a new session control block with undefine method\n", 
	    testnum++);
    newSession(2, ghandle1, ghandle2, 100);
}

/*****************************************************************************
 * ktest_hSHA
 * Test 0: Request to compute secure hash number
 * Test 1: Request to compute secure hash with message size is zero
 *****************************************************************************/
static void ktest_hSHA (void)
{
    int testnum = 0;
    char message[SHA_HASH_SIZE] = "hellohellohellohello";

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING SECURE HASH ****\n\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_SHA][testnum].desc,
	    "Test %d: Request to compute secure hash number\n", testnum++);
    hSHA(0, SHA_HASH_SIZE, message);

    /**** TEST 1 ****/    
    sprintf(ktest_table[CRYPTO_SHA][testnum].desc,
	    "Test %d: Request to compute secure hash with message size is zero\n", 
	    testnum++);
    hSHA(1, 0, message);
}

/*****************************************************************************
 * ktest_hSHAinit
 * Test 0: Request to initialize secure hash
 *****************************************************************************/
static void ktest_hSHAinit (void) 
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hSHAinit_t input;
    char message[20] ="hellohellohellohello";
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    ds->callback = service_ktest_hSHAinit;
    if (verbose) 
        pas_diag_errmsg(MSG_STATUS, "**** TESTING SECURE HASH INIT ****\n\n");
    input.call_id = CRYPTO_SHA_INIT;

    /**** TEST 0 ****/
    pak = create_key_cmd(inpak, CRYPTO_SHA_INIT, &input, message);
    pak->cryptoflags = 0; 
    sprintf(ktest_table[(int)input.call_id][(int)pak->cryptoflags].desc,
	    "Test %d: Request to initialize secure hash\n", testnum++);
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * ktest_hSHAupdate
 * Test 0: Request an update of the secure hash
 *****************************************************************************/
static void ktest_hSHAupdate (void)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hSHAupdate_t input;
    char message[20] = "hellohellohellohello";
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    ds->callback = service_ktest_hSHAupdate;
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING SECURE HASH UPDATE ****\n\n");
    input.call_id = CRYPTO_SHA_UPDATE;

    /**** TEST 0 ****/
    input.size = sizeof(message);
    pak = create_key_cmd(inpak, CRYPTO_SHA_UPDATE, &input, message);
    pak->cryptoflags = 0; 
    sprintf(ktest_table[(int)input.call_id][(int)pak->cryptoflags].desc,
	    "Test %d: Request to update secure hash\n", testnum++);
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * ktest_hSHAfinal
 * Test 0: Request to finalize a secure hash computations
 *****************************************************************************/
static void ktest_hSHAfinal (void)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak=NULL;
    hSHAfinal_t input;
    char message[20] = "hellohellohellohello";
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    ds->callback = service_ktest_hSHAfinal;
    if (verbose) 
        pas_diag_errmsg(MSG_STATUS, "**** TESTING SECURE HASH FINAL ****\n\n");
    input.call_id = CRYPTO_SHA_FINAL;

    /**** TEST 0 ****/
    /* set input.context = */
    pak = create_key_cmd(inpak, CRYPTO_SHA_FINAL, &input, message);
    pak->cryptoflags = 0;
    sprintf(ktest_table[(int)input.call_id][(int)pak->cryptoflags].desc,
	    "Test %d: Request to finalize a secure hash computations\n", 
	    testnum++);
    send_to_epad(idb, pak);
    wait_test_done();
}
/*****************************************************************************
 * ktest_hGenDSSparameters
 * Test 0: Request to generate DSS parameters with size 64
 * Test 1: Request to generate DSS parameters with size 0
 * Test 2: Request to generate DSS parameters with size 200
 *****************************************************************************/
static void ktest_hGenDSSparameters (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING GEN DSS PARAMETERS ****\n\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_GEN_DSS_PARAMETERS][testnum].desc,
	    "Test %d: Request to generate DSS parameters with size 64\n", 
	    testnum++);
    hGenDSSparameters(0, 64);

    /**** TEST 1 ****/    
    sprintf(ktest_table[CRYPTO_GEN_DSS_PARAMETERS][testnum].desc,
	    "Test %d: Request to generate DSS parameters with size 0\n", 
	    testnum++);
    hGenDSSparameters(1, 0);

    /**** TEST 2 ****/    
    sprintf(ktest_table[CRYPTO_GEN_DSS_PARAMETERS][testnum].desc,
	    "Test %d: Request to generate DSS parameters with size 200\n", 
	    testnum++);
    hGenDSSparameters(2, 200);
}

/*****************************************************************************
 * ktest_hGetDSSpqg
 * Test 0: Request to get DSS pgq
 * Test 1: Request to get DSS pqg of size 5
 * Test 2: Request to get DSS pqg with size 0
 * Test 3: Request to get DSS pqg with size 500
 *****************************************************************************/
static void ktest_hGetDSSpqg (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING GET DSS PQG ****\n\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_GET_DSS_PQG][testnum].desc,
	    "Test %d: Request to get DSS pqg of size = %d\n", testnum++,	64);
    hGetDSSpqg(0, 64);

    /**** TEST 1 ****/
    sprintf(ktest_table[CRYPTO_GET_DSS_PQG][testnum].desc,
	    "Test %d: Request to get DSS pqg of size = %d\n", 
	    testnum++,	5);
    hGetDSSpqg(1, 5);

    /**** TEST 2 ****/    
    sprintf(ktest_table[CRYPTO_GET_DSS_PQG][testnum].desc,
	    "Test %d: Request to get DSS pqg with size 0\n", testnum++);
    hGetDSSpqg(2, 0);

    /**** TEST 3 ****/    
    sprintf(ktest_table[CRYPTO_GET_DSS_PQG][testnum].desc,
	    "Test %d: Request to get DSS pqg with size 500\n", testnum++);
    hGetDSSpqg(3, 500);
}
/*****************************************************************************
 * ktest_hGenDSSnumber
 * Test 0: Request to generate DSS number
 * Test 1: Request to generate DSS number with invalid HMA handle
 *****************************************************************************/
static void ktest_hGenDSSnumber (void)
{
    int testnum = 0;

    process_set_boolean(next_test, FALSE);
    if (ABORT_TEST) return;
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING GEN DSS NUMBER ****\n\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_GEN_DSS_NUMBER][testnum].desc,
	    "Test %d: Request to generate DSS number\n", testnum++);
    hGenDSSnumber(0, ghandle1);

    /**** TEST 1 ****/
    sprintf(ktest_table[CRYPTO_GEN_DSS_NUMBER][testnum].desc,
	    "Test %d: Request to generate DSS number with invalid handle\n", 
	    testnum++);
    hGenDSSnumber(1, 32000);
}

/*****************************************************************************
 * ktest_hGenDSSkey
 * Test 0: Request to generate DSS key
 * Test 1: REquest to generate DSS key with an invalid key handle
 * Test 2: Request to generate DSS key with size = 0
 *****************************************************************************/
static void ktest_hGenDSSkey (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING GEN DSS KEY ****\n\n");
    set_priviledge();
    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_GEN_DSS_KEY][testnum].desc,
	    "Test %d: Request to generate DSS key\n", testnum++);
    hGenDSSkey(0, ghandle3, 64);

    /**** TEST 1 ****/   
    set_priviledge();
    sprintf(ktest_table[CRYPTO_GEN_DSS_KEY][testnum].desc,
	    "Test %d: Request to generate DSS key with an invalid key handle\n", 
	    testnum++);
    hGenDSSkey(1, 0, 64);

    /**** TEST 2 ****/
    set_priviledge();
    sprintf(ktest_table[CRYPTO_GEN_DSS_KEY][testnum].desc,
	    "Test %d: Request to generate DSS key with size = 0\n", testnum++);
    hGenDSSkey(2, ghandle1, 0);
}

/*****************************************************************************
 * ktest_hGenDSSsignature
 * Test 0: Request to generate DSS signature
 * Test 1: Request to generate DSS signature with an invalid handle
 *****************************************************************************/
static void ktest_hGenDSSsignature (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING GEN DSS SIGNATURE ****\n\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_GEN_DSS_SIGNATURE][testnum].desc,
	    "Test %d: Request to generate DSS signature\n", testnum++);
    hGenDSSsignature(0, ghandle3, 64, ghash);

    /**** TEST 1 ****/
    sprintf(ktest_table[CRYPTO_GEN_DSS_SIGNATURE][testnum].desc,
	    "Test %d: Request to generate DSS signature with an invalid handle\n", 
	    testnum++);
    hGenDSSsignature(1, 0, 64, ghash);
}
/*****************************************************************************
 * ktest_hVerDSSsignature
 * Test 0: Request to verify DSS signature
 * Test 1: Request to verify DSS signature on an invalid hash
 * Test 2: Request to verify DSS signature on an invalid R
 * Test 3: Request to verify DSS signature on an invalid S
 * Test 4: Request to verify DSS signature on an invalid public number
 *****************************************************************************/
static void ktest_hVerDSSsignature (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING VER DSS SIGNATURE ****\n\n");

    /**** TEST 0 ****/
    sprintf(ktest_table[CRYPTO_VER_DSS_SIGNATURE][testnum].desc,
	    "Test %d: Request to verify DSS signature\n", testnum++);
    hverDSSsignature(0, sizeof(gdss_public), ghash, gR, gS, gdss_public);

    /**** TEST 1 ****/    
    sprintf(ktest_table[CRYPTO_VER_DSS_SIGNATURE][testnum].desc,
	    "Test %d: Request to verify DSS signature on an invalid hash\n", 
	    testnum++);
    hverDSSsignature(1, sizeof(gdss_public), 0, gR, gS, gdss_public);

    /**** TEST 2 ****/    
    sprintf(ktest_table[CRYPTO_VER_DSS_SIGNATURE][testnum].desc,
	    "Test %d: Request to verify DSS signature on an invalid R\n", 
	    testnum++);
    hverDSSsignature(2, sizeof(gdss_public), ghash, 0, gS, gdss_public);

    /**** TEST 3 ****/    
    sprintf(ktest_table[CRYPTO_VER_DSS_SIGNATURE][testnum].desc,
	    "Test %d: Request to verify DSS signature on an invalid S\n", 
	    testnum++);
    hverDSSsignature(3, sizeof(gdss_public), ghash, gR, 0, gdss_public);

    /**** TEST 4 ****/    
    sprintf(ktest_table[CRYPTO_VER_DSS_SIGNATURE][testnum].desc,
	    "Test %d: Request to verify DSS signature on an invalid public number\n", 
	    testnum++);
    hverDSSsignature(4, 0, ghash, gR, gS, NULL);
}

/*****************************************************************************
 * ktest_epaGetStatus
 * Test 0:  Request to get system status
 *****************************************************************************/
static void ktest_epaGetStatus (void)
{    
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    epaGetStatus_t istatus;
    paktype *pak;

    if (ABORT_TEST) return;
    ds->callback = service_ktest_epaGetStatus;

    istatus.call_id =  CRYPTO_GET_EPA_STATUS;
    pak = create_key_cmd(inpak, CRYPTO_GET_EPA_STATUS, &istatus, NULL);
    pak->cryptoflags = 0;
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * ktest_epaSetStatus
 * Various test to test the system status command
 *****************************************************************************/
static void ktest_epaSetStatus (void)
{
    int testnum = 0;

    if (ABORT_TEST) return;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING SET STATUS ****\n\n");

    /**** TEST 0 ****/
    ktest_epaZeroize();
    set_passwd();
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][0].desc,
	    "Test %d: Request to set system status TESTING\n", testnum++);
    epaSetStatus(0, sizeof(passwd), EPA_TESTING, EPA_TESTING, (uchar *)passwd);
    wait_test_done();
    
    /**** TEST 1 ****/ 
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][1].desc, 
	    "Test %d: Request to set system status\n  with undefine mask = 0x%x\n", testnum++, 0x02000000);
    epaSetStatus(1, sizeof(passwd), 0x02000000, EPA_TESTING, (uchar *)passwd);

    /**** TEST 2 ****/
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][2].desc, 
	    "Test %d: Request to set EPA_TAMPER status bit which\n  require password, but don't give password\n", testnum++);
    epaSetStatus(2, 0, EPA_TAMPER, EPA_TESTING, (uchar *)passwd);

    /**** TEST 3 ****/ 
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][3].desc
	    , "Test %d: Request to set system status bit 0x%x\n  which doesn't allow to set\n", 
	    testnum++, EPA_PASSWD);
    epaSetStatus(3, sizeof(passwd), EPA_PASSWD, EPA_PASSWD, (uchar *)passwd);

    /**** TEST 4 ****/
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][4].desc, 
	    "Test %d: Request to set system status bit 0x%x\n   which doesn't allow to set\n", testnum++, EPA_RNG_SD);
    epaSetStatus(4, sizeof(passwd), EPA_RNG_SD, EPA_RNG_SD, (uchar *)passwd);

    /**** TEST 5 ****/
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][5].desc, 
	    "Test %d: Request to set system status bit 0x%x\n  which doesn't allow to set\n", testnum++, EPA_DSSKEY);
    epaSetStatus(5, sizeof(passwd),EPA_DSSKEY, EPA_DSSKEY , (uchar *)passwd);

    /**** TEST 6 ****/
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][6].desc,
	    " Test %d: Request to set system status bit 0x%x\n  which doesn't allow to set\n", 
	    testnum++, EPA_1024FAIL);
    epaSetStatus(6, sizeof(passwd), EPA_1024FAIL, EPA_1024FAIL, 
		 (uchar *)passwd);

    /**** TEST 7 ****/
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][7].desc, 
	    "Test %d: Request to set system status bit 0x%x\n  which doesn't allow to set\n", 
	    testnum++, EPA_1046FAIL);
    epaSetStatus(7, sizeof(passwd), EPA_1046FAIL, EPA_1046FAIL, (uchar *)passwd);

    /**** TEST 8 ****/ 
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][8].desc, 
	    "Test %d: Request to set system status bit 0x%x\n  which doesn't allow to set\n", 
	    testnum++, EPA_BOOTFAIL);
    epaSetStatus(8, sizeof(passwd), EPA_BOOTFAIL, EPA_BOOTFAIL, (uchar *)passwd);

    /**** TEST 9 ****/
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][9].desc, 
	    "Test %d: Request to set system status bit 0x%x\n  which doesn't allow to set\n",
	    testnum++, EPA_LED_BOOT);
    epaSetStatus(9, sizeof(passwd), EPA_LED_BOOT, EPA_LED_BOOT, (uchar *)passwd);

    /**** TEST 10 ****/
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][10].desc, 
	    "Test %d: Request to set system status bit 0x%x\n  which doesn't allow to set\n", 
testnum++, EPA_LED_NORMAL);
    epaSetStatus(10, sizeof(passwd), EPA_LED_NORMAL, EPA_LED_NORMAL, (uchar *)passwd);

     /**** TEST 11 ****/
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][11].desc, 
	    "Test %d: Request to set system status bit 0x%x\n  which doesn't allow to set\n", 
	    testnum++, EPA_LED_ERROR);
    epaSetStatus(11, sizeof(passwd), EPA_LED_ERROR, EPA_LED_ERROR, (uchar *)passwd);

    /**** TEST 12 ****/
    sprintf(ktest_table[CRYPTO_SET_EPA_STATUS][12].desc, 
	    "Test %d: Request to set system status bit 0x%x\n  which doesn't allow to set\n", testnum++, EPA_LED_TAMPER);
    epaSetStatus(12, sizeof(passwd), EPA_LED_TAMPER, EPA_LED_TAMPER, (uchar *)passwd);
}  
/*****************************************************************************
 * ktest_epaGetTime
 * Test 0: Get time, then get time again, expect the real time clock
 *	to increase at least by 1.
 *****************************************************************************/
static void ktest_epaGetTime (void)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    int testnum = 0;
    epaGetTime_t input;

    if (ABORT_TEST) return;
    ds->callback = service_ktest_epaGetTime;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING GET TIME ****\n\n");
    input.call_id = CRYPTO_GET_TIME;
    pak = create_key_cmd(inpak, CRYPTO_GET_TIME, &input, NULL);
    pak->cryptoflags = 0; 
    sprintf(ktest_table[(int)input.call_id][(int)pak->cryptoflags].desc, 
	    "Test %d: Get time, then get time again expect \n the real time clock to increase at least by 1\n",
	    testnum++);
    send_to_epad(idb, pak);
    wait_test_done();

    DELAY(1000);
    pak = create_key_cmd(inpak, CRYPTO_GET_TIME, &input, NULL);
    pak->cryptoflags = 1; 
    sprintf(ktest_table[CRYPTO_GET_TIME][(int)pak->cryptoflags].desc,
	    ktest_table[CRYPTO_GET_TIME][0].desc);
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * ktest_epaGetMethods
 * Test 0: Get methods, check for valid methods
 *****************************************************************************/
static void ktest_epaGetMethods (void)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    int testnum = 0;
    epaGetMethods_t input;

    if (ABORT_TEST) return;
    ds->callback = service_ktest_epaGetMethods;
    process_set_boolean(next_test, FALSE);
    if (verbose)
        pas_diag_errmsg(MSG_STATUS, "**** TESTING GET METHODS ****\n\n");
    input.call_id = CRYPTO_GET_METHODS;
    pak = create_key_cmd(inpak, CRYPTO_GET_METHODS, &input, NULL);
    pak->cryptoflags = 0; 
    sprintf(ktest_table[(int)input.call_id][(int)pak->cryptoflags].desc, 
	    "Test %d: Request to get methods and check for valid methods\n", 
	    testnum++);
    send_to_epad(idb, pak);
    wait_test_done();
}

/*****************************************************************************
 * epad_key_hma_cmd
 * All test to test hma command
 *****************************************************************************/
void epad_key_hma_cmd (parseinfo *csb)
{
    epad_instance_t *ds = INSTANCE(epad_gidb[EPAD_KEY_DEVICE]);

    if (verbose) {
	pas_diag_errmsg(MSG_STATUS, "===============================\n");
	pas_diag_errmsg(MSG_STATUS, "HIDDEN MEMORY ARRAY TEST\n");
	pas_diag_errmsg(MSG_STATUS, "===============================\n\n");
    }
    if (!test_setup(2)) {
	return;
    }
    ktest_epaZeroize();
    ktest_hMemAlloc();

    ktest_hMemFree();

    ktest_epaZeroize();
    alloc_global_handle();
    set_passwd();
    clear_tamper();
    ktest_hMemWrite();
    ktest_hMemRead();
    ktest_hMemCopy();
    ktest_hMemSyndrome();

    free_global_handle();

    test_cleanup(2);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epad_key_rnd_cmd
 * All test that would test random command
 *****************************************************************************/
void epad_key_rnd_cmd (parseinfo *csb)
{
    epad_instance_t *ds = INSTANCE(epad_gidb[EPAD_KEY_DEVICE]);

    if (verbose) {
	pas_diag_errmsg(MSG_STATUS, "===============================\n");
	pas_diag_errmsg(MSG_STATUS, "RANDOM NUMBER GENERATOR TEST\n");
	pas_diag_errmsg(MSG_STATUS, "===============================\n\n");
    }
    if (!test_setup(1)) {
	return;
    }
    ktest_epaZeroize();
    alloc_global_handle();
    set_passwd();
    ktest_hGenRandom();
    ktest_hReadRandom();
    free_global_handle();

    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epad_key_sig_cmd
 * Call all test that would test Digital Signature Standard command.
 *****************************************************************************/
void epad_key_dss_cmd (parseinfo *csb)
{
    epad_instance_t *ds = INSTANCE(epad_gidb[EPAD_KEY_DEVICE]);

    if (verbose) {
	pas_diag_errmsg(MSG_STATUS, "===============================\n");
	pas_diag_errmsg(MSG_STATUS, "DIGITAL SIGNATURE STANDARD TEST\n");
	pas_diag_errmsg(MSG_STATUS, "===============================\n\n");
    }
    if (!test_setup(1)) {
	return;
    }
    ktest_epaZeroize();
    alloc_global_handle();
    set_passwd();
    clear_tamper();
    clear_xtract();

    ktest_hSHA();
    ktest_hGenDSSkey();
    ktest_hGenDSSsignature();
    ktest_hVerDSSsignature();

    ktest_hSHAinit();
    ktest_hSHAupdate();
    ktest_hSHAfinal();
    ktest_hGenDSSparameters();
    ktest_hGetDSSpqg();
    ktest_hGenDSSnumber();
    free_global_handle();

    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epad_key_dh_cmd
 * Call all test that would test Diffie Hellman.
 *****************************************************************************/
void epad_key_dh_cmd (parseinfo *csb)
{
    epad_instance_t *ds = INSTANCE(epad_gidb[EPAD_KEY_DEVICE]);

    if (verbose) {
	pas_diag_errmsg(MSG_STATUS, "===============================\n");
	pas_diag_errmsg(MSG_STATUS, "DIFFIE HELLMAN TEST\n");
	pas_diag_errmsg(MSG_STATUS, "===============================\n\n");
    }
    if (!test_setup(1)) {
	return;
    }

    ktest_epaZeroize();
    alloc_global_handle();
    set_passwd();
    clear_tamper();
    clear_xtract();
    ktest_gendhpair();

    ktest_getdhshared();
    ktest_hSetCipherkey();
    ktest_hDHoneTimePad();
    ktest_hGetDHkey();

    free_global_handle();

    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epad_key_session_cmd
 * Call all tests that would test session control block
 *****************************************************************************/
void epad_key_session_cmd (parseinfo *csb)
{
    epad_instance_t *ds = INSTANCE(epad_gidb[EPAD_KEY_DEVICE]);

    if (verbose) {
	pas_diag_errmsg(MSG_STATUS, "===============================\n");
	pas_diag_errmsg(MSG_STATUS, "SESSION CONTROL BLOCK TEST\n");
	pas_diag_errmsg(MSG_STATUS, "===============================\n\n");
    }
    if (!test_setup(2)) {
	return;
    }
    alloc_global_handle();

    ktest_newsession();
    ktest_freesession();

    free_global_handle();
    test_cleanup(2);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epad_key_status_cmd
 * Call all tests that would check status command.
 *****************************************************************************/
void epad_key_status_cmd (parseinfo *csb)
{
    epad_instance_t *ds = INSTANCE(epad_gidb[EPAD_KEY_DEVICE]);

    if (verbose) {
	pas_diag_errmsg(MSG_STATUS, "===============================\n");
	pas_diag_errmsg(MSG_STATUS, "STATUS TEST \n");
	pas_diag_errmsg(MSG_STATUS, "===============================\n\n");
    }
    if (!test_setup(1)) {
	return;
    }
    ktest_epaGetStatus();
    ktest_epaSetStatus();
    ktest_epaGetTime();
    ktest_epaGetMethods();
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epad_key_all_cmd
 *****************************************************************************/
void epad_key_all_cmd (parseinfo *csb)
{
    int i, j;
    int numfail=0, numpass=0;

    if (csb) {
	verbose = GETOBJ(int, 1);
    } else {
	verbose = 2;
    }
    ABORT_TEST = FALSE;
    pas_diag_errmsg(MSG_STATUS, "CY124 Public Key Processor Chip test\n");
    init_ktest_result(); /* For key test purposes only */
    epad_key_hma_cmd(csb);
    epad_key_rnd_cmd(csb);
    epad_key_dss_cmd(csb);
    epad_key_dh_cmd(csb);
    epad_key_session_cmd(csb);
    epad_key_status_cmd(csb);

    for (i=0; i<NUM_KEY_CMD; i++) {
	for (j=0; j<MAX_KEY_TEST; j++) {
	    if (ktest_table[i][j].result == 1) {
		numpass++;
	    } else if (ktest_table[i][j].result == 2) {
		numfail++;
	    }
	}
    }

    pas_diag_errmsg(MSG_SYSTEM, "***Summary:\n");
    pas_diag_errmsg(MSG_SYSTEM, "CY124 Public Key Processor Chip Test\n");
    pas_diag_errmsg(MSG_SYSTEM, "Result: %d test passed, %d test failed\n\n",
		    numpass, numfail);
}

/*****************************************************************************
 * epad_data_session_cmd
 * Allow user to create session, and then send data through that session.
 * Required input are:
 *	number of session to create
 *	number of packet to send to each session
 *	packet size (0=random size)
 *	verbosity level
 *****************************************************************************/
void epad_data_session_cmd (parseinfo *csb)
{
    hwidbtype *keyidb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *keyds = INSTANCE(keyidb);
    hwidbtype *idb = epad_gidb[EPAD_FRAME_DEVICE]; 
    epad_instance_t *ds = INSTANCE(idb);
    int numloop;
    int i, j, cont, ring = EPAD_FRAME_DEVICE;
    int numpack; 
    paktype *pak;
    particletype *hdr;
    int size;
    boolean rndsize;
    boolean result;
    sys_timestamp traffic_timer;
    ulong time;
    int num_err = 0;

    if (csb) {
	numloop = GETOBJ(int, 1);
	numpack = GETOBJ(int, 2);
	size = GETOBJ(int, 3);
	verbose = GETOBJ(int, 4);
    } else {
	numloop = 1;
	numpack = 10000;
	size = 0;
	verbose = 2;
    }
    rndsize = (size == 0) ? TRUE : FALSE;
    decrypt_id = send_id[0];
    if (!test_setup(2)) {
	return;
    }
    ktest_epaZeroize();
    if (ktest_table[CRYPTO_ZEROIZE_EPA][0].result != 1) {
	ABORT_TEST = TRUE;
    }

    set_passwd();
    if (ktest_table[CRYPTO_SET_EPA_PASSWD][0].result != 1) {
	ABORT_TEST = TRUE;
    }

    clear_tamper();
    if (ktest_table[CRYPTO_SET_EPA_STATUS][14].result != 1) {
	ABORT_TEST = TRUE;
    }

    clear_xtract();
    if (ktest_table[CRYPTO_SET_EPA_STATUS][15].result != 1) {
	ABORT_TEST = TRUE;
    }
    stat.num_encrypt_snt = 0;
    stat.num_encrypt_rcv = 0;
    stat.num_decrypt_snt = 0;
    stat.num_decrypt_rcv = 0;

    GET_TIMESTAMP(traffic_timer);
    for (i=0; i < numloop; i++) {
	if (ABORT_TEST) {
	    pas_diag_errmsg(MSG_ERROR, "... ABORT TEST ...\n");
	    break;
	}
	pas_diag_errmsg(MSG_STATUS, "Session #%d", i);
	alloc_global_handle();

	sprintf(ktest_table[CRYPTO_GEN_DH_PAIR][0].desc, 
		"gendhpair %d\n", ghandle1);
	gendhpair(SUCCESS, ghandle1, 64);
	if (ktest_table[CRYPTO_GEN_DH_PAIR][0].result != 1) {
	    ABORT_TEST = TRUE;
	    break;
	}
	
	sprintf(ktest_table[CRYPTO_GET_DH_SHARED][0].desc,
		    "gendhshared secret = %d, shared = %d\n", 
		    ghandle1, ghandle2);
	getdhshared(SUCCESS, ghandle1, ghandle2, 64, gdh_public);
	if (ktest_table[CRYPTO_GET_DH_SHARED][0].result != 1) {
	    ABORT_TEST = TRUE;
	    break;
	}

	sprintf(ktest_table[CRYPTO_SET_CIPHER_KEY][0].desc,
		    "setCipherkey shared = %d, key = %d\n", 
		    ghandle2, ghandle3);
	setCipherkey(SUCCESS, GHANDLE2_SIZE, ghandle2, ghandle3, 0, 0xA);
	if (ktest_table[CRYPTO_SET_CIPHER_KEY][0].result != 1) {
	    ABORT_TEST = TRUE;
	    break;
	}

	sprintf(ktest_table[CRYPTO_SET_CIPHER_KEY][0].desc,
		    "setCipherkey shared = %d, key = %d\n", 
		    ghandle2, ghandle4);
	setCipherkey(SUCCESS, GHANDLE2_SIZE, ghandle2, ghandle4, 0, 0xA);
	if (ktest_table[CRYPTO_SET_CIPHER_KEY][0].result != 1) {
	    ABORT_TEST = TRUE;
	    break;
	}

	sprintf(ktest_table[CRYPTO_NEW_SESSION][0].desc,
		    "newSessiion iv = %d, key = %d\n", ghandle3, ghandle4);
	newSession(SUCCESS, ghandle3, ghandle4, 0xA);
	if (ktest_table[CRYPTO_NEW_SESSION][0].result != 1) {
	     ABORT_TEST = TRUE;
	    break;
	}
	num_traffic = numpack * (i+1);
	((crypto_vector_t *)outidb->crypto_vectors)->media_fastsend = 
	service_encrypt_traffic;
	((crypto_vector_t *)outidb2->crypto_vectors)->media_fastsend = 
	service_decrypt_traffic;
	for (j=0; j < numpack; j++) {
	    if (ABORT_TEST) {
		pas_diag_errmsg(MSG_ERROR, "\n... ABORT TEST ...\n");
		break;
	    }
	    get_rnd_param(FALSE, TRUE, rndsize,&ring, &cont, &size);
	    if ( idb->output_qcount ) {
		if (epad_throttle(idb, NULL)) {
		    j--;
		    continue;
		} else {
		    break;
		}
	    }
	    if (!(pak = epad_genpack(EPAD_FRAME_DEVICE,
				     cont, size, 0, rndsize))) {
		break;
	    }
	    pak->cryptoflags = CRYPTO_ENCRYPT;

	    hdr = epad_create_encryption_blk(idb, ENCRYPT,
					     gsession, giv, 0, pak);
	    if (!hdr) {
		epad_drop_packet(idb, pak);
		break;
	    }
	    pak->unspecified[0] = (ulong)hdr;
	    send_to_epad(idb, pak);
	    if ((verbose) && (!(j % 256))) {	/* display every 256 loops */
		encryption_test_summary(MSG_STATUS);
	    } else if ((verbose == 0) && (!(j % 256))) {
		ttyprintf(CONTTY, ".");
	    }
	}
	wait_test_done();

	sprintf(ktest_table[CRYPTO_FREE_SESSION][0].desc,
		    "freeSession handle = %d\n", gsession);
	freeSession(SUCCESS, gsession);
	if (ktest_table[CRYPTO_FREE_SESSION][0].result != 1) {
	    ABORT_TEST = TRUE;
	    break;
	}
	free_global_handle();
    }
    time = ELAPSED_TIME(traffic_timer);
    num_err = ds->num_too_big + ds->num_nohdr + 
              ds->num_desc_err + ds->num_status_err;
    result = ((!ABORT_TEST) && (!num_err) && 
	      (stat.num_encrypt_snt == stat.num_decrypt_rcv));

    pas_diag_errmsg(MSG_SYSTEM, "***Summary:\n");
    pas_diag_errmsg(MSG_SYSTEM, "***Test ran for %d min\n", time/60000);
    pas_diag_errmsg(MSG_SYSTEM, "Set up %d connection(s).\n", i);
    encryption_test_summary(MSG_SYSTEM);
    pas_diag_errmsg(MSG_SYSTEM, "***Result:  %s\n\n", result ? "Passed" : "Failed");   
   
    test_cleanup(2);
    keyds->callback = crypto_engine_key_callback;
}

/*****************************************************************************
 * epa_frame_diag
 * Call from diagnostic menu to test 1046 Des chip
 *****************************************************************************/
int epa_frame_diag (int bay)
{
    epad_frame_cmd(NULL);
    return 1;
}

/*****************************************************************************
 * epa_key_diag
 * Call from diagnostic menu to test CY124 processor chip
 *****************************************************************************/
int epa_key_diag (int bay)
{
    epad_key_all_cmd(NULL);
    return 1;
}

/*****************************************************************************
 * epa_diag
 * Call from diagnostic menu to test both the CY124 and 1046 chip
 *****************************************************************************/
int epa_key_frame_diag (int bay)
{
    epad_data_session_cmd(NULL);
    return 1;
}

/*****************************************************************************
 * epa_diag
 * 
 *****************************************************************************/
void display_cyl_diag_err (ulong status)
{
    if (status & 0x80000000) {
	pas_diag_errmsg(MSG_ERROR, "Illegal diagnostic request\n");
    } 
    if (status & 0x40000000) {
	pas_diag_errmsg(MSG_ERROR, "DES CFB8 encryption failed\n");
    }
    if (status & 0x20000000) {
	pas_diag_errmsg(MSG_ERROR, "DES CFB8 decryption failed\n");
    }
    if (status & 0x10000000) {
	pas_diag_errmsg(MSG_ERROR, "DES CFB64 encryption failed\n");
    }
    if (status & 0x08000000) {
	pas_diag_errmsg(MSG_ERROR, "DES CFB64 decryption failed\n");
    }
    if (status & 0x04000000) {
	pas_diag_errmsg(MSG_ERROR, "DES CBC64 encryption failed\n");
    }
    if (status & 0x02000000) {
	pas_diag_errmsg(MSG_ERROR, "DES CBC64 decryption failed\n");
    }
    if (status & 0x01000000) {
	pas_diag_errmsg(MSG_ERROR, "DES ECB encryption failed\n");
    }
    if (status & 0x00800000) {
	pas_diag_errmsg(MSG_ERROR, "DES ECB decryption failed\n");
    }
    if (status & 0x00400000) {
	pas_diag_errmsg(MSG_ERROR, "SRAM failed\n");
    }
    if (status & 0x00200000) {
	pas_diag_errmsg(MSG_ERROR, "DRAM failed\n");
    }
    if (status & 0x00100000) {
	pas_diag_errmsg(MSG_ERROR, "BRAM failed\n");
    }
    if (status & 0x00080000) {
	pas_diag_errmsg(MSG_ERROR, "PKP CY1024 failed\n");
    }
    if (status & 0x00040000) {
	pas_diag_errmsg(MSG_ERROR, "DES CY1046 failed\n");
    }
    if (status & 0x00020000) {
	pas_diag_errmsg(MSG_ERROR, "Run-timer clock failed\n");
    }
	
}

void display_cyl_diag_msg (ushort flags, paktype *pak)
{
    particletype *mp;
    cyl_epa_diag_t *diag;

    verbose = TRUE;

    if (!pak_has_particles(pak)) {
	dump_buffer(pak->datagramstart, pak->datagramsize);
	diag = (cyl_epa_diag_t *)pak->datagramstart;
	if (diag->status == 0) {
	    print_pass(CRYPTO_EPA_DIAG, diag->status);
	} else {
	    print_fail(CRYPTO_EPA_DIAG, 0, diag->call_id, diag->status);
	    ttyprintf(CONTTY, "diag message: ");
	    dump_buffer(pak->datagramstart, pak->datagramsize);
	    ttyprintf(CONTTY, "\n");
	    display_cyl_diag_err(diag->diag_status);
	}
	datagram_done(pak);
    } else {
	while ((mp = dequeue_inline(&pak->particlequeue))) {
	    diag = (cyl_epa_diag_t *)mp->data_start;
	    if (diag->status == 0) {
		print_pass(CRYPTO_EPA_DIAG, diag->status);
	    } else {
		print_fail(CRYPTO_EPA_DIAG, 0, diag->call_id, diag->status);
		dump_buffer(mp->data_start, mp->data_bytes);
		ttyprintf(CONTTY, "\n");
		display_cyl_diag_err(diag->diag_status);
	    }	    
	    retparticle(mp);
	}
    }
    process_set_boolean(next_test, TRUE);
}

void cyl_epa_diag (ushort size, uchar function_code, ushort request_length)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    cyl_epa_diag_t input;

    ds->callback = display_cyl_diag_msg;
    process_set_boolean(next_test, FALSE);
    input.call_id = CRYPTO_EPA_DIAG;
    input.status = 0;
    input.size = size;
    input.function_code = function_code;
    input.request_length = request_length;
    input.diag_status = 0;
    pak = create_key_cmd(inpak, CRYPTO_EPA_DIAG, &input, NULL);
    pak->cryptoflags = 0; 
    send_to_epad(idb, pak);
    wait_test_done();
}

int epa_diag4 (int bay)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    if (!test_setup(1)) {
	return 0;
    }
    sprintf(ktest_table[CRYPTO_EPA_DIAG][0].desc, 
	    "Test %d: PKP test\n", 0);
    cyl_epa_diag(16, 4, 8);
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
    return 1;
}

int epa_diag5 (int bay)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    if (!test_setup(1)) {
	return 0;
    }
    sprintf(ktest_table[CRYPTO_EPA_DIAG][0].desc, 
	    "Test %d: RTC test\n", 0);
    cyl_epa_diag(16, 5, 8);
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
    return 1;
}

int epa_diag6 (int bay)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    if (!test_setup(1)) {
	return 0;
    }
    sprintf(ktest_table[CRYPTO_EPA_DIAG][0].desc, 
	    "Test %d: DES CFB8 test\n", 0);
    cyl_epa_diag(16, 6, 8);
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
    return 1;
}

int epa_diag7 (int bay)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    if (!test_setup(1)) {
	return 0;
    }
    sprintf(ktest_table[CRYPTO_EPA_DIAG][0].desc, 
	    "Test %d: DES CFB64 test\n", 0);
    cyl_epa_diag(16, 7, 8);
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
    return 1;
}

int epa_diag8 (int bay)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    if (!test_setup(1)) {
	return 0;
    }
    sprintf(ktest_table[CRYPTO_EPA_DIAG][0].desc, 
	    "Test %d: DES CBC64 test\n", 0);
    cyl_epa_diag(16, 8, 8);
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
    return 1;
}

int epa_diag9 (int bay)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    if (!test_setup(1)) {
	return 0;
    }
    sprintf(ktest_table[CRYPTO_EPA_DIAG][0].desc, 
	    "Test %d: DES ECB test\n", 0);
    cyl_epa_diag(16, 9, 8);
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
    return 1;
}

int epa_diag10 (int bay)
{
    hwidbtype *idb = epad_gidb[EPAD_KEY_DEVICE];
    epad_instance_t *ds = INSTANCE(idb);
    if (!test_setup(1)) {
	return 0;
    }
    sprintf(ktest_table[CRYPTO_EPA_DIAG][0].desc, 
	    "Test %d: To be define test\n", 0);
    cyl_epa_diag(16, 10, 8);
    test_cleanup(1);
    ds->callback = crypto_engine_key_callback;
    return 1;
}

static char test_particle1[512] =  {
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"ABCDABCDABCDABCD"
"Re-reading the d"
"atabase...Alias "
"updated successf"
"modifying alias "
"ases.n = Add a n"
"with prompting)."
" used in this se"
"ating aliases..?"
"atching specifie"
"imit displays me"
"ding deletions.T"
"hat key isn't us"
"ed in this secti"
"ion.Press the ke"
"y you want help "
"for, '?' for a k"
};

static char test_particle2[512] =  {
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
"ABCDEFGHIJKLMNOP"
};

#define NUM_TEST_PARTICLES 1024

static particletype* temp_mp[NUM_TEST_PARTICLES];

/*****************************************************************************
 * epad_show_stats_now - for diagnostics only 
 *****************************************************************************/
void epad_show_stats_now (ushort bay)
{
    ulong nsec, bitrate, bitrate2;
    hwidbtype *idb;
    epad_instance_t *ds;

	idb = epad_gidb[0];
	ds = INSTANCE(idb);
	nsec = ELAPSED_TIME(ds->last_clear);

	ttyprintf(CONTTY,"%s\n",idb->hw_namestring);
	ttyprintf(CONTTY,"    ds:0x%08x idb:0x%08x\n", ds, idb);
	

	ttyprintf(CONTTY,"    %-10lu paks in        -->   %-10lu paks out\n", 
		ds->num_snt, ds->num_rcv);

	ttyprintf(CONTTY,"    %-10lu bytes in       -->   %-10lu bytes out\n", 
		ds->bytes_snt, ds->bytes_rcv);

	if (nsec >= 1000) {
	    bitrate = (ds->num_snt / (nsec/1000));
	    bitrate2 = (ds->num_rcv / (nsec/1000));
	    ttyprintf(CONTTY,"    %-10lu paks/sec in    -->   %-10lu paks/sec out\n", 
		bitrate, bitrate2);

	    bitrate = (ds->bytes_snt / (nsec/8));
	    bitrate2 = (ds->bytes_rcv / (nsec/8));
	    ttyprintf(CONTTY,"    %-10lu Kbits/sec in   -->   %-10lu Kbits/sec out\n", 
		bitrate, bitrate2);
	}

	ttyprintf(CONTTY,"    %lu seconds since last clear\n", nsec/1000);

}

/*****************************************************************************
 * epa_diags_en_callback
 *****************************************************************************/
void epad_diags_en_callback(ushort type, paktype *pak)
{

    particletype *mp;

    /*
     * Free new particles
     */
    while ((mp = dequeue_inline(&pak->particlequeue)))
	retparticle(mp);
    /*
    epad_fastsend(idb, pak);
     */
}

/*****************************************************************************
 * epad_diags_de_callback -
 *****************************************************************************/
void epad_diags_de_callback(hwidbtype *idb, paktype *pak)
{
    particletype *mp;

    /*
     * Free new particles
     */
    while ((mp = dequeue_inline(&pak->particlequeue)))
	retparticle(mp);
}

extern particletype *dma_getparticle(void);
extern epad_clearcounters(uint);
void init_ses_cntl_blk(void)
{
    encrypt_cntl_blk_t *blk = (encrypt_cntl_blk_t *)ses_cntl_blk;
    blk->call_id = ENCRYPT;
    blk->status = 0;
    if (giv) {
        bcopy(giv, blk->iv_gen, 8);
    } else {
        bzero(blk->iv_gen, 8);
    }
    blk->session = gsession;
    blk->offset = 0;
}
/*****************************************************************************
 * epad_diags_loop -
 *****************************************************************************/
void epad_diags_loop (uint bay, uint num_packets, uint size_paks, uint delay, 
	uint test_part)
{
    leveltype level;
    ulong n, i, idle, work;
    epad_instance_t *ds;
    hwidbtype *idb;
    static paktype *mypak;
    particletype *mp;
    union {
        ulong w;
        ushort ind[2];
    } cause;
    particletype *hdr;
    encrypt_cntl_blk_t *blk;

    /*
     * Sanity check
     */
    if (!epad_gidb[0]) {
        ttyprintf(CONTTY,"Error: IDB not found: Driver not initialized\n");
        return;
    }

    /*
     * Init variables
     */
    idb = epad_gidb[0];
    ds = INSTANCE(idb);
    idle = 0;
    bzero(temp_mp,NUM_TEST_PARTICLES*4);

    /*
     * Init static pak
     */
    if (!mypak) {
	mypak = pas_allocate_fspak();
	if (!mypak) {
	    ttyprintf(CONTTY,"FATAL:Can't init my static pak\n");
	    return;
	}
    }

    /*
     * Open new contexts 
     */
    if (!test_setup(2)) {
        return;
    }
    if (create_session() == FALSE) {
        pas_diag_errmsg(MSG_ERROR, "Failed to create session\n");
        return;
    }
    ds->callback = epad_diags_en_callback;

    init_ses_cntl_blk();
    

    ttyprintf(CONTTY,"\nConnection setup\n");

    /*
     * Initialize the test particle pool
     */
    level = raise_interrupt_level(NETS_DISABLE);
    for (n=0;n<NUM_TEST_PARTICLES;n++) {
#if defined(SVIP) || defined(RVIP)
	temp_mp[n] = dma_getparticle();
#endif
#if defined(PREDATOR)
	temp_mp[n] = pool_getparticle(pas_interface_fallback_pool(compr_idb));
#endif
	if (!temp_mp[n]) {
	    break;
	}
	if (test_part)
	    bcopy(test_particle1, temp_mp[n]->data_start, 
		  sizeof(test_particle1));
	else
	    bcopy(test_particle2, temp_mp[n]->data_start, 
		  sizeof(test_particle2));

    }
    ttyprintf(CONTTY,"%d particles initialized\n",n);
    while (n--) {
	retparticle(temp_mp[n]);
    }

    /*
     * Start the show
     */
    epad_clearcounters(0);
    for (n=0;n<num_packets;) {
	
	work = 0;
	
	if (idb->output_qcount < 30) {
	    n++;
	    epad_cleanup_particles(mypak);
	    /*
             * Attach session control block to the packet
	     */
            hdr = epad_get_particle_inline(idb, TRUE);
            if (!hdr) {
                ttyprintf(CONTTY, "No more esa session block\n");
                n = num_packets;
                break;
            }
	    if (size_paks == 0) {
#if defined(SVIP) || defined(RVIP)
		mp = dma_getparticle();
#endif
#if defined(PREDATOR)
		mp = pool_getparticle(pas_interface_fallback_pool(idb));
#endif
		if (!mp) {
		    ttyprintf(CONTTY,"No more dma particles\n");
		    n = num_packets;
		    break;
		}
		mp->data_bytes = 64;
		particle_enqueue_inline(mypak, mp);
	    } else if (size_paks == 1) {
		for (i=0;i<3;i++) {
#if defined(SVIP) || defined(RVIP)
		    mp = dma_getparticle();
#endif
#if defined(PREDATOR)
		    mp = pool_getparticle(pas_interface_fallback_pool(idb));
#endif
                    if (!mp) {
                        ttyprintf(CONTTY,"No more dma particles\n");
                        n = num_packets;
                        break;
                    }
		    mp->data_bytes = 512;
		    particle_enqueue_inline(mypak, mp);
		}
	    } else {
                for (i=0;i<8;i++) {
#if defined(SVIP) || defined(RVIP)
                    mp = dma_getparticle();
#endif
#if defined(PREDATOR)
                    mp = pool_getparticle(pas_interface_fallback_pool(idb));
#endif
		    if (!mp) {
                        mp = epad_get_particle_inline(idb, FALSE);
		        if (!mp) {
			    ttyprintf(CONTTY,"No more dma particles\n");
                            ttyprintf(CONTTY,"No more epa diag particles\n");
			    n = num_packets;
			    break;
                        }
		    }
                    mp->data_bytes = 512;
                    particle_enqueue_inline(mypak, mp);
                }
            }
            mypak->unspecified[0] = (ulong)hdr;
            mypak->cryptoflags = CRYPTO_ENCRYPT;
            blk = (encrypt_cntl_blk_t *)ses_cntl_blk;
	    blk->total_length = mypak->datagramsize;
	    bcopy(&ses_cntl_blk[0], hdr->data_start, sizeof(encrypt_cntl_blk_t));
	    epad_fastsend(idb, mypak);
	    work++;
	}

	/*
	 * Check compr ring
	 */
        if (ds->op_regs->mbef & IMB3_FULL) {
            cause.w = ds->op_regs->imb3;
 
            if (ds->head != cause.ind[1]) {
                epad_interrupt(idb, cause.ind[1]);
	        work++;
            }
        }

	if (!work) {
	    idle++;
	}

	usecdelay(delay);

    }
    epad_show_stats_now(0);
    ttyprintf(CONTTY,"idle loops = %lu\n", idle); 
    reset_interrupt_level(level);

    /*
     * Clean up after test
     */
    destroy_session();
    test_cleanup(2);
    ds->callback = crypto_engine_callback;

    ttyprintf(CONTTY,"Closed session\n");
}

/*****************************************************************************
 * epad_diags_cmd -
 *****************************************************************************/
void epad_diags_cmd (parseinfo *csb)
{
    uint bay = GETOBJ(int,1);
    uint num_packets = GETOBJ(int,2);
    uint size_paks = GETOBJ(int,3);
    uint delay = GETOBJ(int,4);
    uint test_part = GETOBJ(int,6);
 
    verbose = GETOBJ(int, 5);
 
    epad_diags_loop(bay, num_packets, size_paks, delay, test_part);
}

