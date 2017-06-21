
c7100_pci.c,1432
#define C7100_MB_UP_PRIMARY_LATENCY	183,6169
#define C7100_MB_DOWN_PRIMARY_LATENCY	184,6216
#define C7100_MB_UP_BURST	186,6266
#define C7100_MB_DOWN_BURST	187,6300
#define C7100_MB_UP_BRIDGE_CONTROL	189,6337
#define C7100_MB_DOWN_BRIDGE_CONTROL	190,6383
#define C7100_MB_UP_PRIMARY_TIMER	192,6432
#define C7100_MB_UP_SECONDARY_TIMER	193,6471
#define C7100_MB_DOWN_PRIMARY_TIMER	195,6513
#define C7100_MB_DOWN_SECONDARY_TIMER	196,6554
#define C7100_GENERAL_PRIMARY_LATENCY	198,6598
#define C7100_PA_BURST	203,6684
#define C7100_PA_PRIMARY_TIMER	204,6716
#define C7100_PA_SECONDARY_TIMER	205,6753
#define C7100_PA_BRIDGE_CONTROL	206,6791
typedef struct c7100_bay_cntl_t_ c7100_bay_cntl_t_211,6887
} c7100_bay_cntl_t;c7100_bay_cntl_t238,7798
boolean pas_pa_present 811,26827
void pas_control_power 840,27328
void pas_netio_interrupts 892,28549
void pas_mngmt_interrupts 916,29122
void pas_control_reset 936,29525
pci_bridge_config_t *get_bridge_config 948,29790
boolean pas_bridge_config 961,30039
uchar get_pci_bus_no 1001,30906
void *get_io_base_address 1020,31333
void *get_pci_device_base 1047,32119
ulong get_pci_device_base_offset 1065,32613
void pas_nmi_dispatcher 1082,33082
void pas_pci_controller_summary 1130,34332
void c7100_pci_init 1158,34838
boolean c7100_pci_enable_packet_sram 1209,36304
void gt64010_setup_parallel_bridge 1252,37623
void gt64010_restore_parallel_bridge 1285,38513
