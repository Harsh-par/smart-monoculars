#include "rotary_encoder.h"

static volatile uint16_t encoder_layer = 0;

static volatile uint64_t previous_increment_us = 0; 
static volatile uint64_t previous_decrement_us = 0; 

static int rotary_encoder_handler(void *parameter)
{
    static const int8_t encoder_state_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

    uint64_t current_time_us = sysctl_get_time_us();
  
    static uint8_t previous_states = 0b00000011;  
    static int8_t encoder_value = 0;  

    previous_states = previous_states << 2; 

    if(gpiohs_get_pin(ROT_GPIO_ENCA)) previous_states |= 0b00000010; 
    if(gpiohs_get_pin(ROT_GPIO_ENCB)) previous_states |= 0b00000001; 
  
    encoder_value = encoder_value + encoder_state_table[(previous_states & 0b00001111)];

    if(encoder_value > 3)
    {      
        int8_t encoder_step = 1;

        if(current_time_us - previous_increment_us < ROT_ENC_DEBOUNCE_US)
        {
            encoder_step = ROT_ENC_STEP_SIZE * encoder_step; 
        }

        previous_increment_us = current_time_us;
        encoder_layer = (encoder_layer + encoder_step + ROT_ENC_LAYER_COUNT) % ROT_ENC_LAYER_COUNT;            
        encoder_value = 0;
    }
    else if(encoder_value < -3)
    {        
        int8_t encoder_step = -1;

        if(current_time_us - previous_decrement_us < ROT_ENC_DEBOUNCE_US)
        {
            encoder_step = ROT_ENC_STEP_SIZE * encoder_step; 
        }

        previous_decrement_us = current_time_us;
        encoder_layer = (encoder_layer + encoder_step + ROT_ENC_LAYER_COUNT) % ROT_ENC_LAYER_COUNT;            
        encoder_value = 0;
    }

    return 0; 
}

void rotary_encoder_init(void)
{
    fpioa_set_function(ROT_PIN_BTN,  FUNC_GPIOHS6);
    fpioa_set_function(ROT_PIN_ENCA, FUNC_GPIOHS7);
    fpioa_set_function(ROT_PIN_ENCB, FUNC_GPIOHS8);

    gpiohs_set_drive_mode(ROT_GPIO_BTN,  GPIO_DM_INPUT_PULL_UP);
    gpiohs_set_drive_mode(ROT_GPIO_ENCA, GPIO_DM_INPUT_PULL_UP);
    gpiohs_set_drive_mode(ROT_GPIO_ENCB, GPIO_DM_INPUT_PULL_UP);

    gpiohs_set_pin_edge(ROT_GPIO_ENCA, GPIO_PE_BOTH);
    gpiohs_set_pin_edge(ROT_GPIO_ENCB, GPIO_PE_BOTH);

    gpiohs_irq_register(ROT_GPIO_ENCA, 1, rotary_encoder_handler, NULL);
    gpiohs_irq_register(ROT_GPIO_ENCB, 1, rotary_encoder_handler, NULL);
}

uint16_t rotary_encoder_get_layer(void)
{
    return encoder_layer;
}

uint8_t rotary_encoder_get_state(void)
{
    return !gpiohs_get_pin(ROT_GPIO_BTN);
}