#include "MainView.h"
#include "DrawHelper.h"
#include "FileHelper.h"
#include "FiraSans12.h"
#include "FiraSans16.h"
#include "FiraSans26.h"
#include "bookimg.h"
#include "setting_img.h"
#include "books_img.h"


#define LANG_RU
//#define LANG_EN

#ifdef LANG_RU
#define SELECT_SMALL_FONT "Выбрать маленький шрифт"
#define SELECT_MIDDLE_FONT "Выбрать средний шрифт"
#define SELECT_LARGE_FONT "Выбрать большой шрифт"
#endif // LANG_RU

#ifdef LANG_EN
#define SELECT_SMALL_FONT "Select small font"
#define SELECT_MIDDLE_FONT "Select middle font"
#define SELECT_LARGE_FONT "Select large font"
#endif // LANG_EN


void TriangleGlyph::SetOrientation(Orientation orientation)
{
    m_orientation = orientation;
}

void TriangleGlyph::Draw()
{
    Window::Draw();

    if (m_orientation == Orientation::Right)
        fillTriangle(m_rcWindow.nX, m_rcWindow.nY, m_rcWindow.nX + m_rcWindow.Width, m_rcWindow.nY + m_rcWindow.Height / 2, m_rcWindow.nX, m_rcWindow.nY + m_rcWindow.Height);
    else if (m_orientation == Orientation::Left)
        fillTriangle(m_rcWindow.nX, m_rcWindow.nY + m_rcWindow.Height / 2, m_rcWindow.nX + m_rcWindow.Width, m_rcWindow.nY, m_rcWindow.nX + m_rcWindow.Width, m_rcWindow.nY + m_rcWindow.Height);
    else if (m_orientation == Orientation::Up)
        fillTriangle(m_rcWindow.nX, m_rcWindow.nY, m_rcWindow.nX + m_rcWindow.Width / 2, m_rcWindow.nY + m_rcWindow.Height, m_rcWindow.nX + m_rcWindow.Width, m_rcWindow.nY);
    else if (m_orientation == Orientation::Down)
        fillTriangle(m_rcWindow.nX, m_rcWindow.nY, m_rcWindow.nX + m_rcWindow.Width / 2, m_rcWindow.nY, m_rcWindow.nX + m_rcWindow.Width, m_rcWindow.nY + m_rcWindow.Height);
}

constexpr int nTopPanelHeight = 48;
constexpr int nLeftPanelWidth = 100;

MainView::MainView() 
{
    m_wPanelBooks.Select();

    m_wPanelBooks.SetIcon((uint8_t*)books_img_data, books_img_width, books_img_height);
    m_wPanelOptions.SetIcon((uint8_t*)setting_img_data, setting_img_width, setting_img_height);

    m_wSmallFont.SetTitle(SELECT_SMALL_FONT);
    m_wMiddleFont.SetTitle(SELECT_MIDDLE_FONT);
    m_wLargeFont.SetTitle(SELECT_LARGE_FONT);

    m_wSmallFont.SetFont(FiraSans12);
    m_wMiddleFont.SetFont(FiraSans16);
    m_wLargeFont.SetFont(FiraSans26);

    m_wMiddleFont.Select();

    m_wPageView.SetFont(FiraSans16);

    constexpr int nButtonMargin = 10;
    constexpr int nButtonSize = 80;

    Rect rcButton(nButtonMargin, nButtonMargin + nTopPanelHeight, nButtonSize, nButtonSize);
    m_wPanelBooks.SetWindowPos(rcButton);

    rcButton.nY += nButtonSize + nButtonMargin;
    m_wPanelOptions.SetWindowPos(rcButton);

    constexpr int nBooksMargin = 30;
    constexpr int nFrameXMargin = 60;
    constexpr int nBookHeight = EPD_HEIGHT - 2 * nBooksMargin - nTopPanelHeight;
    constexpr int nBookWidth = (EPD_WIDTH - nFrameXMargin * 2 - nLeftPanelWidth - nBooksMargin * 2) / 3;

    constexpr int nMiddle = (EPD_HEIGHT + nTopPanelHeight) / 2;
    constexpr int nGlyphSize = 24;
    constexpr int nRight = EPD_WIDTH - nFrameXMargin + (nFrameXMargin - nGlyphSize) / 2;
    	
    m_wLeftGlyph.SetWindowPos(Rect(nLeftPanelWidth + (nFrameXMargin - nGlyphSize) / 2, nMiddle - nGlyphSize, nGlyphSize, nGlyphSize * 2));
    m_wRightGlyph.SetWindowPos(Rect(nRight, nMiddle - nGlyphSize, nGlyphSize, nGlyphSize * 2));
	
    m_wLeftGlyph.SetOrientation(TriangleGlyph::Orientation::Left);
	m_wRightGlyph.SetOrientation(TriangleGlyph::Orientation::Right);

    Rect rcBook(nLeftPanelWidth + nFrameXMargin, nBooksMargin + nTopPanelHeight, nBookWidth, nBookHeight);

    for (auto book : { &m_wMenuBook1, &m_wMenuBook2, &m_wMenuBook3 })
    {
        book->SetWindowPos(rcBook);
        rcBook.nX += nBooksMargin + nBookWidth;
    }

    constexpr int nFontBtnWidth = EPD_WIDTH - nButtonMargin * 2 - nLeftPanelWidth;
    constexpr int nFontBtnHeight = (EPD_HEIGHT - nTopPanelHeight - nButtonMargin * 4) / 3;

    Rect rcFontButton(nButtonMargin + nLeftPanelWidth, nButtonMargin + nTopPanelHeight, nFontBtnWidth, nFontBtnHeight);

    for (auto button : { &m_wSmallFont, &m_wMiddleFont, &m_wLargeFont })
    {
        button->SetWindowPos(rcFontButton);
        rcFontButton.nY += nButtonMargin + nFontBtnHeight;
    };
}

void MainView::Show()
{
    if (m_state == BookState::BOOK)
        m_wPageView.Draw();
    else
    {
        FindBooks();
        DrawMainPage();
    }
}

void MainView::FindBooks()
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

            m_pBookNames[m_nBooksCount - 1] = new String(dirname);
            *m_pBookNames[m_nBooksCount - 1] += file.name();
        }

        file = root.openNextFile();
    }
}

void MainView::DrawMainPage()
{
    epd_poweron();
    ClearFrameBuffer();

    constexpr int nBatteryMargin = 10;
    constexpr int nBatteryWidth = 60;
    constexpr int nBatteryHeight = 30;

    DrawWiFi(EPD_WIDTH - (nBatteryMargin + nBatteryWidth) * 2, nBatteryMargin, m_nWiFiSignal);
    DrawBattery(EPD_WIDTH - nBatteryMargin - nBatteryWidth, nBatteryMargin, nBatteryWidth, nBatteryHeight);

    // Draw top panel separator
    drawFastHLine(0, nTopPanelHeight, EPD_WIDTH, 2);

    // Draw left panel buttons
    m_wPanelBooks.Draw();
    m_wPanelOptions.Draw();

    // Draw Left panel separator
    drawFastVLine(nLeftPanelWidth, nTopPanelHeight, EPD_HEIGHT - nTopPanelHeight, 2);

    if (m_state == BookState::MENU)
        DrawBooks();
    else if (m_state == BookState::OPTIONS)
        DrawOptions();

    epd_clear();
    UpdateScreen();
    epd_poweroff();
}

void MainView::DrawBooks()
{
    // Draw glyphs
    if (m_nSelected > 1)
    {
        m_wLeftGlyph.ShowWindow();
        m_wLeftGlyph.Draw();
    }
    else
        m_wLeftGlyph.HideWindow();

    if (m_nSelected * m_nVisibleBooks < m_nBooksCount)
    {
        m_wRightGlyph.ShowWindow();
        m_wRightGlyph.Draw();
    }
    else
        m_wRightGlyph.HideWindow();

    // Draw books
    int nBookIndex = (m_nSelected - 1) * m_nVisibleBooks;

    for (auto book : { &m_wMenuBook1, &m_wMenuBook2, &m_wMenuBook3 })
    {
        if (nBookIndex < m_nBooksCount)
        {
            book->SetTitle(*m_pBookNames[nBookIndex]);
            book->ShowWindow();
            book->Draw();
            nBookIndex++;
        }
        else
        {
            book->HideWindow();
        }
    }
}

void MainView::DrawOptions()
{
    m_wSmallFont.Draw();
    m_wMiddleFont.Draw();
    m_wLargeFont.Draw();
}

void MainView::HitTest(uint16_t x, uint16_t y)
{
    switch (m_state)
    {
    case BookState::BOOK:
    {
        constexpr int nMiddle = EPD_WIDTH / 2;
        constexpr int nHalfWidth = EPD_WIDTH / 6;
        constexpr int nHeight = 100;

        if (x > nMiddle - nHalfWidth && x < nMiddle + nHalfWidth && y < nHeight)
        {
            m_state = BookState::MENU;
            Show();
        }
        else if (x < nMiddle)
            m_wPageView.GoToPreviousPage();
        else
            m_wPageView.GoToNextPage();
    }
    break;
    case BookState::MENU:
    {
        constexpr int nSmoothGlyphs = 20;

        if (m_wPanelOptions.HitTest(x, y))
        {
            m_wPanelOptions.Select(true);
            m_wPanelBooks.Select(false);

            m_state = BookState::OPTIONS;
            DrawMainPage();
        }
        else if (m_wLeftGlyph.HitTest(x, y, nSmoothGlyphs))
        {
            m_nSelected--;
            DrawMainPage();
        }
        else if (m_wRightGlyph.HitTest(x, y, nSmoothGlyphs))
        {
            m_nSelected++;
            DrawMainPage();
        }
        else // books
        {

            for (auto book : { &m_wMenuBook1, &m_wMenuBook2, &m_wMenuBook3 })
            {
                if (book->HitTest(x, y))
				{
					m_state = BookState::BOOK;
                    m_wPageView.OpenBook(book->GetFullName());
					break;
				}
            }
        }
    }
    break;
    case BookState::OPTIONS:
    {
        if (m_wPanelBooks.HitTest(x, y))
        {
            m_wPanelBooks.Select(true);
            m_wPanelOptions.Select(false);

            m_state = BookState::MENU;
            DrawMainPage();
        }

        for (auto button : { &m_wSmallFont, &m_wMiddleFont, &m_wLargeFont })
        {
            if (button->HitTest(x, y))
            {
                UnselectFontButtons();
                button->Select();
                m_wPageView.SetFont(button->GetFont());

                DrawMainPage();
            }
        }
    }
    break;
    default:
        Serial.println("Unrecognized state");
		break;
    }
}

void MainView::UnselectFontButtons()
{
    m_wSmallFont.Select(false);
    m_wMiddleFont.Select(false);
    m_wLargeFont.Select(false);
}

MenuBook::MenuBook() : Window()
{
    SetIcon((uint8_t*)bookimg_data, bookimg_width, bookimg_height);
}

void MenuBook::SetTitle(String sFullName)
{
    m_sFullName = sFullName;
    sFullName.replace("/", "");
    sFullName.replace(".txt", "");
    m_sTitle = sFullName;
}

String MenuBook::GetFullName()
{
    return m_sFullName;
}

void MenuBook::Draw()
{
    Window::Draw();

    constexpr int nTopMargin = 35;
    constexpr int nLeftMargin = 30;
    constexpr int nRightMargin = 50;
	constexpr int nBottomMargin = 10;
	
	const int nTitleWidth = m_nIconWidth - nLeftMargin - nRightMargin;
	const int nTitleHeight = m_nIconHeight - nTopMargin - nBottomMargin;
	
	const int nTitleX = m_rcWindow.nX + nLeftMargin + (m_rcWindow.Width - m_nIconWidth) / 2;
	const int nTitleY = m_rcWindow.nY + nTopMargin + (m_rcWindow.Height - m_nIconHeight) / 2;
    drawMultilineString(nTitleX, nTitleY, nTitleWidth, nTitleHeight, m_sTitle, CENTER);
}

void SelectionButton::Draw()
{
    Window::Draw();

	if (!m_sTitle.isEmpty())
        drawString(m_rcWindow.nX + m_rcWindow.Width / 2, m_rcWindow.nY + m_rcWindow.Height / 2 + 15, m_sTitle, CENTER);
        	
    if (m_bSelected)
        drawRect(m_rcWindow.nX, m_rcWindow.nY, m_rcWindow.Width, m_rcWindow.Height, 2);
}

void SelectionButton::SetTitle(String sTitle)
{
    m_sTitle = sTitle;
}
