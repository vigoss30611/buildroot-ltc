// ManageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IQDebug.h"
#include "ManageDlg.h"
#include "afxdialogex.h"

#include "PlayDlg.h"
// CManageDlg 对话框

IMPLEMENT_DYNAMIC(CManageDlg, CDialogEx)

CManageDlg::CManageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CManageDlg::IDD, pParent)
{
	m_uIP = 0;
	m_nPort = 0;
	m_nProtocol = 0;
}

CManageDlg::~CManageDlg()
{
}

void CManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_IPAddress(pDX, IDC_IPADDRESS, m_uIP);
	DDX_Text(pDX, IDC_IPPORT, m_nPort);
	DDV_MinMaxInt(pDX, m_nPort, 1, 65535);
	DDX_Check(pDX, IDC_PLAY_SAVE, m_bSaveData);
	DDX_CBIndex(pDX, IDC_PROTOCOL, m_nProtocol);
}


BEGIN_MESSAGE_MAP(CManageDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CONNECT, &CManageDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_PLAY, &CManageDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_DATADIR, &CManageDlg::OnBnClickedDatadir)
	ON_BN_CLICKED(IDC_YUVDIR, &CManageDlg::OnBnClickedYuvdir)
	ON_BN_CLICKED(IDC_JPGDIR, &CManageDlg::OnBnClickedJpgdir)
ON_MESSAGE(WM_ITTCONNECT, &CManageDlg::OnIttconnect)
END_MESSAGE_MAP()


// CManageDlg 消息处理程序


void CManageDlg::OnBnClickedConnect()
{
	UpdateData();
	((CIQDebugDlg*)AfxGetApp()->m_pMainWnd)->IttdConnect(m_uIP, m_nPort);
}


void CManageDlg::OnBnClickedPlay()
{
	CString str;
	CComboBox *pCtrl = (CComboBox*)GetDlgItem(IDC_RTSPURL);
	pCtrl->GetWindowText(str);
	if (!str.IsEmpty())
	{
		CStringA strAnsi(str.GetBuffer(0));
		CPlayDlg *pDlg = new CPlayDlg(this);
		if (pDlg == NULL)
		{
			return;
		}
		pDlg->Create(IDD_PLAY_DIALOG, this);
		pDlg->ShowWindow(SW_SHOWNORMAL);

		TaskScheduler* scheduler = BasicTaskScheduler::createNew();
		UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

		char s[MAX_PATH];
		GetModuleFileNameA(NULL, s, MAX_PATH);
		ourRTSPClient* rtspClient = ourRTSPClient::createNew(*env, strAnsi.GetBuffer(0), RTSP_CLIENT_VERBOSITY_LEVEL, s, pDlg);
		if (rtspClient == NULL) {
			*pDlg << "Failed to create a RTSP client for URL \"" << s << "\": " << env->getResultMsg() << "\n";
			pDlg->DestroyWindow();
			return;
		}

		UpdateData();
		pDlg->SetRTSPClient(this, str, rtspClient, env, m_nProtocol == 0 ? False : True);

		if (pCtrl->FindString(-1, str) == CB_ERR)
		{
			pCtrl->AddString(str);
		}
	}
}


void CManageDlg::OnBnClickedDatadir()
{
	CString str;
	if (DirectoryChoose(this, str))
	{
		GetDlgItem(IDC_DATAFOLDER)->SetWindowText(str);
	}
}


void CManageDlg::OnBnClickedYuvdir()
{
	CString str;
	if (DirectoryChoose(this, str))
	{
		GetDlgItem(IDC_YUVFOLDER)->SetWindowText(str);
	}
}


void CManageDlg::OnBnClickedJpgdir()
{
	CString str;
	if (DirectoryChoose(this, str))
	{
		GetDlgItem(IDC_JPGFOLDER)->SetWindowText(str);
	}
}


BOOL CManageDlg::DirectoryChoose(CWnd* pOwner, CString& strDir)
{
	BOOL r = FALSE;

	LPMALLOC lpMalloc;
	if (::SHGetMalloc(&lpMalloc) != NOERROR)
	{
		return FALSE;
	}

	BROWSEINFO browseInfo;
	TCHAR szDisplayName[_MAX_PATH];
	browseInfo.hwndOwner = pOwner->GetSafeHwnd();
	browseInfo.pidlRoot = NULL; 
	browseInfo.pszDisplayName = szDisplayName;
	browseInfo.lpszTitle = NULL;
	browseInfo.ulFlags = (BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS);
	browseInfo.lpfn = NULL;
	browseInfo.lParam = 0;

	LPITEMIDLIST lpItemIDList;
	if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
	{
		TCHAR szBuffer[_MAX_PATH];
		// Get the path of the selected folder from the item ID list.
		if (::SHGetPathFromIDList(lpItemIDList, szBuffer))
		{
			strDir = szBuffer;
			if (!strDir.IsEmpty())
			{
				// We have a path in szBuffer!
				r = TRUE;
			}
		}
	}

	lpMalloc->Free(lpItemIDList);
	lpMalloc->Release();
	return r;
}


BOOL CManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_IPPORT)->SetWindowText(_T("11111"));
	GetDlgItem(IDC_CONNECT)->SetWindowText(((CIQDebugDlg*)AfxGetApp()->m_pMainWnd)->IsIttdConnect() ? _T("Disconnect") : _T("Connect"));
	GetDlgItem(IDC_RTSPURL)->SetWindowText(_T("rtsp://"));
	((CComboBox*)GetDlgItem(IDC_PROTOCOL))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CManageDlg::GetSaveDir(int nID, CString& strDir)
{
	UpdateData();
	if (nID != IDC_DATAFOLDER || m_bSaveData)
	{
		CWnd *pWnd = GetDlgItem(nID);
		if (pWnd)
		{
			pWnd->GetWindowText(strDir);
			if (!strDir.IsEmpty())
			{
				CStringA strAnsi(strDir);
				return MakeSureDirectoryPathExists(strAnsi);
			}
		}
	}
	strDir.Empty();
	return FALSE;
}


afx_msg LRESULT CManageDlg::OnIttconnect(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_CONNECT)->SetWindowText((int)wParam ? _T("Disconnect") : _T("Connect"));
	return 0;
}
