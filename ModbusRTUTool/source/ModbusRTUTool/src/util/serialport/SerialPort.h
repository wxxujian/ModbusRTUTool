/*
**	FILENAME			CSerialPort.h
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
**
************************************************************************************
**  author: mrlong date:2007-12-25
**
**  �Ľ�
**	1) ���� ClosePort
**	2) ���� WriteToPort ��������
**	3) ���� SendData �� RecvData ����
**************************************************************************************
***************************************************************************************
**  author��liquanhai date:2011-11-06
**
**  �Ľ�
**	1) ���� ClosePort �н�������Ȩ����ֹ��������
**	2) ���� ReceiveChar �з�ֹ�߳�����
**************************************************************************************
***************************************************************************************
**  author��viruscamp date:2013-12-04
**
**  �Ľ�
**	1) ���� IsOpen �ж��Ƿ��
**	2) ���� InitPort �� parity Odd Even ����ȡֵ����
**	3) �޸� InitPort �� portnr ȡֵ��Χ��portnr>9 ʱ���⴦��
**	4) ȡ���� MFC ��������ʹ�� HWND ��� CWnd��ʹ�� win32 thread ���������� MFC ��
**	5) �����û���Ϣ����Զ��壬�������� CnComm
***************************************************************************************
***************************************************************************************
**  author: itas109  date:2014-01-10
**  Blog��blog.csdn.net/itas109
**
**  �Ľ�
**    1) ���COM10���϶˿��޷���ʾ������
**    2) ��չ��ѡ��˿ڣ����ֵMaxSerialPortNum�����Զ���
**    3) ���QueryKey()��Hkey2ComboBox���������������Զ���ѯ��ǰ��Ч�Ĵ��ںš�
***************************************************************************************
**  author: itas109  date:2016-05-06
**  Blog��blog.csdn.net/itas109
**
**  �Ľ�
**    1) �޸�ÿ�δ򿪴��ڷ���һ�Σ���������Ӧ��ʱ����Ҫ�ر��ٴ򿪻��߽��������ݲ��ܷ��͵����⡣
**		 ����취����m_hEventArray�е���m_hWriteEvent�����ȼ����ڶ������ȼ���CommThread(LPVOID pParam)�����ж�д��λ��Ҳ������
**		 �ο���http://zhidao.baidu.com/link?url=RSrbPcfTZRULFFd2ziHZPBwnoXv1iCSu_Nmycb_yEw1mklT8gkoNZAkWpl3UDhk8L35DtRPo5VV5kEGpOx-Gea
**    2) �޸�ֹͣλ��ͷ�ļ��ж����1����SetCommState��������⣬ӦΪ1��Ӧ��ֹͣλ��1.5��UINT stopsbits = ONESTOPBIT
**    3) switch(stopbits)��switch(parity)����Ĭ���������ǿ����׳��
** ***************************************************************************************
**  author: itas109  date:2016-06-22
**  Blog��blog.csdn.net/itas109
**
**  �Ľ�
**  1�� ����ReceiveStr���������ڽ����ַ��������ջ������ж����ַ��ͽ��ն����ַ�����
**      ���ReceiveCharֻ�ܽ��յ����ַ������⡣
** ***************************************************************************************
**  author: itas109  date:2016-06-29
**  Blog��blog.csdn.net/itas109
**
**  �Ľ�
**  1�� ���RestartMonitoring������StopMonitoring�������׼ȷ��������壬����ʵ�����á�
**		��RestartMonitoring����ΪResumeMonitoring����StopMonitoring����ΪSuspendMonitoring��
**	2�� ����IsThreadSuspend�����������ж��߳��Ƿ����
**	3�� �Ľ�ClosePort�����������̹߳����жϣ���������̹߳����´��ڹر����������⡣
**  4�� ����IsReceiveString�궨�壬���ڽ���ʱ���õ��ֽڽ��ջ��Ƕ��ֽڽ���
** ***************************************************************************************
**  author: itas109  date:2016-08-02
**  Blog��blog.csdn.net/itas109
**  �Ľ�
**  1�� �Ľ�IsOpen������m_hComm����INVALID_HANDLE_VALUE���������ΪCreateFile����ʧ�ܷ��ص���INVALID_HANDLE_VALUE������NULL
**  2�� �Ľ�ClosePort���������Ӵ��ھ����Ч���ж�(��ֹ�ر�����)��m_hWriteEvent��ʹ��CloseHandle�ر�
**  3�� �Ľ�CommThread��ReceiveChar��ReceiveStr��WriteChar�������쳣������жϣ����������жϣ����ڴ�ʧ��(error code:ERROR_INVALID_HANDLE)�����ӹ����зǷ��Ͽ�(error code:ERROR_BAD_COMMAND)�;ܾ�����(error code:ERROR_ACCESS_DENIED)
**  4�� ���ð�ȫ����sprintf_s��strcpy_s�����滻��sprintf��strcpy
**  5�� �Ľ�QueryKey���������ڲ�ѯע���Ŀ��ô���ֵ����������������Ŀ��ô���
**  6�� �Ľ�InitPort���������ڴ�ʧ�ܣ�������ʾ��Ϣ:���ڲ�����(error code:ERROR_FILE_NOT_FOUND)�ʹ��ھܾ�����(error code:ERROR_ACCESS_DENIED)
**  7�� ����viruscamp ȡ���� MFC ������
**  8�� �Ľ�InitPort����������ϴδ����Ǵ򿪣��ٴε���InitPort�������رմ�����Ҫ��һ������ʱ�������м��ʵ���ERROR_ACCESS_DENIED�ܾ����ʣ�Ҳ���Ǵ���ռ������
**  9�� ��ʼ��Ĭ�ϲ������޸�Ϊ9600
**  10���޸�һЩ�ͷŵ�BUG
**  11���淶��һЩ������Ϣ���ο�winerror.h --  error code definitions for the Win32 API functions
** ***************************************************************************************
**  author: itas109  date:2016-08-10
**  Blog��blog.csdn.net/itas109
**  �Ľ�
**  1�� �Ľ�ReceiveStr������comstat.cbInQue = 0xcccccccc��������紮���쳣�Ͽ������ᵼ��RXBuff��ʼ��ʧ��
** ***************************************************************************************
**  author: itas109  date:2017-02-14
**  Blog��blog.csdn.net/itas109
**  �Ľ�
**  1)  ����ASCII��UNICODE����
**  2)  ReceiveStr�����з��ͺ���SendMessage�ĵڶ����������ýṹ����ʽ������portNr���ںź�bytesRead��ȡ���ֽ��������Դ���16���Ƶ�ʱ��0x00�ض�����
**  3)  ���򲻱�Ҫ�ĺ���SendData��RecvData
**  4)  ������ȡ���� MFC ��������Hkey2ComboBox������ʱ����
**  5)  ����С�����޸�
** ***************************************************************************************
**  author: itas109  date:2017-03-12
**  Blog��blog.csdn.net/itas109
**  �Ľ�
**  1)  ���Ӻ궨��_AFX�����ڴ���MFC�ı�Ҫ����Hkey2ComboBox
**  2)  ��һ��ȥ��MFC�������޸�AfxMessageBox����
** ***************************************************************************************
**  author: itas109  date:2017-12-16
**  Blog��blog.csdn.net/itas109
**  �Ľ�
**	1)	֧��DLL���
**  2)  ȥ��QueryKey��Hkey2ComboBox������CSerialPortInfo::availablePorts()��������
**  3)  ����CSerialPortInfo�࣬Ŀǰֻ��availablePorts��̬���������ڻ�ȡ��Ծ�Ĵ��ڵ�list
**  4)  ���������ռ�itas109
**  5)  ���򲻱�Ҫ��ͷ�ļ�
**  6)	InitPort��~CSerialPort()��ֱ������ClosePort()
** ***************************************************************************************
**  author: itas109  date:2018-02-14
**  Blog��blog.csdn.net/itas109
**  �Ľ�
**	1)	���޸������������͵����� �� fix can not continue send error
**  2)  ��һ����д�뾡���ܶ�����ݵ����� �� try best to send mutil data once in WriteChar funtion
**  3)  �޸�BYTE�ڴ����õ����� fix BYTE memset error
**  4)  �ڹ��캯���г�ʼ�����ͷ��ٽ��� initialize and delete critical section in Constructor
**  5)  �������
** ***************************************************************************************
**  author: itas109  date:2018-06-15
**  Blog��blog.csdn.net/itas109
**  �Ľ�
**	1)	�޸�availablePorts��������ö�����д������� fix function availablePorts can not enum all port error
** ***************************************************************************************
**  author: itas109  date:2018-06-21
**  Blog��blog.csdn.net/itas109
**  �Ľ�
**  1)  �����ź���ۻ��ƴ������� add sigslot.h to send data
**  2)  �޸�handle��ʼ�����⣬ȫ����ʼ��ΪINVALID_HANDLE_VALUE modify handle init INVALID_HANDLE_VALUE
**  3)  �޸����ڴ�ʧ��,�ٴγɹ��򿪴��ں�,����������д���� fix can not read and write when the port open failed
*/

#ifndef __CSERIALPORT_H__
#define __CSERIALPORT_H__

#include "stdio.h"
#include "tchar.h"

#include "Windows.h"

#include <string>
#include <list>
#include <vector>


#ifndef WM_COMM_MSG_BASE 
#define WM_COMM_MSG_BASE		    WM_USER + 109		//!< ��Ϣ��ŵĻ���  
#endif

#define WM_COMM_BREAK_DETECTED		WM_COMM_MSG_BASE + 1	// A break was detected on input.
#define WM_COMM_CTS_DETECTED		WM_COMM_MSG_BASE + 2	// The CTS (clear-to-send) signal changed state. 
#define WM_COMM_DSR_DETECTED		WM_COMM_MSG_BASE + 3	// The DSR (data-set-ready) signal changed state. 
#define WM_COMM_ERR_DETECTED		WM_COMM_MSG_BASE + 4	// A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY. 
#define WM_COMM_RING_DETECTED		WM_COMM_MSG_BASE + 5	// A ring indicator was detected. 
#define WM_COMM_RLSD_DETECTED		WM_COMM_MSG_BASE + 6	// The RLSD (receive-line-signal-detect) signal changed state. 
#define WM_COMM_RXCHAR				WM_COMM_MSG_BASE + 7	// A character was received and placed in the input buffer. 
#define WM_COMM_RXFLAG_DETECTED		WM_COMM_MSG_BASE + 8	// The event character was received and placed in the input buffer.  
#define WM_COMM_TXEMPTY_DETECTED	WM_COMM_MSG_BASE + 9	// The last character in the output buffer was sent.  
#define WM_COMM_RXSTR               WM_COMM_MSG_BASE + 10   // Receive string

#define MaxSerialPortNum 200    ///��Ч�Ĵ����ܸ��������Ǵ��ڵĺ� //add by itas109 2014-01-09
#define IsReceiveString  0      //���ú��ַ�ʽ���գ�ReceiveString 1���ַ������գ���Ӧ��Ӧ����ΪWm_SerialPort_RXSTR����ReceiveString 0һ���ַ�һ���ַ����գ���Ӧ��Ӧ����ΪWm_SerialPort_RXCHAR��
//#define _SEND_DATA_WITH_SIGSLOT //ʹ��sigslot������յ�����

namespace itas109 {

#ifdef _SEND_DATA_WITH_SIGSLOT
	#include "sigslot.h"
#else
	//will remove on V4.0
	struct serialPortInfo
	{
		UINT portNr;//���ں�
		DWORD bytesRead;//��ȡ���ֽ���
	};
#endif

    struct serialPortBuffer
    {
        int len;
        PBYTE buffer;
    };

    struct SerialPortInfo {
        std::string portName;
        std::string description;
    };

	class _declspec(dllexport) CSerialPort
	{
	public:
		// contruction and destruction
		CSerialPort();
		virtual ~CSerialPort();

		// port initialisation		
		// UINT stopsbits = ONESTOPBIT   stop is index 0 = 1 1=1.5 2=2 
		// �мǣ�stopsbits = 1������ֹͣλΪ1��
		// by itas109 20160506
		BOOL		InitPort(HWND pPortOwner, UINT portnr = 1, UINT baud = 9600,
			TCHAR parity = _T('N'), UINT databits = 8, UINT stopsbits = ONESTOPBIT,
			DWORD dwCommEvents = EV_RXCHAR | EV_CTS, UINT nBufferSize = 512,

			DWORD ReadIntervalTimeout = 1000,
			DWORD ReadTotalTimeoutMultiplier = 1000,
			DWORD ReadTotalTimeoutConstant = 1000,
			DWORD WriteTotalTimeoutMultiplier = 1000,
			DWORD WriteTotalTimeoutConstant = 1000);

		// start/stop comm watching
		///���ƴ��ڼ����߳�
		BOOL		 StartMonitoring();//��ʼ����
		BOOL		 ResumeMonitoring();//�ָ�����
		BOOL		 SuspendMonitoring();//�������
		BOOL         IsThreadSuspend(HANDLE hThread);//�ж��߳��Ƿ���� //add by itas109 2016-06-29

		DWORD		 GetWriteBufferSize();///��ȡд�����С
		DWORD		 GetCommEvents();///��ȡ�¼�
		DCB			 GetDCB();///��ȡDCB

		///д���ݵ�����
		void		WriteToPort(char* string, size_t n); // add by mrlong 2007-12-25
		void		WriteToPort(PBYTE Buffer, size_t n);// add by mrlong
		void		ClosePort();					 // add by mrlong 2007-12-2  
		BOOL		IsOpened();

#ifdef _SEND_DATA_WITH_SIGSLOT
		//sigslot
		static signal3<unsigned char*, int, int> sendMessageSignal;
#endif
		
		std::string GetVersion();

	protected:
		// protected memberfunctions
		void		ProcessErrorMessage(TCHAR* ErrorText);///������
		static DWORD WINAPI CommThread(LPVOID pParam);///�̺߳���
		static void	ReceiveChar(CSerialPort* port);
		static void ReceiveStr(CSerialPort* port); //add by itas109 2016-06-22
		static void	WriteChar(CSerialPort* port);

	private:
		// thread
		HANDLE			    m_Thread;
		BOOL                m_bIsSuspened;///thread�����߳��Ƿ����

		// synchronisation objects
		CRITICAL_SECTION	m_csCommunicationSync;///�ٽ���Դ
		BOOL				m_bThreadAlive;///�����߳����б�־

		// handles
		HANDLE				m_hShutdownEvent;  //stop�������¼�
		HANDLE				m_hComm;		   // ���ھ�� 
		HANDLE				m_hWriteEvent;	 // write

		// Event array. 
		// One element is used for each event. There are two event handles for each port.
		// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
		// There is a general shutdown when the port is closed. 
		///�¼����飬����һ��д�¼��������¼����ر��¼�
		///һ��Ԫ������һ���¼����������¼��̴߳���˿ڡ�
		///д�¼��ͽ����ַ��¼�λ��overlapped�ṹ�壨m_ov.hEvent����
		///���˿ڹر�ʱ����һ��ͨ�õĹرա�
		HANDLE				m_hEventArray[3];

		// structures
		OVERLAPPED			m_ov;///�첽I/O
		COMMTIMEOUTS		m_SerialPortTimeouts;///��ʱ����
		DCB					m_dcb;///�豸���ƿ�

		// owner window
		HWND				m_pOwner;

		// misc
		UINT				m_nPortNr;		///���ں�
		PBYTE				m_szWriteBuffer;///д������
		std::list<serialPortBuffer> m_bufferList;
		serialPortBuffer m_bufferStruct;
		DWORD				m_dwCommEvents;
		DWORD				m_nWriteBufferSize;///д�����С

		size_t				m_nWriteSize;//д���ֽ��� //add by mrlong 2007-12-25
	};

	class _declspec(dllexport) CSerialPortInfo
	{
	public:
		CSerialPortInfo();
		~CSerialPortInfo();

		static std::list<std::string> availablePorts();
        static std::vector<SerialPortInfo> availablePortInfos();
	};
};

#endif __CSERIALPORT_H__
