#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

//==============================================================
// KD8CEC'S DEBUG CODE
//DEBUGER OPTION
//must remark below 1line before release
//#define _DEBUG_UART
#ifdef _DEBUG_UART
void DBGUART_Init(void);
void DBG_Str(const char *str);
int DBG_Printf(const char *fmt, ...);
#endif
//==============================================================

#include "LCD.h"
#define AAVERSION "3.0d"      //Must be 4 characters
#define AAVERSION_CECV "1.04" //Must be 4 characters

/*bool DatumDDMMYYYY = true; */

#define SI5351_USE_I2C3	1       // перенос si5351 на i2c3 в случае неисправности портов i2c1
#define NO_LOGO_NO_WAIT 1       // ускорение запуска прибора за счет отключения показа логотипа и ожидания нажатий
//#define WITH_RTC        1     // если физически отсутствует rtc, запуск прибора происходит дольше из-за попыток олбращения к rtc
#define TOUCH_FT5336    1       // контроллер тачскрина штатный - FT5336
//#define TOUCH_GT911     1     // контроллер тачскрина Goodix GT911

#define DatumDDMMYYYY false

//#define BAND_FMIN 500000ul    //BAND_FMIN must be multiple 100000
// ** WK **:
#define BAND_FMIN 100000ul //BAND_FMIN must be multiple 100000

//#define MAX_BAND_FREQ  450000000ul
//#define MAX_BAND_FREQ  600000000ul
//#define MAX_BAND_FREQ  890000000ul
#define MAX_BAND_FREQ (unsigned long)1450000000ul

#if (BAND_FMIN % 100000) != 0
#error "Incorrect band limit settings"
#endif

typedef enum
{
    CFG_SYNTH_SI5351 = 0,
    CFG_SYNTH_ADF4350 = 1,
    CFG_SYNTH_ADF4351 = 2,
    CFG_SYNTH_SI5338A = 3,
} CFG_SYNTH_TYPE_t;

typedef enum
{
    CFG_S1P_TYPE_S_MA = 0,
    CFG_S1P_TYPE_S_RI = 1,
    CFG_S1P_TYPE_Z_RI = 2,
} CFG_S1P_TYPE_t;

typedef enum
{
    CFG_PROTO_AA600 = 0,
    CFG_PROTO_LINSMITH = 1,
    CFG_PROTO_NANOVNA = 2
} CFG_SEREMUL_TYPE_t;

typedef enum
{
    CFG_PARAM_VERSION,              //4 characters of version string
    CFG_PARAM_PAN_F1,               //Initial frequency for panoramic window
    CFG_PARAM_PAN_SPAN,             //Span for panoramic window
    CFG_PARAM_MEAS_F,               //Measurement window frequency
    CFG_PARAM_SYNTH_TYPE,           //Synthesizer type used: 0 - Si5351a
    CFG_PARAM_SI5351_XTAL_FREQ,     //Si5351a Xtal frequency, Hz
    CFG_PARAM_SI5351_BUS_BASE_ADDR, //Si5351a I2C bus base address
    CFG_PARAM_SI5351_CORR,          //Si5351a Xtal correction (signed, int16_t)
    CFG_PARAM_OSL_SELECTED,         //Selected OSL file
    CFG_PARAM_R0,                   //Base R0 for G measurements
    CFG_PARAM_OSL_RLOAD,            //RLOAD for OSL calibration
    CFG_PARAM_OSL_RSHORT,           //RSHORT for OSL calibration
    CFG_PARAM_OSL_ROPEN,            //ROPEN for OSL calibration
    CFG_PARAM_OSL_NSCANS,           //Number of scans to average during OSL
    CFG_PARAM_MEAS_NSCANS,          //Number of scans to average in measurement window
    CFG_PARAM_PAN_NSCANS,           //Number of scans to average in panoramic window
    CFG_PARAM_LIN_ATTENUATION,      //Linear audio input attenuation, dB
    CFG_PARAM_F_LO_DIV_BY_TWO,      //LO frequency is divided by two in quadrature mixer
    CFG_PARAM_GEN_F,                //Frequency for generator window, Hz
    CFG_PARAM_PAN_CENTER_F,         //Way of setting panoramic window. 0: F0+bandspan, 1: Fcenter +/- Bandspan/2
    CFG_PARAM_BRIDGE_RM,            //Value of measurement resistor in bridge, float32
    CFG_PARAM_BRIDGE_RADD,          //Value of series resistor in bridge, float32
    CFG_PARAM_BRIDGE_RLOAD,         //Value of load resistor in bridge, float32
    CFG_PARAM_COM_PORT,             //Serial (COM) port to be used: COM1 or COM2
    CFG_PARAM_COM_SPEED,            //Serial (COM) port speed, bps
    CFG_PARAM_LOWPWR_TIME,          //Time in milliseconds after which to lower power consumption mode (0 - disabled)
    CFG_PARAM_3RD_HARMONIC_ENABLED, //Enable setting frequency on 3rd harmonic (1) above BAND_FMAX, or disabe (0)
    CFG_PARAM_S11_SHOW,             //Show S11 graph in the panoramic window
    CFG_PARAM_S1P_TYPE,             //Type of Touchstone S1P file saved with panoramic screenshot
    CFG_PARAM_SHOW_HIDDEN,          //Show hidden options in configuration menu
    CFG_PARAM_SCREENSHOT_FORMAT,    //If 0, use BMP format for screenshots, otherwise use PNG
    CFG_PARAM_BAND_FMIN,            //Minimum frequency of the device's working band, Hz
    CFG_PARAM_BAND_FMAX,            //Maximum frequency of the device's working band, Hz
    CFG_PARAM_SI5351_MAX_FREQ,      //Maximum frequency that Si5351 can output, Hz (160 MHz by default, but some samples can reliably provide 200 MHz)
    CFG_PARAM_SI5351_CAPS,          //Si5351a crystal capacitors setting
    CFG_PARAM_TDR_VF,               //Velocity factor for TDR, % (1..100)
    CFG_PARAM_MULTI_F1,             //Frequency 1 for multi SWR window
    CFG_PARAM_MULTI_F2,             //Frequency 2 for multi SWR window
    CFG_PARAM_MULTI_F3,             //Frequency 3 for multi SWR window
    CFG_PARAM_MULTI_F4,             //Frequency 4 for multi SWR window
    CFG_PARAM_MULTI_F5,             //Frequency 5 for multi SWR window
    CFG_PARAM_MULTI_BW1,            //Bandwidth 1 for multi SWR window
    CFG_PARAM_MULTI_BW2,            //Bandwidth 2 for multi SWR window
    CFG_PARAM_MULTI_BW3,            //Bandwidth 3 for multi SWR window
    CFG_PARAM_MULTI_BW4,            //Bandwidth 4 for multi SWR window
    CFG_PARAM_MULTI_BW5,            //Bandwidth 5 for multi SWR window
    CFG_PARAM_Volt_max,             //Maximum Voltage (with full Accu)
    CFG_PARAM_Volt_max_Display,     //Maximum displayed Voltage (with full Accu) 0 = Voltage Display off
    CFG_PARAM_Volt_max_Factor,      //Factor for correct Max Voltage (Accu has to be loaded)
    CFG_PARAM_Volt_min_Display,     //Minimum Voltage (Accu must be loaded immediately)
    CFG_PARAM_Daylight,             // Daylight (1)  Inhouse  (0)
    CFG_PARAM_Fatlines,             // Fat Lines (1) Thin Lines (0)
    CFG_PARAM_BeepOn,               // Beep on (1) Beep off (0)
    CFG_PARAM_Date,                 // Date yyyymmdd
    CFG_PARAM_Time,                 // Time hhmm

    //Added by KD8CEC
    CFG_PARAM_LC_OSL_SELECTED, //OSL File for LC Meter
    CFG_PARAM_LC_INDEX,        //Default Frequency Index for LC Meter

    CFG_PARAM_S21_F1,   //S21 Gain Frequency
    CFG_PARAM_S21_SPAN, //S21 Gain Span

    //Extend Configuration for Multi SWR window (Frequency Group)
    CFG_PARAM_MULTI_ANT, //Frequency group (as Antenna Index 0 ~ 4)

    //Antenna 2
    CFG_PARAM_MULTI_2F1,  //Frequency 1 for Antenna 2 in multi SWR window
    CFG_PARAM_MULTI_2F2,  //Frequency 2 for Antenna 2 in multi SWR window
    CFG_PARAM_MULTI_2F3,  //Frequency 3 for Antenna 2 in multi SWR window
    CFG_PARAM_MULTI_2F4,  //Frequency 4 for Antenna 2 in multi SWR window
    CFG_PARAM_MULTI_2F5,  //Frequency 5 for Antenna 2 in multi SWR window
    CFG_PARAM_MULTI_2BW1, //Bandwidth 1 for Antenna 2 in multi SWR window
    CFG_PARAM_MULTI_2BW2, //Bandwidth 2 for Antenna 2 in multi SWR window
    CFG_PARAM_MULTI_2BW3, //Bandwidth 3 for Antenna 2 in multi SWR window
    CFG_PARAM_MULTI_2BW4, //Bandwidth 4 for Antenna 2 in multi SWR window
    CFG_PARAM_MULTI_2BW5, //Bandwidth 5 for Antenna 2 in multi SWR window

    //Antenna 3
    CFG_PARAM_MULTI_3F1,  //Frequency 1 for Antenna 3 in multi SWR window
    CFG_PARAM_MULTI_3F2,  //Frequency 2 for Antenna 3 in multi SWR window
    CFG_PARAM_MULTI_3F3,  //Frequency 3 for Antenna 3 in multi SWR window
    CFG_PARAM_MULTI_3F4,  //Frequency 4 for Antenna 3 in multi SWR window
    CFG_PARAM_MULTI_3F5,  //Frequency 5 for Antenna 3 in multi SWR window
    CFG_PARAM_MULTI_3BW1, //Bandwidth 1 for Antenna 3 in multi SWR window
    CFG_PARAM_MULTI_3BW2, //Bandwidth 2 for Antenna 3 in multi SWR window
    CFG_PARAM_MULTI_3BW3, //Bandwidth 3 for Antenna 3 in multi SWR window
    CFG_PARAM_MULTI_3BW4, //Bandwidth 4 for Antenna 3 in multi SWR window
    CFG_PARAM_MULTI_3BW5, //Bandwidth 5 for Antenna 3 in multi SWR window

    //Antenna 4
    CFG_PARAM_MULTI_4F1,  //Frequency 1 for Antenna 4 in multi SWR window
    CFG_PARAM_MULTI_4F2,  //Frequency 2 for Antenna 4 in multi SWR window
    CFG_PARAM_MULTI_4F3,  //Frequency 3 for Antenna 4 in multi SWR window
    CFG_PARAM_MULTI_4F4,  //Frequency 4 for Antenna 4 in multi SWR window
    CFG_PARAM_MULTI_4F5,  //Frequency 5 for Antenna 4 in multi SWR window
    CFG_PARAM_MULTI_4BW1, //Bandwidth 1 for Antenna 4 in multi SWR window
    CFG_PARAM_MULTI_4BW2, //Bandwidth 2 for Antenna 4 in multi SWR window
    CFG_PARAM_MULTI_4BW3, //Bandwidth 3 for Antenna 4 in multi SWR window
    CFG_PARAM_MULTI_4BW4, //Bandwidth 4 for Antenna 4 in multi SWR window
    CFG_PARAM_MULTI_4BW5, //Bandwidth 5 for Antenna 4 in multi SWR window

    //Antenna 5
    CFG_PARAM_MULTI_5F1,  //Frequency 1 for Antenna 5 in multi SWR window
    CFG_PARAM_MULTI_5F2,  //Frequency 2 for Antenna 5 in multi SWR window
    CFG_PARAM_MULTI_5F3,  //Frequency 3 for Antenna 5 in multi SWR window
    CFG_PARAM_MULTI_5F4,  //Frequency 4 for Antenna 5 in multi SWR window
    CFG_PARAM_MULTI_5F5,  //Frequency 5 for Antenna 5 in multi SWR window
    CFG_PARAM_MULTI_5BW1, //Bandwidth 1 for Antenna 5 in multi SWR window
    CFG_PARAM_MULTI_5BW2, //Bandwidth 2 for Antenna 5 in multi SWR window
    CFG_PARAM_MULTI_5BW3, //Bandwidth 3 for Antenna 5 in multi SWR window
    CFG_PARAM_MULTI_5BW4, //Bandwidth 4 for Antenna 5 in multi SWR window
    CFG_PARAM_MULTI_5BW5, //Bandwidth 5 for Antenna 5 in multi SWR window

    //AUTO SPEED
    CFG_PARAM_PAN_AUTOSPEED, //Panorama Auto Speed
    CFG_PARAM_S21_AUTOSPEED, //Panorama Auto Speed

    // added by ve7it to support multiple serial remote control protocols
    CFG_PARAM_SEREMUL,
    //  add by DG2DRF to support Bluetooth
    CFG_PARAM_BT_SPEED,
    // add by DG2DRF to support Region change
    CFG_PARAM_REGION,
    // added by DH1AKF (05.10.2020)
    CFG_PARAM_ORIENTATION,
    CFG_PARAM_LOGLOG,
    CFG_PARAM_CURSOR,
    CFG_PARAM_ATTENUATOR,   // DH1AKF 03.11.2020
    CFG_PARAM_ShowLogoTime, // DH1AKF 16.11.2020
    //For count of params ---------------------
    CFG_NUM_PARAMS
} CFG_PARAM_t;

extern const char *g_cfg_osldir;
extern const char *g_aa_dir;

extern uint8_t ColourSelection;
extern bool FatLines;
extern int BeepOn1;
extern uint32_t BackGrColor;
extern uint32_t CurvColor;
extern uint32_t TextColor;
extern uint32_t Color1;
extern uint32_t Color2;
extern uint32_t Color3;
extern uint32_t Color4;
extern void SetColours();

void CFG_Init(void);
uint32_t CFG_GetParam(CFG_PARAM_t param);
void CFG_SetParam(CFG_PARAM_t param, uint32_t value);
void CFG_Flush(void);
void CFG_ParamWnd(void);

#endif // _CONFIG_H_
