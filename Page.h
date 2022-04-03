#pragma once
#include <Arduino.h>

#define ROWS_ON_SCREEN 10

class PageManager
{
public:

    struct PageData
    {
        int startRowPositions[ROWS_ON_SCREEN];
        int endRowPositions[ROWS_ON_SCREEN];
        int rows;
    };

    void SetBookText(const String& sBookText);
    void DrawCurrentPage();

    void GoToNextPage();
    void GoToPreviousPage();

protected:
    void CalculateAllPositions();
    void AddPage();

protected:
    PageData* m_Pages = NULL;
    int m_nNumberOfPages = 0;
    int m_nXOffset = 20;
    int m_nYOffset = 95;
    String m_sBookText = "";
    int m_nCurrentPage = 1;
};

