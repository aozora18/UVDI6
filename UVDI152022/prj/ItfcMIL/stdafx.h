
// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS
//[2017/12/04]: Vision Server comm
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.


#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.


#pragma comment(lib, "UxTheme.lib")

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����

#include "Mil.h"
//#include "UserDefine.h"

//#pragma comment (lib, "mil.lib")
//#pragma comment (lib, "milim.lib")
//#pragma comment (lib, "milpat.lib")
//#pragma comment (lib, "milmod.lib")

// GDI+ 
//#include <GdiPlus.h>
//#pragma comment ( lib, "gdiplus.lib" )
//using namespace Gdiplus;

//[2017/12/04]: Vision Server comm
#include <queue>
#include <array>
using namespace std;

//#include "./Comm/ISofSockCls.h"
//#include "./Comm/Logger.h"

class CDPointTh
{
public:
	double x, y;
	//double long theta[5]; // y�� -2,-1,0,+1,+2
	double theta;
	CDPointTh()
	{
		x = y = theta = 0.0;
		/*for (int i = 0; i < 5; i++){
			theta[i] = 0.0;
		}*/
	}
};

//class CDPoint
//{
//public:
//	double x, y;
//	CDPoint()
//	{
//		x = y = 0.0;
//	}
//};

enum eMilColor
{
	eM_COLOR_BLACK = 0,
	eM_COLOR_RED,
	eM_COLOR_GREEN,
	eM_COLOR_BLUE,
	eM_COLOR_YELLOW,
	eM_COLOR_MAGENTA,
	eM_COLOR_CYAN,
	eM_COLOR_WHITE,
	eM_COLOR_GRAY,
	eM_COLOR_BRIGHT_GRAY,
	eM_COLOR_LIGHT_GRAY,
	eM_COLOR_LIGHT_GREEN,
	eM_COLOR_LIGHT_BLUE,
	eM_COLOR_LIGHT_WHITE,
	eM_COLOR_DARK_RED,
	eM_COLOR_DARK_GREEN,
	eM_COLOR_DARK_BLUE,
	eM_COLOR_DARK_YELLOW,
	eM_COLOR_DARK_MAGENTA,
	eM_COLOR_DARK_CYAN,
};

//#include "Label.h"

#include <math.h>

//#define M_COLOR_ORANGE                            M_RGB888(255,204,0)
//#define M_COLOR_DARK_ORANGE                       M_RGB888(153,102,51)



//#ifdef _UNICODE
//#if defined _M_IX86
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_X64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#else
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif
//#endif
//
//#if defined _M_IX86
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_IA64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_X64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#else
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif

