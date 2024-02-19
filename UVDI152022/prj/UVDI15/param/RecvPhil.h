#pragma once


class CRecvPhil
{

private:

public:
	/*노광 결과 기록*/
	//STG_CELA			m_stExpoLog;
	//STG_CPVE			m_stPhilStatus;
	VOID				Init(HWND hWnd);
	HWND				m_hMainWnd;

	VOID				PhilSendCreateRecipe(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendDelectRecipe(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendModifyRecipe(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendSelectRecipe(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendListRecipe(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendInfoRecipe(STG_PP_PACKET_RECV* stRecv);
	//VOID				PhilSendMove(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendStatusValue(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendChageMode(STG_PP_PACKET_RECV* stRecv);
	//VOID				PhilSendProcessExecute(STG_PP_PACKET_RECV* stRecv);
	//VOID				PhilSendEventStatus(STG_PP_PACKET_RECV* stRecv);
protected:
	CRecvPhil();
	CRecvPhil(const CRecvPhil&); // Prohibit Copy Constructor
	CRecvPhil& operator =(const CRecvPhil&);



public:
	virtual ~CRecvPhil(void);

	static CRecvPhil* GetInstance()
	{
		static CRecvPhil _inst;
		return &_inst;
	}
};

