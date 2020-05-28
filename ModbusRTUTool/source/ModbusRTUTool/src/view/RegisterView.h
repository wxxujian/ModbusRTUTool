#pragma once
#include <stdint.h>

#define WM_SET_REGISTER WM_USER + 140
// RegisterView 窗体视图

class RegisterView : public CFormView
{
	DECLARE_DYNCREATE(RegisterView)

protected:
	RegisterView();           // 动态创建所使用的受保护的构造函数
	virtual ~RegisterView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIEW_REGISTER };
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
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnInitialUpdate();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEnKillfocusEditInput();

    int show(int startAddress, int endAddress, uint8_t* data, int dataLength);

private:
    int         setRegister();

private:
    CListCtrl   listCtrl;
    CEdit       editInput;
    int         startAddress;
    int         endAddress;
    int         row;
};


