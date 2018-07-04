#pragma once
#include "XMLHttpEventSinkQ.h"
class FunData
{
public:
	FunData();
	FunData(const FunData &c);
	~FunData();
	void ressize(int len, bool isclear = true);
	void write(char *data, int len);
	void appdata(char *data, int len);
	char* getdata();
	int  size();

private:
	char *_fdata = 0;
	int _flen = 0;
	int _alen = 0;
};

enum PATH_N
{
	PATH_DRIVE = 0x01,
	PATH_DIR = 0x02,
	PATH_FNAME = 0x04,
	PATH_EXT = 0x08,
};
#define FNAMEEXTT PATH_FNAME|PATH_EXT

//string sendhttp(LPCSTR Hurl,LPCSTR data,bool method=true,bool async=true);
FunData* sendhttp(LPCSTR url, LPCSTR data = 0, bool method = false, bool async = true);
//data是post模式发送的数据，async=true异步
FunData sendhttpc(LPCSTR url, LPCSTR data = 0, bool method = false);
int writefile(const char* fn, char *buf, int len);
