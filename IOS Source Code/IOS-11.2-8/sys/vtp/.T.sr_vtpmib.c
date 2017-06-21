
sr_vtpmib.c,2561
#define LNvtpNotifications 243,9864
vtpStatus_get 266,10568
vtpStatus_free 351,12554
vtpStatus_cleanup(362,12843
vtpStatus_test(382,13576
vtpStatus_set 487,16630
managementDomainEntry_get 505,17227
managementDomainEntry_free 720,22911
managementDomainEntry_cleanup 734,23435
Clone_managementDomainEntry 746,23858
managementDomainEntry_test 783,25200
managementDomainEntry_set 1061,33236
vtpVlanEntry_get 1080,33870
vtpVlanEditEntry_get 1244,37929
vtpVlanEditEntry_free 1412,42204
vtpVlanEditEntry_cleanup 1425,42601
Clone_vtpVlanEditEntry 1437,42999
vtpVlanEditEntry_test 1466,44047
vtpVlanEditEntry_set 1781,53027
vlanTrunkPorts_get 1798,53637
vlanTrunkPorts_free 1870,55393
vlanTrunkPorts_cleanup 1881,55697
vlanTrunkPorts_test 1901,56446
vlanTrunkPorts_set 2004,59613
vlanTrunkPortEntry_get 2021,60217
vlanTrunkPortEntry_free 2170,64186
vlanTrunkPortEntry_cleanup 2185,64719
Clone_vlanTrunkPortEntry 2197,65127
vlanTrunkPortEntry_test 2230,66459
vlanTrunkPortEntry_set 2473,73634
k_vtpStatus_get 2488,73923
k_vtpStatus_test 2506,74368
k_vtpStatus_ready 2513,74506
k_vtpStatus_set 2521,74662
vtpStatus_undo 2536,75006
unsigned char k_get_vtp_domain 2557,75830
k_managementDomainEntry_get 2582,76540
k_managementDomainEntry_test 2665,79639
k_managementDomainEntry_ready 2728,81407
k_managementDomainEntry_set_defaults 2775,82756
k_managementDomainEntry_set 2791,83137
managementDomainEntry_undo 2898,87235
unsigned char k_get_vtp_vlan 2905,87432
k_vtpVlanEntry_get 2918,87796
k_vtpVlanEditEntry_get 2996,90429
k_vtpVlanEditEntry_test 3079,93255
k_vtpVlanEditEntry_ready 3120,94407
k_vtpVlanEditEntry_set_defaults 3142,94980
k_vtpVlanEditEntry_set 3165,95767
vtpVlanEditEntry_undo 3191,96510
k_vlanTrunkPorts_get 3199,96718
k_vlanTrunkPorts_test(3209,96972
k_vlanTrunkPorts_ready 3219,97196
k_vlanTrunkPorts_set 3227,97355
vlanTrunkPorts_undo(3246,97763
k_vlanTrunkPortEntry_get 3257,98015
k_vlanTrunkPortEntry_test 3362,101603
k_vlanTrunkPortEntry_ready 3401,102881
k_vlanTrunkPortEntry_set_defaults 3443,104171
k_vlanTrunkPortEntry_set 3449,104251
vlanTrunkPortEntry_undo 3500,106005
void vtp_revnum_error_detected 3508,106214
void vtp_digest_error_detected(3580,108155
void vtp_mtu_too_big 3646,109761
void vtp_storage_exceeded 3725,111453
void vtp_snmp_host_nvgen 3826,114145
void vtp_trap_cfg_set 3830,114232
void vtp_trap_cfg_nvwrt 3837,114461
init_vtpmib 3844,114632
#define MAJVERSION_vtpmib 3868,115362
#define MINVERSION_vtpmib 3869,115390
#define EDITVERSION_vtpmib 3870,115418
