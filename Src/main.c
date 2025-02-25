#include "main.h"
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "font.h"
#include "touch.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "config.h"
#include "crash.h"
#include "si5351.h"
#include "dsp.h"
#include "mainwnd.h"
#include "aauart.h"
#include "custom_spi2.h"
#include "gen.h"
#include "keyboard.h"
#include "bitmaps/bitmaps.h"
#include "screenshot.h"
#include "DS3231.h"
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_def.h"
#include "stm32746g_discovery.h"
#include "stm32f7xx_hal_adc.h"

#include "gpio_control.h"
#include "shell.h"

static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);
static void MPU_Config(void);
extern uint32_t date, time;
extern uint32_t RTCpresent;
volatile uint32_t main_sleep_timer = 0;
volatile uint32_t autosleep_timer = 0xFFFFFFFFul;

volatile uint32_t secondsCounter;
uint8_t NotSleepMode = 0;

void Sleep(uint32_t nms)
{
    if (CFG_PROTO_NANOVNA == CFG_GetParam(CFG_PARAM_SEREMUL))
    { // NanoVNA protocol runs in any window
        shell_rx_proc();
    }

    uint32_t ts = CFG_GetParam(CFG_PARAM_LOWPWR_TIME);
    if (ts != 0)
    {
        if (autosleep_timer == 0 &&
            !LCD_IsOff())
        {
            BSP_LCD_DisplayOff();
        }
    }

    if (nms == 0)
        return;

    if (nms == 1)
    {
        HAL_Delay(1);
        return;
    }

    // Enter sleep mode. The device will wake up on interrupts, and go sleep again
    // after interrupt routine exit. The SysTick_Handler interrupt routine will
    // leave device running when the main_sleep_timer downcount reaches zero,
    // until then the device remains in Sleep state with only interrupts running.
    main_sleep_timer = nms;
    __disable_irq();
    HAL_PWR_EnableSleepOnExit();
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    __enable_irq();
    __NOP();
    while (main_sleep_timer)
    {
        __WFI();
        if (CFG_PROTO_NANOVNA == CFG_GetParam(CFG_PARAM_SEREMUL))
        { // NanoVNA protocol runs in any window
            shell_rx_proc();
        }
    }
}

//SDFatFs must be aligned to 32 bytes in order the buffer to be valid for DCache operataions
__attribute__((aligned(32))) FATFS SDFatFs; // File system object for SD card logical drive
char SDPath[4];                             // SD card logical drive path

//extern ADC_HandleTypeDef Adc3Handle;

void ADC3_Init(void)
{
    UB_ADC3_SINGLE_Init();
}

static uint32_t date1, time1;
static uint8_t second, second1;
static short AMPM, AMPM1;
extern int BeepIsActive, SWRTone;
extern uint8_t AUDIO1;

int main(void)
{
    MPU_Config();
    CPU_CACHE_Enable();
    HAL_Init();
    SystemClock_Config();
    setvbuf(stdout, NULL, _IONBF, 0); // disable buffering on stdout
    BSP_LED_Init(LED1);
    LCD_Init();

    InitTimer2_4_5(); // WK
    ADC3_Init();      // WK
    SPI2_Init();
    BeepIsActive = SWRTone = 0;
    Sleep(300);
    TOUCH_Init();
    setup_GPIO();

    //Set GPIO PTT
    GPIO_PTT_Setup();

    AUDIO1 = 0;
    //Mount SD card
    if (FATFS_LinkDriver(&SD_Driver, SDPath) != 0)
        CRASH("FATFS_LinkDriver failed");
    if (f_mount(&SDFatFs, (TCHAR const *)SDPath, 0) != FR_OK)
        CRASH("f_mount failed");
    CFG_Init(); //Load configuration
    if (CFG_GetParam(CFG_PARAM_ORIENTATION) != 0)
        LCD_Set_Orientation(1); //   *** DH1AKF 07.10.2020
    CFG_Flush();
    GEN_Init(); //Initialize frequency synthesizer (only after CFG_Init())

    DSP_Init(); //Initialize DSP module. Also loads calibration files inside.

    AAUART_Init(); //Initialize remote control protocol handler

    CAMERA_IO_Init(); // I2C connection
    Sleep(50);
#ifdef WITH_RTC
    getTime(&time, &second, &AMPM, 0);
    getDate(&date);
#endif /* WITH_RTC */

    shell_init("ch"); // Initialize shell with ChibiOS-like command prompt (for NanoVNA protocol)

#ifndef NO_LOGO_NO_WAIT
#ifndef _DEBUG_UART
    if (ShowLogo() == -1)                                               // no logo.bmp or logo.png file found:
        LCD_DrawBitmap(LCD_MakePoint(90, 24), logo_bmp, logo_bmp_size); // show original logo
#endif
    int WaitTime = 10 * CFG_GetParam(CFG_PARAM_ShowLogoTime); // to show logo n seconds or until touch
    while (--WaitTime >= 0)
    { // 0 causes immediate break
        Sleep(100);
        if (TOUCH_IsPressed())
            break;
    }
#endif /* NO_LOGO_NO_WAIT */

    //TODO : Remove comment before Release / by KD8CEC
    //Sleep (3000);
    autosleep_timer = CFG_GetParam(CFG_PARAM_LOWPWR_TIME);
    if (autosleep_timer != 0 && autosleep_timer < 10000)
    {
        //Disable too low value for autosleep timer. Minimal value is 10 seconds.
        CFG_SetParam(CFG_PARAM_LOWPWR_TIME, 0);
        autosleep_timer = 0;
    }

#ifndef _DEBUG_UART
    Sleep(1000);
#endif
    ColourSelection = CFG_GetParam(CFG_PARAM_Daylight);
    FatLines = true;
    if (0 == CFG_GetParam(CFG_PARAM_Fatlines))
        FatLines = false;
    BeepOn1 = CFG_GetParam(CFG_PARAM_BeepOn);

 #ifdef WITH_RTC
    getTime(&time1, &second1, &AMPM1, 0);
    // second=second1;//      ************************** TEST without RTC
    getDate(&date1);
    if (second == second1)
    { // realtime clock is not present
        RTCpresent = 0;
        date = CFG_GetParam(CFG_PARAM_Date);
        if (date == 0)
            NoDate = 1;
        time = CFG_GetParam(CFG_PARAM_Time);
    }
    else
        RTCpresent = 1;
#else
    NoDate = 1;
    RTCpresent = 0;
#endif /* WITH_RTC */
    //  RTCpresent=false;//   ************************** TEST
    //Run main window function

    MainWnd(); //Never returns

    return 0;
}

/**
  * @brief This function provides accurate delay (in milliseconds) based
  *        on SysTick counter flag.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */

void HAL_Delay(__IO uint32_t Delay)
{
    while (Delay)
    {
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        {
            Delay--;
        }
    }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    HAL_StatusTypeDef ret = HAL_OK;

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 432;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;

    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if (ret != HAL_OK)
    {
        while (1)
        {
            ;
        }
    }

    /* Activate the OverDrive to reach the 216 MHz Frequency */
    ret = HAL_PWREx_EnableOverDrive();
    if (ret != HAL_OK)
    {
        while (1)
        {
            ;
        }
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
    if (ret != HAL_OK)
    {
        while (1)
        {
            ;
        }
    }
}

/**
  * @brief  Configure the MPU attributes as Write Through for SRAM1/2.
  * @note   The Base Address is 0x20010000 since this memory interface is the AXI.
  *         The Region Size is 256KB, it is related to SRAM1 and SRAM2  memory size.
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct;

    /* Disable the MPU */
    HAL_MPU_Disable();

    /* Configure the MPU attributes as WT for SRAM */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x20010000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes for SDRAM */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = SDRAM_DEVICE_ADDR;
    MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Enable the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

//CPU L1-Cache enable.
static void CPU_CACHE_Enable(void)
{
    /* Enable branch prediction */
    SCB->CCR |= (1 << 18);
    __DSB();

    /* Enable I-Cache */
    SCB_InvalidateICache();
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_InvalidateDCache();
    SCB_EnableDCache();
}

void CRASH(const char *text)
{
    if (LCD_IsOff())
        BSP_LCD_DisplayOn();
    FONT_Write(FONT_FRAN, LCD_RED, LCD_BLACK, 0, 0, text);
    FONT_Write(FONT_FRAN, LCD_RED, LCD_BLACK, 0, 14, "SYSTEM HALTED ");
    for (;;)
        ;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    static uint32_t in_assert = 0;
    if (in_assert)
        return;
    in_assert = 1;
    CRASHF("ASSERT @ %s:%d", file, line);
}
#endif
