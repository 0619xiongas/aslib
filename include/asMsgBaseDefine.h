#ifndef AS_MSGBASEDEFINE_H
#define AS_MSGBASEDEFINE_H

#include "asBaseDefine.h"

namespace asNet
{
#define AS_NET_TCP_FLAG 0x20250422
#define AS_NET_MAX_LEN 1073741824
#define AS_NET_MSG_DIFF 0x10000000

	struct asNetTcpMsgHead
	{
		asNetTcpMsgHead() :m_flag(AS_NET_TCP_FLAG), m_len(0), m_msgId(0)
		{
		}
		u32		m_flag;//消息头标志
		u32		m_len; // 消息体长度，包括head
		u32		m_msgId;//消息编号
	};
}

namespace asThread
{
#define AS_THREAD_STOP 1
#define AS_TRHEAD_DEL 2
#define AS_THREAD_CLEAR 3
#define AS_THREAD_SLEEP 4

#define AS_THREADMSG_LOGGER 100
	struct asMsg
	{
		asMsg() 
		{
			msgID = 0;
			subID = 0;
			params[0] = params[1] = params[2] = 0;
		}
		asMsg(u32 mID, u32 sID, ulint p1 = 0, ulint p2 = 0, ulint p3 = 0)
		{
			msgID = mID;
			subID = sID;
			params[0] = p1;
			params[1] = p2;
			params[2] = p3;
		}
		u32 msgID;
		u32 subID;
		ulint params[3];
	};
}

#endif
