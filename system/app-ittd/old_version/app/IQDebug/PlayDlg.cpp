// PlayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IQDebug.h"
#include "PlayDlg.h"
#include "ManageDlg.h"
#include "afxdialogex.h"
#include "h264or5parser.h"

int __stdcall play_capture_yuv(void *pPlay, const char *sFileName); // 特别实现的接口不公开

void CALLBACK play_yuv_callback(void *pPlay, disp_info_t *pInfo, void *pUser)
{
	CPlayDlg *pDlg = (CPlayDlg*)pUser;
//	pInfo->uPicWidth; // 图像的实际宽度
//	pInfo->uPicHeight; // 图像的实际高度
//	pInfo->yuv.uYStride; // 图像的Y分量跨度，指Y分量对齐的宽度，可能比图像的实际宽度大
//	pInfo->yuv.uUVStride; // 图像的UV分量跨度，指UV分量对齐的宽度，可能比图像的实际半宽度大
//	pInfo->yuv.pY; // 图像Y分量，注意根据uYStride操作
//	pInfo->yuv.pU; // 图像U分量，注意根据uUVStride操作
//	pInfo->yuv.pV; // 图像V分量，注意根据uUVStride操作
}

// Implementation of the RTSP 'response handlers':

void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) {
	do {
		UsageEnvironment& env = rtspClient->envir(); // alias
		StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
		CPlayDlg *pDlg = (CPlayDlg*)((ourRTSPClient*)rtspClient)->dlg;

		if (resultCode != 0) {
			*pDlg << *rtspClient << "Failed to get a SDP description: " << resultString << "\r\n";
			delete[] resultString;
			break;
		}

		char* const sdpDescription = resultString;
		*pDlg << *rtspClient << "Got a SDP description:\r\n" << sdpDescription << "\r\n";

		// Create a media session object from this SDP description:
		scs.session = MediaSession::createNew(env, sdpDescription);
		delete[] sdpDescription; // because we don't need it anymore
		if (scs.session == NULL) {
			*pDlg << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\r\n";
			break;
		} else if (!scs.session->hasSubsessions()) {
			*pDlg << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\r\n";
			break;
		}

		// Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
		// calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
		// (Each 'subsession' will have its own data source.)
		scs.iter = new MediaSubsessionIterator(*scs.session);
		setupNextSubsession(rtspClient);
		return;
	} while (0);

	// An unrecoverable error occurred with this stream.
	shutdownStream(rtspClient);
}

void setupNextSubsession(RTSPClient* rtspClient) {
	UsageEnvironment& env = rtspClient->envir(); // alias
	StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
	CPlayDlg *pDlg = (CPlayDlg*)((ourRTSPClient*)rtspClient)->dlg;

	scs.subsession = scs.iter->next();
	if (scs.subsession != NULL) {
		if (!scs.subsession->initiate()) {
			*pDlg << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\r\n";
			setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
		} else {
			*pDlg << *rtspClient << "Initiated the \"" << *scs.subsession << "\" subsession (";
			if (scs.subsession->rtcpIsMuxed()) {
				*pDlg << "client port " << scs.subsession->clientPortNum();
			} else {
				*pDlg << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1;
			}
			*pDlg << ")\r\n";

			// Continue setting up this subsession, by sending a RTSP "SETUP" command:
			rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, pDlg->m_bUsingTCP);
		}
		return;
	}

	// We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
	if (scs.session->absStartTime() != NULL) {
		// Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
		rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());
	} else {
		scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
		rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
	}
}

void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) {
	do {
		UsageEnvironment& env = rtspClient->envir(); // alias
		StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
		CPlayDlg *pDlg = (CPlayDlg*)((ourRTSPClient*)rtspClient)->dlg;

		if (resultCode != 0) {
			*pDlg << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\r\n";
			break;
		}

		*pDlg << *rtspClient << "Set up the \"" << *scs.subsession << "\" subsession (";
		if (scs.subsession->rtcpIsMuxed()) {
			*pDlg << "client port " << scs.subsession->clientPortNum();
		} else {
			*pDlg << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1;
		}
		*pDlg << ")\r\n";

		// Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
		// (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
		// after we've sent a RTSP "PLAY" command.)

		if (scs.subsession->readSource() == NULL) break; // was not initiated

		DataSink *dataSink = NULL;
		if (strcmp(scs.subsession->mediumName(), "video") == 0) {
			const char *sps = NULL;
			// For H.264 video stream, we use a special sink that adds 'start codes',
			// and (at the start) the SPS and PPS NAL units:
			if (strcmp(scs.subsession->codecName(), "H264") == 0) {
				sps = scs.subsession->fmtp_spropparametersets();
			}  else if (strcmp(scs.subsession->codecName(), "H265") == 0) {
				sps = scs.subsession->fmtp_spropsps();
			} else if (strcmp(scs.subsession->codecName(), "YUV") == 0) {
				sps = scs.subsession->codecName(); // 以便能继续执行
			}
			if (sps && sps[0] != '\0')
			{
				int width, height;
				video_format_t vfmt = {0};
				if (strcmp(scs.subsession->codecName(), "H264") == 0 ||
					strcmp(scs.subsession->codecName(), "H265") == 0) {
						unsigned i, numSPropRecords;
						SPropRecord* sPropRecords = parseSPropParameterSets(sps, numSPropRecords);
						for (i = 0; i < numSPropRecords; ++i) {
							if (sPropRecords[i].sPropBytes[0] == H265_SPS_TYPE) {
								if (h265_decode_sps((char*)sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength, &width, &height) == 0) {
									vfmt.nEncProfile = VIDEO_PROFILE_H265_MP;
									break;
								}
							} else {
								h264_type_t *h264 = (h264_type_t*)&sPropRecords[i].sPropBytes[0];
								if (h264->u5Type == H264_SPS_TYPE) {
									if (h264_decode_sps((char*)sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength, &width, &height) == 0) {
										vfmt.nEncProfile = VIDEO_PROFILE_H264_MP;
										break;
									}
								}
							}
						}
						delete[] sPropRecords;
				} else if (strcmp(scs.subsession->codecName(), "YUV") == 0) {
						width = scs.subsession->videoWidth();
						height = scs.subsession->videoHeight();
						vfmt.nEncProfile = VIDEO_PROFILE_RAW_YUV;
				}
				if (vfmt.nEncProfile) {
					vfmt.lPicWidth = width;
					vfmt.lPicHeight = height;
					pDlg->m_pPlayHandle = play_create(pDlg->m_hPlayWnd, &vfmt, NULL, 1);
					if (pDlg->m_pPlayHandle)
					{
						play_display_func(pDlg->m_pPlayHandle, DISP_TYPE_YUV420, 100U, play_yuv_callback, pDlg);
					}
					pDlg->PostMessage(WM_VIDEOPLAY, width, height);
				}
				// For H.264 video stream, we use a special sink that adds 'start codes',
				// and (at the start) the SPS and PPS NAL units:
				CStringA strFileName;
				const char *str = NULL;
				if (strcmp(scs.subsession->codecName(), "H264") == 0) {
					if (!pDlg->m_strSaveDir.IsEmpty())
					{
						pDlg->TimeToFileName(strFileName, pDlg->m_strSaveDir, CString(_T(".264")));
						str = strFileName;
					}
					dataSink = H264VideoDataSink::createNew(env, str,
						pDlg->m_pPlayHandle,
						scs.subsession->fmtp_spropparametersets(),
						VIDEO_DATA_SINK_DATA_BUFSZ);
				}  else if (strcmp(scs.subsession->codecName(), "H265") == 0) {
					// For H.265 video stream, we use a special sink that adds 'start codes',
					// and (at the start) the VPS, SPS, and PPS NAL units:
					if (!pDlg->m_strSaveDir.IsEmpty())
					{
						pDlg->TimeToFileName(strFileName, pDlg->m_strSaveDir, CString(_T(".265")));
						str = strFileName;
					}
					dataSink = H265VideoDataSink::createNew(env, str,
						pDlg->m_pPlayHandle,
						scs.subsession->fmtp_spropvps(),
						scs.subsession->fmtp_spropsps(),
						scs.subsession->fmtp_sproppps(),
						VIDEO_DATA_SINK_DATA_BUFSZ);
				}  else if (strcmp(scs.subsession->codecName(), "YUV") == 0) {
					// For H.265 video stream, we use a special sink that adds 'start codes',
					// and (at the start) the VPS, SPS, and PPS NAL units:
					if (!pDlg->m_strSaveDir.IsEmpty())
					{
						pDlg->TimeToFileName(strFileName, pDlg->m_strSaveDir, CString(_T(".yuv")));
						str = strFileName;
					}
					dataSink = YUVVideoDataSink::createNew(env, str,
						pDlg->m_pPlayHandle, VIDEO_DATA_SINK_DATA_BUFSZ);
				}
			}
		}

		scs.subsession->sink = dataSink;
		// perhaps use your own custom "MediaSink" subclass instead
		if (scs.subsession->sink == NULL) {
			*pDlg << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession
				<< "\" subsession: " << env.getResultMsg() << "\r\n";
			break;
		}

		*pDlg << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\r\n";
		scs.subsession->miscPtr = rtspClient; // a hack to let subsession handler functions get the "RTSPClient" from the subsession 
		scs.subsession->sink->startPlaying(*(scs.subsession->readSource()),
			subsessionAfterPlaying, scs.subsession);
		// Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
		if (scs.subsession->rtcpInstance() != NULL) {
			scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.subsession);
		}
	} while (0);
	delete[] resultString;

	// Set up the next subsession, if any:
	setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) {
	Boolean success = False;

	do {
		UsageEnvironment& env = rtspClient->envir(); // alias
		StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
		CPlayDlg *pDlg = (CPlayDlg*)((ourRTSPClient*)rtspClient)->dlg;

		if (resultCode != 0) {
			*pDlg << *rtspClient << "Failed to start playing session: " << resultString << "\r\n";
			break;
		}

		// Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
		// using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
		// 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
		// (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
		if (scs.duration > 0) {
			unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
			scs.duration += delaySlop;
			unsigned uSecsToDelay = (unsigned)(scs.duration*1000000);
			scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
		}

		*pDlg << *rtspClient << "Started playing session";
		if (scs.duration > 0) {
			*pDlg << " (for up to " << scs.duration << " seconds)";
		}
		*pDlg << "...\r\n";

		success = True;
	} while (0);
	delete[] resultString;

	if (!success) {
		// An unrecoverable error occurred with this stream.
		shutdownStream(rtspClient);
	}
}


// Implementation of the other event handlers:

void subsessionAfterPlaying(void* clientData) {
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

	// Begin by closing this subsession's stream:
	Medium::close(subsession->sink);
	subsession->sink = NULL;

	// Next, check whether *all* subsessions' streams have now been closed:
	MediaSession& session = subsession->parentSession();
	MediaSubsessionIterator iter(session);
	while ((subsession = iter.next()) != NULL) {
		if (subsession->sink != NULL) return; // this subsession is still active
	}

	// All subsessions' streams have now been closed, so shutdown the client:
	shutdownStream(rtspClient);
}

void subsessionByeHandler(void* clientData) {
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
	UsageEnvironment& env = rtspClient->envir(); // alias
	CPlayDlg *pDlg = (CPlayDlg*)((ourRTSPClient*)rtspClient)->dlg;

	*pDlg << *rtspClient << "Received RTCP \"BYE\" on \"" << *subsession << "\" subsession\r\n";

	// Now act as if the subsession had closed:
	subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void* clientData) {
	ourRTSPClient* rtspClient = (ourRTSPClient*)clientData;
	StreamClientState& scs = rtspClient->scs; // alias

	scs.streamTimerTask = NULL;

	// Shut down the stream:
	shutdownStream(rtspClient);
}

void shutdownStream(RTSPClient* rtspClient) {
	UsageEnvironment& env = rtspClient->envir(); // alias
	StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
	CPlayDlg *pDlg = (CPlayDlg*)((ourRTSPClient*)rtspClient)->dlg;

	// First, check whether any subsessions have still to be closed:
	if (scs.session != NULL) {
		Boolean someSubsessionsWereActive = False;
		MediaSubsessionIterator iter(*scs.session);
		MediaSubsession* subsession;

		while ((subsession = iter.next()) != NULL) {
			if (subsession->sink != NULL) {
				Medium::close(subsession->sink);
				subsession->sink = NULL;

				if (subsession->rtcpInstance() != NULL) {
					subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
				}

				someSubsessionsWereActive = True;
			}
		}

		if (someSubsessionsWereActive) {
			// Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
			// Don't bother handling the response to the "TEARDOWN".
			rtspClient->sendTeardownCommand(*scs.session, NULL);
		}
	}

	*pDlg << *rtspClient << "Closing the stream.\r\n";
	Medium::close(rtspClient);
	// Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.

	//if (--rtspClientCount == 0) {
		// The final stream has ended, so exit the application now.
		// (Of course, if you're embedding this code into your own application, you might want to comment this out,
		// and replace it with "eventLoopWatchVariable = 1;", so that we leave the LIVE555 event loop, and continue running "main()".)
		pDlg->m_eventLoopWatchVariable = 1;
		pDlg->m_env = NULL;
		pDlg->m_pRTSPClient = NULL;
	//}
}

void playPauseStream(RTSPClient* rtspClient) {
	UsageEnvironment& env = rtspClient->envir(); // alias
	StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

	// Check whether any subsessions have still to be set:
	if (scs.session != NULL) {
		MediaSubsessionIterator iter(*scs.session);
		MediaSubsession* subsession;

		while ((subsession = iter.next()) != NULL) {
			if (subsession->sink != NULL) {
				DataSink *dataSink = dynamic_cast<DataSink*>(subsession->sink);
				if (dataSink)
				{
					if (dataSink->fPlayPause)
					{
						dataSink->fPlayPause = False;
						dataSink->fHaveKeyFrame = True;
					}
					else
					{
						dataSink->fPlayPause = True;
					}
				}
			}
		}
	}
}

// CPlayDlg 对话框

IMPLEMENT_DYNAMIC(CPlayDlg, CDialogEx)

CPlayDlg::CPlayDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPlayDlg::IDD, pParent)
	, m_env(NULL)
	, m_eventLoopWatchVariable(0)
	, m_pRTSPClient(NULL)
	, m_rtspMsg(_T(""))
	, m_pThread(NULL)
	, m_pPlayHandle(NULL)
	, m_hPlayWnd(NULL)
	, m_rtspURL(_T(""))
	, m_mousePoint(0)
	, m_bPlayZoom(FALSE)
	, m_capturePoint(0)
	, m_bMouseCapture(FALSE)
	, m_strSaveDir(_T(""))
	, m_manageDlg(NULL)
{

}

CPlayDlg::~CPlayDlg()
{
}

void CPlayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlayDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CPlayDlg::OnBnClickedOk)
	ON_WM_NCDESTROY()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_VIDEOPLAY, &CPlayDlg::OnVideoplay)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SAVE_JPG, &CPlayDlg::OnSaveJpg)
	ON_COMMAND(ID_SAVE_YUV, &CPlayDlg::OnSaveYuv)
	ON_COMMAND(ID_PLAY_PAUSE, &CPlayDlg::OnPlayPause)
	ON_COMMAND(ID_PLAY_STOP, &CPlayDlg::OnPlayStop)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_PLAY_ZOOM, &CPlayDlg::OnPlayZoom)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPlayDlg 消息处理程序

UINT TaskSchedulerLoop(LPVOID pParm)
{
	CPlayDlg* pDlg = (CPlayDlg*)pParm;
	if (pDlg->m_env)
	{
		pDlg->m_env->taskScheduler().doEventLoop(&pDlg->m_eventLoopWatchVariable);
	}
	return 0;
}

int CPlayDlg::SetRTSPClient(CManageDlg *pDlg, CString &rtspURL, ourRTSPClient *rtspClient, UsageEnvironment* env, Boolean usingTCP)
{
	SetWindowText(rtspURL);

	m_manageDlg = pDlg;
	m_manageDlg->GetSaveDir(IDC_DATAFOLDER, m_strSaveDir);

	*this << "RTSP connecting...\r\n";
	rtspClient->sendDescribeCommand(continueAfterDESCRIBE);

	m_env = env;
	m_rtspURL = rtspURL;
	m_pRTSPClient = rtspClient;
	m_bUsingTCP = usingTCP;

	m_pThread = AfxBeginThread(TaskSchedulerLoop, this);
	if (m_pThread)
	{
		m_pThread->SetThreadPriority(THREAD_PRIORITY_HIGHEST);
		return 0;
	}

	return -1;
}

CPlayDlg& CPlayDlg::operator<<(LPCSTR msg)
{
	m_rtspMsg += msg;
	GetDlgItem(IDC_DISPLAY)->SetWindowText(m_rtspMsg);
	return *this;
}

CPlayDlg& CPlayDlg::operator<<(const RTSPClient& rtspClient) {
	return *this << "[URL:\"" << rtspClient.url() << "\"]: ";
}

CPlayDlg& CPlayDlg::operator<<(const MediaSubsession& subsession) {
	return *this << subsession.mediumName() << "/" << subsession.codecName();
}

CPlayDlg& CPlayDlg::operator<<(int i) {
	char str[32];
	return *this << _itoa(i, str, 10);
}

CPlayDlg& CPlayDlg::operator<<(unsigned u) {
	char str[32];
	return *this << _ultoa(u, str, 10);
}

CPlayDlg& CPlayDlg::operator<<(double d) {
	int decimal, sign;
	int precision = 10;
	return *this << _ecvt(d, precision, &decimal, &sign);
}

void CPlayDlg::OnClose()
{
	DestroyWindow();
//	CDialogEx::OnClose();
}


void CPlayDlg::OnBnClickedOk()
{
	OnClose();
}


void CPlayDlg::OnNcDestroy()
{
	CDialogEx::OnNcDestroy();

	delete this;
}


void CPlayDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (m_pRTSPClient != NULL)
	{
		if (m_pThread != NULL)
		{
			m_pThread->SuspendThread();
			shutdownStream(m_pRTSPClient);
			m_pThread->ResumeThread(); /* thread exit */
		}
	}

	if (m_pPlayHandle)
	{
		play_destroy(m_pPlayHandle);
	}
}


afx_msg LRESULT CPlayDlg::OnVideoplay(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_DISPLAY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDOK)->ShowWindow(SW_HIDE);

	CRect WinRect, ClientRect;
	GetWindowRect(WinRect);
	GetClientRect(ClientRect);

	int Width = WinRect.Width() - ClientRect.Width() + (int)wParam;
	int Height = WinRect.Height() - ClientRect.Height() + (int)lParam;

	int XPos = (GetSystemMetrics(SM_CXSCREEN) - Width) / 2;
	int YPos = (GetSystemMetrics(SM_CYSCREEN) - Height) / 2;

	MoveWindow(XPos < 0 ? 0 : XPos, YPos < 0 ? 0 : YPos, Width, Height);
	GetDlgItem(IDC_VIDEOWND)->MoveWindow(0, 0, (int)wParam, (int)lParam);
	GetDlgItem(IDC_VIDEOWND)->ShowWindow(SW_SHOW);

	CString str;
	str.Format(_T(" %d x %d"), (int)wParam, (int)lParam);
	SetWindowText(m_rtspURL + str);
	return 0;
}


BOOL CPlayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	m_hPlayWnd = GetDlgItem(IDC_VIDEOWND)->GetSafeHwnd();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CPlayDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rect;
	GetClientRect(rect);
	ClientToScreen(rect);
	if (rect.PtInRect(point))
	{
		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_MENU1));
		CMenu *pPopup = menu.GetSubMenu(0);
		if (pPopup)
		{
			pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
	}
}


void CPlayDlg::OnSaveJpg()
{
	if (m_pPlayHandle)
	{
		CString strPath;
		if (m_manageDlg->GetSaveDir(IDC_JPGFOLDER, strPath))
		{
			CStringA strFileName;
			TimeToFileName(strFileName, strPath, CString(_T(".jpg")));
			play_capture_jpeg(m_pPlayHandle, 100U, strFileName);
		}
	}
}


void CPlayDlg::OnSaveYuv()
{
	if (m_pPlayHandle)
	{
		CString strPath;
		if (m_manageDlg->GetSaveDir(IDC_YUVFOLDER, strPath))
		{
			CStringA strFileName;
			TimeToFileName(strFileName, strPath, CString(_T(".yuv")));
			play_capture_yuv(m_pPlayHandle, strFileName);
		}
	}
}


void CPlayDlg::OnPlayPause()
{
	if (m_pRTSPClient != NULL)
	{
		if (m_pThread != NULL)
		{
			m_pThread->SuspendThread();
			playPauseStream(m_pRTSPClient);
			m_pThread->ResumeThread();
		}
	}
}


void CPlayDlg::OnPlayStop()
{
	DestroyWindow();
}


void CPlayDlg::TimeToFileName(CStringA &strFilename, CString &strPath, CString &strFileExt)
{
	CString strTime;
	CTime timeCurr = CTime::GetCurrentTime();
	strTime.Format(_T("\\%d%02d%02d_%02d%02d%02d"),
		timeCurr.GetYear(),
		timeCurr.GetMonth(),
		timeCurr.GetDay(),
		timeCurr.GetHour(),
		timeCurr.GetMinute(),
		timeCurr.GetSecond());

	CString str = strPath + strTime + strFileExt;
	
	int i = 0;
	char s[32];
	while (PathFileExists(str)) {
		str = strPath + strTime;
		str += '_';
		str += _itoa(++i, s, 10);
		str += strFileExt;
	}
	strFilename = str;
}


BOOL CPlayDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccel)
	{
		if (TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CPlayDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_pPlayHandle)
	{
		CRect rect;
		GetDlgItem(IDC_VIDEOWND)->GetWindowRect(rect);
		ScreenToClient(rect);
		if (m_bPlayZoom)
		{
			if (m_bMouseCapture)
			{
				point -= rect.TopLeft();
				if (point.x < 0)
				{
					point.x = 0;
				}
				else if (point.x >= rect.Width())
				{
					point.x = rect.Width() - 1;
				}
				if (point.y < 0)
				{
					point.y = 0;
				}
				else if (point.y >= rect.Height())
				{
					point.y = rect.Height() - 1;
				}

				CPoint position(m_capturePoint - point);
				m_capturePoint = point;
				position.x /= 2;
				position.y /= 2;
				position += m_ZoomRect.TopLeft();
				if (position.x < 0)
				{
					position.x = 0;
				}
				else if (position.x + m_ZoomRect.Width() > rect.Width())
				{
					position.x = rect.Width() - m_ZoomRect.Width();
				}
				if (position.y < 0)
				{
					position.y = 0;
				}
				else if (position.y + m_ZoomRect.Height() > rect.Height())
				{
					position.y = rect.Height() - m_ZoomRect.Height();
				}
				m_ZoomRect.MoveToXY(position);
				play_set_rect(m_pPlayHandle, m_hPlayWnd, m_ZoomRect);
			}
		}
		else
		{
			if (rect.PtInRect(point))
			{
				m_mousePoint = point - rect.TopLeft();
			}
		}
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CPlayDlg::OnPlayZoom()
{
	if (m_pPlayHandle)
	{
		if (m_bPlayZoom)
		{
			play_set_rect(m_pPlayHandle, m_hPlayWnd, NULL);
		}
		else
		{
			CRect rect;
			GetDlgItem(IDC_VIDEOWND)->GetWindowRect(rect);

			int Width = rect.Width() / 2;
			int Height = rect.Height() / 2;

			int xPos, yPos;
			if (m_mousePoint.x < Width / 2)
			{
				xPos = 0;
			}
			else if (m_mousePoint.x > rect.Width() - Width / 2)
			{
				xPos = rect.Width() - Width;
			}
			else
			{
				xPos = m_mousePoint.x - Width / 2;
			}

			if (m_mousePoint.y < Height / 2)
			{
				yPos = 0;
			}
			else if (m_mousePoint.y > rect.Height() - Height / 2)
			{
				yPos = rect.Height() - Height;
			}
			else
			{
				yPos = m_mousePoint.y - Height / 2;
			}

			m_ZoomRect.SetRect(xPos, yPos, xPos + Width - 1, yPos + Height - 1);
			play_set_rect(m_pPlayHandle, m_hPlayWnd, m_ZoomRect);
		}
		m_bPlayZoom = !m_bPlayZoom;
	}
}


void CPlayDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bPlayZoom)
	{
		if (!m_bMouseCapture)
		{
			CRect rect;
			GetDlgItem(IDC_VIDEOWND)->GetWindowRect(rect);
			ScreenToClient(rect);
			if (rect.PtInRect(point))
			{
				m_capturePoint = point - rect.TopLeft();
				SetCapture();
				m_bMouseCapture = TRUE;
			}
		}
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPlayDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bMouseCapture)
	{
		ReleaseCapture();
		m_bMouseCapture = FALSE;
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CPlayDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (m_pPlayHandle)
	{
		play_refresh(m_pPlayHandle);
	}
	// 不为绘图消息调用 CDialogEx::OnPaint()
}
