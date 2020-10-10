#pragma once
#include "TangramMfcAppbase.h"
#include <atlbase.h>
#include <atlcom.h>

class CAppProxy : public Universe::CTangramBrowserAppBase
{
public:
	CAppProxy();
	virtual ~CAppProxy();

	BOOL InitCloudApp(bool bCrashReporting);
	BOOL IsBrowserModel(bool bCrashReporting);
	virtual BOOL OnAppIdle(BOOL& bIdle, LONG lCount);
	virtual bool DoIdleWork();
	virtual BOOL IsAppIdleMessage();
	virtual void ProcessMsg(MSG* msg);
	virtual void ForegroundIdleProc();
	virtual CString GetNTPXml();
	virtual void HubbleInit();
	virtual bool TangramInitEx();
	virtual bool OnUniversePreTranslateMessage(MSG* pMsg);
	virtual HWND GetMainWnd();

	IHubble* m_pHubble = nullptr;
	CHubbleImpl* m_pHubbleImpl = nullptr;
	CTangramBase* m_pHubbleBase = nullptr;
};

extern CAppProxy theAppProxy;
