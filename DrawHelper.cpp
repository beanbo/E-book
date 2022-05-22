#include "DrawHelper.h"
#include "logo.h"
#include "lilygo.h"
#include "esp_adc_cal.h" 
#include "sleep_img.h"
#include "load_img.h"


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

void drawStartImage(bool bClearAll)
{
    Rect_t area = {
        .x = (EPD_WIDTH - load_img_width) / 2,
        .y = (EPD_HEIGHT - load_img_height) / 2,
        .width = load_img_width,
        .height = load_img_height,
    };

    epd_poweron();
	
	if (bClearAll)
        epd_clear();
    else
        epd_clear_area(area);
	
    epd_draw_grayscale_image(area, (uint8_t*)load_img_data);
	
    epd_poweroff();
}

void drawSleepImage()
{
    Rect_t area = {
        .x = (EPD_WIDTH - sleep_img_width) / 2,
        .y = (EPD_HEIGHT - sleep_img_height) / 2,
        .width = sleep_img_width,
        .height = sleep_img_height,
    };

    epd_poweron();
    epd_clear();
    epd_draw_grayscale_image(area, (uint8_t*)sleep_img_data);
    epd_poweroff();
}

void drawString(int x, int y, const String& text, alignment align)
{
    int width = GetTextWidth(text);
	
    if (align == RIGHT)  x = x - width;
    if (align == CENTER) x = x - width / 2;
	
    int cursor_y = y;
    write_string(&currentFont, const_cast<char*>(text.c_str()), &x, &cursor_y, framebuffer);
}

void drawMultilineString(int x, int y, int w, int h, const String& text, alignment align)
{
    if (text.isEmpty())
        return;
	
    int maxY = y + h;
    y += currentFont.advance_y;
	
    char* token, * newstring, * tofree;
    tofree = newstring = strdup(text.c_str());
	
    if (align == CENTER)
        x += w / 2;
	
    if (newstring == NULL)
        return;
	
    String out;
    
    while ((token = strsep(&newstring, " .,;:!-_")) != NULL) 
    {
        do
        {
            if (GetTextWidth(out) > w)
            {
                int subIndex = out.length() - 1;

                do
                {
                    subIndex--;
                } 
                while (GetTextWidth(out.substring(0, subIndex)) > w);

                drawString(x, y, out.substring(0, subIndex), align);
                y += currentFont.advance_y;

                if (y > maxY)
                    return;

                out = out.substring(subIndex);
            }
            else
            {
                String temp = out + token;

                int tempWidth = GetTextWidth(temp);

                if (tempWidth > w)
                {
                    if (!out.isEmpty())
                    {
                        drawString(x, y, out, align);
                        y += currentFont.advance_y;

                        if (y > maxY)
                            return;

                        out = token;
                        out += " ";
                    }
                }
                else
                {
                    out += token;
                    out += " ";
                }
            }
        } while (GetTextWidth(out) > w);
    }

	if (!out.isEmpty())
        drawString(x, y, out, align);
}

void fillCircle(int x, int y, int r, uint8_t color)
{
    epd_fill_circle(x, y, r, color, framebuffer);
}

void drawFastHLine(int16_t x0, int16_t y0, int length, int16_t width, uint16_t color)
{
    for (int16_t i = 0; i < width; i++)
        epd_draw_hline(x0, y0 - i, length, color, framebuffer);
}

void drawFastVLine(int16_t x0, int16_t y0, int length, int16_t width, uint16_t color)
{
    for (int16_t i = 0; i < width; i++)
        epd_draw_vline(x0 + i, y0, length, color, framebuffer);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    epd_write_line(x0, y0, x1, y1, color, framebuffer);
}

void drawCircle(int x0, int y0, int r, uint8_t color)
{
    epd_draw_circle(x0, y0, r, color, framebuffer);
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t width, uint16_t color)
{
	for (int16_t i = 0; i < width; i++)
        epd_draw_rect(x + i, y + i, w - i * 2, h - i * 2, color, framebuffer);
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
    epd_draw_pixel(x, y, color, nullptr);
}

void DrawBattery(int x, int y, int w, int h)
{
    static uint32_t vref = 1100;
    uint8_t percentage = 100;
	
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
	
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) 
    {
        Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
        vref = adc_chars.vref;
    }
	
    float voltage = analogRead(36) / 4096.0 * 6.566 * (vref / 1000.0);
	
    // Only display if there is a valid reading
    if (voltage > 1) 
    { 
        Serial.println("\nVoltage = " + String(voltage));
        percentage = 2836.9625 * pow(voltage, 4) - 43987.4889 * pow(voltage, 3) + 255233.8134 * pow(voltage, 2) - 656689.7123 * voltage + 632041.7303;
        if (voltage >= 4.20) percentage = 100;
        if (voltage <= 3.20) percentage = 0;  // orig 3.5

        drawRect(x, y, w * 0.9, h, 2);
        fillRect(x + w * 0.9, y + h * 0.25, w * 0.1, h / 2);
        fillRect(x, y, w * 0.9 * percentage / 100.0, h);
    }
}

void DrawWiFi(int x, int y, int rssi)
{
    int WIFIsignal = 0;
    int xpos = 1;
    for (int _rssi = -100; _rssi <= rssi; _rssi = _rssi + 20) {
        if (_rssi <= -20)  WIFIsignal = 30; //            <-20dbm displays 5-bars
        if (_rssi <= -40)  WIFIsignal = 24; //  -40dbm to  -21dbm displays 4-bars
        if (_rssi <= -60)  WIFIsignal = 18; //  -60dbm to  -41dbm displays 3-bars
        if (_rssi <= -80)  WIFIsignal = 12; //  -80dbm to  -61dbm displays 2-bars
        if (_rssi <= -100) WIFIsignal = 6;  // -100dbm to  -81dbm displays 1-bar
        fillRect(x + xpos * 8, y + 30 - WIFIsignal, 6, WIFIsignal);
        xpos++;
    }
}

void DrawImage(Rect_t image_area, uint8_t* image_data)
{
    epd_copy_to_framebuffer(image_area, image_data, framebuffer);
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

int GetTextWidth(const String& text)
{
    if (text.isEmpty())
        return 0;
	
    char* data = const_cast<char*>(text.c_str());
    int x1, y1; //the bounds of x,y and w and h of the variable 'text' in pixels.
    int ww, hh;
    int xx = 0, yy = 0;
    get_text_bounds(&currentFont, data, &xx, &yy, &x1, &y1, &ww, &hh, NULL);
	
    return ww;
}
