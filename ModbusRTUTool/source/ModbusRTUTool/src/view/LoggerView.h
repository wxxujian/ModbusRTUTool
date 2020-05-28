#pragma once
#include "control/ListBox.h"


// LogView 窗体视图

class LoggerView : public CFormView
{
	DECLARE_DYNCREATE(LoggerView)

protected:
    LoggerView();           // 动态创建所使用的受保护的构造函数
	virtual ~LoggerView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIEW_LOG };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    void info(const CString& content);
    void error(const CString& content);
private:
    ListBox listBox;
};


