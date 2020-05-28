
// ModbusRTUToolDlg.h: 头文件
//

#pragma once
#include "../model/handler/ModbusRtuHandler.h"


// ModbusRTUToolDlg 对话框
class ModbusRTUToolDlg : public CDialogEx
{
// 构造
public:
	ModbusRTUToolDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODBUSRTUTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL    PreTranslateMessage(MSG* pMsg);
    afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void    OnSize(UINT nType, int cx, int cy);
    afx_msg void    OnDestroy();
    afx_msg void    OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnModbusRxFrame(WPARAM dataBuff, LPARAM dataLength);
    afx_msg LRESULT OnModbusTxFrame(WPARAM dataBuff, LPARAM dataLength);
    afx_msg LRESULT OnSetRegister(WPARAM registerAddress, LPARAM registerValue);

    int openPort();
    int closePort();
    int setRegister(int registerAddress, int registerValue);
private:
    CFrameWnd*              m_pFrameWnd;
    CSplitterWnd            splitterWnd;
    ModbusRtuHandler        serialPortHandler;
};
