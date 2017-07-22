//---------------------------------------------------------------------------
#include "stdafx.h"
#include "MC_EncodeTranslate.h"
#include <Windows.h>
//---------------------------------------------------------------------------
MC_CEncodeTranslate::MC_CEncodeTranslate(void)
{
}
//---------------------------------------------------------------------------
MC_CEncodeTranslate::~MC_CEncodeTranslate(void)
{
}
//---------------------------------------------------------------------------
bool MC_CEncodeTranslate::AsciiToUtf8(const char* vSrc, string& vStrDst)
{
	if(vSrc == NULL)		return false;

	//��AsciiתΪunicode
	wstring twstr;
	if(true == AsciiToUnicode(vSrc, twstr))
	{
		//��unicodeתΪUTF8
		if(true == UnicodeToUtf8(twstr.c_str(), vStrDst))
		{
			return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------
bool MC_CEncodeTranslate::Utf8ToAscii(const char* vSrc, string& vStrDst)
{
	if(vSrc == NULL)		return false;

	//��UTF8תΪUNICODE
	wstring wstr;
	if(true == Utf8ToUnicode(vSrc, wstr))
	{
		//��UNICODEתΪAscii
		if(true == UnicodeToAscii(wstr.c_str(), vStrDst))
		{
			return true;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
bool MC_CEncodeTranslate::AsciiToUnicode(const char* vSrc, wstring& vStrDst)
{
	if(vSrc == NULL)		return false;

	int tLen = MultiByteToWideChar(CP_ACP, 0, vSrc, -1, NULL, 0);
	if(0 != tLen)
	{
		wchar_t* lpWbuffer = (wchar_t*)malloc(tLen*sizeof(wchar_t));
		if(NULL != lpWbuffer)
		{
			MultiByteToWideChar(CP_ACP, 0, vSrc, -1, lpWbuffer, tLen);
			vStrDst = lpWbuffer;
			free(lpWbuffer);
			return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------
bool MC_CEncodeTranslate::UnicodeToAscii(const wchar_t* vSrc, string& vStrDst)
{
	if(vSrc == NULL)		return false;

	int tLen = WideCharToMultiByte(CP_ACP, 0, vSrc, -1, NULL, 0, NULL, NULL);
	if(0 != tLen)
	{
		char *lpBuffer = (char*)malloc(tLen*sizeof(char));
		if(NULL != lpBuffer)
		{
			WideCharToMultiByte(CP_ACP, 0, vSrc, -1, lpBuffer, tLen, NULL, NULL);
			vStrDst = lpBuffer;
			free(lpBuffer);
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool MC_CEncodeTranslate::UnicodeToUtf8(const wchar_t* vSrc, string& vStrDst)
{
	if(vSrc == NULL)		return false;

	int tLen = WideCharToMultiByte(CP_UTF8, 0, vSrc, -1, NULL, 0, NULL, NULL);
	if(0 != tLen)
	{
		char *lpUtf8 = (char*)malloc(tLen*sizeof(char));
		if(NULL != lpUtf8)
		{
			WideCharToMultiByte(CP_UTF8, 0, vSrc, -1, lpUtf8, tLen, NULL, NULL);
			vStrDst = lpUtf8;
			free(lpUtf8);
			return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------
bool MC_CEncodeTranslate::Utf8ToUnicode(const char* vSrc, wstring& vStrDst)
{
	if(vSrc == NULL)		return false;

	//��UTF8תΪUNICODE
	int tLen = MultiByteToWideChar(CP_UTF8, 0, vSrc, -1, NULL, 0);
	if(0 != tLen)
	{
		wchar_t *lpWbuffer = (wchar_t*)malloc(tLen*sizeof(wchar_t));
		if(NULL != lpWbuffer)
		{
			MultiByteToWideChar(CP_UTF8, 0, vSrc, -1, lpWbuffer, tLen);
			vStrDst = lpWbuffer;
			free(lpWbuffer);
			return true;
		}
	}

	return false;
}


void MC_CEncodeTranslate::UTF_8ToUnicode(wchar_t* pOut,char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

void MC_CEncodeTranslate::UnicodeToUTF_8(char* pOut,wchar_t* pText)
{
	// ע�� WCHAR�ߵ��ֵ�˳��,���ֽ���ǰ�����ֽ��ں�
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));

	return;
}

void MC_CEncodeTranslate::UnicodeToGB2312(char* pOut,wchar_t uData)
{
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL);
	return;
}     

void MC_CEncodeTranslate::Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer)
{
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
	return ;
}

void MC_CEncodeTranslate::GB2312ToUTF_8(string& pOut,char *pText, int pLen)
{
	char buf[4];
	int nLength = pLen* 3;
	char* rst = new char[nLength];

	memset(buf,0,4);
	memset(rst,0,nLength);

	int i = 0;
	int j = 0;      
	while(i < pLen)
	{
		//�����Ӣ��ֱ�Ӹ��ƾͿ���
		if( *(pText + i) >= 0)
		{
			rst[j++] = pText[i++];
		}
		else
		{
			wchar_t pbuffer;
			Gb2312ToUnicode(&pbuffer,pText+i);

			UnicodeToUTF_8(buf,&pbuffer);

			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];    

			j += 3;
			i += 2;
		}
	}
	rst[j] = '\0';

	//���ؽ��
	pOut = rst;
	//URL��UTF-8������,���ո�ת����"%20"
	/*size_t npos=pOut.find(" ");  
	while(npos+1)
	{  
		npos=pOut.find(" ");  
		if(npos==-1)
			break;
		pOut.replace(npos,strlen(" "),"%20");  
	}*/
	delete []rst;   

	return;
}

void MC_CEncodeTranslate::UTF_8ToGB2312(string &pOut, char *pText, int pLen)
{
	char * newBuf = new char[pLen];
	char Ctemp[4];
	memset(Ctemp,0,4);

	int i =0;
	int j = 0;

	while(i < pLen)
	{
		if(pText[i] > 0)
		{
			newBuf[j++] = pText[i++];                       
		}
		else                 
		{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp,pText + i);

			UnicodeToGB2312(Ctemp,Wtemp);

			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	}
	newBuf[j] = '\0';
	pOut = newBuf;
	delete []newBuf;

	return; 
}


void MC_CEncodeTranslate::UTF82GBK(string strUTF8,OUT string&strGBK )
{
	strGBK = "";  

	int nWsize = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);  
	WCHAR * strW = new WCHAR[nWsize + 1];
	strW[nWsize] = 0;
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, strW, nWsize);  
	int nMSize = WideCharToMultiByte(CP_ACP, 0, strW, -1, NULL, 0, NULL, NULL);  
	char * strM = new char[nMSize + 1];  
	strM[nMSize] = 0;
	WideCharToMultiByte(CP_ACP, 0, strW, -1, strM, nMSize, NULL, NULL);  
	strGBK = strM;  
	delete[]strW;  
	delete[]strM;  

}

VOID MC_CEncodeTranslate::GBK2UTF8(string strGBK,OUT string& strUTF8)
{
	strUTF8 = "";  

	int nWsize = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);  
	WCHAR * strW = new WCHAR[nWsize + 1];
	strW[nWsize] = 0;
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, strW, nWsize);  
	int nMSize = WideCharToMultiByte(CP_UTF8, 0, strW, -1, NULL, 0, NULL, NULL);  
	char * strM = new char[nMSize + 1];  
	strM[nMSize] = 0;
	WideCharToMultiByte(CP_UTF8, 0, strW, -1, strM, nMSize, NULL, NULL);  
	strUTF8 = strM;  
	delete[]strW;  
	delete[]strM;  

}