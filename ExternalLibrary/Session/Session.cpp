#include "pch.h"

#include "SendBuffer.h"
#include "ReceiveBuffer.h"
#include "Session.h"

namespace TCPIO {

	Session::Session(SOCKET socket)
	: _receiveBuffer()
	, _socket(socket)
	{
		_receiveBuffer.len = 65536;
		_receiveBuffer.buf = receivebuf;
		_sendBuffer.len = 65536;
		_sendBuffer.buf = sendbuf;
	}

}