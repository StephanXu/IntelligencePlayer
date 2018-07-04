#include "stdafx.h"
#include "StdAfx.h"
#include "http.h"
#include "atlcomcli.h"

#include "direct.h"
#include "io.h"
#include <map>
#include <string>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

using namespace MSXML2;

FunData::FunData()
{
}
FunData::FunData(const FunData &c)
{
	write(c._fdata, c._flen);
}
FunData::~FunData()
{
	if (_fdata)
	{
		free(_fdata);
	}
}

void FunData::ressize(int len, bool isclear)
{
	if (_flen > 0 && _fdata)
	{
		if (_alen < len)
		{
			char *t = (char*)realloc(_fdata, len);
			if (t)
			{
				_fdata = t;
				_flen = _alen = len;
			}
		}
	}
	else
	{
		_flen = _alen = len;
		_fdata = (char*)malloc(len);
	}
	if (isclear && _fdata && _flen > 0)
	{
		ZeroMemory(_fdata, _alen);
	}
}
void FunData::write(char *data, int len)
{
	if (data && len > 0)
	{
		ressize(len);
		memcpy(_fdata, data, len);
	}
}
void FunData::appdata(char *data, int len)
{
	if (data && len > 0)
	{
		int newlen = len + _flen, end = _flen;
		ressize(newlen, false);
		memcpy(_fdata + end, data, len);
	}
}
char* FunData::getdata()
{
	*(_fdata + this->size() - 1) = 0;
	return _fdata;
}
int  FunData::size()
{
	return _flen;
}
//---------------------------------------------------------------------

FunData _temdata;

FunData* sendhttp(LPCSTR url, LPCSTR data, bool method, bool async)
{
	//char* relu = buf;
	int len = 0;
	LPSTR  Vmethod = method ? "POST" : "GET";
	BSTR bstrbody = 0;
	char*ret = NULL;//char* buf=NULL;
	CoInitialize(NULL);
	try
	{
		CXMLSink   xmlsink;
		IXMLHTTPRequestPtr xmlRequest;
		xmlRequest.CreateInstance(__uuidof(XMLHTTP60));
		CComVariant vAsync(async);
		CComVariant vNull(NULL);

		xmlRequest->open(Vmethod, _bstr_t(url), vAsync, vNull, vNull);
		xmlRequest->put_onreadystatechange((IDispatch*)&xmlsink);
		xmlsink.SetEventSource(xmlRequest);
		xmlRequest->setRequestHeader("Content-Type:", "application/x-www-form-urlencoded");

		//xmlRequest->setRequestHeader("text/plain:", "charset=x-user-defined");
		if (method)
		{
			_bstr_t bsdata(data);//use your login name and password
			xmlRequest->send(_variant_t(bsdata));
		}
		else{
			xmlRequest->send();
		}
		if (async)
		{
			MSG   msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
				if (WM_USER_STARTWALKING == msg.message   &&   NULL == msg.hwnd)
				{
					break;
				}
				else
				{
					DispatchMessage(&msg);
				}
			}
		}
		//else
		{
			int state = xmlRequest->readyState;
			if ((xmlRequest->readyState) == 4) //4时表示数据已加载完
			{
				_bstr_t arh = xmlRequest->getAllResponseHeaders();

				xmlRequest->get_responseText(&bstrbody);
				VARIANT st;
				xmlRequest->get_responseBody(&st);
				if (st.vt)
				{
					SAFEARRAY *ds = st.parray;
					len = ds->rgsabound->cElements;
					_temdata.write((char*)ds->pvData, len+1);//复制内容
				}
			}

		}

		xmlRequest.Release();
	}
	catch (_com_error &e)
	{
		//strcpy((char*)buf,(char*)e.Description());
		wstring ej = e.Description();
		//relu=NULL;
	}
	CoInitialize(NULL);
	return &_temdata;
}

FunData sendhttpc(LPCSTR url, LPCSTR data, bool method)
{
	return *sendhttp(url, data, method);
}
//----------------------------------------------------
