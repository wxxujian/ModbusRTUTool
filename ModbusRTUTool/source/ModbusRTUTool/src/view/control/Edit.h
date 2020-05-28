#pragma once


// Edit

class Edit : public CEdit
{
	DECLARE_DYNAMIC(Edit)

public:
	Edit();
	virtual ~Edit();

protected:
	DECLARE_MESSAGE_MAP()

public:
    void        SetBackColor(COLORREF rgb);                     // 设置文本框背景色
    void        SetTextColor(COLORREF rgb);                     // 设置文本框的字体颜色
    void        SetTextFont(const LOGFONT& lf);                 // 设置字体
    COLORREF    GetBackColor(void) { return m_crBackGnd; }      // 获取当前背景色
    COLORREF    GetTextColor(void) { return m_crText; }         // 获取当前文本色
    BOOL        GetTextFont(LOGFONT& lf);                       // 获取当前字体

private:
    afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);

private:
    COLORREF    m_crText;                                       // 字体的颜色
    COLORREF    m_crBackGnd;                                    // 字体的背景颜色
    CFont       m_font;                                         // 字体
    CBrush      m_brBackGnd;                                    // 整个文本区的画刷
};


