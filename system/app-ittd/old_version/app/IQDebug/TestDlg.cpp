// TestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IQDebug.h"
#include "TestDlg.h"
#include "afxdialogex.h"


// CTestDlg 对话框

IMPLEMENT_DYNAMIC(CTestDlg, CDialogEx)

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDlg::IDD, pParent)
{

}

CTestDlg::~CTestDlg()
{
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTestDlg::OnBnClickedOk)
	ON_MESSAGE(WM_ITTDDATA, &CTestDlg::OnIttddata)
	ON_MESSAGE(WM_ITTCONNECT, &CTestDlg::OnIttconnect)
END_MESSAGE_MAP()


// CTestDlg 消息处理程序


void CTestDlg::OnBnClickedOk()
{
	CString str;
	GetDlgItem(IDC_EDIT1)->GetWindowText(str);
	if (!str.IsEmpty())
	{
		int nLen = str.GetLength(); // UNICODE字符占2个字节，实际长度还要包括结束符
		((CIQDebugDlg*)AfxGetApp()->m_pMainWnd)->MsgDataSend((char*)str.GetBuffer(0), 2 * (nLen + 1));
	}
	//CDialogEx::OnOK();
}


afx_msg LRESULT CTestDlg::OnIttddata(WPARAM wParam, LPARAM lParam)
{
	// wParam是数据指针char*，lParam是数据长度int
	CString str((LPCTSTR)wParam); // 字符串以0结尾
	GetDlgItem(IDC_EDIT2)->SetWindowText(str);
	return 0;
}


BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDOK)->EnableWindow(((CIQDebugDlg*)AfxGetApp()->m_pMainWnd)->IsIttdConnect());
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


afx_msg LRESULT CTestDlg::OnIttconnect(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDOK)->EnableWindow((int)wParam);
	return 0;
}
