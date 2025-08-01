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

	struct asNetMsgHead
	{
		asNetMsgHead() :m_flag(AS_NET_TCP_FLAG), m_len(0), m_msgId(0), m_gateId(0), m_param(0)
		{
		}
		u32		m_flag;//消息头标志
		u32		m_len; // 消息体长度，包括head
		u32		m_msgId;//消息编号
		u32		m_gateId; // 连接id 网关使用
		u32		m_param; //参数
	};
#define AS_MSG_TCP_HEAD_SIZE sizeof(asNetTcpMsgHead)
#define AS_MSG_HEAD_SIZE sizeof(asNetMsgHead)
}

namespace asThread
{
#define AS_THREAD_STOP 1
#define AS_TRHEAD_DEL 2
#define AS_THREAD_CLEAR 3
#define AS_THREAD_SLEEP 4

#define AS_THREAD_MAX_GROUP_COUNT		512 //单组最大线程数

#define AS_THREADMSG_LOGGER				100 //日志消息
#define AS_THREADMSG_THREAD				101 //线程间消息
//NET
#define AS_THREADMSG_NET_CONNECT		200 // 网络新连接
#define AS_THREADMSG_NET_DISCONNECT		201 // 网络断开连接
#define AS_THREADMSG_NET_RECV			202 // 网络收到消息
#define AS_THREADMSG_NET_SEND			203 // 网络发送消息
#define AS_THREADMSG_NET_CLOSE			204 // 主动断开
#define AS_THREADMSG_NET_NRESP			205 // 不回包
#define AS_THREADMSG_NET_BROADCAST		206 // 广播
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
