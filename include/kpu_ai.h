#ifndef KPU_AI_H
#define KPU_AI_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "kpu.h"
#include "dmac.h"
#include "spi.h"

#include "region_layer.h"
#include "w25qxx.h"

#include "configuration.h"
#include "constant.h"
#include "st7789.h"

#define KMODEL_FLASH_ADDRESS 0x00C00000
#define KMODEL_SIZE_BYTES    1351592 

#define YOLO_ANCHOR_NUMBERS  5
#define YOLO_GRID_WIDTH      10
#define YOLO_GRID_HEIGHT     7
#define YOLO_TENSOR_CHANNELS 125
#define YOLO_ANCHOR_NUMBERS  5

#define REGION_LAYER_THRESHOLD (0.65f)
#define REGION_LAYER_NMS_VALUE (0.30f)

extern uint8_t ai_buffer[LCD_WIDTH * LCD_HEIGHT * RGB_CHANNELS] __attribute__((aligned(LCD_ALIGNMENT)));

void kpu_ai_init(void);
void kpu_ai_run_inference(void);
void kpu_ai_process_results(volatile obj_info_t *obj_info);
void kpu_ai_draw_bounding_box(st7789_t *st7789, volatile obj_info_t *obj_info, uint16_t color);

#endif