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

#pragma once

#include <map>
#include "Universe.h"

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include "metahost.h"
#include "CommonUniverse.h"
#include "TangramXmlParse.h"
#include <ppl.h>
#include <ppltasks.h>
#include <agents.h>
#include <shlobj.h>
#include <atlctl.h>
#include <afxcview.h>

using namespace std;
using namespace ATL;
using namespace concurrency;

#pragma comment(lib, "imagehlp.lib")

namespace CommonUniverse
{
	class CHubbleImpl;
	class CTangramXDoc;
	class IUniverseAppProxy;
	class CTangramTabbedPane;

	class CTangramMFCTabCtrl : public CMFCTabCtrl
	{
		DECLARE_DYNCREATE(CTangramMFCTabCtrl)

		// Construction
	public:
		CTangramMFCTabCtrl();
		// Implementation
	public:
		virtual ~CTangramMFCTabCtrl();

	protected:
		void FireChangeActiveTab(int nNewTab);
		BOOL FireChangingActiveTab(int nNewTab);
		DECLARE_MESSAGE_MAP()
	};

	class CTangramMFCToolBar : public CMFCToolBar
	{
		DECLARE_SERIAL(CTangramMFCToolBar);
	public:
		CTangramMFCToolBar();
		virtual ~CTangramMFCToolBar();
	};

	class CHubbleDockablePane : public CDockablePane
	{
		DECLARE_SERIAL(CHubbleDockablePane);
	public:
		CHubbleDockablePane();
		virtual ~CHubbleDockablePane();
	protected:
		afx_msg LRESULT OnShowCtrlBar(WPARAM, LPARAM);
		DECLARE_MESSAGE_MAP()
		CTabbedPane* CreateTabbedPane();
		void ToggleAutoHide();
		BOOL CreateEx(DWORD dwStyleEx, LPCTSTR lpszCaption, CWnd* pParentWnd, const RECT& rect, BOOL bHasGripper,
			UINT nID, DWORD dwStyle, DWORD dwTabbedStyle, DWORD dwControlBarStyle, CCreateContext* pContext);
	};

	class CTangramTabbedPane : public CTabbedPane
	{
		DECLARE_SERIAL(CTangramTabbedPane);
	public:
		CTangramTabbedPane();
		virtual ~CTangramTabbedPane();
	protected:
		DECLARE_MESSAGE_MAP()
		CTabbedPane* CreateTabbedPane();
		void ToggleAutoHide();
		BOOL CreateEx(DWORD dwStyleEx, LPCTSTR lpszCaption, CWnd* pParentWnd, const RECT& rect, BOOL bHasGripper,
			UINT nID, DWORD dwStyle, DWORD dwTabbedStyle, DWORD dwControlBarStyle, CCreateContext* pContext);
	};

	class CTangramAppDelegate :
		public IHubbleDelegate,
		public IUniverseAppProxy,
		public IHubbleWindowProvider
	{
	public:
		CTangramAppDelegate();
		virtual ~CTangramAppDelegate();
		bool m_bCrashReporting = false;
		BOOL IsBrowserModel(bool bCrashReporting);
		bool ProcessAppType(bool bCrashReporting);

		virtual void InitTangramInstance() {};

		//IHubbleDelegate:
		virtual BOOL OnAppIdle(BOOL& bIdle, LONG lCount);
		virtual bool DoIdleWork();
		virtual BOOL IsAppIdleMessage();
		virtual void ProcessMsg(MSG* msg);
		virtual void ForegroundIdleProc();
		virtual CString GetNTPXml();
		virtual bool OnUniversePreTranslateMessage(MSG* pMsg);
		virtual HWND GetMainWnd();
		virtual HWND QueryCanClose(HWND hWnd);
		virtual bool GetClientAreaBounds(HWND hWnd, RECT& rc) { return false; };
		virtual bool HookAppDocTemplateInfo();
		virtual bool EclipseAppInit();
		virtual void IPCMsg(HWND hWnd, CString strType, CString strParam1, CString strParam2);
		virtual void CustomizedDOMElement(HWND hWnd, CString strRuleName, CString strHTML);

		//IUniverseAppProxy:
		virtual void OnHubbleEvent(IHubbleEventObj* NotifyObj);
		virtual void OnObserverComplete(HWND hContentLoaderWnd, CString strUrl, IGrid* pRootNode);
		virtual CGridProxy* OnGridInit(IGrid* pNewNode);
		virtual CGalaxyProxy* OnGalaxyCreated(IGalaxy* pNewFrame);
		virtual CGalaxyClusterProxy* OnGalaxyClusterCreated(IGalaxyCluster* pNewContentLoaderManager);
		virtual IHubbleDoc* OpenDocument(void* pDocTemplate, CString strFile, BOOL bNewFrame);

		//IHubbleWindowProvider:
		virtual bool HubbleInit(CString strID);
		virtual CString GetNames();
		virtual CString GetTags(CString strName);
	};

	// MDI support (zero or more documents)
	class CTangramMultiDocTemplate : public CMultiDocTemplate
	{
		DECLARE_DYNAMIC(CTangramMultiDocTemplate)

		// Constructors
	public:
		CTangramMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
			CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
	};

	// MDT support (zero or more documents)
	class CTangramMDTDocTemplate : public CMultiDocTemplate
	{
		DECLARE_DYNAMIC(CTangramMDTDocTemplate)

		// Constructors
	public:
		CTangramMDTDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
			CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
	};

	// SDI support 
	class CHubbleDocTemplate : public CSingleDocTemplate
	{
		DECLARE_DYNAMIC(CHubbleDocTemplate)

		// Constructors
	public:
		CHubbleDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
			CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
	};

	class CTangramComponentApp :
		public CWinApp,
		public IHubbleWindowProvider
	{
	public:
		CTangramComponentApp();
		virtual ~CTangramComponentApp();
		bool HubbleInit(CString strID);
		IHubble* m_pHubble = nullptr;
		virtual int ExitInstance();
	private:
		virtual CString GetNames();
		virtual CString GetTags(CString strName);
		virtual HWND Create(HWND hParentWnd, IGrid* pGrid);
	};

	class CTangramWinApp :
		public CWinApp,
		public CTangramAppDelegate
	{
	public:
		CTangramWinApp();
		virtual ~CTangramWinApp();

		afx_msg void OnFileNew();

		virtual BOOL InitInstance();
	private:
		virtual BOOL InitApplication();
		virtual HWND Create(HWND hParentWnd, IGrid* pGrid);
		virtual HWND GetActivePopupMenu(HWND hWnd) { return nullptr; };
	};

	class CTangramWinAppEx :
		public CWinAppEx,
		public CTangramAppDelegate
	{
	public:
		CTangramWinAppEx();
		virtual ~CTangramWinAppEx();

		afx_msg void OnFileNew();

		virtual BOOL InitInstance();
	private:
		virtual BOOL InitApplication();
		virtual HWND Create(HWND hParentWnd, IGrid* pGrid);
		virtual bool GetClientAreaBounds(HWND hWnd, RECT& rc);
		virtual HWND GetActivePopupMenu(HWND hWnd) { return nullptr; };
	};

	class CTangramFrameWnd :
		public CFrameWnd,
		public CGalaxyClusterProxy
	{
	public:
		CTangramFrameWnd();
		virtual ~CTangramFrameWnd();

		DECLARE_DYNCREATE(CTangramFrameWnd)

		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = NULL, LPCTSTR lpszMenuName = NULL, DWORD dwExStyle = 0, CCreateContext* pContext = NULL);
		virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
		virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	protected:
		afx_msg void OnClose();
		DECLARE_MESSAGE_MAP()

	private:
		void OnTabChange(IGrid* sender, LONG ActivePage, LONG OldPage);
		void OnClrControlCreated(IGrid* Node, IDispatch* Ctrl, CString CtrlName, HWND CtrlHandle);
		void OnEvent(IDispatch* sender, IDispatch* EventArg);
		void OnControlNotify(IGrid* sender, LONG NotifyCode, LONG CtrlID, HWND CtrlHandle, CString CtrlClassName);
		void OnHubbleEvent(IHubbleEventObj* NotifyObj);
	};

	class CTangramFrameWndEx :
		public CFrameWndEx,
		public CGalaxyClusterProxy
	{
	public:
		CTangramFrameWndEx();
		virtual ~CTangramFrameWndEx();

		static CTangramFrameWndEx* m_pActiveTangramFrameWnd;

		DECLARE_DYNCREATE(CTangramFrameWndEx)

		virtual BOOL OnShowPopupMenu(CMFCPopupMenu* /*pMenuPopup*/);
		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = NULL, LPCTSTR lpszMenuName = NULL, DWORD dwExStyle = 0, CCreateContext* pContext = NULL);
		virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
		virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	protected:
		afx_msg void OnClose();
		DECLARE_MESSAGE_MAP()

	private:
		void OnTabChange(IGrid* sender, LONG ActivePage, LONG OldPage);
		void OnClrControlCreated(IGrid* Node, IDispatch* Ctrl, CString CtrlName, HWND CtrlHandle);
		void OnEvent(IDispatch* sender, IDispatch* EventArg);
		void OnControlNotify(IGrid* sender, LONG NotifyCode, LONG CtrlID, HWND CtrlHandle, CString CtrlClassName);
		void OnHubbleEvent(IHubbleEventObj* NotifyObj);
	};

	class CTangramMDIFrameWnd :
		public CMDIFrameWnd,
		public CGalaxyClusterProxy
	{
	public:
		CTangramMDIFrameWnd();
		virtual ~CTangramMDIFrameWnd();

		DECLARE_DYNCREATE(CTangramMDIFrameWnd)

		virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	protected:
		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = NULL, LPCTSTR lpszMenuName = NULL, DWORD dwExStyle = 0, CCreateContext* pContext = NULL);
		virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnQueryAppProxy(WPARAM wp, LPARAM lp);

		DECLARE_MESSAGE_MAP()

	private:
		void OnTabChange(IGrid* sender, LONG ActivePage, LONG OldPage);
		void OnClrControlCreated(IGrid* Node, IDispatch* Ctrl, CString CtrlName, HWND CtrlHandle);
		void OnEvent(IDispatch* sender, IDispatch* EventArg);
		void OnControlNotify(IGrid* sender, LONG NotifyCode, LONG CtrlID, HWND CtrlHandle, CString CtrlClassName);
		void OnHubbleEvent(IHubbleEventObj* NotifyObj);
	};

	class CTangramMDIChildWnd :
		public CMDIChildWnd,
		public CGalaxyClusterProxy
	{
		DECLARE_DYNCREATE(CTangramMDIChildWnd)
	public:
		CTangramMDIChildWnd();

		// Attributes
		CTangramXDoc* m_pDoc;
		// Implementation
	public:
		virtual ~CTangramMDIChildWnd();

		// Generated message map functions
	protected:
		afx_msg LRESULT OnCosmosMsg(WPARAM wp, LPARAM lp);
		DECLARE_MESSAGE_MAP()

	private:
		void OnTabChange(IGrid* sender, LONG ActivePage, LONG OldPage);
		void OnClrControlCreated(IGrid* Node, IDispatch* Ctrl, CString CtrlName, HWND CtrlHandle);
		void OnEvent(IDispatch* sender, IDispatch* EventArg);
		void OnControlNotify(IGrid* sender, LONG NotifyCode, LONG CtrlID, HWND CtrlHandle, CString CtrlClassName);
		void OnHubbleEvent(IHubbleEventObj* NotifyObj);

		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	};

	class CTangramMDIFrameWndEx :
		public CMDIFrameWndEx,
		public CGalaxyClusterProxy
	{
	public:
		CTangramMDIFrameWndEx();
		virtual ~CTangramMDIFrameWndEx();

		CRect GetClientAreaBounds();
		DECLARE_DYNCREATE(CTangramMDIFrameWndEx)

		virtual void AdjustClientArea();

		virtual BOOL OnShowPopupMenu(CMFCPopupMenu* /*pMenuPopup*/);
		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = NULL, LPCTSTR lpszMenuName = NULL, DWORD dwExStyle = 0, CCreateContext* pContext = NULL);
		virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
		virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnQueryAppProxy(WPARAM wp, LPARAM lp);

	protected:
		DECLARE_MESSAGE_MAP()

	private:
		void OnTabChange(IGrid* sender, LONG ActivePage, LONG OldPage);
		void OnClrControlCreated(IGrid* Node, IDispatch* Ctrl, CString CtrlName, HWND CtrlHandle);
		void OnEvent(IDispatch* sender, IDispatch* EventArg);
		void OnControlNotify(IGrid* sender, LONG NotifyCode, LONG CtrlID, HWND CtrlHandle, CString CtrlClassName);
		void OnHubbleEvent(IHubbleEventObj* NotifyObj);
	};

	class CTangramMDIChildWndEx :
		public CMDIChildWndEx,
		public CGalaxyClusterProxy
	{
		DECLARE_DYNCREATE(CTangramMDIChildWndEx)
	public:
		CTangramMDIChildWndEx();

		// Attributes
		CTangramXDoc* m_pDoc;
		// Implementation
	public:
		virtual ~CTangramMDIChildWndEx();

		// Generated message map functions
	protected:
		afx_msg LRESULT OnCosmosMsg(WPARAM wp, LPARAM lp);
		DECLARE_MESSAGE_MAP()

	private:
		void OnTabChange(IGrid* sender, LONG ActivePage, LONG OldPage);
		void OnClrControlCreated(IGrid* Node, IDispatch* Ctrl, CString CtrlName, HWND CtrlHandle);
		void OnEvent(IDispatch* sender, IDispatch* EventArg);
		void OnControlNotify(IGrid* sender, LONG NotifyCode, LONG CtrlID, HWND CtrlHandle, CString CtrlClassName);
		void OnHubbleEvent(IHubbleEventObj* NotifyObj);

		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	};

	class CTangramXDoc : public CDocument
	{
	protected: // create from serialization only
		CTangramXDoc();
		DECLARE_DYNCREATE(CTangramXDoc)

		// Attributes
	public:
		bool	m_bDocLoaded;
		CString m_strXmlData;
		CMapStringToString m_mapDataMap;
		IHubbleDocTemplate* m_pHubbleTemplate;

		// Overrides
	public:
		virtual BOOL OnNewDocument();
		virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
		virtual void InitializeSearchContent();
		virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

		// Implementation
	public:
		virtual ~CTangramXDoc();

		// Generated message map functions
	protected:
		DECLARE_MESSAGE_MAP()
	};

	#define TGM_NAME				_T("name")
	#define TGM_CAPTION				_T("caption")
	#define TGM_NODE_TYPE			_T("id")
	#define TGM_CNN_ID				_T("cnnid")
	#define TGM_HEIGHT				_T("height")
	#define TGM_WIDTH				_T("width")
	#define TGM_STYLE				_T("style")
	#define TGM_ACTIVE_PAGE			_T("activepage")
	#define TGM_TAG					_T("tag")
	#define TGM_NODE				_T("node")

	#define TGM_ROWS				_T("rows")
	#define TGM_COLS				_T("cols")

	#define TGM_SETTING_HEAD		_T("#$^&TANGRAM")
	#define TGM_SETTING_FOMRAT		_T("#$^&TANGRAM[%ld][%ld]")

	#define TGM_S_EXCEL_INPUT		1
};

using namespace CommonUniverse;
extern IHubble* g_pHubble;
extern CHubbleBrowserFactory* g_pBrowserFactory;
