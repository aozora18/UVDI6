#pragma once

class CModelDraw
{
/* 생성자 & 파괴자 */
public:

	CModelDraw();
	virtual ~CModelDraw();

/* 로컬 변수 */
protected:



/* 로컬 함수 */
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


/* 공용 함수 */
public:

	VOID				DrawModel(UINT8 cam_id, UINT32 type, CDC *dc, DOUBLE rate, DOUBLE *param);
};
