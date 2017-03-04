#pragma once

#include <list>
using namespace std;
// CClock �Ի���


typedef struct tagClockCont
{
	CTime	tTime;			//ʱ��
	int		nFreq;			//ִ��Ƶ��
	int		nWeek;			//ÿ��ִ��ʱ����
}CLOCK_CONT;


class CClock : public CDialogEx
{
	DECLARE_DYNAMIC(CClock)

public:
	CClock(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClock();

// �Ի�������
	enum { IDD = IDD_DLG_CLOCK_CFG };

protected:
	virtual void			DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL			OnInitDialog();
	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	DECLARE_MESSAGE_MAP()

	CBrush					m_MyBrush;
	HBRUSH					m_MyHBrush;
	afx_msg HBRUSH			OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL			OnEraseBkgnd(CDC* pDC);

public:

	static list<CLOCK_CONT>	m_ClcokList;
};
