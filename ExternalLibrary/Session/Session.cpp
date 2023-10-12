#include "pch.h"

#include "SendBuffer.h"
#include "ReceiveBuffer.h"
#include "Session.h"

namespace TCPIO {

	Session::Session()
	{
	}

	Session::Session(SOCKET socket)
	: _receiveBuffer()
	, _socket(socket)
	{
		_receiveBuffer.len = 256;
		_receiveBuffer.buf = receivebuf;
		_sendBuffer.len = 256;
		_sendBuffer.buf = sendbuf;
		memset(this, 0, sizeof(OVERLAPPED));
	}

}