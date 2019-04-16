#ifndef __I2S_H__
#define __I2S_H__
#include <stdint.h>

#define FS              44400
#define SOUND_MIXER_BITS 16
typedef int16_t sample_type;
#define SOUND_MIXER_CHANNELS 2

void i2s_init(void);

#endif  // __I2S_H__

