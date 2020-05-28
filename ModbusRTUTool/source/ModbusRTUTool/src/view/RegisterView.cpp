// RegisterView.cpp: 实现文件
//

#include "../pch.h"
#include "../controller/ModbusRTUToolDlg.h"
#include "../../res/resource.h"
#include "RegisterView.h"


// RegisterView

IMPLEMENT_DYNCREATE(RegisterView, CFormView)

RegisterView::RegisterView()
	: CFormView(IDD_VIEW_REGISTER)
{

}

RegisterView::~RegisterView()
{
}

void RegisterView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, listCtrl);
    DDX_Control(pDX, IDC_EDIT_INPUT, editInput);
}

BEGIN_MESSAGE_MAP(RegisterView, CFormView)
    ON_WM_SIZE()
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &RegisterView::OnNMDblclkList1)
    ON_EN_KILLFOCUS(IDC_EDIT_INPUT, &RegisterView::OnEnKillfocusEditInput)
END_MESSAGE_MAP()


// RegisterView 诊断

#ifdef _DEBUG
void RegisterView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void RegisterView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// RegisterView 消息处理程序

void RegisterView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    // TODO: 在此添加专用代码和/或调用基类

    DWORD style = GetWindowLong(listCtrl.GetSafeHwnd(), GWL_STYLE);
    style &= ~LVS_TYPEMASK;
    style |= LVS_REPORT;
    SetWindowLong(listCtrl.GetSafeHwnd(), GWL_STYLE, style);

    DWORD exStyle = listCtrl.GetExtendedStyle();
    //exStyle |= LVS_EX_FULLROWSELECT;
    exStyle |= LVS_EX_GRIDLINES;                     // 网格线（只适用与报表风格的listctrl）
    listCtrl.SetExtendedStyle(exStyle);                       // 设置扩展风格

    //listCtrl.SetBkColor(RGB(200, 200, 200));                  // 设置背景颜色
    //listCtrl.SetTextBkColor(RGB(200, 200, 200));              // 设置文本背景颜色
    //listCtrl.SetTextColor(RGB(10, 10, 80));                   // 设置文本颜色


    // 设置CListCtrl的行高没有函数接口，可以通过自绘来实现，但是比较麻烦。
    // 有一个比较简单的方法是通过使用一个空白的图像将行撑起来，使其高度发生变化
    // 可以调节CReate函数的第二个参数进行调节
    CImageList m_image;
    m_image.Create(1, 36, ILC_COLOR32, 1, 0);
    listCtrl.SetImageList(&m_image, LVSIL_SMALL);

    // 第0列默认不能居中，要想居中，方法是先正常添加各个列，然后删掉第0列，让第1列成为第0列，如此并不影响其他操作
    listCtrl.InsertColumn(0, L"地址", LVCFMT_CENTER, 80);
    listCtrl.InsertColumn(1, L"地址", LVCFMT_CENTER, 80);
    listCtrl.InsertColumn(2, L"读数", LVCFMT_CENTER, 80);
    listCtrl.InsertColumn(3, L"写数", LVCFMT_CENTER, 80);
    listCtrl.DeleteColumn(0);                               

    //editInput.Create(WS_VISIBLE | WS_CHILD | ES_CENTER | BS_VCENTER | WS_BORDER, CRect(20, 50, 200, 200), this, 1);
    //editInput.SetParent(this);
    ////editInput.SetWindowPos()
    ////editInput.SetWindowTextW(L"这是要用的edit");
    //editInput.ShowWindow(SW_HIDE);
}


void RegisterView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (listCtrl.GetSafeHwnd())
        listCtrl.MoveWindow(0, 0, cx, cy);
}

int RegisterView::show(int startAddress, int endAddress, uint8_t* data, int dataLength)
{
    if ((endAddress - startAddress + 1) != (data[2] >> 1)) {
        return 1;
    }

    bool isDeleteAllItems = false;
    if (this->startAddress != startAddress) {
        this->startAddress = startAddress;
        isDeleteAllItems = true;
    }
    if (this->endAddress != endAddress) {
        this->endAddress = endAddress;
        isDeleteAllItems = true;
    }

    CString str;
    if (isDeleteAllItems) {
        listCtrl.DeleteAllItems();
        for (int i = startAddress, j = 3, k = 0; i <= endAddress; ++i, j += 2, ++k) {
            str.Format(L"%d", i);
            listCtrl.InsertItem(k, str);

            str.Format(L"%d", ((data[j] << 8) | data[j + 1]));
            listCtrl.SetItemText(k, 1, str);
        }
    } else {
        for (int i = startAddress, j = 3, k = 0; i <= endAddress; ++i, j += 2, ++k) {
            str.Format(L"%d", ((data[j] << 8) | data[j + 1]));
            listCtrl.SetItemText(k, 1, str);
        }
    }


    return 0;
}

int         RegisterView::setRegister()
{
    CString str;
    editInput.GetWindowTextW(str);
    int value = _ttoi(str);
    if (value > 65535) {
        MessageBox(L"数据不能大于65535", L"错误", MB_ICONERROR);
        editInput.SetFocus();
    } else {
        editInput.ShowWindow(SW_HIDE);
        // 调用主对话框，发送函数
        listCtrl.SetItemText(row, 2, str);

        str = listCtrl.GetItemText(row, 0);
        int address = _ttoi(str);
        
        ModbusRTUToolDlg* modbusRTUToolDlg = (ModbusRTUToolDlg*)AfxGetMainWnd();
        modbusRTUToolDlg->setRegister(address, value);
        //::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_SET_REGISTER, (WPARAM)address, (LPARAM)value);
        //::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_SET_REGISTER, (WPARAM)0, (LPARAM)0);
        //::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_SET_REGISTER, (WPARAM)address, (LPARAM)value);
    }
    return 0;
}

void RegisterView::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    LVHITTESTINFO lvhti;

    // Clear the subitem text the user clicked on.
    lvhti.pt = pNMItemActivate->ptAction;
    listCtrl.SubItemHitTest(&lvhti);

    if (lvhti.flags & LVHT_ONITEMLABEL) {
        if (lvhti.iSubItem == 2) {
            row = lvhti.iItem;
            CRect rect;
            listCtrl.GetSubItemRect(lvhti.iItem, lvhti.iSubItem, LVIR_LABEL, rect);
            CString str = listCtrl.GetItemText(lvhti.iItem, lvhti.iSubItem);
            editInput.MoveWindow(rect);
            editInput.SetWindowTextW(str);
            editInput.ShowWindow(SW_SHOW);
            editInput.SetFocus();
            editInput.SetSel(0, -1, TRUE);
        }
    }
}


void RegisterView::OnEnKillfocusEditInput()
{
    // TODO: 在此添加控件通知处理程序代码
    setRegister();
    //editInput.ShowWindow(SW_HIDE);
}


BOOL RegisterView::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)) {
        if(GetFocus()->GetDlgCtrlID() == IDC_EDIT_INPUT) {
        //if (pMsg->hwnd == editInput.GetSafeHwnd()) {
            //setRegister();
            editInput.ShowWindow(SW_HIDE);
            return TRUE;
        }
    }

    return CFormView::PreTranslateMessage(pMsg);
}
