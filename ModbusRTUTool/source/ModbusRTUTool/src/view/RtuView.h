#pragma once
#include "control/Edit.h"



// RtuView 窗体视图

class RtuView : public CFormView
{
	DECLARE_DYNCREATE(RtuView)

protected:
	RtuView();           // 动态创建所使用的受保护的构造函数
	virtual ~RtuView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIEW_RTU };
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
    afx_msg void OnBnClickedButtonOpen();
    afx_msg void OnBnClickedButtonClose();
    virtual void OnInitialUpdate();

    int     getPort() const;
    int     getBaud() const;
    int     getDataBits() const;
    int     getStopBits() const;
    TCHAR   getParity() const;

    int     getDeviceAddress() const;
    int     getBeginAddress() const;
    int     getEndAddress() const;
    int     getReadPeriod() const;
    int     getWriteFunctionCode() const;
    int     checkRead();

private:
    CComboBox   comboPort;
    CComboBox   comboBaud;
    CComboBox   comboDataBits;
    CComboBox   comboStopBits;
    CComboBox   comboParity;
    CButton     buttonOpen;
    CButton     buttonClose;
    CButton     radioFunctionCode0x06;
    CButton     radioFunctionCode0x10;
    Edit        editDeviceAddress;
    Edit        editBeginAddress;
    Edit        editEndAddress;
    Edit        editReadPeriod;
    bool        isPortError;                                // 串口号未选择
    bool        isDeviceAddressError;                       // 设备地址超范围
    bool        isBeginAddressError;                        // 开始地址超范围
    bool        isEndAddressError;                          // 结束地址超范围
    bool        isBeginAddressGTEndAddressError;            // 开始地址大于结束地址
    bool        isEndAddressGTBeginAddressMoreError;        // 结束地址大于开始地址太多
public:
    CButton buttonSearch;
    afx_msg void OnBnClickedButtonSearch();
};


