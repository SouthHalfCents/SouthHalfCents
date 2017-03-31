
#include "StringToolEx.h"


CStringToolEx::CStringToolEx()
{
}


CStringToolEx::~CStringToolEx()
{
}


CStringA CStringToolEx::CStrT2CStrA(const CString &cstrSrc)
{
#ifdef _UNICODE
	return CStrW2CStrA(cstrSrc);
#else
	return cstrSrc;
#endif
}

CStringW CStringToolEx::CStrT2CStrW(const CString &cstrSrc)
{
#ifdef _UNICODE
	return cstrSrc;
#else
	return CStrA2CStrW(cstrSrc);
#endif
}

CString CStringToolEx::CStrA2CStrT(const CStringA &cstrSrc)
{
#ifdef _UNICODE
	return CStrA2CStrW(cstrSrc);
#else
	return cstrSrc;
#endif
}

CString CStringToolEx::CStrW2CStrT(const CStringW &cstrSrc)
{
#ifdef _UNICODE
	return cstrSrc;
#else
	return CStrW2CStrA(cstrSrc);
#endif
}

CStringA CStringToolEx::CStrW2CStrA(const CStringW &cstrSrcW)
{
	int len = WideCharToMultiByte(CP_ACP, 0, LPCTSTR(cstrSrcW), -1, NULL, 0, NULL, NULL);
	char *str = new char[len];
	memset(str, 0, len * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, str, len, NULL, NULL);
	CStringA cstrDestA = str;
	delete str;
	return cstrDestA;
}

CStringW CStringToolEx::CStrA2CStrW(const CStringA &cstrSrcA)
{
	int len = MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len];
	memset(wstr, 0, len * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, wstr, len);
	CStringW cstrDestW = wstr;
	delete wstr;
	return cstrDestW;
}