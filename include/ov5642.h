#ifndef OV5642_H
#define OV5642_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "sleep.h"
#include "dvp.h"
#include "fpioa.h"
#include "gpiohs.h"
#include "plic.h"

#include "configuration.h"
#include "constant.h"

#define OV5642_ADDRESS        0x78 
#define OV5642_ID             0x5642

#define OV5642_REG_SYS_CTL    0x3008
#define OV5642_REG_ID_HIGH    0x300A	
#define OV5642_REG_ID_LOW     0x300B
#define OV5642_REG_FORMAT_CTL 0x3818
#define OV5642_REG_BAYER_CTL  0x3621

#define OV5642_CMD_SWRESET    0x82
#define OV5642_CMD_WAKEUP     0x02
#define OV5642_CMD_VFLP_HMIR  0x41

#define OV5642_PORT_LENGTH    16
#define OV5642_XCLK_RATE      27000000

#define OV5642_REG_TIMING_HS_H 0x3800
#define OV5642_REG_TIMING_HS_L 0x3801
#define OV5642_REG_TIMING_VS_H 0x3802
#define OV5642_REG_TIMING_VS_L 0x3803
#define OV5642_REG_TIMING_HW_H 0x3804
#define OV5642_REG_TIMING_HW_L 0x3805
#define OV5642_REG_TIMING_VH_H 0x3806
#define OV5642_REG_TIMING_VH_L 0x3807

#define OV5642_REG_ISP_CTL_01  0x5001
#define OV5642_CMD_ISP_CTL_DEF 0xFF
#define OV5642_CMD_ISP_CTL_01  0xFF

#define OV5642_REG_SDE_CTL_0  0x5580
#define OV5642_REG_SDE_CTL_1  0x5581
#define OV5642_REG_SDE_CTL_2  0x5582
#define OV5642_REG_SDE_CTL_3  0x5583
#define OV5642_REG_SDE_CTL_4  0x5584
#define OV5642_REG_SDE_CTL_5  0x5585
#define OV5642_REG_SDE_CTL_6  0x5586
#define OV5642_REG_SDE_CTL_7  0x5587
#define OV5642_REG_SDE_CTL_8  0x5588
#define OV5642_REG_SDE_CTL_9  0x5589
#define OV5642_REG_SDE_CTL_10 0x558A

#define OV5642_CMD_SDE_CTL_0_DEF             0x00
#define OV5642_CMD_SDE_CTL_0_FILTER_NONE     0b00000110
#define OV5642_CMD_SDE_CTL_0_FILTER_INVERTED 0b01000110
#define OV5642_CMD_SDE_CTL_0_FILTER_GRAY     0b00100110
#define OV5642_CMD_SDE_CTL_0_FILTER_COLOR    0b00111110

extern uint16_t camera_buffer[LCD_WIDTH * LCD_HEIGHT] __attribute__((aligned(LCD_ALIGNMENT)));

void ov5642_init(uint8_t *ai_buffer);
void ov5642_enable_interrupts(void);
void ov5642_clear_frame_ready(void);
void ov5642_setup_ai_pipeline(uint8_t *ai_buffer);

uint8_t  ov5642_check_frame_ready(void);
uint16_t ov5642_read_id(void);

#endif