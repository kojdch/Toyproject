#include "pch.h"

#include "SendBuffer.h"
#include "ReceiveBuffer.h"
#include "Session.h"

namespace TCPIO {

	Context::Context(SOCKET socket, Type type, int uid)
		: _receiveBuffer()
		, _socket(socket)
		, _type(type)
		, _uid(uid)
	{
		_receiveBuffer.len = 256;
		_receiveBuffer.buf = receivebuf;
		_sendBuffer.len = 256;
		_sendBuffer.buf = sendbuf;
		memset(this, 0, sizeof(OVERLAPPED));
	}

	Session::Session(Context context)
	: _socket(context._socket)
	, _context(context)
	, _uid(context._uid)
	{
		//memset(this, 0, sizeof(OVERLAPPED));
	}

	Session::Session(SOCKET socket, int uid)
	: _socket(socket)
	, _context(_socket, Type::Recv, uid)
	, _uid(uid)
	{
		//memset(this, 0, sizeof(OVERLAPPED));
	}

}