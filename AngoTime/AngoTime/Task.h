#pragma once


// CTask �Ի���
#include <list>
#include "Database.h"
#include "afxcmn.h"
#include "TaskSet.h"

using namespace std;

//��������
#define TASK_TYPE_CLOSE		0		//��ʱ�ػ�



typedef struct tagTaskCont
{
	int		nTaskType;		//��������
	CTime	tTime;			//ʱ��
	int		nFreq;			//ִ��Ƶ��
	int		nWeek;			//ÿ��ִ��ʱ����
}TASKCONT;


class CTask : public CDialogEx
{
	DECLARE_DYNAMIC(CTask)

public:
	CTask(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTask();

// �Ի�������
	enum { IDD = IDD_DLG_TASK };

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

	static list<TASKCONT>	m_TaskList;						
	static void				CheckTask(CTime & tTime);		//�ж��Ƿ��ж�ʱ����Ӧ��ִ��

	static void				ClosePC(void *pData);


	CListCtrl				m_lstContent;
	afx_msg void			OnBnClickedButtonAdd();
	afx_msg void			OnBnClickedButtonModi();
	afx_msg void			OnBnClickedButtonDel();
};
