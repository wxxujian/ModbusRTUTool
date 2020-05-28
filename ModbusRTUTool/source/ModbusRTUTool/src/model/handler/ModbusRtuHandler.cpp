// ModbusRtuHandler.cpp: 实现文件
//

#include "../../pch.h"
#include "../../util/crc16.h"
#include "ModbusRtuHandler.h"


// ModbusRtuHandler

IMPLEMENT_DYNAMIC(ModbusRtuHandler, CWnd)

ModbusRtuHandler::ModbusRtuHandler(CWnd* pParentWnd)
{
    rxLength = 0;
    startPosion = -1;
    //Create(AfxRegisterWndClass(0), L"", WS_OVERLAPPED, CRect(0, 0, 2, 2), nullptr, 0, nullptr);
}

ModbusRtuHandler::~ModbusRtuHandler()
{
}


BEGIN_MESSAGE_MAP(ModbusRtuHandler, CWnd)
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_MESSAGE(WM_COMM_RXCHAR, ModbusRtuHandler::OnCommRxChar)
END_MESSAGE_MAP()



// ModbusRtuHandler 消息处理程序

int ModbusRtuHandler::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  在此添加您专用的创建代码
    //MessageBox(L"ModbusRtuHandler::OnCreate");
    
    return 0;
}

void ModbusRtuHandler::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (serialPort.IsOpened()) {
        uint16_t registerCount = readEndAddress - readBeginAddress + 1;
        uint8_t  txBuff[16];
        txBuff[0] = deviceAddress;
        txBuff[1] = 0x03;
        txBuff[2] = (readBeginAddress & 0xFF00) >> 8;
        txBuff[3] = (readBeginAddress & 0x00FF);
        txBuff[4] = (uint8_t)(registerCount >> 8);
        txBuff[5] = (uint8_t)registerCount;
        uint16_t crc = getCRC16(txBuff, 6);
        txBuff[6] = (uint8_t)crc;
        txBuff[7] = (uint8_t)(crc >> 8);
        serialPort.WriteToPort(txBuff, 8);
        ::SendMessage(GetParent()->GetSafeHwnd(), WM_MODBUS_TX_FRAME, (WPARAM)txBuff, 8);
    }

    CWnd::OnTimer(nIDEvent);
}


afx_msg LRESULT ModbusRtuHandler::OnCommRxChar(WPARAM ch, LPARAM port) {

    rxBuff[rxLength++] = (uint8_t)ch;

    if (rxLength < 5) return 1;

    // 寻找帧起始位置
    //int startPosion = -1;
    if (startPosion < 0) {
        for (int i = 0; i < rxLength; ++i) {
            if (rxBuff[i] == deviceAddress) {
                startPosion = i;
            }
        }
        if (startPosion < 0) {
            rxLength = 0;
            startPosion = -1;
            return 1;
        }
    }

    // 
    int length = 0;
    switch (rxBuff[startPosion + 1]) {
    case 0x03:
        length = rxBuff[startPosion + 2] + 5;
        break;

    case 0x83:
        length = 5;
        break;

    case 0x06:
        length = 8;
        break;

    case 0x86:
        length = 5;
        break;

    case 0x10:
        length = 8;
        break;

    case 0x90:
        length = 5;
        break;

    default:
        memcpy(rxBuff, rxBuff + 1, rxLength - 1);
        rxLength -= 1;
        startPosion = -1;
        break;
    }

    if (length > (rxLength - startPosion)) {
        return 2;
    }

    uint16_t crc = getCRC16(rxBuff + startPosion, length - 2);
    uint16_t crc2 = (rxBuff[startPosion + length - 1] << 8) + rxBuff[startPosion + length - 2];

    if (crc != crc2) {
        memcpy(rxBuff, rxBuff + startPosion + 2, rxLength - startPosion - 2);
        //for (int i = startPosion + 2, j = 0; i < rxLength; ++i, ++j) {
        //    rxBuff[j] = rxBuff[i];
        //}
        rxLength -= startPosion + 2;
        //loggerView->info(L"OnCommRxStr::3");
        return 3;
    }

    memcpy(frameData, rxBuff + startPosion, length);
    memcpy(rxBuff, rxBuff + startPosion + length, rxLength - startPosion - length);
    rxLength -= startPosion + length;
    startPosion = -1;

    ::SendMessage(GetParent()->GetSafeHwnd(), WM_MODBUS_RX_FRAME, (WPARAM)frameData, (LPARAM)length);

    return 0;
}

void    ModbusRtuHandler::setDeviceAddress(int deviceAddress)
{
    if (this->deviceAddress != deviceAddress) {
        this->deviceAddress = deviceAddress;
    }
}

int     ModbusRtuHandler::getDeviceAddress() const
{
    return deviceAddress;
}

void    ModbusRtuHandler::setReadBeginAddress(int readBeginAddress)
{
    if (this->readBeginAddress != readBeginAddress) {
        this->readBeginAddress = readBeginAddress;
    }
}

int     ModbusRtuHandler::getReadBeginAddress() const
{
    return readBeginAddress;
}

void    ModbusRtuHandler::setReadEndAddress(int readEndAddress)
{
    if (this->readEndAddress != readEndAddress) {
        this->readEndAddress = readEndAddress;
    }
}

int     ModbusRtuHandler::getReadEndAddress() const
{
    return readEndAddress;
}

void    ModbusRtuHandler::setReadPeriod(int readPeriod)
{
    if (this->readPeriod == readPeriod) {
        return;
    }

    KillTimer(0);
    this->readPeriod = readPeriod;
    if (readPeriod <= 0) {
        return;
    }

    SetTimer(0, readPeriod, nullptr);
}

int     ModbusRtuHandler::getReadPeriod() const
{
    return readPeriod;
}

int     ModbusRtuHandler::openPort(UINT portnr /*= 1*/, UINT baud /*= 9600*/, UINT databits /*= 8*/, UINT stopsbits /*= ONESTOPBIT*/, TCHAR parity /*= _T('N')*/)
{
    rxLength = 0;

    if (GetSafeHwnd() == nullptr) {
        return 1;
    }

    if (!serialPort.InitPort(GetSafeHwnd(), portnr, baud, parity, databits, stopsbits)) {
        return 2;
    }

    if (!serialPort.StartMonitoring()) {
        return 3;
    }

    //if (!serialPort.IsOpened()) {
    //    return 2;
    //}

    if (readPeriod > 0) {
        SetTimer(0, readPeriod, nullptr);
    }
    
    return 0;
}

void    ModbusRtuHandler::closePort()
{
    KillTimer(0);
    serialPort.ClosePort();
}


int     ModbusRtuHandler::receive(uint8_t* buff, size_t buffLength)
{
    int length = buffLength < rxLength ? buffLength : rxLength;
    memcpy(buff, frameData, length);
    return length;
}

int     ModbusRtuHandler::send(int functionCode, int registerAddress, int registerValue)
{
    if (!serialPort.IsOpened()) {
        return 1;
    }

    uint8_t  txBuff[16];
    uint16_t crc;
    switch (functionCode)
    {
    case 0x06:
        txBuff[0] = deviceAddress;
        txBuff[1] = 0x06;
        txBuff[2] = (registerAddress & 0xFF00) >> 8;
        txBuff[3] = (registerAddress & 0x00FF);
        txBuff[4] = (registerValue & 0xFF00) >> 8;
        txBuff[5] = (registerValue & 0x00FF);
        crc       = getCRC16(txBuff, 6);
        txBuff[6] = (uint8_t)crc;
        txBuff[7] = (uint8_t)(crc >> 8);
        serialPort.WriteToPort(txBuff, 8);

        ::SendMessage(GetParent()->GetSafeHwnd(), WM_MODBUS_TX_FRAME, (WPARAM)txBuff, (LPARAM)8);
        break;

    case 0x10:
        txBuff[0] = deviceAddress;
        txBuff[1] = 0x10;
        txBuff[2] = (registerAddress & 0xFF00) >> 8;
        txBuff[3] = (registerAddress & 0x00FF);
        txBuff[4] = 0x00;
        txBuff[5] = 0x01;
        txBuff[6] = 0x02;
        txBuff[7] = (registerValue & 0xFF00) >> 8;
        txBuff[8] = (registerValue & 0x00FF);
        crc = getCRC16(txBuff, 9);
        txBuff[9] = (uint8_t)crc;
        txBuff[10] = (uint8_t)(crc >> 8);
        serialPort.WriteToPort(txBuff, 11);
        ::SendMessage(GetParent()->GetSafeHwnd(), WM_MODBUS_TX_FRAME, (WPARAM)txBuff, (LPARAM)11);
        break;

    default:
        break;
    }
    return 0;
}


