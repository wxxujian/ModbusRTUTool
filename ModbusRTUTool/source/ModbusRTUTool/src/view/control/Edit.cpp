// Edit.cpp: 实现文件
//

#include "../../pch.h"
#include "Edit.h"


// Edit

IMPLEMENT_DYNAMIC(Edit, CEdit)

Edit::Edit()
{
    //初始化为系统字体和窗口颜色
    m_crText = GetSysColor(COLOR_WINDOWTEXT);
    m_crBackGnd = GetSysColor(COLOR_WINDOW);
    m_font.CreatePointFont(90, L"宋体");
    m_brBackGnd.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
}

Edit::~Edit()
{
}


BEGIN_MESSAGE_MAP(Edit, CEdit)
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// Edit 消息处理程序


HBRUSH Edit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    // TODO:  在此更改 DC 的任何特性
    //刷新前更改文本颜色
    pDC->SetTextColor(m_crText);

    //刷新前更改文本背景
    pDC->SetBkColor(m_crBackGnd);

    //返回画刷,用来绘制整个控件背景
    return m_brBackGnd;
}

void Edit::SetBackColor(COLORREF rgb)
{
    //设置文字背景颜色
    m_crBackGnd = rgb;

    //释放旧的画刷
    if (m_brBackGnd.GetSafeHandle())
        m_brBackGnd.DeleteObject();
    //使用文字背景颜色创建新的画刷,使得文字框背景和文字背景一致
    m_brBackGnd.CreateSolidBrush(rgb);
    //redraw
    Invalidate(TRUE);
}

void Edit::SetTextColor(COLORREF rgb)
{
    //设置文字颜色
    m_crText = rgb;
    //redraw
    Invalidate(TRUE);
}

void Edit::SetTextFont(const LOGFONT& lf)
{
    //创建新的字体,并设置为CEDIT的字体
    if (m_font.GetSafeHandle())
    {
        m_font.DeleteObject();
    }
    m_font.CreateFontIndirect(&lf);
    CEdit::SetFont(&m_font);
    //redraw
    Invalidate(TRUE);
}

BOOL Edit::GetTextFont(LOGFONT& lf)
{
    if (m_font.GetLogFont(&lf) != 0)
    {
        return TRUE;
    }
    return FALSE;
}