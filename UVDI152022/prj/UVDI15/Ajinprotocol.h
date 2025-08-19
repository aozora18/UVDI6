#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <type_traits>



namespace mini
{

	enum MainHeader : int
	{
		act,
		broadcast,
		etc,
		mainHeaderEnd,
	};

	enum SubHeader : int
	{
		home,
		reset,
		moveAbs,
		moveRel,
		motorEnable,
		stop,
		Info,
		msg,
		result,
		subHeaderEnd,
	};

#pragma pack(push,1)
	struct axisInfo
	{
		double position;
		double velocity;
		bool isFault;
		bool isEnable;
		bool isInposition;
		bool isHommed;
		bool IsMinlimit;
		bool isMaxLimit;
	};
#pragma pack(pop)


#pragma pack(push,1)
	struct MessageHeader
	{
		MessageHeader(MainHeader h, SubHeader sh)
			: header(static_cast<uint16_t>(h)),
			subheader(static_cast<uint16_t>(sh)),
			size(0), uniqIdx(0) {}

		MessageHeader() : header(0), subheader(0), size(0), uniqIdx(0) {}

		uint16_t header;     // 상위 헤더
		uint16_t subheader;  // 서브헤더
		uint32_t size;       // 전체 구조체 크기(bytes). sizeof(T)
		uint32_t uniqIdx; //메세지 인덱스

	};
#pragma pack(pop)




#pragma pack(push,1)

	// (1,1) 핑 → (1,2) 퐁
	struct MsgHome
	{
		MessageHeader hdr = MessageHeader(act, home);
		int axisIndex;
	};

	struct MsgResult
	{
		MessageHeader hdr = MessageHeader(etc, result);
		int resultCode;
		int reqMsgIdx;
	};


	struct MsgReset
	{
		MessageHeader hdr = MessageHeader(act, reset);
		int axisIndex;
	};

	struct MsgMotorEnable
	{
		MessageHeader hdr = MessageHeader(act, motorEnable);
		int axisIndex;
		bool enable;
	};

	struct MsgStop
	{
		MessageHeader hdr = MessageHeader(act, stop);
		int axisIndex;
		bool all;
	};





	struct MsgMoveAbs
	{
		MessageHeader hdr = MessageHeader(act, moveAbs);
		int axisIndex;
		double pos;
		double velocity;
	};


	struct MsgMoveRel
	{
		MessageHeader hdr = MessageHeader(act, moveRel);
		int axisIndex;
		double pos;
		double velocity;
	};


	struct MsgTextFixed {
		MessageHeader hdr = MessageHeader(etc, msg);
		char text[128];         // 127자 + NUL
	};

	struct MsgSrvBroadcast
	{
		MessageHeader hdr = MessageHeader(broadcast, Info);
		axisInfo axis[4];
	};



#pragma pack(pop)
}