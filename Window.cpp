#include "Window.h"
#include "DrawHelper.h"

Rect::Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    nX = x;
    nY = y;
    Width = w;
    Height = h;
}

Rect::Rect(const Rect& other)
{
    nX = other.nX;
    nY = other.nY;
    Width = other.Width;
    Height = other.Height;
}

Rect::Rect()
{
    nX = 0;
    nY = 0;
    Width = 0;
    Height = 0;
}

bool Rect::HitTest(uint16_t x, uint16_t y, uint16_t smooth)
{
    return x >= nX - smooth && x <= nX + Width + smooth && y >= nY - smooth && y <= nY + Height + smooth;
}

Window::Window(Rect rc)
{
    m_rcWindow = rc;
}

Window::Window()
{
}

void Window::Draw()
{
    setFont(m_font);

    if (m_pIcon)
    {
        Rect_t area = {
            .x = m_rcWindow.nX + (m_rcWindow.Width - m_nIconWidth) / 2,
            .y = m_rcWindow.nY + (m_rcWindow.Height - m_nIconHeight) / 2,
            .width = m_nIconWidth,
            .height = m_nIconHeight,
        };

        DrawImage(area, m_pIcon);
    }
}

bool Window::HitTest(uint16_t x, uint16_t y, uint16_t smooth)
{
    if (m_eVisibility == Visibility::Hidden || m_bSelected)
        return false;

    return m_rcWindow.HitTest(x, y, smooth);
}

void Window::SetWindowPos(Rect rc)
{
    m_rcWindow = rc;
}

void Window::ShowWindow()
{
    m_eVisibility = Visible;
}

void Window::HideWindow()
{
    m_eVisibility = Hidden;
}

void Window::Select(bool bSelected)
{
    m_bSelected = bSelected;
}

void Window::SetFont(GFXfont const& font)
{
    m_font = font;
}

GFXfont const& Window::GetFont()
{
    return m_font;
}

void Window::SetIcon(uint8_t* icon, uint32_t width, uint32_t height)
{
	m_pIcon = icon;
	m_nIconWidth = width;
	m_nIconHeight = height;
}
