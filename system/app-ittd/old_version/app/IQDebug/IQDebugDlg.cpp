
// IQDebugDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IQDebug.h"
#include "IQDebugDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIQDebugDlg 对话框
#define COUNTOF(a) sizeof(a)/sizeof(a[0])


CIQDebugDlg::CIQDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIQDebugDlg::IDD, pParent)
	, m_bConnect(false), m_nHeaderLen(0), m_nDataLen(0)
	, m_nSelect(0)
	, m_pDataBuf(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIQDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CIQDebugDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET, &CIQDebugDlg::OnSocket)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CIQDebugDlg::OnSelchangeTab)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CIQDebugDlg 消息处理程序
static TCHAR *Title[] = {_T("Manage"), _T("Test")};
static UINT ID[] = {CManageDlg::IDD, CTestDlg::IDD};
static CDialogEx *Dlg[COUNTOF(Title)];

BOOL CIQDebugDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_pDataBuf = new char[MAX_DATA_BUFSZ];

	int i = 0;
	Dlg[i++] = new CManageDlg(this);
	Dlg[i++] = new CTestDlg(this);

	CRect rect;
	m_tabCtrl.GetClientRect(rect);
	for (int i=0; i<COUNTOF(Title); i++)
	{
		CRect rc;
		m_tabCtrl.InsertItem(i, Title[i]);
		m_tabCtrl.GetItemRect(i, rc);
		Dlg[i]->Create(ID[i], &m_tabCtrl);
		Dlg[i]->MoveWindow(0, rc.Height(), rect.Width(), rect.Height() - rc.Height());
	}
	Dlg[0]->ShowWindow(SW_SHOW);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIQDebugDlg::OnPaint()
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
HCURSOR CIQDebugDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIQDebugDlg::IttdConnect(DWORD dwIP, int nPort)
{
	if (m_bConnect)
	{
		closesocket(m_hSocket);
	}
	else
	{
		UpdateData();

		SOCKET hSocket = socket(AF_INET, SOCK_STREAM, 0);

		struct sockaddr_in sin;
		sin.sin_family           = AF_INET;
		sin.sin_port             = ntohs(nPort);
		sin.sin_addr.S_un.S_addr = htonl(dwIP);
		if (connect(hSocket,(sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR ||
			WSAAsyncSelect(hSocket, m_hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
		{
			closesocket(hSocket);
			MessageBox(_T("Connect fail\n"));
			return;
		}

		m_hSocket = hSocket;
	}
	m_bConnect = !m_bConnect;
	for (int i=0; i<COUNTOF(Title); i++)
	{
		if (Dlg[i])
		{
			Dlg[i]->PostMessage(WM_ITTCONNECT, m_bConnect ? 1 : 0, 0);
		}
	}
}


afx_msg LRESULT CIQDebugDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
	if(WSAGETSELECTERROR(lParam))
	{
		return 0;
	}
	char *pBuf;
	int r, nLen;
	SOCKET hSocket = (SOCKET)wParam;
	switch (WSAGETSELECTEVENT(lParam)) {
	case FD_READ:
		if (m_nHeaderLen < sizeof(iq_test_msg_t))
		{
			pBuf = (char*)&m_msgHeader + m_nHeaderLen;
			nLen = sizeof(iq_test_msg_t) - m_nHeaderLen;
			r = recv(hSocket, pBuf, nLen, 0);
			if (r == SOCKET_ERROR)
			{
				break;
			}
			m_nHeaderLen += r;
		}
		else
		{
			if (m_msgHeader.xorcheck == IQ_TEST_MSG_XOR(m_msgHeader) &&
				m_msgHeader.len > 0 && m_msgHeader.len < MAX_DATA_BUFSZ)
			{
				if (m_nDataLen < m_msgHeader.len)
				{
					pBuf = m_pDataBuf + m_nDataLen;
					nLen = m_msgHeader.len - m_nDataLen;
					r = recv(hSocket, pBuf, nLen, 0);
					if (r == SOCKET_ERROR)
					{
						break;
					}
					m_nDataLen += r;
					if (r == nLen)
					{
						MsgDataRecv(m_pDataBuf, m_nDataLen);
						/* 当前数据接收完成，准备接收下一数据 */
						m_nHeaderLen = 0;
						m_nDataLen   = 0;
					}
				}
			}
		}
		break;
	case FD_WRITE:
		break;
	case FD_CLOSE:
		closesocket(m_hSocket);
		m_bConnect = FALSE;
		for (int i=0; i<COUNTOF(Title); i++)
		{
			if (Dlg[i])
			{
				Dlg[i]->PostMessage(WM_ITTCONNECT, 0, 0);
			}
		}
		MessageBox(_T("Ittd socket closed\n"));
		break;
	default:
		break;
	}
	return 0;
}


int CIQDebugDlg::MsgDataSend(char* pData, int nDataLen)
{
	if (m_bConnect)
	{
		if (pData && nDataLen < MAX_DATA_BUFSZ)
		{
			iq_test_msg_t msg;
			SET_IQ_TEST_MSG(msg, IQ_TEST_DATA, nDataLen);
			if (send(m_hSocket, (char*)&msg, sizeof(iq_test_msg_t), 0) == SOCKET_ERROR ||
				send(m_hSocket, (char*)pData, nDataLen, 0) == SOCKET_ERROR)
			{
				// send error
				return -1;
			}
			return 0;
		}
	}
	return -1;
}


void CIQDebugDlg::OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nSel = m_tabCtrl.GetCurSel();
	if (nSel != m_nSelect)
	{
		Dlg[m_nSelect]->ShowWindow(SW_HIDE);
		m_nSelect = nSel;
	}
	Dlg[nSel]->ShowWindow(SW_SHOW);
	*pResult = 0;
}


int CIQDebugDlg::MsgDataRecv(char* pData, int nDataLen)
{
	if (pData && nDataLen > 0)
	{
		// 此处可以解析具体的数据，并发送到对应的窗口处理
		// 此处测试直接发给CTestDlg对话框
		for (int i=0; i<COUNTOF(Title); i++)
		{
			if (Dlg[i])
			{
				if (Dlg[i]->IsKindOf(RUNTIME_CLASS(CTestDlg)))
				{
					return Dlg[i]->SendMessage(WM_ITTDDATA, (WPARAM)pData, (LPARAM)nDataLen); /* 这是一个同步调用 */
				}
			}
		}
	}
	return 0;
}


void CIQDebugDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	for (int i=0; i<COUNTOF(Title); i++)
	{
		if (Dlg[i])
		{
			Dlg[i]->DestroyWindow();
			delete Dlg[i];
		}
	}
	if (m_bConnect)
	{
		closesocket(m_hSocket);
	}
	delete []m_pDataBuf;
}
