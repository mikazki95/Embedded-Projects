/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Auto generated file
 */

#include "ti_drivers_config.h"
#include <drivers/sciclient.h>

/* DDR */

/* DDR register config .h file as generated from DDR SUBSYSTEM REGISTER CONFIGURATION tool
 * Make sure path to this file is specified in your application project/makefile include path
 */
#include "drivers/ddr/v0/soc/am64x_am243x/board_ddrReginit.h"


DDR_EccRegion gDdrEccRegion =
{
    .ddrEccStart0 = DDR_ECC_REGION0_START,
    .ddrEccEnd0 = DDR_ECC_REGION0_END,

    .ddrEccStart1 = DDR_ECC_REGION1_START,
    .ddrEccEnd1 = DDR_ECC_REGION1_END,

    .ddrEccStart2 = DDR_ECC_REGION2_START,
    .ddrEccEnd2 = DDR_ECC_REGION2_END,
};

static DDR_Params gDdrParams =
{
    /* below values are set using the globals defined in drivers/ddr/v0/soc/am64x_am243x/board_ddrReginit.h */
    .clk1Freq              = DDRSS_PLL_FREQUENCY_1,
    .clk2Freq              = DDRSS_PLL_FREQUENCY_2,
    .ddrssCtlReg           = DDRSS_ctlReg,
    .ddrssPhyIndepReg      = DDRSS_phyIndepReg,
    .ddrssPhyReg           = DDRSS_phyReg,
    .ddrssCtlRegNum        = DDRSS_ctlRegNum,
    .ddrssPhyIndepRegNum   = DDRSS_phyIndepRegNum,
    .ddrssPhyRegNum        = DDRSS_phyRegNum,
    .ddrssCtlRegCount      = DDRSS_CTL_REG_INIT_COUNT,
    .ddrssPhyIndepRegCount = DDRSS_PHY_INDEP_REG_INIT_COUNT,
    .ddrssPhyRegCount      = DDRSS_PHY_REG_INIT_COUNT,
	.fshcount              = DDRSS_PLL_FHS_CNT,
    .enableEccFlag = 1,
    .eccRegion = &gDdrEccRegion,
};

/*
 * ESM
 */
/* ESM atrributes */
static ESM_Attrs gEsmAttrs[CONFIG_ESM_NUM_INSTANCES] =
{
    {
        .ptrESMRegs             = (CSL_esmRegs *)CSL_ESM0_CFG_BASE,
        .highPrioIntNum         = CSLR_R5FSS0_CORE0_INTR_ESM0_ESM_INT_HI_LVL_0,
        .lowPrioIntNum          = CSLR_R5FSS0_CORE0_INTR_ESM0_ESM_INT_LOW_LVL_0,
        .intrHighPriority       = 15U,
        .intrLowPriority        = 8U,
    },
};
/* ESM objects - initialized by the driver */
static ESM_Object gEsmObjects[CONFIG_ESM_NUM_INSTANCES];
/* ESM driver configuration */
ESM_Config gEsmConfig[CONFIG_ESM_NUM_INSTANCES] =
{
    {
        &gEsmAttrs[CONFIG_ESM0],
        &gEsmObjects[CONFIG_ESM0],
    },
};

uint32_t gEsmConfigNum = CONFIG_ESM_NUM_INSTANCES;

/*
 * IPC Notify
 */
#include <drivers/ipc_notify.h>
#include <drivers/ipc_notify/v0/ipc_notify_v0.h>

/* this function is called within IpcNotify_init, this function returns core specific IPC config */
void IpcNotify_getConfig(IpcNotify_InterruptConfig **interruptConfig, uint32_t *interruptConfigNum)
{
    /* extern globals that are specific to this core */
    extern IpcNotify_InterruptConfig gIpcNotifyInterruptConfig_r5fss0_0[];
    extern uint32_t gIpcNotifyInterruptConfigNum_r5fss0_0;

    *interruptConfig = &gIpcNotifyInterruptConfig_r5fss0_0[0];
    *interruptConfigNum = gIpcNotifyInterruptConfigNum_r5fss0_0;
}



/*
 * UART
 */

/* UART atrributes */
static UART_Attrs gUartAttrs[CONFIG_UART_NUM_INSTANCES] =
{
    {
        .baseAddr           = CSL_UART0_BASE,
        .inputClkFreq       = 48000000U,
    },
};
/* UART objects - initialized by the driver */
static UART_Object gUartObjects[CONFIG_UART_NUM_INSTANCES];
/* UART driver configuration */
UART_Config gUartConfig[CONFIG_UART_NUM_INSTANCES] =
{
    {
        &gUartAttrs[CONFIG_UART_CONSOLE],
        &gUartObjects[CONFIG_UART_CONSOLE],
    },
};

uint32_t gUartConfigNum = CONFIG_UART_NUM_INSTANCES;

#include <drivers/uart/v0/dma/uart_dma.h>


UART_DmaConfig gUartDmaConfig[CONFIG_UART_NUM_DMA_INSTANCES] =
{
};

uint32_t gUartDmaConfigNum = CONFIG_UART_NUM_DMA_INSTANCES;


void Drivers_uartInit(void)
{
    UART_init();
}


void Pinmux_init(void);
void PowerClock_init(void);
void PowerClock_deinit(void);

/*
 * Common Functions
 */
void System_init(void)
{
    /* DPL init sets up address transalation unit, on some CPUs this is needed
     * to access SCICLIENT services, hence this needs to happen first
     */
    Dpl_init();
    /* We should do sciclient init before we enable power and clock to the peripherals */
    /* SCICLIENT init */
    {
        int32_t retVal = SystemP_SUCCESS;

        retVal = Sciclient_init(CSL_CORE_ID_R5FSS0_0);
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);
    }

    
    /* initialize PMU */
    CycleCounterP_init(SOC_getSelfCpuClk());


    PowerClock_init();
    /* Now we can do pinmux */
    Pinmux_init();
    /* finally we initialize all peripheral drivers */
	DDR_init(&gDdrParams);

    ESM_init();
    /* IPC Notify */
    {
        IpcNotify_Params notifyParams;
        int32_t status;

        /* initialize parameters to default */
        IpcNotify_Params_init(&notifyParams);

        /* specify the priority of IPC Notify interrupt */
        notifyParams.intrPriority = 15U;

        /* specify the core on which this API is called */
        notifyParams.selfCoreId = CSL_CORE_ID_R5FSS0_0;

        /* list the cores that will do IPC Notify with this core
        * Make sure to NOT list 'self' core in the list below
        */
        notifyParams.numCores = 1;
        notifyParams.coreIdList[0] = CSL_CORE_ID_M4FSS0_0;

        /* initialize the IPC Notify module */
        status = IpcNotify_init(&notifyParams);
        DebugP_assert(status==SystemP_SUCCESS);

    }

    Drivers_uartInit();
}

void System_deinit(void)
{
    ESM_deinit();
    IpcNotify_deInit();

    UART_deinit();
    PowerClock_deinit();
    /* SCICLIENT deinit */
    {
        int32_t         retVal = SystemP_SUCCESS;

        retVal = Sciclient_deinit();
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);
    }
    Dpl_deinit();
}
