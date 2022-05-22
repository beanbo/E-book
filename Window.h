#pragma once
#include <Arduino.h>
#include <string.h>
#include "FiraSans16.h"
#include "epd_driver.h"

class Rect
{
public:
	Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	Rect(const Rect& other);
	Rect();

	bool HitTest(uint16_t x, uint16_t y, uint16_t smooth = 0);

	uint16_t nX;
	uint16_t nY;
	uint16_t Width;
	uint16_t Height;
};

class Window
{
public:
	Window(Rect rc);
	Window();

	enum Visibility { Visible, Hidden };

	virtual void Draw();

	bool HitTest(uint16_t x, uint16_t y, uint16_t smooth = 0);

	void SetWindowPos(Rect rc);

	void ShowWindow();
	void HideWindow();

	void Select(bool bSelected = true);

	void SetFont(GFXfont const& font);
	GFXfont const& GetFont();

	void SetIcon(uint8_t* icon, uint32_t width, uint32_t height);

protected:
	Rect m_rcWindow;
	Visibility m_eVisibility = Visible;
	bool m_bSelected = false;
	GFXfont m_font = FiraSans16;

	uint32_t m_nIconWidth = 0;
	uint32_t m_nIconHeight = 0;
	uint8_t* m_pIcon = nullptr;
};
