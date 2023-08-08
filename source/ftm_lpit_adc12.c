/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_lpit.h"
#include "fsl_adc12.h"
#include "fsl_trgmux.h"
#include "fsl_ftm.h"
#include "fsl_gpio.h"   

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* For ADC. */
#define DEMO_ADC12_BASEADDR ADC0
#define DEMO_ADC12_USER_CHANNEL_A (27U)
#define DEMO_ADC12_USER_CHANNEL_B (28U)
#define DEMO_ADC12_USER_CHANNEL_C (30U)
#define DEMO_ADC12_USER_CHANNEL_D (8U)
#define DEMO_ADC12_CHANNEL_GROUP_A   (0U)
#define DEMO_ADC12_CHANNEL_GROUP_B   (1U)
#define DEMO_ADC12_CHANNEL_GROUP_C   (2U)
#define DEMO_ADC12_CHANNEL_GROUP_D   (3U)
#define DEMO_ADC12_IRQ_ID           ADC0_IRQn
#define DEMO_ADC12_IRQ_HANDLER_FUNC ADC0_IRQHandler

/* For LPIT. */
#define DEMO_LPIT_BASE LPIT0
//#define DEMO_LPIT_USER_CHANNEL  kLPIT_Chnl_0
#define DEMO_LPIT_USER_TIMER_CH kLPIT_Trigger_TimerChn0
#define DEMO_LPIT_SOURCECLOCK   CLOCK_GetIpFreq(kCLOCK_Lpit0)
/* LPIT triggers ADC every LPIT_TRIGGER_TIME us*/
#define LPIT_TRIGGER_TIME (5U)
#define DEMO_LPIT_IRQHandler LPIT0_IRQHandler
   
/* For FTM. */
#define DEMO_FTM_BASE FTM0
#define DEMO_FTM_COUNTER_CLOCK_HZ CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define DEMO_FTM_COUNTER_CLOCK_SOURCE kFTM_SystemClock
#define DEMO_FTM_PWM_HZ 10000U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_ConfigTriggerSource(void);
/*!
 * @brief Initialize the LPIT to periodic counter mode
 */
static void DEMO_InitLpitTrigger(void);

/*!
 * @brief Initialize the FTM to periodic counter mode
 */
static void DEMO_Init_FtmTrigger(void);

/*!
 * @brief Initialize the ADC
 */
static void DEMO_InitAdc(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_Adc12InterruptCounter;
volatile bool g_Adc12ConversionCompletedFlag;
volatile uint32_t g_Adc12ConvValue_0;
volatile uint32_t g_Adc12ConvValue_1;
volatile uint32_t g_Adc12ConvValue_2;
volatile uint32_t g_Adc12ConvValue_3;
const uint32_t g_Adc12_12bitFullRange = 4096U;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief ISR for ADC12 interrupt function
 */
void DEMO_ADC12_IRQ_HANDLER_FUNC(void)
{ 
    /* Set GPIO. */
    GPIO_PortSet(GPIOB, 1U << 8U);
    if (0U !=
        (kADC12_ChannelConversionCompletedFlag & ADC12_GetChannelStatusFlags(DEMO_ADC12_BASEADDR, DEMO_ADC12_CHANNEL_GROUP_D)))
    {
     g_Adc12ConvValue_0 = ADC12_GetChannelConversionValue(DEMO_ADC12_BASEADDR, DEMO_ADC12_CHANNEL_GROUP_A);
     g_Adc12ConvValue_1 = ADC12_GetChannelConversionValue(DEMO_ADC12_BASEADDR, DEMO_ADC12_CHANNEL_GROUP_B); 
     g_Adc12ConvValue_2 = ADC12_GetChannelConversionValue(DEMO_ADC12_BASEADDR, DEMO_ADC12_CHANNEL_GROUP_C); 
     g_Adc12ConvValue_3 = ADC12_GetChannelConversionValue(DEMO_ADC12_BASEADDR, DEMO_ADC12_CHANNEL_GROUP_D);  
     
     g_Adc12InterruptCounter++;   
    }    
     g_Adc12ConversionCompletedFlag = true;   
     /* Clear GPIO. */
     GPIO_PortClear(GPIOB, 1U << 8U);
     SDK_ISR_EXIT_BARRIER;
}

/*!
 * @brief ISR for LPIT interrupt function
 */
void DEMO_LPIT_IRQHandler(void)
{
     /* Set GPIO. */
     GPIO_PortSet(GPIOA, 1U << 16U);

    uint32_t tempFlag = LPIT_GetStatusFlags(DEMO_LPIT_BASE);
     /* Clear interrupt flag.*/
    if (0U != (tempFlag & kLPIT_Channel0TimerFlag))
    {
        /* Clear interrupt flag.*/
       LPIT_ClearStatusFlags(DEMO_LPIT_BASE, kLPIT_Channel0TimerFlag);
    }

    if (0U != (tempFlag & kLPIT_Channel1TimerFlag))
    {
        /* Clear interrupt flag.*/
        LPIT_ClearStatusFlags(DEMO_LPIT_BASE, kLPIT_Channel1TimerFlag);
    }
        if (0U != (tempFlag & kLPIT_Channel2TimerFlag))
    {
        LPIT_ClearStatusFlags(DEMO_LPIT_BASE, kLPIT_Channel2TimerFlag);
    }
        if (0U != (tempFlag & kLPIT_Channel3TimerFlag))
    {
        LPIT_ClearStatusFlags(DEMO_LPIT_BASE, kLPIT_Channel3TimerFlag);
    }
     /* Clear GPIO. */    
     GPIO_PortClear(GPIOA, 1U << 16U);
    
     SDK_ISR_EXIT_BARRIER;   
}

void BOARD_ConfigTriggerSource(void)
{
    /* Configure SIM for ADC hardware trigger source selection */
    /* Use TRGMUX output as ADC pre-trigger trigger source */
    SIM->ADCOPT |= SIM_ADCOPT_ADC0TRGSEL(1U);
    SIM->ADCOPT |= SIM_ADCOPT_ADC0PRETRGSEL(1U);
    
    /* LPIT Channel 0 trigger input source: FTM0 */
    TRGMUX_SetTriggerSource(TRGMUX0, kTRGMUX_Lpit, kTRGMUX_TriggerInput0, kTRGMUX_SourceFtm0);
    /* ADC0 Channel A trigger input source: LPIT0CH0  */
    TRGMUX_SetTriggerSource(TRGMUX0, kTRGMUX_Adc0, kTRGMUX_TriggerInput0, kTRGMUX_SourceLpit0Ch0); 
  
    /* LPIT Channel 1 trigger input source: ADC0_COCOA */ 
    TRGMUX_SetTriggerSource(TRGMUX0, kTRGMUX_Lpit, kTRGMUX_TriggerInput1, kTRGMUX_SourceAdc0CocoA);
    /* ADC0 Channel B trigger input source: LPIT0CH1  */
    TRGMUX_SetTriggerSource(TRGMUX0, kTRGMUX_Adc0, kTRGMUX_TriggerInput1, kTRGMUX_SourceLpit0Ch1);
 
    /* LPIT Channel 2 trigger input source: ADC0_COCOB */ 
    TRGMUX_SetTriggerSource(TRGMUX0, kTRGMUX_Lpit, kTRGMUX_TriggerInput2, kTRGMUX_SourceAdc0CocoB); 
    /* ADC0 Channel C trigger input source: LPIT0CH2  */
    TRGMUX_SetTriggerSource(TRGMUX0, kTRGMUX_Adc0, kTRGMUX_TriggerInput2, kTRGMUX_SourceLpit0Ch2); 
    
    /* LPIT Channel 3 trigger input source: ADC0_COCOC */ 
    TRGMUX_SetTriggerSource(TRGMUX0, kTRGMUX_Lpit, kTRGMUX_TriggerInput3, kTRGMUX_SourceAdc0CocoC);
    /* ADC0 Channel D trigger input source: LPIT0CH3  */
    TRGMUX_SetTriggerSource(TRGMUX0, kTRGMUX_Adc0, kTRGMUX_TriggerInput3, kTRGMUX_SourceLpit0Ch3);
}

static void DEMO_InitLpitTrigger(void)
{
    /* Structure of initialize LPIT */
    lpit_config_t lpitConfig;
    lpit_chnl_params_t lpitChannelConfig;

    LPIT_GetDefaultConfig(&lpitConfig);
    
    /* Init lpit module */
    LPIT_Init(DEMO_LPIT_BASE, &lpitConfig);
    
    lpitChannelConfig.chainChannel          = false;
    lpitChannelConfig.enableReloadOnTrigger = false;
    /* Timer starts to decrement when rising edge on selected trigger is detected */    
    lpitChannelConfig.enableStartOnTrigger  = true;
    lpitChannelConfig.enableStopOnTimeout   = true;
    lpitChannelConfig.timerMode             = kLPIT_PeriodicCounter;
    /* Set default values for the trigger source */
    lpitChannelConfig.triggerSource = kLPIT_TriggerSource_External;

    /* Init lpit channel 0 */
    lpitChannelConfig.triggerSelect = DEMO_LPIT_USER_TIMER_CH;
    LPIT_SetupChannel(DEMO_LPIT_BASE, kLPIT_Chnl_0, &lpitChannelConfig);
    /* Init lpit channel 1 */
    lpitChannelConfig.triggerSelect = kLPIT_Trigger_TimerChn1;   
    LPIT_SetupChannel(DEMO_LPIT_BASE, kLPIT_Chnl_1, &lpitChannelConfig);
    /* Init lpit channel 2 */     
    lpitChannelConfig.triggerSelect = kLPIT_Trigger_TimerChn2;   
    LPIT_SetupChannel(DEMO_LPIT_BASE, kLPIT_Chnl_2, &lpitChannelConfig);    
    /* Init lpit channel 3 */     
    lpitChannelConfig.triggerSelect = kLPIT_Trigger_TimerChn3;   
    LPIT_SetupChannel(DEMO_LPIT_BASE, kLPIT_Chnl_3, &lpitChannelConfig);
     
    /* Set timer period for channel 0-3 */
    LPIT_SetTimerPeriod(DEMO_LPIT_BASE, kLPIT_Chnl_0, USEC_TO_COUNT(LPIT_TRIGGER_TIME, DEMO_LPIT_SOURCECLOCK));
    LPIT_SetTimerPeriod(DEMO_LPIT_BASE, kLPIT_Chnl_1, USEC_TO_COUNT(LPIT_TRIGGER_TIME, DEMO_LPIT_SOURCECLOCK));
    LPIT_SetTimerPeriod(DEMO_LPIT_BASE, kLPIT_Chnl_2, USEC_TO_COUNT(LPIT_TRIGGER_TIME, DEMO_LPIT_SOURCECLOCK));
    LPIT_SetTimerPeriod(DEMO_LPIT_BASE, kLPIT_Chnl_3, USEC_TO_COUNT(LPIT_TRIGGER_TIME, DEMO_LPIT_SOURCECLOCK));
    
    /* Enable timer interrupts for channel 0-3 */
    LPIT_EnableInterrupts(DEMO_LPIT_BASE, kLPIT_Channel0TimerInterruptEnable);
    LPIT_EnableInterrupts(DEMO_LPIT_BASE, kLPIT_Channel1TimerInterruptEnable);
    LPIT_EnableInterrupts(DEMO_LPIT_BASE, kLPIT_Channel2TimerInterruptEnable);
    LPIT_EnableInterrupts(DEMO_LPIT_BASE, kLPIT_Channel3TimerInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(LPIT0_IRQn);
    
    /* Start the timer */
    LPIT_StartTimer(DEMO_LPIT_BASE, kLPIT_Chnl_0);
    LPIT_StartTimer(DEMO_LPIT_BASE, kLPIT_Chnl_1);
    LPIT_StartTimer(DEMO_LPIT_BASE, kLPIT_Chnl_2);
    LPIT_StartTimer(DEMO_LPIT_BASE, kLPIT_Chnl_3);
}

static void DEMO_Init_FtmTrigger(void)
{
    ftm_config_t ftmConfigStruct;
    ftm_chnl_pwm_signal_param_t pwmParam;
    ftm_pwm_level_select_t pwmLevel = kFTM_LowTrue;

    /* Initialize FTM module. */
    FTM_GetDefaultConfig(&ftmConfigStruct);
    ftmConfigStruct.extTriggers =kFTM_InitTrigger; /* Enable to output the trigger. */
    FTM_Init(DEMO_FTM_BASE, &ftmConfigStruct);

    /* Configure ftm params with frequency 10kHz */
    pwmParam.chnlNumber = kFTM_Chnl_0;
    pwmParam.level = pwmLevel;
    pwmParam.dutyCyclePercent = 50U; /* Percent: 0 - 100. */
    pwmParam.firstEdgeDelayPercent = 0U;
    FTM_SetupPwm(DEMO_FTM_BASE, &pwmParam, 1U, kFTM_CenterAlignedPwm, DEMO_FTM_PWM_HZ, DEMO_FTM_COUNTER_CLOCK_HZ);
}


static void DEMO_InitAdc(void)
{
    adc12_config_t adc12ConfigStruct;
    adc12_channel_config_t adc12ChannelConfigStruct;
    
    /* Initialize ADC. */
    ADC12_GetDefaultConfig(&adc12ConfigStruct);
    adc12ConfigStruct.referenceVoltageSource     = kADC12_ReferenceVoltageSourceVref;
    adc12ConfigStruct.resolution                 = kADC12_Resolution12Bit;
    adc12ConfigStruct.enableContinuousConversion = false;
    ADC12_Init(DEMO_ADC12_BASEADDR, &adc12ConfigStruct);
    /* Set to hardware trigger mode. */
    ADC12_EnableHardwareTrigger(DEMO_ADC12_BASEADDR, true);

    /* Calibrate ADC. */
    if (kStatus_Success != ADC12_DoAutoCalibration(DEMO_ADC12_BASEADDR))
    {
        PRINTF("ADC calibration failed!\r\n");
    }

    adc12ChannelConfigStruct.channelNumber                        = DEMO_ADC12_USER_CHANNEL_A;
    adc12ChannelConfigStruct.enableInterruptOnConversionCompleted = false; /* Disable the interrupt. */
    ADC12_SetChannelConfig(DEMO_ADC12_BASEADDR, DEMO_ADC12_CHANNEL_GROUP_A, &adc12ChannelConfigStruct);
    
    adc12ChannelConfigStruct.channelNumber                        = DEMO_ADC12_USER_CHANNEL_B;
    adc12ChannelConfigStruct.enableInterruptOnConversionCompleted = false; /* Disable the interrupt. */
    ADC12_SetChannelConfig(DEMO_ADC12_BASEADDR, DEMO_ADC12_CHANNEL_GROUP_B, &adc12ChannelConfigStruct);

    adc12ChannelConfigStruct.channelNumber                        = DEMO_ADC12_USER_CHANNEL_C;
    adc12ChannelConfigStruct.enableInterruptOnConversionCompleted = false; /* Disable the interrupt. */
    ADC12_SetChannelConfig(DEMO_ADC12_BASEADDR, DEMO_ADC12_CHANNEL_GROUP_C, &adc12ChannelConfigStruct);

    adc12ChannelConfigStruct.channelNumber                        = DEMO_ADC12_USER_CHANNEL_D;
    adc12ChannelConfigStruct.enableInterruptOnConversionCompleted = true; /* Enable the interrupt. */
    ADC12_SetChannelConfig(DEMO_ADC12_BASEADDR, DEMO_ADC12_CHANNEL_GROUP_D, &adc12ChannelConfigStruct);    

    NVIC_EnableIRQ(DEMO_ADC12_IRQ_ID);
}

/*!
 * @brief Main function
 */
int main(void)
{
     BOARD_InitPins();
     BOARD_BootClockRUN();
     BOARD_InitDebugConsole();
     
     g_Adc12InterruptCounter = 0U;
     
     /* Define the init structure for the output pin */
     gpio_pin_config_t pin_config = {
        kGPIO_DigitalOutput,
        0,
     };

     /* Init output GPIO. */
     GPIO_PinInit(GPIOB, 8, &pin_config);
     GPIO_PinInit(GPIOA, 16, &pin_config);
     
     /* clear GPIO. */
     GPIO_PortClear(GPIOB, 1U << 8U);
     GPIO_PortClear(GPIOA, 1U << 16U);

     /* Set the source for the ADC module */
     CLOCK_SetIpSrc(kCLOCK_Adc0, kCLOCK_IpSrcLpFllAsync);  
     /* Set the source for the LPIT module */
     CLOCK_SetIpSrc(kCLOCK_Lpit0, kCLOCK_IpSrcLpFllAsync);

     /* Initialize ADC */
     DEMO_InitAdc();
     /* Configure the LPIT to trigger ADC periodically */
     DEMO_InitLpitTrigger();
     /* Initialize ftm */
     DEMO_Init_FtmTrigger();

     /* Set the hardware trigger source for the LPIT, ADC*/
     BOARD_ConfigTriggerSource(); 

     PRINTF("\r\nFTM0 trigger LPIT0, LPIT0 trigger four ADC channels conversion\r\n");    
          
     FTM_StartTimer(DEMO_FTM_BASE, DEMO_FTM_COUNTER_CLOCK_SOURCE);
  
    while (1)
    {        
        g_Adc12ConversionCompletedFlag = false;

        while (!g_Adc12ConversionCompletedFlag)
        {
        }

//        PRINTF("\r\nADC12 Interrupt Counter: %d\r\n", g_Adc12InterruptCounter);
        PRINTF("ADC12 Conversion Completed, result value: %d,%d,%d,%d\r\n", g_Adc12ConvValue_0,g_Adc12ConvValue_1,g_Adc12ConvValue_2,g_Adc12ConvValue_3);
        g_Adc12ConvValue_0 = 0;
        g_Adc12ConvValue_1 = 0;
        g_Adc12ConvValue_2 = 0;
        g_Adc12ConvValue_3 = 0;
        while(1);
     
    }
}
