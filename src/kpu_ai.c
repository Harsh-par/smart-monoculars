#include "kpu_ai.h"

uint8_t ai_buffer[LCD_WIDTH * LCD_HEIGHT * RGB_CHANNELS] __attribute__((aligned(LCD_ALIGNMENT)));

static uint8_t *model_data = NULL;
static kpu_model_context_t kpu_model_context;
static region_layer_t region_layer;

static volatile bool kpu_done_flag = 0;

static float yolo_anchors[YOLO_ANCHOR_NUMBERS * 2] = { 1.08, 1.19,  3.42, 4.41,  6.63, 11.38,  9.42, 5.11,  16.62, 10.52 };

static void kpu_dma_callback(void *parameter)
{
    kpu_done_flag = true;
}

void kpu_ai_init(void)
{
    model_data = (uint8_t *)malloc(KMODEL_SIZE_BYTES + (LCD_ALIGNMENT - 1));

    if(model_data == NULL) return;

    uint8_t *model_aligned = (uint8_t *)(((uintptr_t)model_data + (LCD_ALIGNMENT - 1)) & ~(LCD_ALIGNMENT - 1));

    w25qxx_init(SPI_DEVICE_3, SPI_CHIP_SELECT_0);
    w25qxx_enable_quad_mode(); 
    w25qxx_read_data(KMODEL_FLASH_ADDRESS, model_aligned, KMODEL_SIZE_BYTES, W25QXX_QUAD_FAST); 

    if(kpu_load_kmodel(&kpu_model_context, model_aligned) != 0) return;

    region_layer.anchor_number = YOLO_ANCHOR_NUMBERS;
    region_layer.anchor  = yolo_anchors;
    region_layer.threshold = REGION_LAYER_THRESHOLD; 
    region_layer.nms_value = REGION_LAYER_NMS_VALUE; 
    
    if(region_layer_init(&region_layer, YOLO_GRID_WIDTH, YOLO_GRID_HEIGHT, YOLO_TENSOR_CHANNELS, LCD_WIDTH, LCD_HEIGHT) != 0) return;
}

void kpu_ai_run_inference(void)
{
    kpu_done_flag = false;

    kpu_run_kmodel(&kpu_model_context, ai_buffer, DMAC_CHANNEL5, (kpu_done_callback_t)kpu_dma_callback, NULL);

    while(!kpu_done_flag);
}

void kpu_ai_process_results(volatile obj_info_t *obj_info)
{
    float *output_tensors;
    size_t output_size;

    kpu_get_output(&kpu_model_context, 0, (uint8_t **)&output_tensors, &output_size);
    
    region_layer.input = output_tensors;

    region_layer_run(&region_layer, obj_info);
}

static inline void draw_bounding_box(uint16_t *buffer, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) 
{
    if(x1 > x2)
    { 
        uint16_t tmp = x1; 
        x1 = x2; 
        x2 = tmp; 
    }

    if(y1 > y2) 
    { 
        uint16_t tmp = y1; 
        y1 = y2; 
        y2 = tmp; 
    }

    if(x1 < 1) x1 = 1;
    if(x2 >= LCD_WIDTH - 1) x2 = LCD_WIDTH - 2;

    if(y1 < 1) y1 = 1;
    if(y2 >= LCD_HEIGHT - 1) y2 = LCD_HEIGHT - 2;

    if(x1 >= x2 || y1 >= y2) return;

    uint16_t corner_len = 15;
    
    if(corner_len > (x2 - x1) / 2) corner_len = (x2 - x1) / 2;
    if(corner_len > (y2 - y1) / 2) corner_len = (y2 - y1) / 2;

    for(uint16_t offset = 0; offset <= corner_len; offset++) 
    {
        buffer[y1 * LCD_WIDTH + (x1 + offset)]       = color;
        buffer[(y1 + 1) * LCD_WIDTH + (x1 + offset)] = color;
        
        buffer[y1 * LCD_WIDTH + (x2 - offset)]       = color;
        buffer[(y1 + 1) * LCD_WIDTH + (x2 - offset)] = color;
        
        buffer[y2 * LCD_WIDTH + (x1 + offset)]       = color;
        buffer[(y2 - 1) * LCD_WIDTH + (x1 + offset)] = color;
        
        buffer[y2 * LCD_WIDTH + (x2 - offset)]       = color;
        buffer[(y2 - 1) * LCD_WIDTH + (x2 - offset)] = color;
    }

    for(uint16_t offset = 0; offset <= corner_len; offset++) 
    {
        buffer[(y1 + offset) * LCD_WIDTH + x1]       = color;
        buffer[(y1 + offset) * LCD_WIDTH + (x1 + 1)] = color;
        
        buffer[(y1 + offset) * LCD_WIDTH + x2]       = color;
        buffer[(y1 + offset) * LCD_WIDTH + (x2 - 1)] = color;
        
        buffer[(y2 - offset) * LCD_WIDTH + x1]       = color;
        buffer[(y2 - offset) * LCD_WIDTH + (x1 + 1)] = color;
        
        buffer[(y2 - offset) * LCD_WIDTH + x2]       = color;
        buffer[(y2 - offset) * LCD_WIDTH + (x2 - 1)] = color;
    }
}

void kpu_ai_draw_bounding_box(st7789_t *st7789, volatile obj_info_t *obj_info, uint16_t color)
{
    for(uint32_t i = 0; i < obj_info->obj_number; i++) 
    {
        uint16_t x1 = obj_info->obj[i].x1;
        uint16_t y1 = obj_info->obj[i].y1;
        uint16_t x2 = obj_info->obj[i].x2;
        uint16_t y2 = obj_info->obj[i].y2;
        
        draw_bounding_box(st7789->buffer, x1, y1, x2, y2, color);
    }
}