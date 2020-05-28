// LoggerView.cpp: 实现文件
//

#include "../pch.h"
#include "../../res/resource.h"
#include "LoggerView.h"


// LoggerView

IMPLEMENT_DYNCREATE(LoggerView, CFormView)

LoggerView::LoggerView()
	: CFormView(IDD_VIEW_LOGGER)
{

}

LoggerView::~LoggerView()
{
}

void LoggerView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, listBox);
}

BEGIN_MESSAGE_MAP(LoggerView, CFormView)
    ON_WM_SIZE()
END_MESSAGE_MAP()


// LoggerView 诊断

#ifdef _DEBUG
void LoggerView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void LoggerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// LoggerView 消息处理程序


BOOL LoggerView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
    // TODO: 在此添加专用代码和/或调用基类

    return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void LoggerView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if(listBox.GetSafeHwnd())
        listBox.MoveWindow(0, 0, cx, cy);
}

void LoggerView::info(const CString& content)
{
    SYSTEMTIME systemTime;
    //GetSystemTime(&systemTime);
    GetLocalTime(&systemTime);
    
    CString str;
    str.Format(L"[%02d:%02d:%02d:%03d]  [INFO ]  ", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
    listBox.AddString(str + content);
    listBox.SetTopIndex(listBox.GetCount() - 1);
}

void LoggerView::error(const CString& content)
{
    SYSTEMTIME systemTime;
    //GetSystemTime(&systemTime);
    GetLocalTime(&systemTime);

    CString str;
    str.Format(L"[%02d:%02d:%02d:%03d]  [ERROR]  ", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

    listBox.AddString(str + content);
    listBox.SetTopIndex(listBox.GetCount() - 1);
}

