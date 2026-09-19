#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <math.h>

#include "time.h"
#include "sysctl.h"

#include "audio.h"
#include "st7789.h"
#include "mpu6050.h"
#include "ov5642.h"
#include "constant.h"
#include "rotary_encoder.h"

#define GUI_SCREEN_W            320
#define GUI_SCREEN_H            240
#define GUI_HUD_CENTER_X        (GUI_SCREEN_W / 2)
#define GUI_HUD_CENTER_Y        (GUI_SCREEN_H / 2)
#define GUI_HUD_ROLL_LIMIT      60.0f
#define GUI_HUD_PITCH_LIMIT     45.0f
#define GUI_HUD_AIRCRAFT_WING   20
#define GUI_HUD_AIRCRAFT_HEIGHT 6
#define GUI_HUD_HORIZON_LENGTH  60
#define GUI_HUD_PITCH_SCALE     2.0f
#define GUI_HUD_ROLL_SCALE_Y    25
#define GUI_HUD_ROLL_RADIUS     90
#define GUI_HUD_TEXT_X          105
#define GUI_HUD_TEXT_Y          220

#define GUI_AUDIO_MAX_BINS   30
#define GUI_AUDIO_BAR_LENGTH 90
#define GUI_AUDIO_START_X    5
#define GUI_AUDIO_BASE_Y     235
#define GUI_AUDIO_MAX_H      80
#define GUI_AUDIO_LINE_Y     237
#define GUI_AUDIO_MAG_SCALE  2
#define GUI_AUDIO_BAR_WIDTH  2
#define GUI_AUDIO_BAR_EMPTY  1
#define GUI_AUDIO_BAR_TOTAL  3
#define GUI_AUDIO_MAG_START_X  5
#define GUI_AUDIO_DB_START_X   225

typedef enum {
    GUI_IMU,
    GUI_AUDIO,
    GUI_DATA,
    GUI_FILTER,
    GUI_ZOOM,
    GUI_5,
    GUI_6,
    GUI_7,
    GUI_8,
    GUI_9,
    GUI_TOTAL
} gui_layer_t;

typedef enum {
    FILTER_NONE,
    FILTER_GREEN,
    FILTER_RED,
    FILTER_BLUE, 
    FILTER_MONOCHROME,
    FILTER_INVERTED,
    FILTER_SEPIA,
    FILTER_TOTAL
} filter_t;

typedef enum {
    ZOOM_1p0x,
    ZOOM_1p5x,
    ZOOM_2p0x,
    ZOOM_3p0x,
    ZOOM_4p0x,
    ZOOM_TOTAL
} zoom_t;

void gui_display_overlay(st7789_t *st7789, uint8_t state, uint16_t layer, bool ai_running, uint16_t color, uint16_t fill_color);

void gui_display_hud_imu(st7789_t *st7789, mpu6050_t* mpu6050, uint16_t horizon_color, uint16_t hud_color);
void gui_display_hud_audio(st7789_t *st7789, uint16_t color);
void gui_display_hud_data(st7789_t *st7789, uint16_t fps_count_core0, uint16_t fps_count_core1, uint16_t obj_count, uint16_t color);
void gui_display_hud_filters(st7789_t *st7789, uint16_t *current_filter, char **text_buffer_filter);
void gui_display_hud_zoom(st7789_t *st7789, uint16_t *current_zoom, char **text_buffer_zoom);

void gui_draw_text_tag(st7789_t *st7789, uint16_t x, uint16_t y, const char *text, uint16_t color);

void gui_display_crosshair(st7789_t* st7789, uint16_t color);
void gui_display_fps(st7789_t* st7789, uint8_t fps_count, uint8_t obj_count, uint16_t color);
void gui_display_time(st7789_t* st7789, uint16_t color);

void gui_display_rot_enc(st7789_t* st7789, uint16_t state, uint16_t layer, uint16_t color);


#endif