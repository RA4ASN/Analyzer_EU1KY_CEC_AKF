/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GT911_H
#define __GT911_H

#ifdef __cplusplus
extern "C" {
#endif

#define TS_MONO_TOUCH_SUPPORTED     1

/* Set Multi-touch as supported */
#if !defined(TS_MONO_TOUCH_SUPPORTED)
#define TS_MULTI_TOUCH_SUPPORTED        1
#endif /* TS_MONO_TOUCH_SUPPORTED */

/* Includes ------------------------------------------------------------------*/
#include "../Common/ts.h"

/* Macros --------------------------------------------------------------------*/

#if defined(gt911_ENABLE_ASSERT)
/* Assert activated */
#define GT911_ASSERT(__condition__)      do { if(__condition__) \
                                               {  \
                                                 while(1);  \
                                               } \
                                          }while(0)
#else
/* Assert not activated : macro has no effect */
#define GT911_ASSERT(__condition__)    do { if(__condition__) \
                                             {  \
                                                ;  \
                                             } \
                                            }while(0)
#endif /* gt911_ENABLE_ASSERT == 1 */


typedef struct
{
  uint8_t i2cInitialized;

  /* field holding the current number of simultaneous active touches */
  uint8_t currActiveTouchNb;

  /* field holding the touch index currently managed */
  uint8_t currActiveTouchIdx;

} gt911_handle_TypeDef;



#define GOODIX_OK   0
#define GT911_ID	0x39313100	// "911\0"

// 0x28/0x29 (0x14 7bit)
#define GOODIX_I2C_ADDR_28  0x28
// 0xBA/0xBB (0x5D 7bit)
#define GOODIX_I2C_ADDR_BA  0xBA

#define GOODIX_MAX_HEIGHT   4096
#define GOODIX_MAX_WIDTH    4096
#define GOODIX_INT_TRIGGER    1
#define GOODIX_CONTACT_SIZE   8
#define GOODIX_MAX_CONTACTS   10

#define GOODIX_CONFIG_MAX_LENGTH  240
#define GOODIX_CONFIG_911_LENGTH  186
#define GOODIX_CONFIG_967_LENGTH  228

/* Register defines */
#define GT_REG_CMD  0x8040

#define GT_REG_CFG  0x8047
#define GT_REG_DATA 0x8140


// Write only registers
#define GOODIX_REG_COMMAND        0x8040
#define GOODIX_REG_LED_CONTROL    0x8041
#define GOODIX_REG_PROXIMITY_EN   0x8042

// Read/write registers
// The version number of the configuration file
#define GOODIX_REG_CONFIG_DATA  0x8047
// X output maximum value (LSB 2 bytes)
#define GOODIX_REG_MAX_X        0x8048
// Y output maximum value (LSB 2 bytes)
#define GOODIX_REG_MAX_Y        0x804A
// Maximum number of output contacts: 1~5 (4 bit value 3:0, 7:4 is reserved)
#define GOODIX_REG_MAX_TOUCH    0x804C

// Module switch 1
// 7:6 Reserved, 5:4 Stretch rank, 3 X2Y, 2 SITO (Single sided ITO touch screen), 1:0 INT Trigger mode */
#define GOODIX_REG_MOD_SW1      0x804D
// Module switch 2
// 7:1 Reserved, 0 Touch key */
#define GOODIX_REG_MOD_SW2      0x804E

// Number of debuffs fingers press/release
#define GOODIX_REG_SHAKE_CNT    0x804F

// ReadOnly registers (device and coordinates info)
// Product ID (LSB 4 bytes, GT9110: 0x06 0x00 0x00 0x09)
#define GOODIX_REG_ID           0x8140
// Firmware version (LSB 2 bytes)
#define GOODIX_REG_FW_VER       0x8144

// Current output X resolution (LSB 2 bytes)
#define GOODIX_READ_X_RES       0x8146
// Current output Y resolution (LSB 2 bytes)
#define GOODIX_READ_Y_RES       0x8148
// Module vendor ID
#define GOODIX_READ_VENDOR_ID   0x814A

#define GOODIX_READ_COORD_ADDR	0x814E

/* Commands for REG_COMMAND */
//0: read coordinate state
#define GOODIX_CMD_READ         0x00
// 1: difference value original value
#define GOODIX_CMD_DIFFVAL      0x01
// 2: software reset
#define GOODIX_CMD_SOFTRESET    0x02
// 3: Baseline update
#define GOODIX_CMD_BASEUPDATE   0x03
// 4: Benchmark calibration
#define GOODIX_CMD_CALIBRATE    0x04
// 5: Off screen (send other invalid)
#define GOODIX_CMD_SCREEN_OFF   0x05

/* When data needs to be sent, the host sends command 0x21 to GT9x,
 * enabling GT911 to enter "Approach mode" and work as a transmitting terminal */
#define GOODIX_CMD_HOTKNOT_TX   0x21

#define RESOLUTION_LOC    1
#define MAX_CONTACTS_LOC  5
#define TRIGGER_LOC 6


void gt911_Init(uint16_t DeviceAddr);
void gt911_Reset(uint16_t DeviceAddr);
uint32_t gt911_ReadID(uint16_t DeviceAddr);
void gt911_TS_Start(uint16_t DeviceAddr);
uint8_t gt911_TS_DetectTouch(uint16_t DeviceAddr);
void gt911_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y);
void gt911_TS_EnableIT(uint16_t DeviceAddr);
void gt911_TS_DisableIT(uint16_t DeviceAddr);
uint8_t gt911_TS_ITStatus (uint16_t DeviceAddr);
void gt911_TS_ClearIT (uint16_t DeviceAddr);

/**** NEW FEATURES enabled when Multi-touch support is enabled ****/

#if (TS_MULTI_TOUCH_SUPPORTED == 1)
void gt911_TS_GetGestureID(uint16_t DeviceAddr, uint32_t * pGestureId);
void gt911_TS_GetTouchInfo(uint16_t   DeviceAddr,
                            uint32_t   touchIdx,
                            uint32_t * pWeight,
                            uint32_t * pArea,
                            uint32_t * pEvent);

#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */


extern void     TS_IO_Init(void);
extern void    TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
extern uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg);
extern void    TS_IO_Delay(uint32_t Delay);
extern TS_DrvTypeDef gt911_ts_drv;

#ifdef __cplusplus
}
#endif
#endif /* __GT911_H */
