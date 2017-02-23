
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
	enum { IDD = IDD_ANGO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	afx_msg void			OnPaint();
	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HCURSOR			OnQueryDragIcon();

	virtual BOOL			OnInitDialog();
	virtual void			OnOK();							//重载，防止按回车退出程序
	virtual void			OnCancel();						//重载
	DECLARE_MESSAGE_MAP()


	HRGN					BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance=NULL);	//加载BMP到显示区域
	BOOL					OnReInitDialog();				//运行中重绘对话框界面



};
