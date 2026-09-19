#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "fpioa.h"
#include "sysctl.h"
#include "i2s.h"
#include "fft.h"
#include "dmac.h"
#include "plic.h"

#include "configuration.h"

#define AUDIO_I2S_DEVICE        I2S_DEVICE_0
#define AUDIO_I2S_CHANNEL       I2S_CHANNEL_0
#define AUDIO_I2S_CHANNEL_MASK  0x03
#define AUDIO_SAMPLE_RATE       16000 

#define AUDIO_FFT_ALIGNMENT     8
#define AUDIO_FFT_POINTS        512
#define AUDIO_FFT_BINS          (AUDIO_FFT_POINTS / 2)

#define AUDIO_DMA_CH_RX         DMAC_CHANNEL1
#define AUDIO_DMA_CH_FFT_RX     DMAC_CHANNEL3
#define AUDIO_DMA_CH_FFT_TX     DMAC_CHANNEL4

#define AUDIO_VOICE_BIN_START   10
#define AUDIO_VOICE_BIN_END     96
#define AUDIO_VOICE_THRESHOLD   150000

#define AUDIO_DB_NOISE_FLOOR    (-96.0f)    

void audio_init(void);
void audio_read(void);

void audio_get_magnitudes(uint16_t *output_magnitudes, uint16_t max_bins);
void audio_get_power(uint32_t *output_power, uint16_t max_bins);
void audio_get_db(float *output_db, uint16_t max_bins);

uint16_t audio_get_peak_frequency(void);
uint32_t audio_get_rms_volume(void);

#endif