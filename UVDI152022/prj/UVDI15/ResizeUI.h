#pragma once

#include <array>

#define MAX_PARTITION_COUNT		4

class CResizeUI
{
public:
	CResizeUI();
	~CResizeUI();

public:
	void	ResizeControl(CWnd* pwndParent, CWnd *pwndCtrl, BOOL bIsOnlySize = FALSE);

	double	GetRateX() { return m_dSizeRateX; }
	double	GetRateY() { return m_dSizeRateY; }

	BOOL	PartitionTitle(const std::array<CWnd*, 4>& pwndCtrls, std::array<CRect, MAX_PARTITION_COUNT>& rtCtrls, UINT nCount);
	BOOL	PartitionContent(const std::array<CWnd*, 4>& pwndCtrls, std::array<CRect, MAX_PARTITION_COUNT>& rtCtrls, UINT nCount);

private:
	int		GetSizeX();
	int		GetSizeY();
	double	GetSizeRateX();
	double	GetSizeRateY();

	double	m_dSizeRateX;
	double	m_dSizeRateY;

	int		m_nCheckedTitleOld;
	int		m_nCheckedContentOld;
};

