/*
 **
 ** sru_config.c source file generated on December 8, 2023 at 14:22:02.
 **
 ** Copyright (C) 2014-2023 Analog Devices Inc., All Rights Reserved.
 **
 ** This file is generated automatically based upon the options selected in
 ** the SRU Configuration editor. Changes to the SRU configuration should
 ** be made by changing the appropriate options rather than editing this file.
 **
 ** By default only the SRU registers with values different from their default
 ** reset value are initialized. Alternatively, macro SRU_FULL_INIT may be
 ** defined in the project settings to cause all SRU registers to be
 ** initialized.
 **
 */

#include <stdint.h>
#include <sys/platform.h>

int32_t adi_SRU_Init(void);

/*
 * Initialize the Signal Routing Unit
 */
int32_t adi_SRU_Init(void)
{
#if defined(SRU_FULL_INIT)
    /* Connections:
     *    DAI0_PB19_O - SPT2B_CLK_I
     *    DAI0_PB07_O - SPT0B_CLK_I
     *    DAI0_PB13_O - SPT1B_CLK_I
     *    DAI0_PB13_O - SPT1A_CLK_I
     *    DAI0_PB03_O - SPT0A_CLK_I
     *    DAI0_PB19_O - SPT2A_CLK_I
     */
    *pREG_DAI0_CLK0 = 0x252630c2uL;
    *pREG_DAI0_EXTD_CLK0 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - SRC0_CLK_OP_I
     *    DAI0_LOW - SRC1_CLK_IP_I
     *    DAI0_LOW - SRC2_CLK_IP_I
     *    DAI0_LOW - SRC2_CLK_OP_I
     *    DAI0_LOW - SRC0_CLK_IP_I
     *    DAI0_LOW - SRC1_CLK_OP_I
     */
    *pREG_DAI0_CLK1 = 0x3def7bdeuL;
    *pREG_DAI0_EXTD_CLK1 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - SPDIF0_TX_CLK_I
     *    DAI0_LOW - PDM0_BCLK_I
     *    DAI0_LOW - SRC3_CLK_IP_I
     *    DAI0_LOW - PDM0_CLK0_I
     *    DAI0_LOW - SRC3_CLK_OP_I
     */
    *pREG_DAI0_CLK2 = 0x3def7bdeuL;
    *pREG_DAI0_EXTD_CLK2 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - SPDIF0_TX_HFCLK_I
     */
    *pREG_DAI0_CLK3 = 0x3def7bdeuL;
    *pREG_DAI0_EXTD_CLK3 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - SPDIF0_TX_EXT_SYNC_I
     *    DAI0_LOW - PCG0_EXTCLKB_I
     *    DAI0_LOW - PCG0_SYNC_CLKA_I
     *    DAI0_LOW - PCG0_EXTCLKA_I
     *    DAI0_LOW - PCG0_SYNC_CLKB_I
     */
    *pREG_DAI0_CLK4 = 0x3def7bdeuL;
    *pREG_DAI0_EXTD_CLK4 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - PCG0_EXTCLKF_I
     *    DAI0_LOW - SPT3B_CLK_I
     *    DAI0_LOW - PCG0_EXTCLKE_I
     *    DAI0_LOW - SPT3A_CLK_I
     *    DAI0_LOW - PCG0_SYNC_CLKE_I
     *    DAI0_LOW - PCG0_SYNC_CLKF_I
     */
    *pREG_DAI0_CLK5 = 0x3def7bdeuL;
    *pREG_DAI0_EXTD_CLK5 = 0x00000000uL;

    /* Connections:
     *    DAI0_PB09_O - SPT1A_D0_I
     *    DAI0_PB05_O - SPT0B_D0_I
     *    DAI0_PB06_O - SPT0B_D1_I
     *    DAI0_PB01_O - SPT0A_D0_I
     *    DAI0_PB02_O - SPT0A_D1_I
     */
    *pREG_DAI0_DAT0 = 0x08144040uL;
    *pREG_DAI0_EXTD_DAT0 = 0x00000000uL;

    /* Connections:
     *    DAI0_PB16_O - SPT2A_D1_I
     *    DAI0_PB10_O - SPT1A_D1_I
     *    DAI0_PB11_O - SPT1B_D0_I
     *    DAI0_PB15_O - SPT2A_D0_I
     *    DAI0_PB12_O - SPT1B_D1_I
     */
    *pREG_DAI0_DAT1 = 0x0f38b289uL;
    *pREG_DAI0_EXTD_DAT1 = 0x00000000uL;

    /* Connections:
     *    DAI0_PB01_O - SRC1_DAT_IP_I
     *    DAI0_PB17_O - SPT2B_D0_I
     *    DAI0_PB18_O - SPT2B_D1_I
     *    DAI0_PB01_O - SRC2_DAT_IP_I
     *    DAI0_PB01_O - SRC0_DAT_IP_I
     */
    *pREG_DAI0_DAT2 = 0x00000450uL;
    *pREG_DAI0_EXTD_DAT2 = 0x00000000uL;

    /* Connections:
     *    DAI0_PB01_O - SRC3_DAT_TDM_OP_I
     *    DAI0_PB01_O - SRC3_DAT_IP_I
     *    DAI0_PB01_O - SRC0_DAT_TDM_OP_I
     *    DAI0_PB01_O - SRC1_DAT_TDM_OP_I
     *    DAI0_PB01_O - SRC2_DAT_TDM_OP_I
     */
    *pREG_DAI0_DAT3 = 0x00000000uL;
    *pREG_DAI0_EXTD_DAT3 = 0x00000000uL;

    /* Connections:
     *    DAI0_PB01_O - PDM0_DAT1_I
     *    DAI0_PB01_O - SPDIF0_TX_DAT_I
     *    DAI0_PB01_O - PDM0_DAT0_I
     */
    *pREG_DAI0_DAT4 = 0x00000000uL;
    *pREG_DAI0_EXTD_DAT4 = 0x00000000uL;

    /* Connections:
     *    DAI0_PB01_O - SPDIF0_RX_I
     */
    *pREG_DAI0_DAT5 = 0x00000000uL;
    *pREG_DAI0_EXTD_DAT5 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - SPT3B_D0_I
     *    DAI0_LOW - SPT3A_D0_I
     *    DAI0_LOW - SPT3A_D1_I
     *    DAI0_LOW - SPT3B_D1_I
     */
    *pREG_DAI0_DAT6 = 0x00fbefbeuL;
    *pREG_DAI0_EXTD_DAT6 = 0x00000000uL;

    /* Connections:
     *    DAI0_PB14_O - SPT1A_FS_I
     *    DAI0_PB20_O - SPT2A_FS_I
     *    DAI0_PB08_O - SPT0B_FS_I
     *    DAI0_PB20_O - SPT2B_FS_I
     *    DAI0_PB04_O - SPT0A_FS_I
     *    DAI0_PB14_O - SPT1B_FS_I
     */
    *pREG_DAI0_FS0 = 0x2736b4e3uL;
    *pREG_DAI0_EXTD_FS0 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - SRC2_FS_IP_I
     *    DAI0_LOW - SRC0_FS_OP_I
     *    DAI0_LOW - SRC0_FS_IP_I
     *    DAI0_LOW - SRC1_FS_OP_I
     *    DAI0_LOW - SRC2_FS_OP_I
     *    DAI0_LOW - SRC1_FS_IP_I
     */
    *pREG_DAI0_FS1 = 0x3def7bdeuL;
    *pREG_DAI0_EXTD_FS1 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - SPDIF0_TX_FS_I
     *    DAI0_LOW - SRC3_FS_IP_I
     *    DAI0_LOW - SRC3_FS_OP_I
     */
    *pREG_DAI0_FS2 = 0x3def7bdeuL;
    *pREG_DAI0_EXTD_FS2 = 0x00000000uL;
#endif /* defined(SRU_FULL_INIT) */

    /* Connections:
     *    DAI0_PB01_O - TM0_ACI14_I (non-default)
     *    DAI0_PB01_O - PDM0_LRCLK_I (non-default)
     *    DAI0_LOW - SPT3A_FS_I
     *    DAI0_LOW - SPT3B_FS_I
     */
    *pREG_DAI0_FS4 = 0x000003deuL;
    *pREG_DAI0_EXTD_FS4 = 0x00000000uL;

#if defined(SRU_FULL_INIT)
    /* Connections:
     *    DAI0_LOW - DAI0_INT_8_I
     *    DAI0_LOW - DAI0_INT_7_I
     *    DAI0_LOW - DAI0_INT_6_I
     *    DAI0_LOW - DAI0_MISCA4_I
     *    DAI0_LOW - DAI0_MISCA5_I
     *    DAI0_LOW - DAI0_INV_MISCA4_I
     *    DAI0_LOW - DAI0_INT_9_I
     *    DAI0_LOW - DAI0_INV_MISCA5_I
     *    DAI0_LOW - DAI0_MISCA0_I
     *    DAI0_LOW - DAI0_MISCA1_I
     *    DAI0_LOW - DAI0_MISCA2_I
     *    DAI0_LOW - DAI0_MISCA3_I
     */
    *pREG_DAI0_MISC0 = 0x3def7bdeuL;
    *pREG_DAI0_EXTD_MISC0 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - DAI0_INT_4_I
     *    DAI0_LOW - DAI0_INT_3_I
     *    DAI0_LOW - DAI0_INT_2_I
     *    DAI0_LOW - DAI0_INT_1_I
     *    DAI0_LOW - DAI0_INT_5_I
     *    DAI0_LOW - DAI0_INT_0_I
     */
    *pREG_DAI0_MISC1 = 0x3def7bdeuL;
    *pREG_DAI0_EXTD_MISC1 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - PCG0_HWB_TRIG_I
     *    DAI0_LOW - PCG0_HWA_TRIG_I
     *    DAI0_LOW - PCG0_HWF_TRIG_I
     *    DAI0_LOW - PCG0_HWE_TRIG_I
     */
    *pREG_DAI0_MISC2 = 0x000f7bdeuL;
    *pREG_DAI0_EXTD_MISC2 = 0x00000000uL;

    /* Connections:
     *    SPT0A_CLK_PBEN_O - DAI0_PBEN03_I
     *    SPT0A_D1_PBEN_O - DAI0_PBEN02_I
     *    SPT0A_D0_PBEN_O - DAI0_PBEN01_I
     *    SPT0B_D0_PBEN_O - DAI0_PBEN05_I
     *    SPT0A_FS_PBEN_O - DAI0_PBEN04_I
     */
    *pREG_DAI0_PBEN0 = 0x0e2482cauL;
    *pREG_DAI0_EXTD_PBEN0 = 0x00000000uL;

    /* Connections:
     *    SPT0B_CLK_PBEN_O - DAI0_PBEN07_I
     *    SPT0B_D1_PBEN_O - DAI0_PBEN06_I
     *    SPT1A_D0_PBEN_O - DAI0_PBEN09_I
     *    SPT0B_FS_PBEN_O - DAI0_PBEN08_I
     *    SPT1A_D1_PBEN_O - DAI0_PBEN10_I
     */
    *pREG_DAI0_PBEN1 = 0x1348d30fuL;
    *pREG_DAI0_EXTD_PBEN1 = 0x00000000uL;

    /* Connections:
     *    SPT2A_D0_PBEN_O - DAI0_PBEN15_I
     *    SPT1B_D1_PBEN_O - DAI0_PBEN12_I
     *    SPT1B_D0_PBEN_O - DAI0_PBEN11_I
     *    SPT1B_CLK_PBEN_O - DAI0_PBEN13_I
     *    SPT1B_FS_PBEN_O - DAI0_PBEN14_I
     */
    *pREG_DAI0_PBEN2 = 0x1a5545d6uL;
    *pREG_DAI0_EXTD_PBEN2 = 0x00000000uL;

    /* Connections:
     *    SPT2A_D1_PBEN_O - DAI0_PBEN16_I
     *    SPT2B_D0_PBEN_O - DAI0_PBEN17_I
     *    SPT2B_D1_PBEN_O - DAI0_PBEN18_I
     *    SPT2B_FS_PBEN_O - DAI0_PBEN20_I
     *    SPT2B_CLK_PBEN_O - DAI0_PBEN19_I
     */
    *pREG_DAI0_PBEN3 = 0x1d71f79buL;
    *pREG_DAI0_EXTD_PBEN3 = 0x00000000uL;

    /* Connections:
     *    SPT0A_D1_O - DAI0_PB02_I
     *    SPT0A_D0_O - DAI0_PB01_I
     *    SPT0A_FS_O - DAI0_PB04_I
     *    SPT0A_CLK_O - DAI0_PB03_I
     */
    *pREG_DAI0_PIN0 = 0x04c80a94uL;
    *pREG_DAI0_EXTD_PIN0 = 0x00000000uL;

    /* Connections:
     *    SPT0B_D1_O - DAI0_PB06_I
     *    SPT0B_D0_O - DAI0_PB05_I
     *    SPT0B_FS_O - DAI0_PB08_I
     *    SPT0B_CLK_O - DAI0_PB07_I
     */
    *pREG_DAI0_PIN1 = 0x04e84b96uL;
    *pREG_DAI0_EXTD_PIN1 = 0x00000000uL;

    /* Connections:
     *    SPT1B_D1_O - DAI0_PB12_I
     *    SPT1A_D1_O - DAI0_PB10_I
     *    SPT1B_D0_O - DAI0_PB11_I
     *    SPT1A_D0_O - DAI0_PB09_I
     */
    *pREG_DAI0_PIN2 = 0x03668c98uL;
    *pREG_DAI0_EXTD_PIN2 = 0x00000000uL;

    /* Connections:
     *    SPT2A_D1_O - DAI0_PB16_I
     *    SPT1B_FS_O - DAI0_PB14_I
     *    SPT2A_D0_O - DAI0_PB15_I
     *    SPT1B_CLK_O - DAI0_PB13_I
     */
    *pREG_DAI0_PIN3 = 0x03a714a3uL;
    *pREG_DAI0_EXTD_PIN3 = 0x00000000uL;

    /* Connections:
     *    SPT2B_D1_O - DAI0_PB18_I
     *    SPT2B_CLK_O - DAI0_PB19_I
     *    DAI0_LOW - DAI0_INV_PB19_I
     *    SPT2B_D0_O - DAI0_PB17_I
     *    SPT2B_FS_O - DAI0_PB20_I
     *    DAI0_LOW - DAI0_INV_PB20_I
     */
    *pREG_DAI0_PIN4 = 0x05694f9euL;
    *pREG_DAI0_EXTD_PIN4 = 0x00000000uL;

    /* Connections:
     *    DAI1_PB03_O - SPT4A_CLK_I
     *    DAI1_PB13_O - SPT5A_CLK_I
     *    DAI1_PB07_O - SPT4B_CLK_I
     *    DAI1_PB13_O - SPT5B_CLK_I
     *    DAI1_PB19_O - SPT6B_CLK_I
     *    DAI1_PB19_O - SPT6A_CLK_I
     */
    *pREG_DAI1_CLK0 = 0x252630c2uL;
    *pREG_DAI1_EXTD_CLK0 = 0x00000000uL;

    /* Connections:
     *    DAI1_LOW - SRC4_CLK_IP_I
     *    DAI1_LOW - SRC6_CLK_OP_I
     *    DAI1_LOW - SRC4_CLK_OP_I
     *    DAI1_LOW - SRC5_CLK_IP_I
     *    DAI1_LOW - SRC6_CLK_IP_I
     *    DAI1_LOW - SRC5_CLK_OP_I
     */
    *pREG_DAI1_CLK1 = 0x3def7bdeuL;
    *pREG_DAI1_EXTD_CLK1 = 0x00000000uL;

    /* Connections:
     *    DAI1_LOW - SRC7_CLK_IP_I
     *    DAI1_LOW - PDM1_CLK0_I
     *    DAI1_LOW - SRC7_CLK_OP_I
     *    DAI1_LOW - PDM1_BCLK_I
     *    DAI1_LOW - SPDIF1_TX_CLK_I
     */
    *pREG_DAI1_CLK2 = 0x3def7bdeuL;
    *pREG_DAI1_EXTD_CLK2 = 0x00000000uL;

    /* Connections:
     *    DAI1_LOW - SPDIF1_TX_HFCLK_I
     */
    *pREG_DAI1_CLK3 = 0x3def7bdeuL;
    *pREG_DAI1_EXTD_CLK3 = 0x00000000uL;

    /* Connections:
     *    DAI1_LOW - PCG0_EXTCLKC_I
     *    DAI1_LOW - PCG0_EXTCLKD_I
     *    DAI1_LOW - SPDIF1_TX_EXT_SYNC_I
     *    DAI1_LOW - PCG0_SYNC_CLKC_I
     *    DAI1_LOW - PCG0_SYNC_CLKD_I
     */
    *pREG_DAI1_CLK4 = 0x3def7bdeuL;
    *pREG_DAI1_EXTD_CLK4 = 0x00000000uL;

    /* Connections:
     *    DAI1_LOW - PCG0_SYNC_CLKG_I
     *    DAI1_LOW - PCG0_SYNC_CLKH_I
     *    DAI1_LOW - PCG0_EXTCLKG_I
     *    DAI1_LOW - PCG0_EXTCLKH_I
     *    DAI1_LOW - SPT7B_CLK_I
     *    DAI1_LOW - SPT7A_CLK_I
     */
    *pREG_DAI1_CLK5 = 0x3def7bdeuL;
    *pREG_DAI1_EXTD_CLK5 = 0x00000000uL;

    /* Connections:
     *    DAI1_PB06_O - SPT4B_D1_I
     *    DAI1_PB05_O - SPT4B_D0_I
     *    DAI1_PB09_O - SPT5A_D0_I
     *    DAI1_PB02_O - SPT4A_D1_I
     *    DAI1_PB01_O - SPT4A_D0_I
     */
    *pREG_DAI1_DAT0 = 0x08144040uL;
    *pREG_DAI1_EXTD_DAT0 = 0x00000000uL;

    /* Connections:
     *    DAI1_PB10_O - SPT5A_D1_I
     *    DAI1_PB11_O - SPT5B_D0_I
     *    DAI1_PB15_O - SPT6A_D0_I
     *    DAI1_PB12_O - SPT5B_D1_I
     *    DAI1_PB16_O - SPT6A_D1_I
     */
    *pREG_DAI1_DAT1 = 0x0f38b289uL;
    *pREG_DAI1_EXTD_DAT1 = 0x00000000uL;

    /* Connections:
     *    DAI1_PB01_O - SRC5_DAT_IP_I
     *    DAI1_PB18_O - SPT6B_D1_I
     *    DAI1_PB17_O - SPT6B_D0_I
     *    DAI1_PB01_O - SRC4_DAT_IP_I
     *    DAI1_PB01_O - SRC6_DAT_IP_I
     */
    *pREG_DAI1_DAT2 = 0x00000450uL;
    *pREG_DAI1_EXTD_DAT2 = 0x00000000uL;

    /* Connections:
     *    DAI1_PB01_O - SRC4_DAT_TDM_OP_I
     *    DAI1_PB01_O - SRC5_DAT_TDM_OP_I
     *    DAI1_PB01_O - SRC7_DAT_IP_I
     *    DAI1_PB01_O - SRC7_DAT_TDM_OP_I
     *    DAI1_PB01_O - SRC6_DAT_TDM_OP_I
     */
    *pREG_DAI1_DAT3 = 0x00000000uL;
    *pREG_DAI1_EXTD_DAT3 = 0x00000000uL;

    /* Connections:
     *    DAI1_PB01_O - PDM1_DAT1_I
     *    DAI1_PB01_O - PDM1_DAT0_I
     *    DAI1_PB01_O - SPDIF1_TX_DAT_I
     */
    *pREG_DAI1_DAT4 = 0x00000000uL;
    *pREG_DAI1_EXTD_DAT4 = 0x00000000uL;

    /* Connections:
     *    DAI1_PB01_O - SPDIF1_RX_I
     */
    *pREG_DAI1_DAT5 = 0x00000000uL;
    *pREG_DAI1_EXTD_DAT5 = 0x00000000uL;

    /* Connections:
     *    DAI1_LOW - SPT7B_D1_I
     *    DAI1_LOW - SPT7A_D1_I
     *    DAI1_LOW - SPT7A_D0_I
     *    DAI1_LOW - SPT7B_D0_I
     */
    *pREG_DAI1_DAT6 = 0x00fbefbeuL;
    *pREG_DAI1_EXTD_DAT6 = 0x00000000uL;

    /* Connections:
     *    DAI1_PB20_O - SPT6B_FS_I
     *    DAI1_PB08_O - SPT4B_FS_I
     *    DAI1_PB14_O - SPT5A_FS_I
     *    DAI1_PB20_O - SPT6A_FS_I
     *    DAI1_PB14_O - SPT5B_FS_I
     *    DAI1_PB04_O - SPT4A_FS_I
     */
    *pREG_DAI1_FS0 = 0x2736b4e3uL;
    *pREG_DAI1_EXTD_FS0 = 0x00000000uL;

    /* Connections:
     *    DAI1_LOW - SRC4_FS_OP_I
     *    DAI1_LOW - SRC6_FS_IP_I
     *    DAI1_LOW - SRC5_FS_IP_I
     *    DAI1_LOW - SRC6_FS_OP_I
     *    DAI1_LOW - SRC4_FS_IP_I
     *    DAI1_LOW - SRC5_FS_OP_I
     */
    *pREG_DAI1_FS1 = 0x3def7bdeuL;
    *pREG_DAI1_EXTD_FS1 = 0x00000000uL;

    /* Connections:
     *    DAI1_LOW - SPDIF1_TX_FS_I
     *    DAI1_LOW - SRC7_FS_OP_I
     *    DAI1_LOW - SRC7_FS_IP_I
     */
    *pREG_DAI1_FS2 = 0x3def7bdeuL;
    *pREG_DAI1_EXTD_FS2 = 0x00000000uL;
#endif /* defined(SRU_FULL_INIT) */

    /* Connections:
     *    DAI1_PB01_O - PDM1_LRCLK_I (non-default)
     *    DAI1_LOW - SPT7B_FS_I
     *    DAI1_PB01_O - TM0_ACI15_I (non-default)
     *    DAI1_LOW - SPT7A_FS_I
     */
    *pREG_DAI1_FS4 = 0x000003deuL;
    *pREG_DAI1_EXTD_FS4 = 0x00000000uL;

#if defined(SRU_FULL_INIT)
    /* Connections:
     *    DAI1_LOW - DAI1_INT_8_I
     *    DAI1_LOW - DAI1_INT_9_I
     *    DAI1_LOW - DAI1_MISCA5_I
     *    DAI1_LOW - DAI1_MISCA2_I
     *    DAI1_LOW - DAI1_INV_MISCA5_I
     *    DAI1_LOW - DAI1_MISCA1_I
     *    DAI1_LOW - DAI1_MISCA4_I
     *    DAI1_LOW - DAI1_MISCA3_I
     *    DAI1_LOW - DAI1_INV_MISCA4_I
     *    DAI1_LOW - DAI1_MISCA0_I
     *    DAI1_LOW - DAI1_INT_6_I
     *    DAI1_LOW - DAI1_INT_7_I
     */
    *pREG_DAI1_MISC0 = 0x3def7bdeuL;
    *pREG_DAI1_EXTD_MISC0 = 0x00000000uL;

    /* Connections:
     *    DAI1_LOW - DAI1_INT_0_I
     *    DAI1_LOW - DAI1_INT_1_I
     *    DAI1_LOW - DAI1_INT_2_I
     *    DAI1_LOW - DAI1_INT_3_I
     *    DAI1_LOW - DAI1_INT_4_I
     *    DAI1_LOW - DAI1_INT_5_I
     */
    *pREG_DAI1_MISC1 = 0x3def7bdeuL;
    *pREG_DAI1_EXTD_MISC1 = 0x00000000uL;

    /* Connections:
     *    DAI1_LOW - PCG0_HWC_TRIG_I
     *    DAI1_LOW - PCG0_HWH_TRIG_I
     *    DAI1_LOW - PCG0_HWD_TRIG_I
     *    DAI1_LOW - PCG0_HWG_TRIG_I
     */
    *pREG_DAI1_MISC2 = 0x000f7bdeuL;
    *pREG_DAI1_EXTD_MISC2 = 0x00000000uL;

    /* Connections:
     *    SPT4B_D0_PBEN_O - DAI1_PBEN05_I
     *    SPT4A_D0_PBEN_O - DAI1_PBEN01_I
     *    SPT4A_D1_PBEN_O - DAI1_PBEN02_I
     *    SPT4A_CLK_PBEN_O - DAI1_PBEN03_I
     *    SPT4A_FS_PBEN_O - DAI1_PBEN04_I
     */
    *pREG_DAI1_PBEN0 = 0x0e2482cauL;
    *pREG_DAI1_EXTD_PBEN0 = 0x00000000uL;

    /* Connections:
     *    SPT5A_D1_PBEN_O - DAI1_PBEN10_I
     *    SPT5A_D0_PBEN_O - DAI1_PBEN09_I
     *    SPT4B_D1_PBEN_O - DAI1_PBEN06_I
     *    SPT4B_FS_PBEN_O - DAI1_PBEN08_I
     *    SPT4B_CLK_PBEN_O - DAI1_PBEN07_I
     */
    *pREG_DAI1_PBEN1 = 0x1348d30fuL;
    *pREG_DAI1_EXTD_PBEN1 = 0x00000000uL;

    /* Connections:
     *    SPT5B_D0_PBEN_O - DAI1_PBEN11_I
     *    SPT5B_CLK_PBEN_O - DAI1_PBEN13_I
     *    SPT5B_D1_PBEN_O - DAI1_PBEN12_I
     *    SPT6A_D0_PBEN_O - DAI1_PBEN15_I
     *    SPT5B_FS_PBEN_O - DAI1_PBEN14_I
     */
    *pREG_DAI1_PBEN2 = 0x1a5545d6uL;
    *pREG_DAI1_EXTD_PBEN2 = 0x00000000uL;

    /* Connections:
     *    SPT6B_FS_PBEN_O - DAI1_PBEN20_I
     *    SPT6B_D0_PBEN_O - DAI1_PBEN17_I
     *    SPT6A_D1_PBEN_O - DAI1_PBEN16_I
     *    SPT6B_CLK_PBEN_O - DAI1_PBEN19_I
     *    SPT6B_D1_PBEN_O - DAI1_PBEN18_I
     */
    *pREG_DAI1_PBEN3 = 0x1d71f79buL;
    *pREG_DAI1_EXTD_PBEN3 = 0x00000000uL;

    /* Connections:
     *    SPT4A_FS_O - DAI1_PB04_I
     *    SPT4A_D1_O - DAI1_PB02_I
     *    SPT4A_CLK_O - DAI1_PB03_I
     *    SPT4A_D0_O - DAI1_PB01_I
     */
    *pREG_DAI1_PIN0 = 0x04c80a94uL;
    *pREG_DAI1_EXTD_PIN0 = 0x00000000uL;

    /* Connections:
     *    SPT4B_FS_O - DAI1_PB08_I
     *    SPT4B_D1_O - DAI1_PB06_I
     *    SPT4B_CLK_O - DAI1_PB07_I
     *    SPT4B_D0_O - DAI1_PB05_I
     */
    *pREG_DAI1_PIN1 = 0x04e84b96uL;
    *pREG_DAI1_EXTD_PIN1 = 0x00000000uL;

    /* Connections:
     *    SPT5A_D0_O - DAI1_PB09_I
     *    SPT5B_D1_O - DAI1_PB12_I
     *    SPT5B_D0_O - DAI1_PB11_I
     *    SPT5A_D1_O - DAI1_PB10_I
     */
    *pREG_DAI1_PIN2 = 0x03668c98uL;
    *pREG_DAI1_EXTD_PIN2 = 0x00000000uL;

    /* Connections:
     *    SPT6A_D1_O - DAI1_PB16_I
     *    SPT6A_D0_O - DAI1_PB15_I
     *    SPT5B_FS_O - DAI1_PB14_I
     *    SPT5B_CLK_O - DAI1_PB13_I
     */
    *pREG_DAI1_PIN3 = 0x03a714a3uL;
    *pREG_DAI1_EXTD_PIN3 = 0x00000000uL;

    /* Connections:
     *    SPT6B_CLK_O - DAI1_PB19_I
     *    SPT6B_D1_O - DAI1_PB18_I
     *    SPT6B_D0_O - DAI1_PB17_I
     *    DAI1_LOW - DAI1_INV_PB20_I
     *    SPT6B_FS_O - DAI1_PB20_I
     *    DAI1_LOW - DAI1_INV_PB19_I
     */
    *pREG_DAI1_PIN4 = 0x05694f9euL;
    *pREG_DAI1_EXTD_PIN4 = 0x00000000uL;
#endif /* defined(SRU_FULL_INIT) */
    /* PADS0 DAI0 Port Input Enable Control Register */
    *pREG_PADS0_DAI0_IE = BITM_PADS_DAI0_IE_VALUE;

    /* PADS0 DAI1 Port Input Enable Control Register */
    *pREG_PADS0_DAI1_IE = BITM_PADS_DAI1_IE_VALUE;

    return 0;
}

