
// ModbusRTUToolDlg.cpp: 实现文件
//

#include "../pch.h"
#include "../framework.h"
#include "../ModbusRTUToolApp.h"
#include "ModbusRTUToolDlg.h"
#include "afxdialogex.h"
#include "../view/RtuView.h"
#include "../view/LoggerView.h"
#include "../view/RegisterView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// ModbusRTUToolDlg 对话框



ModbusRTUToolDlg::ModbusRTUToolDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MODBUSRTUTOOL_DIALOG, pParent)
    , serialPortHandler(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ModbusRTUToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ModbusRTUToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_MESSAGE(WM_MODBUS_RX_FRAME, ModbusRTUToolDlg::OnModbusRxFrame)
    ON_MESSAGE(WM_MODBUS_TX_FRAME, ModbusRTUToolDlg::OnModbusTxFrame)
    ON_MESSAGE(WM_SET_REGISTER, ModbusRTUToolDlg::OnSetRegister)

END_MESSAGE_MAP()


// ModbusRTUToolDlg 消息处理程序

BOOL ModbusRTUToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

    // 让RegisterView和LogView中的控件布满窗体
    CRect rect;
    GetWindowRect(&rect);
    MoveWindow(0, 0, rect.Width()-1, rect.Height()-1);

    serialPortHandler.Create(AfxRegisterWndClass(0), L"", WS_CHILD, CRect(0, 0, 2, 2), this, 0, nullptr);

    SetTimer(0, 1000, nullptr);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void ModbusRTUToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void ModbusRTUToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR ModbusRTUToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL ModbusRTUToolDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN) {
        if (pMsg->wParam == VK_ESCAPE) {
            return TRUE;
        }
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


int ModbusRTUToolDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialogEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  在此添加您专用的创建代码
    CString strMyClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
        ::LoadCursor(NULL, IDC_ARROW), (HBRUSH) ::GetStockObject(WHITE_BRUSH),
        ::LoadIcon(NULL, IDI_APPLICATION));

    m_pFrameWnd = new CFrameWnd{};
    m_pFrameWnd->Create(strMyClass, _T(""), WS_CHILD, CRect(0, 0, 300, 300), this);
    m_pFrameWnd->ShowWindow(SW_SHOW);


    splitterWnd.CreateStatic(m_pFrameWnd, 1, 3); //在Frame里切分视图窗口为1×2，就是一行两列
    splitterWnd.CreateView(0, 0, RUNTIME_CLASS(RtuView), CSize(280, 100), NULL);//第一行一列
    splitterWnd.CreateView(0, 1, RUNTIME_CLASS(RegisterView), CSize(300, 100), NULL);//第一行二列
    splitterWnd.CreateView(0, 2, RUNTIME_CLASS(LoggerView), CSize(200, 100), NULL);//第一行二列

    return 0;
}

void ModbusRTUToolDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    if (m_pFrameWnd) delete m_pFrameWnd;
}


void ModbusRTUToolDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    CRect rect;
    GetClientRect(&rect);
    m_pFrameWnd->MoveWindow(&rect);
    m_pFrameWnd->ShowWindow(SW_SHOW);
    //splitterWnd.MoveWindow(&rect);
}

int ModbusRTUToolDlg::openPort()
{
    RtuView* rtuView = (RtuView*)splitterWnd.GetPane(0, 0);
    if (rtuView == nullptr) {
        return 1;
    }


    if (rtuView->checkRead()) {
        return 2;
    }

    serialPortHandler.setDeviceAddress(rtuView->getDeviceAddress());
    serialPortHandler.setReadBeginAddress(rtuView->getBeginAddress());
    serialPortHandler.setReadEndAddress(rtuView->getEndAddress());
    serialPortHandler.setReadPeriod(rtuView->getReadPeriod());

    int port = rtuView->getPort();
    if (port == 0) {
        return 3;
    }

    if (serialPortHandler.openPort(rtuView->getPort(), rtuView->getBaud(), rtuView->getDataBits(), rtuView->getStopBits(), rtuView->getParity())) {
        return 4;
    }

    LoggerView* loggerView = (LoggerView*)splitterWnd.GetPane(0, 2);
    if (loggerView == nullptr) {
        return 5;
    }
    loggerView->info(L"打开串口");
    
    return 0;
}

int ModbusRTUToolDlg::closePort()
{
    serialPortHandler.closePort();
    LoggerView* loggerView = (LoggerView*)splitterWnd.GetPane(0, 2);
    if (loggerView != nullptr) {
        loggerView->info(L"关闭串口");
    }
    return 0;
}

int ModbusRTUToolDlg::setRegister(int registerAddress, int registerValue)
{
    RtuView* rtuView = (RtuView*)splitterWnd.GetPane(0, 0);
    if (rtuView == nullptr) {
        return 2;
    }

    if (serialPortHandler.send(rtuView->getWriteFunctionCode(), registerAddress, registerValue)) {
        MessageBox(L"发送失败");
    }
    return 0;
}

void ModbusRTUToolDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    RtuView* rtuView = (RtuView*)splitterWnd.GetPane(0, 0);
    if (rtuView == nullptr) {
        return;
    }

    if (rtuView->checkRead()) {
        return;
    }

    serialPortHandler.setDeviceAddress(rtuView->getDeviceAddress());
    serialPortHandler.setReadBeginAddress(rtuView->getBeginAddress());
    serialPortHandler.setReadEndAddress(rtuView->getEndAddress());
    serialPortHandler.setReadPeriod(rtuView->getReadPeriod());

    CDialogEx::OnTimer(nIDEvent);
}

afx_msg LRESULT ModbusRTUToolDlg::OnModbusRxFrame(WPARAM dataBuff, LPARAM dataLength)
{
    uint8_t* data = (uint8_t*)dataBuff;
    int length = (int)dataLength;
    if (data == nullptr) return 1;
    if (length <= 0) return 2;

    CString str;
    CString strTemp;
    for (int i = 0; i < length; ++i) {
        strTemp.Format(L"%02X ", data[i]);
        str += strTemp;
    }

    LoggerView* loggerView = (LoggerView*)splitterWnd.GetPane(0, 2);
    if (loggerView == nullptr) {
    }
    loggerView->info(L"收到  " + str);

    if (data[1] != 0x03) {
        return 4;
    }

    RegisterView* registerView = (RegisterView*)splitterWnd.GetPane(0, 1);
    if (registerView == nullptr) {
        return 5;
    }
    
    if (registerView->show(serialPortHandler.getReadBeginAddress(), serialPortHandler.getReadEndAddress(), data, length)) {
        
        loggerView->info(L"显示数据错误");
    }
    return 0;
}

afx_msg LRESULT ModbusRTUToolDlg::OnModbusTxFrame(WPARAM dataBuff, LPARAM dataLength)
{
    uint8_t* data = (uint8_t*)dataBuff;
    int length = (int)dataLength;
    if (data == nullptr) return 1;
    if (length <= 0) return 2;

    CString str;
    CString strTemp;
    for (int i = 0; i < length; ++i) {
        strTemp.Format(L"%02X ", data[i]);
        str += strTemp;
    }

    LoggerView* loggerView = (LoggerView*)splitterWnd.GetPane(0, 2);
    if (loggerView == nullptr) {
        return 3;
    }
    loggerView->info(L"发送  " + str);

    return 0;
}

afx_msg LRESULT ModbusRTUToolDlg::OnSetRegister(WPARAM registerAddress, LPARAM registerValue)
{
    int address = (int)registerAddress;
    int value = (int)registerValue;

    LoggerView* loggerView = (LoggerView*)splitterWnd.GetPane(0, 1);
    if (loggerView == nullptr) {
        return 1; 
    }
    loggerView->info(L"设置寄存器");

    RtuView* rtuView = (RtuView*)splitterWnd.GetPane(0, 0);
    if (rtuView == nullptr) {
        return 1;
    }

    loggerView->info(L"下面发送数据");
    serialPortHandler.send(rtuView->getWriteFunctionCode(), address, value);

    return 0;
}

