#pragma once
#import "msxml6.dll" 
#include <objbase.h>
#include <OleCtl.h>

#define   ODS   OutputDebugString 
#define   WM_USER_STARTWALKING ((WM_USER)   +   1) 
//using   namespace   MSXML2; 

class CXMLSink : public IDispatch 
{ 
public: 
CXMLSink()   :   m_dwRef(1),m_pEventSource(NULL){ } 
~CXMLSink() 
{ 
if(m_pEventSource) 
m_pEventSource-> Release(); 
m_pEventSource=NULL; 
} 
//   IUnknown   methods 
STDMETHOD(QueryInterface)(REFIID   riid,   LPVOID*   ppv) 
{ 
ODS( L"QueryInterface ");  
*ppv   =   NULL; 
if   (IID_IUnknown   ==   riid) 
{ 
*ppv   =   (LPUNKNOWN)this; 
AddRef(); 
return   NOERROR; 
} 
else   if   (IID_IDispatch   ==   riid) 
{ 
*ppv   =   (IDispatch*)this; 
AddRef(); 
return   NOERROR; 
} 
else 
return   E_NOTIMPL; 
} 

STDMETHOD_(ULONG,   AddRef)(){   return   ++m_dwRef;} 
STDMETHOD_(ULONG,   Release)() 
{ 
if   (--m_dwRef   ==   0) 
{ 
delete   this; 
return   0; 
} 
return   m_dwRef; 
} 
//   IDispatch   method 
STDMETHOD(GetTypeInfoCount)(UINT*   pctinfo) 
{   ODS( L"GetTypeInfoCount\n ");   return   E_NOTIMPL;   } 
STDMETHOD(GetTypeInfo)(UINT   iTInfo, 
LCID   lcid, 
ITypeInfo**   ppTInfo) 
{   ODS( L"GetTypeInfo\n ");   return   E_NOTIMPL;   } 
STDMETHOD(GetIDsOfNames)(REFIID   riid, 
LPOLESTR*   rgszNames, 
UINT   cNames, 
LCID   lcid, 
DISPID*   rgDispId) 
{   ODS( L"GetIDsOfNames\n ");   return   E_NOTIMPL;   }                 
STDMETHOD(Invoke)(DISPID   dispIdMember, 
REFIID   riid, 
LCID   lcid, 
WORD   wFlags, 
DISPPARAMS   __RPC_FAR   *pDispParams, 
VARIANT   __RPC_FAR   *pVarResult, 
EXCEPINFO   __RPC_FAR   *pExcepInfo, 
UINT   __RPC_FAR   *puArgErr) 
{ 
ODS( L"Invoke "); 
MSXML2::IXMLHTTPRequestPtr   xmldom(m_pEventSource); 
if(xmldom) 
{ 
long   lReadyState; 
xmldom-> get_readyState(&lReadyState); 
TCHAR  buf[256];
wsprintf(buf, L"get_readystate   return   %d ",lReadyState); 
ODS(buf); 
if(lReadyState==4) 
{ 
BOOL   fRet   =   PostThreadMessage(GetCurrentThreadId(), 
WM_USER_STARTWALKING, 
(WPARAM)0, 
(LPARAM)0); 
OutputDebugString( L"DISPID_XMLDOMEVENT_ONREADYSTATECHANGE   event   fired "); 
} 
} 
return   NOERROR; 
} 
void   SetEventSource(IUnknown*   punk){   m_pEventSource=punk;if(punk)   punk-> AddRef();} 
protected: 
IUnknown*   m_pEventSource; 
DWORD   m_dwRef; 
}; 
