/*
 * @brief UDA1380 Audio codec interface file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */
#include <stdint.h>
#include "uda1380.h"

#include "board.h"

/** @ingroup BOARD_COMMON_UDA1380
 * @{
 */
/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Defalut UDA values
 * Important: Remember registers are auto incremented after
 * each write.
 *
 * */
/* System Register Data Set */
static const uint8_t UDA_sys_regs_dat[] = {
	UDA_EVALM_CLK,	/* Initial Register to which following data be written */
	UDA1380_U8(UDA1380_REG_EVALCLK_DEFAULT_VALUE),
	//i2s register data:
	UDA1380_U8(UDA1380_REG_I2S_DEFAULT_VALUE),
	//PWRCTRL data
	UDA1380_U8(UDA1380_REG_PWRCTRL_DEFAULT_VALUE),
	//ANALOG MIXER Settings
	UDA1380_U8(UDA1380_REG_ANAMIX_DEFAULT_VALUE),
	//HEADPHONE AMP
	UDA1380_U8(UDA1380_REG_HEADAMP_DEFAULT_VALUE)
};

/* System Register Data Set */
static const uint8_t UDA_interfil_regs_dat[] = {
	UDA_MASTER_VOL_CTRL,/* 0x10 Register to which following data be written */
	UDA1380_U8(UDA1380_REG_MSTRVOL_DEFAULT_VALUE),
	//mixer volume 0x11
	UDA1380_U8(UDA1380_REG_MIXVOL_DEFAULT_VALUE),
	//Bass and treble 0x12
	UDA1380_U8(UDA1380_REG_MODEBBT_DEFAULT_VALUE),
	//Mute register 0x13
	UDA1380_U8(UDA1380_REG_MSTRMUTE_DEFAULT_VALUE),
	//Mixer, silence detector and oversamplig register 0x14
	UDA1380_U8(UDA1380_REG_MIXSDO_DEFAULT_VALUE)
};

/* decimator Register Data Set */
static const uint8_t UDA_decimator_regs_dat[] = {
	UDA_DEC_VOL_CTRL,	/* 0x20 Register to which following data be written */
	UDA1380_U8(UDA1380_REG_DECVOL_DEFAULT_VALUE),
	// 0x21
	UDA1380_U8(UDA1380_REG_PGA_DEFAULT_VALUE),
	// 0x22
	UDA1380_U8(UDA1380_REG_ADC_DEFAULT_VALUE),
	// 0x23
	UDA1380_U8(UDA1380_REG_AGC_DEFAULT_VALUE)
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
/* Set the default values to the codec registers */
static int Audio_Codec_SetDefaultValues(const uint8_t *values, int sz)
{
	int ret;
	uint8_t buff[10];	/* Verification buffer */

	/* Set System register's default values */
	ret = UDA1380_REG_WriteMult(values, sz);
	if (ret) {
		ret = UDA1380_REG_VerifyMult(values[0], &values[1], buff, sz - 1);
	}
	return ret;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Initialize UDA1380 to its default state
 * @param	input	: Audio input source (Must be one of  #UDA1380_LINE_IN
 *                    or #UDA1380_MIC_IN_L or #UDA1380_MIC_IN_LR)
 * @return	1 on Success and 0 on failure
 */
int UDA1380_Init(int input)
{
	I2C_EVENTHANDLER_T old = Chip_I2C_GetMasterEventHandler(UDA1380_I2C_BUS);
	int ret;

	/* Initialize I2C */
	Board_I2C_Init(UDA1380_I2C_BUS);
	Chip_I2C_Init(UDA1380_I2C_BUS);
	Chip_I2C_SetClockRate(UDA1380_I2C_BUS, 100000);
	Chip_I2C_SetMasterEventHandler(UDA1380_I2C_BUS, Chip_I2C_EventHandlerPolling);

	/* Initialize the default values */
	ret = Audio_Codec_SetDefaultValues(UDA_sys_regs_dat, sizeof(UDA_sys_regs_dat));

	if (ret) {
		ret = Audio_Codec_SetDefaultValues(UDA_interfil_regs_dat, sizeof(UDA_interfil_regs_dat));
	}

	if (ret) {
		ret = Audio_Codec_SetDefaultValues(UDA_decimator_regs_dat, sizeof(UDA_decimator_regs_dat));
	}

	if (ret && input) {
		/* Disable Power On for ADCR, PGAR, PGAL to get mic sound more clearly */
		ret = UDA1380_REG_WriteVerify(UDA_POWER_CTRL,
									  UDA1380_REG_PWRCTRL_DEFAULT_VALUE & (~(0x0B)));

		if (ret) {
			ret = UDA1380_REG_WriteVerify(UDA_ADC_CTRL,
										  UDA1380_REG_ADC_DEFAULT_VALUE | input);
		}
	}
	Chip_I2C_SetMasterEventHandler(UDA1380_I2C_BUS, old);

	return ret;
}

/* Write data to UDA register */
void UDA1380_REG_Write(uint8_t reg, uint16_t val)
{
	uint8_t dat[3];
	dat[0] = reg; dat[1] = val >> 8; dat[2] = val & 0xFF;
	Chip_I2C_MasterSend(UDA1380_I2C_BUS, I2CDEV_UDA1380_ADDR, dat, sizeof(dat));
}

/* Read data from UDA register */
uint16_t UDA1380_REG_Read(uint8_t reg) {
	uint8_t rx_data[2];
	if (Chip_I2C_MasterCmdRead(UDA1380_I2C_BUS, I2CDEV_UDA1380_ADDR, reg, rx_data, 2) == 2) {
		return (rx_data[0] << 8) | rx_data[1];
	}
	return 0;
}

/* Write data to UDA register and verify the value by reading it back */
int UDA1380_REG_WriteVerify(uint8_t reg, uint16_t val)
{
	uint16_t ret;
	UDA1380_REG_Write(reg, val);
	ret = UDA1380_REG_Read(reg);
	return ret == val;
}

/* Multiple value verification function */
int UDA1380_REG_VerifyMult(uint8_t reg, const uint8_t *value, uint8_t *buff, int len)
{
	int i;
	if (Chip_I2C_MasterCmdRead(UDA1380_I2C_BUS, I2CDEV_UDA1380_ADDR, reg, buff, len) != len) {
		return 0;	/* Partial read */

	}
	/* Compare the values */
	for (i = 0; i < len; i++) {
		if (value[i] != buff[i]) {
			break;
		}
	}

	return i == len;
}


/* Write multiple registers in one go */
int UDA1380_REG_WriteMult(const uint8_t *buff, int len)
{
	return Chip_I2C_MasterSend(UDA1380_I2C_BUS, I2CDEV_UDA1380_ADDR, buff, len) == len;
}

/**
 * @}
 */
