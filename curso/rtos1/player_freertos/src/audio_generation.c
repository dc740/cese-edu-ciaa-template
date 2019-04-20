#include <stdint.h>
#include "audio_generation.h"
#include "uda1380.h"
#include "dma_handler.h"
#include "wavData.h"

//Array population function
uint32_t currentPCMPosition = 0;
void populate_wave(uint32_t pos){
	sample_type sample;
	uint32_t n;

	for(n = pos; n<pos+HALF_DMA_BUFSIZ; n=n+2){
		int i=0;
			/* Just fill the stream with sine! */
			sample = (sample_type) Ref440_PCM[currentPCMPosition];
			//Write sample to dma buffer
			dmabuf[n] = sample;
			dmabuf[n+1] = sample;
			currentPCMPosition++;

			if (currentPCMPosition > AUDIO_LEN-1){
				currentPCMPosition = 0;
			}
	}
}

