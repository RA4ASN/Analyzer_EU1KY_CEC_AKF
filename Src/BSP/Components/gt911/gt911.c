/* Includes ------------------------------------------------------------------*/
#include "gt911.h"

static uint8_t gt911_addr = 0, is_touched = 0;
static uint8_t buf_raw [6];

/* Touch screen driver structure initialization */
TS_DrvTypeDef gt911_ts_drv =
{
  gt911_Init,
  gt911_ReadID,
  gt911_Reset,

  gt911_TS_Start,
  gt911_TS_DetectTouch,
  gt911_TS_GetXY,

  gt911_TS_EnableIT,
  gt911_TS_ClearIT,
  gt911_TS_ITStatus,
  gt911_TS_DisableIT

};

/* Global gt911 handle */
static gt911_handle_TypeDef gt911_handle = { 0, 0, 0 };

static uint8_t gt911_Get_I2C_InitializedStatus(void);
static void gt911_I2C_InitializeIfRequired(void);
static uint32_t gt911_TS_Configure(uint16_t DeviceAddr);

void TS_IO_Write_buf(uint8_t Addr, uint8_t * buf, uint8_t len);
void TS_IO_Read_buf(uint8_t Addr, uint8_t * buf, uint8_t len);

void gt911_set_reg(uint8_t addr, uint16_t reg)
{
	uint8_t buf[2] = { (reg >> 8), (reg & 0xFF), };
	TS_IO_Write_buf(addr, buf, 2);
}

void gt911_read(uint8_t addr, uint16_t reg, uint8_t * buf, uint8_t len)
{
	gt911_set_reg(addr, reg);
	TS_IO_Read_buf(addr, buf, len);
}

void gt911_write_reg(uint8_t addr, uint16_t reg, uint8_t val)
{
	uint8_t buf[3] = { (reg >> 8), (reg & 0xFF), val, };
	TS_IO_Write_buf(addr, buf, 3);
}

void gt911_Init(uint16_t DeviceAddr)
{
  TS_IO_Delay(200);
  gt911_I2C_InitializeIfRequired();
}

/**
  * @brief  Software Reset the gt911.
  *         @note : Not applicable to FT5336.
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT5336).
  * @retval None
  */
void gt911_Reset(uint16_t DeviceAddr)
{
  /* Do nothing */
  /* No software reset sequence available in FT5336 IC */
}


uint32_t gt911_ReadID(uint16_t DeviceAddr)
{
    uint32_t res;
    uint8_t bufr [4];
    gt911_I2C_InitializeIfRequired();
    gt911_read(DeviceAddr, GOODIX_REG_ID, bufr, 4);
    res = bufr [3] | (bufr [2] << 8) | (bufr [1] << 16) | (bufr [0] << 24);
    return (res);
}

uint8_t gt911_calcChecksum(uint8_t * buf, uint8_t len)
{
	uint8_t ccsum = 0;
	for (uint8_t i = 0; i < len; i++) {
		ccsum += buf [i];
	}
	//ccsum %= 256;
	ccsum = (~ ccsum) + 1;
	return ccsum & 0xFF;
}

uint8_t gt911_readChecksum(void)
{
	uint16_t aStart = GT_REG_CFG;
	uint16_t aStop = 0x80FE;
	uint8_t len = aStop - aStart + 1;
	uint8_t buf [len];

	gt911_read(gt911_addr, aStart, buf, len);
	return gt911_calcChecksum(buf, len);
}

void gt911_fwResolution(uint16_t maxX, uint16_t maxY)
{
	uint8_t len = GOODIX_CONFIG_911_LENGTH;
	uint16_t pos = 0;
	uint8_t cfg [len];
	gt911_read(gt911_addr, GT_REG_CFG, cfg, GOODIX_CONFIG_911_LENGTH);

	cfg [1] = (maxX & 0xff);
	cfg [2] = (maxX >> 8);
	cfg [3] = (maxY & 0xff);
	cfg [4] = (maxY >> 8);
	cfg [15] = 0xf;			// период опроса 15 + 5 мс
	cfg [len - 2] = gt911_calcChecksum(cfg, len - 2);
	cfg [len - 1] = 1;

	while (pos < len) {
		gt911_write_reg(gt911_addr, GT_REG_CFG + pos, cfg [pos]);
		pos ++;
	}
}

void gt911_TS_Start(uint16_t DeviceAddr)
{
    gt911_addr = DeviceAddr;

    gt911_fwResolution(480, 272);
    gt911_write_reg(gt911_addr, GOODIX_READ_COORD_ADDR, 0);
}

/**
  * @brief  Return if there is touches detected or not.
  *         Try to detect new touches and forget the old ones (reset internal global
  *         variables).
  * @param  DeviceAddr: Device address on communication Bus.
  * @retval : Number of active touches detected (can be 0, 1 or 2).
  */
uint8_t gt911_TS_DetectTouch(uint16_t DeviceAddr)
{
    gt911_read(gt911_addr, GOODIX_READ_COORD_ADDR, buf_raw, 6);
    gt911_write_reg(gt911_addr, GOODIX_READ_COORD_ADDR, 0);
	uint8_t buf_state = buf_raw [0] >> 7;

	if (! buf_state)
		return 0;

	uint8_t touch_num = buf_raw [0] & 0x0F;
	if (touch_num > 0)
        return 1;
    else
        return 0;
}


void gt911_TS_GetXY(uint16_t DeviceAddr, uint16_t * X, uint16_t * Y)
{
    uint16_t x = (buf_raw [2] << 0) | (buf_raw [3] << 8);
    uint16_t y = (buf_raw [4] << 0) | (buf_raw [5] << 8);
    * X = x;
    * Y = y;
    //DBG_Printf("%d, %d\n", x, y);
}

void gt911_TS_EnableIT(uint16_t DeviceAddr)
{
}

void gt911_TS_DisableIT(uint16_t DeviceAddr)
{
}

uint8_t gt911_TS_ITStatus(uint16_t DeviceAddr)
{
  /* Always return 0 as feature not applicable to FT5336 */
  return 0;
}

void gt911_TS_ClearIT(uint16_t DeviceAddr)
{
}

/**** NEW FEATURES enabled when Multi-touch support is enabled ****/

#if (TS_MULTI_TOUCH_SUPPORTED == 1)

void gt911_TS_GetGestureID(uint16_t DeviceAddr, uint32_t * pGestureId)
{
}

void gt911_TS_GetTouchInfo(uint16_t   DeviceAddr,
                            uint32_t   touchIdx,
                            uint32_t * pWeight,
                            uint32_t * pArea,
                            uint32_t * pEvent)
{
}

#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

static uint8_t gt911_Get_I2C_InitializedStatus(void)
{
  return(gt911_handle.i2cInitialized);
}

static void gt911_I2C_InitializeIfRequired(void)
{
  if(gt911_Get_I2C_InitializedStatus() == 0)
  {
    TS_IO_Init();
    gt911_handle.i2cInitialized = 1;
  }
}

static uint32_t gt911_TS_Configure(uint16_t DeviceAddr)
{
  uint32_t status = GOODIX_OK;
  return(status);
}
