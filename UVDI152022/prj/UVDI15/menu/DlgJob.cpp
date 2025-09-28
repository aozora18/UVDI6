
/*
 desc : Gerber Recipe
*/


using namespace std;

#include "pch.h"
#include "../MainApp.h"
#include "DlgJob.h"
#include <map>
#include "./gerb/DrawPrev.h"	/* Gerber Preivew File */
#include "./mark/DrawModel.h"	/* Mark Model Object */
#include "../pops/DlgSped.h"
#include "../mesg/DlgMesg.h"
#include "../../../inc/kybd/DlgKBDT.h"
#include "../../../inc/conf/conf_comn.h"
#include "../GlobalVariables.h"

#include <afxtaskdialog.h>

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgJob::CDlgJob(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	/* �ڽ��� ���̾�α� ID ���� */
	m_enDlgID	= ENG_CMDI::en_menu_gerb;
	
	m_nSelectRecipe[eRECIPE_MODE_SEL] = -1;
	m_nSelectRecipe[eRECIPE_MODE_VIEW] = -1;
	

	//m_nSelectRecipe[eRECIPE_MODE_LOCAL] = -1;

	m_nSelectRecipeOld[eRECIPE_MODE_SEL] = -1;
	m_nSelectRecipeOld[eRECIPE_MODE_VIEW] = -1;
	//m_nSelectRecipeOld[eRECIPE_MODE_LOCAL] = -1;

	m_bIsEditing = FALSE;
}

CDlgJob::~CDlgJob()
{

}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgJob::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* groupbox - normal */
// 	u32StartID	= IDC_JOB_PIC_GERBER_PREVIEW;
// 	for (i=0; i<eJOB_GRP_MAX; i++)				DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* static */
	u32StartID	= IDC_JOB_STT_RECIPE_LIST;
	for (i=0; i<eJOB_TXT_MAX; i++)				DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* btn - normal */
	u32StartID	= IDC_JOB_BTN_RECIPE_ADD;
	for (i=0; i<eJOB_BTN_MAX; i++)				DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);

	u32StartID = IDC_JOB_GRD_RECIPE_LIST;
	for (i=0; i<eJOB_GRD_MAX; i++)				DDX_Control(dx, u32StartID+i,	m_grd_ctl[i]);

	//u32StartID = IDC_JOB_PIC_GLOBAL_MARK;
	//for (i = 0; i < eJOB_PIC_MAX; i++)			DDX_Control(dx, u32StartID + i, m_pic_img[i]);
}

BEGIN_MESSAGE_MAP(CDlgJob, CDlgMenu)
	ON_NOTIFY(NM_CLICK				, IDC_JOB_GRD_RECIPE_LIST		, OnClickGridRecipeList)
	ON_NOTIFY(NM_DBLCLK, IDC_JOB_GRD_RECIPE_LIST , OnDblClickGridRecipeList)
	ON_NOTIFY(NM_CLICK				, IDC_JOB_GRD_JOB_PARAMETER		, OnClickGridJobParameter)
	ON_NOTIFY(NM_CLICK				, IDC_JOB_GRD_EXPOSE_PARAMETER	, OnClickGridExposeParameter)
	ON_NOTIFY(NM_CLICK				, IDC_JOB_GRD_ALIGN_PARAMETER	, OnClickGridAlignParameter)
	ON_NOTIFY(GVN_ENDLABELEDIT		, IDC_JOB_GRD_JOB_PARAMETER		, OnClickGridJobParameterChanged)
	ON_NOTIFY(GVN_ENDLABELEDIT		, IDC_JOB_GRD_EXPOSE_PARAMETER	, OnClickGridExposeParameterChanged)
	ON_NOTIFY(GVN_ENDLABELEDIT		, IDC_JOB_GRD_ALIGN_PARAMETER	, OnClickGridAlignParameterChanged)
	ON_CONTROL_RANGE(BN_CLICKED		, IDC_JOB_BTN_RECIPE_ADD, IDC_JOB_BTN_RECIPE_SAVE, OnBtnClick)
END_MESSAGE_MAP()


/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgJob::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgJob::OnInitDlg()
{
	for (int i=0; i<= EN_RECIPE_TAB::ALIGN; i++)
	{
		m_vChangeList[i].clear();
	}
	
	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	if (!InitGrid())	return FALSE;
	/* ��ü �ʱ�ȭ */
	if (!InitObject())	return FALSE;
	/* �⺻ ���� �� */
	SetBaseData();

	/* MIL DISP ���� */
	InitDispMark();

	/*������ ���Ŷ� ���� ����*/
	//RecipeSelect();

	/*������ ���� ���� �� ����*/
	//RecipeSelect();
	CRecipeManager::GetInstance()->GetLEDFrameRate();
	CRecipeManager::GetInstance()->CalcMarkDist();


	bool isLocalSelect = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF rcp = uvEng_JobRecipe_GetSelectRecipe(isLocalSelect);
	if (rcp)
	{
		int selRecipe = CRecipeManager::GetInstance()->GetRecipeIndex(CString (rcp->job_name));
		if (selRecipe != -1)
			UpdateRecipe(selRecipe);
	}

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgJob::OnExitDlg()
{
	CloseObject();

	DeleteGridParam(EN_RECIPE_TAB::JOB);
	DeleteGridParam(EN_RECIPE_TAB::EXPOSE);
	DeleteGridParam(EN_RECIPE_TAB::ALIGN);

	CloseGrid();
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgJob::OnPaintDlg(CDC *dc)
{
	if (m_pDrawPrev)	m_pDrawPrev->Draw();

	ENG_VCCM enMode = uvEng_Camera_GetCamMode();
	RECT rDraw;

	/* ���� Ȥ�� ���� �ֱٿ� ���õ� ��ũ ���� ��� (����) */
	//for (int i = 0; i < eJOB_PIC_MAX && m_pDrawModel; i++)	m_pDrawModel[i]->DrawModel(dc->m_hDC);
	for (int i = 0; i < eJOB_PIC_MAX && m_pDrawModel; i++) {
		if (m_pDrawModel[i]->GetModelType() != ENG_MMDT::en_image) {
			GetDlgItem(IDC_JOB_PIC_GLOBAL_MARK + i)->ShowWindow(SW_HIDE);
		}
	}
	for (int i = 0; i < eJOB_PIC_MAX && m_pDrawModel; i++) {
		if (m_pDrawModel[i]->GetModelType() != ENG_MMDT::en_image) {
			m_pDrawModel[i]->DrawModel(dc->m_hDC);
		}
		else { // image
			TCHAR* tcharFileName = _T("");
			STG_CMPV stRecipe = { NULL };
			CUniToChar csCnv;

			CFileFind	finder;
			CString		strDir;
			BOOL IsFind;
			int tmpfindType;
			
			strDir.Format(_T("%s\\%s\\mmf\\%s.mmf"), g_tzWorkDir, CUSTOM_DATA_CONFIG,  m_pDrawModel[i]->GetMMFFile());
			IsFind = finder.FindFile(strDir);
			if (IsFind) {
				// CString -> TCHAR * 
				tcharFileName = (TCHAR*)(LPCTSTR)strDir;
				tmpfindType = 0;
			}
			else {
				strDir.Format(_T("%s\\%s\\pat\\%s.pat"), g_tzWorkDir, CUSTOM_DATA_CONFIG, m_pDrawModel[i]->GetPATFile());
				IsFind = finder.FindFile(strDir);
				if (IsFind) {
					// CString -> TCHAR * 
					tcharFileName = (TCHAR*)(LPCTSTR)strDir;
					tmpfindType = 1;
				}
			}
			if (IsFind) {
				GetDlgItem(IDC_JOB_PIC_GLOBAL_MARK + i)->ShowWindow(SW_SHOW);
				/* �̹����� ��µ� ���� ���� */
				GetDlgItem(IDC_JOB_PIC_GLOBAL_MARK + i)->GetWindowRect(&rDraw);
				ScreenToClient(&rDraw);
				uvCmn_Camera_PutMarkDisp(GetDlgItem(IDC_JOB_PIC_GLOBAL_MARK + i)->GetSafeHwnd(), i, rDraw, (int)(i / 2) + 1, tcharFileName, tmpfindType);
			}
			finder.Close();
		}
	}

	/* �ڽ� ���� ȣ�� */
	CDlgMenu::DrawOutLine();
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgJob::OnResizeDlg()
{
}

/*
 desc : �θ� �����忡 ���� �ֱ������� ȣ���
 parm : tick	- [in]  ���� CPU �ð�
		is_busy	- [in]  TRUE: ���� �ó����� ���� �� ..., FALSE: ���� Idle ����
 retn : None
*/
VOID CDlgJob::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
}

/*
 desc : ��Ʈ��
 parm : tick	- [in]  ���� CPU �ð�
		is_busy	- [in]  TRUE: ���� �ó����� ���� �� ..., FALSE: ���� Idle ����
 retn : None
*/
VOID CDlgJob::UpdateControl(UINT64 tick, BOOL is_busy)
{
	BOOL bSelect	= (0 <= m_nSelectRecipe) ? TRUE : FALSE;

	//if (m_btn_ctl[eJOB_BTN_RECIPE_SELECT] && m_btn_ctl[eJOB_BTN_RECIPE_DELETE] && m_btn_ctl[eJOB_BTN_RECIPE_SAVE])
	//{
	//	if (m_btn_ctl[eJOB_BTN_RECIPE_SELECT].IsWindowEnabled() != (bSelect && !is_busy))	m_btn_ctl[eJOB_BTN_RECIPE_SELECT].EnableWindow(bSelect && !is_busy);		/* Recipe Selected */
	//	if (m_btn_ctl[eJOB_BTN_RECIPE_DELETE].IsWindowEnabled() != (bSelect && !is_busy))	m_btn_ctl[eJOB_BTN_RECIPE_DELETE].EnableWindow(bSelect && !is_busy);		/* Recipe Deleted */
	//	if (m_btn_ctl[eJOB_BTN_RECIPE_SAVE].IsWindowEnabled() != (bSelect && !is_busy))		m_btn_ctl[eJOB_BTN_RECIPE_SAVE].EnableWindow(bSelect && !is_busy);		/* Recipe Modified */
	//}

	
	if (m_btn_ctl[eJOB_BTN_RECIPE_DELETE] && m_btn_ctl[eJOB_BTN_RECIPE_DELETE].IsWindowEnabled() != (bSelect && !is_busy && g_loginLevel >= eLOGIN_LEVEL_ENGINEER))
		m_btn_ctl[eJOB_BTN_RECIPE_DELETE].EnableWindow(bSelect && !is_busy && g_loginLevel >= eLOGIN_LEVEL_ENGINEER);		/* Recipe Deleted */

	if (m_btn_ctl[eJOB_BTN_RECIPE_SAVE] && m_btn_ctl[eJOB_BTN_RECIPE_SAVE].IsWindowEnabled() != (bSelect && !is_busy && g_loginLevel >= eLOGIN_LEVEL_ENGINEER))
		m_btn_ctl[eJOB_BTN_RECIPE_SAVE].EnableWindow(bSelect && !is_busy && g_loginLevel >= eLOGIN_LEVEL_ENGINEER);		/* Recipe Modified */
	
	if (m_btn_ctl[eJOB_BTN_RECIPE_ADD] && m_btn_ctl[eJOB_BTN_RECIPE_ADD].IsWindowEnabled() != (g_loginLevel >= eLOGIN_LEVEL_ENGINEER))
		m_btn_ctl[eJOB_BTN_RECIPE_ADD].EnableWindow(g_loginLevel >= eLOGIN_LEVEL_ENGINEER);

	

}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgJob::InitCtrl()
{
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	// by sysandj : Resize UI

	INT32 i = 0;

	/* group box */
	for (i = 0; i < eJOB_GRP_MAX; i++)
	{
		//m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
//		clsResizeUI.ResizeControl(this, GetDlgItem(IDC_JOB_PIC_GERBER_PREVIEW + i));
		// by sysandj : Resize UI
	}
	/* static - normal */
	for (i=0; i< eJOB_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, FORD_GRAPHITE, 0, WHITE_);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}

	/* button - normal */
	for (i = 0; i < eJOB_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_btn_ctl[i].SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	/* static - Picture - Model & Mark (Grabbed) */
	for (i = 0; i < eJOB_PIC_MAX; i++)
	{
		//m_pic_img[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		//m_pic_img[i].SetDrawBg(1);
		//m_pic_img[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		//clsResizeUI.ResizeControl(this, &m_pic_img[i]);
		clsResizeUI.ResizeControl(this, GetDlgItem(IDC_JOB_PIC_GLOBAL_MARK + i));
		// by sysandj : Resize UI
	}
	CRect rtStt;
	m_txt_ctl[eJOB_TXT_GERBER_PREVIEW].GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	CRect rtPreview;
	GetDlgItem(IDC_JOB_PIC_GERBER_PREVIEW)->GetWindowRect(rtPreview);
	this->ScreenToClient(rtPreview);

	int nHeight = (int)(rtPreview.Height() * clsResizeUI.GetRateY());
	rtPreview = rtStt;
	rtPreview.top = rtStt.bottom + 5;
	rtPreview.bottom = rtPreview.top + 5 + nHeight;
	GetDlgItem(IDC_JOB_PIC_GERBER_PREVIEW)->MoveWindow(rtPreview);
	CRect rtOffset;
	m_pDrawPrev = new CDrawPrev(GetDlgItem(IDC_JOB_PIC_GERBER_PREVIEW)->GetSafeHwnd(), rtOffset);
}

/*
 desc : GridControl �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgJob::InitGrid()
{
// 	CRect rtOffset;
// 	/* Gerber Preivew File */
// 	m_pDrawPrev	= new CDrawPrev(GetDlgItem(IDC_JOB_PIC_GERBER_PREVIEW)->GetSafeHwnd(), rtOffset);
// 	ASSERT(m_pDrawPrev);

	InitGridParam(EN_RECIPE_TAB::JOB);
	InitGridParam(EN_RECIPE_TAB::EXPOSE);
	InitGridParam(EN_RECIPE_TAB::ALIGN);
	InitGridRecipeList(FALSE);

	return TRUE;
}

BOOL CDlgJob::InitGridRecipeList(BOOL bUpdate)
{
	CResizeUI					clsResizeUI;
	CRect						rGrid;

	CStringArray				strArrRecipeList;
	CRecipeManager::GetInstance()->GetRecipeList(strArrRecipeList);

	CGridCtrl* pGrid = &m_grd_ctl[eJOB_GRD_RECIPE_LIST];
	pGrid->DeleteAllItems();

	if (FALSE == bUpdate)
	{
		clsResizeUI.ResizeControl(this, pGrid);
	}
	
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	if (0 == strArrRecipeList.GetCount())
	{
		return FALSE;
	}

	std::vector <int>			vRowSize(strArrRecipeList.GetCount());
	std::vector <int>			vColSize(eJOB_GRD_COL_RECIPE_LIST_MAX);

	vColSize[eJOB_GRD_COL_RECIPE_LIST_NO]	= (int)((double)rGrid.Width() * 0.2);
	vColSize[eJOB_GRD_COL_RECIPE_LIST_NAME] = (int)((double)rGrid.Width() * 0.8);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int nTotalHeight = 0;
	int nTotalWidth = 0;

	for (auto& height : vRowSize)
	{
		height = nHeight;
	}

// 	if (rGrid.Height() < nTotalHeight)
// 	{
// 		nTotalHeight = 0;
// 
// 		for (auto& height : vRowSize)
// 		{
// 			height = (rGrid.Height()) / (int)vRowSize.size();
// 			nTotalHeight += height;
// 		}
// 	}

	for (auto& width : vColSize)
	{
		width = width;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth;

	/* ��ü �⺻ ���� */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(WHITE_);
	pGrid->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (ù ��° ��) */
	for (int i = 0; i < (int)vColSize.size(); i++)
	{
		pGrid->SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pGrid->SetColumnWidth(i, vColSize[i]);
	}

	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, vRowSize[nRow]);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			if (0 == nCol)
			{
				pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
			}
			else
			{
				pGrid->SetItemBkColour(nRow, nCol, WHITE_);
			}
			
			pGrid->SetItemFgColour(nRow, nCol, BLACK_);

			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		pGrid->SetItemTextFmt(nRow, 0, _T("%d"), nRow + 1);
		pGrid->SetItemText(nRow, 1, strArrRecipeList.GetAt(nRow));
	}


	
	
	int nSelRecipe = CRecipeManager::GetInstance()->GetSelectRecipeIndex(EN_RECIPE_MODE::eRECIPE_MODE_LOCAL);
	if (-1 != nSelRecipe)
		SelectRecipe(nSelRecipe, eRECIPE_MODE_LOCAL);

	nSelRecipe = CRecipeManager::GetInstance()->GetSelectRecipeIndex(EN_RECIPE_MODE::eRECIPE_MODE_SEL);
	if (-1 != nSelRecipe)
		SelectRecipe(nSelRecipe, eRECIPE_MODE_SEL);

	/* �⺻ �Ӽ� �� ���� */
	pGrid->SetColumnResize(FALSE);
	pGrid->SetRowResize(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->SetEditable(FALSE);
	pGrid->Refresh();

	return TRUE;
}

void CDlgJob::DeleteGridParam(int nRecipeTab)
{
	CArrGrdParam* pArrGrdParam = NULL;
	ST_GRD_PARAM* pStGrdPram = NULL;

	for (int nCntPage = 0; nCntPage < m_arrGrdPage[nRecipeTab].GetCount(); nCntPage++)
	{
		pArrGrdParam = m_arrGrdPage[nRecipeTab].GetAt(nCntPage);
		for (int nCntParam = 0; nCntParam < pArrGrdParam->GetCount(); nCntParam++)
		{
			pStGrdPram = pArrGrdParam->GetAt(nCntParam);
			delete pStGrdPram;
		}

		pArrGrdParam->RemoveAll();
		delete pArrGrdParam;
	}

	m_arrGrdPage[nRecipeTab].RemoveAll();

	m_grd_ctl[eJOB_GRD_JOB_PARAMETER + nRecipeTab].ReleaseMergeCells();
	m_grd_ctl[eJOB_GRD_JOB_PARAMETER + nRecipeTab].DeleteAllItems();
}

void CDlgJob::InitGridParam(int nRecipeTab)
{
	DeleteGridParam(nRecipeTab);

	BOOL bPageSingle = TRUE;

	CGridCtrl*	pGrid = &m_grd_ctl[eJOB_GRD_JOB_PARAMETER + nRecipeTab];
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int	nHeight			= (int)((double)DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int	nMaxRowDisplay	= rGrid.Height() / nHeight;

	int nMaxParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParamCount(nRecipeTab);
	CString strGroupOld = _T("");
	//�Ķ���� �迭
	CArrGrdParam* pArrGrdParamTemp = NULL;
	//////////////////////////////////////////////////////////////////////////
	//�׷����� ���� : �׷�- �Ķ��迭�� ������ �ܰ�
	//�ӽ÷� �׷캰�� ����
	CArrGrdPage arrGrdGroup;
	for (int nCntParam = 0; nCntParam < nMaxParam; nCntParam++)
	{
		ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(nRecipeTab, nCntParam);
		if (FALSE == stParam.bShow)
			continue;

		//��������ʴ� �Ķ���ʹ� �н�
		if (FALSE == stParam.bUse)
			continue;

		if (strGroupOld != stParam.strGroup)
		{
			//���Ķ���� Ÿ��Ʋ��
			ST_GRD_PARAM* pStGrdParam = new ST_GRD_PARAM();
			pStGrdParam->bIsGroup = TRUE;
			pStGrdParam->nIndexParam = nCntParam;
			//�Ķ���� �迭
			pArrGrdParamTemp = new CArrGrdParam();
			pArrGrdParamTemp->Add(pStGrdParam);
			//�׷� �迭			
			arrGrdGroup.Add(pArrGrdParamTemp);

			strGroupOld = stParam.strGroup;
		}

		//���� �Ķ���� ��Ī
		ST_GRD_PARAM* pStGrdParam = new ST_GRD_PARAM();
		pStGrdParam->bIsGroup = FALSE;
		pStGrdParam->nIndexParam = nCntParam;
		//���� �Ķ���� �ֱ�
		pArrGrdParamTemp->Add(pStGrdParam);
	}
	//////////////////////////////////////////////////////////////////////////
	//�������� ���� : ������ - �Ķ��迭�� ���� �Ϸ�
	//Param Page�� 2ĭ View�� 1ĭ 
	if (FALSE == bPageSingle)
	{
		CArrGrdParam* pArrGrdParam = NULL;
		int nCntParamInPage = 0;
		for (int nCntGroup = 0; nCntGroup < arrGrdGroup.GetCount(); nCntGroup++)
		{
			pArrGrdParamTemp = arrGrdGroup.GetAt(nCntGroup);
			int nCntCurGroup = (int)pArrGrdParamTemp->GetCount();
			//0�϶� ������ ���� ����

			if (0 == nCntParamInPage || nCntParamInPage + nCntCurGroup > nMaxRowDisplay)
			{
				//
				pArrGrdParam = new CArrGrdParam();

				for (int nCntParam = 0; nCntParam < nCntCurGroup; nCntParam++)
				{
					pArrGrdParam->Add(pArrGrdParamTemp->GetAt(nCntParam));
				}
				//�������� �׷��߰�
				m_arrGrdPage[nRecipeTab].Add(pArrGrdParam);
				if (nCntCurGroup > nMaxRowDisplay) //�ѱ׷츸���ε� ���������� ������, �ݺ����� �������� ���� �������� ����� �ֵ���
				{
					nCntParamInPage = 0;
				}
				else
				{
					nCntParamInPage = 0;
					nCntParamInPage += nCntCurGroup;
				}

			}
			else if (nCntParamInPage + nCntCurGroup <= nMaxRowDisplay)
			{
				for (int nCntParam = 0; nCntParam < nCntCurGroup; nCntParam++)
				{
					pArrGrdParam->Add(pArrGrdParamTemp->GetAt(nCntParam));
				}
				nCntParamInPage += nCntCurGroup;
			}
		}
	}
	else //Param Page�� 1ĭ View�� 2ĭ 
	{
		CArrGrdParam* pArrGrdParam = NULL;
		int nCntParamInPage = 0;
		pArrGrdParam = new CArrGrdParam();
		m_arrGrdPage[nRecipeTab].Add(pArrGrdParam);

		for (int nCntGroup = 0; nCntGroup < arrGrdGroup.GetCount(); nCntGroup++)
		{
			pArrGrdParamTemp = arrGrdGroup.GetAt(nCntGroup);
			int nCntCurGroup = (int)pArrGrdParamTemp->GetCount();

			for (int nCntParam = 0; nCntParam < nCntCurGroup; nCntParam++)
			{
				pArrGrdParam->Add(pArrGrdParamTemp->GetAt(nCntParam));
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//���� ����
	for (int nCntGroup = 0; nCntGroup < arrGrdGroup.GetCount(); nCntGroup++)
	{
		pArrGrdParamTemp = arrGrdGroup.GetAt(nCntGroup);
		delete pArrGrdParamTemp;
	}
	arrGrdGroup.RemoveAll();
	UpdateGridParam(nRecipeTab);
}

void CDlgJob::UpdateGridParam(int nRecipeTab)
{
	//////////////////////////////////////////////////////////////////////////
	//UI 	
	CGridCtrl* pGrid = &m_grd_ctl[eJOB_GRD_JOB_PARAMETER + nRecipeTab];
	CResizeUI	clsResizeUI;
	CRect		rGrid;

	//Col ������ �� ������ ��Ʈ ����
	pGrid->GetWindowRect(&rGrid);
	ScreenToClient(rGrid);

	int nMaxCol			= eJOB_GRD_COL_PARAMETER_MAX;
	int nMaxRow			= 0;
	int nHeightRow		= (int)((double)DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int nMaxRowDisplay	= rGrid.Height() / nHeightRow;

	int nWidthID		= 0;
	int nWidthParameter = (int)((double)rGrid.Width() * 0.5);
	int nWidthValue		= (int)((double)rGrid.Width() * 0.3);
	int nWidthUnit		= (int)((double)rGrid.Width() * 0.2);

	nWidthValue += rGrid.Width() - (nWidthParameter + nWidthValue + nWidthUnit + 1);

	int nWidth[eJOB_GRD_COL_PARAMETER_MAX] = { nWidthID, nWidthParameter, nWidthValue, nWidthUnit };

	UINT nForamt = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
	//////////////////////////////////////////////////////////////////////////

	CArrGrdParam* pArrGrdParam = NULL;
	ST_GRD_PARAM* pStGrdPram = NULL;

	//�ʱ�ȭ �ʿ�
	nWidth[eJOB_GRD_COL_PARAMETER_UNIT]		= nWidthUnit;

	//MaxRow �Ǵ�		
	pArrGrdParam	= m_arrGrdPage[nRecipeTab].GetAt(0);
	nMaxRow			= (int)pArrGrdParam->GetCount();

	//UI�ʱ�ȭ
	pGrid->ReleaseMergeCells();
	pGrid->DeleteAllItems();
	pGrid->SetRowCount(nMaxRow);
	pGrid->SetColumnCount(nMaxCol);
	pGrid->SetFixedRowCount(1);


	if (nMaxRow > nMaxRowDisplay)
	{
		nWidth[eJOB_GRD_COL_PARAMETER_UNIT] = nWidthUnit - ::GetSystemMetrics(SM_CXVSCROLL);
	}
	else
	{
		rGrid.bottom = rGrid.top + nHeightRow * nMaxRow + 1;
		pGrid->MoveWindow(rGrid);
	}

	for (int nCol = 0; nCol < nMaxCol; nCol++)
	{
		pGrid->SetColumnWidth(nCol, nWidth[nCol]);
	}

	for (int nRow = 0; nRow < pArrGrdParam->GetCount(); nRow++)
	{
		pStGrdPram = pArrGrdParam->GetAt(nRow);
		ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(nRecipeTab, pStGrdPram->nIndexParam);

		for (int nCol = 0; nCol < nMaxCol; nCol++)
		{
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
		}

		if (TRUE == pStGrdPram->bIsGroup)
		{
			pGrid->MergeCells(CCellRange(nRow, eJOB_GRD_COL_PARAMETER_ID, nRow, eJOB_GRD_COL_PARAMETER_UNIT));
			pGrid->SetItemText(nRow, 0, stParam.strGroup);
			pGrid->SetItemFormat(nRow, 0, nForamt);
			pGrid->SetItemBkColour(nRow, 0, DEF_COLOR_BTN_MENU_NORMAL);
			pGrid->SetItemFgColour(nRow, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		}
		else
		{
			//���� ���� for ��
			for (int nCol = 0; nCol < nMaxCol; nCol++)
			{
				pGrid->SetItemFormat(nRow, nCol, nForamt);
				if (eJOB_GRD_COL_PARAMETER_VALUE != nCol) //�Ķ���� �Է¶��� ������� ���
				{
					pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
				}
			}
			//���÷��� ����
			pGrid->SetItemTextFmt(nRow, eJOB_GRD_COL_PARAMETER_ID, _T("%d"), stParam.nIndexParam);
			pGrid->SetItemText(nRow, eJOB_GRD_COL_PARAMETER_NAME, stParam.strDisplayName);
			pGrid->SetItemTextDes(nRow, eJOB_GRD_COL_PARAMETER_NAME, stParam.strDescript);
			pGrid->SetItemText(nRow, eJOB_GRD_COL_PARAMETER_UNIT, stParam.strUnit);
			
			if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
			{
				pGrid->SetCellType(nRow, eJOB_GRD_COL_PARAMETER_VALUE, RUNTIME_CLASS(CGridCellCombo));
				
				if (stParam.strName == _T("SERIAL_FLIP_HORZ") ||
					stParam.strName == _T("SCALE_FLIP_HORZ") ||
					stParam.strName == _T("TEXT_FLIP_HORZ"))
				{
					CStringArray options;
					options.Add(_T("Up"));
					options.Add(_T("Down"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, eJOB_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, eJOB_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt())
					{
						pComboCell->SetText(_T("Up"));
					}
					else
					{
						pComboCell->SetText(_T("Down"));
					}
				}
				else if (stParam.strName == _T("SERIAL_FLIP_VERT") ||
					stParam.strName == _T("SCALE_FLIP_VERT") ||
					stParam.strName == _T("TEXT_FLIP_VERT"))
				{
					CStringArray options;
					options.Add(_T("Left"));
					options.Add(_T("Right"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, eJOB_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, eJOB_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt())
					{
						pComboCell->SetText(_T("Left"));
					}
					else
					{
						pComboCell->SetText(_T("Right"));
					}
				}
				else if (stParam.strName == _T("MARK_TYPE"))
				{
					CStringArray options;
					options.Add(_T("Geomatrix"));
					options.Add(_T("Pattern Image"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, eJOB_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, eJOB_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt())
					{
						pComboCell->SetText(_T("Geomatrix"));
					}
					else
					{
						pComboCell->SetText(_T("Pattern Image"));
					}
				}
				else if (stParam.strName == _T("LAMP_TYPE"))
				{
					CStringArray options;
					options.Add(_T("Ring"));
					options.Add(_T("Coaxial"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, eJOB_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, eJOB_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt())
					{
						pComboCell->SetText(_T("Ring"));
					}
					else
					{
						pComboCell->SetText(_T("Coaxial"));
					}
				}
				else
				{
					CStringArray options;
					options.Add(_T("NO"));
					options.Add(_T("YES"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, eJOB_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, eJOB_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt())
					{
						pComboCell->SetText(_T("NO"));
					}
					else
					{
						pComboCell->SetText(_T("YES"));
					}
				}
			}
			else if (DEF_DATA_TYPE_INT == stParam.strDataType)
			{

				if (stParam.strName == _T("SEARCH_TYPE"))
				{
					pGrid->SetCellType(nRow, eJOB_GRD_COL_PARAMETER_VALUE, RUNTIME_CLASS(CGridCellCombo));

					CStringArray options;
					options.Add(_T("Single"));
//					options.Add(_T("Cent_side"));
					options.Add(_T("Multi_only"));
					//options.Add(_T("Ring_cirecle"));
					//options.Add(_T("Ph_step"));

					CGridCellCombo* pComboCell1 = (CGridCellCombo*)pGrid->GetCell(nRow, eJOB_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, eJOB_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell1->SetOptions(options);
					pComboCell1->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt() || 1 == stParam.GetInt())
					{
						pComboCell1->SetText(_T("Single"));
					}
					/*else if (2 == stParam.GetInt())
					{
						pComboCell1->SetText(_T("Cent_side"));
					}*/
					else if (2 == stParam.GetInt())
					{
						pComboCell1->SetText(_T("Multi_only"));
					}
					/*else if (4 == stParam.GetInt())
					{
						pComboCell1->SetText(_T("Ring_cirecle"));
					}
					else if (5 == stParam.GetInt())
					{
						pComboCell1->SetText(_T("Ph_step"));
					}*/
				}
				else if (stParam.strName == _T("ALIGN_MOTION"))
				{
					pGrid->SetCellType(nRow, eJOB_GRD_COL_PARAMETER_VALUE, RUNTIME_CLASS(CGridCellCombo));


					CStringArray options;
					auto nameTag = GlobalVariables::GetInstance()->nameTag;

					for (const auto& pair : nameTag) 
					{
						options.Add(CString(pair.second.c_str()));
					}

					CGridCellCombo* pComboCell2 = (CGridCellCombo*)pGrid->GetCell(nRow, eJOB_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, eJOB_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell2->SetOptions(options);
					pComboCell2->SetStyle(CBS_DROPDOWN);
					try
					{
						auto value = nameTag.find(stParam.GetInt());
						pComboCell2->SetText(CString(value != nameTag.end() ? value->second.c_str() : "error"));
					}
					catch(exception e)
					{
						pComboCell2->SetText(CString(nameTag.at(0).c_str()));
					}

				}
				else if (stParam.strName == _T("MATERIAL_TYPE"))
				{
					pGrid->SetCellType(nRow, eJOB_GRD_COL_PARAMETER_VALUE, RUNTIME_CLASS(CGridCellCombo));

					auto& offsets = uvEng_GetConfig()->headOffsets.GetOffsets();

					CStringArray options;
					USES_CONVERSION;
					for_each(offsets.begin(), offsets.end(), [&](Headoffset& v)
						{
							options.Add(CString(A2W(v.offsetName)));
						});


					CGridCellCombo* pComboCell2 = (CGridCellCombo*)pGrid->GetCell(nRow, eJOB_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, eJOB_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell2->SetOptions(options);
					pComboCell2->SetStyle(CBS_DROPDOWN);

					Headoffset offset;

					
					if (uvEng_GetConfig()->headOffsets.GetOffsets(stParam.GetInt(), offset))
						pComboCell2->SetText(A2W((char*)offset.offsetName));

				}
				else if (stParam.strName == _T("ALIGN_TYPE"))
				{
					pGrid->SetCellType(nRow, eJOB_GRD_COL_PARAMETER_VALUE, RUNTIME_CLASS(CGridCellCombo));



					CStringArray options;
					options.Add(_T("Global 0 Local 0"));	/*Global(0) points / Local Division(0 x 0) (00) points */
					options.Add(_T("Global 4 Local 0"));	/* Global (4) points / Local Division (0 x 0) (00) points */
					options.Add(_T("Global 4 Local n"));	/* Global (4) points / Local Division (2 x 1) (08) points */
					
					//options.Add(_T("Global 4 Local 2_2"));	/* Global (4) points / Local Division (2 x 2) (16) points */
					//options.Add(_T("Global 4 Local 3_2"));	/* Global (4) points / Local Division (3 x 2) (24) points */
					//options.Add(_T("Global 4 Local 4_2"));	/* Global (4) points / Local Division (4 x 2) (32) points */
					//options.Add(_T("Global 4 Local 5_2"));	/* Global (4) points / Local Division (5 x 2) (40) points */

					CGridCellCombo* pComboCell2 = (CGridCellCombo*)pGrid->GetCell(nRow, eJOB_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, eJOB_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell2->SetOptions(options);
					pComboCell2->SetStyle(CBS_DROPDOWN);

					if (0x00 == stParam.GetInt())
					{
						pComboCell2->SetText(_T("Global 0 Local 0"));
					}
					else if (0x04 == stParam.GetInt())
					{
						pComboCell2->SetText(_T("Global 4 Local 0"));
					}
					else
					{
						pComboCell2->SetText(_T("Global 4 Local n"));
					}

					//else if (0x11 == stParam.GetInt())
					//{
					//	pComboCell2->SetText(_T("Global 4 Local 2_1"));
					//}
					//else if (0x21 == stParam.GetInt())
					//{
					//	pComboCell2->SetText(_T("Global 4 Local 2_2"));
					//}
					//else if (0x22 == stParam.GetInt())
					//{
					//	pComboCell2->SetText(_T("Global 4 Local 2_3"));
					//}
					//else if (0x23 == stParam.GetInt())
					//{
					//	pComboCell2->SetText(_T("Global 4 Local 2_4"));
					//}
					//else if (0x24 == stParam.GetInt())
					//{
					//	pComboCell2->SetText(_T("Global 4 Local 2_5"));
					//}
				}
				else
				{
					CString strTemp = stParam.GetScaledValue();
					pGrid->SetItemText(nRow, eJOB_GRD_COL_PARAMETER_VALUE, strTemp);
				}
			}
			else if (DEF_DATA_TYPE_LOAD == stParam.strDataType)
			{
				CString strTemp = stParam.GetScaledValue();
				pGrid->SetItemText(nRow, eJOB_GRD_COL_PARAMETER_VALUE, strTemp);
				pGrid->SetItemText(nRow, eJOB_GRD_COL_PARAMETER_UNIT, stParam.strDataType);
				pGrid->SetItemBkColour(nRow, eJOB_GRD_COL_PARAMETER_UNIT, DEF_COLOR_BTN_PAGE_NORMAL);
				pGrid->SetItemFgColour(nRow, eJOB_GRD_COL_PARAMETER_UNIT, DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
			}
			else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
			{
				CString strTemp = stParam.GetDefaultValue();
				CStringArray strArrData;
				CString strData;
				while (AfxExtractSubString(strData, strTemp, (int)strArrData.GetCount(), ';'))
				{
					strArrData.Add(strData);
				}
				pGrid->SetCellType(nRow, eJOB_GRD_COL_PARAMETER_VALUE, RUNTIME_CLASS(CGridCellCombo));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, eJOB_GRD_COL_PARAMETER_VALUE);

				pGrid->SetListMode(FALSE);
				pGrid->EnableDragAndDrop(TRUE);
				pGrid->SetItemState(nRow, eJOB_GRD_COL_PARAMETER_VALUE, 0x00);

				pComboCell->SetOptions(strArrData);
				pComboCell->SetStyle(CBS_DROPDOWN);

				BOOL bIsExist = FALSE;
				for (int i = 0; i < strArrData.GetCount(); i++)
				{
					if (0 == strArrData.GetAt(i).Compare(stParam.GetValue()))
					{
						bIsExist = TRUE;
						break;
					}
				}

				if (bIsExist)
				{
					pComboCell->SetText(stParam.GetValue());
				}
				else
				{
					pComboCell->SetText(strArrData.GetAt(0));
				}
			}
			else if (DEF_DATA_TYPE_STRING == stParam.strDataType)
			{
				pGrid->MergeCells(CCellRange(nRow, eJOB_GRD_COL_PARAMETER_VALUE, nRow, eJOB_GRD_COL_PARAMETER_UNIT));
				CString strTemp = stParam.GetScaledValue();
				pGrid->SetItemText(nRow, eJOB_GRD_COL_PARAMETER_VALUE, strTemp);
			}
			else
			{
				CString strTemp = stParam.GetScaledValue();
				pGrid->SetItemText(nRow, eJOB_GRD_COL_PARAMETER_VALUE, strTemp);
			}
		}
		pGrid->SetRowHeight(nRow, nHeightRow);
	}
	pGrid->SetColumnResize(FALSE);
	pGrid->SetRowResize(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->SetEditable(m_bEnableEdit);
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->Refresh();
}

/*
 desc : GridControl ����
 parm : None
 retn : None
*/
VOID CDlgJob::CloseGrid()
{
	if (m_pDrawPrev)	delete m_pDrawPrev;
}

/*
 desc : ��ü �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgJob::InitObject()
{
	UINT32 i = 0;
	RECT rDraw;

	/* ��ũ �˻� ��� ��� ��ü */
	m_pDrawModel = new CDrawModel * [eJOB_PIC_MAX];
	ASSERT(m_pDrawModel);
	for (i = 0; i < eJOB_PIC_MAX; i++)
	{
		//m_pic_img[i].GetWindowRect(&rDraw);
		GetDlgItem(IDC_JOB_PIC_GLOBAL_MARK + i)->GetWindowRect(&rDraw);
		ScreenToClient(&rDraw);
		m_pDrawModel[i] = new CDrawModel(rDraw);
		ASSERT(m_pDrawModel[i]);
	}

	return TRUE;
}

/*
 desc : ��ü ����
 parm : None
 retn : None
*/
VOID CDlgJob::CloseObject()
{
	UINT8 i = 0x00;

	if (m_pDrawModel)
	{
		for (i = 0; i < eJOB_PIC_MAX; i++)
		{
			if (NULL != m_pDrawModel[i])
			{
				delete m_pDrawModel[i];
			}
		}
		delete[] m_pDrawModel;
	}
}

/*
 desc : �⺻ ������ ���� �� ��Ʈ�ѿ� ����
 parm : None
 retn : None
*/
VOID CDlgJob::SetBaseData()
{
	if (CRecipeManager::GetInstance()->LoadSelectRecipe())
	{
		SelectRecipe(CRecipeManager::GetInstance()->GetRecipeIndex(CRecipeManager::GetInstance()->GetRecipeName()), eRECIPE_MODE_VIEW);
	}
}

/*
 desc : �Ϲ� ��ư Ŭ���� ���
 parm : id	- [in]  �Ϲ� ��ư ID
 retn : None
*/
VOID CDlgJob::OnBtnClick(UINT32 id)
{
	switch (id)
	{
		case IDC_JOB_BTN_RECIPE_ADD		:	
		{
			RecipeControl(0x00);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_normal, _T("user act : add recipe"));
		}
		break;

		case IDC_JOB_BTN_RECIPE_DEL		:	
		{
			RecipeControl(0x02);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_normal, _T("user act : del recipe"));
		}
		break;

//#ifndef NO_SELECT
//		case IDC_JOB_BTN_RECIPE_SELECT	:	
//		{
//			RecipeSelect();
//			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_normal, _T("user act : select recipe"));
//		}
//		break;
//#endif
		case IDC_JOB_BTN_RECIPE_SAVE	:	
		{
			RecipeControl(0x01);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_normal, _T("user act : save recipe"));
		}
		break;
	}
}
void CDlgJob::OnDblClickGridRecipeList(NMHDR* pNotifyStruct, LRESULT* pResult)
{

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (m_nSelectRecipe[eRECIPE_MODE_VIEW] != pItem->iRow)
		return;

	if (TRUE == CheckEditing())
		return;

	
	CString strSelectRecipe = m_grd_ctl[eJOB_GRD_RECIPE_LIST].GetItemText(pItem->iRow, eJOB_GRD_COL_RECIPE_LIST_NAME);
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeOnlyName(strSelectRecipe.GetBuffer());
	strSelectRecipe.ReleaseBuffer();

	if (pstRecipe == nullptr)
		return;
	
	
	uvEng_JobRecipe_SetWhatLastSelectIsLocal(true); //���÷����Ƿ� ����.
	
	if (SelectLocalRecipe() == false)
		return;

	

	SelectRecipe(pItem->iRow, eRECIPE_MODE_LOCAL);
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MAIN_RECIPE_UPDATE, 0, 0);

}

void CDlgJob::OnClickGridRecipeList(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	CString strSelectRecipe = m_grd_ctl[eJOB_GRD_RECIPE_LIST].GetItemText(pItem->iRow, eJOB_GRD_COL_RECIPE_LIST_NAME);
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeOnlyName(strSelectRecipe.GetBuffer());
	strSelectRecipe.ReleaseBuffer();

	if (pstRecipe)
	{

		TCHAR tzPath[MAX_PATH_LEN] = { NULL };
		CUniToChar csCnv1, csCnv2;
		swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\%s",
			csCnv1.Ansi2Uni(pstRecipe->gerber_path), csCnv2.Ansi2Uni(pstRecipe->gerber_name));
		if (!uvCmn_FindPath(tzPath))
		{
			if (ShowYesNoMsg(eQUEST, _T("Failed to find the path of gerber file\nDo you want to delete the recipe?")) == IDYES)
			{
				/* ��� ���� ���� */
				if (!CRecipeManager::GetInstance()->DeleteRecipe(strSelectRecipe))	return;

				// ���� �� ���� �����Ƿ� ����
				SelectRecipe(m_nSelectRecipe[eRECIPE_MODE_SEL], eRECIPE_MODE_VIEW);

				/* ���Ӱ� ��ϵ� ������ ���� */
				InitGridRecipeList();
				return;
			}
		}
		else
		{
			//���õǾ��ִ°��� Ŭ�������� ó��
			if (m_nSelectRecipe[eRECIPE_MODE_VIEW] == pItem->iRow)
			{
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			//������ ����üũ
			//�ϳ��� �������̶�� �޼���
			if (TRUE == CheckEditing())
			{
				if (ShowYesNoMsg(eQUEST, _T("���� ���Դϴ�. �������� �ʰ� �̵��� ���, ������ ��ҵ˴ϴ�.\r\n�̵��Ͻðڽ��ϱ�?"), 0x00) == IDNO)
				{
					return;
				}
				else //�̵��Ѵٸ� Ŭ����
				{
					SetEditing(FALSE);
				}
			}
			//////////////////////////////////////////////////////////////////////////
		}

		SelectRecipe(pItem->iRow, eRECIPE_MODE_VIEW);
		UpdateRecipe(m_nSelectRecipe[eRECIPE_MODE_VIEW]);
	}
	Invalidate(TRUE);
}

void CDlgJob::OnClickGridJobParameter(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (eJOB_GRD_COL_PARAMETER_NAME >= pItem->iColumn)
	{
		return;
	}

	CGridCtrl* pGrid = &m_grd_ctl[eJOB_GRD_JOB_PARAMETER + EN_RECIPE_TAB::JOB];
	int				nParamID = _ttoi(pGrid->GetItemText(pItem->iRow, eJOB_GRD_COL_PARAMETER_ID));

	if (EN_RECIPE_JOB::JOB_NAME == nParamID || EN_RECIPE_JOB::GERBER_PATH == nParamID)
	{
		return;
	}

	ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::JOB, nParamID);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"�α��� ���� ����! \n�ش� �Ķ���͸� ������ �� �����ϴ�.", 0x01);
		return;
	}

	CString strValueOld = stParam.GetScaledValue();
	CString strValue = stParam.GetScaledValue();

	CUniToChar		csCnv;
	if (eJOB_GRD_COL_PARAMETER_UNIT == pItem->iColumn)
	{
		if (DEF_DATA_TYPE_LOAD == stParam.strDataType)
		{
			if (EN_RECIPE_JOB::GERBER_NAME == nParamID)
			{
				CString strPathName = SelectGerber();
				if (FALSE == strPathName.IsEmpty())
				{
					int nFindIndex = strPathName.ReverseFind('\\');
					CString strFileName = strPathName.Right(strPathName.GetLength() - nFindIndex - 1);
					strPathName = strPathName.Left(nFindIndex);
					// ���� ��θ� ������ ����� ���, Edit Control�� �� ����
					CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetValue(EN_RECIPE_TAB::JOB, EN_RECIPE_JOB::GERBER_PATH, strPathName);
					stParam.SetValue(strFileName);
				}
				else
				{
					return;
				}
			}
			if (EN_RECIPE_JOB::EXPO_RECIPE == nParamID)
			{
				// Load
				CStringArray	strArrRecipe;
				strArrRecipe.Add(_T("[CANCEL]"));
				int nCount = uvEng_ExpoRecipe_GetCount();
				for (int i = 0; i < nCount; i++)
				{
					LPG_REAF pstRecipe = uvEng_ExpoRecipe_GetRecipeIndex(i);

					strArrRecipe.Add((CString)pstRecipe->expo_name);
				}

				int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("LOAD EXPO RECIPE"), strArrRecipe);

				if (0 == nSelectRecipe)
				{
					// cancel
					return;
				}
				else
				{
					CString strRecipe = strArrRecipe.GetAt(nSelectRecipe);
					stParam.SetValue(strRecipe);
				}
			}
			if (EN_RECIPE_JOB::ALIGN_RECIPE == nParamID)
			{
				CStringArray	strArrRecipe;
				strArrRecipe.Add(_T("[CANCEL]"));
				int nCount = uvEng_Mark_GetAlignRecipeCount();
				for (int i = 0; i < nCount; i++)
				{
					LPG_RAAF pstRecipe = uvEng_Mark_GetAlignRecipeIndex(i);

					strArrRecipe.Add((CString)pstRecipe->align_name);
				}

				int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("LOAD ALIGN RECIPE"), strArrRecipe);

				if (0 == nSelectRecipe)
				{
					// cancel
					return;
				}
				else
				{
					CString strRecipe = strArrRecipe.GetAt(nSelectRecipe);
					stParam.SetValue(strRecipe);
				}
			}
		}
	}
	else
	{
		if (pItem->iColumn != eJOB_GRD_COL_PARAMETER_VALUE)
		{
			return;
		}

		if (DEF_DATA_TYPE_STRING == stParam.strDataType)
		{
			TCHAR tzText[RECIPE_NAME_LENGTH] = { NULL };
			CDlgKBDT dlg;
			if (IDOK == dlg.MyDoModal(RECIPE_NAME_LENGTH))
			{
				dlg.GetText(tzText, RECIPE_NAME_LENGTH);
				stParam.SetValue(tzText);
				pGrid->SetItemText(pItem->iRow, pItem->iColumn, tzText);
			}
		}
		else
		{
			CString strOutput;
			CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);

			double	dMin = stParam.dMin;
			double	dMax = stParam.dMax;

			if (DEF_DATA_TYPE_DOUBLE == stParam.strDataType)
			{
				if (PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax))
				{
					stParam.SetValue(strOutput);
					pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
				}
			}
			else if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
			{
				return;
			}
			else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
			{
				return;
			}
			else if (DEF_DATA_TYPE_INT == stParam.strDataType)
			{
				if (PopupKBDN(ENM_DITM::en_int16, strInput, strOutput, dMin, dMax))
				{



					stParam.SetValue(strOutput);
					pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
				}
			}

		}
	}



	SetEditing(TRUE);
	CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::JOB, stParam);
	if (TRUE == CheckModifyedParam(stParam, strValueOld))
	{
		BOOL bequalParam = FALSE;
		for (int i=0; i<m_vChangeList[EN_RECIPE_TAB::JOB].size(); i++)
		{
			if (m_vChangeList[EN_RECIPE_TAB::JOB].at(i) == pItem->iRow)
			{
				bequalParam = TRUE;
			}
			
		}
		if (!bequalParam)
		{
			m_vChangeList[EN_RECIPE_TAB::JOB].push_back(pItem->iRow);
		}
		
	}
	UpdateGridParam(EN_RECIPE_TAB::JOB);

	ChangedParamColorModify(EN_RECIPE_TAB::JOB, m_vChangeList[EN_RECIPE_TAB::JOB]);

	pGrid->Refresh();
}

void CDlgJob::OnClickGridExposeParameter(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (eJOB_GRD_COL_PARAMETER_NAME >= pItem->iColumn)
	{
		return;
	}

	CGridCtrl* pGrid = &m_grd_ctl[eJOB_GRD_JOB_PARAMETER + EN_RECIPE_TAB::EXPOSE];
	int				nParamID = _ttoi(pGrid->GetItemText(pItem->iRow, eJOB_GRD_COL_PARAMETER_ID));
	ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::EXPOSE, nParamID);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"�α��� ���� ����! \n�ش� �Ķ���͸� ������ �� �����ϴ�.", 0x01);
		return;
	}

	CString strValueOld = stParam.GetScaledValue();
	CString strValue = stParam.GetScaledValue();

	CUniToChar		csCnv;
	if (eJOB_GRD_COL_PARAMETER_UNIT == pItem->iColumn)
	{
		if (stParam.strName == _T("MARK_ERR_DIST_RATE"))
		{
			/*Gerber Size�� ������ ��� */
			MarkRateMarkDist();

			int	nParamID2;
			ST_RECIPE_PARAM stParam2;
			for (int i = 0x00; i < 6; i++)
			{
				//stParam.SetValue(i32DistS[i]);
				//stParam.SetValue(m_i32DistS[i]);
				pGrid->SetItemTextFmt(5 + i, eJOB_GRD_COL_PARAMETER_VALUE, _T("%d"), m_i32DistS[i]);
				m_vChangeList[EN_RECIPE_TAB::EXPOSE].push_back(5 + i);

				nParamID2 = _ttoi(pGrid->GetItemText(5 + i, eJOB_GRD_COL_PARAMETER_ID));
				stParam2 = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::EXPOSE, nParamID2);
				stParam2.SetValue(m_i32DistS[i]);

				CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::EXPOSE, stParam2);
			}
		}


		if (DEF_DATA_TYPE_LOAD == stParam.strDataType)
		{
			if (EN_RECIPE_EXPOSE::EXPO_NAME == nParamID)
			{
				// Load
				CStringArray	strArrRecipe;
				strArrRecipe.Add(_T("[CANCEL]"));
				strArrRecipe.Add(_T("[ADD	]"));
				strArrRecipe.Add(_T("[DELETE]"));

				int nCount = uvEng_ExpoRecipe_GetCount();
				for (int i = 0; i < nCount; i++)
				{
					LPG_REAF pstRecipe = uvEng_ExpoRecipe_GetRecipeIndex(i);

					strArrRecipe.Add((CString)pstRecipe->expo_name);
				}

				int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("LOAD EXPO RECIPE"), strArrRecipe);

				if (0 == nSelectRecipe)
				{
					return;
				}
				else if (1 == nSelectRecipe)
				{
					// �ű� ����
					TCHAR tzText[RECIPE_NAME_LENGTH] = { NULL };
					CDlgKBDT dlg;
					if (IDOK == dlg.MyDoModal(RECIPE_NAME_LENGTH))
					{
						dlg.GetText(tzText, RECIPE_NAME_LENGTH);
					}
					else
					{
						return;
					}

					if (ShowYesNoMsg(eQUEST, _T("Do you want to create the[% s] recipe ? "), tzText) == IDYES)
					{
						stParam.SetValue(tzText);
						CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::EXPOSE, stParam);
						CRecipeManager::GetInstance()->CreateExpoRecipe(tzText);
					}
				}
				else if (2 == nSelectRecipe)
				{
					if (0 == nCount)
					{
						ShowMsg(eSTOP, _T("Not Exist Recipe"));
						return;
					}
					// ����
					
					if (0 == stParam.GetValue().Compare(csCnv.Ansi2Uni(uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal())->expo_recipe)))
					{
						ShowMsg(eSTOP, L"Recipes currently in use cannot be deleted.");
						return;
					}

					if (ShowYesNoMsg(eQUEST, _T("Do you want to delete the [%s] recipe?"), stParam.GetValue()) == IDYES)
					{
						CRecipeManager::GetInstance()->DeleteExpoRecipe(stParam.GetValue());
						CRecipeManager::GetInstance()->LoadExpoRecipe(csCnv.Ansi2Uni(uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal())->expo_recipe), eRECIPE_MODE_VIEW);
					}
				}
				else
				{
					CString strRecipe = strArrRecipe.GetAt(nSelectRecipe);
					stParam.SetValue(strRecipe);
					CRecipeManager::GetInstance()->LoadExpoRecipe(strRecipe, eRECIPE_MODE_VIEW);
				}
			}
			else if (EN_RECIPE_EXPOSE::POWER_NAME == nParamID)
			{
				CStringArray	strArrRecipe;
				strArrRecipe.Add(_T("[CANCEL]"));

				int nCount = uvEng_LedPower_GetCount();
				for (int i = 0; i < nCount; i++)
				{
					LPG_PLPI pstPowerI = uvEng_LedPower_GetLedPowerIndex(i);

					CString strLedPower;
					strLedPower.Format(_T("%s"), (CString)pstPowerI->led_name);

					for (int nHead = 0; nHead < uvEng_GetConfig()->luria_svc.ph_count; nHead++)
					{
						strLedPower.AppendFormat(_T("\n\rPH%d("), nHead+1);
						for (int nWatt = 0; nWatt < 4; nWatt++)
						{
							strLedPower.AppendFormat(_T("%.1fW,"), pstPowerI->led_watt[nHead][nWatt]);
						}
						strLedPower.Delete(strLedPower.GetLength() - 1, 1);
						strLedPower.AppendFormat(_T(")"), nHead);
					}
					strArrRecipe.Add(strLedPower);
				}

				int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("LOAD LED POWER RECIPE"), strArrRecipe);

				if (0 == nSelectRecipe)
				{
					return;
				}
				else
				{
					CString strLedPower = strArrRecipe.GetAt(nSelectRecipe);

					int nSplitIndex = strLedPower.Find('\n');
					if (0 > nSplitIndex)
					{
						return;
					}

					strLedPower = strLedPower.Left(nSplitIndex);
					stParam.SetValue(strLedPower);
				}
			}
		}
	}
	else
	{
		if (pItem->iColumn != eJOB_GRD_COL_PARAMETER_VALUE)
		{
			return;
		}

		if (DEF_DATA_TYPE_STRING == stParam.strDataType)
		{
			TCHAR tzText[RECIPE_NAME_LENGTH] = { NULL };
			CDlgKBDT dlg;
			if (IDOK == dlg.MyDoModal(RECIPE_NAME_LENGTH))
			{
				dlg.GetText(tzText, RECIPE_NAME_LENGTH);
				stParam.SetValue(tzText);
				pGrid->SetItemText(pItem->iRow, pItem->iColumn, tzText);
			}
		}
		else if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
		{
			return;
		}
		else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
		{
			return;
		}
		else
		{
			CString strOutput;
			CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);

			double	dMin = stParam.dMin;
			double	dMax = stParam.dMax;

			if (DEF_DATA_TYPE_DOUBLE == stParam.strDataType)
			{
				if (PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax, 4))
				{
					stParam.SetValue(strOutput);
					pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
				}
			}
			else if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
			{
				return;
			}
			else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
			{
				return;
			}
			else if (DEF_DATA_TYPE_INT == stParam.strDataType)
			{
				if (stParam.strName == _T("MATERIAL_TYPE"))
				{
					return;
				}

				if (PopupKBDN(ENM_DITM::en_int16, strInput, strOutput, dMin, dMax))
				{
					stParam.SetValue(strOutput);
					pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
				}
			}

		}
	}

	SetEditing(TRUE);
	CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::EXPOSE, stParam);
	if (TRUE == CheckModifyedParam(stParam, strValueOld))
	{
		BOOL bequalParam = FALSE;
		for (int i = 0; i < m_vChangeList[EN_RECIPE_TAB::EXPOSE].size(); i++)
		{
			if (m_vChangeList[EN_RECIPE_TAB::EXPOSE].at(i) == pItem->iRow)
			{
				bequalParam = TRUE;
			}

		}
		if (!bequalParam)
		{
			m_vChangeList[EN_RECIPE_TAB::EXPOSE].push_back(pItem->iRow);
		}

	}
	UpdateGridParam(EN_RECIPE_TAB::EXPOSE);

	ChangedParamColorModify(EN_RECIPE_TAB::EXPOSE, m_vChangeList[EN_RECIPE_TAB::EXPOSE]);
	pGrid->Refresh();
}

void CDlgJob::OnClickGridAlignParameter(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (eJOB_GRD_COL_PARAMETER_NAME >= pItem->iColumn)
	{
		return;
	}

	CGridCtrl*		pGrid = &m_grd_ctl[eJOB_GRD_JOB_PARAMETER + EN_RECIPE_TAB::ALIGN];
	int				nParamID = _ttoi(pGrid->GetItemText(pItem->iRow, eJOB_GRD_COL_PARAMETER_ID));
	ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::ALIGN, nParamID);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"�α��� ���� ����! \n�ش� �Ķ���͸� ������ �� �����ϴ�.", 0x01);
		return;
	}

	CString strValueOld = stParam.GetScaledValue();
	CString strValue = stParam.GetScaledValue();

	CUniToChar		csCnv;
	if (eJOB_GRD_COL_PARAMETER_UNIT == pItem->iColumn)
	{
		if (DEF_DATA_TYPE_LOAD == stParam.strDataType)
		{
			if (EN_RECIPE_ALIGN::ALIGN_NAME == nParamID)
			{
				// Load
				CStringArray	strArrRecipe;
				strArrRecipe.Add(_T("[CANCEL]"));
				strArrRecipe.Add(_T("[ADD	]"));
				strArrRecipe.Add(_T("[DELETE]"));

				int nCount = uvEng_Mark_GetAlignRecipeCount();
				for (int i = 0; i < nCount; i++)
				{
					LPG_RAAF pstRecipe = uvEng_Mark_GetAlignRecipeIndex(i);

					strArrRecipe.Add((CString)pstRecipe->align_name);
				}

				int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("LOAD ALIGN RECIPE"), strArrRecipe);

				if (0 == nSelectRecipe)
				{
					return;
				}
				else if (1 == nSelectRecipe)
				{
					// �ű� ����
					TCHAR tzText[RECIPE_NAME_LENGTH] = { NULL };
					CDlgKBDT dlg;
					if (IDOK == dlg.MyDoModal(RECIPE_NAME_LENGTH))
					{
						dlg.GetText(tzText, RECIPE_NAME_LENGTH);
					}
					else
					{
						return;
					}

					if (ShowYesNoMsg(eQUEST, _T("Do you want to create the[% s] recipe ? "), tzText) == IDYES)
					{
						stParam.SetValue(tzText);
						CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::ALIGN, stParam);
						CRecipeManager::GetInstance()->CreateAlignRecipe(tzText);
					}
				}
				else if (2 == nSelectRecipe)
				{
					if (0 == nCount)
					{
						ShowMsg(eSTOP, _T("Not Exist Recipe"));
						return;
					}
					// ����
					if (0 == stParam.GetValue().Compare(csCnv.Ansi2Uni(uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal())->expo_recipe)))
					{
						ShowMsg(eSTOP, L"Recipes currently in use cannot be deleted.");
						return;
					}
					
					if (ShowYesNoMsg(eQUEST, _T("Do you want to delete the [%s] recipe?"), stParam.GetValue()) == IDYES)
					{
						CRecipeManager::GetInstance()->DeleteAlignRecipe(stParam.GetValue());
						CRecipeManager::GetInstance()->LoadAlignRecipe(csCnv.Ansi2Uni(uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal())->align_recipe), eRECIPE_MODE_VIEW);
					}
				}
				else
				{
					CString strRecipe = strArrRecipe.GetAt(nSelectRecipe);
					stParam.SetValue(strRecipe);
					CRecipeManager::GetInstance()->LoadAlignRecipe(strRecipe, eRECIPE_MODE_VIEW);
					for (int i = 0; i < eJOB_PIC_MAX; i++)
					{
						SetMark(i);
					}
				}
			}
			if (EN_RECIPE_ALIGN::GLOBAL_MARK_NAME == nParamID)
			{
				CStringArray	strArrRecipe;
				strArrRecipe.Add(_T("[CANCEL]"));
				strArrRecipe.Add(_T("[NONE]"));

				int nCount = uvEng_Mark_GetModelCount();
				for (int i = 0; i < nCount; i++)
				{
					LPG_CMPV pstMark = uvEng_Mark_GetModelIndex(i);
					strArrRecipe.Add((CString)pstMark->name);
				}

				int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("LOAD MARK RECIPE"), strArrRecipe);

				if (0 == nSelectRecipe)
				{
					return;
				}
				else
				{
					CString strMark = strArrRecipe.GetAt(nSelectRecipe);
					stParam.SetValue(strMark);
					CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::ALIGN, stParam);
					SetMark(eJOB_PIC_MARK_GLOBAL);
				}
			}
			if (EN_RECIPE_ALIGN::LOCAL_MARK_NAME == nParamID)
			{
				CStringArray	strArrRecipe;
				strArrRecipe.Add(_T("[CANCEL]"));
				strArrRecipe.Add(_T("[NONE]"));

				int nCount = uvEng_Mark_GetModelCount();
				for (int i = 0; i < nCount; i++)
				{
					LPG_CMPV pstMark = uvEng_Mark_GetModelIndex(i);
					strArrRecipe.Add((CString)pstMark->name);
				}

				int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("LOAD MARK RECIPE"), strArrRecipe);

				if (0 == nSelectRecipe)
				{
					return;
				}
				else
				{
					CString strMark = strArrRecipe.GetAt(nSelectRecipe);
					stParam.SetValue(strMark);
					CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::ALIGN, stParam);
					SetMark(eJOB_PIC_MARK_LOCAL);
				}
			}
		}
	}
	else
	{
		if (pItem->iColumn != eJOB_GRD_COL_PARAMETER_VALUE)
		{
			return;
		}

		if (DEF_DATA_TYPE_STRING == stParam.strDataType)
		{
			TCHAR tzText[RECIPE_NAME_LENGTH] = { NULL };
			CDlgKBDT dlg;
			if (IDOK == dlg.MyDoModal(RECIPE_NAME_LENGTH))
			{
				dlg.GetText(tzText, RECIPE_NAME_LENGTH);
				stParam.SetValue(tzText);
				pGrid->SetItemText(pItem->iRow, pItem->iColumn, tzText);
			}
		}
		else
		{
			CString strOutput;
			CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);

			double	dMin = stParam.dMin;
			double	dMax = stParam.dMax;

			if (DEF_DATA_TYPE_DOUBLE == stParam.strDataType)
			{
				if (PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax))
				{
					stParam.SetValue(strOutput);
					pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
				}
			}
			else if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
			{
				return;
			}
			else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
			{
				return;
			}
			else if (DEF_DATA_TYPE_INT == stParam.strDataType)
			{
				if (stParam.strName == _T("SEARCH_TYPE"))
				{
					return;
				}
				else if (stParam.strName == _T("ALIGN_TYPE") || stParam.strName == _T("ALIGN_MOTION")) 
				{
					return;
				}
				else if (PopupKBDN(ENM_DITM::en_int16, strInput, strOutput, dMin, dMax))
				{
					stParam.SetValue(strOutput);
					pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
				}
			}
			
		}
	}

	SetEditing(TRUE);
	CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::ALIGN, stParam);
	if (TRUE == CheckModifyedParam(stParam, strValueOld))
	{
		BOOL bequalParam = FALSE;
		for (int i = 0; i < m_vChangeList[EN_RECIPE_TAB::ALIGN].size(); i++)
		{
			if (m_vChangeList[EN_RECIPE_TAB::ALIGN].at(i) == pItem->iRow)
			{
				bequalParam = TRUE;
			}

		}
		if (!bequalParam)
		{
			m_vChangeList[EN_RECIPE_TAB::ALIGN].push_back(pItem->iRow);
		}

	}
	UpdateGridParam(EN_RECIPE_TAB::ALIGN);

	ChangedParamColorModify(EN_RECIPE_TAB::ALIGN, m_vChangeList[EN_RECIPE_TAB::ALIGN]);
	pGrid->Refresh();
}

void CDlgJob::OnClickGridJobParameterChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	if (FALSE == m_bEnableEdit)
		return;

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (eJOB_GRD_COL_PARAMETER_NAME >= pItem->iColumn)
	{
		return;
	}

	CGridCtrl*		pGrid		= &m_grd_ctl[eJOB_GRD_JOB_PARAMETER + EN_RECIPE_TAB::JOB];
	int				nParamID	= _ttoi(pGrid->GetItemText(pItem->iRow, eJOB_GRD_COL_PARAMETER_ID));
	ST_RECIPE_PARAM stParam		= CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::JOB, nParamID);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"�α��� ���� ����! \n�ش� �Ķ���͸� ������ �� �����ϴ�.", 0x01);
		return;
	}

	CString strValue			= pGrid->GetItemText(pItem->iRow, eJOB_GRD_COL_PARAMETER_VALUE);
	CString strValueOld			= stParam.GetScaledValue();

	if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
	{
		BOOL bIsTrue = FALSE;
		if (_T("YES") == strValue)
		{
			bIsTrue = TRUE;
		}
		else
		{
			bIsTrue = FALSE;
		}

		stParam.SetValue(bIsTrue);
	}
	else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
	{
		CStringArray strArrValue;
		stParam.GetDefaultValueList(strArrValue);

		for (int i = 0; i < (int)strArrValue.GetCount(); i++)
		{
			if (0 == strValue.Compare(strArrValue.GetAt(i)))
			{
				stParam.SetValue(i);
				break;
			}
		}
	}
	else
	{
		return;
	}
	
	SetEditing(TRUE);

	CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::JOB, stParam);

	if (TRUE == CheckModifyedParam(stParam, strValueOld))
	{
		BOOL bequalParam = FALSE;
		for (int i = 0; i < m_vChangeList[EN_RECIPE_TAB::JOB].size(); i++)
		{
			if (m_vChangeList[EN_RECIPE_TAB::JOB].at(i) == pItem->iRow)
			{
				bequalParam = TRUE;
			}

		}
		if (!bequalParam)
		{
			m_vChangeList[EN_RECIPE_TAB::JOB].push_back(pItem->iRow);
		}

	}

	ChangedParamColorModify(EN_RECIPE_TAB::JOB, m_vChangeList[EN_RECIPE_TAB::JOB]);
}

void CDlgJob::OnClickGridExposeParameterChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	if (FALSE == m_bEnableEdit)
		return;

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (eJOB_GRD_COL_PARAMETER_NAME >= pItem->iColumn)
	{
		return;
	}

	CGridCtrl* pGrid = &m_grd_ctl[eJOB_GRD_JOB_PARAMETER + EN_RECIPE_TAB::EXPOSE];
	int				nParamID = _ttoi(pGrid->GetItemText(pItem->iRow, eJOB_GRD_COL_PARAMETER_ID));
	ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::EXPOSE, nParamID);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"�α��� ���� ����! \n�ش� �Ķ���͸� ������ �� �����ϴ�.", 0x01);
		return;
	}

	CString strValue = pGrid->GetItemText(pItem->iRow, eJOB_GRD_COL_PARAMETER_VALUE);
	CString strValueOld = stParam.GetScaledValue();

	if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
	{
		BOOL bIsTrue = FALSE;
		if (_T("YES") == strValue || _T("Down") == strValue || _T("Right") == strValue)
		{
			bIsTrue = TRUE;
		}
		else
		{
			bIsTrue = FALSE;
		}



		stParam.SetValue(bIsTrue);
	}
	else if (DEF_DATA_TYPE_INT == stParam.strDataType)
	{
		USES_CONVERSION;
		if (stParam.strName == _T("MATERIAL_TYPE"))
		{
			int options = 0;
			Headoffset find;



			if (uvEng_GetConfig()->headOffsets.GetOffsets(string(CW2A(strValue)), find) != false)
				options = find.idx;

			stParam.SetValue(options);
		}
	}

	else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
	{
		CStringArray strArrValue;
		stParam.GetDefaultValueList(strArrValue);

		for (int i = 0; i < (int)strArrValue.GetCount(); i++)
		{
			if (0 == strValue.Compare(strArrValue.GetAt(i)))
			{
				stParam.SetValue(i);
				break;
			}
		}
	}
	else
	{
		return;
	}

	SetEditing(TRUE);
	CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::EXPOSE, stParam);

	if (TRUE == CheckModifyedParam(stParam, strValueOld))
	{
		BOOL bequalParam = FALSE;
		for (int i = 0; i < m_vChangeList[EN_RECIPE_TAB::EXPOSE].size(); i++)
		{
			if (m_vChangeList[EN_RECIPE_TAB::EXPOSE].at(i) == pItem->iRow)
			{
				bequalParam = TRUE;
			}

		}
		if (!bequalParam)
		{
			m_vChangeList[EN_RECIPE_TAB::EXPOSE].push_back(pItem->iRow);
		}

	}

	ChangedParamColorModify(EN_RECIPE_TAB::EXPOSE, m_vChangeList[EN_RECIPE_TAB::EXPOSE]);
}

void CDlgJob::OnClickGridAlignParameterChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	if (FALSE == m_bEnableEdit)
		return;

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (eJOB_GRD_COL_PARAMETER_NAME >= pItem->iColumn)
	{
		return;
	}

	CGridCtrl* pGrid = &m_grd_ctl[eJOB_GRD_JOB_PARAMETER + EN_RECIPE_TAB::ALIGN];
	int				nParamID = _ttoi(pGrid->GetItemText(pItem->iRow, eJOB_GRD_COL_PARAMETER_ID));
	ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::ALIGN, nParamID);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"�α��� ���� ����! \n�ش� �Ķ���͸� ������ �� �����ϴ�.", 0x01);
		return;
	}

	CString strValue = pGrid->GetItemText(pItem->iRow, eJOB_GRD_COL_PARAMETER_VALUE);
	CString strValueOld = stParam.GetScaledValue();

	if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
	{
		BOOL bIsTrue = FALSE;
		if (_T("YES") == strValue || _T("Pattern Image") == strValue || _T("Coaxial") == strValue)
		{
			bIsTrue = TRUE;
		}
		else
		{
			bIsTrue = FALSE;
		}

		stParam.SetValue(bIsTrue);
	}
	else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
	{
		CStringArray strArrValue;
		stParam.GetDefaultValueList(strArrValue);

		for (int i = 0; i < (int)strArrValue.GetCount(); i++)
		{
			if (0 == strValue.Compare(strArrValue.GetAt(i)))
			{
				stParam.SetValue(i);
				break;
			}
		}
	}
	else if (DEF_DATA_TYPE_INT == stParam.strDataType)
	{
		int options;
		if (stParam.strName == _T("SEARCH_TYPE"))
		{
			options = 0;
			if (_T("Single") == strValue)				options = 1;
			//else if (_T("Cent_side") == strValue)		options = 2;
			else if (_T("Multi_only") == strValue)		options = 2;
			//else if (_T("Ring_cirecle") == strValue)	options = 4;
			//else if (_T("Ph_step") == strValue)			options = 5;
			else
			{
				return;
			}
		}
#ifdef USE_ALIGNMOTION
		else if (stParam.strName == _T("ALIGN_MOTION"))
		{
			auto nameTag = GlobalVariables::GetInstance()->nameTag;

			std::wstring wstr(static_cast<const wchar_t*>(strValue));
			std::string targetValue(wstr.begin(), wstr.end());
			
			auto it = std::find_if(nameTag.begin(), nameTag.end(),
				[targetValue](const std::pair<int, std::string>& element) {
					return element.second == targetValue;
				});

			options = it != nameTag.end() ? it->first : (int)ENG_AMOS::en_onthefly_2cam;
		}
#endif
		else if (stParam.strName == _T("ALIGN_TYPE"))
		{
			options = 0;
			if (_T("Global 0 Local 0") == strValue)			options = 0x00;
			else if (_T("Global 4 Local 0") == strValue)		options = 0x04;
			else if (_T("Global 4 Local n") == strValue)		options = 0x05;
			/*else if (_T("Global 4 Local 2_2") == strValue)		options = 0x21;
			else if (_T("Global 4 Local 2_3") == strValue)		options = 0x22;
			else if (_T("Global 4 Local 2_4") == strValue)		options = 0x23;
			else if (_T("Global 4 Local 2_5") == strValue)		options = 0x24;*/
			else
			{
				return;
			}

		}

		stParam.SetValue(options);
	}
	else
	{
		return;
	}

	SetEditing(TRUE);
	CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::ALIGN, stParam);

	if (TRUE == CheckModifyedParam(stParam, strValueOld))
	{
		BOOL bequalParam = FALSE;
		for (int i = 0; i < m_vChangeList[EN_RECIPE_TAB::ALIGN].size(); i++)
		{
			if (m_vChangeList[EN_RECIPE_TAB::ALIGN].at(i) == pItem->iRow)
			{
				bequalParam = TRUE;
			}

		}
		if (!bequalParam)
		{
			m_vChangeList[EN_RECIPE_TAB::ALIGN].push_back(pItem->iRow);
		}

	}

	ChangedParamColorModify(EN_RECIPE_TAB::ALIGN, m_vChangeList[EN_RECIPE_TAB::ALIGN]);
}

/*
 desc : Gerber Recipe ������ ȭ�鿡 ����
 parm : recipe	- [in]  Gerber Recipe ������ ����� ����ü ������
 retn : None
*/
VOID CDlgJob::UpdateRecipe(int nSelectRecipe)
{
	CUniToChar csCnv;

	/* ȭ�� ���� ��� */
	SetRedraw(FALSE);

	LPG_RJAF recipe = NULL;
	CString strSelectRecipe = m_grd_ctl[eJOB_GRD_RECIPE_LIST].GetItemText(nSelectRecipe, eJOB_GRD_COL_RECIPE_LIST_NAME);
	/* ���� ���õ� �����ǰ� �ִٸ�, ���� ���� ��� */
	if ((recipe = uvEng_JobRecipe_GetRecipeOnlyName(strSelectRecipe.GetBuffer())))
	{
		CRecipeManager::GetInstance()->LoadRecipe(strSelectRecipe, eRECIPE_MODE_VIEW); //����Ŭ���̴� ù �ѹ��� ������ Ŭ������ VIEW�� �߰��ǰԵȴ�. �׷��� LOCALSELECT�� VIEW�� ����ȴ�. 

		UpdateGridParam(EN_RECIPE_TAB::JOB);
		UpdateGridParam(EN_RECIPE_TAB::EXPOSE);
		UpdateGridParam(EN_RECIPE_TAB::ALIGN);

		/* Memory DC ������ ��� */
		m_pDrawPrev->LoadMark(recipe);
		m_pDrawPrev->DrawMem(recipe);

		for (int i = 0; i < eJOB_PIC_MAX; i++)
		{
			SetMark(i);
		}

		/* ȭ�� ���� Ȱ��ȭ */
		SetRedraw(TRUE);
		Invalidate(FALSE);
	}
	strSelectRecipe.ReleaseBuffer();
}

/*
 desc : ���ο� Job Name ���
 parm : None
 retn : None
*/
PTCHAR CDlgJob::SelectGerber()
{
	TCHAR  tzJobName[MAX_PATH_LEN] = { NULL }, tzRLTFiles[MAX_PATH_LEN] = {NULL};
	CDlgMesg dlgMesg;
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* Job Name ���丮 ���� */
	if (!uvCmn_RootSelectPath(m_hWnd, uvEng_GetConfig()->set_comn.gerber_path, tzJobName))	return NULL;
	/* Job Name ���� ���� ���丮�� �����ϴ��� Ȯ�� */
	if (uvCmn_GetChildPathCount(tzJobName) != 0)
	{
		dlgMesg.MyDoModal(L"This is not a gerber directory", 0x01);
		return NULL;
	}
	/* �ش� ���丮 ���� rlt_settings.xml ������ �����ϴ��� Ȯ�� */
	swprintf_s(tzRLTFiles, MAX_PATH_LEN, L"%s\\rlt_settings.xml", tzJobName);
	if (!uvCmn_FindFile(tzRLTFiles))
	{
		dlgMesg.MyDoModal(L"File (rlt_settings.xml) does not exist", 0x01);
		return NULL;
	}
	/* ������ ��ϵ� �Ź� ���� �߿� ������ �Ź��� �ִ��� ���� Ȯ�� */
	if (pstJobMgt->IsJobFullFinded(csCnv.Uni2Ansi(tzJobName)))
	{
		dlgMesg.MyDoModal(L"Job Name already registered exist", 0x01);
		return NULL;
	}

	return tzJobName;
}


/*
 desc : ��ũ ���� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgJob::ResetMarkInfo()
{
	/* Gerber Preview */
	if (m_pDrawPrev)	m_pDrawPrev->DrawMem(NULL);

	CalcMarkDist();
}


VOID CDlgJob::CalcMarkDist()
{
	UINT32 u32Dist[6] = { NULL };	/* um */
	DOUBLE dbRate = 0.0003 /* Medium (Middle) */, dbDist = 0.0f /* mm */;
	CDlgMesg dlgMesg;
	CAtlList <DOUBLE> lstX, lstY;
	TCHAR tzGerb[MAX_GERBER_NAME] = { NULL };
	//CUniToChar csCnv;


	LPG_RJAF pstJob = uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal());
	//* ���� ���õ� �Ź� ���� (��ü ���) ��� */
	swprintf_s(tzGerb, MAX_PATH_LEN, L"%S\\%S", pstJob->gerber_path, pstJob->gerber_name);


	/* �Ź��� ���� ��ũ ���� ��� */
	if (0x00 != uvEng_Luria_GetGlobalMarkJobName(tzGerb, lstX, lstY,
		ENG_ATGL::en_global_4_local_0_point))
	{
		dlgMesg.MyDoModal(L"Failed to get the mark info. of gerber file", 0x01);
	}
	else
	{
		//LPG_REAF pstExpo = uvEng_ExpoRecipe_GetSelectRecipe();
		//dbRate = 0.0001f;
		///LPG_REAF pstExpo = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJob->expo_recipe));


		//dbRate = pstExpo->global_mark_dist_rate;

		/* ���� ���� ��ϵ� ��ũ�� ���ٸ� */
		if (lstX.GetCount() >= 4 && lstY.GetCount() >= 4)
		{
			/* 1�� / 3�� ��ũ ���� ���� (�Ÿ�) ���� ���� ���� �� ���� */
			dbDist = abs(lstX.GetAt(lstX.FindIndex(0)) - lstX.GetAt(lstX.FindIndex(2)));
			u32Dist[0] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 2�� / 4�� ��ũ ���� ���� (�Ÿ�) ���� ���� ���� �� ���� */
			dbDist = abs(lstX.GetAt(lstX.FindIndex(1)) - lstX.GetAt(lstX.FindIndex(3)));
			u32Dist[1] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 1�� / 2�� ��ũ ���� ���� (�Ÿ�) ���� ���� ���� �� ���� */
			dbDist = abs(lstY.GetAt(lstY.FindIndex(0)) - lstY.GetAt(lstY.FindIndex(1)));
			u32Dist[2] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 3�� / 4�� ��ũ ���� ���� (�Ÿ�) ���� ���� ���� �� ���� */
			dbDist = abs(lstY.GetAt(lstY.FindIndex(2)) - lstY.GetAt(lstY.FindIndex(3)));
			u32Dist[3] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 1�� / 4�� ��ũ ���� �밢 (�Ÿ�) ���� ���� ���� �� ���� */
			dbDist = sqrt(pow(lstY.GetAt(lstY.FindIndex(0)) - lstY.GetAt(lstY.FindIndex(1)), 2) +
				pow(lstX.GetAt(lstX.FindIndex(0)) - lstX.GetAt(lstX.FindIndex(3)), 2));
			u32Dist[4] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 2�� / 3�� ��ũ ���� �밢 (�Ÿ�) ���� ���� ���� �� ���� */
			dbDist = sqrt(pow(lstY.GetAt(lstY.FindIndex(2)) - lstY.GetAt(lstY.FindIndex(3)), 2) +
				pow(lstX.GetAt(lstX.FindIndex(1)) - lstX.GetAt(lstX.FindIndex(2)), 2));
			u32Dist[5] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
		}
	}

	//CGridCtrl* pGrid22 = &m_grd_ctl[eJOB_GRD_JOB_PARAMETER + EN_RECIPE_TAB::EXPOSE];

	//
	//int i = 0;
	//CString strOutput;
	//for (i = 0; i < 6; i++)
	//{
	//	//ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::ALIGN, nParamID);
	////	strOutput.Format(_T("%d"), u32Dist[i]);
	//	//stParam.SetValue(strOutput);
	//	//pGrid->SetItemTextFmt(i + 5, 2, _T("%s"), strOutput);
	//}

	//UpdateGridParam(EN_RECIPE_TAB::EXPOSE);

	//ChangedParamColorModify(EN_RECIPE_TAB::EXPOSE, m_vChangeList[EN_RECIPE_TAB::EXPOSE]);
	//pGrid->Refresh();

	lstX.RemoveAll();
	lstX.RemoveAll();
}



VOID CDlgJob::MarkRateMarkDist()
{
	BOOL bSucc = TRUE;
	UINT8 i, u8MarkG = 0x00;
	//INT32 i32DistS[6] = { NULL };
	STG_XMXY stMarkPos1 = { STG_XMXY(), }, stMarkPos2 = { STG_XMXY(), };
	UINT8 u8Reg1[6] = { 0, 1, 0, 2, 0, 1 }, u8Reg2[6] = { 2, 3, 1, 3, 3, 2 };
	CAtlList <STG_XMXY> lstMarks;
	CUniToChar csCnv;

	/* XML ���Ͽ� ��ϵ� Mark ���� Ȯ�� */
	u8MarkG = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal());
	
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJobRecipe->expo_recipe));
	/* Global ��ũ�� ��ϵǾ� ���� �ʴٸ� Pass */
	if (u8MarkG < 4)
	{
		return;
	}

	for (i = 0; i < u8MarkG; i++)
	{
		bSucc = uvEng_Luria_GetGlobalMark(i, &stMarkPos1);
		if (!bSucc)	LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to get the global mark from selected gerber file");
		else		lstMarks.AddTail(stMarkPos1);

	}

	/* ���� �Ź� ��ǥ ���� ��ũ ���� / ���� �� �ӽ� ���� */
	for (i = 0x00; bSucc && i < 6; i++)
	{
		stMarkPos1 = lstMarks.GetAt(lstMarks.FindIndex(u8Reg1[i]));
		stMarkPos2 = lstMarks.GetAt(lstMarks.FindIndex(u8Reg2[i]));
		/* mm -> 0.1 um or 100 nm */
		m_i32DistS[i] = (INT32)ROUNDED(sqrt(pow(stMarkPos1.mark_x - stMarkPos2.mark_x, 2) +
			pow(stMarkPos1.mark_y - stMarkPos2.mark_y, 2)) * 10000.0f, 0);

		//i32DistS[i] = pstExpoRecipe->global_mark_dist_rate * i32DistS[i];
		//pstExpoRecipe->global_mark_dist[i] = (pstExpoRecipe->global_mark_dist_rate * i32DistS[i]) / 10.0f;
		m_i32DistS[i] = (pstExpoRecipe->global_mark_dist_rate * m_i32DistS[i]) / 10.0f;

	}

	//return i32DistS;
}


/*
 desc : ���ο� ������ �߰�
 parm : mode	- [in]  0x00:Append, 0x01:Modify, 0x02:Delete
 retn : None
*/
VOID CDlgJob::RecipeControl(UINT8 mode)
{
	BOOL bUpdated		= FALSE;
	TCHAR tzText[1024]	= {NULL}, tzPath[MAX_PATH_LEN] = {NULL}, tzName[MAX_GERBER_NAME] = {NULL};
	
	CUniToChar csCnv, csCnv1;
	CDlgMesg dlgMesg;

	/* ������ ���� ����ü �ʱ�ȭ */

	CString strRecipeName = m_grd_ctl[eJOB_GRD_RECIPE_LIST].GetItemText(m_nSelectRecipe[eRECIPE_MODE_VIEW], 1);

	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"--warning-- Do you want to recipe %s?" ,(mode == 0x01 ? L"modifying" : mode == 0x02 ? L"delete" : L"add"));

	if (IDOK != dlgMesg.MyDoModal(tzMsg, 0x02))
	{
		return;
	}
	

	/* ������ ���/����/���� ���� */
	switch (mode)
	{
	case 0x00	: 
	{
		TCHAR tzText[RECIPE_NAME_LENGTH] = { NULL };
		CString strPathName = SelectGerber();

		if (FALSE == strPathName.IsEmpty())
		{
			int nFindIndex = strPathName.ReverseFind('\\');
			CString strFileName = strPathName.Right(strPathName.GetLength() - nFindIndex - 1);
			/*�Ź� �̸� ����*/
			swprintf_s(tzText, 128, L"%s", strFileName);
			strPathName = strPathName.Left(nFindIndex);
			// ���� ��θ� ������ ����� ���, Edit Control�� �� ����
			CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetValue(EN_RECIPE_TAB::JOB, EN_RECIPE_JOB::GERBER_PATH, strPathName);
			CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetValue(EN_RECIPE_TAB::JOB, EN_RECIPE_JOB::GERBER_NAME, strFileName);
		}
		else
		{
			return;
		}

		CDlgKBDT dlg;
		//if (IDOK == dlg.MyDoModal(RECIPE_NAME_LENGTH))
		if (IDOK == dlg.MyDoModal(tzText, RECIPE_NAME_LENGTH))
		{
			wmemset(tzText, 0x00, 128);
			dlg.GetText(tzText, RECIPE_NAME_LENGTH);
			strRecipeName = tzText;

			if (IDNO == ShowYesNoMsg(eQUEST, _T("Do you want create [%s] recipe?"), strRecipeName))
			{
				return;
			}
		}
		else
		{
			return;
		}
		bUpdated = CRecipeManager::GetInstance()->CreateRecipe(strRecipeName);
		/* ��� ���� ���� */
		if (!bUpdated)	return;
		/*PhilHMI�� ���� �Ǿ� �ִٸ� ����*/
		if (uvEng_Philhmi_IsConnected())
		{
		/*	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();*/

		/*	CString strSelectRecipe = m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].GetItemText(pItem->iRow, ePHILHMI_GRD_COL_RECIPE_LIST_NAME);*/
			LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeOnlyName(strRecipeName.GetBuffer());

			CRecipeManager::GetInstance()->PhilSendCreateRecipe(pstRecipe);
		}

		/* ���Ӱ� ��ϵ� ������ ���� */
		InitGridRecipeList();
		break;
	}
	case 0x01	:  //<-�ٵ� ���� uvdi���� select�� ���� ������ ��������� �ʰԵ����� ������� ������� �𸣴� ����.
		bUpdated = CRecipeManager::GetInstance()->SaveRecipe(strRecipeName, eRECIPE_MODE_VIEW);		
		/* ��� ���� ���� */
		if (!bUpdated)	return;
		/*PhilHMI�� ���� �Ǿ� �ִٸ� ����*/
		if (uvEng_Philhmi_IsConnected())
		{
			LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeOnlyName(strRecipeName.GetBuffer());
			CRecipeManager::GetInstance()->PhilSendModifyRecipe(pstRecipe);
		}

		for (int i = 0; i <= EN_RECIPE_TAB::ALIGN; i++)
		{
			m_vChangeList[i].clear();
		}

		UpdateRecipe(m_nSelectRecipe[eRECIPE_MODE_VIEW]);

		/*������ �Ķ���� ������ ����*/
		//SelectHostRecipe();
		SetEditing(FALSE);
		break;

	case 0x02	: 
	{
		if (m_nSelectRecipe[eRECIPE_MODE_SEL] == m_nSelectRecipe[eRECIPE_MODE_VIEW])
		{
			ShowMsg(eWARN, _T("can not delete current recipe"));
			return;
		}
		bUpdated = CRecipeManager::GetInstance()->DeleteRecipe(strRecipeName);
		/* ��� ���� ���� */
		if (!bUpdated)	return;
		/*PhilHMI�� ���� �Ǿ� �ִٸ� ����*/
		if (uvEng_Philhmi_IsConnected())
		{
			CRecipeManager::GetInstance()->PhilSendDeleteRecipe(strRecipeName);
		}

		// ���� �� ���� �����Ƿ� ����
		SelectRecipe(m_nSelectRecipe[eRECIPE_MODE_SEL], eRECIPE_MODE_VIEW);

		/* ���Ӱ� ��ϵ� ������ ���� */
		InitGridRecipeList();
	}
		break;
	}

	/* ��ü ȭ�� ���� */
	Invalidate(TRUE);
}


VOID CDlgJob::SelectHostRecipe()
{
	CUniToChar csCnv;
	CString strReicpe, strExpo, strAlign;
	strReicpe = CRecipeManager::GetInstance()->GetRecipeName();

	CString strRecipeName = m_grd_ctl[eJOB_GRD_RECIPE_LIST].GetItemText(m_nSelectRecipe[eRECIPE_MODE_VIEW], eJOB_GRD_COL_RECIPE_LIST_NAME);

	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"--warning-- Cancel recipe change?\n %s => %s", strReicpe, strRecipeName);

	CDlgMesg dlgMesg;
	if (IDOK == dlgMesg.MyDoModal(tzMsg, 0x04))
	{
		return;
	}

	/* ���� �޸𸮿� �׸� ���� ���� ���� */
	if (!CRecipeManager::GetInstance()->SelectRecipe(strRecipeName, eRECIPE_MODE_SEL_FROM_HOST))
	{
		ShowMsg(eWARN, L"Failed to select the gerber recipe", 0x01);
		return;
	}
	/*PhilHMI�� ���� �Ǿ� �ִٸ� ����*/
	if (uvEng_Philhmi_IsConnected())
	{
		CRecipeManager::GetInstance()->PhilSendSelectRecipe(strRecipeName);
	}


	//strReicpe = CRecipeManager::GetInstance()->GetRecipeName();
	//strExpo = CRecipeManager::GetInstance()->GetExpoRecipeName();
	//strAlign = CRecipeManager::GetInstance()->GetAlignRecipeName();

	m_pDlgMain->SendMessageW(WM_MAIN_RECIPE_UPDATE, 0, 0);
	//m_pDlgMain->SendMessageW(WM_MAIN_RECIPE_UPDATE, EN_RECIPE_TAB::EXPOSE, (LPARAM)&strExpo);
	//m_pDlgMain->SendMessageW(WM_MAIN_RECIPE_UPDATE, EN_RECIPE_TAB::ALIGN, (LPARAM)&strAlign);

	SelectRecipe(m_nSelectRecipe[eRECIPE_MODE_VIEW]);

	/* ���������� ���� �޸𸮿� ���õ� ��ũ ������ pstRecipeMark ���� */
	// CString -> TCHAR * 
	TCHAR* tcharRecipeName = _T("");
	tcharRecipeName = (TCHAR*)(LPCTSTR)strAlign;
	//if (!uvEng_Mark_SelAlignRecipeName(csCnv.Ansi2Uni(strAlign)))

	LPG_RAAF align = uvEng_Mark_GetAlignRecipeName(tcharRecipeName);

	if (align == nullptr)
	{
		ShowMsg(eWARN, L"Failed to select the recipe for mark", 0x01);
		return;
	}
	else
	{
		for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) 
		{
			for (int j = 0; j < 2; j++) 
			{ // global, local 2��
				uvCmn_Camera_SetMarkFindMode(i + 1, align->mark_type, j);
			}
		}

		uvEng_Camera_SetMarkMethod(ENG_MMSM(align->search_type), align->search_count);
	}
	
}




/*
 desc : ���� ���õ� Gerber Recipe �׸� �޸𸮿� ����
 parm : None
 retn : None
*/
BOOL CDlgJob::SelectLocalRecipe()
{
	CUniToChar csCnv;
	CString   strAlign;


	CString strRecipeName	= m_grd_ctl[eJOB_GRD_RECIPE_LIST].GetItemText(m_nSelectRecipe[eRECIPE_MODE_VIEW], eJOB_GRD_COL_RECIPE_LIST_NAME);
	
	if (!CRecipeManager::GetInstance()->SelectRecipe(strRecipeName, eRECIPE_MODE_SEL_FROM_LOCAL)) //<-�̰� ��� ������.
	{
		ShowMsg(eWARN, L"Failed to select the gerber recipe", 0x01);
		return false;
	}

	/*strReicpe = CRecipeManager::GetInstance()->GetRecipeName();
	strExpo = CRecipeManager::GetInstance()->GetExpoRecipeName();*/
	strAlign = CRecipeManager::GetInstance()->GetAlignRecipeName( eRECIPE_MODE_LOCAL);

	//SelectRecipe(m_nSelectRecipe[eRECIPE_MODE_VIEW]);

	/* ���������� ���� �޸𸮿� ���õ� ��ũ ������ pstRecipeMark ���� */
	// CString -> TCHAR * 
	TCHAR tcharRecipeName[256] = { 0 };
	_tcscpy_s(tcharRecipeName, strAlign);  // �� ���纻���� �����ϰ� ���
	
	
	LPG_RAAF pstRecipeAlign = uvEng_Mark_GetAlignRecipeName(tcharRecipeName);

	//if (!uvEng_Mark_SelAlignRecipeName(csCnv.Ansi2Uni(strAlign)))
	if (pstRecipeAlign == nullptr)
	{
		ShowMsg(eWARN, L"Failed to select the recipe for mark", 0x01);
		return false;
	}
	
	for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) 
	{
		for (int j = 0; j < 2; j++) 
		{ // global, local 2��
			uvCmn_Camera_SetMarkFindMode(i + 1, pstRecipeAlign->mark_type, j);
		}
	}
	
	uvEng_Camera_SetMarkMethod(ENG_MMSM(pstRecipeAlign->search_type), pstRecipeAlign->search_count);

	return TRUE;
}

void CDlgJob::SelectRecipe(int nSelect, EN_RECIPE_MODE eRecipeMode) 
{

	m_nSelectRecipeOld[eRecipeMode] = m_nSelectRecipe[eRecipeMode];
	m_nSelectRecipe[eRecipeMode] = nSelect;

	//Old���� ó��
	for (int nCnt = 0; nCnt < eRECIPE_MODE_MAX; nCnt++)
	{
		if (-1 != m_nSelectRecipeOld[nCnt] && m_grd_ctl[eJOB_GRD_RECIPE_LIST].GetRowCount() > m_nSelectRecipeOld[nCnt])
		{
			m_grd_ctl[eJOB_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipeOld[nCnt], 0, ALICE_BLUE);
			m_grd_ctl[eJOB_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipeOld[nCnt], 1, WHITE_);
		}
	}

	

	if (-1 != m_nSelectRecipe[eRECIPE_MODE_VIEW])
	{
		m_grd_ctl[eJOB_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipe[eRECIPE_MODE_VIEW], 0, PALE_GREEN);
		m_grd_ctl[eJOB_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipe[eRECIPE_MODE_VIEW], 1, PALE_GREEN);

		
	}

	if (-1 != m_nSelectRecipe[eRECIPE_MODE_LOCAL])
	{
		m_grd_ctl[eJOB_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipe[eRECIPE_MODE_LOCAL], 0, LIGHT_MAGENTA);
		m_grd_ctl[eJOB_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipe[eRECIPE_MODE_LOCAL], 1, LIGHT_MAGENTA);
	}

	if (-1 != m_nSelectRecipe[eRECIPE_MODE_SEL])
	{
		m_grd_ctl[eJOB_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipe[eRECIPE_MODE_SEL], 0, LIGHT_CORAL);
		m_grd_ctl[eJOB_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipe[eRECIPE_MODE_SEL], 1, LIGHT_CORAL);
	}

	if (m_nSelectRecipe[eRECIPE_MODE_SEL] == m_nSelectRecipe[eRECIPE_MODE_VIEW] &&
		m_nSelectRecipe[eRECIPE_MODE_SEL] != -1)
	{
		
	}

	m_grd_ctl[eJOB_GRD_RECIPE_LIST].Refresh();
}

void CDlgJob::SetEditing(BOOL bEditing)
{
	m_bIsEditing = bEditing;
}

BOOL CDlgJob::CheckEditing()
{
	return m_bIsEditing;
}

BOOL CDlgJob::EnableEdit()
{
	BOOL bEdit = TRUE;
	//View�� Sel�̵� �ϳ��� �̸��� �������� Ȯ���ϰ�
	if (_T("") == CRecipeManager::GetInstance()->GetRecipeName())
	{
		bEdit = FALSE;
	}
	//�̸��� ���������� Ȱ��ȭ �����ʴ´�.
	if (CRecipeManager::GetInstance()->GetRecipeName() != CRecipeManager::GetInstance()->GetRecipeName(eRECIPE_MODE_VIEW))
	{
		bEdit = FALSE;
	}

	if (TRUE == bEdit)
	{
		m_bEnableEdit = TRUE;
		m_btn_ctl[eJOB_BTN_RECIPE_SAVE].EnableWindow(TRUE);
	}
	else
	{
		m_bEnableEdit = FALSE;
		m_btn_ctl[eJOB_BTN_RECIPE_SAVE].EnableWindow(FALSE);
	}

	return bEdit;
}

VOID CDlgJob::SetMark(UINT8 index) // index : Global, Local
{
	CUniToChar csCnv, csCnv2;

	/* ���� ��Ȱ��ȭ */
	SetRedraw(FALSE);

	m_pDrawModel[index]->ResetModel();

	ST_RECIPE_PARAM stAlignParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::ALIGN, EN_RECIPE_ALIGN::ALIGN_NAME);
	ST_RECIPE_PARAM stMarkParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::ALIGN, EN_RECIPE_ALIGN::GLOBAL_MARK_NAME + index);

	LPG_RAAF pstAlign = uvEng_Mark_GetAlignRecipeName(stAlignParam.GetValue().GetBuffer()); stAlignParam.GetValue().ReleaseBuffer();
	LPG_CMPV pstMark = uvEng_Mark_GetModelName(stMarkParam.GetValue().GetBuffer()); stMarkParam.GetValue().ReleaseBuffer();

	if (NULL == pstMark)
	{
		/* ���� Ȱ��ȭ */
		SetRedraw(TRUE);
		Invalidate(TRUE);
		return;
	}
	/* �� ���� �� ȭ�� ���� */
	m_pDrawModel[index]->SetModel(csCnv.Ansi2Uni(pstMark->name), ENG_MMDT(pstMark->type), pstAlign->acam_num[index], pstMark->param);
	/* �� ���� �� ȭ�� ���� */
	for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
		if (ENG_MMDT::en_image != ENG_MMDT(pstMark->type))
		{
			m_pDrawModel[index]->SetModel(csCnv.Ansi2Uni(pstMark->name),
				ENG_MMDT(pstMark->type), i+1,
				pstMark->param);
		}
		else {
			m_pDrawModel[index]->SetModel(csCnv.Ansi2Uni(pstMark->name),
				i+1, csCnv2.Ansi2Uni(pstMark->file), pstMark->iSizeP, pstMark->iOffsetP);

			uvCmn_Camera_SetMarkOffset(i+1, pstMark->iOffsetP, 3, index);
			uvCmn_Camera_SetMarkSize(i+1, pstMark->iSizeP, 1, index);
		}
	}

	/* ���� Ȱ��ȭ */
	SetRedraw(TRUE);
	Invalidate(TRUE);
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION �߰� �Լ�                                     */
/* ----------------------------------------------------------------------------------------- */

/* desc: Frame Control�� MIL DISP�� ���� */
VOID CDlgJob::InitDispMark()
{
	CWnd* pWnd_Mark[2];
	for (int i = 0; i < 2; i++) { // global, local 2��
		pWnd_Mark[i] = GetDlgItem(IDC_JOB_PIC_GLOBAL_MARK + i);
	}
	uvEng_Camera_SetDispRecipeMark(pWnd_Mark);
}

BOOL CDlgJob::CheckModifyedParam(ST_RECIPE_PARAM stParam, CString strValueOld)
{
	//�Ϲ����� ���� ���濩�� �˻�
	if (DEF_DATA_TYPE_BOOL == stParam.strDataType) //�ƹ��͵� ��������
	{
		if (_ttoi(strValueOld) == stParam.GetInt())
			return FALSE;
	}
	else if (DEF_DATA_TYPE_INT == stParam.strDataType)
	{
		if (_ttoi(strValueOld) == stParam.GetInt())
			return FALSE;
	}
	else if (DEF_DATA_TYPE_DOUBLE == stParam.strDataType)
	{
		if (_ttof(strValueOld) == stParam.GetDouble())
			return FALSE;
	}
	else if (DEF_DATA_TYPE_STRING == stParam.strDataType)
	{
		if (0 == strValueOld.Compare(stParam.GetValue()))
			return FALSE;
	}
	else if (DEF_DATA_TYPE_LOAD == stParam.strDataType)
	{
		if (0 == strValueOld.Compare(stParam.GetValue()))
			return FALSE;
	}
	else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
	{
		if (0 == strValueOld.Compare(stParam.GetValue()))
			return FALSE;
	}

	return TRUE;
}

void CDlgJob::ChangedParamColorModify(int nRecipeTab, std::vector <int> vChangeList)
{
	CGridCtrl* pGrid = &m_grd_ctl[eJOB_GRD_JOB_PARAMETER + nRecipeTab];

	for (int i =0; i<vChangeList.size(); i++)
	{
		pGrid->SetItemFgColour(vChangeList.at(i), eJOB_GRD_COL_PARAMETER_VALUE, WHITE_);
		pGrid->SetItemBkColour(vChangeList.at(i), eJOB_GRD_COL_PARAMETER_VALUE, GREEN_);
	}

}
