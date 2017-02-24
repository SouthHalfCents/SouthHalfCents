
// AngoDlg.h : 头文件
//

#pragma once


// CAngoDlg 对话框
class CAngoDlg : public CDialogEx
{
// 构造
public:
	CAngoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ANGO_MAIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON					m_hIcon;

	//// 
	virtual BOOL			OnInitDialog();
	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void			OnPaint();
	afx_msg HCURSOR			OnQueryDragIcon();
	afx_msg void			OnDestroy();

	///// 响应菜单
	afx_msg void			OnShowDlg();
	afx_msg void			OnAboutDlg();
	afx_msg void			OnExitDlg();
	afx_msg void			OnConfig();
	
	//// 响应按键
	virtual void			OnOK();							//重载
	virtual void			OnCancel();						//重载
	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	//// 自定义消息
	afx_msg LRESULT			OnNotifyIcon(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()


	HRGN					BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance=NULL);	//加载BMP到显示区域
	BOOL					OnReInitDialog();				//运行中重绘对话框界面


public:
	BOOL					m_bMin;				//是否最小化状态
	NOTIFYICONDATA			m_ntIcon;			//添加通知区域图标

	
};
