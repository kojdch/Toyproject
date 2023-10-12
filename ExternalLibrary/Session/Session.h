#pragma once


namespace TCPIO {
	
	class Session : public OVERLAPPED
	{
	public:
		Session();
		Session(SOCKET socket);
		~Session() = default;

		WSABUF _receiveBuffer;
		WSABUF _sendBuffer;
		SOCKET _socket;
		char receivebuf[256];
		char sendbuf[256];
	private:
		//SendBuffer _sendBuffer;

	};


}