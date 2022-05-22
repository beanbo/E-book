#include "Page.h"
#include "DrawHelper.h"
#include "FiraSans16.h"
#include "FileHelper.h"


void PageView::OpenBook(const String& sFileName)
{
    drawStartImage(false);
	
    Serial.println("Open book " + sFileName);
	
    if (m_Pages != nullptr)
    {
		delete m_Pages;
        m_Pages = nullptr;
        m_nNumberOfPages = 0;
    }

    fileHelper.ReadFile(sFileName.c_str(), m_sBookText);
    m_nCurrentPage = 1;

    CalculateAllPositions();
    Draw();
}

void PageView::CalculateAllPositions()
{
    setFont(GetFont());
	
    int yOffset = 0;
    String CurrStr;
    const int SymbolsInRow = EPD_WIDTH / (GetFont().advance_y / 5);
    const int MaxRows = (EPD_HEIGHT - m_nYOffset - GetFont().advance_y) / GetFont().advance_y;
    int StartPosition = 0;

    char* data;
    int  x1, y1; //the bounds of x,y and w and h of the variable 'text' in pixels.
    int w, h;
    int xx, yy;
    bool NeedEndCycle = false;
    int CurrentRow = 1;
    int CurrentReadingPage = 0;
    int nMaxWidth = EPD_WIDTH - m_nXOffset * 2;

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
            if (CurrStr[0] == '\n')
            {
                CurrStr = " ";
                xx = 0;
            }
            else
            {
                if (!NeedEndCycle)
                {
                    int nIndex = CurrStr.indexOf('\n');

                    if (nIndex == -1)
                        nIndex = CurrStr.lastIndexOf(' ');

                    CurrStr.remove(nIndex, CurrStr.length() - nIndex);
                }

                data = const_cast<char*>(CurrStr.c_str());
                xx = m_nXOffset;
                yy = yOffset;
                get_text_bounds(&currentFont, data, &xx, &yy, &x1, &y1, &w, &h, NULL);

                if (xx > nMaxWidth && NeedEndCycle)
                    NeedEndCycle = false;
            }
            
			
        } while (xx > nMaxWidth);
		
        StartPosition += CurrStr.length() + 1;

        if (NeedEndCycle)
        {
            m_Pages[CurrentReadingPage].endRowPositions[CurrentRow - 1] = StartPosition;
            m_Pages[CurrentReadingPage].rows = CurrentRow;
            break;
        }

        m_Pages[CurrentReadingPage].endRowPositions[CurrentRow - 1] = StartPosition - 1;

        if (++CurrentRow > MaxRows)
        {
            CurrentRow = 1;
            m_Pages[CurrentReadingPage].rows = MaxRows;
        }
    }
}

void PageView::AddPage()
{
    m_nNumberOfPages++;

    constexpr int nBlocksize = 15;
	
    if (m_nNumberOfPages % nBlocksize == 1)
    {
        m_Pages = (PageData*)realloc(m_Pages, nBlocksize * ((m_nNumberOfPages / nBlocksize) + 1) * sizeof(PageData));
    }
}

void PageView::Draw()
{
    Serial.println("PageManager::Draw()");
	
    epd_poweron();
    ClearFrameBuffer();

    setFont(FiraSans16);
	
    drawString(EPD_WIDTH / 2, 40, String(m_nCurrentPage) + " / " + String(m_nNumberOfPages), CENTER);

    setFont(GetFont());
	
    drawLine(0, 47, EPD_WIDTH, 47, Black);
    drawLine(0, 48, EPD_WIDTH, 48, Black);

    int nYOfsset = m_nYOffset + GetFont().advance_y;

    for (int currentRow = 0; currentRow < m_Pages[m_nCurrentPage - 1].rows; currentRow++)
    {
        drawString(m_nXOffset, nYOfsset, m_sBookText.substring(m_Pages[m_nCurrentPage - 1].startRowPositions[currentRow], m_Pages[m_nCurrentPage - 1].endRowPositions[currentRow]), LEFT);
        nYOfsset += GetFont().advance_y + 1;
        Serial.println(m_sBookText.substring(m_Pages[m_nCurrentPage - 1].startRowPositions[currentRow], m_Pages[m_nCurrentPage - 1].endRowPositions[currentRow]));
    }

    epd_clear();
    UpdateScreen();
    epd_poweroff();
}

void PageView::GoToNextPage()
{
    if (m_nCurrentPage < m_nNumberOfPages)
    {
        m_nCurrentPage++;
        Draw();
    }
}

void PageView::GoToPreviousPage()
{
    if (m_nCurrentPage > 1)
    {
        m_nCurrentPage--;
        Draw();
    }
}
