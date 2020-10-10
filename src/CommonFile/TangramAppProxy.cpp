#include "TangramAppProxy.h"
#include "TangramMfcappbase.cpp"

CAppProxy theAppProxy;

CAppProxy::CAppProxy()
{
}

CAppProxy::~CAppProxy()
{
	//HMODULE hModule = ::GetModuleHandle(L"universe.dll");
	//if (hModule) {
	//	typedef int(__stdcall* FreeCLR)();
	//	FreeCLR FuncFreeCLR;
	//	FuncFreeCLR = (FreeCLR)GetProcAddress(hModule, "FreeCLR");
	//	if (FuncFreeCLR != NULL) {
	//		FuncFreeCLR();
	//	}
	//	BOOL bUnload = ::FreeLibrary(hModule);
	//	while (bUnload)
	//		bUnload = ::FreeLibrary(hModule);
	//}
	
	//hModule = ::GetModuleHandle(L"tangram_chrome_rt.dll");
	//if (hModule) {
	//	BOOL bUnload = ::FreeLibrary(hModule);
	//	while (bUnload)
	//		bUnload = ::FreeLibrary(hModule);
	//}
	//
	//hModule = ::GetModuleHandle(L"chrome.dll");
	//if (hModule) {
	//	BOOL bUnload = ::FreeLibrary(hModule);
	//	while (bUnload)
	//		bUnload = ::FreeLibrary(hModule);
	//}
}

BOOL CAppProxy::OnAppIdle(BOOL& bIdle, LONG lCount)
{
	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	while (bIdle &&
		!::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE)) {
		if (!AfxGetApp()->OnIdle(lCount++))
			bIdle = FALSE;  // assume "no idle" state
	}
	//if (m_hMainWnd == NULL)
	//	m_hMainWnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	return true;
}

void CAppProxy::ForegroundIdleProc()
{
	//if (::IsWindow(m_hMainWnd) == FALSE)
	//	return 0;
	// send WM_IDLEUPDATECMDUI to the main window
	CWnd* pMainWnd = AfxGetApp()->m_pMainWnd;
	if (pMainWnd != NULL && pMainWnd->m_hWnd != NULL &&
		pMainWnd->IsWindowVisible())
	{
		AfxCallWndProc(pMainWnd, pMainWnd->m_hWnd, WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
		pMainWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
			(WPARAM)TRUE, 0, TRUE, TRUE);
	}
}

bool CAppProxy::OnUniversePreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CHAR || pMsg->hwnd == nullptr)
		return TRUE;
	HWND hwnd = pMsg->hwnd;
	CWinApp* pApp = AfxGetApp();
	CWnd* pWnd = CWnd::FromHandlePermanent(hwnd);
	if (pWnd)
	{
		if (CMFCPopupMenu::GetActiveMenu() && !::IsWindow(CMFCPopupMenu::GetActiveMenu()->m_hWnd))
		{
			return true;
		}
		//if (::GetAncestor(hwnd, GA_ROOTOWNER) == ::GetAncestor(pApp->m_pMainWnd->m_hWnd, GA_ROOTOWNER))
		//	AFXSetTopLevelFrame((CFrameWnd*)pApp->m_pMainWnd);
		if (pMsg->message == WM_LBUTTONDOWN)
		{
			pApp->m_pMainWnd->PreTranslateMessage(pMsg);
			return true;
		}
		return pWnd->PreTranslateMessage(pMsg);
	}
	else
		return TRUE;
	return pApp->PreTranslateMessage(pMsg);
}

bool CAppProxy::DoIdleWork()
{
	if (m_hMainWnd && ::IsWindow(m_hMainWnd) == false)
		m_hMainWnd = NULL;
	return false;
}

HWND CAppProxy::GetMainWnd()
{
	if (AfxGetApp()->m_pMainWnd)
		return AfxGetApp()->m_pMainWnd->m_hWnd;
	return nullptr;
}

void CAppProxy::HubbleInit()
{
	if (::GetModuleHandle(_T("universe.dll")) == nullptr)
	{
		HMODULE hModule = ::LoadLibrary(L"universe.dll");
		if (hModule == nullptr) {
			TCHAR m_szBuffer[MAX_PATH];
			if (SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, m_szBuffer) ==
				S_OK) {
				CString m_strProgramFilePath = CString(m_szBuffer);
				m_strProgramFilePath += _T("\\Tangram\\universe.dll");
				if (::PathFileExists(m_strProgramFilePath)) {
					hModule = ::LoadLibrary(m_strProgramFilePath);
				}
			}
		}
		if (hModule) {
			GetHubbleImpl _pHubbleImplFunction;
			_pHubbleImplFunction = (GetHubbleImpl)GetProcAddress(hModule, "GetHubbleImpl");
			if (_pHubbleImplFunction != NULL) {
				m_pHubbleImpl = _pHubbleImplFunction();
			}
			GetHubbleBase _pHubbleBaseFunction;
			_pHubbleBaseFunction = (GetHubbleBase)GetProcAddress(hModule, "GetHubbleBase");
			if (_pHubbleBaseFunction != NULL) {
				m_pHubbleBase = _pHubbleBaseFunction();
			}
			GetHubble _pHubbleFunction;
			_pHubbleFunction = (GetHubble)GetProcAddress(hModule, "GetHubble");
			if (_pHubbleFunction != NULL) {
				m_pHubble = _pHubbleFunction();
				BSTR bstrKey = ::SysAllocString(L"TangramApplicationImpl");
				HRESULT hr = m_pHubble->put_AppKeyValue(bstrKey, CComVariant((__int64)&theAppImpl));
				::SysFreeString(bstrKey);
				::PostAppMessage(::GetCurrentThreadId(), WM_CHROMEAPPINIT, (WPARAM)this, (LPARAM)&theAppImpl);
			}
		}
	}
}

bool CAppProxy::TangramInitEx()
{
	if (IsBrowserModel(false))
		return false;
	if (::GetModuleHandle(_T("universe.dll")) == nullptr)
	{
		HMODULE hModule = ::LoadLibrary(L"universe.dll");
		if (hModule == nullptr) {
			TCHAR m_szBuffer[MAX_PATH];
			if (SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, m_szBuffer) ==
				S_OK) {
				CString m_strProgramFilePath = CString(m_szBuffer);
				m_strProgramFilePath += _T("\\Tangram\\universe.dll");
				if (::PathFileExists(m_strProgramFilePath)) {
					hModule = ::LoadLibrary(m_strProgramFilePath);
				}
			}
		}
		if (hModule) {
			GetHubbleImpl _pHubbleImplFunction;
			_pHubbleImplFunction = (GetHubbleImpl)GetProcAddress(hModule, "GetHubbleImpl");
			if (_pHubbleImplFunction != NULL) {
				m_pHubbleImpl = _pHubbleImplFunction();
			}
			GetHubbleBase _pHubbleBaseFunction;
			_pHubbleBaseFunction = (GetHubbleBase)GetProcAddress(hModule, "GetHubbleBase");
			if (_pHubbleBaseFunction != NULL) {
				m_pHubbleBase = _pHubbleBaseFunction();
			}
			GetHubble _pHubbleFunction;
			_pHubbleFunction = (GetHubble)GetProcAddress(hModule, "GetHubble");
			if (_pHubbleFunction != NULL) {
				m_pHubble = _pHubbleFunction();
				BSTR bstrKey = ::SysAllocString(L"TangramApplicationImpl");
				HRESULT hr = m_pHubble->put_AppKeyValue(bstrKey, CComVariant((__int64)&theAppImpl));
				::SysFreeString(bstrKey);
				::PostAppMessage(::GetCurrentThreadId(), WM_CHROMEAPPINIT, (WPARAM)this, (LPARAM)&theAppImpl);
			}
		}
	}
	return true;
}

BOOL CAppProxy::IsAppIdleMessage()
{
	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	if (AfxGetApp()->IsIdleMessage(&(pState->m_msgCur))) {
		return true;
	}
	return false;
}

void CAppProxy::ProcessMsg(MSG* msg)
{
	if (msg->message != WM_KICKIDLE &&
		//msg->message != WM_CONTROLBARCREATED &&
		//msg->message != WM_MDICLIENTCREATED && 
		!AfxPreTranslateMessage(msg)) {
		if (msg->hwnd == 0 && msg->message == WM_COSMOSMSG)
		{
			switch (msg->lParam)
			{
			case 20190422:
			{
			}
			break;
			default:
				break;
			}
		}
		::TranslateMessage(msg);
		::DispatchMessage(msg);
	}
}

BOOL CAppProxy::InitCloudApp(bool bCrashReporting)
{
	HMODULE hModule = ::GetModuleHandle(L"tangram_chrome_rt.dll");
	if (hModule == nullptr)
		hModule = ::LoadLibrary(L"tangram_chrome_rt.dll");
	if (hModule) {
		FuncInitApp = (_InitApp)GetProcAddress(hModule, "InitApp");
		if (FuncInitApp != NULL) {
			FuncInitApp(bCrashReporting, this, &theAppImpl);
			::PostQuitMessage(0);
		}
		return true;
	}
	else
		return false;
};

CString CAppProxy::GetNTPXml()
{
	return _T("");
}

BOOL CAppProxy::IsBrowserModel(bool bCrashReporting)
{
	HMODULE hModule = ::GetModuleHandle(L"tangram_chrome_rt.dll");
	if (hModule == nullptr)
		hModule = ::LoadLibrary(L"tangram_chrome_rt.dll");
	if (hModule) {
		FuncIsBrowserModel = (_IsBrowserModel)GetProcAddress(hModule, "IsBrowserModel");
		if (FuncIsBrowserModel != NULL) {
			bool bRet = FuncIsBrowserModel(bCrashReporting, this, &theAppImpl);
			if (bRet == false)
				::FreeLibrary(hModule);
			return bRet;
		}
	}

	return false;
};

