#include "CommonUniverse.h"

namespace CommonUniverse
{
	CHubbleImpl* g_pHubbleImpl = nullptr;
	IHubbleWindow::IHubbleWindow()
	{
		if(g_pHubbleImpl)
			g_pHubbleImpl->m_pCreatingWindow = this;
	}

	IHubbleWindowProvider::~IHubbleWindowProvider() 
	{
		if (::GetModuleHandle(L"universe") != NULL&&g_pHubbleImpl)
		{
			g_pHubbleImpl->InserttoDataMap(0, m_strProviderID, nullptr);
		}
	}

	bool IHubbleWindowProvider::HubbleInit(CString strID)
	{
		strID.MakeLower().Trim();
		if (strID != _T("") && g_pHubbleImpl == nullptr)
		{
			m_strProviderID = strID;
			HMODULE hModule = ::GetModuleHandle(_T("universe.dll"));
			if (hModule) {
				typedef CHubbleImpl* (__stdcall* GetHubbleImpl)(IHubble**);
				GetHubbleImpl _pHubbleFunction;
				_pHubbleFunction = (GetHubbleImpl)GetProcAddress(hModule, "GetHubbleImpl");
				IHubble* pHubble = nullptr;
				g_pHubbleImpl = _pHubbleFunction(&pHubble);
				m_strProviderID.MakeLower();
				g_pHubbleImpl->InserttoDataMap(1, m_strProviderID, static_cast<IHubbleWindowProvider*>(this));
				return true;
			}
		}
		return false;
	}
}
