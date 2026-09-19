#include "ov5642.h"
#include "ov5642_configuration.h"

uint16_t camera_buffer[LCD_WIDTH * LCD_HEIGHT] __attribute__((aligned(LCD_ALIGNMENT)));

static volatile uint8_t frame_ready = false;

void ov5642_init(uint8_t *ai_buffer)
{
    fpioa_set_function(CAM_PIN_XCLK,  FUNC_CMOS_XCLK);
    fpioa_set_function(CAM_PIN_VSYNC, FUNC_CMOS_VSYNC);
    fpioa_set_function(CAM_PIN_HREF,  FUNC_CMOS_HREF);
    fpioa_set_function(CAM_PIN_PCLK,  FUNC_CMOS_PCLK);
    fpioa_set_function(CAM_PIN_SCL,   FUNC_SCCB_SCLK);
    fpioa_set_function(CAM_PIN_SDA,   FUNC_SCCB_SDA);
    fpioa_set_function(CAM_PIN_RST,   FUNC_GPIOHS4);
    fpioa_set_function(CAM_PIN_PWDN,  FUNC_GPIOHS5);

    dvp_init(OV5642_PORT_LENGTH);
    dvp_set_xclk_rate(OV5642_XCLK_RATE); 

    gpiohs_set_drive_mode(CAM_GPIO_RST,  GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(CAM_GPIO_PWDN, GPIO_DM_OUTPUT);

    gpiohs_set_pin(CAM_GPIO_PWDN, GPIO_PV_LOW); 

    gpiohs_set_pin(CAM_GPIO_RST, GPIO_PV_LOW);  msleep(300);                      
    gpiohs_set_pin(CAM_GPIO_RST, GPIO_PV_HIGH); msleep(300);            

    dvp_enable_burst();

    dvp_set_output_enable(DVP_OUT_KPU, true); 
    dvp_set_output_enable(DVP_OUT_LCD, true); 
    dvp_set_image_format(DVP_CFG_YUV_FORMAT); 
    dvp_set_image_size(LCD_WIDTH, LCD_HEIGHT);
    
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);

    dvp_disable_auto();
	
    if(ov5642_read_id() != OV5642_ID) return;

    dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SYS_CTL, OV5642_CMD_SWRESET); msleep(100); 
    
    for(int i = 0; ov5642_configuration[i][0] != 0xffff; i++) 
	{
        uint16_t data_register = ov5642_configuration[i][0];
        uint8_t  data_value    = (uint8_t)ov5642_configuration[i][1];

        if(data_register == OV5642_REG_SYS_CTL && data_value == OV5642_CMD_SWRESET) continue; 

        dvp_sccb_send_data(OV5642_ADDRESS, data_register, data_value); 
    }

    #ifdef NVG_RIGHT_SCREEN
        uint8_t value_format_ctl = dvp_sccb_receive_data(OV5642_ADDRESS, OV5642_REG_FORMAT_CTL);
        value_format_ctl ^= 0x60; 
        dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_FORMAT_CTL, value_format_ctl);

        uint8_t value_bayer_ctl = dvp_sccb_receive_data(OV5642_ADDRESS, OV5642_REG_BAYER_CTL);
        value_bayer_ctl ^= 0x20; 
        dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_BAYER_CTL, value_bayer_ctl);
    #endif
    
    dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SYS_CTL, OV5642_CMD_WAKEUP); msleep(100); 

    ov5642_setup_ai_pipeline(ai_buffer);
}

int dvp_interrupt(void *parameter) 
{
    if(dvp_get_interrupt(DVP_STS_FRAME_FINISH)) 
    {
        dvp_clear_interrupt(DVP_STS_FRAME_FINISH);
        frame_ready = true; 
    } 

    if(dvp_get_interrupt(DVP_STS_FRAME_START)) 
    {
        dvp_clear_interrupt(DVP_STS_FRAME_START);
    }
    return 0;
}

void ov5642_clear_frame_ready(void) 
{
    frame_ready = false;
}

void ov5642_enable_interrupts(void)
{
    plic_set_priority(IRQN_DVP_INTERRUPT, 1); 
    plic_irq_register(IRQN_DVP_INTERRUPT, dvp_interrupt, NULL); 
    plic_irq_enable(IRQN_DVP_INTERRUPT); 

    dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);
    dvp_enable_auto();
}

void ov5642_setup_ai_pipeline(uint8_t *ai_buffer)
{
    dvp_set_output_enable(DVP_OUT_KPU, true); 
    dvp_set_output_enable(DVP_OUT_LCD, true); 

    uint32_t r_plane = (uint32_t)(ai_buffer + (LCD_WIDTH * LCD_HEIGHT * 0));
    uint32_t g_plane = (uint32_t)(ai_buffer + (LCD_WIDTH * LCD_HEIGHT * 1));
    uint32_t b_plane = (uint32_t)(ai_buffer + (LCD_WIDTH * LCD_HEIGHT * 2));
    
    dvp_set_ai_addr(r_plane, g_plane, b_plane);
}

uint8_t ov5642_check_frame_ready(void) 
{
    return frame_ready;
}

uint16_t ov5642_read_id(void)
{
    uint8_t id_high = 0;
    uint8_t id_low  = 0;

    id_high = dvp_sccb_receive_data(OV5642_ADDRESS, OV5642_REG_ID_HIGH);
    id_low  = dvp_sccb_receive_data(OV5642_ADDRESS, OV5642_REG_ID_LOW);
    
    uint16_t device_id = (id_high << 8) | id_low;

    return device_id;
}