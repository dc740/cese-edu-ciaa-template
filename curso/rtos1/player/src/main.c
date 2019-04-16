#include "../../player/inc/sd_spi.h"   // <= own header (optional)

#include "board.h"
#include "sapi.h"     // <= sAPI header
#include "ff.h"       // <= Biblioteca FAT FS
#include "fssdc.h"    // API de bajo nivel para unidad SD en FAT FS
#include "uda1380.h"
#include "wavData.h"
#include <string.h>
#include <stdint.h>
#include <math.h>

#define SAPI_USE_INTERRUPTS
#define I2S_DMA_TX_CHAN GPDMA_CONN_I2S_Tx_Channel_0 //for i2s1 this is called GPDMA_CONN_I2S1_Tx_Channel_0
#define M_PI		3.14159265358979323846

/* This is basically an arbitrary number for the tone generator*/
#define VOLUME 127.0


//(DMA Buffer size)/2, this can be adjusted if samples seem to be dropped
#define HALF_DMA_BUFSIZ	2000

//Sampling frequency
//#define FS			96000
//#define FS			48000
//Sampling frequency with error correction - 48000*(100-2.3438)/100 = 46874.98Hz
//#define FS			46875
//#define FS              44100
#define FS              44400
//#define FS              32000
//#define FS              21250
//#define FS              16000

//Waveform output frequency (subject to 2.34% error due to PLL)
#define TONE_FREQUENCY		220

// I2S Port configuration
#define SOUND_I2S_PORT LPC_I2S0
#define SOUND_MIXER_BITS 16
typedef int16_t sample_type;
#define SOUND_MIXER_CHANNELS 2
//big endian for both DMA channels!!! and the enable bit 0
#define DMA_CONFIG (GPDMA_DMACConfig_E | 0x6)

// FUNCION que se ejecuta cada vezque ocurre un Tick
void diskTickHook( void *ptr );

FATFS fs;
FRESULT res;
char buff[256];

/**
 * Initialize the SPI but use P1_19 as the CLK output
 * This is needed to use the i2s0 MCLK generation in PF_4
 */
bool_t customSpiInit( spiMap_t spi )
{

   bool_t retVal = TRUE;

   if( spi == SPI0 ) {

      /* Set up clock and power for SSP1 module */
      // Configure SSP SSP1 pins
      //Chip_SCU_PinMuxSet(0xf, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC0)); // CLK0
      Chip_SCU_PinMuxSet(0x1, 19, (SCU_MODE_PULLUP | SCU_MODE_FUNC1)); // USE ENET_REF_CLK for the clock output
      Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)); // MISO1
      Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC5)); // MOSI1

      // Don't use GPIO0 as it has important i2s RX functions. Use GPIO1
      Chip_SCU_PinMuxSet(0x6, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC0)); // CS configured as GPIO
      Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 3, 3);

      // Initialize SSP Peripheral
      Chip_SSP_Init( LPC_SSP1 );
      Chip_SSP_Enable( LPC_SSP1 );

   } else {
      retVal = FALSE;
   }

   return retVal;
}

//DMA Buffer
sample_type dmabuf[HALF_DMA_BUFSIZ*2] = {0};



static uint32_t send_index = 0; //which part of buffer we send
static uint8_t dmaChannelNum_I2S_Tx;
uint32_t dmaTransferComplete = 0;
uint8_t mute_status = 0;

static void mute_toggle()
{
	mute_status = !mute_status;
	if (mute_status) {
		Chip_I2S_EnableMute(SOUND_I2S_PORT);
	}
	else {
		Chip_I2S_DisableMute(SOUND_I2S_PORT);
	}
}


//Array population function
float sinPos;
float sinStep = 2 * M_PI * TONE_FREQUENCY / FS;
void populate_wave(uint32_t pos){
	sample_type sample;
	uint32_t n;

	for(n = pos; n<pos+HALF_DMA_BUFSIZ; n++){
		int i=0;
			/* Just fill the stream with sine! */
			sample = (sample_type) (VOLUME * sinf(sinPos));
			//Write sample to dma buffer
			dmabuf[n] = sample;
			if (n&1) {
				//send the same in the 2 channels, but step sine after both. not always
				sinPos += sinStep;
				//dmabuf[n] = 32767 - 1;//test code to see how it's encoding the bits (specially the MSB)
			} else {
				//dmabuf[n] = -32768 + 1; //test code
			}
	}
}


#ifdef SAPI_USE_INTERRUPTS

__attribute__ ((section(".after_vectors")))
/**
 * @brief	DMA interrupt handler sub-routine
 * @return	Nothing
 */
void DMA_IRQHandler(void)
{
	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChannelNum_I2S_Tx) == SUCCESS) {
		dmaChannelNum_I2S_Tx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, I2S_DMA_TX_CHAN);

		Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNum_I2S_Tx,
							(uint32_t) &dmabuf[send_index], //src is the bufffer
							I2S_DMA_TX_CHAN,
							GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
							HALF_DMA_BUFSIZ,
							DMA_CONFIG); //Big Endian
		//update the index and populate the new region
		if (send_index) {
			send_index = 0;
		} else {
			send_index = HALF_DMA_BUFSIZ;
		}
		dmaTransferComplete ++;
	}
	else {
		/* Process error here */
		printf("Error escribiendo buffer i2s\n");
	}
}

#endif /* SAPI_USE_INTERRUPTS */




void initI2S0() {
	printf("Inicializando i2s\n");
	Chip_SCU_PinMuxSet (3, 0, SCU_PINIO_FAST | SCU_MODE_FUNC2); //i2s0 tx sck
	Chip_SCU_PinMuxSet (3, 1, SCU_PINIO_FAST | SCU_MODE_FUNC0); //i2s0 tx ws (también en 0,0 func6)
	Chip_SCU_PinMuxSet (3, 2, SCU_PINIO_FAST | SCU_MODE_FUNC0); //i2s0 tx sda
	Chip_SCU_PinMuxSet (0xF, 4, SCU_PINIO_FAST | SCU_MODE_FUNC6); //i2s0 tx mclk
	I2S_AUDIO_FORMAT_T i2sAudioFmt;
	i2sAudioFmt.SampleRate = FS;
	i2sAudioFmt.ChannelNumber = SOUND_MIXER_CHANNELS;
	i2sAudioFmt.WordWidth = SOUND_MIXER_BITS;
	Chip_I2S_Init (SOUND_I2S_PORT);
	Chip_I2S_TxConfig (SOUND_I2S_PORT, &i2sAudioFmt, TRUE);
	Chip_I2S_TxStop (SOUND_I2S_PORT);
	Chip_I2S_DisableMute (SOUND_I2S_PORT);
	Chip_I2S_TxStart (SOUND_I2S_PORT);
	Chip_I2S_DMA_TxCmd (SOUND_I2S_PORT, I2S_DMA_REQUEST_CHANNEL_1, ENABLE, 4); //dma channel 1=i2s tx, 2=i2s rx
	Chip_GPDMA_Init (LPC_GPDMA);
	NVIC_DisableIRQ (DMA_IRQn);
	NVIC_SetPriority (DMA_IRQn, 3);
	NVIC_EnableIRQ (DMA_IRQn);
	printf("i2s listo\n");
	/* ENET_MDC: ~AUDIO_MUTE
	Chip_SCU_PinMuxSet (7, 7, SCU_PINIO_FAST | SCU_MODE_FUNC0);
	Chip_GPIO_SetPinDIROutput (LPC_GPIO_PORT, 3, 15);
	end of ENET_MDC*/
}


/**
 * Initialize i2c, and send a couple of commands to set up
 * a working UDA1380 state.
 */
void initDac(){
	//I2S and DMA init
	initI2S0();

	// Default initialization routine
	// IMPORTANT: i2s clock MUST be working or the UDA1380 will lock
	// some registers and you won't be able to change them
	// so we add a small delay to let it sense it
	delay(100);
	UDA1380_Init(0);
	// custom setup touches
}

int main (void)
{

	boardConfig();

	customSpiInit( SPI0 ); // TODO: re-include sd card support

	//Populate the entire buffer. Then we will send one half, and the
	// DMA irq will trigger the next one.
	populate_wave(0);
	populate_wave(HALF_DMA_BUFSIZ);
	initDac();
	//Trigger the transfer
	printf("Iniciando primera trama\n");
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNum_I2S_Tx,
						(uint32_t) &dmabuf[0], //src is the bufffer
						I2S_DMA_TX_CHAN,
						GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
						HALF_DMA_BUFSIZ,
						DMA_CONFIG);
	printf("Dejando el control a la interrupcion de DMA\n");


/*
	FSSDC_InitSPI ();
	   if( f_mount( &fs, "SDC:", 0 ) != FR_OK ){
	      while (1) {
	         gpioToggle( LEDR );
	         printf("SD no disponible\n");
	         delay(1000);
	      }
	   }
	   // Leer archivo
	   if( f_open( &fp, "SDC:/test.wav", FA_READ ) == FR_OK ){
	      while (!f_eof( &fp ) ) {
	         int nbytes;
	         f_read ( &fp , buf, 100, &nbytes);
	         printf("%s", buf );
	      }
	   } else
	      printf(" Error al abrir archivo\n");
	   fflush(stdout);
	   while(1);*/

		uint32_t oldDmaTc = 0;
	   while (1) {
		   if (dmaTransferComplete != oldDmaTc) { //only enter after a new one
			   //this code will keep populating the buffer, while the DMA interrupt
			   //will trigger the next transfer
			   oldDmaTc = dmaTransferComplete;
			   populate_wave(send_index);
		   }
	   }

	return res;
}


// FUNCION que se ejecuta cada vezque ocurre un Tick
void diskTickHook( void *ptr )
{
   disk_timerproc();   // Disk timer process
}
