#include "gui.h"

void gui_display_overlay(st7789_t *st7789, uint8_t state, uint16_t layer, bool ai_running, uint16_t color, uint16_t fill_color)
{
    #define GUI_OVERLAY_LAYER_X 2
    #define GUI_OVERLAY_LAYER_Y 2
    #define GUI_OVERLAY_LAYER_W 60
    #define GUI_OVERLAY_LAYER_H 10 

    #define GUI_OVERLAY_LAYER_BOX_X 4
    #define GUI_OVERLAY_LAYER_BOX_Y 4
    #define GUI_OVERLAY_LAYER_BOX_W 4
    #define GUI_OVERLAY_LAYER_BOX_W_PAD 2
    #define GUI_OVERLAY_LAYER_BOX_H 6

    st7789_draw_line(st7789, GUI_OVERLAY_LAYER_X, GUI_OVERLAY_LAYER_Y, GUI_OVERLAY_LAYER_X + GUI_OVERLAY_LAYER_W, GUI_OVERLAY_LAYER_Y, color);
    st7789_draw_line(st7789, GUI_OVERLAY_LAYER_X, GUI_OVERLAY_LAYER_Y + GUI_OVERLAY_LAYER_H, GUI_OVERLAY_LAYER_X + GUI_OVERLAY_LAYER_W, GUI_OVERLAY_LAYER_Y + GUI_OVERLAY_LAYER_H, color);
    st7789_draw_line(st7789, GUI_OVERLAY_LAYER_X, GUI_OVERLAY_LAYER_Y, GUI_OVERLAY_LAYER_X, GUI_OVERLAY_LAYER_Y + GUI_OVERLAY_LAYER_H, color);
    st7789_draw_line(st7789, GUI_OVERLAY_LAYER_X + GUI_OVERLAY_LAYER_W, GUI_OVERLAY_LAYER_Y, GUI_OVERLAY_LAYER_X + GUI_OVERLAY_LAYER_W, GUI_OVERLAY_LAYER_Y + GUI_OVERLAY_LAYER_H, color);

    uint16_t layer_box_x = GUI_OVERLAY_LAYER_BOX_X + (layer * (GUI_OVERLAY_LAYER_BOX_W + GUI_OVERLAY_LAYER_BOX_W_PAD));
    uint16_t layer_box_y = GUI_OVERLAY_LAYER_BOX_Y;

    st7789_draw_rectangle(st7789, layer_box_x, layer_box_y, GUI_OVERLAY_LAYER_BOX_W, GUI_OVERLAY_LAYER_BOX_H, color);

    #define GUI_OVERLAY_BTN_X (GUI_OVERLAY_LAYER_X + GUI_OVERLAY_LAYER_W + GUI_OVERLAY_LAYER_BOX_W_PAD)
    #define GUI_OVERLAY_BTN_Y 2
    #define GUI_OVERLAY_BTN_W 10
    #define GUI_OVERLAY_BTN_H 10

    st7789_draw_line(st7789, GUI_OVERLAY_BTN_X, GUI_OVERLAY_BTN_Y, GUI_OVERLAY_BTN_X + GUI_OVERLAY_BTN_W, GUI_OVERLAY_BTN_Y, color);
    st7789_draw_line(st7789, GUI_OVERLAY_BTN_X, GUI_OVERLAY_BTN_Y + GUI_OVERLAY_BTN_H, GUI_OVERLAY_BTN_X + GUI_OVERLAY_BTN_W, GUI_OVERLAY_BTN_Y + GUI_OVERLAY_BTN_H, color);
    st7789_draw_line(st7789, GUI_OVERLAY_BTN_X, GUI_OVERLAY_BTN_Y, GUI_OVERLAY_BTN_X, GUI_OVERLAY_BTN_Y + GUI_OVERLAY_BTN_H, color);
    st7789_draw_line(st7789, GUI_OVERLAY_BTN_X + GUI_OVERLAY_BTN_W, GUI_OVERLAY_BTN_Y, GUI_OVERLAY_BTN_X + GUI_OVERLAY_BTN_W, GUI_OVERLAY_BTN_Y + GUI_OVERLAY_BTN_H, color);

    if(state)
    {
        uint16_t inner_x = GUI_OVERLAY_BTN_X + 2;
        uint16_t inner_y = GUI_OVERLAY_BTN_Y + 2;
        uint16_t inner_w = GUI_OVERLAY_BTN_W - 3; 
        uint16_t inner_h = GUI_OVERLAY_BTN_H - 3;
        st7789_draw_rectangle(st7789, inner_x, inner_y, inner_w, inner_h, color);
    }

    #define GUI_OVERLAY_AI_X (LCD_WIDTH - 42)
    #define GUI_OVERLAY_AI_Y 0

    char* ai_text;
    char* mode_text;

    if(ai_running)
    {
        ai_text = "AI[X]";
    }
    else ai_text = "AI[ ]";
    st7789_write_text(st7789, GUI_OVERLAY_AI_X, GUI_OVERLAY_AI_Y, ai_text, color);

    switch(layer)
    {
        case GUI_IMU:    mode_text = "imu"; break;
        case GUI_AUDIO:  mode_text = "audio"; break;
        case GUI_DATA:   mode_text = "data"; break;
        case GUI_FILTER: mode_text = "filter"; break;
        case GUI_ZOOM:   mode_text = "zoom"; break;
        case 5: mode_text = "5"; break;
        case 6: mode_text = "6"; break;
        case 7: mode_text = "7"; break;
        case 8: mode_text = "8"; break;
        case 9: mode_text = "none"; break;
        default: mode_text = "err"; break;
    }

    uint16_t mode_text_x = GUI_OVERLAY_LAYER_X;
    uint16_t mode_text_y = GUI_OVERLAY_LAYER_Y + GUI_OVERLAY_LAYER_H + 1;
    st7789_write_text(st7789, mode_text_x, mode_text_y, mode_text, color);
    st7789_draw_rectangle(st7789, mode_text_x, mode_text_y + 17, GUI_OVERLAY_LAYER_W, 2, color);
}

void gui_display_hud_imu(st7789_t *st7789, mpu6050_t* mpu6050, uint16_t horizon_color, uint16_t hud_color)
{
    float roll  = -mpu6050->kalman_roll;
    float pitch = -mpu6050->kalman_pitch;

    if(roll > GUI_HUD_ROLL_LIMIT)  roll =  GUI_HUD_ROLL_LIMIT;
    if(roll < -GUI_HUD_ROLL_LIMIT) roll = -GUI_HUD_ROLL_LIMIT;

    if(pitch > GUI_HUD_PITCH_LIMIT)  pitch =  GUI_HUD_PITCH_LIMIT;
    if(pitch < -GUI_HUD_PITCH_LIMIT) pitch = -GUI_HUD_PITCH_LIMIT;

    int16_t arrow_y = GUI_HUD_CENTER_Y - (int16_t)(pitch * GUI_HUD_PITCH_SCALE);

    if(arrow_y < 0) arrow_y = 0;
    if(arrow_y > GUI_SCREEN_H - 1) arrow_y = GUI_SCREEN_H - 1;

    st7789_write_ascii(st7789, GUI_HUD_CENTER_X - 4, arrow_y, '^', horizon_color);

    st7789_draw_line(st7789, GUI_HUD_CENTER_X - GUI_HUD_AIRCRAFT_WING, GUI_HUD_CENTER_Y, GUI_HUD_CENTER_X - 4, GUI_HUD_CENTER_Y, hud_color);
    st7789_draw_line(st7789, GUI_HUD_CENTER_X + 4, GUI_HUD_CENTER_Y, GUI_HUD_CENTER_X + GUI_HUD_AIRCRAFT_WING, GUI_HUD_CENTER_Y, hud_color);
    st7789_draw_line(st7789, GUI_HUD_CENTER_X, GUI_HUD_CENTER_Y - GUI_HUD_AIRCRAFT_HEIGHT, GUI_HUD_CENTER_X, GUI_HUD_CENTER_Y + GUI_HUD_AIRCRAFT_HEIGHT, hud_color);

    float roll_radians = roll * (float)(M_PI / 180.0f);

    int16_t mx = (int16_t)(GUI_HUD_CENTER_X + sinf(roll_radians) * GUI_HUD_ROLL_RADIUS);
    int16_t my = (int16_t)(GUI_HUD_ROLL_SCALE_Y + cosf(roll_radians) * GUI_HUD_ROLL_RADIUS);

    st7789_draw_line(st7789, mx, my, mx, my - 8, hud_color);

    #define GUI_IMU_HUD_TOTAL_ANGLES 11
    const int angles[] = { -60, -45, -30, -20, -10, 0, 10, 20, 30, 45, 60 };

    for(uint8_t i = 0; i < GUI_IMU_HUD_TOTAL_ANGLES; i++)
    {
        float angle = angles[i] * (float)(M_PI / 180.0f);

        int16_t _x0 = (int16_t)(GUI_HUD_CENTER_X + sinf(angle) * (GUI_HUD_ROLL_RADIUS - 5));
        int16_t _y0 = (int16_t)(GUI_HUD_ROLL_SCALE_Y + cosf(angle) * (GUI_HUD_ROLL_RADIUS - 5));

        int16_t _x1 = (int16_t)(GUI_HUD_CENTER_X + sinf(angle) * (GUI_HUD_ROLL_RADIUS + 5));
        int16_t _y1 = (int16_t)(GUI_HUD_ROLL_SCALE_Y + cosf(angle) * (GUI_HUD_ROLL_RADIUS + 5));

        st7789_draw_line(st7789, _x0, _y0, _x1, _y1, hud_color);
    }
    
    char text_buffer0[8];
    char text_buffer1[8];

    snprintf(text_buffer0, sizeof(text_buffer0), "R%+03.0f", roll);
    snprintf(text_buffer1, sizeof(text_buffer1), "P%+03.0f", pitch);

    gui_draw_text_tag(st7789, 2, 198, text_buffer0, hud_color);
    gui_draw_text_tag(st7789, 2, 218, text_buffer1, hud_color);
}

void gui_display_hud_audio(st7789_t *st7789, uint16_t color)
{
    static uint16_t magnitudes[GUI_AUDIO_MAX_BINS] = {0};
    audio_get_magnitudes(magnitudes, GUI_AUDIO_MAX_BINS);

    for(uint16_t length = 0; length < GUI_AUDIO_BAR_LENGTH; length++)
    {
        uint16_t x_mag = GUI_AUDIO_MAG_START_X + length;
        uint16_t x_db  = GUI_AUDIO_DB_START_X + length;
        
        if(x_mag < st7789->width && GUI_AUDIO_LINE_Y < st7789->height)
        {
            st7789->buffer[GUI_AUDIO_LINE_Y * st7789->width + x_mag] = color;
        }
        if(x_db < st7789->width && GUI_AUDIO_LINE_Y < st7789->height)
        {
            st7789->buffer[GUI_AUDIO_LINE_Y * st7789->width + x_db] = color;
        }
    }

    for(uint8_t i = 0; i < GUI_AUDIO_MAX_BINS; i++)
    {
        uint16_t h_mag = magnitudes[i] >> GUI_AUDIO_MAG_SCALE;
        if(h_mag > GUI_AUDIO_MAX_H) h_mag = GUI_AUDIO_MAX_H;

        uint16_t x_mag = GUI_AUDIO_MAG_START_X + (i * GUI_AUDIO_BAR_TOTAL);

        for(uint16_t y_off = 0; y_off < h_mag; y_off++)
        {
            uint16_t y = GUI_AUDIO_BASE_Y - y_off;
            if(x_mag < (st7789->width - 1) && y < st7789->height)
            {
                st7789->buffer[y * st7789->width + x_mag]       = color;
                st7789->buffer[y * st7789->width + (x_mag + 1)] = color;
            }
        }

        float db = -96.0f;
        if(magnitudes[i] > 0) 
        {
            db = 20.0f * log10f((float)magnitudes[i] / 32767.0f);
        }

        if(db > 0.0f) db = 0.0f;

        uint16_t h_db = (uint16_t)((db + 96.0f) * (float)GUI_AUDIO_MAX_H / 96.0f);

        if(h_db > GUI_AUDIO_MAX_H) h_db = GUI_AUDIO_MAX_H;

        uint16_t x_db = GUI_AUDIO_DB_START_X + (i * GUI_AUDIO_BAR_TOTAL);

        for(uint16_t y_off = 0; y_off < h_db; y_off++)
        {
            uint16_t y = GUI_AUDIO_BASE_Y - y_off;
            if(x_db < (st7789->width - 1) && y < st7789->height)
            {
                st7789->buffer[y * st7789->width + x_db]       = color;
                st7789->buffer[y * st7789->width + (x_db + 1)] = color;
            }
        }
    }

    #define GUI_AUD_HUD_X 2
    #define GUI_AUD_HUD_Y1 52
    #define GUI_AUD_HUD_Y2 70
    #define GUI_AUD_HUD_Y3 88

    char text_buffer0[20];
    char text_buffer1[20];
    char text_buffer2[20];

    uint16_t frequency  = audio_get_peak_frequency();
    uint32_t rms_volume = audio_get_rms_volume();
    float db_volume = AUDIO_DB_NOISE_FLOOR;

    if(rms_volume > 0) 
    {
        db_volume = 20.0f * log10f((float)rms_volume / 32767.0f);
        if(db_volume > 0.0f) db_volume = 0.0f;
    }

    snprintf(text_buffer0, sizeof(text_buffer0), "FRQ:%6dHz", frequency);
    snprintf(text_buffer1, sizeof(text_buffer1), "RMS:%6d  ", rms_volume);
    snprintf(text_buffer2, sizeof(text_buffer2), "VOL:%6.1fdB", db_volume);

    gui_draw_text_tag(st7789, GUI_AUD_HUD_X, GUI_AUD_HUD_Y1, text_buffer0, color);
    gui_draw_text_tag(st7789, GUI_AUD_HUD_X, GUI_AUD_HUD_Y2, text_buffer1, color);
    gui_draw_text_tag(st7789, GUI_AUD_HUD_X, GUI_AUD_HUD_Y3, text_buffer2, color);
 
    gui_display_time(st7789, color);
}

void gui_display_hud_data(st7789_t *st7789, uint16_t fps_count_core0, uint16_t fps_count_core1, uint16_t obj_count, uint16_t color)
{
    #define GUI_DATA_TEXT_X  2
    #define GUI_DATA_TEXT_Y0 52
    #define GUI_DATA_TEXT_Y1 72
    #define GUI_DATA_TEXT_Y2 92

    char text_buffer0[24];
    char text_buffer1[24];
    char text_buffer2[24];

    snprintf(text_buffer0, sizeof(text_buffer0), "FPS Core0 [%02d]", fps_count_core0);
    snprintf(text_buffer1, sizeof(text_buffer1), "FPS Core1 [%02d]", fps_count_core1);
    snprintf(text_buffer2, sizeof(text_buffer2), "OBJ : %02d", obj_count);
    
    gui_draw_text_tag(st7789, GUI_DATA_TEXT_X, GUI_DATA_TEXT_Y0, text_buffer0, color);
    gui_draw_text_tag(st7789, GUI_DATA_TEXT_X, GUI_DATA_TEXT_Y1, text_buffer1, color);
    gui_draw_text_tag(st7789, GUI_DATA_TEXT_X, GUI_DATA_TEXT_Y2, text_buffer2, color);

    gui_display_time(st7789, color);
}

void gui_display_hud_filters(st7789_t *st7789, uint16_t *current_filter, char **text_buffer_filter)
{
    *current_filter = ((*current_filter) + 1) % FILTER_TOTAL;

    switch(*current_filter)
    {
        case FILTER_NONE: 
            *text_buffer_filter = "Filter : None";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_ISP_CTL_01, OV5642_CMD_ISP_CTL_01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_0,  OV5642_CMD_SDE_CTL_0_FILTER_NONE);
            break;

        case FILTER_GREEN: 
            *text_buffer_filter = "Filter : Green";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_ISP_CTL_01, OV5642_CMD_ISP_CTL_01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_0,  OV5642_CMD_SDE_CTL_0_FILTER_COLOR);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_5,  0x40);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_6,  0x40);
            break;

        case FILTER_RED: 
            *text_buffer_filter = "Filter : Red";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_ISP_CTL_01, OV5642_CMD_ISP_CTL_01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_0,  OV5642_CMD_SDE_CTL_0_FILTER_COLOR);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_5,  0x80);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_6,  0xE0);
            break;

        case FILTER_BLUE: 
            *text_buffer_filter = "Filter : Blue";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_ISP_CTL_01, OV5642_CMD_ISP_CTL_01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_0,  OV5642_CMD_SDE_CTL_0_FILTER_COLOR);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_5,  0xE0);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_6,  0x80);
            break;

        case FILTER_MONOCHROME: 
            *text_buffer_filter = "Filter : Monochrome";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_ISP_CTL_01, OV5642_CMD_ISP_CTL_01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_0,  OV5642_CMD_SDE_CTL_0_FILTER_GRAY);
            break;

        case FILTER_INVERTED: 
            *text_buffer_filter = "Filter : Inverted";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_ISP_CTL_01, OV5642_CMD_ISP_CTL_01); 
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_0,  OV5642_CMD_SDE_CTL_0_FILTER_INVERTED); 
            break;

        case FILTER_SEPIA: 
            *text_buffer_filter = "Filter : Sepia";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_ISP_CTL_01, OV5642_CMD_ISP_CTL_01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_0,  OV5642_CMD_SDE_CTL_0_FILTER_COLOR);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_5,  0x40);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_SDE_CTL_6,  0xA0);
            break;
    }
}

void gui_display_hud_zoom(st7789_t *st7789, uint16_t *current_zoom, char **text_buffer_zoom)
{
    *current_zoom = ((*current_zoom) + 1) % ZOOM_TOTAL;

    switch(*current_zoom)
    {
        case ZOOM_1p0x:
            *text_buffer_zoom = "Zoom : 1.0x";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HS_H, 0x00);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HS_L, 0x50);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VS_H, 0x00);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VS_L, 0x08);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HW_H, 0x05);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HW_L, 0x00);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VH_H, 0x03);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VH_L, 0xC0);
            break;

        case ZOOM_1p5x:
            *text_buffer_zoom = "Zoom : 1.5x";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HS_H, 0x01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HS_L, 0x24);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VS_H, 0x00);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VS_L, 0xA7);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HW_H, 0x03);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HW_L, 0x58);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VH_H, 0x02);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VH_L, 0x82);
            break;

        case ZOOM_2p0x:
            *text_buffer_zoom = "Zoom : 2.0x";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HS_H, 0x01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HS_L, 0x90);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VS_H, 0x00);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VS_L, 0xF8);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HW_H, 0x02);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HW_L, 0x80);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VH_H, 0x01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VH_L, 0xE0);
            break;

        case ZOOM_3p0x:
            *text_buffer_zoom = "Zoom : 3.0x";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HS_H, 0x01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HS_L, 0xFC);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VS_H, 0x01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VS_L, 0x49);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HW_H, 0x01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HW_L, 0xA8);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VH_H, 0x01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VH_L, 0x3E);
            break;

        case ZOOM_4p0x:
            *text_buffer_zoom = "Zoom : 4.0x";
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HS_H, 0x02);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HS_L, 0x30);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VS_H, 0x01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VS_L, 0x70);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HW_H, 0x01);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_HW_L, 0x40);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VH_H, 0x00);
            dvp_sccb_send_data(OV5642_ADDRESS, OV5642_REG_TIMING_VH_L, 0xF0);
            break;
    }
}

void gui_draw_text_tag(st7789_t *st7789, uint16_t x, uint16_t y, const char *text, uint16_t color)
{
    #define ASCII_CHAR_BYTES  16
    #define ASCII_CHAR_HEIGHT 16
    #define ASCII_CHAR_WIDTH  8

    uint32_t current_length = 0;
    uint32_t maximum_length = 0;
    
    for(const char *current_char = text; *current_char != '\0'; current_char++) 
    {
        if(*current_char == '\n') 
        {
            current_length = 0;
        } 
        else 
        {
            current_length++;

            if(current_length > maximum_length) maximum_length = current_length;
        }
    }

    uint16_t rectangle_w = (maximum_length * ASCII_CHAR_WIDTH) + 8;
    uint16_t rectangle_h = ASCII_CHAR_HEIGHT + 2;

    uint16_t triangle_size = 6;

    st7789_draw_line(st7789, x, y, x + rectangle_w, y, color);
    st7789_draw_line(st7789, x, y + rectangle_h, x + rectangle_w, y + rectangle_h, color);
    st7789_draw_line(st7789, x, y, x, y + rectangle_h, color);
    st7789_draw_line(st7789, x + rectangle_w, y, x + rectangle_w + triangle_size, y + (rectangle_h / 2), color);
    st7789_draw_line(st7789, x + rectangle_w + triangle_size, y + (rectangle_h / 2), x + rectangle_w, y + rectangle_h, color);

    st7789_write_text(st7789, x + 4, y + 1, text, color);
}

void gui_display_fps(st7789_t *st7789, uint8_t fps_count, uint8_t obj_count, uint16_t color)
{
    char text_buffer[30];

    snprintf(text_buffer, sizeof(text_buffer), "FPS:%02d\nOBJ:%02d\n", fps_count, obj_count);
    
    st7789_write_text(st7789, 2, 50, text_buffer, color);
}

void gui_display_time(st7789_t *st7789, uint16_t color)
{
    #define GUI_AUD_HUD_X 2
    #define GUI_AUD_HUD_Y 33

    uint64_t time_elapsed = sysctl_get_time_us() * MICROSECOND_TO_SECOND;

    char text_buffer[16];

    int hours   = time_elapsed / 3600;
    int minutes = (time_elapsed % 3600) / 60;
    int seconds = time_elapsed % 60;

    snprintf(text_buffer, sizeof(text_buffer), "%02d:%02d:%02d", hours, minutes, seconds);

    gui_draw_text_tag(st7789, GUI_AUD_HUD_X, GUI_AUD_HUD_Y, text_buffer, color);
}

void gui_display_rot_enc(st7789_t *st7789, uint16_t state, uint16_t layer, uint16_t color)
{
    char text_buffer[32];
    snprintf(text_buffer, sizeof(text_buffer), "S:%d\nL:%d", state, layer);
    st7789_write_text(st7789, 294, 2, text_buffer, color);
}

void gui_display_crosshair(st7789_t* st7789, uint16_t color)
{
    st7789_write_text(st7789, GUI_HUD_CENTER_X - 20, GUI_HUD_CENTER_Y, "< + >", color);
}