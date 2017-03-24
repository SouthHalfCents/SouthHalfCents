
// AngoWeatherDlg.h : ͷ�ļ�
//

#pragma once

#include "ParseJson.h"
#include "ServiceThread.h"
#include "HttpWeather.h"
#include "ImageEx.h"
#include "OptionDlg.h"


struct tagWeather
{
	int			m_nTp;
	int			m_nWt;
	int			m_nWd;
	int			m_nWp;
	int			m_nPs;
	int			m_nVb;
	int			m_nHd;
	TCHAR		szCity[64];
};

// CAngoWeatherDlg �Ի���
class CAngoWeatherDlg : public CDialogEx,public CServiceThread
{
// ����
public:
	CAngoWeatherDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CAngoWeatherDlg();
// �Ի�������
	enum { IDD = IDD_ANGOWEATHER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	HICON			m_hIcon;
	virtual BOOL	OnInitDialog();
	afx_msg void	OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	CParseJson*					m_pParseJson;
	CHttpWeather				m_HttpWeather;
	tagWeather					m_Weather;
	COptionDlg					m_OptionDlg;

	CImageEx					m_pImageBack;
	CImageEx					m_ImageWeather;

	CString						m_strProvince;
	CString						m_strCity;

	bool						m_bLoadFinished;

public:
	virtual bool		OnEventThreadRun();

	int					GetValue(CString strData);
	void				DrawText(CDC*pDC, LPCTSTR lpszText, int nPosX, int nPosY, int nFlag = DT_RIGHT, int nSize = 17, int Weight = FW_MEDIUM, COLORREF colText = RGB(255, 255, 255));

	//���´���
	void				UpdateLayered();
	afx_msg void		OnDestroy();
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnBnClickOption();
};