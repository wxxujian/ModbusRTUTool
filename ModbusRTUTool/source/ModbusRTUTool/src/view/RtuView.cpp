// RtuView.cpp: 实现文件
//

#include "../pch.h"
#include "../../res/resource.h"
#include "../util/serialport/SerialPort.h"
#include <string>
#include <list>
#include "RtuView.h"
#include "../controller/ModbusRTUToolDlg.h"

using namespace std;


// RtuView

IMPLEMENT_DYNCREATE(RtuView, CFormView)

RtuView::RtuView()
	: CFormView(IDD_VIEW_RTU)
{
    isPortError = false;
    isDeviceAddressError = false;
    isBeginAddressError = false;
    isEndAddressError = false;
    isBeginAddressGTEndAddressError = false;            // 开始地址大于结束地址
    isEndAddressGTBeginAddressMoreError = false;        // 结束地址大于开始地址太多
}

RtuView::~RtuView()
{
}

void RtuView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_PORT, comboPort);
    DDX_Control(pDX, IDC_COMBO_BAUD, comboBaud);
    DDX_Control(pDX, IDC_COMBO_DATABITS, comboDataBits);
    DDX_Control(pDX, IDC_COMBO_STOPBITS, comboStopBits);
    DDX_Control(pDX, IDC_BUTTON_OPEN, buttonOpen);
    DDX_Control(pDX, IDC_BUTTON_CLOSE, buttonClose);
    DDX_Control(pDX, IDC_EDIT_DEVICE_ADDRESS, editDeviceAddress);
    DDX_Control(pDX, IDC_EDIT_BEGIN_ADDRESS, editBeginAddress);
    DDX_Control(pDX, IDC_EDIT_END_ADDRESS, editEndAddress);
    DDX_Control(pDX, IDC_EDIT_READ_PERIOD, editReadPeriod);
    DDX_Control(pDX, IDC_RADIO_FUNCTION_CODE_0X06, radioFunctionCode0x06);
    DDX_Control(pDX, IDC_RADIO_FUNCTION_CODE_0X10, radioFunctionCode0x10);
    DDX_Control(pDX, IDC_COMBO_PARITY, comboParity);
    DDX_Control(pDX, IDC_BUTTON_SEARCH, buttonSearch);
}

BEGIN_MESSAGE_MAP(RtuView, CFormView)
    ON_BN_CLICKED(IDC_BUTTON_OPEN, &RtuView::OnBnClickedButtonOpen)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &RtuView::OnBnClickedButtonClose)
    ON_BN_CLICKED(IDC_BUTTON_SEARCH, &RtuView::OnBnClickedButtonSearch)
END_MESSAGE_MAP()


// RtuView 诊断

#ifdef _DEBUG
void RtuView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void RtuView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// RtuView 消息处理程序

void RtuView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    // TODO: 在此添加专用代码和/或调用基类

    //获取串口号
    OnBnClickedButtonSearch();

    buttonOpen.ShowWindow(SW_SHOW);
    buttonClose.ShowWindow(SW_HIDE);

    // 波特率
    //comboBaud.AddString(L"110");
    comboBaud.AddString(L"300");
    comboBaud.AddString(L"600");
    comboBaud.AddString(L"1200");
    comboBaud.AddString(L"2400");
    comboBaud.AddString(L"4800");
    comboBaud.AddString(L"9600");
    comboBaud.AddString(L"14400");
    comboBaud.AddString(L"19200");
    comboBaud.AddString(L"38400");
    comboBaud.AddString(L"56000");
    comboBaud.AddString(L"57600");
    comboBaud.AddString(L"115200");
    //comboBaud.AddString(L"921600");
    comboBaud.SetCurSel(5);

    // 数据位
    comboDataBits.AddString(L"5");
    comboDataBits.AddString(L"6");
    comboDataBits.AddString(L"7");
    comboDataBits.AddString(L"8");
    comboDataBits.SetCurSel(3);

    // 停止位
    comboStopBits.AddString(L"1");
    comboStopBits.AddString(L"1.5");
    comboStopBits.AddString(L"2");
    comboStopBits.SetCurSel(0);

    // 校验位
    comboParity.AddString(L"None");
    comboParity.AddString(L"Odd");
    comboParity.AddString(L"Even");
    comboParity.AddString(L"Mark");
    comboParity.AddString(L"Space");
    comboParity.SetCurSel(0);

    SetDlgItemInt(IDC_EDIT_DEVICE_ADDRESS, 100);
    SetDlgItemInt(IDC_EDIT_BEGIN_ADDRESS, 0);
    SetDlgItemInt(IDC_EDIT_END_ADDRESS, 35);
    SetDlgItemInt(IDC_EDIT_READ_PERIOD, 1000);

    radioFunctionCode0x06.SetCheck(TRUE);
}

void RtuView::OnBnClickedButtonSearch()
{
    // TODO: 在此添加控件通知处理程序代码
    comboPort.ResetContent();
        //获取串口号
    std::vector<itas109::SerialPortInfo> m_portsList = itas109::CSerialPortInfo::availablePortInfos();
    TCHAR m_regKeyValue[255];
    for (int i = 0; i < m_portsList.size(); i++)
    {
#ifdef UNICODE
        int iLength;
        const char* _char = m_portsList[i].portName.c_str();
        iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
        MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, m_regKeyValue, iLength);
#else
        strcpy_s(m_regKeyValue, 255, m_portsList[i].portName.c_str());
#endif
        comboPort.AddString(m_regKeyValue);
    }

}

void RtuView::OnBnClickedButtonOpen()
{
    // TODO: 在此添加控件通知处理程序代码

    if (getPort() == 0) {
        MessageBox(L"请选择串口");
        comboPort.SetFocus();
        return;
    }

    if (checkRead()) {
        return;
    }

    ModbusRTUToolDlg* pModbusRTUToolDlg = (ModbusRTUToolDlg*)AfxGetMainWnd();
    if (pModbusRTUToolDlg->openPort()) {
        comboPort.SetFocus();
    } else {
        buttonSearch.EnableWindow(FALSE);
        buttonOpen.ShowWindow(SW_HIDE);
        buttonClose.ShowWindow(SW_SHOW);
        comboPort.EnableWindow(FALSE);
        comboBaud.EnableWindow(FALSE);
        comboDataBits.EnableWindow(FALSE);
        comboStopBits.EnableWindow(FALSE);
        comboParity.EnableWindow(FALSE);
    }
}


void RtuView::OnBnClickedButtonClose()
{
    // TODO: 在此添加控件通知处理程序代码
    ModbusRTUToolDlg* pModbusRTUToolDlg = (ModbusRTUToolDlg*)AfxGetMainWnd();
    buttonSearch.EnableWindow(TRUE);
    buttonOpen.ShowWindow(SW_SHOW);
    buttonClose.ShowWindow(SW_HIDE);
    pModbusRTUToolDlg->closePort();
    comboPort.EnableWindow(TRUE);
    comboBaud.EnableWindow(TRUE);
    comboDataBits.EnableWindow(TRUE);
    comboStopBits.EnableWindow(TRUE);
    comboParity.EnableWindow(TRUE);
}

int     RtuView::checkRead()
{
    if (getDeviceAddress() > 255) {
        if (isDeviceAddressError == false) {
            isDeviceAddressError = true;
            MessageBox(L"设备地址不能大于255");
        }
        editDeviceAddress.SetTextColor(RGB(255, 0, 0));
        editDeviceAddress.SetFocus();
        return 2;
    } else {
        isDeviceAddressError = false;
        editDeviceAddress.SetTextColor(RGB(0, 0, 0));
    }

    if (getBeginAddress() > 65535) {
        if (isBeginAddressError == false) {
            isBeginAddressError = true;
            MessageBox(L"起始地址不能大于65535");
        }
        editBeginAddress.SetTextColor(RGB(255, 0, 0));
        editBeginAddress.SetFocus();
        return 3;
    }
    else {
        isBeginAddressError = false;
        editBeginAddress.SetTextColor(RGB(0, 0, 0));
    }

    if (getEndAddress() > 65535) {
        if (isEndAddressError == false) {
            isEndAddressError = true;
            MessageBox(L"结束地址不能大于65535");
        }
        editEndAddress.SetTextColor(RGB(255, 0, 0));
        editEndAddress.SetFocus();
        return 4;
    }
    else {
        isEndAddressError = false;
        editEndAddress.SetTextColor(RGB(0, 0, 0));
    }

    if (getBeginAddress() > getEndAddress()) {
        if (isBeginAddressGTEndAddressError == false) {
            isBeginAddressGTEndAddressError = true;
            MessageBox(L"起始地址不能大于结束地址");
        }
        editBeginAddress.SetTextColor(RGB(255, 0, 0));
        editEndAddress.SetTextColor(RGB(255, 0, 0));
        return 5;
    }
    else {
        isBeginAddressGTEndAddressError = false;
        editBeginAddress.SetTextColor(RGB(0, 0, 0));
        editEndAddress.SetTextColor(RGB(0, 0, 0));
    }

    if (getEndAddress() - getBeginAddress() > 126) {
        if (isEndAddressGTBeginAddressMoreError == false) {
            isEndAddressGTBeginAddressMoreError = true;
            MessageBox(L"寄存器数量不能超过127个");
        }
        editBeginAddress.SetTextColor(RGB(255, 0, 0));
        editEndAddress.SetTextColor(RGB(255, 0, 0));
        return 6;
    }
    else {
        isEndAddressGTBeginAddressMoreError = false;
        editBeginAddress.SetTextColor(RGB(0, 0, 0));
        editEndAddress.SetTextColor(RGB(0, 0, 0));
    }

    return 0;
}

int RtuView::getPort() const
{
    CString str;
    comboPort.GetWindowTextW(str);
    str = str.Right(str.GetLength() - 3);
    return _ttoi(str);
}

int RtuView::getBaud() const
{
    CString str;
    comboBaud.GetWindowTextW(str);
    return _ttoi(str);
}

int RtuView::getDataBits() const
{
    CString str;
    comboDataBits.GetWindowTextW(str);
    return _ttoi(str);
}

int RtuView::getStopBits() const
{
    return comboStopBits.GetCurSel();
}

TCHAR RtuView::getParity() const
{
    CString str;
    comboParity.GetWindowTextW(str);
    return str.GetAt(0);
}

int RtuView::getDeviceAddress() const
{
    return GetDlgItemInt(IDC_EDIT_DEVICE_ADDRESS);
}

int RtuView::getBeginAddress() const
{
    return GetDlgItemInt(IDC_EDIT_BEGIN_ADDRESS);
}

int RtuView::getEndAddress() const
{
    return GetDlgItemInt(IDC_EDIT_END_ADDRESS);
}

int RtuView::getReadPeriod() const
{
    return GetDlgItemInt(IDC_EDIT_READ_PERIOD);
}

int RtuView::getWriteFunctionCode() const
{
    return radioFunctionCode0x06.GetCheck() ? 0x06 : 0x10;
}



