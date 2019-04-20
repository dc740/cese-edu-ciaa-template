/*
 * dac_setup.c
 *
 *  Created on: Apr 19, 2019
 *      Author: dc740
 */
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include "dac_setup.h"
#include "board.h"
#include "audio_generation.h"
#include "uda1380.h"

void initI2S0() {
	printf("Inicializando i2s\n");
	Chip_SCU_PinMuxSet(3, 0, SCU_PINIO_FAST | SCU_MODE_FUNC2); //i2s0 tx sck
	Chip_SCU_PinMuxSet(3, 1, SCU_PINIO_FAST | SCU_MODE_FUNC0); //i2s0 tx ws (también en 0,0 func6)
	Chip_SCU_PinMuxSet(3, 2, SCU_PINIO_FAST | SCU_MODE_FUNC0); //i2s0 tx sda
	Chip_SCU_PinMuxSet(0xF, 4, SCU_PINIO_FAST | SCU_MODE_FUNC6); //i2s0 tx mclk
	I2S_AUDIO_FORMAT_T i2sAudioFmt;
	i2sAudioFmt.SampleRate = FS;
	i2sAudioFmt.ChannelNumber = SOUND_MIXER_CHANNELS;
	i2sAudioFmt.WordWidth = SOUND_MIXER_BITS;
	Chip_I2S_Init(SOUND_I2S_PORT);
	Chip_I2S_TxConfig(SOUND_I2S_PORT, &i2sAudioFmt, FALSE);
	Chip_I2S_TxStop(SOUND_I2S_PORT);
	Chip_I2S_DisableMute(SOUND_I2S_PORT);
	Chip_I2S_TxStart(SOUND_I2S_PORT);
	Chip_I2S_DMA_TxCmd(SOUND_I2S_PORT, I2S_DMA_REQUEST_CHANNEL_1, ENABLE, 4); //dma channel 1=i2s tx, 2=i2s rx
	Chip_GPDMA_Init(LPC_GPDMA);
	NVIC_DisableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, 3);
	NVIC_EnableIRQ(DMA_IRQn);
	printf("i2s listo\n");
}

/**
 * Initialize i2c, and send a couple of commands to set up
 * a working UDA1380 state.
 */
void initDac() {
	//I2S and DMA init
	initI2S0();

	// Default initialization routine
	// IMPORTANT: i2s clock MUST be working or the UDA1380 will lock
	// some registers and you won't be able to change them
	// so we add a small delay to let it sense it
	vTaskDelay(100 / portTICK_RATE_MS);
	UDA1380_Init(0);
	// custom setup touches:
	//None for now
}

