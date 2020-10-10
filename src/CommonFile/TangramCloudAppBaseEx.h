#pragma once
#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>
#include <atlwin.h>
#include "tangramappbase.h"

class CMessagePumpImplProxy : public Universe::CTangramBrowserAppBase
{
public:
	CMessagePumpImplProxy();
	virtual ~CMessagePumpImplProxy();
	virtual BOOL OnAppIdle(BOOL& bIdle, LONG lCount);
	virtual bool DoIdleWork() { return false; };
	virtual BOOL IsAppIdleMessage();
	virtual void ProcessMsg(MSG* msg);

	CWinAppEx* m_pApp;
};

class CTangramAppBaseEx : 
	//public ChromePlus::CTangramMessagePumpImplProxy,
	public CWinAppEx, 
	public Universe::CTangramAppProxy
	{
public:
	CTangramAppBaseEx();
	virtual ~CTangramAppBaseEx();

	//virtual BOOL OnAppIdle(BOOL& bIdle, LONG lCount);
	//virtual bool DoIdleWork() {return false;};
	//virtual BOOL IsAppIdleMessage();
	//virtual void ProcessMsg(MSG* msg);
	virtual int Run();

	BOOL InitCloudApp(bool bCrashReporting);
	BOOL CheckChromeRT();
};
