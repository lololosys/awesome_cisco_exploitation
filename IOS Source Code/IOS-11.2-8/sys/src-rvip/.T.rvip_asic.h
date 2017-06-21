
rvip_asic.h,6854
#define __RVIP_ASIC_H__43,1487
#define PAD 52,1625
typedef struct nev_asic_{nev_asic_54,1652
} nev_asic;nev_asic92,3311
#define IO_ASIC 99,3375
#define IO_ASIC_KSEG1 100,3429
#define IO_REG_ADDRESS(IO_REG_ADDRESS104,3542
#define IO_CT0_REG	109,3659
#define IO_WATCHDOG_REG	110,3713
#define IO_CT1_REG	111,3771
#define IO_CT2_REG	112,3824
#define IO_CCR_REG	113,3877
#define IO_IMASK_REG	114,3930
#define IO_IVECT_REG	115,3989
#define IO_IEOI_REG	116,4048
#define IO_IVMI_REG	117,4105
#define IO_CFGA_REG	118,4162
#define IO_CFGB_REG	119,4217
#define IO_STATA_REG 120,4279
#define IO_STATB_REG	121,4340
#define IO_CSA_REG 122,4397
#define IO_CSB_REG	123,4455
#define IO_CSC_REG	124,4515
#define IO_COOKIE_REG	125,4575
#define IO_STATIC_IN_REG	126,4633
#define IO_ASIC_REG	127,4696
#define IO_CTI_REG	128,4754
#define CCR_CT2_EN 135,4898
#define CCR_CT1_EN 136,4970
#define CCR_CT0_EN 137,5042
#define IMR_MASK_ALL 146,5402
#define IMR_MASK_NONE 147,5459
#define INT_LEV0 148,5518
#define INT_LEV1 149,5580
#define INT_LEV2 150,5642
#define INT_LEV3 151,5704
#define INT_LEV4 152,5766
#define INT_LEV5 153,5828
#define INT_LEV6 154,5890
#define INT_LEV7 155,5952
#define INT_LEV(INT_LEV156,6014
#define IVR_NO_INT 163,6242
#define IVR_LEV_0 164,6300
#define IVR_LEV_1 165,6363
#define IVR_LEV_2 166,6426
#define IVR_LEV_3 167,6489
#define IVR_LEV_4 168,6552
#define IVR_LEV_5 169,6615
#define IVR_LEV_6 170,6678
#define IVR_LEV_7 171,6741
#define NEV_PA_STALL_INT 177,6909
#define NEV_PA_STALL_VEC 178,6974
#define NEV_TIMER_INT 179,7046
#define NEV_TIMER_VEC 180,7099
#define NEV_DRAM_ERR_INT 181,7159
#define NEV_DRAM_ERR_VEC 182,7214
#define NEV_IO_ERR_INT 183,7275
#define NEV_IO_ERR_VEC 184,7325
#define NEV_PMAC_ERR_INT 185,7381
#define NEV_PMAC_ERR_VEC 186,7440
#define NEV_PA_NMI_INT 187,7506
#define NEV_PA_NMI_VEC 188,7551
#define NEV_INT_MASK 189,7602
#define CFGA_FLEX_DCLK1 195,7765
#define CFGA_FLEX_DCLK0 196,7839
#define CFGA_FLEX_PMACPM_DCLK 197,7913
#define CFGA_FLEX_PMACIO_DCLK 198,7975
#define CFGA_EEPROM_CLOCK_PA0 199,8037
#define CFGA_EEPROM_CLOCK_PA1 200,8106
#define CFGA_SW_HW_INT 201,8175
#define CFGA_EEPROM_DATA_PA0 202,8251
#define CFGA_EEPROM_SELECT_PA0 203,8309
#define CFGA_EEPROM_DATA_PA1 204,8378
#define CFGA_EEPROM_SELECT_PA1 205,8436
#define CFGB_RESET_PA0 212,8600
#define CFGB_RESET_PA1 213,8667
#define CFGB_POWERON_PA1 214,8734
#define CFGB_POWERON_PA0 215,8791
#define CFGB_RESET_FLEX 216,8848
#define CFGB_ENBL_LED_PA1 217,8909
#define CFGB_ENBL_LED_PA0 218,8968
#define CFGB_PROM_MASK 219,9027
#define CFGB_DRAM_CFG_MASK 220,9085
#define CFGB_DRAM_VAL_MASK_128 221,9148
#define CFGB_DRAM_VAL_MASK_32 222,9228
#define CFGB_DRAM_VAL_MASK_8 223,9307
#define CFGB_DRAM_VAL_MASK_2 224,9385
#define CFGB_BOOT 225,9463
#define STATA_PRSNT_PA0 231,9631
#define STATA_PRSNT_PA1 232,9690
#define STATA_STALL_PA0 233,9749
#define STATA_STALL_PA1 234,9812
#define STATB_EEPROM_WR_EN 240,9968
#define STATB_BOARD_ID_MASK 241,10032
#define STATB_IO_WR_BERR 242,10097
#define CSA_FLX_PCI0_DONE 249,10315
#define CSA_FLX_PCI1_DONE 250,10358
#define CSA_FLX_PMAC_PM_DONE 251,10401
#define CSA_FLX_PMAC_IO_DONE 252,10457
#define CSA_FLX_CH_DONE 253,10513
#define CSA_FLX_CB_DATA_DONE 254,10569
#define CSA_FLX_CB_CTRL_DONE 255,10627
#define CSA_FLX_PCI0_DI 256,10688
#define CSA_FLX_PCI1_DI 257,10749
#define CSA_FLX_PMAC_PM_STAT 258,10810
#define CSA_FLX_PMAC_IO_STAT 259,10879
#define CSA_FLX_CH_STAT 260,10948
#define CSA_FLX_CB_DATA_STAT 261,11017
#define CSA_FLX_CB_CTRL_STAT 262,11088
#define CSB_CLR_DRAM_WR_BERR 269,11316
#define CSB_CLR_IO_WR_BERR 270,11388
#define CSC_FLEX_CONFIG 277,11623
#define CSC_FLX_PCI0_D0 278,11691
#define CSC_FLX_PCI1_D0 279,11750
#define CSC_FLX_PMAC_PM_D0 280,11809
#define CSC_FLX_PMAC_IO_D0 281,11862
#define CSC_FLX_CH_D0 282,11915
#define CSC_FLX_CB_DATA_D0 283,11968
#define CSC_FLX_CB_CNTRL_D0 284,12023
#define X2444_CE 292,12265
#define X2444_SK 293,12336
#define X2444_DI 294,12394
#define X2444_DO 295,12452
#define ASIC_REV_MASK 301,12598
#define ASIC_CONT_C 302,12662
#define ASIC_CONT_B 303,12740
#define ASIC_CONT_A 304,12818
typedef struct pmac_config_ pmac_config_308,12917
} pmac_config_t;pmac_config_t315,13133
typedef struct pmac_intrpt_mask_ pmac_intrpt_mask_317,13151
} pmac_intrpt_mask_t;pmac_intrpt_mask_t324,13343
typedef struct pmac_intrpt_status_ pmac_intrpt_status_326,13366
} pmac_intrpt_status_t;pmac_intrpt_status_t333,13589
typedef struct pci_asic_ pci_asic_338,13677
} pci_asic_t;pci_asic_t342,13845
#define PMAC_ERROR_MASK 345,13919
#define PMAC_ERROR_SHFT 346,13957
#define PMAC_PA0_NMI_MASK 348,13996
#define PMAC_PA0_NMI_SHFT 349,14034
#define PMAC_PA0_PKT_MASK 350,14073
#define PMAC_PA0_PKT_SHFT 351,14111
#define PMAC_PA1_NMI_MASK 352,14149
#define PMAC_PA1_NMI_SHFT 353,14187
#define PMAC_PA1_PKT_MASK 354,14226
#define PMAC_PA1_PKT_SHFT 355,14264
#define NMI_PER_PA 356,14302
#define PMAC_IO_PARITY_ERROR 359,14404
#define PMAC_PM_FIFO_ERROR 360,14464
#define PMAC_PM_CTRLR_ERROR 361,14532
#define PMAC_PCI_BUS_TIMEOUT 362,14606
#define PMAC_PCI_PARITY_ERROR 363,14673
#define PMAC_PCI_SYSTEM_ERROR 364,14741
#define PCI_ASIC 369,14854
typedef struct dma_reg0_ dma_reg0_371,14908
}dma_reg0_t;dma_reg0_t377,15050
typedef struct dma_reg1_ dma_reg1_379,15064
}dma_reg1_t;dma_reg1_t382,15140
typedef struct dma_reg2_ dma_reg2_384,15154
}dma_reg2_t;dma_reg2_t387,15231
typedef struct dma_reg3_ dma_reg3_389,15245
}dma_reg3_t;dma_reg3_t399,15570
typedef struct dma_reg4_ dma_reg4_401,15584
}dma_reg4_t;dma_reg4_t409,15844
typedef struct dma_reg5_ dma_reg5_411,15858
}dma_reg5_t;dma_reg5_t418,16084
typedef struct dma_reg6_ dma_reg6_420,16098
}dma_reg6_t;dma_reg6_t422,16159
typedef struct dma_reg7_ dma_reg7_424,16173
}dma_reg7_t;dma_reg7_t428,16301
typedef struct dma_regs_ dma_regs_431,16316
} dma_regs_t;dma_regs_t464,17193
#define dma_reg0 466,17208
#define dma_int0 467,17233
#define dma_reg1 468,17258
#define dma_int1 469,17283
#define dma_length 470,17308
#define dma_reg2 471,17342
#define dma_int2 472,17367
#define dma_reg3 473,17392
#define dma_int3 474,17417
#define dma_reg4 475,17442
#define dma_int4 476,17467
#define dma_reg5 477,17492
#define dma_int5 478,17517
#define dma_reg6 479,17542
#define dma_int6 480,17567
#define dma_reg7 481,17592
#define dma_int7 482,17617
#define DMA_ASIC 484,17643
#define DMA_REG_ADDRESS(DMA_REG_ADDRESS488,17724
#define DMA_MEMD_REG 493,17844
#define DMA_PCI_REG 494,17924
#define DMA_LENGTH_REG 495,18004
#define DMA_ERR_REG 496,18084
#define DMA_CPU_ERR_REG 497,18164
#define DMA_MISC_REG 498,18244
#define DMA_LOCK_REQ_REG 499,18324
#define DMA_LOCK_GRNT_REG 500,18404
