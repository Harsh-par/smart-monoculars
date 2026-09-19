#include "st7789.h"
#include "font.h"

st7789_t st7789;

static uint8_t dma_buffer_command __attribute__((aligned(8)));;
static uint8_t dma_buffer_data[4] __attribute__((aligned(8)));;

static inline void write_command(uint8_t command)
{
    uint8_t *non_cached_command = (uint8_t *)((uintptr_t)&dma_buffer_command - NON_CACHED_MEMORY_OFFSET);
    *non_cached_command = command;

    gpiohs_set_pin(LCD_GPIO_DC, GPIO_LOW);

    spi_init(SPI_DEVICE_0, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(SPI_DEVICE_0, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);

    spi_send_data_normal_dma(DMAC_CHANNEL0, SPI_DEVICE_0, SPI_CHIP_SELECT_3, non_cached_command, 1, SPI_TRANS_CHAR);
}

static inline void write_data(const uint8_t *data, size_t length)
{
    if(!length) return;

    gpiohs_set_pin(LCD_GPIO_DC, GPIO_HIGH);

    spi_init(SPI_DEVICE_0, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(SPI_DEVICE_0, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);

    if(length <= 4) 
    {
        uint8_t *non_cached_data = (uint8_t *)((uintptr_t)dma_buffer_data - NON_CACHED_MEMORY_OFFSET);

        for(size_t i = 0; i < length; i++) 
        { 
            non_cached_data[i] = data[i]; 
        }

        spi_send_data_normal_dma(DMAC_CHANNEL0, SPI_DEVICE_0, SPI_CHIP_SELECT_3, non_cached_data, length, SPI_TRANS_CHAR);
    } 
    else 
    {
        spi_send_data_normal_dma(DMAC_CHANNEL0, SPI_DEVICE_0, SPI_CHIP_SELECT_3, data, length, SPI_TRANS_CHAR);
    }
}

static inline void write_data_byte(uint8_t data)
{ 
    write_data(&data, sizeof(data)); 
}

void st7789_set_window(st7789_t *st7789, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t buffer[4];

    write_command(ST7789_CMD_CASET);

    buffer[0] = x0 >> 8; 
    buffer[1] = x0; 
    buffer[2] = x1 >> 8; 
    buffer[3] = x1;

    write_data(buffer, sizeof(buffer));

    write_command(ST7789_CMD_RASET);

    buffer[0] = y0 >> 8; 
    buffer[1] = y0; 
    buffer[2] = y1 >> 8; 
    buffer[3] = y1;

    write_data(buffer, sizeof(buffer));
}

void st7789_fill_screen(st7789_t *st7789)
{
    uint32_t total_words = (st7789->width * st7789->height) >> 1; 
    uint32_t *buffer = (uint32_t*)st7789->buffer;

    uint32_t chunk_size = 3840; 

    for(uint32_t i = 0; i < total_words; i += chunk_size)
    {
        if(i == 0)
        {
            write_command(ST7789_CMD_RAMWR);
        } 
        else write_command(ST7789_CMD_RAMWRC); 
        

        gpiohs_set_pin(LCD_GPIO_DC, GPIO_HIGH);

        spi_init(SPI_DEVICE_0, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 0);
        spi_init_non_standard(SPI_DEVICE_0, 0, 32, 0, SPI_AITM_AS_FRAME_FORMAT);

        uint32_t length = (i + chunk_size < total_words) ? chunk_size : (total_words - i);

        spi_send_data_normal_dma(DMAC_CHANNEL0, SPI_DEVICE_0, SPI_CHIP_SELECT_3, buffer + i, length, SPI_TRANS_INT);
    }
}

void st7789_init(st7789_t *st7789, uint16_t *buffer, uint16_t width, uint16_t height)
{
    st7789->width  = width;
    st7789->height = height;
    st7789->buffer = buffer;

    sysctl_set_spi0_dvp_data(true); 

    fpioa_set_function(LCD_PIN_CLK, FUNC_SPI0_SCLK);
    fpioa_set_function(LCD_PIN_CS,  FUNC_SPI0_SS3);
    fpioa_set_function(LCD_PIN_RST, FUNC_GPIOHS1);
    fpioa_set_function(LCD_PIN_DC,  FUNC_GPIOHS0);
    fpioa_set_function(LCD_PIN_BLK, FUNC_GPIOHS3);

    gpiohs_set_drive_mode(LCD_GPIO_DC,  GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(LCD_GPIO_RST, GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(LCD_GPIO_BLK, GPIO_DM_OUTPUT);

    gpiohs_set_drive_mode(LCD_GPIO_DC, GPIO_DM_OUTPUT);
    gpiohs_set_pin(LCD_GPIO_DC, GPIO_HIGH);

    spi_set_clk_rate(SPI_DEVICE_0, LCD_CLK_SPEED);

    gpiohs_set_pin(LCD_GPIO_RST, GPIO_LOW);  msleep(50);
    gpiohs_set_pin(LCD_GPIO_RST, GPIO_HIGH); msleep(120);

    write_command(ST7789_CMD_SWRESET); msleep(120);
    write_command(ST7789_CMD_SLPOUT);  msleep(120);

    write_command(ST7789_CMD_COLMOD); write_data_byte(0x05); 

    
    #ifdef NVG_LEFT_SCREEN 
        write_command(ST7789_CMD_MADCTL); write_data_byte(0x60);
    #endif 
    
    #ifdef NVG_RIGHT_SCREEN
        write_command(ST7789_CMD_MADCTL); write_data_byte(0xA0); 
    #endif
    
    write_command(ST7789_CMD_INVOFF);
    write_command(ST7789_CMD_DISPON); msleep(50);
    
    dvp_set_display_addr((uint32_t)st7789->buffer);

    for(int i=0; i<(st7789->width * st7789->height); i++) st7789->buffer[i] = ST7789_BLACK;

    st7789_set_window(st7789, 0, 0, (st7789->width - 1), (st7789->height - 1));
    st7789_fill_screen(st7789);

    gpiohs_set_pin(LCD_GPIO_BLK, GPIO_HIGH);

    ov5642_enable_interrupts();
}

void st7789_draw_pixel(st7789_t *st7789, uint16_t x, uint16_t y, uint16_t color)
{
    if(x < st7789->width && y < st7789->height) 
    {
        st7789->buffer[y * st7789->width + (x ^ 1)] = color;
    }
}

void st7789_draw_rectangle(st7789_t *st7789, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    for(uint16_t j = 0; j < h; j++) 
    {
        for(uint16_t i = 0; i < w; i++) 
        {
            uint16_t px = x + i;
            uint16_t py = y + j;
            
            if(px < st7789->width && py < st7789->height) 
            {
                st7789->buffer[py * st7789->width + (px ^ 1)] = color;
            }
        }
    }
}

void st7789_draw_line(st7789_t *st7789, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t dx = abs(x1 - x0);
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0);
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;

    while(true)
    {
        if(x0 >= 0 && x0 < st7789->width && y0 >= 0 && y0 < st7789->height)
        {
            st7789->buffer[y0 * st7789->width + (x0 ^ 1)] = color;
        }

        if(x0 == x1 && y0 == y1) break;
        
        int16_t e2 = err << 1;

        if(e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }

        if(e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void st7789_write_ascii(st7789_t *st7789, uint16_t x, uint16_t y, char ascii, uint16_t color)
{
    if(x >= st7789->width || y >= st7789->height) return;

    const unsigned char *char_data = font8x16[(uint8_t)ascii];

    for(uint8_t row = 0; row < ASCII_CHAR_HEIGHT; row++) 
    {
        if(y + row >= st7789->height) break;

        uint8_t row_pixels = char_data[row];

        for(uint8_t column = 0; column < ASCII_CHAR_WIDTH; column++) 
        {
            if(x + column >= st7789->width) break;

            if(row_pixels & (0b10000000 >> column)) 
            {
                st7789_draw_pixel(st7789, x + column, y + row, color);
            }
        }
    }
}

void st7789_write_text(st7789_t *st7789, uint16_t x, uint16_t y, const char *text, uint16_t color) 
{
    uint16_t current_x = x;
    uint16_t current_y = y;

    uint8_t letter_spacing = 0;

    while(*text) 
    {
        if(*text == '\n') 
        {
            current_x = x;         
            current_y = current_y + ASCII_CHAR_HEIGHT;
        } 
        else 
        {
            st7789_write_ascii(st7789, current_x, current_y, *text, color);
            
            current_x = current_x + ASCII_CHAR_WIDTH + letter_spacing;

            if(current_x + ASCII_CHAR_WIDTH > st7789->width) 
            {
                current_x = x;   
                current_y = current_y + ASCII_CHAR_HEIGHT;    
            }
        }

        if(current_y >= st7789->height) break;

        text++;
    }
}