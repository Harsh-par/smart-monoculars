#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdint.h>
#include <stdbool.h>

#include "sysctl.h"
#include "fpioa.h"
#include "gpiohs.h"

#include "configuration.h"

#define ROT_ENC_LAYER_COUNT  10    
#define ROT_ENC_DEBOUNCE_US  10000 
#define ROT_ENC_BTN_DEBOUNCE_US (ROT_ENC_DEBOUNCE_US * 22)
#define ROT_ENC_STEP_SIZE    2     

void rotary_encoder_init(void);
uint16_t rotary_encoder_get_layer(void);
uint8_t rotary_encoder_get_state(void);

#endif