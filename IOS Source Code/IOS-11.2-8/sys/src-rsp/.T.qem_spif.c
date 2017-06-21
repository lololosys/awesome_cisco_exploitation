
qem_spif.c,2242
#define	CLRQAINTR	79,2262
#define	GETSPVERSION	80,2287
#define	WRITEMEMDLONG	81,2344
#define	READMEMDLONG	82,2373
#define	SETCOPYADDR	83,2401
#define	WRITEDIAG	84,2428
#define	READDIAG	85,2453
#define	WRITEACC	86,2477
#define	READACC	87,2501
#define	WRITEQ	88,2525
#define	READQ	89,2548
#define	HDRLWORDS	91,2571
#define	MAXRXOFFSET	92,2592
#define QA_INTLEVEL	94,2618
#define	boffset2mdptr(boffset2mdptr100,2701
#define	mdptr2boffset(mdptr2boffset101,2772
#define	boffset2qaptr(boffset2qaptr103,2845
#define	qaptr2boffset(qaptr2boffset104,2917
#define	qaptr2soffset(qaptr2soffset105,2990
#define	boffset2bufnum(boffset2bufnum107,3039
#define	bufptr2bufnum(bufptr2bufnum108,3076
#define	QNUM(QNUM110,3139
#define	ANUM(ANUM111,3192
static int qa_error 128,3583
int	is_diag_off 138,3765
int	is_memd_off 145,3947
int	is_bufhdr 153,4147
static int	chk_qnum 162,4362
static int	chk_qptr 171,4566
static int	chk_anum 180,4778
static int	chk_aptr(189,4992
static int	chk_diag_off 198,5213
static int	chk_memd_off 208,5459
static int	chk_hdroff 217,5682
static int	chk_hdr 226,5903
static void read_hw_memd 243,6278
static void read_memd 261,6711
static void write_hw_memd 279,7221
void write_memd 297,7656
static ulong	read_qval_and_hdr 312,8063
static bufhdr_t *read_q_and_buf 339,8648
static void     write_q_and_hdr 362,9107
static void      write_q_and_buf 380,9538
static bufhdr_t *read_raw_q 391,9810
static bufhdr_t *read_q_and_hdr 425,10608
static void     write_tx_q 443,10907
static bufhdr_t *hwq_read 464,11408
void	emulator_init 493,12140
int	init_bufhdr 502,12354
void	hwq_init 516,12692
int emulate_queue_type 523,12804
qa_hwq_t  *queue_ptr 539,13073
qa_acc_t  *acc_ptr 549,13275
ulong    read_memd_long 560,13527
void      write_memd_long 568,13720
ulong peek_hw_memd 580,14020
void poke_hw_memd 602,14485
bufhdr_t *hwq_dequeue 620,14875
void      hwq_enqueue 630,15056
bufhdr_t *hwq_peek 652,15479
short   hwacc_read_dec 663,15679
short	hwacc_peek 681,16060
void	hwacc_sub 699,16446
void	hwacc_set 715,16800
ulong read_qa_diag 731,17166
void write_qa_diag 764,17898
ulong get_sp_version 821,19202
void clr_qa_intr 835,19510
ushort	read_ccb_done 842,19643
