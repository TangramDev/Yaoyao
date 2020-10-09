/********************************************************************************
*					Open Universe - version 1.0.1.8							*
*********************************************************************************
* Copyright (C) 2002-2020 by Tangram Team.   All Rights Reserved.				*
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
* mailto:tangramteam@outlook.com or mailto:sunhuizlz@yeah.net
* http://www.tangramteam.com/
*
********************************************************************************/

#include "StdAfx.h"
#include "HubbleEvents.h"


_ATL_FUNC_INFO OpenComplete = {CC_STDCALL,VT_EMPTY,3,VT_I4,VT_BSTR,VT_I4|VT_BYREF};
_ATL_FUNC_INFO HubbleNotify = {CC_STDCALL,VT_EMPTY,1,VT_BSTR};
_ATL_FUNC_INFO Initialize= { CC_STDCALL, VT_EMPTY, 2, VT_DISPATCH, VT_BSTR};
_ATL_FUNC_INFO Destroy = { CC_STDCALL, VT_EMPTY, 0, NULL };
_ATL_FUNC_INFO HubbleClose = { CC_STDCALL, VT_EMPTY, 0, NULL };
_ATL_FUNC_INFO TangramEvent = { CC_STDCALL, VT_EMPTY, 1, VT_I4 | VT_BYREF };
_ATL_FUNC_INFO NodeOpenComplete = { CC_STDCALL, VT_EMPTY, 0, NULL };
_ATL_FUNC_INFO GridAddInCreated = {CC_STDCALL,VT_EMPTY,3,VT_DISPATCH,VT_BSTR,VT_BSTR};
_ATL_FUNC_INFO GridAddInsCreated = {CC_STDCALL,VT_EMPTY,0,NULL};
_ATL_FUNC_INFO TabChange= {CC_STDCALL,VT_EMPTY,3,VT_DISPATCH,VT_I4,VT_I4};
_ATL_FUNC_INFO TabChange2= {CC_STDCALL,VT_EMPTY,2,VT_I4,VT_I4};
_ATL_FUNC_INFO MessageHandle = {CC_STDCALL,VT_EMPTY,5,VT_BSTR,VT_BSTR,VT_BSTR,VT_BSTR,VT_BSTR };
_ATL_FUNC_INFO IPCMsg2 = {CC_STDCALL,VT_EMPTY,4,VT_DISPATCH,VT_BSTR,VT_BSTR,VT_BSTR};
