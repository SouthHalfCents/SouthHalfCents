#pragma once


// CTask 对话框
#include <list>
#include "Database.h"
#include "afxcmn.h"
#include "TaskSet.h"

using namespace std;

//任务类型
#define TASK_TYPE_CLOSE		0		//定时关机



typedef struct tagTaskCont
{
	int		nTaskType;		//任务类型
	CTime	tTime;			//时间
	int		nFreq;			//执行频率
	int		nWeek;			//每周执行时有用
}TASKCONT;


class CTask : public CDialogEx
{
	DECLARE_DYNAMIC(CTask)

public:
	CTask(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTask();

// 对话框数据
	enum { IDD = IDD_DLG_TASK };

protected:
	virtual void			DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL			OnInitDialog();
	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	CBrush					m_MyBrush;
	HBRUSH					m_MyHBrush;
	afx_msg HBRUSH			OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL			OnEraseBkgnd(CDC* pDC);

public:

	static list<TASKCONT>	m_TaskList;						
	static void				CheckTask(CTime & tTime);		//判断是否有定时任务应被执行

	static void				ClosePC(void *pData);


	CListCtrl				m_lstContent;
	afx_msg void			OnBnClickedButtonAdd();
	afx_msg void			OnBnClickedButtonModi();
	afx_msg void			OnBnClickedButtonDel();
};
