#pragma once
#include <Arduino.h>
#include <string.h>
#include "firasans.h"
#include "Page.h"
#include "Window.h"


class TriangleGlyph : public Window
{
public:
	enum Orientation { Left, Right, Up, Down };
	
	void SetOrientation(Orientation orientation);
	
	void Draw() override;

protected:
	Orientation m_orientation = Left;
};

class MenuBook : public Window
{
public:
	MenuBook();

	void SetTitle(String sFullName);

	String GetFullName();

	void Draw() override;

protected:
	String m_sTitle;
	String m_sFullName;
};

class SelectionButton : public Window
{
public:

	void Draw() override;

	void SetTitle(String sTitle);

protected:
	String m_sTitle;
};

class MainView
{
public:
	MainView();
	
	enum BookState { MENU, BOOK, OPTIONS };
	
	void Show();
	void FindBooks();
	void DrawMainPage();
	void DrawBooks();
	void DrawOptions();
	void HitTest(uint16_t x, uint16_t y);

	void UnselectFontButtons();
	
protected:
	String** m_pBookNames = nullptr;
	int m_nBooksCount = 0;
	int m_nSelected = 1;
	const int m_nVisibleBooks = 3;
	BookState m_state = MENU;
	
	TriangleGlyph m_wLeftGlyph;
	TriangleGlyph m_wRightGlyph;
	
	MenuBook m_wMenuBook1;
	MenuBook m_wMenuBook2;
	MenuBook m_wMenuBook3;
	
	SelectionButton m_wPanelBooks;
	SelectionButton m_wPanelOptions;

	SelectionButton m_wSmallFont;
	SelectionButton m_wMiddleFont;
	SelectionButton m_wLargeFont;

	PageView m_wPageView;
	
	int m_nWiFiSignal = -20;
};