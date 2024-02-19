#include "pch.h"
#include "ResizeUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEF_UI_DEFAULT_SIZE_X 1462
#define DEF_UI_DEFAULT_SIZE_Y 1092

CResizeUI::CResizeUI()
{
	m_dSizeRateX = GetSizeRateX();
	m_dSizeRateY = GetSizeRateY();
}

CResizeUI::~CResizeUI()
{
}

int	CResizeUI::GetSizeX()
{
	MONITORINFOEX stMonitorInfo;
	CRect rtMonitor;
	stMonitorInfo.cbSize = sizeof(MONITORINFOEX);

	// 윈도우가 있는 모니터 핸들( rect는 윈도우창 영역의 정보가 들어있는 RECT )
	HMONITOR hMOnitor = MonitorFromRect(&rtMonitor, MONITOR_DEFAULTTONEAREST);
	GetMonitorInfo(hMOnitor, &stMonitorInfo);

	int nSizeX = stMonitorInfo.rcMonitor.right;//GetSystemMetrics(SM_CXFULLSCREEN);

	HWND hTrayWnd = ::FindWindow(_T("Shell_TrayWnd"), _T(""));
	if (hTrayWnd)
	{
		CRect rcWorkRect(0, 0, 0, 0);
		int nWorkSizeOffsetX = 0;
		int nWorkSizeOffsetY = 0;

		::GetWindowRect(hTrayWnd, &rcWorkRect);

		if (rcWorkRect.Width() > rcWorkRect.Height())
		{
			nWorkSizeOffsetY = rcWorkRect.Height();
		}
		else
		{
			nWorkSizeOffsetX = rcWorkRect.Width();
		}

		nSizeX = nSizeX - nWorkSizeOffsetX;
	}

	return nSizeX - 1;
}

int	CResizeUI::GetSizeY()
{
	//int nSizeY = GetSystemMetrics(SM_CYMAXTRACK) - 20;
	MONITORINFOEX stMonitorInfo;
	CRect rtMonitor;
	stMonitorInfo.cbSize = sizeof(MONITORINFOEX);

	// 윈도우가 있는 모니터 핸들( rect는 윈도우창 영역의 정보가 들어있는 RECT )
	HMONITOR hMOnitor = MonitorFromRect(&rtMonitor, MONITOR_DEFAULTTONEAREST);
	GetMonitorInfo(hMOnitor, &stMonitorInfo);

	int nSizeY = stMonitorInfo.rcMonitor.bottom;

	HWND hTrayWnd = ::FindWindow(_T("Shell_TrayWnd"), _T(""));
	if (hTrayWnd)
	{
		CRect rcWorkRect(0, 0, 0, 0);
		int nWorkSizeOffsetX = 0;
		int nWorkSizeOffsetY = 0;

		::GetWindowRect(hTrayWnd, &rcWorkRect);

		if (rcWorkRect.Width() > rcWorkRect.Height())
		{
			nWorkSizeOffsetY = rcWorkRect.Height();
		}
		else
		{
			nWorkSizeOffsetX = rcWorkRect.Width();
		}

		nSizeY = nSizeY - nWorkSizeOffsetY;
	}

	return nSizeY - 1;
}

double CResizeUI::GetSizeRateX()
{
	return (double)GetSizeX() / (double)DEF_UI_DEFAULT_SIZE_X;
}

double CResizeUI::GetSizeRateY()
{
	return (double)GetSizeY() / (double)DEF_UI_DEFAULT_SIZE_Y;
}

void CResizeUI::ResizeControl(CWnd *pwndParent, CWnd *pwndCtrl, BOOL bIsOnlySize)
{
	CRect rtSize;
	pwndCtrl->GetWindowRect(&rtSize);
	pwndParent->ScreenToClient(&rtSize);

	if (bIsOnlySize)
	{
		rtSize.right = (long)((double)rtSize.right * m_dSizeRateX);
		rtSize.bottom = (long)((double)rtSize.bottom * m_dSizeRateY);
	}
	else
	{
		rtSize.left = (long)((double)rtSize.left * m_dSizeRateX);
		rtSize.top = (long)((double)rtSize.top * m_dSizeRateY);
		rtSize.right = (long)((double)rtSize.right * m_dSizeRateX);
		rtSize.bottom = (long)((double)rtSize.bottom * m_dSizeRateY);
	}
	
	pwndCtrl->MoveWindow(rtSize);
}

BOOL CResizeUI::PartitionTitle(const std::array<CWnd*, MAX_PARTITION_COUNT>& pwndCtrls, std::array<CRect, MAX_PARTITION_COUNT>& rtCtrls, UINT nCount)
{
	if (nCount <= 0)
		return FALSE;
	if (nCount > MAX_PARTITION_COUNT)
		return FALSE;
	if (m_nCheckedTitleOld == nCount)
		return FALSE;

	m_nCheckedTitleOld = nCount;

	for (UINT i = 0; i < MAX_PARTITION_COUNT; i++)
	{
		if (i < nCount)	pwndCtrls[i]->ShowWindow(TRUE);
		else			pwndCtrls[i]->ShowWindow(FALSE);
	}

	//	1분할
	if (nCount == 1)
	{
		rtCtrls[0].right = rtCtrls[1].right;
	}
	//	2분할
	else if (nCount == 2)
	{

	}
	//	4분할
	else
	{
		pwndCtrls[3]->SetRedraw(TRUE);
		pwndCtrls[3]->Invalidate(FALSE);
	}

	pwndCtrls[0]->MoveWindow(rtCtrls[0]);

	return TRUE;
}

BOOL CResizeUI::PartitionContent(const std::array<CWnd*, MAX_PARTITION_COUNT>& pwndCtrls, std::array<CRect, MAX_PARTITION_COUNT>& rtCtrls, UINT nCount)
{
	if (nCount <= 0)
		return FALSE;
	if (nCount > MAX_PARTITION_COUNT)
		return FALSE;
	if (m_nCheckedContentOld == nCount)
		return FALSE;

	m_nCheckedContentOld = nCount;

	for (UINT i = 0; i < MAX_PARTITION_COUNT; i++)
	{
		if (i < nCount)	pwndCtrls[i]->ShowWindow(TRUE);
		else			pwndCtrls[i]->ShowWindow(FALSE);
	}

	//	1분할
	if (nCount == 1)
	{
		//	전체 영역 확장
		rtCtrls[0].bottom = rtCtrls[3].bottom;
		rtCtrls[0].right  = rtCtrls[3].right;
	}
	//	2분할
	else if (nCount == 2)
	{
		//	세로 영역 확장
		rtCtrls[0].bottom = rtCtrls[2].bottom;
		rtCtrls[0].right = rtCtrls[2].right;

		rtCtrls[1].bottom = rtCtrls[3].bottom;
		rtCtrls[1].right = rtCtrls[3].right;
	}
	//	4분할
	else
	{

	}

	pwndCtrls[0]->MoveWindow(rtCtrls[0]);
	pwndCtrls[1]->MoveWindow(rtCtrls[1]);

	return TRUE;
}
