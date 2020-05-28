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
    void        SetBackColor(COLORREF rgb);                     // �����ı��򱳾�ɫ
    void        SetTextColor(COLORREF rgb);                     // �����ı����������ɫ
    void        SetTextFont(const LOGFONT& lf);                 // ��������
    COLORREF    GetBackColor(void) { return m_crBackGnd; }      // ��ȡ��ǰ����ɫ
    COLORREF    GetTextColor(void) { return m_crText; }         // ��ȡ��ǰ�ı�ɫ
    BOOL        GetTextFont(LOGFONT& lf);                       // ��ȡ��ǰ����

private:
    afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);

private:
    COLORREF    m_crText;                                       // �������ɫ
    COLORREF    m_crBackGnd;                                    // ����ı�����ɫ
    CFont       m_font;                                         // ����
    CBrush      m_brBackGnd;                                    // �����ı����Ļ�ˢ
};


