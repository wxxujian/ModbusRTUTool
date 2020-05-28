#pragma once

#include <stdint.h>
#include "../../util/serialport/SerialPort.h"
#include "../../view/LoggerView.h"

#define WM_MODBUS_RX_FRAME              WM_USER + 130
#define WM_MODBUS_TX_FRAME              WM_USER + 131

// ModbusRtuHandler

class ModbusRtuHandler : public CWnd
{
	DECLARE_DYNAMIC(ModbusRtuHandler)

public:
    ModbusRtuHandler(CWnd* pParentWnd);
	virtual ~ModbusRtuHandler();


protected:
	DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnCommRxChar(WPARAM ch, LPARAM port);

public:
    void    setDeviceAddress(int deviceAddress);
    int     getDeviceAddress() const;
    void    setReadBeginAddress(int readStartAddress);
    int     getReadBeginAddress() const;
    void    setReadEndAddress(int readEndAddress);
    int     getReadEndAddress() const;
    void    setReadPeriod(int readPeriod);
    int     getReadPeriod() const;

    int     openPort(UINT portnr = 1, UINT baud = 9600, UINT databits = 8, UINT stopsbits = ONESTOPBIT, TCHAR parity = _T('N'));
    void    closePort();

    int     receive(uint8_t* buff, size_t buffLength);
    int     send(int functionCode, int registerAddress, int registerValue);

private:
    itas109::CSerialPort serialPort;

    int     deviceAddress;

    int     readBeginAddress;
    int     readEndAddress;
    int     readPeriod;

    int     writeFunctionCode;

    uint8_t rxBuff[512];            // 接收缓冲区
    size_t     rxLength;               // 接收缓冲区中的字符数

    uint8_t frameData[512];              // 帧数据
    int     frameLength;

    int     startPosion;
};


