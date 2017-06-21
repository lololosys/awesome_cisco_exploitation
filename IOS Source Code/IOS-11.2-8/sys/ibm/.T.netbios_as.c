
netbios_as.c,1276
#define NBF_MAJVERSION 160,5217
#define NBF_MINVERSION 161,5242
#define NBF_EDITVERSION 162,5267
static boolean nbf_on_idb 223,8241
static void netbios_as_init 237,8567
static void nbf_register 282,9778
static void nbf_enqueue 291,9917
static process nbf_input 411,14568
static void nbf_process_input 445,15283
static netbios_nbfcp_name_proj_entry *nbf_name_proj_get_next 643,21907
static netbios_nbfcp_name_proj_entry *nbf_name_proj_search 655,22169
static void nbf_housekeeping 674,22633
static void nbf_flush_queue 690,23089
static void nbf_forward_pakout 717,23726
static void nbf_pakout 777,25099
static boolean nbf_netbios_processing 841,27825
static boolean nbf_nbfcp_processing 1363,46841
static boolean nbf_prep_interface 1511,51930
static boolean nbf_check_and_enqueue 1577,54110
static paktype *nbf_check_and_dequeue 1800,62313
static void nbf_llc2_open(1947,67389
static void nbf_llc2_link_state_change 1999,68661
static void nbf_cleanup_llc 2049,69999
static void netbios_as_if_state_change 2087,71125
static void nbf_action_pending_session_init 2107,71773
void netbios_as_if_command 2158,73329
void show_netbios_nbf 2225,75103
static paktype *nbf_create_netbios_add_name_query 2257,76069
void nbf_xmit_netbios_session_end 2308,77566
