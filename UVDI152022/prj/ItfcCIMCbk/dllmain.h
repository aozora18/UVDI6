// dllmain.h : 모듈 클래스의 선언입니다.

class CItfcCIMCbkModule : public ATL::CAtlDllModuleT< CItfcCIMCbkModule >
{
public :
	DECLARE_LIBID(LIBID_ItfcCIMCbkLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ITFCCIMCBK, "{a7aacf49-de90-4256-8e84-c29cb0137880}")
};

extern class CItfcCIMCbkModule _AtlModule;
