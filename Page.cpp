#include "Page.h"
#include "DrawHelper.h"

void PageManager::SetBookText(const String& sBookText)
{
    m_sBookText = sBookText;
    m_nCurrentPage = 1;

    CalculateAllPositions();
    DrawCurrentPage();
}

void PageManager::CalculateAllPositions()
{
    int xOffSet = 5;
    int yOffset = 0;
    String CurrStr;
    const int SymbolsInRow = EPD_WIDTH / 12;
    int StartPosition = 0;

    char* data;
    int  x1, y1; //the bounds of x,y and w and h of the variable 'text' in pixels.
    int w, h;
    int xx, yy;
    bool NeedEndCycle = false;
    int CurrentRow = 1;
    int CurrentReadingPage = 0;

    while (true)
    {
        if (CurrentRow == 1)
        {
            AddPage();
            CurrentReadingPage = m_nNumberOfPages - 1;
        }

        m_Pages[CurrentReadingPage].startRowPositions[CurrentRow - 1] = StartPosition;

        if (StartPosition + SymbolsInRow > m_sBookText.length() - 1)
        {
            CurrStr = m_sBookText.substring(StartPosition);
            NeedEndCycle = true;
        }
        else
            CurrStr = m_sBookText.substring(StartPosition, StartPosition + SymbolsInRow);

        do
        {
            if (!NeedEndCycle)
            {
                do
                {
                    CurrStr.remove(CurrStr.length() - 1);
                } while (CurrStr[CurrStr.length() - 1] != ' ');
            }

            data = const_cast<char*>(CurrStr.c_str());
            xx = xOffSet;
            yy = yOffset;
            get_text_bounds(&currentFont, data, &xx, &yy, &x1, &y1, &w, &h, NULL);

            if (xx > EPD_WIDTH && NeedEndCycle)
                NeedEndCycle = false;
        } while (xx > EPD_WIDTH);

        StartPosition += CurrStr.length();

        if (NeedEndCycle)
        {
            m_Pages[CurrentReadingPage].endRowPositions[CurrentRow - 1] = StartPosition;
            m_Pages[CurrentReadingPage].rows = CurrentRow;
            break;
        }

        m_Pages[CurrentReadingPage].endRowPositions[CurrentRow - 1] = StartPosition - 1;

        if (++CurrentRow > ROWS_ON_SCREEN)
        {
            CurrentRow = 1;
            m_Pages[CurrentReadingPage].rows = ROWS_ON_SCREEN;
        }
    }
}

void PageManager::AddPage()
{
    m_nNumberOfPages++;

    if (m_nNumberOfPages % 5 == 1)
    {
        m_Pages = (PageData*)realloc(m_Pages, 5 * ((m_nNumberOfPages / 5) + 1) * sizeof(PageData));
    }
}

void PageManager::DrawCurrentPage()
{
    epd_poweron();
    ClearFrameBuffer();

    drawString(EPD_WIDTH / 2, 40, String(m_nCurrentPage) + " / " + String(m_nNumberOfPages), CENTER);

    drawLine(0, 47, EPD_WIDTH, 47, Black);
    drawLine(0, 48, EPD_WIDTH, 48, Black);

    int nYOfsset = m_nYOffset;

    for (int currentRow = 0; currentRow < m_Pages[m_nCurrentPage - 1].rows; currentRow++)
    {
        drawString(m_nXOffset, nYOfsset, m_sBookText.substring(m_Pages[m_nCurrentPage - 1].startRowPositions[currentRow], m_Pages[m_nCurrentPage - 1].endRowPositions[currentRow]), LEFT);
        nYOfsset += 45;
        Serial.println(m_sBookText.substring(m_Pages[m_nCurrentPage - 1].startRowPositions[currentRow], m_Pages[m_nCurrentPage - 1].endRowPositions[currentRow]));
    }

    epd_clear();
    UpdateScreen();
    epd_poweroff();
}

void PageManager::GoToNextPage()
{
    if (m_nCurrentPage < m_nNumberOfPages)
    {
        m_nCurrentPage++;
        DrawCurrentPage();
    }
}

void PageManager::GoToPreviousPage()
{
    if (m_nCurrentPage > 1)
    {
        m_nCurrentPage--;
        DrawCurrentPage();
    }
}
