#pragma once


// CTestDlg 对话框

class CTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTestDlg)

public:
	CTestDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTestDlg();

// 对话框数据
	enum { IDD = IDD_TEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
protected:
	afx_msg LRESULT OnIttddata(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();
protected:
	afx_msg LRESULT OnIttconnect(WPARAM wParam, LPARAM lParam);
};
