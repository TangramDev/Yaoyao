/********************************************************************************
*					Tangram Library - version 10.0.0							*
*********************************************************************************
* Copyright (C) 2002-2019 by Tangram Team.   All Rights Reserved.				*
*
* THIS SOURCE FILE IS THE PROPERTY OF TANGRAM TEAM AND IS NOT TO
* BE RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED
* WRITTEN CONSENT OF TANGRAM TEAM.
*
* THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS
* OUTLINED IN THE GPL LICENSE AGREEMENT.TANGRAM TEAM
* GRANTS TO YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE
* THIS SOFTWARE ON A SINGLE COMPUTER.
*
* CONTACT INFORMATION:
* mailto:tangramteam@outlook.com
* https://www.tangramteam.com
*
********************************************************************************/

#include "TangramAppBasic.h"
#include "TangramXmlParse.cpp"
//#include "Universe.c"
#include <afxole.h>

namespace CommonUniverse
{
	CHubbleImpl* g_pHubbleImpl = nullptr;
}

#ifdef _AFXDLL
extern CFrameWnd* g_pTopLevelFrame = nullptr;
#endif

typedef IHubble* (__stdcall* GetHubble)();
typedef CHubbleImpl* (__stdcall* GetHubbleImpl)(IHubble**);

typedef int(__stdcall* _InitApp)(bool bSupportCrashReporting, void*);
typedef bool(__stdcall* _IsBrowserModel)(bool bSupportCrashReporting, void*);
_InitApp FuncInitApp;
_IsBrowserModel FuncIsBrowserModel;

CWinApp* g_pAppBase = nullptr;
CTangramWinApp* g_pApp = nullptr;
IUniverseAppProxy* g_pAppProxy = nullptr;
IHubble* g_pHubble = nullptr;

namespace CommonUniverse
{
	CHubbleBrowserFactory* g_pBrowserFactory = nullptr;

	class CTangramOleMessageFilter : public COleMessageFilter
	{
	public:
		CTangramOleMessageFilter()
		{
		}

		virtual ~CTangramOleMessageFilter()
		{
		}

		// Overridables
		virtual BOOL OnMessagePending(const MSG* pMsg)
		{
			return COleMessageFilter::OnMessagePending(pMsg);
		}
	};

	class CTangramHelperWnd : public CWnd
	{
	public:
		CTangramHelperWnd() {}
		virtual ~CTangramHelperWnd() {}
		void PostNcDestroy()
		{
			CWnd::PostNcDestroy();
			delete this;
		}
		BEGIN_MSG_MAP(CTangramHelperWnd)
		END_MSG_MAP()
	};

	// CTangramMultiDocTemplate construction/destruction

	CTangramMultiDocTemplate::CTangramMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
		: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
	{
		g_pAppProxy->m_mapHubbleDocTemplateID[this] = nIDResource;
		g_pAppProxy->m_pHubbleImpl->m_nAppType = 1992;
	}

	// CTangramMDTDocTemplate construction/destruction

	CTangramMDTDocTemplate::CTangramMDTDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
		: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
	{
		g_pAppProxy->m_mapHubbleDocTemplateID[this] = nIDResource;
		g_pAppProxy->m_pHubbleImpl->m_nAppType = 1963;
	}

	// CHubbleDocTemplate construction/destruction

	CHubbleDocTemplate::CHubbleDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
		: CSingleDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
	{
		g_pAppProxy->m_mapHubbleDocTemplateID[this] = nIDResource;
		g_pAppProxy->m_pHubbleImpl->m_nAppType = 1965;
	}

	IMPLEMENT_DYNAMIC(CHubbleDocTemplate, CSingleDocTemplate)
		IMPLEMENT_DYNAMIC(CTangramMDTDocTemplate, CMultiDocTemplate)
		IMPLEMENT_DYNAMIC(CTangramMultiDocTemplate, CMultiDocTemplate)

		CTangramAppDelegate::CTangramAppDelegate()
	{
		m_strProviderID = _T("");
		g_pAppProxy = this;
	}

	CTangramAppDelegate::~CTangramAppDelegate()
	{
		if (m_pHubbleImpl)
		{
			m_pHubbleImpl->InserttoDataMap(0, m_strProviderID, nullptr);
			m_pHubbleImpl->InserttoDataMap(1, m_strProviderID, nullptr);
		}
	}

	BOOL CTangramAppDelegate::OnAppIdle(BOOL& bIdle, LONG lCount)
	{
		_AFX_THREAD_STATE* pState = AfxGetThreadState();
		while (bIdle &&
			!::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE)) {
			if (!AfxGetApp()->OnIdle(lCount++))
				bIdle = FALSE;  // assume "no idle" state
		}
		return true;
	}

	void CTangramAppDelegate::ForegroundIdleProc()
	{
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

	bool CTangramAppDelegate::OnUniversePreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message == WM_CHAR || pMsg->hwnd == nullptr)
			return TRUE;
		HWND hwnd = pMsg->hwnd;
		CWinApp* pApp = AfxGetApp();
		CWnd* pWnd = CWnd::FromHandlePermanent(hwnd);
		if (pWnd)
		{
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

	HWND CTangramAppDelegate::GetMainWnd()
	{
		if (AfxGetApp()->m_pMainWnd)
			return AfxGetApp()->m_pMainWnd->m_hWnd;
		return nullptr;
	}

	bool CTangramAppDelegate::HookAppDocTemplateInfo()
	{
		CString m_strDocTemplateScript = _T("<template>");
		if (m_pHubbleImpl->m_strDefaultTemplate == _T(""))
		{
			BOOL bCanHaveDocTemplateScript = true;
			POSITION nPos = g_pAppBase->GetFirstDocTemplatePosition();
			while (nPos)
			{
				CDocTemplate* pTemplate = g_pAppBase->GetNextDocTemplate(nPos);
				CString strExt = _T("");
				pTemplate->GetDocString(strExt, CDocTemplate::filterExt);
				strExt.MakeLower();
				if (strExt == _T(""))
					strExt = _T("tangramdefault");
				if (strExt != _T(""))
				{
					CString strFileTypeID = _T("");
					pTemplate->GetDocString(strFileTypeID, CDocTemplate::regFileTypeId);
					strFileTypeID.MakeLower();
					CString strfilterName = _T("");
					pTemplate->GetDocString(strfilterName, CDocTemplate::filterName);
					auto it = m_mapHubbleDocTemplateID.find(pTemplate);
					if (it != m_mapHubbleDocTemplateID.end())
					{
						CString s = _T("");
						s.Format(_T("<tangram%I64d%s ResID=\"%d\">%s|%s|*%s|%s|</tangram%I64d%s>"), (__int64)pTemplate, strExt, it->second, strFileTypeID, strfilterName, strExt, g_pAppBase->m_pszExeName, (__int64)pTemplate, strExt);
						m_strDocTemplateScript += s;
					}
				}
				else
				{
					bCanHaveDocTemplateScript = false;
					break;
				}
			}
			if (bCanHaveDocTemplateScript)
			{
				m_strDocTemplateScript += _T("</template>");
				m_pHubbleImpl->m_strDefaultTemplate = m_strDocTemplateScript;
			}
			else
				m_strDocTemplateScript = _T("");
		}
		return (m_strDocTemplateScript == _T("")) ? false : true;
	}

	BOOL CTangramAppDelegate::IsAppIdleMessage()
	{
		_AFX_THREAD_STATE* pState = AfxGetThreadState();
		if (AfxGetApp()->IsIdleMessage(&(pState->m_msgCur))) {
			return true;
		}
		return false;
	}

	void CTangramAppDelegate::ProcessMsg(MSG* msg)
	{
		if (msg->message != WM_KICKIDLE) {

			//BOOL bProcess = false;
			//CWinThread* pThread = AfxGetThread();
			//if (pThread)
			//{
			//	if (msg->hwnd == NULL)
			//		bProcess = AfxPreTranslateMessage(msg);
			//}
			//if (bProcess == false)
			//{
			//	// walk from target to main window
			//	CWnd* pMainWnd = AfxGetMainWnd();
			//	if (CWnd::WalkPreTranslateTree(pMainWnd->GetSafeHwnd(), msg))
			//		bProcess = TRUE;

			//	// in case of modeless dialogs, last chance route through main
			//	//   window's accelerator table
			//	if (pMainWnd != NULL)
			//	{
			//		if (::IsWindow(msg->hwnd))
			//		{
			//			CWnd* pWnd = CWnd::FromHandle(msg->hwnd);
			//			if (pWnd->GetTopLevelParent() != pMainWnd)
			//				bProcess = pMainWnd->PreTranslateMessage(msg);
			//		}
			//	}
			//}

			//if (msg->hwnd == 0 && msg->message == WM_COSMOSMSG)
			//{
			//}
			//if (bProcess)
			//	return;
			::TranslateMessage(msg);
			::DispatchMessage(msg);
		}
	}

	HWND CTangramAppDelegate::QueryCanClose(HWND hWnd)
	{
		CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);
		if (pWnd && pWnd == g_pAppBase->m_pMainWnd)
		{
			POSITION nPos = g_pAppBase->GetFirstDocTemplatePosition();
			while (nPos)
			{
				CDocTemplate* pTemplate = g_pAppBase->GetNextDocTemplate(nPos);
				POSITION pos = pTemplate->GetFirstDocPosition();
				while (pos != NULL)
				{
					CDocument* pDoc = pTemplate->GetNextDoc(pos);
					POSITION pos2 = pDoc->GetFirstViewPosition();
					while (pos2 != NULL)
					{
						CView* pView = pDoc->GetNextView(pos2);
						ASSERT_VALID(pView);
						CFrameWnd* pFrame = pView->GetParentFrame();
						if (g_pAppBase->m_pMainWnd != pFrame)
						{
							g_pAppBase->m_pMainWnd = pFrame;
							m_pHubbleImpl->m_hMainWnd = g_pAppBase->m_pMainWnd->m_hWnd;
							return pFrame->m_hWnd;
						}
					}
				}
			}
		}
		return NULL;
	};

	CString CTangramAppDelegate::GetNTPXml()
	{
		return _T("");
	}

	bool CTangramAppDelegate::HubbleInit(CString strID)
	{
		if (!afxContextIsDLL)
		{
			int		nArgs;
			LPWSTR* szArglist = nullptr;
			szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
			if (nArgs >= 2)
			{
				CString s = szArglist[1];
				if (s.CompareNoCase(_T("/RegServer")) == 0 ||
					s.CompareNoCase(_T("/Register")) == 0 ||
					s.CompareNoCase(_T("/Unregserver")) == 0 ||
					s.CompareNoCase(_T("/Unregister")) == 0)
					return true;
			}
			//CWinThread* pThread = AfxGetThread();
			//if (pThread != NULL)
			//{
			//	// destroy message filter (may be derived class)
			//	delete pThread->m_pMessageFilter;
			//	pThread->m_pMessageFilter = NULL;
			//}
			//// allocate and initialize default message filter
			//if (pThread->m_pMessageFilter == NULL)
			//{
			//	pThread->m_pMessageFilter = new CTangramOleMessageFilter;
			//	ASSERT(AfxOleGetMessageFilter() != NULL);
			//	pThread->m_pMessageFilter->EnableNotRespondingDialog(false);
			//	pThread->m_pMessageFilter->SetMessagePendingDelay(20000);
			//	AfxOleGetMessageFilter()->Register();
			//}
			if (IsBrowserModel(false))
				return false;
		}
		TCHAR m_szBuffer[MAX_PATH];
		HMODULE hModule = ::GetModuleHandle(_T("universe.dll"));
		if (hModule == nullptr)
		{
			TCHAR szDriver[MAX_PATH] = { 0 };
			TCHAR szDir[MAX_PATH] = { 0 };
			TCHAR szExt[MAX_PATH] = { 0 };
			TCHAR szFile2[MAX_PATH] = { 0 };
			::GetModuleFileName(NULL, m_szBuffer, MAX_PATH);
			_tsplitpath_s(m_szBuffer, szDriver, szDir, szFile2, szExt);
			CString strTangramDll = szDriver;
			strTangramDll += szDir;
			strTangramDll += _T("universe.dll");
			hModule = ::LoadLibrary(strTangramDll);
		}
		if (hModule == nullptr && SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, m_szBuffer) == S_OK)
		{
			CString m_strProgramFilePath = CString(m_szBuffer);
			m_strProgramFilePath += _T("\\Tangram\\universe.dll");
			if (::PathFileExists(m_strProgramFilePath)) {
				hModule = ::LoadLibrary(m_strProgramFilePath);
			}
		}

		if (hModule)
		{
			if (m_strContainer != _T(""))
			{
				m_strContainer = _T(",") + m_strContainer + _T(",");
				m_strContainer.Replace(_T(",,"), _T(","));
			}
			GetHubbleImpl _pHubbleImplFunction;
			_pHubbleImplFunction = (GetHubbleImpl)GetProcAddress(hModule, "GetHubbleImpl");
			g_pHubbleImpl = m_pHubbleImpl = _pHubbleImplFunction(&m_pHubble);
			g_pHubble = m_pHubble;

			if (!afxContextIsDLL)
			{
				m_strProviderID = AfxGetApp()->m_pszAppName;
				m_strProviderID += _T(".host");
				m_strProviderID.MakeLower();

				m_pHubbleImpl->m_pHubbleDelegate = static_cast<IHubbleDelegate*>(this);
				g_pHubbleImpl->InserttoDataMap(0, m_strProviderID, static_cast<IUniverseAppProxy*>(this));
				g_pHubbleImpl->InserttoDataMap(1, m_strProviderID, static_cast<IHubbleWindowProvider*>(this));

				if (g_pHubbleImpl->m_nAppType != APP_BROWSER &&
					g_pHubbleImpl->m_nAppType != APP_BROWSER_ECLIPSE)
					::PostAppMessage(::GetCurrentThreadId(), WM_CHROMEAPPINIT, (WPARAM)m_pHubbleImpl->m_pHubbleDelegate, g_pHubbleImpl->m_nAppType);
				m_pHubbleImpl->m_pUniverseAppProxy = this;
			}
			else
			{
				strID.Trim();
				if (strID == _T(""))
					strID = _T("component");
				if (m_strProviderID == _T(""))
				{
					CString strName = AfxGetApp()->m_pszAppName;
					m_strProviderID = strName + _T(".") + strID;
				}
				if (m_strProviderID != _T(""))
				{
					m_strProviderID.MakeLower();
					g_pHubbleImpl->InserttoDataMap(0, m_strProviderID, static_cast<IUniverseAppProxy*>(this));
					g_pHubbleImpl->InserttoDataMap(1, m_strProviderID, static_cast<IHubbleWindowProvider*>(this));
				}
			}
		}
		return true;
	}

	bool CTangramAppDelegate::ProcessAppType(bool bCrashReporting)
	{
		if (m_pHubbleImpl)
		{
			switch (m_pHubbleImpl->m_nAppType)
			{
			case APP_WIN32:
				return true;
				break;
			case APP_BROWSER:
			case APP_BROWSER_ECLIPSE:
			{
				if (m_pHubbleImpl->m_nAppType == APP_BROWSER_ECLIPSE)
				{
#ifdef _AFXDLL
#ifdef _DEBUG
					::MessageBox(NULL, _T("Chrome-Eclipse Model is not support MFC Share Dll"), _T("Warnning"), MB_OK);
#endif 
					TRACE(_T("\r\n\r\n********Chrome-Eclipse-CLR Mix-Model is not support MFC Share Dll********\r\n\r\n"));
#endif
				}
				m_pHubbleImpl->m_hMainWnd = NULL;
				HMODULE hModule = ::GetModuleHandle(L"tangram_chrome_rt.dll");
				if (hModule == nullptr)
					hModule = ::LoadLibrary(L"tangram_chrome_rt.dll");
				if (hModule) {
					typedef int(__stdcall* _InitApp)(bool bSupportCrashReporting, void*);
					_InitApp _pInitAppFunction;
					_pInitAppFunction = (_InitApp)GetProcAddress(hModule, "InitApp");
					if (_pInitAppFunction != NULL) {
						_pInitAppFunction(bCrashReporting, m_pHubbleImpl->m_pHubbleDelegate);
						return false;
					}
				}
			}
			break;
			case APP_ECLIPSE:
				if (m_pHubble && !m_pHubbleImpl->m_bIsEclipseInit)
				{
					m_pHubble->InitEclipseApp();
					return false;
				}
				break;
			}
		}
		return true;
	}

	bool CTangramAppDelegate::DoIdleWork()
	{
		if (m_pHubbleImpl->m_hMainWnd && ::IsWindow(m_pHubbleImpl->m_hMainWnd) == false)
			m_pHubbleImpl->m_hMainWnd = NULL;
		return false;
	}

	BOOL CTangramAppDelegate::IsBrowserModel(bool bCrashReporting)
	{
		HMODULE hModule = ::GetModuleHandle(L"tangram_chrome_rt.dll");
		if (hModule == nullptr)
			hModule = ::LoadLibrary(L"tangram_chrome_rt.dll");
		if (hModule) {
			m_bCrashReporting = bCrashReporting;
			FuncIsBrowserModel = (_IsBrowserModel)GetProcAddress(hModule, "IsBrowserModel");
			if (FuncIsBrowserModel != NULL) {
				return FuncIsBrowserModel(bCrashReporting, this);
			}
		}

		return false;
	};

	bool CTangramAppDelegate::EclipseAppInit()
	{
		return false;
	}

	void CTangramAppDelegate::IPCMsg(HWND hWnd, CString strType, CString strParam1, CString strParam2)
	{

	}

	void CTangramAppDelegate::CustomizedDOMElement(HWND hWnd, CString strRuleName, CString strHTML)
	{

	}

	void CTangramAppDelegate::OnObserverComplete(HWND hContentLoaderWnd, CString strUrl, IGrid* pRootNode)
	{
	}

	void CTangramAppDelegate::OnHubbleEvent(IHubbleEventObj* NotifyObj)
	{
	}

	CGridProxy* CTangramAppDelegate::OnGridInit(IGrid* pNewNode)
	{
		CComBSTR bstrName("");
		pNewNode->get_Name(&bstrName);
		CComBSTR bstrName2("");
		pNewNode->get_NameAtWindowPage(&bstrName2);
		return nullptr;
	}

	CGalaxyProxy* CTangramAppDelegate::OnContentLoaderCreated(IGalaxy* pNewFrame)
	{
		__int64 h = 0;
		pNewFrame->get_HWND(&h);
		CComBSTR bstrName("");
		pNewFrame->get_Name(&bstrName);
		return nullptr;
	}

	CGalaxyClusterProxy* CTangramAppDelegate::OnGalaxyClusterCreated(IGalaxyCluster* pNewGalaxyCluster)
	{
		CGalaxyClusterProxy* pHubbleGalaxyClusterProxy = nullptr;
		__int64 h = 0;
		pNewGalaxyCluster->get_Handle(&h);
		if (h)
		{
			CWnd* pWnd = CWnd::FromHandlePermanent((HWND)h);
			if (pWnd == NULL)
				return NULL;
			
			if (pWnd->IsKindOf(RUNTIME_CLASS(CTangramMDIChildWnd)))
			{
				pHubbleGalaxyClusterProxy = (CGalaxyClusterProxy*)(CTangramMDIChildWnd*)pWnd;
			}
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CTangramMDIFrameWnd)))
			{
				pHubbleGalaxyClusterProxy = (CGalaxyClusterProxy*)(CTangramMDIFrameWnd*)pWnd;
			}
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CTangramFrameWnd)))
			{
				pHubbleGalaxyClusterProxy = (CGalaxyClusterProxy*)(CTangramFrameWnd*)pWnd;
			}
			if (pHubbleGalaxyClusterProxy)
				pHubbleGalaxyClusterProxy->m_bAutoDelete = false;
		}
		return pHubbleGalaxyClusterProxy;
	}

	IHubbleDoc* CTangramAppDelegate::OpenDocument(void* pDocTemplate, CString strFile, BOOL bNewFrame)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)pDocTemplate;
		if (pTemplate == nullptr)
			return nullptr;
		if (::PathFileExists(strFile))
		{
			pTemplate->OpenDocumentFile(strFile);
		}
		else
		{
			pTemplate->OpenDocumentFile(nullptr);
		}
		return nullptr;
	}

	//IHubbleWindowProvider:
	CString CTangramAppDelegate::GetNames()
	{
		if (m_mapInnerObjInfo.size())
		{
			CString strNames = _T("");
			for (auto it = m_mapInnerObjInfo.begin(); it != m_mapInnerObjInfo.end(); it++)
			{
				strNames += it->first;
				strNames += _T(",");
			}
			return strNames.MakeLower();
		}
		return _T("");
	}

	CString CTangramAppDelegate::GetTags(CString strName)
	{
		strName.Trim().MakeLower();
		if (strName != _T(""))
		{
			auto it = m_mapInnerObjStyle.find(strName);
			if (it != m_mapInnerObjStyle.end())
			{
				return it->second;
			}
		}
		return _T("");
	}

	CTangramComponentApp::CTangramComponentApp()
	{
		g_pAppBase = this;
	}

	CTangramComponentApp::~CTangramComponentApp()
	{
	}

	bool CTangramComponentApp::HubbleInit(CString strID)
	{
		HMODULE hModule = ::GetModuleHandle(_T("universe.dll"));;
		if (hModule)
		{
			if (m_strContainer != _T(""))
			{
				m_strContainer = _T(",") + m_strContainer + _T(",");
				m_strContainer.Replace(_T(",,"), _T(","));
			}
			GetHubbleImpl _pHubbleImplFunction;
			_pHubbleImplFunction = (GetHubbleImpl)GetProcAddress(hModule, "GetHubbleImpl");
			g_pHubbleImpl = _pHubbleImplFunction(&m_pHubble);
			g_pHubble = m_pHubble;
			if (g_pHubbleImpl->m_nAppType == APP_BROWSER_ECLIPSE)
			{
#ifdef _AFXDLL
#ifdef _DEBUG
				::MessageBox(NULL, _T("Chrome-Eclipse Model is not support MFC Share Dll"), _T("Warnning"), MB_OK);
#endif 
				TRACE(_T("\r\n\r\n********Chrome-Eclipse-CLR Mix-Model is not support MFC Share Dll********\r\n\r\n"));
#endif
			}
			strID.Trim();
			if (strID == _T(""))
				strID = _T("ui");
			if (m_strProviderID == _T(""))
			{
				CString strName = g_pAppBase->m_pszAppName;
				m_strProviderID = strName + _T(".") + strID;
			}
			if (m_strProviderID != _T(""))
			{
				m_strProviderID.MakeLower();
				g_pHubbleImpl->InserttoDataMap(1, m_strProviderID, static_cast<IHubbleWindowProvider*>(this));
			}
		}
		return true;
	}

	CString CTangramComponentApp::GetNames()
	{
		if (m_mapInnerObjInfo.size())
		{
			CString strNames = _T("");
			for (auto it = m_mapInnerObjInfo.begin(); it != m_mapInnerObjInfo.end(); it++)
			{
				strNames += it->first;
				strNames += _T(",");
			}
			return strNames.MakeLower();
		}
		return _T("");
	}

	CString CTangramComponentApp::GetTags(CString strName)
	{
		strName.Trim().MakeLower();
		if (strName != _T(""))
		{
			auto it = m_mapInnerObjStyle.find(strName);
			if (it != m_mapInnerObjStyle.end())
			{
				return it->second;
			}
		}
		return _T("");
	}

	int CTangramComponentApp::ExitInstance()
	{
		return CWinApp::ExitInstance();
	}

	HWND CTangramComponentApp::Create(HWND hParentWnd, IGrid* pGrid)
	{
		CWnd* pParent = CWnd::FromHandlePermanent(hParentWnd);
		if (pParent == nullptr)
		{
			pParent = new CTangramHelperWnd();
			if (!pParent->SubclassWindow(hParentWnd))
			{
				TRACE(_T("\n**********************Error**********************\n"));
				return NULL;
			}
		}
		BSTR bstrTag = L"";
		pGrid->get_Attribute(L"startype", &bstrTag);
		USES_CONVERSION;
		CString m_strTag = OLE2T(bstrTag);
		::SysFreeString(bstrTag);
		m_strTag.Trim().MakeLower();
		if (m_strTag != _T(""))
		{
			auto it = m_mapInnerObjInfo.find(m_strTag);
			if (it != m_mapInnerObjInfo.end())
			{
				CRuntimeClass* pCls = (CRuntimeClass*)it->second;
				CWnd* pWnd = (CWnd*)pCls->CreateObject();
				if (pWnd)
				{
					if (pCls->IsDerivedFrom(RUNTIME_CLASS(CFormView)))
					{
						AfxSetResourceHandle(CWinApp::m_hInstance);
					}
					pGrid->get_Attribute(L"style", &bstrTag);
					CString strStyle = OLE2T(bstrTag);
					strStyle.Trim();
					if (strStyle == _T(""))
					{
						::SysFreeString(bstrTag);
						pGrid->get_Attribute(L"tabstyle", &bstrTag);
						strStyle = OLE2T(bstrTag);
						::SysFreeString(bstrTag);
						strStyle.Trim();
					}
					if (pWnd->Create(nullptr, strStyle, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), pParent, 0, nullptr))
					{
						::PostMessage(pWnd->m_hWnd, WM_COSMOSMSG, (WPARAM)pGrid, 20191031);
						pGrid->get_Attribute(L"activepage", &bstrTag);
						CString m_strTag = OLE2T(bstrTag);
						::SysFreeString(bstrTag);
						int nActivePage = _wtoi(m_strTag);
						::PostMessage(pWnd->m_hWnd, WM_TABCHANGE, nActivePage, 0);
						return pWnd->m_hWnd;
					}
				}
			}
		}
		return NULL;
	}

	CTangramWinApp::CTangramWinApp()
	{
		g_pAppBase = g_pApp = this;
	}

	CTangramWinApp::~CTangramWinApp()
	{
	}

	BOOL CTangramWinApp::InitApplication()
	{
		return HubbleInit(_T("")) ? CWinApp::InitApplication() : false;
	}

	BOOL CTangramWinApp::InitInstance()
	{
		InitTangramInstance();
		CWinApp::InitInstance();
		if (ProcessAppType(m_bCrashReporting) == false)
			return false;
		return true;
	}

	HWND CTangramWinApp::Create(HWND hParentWnd, IGrid* pGrid)
	{
		CWnd* pParent = CWnd::FromHandlePermanent(hParentWnd);
		if (pParent == nullptr)
		{
			pParent = new CTangramHelperWnd();
			if (!pParent->SubclassWindow(hParentWnd))
			{
				TRACE(_T("\n**********************Error**********************\n"));
				return NULL;
			}
		}
		BSTR bstrTag = L"";
		pGrid->get_Attribute(L"startype", &bstrTag);
		USES_CONVERSION;
		CString m_strTag = OLE2T(bstrTag);
		::SysFreeString(bstrTag);
		m_strTag.Trim().MakeLower();
		if (m_strTag != _T(""))
		{
			auto it = m_mapInnerObjInfo.find(m_strTag);
			if (it != m_mapInnerObjInfo.end())
			{
				CRuntimeClass* pCls = (CRuntimeClass*)it->second;
				CWnd* pWnd = (CWnd*)pCls->CreateObject();
				if (pWnd)
				{
					if (pCls->IsDerivedFrom(RUNTIME_CLASS(CFormView)))
					{
						AfxSetResourceHandle(CWinApp::m_hInstance);
					}
					pGrid->get_Attribute(L"style", &bstrTag);
					CString strStyle = OLE2T(bstrTag);
					strStyle.Trim();
					if (strStyle == _T(""))
					{
						::SysFreeString(bstrTag);
						pGrid->get_Attribute(L"tabstyle", &bstrTag);
						strStyle = OLE2T(bstrTag);
						::SysFreeString(bstrTag);
						strStyle.Trim();
					}

					if (pWnd->Create(nullptr, strStyle, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), pParent, 0, nullptr))
					{
						::PostMessage(pWnd->m_hWnd, WM_COSMOSMSG, (WPARAM)pGrid, 20191031);
						pGrid->get_Attribute(L"activepage", &bstrTag);
						CString m_strTag = OLE2T(bstrTag);
						::SysFreeString(bstrTag);
						int nActivePage = _wtoi(m_strTag);
						::PostMessage(pWnd->m_hWnd, WM_TABCHANGE, nActivePage, 0);
						return pWnd->m_hWnd;
					}
				}
			}
		}
		return NULL;
	}

	void CTangramWinApp::OnFileNew()
	{
		CWinApp::OnFileNew();
	}

	IMPLEMENT_DYNCREATE(CTangramFrameWnd, CFrameWnd)

	BEGIN_MESSAGE_MAP(CTangramFrameWnd, CFrameWnd)
		ON_WM_CLOSE()
	END_MESSAGE_MAP()

	CTangramFrameWnd::CTangramFrameWnd()
	{
	}

	CTangramFrameWnd::~CTangramFrameWnd()
	{
	}

	BOOL CTangramFrameWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
	{
		if (!CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
		{
			return FALSE;
		}
		return TRUE;
	}

	BOOL CTangramFrameWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, LPCTSTR lpszMenuName, DWORD dwExStyle, CCreateContext* pContext)
	{
		BOOL bCreate = CFrameWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, lpszMenuName, dwExStyle, pContext);
		return bCreate;
	}

	void CTangramFrameWnd::OnClose()
	{
		g_pApp->QueryCanClose(m_hWnd);
		CFrameWnd::OnClose();
	}

	BOOL CTangramFrameWnd::OnCommand(WPARAM wParam, LPARAM lParam)
	{
		//if (m_hClient == nullptr)
		//{
		//	AFXSetTopLevelFrame(this);
		//}
		//return CFrameWndEx::OnCommand(wParam, lParam);
		HWND hWndCtrl = (HWND)lParam;
		UINT nID = LOWORD(wParam);
		//AFXSetTopLevelFrame(this);
		CFrameWnd* pFrameWnd = this;
		ENSURE_VALID(pFrameWnd);
		if (pFrameWnd->m_bHelpMode && hWndCtrl == NULL &&
			nID != ID_HELP && nID != ID_DEFAULT_HELP && nID != ID_CONTEXT_HELP)
		{
			// route as help
			if (!SendMessage(WM_COMMANDHELP, 0, HID_BASE_COMMAND + nID))
				SendMessage(WM_COMMAND, ID_DEFAULT_HELP);
			return TRUE;
		}

		// route as normal command
		return CWnd::OnCommand(wParam, lParam);
	}

	void CTangramFrameWnd::OnTabChange(IGrid* sender, LONG ActivePage, LONG OldPage)
	{
		__int64 h = 0;
		sender->get_Handle(&h);
		IGrid* pActiveNode = nullptr;
		sender->GetGrid(0, ActivePage, &pActiveNode);
		CComBSTR bstrName("");
		pActiveNode->get_Name(&bstrName);
		CComBSTR bstrName2("");
		pActiveNode->get_NameAtWindowPage(&bstrName2);
	}

	void CTangramFrameWnd::OnClrControlCreated(IGrid* Node, IDispatch* Ctrl, CString CtrlName, HWND CtrlHandle)
	{
		CComBSTR bstrName("");
		Node->get_Name(&bstrName);
		CComBSTR bstrName2("");
		Node->get_NameAtWindowPage(&bstrName2);
	}

	void CTangramFrameWnd::OnEvent(IDispatch* sender, IDispatch* EventArg)
	{
	}

	void CTangramFrameWnd::OnControlNotify(IGrid* sender, LONG NotifyCode, LONG CtrlID, HWND CtrlHandle, CString CtrlClassName)
	{
	}

	void CTangramFrameWnd::OnHubbleEvent(IHubbleEventObj* NotifyObj)
	{
	}

	IMPLEMENT_DYNCREATE(CTangramMDIFrameWnd, CMDIFrameWnd)

	BEGIN_MESSAGE_MAP(CTangramMDIFrameWnd, CMDIFrameWnd)
		ON_MESSAGE(WM_QUERYAPPPROXY, OnQueryAppProxy)
	END_MESSAGE_MAP()


	CTangramMDIFrameWnd::CTangramMDIFrameWnd()
	{
	}

	CTangramMDIFrameWnd::~CTangramMDIFrameWnd()
	{
	}

	BOOL CTangramMDIFrameWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
	{
		if (!CMDIFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
		{
			return FALSE;
		}
		return TRUE;
	}

	BOOL CTangramMDIFrameWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, LPCTSTR lpszMenuName, DWORD dwExStyle, CCreateContext* pContext)
	{
		BOOL bCreate = CMDIFrameWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, lpszMenuName, dwExStyle, pContext);
		return bCreate;
	}

	BOOL CTangramMDIFrameWnd::OnCommand(WPARAM wParam, LPARAM lParam)
	{
		//if (m_hClient == nullptr)
		//{
		//	AFXSetTopLevelFrame(this);
		//}
		HWND hWndCtrl = (HWND)lParam;
		UINT nID = LOWORD(wParam);
		//AFXSetTopLevelFrame(this);
		CFrameWnd* pFrameWnd = this;
		ENSURE_VALID(pFrameWnd);
		if (pFrameWnd->m_bHelpMode && hWndCtrl == NULL &&
			nID != ID_HELP && nID != ID_DEFAULT_HELP && nID != ID_CONTEXT_HELP)
		{
			// route as help
			if (!SendMessage(WM_COMMANDHELP, 0, HID_BASE_COMMAND + nID))
				SendMessage(WM_COMMAND, ID_DEFAULT_HELP);
			return TRUE;
		}

		// route as normal command
		return CMDIFrameWnd::OnCommand(wParam, lParam);
	}

	LRESULT CTangramMDIFrameWnd::OnQueryAppProxy(WPARAM wp, LPARAM lp)
	{
		if (lp)
		{
			switch (lp)
			{
			case 1:
				break;
			case 19651965:
				RecalcLayout();
				break;
			case 19631965:
				return (LRESULT)this;
				break;
			case 19631992:
				AfxGetApp()->m_pMainWnd = this;
				break;
			case TANGRAM_CONST_NEWDOC://for new doc
			{
				((CTangramWinApp*)AfxGetApp())->OnFileNew();
			};
			break;
			case TANGRAM_CONST_PANE_FIRST://for mdiclient
			{
				return 1992;
			};
			break;
			case TANGRAM_CONST_OPENFILE://open doc
			{
				if (wp)
				{
					AfxGetApp()->OpenDocumentFile((LPCTSTR)wp);
				}
			};
			break;
			}
		}
		return  0;
	}

	void CTangramMDIFrameWnd::OnTabChange(IGrid* sender, LONG ActivePage, LONG OldPage)
	{
		__int64 h = 0;
		sender->get_Handle(&h);
		IGrid* pActiveNode = nullptr;
		sender->GetGrid(0, ActivePage, &pActiveNode);
		CComBSTR bstrName("");
		pActiveNode->get_Name(&bstrName);
		CComBSTR bstrName2("");
		pActiveNode->get_NameAtWindowPage(&bstrName2);
	}

	void CTangramMDIFrameWnd::OnClrControlCreated(IGrid* Node, IDispatch* Ctrl, CString CtrlName, HWND CtrlHandle)
	{
		CComBSTR bstrName("");
		Node->get_Name(&bstrName);
		CComBSTR bstrName2("");
		Node->get_NameAtWindowPage(&bstrName2);
	}

	void CTangramMDIFrameWnd::OnEvent(IDispatch* sender, IDispatch* EventArg)
	{
	}

	void CTangramMDIFrameWnd::OnControlNotify(IGrid* sender, LONG NotifyCode, LONG CtrlID, HWND CtrlHandle, CString CtrlClassName)
	{
	}

	void CTangramMDIFrameWnd::OnHubbleEvent(IHubbleEventObj* NotifyObj)
	{
	}

	// CTangramXDoc

	IMPLEMENT_DYNCREATE(CTangramXDoc, CDocument)

	BEGIN_MESSAGE_MAP(CTangramXDoc, CDocument)
	END_MESSAGE_MAP()


	// CTangramXDoc construction/destruction

	CTangramXDoc::CTangramXDoc()
	{
		m_bDocLoaded = false;
		m_strXmlData = _T("");
		m_pHubbleTemplate = nullptr;
	}

	CTangramXDoc::~CTangramXDoc()
	{
	}

	BOOL CTangramXDoc::OnNewDocument()
	{
		if (!CDocument::OnNewDocument())
			return FALSE;
		m_strXmlData = _T("");
		m_mapDataMap.RemoveAll();
		m_bDocLoaded = true;
		return TRUE;
	}

	// CTangramXDoc serialization

	void CTangramXDoc::Serialize(CArchive& ar)
	{
		IHubble* pHubble = nullptr;
		CView* pView = nullptr;
		CFrameWnd* pFrame = nullptr;
		POSITION posView = GetFirstViewPosition();
		while (posView != NULL)
		{
			pView = GetNextView(posView);
			pFrame = pView->GetParentFrame();
			if (pFrame)
			{
				IGalaxy* _pContentLoader = nullptr;
				pHubble = ((CTangramWinApp*)AfxGetApp())->m_pHubble;
				break;
			}
		}
		if (ar.IsStoring())
		{
			if (pFrame)
			{
				IGalaxy* _pContentLoader = nullptr;
				pHubble->GetGalaxy((__int64)pView->m_hWnd, &_pContentLoader);
				if (_pContentLoader != nullptr)
				{
					IGalaxyCluster* pGalaxyCluster = nullptr;
					_pContentLoader->get_GalaxyCluster(&pGalaxyCluster);
					CComBSTR bstrXml(L"");
					pGalaxyCluster->get_GalaxyClusterXML(&bstrXml);
					m_strXmlData = OLE2T(bstrXml);
				}
			}
			ar << m_strXmlData;
			m_mapDataMap.Serialize(ar);
		}
		else
		{
			m_strXmlData = _T("");
			m_mapDataMap.RemoveAll();
			ar >> m_strXmlData;
			m_mapDataMap.Serialize(ar);
			pHubble->put_AppKeyValue(CComBSTR(L"currentdocdata"), CComVariant(m_strXmlData));
			m_bDocLoaded = true;
		}
	}

	// CTangramMDIChildWnd
	IMPLEMENT_DYNCREATE(CTangramMDIChildWnd, CMDIChildWnd)

	BEGIN_MESSAGE_MAP(CTangramMDIChildWnd, CMDIChildWnd)
		ON_MESSAGE(WM_COSMOSMSG, OnCosmosMsg)
	END_MESSAGE_MAP()

	// CTangramMDIChildWnd construction/destruction

	CTangramMDIChildWnd::CTangramMDIChildWnd()
	{
		m_pDoc = nullptr;
		m_bAutoDelete = false;
	}

	CTangramMDIChildWnd::~CTangramMDIChildWnd()
	{
	}

	// CTangramMDIChildWnd message handlers

	void CTangramMDIChildWnd::OnTabChange(IGrid* sender, LONG ActivePage, LONG OldPage)
	{
		__int64 h = 0;
		sender->get_Handle(&h);
		IGrid* pActiveNode = nullptr;
		sender->GetGrid(0, ActivePage, &pActiveNode);
		CComBSTR bstrName("");
		pActiveNode->get_Name(&bstrName);
		CComBSTR bstrName2("");
		pActiveNode->get_NameAtWindowPage(&bstrName2);
	}

	void CTangramMDIChildWnd::OnClrControlCreated(IGrid* Node, IDispatch* Ctrl, CString CtrlName, HWND CtrlHandle)
	{
		CComBSTR bstrName("");
		Node->get_Name(&bstrName);
		CComBSTR bstrName2("");
		Node->get_NameAtWindowPage(&bstrName2);
	}

	void CTangramMDIChildWnd::OnEvent(IDispatch* sender, IDispatch* EventArg)
	{
	}

	void CTangramMDIChildWnd::OnControlNotify(IGrid* sender, LONG NotifyCode, LONG CtrlID, HWND CtrlHandle, CString CtrlClassName)
	{
	}

	void CTangramMDIChildWnd::OnHubbleEvent(IHubbleEventObj* NotifyObj)
	{
	}

	BOOL CTangramMDIChildWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext)
	{
		if (pContext)
			m_pDoc = static_cast<CTangramXDoc*>(pContext->m_pCurrentDoc);
		return __super::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
	}

	LRESULT CTangramMDIChildWnd::OnCosmosMsg(WPARAM wp, LPARAM lp)
	{
		switch (lp)
		{
		case TANGRAM_CONST_OPENFILE:
		{
			IHubbleDocTemplate* pDisp = (IHubbleDocTemplate*)wp;
			if (m_pDoc)
			{
				((CTangramXDoc*)m_pDoc)->m_pHubbleTemplate = pDisp;
			}
		}
		break;
		case TANGRAM_CONST_PANE_FIRST:
		{
			if (m_pDoc == nullptr)
			{
				CCreateContext* pContext = (CCreateContext*)wp;
				m_pDoc = static_cast<CTangramXDoc*>(pContext->m_pCurrentDoc);
				CDocTemplate* pTemplate = pContext->m_pNewDocTemplate;
				CString m_strDocID = _T("");
				pTemplate->GetDocString(m_strDocID, CDocTemplate::regFileTypeId);
			}
			return 1992;
		}
		break;
		}
		return __super::DefWindowProc(WM_COSMOSMSG, wp, lp);
	}
}
