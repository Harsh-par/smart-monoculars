#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "fpioa.h"
#include "gpiohs.h"
#include "spi.h"
#include "sysctl.h"
#include "sleep.h"
#include "dvp.h"

#include "configuration.h"
#include "constant.h"
#include "ov5642.h"

#define ST7789_BLACK  0x0000
#define ST7789_WHITE  0xFFFF
#define ST7789_RED    0xF800
#define ST7789_GREEN  0x07E0
#define ST7789_GREEN2 0x0710
#define ST7789_TURQ   0x4f3e
#define ST7789_PINK   0xfaf9
#define ST7789_ORANGE 0xfc47
#define ST7789_BLUE   0x001F
#define ST7789_YELLOW 0xFFE0

#define ST7789_CMD_SWRESET 0x01
#define ST7789_CMD_SLPOUT  0x11
#define ST7789_CMD_INVOFF  0x20
#define ST7789_CMD_DISPON  0x29
#define ST7789_CMD_CASET   0x2A
#define ST7789_CMD_RASET   0x2B
#define ST7789_CMD_RAMWR   0x2C
#define ST7789_CMD_RAMWRC  0x3C
#define ST7789_CMD_MADCTL  0x36
#define ST7789_CMD_COLMOD  0x3A
#define ST7789_CMD_INVON   0x21

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t *buffer;
} st7789_t;

extern st7789_t st7789;

void st7789_init(st7789_t *st7789, uint16_t *buffer, uint16_t width, uint16_t height);
void st7789_set_window(st7789_t *st7789, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void st7789_fill_screen(st7789_t *st7789);

void st7789_draw_pixel(st7789_t *st7789, uint16_t x, uint16_t y, uint16_t color);
void st7789_draw_rectangle(st7789_t *st7789, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void st7789_draw_line(st7789_t *st7789, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

void st7789_write_ascii(st7789_t *st7789, uint16_t x, uint16_t y, char ascii, uint16_t color);
void st7789_write_text(st7789_t *st7789, uint16_t x, uint16_t y, const char *text, uint16_t color);

#endif