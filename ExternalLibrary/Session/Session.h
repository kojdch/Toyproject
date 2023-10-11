#pragma once


namespace TCPIO {
	
	class Session : public OVERLAPPED
	{
	public:

		enum Type
		{
			Recv,
			Send,
			Accept,
			Disconnect,
		};

		Session();
		Session(SOCKET socket);
		~Session() = default;

		WSABUF _receiveBuffer;
		WSABUF _sendBuffer;
		SOCKET _socket;

		Type _type;

		char receivebuf[256];
		char sendbuf[256];
	private:
		//SendBuffer _sendBuffer;

	};


}