#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define NVG_LEFT_SCREEN  
//#define NVG_RIGHT_SCREEN

#define SYSTEM_PLL0_FREQ 800000000UL
#define SYSTEM_PLL1_FREQ 400000000UL
#define SYSTEM_PLL2_FREQ 45158400UL

#define IMU_CLK_SPEED (400 * 1000)

#define IMU_PIN_SDA   34
#define IMU_PIN_SCL   35

#define LCD_CLK_SPEED (22 * 1000 * 1000)

#define LCD_PIN_CLK   39
#define LCD_PIN_CS    36
#define LCD_PIN_RST   37
#define LCD_PIN_DC    38
#define LCD_PIN_BLK   17

#define LCD_GPIO_DC   0
#define LCD_GPIO_RST  1
#define LCD_GPIO_BLK  3

#define CAM_PIN_SDA   40
#define CAM_PIN_SCL   41
#define CAM_PIN_RST   42
#define CAM_PIN_VSYNC 43  
#define CAM_PIN_PWDN  44  
#define CAM_PIN_HREF  45  
#define CAM_PIN_XCLK  46  
#define CAM_PIN_PCLK  47

#define CAM_GPIO_RST  4
#define CAM_GPIO_PWDN 5

#define MIC_PIN_BCLK  18  
#define MIC_PIN_WS    19 
#define MIC_PIN_DAT   20 

#define ROT_GPIO_BTN  6
#define ROT_GPIO_ENCA 7
#define ROT_GPIO_ENCB 8

#define ROT_PIN_BTN   22
#define ROT_PIN_ENCA  23
#define ROT_PIN_ENCB  24

#define DVP_OUT_KPU   0
#define DVP_OUT_LCD   1

#endif