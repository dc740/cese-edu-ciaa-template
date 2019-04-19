#ifndef CURSO_RTOS1_PLAYER_FREERTOS_INC_DMA_HANDLER_H_
#define CURSO_RTOS1_PLAYER_FREERTOS_INC_DMA_HANDLER_H_
#include <stdint.h>
#include <audio_generation.h>
#include "FreeRTOS.h"
#include "task.h"

#define HALF_DMA_BUFSIZ	2000 //(DMA Buffer size)/2, this can be adjusted if samples seem to be dropped
#define DMA_CONFIG (GPDMA_DMACConfig_E | 0x6) //big endian for both DMA channels!!! and the enable bit 0
#define I2S_DMA_TX_CHAN GPDMA_CONN_I2S_Tx_Channel_0 //for i2s1 this is called GPDMA_CONN_I2S1_Tx_Channel_0

extern sample_type dmabuf[HALF_DMA_BUFSIZ*2];
extern uint32_t dmaTransferComplete;
extern TaskHandle_t xTaskToNotifyAboutDMA;

extern uint8_t dmaChannelNum_I2S_Tx;

#endif /* CURSO_RTOS1_PLAYER_FREERTOS_INC_DMA_HANDLER_H_ */
