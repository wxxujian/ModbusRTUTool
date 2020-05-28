// ListBox.cpp: 实现文件
//

#include "../../pch.h"
#include "ListBox.h"


// ListBox

IMPLEMENT_DYNAMIC(ListBox, CListBox)

ListBox::ListBox()
{
    maxWidth = 0;                                     // 最大宽度
    isCtrlDown = false;                                     // ctrl按键是否按下
    isADown = false;                                        // a按键是否按下
    isCDown = false;                                        // c按键是否按下
}

ListBox::~ListBox()
{
}


BEGIN_MESSAGE_MAP(ListBox, CListBox)
    ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// ListBox 消息处理程序

void    ListBox::ResetContent()
{
    CListBox::ResetContent();

    maxWidth = 0;
    SetHorizontalExtent(0);
}

int ListBox::AddString(LPCTSTR lpszItem)
{
    int   nRet = CListBox::AddString(lpszItem);

    // 获取垂直滚动条得信息
    SCROLLINFO   scrollInfo;
    memset(&scrollInfo, 0, sizeof(SCROLLINFO));
    scrollInfo.cbSize = sizeof(SCROLLINFO);
    scrollInfo.fMask = SIF_ALL;
    GetScrollInfo(SB_VERT, &scrollInfo, SIF_ALL);

    int   nScrollWidth = 0;                                             // 垂直滚动条得宽度
    if ((GetCount() > 1) && ((int)scrollInfo.nMax >= (int)scrollInfo.nPage))
    {
        nScrollWidth = GetSystemMetrics(SM_CXVSCROLL);
    }

    SIZE        sSize;
    CClientDC   myDC(this);

    CFont* pListBoxFont = GetFont();
    if (pListBoxFont != NULL)
    {
        CFont* pOldFont = myDC.SelectObject(pListBoxFont);
        GetTextExtentPoint32(myDC.m_hDC, lpszItem, wcslen(lpszItem), &sSize);
        maxWidth = max(maxWidth, (int)sSize.cx);
        SetHorizontalExtent(maxWidth + 3);
        myDC.SelectObject(pOldFont);
    }
    
    return   nRet;
}

int     ListBox::InsertString(int nIndex, LPCTSTR lpszItem)
{
    int   nRet = CListBox::InsertString(nIndex, lpszItem);

    // 获取垂直滚动条得信息
    SCROLLINFO   scrollInfo;
    memset(&scrollInfo, 0, sizeof(SCROLLINFO));
    scrollInfo.cbSize = sizeof(SCROLLINFO);
    scrollInfo.fMask = SIF_ALL;
    GetScrollInfo(SB_VERT, &scrollInfo, SIF_ALL);

    int   nScrollWidth = 0;                                             // 垂直滚动条得宽度
    if ((GetCount() > 1) && ((int)scrollInfo.nMax >= (int)scrollInfo.nPage))
    {
        nScrollWidth = GetSystemMetrics(SM_CXVSCROLL);
    }

    SIZE        sSize;
    CClientDC   myDC(this);

    CFont* pListBoxFont = GetFont();
    if (pListBoxFont != NULL)
    {
        CFont* pOldFont = myDC.SelectObject(pListBoxFont);
        GetTextExtentPoint32(myDC.m_hDC, lpszItem, wcslen(lpszItem), &sSize);
        maxWidth = max(maxWidth, (int)sSize.cx);
        SetHorizontalExtent(maxWidth + 3);
        myDC.SelectObject(pOldFont);
    }

    return   nRet;
}

int     ListBox::DeleteString(UINT   nIndex)
{
    RECT   lRect;
    GetWindowRect(&lRect);

    int   nRet = CListBox::DeleteString(nIndex);

    int   nBoxWidth = lRect.right - lRect.left;
    maxWidth = nBoxWidth;

    SIZE        sSize;
    CClientDC   myDC(this);
    CFont*      pListBoxFont    = GetFont();
    CFont*      pOldFont        = myDC.SelectObject(pListBoxFont);
    wchar_t     szEntry[2048];

    for (int i = 0; i < GetCount(); i++)
    {
        GetText(i, szEntry);
        GetTextExtentPoint32(myDC.m_hDC, szEntry, wcslen(szEntry), &sSize);
        maxWidth = max(maxWidth, (int)sSize.cx);
    }
    myDC.SelectObject(pOldFont);

    if (maxWidth > nBoxWidth)   //   显示水平滚动条
    {
        ShowScrollBar(SB_HORZ, TRUE);
        SetHorizontalExtent(maxWidth + 3);
    }
    else
    {
        ShowScrollBar(SB_HORZ, FALSE);
    }
    return   nRet;

}

BOOL ListBox::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN) {
        switch (pMsg->wParam) {
        case VK_CONTROL:
            isCtrlDown = true;
            break;

        case 'A':
        case 'a':
            isADown = true;
            break;

        case 'C':
        case 'c':
            isCDown = true;
            break;
        }
        if (isCtrlDown) {
            if (isADown) {
                for (int i = 0; i < GetCount(); ++i) {
                    SetSel(i, TRUE);
                }
                return TRUE;
            }
            if (isCDown) {
                int selectedCount = GetSelCount();
                if (selectedCount > 0) {
                    CArray<int> selectedRows;
                    selectedRows.SetSize(selectedCount);
                    GetSelItems(selectedCount, selectedRows.GetData());

                    CString str;
                    CString strTemp;
                    for (int i = 0; i < selectedCount - 1; ++i) {
                        GetText(selectedRows[i], strTemp);
                        str += strTemp;
                        str += L"\n";
                    }

                    GetText(selectedRows[selectedCount - 1], strTemp);
                    str += strTemp;

                    //GetText(0, str);
                    //str += L"\nFF FF FF";

                    if (OpenClipboard())
                    {
                        EmptyClipboard();
                        size_t  cbStr = (str.GetLength() + 1) * sizeof(TCHAR);
                        HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, cbStr);
                        memcpy_s(GlobalLock(hData), cbStr, str.LockBuffer(), cbStr);
                        GlobalUnlock(hData);
                        str.UnlockBuffer();
                        SetClipboardData(CF_UNICODETEXT, hData);
                        CloseClipboard();
                    }
                    return TRUE;
                }
            }
        }
    }
    else if (pMsg->message == WM_KEYUP) {
        switch (pMsg->wParam) {
        case VK_CONTROL:
            isCtrlDown = false;
            break;

        case 'A':
        case 'a':
            isADown = false;
            break;

        case 'C':
        case 'c':
            isCDown = false;
            break;
        }
    }

    return CListBox::PreTranslateMessage(pMsg);
}

void ListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
 
    if (0 == GetCount())
    {
        CListBox::OnRButtonDown(nFlags, point);
        return;
    }
    CMenu menu;
    menu.CreatePopupMenu();
    menu.AppendMenuW(MF_STRING, 0, L"复制            Ctrl+C");
    menu.AppendMenuW(MF_STRING, 1, L"全选            Ctrl+A");
    menu.AppendMenuW(MF_STRING, 2, L"全部清除");

    ClientToScreen(&point);
    menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
    menu.DestroyMenu();
 

    CListBox::OnRButtonDown(nFlags, point);
}

BOOL ListBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    switch (LOWORD(wParam)) {
    case 0:                                     // 复制
    {
        int selectedCount = GetSelCount();
        if (selectedCount <= 0) return FALSE;

        CArray<int> selectedRows;
        selectedRows.SetSize(selectedCount);
        GetSelItems(selectedCount, selectedRows.GetData());

        CString str;
        CString strTemp;
        for (int i = 0; i < selectedCount - 1; ++i) {
            GetText(selectedRows[i], strTemp);
            str += strTemp;
            str += L"\n";
        }

        GetText(selectedRows[selectedCount - 1], strTemp);
        str += strTemp;

        if (OpenClipboard())
        {
            EmptyClipboard();
            size_t cbStr = (str.GetLength() + 1) * sizeof(TCHAR);
            HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, cbStr);
            memcpy_s(GlobalLock(hData), cbStr, str.LockBuffer(), cbStr);
            GlobalUnlock(hData);
            str.UnlockBuffer();
            SetClipboardData(CF_UNICODETEXT, hData);
            CloseClipboard();
        }
        break;
    }

    case 1:                                     // 全选
        for (int i = 0; i < GetCount(); ++i) {
            SetSel(i, TRUE);
        }
        break;

    case 2:
        ResetContent();
        break;
    }

    return CListBox::OnCommand(wParam, lParam);
}

