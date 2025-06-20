/*
 * FC_settings.h
 *
 *  Created on: Jun 20, 2022
 */

#ifndef FC_SETTINGS_H_
#define FC_SETTINGS_H_

#include "led_driver.h"

#if TLC6983
    #define _FC0_2 ((MOD_SIZE_2 | GRP_DLY_B_0 | GRP_DLY_G_0 | GRP_DLY_R_0) >> 32) & 0xFFFF
    #define _FC0_1 ((FREQ_MUL__11 | FREQ_MOD__LF | SUBP_NUM__128 | SCAN_NUM__32) >> 16) & 0xFFFF
    #define _FC0_0 ((LODRM_EN_1 | PSP_MOD_0 | PS_EN_0 | PDC_EN_1 | CHIP_NUM__2)) & 0xFFFF
    
    #define _FC1_2 ((BLK_ADJ_17 | LINE_SWT__120 | LG_ENH_B_2 | LG_ENH_G_15) >> 32) & 0xFFFF
    #define _FC1_1 ((LG_ENH_G_15 | LG_ENH_R_1 | LG_STEP_B__6 | LG_STEP_G__6) >> 16) & 0xFFFF
    #define _FC1_0 ((LG_STEP_G__6 | LG_STEP_R__6 | SEG_LENGTH__524)) & 0xFFFF
    
    #define _FC2_2 ((SUBP_MAX_256_0 | CH_B_IMMUNITY_0 | CH_G_IMMUNITY_0 | CH_R_IMMUNITY_0 | LG_COLOR_B__14) >> 32) & 0xFFFF
    #define _FC2_1 ((LG_COLOR_G__14 | LG_COLOR_R__1 | DE_COUPLE1_B__4 | DE_COUPLE1_G__4) >> 16) & 0xFFFF
    #define _FC2_0 ((DE_COUPLE1_R__1 | V_PDC_B__0V1 | V_PDC_G__0V1 | V_PDC_R__0V1)) & 0xFFFF
    
    #define _FC3_2 ((LSDVTH_B__0V2 | LSDVTH_G__0V2 | LSDVTH_R__0V2 | LSD_RM__16 | BC_2) >> 32) & 0xFFFF
    #define _FC3_1 ((CC_B_64 | CC_G_85) >> 16) & 0xFFFF
    #define _FC3_0 ((CC_R_143 | LOD_LSD_RB_1 | LODVTH_B__0V2 | LODVTH_G__0V2 | LODVTH_R__0V2)) & 0xFFFF
    
    #define _FC4_2 ((DE_COUPLE3_EN_0 | DE_COUPLE3__1 | DE_COUPLE2_0 | FIRST_LINE_DIM__1 | LG_CAURSE_B_1 | LG_CAURSE_G_1 | LG_CAURSE_R_1) >> 32) & 0xFFFF
    #define _FC4_1 ((SR_ON_B_0 | SR_ON_G_0 | SR_ON_R_0 | SR_OFF_B_1 | SR_OFF_G_1 | SR_OFF_R_1 | LG_FINE_B_1 | LG_FINE_G_1 | LG_FINE_R_1) >> 16) & 0xFFFF
    #define _FC4_0 ((SCAN_REV_1 | IMAX_0 | LAST_SOUT_0)) & 0xFFFF
#elif TLC6984
    #define _FC0_2 ((LSD_RM_EN_0 | GRP_DLY_B_0 | GRP_DLY_G_0 | GRP_DLY_R_0 | FREQ_MUL__11) >> 32) & 0xFFFF
    #define _FC0_1 ((FREQ_MUL__11 | FREQ_MOD__LF | SUBP_NUM__128 | SCAN_NUM__32) >> 16) & 0xFFFF
    #define _FC0_0 ((LODRM_EN_1 | PSP_MOD_0 | PS_EN_0 | PDC_EN_1 | CHIP_NUM__2)) & 0xFFFF
    
    #define _FC1_2 ((BLK_ADJ_17 | LINE_SWT__120 | LG_ENH_B_2 | LG_ENH_G_15) >> 32) & 0xFFFF
    #define _FC1_1 ((LG_ENH_G_15 | LG_ENH_R_1 | LG_STEP_B__6 | LG_STEP_G__6) >> 16) & 0xFFFF
    #define _FC1_0 ((LG_STEP_G__6 | LG_STEP_R__6 | SEG_LENGTH__524)) & 0xFFFF
    
    #define _FC2_2 ((MPSM_EN_0 | MOD_SIZE__2 | SUBP_MAX_256_0 | CH_B_IMMUNITY_0 | CH_G_IMMUNITY_0 | CH_R_IMMUNITY_0 | LG_COLOR_B__14) >> 32) & 0xFFFF
    #define _FC2_1 ((LG_COLOR_G__14 | LG_COLOR_R__1 | DE_COUPLE1_B__4 | DE_COUPLE1_G__4) >> 16) & 0xFFFF
    #define _FC2_0 ((DE_COUPLE1_R__1 | V_PDC_B__0V1 | V_PDC_G__0V1 | V_PDC_R__0V1)) & 0xFFFF
    
    #define _FC3_2 ((LSDVTH_B__0V2 | LSDVTH_G__0V2 | LSDVTH_R__0V2 | LSD_RM__16 | BC_2) >> 32) & 0xFFFF
    #define _FC3_1 ((CC_B_64 | CC_G_85) >> 16) & 0xFFFF
    #define _FC3_0 ((CC_R_143 | LOD_LSD_RB_1 | LODVTH_B__0V2 | LODVTH_G__0V2 | LODVTH_R__0V2)) & 0xFFFF
    
    #define _FC4_2 ((DE_COUPLE3_EN_0 | DE_COUPLE3__1 | DE_COUPLE2_0 | FIRST_LINE_DIM__1 | CAURSE_B_1 | CAURSE_G_1 | CAURSE_R_1) >> 32) & 0xFFFF
    #define _FC4_1 ((SR_ON_B_0 | SR_ON_G_0 | SR_ON_R_0 | SR_OFF_B_1 | SR_OFF_G_1 | SR_OFF_R_1 | FINE_B_1 | FINE_G_1 | FINE_R_1) >> 16) & 0xFFFF
    #define _FC4_0 ((SCAN_REV_1 | IMAX_0 | LAST_SOUT_0)) & 0xFFFF
#elif LP5890
    #define _FC0_2 ((MOD_SIZE_1 | GRP_DLY_B_0 | GRP_DLY_G_0 | GRP_DLY_R_0) >> 32) & 0xFFFF
    #define _FC0_1 ((FREQ_MUL__4 | FREQ_MOD__LF | SUBP_NUM__128 | SCAN_NUM__16) >> 16) & 0xFFFF
    #define _FC0_0 ((LODRM_EN_1 | PSP_MOD_0 | PS_EN_0 | PDC_EN_1 | CHIP_NUM__1)) & 0xFFFF
    
    #define _FC1_2 ((BLK_ADJ_0 | LINE_SWT__420 | LG_ENH_B_2 | LG_ENH_G_15) >> 32) & 0xFFFF
    #define _FC1_1 ((LG_ENH_G_15 | LG_ENH_R_1 | LG_STEP_B__6 | LG_STEP_G__6) >> 16) & 0xFFFF
    #define _FC1_0 ((LG_STEP_G__6 | LG_STEP_R__6 | SEG_LENGTH__512)) & 0xFFFF
    
    #define _FC2_2 ((SUBP_MAX_256_0 | CH_B_IMMUNITY_0 | CH_G_IMMUNITY_0 | CH_R_IMMUNITY_0 | LG_COLOR_B__14) >> 32) & 0xFFFF
    #define _FC2_1 ((LG_COLOR_G__14 | LG_COLOR_R__1 | DE_COUPLE1_B__4 | DE_COUPLE1_G__4) >> 16) & 0xFFFF
    #define _FC2_0 ((DE_COUPLE1_R__1 | V_PDC_B__0V1 | V_PDC_G__0V1 | V_PDC_R__0V1)) & 0xFFFF
    
    #define _FC3_2 ((LSDVTH_B__0V2 | LSDVTH_G__0V2 | LSDVTH_R__0V2 | LSD_RM__16 | BC_2) >> 32) & 0xFFFF
    #define _FC3_1 ((CC_B_64 | CC_G_85) >> 16) & 0xFFFF
    #define _FC3_0 ((CC_R_143 | LOD_LSD_RB_1 | LODVTH_B__0V2 | LODVTH_G__0V2 | LODVTH_R__0V2)) & 0xFFFF
    
    #define _FC4_2 ((DE_COUPLE3_EN_0 | DE_COUPLE3__1 | DE_COUPLE2_0 | FIRST_LINE_DIM__1 | LG_CAURSE_B_1 | LG_CAURSE_G_1 | LG_CAURSE_R_1) >> 32) & 0xFFFF
    #define _FC4_1 ((SR_ON_B_0 | SR_ON_G_0 | SR_ON_R_0 | SR_OFF_B_1 | SR_OFF_G_1 | SR_OFF_R_1 | LG_FINE_B_1 | LG_FINE_G_1 | LG_FINE_R_1) >> 16) & 0xFFFF
    #define _FC4_0 ((SCAN_REV_1 | IMAX_0)) & 0xFFFF
#elif LP5891
    #define _FC0_2 ((LSD_RM_EN_0 | GRP_DLY_B_0 | GRP_DLY_G_0 | GRP_DLY_R_0 | FREQ_MUL__4) >> 32) & 0xFFFF
    #define _FC0_1 ((FREQ_MUL__4 | FREQ_MOD__LF | SUBP_NUM__128 | SCAN_NUM__16) >> 16) & 0xFFFF
    #define _FC0_0 ((LODRM_EN_0 | PSP_MOD_0 | PS_EN_0 | PDC_EN_1 | CHIP_NUM__1)) & 0xFFFF
    
    #define _FC1_2 ((BLK_ADJ_0 | LINE_SWT__420 | LG_ENH_B_2 | LG_ENH_G_15) >> 32) & 0xFFFF
    #define _FC1_1 ((LG_ENH_G_15 | LG_ENH_R_1 | LG_STEP_B__6 | LG_STEP_G__6) >> 16) & 0xFFFF
    #define _FC1_0 ((LG_STEP_G__6 | LG_STEP_R__6 | SEG_LENGTH__512)) & 0xFFFF
    
    #define _FC2_2 ((MPSM_EN_0 | MOD_SIZE_0 | SUBP_MAX_256_0 | CH_B_IMMUNITY_0 | CH_G_IMMUNITY_0 | CH_R_IMMUNITY_0 | LG_COLOR_B__14) >> 32) & 0xFFFF
    #define _FC2_1 ((LG_COLOR_G__14 | LG_COLOR_R__1 | DE_COUPLE1_B__4 | DE_COUPLE1_G__4) >> 16) & 0xFFFF
    #define _FC2_0 ((DE_COUPLE1_R__1 | V_PDC_B__0V1 | V_PDC_G__0V1 | V_PDC_R__0V1)) & 0xFFFF
    
    #define _FC3_2 ((LSDVTH_B__0V2 | LSDVTH_G__0V2 | LSDVTH_R__0V2 | LSD_RM__16 | BC_2) >> 32) & 0xFFFF
    #define _FC3_1 ((CC_B_64 | CC_G_85) >> 16) & 0xFFFF
    #define _FC3_0 ((CC_R_143 | LOD_LSD_RB_0 | LODVTH_B__0V2 | LODVTH_G__0V2 | LODVTH_R__0V2)) & 0xFFFF
    
    #define _FC4_2 ((DE_COUPLE3_EN_0 | DE_COUPLE3__1 | DE_COUPLE2_0 | FIRST_LINE_DIM__1 | CAURSE_B_1 | CAURSE_G_1 | CAURSE_R_1) >> 32) & 0xFFFF
    #define _FC4_1 ((SR_ON_B_0 | SR_ON_G_0 | SR_ON_R_0 | SR_OFF_B_1 | SR_OFF_G_1 | SR_OFF_R_1 | FINE_B_1 | FINE_G_1 | FINE_R_1) >> 16) & 0xFFFF
    #define _FC4_0 ((SCAN_REV_1 | IMAX_0)) & 0xFFFF
#endif

#endif /* FC_SETTINGS_H_ */
