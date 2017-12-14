#pragma once
#include "atltypes.h"


// CPlayDlg 对话框

class CPlayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPlayDlg)

public:
	CPlayDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlayDlg();

// 对话框数据
	enum { IDD = IDD_PLAY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UsageEnvironment* m_env;
	ourRTSPClient* m_pRTSPClient;
	char m_eventLoopWatchVariable;
	int SetRTSPClient(CManageDlg *pDlg, CString &rtspURL, ourRTSPClient *rtspClient, UsageEnvironment* env, Boolean usingTCP);
	void TimeToFileName(CStringA &strFilename, CString &strPath, CString &strFileExt);
	CPlayDlg& operator<<(LPCSTR msg);
	CPlayDlg& operator<<(const RTSPClient& rtspClient);
	CPlayDlg& operator<<(const MediaSubsession& subsession);
	CPlayDlg& operator<<(int i);
	CPlayDlg& operator<<(unsigned u);
	CPlayDlg& operator<<(double d);
private:
	CString m_rtspMsg;
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNcDestroy();
	afx_msg void OnDestroy();
private:
	CWinThread* m_pThread;
public:
	void* m_pPlayHandle;
protected:
	afx_msg LRESULT OnVideoplay(WPARAM wParam, LPARAM lParam);
public:
	HWND m_hPlayWnd;
	virtual BOOL OnInitDialog();
private:
	CString m_rtspURL;
public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSaveJpg();
	afx_msg void OnSaveYuv();
	afx_msg void OnPlayPause();
	afx_msg void OnPlayStop();
private:
	HACCEL m_hAccel;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
private:
	CPoint m_mousePoint;
public:
	afx_msg void OnPlayZoom();
private:
	BOOL m_bPlayZoom;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
private:
	CPoint m_capturePoint;
	BOOL m_bMouseCapture;
	CRect m_ZoomRect;
public:
	CString m_strSaveDir;
	afx_msg void OnPaint();
private:
	CManageDlg* m_manageDlg;
public:
	Boolean m_bUsingTCP;
};
