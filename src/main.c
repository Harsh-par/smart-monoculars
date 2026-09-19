#include <stdio.h>

#include "sysctl.h"
#include "dmac.h"
#include "plic.h"
#include "dvp.h"
#include "bsp.h"

#include "configuration.h"
#include "constant.h"
#include "kpu_ai.h"
#include "audio.h"
#include "system.h"
#include "gui.h"
#include "mpu6050.h"
#include "st7789.h"
#include "ov5642.h"
#include "rotary_encoder.h"
#include "time_stamp.h"

int main_core1(void* parameter);

volatile uint8_t frame_count_core0 = 0;
volatile uint8_t frame_count_core1 = 0;

volatile uint8_t fps_count_core0 = 0;
volatile uint8_t fps_count_core1 = 0;

volatile bool run_ai_model = true;

volatile obj_info_t obj_info;

int main(void)
{
    uint32_t non_cached_camera_buffer = (uint32_t)camera_buffer - NON_CACHED_MEMORY_OFFSET;
    uint32_t non_cached_ai_buffer     = (uint32_t)ai_buffer     - NON_CACHED_MEMORY_OFFSET;

    system_init();
    ov5642_init((uint8_t*)non_cached_ai_buffer); 
    st7789_init(&st7789, (uint16_t*)non_cached_camera_buffer, LCD_WIDTH, LCD_HEIGHT); 
    kpu_ai_init();
    mpu6050_init(&mpu6050, IMU_CLK_SPEED, IMU_PIN_SDA, IMU_PIN_SCL);
    rotary_encoder_init();

    register_core1(main_core1, NULL);

    while(true)
    {
        if(ov5642_check_frame_ready())
        {
            ov5642_clear_frame_ready(); 

            if(run_ai_model)
            {
                kpu_ai_run_inference();
                kpu_ai_process_results(&obj_info);
                kpu_ai_draw_bounding_box(&st7789, &obj_info, ST7789_RED);
            }
    
            st7789_set_window(&st7789, 0, 0, (st7789.width - 1), (st7789.height - 1));
            st7789_fill_screen(&st7789);
            
            frame_count_core0 = frame_count_core0 + 1;
        }
    }

    return 0;
}

int main_core1(void *parameter) 
{
    uint64_t current_time_us = sysctl_get_time_us();

    time_stamp_t time_stamp_core1;
    time_stamp_t time_stamp_frame;
    time_stamp_t time_stamp_encoder_knob;
    time_stamp_t time_stamp_encoder_button;
    time_stamp_t time_stamp_overlay;

    time_stamp_init(&time_stamp_core1,          current_time_us);
    time_stamp_init(&time_stamp_frame,          current_time_us);
    time_stamp_init(&time_stamp_encoder_knob,   current_time_us);
    time_stamp_init(&time_stamp_encoder_button, current_time_us);
    time_stamp_init(&time_stamp_overlay,        current_time_us);

    audio_init();

    uint8_t previous_encoder_layer = ROT_ENC_LAYER_COUNT;

    while(true) 
    {
        current_time_us = sysctl_get_time_us();
        time_stamp_update_delta(&time_stamp_core1, current_time_us);
        
        uint8_t current_encoder_layer = rotary_encoder_get_layer();
        uint8_t current_encoder_state = rotary_encoder_get_state();
        
        if(time_stamp_check_elapsed(&time_stamp_frame, current_time_us, ONE_SECOND_IN_US))
        {
            fps_count_core0 = frame_count_core0;
            fps_count_core1 = frame_count_core1;

            frame_count_core0 = 0;
            frame_count_core1 = 0;
        }        

        if((previous_encoder_layer != current_encoder_layer) && time_stamp_check_elapsed(&time_stamp_encoder_knob, current_time_us, ROT_ENC_DEBOUNCE_US))
        {       
            previous_encoder_layer = current_encoder_layer;
        }

        switch(current_encoder_layer)
        {
            case GUI_IMU: 
            {
                if(current_encoder_state && time_stamp_check_elapsed(&time_stamp_encoder_button, current_time_us, ROT_ENC_BTN_DEBOUNCE_US))
                { 
                    run_ai_model = !run_ai_model;
                }

                mpu6050_read(&mpu6050);
                mpu6050_compute(&mpu6050);
                mpu6050_filter_pitch(&mpu6050, time_stamp_core1.delta_s);
                mpu6050_filter_roll(&mpu6050,  time_stamp_core1.delta_s);

                gui_display_hud_imu(&st7789, &mpu6050, ST7789_GREEN2, ST7789_GREEN);
                break;
            }
            case GUI_AUDIO: 
            {
                if(current_encoder_state && time_stamp_check_elapsed(&time_stamp_encoder_button, current_time_us, ROT_ENC_BTN_DEBOUNCE_US))
                { 
                    run_ai_model = !run_ai_model;
                }

                audio_read();

                gui_display_hud_audio(&st7789, ST7789_GREEN2);
                break;
            }
            case GUI_DATA:
            {
                if(current_encoder_state && time_stamp_check_elapsed(&time_stamp_encoder_button, current_time_us, ROT_ENC_BTN_DEBOUNCE_US))
                { 
                    run_ai_model = !run_ai_model;
                }

                if(!run_ai_model) obj_info.obj_number = 0;

                gui_display_hud_data(&st7789, fps_count_core0, fps_count_core1, obj_info.obj_number, ST7789_GREEN2);
                break;
            }
            case GUI_FILTER: 
            {   
                static uint16_t current_filter  = FILTER_NONE;
                static char *text_buffer_filter = "Filter : None";

                if(current_encoder_state && time_stamp_check_elapsed(&time_stamp_encoder_button, current_time_us, ROT_ENC_BTN_DEBOUNCE_US))
                {
                    gui_display_hud_filters(&st7789, &current_filter, &text_buffer_filter);
                }
                
                gui_draw_text_tag(&st7789, 2, 142, text_buffer_filter, ST7789_GREEN2);
                gui_display_fps(&st7789, fps_count_core0, obj_info.obj_number, ST7789_GREEN2);
                break;
            }
            case GUI_ZOOM:
            {
                static uint16_t current_zoom  = ZOOM_1p0x;
                static char *text_buffer_zoom = "Zoom : 1.0x";

                if(current_encoder_state && time_stamp_check_elapsed(&time_stamp_encoder_button, current_time_us, ROT_ENC_BTN_DEBOUNCE_US))
                {
                    gui_display_hud_zoom(&st7789, &current_zoom, &text_buffer_zoom);
                }

                gui_draw_text_tag(&st7789, 2, 142, text_buffer_zoom, ST7789_GREEN2);
                gui_display_fps(&st7789, fps_count_core0, obj_info.obj_number, ST7789_GREEN2);
                break;
            }
        }

        gui_display_overlay(&st7789, current_encoder_state, current_encoder_layer, run_ai_model, ST7789_YELLOW, ST7789_BLACK);
     
        frame_count_core1 = frame_count_core1 + 1;
    }

    return 0;
}