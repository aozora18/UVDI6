// NetworkAjin.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "stdafx.h"
#include <iostream>
#include "AjinMgr.h"
#include "MiniTcpProto.h"
#include "protocol.h"
#include <chrono>
#include <memory>
#include <thread>
#include <conio.h>
#include <cstdio>
#include "CommWrapper.h"

using namespace std;
using namespace mini;
using namespace mini::wrap;
using namespace std::chrono;

TcpProtoServer server(7777);

namespace mini
{
	namespace wrap
	{
		class logger : public ILogger
		{
		public:
			~logger() {}
			logger() {}
			void write(LogLevel lv, const std::string& msg)
			{
				cout << msg.c_str() << endl;
			}
		};
	}
}

unique_ptr<logger> _logger = std::make_unique<logger>();


void NoQuickEdit()
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	if (hInput == INVALID_HANDLE_VALUE) return;

	DWORD mode;
	if (!GetConsoleMode(hInput, &mode)) return;

	// Quick Edit 모드 비활성화
	mode &= ~ENABLE_QUICK_EDIT_MODE;

	// 필요하다면 마우스 입력 비활성화까지 하려면 아래도 같이
	// mode &= ~ENABLE_INSERT_MODE;
	// mode &= ~ENABLE_MOUSE_INPUT;

	SetConsoleMode(hInput, mode);
}

int main()
{
	NoQuickEdit();
	if (CAjinMgr::GetInstance().InitAjin())
	{
		server.setLogger(_logger.get());
		cout << "Ajin Inited." << endl;

		server.on<MsgHome>(act, home, [&](int cid , const MsgHome& v)
		{
			auto res = CAjinMgr::GetInstance().Homming(v.axisIndex);
			MsgResult m;
			m.reqMsgIdx = v.hdr.uniqIdx;
			m.resultCode = res == TRUE ? 1 : 0;
			server.sendTo<MsgResult>(cid, m);
			cout << "axis_" << v.axisIndex << " request HOMMING recved" << endl;
		});


		server.on<MsgMoveRel>(act, moveRel, [&](int cid, const MsgMoveRel& v)
			{
				auto res = CAjinMgr::GetInstance().MoveRel(v.axisIndex,v.pos,v.velocity);
				MsgResult m;
				m.reqMsgIdx = v.hdr.uniqIdx;
				m.resultCode = res == TRUE ? 1 : 0;
				server.sendTo<MsgResult>(cid, m);
				cout << "axis_" << v.axisIndex << " request Relative move recved" << endl;
			});

		server.on<MsgMoveAbs>(act, moveAbs, [&](int cid, const MsgMoveAbs& v)
			{
				auto res = CAjinMgr::GetInstance().MoveAbs(v.axisIndex, v.pos, v.velocity);
				MsgResult m;
				m.reqMsgIdx = v.hdr.uniqIdx;
				m.resultCode = res == TRUE ? 1 : 0;
				server.sendTo<MsgResult>(cid, m);
				cout << "axis_" << v.axisIndex << " request Absolute move recved" << endl;
			});

		server.on<MsgMotorEnable>(act, motorEnable, [&](int cid, const MsgMotorEnable& v)
			{
				auto res = v.enable ? CAjinMgr::GetInstance().Enable(v.axisIndex) : CAjinMgr::GetInstance().Disable(v.axisIndex);
				MsgResult m;
				m.reqMsgIdx = v.hdr.uniqIdx;
				m.resultCode = res == TRUE ? 1 : 0;
				server.sendTo<MsgResult>(cid, m);
				cout << "axis_" << v.axisIndex << " request Motor enable recved" << endl;
			});

		server.on<MsgStop>(act,stop, [&](int cid, const MsgStop& v)
			{
					BOOL res = v.all ? 
					[&]()->BOOL 
					{
						return (CAjinMgr::GetInstance().SStop(cam1z) == TRUE &&
							   CAjinMgr::GetInstance().SStop(cam2z) == TRUE &&
							   CAjinMgr::GetInstance().SStop(cam3z) == TRUE &&
							   CAjinMgr::GetInstance().SStop(tableTheta) == TRUE);
					}() :
					[&]()->BOOL{return CAjinMgr::GetInstance().SStop(v.axisIndex) == TRUE; }();

				MsgResult m;
				m.reqMsgIdx = v.hdr.uniqIdx;
				m.resultCode = res == TRUE ? 1 : 0;
				server.sendTo<MsgResult>(cid, m);
				cout << "axis_" << v.axisIndex << " request Motor enable recved" << endl;
			});


		server.on<MsgReset>(act, reset, [&](int cid, const MsgReset& v)
			{
				auto res = CAjinMgr::GetInstance().MotorReset(v.axisIndex);
				MsgResult m;
				m.reqMsgIdx = v.hdr.uniqIdx;
				m.resultCode = res == TRUE ? 1 : 0;
				server.sendTo<MsgResult>(cid, m);
				cout << "axis_" << v.axisIndex << " request motor Reset recved" << endl;
			});


		server.start();

		std::thread tickThread([&]
			{
				while (true)
				{
					std::this_thread::sleep_for(1s);

					MsgSrvBroadcast bc{};
					axisStatus v;
					for (int i = cam1z; i < axisMax; i++)
						if (CAjinMgr::GetInstance().GetAxisStatus((axis)i, v))
						{

							bc.axis[i].position = v.position;
							bc.axis[i].velocity = v.velocity;
							bc.axis[i].isFault = v.isFault;
							bc.axis[i].isEnable = v.isEnable;
							bc.axis[i].isInposition = v.isInposition;
							bc.axis[i].isHommed = v.isHommed;
							bc.axis[i].IsMinlimit = v.Islimit[axisStatus::limit::neg];
							bc.axis[i].isMaxLimit = v.Islimit[axisStatus::limit::pos];

						}
					server.broadcast(bc);
				}
			});
		tickThread.detach();

		std::cout << "[S] listening on 7777. Press esc or Ctrl+C to exit.\n";

		while (true) 
		{
			
			if (_kbhit()) {
				int ch = _getch();                   
				if (ch == 0 || ch == 224) _getch();  
				else if (ch == 27) break;            
			}
			Sleep(10);  
		}
	}

}
