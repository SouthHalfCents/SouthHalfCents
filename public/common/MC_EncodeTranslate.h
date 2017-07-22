#ifndef	MC_CEncodeTranslateH
#define MC_CEncodeTranslateH
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
using std::string;
using std::wstring;
//---------------------------------------------------------------------------
class MC_CEncodeTranslate
{
public:
	MC_CEncodeTranslate(void);
	~MC_CEncodeTranslate(void);

public:
	//
	static bool AsciiToUtf8		(const char* vSrc, string& vStrDst);
	static bool Utf8ToAscii		(const char* vSrc, string& vStrDst);

	//
	static bool AsciiToUnicode	(const char* vSrc, wstring& vStrDst);
	static bool UnicodeToAscii	(const wchar_t* vSrc, string& vStrDst);

	//
	static bool UnicodeToUtf8	(const wchar_t* vSrc, string& vStrDst);
	static bool Utf8ToUnicode	(const char* vSrc, wstring& vStrDst);

	static void UTF_8ToUnicode	(wchar_t* pOut,char *pText);
	static void UnicodeToUTF_8	(char* pOut,wchar_t* pText);
	static void UnicodeToGB2312	(char* pOut,wchar_t uData);
	static void Gb2312ToUnicode	(wchar_t* pOut,char *gbBuffer);
	static void GB2312ToUTF_8	(string& pOut, char *pText, int pLen);
	static void UTF_8ToGB2312	(string& pOut, char *pText, int pLen);

	//
	static void UTF82GBK(string strUTF8, string&strGBK );
	static void GBK2UTF8(string strGBK, string& strUTF8);
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------