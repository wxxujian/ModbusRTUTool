#pragma once

// TODO:Ctrl+C֮��ȫѡ��ʧ

// ListBox

class ListBox : public CListBox
{
	DECLARE_DYNAMIC(ListBox)

public:
	ListBox();
	virtual ~ListBox();

protected:
	DECLARE_MESSAGE_MAP()
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
    void    ResetContent();
    int     AddString(LPCTSTR lpszItem);
    int     InsertString(int nIndex, LPCTSTR lpszItem);
    int     DeleteString(UINT   nIndex);

protected:
    int     maxWidth;                                       // �����
    bool    isCtrlDown;                                     // ctrl�����Ƿ���
    bool    isADown;                                        // a�����Ƿ���
    bool    isCDown;                                        // c�����Ƿ���
public:
};


