#pragma once

class CDrawModel
{
/* ������ & �ı��� */
public:

	CDrawModel(RECT r_draw);
	virtual ~CDrawModel();

/* ���� ���� */
protected:

	TCHAR				m_tzModelName[MARK_MODEL_NAME_LENGTH];
	TCHAR				m_tzMMFFile[MARK_MODEL_NAME_LENGTH];	/* for mmf file */ 
	TCHAR				m_tzPATFile[MARK_MODEL_NAME_LENGTH];	/* for mmf file */ 

	UINT8				m_u8ACamID;			/* 1 or 2 */
	DOUBLE				m_dbParam[5];		/* Max 5 ea */
	ENG_MMDT			m_enModelType;
	UINT8				m_enFindType;		/* 0 : Model Finder, 1 : Pattern Matching */

	RECT				m_rDraw;			/* ���� �׷����� ���� ���� */

	DOUBLE				m_dbMarkW,m_dbMarkH;/* ���� ������ ��µ� Mark �̹����� ũ�� (����: pixel) */

	// lk91 VISION �߰� 
	CPoint	iBaseP;			// Base Position
	CPoint	iSizeP;			// Mark Size
	CPoint	iOffsetP;		// Mark Center	(AreaBox�� (left,top)�� �������� ���� ���� ��ǥ..)
	CRect	rectSearchArea;	// Search Area
	double	dScore;			// ��Ī��...


/* ���� �Լ� */
protected:

	DOUBLE				GetImageRate();

	VOID				DrawModelCircle(HDC dc, DOUBLE rate);
	VOID				DrawModelEllipse(HDC dc, DOUBLE rate);
	VOID				DrawModelSquare(HDC dc, DOUBLE rate);
	VOID				DrawModelRectangle(HDC dc, DOUBLE rate);
	VOID				DrawModelRing(HDC dc, DOUBLE rate);
	VOID				DrawModelTriangle(HDC dc, DOUBLE rate);
	VOID				DrawModelDiamond(HDC dc, DOUBLE rate);
	VOID				DrawModelCross(HDC dc, DOUBLE rate);
	VOID				DrawModelMMF(HDC dc, DOUBLE rate);


/* ���� �Լ� */
public:

	BOOL				DrawModel(HDC dc);
	VOID				SetModel(PTCHAR m_name, ENG_MMDT type, UINT8 cam_id, DOUBLE *param);
	VOID				SetModel(PTCHAR m_name, ENG_MMDT type, DOUBLE* param);
	//VOID				SetModel(PTCHAR m_name, UINT8 cam_id, UINT8 find_type, PTCHAR m_file, CPoint m_SizeP, CPoint m_CenterP);
	VOID				SetModel(PTCHAR m_name, UINT8 cam_id, PTCHAR m_file, CPoint m_SizeP, CPoint m_CenterP);

	VOID				ResetModel();

	UINT8				GetACamID()					{	return m_u8ACamID;			}
	PTCHAR				GetModelName()				{	return m_tzModelName;		}
	PTCHAR				GetMMFFile()				{	return m_tzMMFFile;		}
	PTCHAR				GetPATFile()				{	return m_tzPATFile;		}
	
	ENG_MMDT			GetModelType()				{	return m_enModelType;		}
	DOUBLE				GetModelColor()				{	return m_dbParam[0];		}
	DOUBLE				GetModelSize()				{	return m_dbParam[1];		}
	DOUBLE				GetParam(UINT8 index)		{	return m_dbParam[index];	}
	UINT8				GetFindType()				{	return m_enFindType; }
	CPoint				GetMarkSizeP() { return iSizeP; }
	CPoint				GetMarkOffsetP() { return iOffsetP; }

	BOOL				IsSetModel()				{	return (m_enModelType != ENG_MMDT::en_none); }
	BOOL				IsModelType(ENG_MMDT type)	{	return type == m_enModelType;	}
	BOOL				IsModelName(PTCHAR m_name)	{	return 0 == wcscmp(m_name, m_tzModelName);	}
	BOOL				IsMMFFile(PTCHAR m_file) { return 0 == wcscmp(m_file, m_tzMMFFile); }
	BOOL				IsPATFile(PTCHAR m_file) { return 0 == wcscmp(m_file, m_tzPATFile); }

	BOOL				IsModelColor(DOUBLE color)	{	return color == m_dbParam[0];	}
	BOOL				IsModelSize(DOUBLE size);
};
