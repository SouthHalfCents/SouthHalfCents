
// AngoDlg.h : ͷ�ļ�
//

#pragma once


// CAngoDlg �Ի���
class CAngoDlg : public CDialogEx
{
// ����
public:
	CAngoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ANGO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	afx_msg void			OnPaint();
	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HCURSOR			OnQueryDragIcon();

	virtual BOOL			OnInitDialog();
	virtual void			OnOK();							//���أ���ֹ���س��˳�����
	virtual void			OnCancel();						//����
	DECLARE_MESSAGE_MAP()


	HRGN					BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance=NULL);	//����BMP����ʾ����
	BOOL					OnReInitDialog();				//�������ػ�Ի������



};
