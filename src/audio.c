#include "audio.h"

static uint32_t rx_dma_buffer[AUDIO_FFT_POINTS * 2]; 

static uint64_t fft_input_buffer [AUDIO_FFT_BINS] __attribute__((aligned(AUDIO_FFT_ALIGNMENT)));
static uint64_t fft_output_buffer[AUDIO_FFT_BINS] __attribute__((aligned(AUDIO_FFT_ALIGNMENT)));

static uint16_t display_magnitudes[AUDIO_FFT_BINS] = {0};

static volatile bool audio_frame_ready = false;

static int audio_dma_isr(void *parameter)
{
    audio_frame_ready = true;
    return 0;
}

void audio_init(void)
{
    fpioa_set_function(MIC_PIN_BCLK, FUNC_I2S0_SCLK);
    fpioa_set_function(MIC_PIN_WS,   FUNC_I2S0_WS);
    fpioa_set_function(MIC_PIN_DAT,  FUNC_I2S0_IN_D0);

    i2s_init(AUDIO_I2S_DEVICE, I2S_RECEIVER, AUDIO_I2S_CHANNEL_MASK);
    i2s_rx_channel_config(AUDIO_I2S_DEVICE, AUDIO_I2S_CHANNEL, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, STANDARD_MODE);

    dmac_irq_register(AUDIO_DMA_CH_RX, audio_dma_isr, NULL, 1);
    i2s_receive_data_dma(AUDIO_I2S_DEVICE, rx_dma_buffer, AUDIO_FFT_POINTS * 2, AUDIO_DMA_CH_RX);

    audio_frame_ready = false;
}

void audio_read(void)
{
    if(!audio_frame_ready) return;

    int16_t *input = (int16_t *)fft_input_buffer;
    
    for(uint16_t i = 0; i < AUDIO_FFT_POINTS; i++) 
    {
        input[i * 2]     = (int16_t)(rx_dma_buffer[2 * i + 1] & 0xFFFF);
        input[i * 2 + 1] = 0;           
    }

    fft_complex_uint16_dma(AUDIO_DMA_CH_FFT_RX, AUDIO_DMA_CH_FFT_TX, 0x1FF, FFT_DIR_FORWARD, fft_input_buffer, AUDIO_FFT_POINTS, fft_output_buffer);
    dmac_wait_done(AUDIO_DMA_CH_FFT_RX);

    int16_t *output = (int16_t *)fft_output_buffer;

    for(uint16_t i = 0; i < AUDIO_FFT_BINS; i++) 
    {
        int16_t real = output[i * 2];
        int16_t imag = output[i * 2 + 1];
        uint32_t raw_mag = (uint32_t)sqrt((real * real) + (imag * imag));
        
        display_magnitudes[i] = (display_magnitudes[i] * 7 + raw_mag * 3) / 10;
    }

    dmac_irq_register(AUDIO_DMA_CH_RX, audio_dma_isr, NULL, 1);
    i2s_receive_data_dma(AUDIO_I2S_DEVICE, rx_dma_buffer, AUDIO_FFT_POINTS * 2, AUDIO_DMA_CH_RX);

    audio_frame_ready = false;
}

void audio_get_magnitudes(uint16_t *output_magnitudes, uint16_t max_bins)
{
    for(uint16_t i = 0; i < max_bins; i++)
    {
        output_magnitudes[i] = display_magnitudes[i];
    }
}

void audio_get_power(uint32_t *output_power, uint16_t max_bins)
{
    for(uint16_t i = 0; i < max_bins; i++)
    {
        uint32_t mag = display_magnitudes[i];
        output_power[i] = mag * mag;
    }
}

void audio_get_db(float *output_db, uint16_t max_bins)
{
    for(uint16_t i = 0; i < max_bins; i++)
    {
        if(display_magnitudes[i] == 0) 
        {
            output_db[i] = AUDIO_DB_NOISE_FLOOR; 
        } 
        else 
        {
            output_db[i] = 20.0f * log10f((float)display_magnitudes[i]);
        }
    }
}

uint16_t audio_get_peak_frequency(void)
{
    uint16_t peak_frequency_bin = 0;
    uint16_t maximum_magnitude = 0;

    for(uint16_t i = 1; i < AUDIO_FFT_BINS; i++) 
    {
        if(display_magnitudes[i] > maximum_magnitude)
        {
            maximum_magnitude = display_magnitudes[i];
            peak_frequency_bin = i;
        }
    }
    
    return (uint16_t)(((uint32_t)peak_frequency_bin * AUDIO_SAMPLE_RATE) / AUDIO_FFT_POINTS);
}

uint32_t audio_get_rms_volume(void)
{
    uint64_t sum_of_squares = 0;
    
    for(uint16_t i = 1; i < AUDIO_FFT_BINS; i++) 
    {
        uint32_t mag = display_magnitudes[i];
        sum_of_squares += (mag * mag);
    }
    
    return (uint32_t)sqrt(sum_of_squares / (AUDIO_FFT_BINS - 1));
}