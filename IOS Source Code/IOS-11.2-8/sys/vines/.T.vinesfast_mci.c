
vinesfast_mci.c,2752
static void v_mci2mci_clear_pad 168,5374
static void v_samemci_read_pad 185,5728
static void v_samemci_write_pad 196,6007
#define V_M2M_START 219,6632
static inline void v_m2m_finish 281,9258
static void v_m2m_smds_finish 304,9944
static boolean vines_mci2mci_ether_snap 343,11155
static boolean vines_mci2mci_ether_arpa 363,11727
static boolean vines_mci2mci_fddi 382,12239
static boolean vines_mci2mci_token_snap 400,12726
static boolean vines_mci2mci_token_vines 417,13210
static boolean vines_mci2mci_hdlc 434,13651
static boolean vines_mci2mci_ppp 451,14082
static boolean vines_mci2mci_frame 468,14530
static boolean vines_mci2mci_frame_ietf 487,15078
static boolean vines_mci2mci_smds_cisco 511,15893
static boolean vines_mci2mci_smds_dxi 537,16675
static boolean vines_mci2mci_atm_dxi 557,17195
enum priority_options priority_options583,17884
enum priority_options {FAST_SWITCH,583,17884
enum priority_options {FAST_SWITCH, HALF_FAST_SWITCH,583,17884
enum priority_options {FAST_SWITCH, HALF_FAST_SWITCH, DROP}583,17884
static inline enum priority_options v_same_start 584,17945
static inline void v_same_finish 616,18910
static boolean vines_samemci_atm 635,19485
static boolean vines_samemci_ether_snap 672,20517
static boolean vines_samemci_ether_arpa 705,21517
static boolean vines_samemci_fddi 739,22474
static boolean vines_samemci_token_snap 773,23426
static boolean vines_samemci_token_vines 805,24354
static boolean vines_samemci_hdlc 837,25240
static boolean vines_samemci_ppp 869,26111
static boolean vines_samemci_frame 901,26987
static boolean vines_samemci_frame_ietf 935,27963
static boolean vines_samemci_smds_cisco 974,29206
static boolean vines_samemci_smds_dxi 1017,30580
static boolean vines_samemci_atm_dxi 1054,31704
boolean vines_access_check_mci 1097,32949
static inline boolean vines_hes_fastswitch 1150,34626
static boolean ether_snap_vines_fastswitch 1241,37504
boolean ether_vines_fastswitch 1263,38178
boolean fddi_vines_fastswitch 1295,39222
boolean token_snap_vines_fastswitch 1320,40017
boolean token_vines_fastswitch 1345,40818
boolean hdlc_vines_fastswitch 1372,41650
static boolean frame_vines_fastswitch 1397,42411
static boolean frame_ietf_vines_fastswitch 1415,42840
static boolean atm_dxi_vines_fastswitch 1432,43270
boolean atm_vines_fastswitch 1453,43840
static boolean smds_vines_fastswitch 1498,45211
void vines_clear_fs_tables 1524,45927
void vines_setup_fs_tables 1539,46320
static boolean vines_fast_supported 1592,48334
static void vines_mci_fs_register_drivers 1663,50080
static void vines_mci_fs_init 1682,50462
#define VINESMCIFS_MAJVERSION 1721,52215
#define VINESMCIFS_MINVERSION 1722,52247
#define VINESMCIFS_EDITVERSION 1723,52279
