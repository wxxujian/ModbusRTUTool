#pragma once

// TODO:Ctrl+C之后全选消失

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
    int     maxWidth;                                       // 最大宽度
    bool    isCtrlDown;                                     // ctrl按键是否按下
    bool    isADown;                                        // a按键是否按下
    bool    isCDown;                                        // c按键是否按下
public:
};


