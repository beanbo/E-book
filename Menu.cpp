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
    m_nBooksCount = 0;
	
	if (m_pBookNames != nullptr)
	{
		for (int i = 0; i < m_nBooksCount; i++)
            free(m_pBookNames[i]);

		free(m_pBookNames);
		m_pBookNames = nullptr;
	}
	
    const char* dirname = "/";
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = SPIFFS.open(dirname);
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
    constexpr int nBookHeight = EPD_HEIGHT - 2 * nBooksMargin;
    constexpr int nBookWidth = ((EPD_WIDTH - nBooksMargin) / 3) - nBooksMargin;

    epd_poweron();
    ClearFrameBuffer();
	
	for (int nBook = 0; nBook < nVisibleBooks; nBook++)
	{
        Serial.println("m_pBookNames[" + String(nBook) + "] = " + *m_pBookNames[nBook]);
		if (nBook >= m_nBooksCount || m_pBookNames[nBook]->isEmpty())
			break;

		int nXOffset = nBooksMargin + nBook * (nBookWidth + nBooksMargin);
		drawRect(nXOffset, nBooksMargin, nBookWidth, nBookHeight);

		String strBookName = m_pBookNames[nBook]->c_str();
        strBookName.replace("/", "");
        strBookName.replace(".txt", "");
		drawString(nXOffset + nBookWidth / 2, nBooksMargin + nBookHeight / 2, strBookName, CENTER);
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
        constexpr int nVisibleBooks = 3;
        constexpr int nBooksMargin = 30;
        constexpr int nBookHeight = EPD_HEIGHT - 2 * nBooksMargin;
        constexpr int nBookWidth = ((EPD_WIDTH - nBooksMargin) / 3) - nBooksMargin;

        for (int nBook = 0; nBook < nVisibleBooks; nBook++)
        {
            if (x >= nBooksMargin + nBook * (nBookWidth + nBooksMargin) && x <= nBooksMargin + nBook * (nBookWidth + nBooksMargin) + nBookWidth &&
                y >= nBooksMargin && y <= nBooksMargin + nBookHeight)
            {
                if (nBook >= m_nBooksCount || m_pBookNames[nBook]->isEmpty())
                    break;

                Serial.println("m_pBookNames[" + String(nBook) + "] = " + *m_pBookNames[nBook]);
                pageManager.OpenBook(*m_pBookNames[nBook]);
                m_state = BOOK;
                break;
            }
        }
    }
}
