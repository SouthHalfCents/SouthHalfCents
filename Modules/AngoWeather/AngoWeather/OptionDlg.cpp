// OptionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AngoWeather.h"
#include "OptionDlg.h"
#include "ParseJson.h"
#include <Shlwapi.h>

LPCTSTR szProvince[]=
{
	TEXT("北京市"),TEXT("上海市"),TEXT("天津市"),TEXT("重庆市"),TEXT("吉林省"),TEXT("黑龙江省"),TEXT("辽宁省"),TEXT("内蒙自治区"),TEXT("新疆维吾尔自治区"),
	TEXT("宁夏回族自治区"),TEXT("青海省"),TEXT("甘肃省"),TEXT("陕西省"),TEXT("西藏自治区"),TEXT("四川省"),TEXT("贵州省"),TEXT("云南省"),TEXT("山西省"),
	TEXT("河北省"),TEXT("山东省"),TEXT("河南省"),TEXT("安徽省"),TEXT("江苏省"),TEXT("湖北省"),TEXT("湖南省"),TEXT("江西省"),TEXT("浙江省"),
	TEXT("福建省"),TEXT("台湾省"),TEXT("广西省"),TEXT("广东省"),TEXT("香港特别行政区"),TEXT("澳门")
};

// COptionDlg 对话框

IMPLEMENT_DYNAMIC(COptionDlg, CDialog)

COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDlg::IDD, pParent)
{

}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PROVINCE, m_comProvince);
	DDX_Control(pDX, IDC_COMBO_CITY, m_comCity);
	DDX_Control(pDX, IDC_COMBO_DISTRICT, m_comDistrict);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_PROVINCE, &COptionDlg::OnCbnSelchangeProvince)
	ON_BN_CLICKED(IDOK, &COptionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// COptionDlg 消息处理程序

BOOL COptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int i=0;i<CountArray(szProvince);i++)
	{
		m_comProvince.AddString(szProvince[i]);
	}

	CString strProvince = AfxGetApp()->GetProfileString(TEXT("weather"), TEXT("Province"), TEXT("广东省"));
	CString strCity = AfxGetApp()->GetProfileString(TEXT("weather"), TEXT("City"), TEXT("广州市"));
	CString strDistrict = AfxGetApp()->GetProfileString(TEXT("weather"), TEXT("District"), TEXT("天河区"));


	int nItem = m_comProvince.FindString(0,strProvince);
	nItem = nItem>=0? nItem:0;
	m_comProvince.SetCurSel(nItem);

	SetCityList(nItem);

	m_comDistrict.ResetContent();
	m_comDistrict.AddString(strDistrict);
	m_comDistrict.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void COptionDlg::SetCityList(int nSel)
{
	m_comCity.ResetContent();

	CString strText;
	m_comProvince.GetLBText(nSel,strText);

	CityInfoArray *pCityArray = CParseJson::GetInstance()->GetCityArray();
	CityInfoArray::iterator iter = pCityArray->begin();

	for (;iter != pCityArray->end(); ++iter )
	{
		if ( StrCmp((*iter)->strProvince,strText ) == 0 )
		{
			m_comCity.AddString((*iter)->strTitle);
		}
	}

	m_comCity.SetCurSel(0);
}

void COptionDlg::OnCbnSelchangeProvince()
{
	int nItem = m_comProvince.GetCurSel();
	SetCityList(nItem);
}

void COptionDlg::OnBnClickedOk()
{
	int nItem = 0;
	m_comProvince.GetLBText(m_comProvince.GetCurSel(),m_strProvince);
	m_comCity.GetLBText(m_comCity.GetCurSel(),m_strCity);

	m_comDistrict.GetLBText(m_comDistrict.GetCurSel(),m_strDistrict);

	OnOK();
}

