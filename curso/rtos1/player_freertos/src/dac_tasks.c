/*
 * dac_tasks.c
 *
 *  Created on: Apr 19, 2019
 *      Author: dc740
 */
#include <stdint.h>
#include "board.h"
#include "dma_handler.h"
#include "audio_generation.h"

void fillBufferTask( void* taskParmPtr )
{
	uint32_t oldDmaTc = 0; //TODO: convertir a semaforo
	//Trigger the transfer
	printf("Iniciando primera trama\n");
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNum_I2S_Tx,
						(uint32_t) &dmabuf[0], //src is the bufffer
						I2S_DMA_TX_CHAN,
						GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
						HALF_DMA_BUFSIZ,
						DMA_CONFIG);
	printf("Dejando el control a la interrupcion de DMA\n");

	while (1) {
		if (dmaTransferComplete != oldDmaTc) { //only enter after a new one
		   //this code will keep populating the buffer, while the DMA interrupt
		   //will trigger the next transfer
		   oldDmaTc = dmaTransferComplete;
		   populate_wave(send_index);
		}
	}
}
