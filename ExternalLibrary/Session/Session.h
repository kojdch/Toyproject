#pragma once


namespace TCPIO {
	
	class Session
	{
	public:
		Session(SOCKET socket);
		~Session() = default;

		WSABUF _receiveBuffer;
		WSABUF _sendBuffer;
		SOCKET _socket;
	private:
		//SendBuffer _sendBuffer;
		char receivebuf[65663];
		char sendbuf[65663];

	};


}