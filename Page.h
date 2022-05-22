#pragma once
#include <Arduino.h>
#include "Window.h"

#define MAX_ROWS_ON_SCREEN 16

class PageView : public Window
{
public:

    struct PageData
    {
        int startRowPositions[MAX_ROWS_ON_SCREEN];
        int endRowPositions[MAX_ROWS_ON_SCREEN];
        int rows;
    };
	
    void OpenBook(const String& sFileName);

    void Draw() override;

    void GoToNextPage();
    void GoToPreviousPage();

protected:
    void CalculateAllPositions();
    void AddPage();

protected:
    PageData* m_Pages = nullptr;
    int m_nNumberOfPages = 0;
    int m_nXOffset = 10;
    int m_nYOffset = 60;
    String m_sBookText = "";
    int m_nCurrentPage = 1;
};

