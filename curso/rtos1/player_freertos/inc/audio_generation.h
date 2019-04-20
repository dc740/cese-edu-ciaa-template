/*
 * audio_generation.h
 *
 *  Created on: Apr 19, 2019
 *      Author: dc740
 */

#ifndef CURSO_RTOS1_PLAYER_FREERTOS_INC_AUDIO_GENERATION_H_
#define CURSO_RTOS1_PLAYER_FREERTOS_INC_AUDIO_GENERATION_H_
#include <stdint.h>
#include <math.h>

//Sampling frequency
//#define FS			96000
//#define FS			48000
//#define FS              44100
//#define FS              44400
//#define FS              32000
#define FS              21250
//#define FS              16000

// I2S Port configuration
#define SOUND_I2S_PORT LPC_I2S0
#define SOUND_MIXER_BITS 8
typedef uint8_t sample_type;
#define SOUND_MIXER_CHANNELS 2

static uint32_t send_index = 0; //which part of buffer we send

#define M_PI		3.14159265358979323846

void populate_wave(uint32_t pos);

#endif /* CURSO_RTOS1_PLAYER_FREERTOS_INC_AUDIO_GENERATION_H_ */
