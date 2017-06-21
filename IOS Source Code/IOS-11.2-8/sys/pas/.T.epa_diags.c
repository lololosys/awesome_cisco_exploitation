
epa_diags.c,7823
#define ALTERNATE 65,1571
#define START_SEQUENCE 82,1969
#define PUBLIC_PARTICLE_BYTES 118,3350
#define PUBLIC_PARTICLE_BYTES 121,3428
static inline void put_check_byte 132,3871
static inline boolean verify_check_byte 144,4234
void process_recv_data 166,4941
void service_bitflip_traffic 204,6281
void test_callback 229,6998
particletype *epad_create_encryption_blk 252,7630
void gen_data_pattern 293,8781
paktype *epad_getpak 322,9554
paktype *epad_genpack 355,10483
static inline ushort rand16 436,12908
void get_rnd_param 458,13613
boolean test_setup 493,14841
static void test_cleanup(576,17430
void hexdump 584,17584
static inline void send_to_epad(616,18470
boolean epad_throttle 632,19118
static void process_encrypt_data 667,20192
static void process_decrypt_data 714,21662
void service_decrypt_traffic 753,23058
void service_encrypt_throughput 791,24197
void service_encrypt_traffic 817,25131
static void encryption_test_summary 872,26823
static void wait_test_done 886,27427
static char ktest_status 897,27817
static void dump_buffer 911,28219
void dump_key_cmd 933,28847
paktype* create_key_cmd 954,29519
static void init_ktest_result 1259,37192
static void print_pass 1410,45142
static void print_fail 1425,45684
static void verify_ktest_result 1439,46425
static void service_global_alloc 1786,56509
static void alloc_global_handle(1865,58690
static void service_global_free 1917,60597
static void free_global_handle(2000,62820
static void service_ktest_epaSetStatus 2047,64369
static void service_ktest_epaSetPassword 2056,64752
static void set_passwd 2065,65126
static void epaSetStatus 2088,65900
static void clear_tamper 2111,66699
static void clear_xtract 2122,67094
static void set_priviledge 2132,67502
static void service_ktest_epaZeroize 2142,67937
static void service_ktest_setRNGseed(2151,68303
static void set_randomseed 2160,68646
static void service_ktest_epaGetStatus 2184,69494
static void service_ktest_epaGetTime 2193,69880
static void service_ktest_epaGetMethods 2203,70290
void service_ktest_hMemAlloc(2212,70675
static void hMemAlloc 2221,71008
void service_ktest_hMemFree(2242,71742
static void hMemFree 2251,72068
static void service_ktest_hMemWrite 2271,72760
static void hMemWrite 2280,73097
static void service_ktest_hMemRead 2301,73855
static void hMemRead 2310,74188
static void service_ktest_hMemCopy 2331,74922
static void hMemCopy 2340,75256
static void service_ktest_hMemSyndrome 2365,76064
static void hMemSyndrome 2374,76405
static void service_ktest_hReadRandom 2399,77243
static void hReadRandom 2409,77670
static void service_ktest_hGenRandom 2429,78384
static void hGenRandom 2438,78821
static void service_ktest_gendhpair 2459,79574
static void gendhpair(2469,80013
static void service_ktest_getdhshared 2491,80755
static void getdhshared 2501,81203
static void service_ktest_hSetCipherkey 2524,82031
static void setCipherkey 2534,82464
static void service_ktest_hDHoneTimePad 2559,83346
static void hDHoneTimePad 2569,83779
static void service_ktest_hGetDHkey 2590,84555
static void getDHkey 2599,84910
static void service_ktest_freesession 2627,85847
static void freeSession 2636,86195
static void service_ktest_newsession 2656,86914
static void newSession 2665,87269
static void service_ktest_hSHA 2688,88042
static void hSHA 2697,88379
static void service_ktest_hSHAinit 2717,89070
static void service_ktest_hSHAupdate 2726,89447
static void service_ktest_hSHAfinal 2735,89833
static void service_ktest_hGenDSSparameters 2745,90241
static void hGenDSSparameters 2754,90623
static void service_ktest_hGetDSSpqg 2775,91396
static void hGetDSSpqg 2784,91771
static void service_ktest_hGenDSSnumber 2814,92621
static void hGenDSSnumber 2823,93009
static void service_ktest_hGenDSSkey 2843,93731
static void hGenDSSkey 2852,94084
static void service_ktest_hGenDSSsignature 2873,94841
static void hGenDSSsignature 2882,95216
static void service_ktest_hVerDSSsignature 2905,96064
static void hverDSSsignature 2913,96383
static void ktest_epaZeroize 2949,97425
static void service_ktest_epaReadFlash 2971,98213
static void service_ktest_epaWriteFlash 2980,98596
void epad_sendinit_cmd 2989,98951
void epad_download_cmd 3010,99571
static void epad_mailbox_test 3039,100500
static void epad_interrupt_test 3113,102876
static void epad_buffer_transfer_test 3147,104052
void epad_bringup_test_cmd 3241,106466
static void dump_pci_opregs(3274,107495
static void dump_pci_config 3301,109082
static void show_amcc_menu(3334,110240
void epad_test_amcc_cmd(3359,111081
static void epad_process_rings 3484,114732
void epad_process_rings_cmd 3533,116341
boolean create_session 3543,116521
void destroy_session 3610,118417
void epad_traffic_cmd 3636,119288
void epad_key_wf_cmd 3800,124155
void epa_memRead_cmd 3841,125297
void epa_memAlloc_cmd 3870,126152
void epa_memFree_cmd 3897,126973
void epa_genRandom_cmd 3923,127762
void epa_genDSSkey_cmd 3950,128619
void epa_genDSSsig_cmd(3978,129478
void epa_genDHpair_cmd(4006,130395
void epa_getDHshared_cmd 4036,131341
void epa_setCipherkey_cmd 4065,132244
void epa_newSession_cmd 4095,133177
void epa_freeSession_cmd 4124,134076
void epa_setPasswd_cmd 4151,134873
void epa_sha_cmd 4180,135775
void epa_verDSSsig_cmd 4210,136580
void epa_setStatus_cmd 4242,137636
void epad_key_zeroize_cmd 4288,138868
void epad_key_rf_cmd 4307,139469
void epad_statustime_cmd 4337,140400
void epad_show_statistic_cmd 4371,141413
void epad_led_cmd 4390,142238
#define LRG_FRAME_SIZE 4412,142740
void epad_frame_cmd(4422,143191
static void ktest_hMemAlloc 4546,147014
static void ktest_hMemFree 4638,149885
static void ktest_hMemWrite 4679,151424
static void ktest_hMemRead 4724,153099
static void ktest_hMemCopy 4751,154023
static void ktest_hMemSyndrome 4792,155596
static void ktest_hReadRandom 4836,157373
static void ktest_hGenRandom 4871,158711
static void ktest_gendhpair 4921,160375
static void ktest_getdhshared 4954,161527
static void ktest_hSetCipherkey 4986,162728
static void ktest_hDHoneTimePad 5018,163978
static void ktest_hGetDHkey 5055,165398
static void ktest_freesession 5117,167833
static void ktest_newsession 5146,168900
static void ktest_hSHA 5179,170066
static void ktest_hSHAinit 5205,170944
static void ktest_hSHAupdate 5234,171968
static void ktest_hSHAfinal 5264,173042
static void ktest_hGenDSSparameters 5296,174255
static void ktest_hGetDSSpqg 5331,175468
static void ktest_hGenDSSnumber 5366,176694
static void ktest_hGenDSSkey 5393,177645
static void ktest_hGenDSSsignature 5426,178800
static void ktest_hVerDSSsignature 5454,179937
static void ktest_epaGetStatus 5497,181614
static void ktest_epaSetStatus 5518,182299
static void ktest_epaGetTime 5608,186438
static void ktest_epaGetMethods 5643,187714
void epad_key_hma_cmd 5670,188682
void epad_key_rnd_cmd 5706,189624
void epad_key_dss_cmd 5733,190460
void epad_key_dh_cmd 5772,191524
void epad_key_session_cmd 5807,192460
void epad_key_status_cmd 5833,193242
void epad_key_all_cmd 5856,193954
void epad_data_session_cmd 5901,195282
int epa_frame_diag 6080,200422
int epa_key_diag 6090,200729
int epa_key_frame_diag 6100,201040
void display_cyl_diag_err 6110,201300
void display_cyl_diag_msg 6160,202754
void cyl_epa_diag 6197,203834
int epa_diag4 6218,204468
int epa_diag5 6233,204823
int epa_diag6 6248,205178
int epa_diag7 6263,205538
int epa_diag8 6278,205899
int epa_diag9 6293,206260
int epa_diag10 6308,206619
#define NUM_TEST_PARTICLES 6393,208283
void epad_show_stats_now 6400,208573
void epad_diags_en_callback(6439,209754
void epad_diags_de_callback(6457,210183
void init_ses_cntl_blk(6470,210460
void epad_diags_loop 6486,210939
void epad_diags_cmd 6689,215865
