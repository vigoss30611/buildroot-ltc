
// IQDebugDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "testdlg.h"


// CIQDebugDlg 对话框
class CIQDebugDlg : public CDialogEx
{
// 构造
public:
	CIQDebugDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IQDEBUG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int MsgDataSend(char* pData, int nDataLen);
	void IttdConnect(DWORD dwIP, int nPort);
private:
	SOCKET m_hSocket;
	int m_nHeaderLen; /* 由于是异步接收，所以要记录已接收的数据头长度 */
	int m_nDataLen; /* 由于是异步接收，所以要记录已接收的数据长度 */
	iq_test_msg_t m_msgHeader;
protected:
	afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);
public:
	CTabCtrl m_tabCtrl;
	afx_msg void OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
private:
	int m_nSelect;
	BOOL m_bConnect;
public:
	int MsgDataRecv(char* pData, int nDataLen);
	BOOL IsIttdConnect(void) { return m_bConnect; }
	afx_msg void OnDestroy();
	char* m_pDataBuf;
};
