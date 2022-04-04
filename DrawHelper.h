#pragma once
#include <Arduino.h>
#include "epd_driver.h"

enum alignment { LEFT, RIGHT, CENTER };

// Colors
#define White         0xFF
#define LightGrey     0xBB
#define Grey          0x88
#define DarkGrey      0x44
#define Black         0x00

extern GFXfont  currentFont;
extern uint8_t* framebuffer;

void InitializeScreen();

void drawString(int x, int y, const String& text, alignment align = LEFT);
void drawFastHLine(int16_t x0, int16_t y0, int length, uint16_t color);
void drawFastVLine(int16_t x0, int16_t y0, int length, uint16_t color);
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawCircle(int x0, int y0, int r, uint8_t color);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void drawPixel(int x, int y, uint8_t color);

void fillCircle(int x, int y, int r, uint8_t color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

void setFont(GFXfont const& font);

void UpdateScreen();
void ClearFrameBuffer();