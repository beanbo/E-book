#include "DrawHelper.h"
#include "logo.h"
#include "lilygo.h"

GFXfont  currentFont;
uint8_t* framebuffer = nullptr;

void InitializeScreen()
{
    epd_init();
	
    framebuffer = (uint8_t*)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer)
    {
        Serial.println("alloc memory failed !!!");
        while (1);
    }

    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
}

void drawStartImage()
{
    Rect_t area = {
        .x = (EPD_WIDTH - lilygo_width) / 2,
        .y = (EPD_HEIGHT - lilygo_height) / 2,
        .width = lilygo_width,
        .height = lilygo_height
    };

    epd_poweron();
    epd_clear();
    epd_draw_grayscale_image(area, (uint8_t*)lilygo_data);
    epd_poweroff();
}

void drawSleepImage()
{	
    Rect_t area = {
        .x = (EPD_WIDTH - logo_width) / 2,
        .y = (EPD_HEIGHT - logo_height) / 2,
        .width = logo_width,
        .height = logo_height,
    };

    epd_poweron();
    epd_clear();
    epd_draw_grayscale_image(area, (uint8_t*)logo_data);
    epd_poweroff();
}

void drawString(int x, int y, const String& text, alignment align)
{
    char* data = const_cast<char*>(text.c_str());
    int x1, y1; //the bounds of x,y and w and h of the variable 'text' in pixels.
    int w, h;
    int xx = x, yy = y;
    get_text_bounds(&currentFont, data, &xx, &yy, &x1, &y1, &w, &h, NULL);
    if (align == RIGHT)  x = x - w;
    if (align == CENTER) x = x - w / 2;
    int cursor_y = y;
    write_string(&currentFont, data, &x, &cursor_y, framebuffer);
}

void fillCircle(int x, int y, int r, uint8_t color)
{
    epd_fill_circle(x, y, r, color, framebuffer);
}

void drawFastHLine(int16_t x0, int16_t y0, int length, uint16_t color)
{
    epd_draw_hline(x0, y0, length, color, framebuffer);
}

void drawFastVLine(int16_t x0, int16_t y0, int length, uint16_t color)
{
    epd_draw_vline(x0, y0, length, color, framebuffer);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    epd_write_line(x0, y0, x1, y1, color, framebuffer);
}

void drawCircle(int x0, int y0, int r, uint8_t color)
{
    epd_draw_circle(x0, y0, r, color, framebuffer);
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    epd_draw_rect(x, y, w, h, color, framebuffer);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    epd_fill_rect(x, y, w, h, color, framebuffer);
}

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    epd_fill_triangle(x0, y0, x1, y1, x2, y2, color, framebuffer);
}

void drawPixel(int x, int y, uint8_t color)
{
    epd_draw_pixel(x, y, color, framebuffer);
}

void setFont(GFXfont const& font)
{
    currentFont = font;
}

void UpdateScreen()
{
    epd_draw_grayscale_image(epd_full_screen(), framebuffer);
}

void ClearFrameBuffer()
{
    fillRect(0, 0, EPD_WIDTH, EPD_HEIGHT, White);
}
