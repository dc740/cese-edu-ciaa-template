#include <stdint.h>
#include "audio_generation.h"
#include "uda1380.h"
#include "dma_handler.h"

//Array population function
float sinPos;
float sinStep = 2 * M_PI * TONE_FREQUENCY / FS;
void populate_wave(uint32_t pos) {
	sample_type sample;
	uint32_t n;

	for (n = pos; n < pos + HALF_DMA_BUFSIZ; n++) {
		int i = 0;
		/* Just fill the stream with sine! */
		sample = (sample_type) (VOLUME * sinf(sinPos));
		//Write sample to dma buffer
		dmabuf[n] = sample;
		if (n & 1) {
			//send the same in the 2 channels, but step sine after both. not always
			sinPos += sinStep;
			//dmabuf[n] = 32767 - 1;//test code to see how it's encoding the bits (specially the MSB)
		} else {
			//dmabuf[n] = -32768 + 1; //test code
		}
	}
}
