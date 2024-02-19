#pragma once

class CModelDraw
{
/* ������ & �ı��� */
public:

	CModelDraw();
	virtual ~CModelDraw();

/* ���� ���� */
protected:



/* ���� �Լ� */
protected:

	DOUBLE				GetPixelSizeToUm(UINT8 cam_id=0x01);

	VOID				DrawModelCircle(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param);
	VOID				DrawModelEllipse(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param);
	VOID				DrawModelSquare(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param);
	VOID				DrawModelRectangle(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param);
	VOID				DrawModelRing(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param);
	VOID				DrawModelTriangle(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param);
	VOID				DrawModelDiamond(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param);
	VOID				DrawModelCross(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param);


/* ���� �Լ� */
public:

	VOID				DrawModel(UINT8 cam_id, UINT32 type, CDC *dc, DOUBLE rate, DOUBLE *param);
};
