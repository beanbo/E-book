#pragma once
#include <Arduino.h>
#include <string.h>

class Menu
{
public:
	enum BookState { MENU, BOOK };
	
	void Show();
	void FindBooks();
	void DrawBooks();
	void HitTest(uint16_t x, uint16_t y);
	
protected:
	String** m_pBookNames = nullptr;
	int m_nBooksCount = 0;
	BookState m_state = MENU;
};