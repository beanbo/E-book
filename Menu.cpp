#include "Menu.h"
#include "DrawHelper.h"
#include "firasans.h"
#include "FileHelper.h"
#include "Page.h"

PageManager pageManager;

void Menu::Show()
{
	FindBooks();
	DrawBooks();	
}

void Menu::FindBooks()
{
    //fileHelper.TryChangeFileSystem();
    m_nBooksCount = 0;
    m_nSelected = 1;
	
	if (m_pBookNames != nullptr)
	{
		for (int i = 0; i < m_nBooksCount; i++)
            free(m_pBookNames[i]);

		free(m_pBookNames);
		m_pBookNames = nullptr;
	}
	
    const char* dirname = "/";
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fileHelper.GetCurrentFileSystem()->open(dirname);
    if (!root)
    {
        Serial.println("- failed to open directory");
        return;
    }

    if (!root.isDirectory())
    {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (!file.isDirectory())
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());

            m_nBooksCount++;

            constexpr int nBlocksize = 3;

            if (m_nBooksCount % nBlocksize == 1)
            {
                m_pBookNames = (String**)realloc(m_pBookNames, nBlocksize * ((m_nBooksCount / nBlocksize) + 1) * sizeof(String*));
            }

            m_pBookNames[m_nBooksCount - 1] = new String(file.name());
        }

        file = root.openNextFile();
    }
}

void Menu::DrawBooks()
{
    setFont(FiraSans);

    constexpr int nVisibleBooks = 3;
    constexpr int nBooksMargin = 30;
    constexpr int nFrameXMargin = 30;
    constexpr int nBookHeight = EPD_HEIGHT - 2 * nBooksMargin;
    constexpr int nBookWidth = ((EPD_WIDTH - (nBooksMargin + nFrameXMargin * 2)) / 3) - nBooksMargin;
    constexpr int nMiddle = EPD_HEIGHT / 2;
    constexpr int nGlyphSize = 20;
    constexpr int nRight = EPD_WIDTH - nFrameXMargin;

    epd_poweron();
    ClearFrameBuffer();

	// Draw triangle glyphs
    fillTriangle(nFrameXMargin, nMiddle - nGlyphSize, nFrameXMargin, nMiddle + nGlyphSize, nFrameXMargin - nGlyphSize, nMiddle);
    fillTriangle(nRight, nMiddle - nGlyphSize, nRight, nMiddle + nGlyphSize, nRight + nGlyphSize, nMiddle);

	// Draw Books
    for (int nBook = 0; nBook < nVisibleBooks; nBook++)
    {
        const int nBookIndex = (m_nSelected - 1) * nVisibleBooks + nBook;
		
        if (nBookIndex >= m_nBooksCount || m_pBookNames[nBookIndex]->isEmpty())
            break;

        Serial.println("m_pBookNames[" + String(nBookIndex) + "] = " + *m_pBookNames[nBookIndex]);

        int nXOffset = nFrameXMargin + nBooksMargin + nBook * (nBookWidth + nBooksMargin);
        drawRect(nXOffset, nBooksMargin, nBookWidth, nBookHeight, 2);

        String strBookName = m_pBookNames[nBookIndex]->c_str();
        strBookName.replace("/", "");
        strBookName.replace(".txt", "");
        drawString(nXOffset + nBookWidth / 2, nBooksMargin + nBookHeight / 2 + 15, strBookName, CENTER);
    }

    epd_clear();
    UpdateScreen();
    epd_poweroff();
}

void Menu::HitTest(uint16_t x, uint16_t y)
{
    if (m_state == BOOK)
    {
        constexpr int nMiddle = EPD_WIDTH / 2;
        constexpr int nHalfWidth = EPD_WIDTH / 6;
        constexpr int nHeight = 100;
		
        if (x > nMiddle - nHalfWidth && x < nMiddle + nHalfWidth && y > EPD_HEIGHT - nHeight)
        {
            Show();
			m_state = MENU;
        }
        else if (x < nMiddle)
            pageManager.GoToPreviousPage();
        else
            pageManager.GoToNextPage();
    }
    else if (m_state == MENU)
    {
        constexpr int nFrameXMargin = 30;
        constexpr int nMiddle = EPD_HEIGHT / 2;
        constexpr int nRight = EPD_WIDTH - nFrameXMargin;
        constexpr int nVisibleBooks = 3;
        constexpr int nCorrection = 15;
        constexpr int nGlyphSize = 20 + nCorrection;

        // check click on triangle glyphs
		if (x < nFrameXMargin + nCorrection && x > 0 && y > nMiddle - nGlyphSize && y < nMiddle + nGlyphSize) // left
		{
            if (m_nSelected > 1)
            {
                m_nSelected--;
                DrawBooks();
            }
		}
		else if (x < EPD_WIDTH && x > nRight - nCorrection && y > nMiddle - nGlyphSize && y < nMiddle + nGlyphSize) // right
		{
            if (m_nSelected * nVisibleBooks < m_nBooksCount)
            {
                m_nSelected++;
                DrawBooks();
            }
		}
        else // books
        {
            constexpr int nBooksMargin = 30;
            constexpr int nBookHeight = EPD_HEIGHT - 2 * nBooksMargin;
            constexpr int nBookWidth = ((EPD_WIDTH - (nBooksMargin + nFrameXMargin * 2)) / 3) - nBooksMargin;

            for (int nBook = 0; nBook < nVisibleBooks; nBook++)
            {
                if (x >= nFrameXMargin + nBooksMargin + nBook * (nBookWidth + nBooksMargin) && x <= nFrameXMargin + nBooksMargin + nBook * (nBookWidth + nBooksMargin) + nBookWidth &&
                    y >= nBooksMargin && y <= nBooksMargin + nBookHeight)
                {
                    const int nBookIndex = (m_nSelected - 1) * nVisibleBooks + nBook;

                    if (nBookIndex >= m_nBooksCount || m_pBookNames[nBookIndex]->isEmpty())
                        break;

                    Serial.println("m_pBookNames[" + String(nBookIndex) + "] = " + *m_pBookNames[nBookIndex]);
                    pageManager.OpenBook(*m_pBookNames[nBookIndex]);
                    m_state = BOOK;
                    break;
                }
            }
        }
    }
}
