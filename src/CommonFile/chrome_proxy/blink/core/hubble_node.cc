﻿#include "hubble.h"
#include "hubble_xobj.h"
#include "hubble_node.h"
#include "hubble_event.h"
#include "hubble_winform.h"
#include "hubble_window.h"
#include "hubble_control.h"
#include "hubble_compositor.h"

#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/html_head_element.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_application_callback.h"

namespace blink {

HubbleNode::HubbleNode(LocalFrame* frame) : DOMWindowClient(frame) {
	m_pParentForm = nullptr;
	m_pRenderframeImpl = nullptr;
	id_ = WTF::CreateCanonicalUUIDString();
}

HubbleNode::~HubbleNode() {
	Hubble* pHubble = hubble_.Get();
	if (pHubble)
	{
		auto it = pHubble->m_mapHubbleNodeforEvent.find(handle_);
		if (it != pHubble->m_mapHubbleNodeforEvent.end())
			pHubble->m_mapHubbleNodeforEvent.erase(it);
		
		auto it2 = pHubble->m_mapHubbleNode.find(handle_);
		if (it2 != pHubble->m_mapHubbleNode.end())
			pHubble->m_mapHubbleNode.erase(it2);
	}
	mapHubbleEventCallback_.clear();
}

HubbleNode* HubbleNode::Create(LocalFrame* frame, const String& strNodeName) {
	return MakeGarbageCollected<HubbleNode>(frame, strNodeName);
}

HubbleNode::HubbleNode(LocalFrame* frame, const String& strNodeName) : DOMWindowClient(frame)
{
	name_ = strNodeName;
}

String HubbleNode::name() {
	return name_;
}

String HubbleNode::getid()
{
	return id_;
}

long HubbleNode::row()
{
	return innerXobj_->getLong(L"row");
}

long HubbleNode::col()
{
	return innerXobj_->getLong(L"col");
}

long HubbleNode::rows()
{
	return innerXobj_->getLong(L"rows");
}

long HubbleNode::cols()
{
	return innerXobj_->getLong(L"cols");
}

HubbleXobj* HubbleNode::xobj()
{
	return innerXobj_;
}

HubbleWinform* HubbleNode::parentForm()
{
	return m_pParentForm;
}

HubbleNode* HubbleNode::getChild(long nIndex)
{
	return getChild(0, nIndex);
}

HubbleNode* HubbleNode::getChild(long row, long col)
{
	int nIndex = col + row* innerXobj_->getLong(L"rows");
	std::wstring strIndex = std::to_wstring(nIndex);
	String strname = strIndex.c_str();
	int64_t nHandle = innerXobj_->getInt64(strname);
	if (nHandle)
	{
		auto it = hubble_->m_mapHubbleNode.find(nHandle);
		if (it != hubble_->m_mapHubbleNode.end())
		{
			return it->value.Get();
		}
	}
	return nullptr;
}

HubbleNode* HubbleNode::getChild(const String& strName)
{
	WebString str = strName;
	auto it = m_mapChildNode2.find(str.Utf16());
	if (it != m_mapChildNode2.end())
		return it->second;
	return nullptr;
}

void HubbleNode::Trace(blink::Visitor* visitor) {
  EventTargetWithInlineData::Trace(visitor);
  ScriptWrappable::Trace(visitor);
  DOMWindowClient::Trace(visitor);
  visitor->Trace(hubble_);
  visitor->Trace(innerXobj_);
  visitor->Trace(m_pParentForm);
  visitor->Trace(mapHubbleEventCallback_);
}

void HubbleNode::addEventListener(const String& eventName, V8ApplicationCallback* callback)
{
	if (callback)
	{
		innerXobj_->addEventListener(eventName, callback);
	}
}

void HubbleNode::addEventListener(const String& subObjName, const String& eventName, V8ApplicationCallback* callback)
{
	if (callback)
	{
		innerXobj_->addEventListener(subObjName, eventName, callback);
	}
}

void HubbleNode::Observe(const String& strKey, const String& xml, V8ApplicationCallback* callback)
{
	if (m_pRenderframeImpl)
	{
		innerXobj_->setStr(L"senderid", id_);
		String callbackid_ = WTF::CreateCanonicalUUIDString();
		innerXobj_->setStr(L"msgID", L"OPEN_XML");
		innerXobj_->setStr(L"open_callbackid", callbackid_);
		innerXobj_->setStr(L"openkey", strKey);
		innerXobj_->setStr(L"openxml", xml);
		WebString strID = callbackid_;
		m_pRenderframeImpl->m_mapHubbleSession[strID.Utf16()] = this;
		if (callback)
		{
			innerXobj_->mapHubbleEventCallback_.insert(callbackid_, callback);
		}
		m_pRenderframeImpl->SendHubbleMessageEx(innerXobj_->session_);
	}
}

void HubbleNode::ObserveEx(const String& strKey, const String& xml, long row, long col, V8ApplicationCallback* callback)
{
	if (m_pRenderframeImpl)
	{
		innerXobj_->setStr(L"senderid", id_);
		innerXobj_->setStr(L"msgID", L"OPEN_XML_SPLITTER");
		innerXobj_->setStr(L"openkey", strKey);
		innerXobj_->setStr(L"openxml", xml);
		innerXobj_->setLong(L"opencol", col);
		innerXobj_->setLong(L"openrow", row);
		String callbackid_ = WTF::CreateCanonicalUUIDString();
		innerXobj_->setStr(L"opencallbackid", callbackid_);
		WebString strID = callbackid_;
		m_pRenderframeImpl->m_mapHubbleSession[strID.Utf16()] = this;
		if (callback)
		{
			innerXobj_->mapHubbleEventCallback_.insert(callbackid_, callback);
		}
		m_pRenderframeImpl->SendHubbleMessageEx(innerXobj_->session_);
	}
}

void HubbleNode::SyncCtrlTextChange(const String& strcontrols, V8ApplicationCallback* callback)
{
	if (callback)
	{
		innerXobj_->setStr(L"eventtype", L"SyncCtrlTextChange");
		innerXobj_->setStr(L"ctrls", strcontrols);
		innerXobj_->addEventListener(L"SyncCtrlTextChange", L"OnTextChanged", callback);
	}
}

void HubbleNode::fireEvent(const String& eventName, HubbleXobj* eventParam)
{
	innerXobj_->fireEvent(eventName, eventParam);
}

void HubbleNode::removeEventListener(const String& eventName)
{
	innerXobj_->removeEventListener(eventName);
}

void HubbleNode::disConnect()
{
	innerXobj_->disConnect();
}

void HubbleNode::sendMessage(HubbleXobj* msg, V8ApplicationCallback* callback)
{
	if (m_pRenderframeImpl)
	{
		if (msg == nullptr)
			msg = innerXobj_;
		msg->setStr(L"senderid", id_);
		String callbackid_ = WTF::CreateCanonicalUUIDString();
		msg->setStr(L"callbackid", callbackid_);
		WebString strID = callbackid_;
		m_pRenderframeImpl->m_mapHubbleSession[strID.Utf16()] = this;
		if (callback)
		{
			mapHubbleEventCallback_.insert(callbackid_, callback);
		}
		m_pRenderframeImpl->SendHubbleMessageEx(msg->session_);
	}
}

void HubbleNode::invokeCallback(wstring callbackid, HubbleXobj* callbackParam)
{
	innerXobj_->invokeCallback(callbackid, callbackParam);
}

void HubbleNode::setControlVal(const String& CtrlID, int64_t CtrlHandle, const String& strVal)
{
	int64_t nHandle = 0;
	if (CtrlID != "")
	{
		Element* const tangramelement = DomWindow()->document()->getElementById(WTF::AtomicString(CtrlID));
		if (tangramelement)
		{
			WTF::AtomicString handle = tangramelement->getAttribute("hwnd");
			if (handle != "")
			{
				WebString webstr = handle;
				std::wstring u16_handle = webstr.Utf16();
				nHandle = _wtoi(u16_handle.c_str());
			}
		}
	}
	else
		nHandle = handle_;
	if (nHandle)
	{
		if (m_pRenderframeImpl) {
			WebString webstr = strVal;
			std::wstring _val = webstr.Utf16();
			webstr = CtrlID;
			std::wstring _strCtrlID = webstr.Utf16();
			m_pRenderframeImpl->SendHubbleMessage(L"TANGRAM_CTRL_VALUE_MESSAGE", _strCtrlID, nHandle, 0, _val, L"");
		}
	}
}

int64_t HubbleNode::hwnd()
{
	return handle_;
}

String HubbleNode::objtype() {
	return innerXobj_->getStr(L"objtype");
}

void HubbleNode::AddedEventListener(const AtomicString& event_type,
                                 RegisteredEventListener& registered_listener) {
  EventTargetWithInlineData::AddedEventListener(event_type,
                                                registered_listener);
}

void HubbleNode::sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5) {
	if (m_pRenderframeImpl) {
		WebString webstr = id;
		std::wstring u16_id = webstr.Utf16();
		webstr = param1;
		std::wstring u16_param1 = webstr.Utf16();
		webstr = param2;
		std::wstring u16_param2 = webstr.Utf16();
		webstr = param3;
		std::wstring u16_param3 = webstr.Utf16();
		webstr = param4;
		std::wstring u16_param4 = webstr.Utf16();
		webstr = param5;
		std::wstring u16_param5 = webstr.Utf16();
		m_pRenderframeImpl->SendHubbleMessage(u16_id, u16_param1, u16_param2, u16_param3, u16_param4, u16_param5);
	}
}

void HubbleNode::GetControlsNames(V8ApplicationCallback* callback)
{

}

int64_t HubbleNode::handle() {
	return handle_;
}

HubbleNode* HubbleNode::parentGrid()
{
	__int64 nHandle = innerXobj_->getInt64(L"parentgridhandle");
	if (nHandle)
	{
		auto it = hubble_->m_mapHubbleNode.find(nHandle);
		if (it != hubble_->m_mapHubbleNode.end())
		{
			return it->value.Get();
		}
	}
	return nullptr;
}

HubbleNode* HubbleNode::root()
{
	__int64 nHandle = innerXobj_->getInt64(L"rootgridhandle");
	if (nHandle)
	{
		auto it = hubble_->m_mapHubbleNode.find(nHandle);
		if (it != hubble_->m_mapHubbleNode.end())
		{
			return it->value.Get();
		}
	}
	return nullptr;
}

HubbleWindow* HubbleNode::parentWindow() {
	__int64 nHandle = innerXobj_->getInt64(L"rootgridhandle");
	auto it = hubble_->m_mapHubbleWindow.find(nHandle);
	if (it != hubble_->m_mapHubbleWindow.end())
		return it->value.Get();
	return nullptr;
}

HubbleNode* HubbleNode::AddChild(int64_t nHandle, const String& strNodeName, blink::Hubble* pHubble)
{
	HubbleNode* node = nullptr;
	auto it = pHubble->m_mapHubbleNode.find(nHandle);
	if (it != pHubble->m_mapHubbleNode.end())
	{
		node = it->value;
		int nSize = m_mapChildNode.size();
		m_mapChildNode[nSize] = node;
	}
	return node;
}

const AtomicString& HubbleNode::InterfaceName() const {
  return event_target_names::kGrid;
}

ExecutionContext* HubbleNode::GetExecutionContext() const {
  return DomWindow()->document();
}

}  // namespace blink
