/*
**	FILENAME			CSerialPort.cpp
**
**	PURPOSE				This class can read, write and watch one serial port.
**						It sends messages to its owner when something happends on the port
**						The class creates a thread for reading and writing so the main
**						program is not blocked.
**
**	CREATION DATE		15-09-1997
**	LAST MODIFICATION	12-11-1997
**
**	AUTHOR				Remon Spekreijse
**
**  2007-12-25 mrlong    https://code.google.com/p/mycom/
**  2011-11-06 liquanhai http://blog.csdn.net/liquanhai/article/details/6941574
**  2013-12-04 viruscamp https://github.com/viruscamp
**  2014-01-10 itas109   http://blog.csdn.net/itas109
**  2014-12-18 liquanhai http://blog.csdn.net/liquanhai/article/details/6941574
**  2016-05-06 itas109   http://blog.csdn.net/itas109
**  2016-06-22 itas109   http://blog.csdn.net/itas109
**  2016-06-29 itas109   http://blog.csdn.net/itas109
**  2016-08-02 itas109   http://blog.csdn.net/itas109
**  2016-08-10 itas109   http://blog.csdn.net/itas109
**  2017-02-14 itas109   http://blog.csdn.net/itas109
**  2017-03-12 itas109   http://blog.csdn.net/itas109
**  2017-12-16 itas109   http://blog.csdn.net/itas109
**  2018-02-14 itas109   http://blog.csdn.net/itas109
**  2018-06-15 itas109   http://blog.csdn.net/itas109
**  2018-06-21 itas109   http://blog.csdn.net/itas109
*/

#include "../../pch.h"
#include "SerialPort.h"
#include "assert.h"

using namespace itas109;

#ifdef _SEND_DATA_WITH_SIGSLOT
	signal3<unsigned char*, int, int> CSerialPort::sendMessageSignal;//���еľ�̬��Ա��������ͷ�ļ��ж������⣬��Ӧ���������¶���һ��
#endif


//��ȡע���ָ�����ݵ�list
bool getRegKeyValues(std::string regKeyPath, std::list<std::string> & portsList)
{
//https://msdn.microsoft.com/en-us/library/ms724256

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

	HKEY hKey;

	TCHAR		achValue[MAX_VALUE_NAME];					// buffer for subkey name
	DWORD		cchValue = MAX_VALUE_NAME;					// size of name string 
	TCHAR		achClass[MAX_PATH] = TEXT("");				// buffer for class name 
	DWORD		cchClassName = MAX_PATH;					// size of class string 
	DWORD		cSubKeys = 0;								// number of subkeys 
	DWORD		cbMaxSubKey;								// longest subkey size 
	DWORD		cchMaxClass;								// longest class string 
	DWORD		cKeyNum;									// number of values for key 
	DWORD		cchMaxValue;								// longest value name 
	DWORD		cbMaxValueData;								// longest value data 
	DWORD		cbSecurityDescriptor;						// size of security descriptor 
	FILETIME	ftLastWriteTime;							// last write time 

	int         iRet    = -1;
	bool        bRet    = false;

	std::string m_keyValue;

	TCHAR m_regKeyPath[MAX_KEY_LENGTH];

	TCHAR strDSName[MAX_VALUE_NAME];
	memset(strDSName, 0, MAX_VALUE_NAME);
	DWORD nValueType    = 0;
	DWORD nBuffLen      = 10;

#ifdef UNICODE
	int iLength;
	const char * _char = regKeyPath.c_str();
	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, m_regKeyPath, iLength);
#else
	strcpy_s(m_regKeyPath, MAX_KEY_LENGTH, regKeyPath.c_str());
#endif

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_regKeyPath, 0, KEY_READ, &hKey))
	{
		// Get the class name and the value count. 
		iRet = RegQueryInfoKey(
			hKey,                    // key handle 
			achClass,                // buffer for class name 
			&cchClassName,           // size of class string 
			NULL,                    // reserved 
			&cSubKeys,               // number of subkeys 
			&cbMaxSubKey,            // longest subkey size 
			&cchMaxClass,            // longest class string 
			&cKeyNum,                // number of values for this key 
			&cchMaxValue,            // longest value name 
			&cbMaxValueData,         // longest value data 
			&cbSecurityDescriptor,   // security descriptor 
			&ftLastWriteTime);       // last write time 

		if (!portsList.empty())
		{
			portsList.clear();
		}

		// Enumerate the key values. 
		if (cKeyNum > 0 && ERROR_SUCCESS == iRet)
		{
			for (int i = 0; i < (int)cKeyNum; i++)
			{
				cchValue    = MAX_VALUE_NAME;
				achValue[0] = '\0';
				nBuffLen    = MAX_KEY_LENGTH;//��ֹ ERROR_MORE_DATA 234L ����

				if (ERROR_SUCCESS == RegEnumValue(hKey, i, achValue, &cchValue, NULL, NULL, (LPBYTE)strDSName, &nBuffLen))
				{

#ifdef UNICODE
					int iLen = WideCharToMultiByte(CP_ACP, 0, strDSName, -1, NULL, 0, NULL, NULL);
					char* chRtn = new char[iLen * sizeof(char)];
					WideCharToMultiByte(CP_ACP, 0, strDSName, -1, chRtn, iLen, NULL, NULL);
					m_keyValue = std::string(chRtn);
					delete chRtn;
					chRtn = NULL;
#else
					m_keyValue = std::string(strDSName);
#endif
					portsList.push_back(m_keyValue);
				}
			}
		}
		else
		{

		}
	}

	if (portsList.empty())
	{
		bRet = false;
	}
	else
	{
		bRet = true;
	}


	RegCloseKey(hKey);

	return bRet;
}

// �콡�����2020.05.16
static bool getRegKeyValues(wchar_t* subKey, std::list<std::wstring>& portsList)
{
    HKEY hKey;

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
        return 1;
    }

    wchar_t szValueName[128];
    wchar_t szPortName[128];
    LONG status;
    DWORD dwIndex = 0;
    DWORD dwSizeValueName = 128;
    DWORD dwSizeofPortName = 128;
    DWORD Type;
    do
    {
        dwSizeValueName = 128;
        dwSizeofPortName = 128;
        status = RegEnumValue(hKey, dwIndex++, szValueName, &dwSizeValueName, NULL, &Type, (unsigned char*)szPortName, &dwSizeofPortName);
        if ((status == ERROR_SUCCESS))
        {
            portsList.push_back(szPortName);

        }
    } while ((status != ERROR_NO_MORE_ITEMS));
    RegCloseKey(hKey);

    return 0;
}

//
// Constructor
//
CSerialPort::CSerialPort()
{
	m_hComm = INVALID_HANDLE_VALUE;

	// initialize overlapped structure members to zero
	///��ʼ���첽�ṹ��
	m_ov.Offset = 0;
	m_ov.OffsetHigh = 0;

	// create events
	m_ov.hEvent = NULL;
	m_hWriteEvent = NULL;
	m_hShutdownEvent = NULL;

	m_szWriteBuffer = NULL;

	m_bThreadAlive = FALSE;
	m_nWriteSize = 1;
	m_bIsSuspened = FALSE;

	// create events
	m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// initialize the event objects
	///�¼������ʼ�����趨���ȼ���
	m_hEventArray[0] = m_hShutdownEvent;	// highest priority
	//Ϊ������Щ�����豸���������룬��һֱ���ض��¼���ʹ�����߳�������
	//���Խ���д���������߳��У������޸Ķ�д�¼����ȼ�
	//�޸����ȼ�������������
	//����һΪ�����߳���WaitCommEvent()���������������䣺
	//if (WAIT_OBJECT_O == WaitForSingleObject(port->m_hWriteEvent, 0))
	//	ResetEvent(port->m_ov.hEvent);
	//������Ϊ��ʼ��ʱ���޸ģ�������������䣺
	m_hEventArray[1] = m_hWriteEvent;
	m_hEventArray[2] = m_ov.hEvent;

	// initialize critical section
	///��ʼ���ٽ���Դ
	InitializeCriticalSection(&m_csCommunicationSync);
}

//
// Delete dynamic memory
//
CSerialPort::~CSerialPort()
{
	ClosePort();

	// close Handles  
	if (m_hShutdownEvent != NULL)
	{
		CloseHandle(m_hShutdownEvent);
	}
	if (m_ov.hEvent != NULL)
	{
		CloseHandle(m_ov.hEvent);
	}
	if (m_hWriteEvent != NULL)
	{
		CloseHandle(m_hWriteEvent);
	}

	// delete critical section
	//�ͷ��ٽ���Դ
	DeleteCriticalSection(&m_csCommunicationSync);
}

//
// Initialize the port. This can be port 1 to MaxSerialPortNum.
///��ʼ�����ڡ�ֻ����1-MaxSerialPortNum
//
//parity:
//  n=none
//  e=even
//  o=odd
//  m=mark
//  s=space
//data:
//  5,6,7,8
//stop:
//  1,1.5,2 
//
BOOL CSerialPort::InitPort(HWND pPortOwner,	// the owner (CWnd) of the port (receives message)
	UINT  portnr,		// portnumber (1..MaxSerialPortNum)
	UINT  baud,			// baudrate
	TCHAR  parity,		// parity 
	UINT  databits,		// databits 
	UINT  stopbits,		// stopbits 
	DWORD dwCommEvents,	// EV_RXCHAR, EV_CTS etc
	UINT  writebuffersize,// size to the writebuffer

	DWORD   ReadIntervalTimeout,
	DWORD   ReadTotalTimeoutMultiplier,
	DWORD   ReadTotalTimeoutConstant,
	DWORD   WriteTotalTimeoutMultiplier,
	DWORD   WriteTotalTimeoutConstant)

{
	assert(portnr > 0 && portnr < MaxSerialPortNum);
	assert(pPortOwner != NULL);

	ClosePort();

	// set buffersize for writing and save the owner
	m_pOwner = pPortOwner;

	if (m_szWriteBuffer != NULL)
	{
		delete[] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
	}
	m_szWriteBuffer = new BYTE[writebuffersize];

	m_nPortNr = portnr;

	m_nWriteBufferSize = writebuffersize;
	m_dwCommEvents = dwCommEvents;

	BOOL bResult = FALSE;
	TCHAR *szPort = new TCHAR[MAX_PATH];
	TCHAR *szBaud = new TCHAR[MAX_PATH];

	/*
	����̲߳�����ͬ������ʱ��һ������Ҫ��˳����ʵģ�������������ݴ��ң�
	�޷��������ݣ�������������Ϊ���������⣬����Ҫ���뻥���������ÿ
	���̶߳���˳��ط��ʱ�������������Ҫʹ��EnterCriticalSection��
	LeaveCriticalSection������
	*/
	// now it critical!
	EnterCriticalSection(&m_csCommunicationSync);

	// if the port is already opened: close it
	///�����Ѵ򿪾͹ص�
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}

	// prepare port strings
	_stprintf_s(szPort, MAX_PATH, _T("\\\\.\\COM%d"), portnr);///������ʾCOM10���϶˿�//add by itas109 2014-01-09

	// stop is index 0 = 1 1=1.5 2=2
	int mystop;
	int myparity;
	switch (stopbits)
	{
	case 0:
		mystop = ONESTOPBIT;
		break;
	case 1:
		mystop = ONE5STOPBITS;
		break;
	case 2:
		mystop = TWOSTOPBITS;
		break;
		//����Ĭ���������Ϊstopbits=1.5ʱ��SetCommState�ᱨ��
		//һ��ĵ��Դ��ڲ�֧��1.5ֹͣλ�����1.5ֹͣλ�ƺ����ں��⴫���ϵġ�
		//by itas109 20160506
	default:
		mystop = ONESTOPBIT;
		break;
	}
	myparity = 0;
	parity = _totupper(parity);
	switch (parity)
	{
	case _T('N'):
		myparity = 0;
		break;
	case _T('O'):
		myparity = 1;
		break;
	case _T('E'):
		myparity = 2;
		break;
	case _T('M'):
		myparity = 3;
		break;
	case _T('S'):
		myparity = 4;
		break;
		//����Ĭ�������
		//by itas109 20160506
	default:
		myparity = 0;
		break;
	}
	_stprintf_s(szBaud, MAX_PATH, _T("baud=%d parity=%c data=%d stop=%d"), baud, parity, databits, mystop);

	// get a handle to the port
	/*
	ͨ�ų�����CreateFile��ָ�������豸����صĲ������ԣ��ٷ���һ�������
	�þ���������ں�����ͨ�Ų��������ᴩ����ͨ�Ź��̴��ڴ򿪺�������
	������ΪĬ��ֵ�����ݾ�����Ҫ��ͨ������GetCommState(hComm,&&dcb)��ȡ
	��ǰ�����豸���ƿ�DCB���ã��޸ĺ�ͨ��SetCommState(hComm,&&dcb)����д
	�롣����ReadFile()��WriteFile()������API����ʵ�ִ��ڶ�д��������Ϊ��
	��ͨ�ŷ�ʽ�������������һ������Ϊָ��OVERLAPPED�ṹ�ķǿ�ָ�룬�ڶ�
	д��������ֵΪFALSE������£�����GetLastError()����������ֵΪERROR_IO_PENDING��
	����I/O�������ң�������ת���̨����ִ�С���ʱ��������WaitForSingleObject()
	���ȴ������źŲ�������ȴ�ʱ��
	*/
	m_hComm = CreateFile(szPort,						// communication port string (COMX)
		GENERIC_READ | GENERIC_WRITE,	// read/write types
		0,								// comm devices must be opened with exclusive access
		NULL,							// no security attributes
		OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
		FILE_FLAG_OVERLAPPED,			// Async I/O
		0);							// template must be 0 for comm devices

	///����ʧ��
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		//add by itas109 2016-08-02
		//���ڴ�ʧ�ܣ�������ʾ��Ϣ
		switch (GetLastError())
		{
			//���ڲ�����
		case ERROR_FILE_NOT_FOUND:
		{
			TCHAR Temp[200] = { 0 };
			_stprintf_s(Temp, 200, _T("COM%d ERROR_FILE_NOT_FOUND,Error Code:%d"), portnr, GetLastError());
			//MessageBox(NULL, Temp, _T("COM InitPort Error"), MB_ICONERROR);
			break;
		}
		//���ھܾ�����
		case ERROR_ACCESS_DENIED:
		{
			TCHAR Temp[200] = { 0 };
			_stprintf_s(Temp, 200, _T("COM%d ERROR_ACCESS_DENIED,Error Code:%d"), portnr, GetLastError());
			//MessageBox(NULL, Temp, _T("COM InitPort Error"), MB_ICONERROR);
			break;
		}
		default:
			break;
		}
		// port not found
		delete[] szPort;
		delete[] szBaud;

		// release critical section
		///�ͷ��ٽ���Դ
		LeaveCriticalSection(&m_csCommunicationSync);

		return FALSE;
	}

	// set the timeout values
	///���ó�ʱ
	m_SerialPortTimeouts.ReadIntervalTimeout = ReadIntervalTimeout * 1000;
	m_SerialPortTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier * 1000;
	m_SerialPortTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant * 1000;
	m_SerialPortTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier * 1000;
	m_SerialPortTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant * 1000;

	// configure
	///����
	///�ֱ����Windows API���ô��ڲ���
	if (SetCommTimeouts(m_hComm, &m_SerialPortTimeouts))///���ó�ʱ
	{
		/*
		���Զ˿����ݵ���Ӧʱ��Ҫ����ϸ񣬿ɲ����¼�������ʽ��
		�¼�������ʽͨ�������¼�֪ͨ������ϣ�����¼�����ʱ��Windows
		�������¼��ѷ�����֪ͨ������DOS�����µ��жϷ�ʽ�����ơ�Windows
		������9�ִ���ͨ���¼����ϳ��õ����������֣�
		EV_RXCHAR:���յ�һ���ֽڣ����������뻺������
		EV_TXEMPTY:����������е����һ���ַ������ͳ�ȥ��
		EV_RXFLAG:���յ��¼��ַ�(DCB�ṹ��EvtChar��Ա)���������뻺����
		����SetCommMask()ָ�������õ��¼���Ӧ�ó���ɵ���WaitCommEvent()���ȴ���
		���ķ�����SetCommMask(hComm,0)��ʹWaitCommEvent()��ֹ
		*/
		if (SetCommMask(m_hComm, dwCommEvents))///����ͨ���¼�
		{

			if (GetCommState(m_hComm, &m_dcb))///��ȡ��ǰDCB����
			{
				m_dcb.EvtChar = 'q';
				m_dcb.fRtsControl = RTS_CONTROL_ENABLE;		// set RTS bit high!
				m_dcb.BaudRate = baud;  // add by mrlong
				m_dcb.Parity = myparity;
				m_dcb.ByteSize = databits;
				m_dcb.StopBits = mystop;

				//if (BuildCommDCB(szBaud &m_dcb))///��дDCB�ṹ
				//{
				if (SetCommState(m_hComm, &m_dcb))///����DCB
					; // normal operation... continue
				else
					ProcessErrorMessage(_T("SetCommState()"));
				//}
				//else
				//	ProcessErrorMessage("BuildCommDCB()");
			}
			else
				ProcessErrorMessage(_T("GetCommState()"));
		}
		else
			ProcessErrorMessage(_T("SetCommMask()"));
	}
	else
		ProcessErrorMessage(_T("SetCommTimeouts()"));

	delete[] szPort;
	delete[] szBaud;

	// flush the port
	// ��ֹ��д����ս��պͷ���
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// release critical section
	///�ͷ��ٽ���Դ
	LeaveCriticalSection(&m_csCommunicationSync);

	//TRACE("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);

	return TRUE;
}

//
//  The CommThread Function.
///�̺߳���
///�����̵߳Ĵ������̣�
///��鴮��-->����ѭ��{WaitCommEvent(������ѯ��)ѯ���¼�-->������¼�����-->����Ӧ����(�ر�\��\д)}
//
DWORD WINAPI CSerialPort::CommThread(LPVOID pParam)
{
	// Cast the void pointer passed to the thread back to
	// a pointer of CSerialPort class
	CSerialPort *port = (CSerialPort*)pParam;

	// Set the status variable in the dialog class to
	// TRUE to indicate the thread is running.
	///TRUE��ʾ�߳���������
	port->m_bThreadAlive = TRUE;

	// Misc. variables
	DWORD BytesTransfered = 0;
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
	COMSTAT comstat;

	BOOL  bResult = TRUE;

	// Clear comm buffers at startup
	///��ʼʱ������ڻ���
	if (port->m_hComm)		// check if the port is opened
	{
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
		//TRACE("Clear comm buffers");
	}

	// begin forever loop.  This loop will run as long as the thread is alive.
	///ֻҪ�̴߳��ھͲ��϶�ȡ����
	for (;;)
	{

		// Make a call to WaitCommEvent().  This call will return immediatly
		// because our port was created as an async port (FILE_FLAG_OVERLAPPED
		// and an m_OverlappedStructerlapped structure specified).  This call will cause the 
		// m_OverlappedStructerlapped element m_OverlappedStruct.hEvent, which is part of the m_hEventArray to 
		// be placed in a non-signeled state if there are no bytes available to be read,
		// or to a signeled state if there are bytes available.  If this event handle 
		// is set to the non-signeled state, it will be set to signeled when a 
		// character arrives at the port.

		// we do this for each port!

		/*
		WaitCommEvent������3������1pOverlapped������һ��OVERLAPPED�ṹ�ı���ָ��
		��Ҳ������NULL������NULLʱ����ʾ�ú�����ͬ���ģ������ʾ�ú������첽�ġ�
		����WaitCommEventʱ������첽��������������ɣ�����������FALSE��ϵͳ��
		WaitCommEvent����ǰ��OVERLAPPED�ṹ��ԱhEvent��Ϊ���ź�״̬���ȵ�����ͨ��
		�¼�ʱ��ϵͳ���������ź�
		*/

		bResult = WaitCommEvent(port->m_hComm, &Event, &port->m_ov);///��ʾ�ú������첽��

		if (!bResult)
		{
			// If WaitCommEvent() returns FALSE, process the last error to determin
			// the reason..
			///���WaitCommEvent����ErrorΪFALSE�����ѯ������Ϣ
			switch (dwError = GetLastError())
			{
			case ERROR_IO_PENDING: 	///���������û���ַ��ɶ� erroe code:997
			{
				// This is a normal return value if there are no bytes
				// to read at the port.
				// Do nothing and continue
				break;
			}
			case ERROR_INVALID_PARAMETER:///ϵͳ���� erroe code:87
			{
				// Under Windows NT, this value is returned for some reason.
				// I have not investigated why, but it is also a valid reply
				// Also do nothing and continue.
				break;
			}
			case ERROR_ACCESS_DENIED:///�ܾ����� erroe code:5
			{
				port->m_hComm = INVALID_HANDLE_VALUE;
				TCHAR Temp[200] = { 0 };
				_stprintf_s(Temp, 200, _T("COM%d ERROR_ACCESS_DENIED��WaitCommEvent() Error Code:%d"), port->m_nPortNr, GetLastError());
				MessageBox(NULL, Temp, _T("COM WaitCommEvent Error"), MB_ICONERROR);
				break;
			}
			case ERROR_INVALID_HANDLE:///�򿪴���ʧ�� erroe code:6
			{
				port->m_hComm = INVALID_HANDLE_VALUE;
				break;
			}
			case ERROR_BAD_COMMAND:///���ӹ����зǷ��Ͽ� erroe code:22
			{
				port->m_hComm = INVALID_HANDLE_VALUE;
				TCHAR Temp[200] = { 0 };
				_stprintf_s(Temp, 200, _T("COM%d ERROR_BAD_COMMAND��WaitCommEvent() Error Code:%d"), port->m_nPortNr, GetLastError());
				MessageBox(NULL, Temp, _T("COM WaitCommEvent Error"), MB_ICONERROR);
				break;
			}
			default:///�����������������д��ڶ�д�жϿ��������ӵĴ��󣨴���22��
			{
				// All other error codes indicate a serious error has
				//��������ʱ�������ھ����Ϊ��Ч���
				port->m_hComm = INVALID_HANDLE_VALUE;
				// occured.  Process this error.
				port->ProcessErrorMessage(_T("WaitCommEvent()"));
				break;
			}
			}
		}
		else	///WaitCommEvent()����ȷ����
		{
			// If WaitCommEvent() returns TRUE, check to be sure there are
			// actually bytes in the buffer to read.  
			//
			// If you are reading more than one byte at a time from the buffer 
			// (which this program does not do) you will have the situation occur 
			// where the first byte to arrive will cause the WaitForMultipleObjects() 
			// function to stop waiting.  The WaitForMultipleObjects() function 
			// resets the event handle in m_OverlappedStruct.hEvent to the non-signelead state
			// as it returns.  
			//
			// If in the time between the reset of this event and the call to 
			// ReadFile() more bytes arrive, the m_OverlappedStruct.hEvent handle will be set again
			// to the signeled state. When the call to ReadFile() occurs, it will 
			// read all of the bytes from the buffer, and the program will
			// loop back around to WaitCommEvent().
			// 
			// At this point you will be in the situation where m_OverlappedStruct.hEvent is set,
			// but there are no bytes available to read.  If you proceed and call
			// ReadFile(), it will return immediatly due to the async port setup, but
			// GetOverlappedResults() will not return until the next character arrives.
			//
			// It is not desirable for the GetOverlappedResults() function to be in 
			// this state.  The thread shutdown event (event 0) and the WriteFile()
			// event (Event2) will not work if the thread is blocked by GetOverlappedResults().
			//
			// The solution to this is to check the buffer with a call to ClearCommError().
			// This call will reset the event handle, and if there are no bytes to read
			// we can loop back through WaitCommEvent() again, then proceed.
			// If there are really bytes to read, do nothing and proceed.

			bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

			if (comstat.cbInQue == 0)
				continue;
		}	// end if bResult

		///���ȴ��������������߳�
		// Main wait function.  This function will normally block the thread
		// until one of nine events occur that require action.
		///�ȴ�3���¼����ض�/��/д����һ���¼������ͷ���
		//if multiple objects signal it will get the index of the first one
		//it means it lost some signal //by itas109 2017-12-17
		Event = MsgWaitForMultipleObjects(3, ///3���¼�
			port->m_hEventArray, ///�¼�����
			FALSE, ///��һ���¼������ͷ���
			INFINITE,
			QS_ALLEVENTS);///��ʱʱ��

		//deal with mutil handle tigger at the same time	//by itas109 2017-12-17
		if (Event >= WAIT_OBJECT_0 && Event < WAIT_OBJECT_0 + 3)
		{
			for (int i = Event - WAIT_OBJECT_0 - 1; i < 3; i++)
			{
				if (WaitForSingleObject(port->m_hEventArray[i], 0) == WAIT_OBJECT_0)
				{
					//TRACE("WaitForSingleObject : %d",i);

					switch (Event)
					{
					case WAIT_OBJECT_0 + 0:
					{
						// Shutdown event.  This is event zero so it will be
						// the higest priority and be serviced first.
						///�ض��¼����رմ���
						CloseHandle(port->m_hComm);
						port->m_hComm = INVALID_HANDLE_VALUE;
						port->m_bThreadAlive = FALSE;

						// Kill this thread.  break is not needed, but makes me feel better.
						//AfxEndThread(100);
						::ExitThread(100);

						break;
					}
					case WAIT_OBJECT_0 + 1: // write event ��������
					{
						// Write character event from port
						WriteChar(port);
						break;
					}
					case WAIT_OBJECT_0 + 2:	// read event ������ĸ�����Ϣ���ͳ�ȥ
					{
						GetCommMask(port->m_hComm, &CommEvent);
						if (CommEvent & EV_RXCHAR) //���յ��ַ������������뻺������
						{
							if (IsReceiveString == 1)
							{
								ReceiveStr(port);//���ַ�����
							}
							else if (IsReceiveString == 0)
							{
								ReceiveChar(port);//���ַ�����
							}
							else
							{
								//Ĭ�϶��ַ�����
								ReceiveStr(port);//���ַ�����
							}
						}

						if (CommEvent & EV_CTS) //CTS�ź�״̬�����仯
							::SendMessage(port->m_pOwner, WM_COMM_CTS_DETECTED, (WPARAM)0, (LPARAM)port->m_nPortNr);
						if (CommEvent & EV_RXFLAG) //���յ��¼��ַ������������뻺������ 
							::SendMessage(port->m_pOwner, WM_COMM_RXFLAG_DETECTED, (WPARAM)0, (LPARAM)port->m_nPortNr);
						if (CommEvent & EV_BREAK)  //�����з����ж�
							::SendMessage(port->m_pOwner, WM_COMM_BREAK_DETECTED, (WPARAM)0, (LPARAM)port->m_nPortNr);
						if (CommEvent & EV_ERR) //������·״̬������·״̬�������CE_FRAME,CE_OVERRUN��CE_RXPARITY 
							::SendMessage(port->m_pOwner, WM_COMM_ERR_DETECTED, (WPARAM)0, (LPARAM)port->m_nPortNr);
						if (CommEvent & EV_RING) //��⵽����ָʾ
							::SendMessage(port->m_pOwner, WM_COMM_RING_DETECTED, (WPARAM)0, (LPARAM)port->m_nPortNr);

						break;
					}
					case WAIT_FAILED:
					{
						// ��������ʧ��
						break;
					}
					case WAIT_TIMEOUT:
					{
						// ��ʱ
						//TRACE("WaitForMultipleObjects Timeout");
						break;
					}
					default:
					{
						//MessageBox(NULL, _T("Receive Error!"), _T("COM Receive Error"), MB_ICONERROR);
						break;
					}

					} // end switch
				}
			}
		}

	} // close forever loop

	return 0;
}

//
// start comm watching
///���������߳�
//
BOOL CSerialPort::StartMonitoring()
{
	//if (!(m_Thread = AfxBeginThread(CommThread, this)))
	if (!(m_Thread = ::CreateThread(NULL, 0, CommThread, this, 0, NULL)))
		return FALSE;
	//TRACE("Thread started\n");
	return TRUE;
}

//
// Restart the comm thread
///�ӹ���ָ������߳�
//
BOOL CSerialPort::ResumeMonitoring()
{
	//TRACE("Thread resumed\n");
	//m_Thread->ResumeThread();
	::ResumeThread(m_Thread);
	return TRUE;
}

//
// Suspend the comm thread
///��������߳�
//
BOOL CSerialPort::SuspendMonitoring()
{
	//TRACE("Thread suspended\n");
	//m_Thread->SuspendThread();
	::SuspendThread(m_Thread);
	return TRUE;
}

BOOL CSerialPort::IsThreadSuspend(HANDLE hThread)
{
	DWORD   count = SuspendThread(hThread);
	if (count == -1)
	{
		return FALSE;
	}
	ResumeThread(hThread);
	return (count != 0);
}

//
// If there is a error, give the right message
///����д��󣬸�����ʾ
//
void CSerialPort::ProcessErrorMessage(TCHAR* ErrorText)
{
	TCHAR Temp[200] = { 0 };

	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	_stprintf_s(Temp, 200, _T("WARNING:  %s Failed with the following error: \n%s\nPort: %d\n"), ErrorText, (TCHAR*)lpMsgBuf, m_nPortNr);
	MessageBox(NULL, Temp, _T("Application Error"), MB_ICONSTOP);

	LocalFree(lpMsgBuf);
}

//
// Write a character.
//
void CSerialPort::WriteChar(CSerialPort* port)
{
	if (port->m_bufferList.size() == 0)
	{
		return;
	}

	// Gain ownership of the critical section
	EnterCriticalSection(&port->m_csCommunicationSync);

	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;

	DWORD BytesSent = 0;
	//DWORD SendLen = port->m_nWriteSize;

	ResetEvent(port->m_hWriteEvent);

	//����iΪ������   
	std::list<serialPortBuffer>::iterator i;

	//in general, for mutil send data can deal once.
	for (i = port->m_bufferList.begin(); i != port->m_bufferList.end(); )
	{
		//TRACE("port->m_bufferList size :%d", port->m_bufferList.size());

		PBYTE sendData = (*i).buffer;
		int sendLen = (*i).len;

		if (bWrite)
		{
			// Initailize variables
			port->m_ov.Offset = 0;
			port->m_ov.OffsetHigh = 0;

			// Clear buffer
			PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

			//TRACE("m_szWriteBufferMap Data : %s, size : %d", (*i).buffer, (*i).len);

			//bResult = WriteFile(port->m_hComm,							// Handle to COMM Port
			//	port->m_szWriteBuffer,					// Pointer to message buffer in calling finction
			//	SendLen,	// add by mrlong
			//	//strlen((char*)port->m_szWriteBuffer),	// Length of message to send
			//	&BytesSent,								// Where to store the number of bytes sent
			//	&port->m_ov);							// Overlapped structure

			bResult = WriteFile(port->m_hComm,							// Handle to COMM Port
				sendData,					// Pointer to message buffer in calling finction
				sendLen,	// add by mrlong
				//strlen((char*)port->m_szWriteBuffer),	// Length of message to send
				&BytesSent,								// Where to store the number of bytes sent
				&port->m_ov);							// Overlapped structure

			// deal with any error codes
			if (!bResult)
			{
				DWORD dwError = GetLastError();
				switch (dwError)
				{
				case ERROR_IO_PENDING:
				{
					// continue to GetOverlappedResults()
					BytesSent = 0;
					bWrite = FALSE;
					break;
				}
				case ERROR_ACCESS_DENIED:///�ܾ����� erroe code:5
				{
					port->m_hComm = INVALID_HANDLE_VALUE;
					TCHAR Temp[200] = { 0 };
					_stprintf_s(Temp, 200, _T("COM%d ERROR_ACCESS_DENIED��WriteFile() Error Code:%d"), port->m_nPortNr, GetLastError());
					MessageBox(NULL, Temp, _T("COM WriteFile Error"), MB_ICONERROR);
					break;
				}
				case ERROR_INVALID_HANDLE:///�򿪴���ʧ�� erroe code:6
				{
					port->m_hComm = INVALID_HANDLE_VALUE;
					break;
				}
				case ERROR_BAD_COMMAND:///���ӹ����зǷ��Ͽ� erroe code:22
				{
					port->m_hComm = INVALID_HANDLE_VALUE;
					TCHAR Temp[200] = { 0 };
					_stprintf_s(Temp, 200, _T("COM%d ERROR_BAD_COMMAND��WriteFile() Error Code:%d"), port->m_nPortNr, GetLastError());
					MessageBox(NULL, Temp, _T("COM WriteFile Error"), MB_ICONERROR);
					break;
				}
				default:
				{
					// all other error codes
					port->ProcessErrorMessage(_T("WriteFile()"));
				}
				}
			}
			else
			{
				//LeaveCriticalSection(&port->m_csCommunicationSync);
				//write ok
			}
		} // end if(bWrite)

		if (!bWrite)
		{
			bWrite = TRUE;

			bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
				&port->m_ov,		// Overlapped structure
				&BytesSent,		// Stores number of bytes sent
				TRUE); 			// Wait flag

			//LeaveCriticalSection(&port->m_csCommunicationSync);

			// deal with the error code 
			if (!bResult)
			{
				port->ProcessErrorMessage(_T("GetOverlappedResults() in WriteFile()"));
			}
			else
			{
				//write ok
			}
		} // end if (!bWrite)

		//release and remove the data sended ok  add by itas109 2018-01-29
		if (bResult)
		{
			if (sendData)
			{
				delete sendData;
				sendData = NULL;
			}

			i = port->m_bufferList.erase(i);
		}
		else
		{
			i++;
		}

	}

	LeaveCriticalSection(&port->m_csCommunicationSync);

	// Verify that the data size send equals what we tried to send
	//if (BytesSent != SendLen /*strlen((char*)port->m_szWriteBuffer)*/)  // add by 
	//{
	//TRACE(_T("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n"), BytesSent, _tcsclen((TCHAR*)port->m_szWriteBuffer));
	//}
}

//
// Character received. Inform the owner
//
void CSerialPort::ReceiveChar(CSerialPort* port)
{
	BOOL  bRead = TRUE;
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	COMSTAT comstat;
	unsigned char RXBuff;

	for (;;)
	{
		//add by liquanhai 2011-11-06  ��ֹ����
		if (WaitForSingleObject(port->m_hShutdownEvent, 0) == WAIT_OBJECT_0)
			return;

		// Gain ownership of the comm port critical section.
		// This process guarantees no other part of this program 
		// is using the port object. 

		EnterCriticalSection(&port->m_csCommunicationSync);

		// ClearCommError() will update the COMSTAT structure and
		// clear any other errors.
		///����COMSTAT

		bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// start forever loop.  I use this type of loop because I
		// do not know at runtime how many loops this will have to
		// run. My solution is to start a forever loop and to
		// break out of it when I have processed all of the
		// data available.  Be careful with this approach and
		// be sure your loop will exit.
		// My reasons for this are not as clear in this sample 
		// as it is in my production code, but I have found this 
		// solutiion to be the most efficient way to do this.

		///�����ַ������������ж�ѭ��
		if (comstat.cbInQue == 0)
		{
			// break out when all bytes have been read
			break;
		}

		EnterCriticalSection(&port->m_csCommunicationSync);

		if (bRead)
		{
			///���ڶ������������������ֽ�
			bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
				&RXBuff,				// RX Buffer Pointer
				1,					// Read one byte
				&BytesRead,			// Stores number of bytes read
				&port->m_ov);		// pointer to the m_ov structure
			// deal with the error code 
			///�����ش��󣬴�����
			if (!bResult)
			{
				switch (dwError = GetLastError())
				{
				case ERROR_IO_PENDING:
				{
					// asynchronous i/o is still in progress 
					// Proceed on to GetOverlappedResults();
					///�첽IO���ڽ���
					bRead = FALSE;
					break;
				}
				case ERROR_ACCESS_DENIED:///�ܾ����� erroe code:5
				{
					port->m_hComm = INVALID_HANDLE_VALUE;
					TCHAR Temp[200] = { 0 };
					_stprintf_s(Temp, 200, _T("COM%d ERROR_ACCESS_DENIED��ReadFile() Error Code:%d"), port->m_nPortNr, GetLastError());
					MessageBox(NULL, Temp, _T("COM ReadFile Error"), MB_ICONERROR);
					break;
				}
				case ERROR_INVALID_HANDLE:///�򿪴���ʧ�� erroe code:6
				{
					port->m_hComm = INVALID_HANDLE_VALUE;
					break;
				}
				case ERROR_BAD_COMMAND:///���ӹ����зǷ��Ͽ� erroe code:22
				{
					port->m_hComm = INVALID_HANDLE_VALUE;
					TCHAR Temp[200] = { 0 };
					_stprintf_s(Temp, 200, _T("COM%d ERROR_BAD_COMMAND��ReadFile() Error Code:%d"), port->m_nPortNr, GetLastError());
					MessageBox(NULL, Temp, _T("COM ReadFile Error"), MB_ICONERROR);
					break;
				}
				default:
				{
					// Another error has occured.  Process this error.
					port->ProcessErrorMessage(_T("ReadFile()"));
					break;
					//return;///��ֹ��д����ʱ�����ڷ������Ͽ�������ѭ��һֱִ�С�add by itas109 2014-01-09 ������liquanhai��ӷ������Ĵ�����
				}
				}
			}
			else///ReadFile����TRUE
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
			}
		}  // close if (bRead)

		///�첽IO�������ڽ��У���Ҫ����GetOverlappedResult��ѯ
		if (!bRead)
		{
			bRead = TRUE;
			bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
				&port->m_ov,		// Overlapped structure
				&BytesRead,		// Stores number of bytes read
				TRUE); 			// Wait flag

			// deal with the error code 
			if (!bResult)
			{
				port->ProcessErrorMessage(_T("GetOverlappedResults() in ReadFile()"));
			}
		}  // close if (!bRead)

		LeaveCriticalSection(&port->m_csCommunicationSync);

#ifdef _SEND_DATA_WITH_SIGSLOT
		
#else
		// notify parent that a byte was received
		//�����̻߳���ȴ�������������ʹ��PostMessage()����SendMessage()
		PostMessage(port->m_pOwner, WM_COMM_RXCHAR, (WPARAM)RXBuff, (LPARAM)port->m_nPortNr);
		//::SendMessage((port->m_pOwner), Wm_SerialPort_RXCHAR, (WPARAM) RXBuff, (LPARAM) port->m_nPortNr);
#endif

	} // end forever loop

}

//
// str received. Inform the owner
//
void CSerialPort::ReceiveStr(CSerialPort* port)
{
	BOOL  bRead = TRUE;
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	COMSTAT comstat;

	for (;;)
	{
		//add by liquanhai 2011-11-06  ��ֹ����
		if (WaitForSingleObject(port->m_hShutdownEvent, 0) == WAIT_OBJECT_0)
			return;

		// Gain ownership of the comm port critical section.
		// This process guarantees no other part of this program 
		// is using the port object. 

		EnterCriticalSection(&port->m_csCommunicationSync);

		// ClearCommError() will update the COMSTAT structure and
		// clear any other errors.
		///����COMSTAT

		bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// start forever loop.  I use this type of loop because I
		// do not know at runtime how many loops this will have to
		// run. My solution is to start a forever loop and to
		// break out of it when I have processed all of the
		// data available.  Be careful with this approach and
		// be sure your loop will exit.
		// My reasons for this are not as clear in this sample 
		// as it is in my production code, but I have found this 
		// solutiion to be the most efficient way to do this.

		///�����ַ������������ж�ѭ��
		//0xcccccccc��ʾ�����쳣�ˣ��ᵼ��RXBuffָ���ʼ������
		if (comstat.cbInQue == 0 || comstat.cbInQue == 0xcccccccc)
		{
			// break out when all bytes have been read
			break;
		}

		//�������'\0'����ô���ݻᱻ�ضϣ�ʵ������ȫ����ȡֻ��û����ʾ��ȫ�����ʱ��ʹ��memcpy����ȫ����ȡ
		unsigned char* RXBuff = new unsigned char[comstat.cbInQue + 1];
		if (RXBuff == NULL)
		{
			return;
		}
		RXBuff[comstat.cbInQue] = '\0';//�����ַ���������

		EnterCriticalSection(&port->m_csCommunicationSync);

		if (bRead)
		{
			///���ڶ������������������ֽ�
			bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
				RXBuff,				// RX Buffer Pointer
				comstat.cbInQue,					// Read cbInQue len byte
				&BytesRead,			// Stores number of bytes read
				&port->m_ov);		// pointer to the m_ov structure
			// deal with the error code 
			///�����ش��󣬴�����
			if (!bResult)
			{
				switch (dwError = GetLastError())
				{
				case ERROR_IO_PENDING:
				{
					// asynchronous i/o is still in progress 
					// Proceed on to GetOverlappedResults();
					///�첽IO���ڽ���
					bRead = FALSE;
					break;
				}
				case ERROR_ACCESS_DENIED:///�ܾ����� erroe code:5
				{
					port->m_hComm = INVALID_HANDLE_VALUE;
					TCHAR Temp[200] = { 0 };
					_stprintf_s(Temp, 200, _T("COM%d ERROR_ACCESS_DENIED��ReadFile() Error Code:%d"), port->m_nPortNr, GetLastError());
					MessageBox(NULL, Temp, _T("COM ReadFile Error"), MB_ICONERROR);
					break;
				}
				case ERROR_INVALID_HANDLE:///�򿪴���ʧ�� erroe code:6
				{
					port->m_hComm = INVALID_HANDLE_VALUE;
					break;
				}
				case ERROR_BAD_COMMAND:///���ӹ����зǷ��Ͽ� erroe code:22
				{
					port->m_hComm = INVALID_HANDLE_VALUE;
					TCHAR Temp[200] = { 0 };
					_stprintf_s(Temp, 200, _T("COM%d ERROR_BAD_COMMAND��ReadFile() Error Code:%d"), port->m_nPortNr, GetLastError());
					MessageBox(NULL, Temp, _T("COM ReadFile Error"), MB_ICONERROR);
					break;
				}
				default:
				{
					// Another error has occured.  Process this error.
					port->ProcessErrorMessage(_T("ReadFile()"));
					break;
					//return;///��ֹ��д����ʱ�����ڷ������Ͽ�������ѭ��һֱִ�С�add by itas109 2014-01-09 ������liquanhai��ӷ������Ĵ�����
				}
				}
			}
			else///ReadFile����TRUE
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
			}
		}  // close if (bRead)

		///�첽IO�������ڽ��У���Ҫ����GetOverlappedResult��ѯ
		if (!bRead)
		{
			bRead = TRUE;
			bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
				&port->m_ov,		// Overlapped structure
				&BytesRead,		// Stores number of bytes read
				TRUE); 			// Wait flag

			// deal with the error code 
			if (!bResult)
			{
				port->ProcessErrorMessage(_T("GetOverlappedResults() in ReadFile()"));
			}
		}  // close if (!bRead)

		LeaveCriticalSection(&port->m_csCommunicationSync);

#ifdef _SEND_DATA_WITH_SIGSLOT
		sendMessageSignal(RXBuff, port->m_nPortNr, BytesRead);
#else
		serialPortInfo commInfo;
		commInfo.portNr = port->m_nPortNr;
		commInfo.bytesRead = BytesRead;
		// notify parent that some byte was received
		::SendMessage((port->m_pOwner), WM_COMM_RXSTR, (WPARAM)RXBuff, (LPARAM)&commInfo);
#endif


		//�ͷ�
		delete[] RXBuff;
		RXBuff = NULL;

	} // end forever loop

}

//
// Return the device control block
//
DCB CSerialPort::GetDCB()
{
	return m_dcb;
}

//
// Return the communication event masks
//
DWORD CSerialPort::GetCommEvents()
{
	return m_dwCommEvents;
}

//
// Return the output buffer size
//
DWORD CSerialPort::GetWriteBufferSize()
{
	return m_nWriteBufferSize;
}

BOOL CSerialPort::IsOpened()
{
	return m_hComm != INVALID_HANDLE_VALUE;//m_hComm����INVALID_HANDLE_VALUE����� add by itas109 2016-07-29
}

void CSerialPort::ClosePort()
{
	MSG message;

	//�����̹߳����жϣ���������̹߳����´��ڹر����������� add by itas109 2016-06-29
	if (IsThreadSuspend(m_Thread))
	{
		ResumeThread(m_Thread);
	}

	//���ھ����Ч  add by itas109 2016-07-29
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		return;
	}

	do
	{
		SetEvent(m_hShutdownEvent);
		//add by liquanhai  ��ֹ����  2011-11-06
		if (::PeekMessage(&message, m_pOwner, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	} while (m_bThreadAlive);

	//�˴�����ʱ����Ҫ����Ϊ������ڿ��ţ����͹ر�ָ����׹ر���Ҫһ����ʱ�䣬�����ʱӦ�ø����Ե��������
	Sleep(50);//add by itas109 2016-08-02

	// if the port is still opened: close it 
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}

	// Reset Handles  
	if (m_hShutdownEvent != NULL)
	{
		ResetEvent(m_hShutdownEvent);
	}

	if (m_ov.hEvent != NULL)
	{
		ResetEvent(m_ov.hEvent);
	}

	if (m_hWriteEvent != NULL)
	{
		ResetEvent(m_hWriteEvent);
	}

	if (m_szWriteBuffer != NULL)
	{
		delete[] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
	}
}


void CSerialPort::WriteToPort(char* string, size_t n)
{
	assert(m_hComm != INVALID_HANDLE_VALUE);
	PBYTE m_bufferTemp = NULL;
	m_bufferTemp = new BYTE[n];

	//memset(m_szWriteBuffer, 0, n);
	//memcpy(m_szWriteBuffer, string, n);
	memset(m_bufferTemp, 0, n);
	memcpy(m_bufferTemp, string, n);
	m_nWriteSize = n;

	//use list is duing to that the send data may same. but map's key can not same
	m_bufferStruct.buffer = m_bufferTemp;
	m_bufferStruct.len = n;
	m_bufferList.push_back(m_bufferStruct);

	// set event for write
	SetEvent(m_hWriteEvent);
}

void CSerialPort::WriteToPort(BYTE* Buffer, size_t n)
{
	assert(m_hComm != INVALID_HANDLE_VALUE);
	PBYTE m_bufferTemp = NULL;
	m_bufferTemp = new BYTE[n];

	//memset(m_szWriteBuffer, 0, n);
	//memcpy(m_szWriteBuffer, string, n);
	memset(m_bufferTemp, 0, n);
	memcpy(m_bufferTemp, Buffer, n);
	m_nWriteSize = n;

	m_bufferStruct.buffer = m_bufferTemp;
	m_bufferStruct.len = n;
	m_bufferList.push_back(m_bufferStruct);

	// set event for write
	SetEvent(m_hWriteEvent);
}

std::string CSerialPort::GetVersion()
{
	std::string m_version = "CSerialPort 3.0.3.180621";
	return m_version;
}

CSerialPortInfo::CSerialPortInfo()
{

}

CSerialPortInfo::~CSerialPortInfo()
{

}

std::list<std::string> CSerialPortInfo::availablePorts()
{
	std::list<std::string> portsList;
	///����XP/Win7ϵͳ��ע���λ�ã�����ϵͳ����ʵ��������޸�
	std::string m_regKeyPath = std::string("HARDWARE\\DEVICEMAP\\SERIALCOMM");
	getRegKeyValues(m_regKeyPath, portsList);
	return portsList;
}

// �콡�����2020.05.16
std::list<std::wstring> CSerialPortInfo::availablePortsW()
{
    std::list<std::wstring> portsList;
    ///����XP/Win7ϵͳ��ע���λ�ã�����ϵͳ����ʵ��������޸�
    wchar_t subkey[] = L"HARDWARE\\DEVICEMAP\\SERIALCOMM";
    getRegKeyValues(subkey, portsList);
    return portsList;
}
